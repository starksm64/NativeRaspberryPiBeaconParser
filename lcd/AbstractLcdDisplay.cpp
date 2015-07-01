#include "AbstractLcdDisplay.h"
#ifdef HAVE_LCD_DISPLAY
#include "WiringPiLcdDisplay.h"
#include "MiniLcdPCD8544.h"
#endif

/**
 * Singleton accessor
 */
static AbstractLcdDisplay *AbstractLcdDisplay::getLcdDisplayInstance(LcdDisplayType type) {
    AbstractLcdDisplay *display = nullptr;
#ifdef HAVE_LCD_DISPLAY
    switch(type) {
        case HD44780U:
            display = new WiringPiLcdDisplay();
            break;
        case PCD8544:
            display = new MiniLcdPCD8544();
            break;
    }
#endif
    return display;
}
