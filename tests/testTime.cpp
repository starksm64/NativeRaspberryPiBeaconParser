#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "../../bluez-5.28/lib/bluetooth.h"
#include <chrono>

using namespace std::chrono;

static void buildTimeString(int64_t tsInMS, char *timestr) {
    struct timeval  tv;
    struct timezone tz;
    struct tm      *tm;

    tv.tv_sec = tsInMS / 1000;
    tv.tv_usec = tsInMS * 1000 - tv.tv_sec * 1000000;
    tm = localtime(&tv.tv_sec);

    size_t length = strftime(timestr, 128, "%F %T", tm);
    snprintf(timestr+length, 128-length, ".%ld", tv.tv_usec/1000);
}

int main(int argc, char **argv) {
    struct timeval  tv;
    struct timezone tz;
    struct tm      *tm;

    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);

    char timestr[256];
    size_t length = strftime(timestr, 128, "%F %T", tm);
    snprintf(timestr+length, 128-length, ".%ld", tv.tv_usec/1000);
    printf("Time string: %s\n", timestr);

    printf("Sizeof(time_t) = %ld\n", sizeof(time_t));
    printf("Sizeof(tv.tv_sec) = %ld\n", sizeof(tv.tv_sec));
    printf("Sizeof(tv.tv_usec) = %ld\n", sizeof(tv.tv_usec));
    printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);
    long ms = htobl(tv.tv_sec)*1000 + htobl(tv.tv_usec)/1000;
    int64_t ms64 = tv.tv_sec;
    ms64 *= 1000;
    ms64 += tv.tv_usec / 1000;
    int64_t ms64h = htobl(ms64);
    printf("Sizeof(long)=%d, sizeof(int64_t)=%ld, ms=%ld/%lu, ms64=%llu, ms64h=%llu\n", sizeof(ms), sizeof(int64_t), ms, ms, ms64, ms64h);
    // convert time in milliseconds to string form
    buildTimeString(ms64, timestr);
    printf("buildTimeString(%lld) = %s\n", ms64, timestr);

    milliseconds now = duration_cast<milliseconds >(high_resolution_clock::now().time_since_epoch());
    buildTimeString(now.count(), timestr);
    printf("buildTimeString from chrono(%lld) = %s\n", now.count(), timestr);

}
