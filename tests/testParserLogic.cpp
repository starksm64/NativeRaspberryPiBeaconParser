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
        "  A4 95 DE AD C5 B1 4B 44 B5 12 13 70 F0 2D 74 DE 30 39 FF FF ",
        "  C5 BC "
    };
    // This is a Gimbal event
    const char *gimbal[] = {
        "> 04 3E 2A 02 01 03 01 26 9B 29 DE 99 2A 1E 02 01 06 1A FF 4C ",
        "  00 02 15 DA F2 46 CE 83 63 11 E4 B1 16 12 3B 93 F7 5C BA 00 ",
        "  00 00 01 C2 C5"
    };
    // This is a Gimbal non-iBeacon event
    const char *gimbal_notibeacon[] = {
            "> 04 3E 27 02 01 04 00 58 C1 C9 72 39 D0 1B 05 09 4C 69 67 74 ",
            "  11 06 DE 74 2D F0 70 13 12 B5 44 4B B1 C5 10 FF 95 A4 02 0A ",
            "  04 C5"
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
    fullLine2.append("\n");
    istringstream stream2(fullLine2);
    paser.processHCIStream(stream2, parseCommand);

    // Test the Gimbal event
    string fullLine3(gimbal_notibeacon[0]);
    if (fullLine3.compare(0, 7, "> 04 3E") == 0 && fullLine3.find(" 1A FF ", 16) != std::string::npos) {
        cerr << "fullLine3 should not match > 04 3E ...  1A FF " << endl;
        cerr << "fullLine3.compare(0, 7, '> 04 3E') = " << fullLine3.compare(0, 7, "> 04 3E") << endl;
        cerr << "fullLine3.find(' 1A FF ', 16) = " << fullLine3.find(" 1A FF ", 16) << endl;
        return 1;
    }
    fullLine3.append("\n");
    fullLine3.append(gimbal_notibeacon[1]);
    fullLine3.append("\n");
    fullLine3.append(gimbal_notibeacon[2]);
    fullLine3.append("\n");
    istringstream stream3(fullLine3);
    paser.processHCIStream(stream3, parseCommand);

    return 0;
}
