//
// Created by starksm on 3/28/15.
//

#ifndef _NATIVESCANNER_MESSAGEEXCHANGER_H_
#define _NATIVESCANNER_MESSAGEEXCHANGER_H_

#include <memory>
#include <mutex>
#include <queue>
#include <atomic>

using namespace std;

typedef struct {
    char uuid[36];
    int32_t code;
    int32_t manufacturer;
    int32_t major;
    int32_t minor;
    int32_t power;
    int32_t calibrated_power;
    int32_t rssi;
    int64_t time;
} event_info;

class MessageExchanger {
private:
    mutable mutex contentsMutex;
    queue<event_info*> events;
    atomic<int> takeCount;
    atomic<int> putCount;

public:
    MessageExchanger() : takeCount{0}, putCount{0}
    {}

    void putEvent(event_info *info);
    unique_ptr<event_info> takeEvent();
    int getPutCount() const;
    int getTakeCount() const;
    size_t size() const;
};

#endif //_NATIVESCANNER_MESSAGEEXCHANGER_H_
