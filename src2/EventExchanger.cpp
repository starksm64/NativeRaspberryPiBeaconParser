//
// Created by starksm on 3/28/15.
//

#include "EventExchanger.h"

/**
 * The beacon_info pointer here must be a new allocated memory whose deallocation will be controlled by the
 * exchanger.
 */
void EventExchanger::putEvent(shared_ptr<EventsBucket> info) {
    std::lock_guard<mutex> guard(contentsMutex);
    events.push(info);
    putCount ++;
}

shared_ptr<EventsBucket> EventExchanger::takeEvent() {
    shared_ptr<EventsBucket> info;
    std::lock_guard<mutex> guard(contentsMutex);
    if(!events.empty()) {
        info = events.front();
        events.pop();
        takeCount ++;
    }
    return info;
}

int EventExchanger::getPutCount() const {
    return putCount;
}

int EventExchanger::getTakeCount() const {
    return takeCount;
}

size_t EventExchanger::size() const {
    return events.size();
}
