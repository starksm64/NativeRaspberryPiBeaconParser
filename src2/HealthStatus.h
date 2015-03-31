//
// Created by Scott Stark on 3/30/15.
//

#ifndef NATIVESCANNER_HEALTHSTATUS_H
#define NATIVESCANNER_HEALTHSTATUS_H

#include <memory>
#include <string>
#include <thread>
#include "../src/MsgPublisher.h"

using namespace std;

/*
 * A class that monitors and reports:
 * 1. the state of the system load
 * 2. the raw event counts and beacon counts last seen
 */
class HealthStatus {
private:
    shared_ptr<MsgPublisher> publisher;
    string statusQueue;
    int statusInterval;
    unique_ptr<thread> monitorThread;
    mutable bool running;

    void monitorStatus();

public:

    shared_ptr<MsgPublisher> const &getPublisher() const {
        return publisher;
    }

    string &getStatusQueue() const {
        return statusQueue;
    }

    int getStatusInterval() const {
        return statusInterval;
    }

    bool isRunning() const {
        return running;
    }

    void setRunning(bool running) {
        HealthStatus::running = running;
    }

    /** Begin monitoring in the background, sending status messages to the indicated queue via the publisher
    */
    void start(shared_ptr<MsgPublisher> publisher, const string& queueName, int statusInterval);

    /**
     * Reset any counters
     */
    void reset();
    void stop();
};

#endif //NATIVESCANNER_HEALTHSTATUS_H
