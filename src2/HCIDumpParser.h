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

public:
    HCIDumpParser() : publisher(nullptr) {
    }

    void processHCI(HCIDumpCommand& parseCommand);
    void beaconEvent(const beacon_info *info);
    void cleanup();
};
#endif
