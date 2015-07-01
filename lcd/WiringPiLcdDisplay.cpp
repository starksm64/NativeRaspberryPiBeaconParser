#include "WiringPiLcdDisplay.h"
#include <wiringPi.h>
#include <lcd.h>
#include <string.h>

/* The pi's GPIO pin 0, header pin 11, wiringPi pin 0
 * https://projects.drogon.net/raspberry-pi/wiringpi/pins/
 */
#define	BUTTON_PIN	0

// Gap to wait for a button press callbacks to indicate state changes
#define	DEBOUNCE_TIME	1000
static volatile int debounceTime = 0;

/**
 * The WiringPiLcdDisplay singleton used invoke in the buttonPressed interrupt callback.
 */
static WiringPiLcdDisplay *theDisplay;

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

WiringPiLcdDisplay *WiringPiLcdDisplay::getLcdDisplayInstance() {
    if(theDisplay == nullptr)
        theDisplay = new WiringPiLcdDisplay();
    return theDisplay;
}


void WiringPiLcdDisplay::displayText(const string &text, int col, int row) {
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

int WiringPiLcdDisplay::init(int rows, int cols) {
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

void WiringPiLcdDisplay::clear() {
    lcdClear(lcdHandle);
}
