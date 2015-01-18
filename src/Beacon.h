#ifndef Beacon_H
#define Beacon_H

#include <sys/types.h>
#include <string>
#include <vector>

using byte = unsigned char;
using namespace std;

/** The current byte[] toByteMsg/fromByteMsg version format */
static int VERSION = 2;

class Beacon {


private:
    string scannerID;
    string uuid;
    int code;
    int manufacturer;
    int major;
    int minor;
    int power;
    int rssi;
    // std::time() value
    long time;

public:

    Beacon()
            :scannerID("DEFAULT"),
    uuid("0123456789ABCDEF"){
    }

    Beacon(string scannerID)
            : scannerID(scannerID),
                uuid("0123456789ABCDEF"){
    }
    Beacon(const char * scannerID, const char * uuid, int code, int manufacturer, int major, int minor, int power, int rssi, long time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              rssi(rssi),
              time(time) {
    }
    Beacon(string scannerID, string uuid, int code, int manufacturer, int major, int minor, int power, int rssi, long time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              rssi(rssi),
              time(time) {
    }
    ~Beacon() {
    }

    string getScannerID() const {
        return scannerID;
    }

    void setScannerID(string scannerID) {
        Beacon::scannerID = scannerID;
    }

    string getUuid() const {
        return uuid;
    }

    void setUuid(string uuid) {
        Beacon::uuid = uuid;
    }

    int getCode() const {
        return code;
    }

    void setCode(int code) {
        Beacon::code = code;
    }

    int getManufacturer() const {
        return manufacturer;
    }

    void setManufacturer(int manufacturer) {
        Beacon::manufacturer = manufacturer;
    }

    int getMajor() const {
        return major;
    }

    void setMajor(int major) {
        Beacon::major = major;
    }

    int getMinor() const {
        return minor;
    }

    void setMinor(int minor) {
        Beacon::minor = minor;
    }

    int getPower() const {
        return power;
    }

    void setPower(int power) {
        Beacon::power = power;
    }

    int getRssi() const {
        return rssi;
    }

    void setRssi(int rssi) {
        Beacon::rssi = rssi;
    }

    long getTime() const {
        return time;
    }

    void setTime(long time) {
        Beacon::time = time;
    }

    string toString();

    vector<byte> toByteMsg();

    static Beacon fromByteMsg(byte* msg, uint32_t length);

    /**
    * Parse the lines corresponding to an hcidump -R output
    */
    static Beacon parseHCIDump(const char * scannerID, std::string packet);
};
#endif