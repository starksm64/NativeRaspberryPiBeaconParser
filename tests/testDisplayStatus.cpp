#include <chrono>
#include <iostream>
#include <memory>
#include <string.h>
#include "../lcd/LcdDisplay.h"
#include "../src2/HealthStatus.h"

using namespace std;
using namespace std::chrono;

int main() {
    unique_ptr<LcdDisplay> lcd(LcdDisplay::getLcdDisplayInstance());
    lcd->init();
    StatusInformation status;
    beacon_info beacon;
    memset(&beacon, sizeof(beacon), 0);
    status.setScannerID("Room206");

    // Add some events
    sprintf(beacon.uuid, "UUID-%.10d", 66);
    beacon.minor = 66;
    beacon.major = 1;
    beacon.manufacturer = 0xdead;
    beacon.code = 0xbeef;
    beacon.count = 3;
    beacon.rssi = -68;
    milliseconds now = duration_cast<milliseconds >(high_resolution_clock::now().time_since_epoch());
    beacon.time = now.count();
    status.addEvent(beacon, false);

    beacon.isHeartbeat = true;
    beacon.minor = 206;
    beacon.time += 10;
    status.addEvent(beacon, true);

    beacon.isHeartbeat = false;
    beacon.minor = 56;
    beacon.time += 10;
    status.addEvent(beacon, false);

    beacon.isHeartbeat = true;
    beacon.minor = 206;
    beacon.time += 10;
    status.addEvent(beacon, true);

    HealthStatus healthStatus;
    healthStatus.calculateStatus(status);
    Properties statusProps = status.getLastStatus();
    char tmp[20];
    snprintf(tmp, 20, "%s:%.5d;%d", status.getScannerID().c_str(), status.getHeartbeatCount(), status.getHeartbeatRSSI());
    lcd->displayText(tmp, 0, 0);
    string uptime = statusProps["Uptime"];
    const char *uptimeStr = uptime.c_str();
    printf("%s; length=%d\n", uptimeStr, uptime.size());
    int days=0, hrs=0, mins=0;
    int count = sscanf (uptimeStr, "uptime: %*d, days:%d, hrs:%d, min:%d", &days, &hrs, &mins);
    printf("matched:%d, UP D:%d H:%d M:%d\n", count, days, hrs, mins);
    snprintf(tmp, 20, "UP D:%d H:%d M:%d", days, hrs, mins);
    lcd->displayText(tmp, 0, 1);
    const char *load = statusProps["LoadAverage"].c_str();
    printf("Load: %s\n", load);
    lcd->displayText(load, 0, 2);
    snprintf(tmp, 20, "Events: %d; Msgs: %d", status.getRawEventCount(), status.getPublishEventCount());
    lcd->displayText(tmp, 0, 3);

    cout << "Enter any key to exit: ";
    std::string line; std::getline(std::cin, line);
    lcd->clear();
}
