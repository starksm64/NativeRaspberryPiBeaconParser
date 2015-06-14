//
// Created by starksm on 4/13/15.
//

#ifndef NATIVESCANNER_MOCKSCANNERVIEW_H
#define NATIVESCANNER_MOCKSCANNERVIEW_H

#include "ScannerView.h"

class MockScannerView : public ScannerView {
private:
    int closestID;
    string closestName;

public:

    virtual void displayBeacon(const Beacon &beacon) {
        int minorID = beacon.getMinor();
        if(minorID != closestID) {
            closestID = minorID;
            if(getBeaconMapper()) {
                closestName = getBeaconMapper()->lookupUser(minorID);
            }
            printf("+++ New closest beacon: (%d,%s)\n", closestID, closestName.c_str());
        }
    }
    virtual void displayHeartbeat(const Beacon &beacon) {
    }
    virtual void displayStatus(const StatusInformation &status) {
    }
};

#endif //NATIVESCANNER_MOCKSCANNERVIEW_H
