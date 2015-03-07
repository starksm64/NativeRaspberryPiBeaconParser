#include <Beacon.h>
#include "HCIDumpParser.h"



void HCIDumpParser::processHCI(HCIDumpCommand& parseCommand) {
    HCIDumpParser::parseCommand = &parseCommand;
    string clientID(parseCommand.getClientID());
    if(clientID.empty())
        clientID = parseCommand.getScannerID();
    publisher = MsgPublisher::create(parseCommand.getPubType(), parseCommand.getBrokerURL(), clientID, "", "");
    if(!parseCommand.isSkipPublish()) {
        publisher->setUseTopics(!parseCommand.isUseQueues());
        publisher->setDestinationName(parseCommand.getDestinationName());
        if(batchCount > 0) {
            publisher->setUseTransactions(true);
            printf("Enabled transactions\n");
        }
        publisher->start(parseCommand.isAsyncMode());
    }
    else {
        printf("Skipping publish of parsed beacons\n");
    }

    char cdev = parseCommand.getHciDev().at(parseCommand.getHciDev().size()-1);
    int device = cdev - '0';
    scan_frames(device,  beacon_event_callback);
}

void HCIDumpParser::beaconEvent(const beacon_info *info) {
    Beacon beacon(parseCommand->getScannerID(), info->uuid, info->code, info->manufacturer, info->major, info->minor,
    info->power, info->calibrated_power, info->rssi, info->time);
    vector<byte> msg = beacon.toByteMsg();
    // Check for heartbeat
    bool isHeartbeat = scannerUUID.compare(info->uuid) == 0;
    if(isHeartbeat)
        beacon.setMessageType(BeconEventType::SCANNER_HEARTBEAT);
    if(!parseCommand->isSkipPublish()) {
        if(batchCount > 0) {
            // Overwrite last event if it is a heartbeat and this is as well
            if(isHeartbeat && events.size() > 0 && events.back().getMessageType() == BeconEventType::SCANNER_HEARTBEAT)
                events.pop_back();
            events.push_back(beacon);
            if(shouldSendMessages()) {
#ifdef PRINT_DEBUG
printf("Sending msg batch, size=%d\n", events.size());
#endif
                publisher->publish(events);
                events.clear();
            } else {
#ifdef PRINT_DEBUG
printf("Batched msg, size=%d\n", events.size());
#endif
            }
        } else if(isHeartbeat) {
#ifdef PRINT_DEBUG
printf("Sending heartbeat\n");
#endif
            publisher->publishStatus(beacon);
        } else {
#ifdef PRINT_DEBUG
printf("Sending msg\n");
#endif
            publisher->publish("", MqttQOS::AT_MOST_ONCE, msg.data(), msg.size());
        }
    }
    else {
        const char *info = isHeartbeat ? "heartbeat" : "event";
        printf("Parsed(%s): %s\n", info, beacon.toString().c_str());
    }
}
void HCIDumpParser::cleanup() {
    if(publisher != nullptr)
        publisher->stop();
    publisher = nullptr;
}
