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

    long ms = htobl(tv.tv_sec)*1000 + htobl(tv.tv_usec)/1000;
    int64_t ms64 = htobl(tv.tv_sec)*1000 + htobl(tv.tv_usec)/1000;
    printf("Sizeof(long) = %d, ms=%ld, ms64=%ld\n", sizeof(ms), ms, ms64);
}