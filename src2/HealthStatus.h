//
// Created by Scott Stark on 3/30/15.
//

#ifndef NATIVESCANNER_HEALTHSTATUS_H
#define NATIVESCANNER_HEALTHSTATUS_H

#include <memory>
#include <string>
#include <thread>
#include "StatusInformation.h"
#include "../src/MsgPublisher.h"

using namespace std;
using Properties = map<string, string>;

enum class StatusProperties {
    SystemTime,
    Uptime,
    Procs,
    LoadAverage,
    RawEventCount,
    PublishEventCount,
    MemTotal,
    MemFree,
    MemAvailable,
    SwapTotal,
    SwapFree,
    N_STATUS_PROPERTIES
};

/*
 * A class that monitors and reports:
 * 1. the state of the system load
 * 2. the raw event counts and beacon counts last seen
 */
class HealthStatus {
private:
    static const string statusPropertyNames[static_cast<unsigned int>(StatusProperties::N_STATUS_PROPERTIES)];

    shared_ptr<MsgPublisher> publisher;

    shared_ptr<StatusInformation> statusInformation;
    unique_ptr<thread> monitorThread;
    mutable bool running;

    void monitorStatus();
    void readLoadAvg(char *buffer, int size);
    void readMeminfo(Properties& properties);

public:

    shared_ptr<MsgPublisher> const &getPublisher() const {
        return publisher;
    }

    const string &getStatusQueue() const {
        return statusInformation->getStatusQueue();
    }

    int getStatusInterval() const {
        return statusInformation->getStatusInterval();
    }

    bool isRunning() const {
        return running;
    }

    void setRunning(bool running) {
        HealthStatus::running = running;
    }

    /** Begin monitoring in the background, sending status messages to the indicated queue via the publisher
    */
    void start(shared_ptr<MsgPublisher>& publisher, shared_ptr<StatusInformation>& statusInformation);

    /**
     * Reset any counters
     */
    void reset();
    void stop();

    static const string& getStatusPropertyName(StatusProperties property) {
        return statusPropertyNames[static_cast<unsigned int>(property)];
    }
};

#endif //NATIVESCANNER_HEALTHSTATUS_H
