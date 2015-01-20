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
    // This is a LightBlueBean event
    const char *lbb[] = {
        "> 04 3E 27 02 01 00 00 58 C1 C9 72 39 D0 1B 1A FF 4C 00 02 15 ",
        "A4 95 DE AD C5 B1 4B 44 B5 12 13 70 F0 2D 74 DE 30 39 FF FF ",
        "C5 BC "
    };
    // This is a Gimbal event
    const char *gimbal[] = {
        "> 04 3E 2A 02 01 03 01 26 9B 29 DE 99 2A 1E 02 01 06 1A FF 4C ",
        "00 02 15 DA F2 46 CE 83 63 11 E4 B1 16 12 3B 93 F7 5C BA 00 ",
        "00 00 01 C2 C5"
    };
    string line(lbb[0]);
    long length = line.size();
    // Check against "> 04 ... 1A FF 4C
    if (line.compare(0, 5, "> 04 ") == 0) {
        cout << "line[0] starts with > 04 .." << endl;
        if(line.compare(length-12, 11, "4C 00 02 15") == 0)
            cout << "line[0] matches > 04 ... 4C 00 02 15" << endl;
        else
            cout << "line[0] does not end with 4C 00 02 15" << endl;
    } else {
        cout << "line[0] does not start with > 04 .." << endl;
    }

    // Test the LightBlueBean event
    string fullLine(lbb[0]);
    fullLine.append("\n");
    fullLine.append(lbb[1]);
    fullLine.append("\n");
    fullLine.append(lbb[2]);
    fullLine.append("\n");
    istringstream stream(fullLine);
    ParseCommand parseCommand("testParserLogic", "tcp://localhost:1883", "testParserLogic", "beaconEvents");
    // Don't publish the beacon to mqtt server
    parseCommand.setSkipPublish(true);
    ParserLogic paser;
    paser.processHCIStream(stream, parseCommand);

    // Test the Gimbal event
    string fullLine2(gimbal[0]);
    fullLine2.append("\n");
    fullLine2.append(gimbal[1]);
    fullLine2.append("\n");
    fullLine2.append(gimbal[2]);
    fullLine.append("\n");
    istringstream stream2(fullLine2);
    paser.processHCIStream(stream2, parseCommand);

    return 0;
}
