#include <Beacon.h>
#include "HCIDumpParser.h"
#include "BeaconEventConsumer.h"

// Uncomment to publish the raw byte[] for the beacon, otherwise, properties are sent
//#define SEND_BINARY_DATA

extern "C" bool stop_scan_frames;

static void generateTestEvents(EventExchanger *exchanger) {
    printf("generateTestEvents, starting...\n");
    beacon_info event;
    int count = 0;
    try {
        bool running = true;
        while (running) {
            count++;
            int32_t minor = count % 150;
            sprintf(event.uuid, "UUID-%.12d", minor);
            event.minor = minor;
            event.rssi = -50 - count % 7;
            event.time = EventsWindow::currentMilliseconds();
            bool stop = beacon_event_callback(&event);
            running = !stop;
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    } catch(exception& e) {
        printf("generateTestEvents failure, %s\n", e.what());
    }
    stop_scan_frames = true;
    printf("generateTestEvents, exiting\n");
}

void HCIDumpParser::processHCI(HCIDumpCommand &parseCommand) {
    HCIDumpParser::parseCommand = parseCommand;
    eventConsumer.setParseCommand(parseCommand);
    string clientID(parseCommand.getClientID());
    if (clientID.empty())
        clientID = parseCommand.getScannerID();
    timeWindow.reset(parseCommand.getAnalyzeWindow());
    // Setup the status information
    statusInformation->setScannerID(parseCommand.getScannerID());
    statusInformation->setStatusInterval(parseCommand.getStatusInterval());
    statusInformation->setStatusQueue(parseCommand.getStatusQueue());
    statusInformation->setBcastAddress(parseCommand.getBcastAddress());
    statusInformation->setBcastPort(parseCommand.getBcastPort());

    if (parseCommand.isAnalyzeMode()) {
        printf("Running in analyze mode, window=%d seconds, begin=%lld\n", parseCommand.getAnalyzeWindow(),
               timeWindow.getBegin());
    }
    else if (!parseCommand.isSkipPublish()) {
        const string& username = parseCommand.getUsername();
        const string& password = parseCommand.getPassword();
        MsgPublisher *mp = MsgPublisher::create(parseCommand.getPubType(), parseCommand.getBrokerURL(), clientID, username, password);
        publisher.reset(mp);
        publisher->setUseTopics(!parseCommand.isUseQueues());
        printf("setUseTopics: %s\n", publisher->isUseTopics() ? "true" : "false");
        publisher->setDestinationName(parseCommand.getDestinationName());
        if (batchCount > 0) {
            publisher->setUseTransactions(true);
            printf("Enabled transactions\n");
        }
        publisher->start(parseCommand.isAsyncMode());

        // Create a thread for the consumer unless running in battery test mode
        if(!parseCommand.isBatteryTestMode()) {
            eventExchanger.reset(new EventExchanger);
            eventConsumer.init(eventExchanger, publisher, statusInformation);
            consumerThread.reset(new thread(&BeaconEventConsumer::publishEvents, &eventConsumer));
            printf("Started event consumer thread\n");
        }
    }
    else {
        printf("Skipping publish of parsed beacons\n");
    }

    // If the status interval is > 0, start the health status monitor
    if(parseCommand.getStatusInterval() > 0) {
        statusMonitor.start(publisher, statusInformation);
    }

    if(parseCommand.isGenerateTestData()) {
        // Generate test data
        thread testThread(generateTestEvents, eventExchanger.get());
        testThread.detach();
    }

    // Scan
    char cdev = parseCommand.getHciDev().at(parseCommand.getHciDev().size() - 1);
    int device = cdev - '0';
    scan_frames(device, beacon_event_callback);

    // Join the consumerThread if it was
    eventConsumer.setRunning(false);
    if(consumerThread && consumerThread->joinable()) {
        printf("Joining the consumer thread...\n");
        consumerThread->join();
        printf("done\n");
    }

    // Stop the status monitor
    statusMonitor.stop();
}

void HCIDumpParser::beaconEvent(const beacon_info &info) {
    // Check for heartbeat
    bool isHeartbeat = scannerUUID.compare(info.uuid) == 0;
    if(parseCommand.isBatteryTestMode()) {
        // Send the raw unaveraged heartbeat info, or ignore non-heartbeat events
        if(isHeartbeat)
            sendRawHeartbeat(info);
        return;
    }

    // Merge the event into the current time window
    shared_ptr<EventsBucket> bucket = timeWindow.addEvent(info, isHeartbeat);
    statusInformation->addEvent(info, isHeartbeat);
    // Now handle the bucket if a new one has been created
    if (bucket) {
        if (!parseCommand.isSkipPublish()) {
            eventExchanger->putEvent(bucket);
        }
        else {
            if (parseCommand.isAnalyzeMode()) {
                printBeaconCounts(bucket);
            } else {
                Beacon beacon(parseCommand.getScannerID(), info.uuid, info.code, info.manufacturer, info.major,
                              info.minor, info.power, info.calibrated_power, info.rssi, info.time);
                const char *info = isHeartbeat ? "heartbeat" : "event";
                if (!isHeartbeat || (isHeartbeat && !parseCommand.isSkipHeartbeat()))
                    printBeaconCounts(beacon, bucket);
            }
        }
        // Display either the closest beacon or status
        if(scannerView) {
            if(scannerView->isDisplayBeaconsMode())
                displayClosestBeacon(bucket);
            else
                displayStatus();
        }
    }
}

void HCIDumpParser::cleanup() {
    if (publisher)
        publisher->stop();
}

void HCIDumpParser::printBeaconCounts(Beacon beacon, const shared_ptr<EventsBucket> &bucket) {
    printf("Window: parsed(%s):\n", beacon.toString().c_str());
    printBeaconCounts(bucket);
}

void HCIDumpParser::printBeaconCounts(const shared_ptr<EventsBucket> &bucket) {
    vector<char> tmp;
    bucket->toString(tmp);
    printf("%s\n", tmp.data());
}

static int lastRSSI[10];
static int heartbeatCount = 0;
void HCIDumpParser::sendRawHeartbeat(const beacon_info &info) {
    Beacon beacon(parseCommand.getScannerID(), info.uuid, info.code, info.manufacturer, info.major, info.minor,
                  info.power, info.calibrated_power, info.rssi, info.time);
    beacon.setMessageType(BeconEventType::SCANNER_HEARTBEAT);
    publisher->publishStatus(beacon);
    lastRSSI[heartbeatCount%10] = info.rssi;
    heartbeatCount ++;
    if(heartbeatCount % 100 == 0) {
        printf("Last[10].RSSI:");
        for(int n = 0; n < 10; n ++)
            printf("%d,", lastRSSI[n]);
        printf("\n");
    }
}

void HCIDumpParser::displayClosestBeacon(const shared_ptr<EventsBucket>& bucket) {
    map<int32_t, beacon_info>::const_iterator iter = bucket->begin();
    int32_t maxRSSI = -100;
    const beacon_info *closest = nullptr;
    const beacon_info *heartbeat = nullptr;
    while (iter != bucket->end()) {
        // Skip the heartbeast beacon...
        if(iter->second.rssi > maxRSSI) {
            if(scannerUUID.compare(iter->second.uuid) == 0)
                heartbeat = &iter->second;
            else {
                maxRSSI = iter->second.rssi;
                closest = &iter->second;
            }
        }
        iter++;
    }
    if(closest != nullptr) {
        Beacon closestBeacon(parseCommand.getScannerID(), closest->uuid, closest->code, closest->manufacturer,
                             closest->major, closest->minor, closest->power, closest->calibrated_power,
                             closest->rssi, closest->time);
        scannerView->displayBeacon(closestBeacon);
    } else if(heartbeat != nullptr) {
        // The only beacon seen was the heartbeat beacon, so display it
        Beacon heartbeatBeacon(parseCommand.getScannerID(), heartbeat->uuid, heartbeat->code, heartbeat->manufacturer,
                               heartbeat->major, heartbeat->minor, heartbeat->power, heartbeat->calibrated_power,
                               heartbeat->rssi, heartbeat->time);
        scannerView->displayHeartbeat(heartbeatBeacon);
    }

}

void HCIDumpParser::displayStatus() {
    scannerView->displayStatus(*statusInformation);
}
