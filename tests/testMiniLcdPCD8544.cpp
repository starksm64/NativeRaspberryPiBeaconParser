#include <iostream>
#include <string>
#include "../lcd/MiniLcdPCD8544.h"

using namespace std;

/**
 * Test the WiringPiLcdDisplay implementation
 */
int main() {
    unique_ptr<MiniLcdPCD8544> lcd(MiniLcdPCD8544::getLcdDisplayInstance());
    lcd->init();
    // Fill display 6 rows of 14 columns
    char text[16];
    for(int row = 0; row < 6; row ++) {
        sprintf(text, "%d:0123456789AB", row);
        lcd->displayText(text, 0, row);
    }
    // ask for test to display
    cout << "Enter text to display: ";
    int count = 0;
    for (std::string line; std::getline(std::cin, line);) {
        count ++;
        int col = 0;
        int row = count % 6 + 1;
        lcd->clear();
        lcd->displayText(line.c_str(), col, row);
        cout << "Displayed at row: " << row << "col: " << col << endl;
        cout << "Enter text to display: ";
    }
    lcd->clear();
}
