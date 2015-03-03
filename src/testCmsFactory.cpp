#include <Beacon.h>
#include "MsgPublisher.h"
#include <decaf/lang/System.h>

using namespace decaf::lang;

Beacon testBeacon() {
    const char *scannerID = "testCmsFactory";
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
    MsgPublisherType type = MsgPublisherType::AMQP_CMS;
    string brokerUrl("tcp://127.0.0.1:61616");
    string clientID("testCmsFactory");
    string userName;
    string password;

    printf("Creating CMS MsgPublisher, brokerUrl=%s\n", brokerUrl.c_str());
    fflush(stdout);

    MsgPublisherFactory factory;
    MsgPublisher cms;
    factory.create(cms, type, brokerUrl, clientID, userName, password);
    printf("Created CMS MsgPublisher, %s\n", cms.toString());
    cms.start(false);
    printf("Started CMS MsgPublisher\n");
    // Create a messages
    Beacon beacon = testBeacon();
    for (int ix = 0; ix < 100; ++ix) {
        int64_t now = System::currentTimeMillis();
        beacon.setTime(now);
        vector<byte> data = beacon.toByteMsg();
        cms.publish("", MqttQOS::AT_MOST_ONCE , data.data(), data.size());
        printf("Sent message #%d\n", ix + 1);
    }
    cms.stop();
    printf("Stopped CMS MsgPublisher\n");
}
