//
// Created by Scott Stark on 3/18/15.
//

#ifndef _NATIVESCANNER_TESTTHREADING_H_
#define _NATIVESCANNER_TESTTHREADING_H_

#include <mutex>
#include <vector>

class backgroundTask {

private:
    mutable std::mutex *contentsMutex;
    std::vector<int> contents;
    void do_something() const;

public:
    backgroundTask() {
        contentsMutex = new std::mutex;
        printf("backgroundTask(%x)\n", this);
    }
    ~backgroundTask();

    void operator()() const
    {
        do_something();
    }

    int methodWithArgs(int x) {
        return x*x;
    }

    void add(int value);
    int get(int index) const;
    int take();
    int size() const;
};


#endif //_NATIVESCANNER_TESTTHREADING_H_
