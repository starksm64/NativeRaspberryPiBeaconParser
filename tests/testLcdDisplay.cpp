#include <iostream>
#include <string>
#include "../lcd/LcdDisplay.h"

using namespace std;

int main() {
    LcdDisplay lcd;
    lcd.init(4, 24);
    lcd.displayText("", 0, 0);
    cout << "Enter text to display:";
}