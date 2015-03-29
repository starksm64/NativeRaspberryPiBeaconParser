
#include <map>
#include <unistd.h>
#include "../src2/EventsWindow.h"

using namespace std;


int main() {
    EventsWindow window;

    window.reset(5);
    beacon_info info;
    info.rssi = 10;
    info.time = EventsWindow::currentMilliseconds();
    for(int n = 0; n < 60; n ++) {
        info.rssi = 10;
        info.minor = n % 5;
        info.time = EventsWindow::currentMilliseconds();
        printf("n=%d\n", n);
        unique_ptr<EventsBucket> bucket = window.addEvent(info);
        if(bucket) {
            char timestr[256];
            struct timeval tv;
            tv.tv_sec = window.getBegin()/1000;
            tv.tv_usec = 0;
            struct tm tm;
            localtime_r(&tv.tv_sec, &tm);
            strftime(timestr, 128, "%r", &tm);
            // Report the stats for this time window and then reset
            printf("+++ Beacon counts for window(%ld,%d): %s\n", bucket->size(), window.getWindowSizeSeconds(), timestr);
            printf("\t");
            EventsBucket::iterator iter = bucket->begin();
            while(iter != bucket->end()) {
                printf("+%2d: %2d; ", iter->first, iter->second.count);
                iter ++;
            }
            printf("\n");
        }
        sleep(1);
    }
}
