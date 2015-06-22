//
// Created by starksm on 3/28/15.
//

#ifndef NATIVESCANNER_EVENTSWINDOW_H
#define NATIVESCANNER_EVENTSWINDOW_H

#include <stdint.h>
#include <map>
#include <memory>
#include "hcidumpinternal.h"
#include "EventsBucket.h"

using namespace std;
//
//using EventsBucket = map<int32_t, beacon_info>;

/**
 * A class that represents a collection of beacon_info events seen within a time window
 */
class EventsWindow {
private:
    int32_t windowSizeSeconds;
    // Current analyze window begin/end in milliseconds to be compatible with beacon_info.time
    int64_t begin;
    int64_t end;
    int32_t eventCount;
    map<int32_t, beacon_info> eventsMap;

    /**
     * Just reset the current bucket begin and end
     */
    void resetCurrentBucket();
public:

    /**
     * Get the milliseconds since the common 1970-1-1 epoch.
     */
    static int64_t currentMilliseconds();

    int32_t getWindowSizeSeconds() const {
        return windowSizeSeconds;
    }

    int64_t getBegin() const {
        return begin;
    }

    int64_t getEnd() const {
        return end;
    }

    map<int32_t, beacon_info> const &getEventsMap() const {
        return eventsMap;
    }

    int32_t getEventCount() const {
        return eventCount;
    }

    /**
     * Get the current events bucket being accumulated
     */
    unique_ptr<EventsBucket> getCurrentBucket();
    /**
     * Setup the event bucket window size and begin/end based on the currentMilliseconds, and clear
     * any existing events.
     */
    int64_t reset(int32_t sizeInSeconds);
    /**
     * Add an event to the event bucket, returning the just finished bucket if the event flows
     * over into the next time window.
     */
    unique_ptr<EventsBucket> addEvent(const beacon_info& info, bool isHeartbeat);
};

#endif //NATIVESCANNER_EVENTSWINDOW_H
