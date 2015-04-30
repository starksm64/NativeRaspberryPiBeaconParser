#include <chrono>
#include <Beacon.h>
#include "MsgPublisher.h"
#include <qpid/messaging/Connection.h>
#include <QpidPublisher.h>

using namespace std::chrono;

int main(int argc, char*argv[]) {
    MsgPublisherType type = MsgPublisherType::AMQP_QPID;
    string brokerUrl("192.168.1.107:5672");
    string clientID("testQpidHeartbeat");
    string userName("");
    string password("");

    if (argc > 1) brokerUrl = argv[1];

    printf("Creating QPID MsgPublisher, brokerUrl=%s\n", brokerUrl.c_str());
    fflush(stdout);

    std::unique_ptr<QpidPublisher> qpid(new QpidPublisher(brokerUrl, clientID, userName, password));
    printf("Created QPID MsgPublisher, %s\n", qpid->toString());
    qpid->setUseTopics(false);
    qpid->start(false);
    printf("Started QPID MsgPublisher\n");
    system_clock::time_point p2 = system_clock::now();
    string ScannerID("ScannerID");
    string SystemType("SystemType");
    string SystemTime("SystemTime");
    string SystemTimeMS("SystemTimeMS");

    system_clock::time_point start = system_clock::now();
    map<string,string> properties;
    properties[ScannerID] = "testQpidHeartbeat";
    properties[SystemType] = "TestSystem";
    time_t now = std::time(nullptr);
    properties[SystemTime] = ctime(&now);

    qpid->publishProperties("scannerHealth", properties);
    system_clock::time_point end = system_clock::now();
    printf("Elapsed, %d\n", end.time_since_epoch() - start.time_since_epoch());
    qpid->stop();

    printf("MsgPublisher will be deleted by ~unique_ptr\n");
}
