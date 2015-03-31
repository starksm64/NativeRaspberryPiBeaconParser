//
// Created by Scott Stark on 3/30/15.
//

#include <string.h>
#include "HealthStatus.h"

const string HealthStatus::statusPropertyNames[static_cast<unsigned int>(StatusProperties::N_STATUS_PROPERTIES)] = {
        string("LoadAverage"),
        string("RawEventCount")
};

void HealthStatus::monitorStatus() {
    int statusInterval = statusInformation->getStatusInterval();
    string statusQueue = statusInformation->getStatusQueue();
    map<string, string> statusProperties;

    while(running) {
        // Wait for statusInterval before
        this_thread::sleep_for(chrono::seconds(statusInterval));
        // Get the load average
        char tmp[64];
        readLoadAvg(tmp, sizeof(tmp));
        statusInformation->setLoadAvg(tmp);
        // Create the status message properties
        statusProperties[getStatusPropertyName(StatusProperties::LoadAverage)] = tmp;
        statusProperties[getStatusPropertyName(StatusProperties::RawEventCount)] = to_string(statusInformation->getRawEventCount());
        // Publish the status
        try {
            publisher->publishProperties(statusQueue, statusProperties);
        } catch(exception& e) {
            fprintf(stderr, "Failed to send status, %s\n", e.what());
        }
    }
}

/** Begin monitoring in the background, sending status messages to the indicated queue via the publisher
*/
void HealthStatus::start(shared_ptr<MsgPublisher>& publisher, shared_ptr<StatusInformation>& statusInformation) {
    running = true;
    this->statusInformation = statusInformation;
    this->publisher = publisher;
    monitorThread.reset(new thread(&HealthStatus::monitorStatus, this));
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

void HealthStatus::readLoadAvg(char *buffer, int size) {
    FILE *loadavg = fopen ("/proc/loadavg", "r");
    if (loadavg == NULL) {
        perror ("Failed to open /proc/loadavg");
    }

    if (!fgets (buffer, size, loadavg)){
        perror ("Failed to read /proc/loadavg");
    }
    fclose(loadavg);
    // Replace trailing newline char with nil
    size_t length = strlen(buffer);
    buffer[length-1] = '\0';
}
