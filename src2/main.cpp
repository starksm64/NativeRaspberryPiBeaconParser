#include <cstdio>
#include <string>
#include <regex>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
// http://tclap.sourceforge.net/manual.html
#include "tclap/CmdLine.h"
#include "HCIDumpParser.h"
#include "MsgPublisherTypeConstraint.h"
#include "../lcd/LcdDisplay.h"
#include "MockScannerView.h"

static HCIDumpParser parserLogic;

#define STOP_MARKER_FILE "/var/run/scannerd.STOP"

inline bool stopMarkerExists() {
    struct stat buffer;
    bool stop = (stat (STOP_MARKER_FILE, &buffer) == 0);
    if(stop) {
        printf("Found STOP marker file, will exit...\n");
        fflush(stdout);
    }
    return stop;
}
static long eventCount = 0;
static long maxEventCount = 0;
static int64_t lastMarkerCheckTime = 0;

/**
* Callback invoked by the hdidumpinternal.c code when a LE_ADVERTISING_REPORT event is seen on the stack
*/
extern "C" bool beacon_event_callback(beacon_info * info) {
#ifdef PRINT_DEBUG
    printf("beacon_event_callback(%ld: %s, code=%d, time=%lld)\n", eventCount, info->uuid, info->code, info->time);
#endif
    parserLogic.beaconEvent(*info);
    eventCount ++;
    // Check for a termination marker every 1000 events or 5 seconds
    bool stop = false;
    int64_t elapsed = info->time - lastMarkerCheckTime;
    if((eventCount % 1000) == 0 || elapsed > 5000) {
        lastMarkerCheckTime = info->time;
        stop = stopMarkerExists();
        printf("beacon_event_callback, status eventCount=%ld, stop=%d\n", eventCount, stop);
        fflush(stdout);
    }
    // Check max event count limit
    if(maxEventCount > 0 && eventCount >= maxEventCount)
        stop = true;
    return stop;
}

using namespace std;

static ScannerView *getDisplayInstance() {
    ScannerView *view = nullptr;
#ifdef HAVE_LCD_DISPLAY
    LcdDisplay *lcd = LcdDisplay::getLcdDisplayInstance();
    lcd->init();
    view = lcd;
#else
    view = new MockScannerView();
#endif
}

/**
* A version of the native scanner that directly integrates with the bluez stack hcidump command rather than parsing
* the hcidump output.
*/
int main(int argc, const char **argv) {

    printf("NativeScanner starting up...\n");
    for(int n = 0; n < argc; n ++) {
        printf("    argv[%d] = %s\n", n, argv[n]);
    }
    fflush(stdout);
    TCLAP::CmdLine cmd("NativeScanner command line options", ' ', "0.1");
    //
    TCLAP::ValueArg<std::string> scannerID("s", "scannerID",
            "Specify the ID of the scanner reading the beacon events",
            true, "DEFAULT", "string", cmd);
    TCLAP::ValueArg<std::string> heartbeatUUID("H", "heartbeatUUID",
            "Specify the UUID of the beacon used to signal the scanner heartbeat event",
            false, "DEFAULT", "string", cmd);
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
    TCLAP::ValueArg<int> analyzeWindow("W", "analyzeWindow",
                                       "Specify the number of seconds in the analyzeMode time window",
                                       false, 1, "int", cmd);
    TCLAP::ValueArg<std::string> hciDev("D", "hciDev",
                                        "Specify the name of the host controller interface to use; default hci0",
                                        false, "hci0", "string", cmd);
    MsgPublisherTypeConstraint pubTypeConstraint;
    TCLAP::ValueArg<std::string> pubType("P", "pubType",
                                         "Specify the MsgPublisherType enum for the publisher implementation to use; default AMQP_QPID",
                                         false, "AMQP_QPID", &pubTypeConstraint, cmd, nullptr);
    TCLAP::ValueArg<int> maxCount("C", "maxCount",
                                  "Specify a maxium number of events the scanner should process before exiting; default 0 means no limit",
                                  false, 0, "int", cmd);
    TCLAP::ValueArg<int> batchCount("B", "batchCount",
                                    "Specify a maxium number of events the scanner should combine before sending to broker; default 0 means no batching",
                                    false, 0, "int", cmd);
    TCLAP::ValueArg<int> statusInterval("I", "statusInterval",
                                        "Specify the interval in seconds between health status messages, <= 0 means no messages; default 30",
                                        false, 30, "int", cmd);
    TCLAP::ValueArg<std::string> statusQueue("q", "statusQueue",
                                             "Specify the name of the status health queue destination; default scannerHealth",
                                             false, "scannerHealth", "string", cmd);
    TCLAP::SwitchArg skipPublish("S", "skipPublish",
            "Indicate that the parsed beacons should not be published",
            false);
    TCLAP::SwitchArg asyncMode("A", "asyncMode",
            "Indicate that the parsed beacons should be published using async delivery mode",
            false);
    TCLAP::SwitchArg useQueues("Q", "useQueues",
            "Indicate that the destination type is a queue. If not given the default type is a topic.",
            false);
    TCLAP::SwitchArg skipHeartbeat("K", "skipHeartbeat",
            "Don't publish the heartbeat messages. Useful to limit the noise when testing the scanner.",
            false);
    TCLAP::SwitchArg analzyeMode("Z", "analzyeMode",
                                 "Run the scanner in a mode that simply collects beacon readings and reports unique beacons seen in a time window",
                                 false);
    TCLAP::SwitchArg generateTestData("T", "generateTestData",
                                 "Indicate that test data should be generated",
                                 false);
    TCLAP::SwitchArg noBrokerReconnect("R", "noBrokerReconnect",
                                      "Don't try to reconnect to the broker on failure, just exit",
                                      false);

    try {
        // Add the flag arguments
        cmd.add(skipPublish);
        cmd.add(asyncMode);
        cmd.add(useQueues);
        cmd.add(skipHeartbeat);
        cmd.add(analzyeMode);
        cmd.add(generateTestData);
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
    command.setUseQueues(useQueues.getValue());
    command.setSkipPublish(skipPublish.getValue());
    command.setSkipHeartbeat(skipHeartbeat.getValue());
    command.setHciDev(hciDev.getValue());
    command.setAsyncMode(asyncMode.getValue());
    command.setAnalyzeMode(analzyeMode.getValue());
    command.setAnalyzeWindow(analyzeWindow.getValue());
    command.setPubType(pubTypeConstraint.toType(pubType.getValue()));
    command.setStatusInterval(statusInterval.getValue());
    command.setStatusQueue(statusQueue.getValue());
    command.setGenerateTestData(generateTestData.getValue());
    if(maxCount.getValue() > 0) {
        maxEventCount = maxCount.getValue();
        printf("Set maxEventCount: %ld\n", maxEventCount);
    }
    parserLogic.setBatchCount(batchCount.getValue());
    if(heartbeatUUID.isSet()) {
        parserLogic.setScannerUUID(heartbeatUUID.getValue());
        printf("Set heartbeatUUID: %s\n", heartbeatUUID.getValue().c_str());
    }
    shared_ptr<ScannerView> lcd(getDisplayInstance());
    parserLogic.setScannerView(lcd);
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("Begin scanning, cwd=%s...\n", cwd);
    fflush(stdout);
    parserLogic.processHCI(command);
    parserLogic.cleanup();
    printf("End scanning\n");
    fflush(stdout);
    return 0;
}
