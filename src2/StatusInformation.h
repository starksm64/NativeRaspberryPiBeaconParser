#ifndef NATIVESCANNER_EVENTCOUNTS_H
#define NATIVESCANNER_EVENTCOUNTS_H

#include <string>
#include "hcidumpinternal.h"

using namespace std;

class StatusInformation {
private:
    string scannerID;
    string statusQueue;
    string loadAvg;
    int32_t statusInterval;
    int32_t rawEventCount;
    int32_t publishEventCount;
    int32_t heartbeatCount;

public:

    StatusInformation() : scannerID(""), rawEventCount(0) {}

    const string &getStatusQueue() const {
        return statusQueue;
    }

    void setStatusQueue(const string &statusQueue) {
        StatusInformation::statusQueue = statusQueue;
    }

    int getStatusInterval() const {
        return statusInterval;
    }

    void setStatusInterval(int statusInterval) {
        StatusInformation::statusInterval = statusInterval;
    }

    void setScannerID(const string& scannerID) {
        StatusInformation::scannerID = scannerID;
    }
    const string& getScannerID() const {
        return scannerID;
    }

    const string& getLoadAvg() const {
        return loadAvg;
    }

    void setLoadAvg(string loadAvg) {
        StatusInformation::loadAvg = loadAvg;
    }

    int32_t getRawEventCount() const {
        return rawEventCount;
    }
    void addEvent(const beacon_info &info, bool isHeartbeat) {
        rawEventCount ++;
        if(isHeartbeat)
            updateHeartbeatCount();
    }

    int32_t getPublishEventCount() const {
        return publishEventCount;
    }
    int32_t updatePublishEventCount() {
        return publishEventCount ++;
    }


    int32_t getHeartbeatCount() const {
        return heartbeatCount;
    }
    int32_t updateHeartbeatCount() {
        return heartbeatCount ++;
    }
};
#endif //NATIVESCANNER_EVENTCOUNTS_H
