//
// Created by starksm on 3/28/15.
//

#include <thread>
#include "BeaconEventConsumer.h"

bool shouldSendMessages() {
    return false;
}

/**
 * Called for non-null beacon_info events by the thread loop
 */
void BeaconEventConsumer::handleMessage(shared_ptr<EventsBucket> &bucket) {
#ifdef PRINT_DEBUG
    printf("handleMessage(%d)\n", bucket->getEventCount());
#endif
    for (map<int32_t, beacon_info>::const_iterator iter = bucket->begin(); iter != bucket->end(); iter++) {
        const beacon_info& info = iter->second;
        Beacon beacon(parseCommand.getScannerID(), info.uuid, info.code, info.manufacturer, info.major, info.minor,
                      info.power, info.calibrated_power, info.rssi, info.time);
        if (info.isHeartbeat)
            beacon.setMessageType(BeconEventType::SCANNER_HEARTBEAT);

        int publishID = statusInformation->updatePublishEventCount();
        beacon.setScannerSequenceNo(publishID);
        if (batchCount > 0) {
            // Overwrite last event if it is a heartbeat and this is as well
            if (info.isHeartbeat && events.size() > 0 &&
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
        } else if (info.isHeartbeat) {
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

}

void BeaconEventConsumer::publishEvents() {
    running = true;
    printf("BeaconEventConsumer::publishEvents, starting\n");
    while (running) {
        shared_ptr<EventsBucket> info = exchanger->takeEvent();
        if(statusInformation->isStatusChanged()) {
            publisher->publishProperties(statusInformation->getStatusQueue(), statusInformation->getLastStatus());
            statusInformation->clearStatusChanged();
        }
        else if (info) {
            handleMessage(info);
        } else {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
    printf("BeaconEventConsumer::publishEvents, exiting\n");
}

void BeaconEventConsumer::init(shared_ptr<EventExchanger>& exchanger, shared_ptr<MsgPublisher> &msgPublisher, shared_ptr<StatusInformation>& statusInformation) {
    this->exchanger = exchanger;
    this->publisher = msgPublisher;
    this->statusInformation = statusInformation;
}
