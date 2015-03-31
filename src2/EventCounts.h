#ifndef NATIVESCANNER_EVENTCOUNTS_H
#define NATIVESCANNER_EVENTCOUNTS_H

class EventCounts {
private:


public:

    string &getStatusQueue() const {
        return statusQueue;
    }

    void setStatusQueue(string &statusQueue) {
        EventCounts::statusQueue = statusQueue;
    }

    int getStatusInterval() const {
        return statusInterval;
    }

    void setStatusInterval(int statusInterval) {
        EventCounts::statusInterval = statusInterval;
    }
};
#endif //NATIVESCANNER_EVENTCOUNTS_H
