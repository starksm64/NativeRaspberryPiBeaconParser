#include <Beacon.h>
#include <sys/time.h>
#include "HCIDumpParser.h"

// Uncomment to publish the raw byte[] for the beacon, otherwise, properties are sent
//#define SEND_BINARY_DATA

static int64_t currentMilliseconds() {
    timeval now;
    gettimeofday(&now, nullptr);

    int64_t nowMS = now.tv_sec;
    nowMS *= 1000;
    nowMS += now.tv_usec/1000;
    return nowMS;
}

void HCIDumpParser::processHCI(HCIDumpCommand& parseCommand) {
    HCIDumpParser::parseCommand = &parseCommand;
    string clientID(parseCommand.getClientID());
    if(clientID.empty())
        clientID = parseCommand.getScannerID();
    publisher = MsgPublisher::create(parseCommand.getPubType(), parseCommand.getBrokerURL(), clientID, "", "");
    if(parseCommand.isAnalyzeMode()) {
        begin = currentMilliseconds();
        end += parseCommand.getAnalyzeWindow();
        printf("Running in analyze mode, window=%d seconds\n", parseCommand.getAnalyzeWindow());
    }
    else if(!parseCommand.isSkipPublish()) {
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
    if(parseCommand->isAnalyzeMode()) {
        updateBeaconCounts(info);
    }
    else if(!parseCommand->isSkipPublish()) {
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
printf("Sending heartbeat, %s\n", !parseCommand->isSkipHeartbeat());
#endif
            if(!parseCommand->isSkipHeartbeat())
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
    else {
        const char *info = isHeartbeat ? "heartbeat" : "event";
        if(!isHeartbeat || (isHeartbeat && !parseCommand->isSkipHeartbeat()))
            printf("Parsed(%s): %s\n", info, beacon.toString().c_str());
    }
}

void HCIDumpParser::cleanup() {
    if(publisher != nullptr)
        publisher->stop();
    publisher = nullptr;
}

void HCIDumpParser::updateBeaconCounts(beacon_info const *info) {
#ifdef PRINT_DEBUG
    printf("updateBeaconCounts(%d); begin=%lld, end=%lld, info.time=%lld\n", beaconCounts.size(), begin, end, info->time);
#endif
    if(info->time < end) {
        // Update the beacon event counts
        beaconCounts[info->minor] ++;
    } else {
        char timestr[256];
        struct timeval tv;
        tv.tv_sec = begin/1000;
        tv.tv_usec = 0;
        struct tm tm;
        localtime_r(&tv.tv_sec, &tm);
        strftime(timestr, 128, "%r", &tm);
        // Report the stats for this time window and then reset
        printf("+++ Beacon counts for window(%d): %s\n", parseCommand->getAnalyzeWindow(), timestr);
        for(map<int32_t, int32_t>::iterator iter = beaconCounts.begin(); iter != beaconCounts.end(); iter++) {
            printf("\t%2d: %2d", iter->first, iter->second);
        }
        begin = end;
        end += 1000*parseCommand->getAnalyzeWindow();
        beaconCounts.clear();
    }
}
