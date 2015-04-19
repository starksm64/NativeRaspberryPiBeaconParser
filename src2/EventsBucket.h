//
// Created by starksm on 3/29/15.
//

#ifndef NATIVESCANNER_EVENTSBUCKET_H
#define NATIVESCANNER_EVENTSBUCKET_H

#include <stdint.h>
#include <map>
#include <vector>
#include "hcidumpinternal.h"

using namespace std;

class EventsBucket {
private:
    int64_t bucketStart;
    int64_t bucketEnd;
    int32_t eventCount;
    map<int32_t, beacon_info> bucket;

public:
    EventsBucket(map<int32_t, beacon_info> &bucket, int32_t eventCount, int64_t start, int64_t end) :
            eventCount(eventCount), bucket(bucket), bucketStart(start), bucketEnd(end)
    { }

    map<int32_t, beacon_info>::const_iterator begin() {
        return bucket.begin();
    }

    map<int32_t, beacon_info>::const_iterator end() {
        return bucket.end();
    }


    int32_t getEventCount() const {
        return eventCount;
    }

    void setEventCount(int32_t eventCount) {
        EventsBucket::eventCount = eventCount;
    }


    int64_t getBucketStart() const {
        return bucketStart;
    }

    void setBucketStart(int64_t bucketStart) {
        EventsBucket::bucketStart = bucketStart;
    }

    int64_t getBucketEnd() const {
        return bucketEnd;
    }

    void setBucketEnd(int64_t bucketEnd) {
        EventsBucket::bucketEnd = bucketEnd;
    }

    size_t size() {
        return bucket.size();
    }

    void toTimeWindowString(vector<char> &output) {
        char timestr[256];
        char *ptr = timestr;
        struct timeval tv;
        tv.tv_sec = bucketStart / 1000;
        tv.tv_usec = 0;
        struct tm tm;
        localtime_r(&tv.tv_sec, &tm);
        int count = strftime(timestr, 128, "%R:%S", &tm);
        output.insert(output.end(), ptr, ptr + count);
        ptr += count;
        tv.tv_sec = bucketEnd / 1000;
        localtime_r(&tv.tv_sec, &tm);
        count = strftime(ptr, 128, "-%R:%S", &tm);
        output.insert(output.end(), ptr, ptr + count);
        output.push_back('\0');
    }

    /**
     * The beacon counts string with a leading timestamp
     */
    void toString(vector<char> &output) {
        char timestr[256];
        char tmp[256];
        char *ptr = tmp;
        struct timeval tv;
        tv.tv_sec = bucketStart / 1000;
        tv.tv_usec = 0;
        struct tm tm;
        localtime_r(&tv.tv_sec, &tm);
        strftime(timestr, 128, "%r", &tm);
        // Report the stats for this time window and then reset
        int width = bucketEnd - bucketStart;
        int count = snprintf(tmp, sizeof(tmp), "+++ Beacon counts for window(%ld,%d): %s\n", size(), width,
                             timestr);
        output.insert(output.end(), ptr, ptr + count);
        toSimpleString(output);
    }
    /**
     * Just the beacon counts string
     */
    void toSimpleString(vector<char> &output) {
        char tmp[256];
        char *ptr = tmp;
        map<int32_t, beacon_info>::const_iterator iter = begin();
        while (iter != end()) {
            int32_t count = snprintf(tmp, sizeof(tmp), "+%d=%d; ", iter->first, iter->second.count);
            output.insert(output.end(), ptr, ptr + count);
            iter++;
        }
        output.push_back('\n');
        output.push_back('\0');
    }
};

#endif //NATIVESCANNER_EVENTSBUCKET_H
