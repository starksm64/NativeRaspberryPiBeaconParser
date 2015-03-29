//
// Created by starksm on 3/28/15.
//

#include <sys/time.h>
#include "EventsWindow.h"

static inline int64_t currentMilliseconds() {
    timeval now;
    gettimeofday(&now, nullptr);

    int64_t nowMS = now.tv_sec;
    nowMS *= 1000;
    nowMS += now.tv_usec/1000;
    return nowMS;
}

int64_t EventsWindow::reset(int32_t sizeInSeconds) {
    windowSizeSeconds = sizeInSeconds;
    begin = currentMilliseconds();
    end = begin + 1000*windowSizeSeconds;
}

unique_ptr<EventsBucket> EventsWindow::addEvent(beacon_info info) {
    unique_ptr<EventsBucket> window;
    if(info.time < end) {
        // Update the beacon event counts
        beacon_info windowInfo = eventsMap[info.minor];
        windowInfo.rssi += info.rssi;
        windowInfo.time += info.time;
        windowInfo.count ++;
    } else {
        // Calculate the bucket averages
        for(EventsBucket::iterator iter = eventsMap.begin(); iter != eventsMap.end(); iter ++) {
            iter->second.rssi /= iter->second.count;
            iter->second.time /= iter->second.count;
        }
        // Copy the current events and return it
        window.reset(new map<int32_t, beacon_info>(eventsMap));
    }
    return window;
}
