#include "Beacon.h"
#include "IOException.h"
#include "ByteBuffer.h"
#include <exception>
#include <execinfo.h>
#include <ctime>
#include <iostream>
#include <unistd.h>

static int UUID_START = 69;
static int MANUFACTURER_CODE_START = 57;
static int BEACON_CODE_START = 63;
static int MAJOR_START = 117;
static int MINOR_START = 123;
static int POWER_START = 129;
static int RSSI_START = 132;

static void stack_trace(exception &e) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    perror("Exception:");
    perror(e.what());
    size = backtrace(array, 10);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}
static int mystoi(string str, int base) {
    int value = 0;
    try {
        value = std::stoi(str, nullptr, base);
    } catch(exception &e) {
        stack_trace(e);
    }
    return value;
}
string Beacon::toString() {
    char tmp[256];

    // convert time to string form
    char* dateStr = ctime(&time);
    sprintf(tmp, "{[%s,%d,%d]@%s; code=%d,manufacturer=%d,power=%d,rssi=%d,time=%s}", uuid.c_str(), major, minor, scannerID.c_str(),
            code, manufacturer, power, rssi, dateStr);
    return string(tmp);
}

Beacon Beacon::fromByteMsg(byte *msg, uint32_t length) {
        ByteBuffer dis(msg, length);
        int version = dis.readInt();
        if(version != VERSION) {
            char *tmp = new char[1024]{'\0'};
            snprintf(tmp, 1024, "Msg version: %d does not match current version: %d", version, VERSION);
        }
        string scannerID = dis.readString();
        string uuid = dis.readString();
        int code = dis.readInt();
        int manufacturer = dis.readInt();
        int major = dis.readInt();
        int minor = dis.readInt();
        int power = dis.readInt();
        int rssi = dis.readInt();
        // The format is milliseconds since Epoch, but c++ time expects seconds
        long time = dis.readLong();
        time /= 1000;
        Beacon beacon = Beacon(scannerID, uuid, code, manufacturer, major, minor, power, rssi, time);
        return beacon;
}

/**
* Write the current beacon to a serialized binary form using a DataOutputStream for use as the form to send
* to a mqtt broker. To unserialize a msg use #fromByteMsg()
*
* @return byte array serialized form
* @throws IOException
*/
vector<byte> Beacon::toByteMsg() {
        ByteBuffer dos;
        dos.writeInt(VERSION);
        dos.writeBytes(scannerID);
        dos.writeBytes(uuid);
        dos.writeInt(code);
        dos.writeInt(manufacturer);
        dos.writeInt(major);
        dos.writeInt(minor);
        dos.writeInt(power);
        dos.writeInt(rssi);
        // Save as milliseconds since Epoch since this is what Java uses
        dos.writeLong(1000*time);
        return dos.getData();
}

Beacon Beacon::parseHCIDump(const char * scannerID, std::string packet) {
    std::string uuid;
    for(int n = UUID_START; n < UUID_START+2*16+15; n ++) {
        char c = packet[n];
        if(c != ' ')
            uuid += c;
    }
    try {
        int manufacturer = 256 * mystoi(packet.substr(MANUFACTURER_CODE_START, MANUFACTURER_CODE_START + 2), 16);
        manufacturer += mystoi(packet.substr(MANUFACTURER_CODE_START + 3, MANUFACTURER_CODE_START + 5), 16);
        string code0 = packet.substr(BEACON_CODE_START, BEACON_CODE_START + 2);
        string code1 = packet.substr(BEACON_CODE_START + 3, BEACON_CODE_START + 5);
        int code = 256 * mystoi(code0, 16);
        code += std::stoi(code1, nullptr, 16);
        string major = packet.substr(MAJOR_START, MAJOR_START + 5);
        string minor = packet.substr(MINOR_START, MINOR_START + 5);
        string power = packet.substr(POWER_START, POWER_START + 2);
        string rssi = packet.substr(RSSI_START, RSSI_START + 2);
        int imajor = 256 * mystoi(major.substr(0, 2), 16) + mystoi(major.substr(3, 5), 16);
        int iminor = 256 * mystoi(minor.substr(0, 2), 16) + mystoi(minor.substr(3, 5), 16);
        int ipower = mystoi(power, 16);
        ipower -= 256;
        int irssi = mystoi(rssi, 16);
        irssi -= 256;
        time_t now;
        ::time(&now);
        char * cuuid = new char [uuid.length()+1];
        std::strcpy (cuuid, uuid.c_str());
        Beacon beacon(scannerID, cuuid, code, manufacturer, imajor, iminor, ipower, irssi, now);
        return beacon;
    } catch(exception &e) {
        stack_trace(e);
    }
    return Beacon();
}
