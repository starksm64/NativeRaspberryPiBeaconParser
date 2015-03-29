#include <Beacon.h>
#include "HCIDumpParser.h"
#include "BeaconEventConsumer.h"

// Uncomment to publish the raw byte[] for the beacon, otherwise, properties are sent
//#define SEND_BINARY_DATA



void HCIDumpParser::processHCI(HCIDumpCommand& parseCommand) {
    HCIDumpParser::parseCommand = parseCommand;
    eventConsumer.setParseCommand(parseCommand);
    string clientID(parseCommand.getClientID());
    if(clientID.empty())
        clientID = parseCommand.getScannerID();
    publisher.reset(MsgPublisher::create(parseCommand.getPubType(), parseCommand.getBrokerURL(), clientID, "", ""));
    timeWindow.reset(parseCommand.getAnalyzeWindow());
    if(parseCommand.isAnalyzeMode()) {
        printf("Running in analyze mode, window=%d seconds, begin=%lld\n", parseCommand.getAnalyzeWindow(), timeWindow.getBegin());
    }
    else if(!parseCommand.isSkipPublish()) {
        publisher->setUseTopics(!parseCommand.isUseQueues());
        publisher->setDestinationName(parseCommand.getDestinationName());
        if(batchCount > 0) {
            publisher->setUseTransactions(true);
            printf("Enabled transactions\n");
        }
        publisher->start(parseCommand.isAsyncMode());
        // Create a thread for the consumer
        eventExchanger.reset(new EventExchanger);
        eventConsumer.init(eventExchanger, publisher);
        consumerThread.reset(new thread(&BeaconEventConsumer::publishEvents, eventConsumer));
        printf("Started event consumer thread\n");
    }
    else {
        printf("Skipping publish of parsed beacons\n");
    }

    char cdev = parseCommand.getHciDev().at(parseCommand.getHciDev().size()-1);
    int device = cdev - '0';
    scan_frames(device,  beacon_event_callback);
}

void HCIDumpParser::beaconEvent(const beacon_info& info) {
    // Check for heartbeat
    bool isHeartbeat = scannerUUID.compare(info.uuid) == 0;
    // Queue the event into the
    unique_ptr<EventsBucket> bucket = timeWindow.addEvent(info);
    // Now handle the bucket if a new one has been created
    if(bucket) {
        if(!parseCommand.isSkipPublish()) {
            beacon_info* event = new beacon_info(info);
            event->isHeartbeat = isHeartbeat;
            eventExchanger->putEvent(event);
        }
        else {
            if(parseCommand.isAnalyzeMode()) {
                printBeaconCounts(bucket);
            } else {
                Beacon beacon(parseCommand.getScannerID(), info.uuid, info.code, info.manufacturer, info.major,
                              info.minor, info.power, info.calibrated_power, info.rssi, info.time);
                const char *info = isHeartbeat ? "heartbeat" : "event";
                if (!isHeartbeat || (isHeartbeat && !parseCommand.isSkipHeartbeat()))
                    printBeaconCounts(beacon, bucket);
            }
        }
    }
}

void HCIDumpParser::cleanup() {
    if(publisher)
        publisher->stop();
}

void HCIDumpParser::printBeaconCounts(Beacon beacon, const unique_ptr<EventsBucket> &bucket) {
    printf("Window: %s, parsed(%s): %s\n", beacon.toString().c_str());
    printBeaconCounts(bucket);
}

void HCIDumpParser::printBeaconCounts(const unique_ptr<EventsBucket> &bucket) {
    vector<char> tmp;
    bucket->toString(tmp);
    printf("%s\n", tmp.data());
}
