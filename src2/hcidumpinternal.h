#ifndef hcidumpinternal_H
#define hcidumpinternal_H

#include "stdbool.h"

typedef struct beacon_info {
    char uuid[20];
    int code;
    int manufacturer;
    int major;
    int minor;
    int power;
    int calibrated_power;
    int rssi;
    long time;
} beacon_info;

// The callback function invoked for each beacon event seen by hcidumpinternal
// const char * uuid, int code, int manufacturer, int major, int minor, int power, int rssi, long time
typedef bool (*beacon_event)(const beacon_info *);

// The function hcidumpinternal exports
int scan_frames(int dev, beacon_event callback);
#endif
