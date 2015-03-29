//
// Created by starksm on 3/28/15.
//

#include "MessageExchanger.h"

void MessageExchanger::putEvent(event_info* info) {
    std::lock_guard<mutex> guard(contentsMutex);
    events.push(info);
    putCount ++;
}

unique_ptr<event_info> MessageExchanger::takeEvent() {
    unique_ptr<event_info> info;
    std::lock_guard<mutex> guard(contentsMutex);
    if(!events.empty()) {
        info.reset(events.front());
        events.pop();
        takeCount ++;
    }
    return info;
}

int MessageExchanger::getPutCount() const {
    return putCount;
}

int MessageExchanger::getTakeCount() const {
    return takeCount;
}

size_t MessageExchanger::size() const {
    return events.size();
}
