#include <stdio.h>
#include <sys/sysinfo.h>
#include "../src2/HealthStatus.h"

int main(int argc, char **argv) {
    int secs, days, hrs, mins;
    const char *uptime = "uptime: 216095, days:2, hrs:12, min:1";
    int matched = sscanf (uptime, "uptime: %d, days:%d, hrs:%d, min:%d", &secs, &days, &hrs, &mins);
    printf("matched:%d, UP D:%d H:%d M:%d\n", matched, days, hrs, mins);

    Properties statusProps;
    struct sysinfo info;
    char tmp[128];
    sysinfo(&info);
    days = info.uptime / (24*3600);
    hrs = (info.uptime - days * 24*3600) / 3600;
    mins = (info.uptime - days * 24*3600 - hrs*3600) / 60;
    sprintf(tmp, "uptime: %ld, days:%d, hrs:%d, min:%d", info.uptime, days, hrs, mins);
    statusProps["Uptime"] = tmp;
    string uptimeProp = statusProps["Uptime"];
    const char *uptimeStr = uptimeProp.c_str();
    printf("uptimeStr=%s\n", uptimeStr);
    int count = sscanf(uptimeStr, "uptime: %*d, days:%d, hrs:%d, min:%d", &days, &hrs, &mins);
    printf("matched:%d, UP D:%d H:%d M:%d\n", count, days, hrs, mins);
    snprintf(tmp, 20, "UP D:%d H:%d M:%d", days, hrs, mins);
    printf("UP D:%d H:%d M:%d\n", days, hrs, mins);
}