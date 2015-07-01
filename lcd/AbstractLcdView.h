//
// Created by Scott Stark on 6/30/15.
//

#ifndef NATIVESCANNER_ABSTRACTLCDVIEW_H
#define NATIVESCANNER_ABSTRACTLCDVIEW_H

#include "AbstractLcdDisplay.h"
#include "ScannerView.h"

/**
 * A base implementation of the ScannerView display methods that uses the AbstractLcdDisplay methods
 */
class AbstractLcdView : public AbstractLcdDisplay, public ScannerView {
protected:
    AbstractLcdView() {}

public:

    /**
     * Singleton accessor
     */
    static AbstractLcdView *getLcdDisplayInstance(LcdDisplayType type);

    /**
    * Display the given beacon information
    */
    virtual void displayBeacon(const Beacon& beacon);
    /**
     * Display the given heartbeat beacon information
     */
    virtual void displayHeartbeat(const Beacon& beacon);
    /**
     * Display the given scanner status information
     */
    virtual void displayStatus(StatusInformation const &status);

    /**
     * Display a time string as HH:MM:SS.ss
     */
    void displayTime(int64_t timeInMS, int col, int row);
};


#endif //NATIVESCANNER_ABSTRACTLCDVIEW_H
