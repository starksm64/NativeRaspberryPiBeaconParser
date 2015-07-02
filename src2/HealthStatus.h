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
#include "../socket/PracticalSocket.h"

using namespace std;
using Properties = map<string, string>;

/**
 * The names of the properties passed in the messages to the scanner status queue
 */
enum class StatusProperties {
    ScannerID,          // the name of the scanner passed in via the --scannerID argument
    HostIPAddress,      // the ip address of the scanner host
    MACAddress,         // the MAC address of the scanner host
    SystemType,         // Type of the system; PiB, PiB+, P2B, IntelNUC, BeagleBoneBlack, ...
    SystemOS,           // Type of the operating system; Pidora, Raspbian, ...
    SystemTime,         // strftime(timestr, 128, "%F %T", tm) = YYYY-MM-DD HH:MM:SS
    SystemTimeMS,       // system time in milliseconds since epoch
    Uptime,             // uptime of scanner process in seconds as string formatted as "uptime: %ld, days:%d, hrs: %d, min: %d"
    SystemUptime,       // uptime of system in seconds as string formatted as "uptime: %ld, days:%d, hrs: %d, min: %d"
    Procs,              // number of procs active on the scanner
    LoadAverage,        // load averages for the past 1, 5, and 15 minutes "load average: 0.00, 0.01, 0.05"
    RawEventCount,      // Raw number of BLE iBeacon type of events from the bluetooth stack
    PublishEventCount,  // The number of time windowed events pushed to the message broker
    HeartbeatCount,     // The number of events from the scanner's associated --heartbeatUUID beacon
    HeartbeatRSSI,      // The average RSSI for the scanner's associated --heartbeatUUID beacon
    EventsWindow,       // The counts of beacon events as a sequence of +{minorID}: {count}; values
    ActiveBeacons,      // The count of beacons active in the last events window
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
    void broadcastInformation(unique_ptr<UDPSocket>& bcast);

public:

    static void getHostInfo(char* hostIPAddress, char* macaddr);
    static string determineSystemType();
    static string determineSystemOS();

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

    /**
     * Run through the status property calculations for debugging
     */
    void calculateStatus(StatusInformation& statusInformation);

    static const string& getStatusPropertyName(StatusProperties property) {
        return statusPropertyNames[static_cast<unsigned int>(property)];
    }
    static vector<string> getStatusPropertyNames(const vector<StatusProperties>& properties) {
        vector<string> names;
        for(vector<StatusProperties>::const_iterator it = properties.begin(); it != properties.end(); it ++) {
            names.push_back(getStatusPropertyName(*it));
        }
        return names;
    }
};

#endif //NATIVESCANNER_HEALTHSTATUS_H
