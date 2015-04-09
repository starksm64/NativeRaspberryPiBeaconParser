//
// Created by Scott Stark on 4/7/15.
//

#ifndef NATIVESCANNER_LCDDISPLAY_H
#define NATIVESCANNER_LCDDISPLAY_H


#include "../src/Beacon.h"

class LcdDisplay {
private:
    int lcdHandle ;

public:
    int init(int rows, int cols);
    void displayBeacon(const Beacon& beacon);
    void displayText(const string& text, int row, int col);
};


#endif //NATIVESCANNER_LCDDISPLAY_H
