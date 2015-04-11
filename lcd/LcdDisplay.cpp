//
// Created by Scott Stark on 4/7/15.
//

#include "LcdDisplay.h"
#include <wiringPi.h>
#include <lcd.h>

void LcdDisplay::displayBeacon(const Beacon &beacon) {
    char tmp[80];
    sprintf(tmp, "Beacon(%d):", beacon.getMinor());
    displayText(tmp, 0, 0);
    sprintf(tmp, "rssi=%d", beacon.getRssi());
    displayText(tmp, 2, 1);
    sprintf(tmp, "time=%lld", beacon.getTime());
    displayText(tmp, 2, 2);
    sprintf(tmp, "Hello Scott");
    displayText(tmp, 2, 3);
}

void LcdDisplay::displayText(const string &text, int col, int row) {
    // Clear the prefix
    lcdPosition(lcdHandle, 0, row);
    for(int c = 0; c < col; c++) {
        lcdPutchar(lcdHandle, ' ');
    }

    // Output the text
    lcdPosition(lcdHandle, col, row);
    lcdPuts(lcdHandle, text.c_str());

    // Clear the rest of the line
    size_t spaces = nCols - text.size() - col;
    for(int c = 0; c < spaces; c++) {
        lcdPutchar(lcdHandle, ' ');
    }
}

int LcdDisplay::init(int rows, int cols) {
    nCols = cols;
    wiringPiSetup () ;

    lcdHandle = lcdInit (rows, cols, 4, 11,10, 4,5,6,7,0,0,0,0) ;
    if (lcdHandle < 0)
    {
        fprintf (stderr, "lcdInit failed\n") ;
        return -1 ;
    }
    return 0;
}

void LcdDisplay::clear() {
    lcdClear(lcdHandle);
}
