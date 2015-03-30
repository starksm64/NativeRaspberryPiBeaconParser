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

unique_ptr<EventsBucket> EventsWindow::addEvent(const beacon_info& info) {
    unique_ptr<EventsBucket> window;
#ifdef PRINT_DEBUG
    printf("addEvent(time=%lld), end=%lld, diff=%d\n", info.time, end, (end - info.time));
#endif
    if(info.time < end) {
        // Update the beacon event counts
        beacon_info& windowInfo = eventsMap[info.minor];
        windowInfo.rssi += info.rssi;
        windowInfo.time += info.time;
        windowInfo.count ++;
        eventCount ++;
    } else {
        // Calculate the bucket averages
        for(map<int32_t, beacon_info>::iterator iter = eventsMap.begin(); iter != eventsMap.end(); iter ++) {
            iter->second.rssi /= iter->second.count;
            iter->second.time /= iter->second.count;
        }
        // Copy the current events and return it
        window.reset(new EventsBucket(eventsMap, eventCount, begin, end));
        // Clear the current event map
        eventsMap.clear();
        begin = end;
        end += 1000*windowSizeSeconds;
        eventCount = 0;
    }
    return window;
}
