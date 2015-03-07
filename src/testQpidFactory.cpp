#include <Beacon.h>
#include "MsgPublisher.h"
#include <decaf/lang/System.h>
#include <qpid/messaging/Connection.h>
#include <sys/time.h>
#include <qpid/messaging/Duration.h>

// Uncomment to test the sending of message properties
#define MSG_PROPERTIES
//#define MSG_PROPERTIES_BATCH

using namespace decaf::lang;

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
    MsgPublisherType type = MsgPublisherType::AMQP_QPID;
    string brokerUrl("192.168.1.107:5672");
    string clientID("testQpidFactory");
    string userName;
    string password;

    if (argc > 1) brokerUrl = argv[1];

    printf("Creating QPID MsgPublisher, brokerUrl=%s\n", brokerUrl.c_str());
    fflush(stdout);

    MsgPublisher *qpid = MsgPublisher::create(type, brokerUrl, clientID, userName, password);
    printf("Created QPID MsgPublisher, %s\n", qpid->toString());
    qpid->setUseTopics(false);
    qpid->start(false);
    printf("Started QPID MsgPublisher\n");
    // Create a messages
    Beacon beacon = testBeacon();
    vector<Beacon> events;
    int N = 1000;
    struct timeval  start;
    gettimeofday(&start, nullptr);
    for (int ix = 0; ix < N; ++ix) {
        int64_t now = System::currentTimeMillis();
        beacon.setTime(now);
#ifdef MSG_PROPERTIES
        qpid->publish("", beacon);
        printf("Sent properties message #%d\n", ix + 1);
#endif
#ifdef MSG_PROPERTIES_BATCH
        events.push_back(beacon);
#else
        vector<byte> data = beacon.toByteMsg();
        qpid->publish("", MqttQOS::AT_MOST_ONCE , data.data(), data.size());
        //printf("Sent byte message #%d\n", ix + 1);
#endif
    }
#ifdef MSG_PROPERTIES_BATCH
    qpid->publish(events);
    printf("Sent %d messages in batch\n", N);
#endif
    struct timeval end;
    gettimeofday(&end, nullptr);
    printf("Elapsed, %d\n", end.tv_sec - start.tv_sec);
    qpid->stop();
    printf("Stopped QPID MsgPublisher\n");
}
