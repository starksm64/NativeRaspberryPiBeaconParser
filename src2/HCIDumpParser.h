#ifndef HCIDumpParserLogic_H
#define HCIDumpParserLogic_H

#include "HCIDumpCommand.h"
#include <MqttPublisher.h>
#include <fstream>
extern "C" {
#include "hcidumpinternal.h"
}
extern "C" void beacon_event_callback(const beacon_info *info);

using namespace std;

class HCIDumpParser {
private:
    HCIDumpCommand *parseCommand;
    MqttPublisher *mqtt;

public:
    HCIDumpParser() : mqtt(nullptr) {
    }

    void processHCI(HCIDumpCommand& parseCommand);
    void beaconEvent(const beacon_info *info);
    void cleanup();
};
#endif
