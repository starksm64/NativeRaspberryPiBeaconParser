//
// Created by starksm on 3/28/15.
//

#include "BeaconEventConsumer.h"

bool shouldSendMessages() {
    return false;
}

/**
 * Called for non-null beacon_info events by the thread loop
 */
void BeaconEventConsumer::handleMessage(unique_ptr<beacon_info>& info) {
    Beacon beacon(parseCommand.getScannerID(), info->uuid, info->code, info->manufacturer, info->major, info->minor,
                  info->power, info->calibrated_power, info->rssi, info->time);
    if (info->isHeartbeat)
        beacon.setMessageType(BeconEventType::SCANNER_HEARTBEAT);

    if (batchCount > 0) {
        // Overwrite last event if it is a heartbeat and this is as well
        if (info->isHeartbeat && events.size() > 0 &&
            events.back().getMessageType() == BeconEventType::SCANNER_HEARTBEAT)
            events.pop_back();
        events.push_back(beacon);
        if (shouldSendMessages()) {
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
    } else if (info->isHeartbeat) {
#ifdef PRINT_DEBUG
printf("Sending heartbeat, %s\n", !parseCommand.isSkipHeartbeat());
#endif
        if (!parseCommand.isSkipHeartbeat())
            publisher->publishStatus(beacon);
    } else {
#ifdef PRINT_DEBUG
printf("Sending msg\n");
#endif
#ifdef SEND_BINARY_DATA
            publisher->publish("", MqttQOS::AT_MOST_ONCE, msg.data(), msg.size());
#else
        publisher->publish("", beacon);
#endif
    }
}

void BeaconEventConsumer::publishEvents() {
    running = true;
    printf("BeaconEventConsumer::publishEvents, starting\n");
    while(running) {
        unique_ptr<beacon_info> info = exchanger->takeEvent();
        if(info)
            handleMessage(info);
    }
    printf("BeaconEventConsumer::publishEvents, exiting\n");
}

void BeaconEventConsumer::init(shared_ptr<EventExchanger> exchanger, shared_ptr<MsgPublisher> &msgPublisher) {
    this->exchanger = exchanger;
    this->publisher = msgPublisher;
}
