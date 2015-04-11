//
// Created by Scott Stark on 4/7/15.
//

#ifndef NATIVESCANNER_LCDDISPLAY_H
#define NATIVESCANNER_LCDDISPLAY_H


#include "../src/Beacon.h"
#include "../src/BeaconViewer.h"

/**
 * A class that configures the wiringPi lcd api to work with a 4x20 lcd display.
 */
class LcdDisplay : public BeaconViewer {
private:
    int nCols;
    int lcdHandle ;

public:
    int init(int rows=4, int cols=20);
    void clear();
    void displayText(const string& text, int col, int row);

    virtual void displayBeacon(const Beacon& beacon);
    virtual void displayHeartbeat(const Beacon& beacon);
};


#endif //NATIVESCANNER_LCDDISPLAY_H
