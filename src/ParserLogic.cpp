#include <iostream>
#include <regex>
#include "ParserLogic.h"
#include "Beacon.h"
#include "MqttPublisher.h"

using namespace std;

static std::string trim(std::string str) {
    size_t endpos = str.find_last_not_of(" \t");
    if( string::npos != endpos ) {
        str = str.substr( 0, endpos+1 );
    }
    return str;
}

void ParserLogic::processHCIStream(istream & stream, ParseCommand parseCommand) {
    string clientID(parseCommand.clientID);
    if(clientID.empty())
        clientID = parseCommand.scannerID;
    MqttPublisher mqtt(parseCommand.brokerURL, clientID);
    mqtt.start();
    string line;
    std::getline(stream, line);
    while(stream.good()) {
        long length = line.size();
        if(line.at(length-1) == ' ') {
            length --;
            line.resize(length);
        }
        // Check against "> 04 ... 1A FF 4C
        if (line.compare(0, 5, "> 04 ") && line.compare(length-8, 8, "1A FF 4C")) {
            string buffer(trim(line.c_str()));
            std::getline(stream, line);
            buffer.append(trim(line.c_str()));
            std::getline(stream, line);
            buffer.append(trim(line.c_str()));

            Beacon beacon = Beacon::parseHCIDump(parseCommand.scannerID.c_str(), buffer);
            vector<byte> msg = beacon.toByteMsg();
            mqtt.publish(parseCommand.topicName, MqttQOS::AT_MOST_ONCE, msg.data(), msg.size());
        } else {
            cout << "No match: " << line << endl;
        }
        std::getline(stream, line);
    }
}

void ParserLogic::cleanup() {

}
