#include <iostream>
#include <string>
#include "../lcd/LcdDisplay.h"

using namespace std;

int main() {
    LcdDisplay lcd;
    lcd.init();
    lcd.displayText("Text to display:", 0, 0);
    cout << "Enter text to display: ";
    for (std::string line; std::getline(std::cin, line);) {
        lcd.displayText(line.c_str(), 0, 1);
        cout << "Enter text to display: ";
    }
}
