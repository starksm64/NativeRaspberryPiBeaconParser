#ifndef NATIVESCANNER_ABSTRACTLCDDISPLAY_H
#define NATIVESCANNER_ABSTRACTLCDDISPLAY_H

#include <string>

using namespace std;


class AbstractLcdDisplay {
private:
    int nCols;
    int lcdHandle;

protected:
    AbstractLcdDisplay() { }

public:
    /**
     * Initialize the display to the given size
     */
    virtual int init(int rows=4, int cols=20) = 0;
    /**
     * Erase the display
     */
    virtual void clear() = 0;
    /**
     * Display a text string starting at the given position. This will wrap around if the string is greater
     * than the number of columns on the display.
     */
    virtual void displayText(const string& text, int col, int row) = 0;
    /**
     * Display a time string as HH:MM:SS.ss
     */
    virtual void displayTime(int64_t timeInMS, int col, int row) = 0;

};
#endif //NATIVESCANNER_ABSTRACTLCDDISPLAY_H
