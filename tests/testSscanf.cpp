#include <stdio.h>

int main(int argc, char **argv) {
    int secs, days, hrs, mins;
    const char *uptime = "uptime: 216095, days:2, hrs:12, min:1";
    sscanf (uptime, "uptime: %d, days:%d, hrs:%d, min:%d", &secs, &days, &hrs, &mins);
    printf("UP D:%d H:%d M:%d", days, hrs, mins);

}