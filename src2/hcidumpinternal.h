#ifndef hcidumpinternal_H
#define hcidumpinternal_H

#include <stdbool.h>
#include <stdint.h>

typedef struct beacon_info {
    char uuid[36];
    bool isHeartbeat;
    int32_t code;
    int32_t manufacturer;
    int32_t major;
    int32_t minor;
    int32_t power;
    int32_t calibrated_power;
    int32_t rssi;
    int64_t time;
    int count;
} beacon_info;

// The callback function invoked for each beacon event seen by hcidumpinternal
// const char * uuid, int32_t code, int32_t manufacturer, int32_t major, int32_t minor, int32_t power, int32_t rssi, int64_t time
//typedef const beacon_info *beacon_info_stack_ptr;
typedef bool (*beacon_event)(beacon_info *);

#ifdef __cplusplus
extern "C" {
#endif
// The function hcidumpinternal exports
int32_t scan_frames(int32_t dev, beacon_event callback);
#ifdef __cplusplus
}
#endif

#endif
