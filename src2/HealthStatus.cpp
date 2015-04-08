//
// Created by Scott Stark on 3/30/15.
//

#include <string.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include "HealthStatus.h"

const string HealthStatus::statusPropertyNames[static_cast<unsigned int>(StatusProperties::N_STATUS_PROPERTIES)] = {
        string("ScannerID"),
        string("SystemTime"),
        string("Uptime"),
        string("Procs"),
        string("LoadAverage"),
        string("RawEventCount"),
        string("PublishEventCount"),
        string("HeartbeatCount"),
        string("MemTotalMb"),
        string("MemFreeMb"),
        string("MemAvailableMb"),
        string("SwapTotalMb"),
        string("SwapFreeMb"),
};

void HealthStatus::monitorStatus() {
    int statusInterval = statusInformation->getStatusInterval();
    const string& statusQueue = statusInformation->getStatusQueue();
    const string& scannerID = statusInformation->getScannerID();
    Properties statusProperties;
    const string& ScannerID = getStatusPropertyName(StatusProperties::ScannerID);
    const string& SystemTime = getStatusPropertyName(StatusProperties::SystemTime);
    const string& Uptime = getStatusPropertyName(StatusProperties::Uptime);
    const string& LoadAverage = getStatusPropertyName(StatusProperties::LoadAverage);
    const string& Procs = getStatusPropertyName(StatusProperties::Procs);
    const string& RawEventCount = getStatusPropertyName(StatusProperties::RawEventCount);
    const string& PublishEventCount = getStatusPropertyName(StatusProperties::PublishEventCount);
    const string& HeartbeatCount = getStatusPropertyName(StatusProperties::HeartbeatCount);
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemActive = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    struct timeval  tv;
    struct tm *tm;

    while(running) {
        // Wait for statusInterval before
        this_thread::sleep_for(chrono::seconds(statusInterval));

        statusProperties[ScannerID] = scannerID;

        // Time
        gettimeofday(&tv, nullptr);
        tm = localtime(&tv.tv_sec);
        char timestr[256];
        strftime(timestr, 128, "%F %T", tm);
        statusProperties[SystemTime] = timestr;
        printf("--- HealthStatus: %s\n", timestr);

        // Get the load average
        char tmp[128];
        readLoadAvg(tmp, sizeof(tmp));
        statusInformation->setLoadAvg(tmp);
        // Create the status message properties
        statusProperties[LoadAverage] = tmp;
        statusProperties[RawEventCount] = to_string(statusInformation->getRawEventCount());
        statusProperties[PublishEventCount] = to_string(statusInformation->getPublishEventCount());
        statusProperties[HeartbeatCount] = to_string(statusInformation->getHeartbeatCount());
        printf("RawEventCount: %d, PublishEventCount: %d, HeartbeatCount: %d",  statusInformation->getRawEventCount(),
               statusInformation->getPublishEventCount(), statusInformation->getHeartbeatCount());
        struct sysinfo info;
        if(sysinfo(&info)) {
            perror("Failed to read sysinfo");
        } else {
            int mb = 1024*1024;
            int days = info.uptime / (24*3600);
            int hours = (info.uptime - days * 24*3600) / 3600;
            int minute = (info.uptime - days * 24*3600 - hours*3600) / 60;
            sprintf(tmp, "uptime: %ld, days:%d, hrs: %d, min: %d", info.uptime, days, hours, minute);
            statusProperties[Uptime] = tmp;
            printf("%s\n", tmp);
            sprintf(tmp, "%.2f, %.2f, %.2f", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);
            printf("loadavg: %s\n", tmp);
            statusProperties[LoadAverage] = tmp;
            statusProperties[Procs] = to_string(info.procs);
            statusProperties[MemTotal] = to_string(info.totalram*info.mem_unit / mb);
            printf("MemTotal: %ld;  MemFree: %ld\n", info.totalram*info.mem_unit,  info.freeram*info.mem_unit);
            statusProperties[MemActive] = to_string((info.totalram - info.freeram)*info.mem_unit / mb);
            statusProperties[MemFree] = to_string(info.freeram*info.mem_unit / mb);
            statusProperties[SwapFree] = to_string(info.freeswap*info.mem_unit / mb);
            statusProperties[SwapTotal] = to_string(info.totalswap*info.mem_unit / mb);
        }
        fflush(stdout);

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

void HealthStatus::readMeminfo(Properties& properties) {
    FILE *meminfo = fopen ("/proc/meminfo", "r");
    if (meminfo == NULL) {
        perror ("Failed to open /proc/meminfo");
    }

    char buffer[64];
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemAvailable = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    while(fgets (buffer, sizeof(buffer), meminfo)){
        // Replace trailing newline char with nil
        size_t length = strlen(buffer);
        buffer[length-1] = '\0';
        if(MemTotal.compare(0, MemTotal.size(), buffer, length))
            properties[MemTotal] = buffer;
        else if(MemFree.compare(0, MemFree.size(), buffer, length))
            properties[MemFree] = buffer;
        else if(MemAvailable.compare(0, MemAvailable.size(), buffer, length))
            properties[MemAvailable] = buffer;
        else if(SwapTotal.compare(0, SwapTotal.size(), buffer, length))
            properties[SwapTotal] = buffer;
        else if(SwapFree.compare(0, SwapFree.size(), buffer, length))
            properties[SwapFree] = buffer;
    }
    fclose(meminfo);

}
