//
// Created by starksm on 4/10/15.
//

#ifndef NATIVESCANNER_SCANNERVIEW_H
#define NATIVESCANNER_SCANNERVIEW_H

#include "../src/Beacon.h"
#include "../src2/StatusInformation.h"

/**
 * A callback interface for displaying information from the scanner.
 *
 * The current implementation is based on an LCD display.
 *
 */
class ScannerView {
private:
    /** Should the scanner display beacon(true) or status information(false) */
    bool displayBeaconsMode;

public:
    ScannerView() : displayBeaconsMode(true) {}

    bool isDisplayBeaconsMode() const {
        return displayBeaconsMode;
    }
    void setDisplayBeaconsMode(bool displayBeaconsMode) {
        ScannerView::displayBeaconsMode = displayBeaconsMode;
    }
    bool toggleDisplayState() {
        displayBeaconsMode = !displayBeaconsMode;
        return displayBeaconsMode;
    }

    /**
     * Display the beacon event's information
     */
    virtual void displayBeacon(const Beacon& beacon) = 0;
    /**
     * Display the heartbeat beacon event's information
     */
    virtual void displayHeartbeat(const Beacon& beacon) = 0;
    /**
     * Display the scanner's status information
     */
    virtual void displayStatus(const StatusInformation& status) = 0;
};
#endif //NATIVESCANNER_SCANNERVIEW_H
