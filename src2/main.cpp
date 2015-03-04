#include <cstdio>
#include <string>
#include <regex>
#include <fstream>
#include <sys/stat.h>
// http://tclap.sourceforge.net/manual.html
#include "tclap/CmdLine.h"
#include "HCIDumpParser.h"
#include "MsgPublisherTypeConstraint.h"

static HCIDumpParser parserLogic;

#define STOP_MARKER_FILE "STOP"

inline bool stopMarkerExists() {
    struct stat buffer;
    bool stop = (stat (STOP_MARKER_FILE, &buffer) == 0);
    if(stop) {
        printf("Found STOP marker file, will exit...\n");
    }
    return stop;
}
static long eventCount = 0;
static long maxEventCount = 0;
static int64_t lastMarkerCheckTime = 0;

/**
* Callback invoked by the hdidumpinternal.c code when a LE_ADVERTISING_REPORT event is seen on the stack
*/
extern "C" bool beacon_event_callback(const beacon_info *info) {
#ifdef PRINT_DEBUG
    printf("beacon_event_callback(%ld: %s, code=%d, time=%lld)\n", eventCount, info->uuid, info->code, info->time);
#endif
    parserLogic.beaconEvent(info);
    eventCount ++;
    // Check for a termination marker every 1000 events or 5 seconds
    bool stop = false;
    int64_t elapsed = info->time - lastMarkerCheckTime;
    if((eventCount % 1000) == 0 || elapsed > 5000) {
        lastMarkerCheckTime = info->time;
        stop = stopMarkerExists();
    }
    // Check max event count limit
    if(maxEventCount > 0 && eventCount >= maxEventCount)
        stop = true;
    return stop;
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
    TCLAP::ValueArg<std::string> topicName("t", "destinationName",
            "Specify the name of the queue on the MQTT broker to publish to; default beaconEvents",
            false, "beaconEvents", "string", cmd);
    TCLAP::SwitchArg skipPublish("S", "skipPublish",
            "Indicate that the parsed beacons should not be published",
            false);
    TCLAP::SwitchArg asyncMode("A", "asyncMode",
            "Indicate that the parsed beacons should be published using async delivery mode",
            false);
    TCLAP::SwitchArg useQueues("Q", "useQueues",
            "Indicate that the destination type is a queue. If not given the default type is a topic.",
            false);
    TCLAP::ValueArg<std::string> hciDev("D", "hciDev",
            "Specify the name of the host controller interface to use; default hci0",
            false, "hci0", "string", cmd);
    MsgPublisherTypeConstraint pubTypeConstraint;
    TCLAP::ValueArg<std::string> pubType("P", "pubType",
            "Specify the MsgPublisherType enum for the publisher implementation to use; default PAHO_MQTT",
            false, "PAHO_MQTT", &pubTypeConstraint, cmd, nullptr);
    TCLAP::ValueArg<int> maxCount("C", "maxCount",
            "Specify a maxium number of events the scanner should process before exiting; default 0 means no limit",
            false, 0, "int", cmd);
    TCLAP::ValueArg<int> batchCount("B", "batchCount",
            "Specify a maxium number of events the scanner should combine before sending to broker; default 0 means no batching",
            false, 0, "int", cmd);
    try {
        // Add the flag arguments
        cmd.add(skipPublish);
        cmd.add(asyncMode);
        cmd.add(useQueues);
        // Parse the argv array.
        printf("Parsing command line...\n");
        cmd.parse( argc, argv );
        printf("done\n");
    }
    catch (TCLAP::ArgException &e) {
        fprintf(stderr, "error: %s for arg: %s\n", e.error().c_str(), e.argId().c_str());
    }

    // Remove any stop marker file
    if(remove(STOP_MARKER_FILE) == 0) {
        printf("Removed existing %s marker file\n", STOP_MARKER_FILE);
    }

    HCIDumpCommand command(scannerID.getValue(), brokerURL.getValue(), clientID.getValue(), topicName.getValue());
    command.setSkipPublish(skipPublish.getValue());
    command.setHciDev(hciDev.getValue());
    command.setAsyncMode(asyncMode.getValue());
    command.setPubType(pubTypeConstraint.toType(pubType.getValue()));
    if(maxCount.getValue() > 0) {
        maxEventCount = maxCount.getValue();
        printf("Set maxEventCount: %ld\n", maxEventCount);
    }
    parserLogic.setBatchCount(batchCount.getValue());
    printf("Begin scanning...\n");
    parserLogic.processHCI(command);
    parserLogic.cleanup();
    printf("End scanning\n");
    return 0;
}
