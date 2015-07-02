//
// Created by Scott Stark on 3/30/15.
//

#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include "HealthStatus.h"
#include "../socket/PracticalSocket.h"
#include <chrono>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>

using namespace std::chrono;

static void getMACAddress(std::string _iface,unsigned char MAC[6]) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , _iface.c_str() , IFNAMSIZ-1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    for(unsigned int i=0;i<6;i++)
        MAC[i] = ifr.ifr_hwaddr.sa_data[i];
    close(fd);
}
/**
 * Lookup host ip and macaddr
 * @param char hostIPAddress[128]
 * @param char macaddr[32]
 */
void HealthStatus::getHostInfo(char* hostIPAddress, char* macaddr) {
    struct ifaddrs *ifaddr, *ifa;
    int family, n;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }
    for (ifa = ifaddr, n = 0; ifa != nullptr; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == nullptr)
            continue;
        family = ifa->ifa_addr->sa_family;
        if(family == AF_INET) {
            char tmp[128];
            int err = getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), tmp, sizeof(tmp), nullptr, 0, NI_NUMERICHOST);
            if(err != 0) {
                printf("getnameinfo(%d) failed: %s\n", err, gai_strerror(err));
            }
            printf("Found hostIPAddress of: %s\n", tmp);
            if(strncasecmp(tmp, "127.0", 5) == 0) {
                printf("Skipping localhost address\n");
                continue;
            }
            strcpy(hostIPAddress, tmp);
            unsigned char MAC[6];
            getMACAddress(ifa->ifa_name, MAC);
            sprintf(macaddr, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
        }
    }
    freeifaddrs(ifaddr);
}

const string HealthStatus::statusPropertyNames[static_cast<unsigned int>(StatusProperties::N_STATUS_PROPERTIES)] = {
        string("ScannerID"),
        string("HostIPAddress"),
        string("MACAddress"),
        string("SystemType"),
        string("SystemOS"),
        string("SystemTime"),
        string("SystemTimeMS"),
        string("Uptime"),
        string("SystemUptime"),
        string("Procs"),
        string("LoadAverage"),
        string("RawEventCount"),
        string("PublishEventCount"),
        string("HeartbeatCount"),
        string("HeartbeatRSSI"),
        string("EventsWindow"),
        string("ActiveBeacons"),
        string("MemTotal"),
        string("MemFree"),
        string("MemAvailable"),
        string("SwapTotal"),
        string("SwapFree"),
};

void HealthStatus::broadcastInformation(unique_ptr<UDPSocket>& bcastSocket) {
    const string &bcastddr = statusInformation->getBcastAddress();
    int bcastPort = statusInformation->getBcastPort();
    ByteBuffer buffer;
    vector<StatusProperties> enums;
    enums.push_back(StatusProperties::HostIPAddress);
    enums.push_back(StatusProperties::MACAddress);
    enums.push_back(StatusProperties::ScannerID);
    enums.push_back(StatusProperties::LoadAverage);
    enums.push_back(StatusProperties::MemTotal);
    enums.push_back(StatusProperties::MemFree);
    enums.push_back(StatusProperties::SystemTime);
    enums.push_back(StatusProperties::SystemType);
    enums.push_back(StatusProperties::SystemOS);
    enums.push_back(StatusProperties::SystemUptime);
    enums.push_back(StatusProperties::Uptime);
    vector<string> names = HealthStatus::getStatusPropertyNames(enums);
    statusInformation->encodeLastStatus(buffer, names);
    vector<uint8_t>& data = buffer.getData();
    try {
        bcastSocket->sendTo(data.data(), data.size(), bcastddr, bcastPort);
    } catch (SocketException &e) {
        printf("Failed to broadcast status: %s\n", e.what());
    }
}

