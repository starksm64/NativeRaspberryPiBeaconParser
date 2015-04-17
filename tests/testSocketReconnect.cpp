#include <map>
#include <sys/time.h>
#include "SocketPublisher.h"
#include <unistd.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    string brokerUrl("192.168.1.107:12345"), clientID("testSocket");
    SocketPublisher socket(brokerUrl, clientID, "", "");
    socket.start(false);

    while(true) {
        // A beacon event message
        system_clock::time_point now = system_clock::now();
        milliseconds ms = duration_cast< milliseconds >(now.time_since_epoch());
        const char *scannerID = "testSerializeBeacon";
        const char *uuid = "15DAF246CE836311E4B116123B93F75C";
        int code = 2;
        int manufacturer = 3852;
        int major = 47616;
        int minor = 12345;
        int power = -253;
        int rssi = -62;
        Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, 0, power, rssi, ms.count());
        socket.publish("beaconEvents", beacon);

        // A beacon heartbeat message
        beacon.setTime(beacon.getTime()+5);
        socket.publishStatus(beacon);

        // A scanner health status message
        map<string,string> properties;
        string ScannerID("ScannerID");
        string HostIPAddress("HostIPAddress");
        string SystemTime("SystemTime");
        string SystemTimeMS("SystemTimeMS");
        string Uptime("Uptime");
        string LoadAverage("LoadAverage");
        string Procs("Procs");
        string RawEventCount("RawEventCount");
        string PublishEventCount("PublishEventCount");
        string HeartbeatCount("HeartbeatCount");
        string HeartbeatRSSI("HeartbeatRSSI");
        string EventsWindow("EventsWindow");
        string MemTotal("MemTotal");
        string MemFree("MemFree");
        string SwapTotal("SwapTotal");
        string SwapFree("SwapFree");
        properties[ScannerID] = "testSocket";
        properties[HostIPAddress] = "127.0.0.1";
        std::time_t now_time = system_clock::to_time_t(now);
        properties[SystemTime] = ctime(&now_time);
        properties[SystemTimeMS] = to_string(ms.count());
        properties[Uptime] = "uptime: 1193, days:0, hrs:0, min:19, sec:53";
        properties[Procs] = "1028";
        properties[LoadAverage] = "0.58, 0.87, 0.68";
        properties[RawEventCount] = "78337";
        properties[PublishEventCount] = "9544";
        properties[HeartbeatCount] = "9897";
        properties[HeartbeatRSSI] = "-43";
        properties[EventsWindow] = "+66: 253; +81: 249; +88: 250; +130: 253; +138: 260; +149: 238; +159: 258; +999: 255";
        properties[MemTotal] = "435";
        properties[MemFree] = "85";
        properties[SwapTotal] = "511";
        properties[SwapFree] = "511";
        socket.publishProperties("scannerHealth", properties);

        sleep(5);
    }

    socket.stop();

}