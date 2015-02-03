#include <iostream>
#include "proton/message.h"
#include "proton/messenger.h"
#include "../src/Beacon.h"

using namespace std;

Beacon testBeacon() {
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
    return beacon;
}

inline void die(const char *file, int line, const char *message)
{
    fprintf(stderr, "%s:%i: %s\n", file, line, message);
    exit(1);
}

inline void check(pn_messenger_t *messenger) {
    if(pn_messenger_errno(messenger)) {
      die(__FILE__, __LINE__, pn_error_text(pn_messenger_error(messenger)));
    }
}

/* Test sending a message using the message properties using the proton api
 */
int main(int argc, char **argv) {

    pn_message_t * message;
    pn_messenger_t * messenger;
    pn_data_t * body;
    //const char *brokerURL = "amqp://192.168.1.107:5672/beaconEvents";
    const char *brokerURL = "amqp://192.168.1.107:5672/topic://beaconEvents";

    message = pn_message();
    messenger = pn_messenger(NULL);

    pn_messenger_start(messenger);
    check(messenger);

    pn_message_set_address(message, brokerURL);
    body = pn_message_body(message);

    Beacon beacon = testBeacon();
    const char *scannerID = beacon.getScannerID().c_str();
    const char *uuid = beacon.getUuid().c_str();
    int code = beacon.getCode();
    int manufacturer = beacon.getManufacturer();
    int major = beacon.getMajor();
    int minor = beacon.getMinor();
    int power = beacon.getPower();
    int rssi = beacon.getRssi();
    long time = beacon.getTime();

    // Encode the beacon as its properties
    pn_data_t *properties = pn_message_properties(message);
    pn_data_put_map(properties);

    pn_data_enter(properties);
    pn_data_put_string(properties, pn_bytes(strlen("scannerID"), (char *) "scannerID"));
    pn_data_put_string(properties, pn_bytes(strlen(scannerID), (char *) scannerID));
    pn_data_put_string(properties, pn_bytes(strlen("uuid"), (char *) "uuid"));
    pn_data_put_string(properties, pn_bytes(strlen(uuid), (char *) uuid));
    pn_data_put_string(properties, pn_bytes(strlen("code"), (char *) "code"));
    pn_data_put_int(properties, code);
    pn_data_put_string(properties, pn_bytes(strlen("manufacturer"), (char *) "manufacturer"));
    pn_data_put_int(properties, manufacturer);
    pn_data_put_string(properties, pn_bytes(strlen("major"), (char *) "major"));
    pn_data_put_int(properties, major);
    pn_data_put_string(properties, pn_bytes(strlen("minor"), (char *) "minor"));
    pn_data_put_int(properties, minor);
    pn_data_put_string(properties, pn_bytes(strlen("power"), (char *) "power"));
    pn_data_put_int(properties, power);
    pn_data_put_string(properties, pn_bytes(strlen("rssi"), (char *) "rssi"));
    pn_data_put_int(properties, rssi);
    pn_data_put_string(properties, pn_bytes(strlen("time"), (char *) "time"));
    pn_data_put_long(properties, time);
    pn_data_exit(properties);

    pn_messenger_put(messenger, message);
    check(messenger);
    pn_messenger_send(messenger, -1);
    check(messenger);

    pn_messenger_stop(messenger);
    pn_messenger_free(messenger);
    pn_message_free(message);
}