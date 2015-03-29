
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
            vector<char> tmp;
            bucket->toTimeWindowString(tmp);
            tmp.resize(0);
            printf("TimeWindow: %s\n", tmp.data());
            bucket->toString(tmp);
            printf("%s\n", tmp.data());
        }
        sleep(1);
    }
}
