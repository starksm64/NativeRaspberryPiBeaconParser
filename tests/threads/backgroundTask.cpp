//
// Created by Scott Stark on 3/18/15.
//

#include <stdio.h>
#include "backgroundTask.h"

backgroundTask::~backgroundTask() {
    delete contentsMutex;
    printf("~backgroundTask(%x)\n", this);
    contentsMutex = nullptr;
}
void backgroundTask::do_something() const {
    printf("Begin do_something()\n");

    printf("End do_something()\n");
}

void backgroundTask::add(int value) {
    std::lock_guard<std::mutex> guard(*contentsMutex);
    contents.push_back(value);
}
int backgroundTask::get(int index) const {
    std::lock_guard<std::mutex> guard(*contentsMutex);
    return contents.at(index);
}
int backgroundTask::take() {
    std::lock_guard<std::mutex> guard(*contentsMutex);
    int last = contents.back();
    contents.pop_back();
    return last;
}
int backgroundTask::size() const {
    return contents.size();
}
