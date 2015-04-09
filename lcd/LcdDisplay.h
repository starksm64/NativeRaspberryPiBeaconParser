//
// Created by Scott Stark on 4/7/15.
//

#ifndef NATIVESCANNER_LCDDISPLAY_H
#define NATIVESCANNER_LCDDISPLAY_H


#include "../src/Beacon.h"

/**
 * A class that configures the wiringPi lcd api to work with a 4x20 lcd display.
 */
class LcdDisplay {
private:
    int nCols;
    int lcdHandle ;

public:
    int init(int rows=4, int cols=20);
    void clear();
    void displayBeacon(const Beacon& beacon);
    void displayText(const string& text, int col, int row);
};


#endif //NATIVESCANNER_LCDDISPLAY_H
