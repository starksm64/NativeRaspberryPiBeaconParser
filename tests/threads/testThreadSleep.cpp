
#include <thread>
#include <chrono>
#include <ostream>
#include <iostream>

using namespace std;
using namespace std::chrono;

static void hello() {
    cout << "\tHello, I'm in a thread" << endl;
    system_clock::time_point now = system_clock::now();
    milliseconds ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("Begin sleep_for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());
    chrono::seconds ten(10);
    this_thread::sleep_for(ten);
    now = system_clock::now();
    ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("End sleep_for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());

    now = system_clock::now();
    ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("Begin sleep_for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());
    this_thread::sleep_for(chrono::seconds(10));
    now = system_clock::now();
    ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("End sleep_for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());

    now = system_clock::now();
    ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("Begin yield for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());
    chrono::seconds sleepTime(10);
    now = system_clock::now();
    system_clock::time_point wakeup = now + sleepTime;
    while(now < wakeup) {
        this_thread::yield();
        now = system_clock::now();
        this_thread::sleep_for(chrono::nanoseconds(1000000));
    }
    now = system_clock::now();
    ms = duration_cast< milliseconds >(now.time_since_epoch());
    printf("End yield for(%lld), now=%lld\n", chrono::seconds(10).count(), ms.count());

}


int main() {
    cout << "+++ Start a thread...\n";
    thread t(hello);
    t.join();
    cout << "Joined the thread\n";
}
