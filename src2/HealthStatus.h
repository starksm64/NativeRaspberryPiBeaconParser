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

/**
 * The names of the properties passed in the messages to the scanner status queue
 */
enum class StatusProperties {
    ScannerID,          // the name of the scanner passed in via the --scannerID argument
    SystemTime,         // strftime(timestr, 128, "%F %T", tm) = YYYY-MM-DD HH:MM:SS
    Uptime,             // uptime in seconds as string formatted as "uptime: %ld, days:%d, hrs: %d, min: %d"
    Procs,              // number of procs active on the scanner
    LoadAverage,        // load averages for the past 1, 5, and 15 minutes "load average: 0.00, 0.01, 0.05"
    RawEventCount,      // Raw number of BLE iBeacon type of events from the bluetooth stack
    PublishEventCount,  // The number of time windowed events pushed to the message broker
    HeartbeatCount,     // The number of events from the scanner's associated --heartbeatUUID beacon
    MemTotal,           // Total memory on scanner in MB
    MemFree,            // Free memory on scanner in MB
    MemActive,          // Total - Free memory on scanner in MB
    SwapTotal,          // Total swap memory on scanner in MB
    SwapFree,           // Free swap memory on scanner in MB
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
