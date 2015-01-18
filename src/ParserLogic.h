#ifndef ParserLogic_H
#define ParserLogic_H

#include "ParseCommand.h"
#include <fstream>
#include <regex>

using namespace std;

class ParserLogic {
private:
    //regex ibeaconRE;

public:
    /*
    ParserLogic() : ibeaconRE(">\\s04.*((?:\\s[[:xdigit:]]{2}){6})((?:\\s[[:xdigit:]]{2}){4}) 1A FF 4C") {
     */
    ParserLogic() {
    }

    void processHCIStream(istream& stream, ParseCommand parseCommand);

    void cleanup();
};
#endif
