#include <map>
#include <unistd.h>
#include <string.h>
#include "../src2/EventsWindow.h"

using namespace std;


void toString(vector<char> &output, const unique_ptr<EventsBucket> &bucket) {
    char timestr[256];
    char tmp[256];
    char *ptr = tmp;
    struct timeval tv;
    int64_t bucketStart = bucket->getBucketStart();
    int64_t bucketEnd = bucket->getBucketEnd();

    tv.tv_sec = bucketStart / 1000;
    tv.tv_usec = 0;
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    strftime(timestr, 128, "%r", &tm);
    // Report the stats for this time window and then reset
    int64_t width = bucketEnd - bucketStart;
    int count = snprintf(tmp, sizeof(tmp), "+++ Beacon counts for window(%lld): %s\n", width, timestr);
    output.insert(output.end(), ptr, ptr + count);
    map<int32_t, beacon_info>::const_iterator iter = bucket->begin();
    while (iter != bucket->end()) {
        output.push_back('\t');
        count = snprintf(tmp, sizeof(tmp), "+%2d(%s): %2d; ", iter->second.minor, iter->second.uuid,
                         iter->second.count);
        output.insert(output.end(), ptr, ptr + count);
        output.push_back('\n');
        iter++;
    }
    output.push_back('\n');
    output.push_back('\0');
}

void fullDump(const unique_ptr<EventsBucket> &bucket) {
    map<int32_t, beacon_info>::const_iterator iter = bucket->begin();
    while (iter != bucket->end()) {
        time_t infoTime = iter->second.time / 1000;
        printf("uuid=%s,isHeartbeat=%d,code=%d,manufacturer=%d,major=%d,minor=%d,power=%d,rssi=%d,count=%d,time=%lld(%s)\n",
               iter->second.uuid,
               iter->second.isHeartbeat,
               iter->second.code,
               iter->second.manufacturer,
               iter->second.major,
               iter->second.minor,
               iter->second.power,
               iter->second.rssi,
               iter->second.count,
               iter->second.time,
               ctime(&infoTime)
               );
        iter ++;
    }
}

int main() {
    EventsWindow window;

    window.reset(10);
    beacon_info info;
    memset(&info, 0, sizeof(info));
    info.rssi = 10;
    info.major = 1;
    info.manufacturer = 0xdead;
    info.code = 0xbeef;
    info.calibrated_power = -40;
    info.time = EventsWindow::currentMilliseconds();

    // Test printing an empty bucket
    unique_ptr<EventsBucket> empty = window.getCurrentBucket();
    vector<char> tmp;
    empty->toString(tmp);
    printf("bucket.toString(): %s\n", tmp.data());

    for (int n = 0; n < 60; n++) {
        int id = n % 5;
        int64_t nowMS = EventsWindow::currentMilliseconds();
        sprintf(info.uuid, "UUID-%.12d", id);
        info.rssi = 10;
        info.minor = id;
        info.time = nowMS;
        printf("n=%d, nowMS=%lld\n", n, nowMS);
        unique_ptr<EventsBucket> bucket = window.addEvent(info, false);
        if (bucket) {
            toString(tmp, bucket);
            printf("%s\n", tmp.data());
            fullDump(bucket);
            tmp.clear();
            bucket->toString(tmp);
            printf("bucket.toString(): %s\n", tmp.data());
        }
        sleep(1);
    }
    fullDump(window.getCurrentBucket());
}