void HealthStatus::monitorStatus() {
    int statusInterval = statusInformation->getStatusInterval();
    const string& statusQueue = statusInformation->getStatusQueue();
    const string& scannerID = statusInformation->getScannerID();
    Properties statusProperties;
    const string& ScannerID = getStatusPropertyName(StatusProperties::ScannerID);
    const string& HostIPAddress = getStatusPropertyName(StatusProperties::HostIPAddress);
    const string& MACAddress = getStatusPropertyName(StatusProperties::MACAddress);
    const string& SystemType = getStatusPropertyName(StatusProperties::SystemType);
    const string& SystemOS = getStatusPropertyName(StatusProperties::SystemOS);
    const string& SystemTime = getStatusPropertyName(StatusProperties::SystemTime);
    const string& SystemTimeMS = getStatusPropertyName(StatusProperties::SystemTimeMS);
    const string& Uptime = getStatusPropertyName(StatusProperties::Uptime);
    const string& SystemUptime = getStatusPropertyName(StatusProperties::SystemUptime);
    const string& LoadAverage = getStatusPropertyName(StatusProperties::LoadAverage);
    const string& Procs = getStatusPropertyName(StatusProperties::Procs);
    const string& RawEventCount = getStatusPropertyName(StatusProperties::RawEventCount);
    const string& PublishEventCount = getStatusPropertyName(StatusProperties::PublishEventCount);
    const string& HeartbeatCount = getStatusPropertyName(StatusProperties::HeartbeatCount);
    const string& HeartbeatRSSI = getStatusPropertyName(StatusProperties::HeartbeatRSSI);
    const string& EventsWindow = getStatusPropertyName(StatusProperties::EventsWindow);
    const string& ActiveBeacons = getStatusPropertyName(StatusProperties::ActiveBeacons);
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemActive = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    // Used to calculate the scanner process uptime relative to its starting time
    struct sysinfo beginInfo;

    // Determine the scanner type
    string systemType = determineSystemType();
    printf("Determined SystemType as: %s\n", systemType.c_str());
    string systemOS = determineSystemOS();
    printf("Determined SystemOS as: %s\n", systemOS.c_str());

    if(sysinfo(&beginInfo)) {
        perror("Failed to read sysinfo");
    };
// Send an initial hello status msg with the host inet address
    char hostIPAddress[128];
    char macaddr[32];
    getHostInfo(hostIPAddress, macaddr);

    unique_ptr<UDPSocket> bcast;
    const string &bcastddr = statusInformation->getBcastAddress();
    if(bcastddr.size() > 0) {
        int bcastPort = statusInformation->getBcastPort();
        bcast.reset(new UDPSocket);
    }

    while(running) {
        statusProperties[ScannerID] = scannerID;
        statusProperties[HostIPAddress] = hostIPAddress;
        statusProperties[MACAddress] = macaddr;
        statusProperties[SystemType] = systemType;
        statusProperties[SystemOS] = systemOS;

        // Time
        system_clock::time_point now = system_clock::now();
        milliseconds ms = duration_cast< milliseconds >(now.time_since_epoch());
        time_t systime = system_clock::to_time_t(now);
        char* timestr = ctime(&systime);
        // Remove trailing '\n'
        size_t length = strlen(timestr);
        timestr[length-1] = 0;
        statusProperties[SystemTime] = timestr;
        statusProperties[SystemTimeMS] = to_string(ms.count());
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
            printf("EventsBucket[%ld]: %s\n", eventsBucket->size(), eventsBucketStr.data());
            statusProperties[ActiveBeacons] = to_string(eventsBucket->size());

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
            printf("Scanner %s\n", tmp);
            sprintf(tmp, "%.2f, %.2f, %.2f", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);
            // Calcualte system uptime
            uptimeDiff = info.uptime;
            days = uptimeDiff / (24*3600);
            hours = (uptimeDiff - days * 24*3600) / 3600;
            minute = (uptimeDiff - days * 24*3600 - hours*3600) / 60;
            seconds = uptimeDiff - days * 24*3600 - hours*3600 - minute*60;
            sprintf(tmp, "uptime: %ld, days:%ld, hrs:%ld, min:%ld, sec:%ld", uptimeDiff, days, hours, minute, seconds);
            statusProperties[SystemUptime] = tmp;
            printf("System %s\n", tmp);
            sprintf(tmp, "%.2f, %.2f, %.2f", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);

            printf("loadavg: %s\n", tmp);
            statusProperties[LoadAverage] = tmp;
            statusProperties[Procs] = to_string(info.procs);
            statusProperties[MemTotal] = to_string(info.totalram*info.mem_unit / mb);
            statusProperties[MemActive] = to_string((info.totalram - info.freeram)*info.mem_unit / mb);
            statusProperties[MemFree] = to_string(info.freeram*info.mem_unit / mb);
            printf("MemTotal: %s;  MemFree: %s\n", statusProperties[MemTotal].c_str(),  statusProperties[MemFree].c_str());
            statusProperties[SwapFree] = to_string(info.freeswap*info.mem_unit / mb);
            statusProperties[SwapTotal] = to_string(info.totalswap*info.mem_unit / mb);
        }
        fflush(stdout);

        // Publish the status
        statusInformation->setLastStatus(statusProperties);
        if(bcast) {
            broadcastInformation(bcast);
        }

        // Wait for statusInterval before next status message
        chrono::seconds sleepTime(statusInterval);
#ifdef USE_YIELD_LOOP
        // Valgrind is causing the sleep_for() call to not work, so use a simple yield spin loop.
        now = system_clock::now();
        ms = duration_cast< milliseconds >(now.time_since_epoch());
        printf("Begin yield for(%lld), now=%lld\n", sleepTime.count(), ms.count());
        system_clock::time_point wakeup = now + sleepTime;
        while(now < wakeup) {
            this_thread::yield();
            now = system_clock::now();
            this_thread::sleep_for(chrono::nanoseconds(1000000));
        }
        now = system_clock::now();
        ms = duration_cast< milliseconds >(now.time_since_epoch());
        printf("End yield for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());
#else
        this_thread::sleep_for(sleepTime);
#endif
    }
}

