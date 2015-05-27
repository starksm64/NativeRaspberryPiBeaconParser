#include <chrono>
#include <Beacon.h>
#include "MsgPublisher.h"
#include <qpid/messaging/Connection.h>
#include <QpidPublisher.h>

using namespace std::chrono;

static volatile bool terminate = false;

static void callback(bool success, int receivedCount, int missedCount) {
    printf("callback(%d), received=%d, missed=%d\n", success, receivedCount, missedCount);
    if(missedCount > 2)
        ::terminate = true;
}

Beacon testBeacon() {
    const char *scannerID = "testQpidFactory";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    time_t time = ::time(nullptr);
    Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, 0, power, rssi, time);
    return beacon;
}

int main(int argc, char*argv[]) {
    setenv("QPID_LOG_ENABLE", "trace+", true);
    MsgPublisherType type = MsgPublisherType::AMQP_QPID;
    string brokerUrl("192.168.1.107:5672");
    string clientID("testQpidReconnect");
    string userName;
    string password;

    if (argc > 1) brokerUrl = argv[1];

    printf("Creating QPID MsgPublisher, brokerUrl=%s\n", brokerUrl.c_str());
    fflush(stdout);

    std::unique_ptr<QpidPublisher> qpid(new QpidPublisher(brokerUrl, clientID, userName, password));
    printf("Created QPID MsgPublisher, %s\n", qpid->toString());
    qpid->setUseTopics(false);
    // QpidPublisher.start uses the an option string of: "{protocol:amqp1.0,reconnect:true,reconnect_interval:30}"
    qpid->start(false);
    printf("Started QPID MsgPublisher\n");
    qpid->monitorHeartbeats("scannerHealth", callback);
    Beacon beacon = testBeacon();

    system_clock::time_point p2 = system_clock::now();
    string ScannerID("ScannerID");
    string SystemType("SystemType");
    string SystemTime("SystemTime");
    string SystemTimeMS("SystemTimeMS");

    system_clock::time_point start = system_clock::now();
    map<string,string> properties;
    properties[ScannerID] = "testQpidReconnect";
    properties[SystemType] = "TestSystem";

    int sentCount = 0;
    do {
        system_clock::time_point time = system_clock::now();
        time_t now = system_clock::to_time_t(time);
        properties[SystemTime] = ctime(&now);
        properties[SystemTimeMS] = to_string(time.time_since_epoch().count());
        qpid->publishProperties("scannerHealth", properties);
        sentCount ++;
        printf("Sent properties, count=%d, isConnected=%d, sleeping for 30\n", sentCount, qpid->isConnected());
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
    while(sentCount < 100);
    system_clock::time_point end = system_clock::now();
    printf("Elapsed, %d\n", end.time_since_epoch() - start.time_since_epoch());
    qpid->stop();

    printf("MsgPublisher will be deleted by ~unique_ptr\n");
}
