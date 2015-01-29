#include <iostream>
#include <CoreFoundation/CoreFoundation.h>
#include "proton/message.h"
#include "proton/messenger.h"
#include "../src/Beacon.h"

using namespace std;

int main(int argc, char **argv) {

    pn_message_t * message;
    pn_messenger_t * messenger;
    pn_data_t * body;
    const char *brokerURL = "amqp://192.168.1.107:5672/beaconEvents";

    message = pn_message();
    messenger = pn_messenger(NULL);

    pn_messenger_start(messenger);

    pn_message_set_address(message, brokerURL);
    body = pn_message_body(message);

    //pn_message_encode(message, char *bytes, size_t *size);
    const char *scannerID = "testSerializeBeacon";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    time_t time = ::time(nullptr);
    Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, 0, power, rssi, time);
    vector<byte> data = beacon.toByteMsg();
    char *bytes = (char *) data.data();
    pn_data_encode(body, bytes, data.size());
    pn_messenger_put(messenger, message);
    pn_messenger_send(messenger, -1);

    pn_messenger_stop(messenger);
    pn_messenger_free(messenger);
    pn_message_free(message);
}