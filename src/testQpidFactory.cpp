#include <Beacon.h>
#include "MsgPublisher.h"
#include <decaf/lang/System.h>

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

int main() {
    MsgPublisherType type = MsgPublisherType::AMQP_QPID;
    string brokerUrl("192.168.1.7:5672");
    string clientID("testQpidFactory");
    string userName;
    string password;

    printf("Creating QPID MsgPublisher, brokerUrl=%s\n", brokerUrl.c_str());
    fflush(stdout);

    MsgPublisher *qpid = MsgPublisher::create(type, brokerUrl, clientID, userName, password);
    printf("Created QPID MsgPublisher, %s\n", qpid->toString());
    qpid->start(false);
    printf("Started QPID MsgPublisher\n");
    // Create a messages
    Beacon beacon = testBeacon();
    for (int ix = 0; ix < 100; ++ix) {
        int64_t now = System::currentTimeMillis();
        beacon.setTime(now);
        vector<byte> data = beacon.toByteMsg();
        qpid->publish("", MqttQOS::AT_MOST_ONCE , data.data(), data.size());
        printf("Sent message #%d\n", ix + 1);
    }
    qpid->stop();
    printf("Stopped QPID MsgPublisher\n");
}
