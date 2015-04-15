//
// Created by Scott Stark on 3/30/15.
//

#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include "HealthStatus.h"

const string HealthStatus::statusPropertyNames[static_cast<unsigned int>(StatusProperties::N_STATUS_PROPERTIES)] = {
        string("ScannerID"),
        string("HostIPAddress"),
        string("SystemTime"),
        string("SystemTimeMS"),
        string("Uptime"),
        string("Procs"),
        string("LoadAverage"),
        string("RawEventCount"),
        string("PublishEventCount"),
        string("HeartbeatCount"),
        string("HeartbeatRSSI"),
        string("EventsWindow"),
        string("MemTotal"),
        string("MemFree"),
        string("MemAvailable"),
        string("SwapTotal"),
        string("SwapFree"),
};

void HealthStatus::monitorStatus() {
    int statusInterval = statusInformation->getStatusInterval();
    const string& statusQueue = statusInformation->getStatusQueue();
    const string& scannerID = statusInformation->getScannerID();
    Properties statusProperties;
    const string& ScannerID = getStatusPropertyName(StatusProperties::ScannerID);
    const string& HostIPAddress = getStatusPropertyName(StatusProperties::HostIPAddress);
    const string& SystemTime = getStatusPropertyName(StatusProperties::SystemTime);
    const string& SystemTimeMS = getStatusPropertyName(StatusProperties::SystemTimeMS);
    const string& Uptime = getStatusPropertyName(StatusProperties::Uptime);
    const string& LoadAverage = getStatusPropertyName(StatusProperties::LoadAverage);
    const string& Procs = getStatusPropertyName(StatusProperties::Procs);
    const string& RawEventCount = getStatusPropertyName(StatusProperties::RawEventCount);
    const string& PublishEventCount = getStatusPropertyName(StatusProperties::PublishEventCount);
    const string& HeartbeatCount = getStatusPropertyName(StatusProperties::HeartbeatCount);
    const string& HeartbeatRSSI = getStatusPropertyName(StatusProperties::HeartbeatRSSI);
    const string& EventsWindow = getStatusPropertyName(StatusProperties::EventsWindow);
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemActive = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    struct timeval  tv;
    struct tm *tm;
    struct sysinfo beginInfo;

    if(sysinfo(&beginInfo)) {
        perror("Failed to read sysinfo");
    };
// Send an initial hello status msg with the host inet address
    char hostIPAddress[128];
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }
    for (ifa = ifaddr, n = 0; ifa != nullptr; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == nullptr)
            continue;
        family = ifa->ifa_addr->sa_family;
        if(family == AF_INET) {
            char tmp[sizeof(hostIPAddress)];
            int err = getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), tmp, sizeof(tmp), nullptr, 0, NI_NUMERICHOST);
            if(err != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
            }
            printf("Found hostIPAddress of: %s\n", tmp);
            if(strncasecmp(tmp, "127.0", 5) == 0) {
                printf("Skipping localhost address\n");
                continue;
            }
            strcpy(hostIPAddress, tmp);
        }
    }

    freeifaddrs(ifaddr);

    while(running) {
        statusProperties[ScannerID] = scannerID;
        statusProperties[HostIPAddress] = hostIPAddress;

        // Time
        gettimeofday(&tv, nullptr);
        tm = localtime(&tv.tv_sec);
        int64_t nowMS = tv.tv_sec;
        nowMS *= 1000;
        nowMS += tv.tv_usec/1000;
        char timestr[256];
        strftime(timestr, 128, "%F %T", tm);
        statusProperties[SystemTime] = timestr;
        statusProperties[SystemTimeMS] = to_string(nowMS);
        printf("--- HealthStatus: %s\n", timestr);

        // Get the load average
        char tmp[128];
        readLoadAvg(tmp, sizeof(tmp));
        // Create the status message properties
        statusProperties[LoadAverage] = tmp;
        statusProperties[RawEventCount] = to_string(statusInformation->getRawEventCount());
        statusProperties[PublishEventCount] = to_string(statusInformation->getPublishEventCount());
        statusProperties[HeartbeatCount] = to_string(statusInformation->getHeartbeatCount());
        statusProperties[HeartbeatRSSI] = to_string(statusInformation->getHeartbeatRSSI());
        printf("RawEventCount: %d, PublishEventCount: %d, HeartbeatCount: %d, HeartbeatRSSI: %d\n",  statusInformation->getRawEventCount(),
               statusInformation->getPublishEventCount(), statusInformation->getHeartbeatCount(), statusInformation->getHeartbeatRSSI());

        // Events bucket info
        shared_ptr<EventsBucket> eventsBucket(statusInformation->getStatusWindow());
        if(eventsBucket) {
            vector<char> eventsBucketStr;
            eventsBucket->toSimpleString(eventsBucketStr);
            statusProperties[EventsWindow] = eventsBucketStr.data();
            printf("EventsBucket[%d]: %s\n", eventsBucket->size(), eventsBucketStr.data());
        }

        // System uptime, load, procs, memory info
        struct sysinfo info;
        if(sysinfo(&info)) {
            perror("Failed to read sysinfo");
        } else {
            int mb = 1024*1024;
            long uptimeDiff = info.uptime - beginInfo.uptime;
            long days = uptimeDiff / (24*3600);
            long hours = (uptimeDiff - days * 24*3600) / 3600;
            long minute = (uptimeDiff - days * 24*3600 - hours*3600) / 60;
            long seconds = uptimeDiff - days * 24*3600 - hours*3600 - minute*60;
            sprintf(tmp, "uptime: %ld, days:%ld, hrs:%ld, min:%ld, sec:%ld", uptimeDiff, days, hours, minute, seconds);
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
            statusInformation->setLastStatus(statusProperties);
        }
        fflush(stdout);

        // Publish the status
        if(publisher) {
            try {
                publisher->publishProperties(statusQueue, statusProperties);
                statusInformation->setLastStatus(statusProperties);
            } catch (exception &e) {
                fprintf(stderr, "Failed to send status, %s\n", e.what());
            }
        }

        // Wait for statusInterval before
        this_thread::sleep_for(chrono::seconds(statusInterval));
    }
}

