#include "MiniLcdPCD8544.h"
#include <wiringPi.h>
#include "PCD8544.h"

// pin setup for lcd schield
static uint8_t _din = 1;
static uint8_t _sclk = 0;
static uint8_t _dc = 2;
static uint8_t _rst = 4;
static uint8_t _cs = 3;
// lcd contrast, may be need modify.  normal: 30- 90 ,default is:45
static int contrast = 70;

/**
 * The MiniLcdPCD8544 singleton
 */
static MiniLcdPCD8544 *theDisplay;

MiniLcdPCD8544 *MiniLcdPCD8544::getLcdDisplayInstance() {
    if(theDisplay == nullptr)
        theDisplay = new MiniLcdPCD8544();
    return theDisplay;
}

/**
 * init lcd, rows/cols are ignored as the display is fixed at 84x48
 */
int MiniLcdPCD8544::init(int rows, int cols) {
    // check wiringPi setup^M
    if (wiringPiSetup() == -1) {
        printf("wiringPi-Error\n");
        return -1;
    }
    // init and clear lcd
    LCDInit(_sclk, _din, _dc, _cs, _rst, contrast);
    LCDclear();
    // show raspberrypi logo for 2 seconds
    LCDshowLogo();
    delay(2000);
    return 0;
}

void MiniLcdPCD8544::clear() {
    LCDclear();
    LCDdisplay();
}

void MiniLcdPCD8544::displayText(const string &text, int col, int row) {
    uint8_t x = col * 6;
    uint8_t y = row * 8;
    char *todraw = (char *) text.c_str();
    LCDdrawstring(x, y, todraw);
    LCDdisplay();
    //printf("MiniLcdPCD8544::displayText(%s, %d[%d], %d[%d])\n", todraw, col, x, row, y);
}
