#ifndef HCIDumpParserLogic_H
#define HCIDumpParserLogic_H

#include "HCIDumpCommand.h"
#include "EventsWindow.h"
#include "BeaconEventConsumer.h"
#include "EventCounts.h"
#include "EventExchanger.h"
#include <MsgPublisher.h>
#include <ctime>
#include <fstream>
#include <map>
#include <memory>
#include <thread>

extern "C" {
#include "hcidumpinternal.h"
}
extern "C" bool beacon_event_callback(beacon_info * info);

using namespace std;

class HCIDumpParser {
private:
    /** Command line argument information */
    HCIDumpCommand parseCommand;
    /** The interface for the messaging layer publisher */
    shared_ptr<MsgPublisher> publisher;
    /** The thread for the publishing beacon_info events via the MsgPublisher */
    unique_ptr<thread> consumerThread;
    BeaconEventConsumer eventConsumer;
    shared_ptr<EventExchanger> eventExchanger;

    /** The count of messages to send in batches if > 0 */
    int batchCount;
    /** vector of beacon events when sending events in batchCount transactions to the broker */
    vector<Beacon> events;
    /** The uuid of the beacon associated with the scanner as its heartbeat/status signal */
    string scannerUUID;
    time_t currentTime;
    /** The time window of collected beacon_info events */
    EventsWindow timeWindow;
    EventCounts eventCounts;

    inline bool shouldSendMessages() {
        if(events.size() >= batchCount)
            return true;
        time_t now;
        time(&now);
        if((now - currentTime) > 5)
            return true;
        currentTime = now;
        return false;
    }

public:
    HCIDumpParser() : publisher(nullptr), batchCount(0), scannerUUID("") {
    }


    int getBatchCount() const {
        return batchCount;
    }
    void setBatchCount(int batchCount) {
        HCIDumpParser::batchCount = batchCount;
        eventConsumer.setBatchCount(batchCount);
    }

    string getScannerUUID() const {
        return scannerUUID;
    }

    void setScannerUUID(string &scannerUUID) {
        HCIDumpParser::scannerUUID = scannerUUID;
    }

    void processHCI(HCIDumpCommand& parseCommand);
    void beaconEvent(const beacon_info& info);
    void cleanup();

    void printBeaconCounts(Beacon beacon, const shared_ptr<EventsBucket> &bucket);
    void printBeaconCounts(const shared_ptr<EventsBucket> &bucket);
};
#endif
