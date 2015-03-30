//
// Created by starksm on 3/28/15.
//

#include <sys/time.h>
#include "EventsWindow.h"

int64_t EventsWindow::currentMilliseconds() {
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

inline void addInfo(map<int32_t, beacon_info>& eventsMap, const beacon_info& info) {
    if(eventsMap.find(info.minor) == eventsMap.end()) {
        // Copy the data
        eventsMap[info.minor] = info;
        eventsMap[info.minor].count = 1;
    } else {
        beacon_info &windowInfo = eventsMap[info.minor];
        windowInfo.rssi += info.rssi;
        windowInfo.time += info.time;
        windowInfo.count++;
    }
}

unique_ptr<EventsBucket> EventsWindow::getCurrentBucket() {
    // Calculate the bucket averages
    for(map<int32_t, beacon_info>::iterator iter = eventsMap.begin(); iter != eventsMap.end(); iter ++) {
        beacon_info& info = eventsMap[iter->first];
        info.rssi /= info.count;
        info.time /= info.count;
    }
    // Copy the current events and return it
    unique_ptr<EventsBucket> window(new EventsBucket(eventsMap, eventCount, begin, end));
    // Clear the current event map
    eventsMap.clear();
    begin = end;
    end += 1000*windowSizeSeconds;
    eventCount = 0;
    return window;
}

unique_ptr<EventsBucket> EventsWindow::addEvent(const beacon_info& info) {
    unique_ptr<EventsBucket> window;
#ifdef PRINT_DEBUG
    printf("addEvent(time=%lld), end=%lld, diff=%d\n", info.time, end, (end - info.time));
#endif
    if(info.time < end) {
        // Update the beacon event counts
        addInfo(eventsMap, info);
        eventCount ++;
    } else {
        // Calculate the bucket averages
        for(map<int32_t, beacon_info>::iterator iter = eventsMap.begin(); iter != eventsMap.end(); iter ++) {
            beacon_info& info = eventsMap[iter->first];
            info.rssi /= info.count;
            info.time /= info.count;
        }
        // Copy the current events and return it
        window.reset(new EventsBucket(eventsMap, eventCount, begin, end));
        // Clear the current event map
        eventsMap.clear();
        begin = end;
        end += 1000*windowSizeSeconds;
        eventCount = 0;
        // Add the event to the next window
        addInfo(eventsMap, info);
        eventCount ++;
    }
    return window;
}
