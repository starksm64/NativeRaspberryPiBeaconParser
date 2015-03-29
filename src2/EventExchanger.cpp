//
// Created by starksm on 3/28/15.
//

#include "EventExchanger.h"

void EventExchanger::putEvent(beacon_info* info) {
    std::lock_guard<mutex> guard(contentsMutex);
    events.push(info);
    putCount ++;
}

unique_ptr<beacon_info> EventExchanger::takeEvent() {
    unique_ptr<beacon_info> info;
    std::lock_guard<mutex> guard(contentsMutex);
    if(!events.empty()) {
        info.reset(events.front());
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
