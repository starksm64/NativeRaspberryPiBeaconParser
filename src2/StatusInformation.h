#ifndef NATIVESCANNER_EVENTCOUNTS_H
#define NATIVESCANNER_EVENTCOUNTS_H

#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include "ByteBuffer.h"
#include "EventsWindow.h"
#include "hcidumpinternal.h"
#include "SMA.h"

using namespace std;
using Properties = map<string, string>;

class StatusInformation {
private:
    string scannerID;
    string statusQueue;
    string bcastAddress;
    int bcastPort = 12345;
    int32_t statusInterval;
    int32_t rawEventCount;
    int32_t publishEventCount;
    int32_t heartbeatCount;
    EventsWindow statusWindow;
    shared_ptr<EventsBucket> lastWindow;
    SMA heartbeatRSSI;
    mutable mutex statusMutex;
    Properties lastStatus;
    atomic_bool statusUpdated;

public:

    StatusInformation() : scannerID(""), rawEventCount(0), publishEventCount(0), heartbeatCount(0), heartbeatRSSI(30),
                          statusUpdated(false),
                          statusInterval(30)
    {}

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
        statusWindow.reset(statusInterval);
    }

    void setScannerID(const string& scannerID) {
        StatusInformation::scannerID = scannerID;
    }
    const string& getScannerID() const {
        return scannerID;
    }

    int32_t getRawEventCount() const {
        return rawEventCount;
    }
    void addEvent(const beacon_info &info, bool isHeartbeat) {
        rawEventCount ++;
        if(isHeartbeat) {
            updateHeartbeatCount();
            heartbeatRSSI.add(info.rssi);
        }
        unique_ptr<EventsBucket> window = statusWindow.addEvent(info, isHeartbeat);
        if(window)
            lastWindow.reset(window.release());
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

    int32_t getHeartbeatRSSI() const {
        return heartbeatRSSI.avg();
    }

    void resetStatusWindow(int32_t sizeInSeconds) {
        statusWindow.reset(sizeInSeconds);
    }
    shared_ptr<EventsBucket>& getStatusWindow() {
        return lastWindow;
    }

    shared_ptr<Properties> getLastStatus() const {
        std::lock_guard<mutex> guard(statusMutex);
        Properties *copy = new map<string,string>(lastStatus);
        shared_ptr<Properties> currentStatus(copy);
        return currentStatus;
    }
    void setLastStatus(Properties &lastStatus) {
        std::lock_guard<mutex> guard(statusMutex);
        StatusInformation::lastStatus = lastStatus;
        statusUpdated = true;
    }
    bool isStatusChanged() {
        return statusUpdated;
    }
    void clearStatusChanged() {
        statusUpdated = false;
    }
    /**
     *
     */
    void encodeLastStatus(ByteBuffer& buffer, const vector<string>& names) {
        std::lock_guard<mutex> guard(statusMutex);
        buffer.writeInt(names.size());
        for(vector<string>::const_iterator it = names.begin(); it != names.end(); it ++) {
            const string& name = *it;
            string value = lastStatus[name];
            buffer.writeBytes(name);
            buffer.writeBytes(value);
        }
    }

    const string &getBcastAddress() const {
        return bcastAddress;
    }

    void setBcastAddress(const string &bcastAddress) {
        StatusInformation::bcastAddress = bcastAddress;
    }

    int getBcastPort() const {
        return bcastPort;
    }

    void setBcastPort(int bcastPort) {
        StatusInformation::bcastPort = bcastPort;
    }
};
#endif //NATIVESCANNER_EVENTCOUNTS_H
