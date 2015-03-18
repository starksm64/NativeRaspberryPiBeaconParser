//
// Created by Scott Stark on 3/18/15.
//

#include <iostream>
#include <future>
#include <thread>
#include "backgroundTask.h"

using namespace std;

static void hello() {
    cout << "\tHello, I'm in a thread" << endl;
}

static void producer(backgroundTask& queue) {
    cout << "\tproducer:generating 1000 ints..." << endl;
    for(int n = 1; n < 100; n ++) {
        queue.add(n*n);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    cout << "\tproducer:done" << endl;
    cout.flush();
}
static void consumer(backgroundTask& queue) {
    cout << "\tconsumer:reading 1000 ints..." << endl;
    for(int n = 0; n < 99; n ++) {
        while(queue.size() <= n) {
            // Sleep for 10 ms
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        int square = queue.get(n);
        printf("%d^2 = %d\n", n+1, square);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cout << "\tconsumer.done" << endl;
    cout.flush();
}

int main() {
    cout << "+++ Start a thread...\n";
    thread t(hello);
    t.join();
    cout << "Joined the thread\n";

    // Use a class with () operator
    backgroundTask task;
    cout << "+++ Start a background class operator()...\n";
    // This fails with a delete error without the std::ref(...) wrapper
    thread t2(std::ref(task));
    t2.join();
    cout << "Joined the background class operator()\n";
    cout.flush();

    // A lambda expression
    cout << "+++ Start a lambda expression...\n";
    thread t4([]{
        cout << "\tHello from within the lambda\n";
    });
    t4.join();
    cout << "Joined the lambda expression\n";
    cout.flush();

    cout << "+++ Start async background.methodWithArgs()...\n";
    auto methodWithArgs = std::async(&backgroundTask::methodWithArgs, &task, 12);
    int result = methodWithArgs.get();
    cout << "Joined the future methodWithArgs: " << result << endl;

    backgroundTask queue;
    cout << "+++ Start a producer and consumer...\n";
    std::thread t5(producer, std::ref(queue));
    thread t6(consumer, std::ref(queue));
    t5.join();
    t6.join();
    cout << "producer and consumer done" << endl;
    cout.flush();
}
