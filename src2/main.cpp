#include <string>
#include <regex>
#include <fstream>
// http://tclap.sourceforge.net/manual.html
#include "tclap/CmdLine.h"
#include "HCIDumpParser.h"

static HCIDumpParser parserLogic;

extern "C" void beacon_event_callback(const beacon_info *info) {
    //printf("beacon_event_callback(%s, code=%d)\n", info->uuid, info->code);
    parserLogic.beaconEvent(info);
}

using namespace std;

/**
* A version of the native scanner that directly integrates with the bluez stack hcidump command rather than parsing
* the hcidump output.
*/
int main(int argc, const char **argv) {

    printf("NativeScanner starting up...\n");
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
    TCLAP::ValueArg<std::string> topicName("t", "topicName",
            "Specify the name of the queue on the MQTT broker to publish to; default beaconEvents",
            false, "beaconEvents", "string", cmd);
    TCLAP::SwitchArg skipPublish("S", "skipPublish",
            "Indicate that the parsed beacons should not be published",
            false);
    TCLAP::ValueArg<std::string> hciDev("D", "hciDev",
            "Specify the name of the host controller interface to use; default hci0",
            false, "hci0", "string", cmd);
    try {
        cmd.add(skipPublish);
        // Parse the argv array.
        printf("Parsing command line...\n");
        cmd.parse( argc, argv );
        printf("done\n");
    }
    catch (TCLAP::ArgException &e) {
        fprintf(stderr, "error: %s for arg: %s\n", e.error().c_str(), e.argId().c_str());
    }

    HCIDumpCommand command(scannerID.getValue(), brokerURL.getValue(), clientID.getValue(), topicName.getValue());
    command.setSkipPublish(skipPublish.getValue());
    command.setHciDev(hciDev.getValue());
    printf("Begin scanning...\n");
    parserLogic.processHCI(command);
    parserLogic.cleanup();
    printf("End scanning\n");
    return 0;
}
