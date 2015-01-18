#include "Beacon.h"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    const char *scannerID = "testSerializeBeacon";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    std::time_t time = std::time(nullptr);
    Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, power, rssi, time);
    vector<byte> data = beacon.toByteMsg();
    // Write out the serialize form of the beacon to a file
    fstream out("/tmp/testSerializeBeacon.ser", ios_base::out);
    out.write((const char *) data.data(), data.size());
    out.close();
    cout << "Wrote " << data.size() << " bytes of serial data for beacon: " << beacon.toString() << endl;
    return 0;
}
