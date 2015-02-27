#include "Beacon.h"
#include "ByteBuffer.h"
#include <execinfo.h>
#include <ctime>
#include <unistd.h>
#include <regex>
#include <sys/time.h>

// The portion of the BLE advertising event specific to the hcidump output
static int HCIDUMP_PREFIX = 7;
static int BADDR_SIZE = 6;
static int UUID_SIZE = 16;

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
        int size = str.size();
        const char *debug = str.c_str();
        value = std::stoi(str, nullptr, base);
    } catch(exception &e) {
        stack_trace(e);
    }
    return value;
}

static void buildTimeString(int64_t tsInMS, char *timestr) {
    struct timeval  tv;
    struct timezone tz;
    struct tm      *tm;

    tv.tv_sec = tsInMS / 1000;
    tv.tv_usec = tsInMS * 1000 - tv.tv_sec * 1000000;
    tm = localtime(&tv.tv_sec);

    size_t length = strftime(timestr, 128, "%F %T", tm);
    snprintf(timestr+length, 128-length, ".%ld", tv.tv_usec/1000);
}
string Beacon::toString() {
    char tmp[256];

    // convert time in milliseconds to string form
    char timestr[128];
    buildTimeString(time, timestr);
    sprintf(tmp, "{[%s,%d,%d]@%s; code=%d,manufacturer=%d,power=%d,calibratedPower=%d,rssi=%d,time=%s}", uuid.c_str(), major, minor, scannerID.c_str(),
            code, manufacturer, power, calibratedPower, rssi, timestr);
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
        int calibratedPower = dis.readInt();
        int rssi = dis.readInt();
        // The format is milliseconds since Epoch, but c++ time expects seconds
        long time = dis.readLong();
        Beacon beacon = Beacon(scannerID, uuid, code, manufacturer, major, minor, power, calibratedPower, rssi, time);
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
        dos.writeInt(calibratedPower);
        dos.writeInt(rssi);
        dos.writeLong(time);
        return dos.getData();
}

/**
* Parse a hex formatted record for a beacon event from the hcidump -R output.
*/
Beacon Beacon::parseHCIDump(const char * scannerID, std::string packet) {
    std::string uuid;
    try {
        long size = packet.size();
        const char *debug = packet.c_str();
        // Move past the hcidump prefix and baddr portion of the packet
        int index = 2 + HCIDUMP_PREFIX * 3; // > N XX hex octets + N spaces
        // Move past the BADDR value
        index += BADDR_SIZE*3;
        // Move past the remaining length value
        index += 3;
        // Now go through the AD Structure elements in the payload until we find the manufacturer specific data
        int length = mystoi(packet.substr(index, 2), 16);
        int type = mystoi(packet.substr(index+3, 2), 16);
        while(type != 0XFF && index < size-1) {
            index += 3*(length+1);
            length = mystoi(packet.substr(index, 2), 16);
            type = mystoi(packet.substr(index+3, 2), 16);
        }
        if(index >= size)
            throw std::invalid_argument( "Input packet has no manufacturer specific data" );

        // Move past length and 0xFF octets
        index += 6;

        // Get the first octet of the manufacturer code
        int manufacturer = 256 * mystoi(packet.substr(index, 2), 16);
        index += 3;
        // Get the second octet of the manufacturer code
        manufacturer += mystoi(packet.substr(index, 2), 16);
        index += 3;

        // Get the first octet of the beacon code
        string code0 = packet.substr(index, 2);
        index += 3;
        // Get the second octet of the beacon code
        string code1 = packet.substr(index, 2);
        index += 3;
        // The second octet referrs to the higher order byte of the 16 bit beacon code
        int code = 256 * mystoi(code0, 16) + mystoi(code1, 16);

        // Get the proximity uuid
        for(int n = 0; n < UUID_SIZE*3; n ++) {
            char c = packet[index+n];
            if(c != ' ')
                uuid += c;
        }
        index += UUID_SIZE*3;

        // Get the first octet of the beacon major id
        string major0 = packet.substr(index, 2);
        index += 3;
        // Get the second octet of the beacon major id
        string major1 = packet.substr(index, 2);
        index += 3;
        int imajor = 256 * mystoi(major0, 16) + mystoi(major1, 16);

        // Get the first octet of the beacon minor id
        string minor0 = packet.substr(index, 2);
        index += 3;
        // Get the second octet of the beacon minor id
        string minor1 = packet.substr(index, 2);
        index += 3;
        int iminor = 256 * mystoi(minor0, 16) + mystoi(minor1, 16);

        // Get the calibrated power, which is encoded as the 2's complement of the calibrated Tx Power
        string power = packet.substr(index, 2);
        index += 3;
        int ipower = mystoi(power, 16);
        ipower -= 256;

        // Get the received signal strength indication, encoded as 2's complement
        string rssi = packet.substr(index, 2);
        int irssi = mystoi(rssi, 16);
        irssi -= 256;

        // Generate the timestamp the beacon was received at
        time_t now;
        ::time(&now);
        now *= 1000;
        char * cuuid = new char [uuid.length()+1];
        std::strcpy (cuuid, uuid.c_str());

        // Create the beacon object
        Beacon beacon(scannerID, cuuid, code, manufacturer, imajor, iminor, 0, ipower, irssi, now);
        return beacon;
    } catch(exception &e) {
        stack_trace(e);
    }
    return Beacon();
}
