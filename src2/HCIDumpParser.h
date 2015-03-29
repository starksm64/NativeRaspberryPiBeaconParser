#ifndef HCIDumpParserLogic_H
#define HCIDumpParserLogic_H

#include "HCIDumpCommand.h"
#include "EventsWindow.h"
#include <MsgPublisher.h>
#include <ctime>
#include <fstream>
#include <map>
#include <memory>
extern "C" {
#include "hcidumpinternal.h"
}
extern "C" bool beacon_event_callback(beacon_info * info);

using namespace std;

class HCIDumpParser {
private:
    HCIDumpCommand parseCommand;
    unique_ptr<MsgPublisher> publisher;
    /** The count of messages to send in batches if > 0 */
    int batchCount;
    /** vector of beacon events when sending events in batchCount transactions to the broker */
    vector<Beacon> events;
    /** The uuid of the beacon associated with the scanner as its heartbeat/status signal */
    string scannerUUID;
    time_t currentTime;
    /** The time window of collected beacon_info events */
    EventsWindow timeWindow;

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

    void updateBeaconCounts(const beacon_info& info);
};
#endif
