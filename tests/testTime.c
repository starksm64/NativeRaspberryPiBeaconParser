#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "../../bluez-5.28/lib/bluetooth.h"

int main(int argc, char **argv) {
    struct timeval  tv;
    struct timezone tz;
    struct tm      *tm;

    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);

    char timestr[128];
    size_t length = strftime(timestr, 128, "%F %T", tm);
    snprintf(timestr+length, 128-length, ".%ld", tv.tv_usec/1000);
    printf("Time string: %s\n", timestr);

    printf("Sizeof(time_t) = %ld\n", sizeof(time_t));
    printf("Sizeof(tv.tv_sec) = %ld\n", sizeof(tv.tv_sec));
    printf("Sizeof(tv.tv_usec) = %ld\n", sizeof(tv.tv_usec));
    long ms = htobl(tv.tv_sec)*1000 + htobl(tv.tv_usec)/1000;
    int64_t ms64 = tv.tv_sec;
    ms64 *= 1000;
    ms64 += tv.tv_usec / 1000;
    int64_t ms64h = htobl(ms64);
    printf("Sizeof(long)=%d, sizeof(int64_t)=%ld, ms=%ld, ms64=%8lu, ms64h=%8lu\n", sizeof(ms), sizeof(int64_t), ms, ms64, ms64h);
}
