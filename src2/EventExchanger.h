//
// Created by starksm on 3/28/15.
//

#ifndef NATIVESCANNER_EVENTEXCHANGER_H
#define NATIVESCANNER_EVENTEXCHANGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <atomic>
#include "hcidumpinternal.h"
#include "EventsBucket.h"

using namespace std;

/**
 * Class for exchanging beacon_info values from the scanner producer stack and the message sending
 * consumer thread
 */
class EventExchanger {
private:
    mutable mutex contentsMutex;
    queue<shared_ptr<EventsBucket>> events;
    atomic<int> takeCount;
    atomic<int> putCount;

public:
    EventExchanger() : takeCount{0}, putCount{0}
    {}

    void putEvent(shared_ptr<EventsBucket> info);
    shared_ptr<EventsBucket> takeEvent();
    int getPutCount() const;
    int getTakeCount() const;
    size_t size() const;
};

#endif //NATIVESCANNER_EVENTEXCHANGER_H