void HealthStatus::calculateStatus(StatusInformation& statusInformation) {
    const string& scannerID = statusInformation.getScannerID();
    Properties statusProperties;
    const string& ScannerID = getStatusPropertyName(StatusProperties::ScannerID);
    const string& HostIPAddress = getStatusPropertyName(StatusProperties::HostIPAddress);
    const string& MACAddress = getStatusPropertyName(StatusProperties::MACAddress);
    const string& SystemType = getStatusPropertyName(StatusProperties::SystemType);
    const string& SystemTime = getStatusPropertyName(StatusProperties::SystemTime);
    const string& SystemTimeMS = getStatusPropertyName(StatusProperties::SystemTimeMS);
    const string& Uptime = getStatusPropertyName(StatusProperties::Uptime);
    const string& SystemUptime = getStatusPropertyName(StatusProperties::SystemUptime);
    const string& LoadAverage = getStatusPropertyName(StatusProperties::LoadAverage);
    const string& Procs = getStatusPropertyName(StatusProperties::Procs);
    const string& RawEventCount = getStatusPropertyName(StatusProperties::RawEventCount);
    const string& PublishEventCount = getStatusPropertyName(StatusProperties::PublishEventCount);
    const string& HeartbeatCount = getStatusPropertyName(StatusProperties::HeartbeatCount);
    const string& HeartbeatRSSI = getStatusPropertyName(StatusProperties::HeartbeatRSSI);
    const string& EventsWindow = getStatusPropertyName(StatusProperties::EventsWindow);
    const string& ActiveBeacons = getStatusPropertyName(StatusProperties::ActiveBeacons);
    const string& MemTotal = getStatusPropertyName(StatusProperties::MemTotal);
    const string& MemFree = getStatusPropertyName(StatusProperties::MemFree);
    const string& MemActive = getStatusPropertyName(StatusProperties::MemActive);
    const string& SwapTotal = getStatusPropertyName(StatusProperties::SwapTotal);
    const string& SwapFree = getStatusPropertyName(StatusProperties::SwapFree);
    struct timeval  tv;
    struct tm *tm;

    // Determine the scanner type
    string systemType = determineSystemType();
    printf("Determined SystemType as: %s\n", systemType.c_str());

    char hostIPAddress[128];
    char macaddr[32];
    getHostInfo(hostIPAddress, macaddr);

    statusProperties[ScannerID] = scannerID;
    statusProperties[HostIPAddress] = hostIPAddress;
    statusProperties[MACAddress] = macaddr;
    statusProperties[SystemType] = systemType;

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
    struct sysinfo& beginInfo = info;
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
        printf("Scanner %s\n", tmp);
        sprintf(tmp, "%.2f, %.2f, %.2f", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);
        // Calcualte system uptime
        uptimeDiff = info.uptime;
        days = uptimeDiff / (24*3600);
        hours = (uptimeDiff - days * 24*3600) / 3600;
        minute = (uptimeDiff - days * 24*3600 - hours*3600) / 60;
        seconds = uptimeDiff - days * 24*3600 - hours*3600 - minute*60;
        sprintf(tmp, "uptime: %ld, days:%ld, hrs:%ld, min:%ld, sec:%ld", uptimeDiff, days, hours, minute, seconds);
        statusProperties[SystemUptime] = tmp;
        printf("System %s\n", tmp);
        sprintf(tmp, "%.2f, %.2f, %.2f", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);

        printf("loadavg: %s\n", tmp);
        statusProperties[LoadAverage] = tmp;
        statusProperties[Procs] = to_string(info.procs);
        statusProperties[MemTotal] = to_string(info.totalram*info.mem_unit / mb);
        statusProperties[MemActive] = to_string((info.totalram - info.freeram)*info.mem_unit / mb);
        statusProperties[MemFree] = to_string(info.freeram*info.mem_unit / mb);
        printf("MemTotal: %s;  MemFree: %s\n", statusProperties[MemTotal].c_str(),  statusProperties[MemFree].c_str());
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
    printf("HealthStatus::start, runnnig with statusInterval: %d\n", statusInformation->getStatusInterval());
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

string HealthStatus::determineSystemType() {
    // First check the env for SYSTEM_TYPE
    const char *systemType = getenv("SYSTEM_TYPE");
    if(systemType != nullptr) {
        return systemType;
    }

    systemType = nullptr;
    // Check cpuinfo for Revision
    FILE *cpuinfo = fopen ("/proc/cpuinfo", "r");
    if (cpuinfo == NULL) {
        perror ("Failed to open /proc/cpuinfo");
    }

    char buffer[64];
    while(fgets (buffer, sizeof(buffer), cpuinfo)){
        // Replace trailing newline char with nil
        size_t length = strlen(buffer);
        buffer[length-1] = '\0';
        length --;
        if(strncasecmp(buffer, "Revision", 8) == 0) {
            // find start of rev string
            string rev(buffer);
            size_t found = rev.find_last_of(' ');
            if(found != string::npos) {
                try {
                    // http://www.raspberrypi-spy.co.uk/2012/09/checking-your-raspberry-pi-board-version/
                    int revision = stoi(rev.substr(found), nullptr, 16);
                    switch (revision) {
                        case 0x007:
                        case 0x008:
                        case 0x009:
                            systemType = "PiA";
                        break;
                        case 0x002:
                        case 0x004:
                        case 0x005:
                        case 0x006:
                        case 0x00d:
                        case 0x00e:
                        case 0x00f:
                            systemType = "PiB";
                            break;
                        case 0x010:
                            systemType = "PiB+";
                            break;
                        case 0x012:
                            systemType = "PiA+";
                            break;
                        case 0xa01041:
                        case 0xa21041:
                            systemType = "Pi2B";
                            break;
                        default:
                            break;
                    }
                } catch (const std::invalid_argument& e) {
                    fprintf(stderr, "Failed to parse revision: %s, ex=%d", rev.c_str(), e.what());
                }
            }
        }
    }
    fclose(cpuinfo);

    if(systemType == nullptr) {
        // TODO
        systemType = "IntelNUC";
    }

    return systemType;
}

string HealthStatus::determineSystemOS() {
    char tmp[128];
    string os;
    ifstream pidora("/etc/redhat-release");
    ifstream debian("/etc/debian_version");
    if(pidora.good()) {
        pidora.getline(tmp, 128);
        os = tmp;
    }
    else if(debian.good()) {
        debian.getline(tmp, 128);
        os = "Debian ";
        os += tmp;
    }
    return os;
}
