//
// Created by starksm on 4/10/15.
//

#ifndef NATIVESCANNER_BEACONVIEWER_H
#define NATIVESCANNER_BEACONVIEWER_H

#include "Beacon.h"

class BeaconViewer {
public:
    virtual void displayBeacon(const Beacon& beacon) = 0;
};
#endif //NATIVESCANNER_BEACONVIEWER_H
