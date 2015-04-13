//
// Created by starksm on 4/13/15.
//

#ifndef NATIVESCANNER_MOCKSCANNERVIEW_H
#define NATIVESCANNER_MOCKSCANNERVIEW_H

#include "ScannerView.h"

class MockScannerView : public ScannerView {
public:

    virtual void displayBeacon(const Beacon &beacon) {
    }
    virtual void displayHeartbeat(const Beacon &beacon) {
    }
    virtual void displayStatus(const StatusInformation &status) {
    }
};

#endif //NATIVESCANNER_MOCKSCANNERVIEW_H
