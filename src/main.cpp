#include <iostream>
#include <string>
#include <regex>
#include <fstream>
// http://tclap.sourceforge.net/manual.html
#include "tclap/CmdLine.h"
#include "ParserLogic.h"

using namespace std;

static istream* getDumpStream(TCLAP::ValueArg<std::string>& rawDumpFile) {
    if(rawDumpFile.isSet()) {
        string testFile = rawDumpFile.getValue();
        cout << "Using hcidump test file: " << testFile << endl;
        ifstream *stream = new ifstream(testFile);
        return stream;
    }
    return &cin;
}

int main(int argc, const char **argv) {

    cout << "NativeScanner starting up..." << endl;
    TCLAP::CmdLine cmd("NativeScanner command line options", ' ', "0.1");
    //
    TCLAP::ValueArg<std::string> scannerID("s", "scannerID",
            "Specify the ID of the scanner generating the beacon events",
            true, "DEFAULT", "string", cmd);
    TCLAP::ValueArg<std::string> rawDumpFile("d", "rawDumpFile",
            "Specify a path to an hcidump file to parse for testing",
            false, "", "string", cmd);
    TCLAP::ValueArg<std::string> clientID("c", "clientID",
            "Specify the clientID to connect to the MQTT broker with",
            false, "", "string", cmd);
    TCLAP::ValueArg<std::string> username("u", "username",
            "Specify the username to connect to the MQTT broker with",
            false, "", "string", cmd);
    TCLAP::ValueArg<std::string> password("p", "password",
            "Specify the password to connect to the MQTT broker with",
            false, "", "string", cmd);
    TCLAP::ValueArg<std::string> brokerURL("b", "brokerURL",
            "Specify the brokerURL to connect to the MQTT broker with; default tcp://localhost:1883",
            false, "tcp://localhost:1883", "string", cmd);
    TCLAP::ValueArg<std::string> topicName("t", "destinationName",
            "Specify the name of the queue on the MQTT broker to publish to; default beaconEvents",
            false, "beaconEvents", "string", cmd);
    TCLAP::SwitchArg skipPublish("S", "skipPublish",
            "Indicate that the parsed beacons should not be published",
            false);
    try {
        cmd.add(skipPublish);
        // Parse the argv array.
        cout << "Parsing command line..." << endl;
        cmd.parse( argc, argv );
    }
    catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    // Get the hcidump stream to use
    istream* dumpStream = getDumpStream(rawDumpFile);
    ParserLogic parserLogic;
    ParseCommand command(scannerID.getValue(), brokerURL.getValue(), clientID.getValue(), topicName.getValue());
    command.setSkipPublish(skipPublish.getValue());
    parserLogic.processHCIStream(*dumpStream, command);
    parserLogic.cleanup();
    if(dumpStream != &cin)
        delete dumpStream;
    cout << "End scanning";
    return 0;
}
