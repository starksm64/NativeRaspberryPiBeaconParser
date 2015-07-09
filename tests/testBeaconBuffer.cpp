#include <cstddef>
#include <stdio.h>
#include "../src2/hcidumpinternal.h"

/**
 * Test the offset of the beacon_info struct fields for use with a direct ByteBuffer via JNI
 */
int main(int argc, char * * argv) {
    printf("sizeof(beacon_info) = %ld\n", sizeof(beacon_info));
    printf("offsetof(beacon_info.isHeartbeat) = %d\n", offsetof(beacon_info, isHeartbeat));
    printf("offsetof(beacon_info.count) = %d\n", offsetof(beacon_info, count));
    printf("offsetof(beacon_info.code) = %d\n", offsetof(beacon_info, code));
    printf("offsetof(beacon_info.manufacturer) = %d\n", offsetof(beacon_info, manufacturer));
    printf("offsetof(beacon_info.major) = %d\n", offsetof(beacon_info, major));
    printf("offsetof(beacon_info.minor) = %d\n", offsetof(beacon_info, minor));
    printf("offsetof(beacon_info.power) = %d\n", offsetof(beacon_info, power));
    printf("offsetof(beacon_info.calibrated_power) = %d\n", offsetof(beacon_info, calibrated_power));
    printf("offsetof(beacon_info.rssi) = %d\n", offsetof(beacon_info, rssi));
    printf("offsetof(beacon_info.time) = %d\n", offsetof(beacon_info, time));
}
