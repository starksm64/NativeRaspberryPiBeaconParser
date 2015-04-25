#include "Beacon.h"
#include "../src/Beacon.h"
#include <fstream>
#include <iostream>

using namespace std;

/**
* Read in the /tmp/testSerializeBeacon.ser output from the testSerializeBeacon program
*/
int main() {
    const char *scannerID = "testSerializeBeacon";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    long time = 0;
    // Read in the serialize form of the beacon to a file
    ifstream in("/tmp/testSerializeBeacon.ser", ios_base::in);
    int size = 0;
    byte *dataptr = nullptr;
    if(in.is_open()) {
        in.seekg(0,ios::end);
        size = in.tellg();
        dataptr = new unsigned char [size];
        in.seekg(0, ios::beg);
        in.read((char *)dataptr, size);
    }
    in.close();
    cout << "Read " << size << " bytes of serial data" << endl;
    Beacon beacon = Beacon::fromByteMsg(dataptr, size);
    cout << "Beacon: " << beacon.toString() << endl;
    return 0;
}
