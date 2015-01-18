#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include "Beacon.h"

using namespace std;



static std::string trim(std::string str) {
    size_t endpos = str.find_last_not_of(" \t");
    if( string::npos != endpos ) {
        str = str.substr( 0, endpos+1 );
    }
    return str;
}

int main() {
    std::ifstream ifs ("/Users/starksm/Dev/IoT/BLE/RaspberryPiBeaconParser/src/test/resources/hcidump.raw", std::ifstream::in);
    regex exp(">\\s04.*((?:\\s[[:xdigit:]]{2}){6})((?:\\s[[:xdigit:]]{2}){4}) 1A FF 4C");
    string line;
    std::getline(ifs, line);
    while(ifs.good()) {
        if (regex_match(trim(line.c_str()), exp)) {
            string buffer(trim(line.c_str()));
            std::getline(ifs, line);
            buffer.append(trim(line.c_str()));
            std::getline(ifs, line);
            buffer.append(trim(line.c_str()));

            Beacon beacon = Beacon::parseHCIDump("testParseHCIDump", buffer);
            cout << "Created: " << beacon.toString() << endl;
        } else {
            cout << "No match: " << line << endl;
        }
        std::getline(ifs, line);
    }

    return 0;
}
