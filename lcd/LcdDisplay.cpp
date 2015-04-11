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
    displayTime(beacon.getTime(), 2, 2);
    sprintf(tmp, "Hello Scott");
    displayText(tmp, 2, 3);
}

void LcdDisplay::displayHeartbeat(const Beacon &beacon) {
    char tmp[80];
    sprintf(tmp, "Heartbeat(%d)*:", beacon.getMinor());
    displayText(tmp, 0, 0);
    sprintf(tmp, "rssi=%d", beacon.getRssi());
    displayText(tmp, 2, 1);
    displayTime(beacon.getTime(), 2, 2);
    sprintf(tmp, "No other in range");
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
    if(spaces > 0) {
        for (int c = 0; c < spaces; c++) {
            lcdPutchar(lcdHandle, ' ');
        }
    }
}

void LcdDisplay::displayTime(int64_t timeInMS, int col, int row) {
    char timestr[256];
    struct timeval  tv;
    struct tm      *tm;

    tv.tv_sec = timeInMS / 1000;
    tv.tv_usec = timeInMS * 1000 - tv.tv_sec * 1000000;
    tm = localtime(&tv.tv_sec);

    size_t length = strftime(timestr, 128, "%T", tm);
    snprintf(timestr+length, 128-length, ".%ld", tv.tv_usec/1000);
    displayText(timestr, col, row);
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
