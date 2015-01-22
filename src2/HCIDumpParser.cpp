#include <Beacon.h>
#include "HCIDumpParser.h"

void HCIDumpParser::processHCI(HCIDumpCommand& parseCommand) {
    HCIDumpParser::parseCommand = &parseCommand;
    string clientID(parseCommand.getClientID());
    if(clientID.empty())
        clientID = parseCommand.getScannerID();
    mqtt = new MqttPublisher(parseCommand.getBrokerURL(), clientID);
    if(!parseCommand.isSkipPublish())
        mqtt->start();
    else
        printf("Skipping publish of parse beacons\n");

    char cdev = parseCommand.getHciDev().at(parseCommand.getHciDev().size()-1);
    int device = cdev - '0';
    scan_frames(device,  beacon_event_callback);
}

void HCIDumpParser::beaconEvent(const beacon_info *info) {
    Beacon beacon(parseCommand->getScannerID(), info->uuid, info->code, info->manufacturer, info->major, info->minor,
    info->power, info->calibrated_power, info->rssi, info->time);
    vector<byte> msg = beacon.toByteMsg();
    if(!parseCommand->isSkipPublish())
        mqtt->publish(parseCommand->getTopicName(), MqttQOS::AT_MOST_ONCE, msg.data(), msg.size());
    else
        printf("Parsed: %s\n", beacon.toString().c_str());
}
void HCIDumpParser::cleanup() {
    if(mqtt != nullptr)
        mqtt->stop();
    mqtt = nullptr;
}
