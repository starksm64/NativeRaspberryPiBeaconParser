//
// Created by starksm on 3/28/15.
//

#ifndef NATIVESCANNER_BEACONEVENTCONSUMER_H
#define NATIVESCANNER_BEACONEVENTCONSUMER_H

#include <memory>
#include <MsgPublisher.h>
#include "EventExchanger.h"
#include "HCIDumpCommand.h"

using namespace std;

class BeaconEventConsumer {
private:
    HCIDumpCommand parseCommand;
    /** The count of messages to send in batches if > 0 */
    int batchCount;
    /** vector of beacon events when sending events in batchCount transactions to the broker */
    vector<Beacon> events;
    shared_ptr<MsgPublisher> publisher;
    shared_ptr<EventExchanger> exchanger;

public:

    int getBatchCount() const {
        return batchCount;
    }
    void setBatchCount(int batchCount) {
        BeaconEventConsumer::batchCount = batchCount;
    }

    void publishEvents();

    HCIDumpCommand const &getParseCommand() const {
        return parseCommand;
    }
    void setParseCommand(HCIDumpCommand const &parseCommand) {
        BeaconEventConsumer::parseCommand = parseCommand;
    }

    void init(shared_ptr<EventExchanger> exchanger, shared_ptr<MsgPublisher>& msgPublisher);

};

#endif //NATIVESCANNER_BEACONEVENTCONSUMER_H
