#include <chrono>
#include <iostream>
#include <memory>
#include <string.h>
#include "../lcd/LcdDisplay.h"
#include "../src2/HealthStatus.h"

using namespace std;
using namespace std::chrono;

static inline void truncateName(string& name) {
    size_t length = name.length();
    if(length > 8) {
        name.resize(8);
        name.replace(7, 1, 1, '.');
    }
}

int main() {
    unique_ptr<LcdDisplay> lcd(LcdDisplay::getLcdDisplayInstance());
    lcd->init();
    StatusInformation status;
    beacon_info beacon;
    memset(&beacon, 0, sizeof(beacon));
    status.setScannerID("LCDScanner");

    // Add some events
    sprintf(beacon.uuid, "UUID-%.10d", 66);
    beacon.minor = 66;
    beacon.major = 1;
    beacon.manufacturer = 0xdead;
    beacon.code = 0xbeef;
    beacon.count = 3;
    beacon.rssi = -68;
    milliseconds now = duration_cast<milliseconds >(system_clock::now().time_since_epoch());
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
    printf("StatusProps dump:\n");
    for(Properties::const_iterator iter = statusProps.begin(); iter != statusProps.end(); iter ++) {
        printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
    }
    printf("+++ End dump\n\n");

    char tmp[21];

    snprintf(tmp, sizeof(tmp), "01234567890123456789");
    lcd->displayText(tmp, 0, 0);

    cout << "\nEnter any key to test local layout: ";
    std::string line;
    std::getline(std::cin, line);
    lcd->clear();

    string name(status.getScannerID());
    truncateName(name);
    snprintf(tmp, sizeof(tmp), "%s:%.7d;%d", name.c_str(), status.getHeartbeatCount(), status.getHeartbeatRSSI());
    lcd->displayText(tmp, 0, 0);
    string uptime = statusProps["Uptime"];
    const char *uptimeStr = uptime.c_str();
    printf("%s; length=%ld\n", uptimeStr, uptime.size());
    int days=0, hrs=0, mins=0;
    int count = sscanf (uptimeStr, "uptime: %*d, days:%d, hrs:%d, min:%d", &days, &hrs, &mins);
    printf("matched:%d, UP D:%d H:%d M:%d\n", count, days, hrs, mins);
    snprintf(tmp, sizeof(tmp), "UP D:%d H:%d M:%d", days, hrs, mins);
    lcd->displayText(tmp, 0, 1);
    const char *load = statusProps["LoadAverage"].c_str();
    printf("Load: %s\n", load);
    lcd->displayText(load, 0, 2);
    snprintf(tmp, sizeof(tmp), "Events: %d; Msgs: %d", status.getRawEventCount(), status.getPublishEventCount());
    lcd->displayText(tmp, 0, 3);

    cout << "\nEnter any key to call displayStatus: ";
    std::getline(std::cin, line);
    lcd->clear();
    lcd->displayStatus(status);

    cout << "\nEnter any key to call exit: ";
    std::getline(std::cin, line);
    lcd->clear();
}
