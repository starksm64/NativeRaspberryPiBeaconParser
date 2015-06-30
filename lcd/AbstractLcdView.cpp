
#include "AbstractLcdView.h"

void AbstractLcdView::displayBeacon(const Beacon &beacon) {
    char tmp[80];
    int minorID = beacon.getMinor();
    sprintf(tmp, "Beacon(%d):", minorID);
    displayText(tmp, 0, 0);
    sprintf(tmp, "rssi=%d", beacon.getRssi());
    displayText(tmp, 2, 1);
    displayTime(beacon.getTime(), 2, 2);

    if(getBeaconMapper()) {
        string user = getBeaconMapper()->lookupUser(minorID);
        sprintf(tmp, "Hello %s", user.c_str());
    } else {
        sprintf(tmp, "Hello Unknown");
    }
    displayText(tmp, 2, 3);
}

void AbstractLcdView::displayHeartbeat(const Beacon &beacon) {
    char tmp[80];
    sprintf(tmp, "Heartbeat(%d)*:", beacon.getMinor());
    displayText(tmp, 0, 0);
    sprintf(tmp, "rssi=%d", beacon.getRssi());
    displayText(tmp, 2, 1);
    displayTime(beacon.getTime(), 2, 2);
    sprintf(tmp, "No other in range");
    displayText(tmp, 2, 3);
}

static inline void truncateName(string& name) {
    size_t length = name.length();
    if(length > 8) {
        name.resize(8);
        name.replace(7, 1, 1, '.');
    }
}

void AbstractLcdView::displayStatus(const StatusInformation& status){
    char tmp[21];
    string name(status.getScannerID());
    truncateName(name);
    snprintf(tmp, sizeof(tmp), "%s:%.7d;%d", name.c_str(), status.getHeartbeatCount(), status.getHeartbeatRSSI());
    displayText(tmp, 0, 0);
    shared_ptr<Properties> statusProps = status.getLastStatus();
#if 0
    printf("StatusProps dump:\n");
    for(Properties::const_iterator iter = statusProps->begin(); iter != statusProps->end(); iter ++) {
        printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
    }
    printf("+++ End dump\n\n");
#endif

    string uptime = (*statusProps)["Uptime"];
    int days=0, hrs=0, mins=0, secs=0;
    int count = sscanf (uptime.c_str(), "uptime: %*d, days:%d, hrs:%d, min:%d, sec:%d", &days, &hrs, &mins, &secs);
    snprintf(tmp, sizeof(tmp), "UP D:%d H:%d M:%d S:%d", days, hrs, mins, secs);
    displayText(tmp, 0, 1);
    const char *load = (*statusProps)["LoadAverage"].c_str();
    displayText(load, 0, 2);
    snprintf(tmp, sizeof(tmp), "S:%.8d;M:%.7d", status.getRawEventCount(), status.getPublishEventCount());
    displayText(tmp, 0, 3);
}
