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
            sprintf(event.uuid, "UUID-%.12d", count);
            event.minor = count % 150;
            event.rssi = -50 - count % 7;
            event.time = EventsWindow::currentMilliseconds();
            //exchanger->putEvent(new beacon_info(event));
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
    publisher.reset(MsgPublisher::create(parseCommand.getPubType(), parseCommand.getBrokerURL(), clientID, "", ""));
    timeWindow.reset(parseCommand.getAnalyzeWindow());
    if (parseCommand.isAnalyzeMode()) {
        printf("Running in analyze mode, window=%d seconds, begin=%lld\n", parseCommand.getAnalyzeWindow(),
               timeWindow.getBegin());
    }
    else if (!parseCommand.isSkipPublish()) {
        publisher->setUseTopics(!parseCommand.isUseQueues());
        publisher->setDestinationName(parseCommand.getDestinationName());
        if (batchCount > 0) {
            publisher->setUseTransactions(true);
            printf("Enabled transactions\n");
        }
        publisher->start(parseCommand.isAsyncMode());
        // Create a thread for the consumer
        eventExchanger.reset(new EventExchanger);
        eventConsumer.init(eventExchanger, publisher);
        consumerThread.reset(new thread(&BeaconEventConsumer::publishEvents, &eventConsumer));
        printf("Started event consumer thread\n");
    }
    else {
        printf("Skipping publish of parsed beacons\n");
    }

    /*
    // Generate test data
    thread testThread(generateTestEvents, eventExchanger.get());
    testThread.detach();
    */

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
}

void HCIDumpParser::beaconEvent(const beacon_info &info) {
    // Check for heartbeat
    bool isHeartbeat = scannerUUID.compare(info.uuid) == 0;
    // Merge the event into the current time window
    shared_ptr<EventsBucket> bucket = timeWindow.addEvent(info);
    eventCounts.addEvent(info);
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
    }
}

void HCIDumpParser::cleanup() {
    if (publisher)
        publisher->stop();
}

void HCIDumpParser::printBeaconCounts(Beacon beacon, const shared_ptr<EventsBucket> &bucket) {
    printf("Window: %s, parsed(%s): %s\n", beacon.toString().c_str());
    printBeaconCounts(bucket);
}

void HCIDumpParser::printBeaconCounts(const shared_ptr<EventsBucket> &bucket) {
    vector<char> tmp;
    bucket->toString(tmp);
    printf("%s\n", tmp.data());
}
