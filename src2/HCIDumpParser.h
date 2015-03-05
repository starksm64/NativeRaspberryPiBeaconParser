#ifndef HCIDumpParserLogic_H
#define HCIDumpParserLogic_H

#include "HCIDumpCommand.h"
#include <MsgPublisher.h>
#include <fstream>
extern "C" {
#include "hcidumpinternal.h"
}
extern "C" bool beacon_event_callback(const beacon_info *info);

using namespace std;

class HCIDumpParser {
private:
    HCIDumpCommand *parseCommand;
    MsgPublisher *publisher;
    /** The count of messages to send in batches if > 0 */
    int batchCount;
    /** vector of beacon events when sending events in batchCount transactions to the broker */
    vector<Beacon> events;
    /** The uuid of the beacon associated with the scanner as its heartbeat/status signal */
    string scannerUUID;

public:
    HCIDumpParser() : publisher(nullptr), batchCount(0), scannerUUID("") {
    }


    int getBatchCount() const {
        return batchCount;
    }
    void setBatchCount(int batchCount) {
        HCIDumpParser::batchCount = batchCount;
    }

    void processHCI(HCIDumpCommand& parseCommand);
    void beaconEvent(const beacon_info *info);
    void cleanup();
};
#endif
