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
    lcd->displayText("Text to display:", 0, 0);
    cout << "Enter text to display: ";
    int count = 0;
    for (std::string line; std::getline(std::cin, line);) {
        count ++;
        int col = 0;
        int row = count % 3 + 1;
        if(count % 2)
            col = 2;
        lcd->clear();
        lcd->displayText(line.c_str(), col, row);
        cout << "Enter text to display: ";
    }
    lcd->clear();
}