void HealthStatus::calculateStatus(StatusInformation& statusInformation) {
    const string& scannerID = statusInformation.getScannerID();
    Properties statusProperties;
    const string& ScannerID = getStatusPropertyName(StatusProperties::ScannerID);
    const string& SystemTime = getStatusPropertyName(StatusProperties::SystemTime);
    const string& Uptime = getStatusPropertyName(StatusProperties::Uptime);
    const string& LoadAverage = getStatusPropertyName(StatusProperties::LoadAverage);
    const string& Procs = getStatusPropertyName(StatusProperties::Procs);
    const string& RawEventCount = getStatusPropertyName(StatusProperties::RawEventCount);
    const string& PublishEventCount = getStatusPropertyName(StatusProperties::PublishEventCount);
    const string& HeartbeatCount = getStatusPropertyName(StatusProperties::HeartbeatCount);
    const string& HeartbeatRSSI = getStatusPropertyName(StatusProperties::HeartbeatRSSI);
    const string& EventsWindow = getStatusPropertyName(StatusProperties::EventsWindow);
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemActive = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    struct timeval  tv;
    struct tm *tm;

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
    // Create the status message properties
    statusProperties[LoadAverage] = tmp;
    statusProperties[RawEventCount] = to_string(statusInformation.getRawEventCount());
    statusProperties[PublishEventCount] = to_string(statusInformation.getPublishEventCount());
    statusProperties[HeartbeatCount] = to_string(statusInformation.getHeartbeatCount());
    statusProperties[HeartbeatRSSI] = to_string(statusInformation.getHeartbeatRSSI());
    printf("RawEventCount: %d, PublishEventCount: %d, HeartbeatCount: %d, HeartbeatRSSI: %d\n",  statusInformation.getRawEventCount(),
           statusInformation.getPublishEventCount(), statusInformation.getHeartbeatCount(), statusInformation.getHeartbeatRSSI());

    // System uptime, load, procs, memory info
    struct sysinfo info;
    if(sysinfo(&info)) {
        perror("Failed to read sysinfo");
    } else {
        int mb = 1024*1024;
        int days = info.uptime / (24*3600);
        int hours = (info.uptime - days * 24*3600) / 3600;
        int minute = (info.uptime - days * 24*3600 - hours*3600) / 60;
        sprintf(tmp, "uptime: %ld, days:%d, hrs:%d, min:%d", info.uptime, days, hours, minute);
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
    statusInformation.setLastStatus(statusProperties);
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
