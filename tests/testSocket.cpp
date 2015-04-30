#include <sys/time.h>
#include "SocketPublisher.h"
#include <map>

using namespace std;

static int64_t currentMilliseconds() {
    timeval now;
    gettimeofday(&now, nullptr);

    int64_t nowMS = now.tv_sec;
    nowMS *= 1000;
    nowMS += now.tv_usec/1000;
    return nowMS;
}
int main() {

    string brokerUrl("192.168.1.107:12345"), clientID("testSocket");
    SocketPublisher socket(brokerUrl, clientID, "", "");
    socket.start(false);

    // A beacon event message
    int64_t nowMS = currentMilliseconds();
    const char *scannerID = "testSerializeBeacon";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    time_t time = nowMS;
    Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, 0, power, rssi, time);
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
    properties[SystemTime] = "2015-04-15 13:57:51";
    properties[SystemTimeMS] = "1429131471594";
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

    socket.stop();
}