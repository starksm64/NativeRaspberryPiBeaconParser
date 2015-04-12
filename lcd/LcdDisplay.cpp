//
// Created by Scott Stark on 4/7/15.
//

#include "LcdDisplay.h"
#include <wiringPi.h>
#include <lcd.h>
#include <string.h>

/* The pi's GPIO pin 0, header pin 11, wiringPi pin 0
 * https://projects.drogon.net/raspberry-pi/wiringpi/pins/
 */
#define	BUTTON_PIN	0

//
#define	DEBOUNCE_TIME	100
static volatile int debounceTime = 0;

/**
 * The LcdDisplay singleton used invoke in the buttonPressed interrupt callback.
 */
static LcdDisplay *theDisplay;

static void buttonPressed() {
    if (millis () < debounceTime)
    {
        debounceTime = millis () + DEBOUNCE_TIME ;
        return;
    }

    int pinState = digitalRead(BUTTON_PIN);
    if(!pinState) {
        bool displayBeaconsMode = theDisplay->toggleDisplayState();
        printf("buttonPressed, displayBeaconsMode=%d\n", displayBeaconsMode);
    }
    debounceTime = millis () + DEBOUNCE_TIME ;
}

LcdDisplay *LcdDisplay::getLcdDisplayInstance() {
    if(theDisplay == nullptr)
        theDisplay = new LcdDisplay();
    return theDisplay;
}


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

void LcdDisplay::displayStatus(const StatusInformation& status){
    char tmp[20];
    snprintf(tmp, 20, "%s: %.7d;%d", status.getScannerID().c_str(), status.getHeartbeatCount(), status.getHeartbeatRSSI());
    displayText(tmp, 0, 0);
    Properties statusProps = status.getLastStatus();
    string uptime = statusProps["Uptime"];
    int days=0, hrs=0, mins=0;
    int count = sscanf (uptime.c_str(), "uptime: %*d, days:%d, hrs:%d, min:%d", &days, &hrs, &mins);
    snprintf(tmp, 20, "UP D:%d H:%d M:%d", days, hrs, mins);
    displayText(tmp, 0, 1);
    const char *load = statusProps["LoadAverage"].c_str();
    displayText(load, 0, 2);
    snprintf(tmp, 20, "S: %.7d; M: %.7d", status.getRawEventCount(), status.getPublishEventCount());
    displayText(tmp, 0, 3);
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
    // Set the button pin to input mode
    pinMode(BUTTON_PIN, INPUT);
    // Set the pull up resistor to up to fix it at 3.3V so it reads high until button is pressed
    pullUpDnControl(BUTTON_PIN, PUD_UP);

    // Register the interrupt callback function
    if (wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &buttonPressed) < 0) {
        fprintf (stderr, "Unable to setup button press callback: %s\n", strerror (errno)) ;
        return 1 ;
    }

    return 0;
}

void LcdDisplay::clear() {
    lcdClear(lcdHandle);
}
