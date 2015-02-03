#ifndef Beacon_H
#define Beacon_H

#include <sys/types.h>
#include <string>
#include <vector>

// Somehow
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

using byte = unsigned char;
using namespace std;

/** The current byte[] toByteMsg/fromByteMsg version format */
static int VERSION = 3;

class Beacon {


private:
    string scannerID;
    string uuid;
    int code;
    int manufacturer;
    int major;
    int minor;
    int power;
    int calibratedPower;
    int rssi;
    // std::time() value * 1000
    int64_t time;

public:

    Beacon()
            :scannerID("DEFAULT"),
    uuid("0123456789ABCDEF"){
    }

    Beacon(string scannerID)
            : scannerID(scannerID),
                uuid("0123456789ABCDEF"){
    }
    Beacon(const char * scannerID, const char * uuid, int code, int manufacturer, int major, int minor, int power, int calibratedPower, int rssi, long time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              calibratedPower(calibratedPower),
              rssi(rssi),
              time(time) {
    }
    Beacon(string scannerID, string uuid, int code, int manufacturer, int major, int minor, int power, int calibratedPower, int rssi, long time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              calibratedPower(calibratedPower),
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

    int getCalibratedPower() const {
        return calibratedPower;
    }

    void setCalibratedPower(int calibratedPower) {
        Beacon::calibratedPower = calibratedPower;
    }

    int getRssi() const {
        return rssi;
    }

    void setRssi(int rssi) {
        Beacon::rssi = rssi;
    }

    int64_t getTime() const {
        return time;
    }

    void setTime(int64_t time) {
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