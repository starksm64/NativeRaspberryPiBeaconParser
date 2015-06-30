//
// Created by starksm on 4/10/15.
//

#ifndef NATIVESCANNER_SCANNERVIEW_H
#define NATIVESCANNER_SCANNERVIEW_H

#include "../src/Beacon.h"
#include "AbstractBeaconMapper.h"
#include "StatusInformation.h"

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
    shared_ptr<AbstractBeaconMapper> beaconMapper;

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

    const shared_ptr<AbstractBeaconMapper> &getBeaconMapper() const {
        return beaconMapper;
    }

    void setBeaconMapper(const shared_ptr<AbstractBeaconMapper> &beaconMapper) {
        ScannerView::beaconMapper = beaconMapper;
    }

/**
     * Display the beacon event's information
     */
    virtual void displayBeacon(const Beacon& beacon) {

    }
    /**
     * Display the heartbeat beacon event's information
     */
    virtual void displayHeartbeat(const Beacon& beacon) {

    }
    /**
     * Display the scanner's status information
     */
    virtual void displayStatus(const StatusInformation& status) {

    }
};
#endif //NATIVESCANNER_SCANNERVIEW_H
