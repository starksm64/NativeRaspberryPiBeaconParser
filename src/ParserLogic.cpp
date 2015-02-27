#include <iostream>
#include <regex>
#include "ParserLogic.h"
#include "Beacon.h"
#include "MqttPublisher.h"

using namespace std;

static string trim(string str) {
    std::size_t first = str.find_first_not_of(' ');
    std::size_t last  = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}

void ParserLogic::processHCIStream(istream & stream, ParseCommand parseCommand) {
    string clientID(parseCommand.getClientID());
    if(clientID.empty())
        clientID = parseCommand.getScannerID();
    MqttPublisher mqtt(parseCommand.getBrokerURL(), clientID);
    if(!parseCommand.isSkipPublish()) {
        mqtt.start(parseCommand.isAsyncMode());
    }
    else {
        cout << "Skipping publish of parse beacons" << endl;
    }

    string line;
    std::getline(stream, line);
    while(stream.good()) {
        long length = line.size();
        if(line.at(length-1) == ' ') {
            length --;
            line.resize(length);
        }
        // Check against "> 04 ...  1A FF
        // May need to do full parsing of the hcidump prefix and AD structures...
        if (line.compare(0, 7, "> 04 3E") == 0 && line.find(" 1A FF ", 16) != std::string::npos) {
            string buffer(trim(line.c_str()));
            buffer.push_back(' ');
            std::getline(stream, line);
            buffer.append(trim(line.c_str()));
            buffer.push_back(' ');
            std::getline(stream, line);
            buffer.append(trim(line.c_str()));

            Beacon beacon = Beacon::parseHCIDump(parseCommand.getScannerID().c_str(), buffer);
            vector<byte> msg = beacon.toByteMsg();
            if(!parseCommand.isSkipPublish())
                mqtt.publish(parseCommand.getDestinationName(), MqttQOS::AT_MOST_ONCE, msg.data(), msg.size());
            else
                cout << "Parsed: " << beacon.toString() << endl;
        } else {
            cout << "No match: " << line << endl;
        }
        std::getline(stream, line);
    }
}

void ParserLogic::cleanup() {

}
