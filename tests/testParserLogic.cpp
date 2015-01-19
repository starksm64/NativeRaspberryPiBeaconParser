#include <iostream>
#include <string>
#include <sstream>
#include "ParseCommand.h"
#include "ParserLogic.h"

using namespace std;

static std::string trim(std::string str) {
    size_t endpos = str.find_last_not_of(" \t");
    if( string::npos != endpos ) {
        str = str.substr( 0, endpos+1 );
    }
    return str;
}


int main() {
    const char *lines[] = {
     "> 04 3E 2A 02 01 03 01 92 EF 0D 06 40 30 1E 02 01 06 1A FF 4C ",
    "00 02 15 DA F2 46 CE 83 63 11 E4 B1 16 12 3B 93 F7 5C BA 00 ",
    "00 00 01 C2 CE "
    };
    string line(lines[0]);
    long length = line.size();
    // Check against "> 04 ... 1A FF 4C
    if (line.compare(0, 5, "> 04 ") == 0) {
        cout << "line[0] starts with > 04 .." << endl;
        if(line.compare(length-9, 8, "1A FF 4C") == 0)
            cout << "line[0] matches > 04 ... 1A FF 4C" << endl;
        else
            cout << "line[0] does not end with 1A FF 4C" << endl;
    } else {
        cout << "line[0] does not start with > 04 .." << endl;
    }

    string fullLine(lines[0]);
    fullLine.append("\n");
    fullLine.append(lines[1]);
    fullLine.append("\n");
    fullLine.append(lines[2]);
    fullLine.append("\n");
    istringstream stream(fullLine);
    ParseCommand parseCommand("testParserLogic", "tcp://localhost:1883", "testParserLogic", "beaconEvents");
    parseCommand.setSkipPublish(true);
    ParserLogic paser;
    paser.processHCIStream(stream, parseCommand);

    return 0;
}
