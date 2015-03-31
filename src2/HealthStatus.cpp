//
// Created by Scott Stark on 3/30/15.
//

#include "HealthStatus.h"

void HealthStatus::monitorStatus() {
    while(running) {
        // Wait for statusInterval before
        this_thread::sleep_for(chrono::seconds(statusInterval));
    }
}

/** Begin monitoring in the background, sending status messages to the indicated queue via the publisher
*/
void HealthStatus::start(shared_ptr<MsgPublisher> publisher, const string& statusQueue, int statusInterval) {
    running = true;
    this->statusInterval = statusInterval;
    this->publisher = publisher;
    this->statusQueue = statusQueue;
    monitorThread.reset(new thread(HealthStatus::monitorStatus(), this));
}

/**
 * Reset any counters
 */
void HealthStatus::reset() {

}
void HealthStatus::stop() {
    running = false;
    if(monitorThread->joinable())
        monitorThread->join();
}
