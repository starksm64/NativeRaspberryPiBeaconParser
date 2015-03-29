//
// Created by starksm on 3/28/15.
//

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <thread>
#include <string.h>
#include <sys/time.h>
#include "MessageExchanger.h"

using namespace std;

const int N = 1000000;
static atomic_bool producerDone;

static void producer(MessageExchanger& queue) {
    printf("Producer, generating %d events\n", N);
    struct timeval tv;
    for(int n = 0; n < N; n ++) {
        event_info *info = new event_info;
        memset(info, 0, sizeof(event_info));
        sprintf(info->uuid, "UUID: %.16d", n);
        info->minor = n;
        gettimeofday(&tv, nullptr);
        int64_t ms64 = tv.tv_sec;
        ms64 *= 1000;
        ms64 += tv.tv_usec / 1000;
        info->time = ms64;
        queue.putEvent(info);
    }
    cout << "\tproducer:done" << endl;
    cout.flush();
    producerDone = true;
}

static void consumer(MessageExchanger& queue) {
    printf("Consumer, handling %d events\n", N);
    bool notDone = true;
    while(notDone) {
        unique_ptr<event_info> info = queue.takeEvent();
        if(!info && queue.size() == 0) {
            this_thread::sleep_for(std::chrono::milliseconds(1));
            // Check the done flag
            notDone = !producerDone || queue.size() > 0;
            printf("notDone check(%u), size=%ld\n", notDone, queue.size());
        }
    }
    cout << "\tconsumer.done" << endl;
    cout.flush();
}

int main() {

    time_t start = time(nullptr);
    MessageExchanger exchanger;
    cout << "+++ Start a producer and consumer...\n";
    producerDone = false;
    std::thread producerThread(producer, std::ref(exchanger));
    thread consumerThread(consumer, std::ref(exchanger));
    producerThread.join();
    consumerThread.join();
    time_t end = time(nullptr);
    printf("producer and consumer done, elapsed=%ld, puts=%d, takes=%d, size=%ld\n", (end - start),
            exchanger.getPutCount(), exchanger.getTakeCount(), exchanger.size());
    cout.flush();

    return 0;
}
