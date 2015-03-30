#ifndef Beacon_H
#define Beacon_H

#include <sys/types.h>
#include <string>
#include <vector>
#include <stdint.h>

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
static int32_t VERSION = 4;

enum BeconEventType {
    /** A beacon event read by a scanner */
            SCANNER_READ,
    /** A status heartbeat from the scanner */
            SCANNER_HEARTBEAT
};

class Beacon {

private:
    string scannerID;
    string uuid;
    int32_t code;
    int32_t manufacturer;
    int32_t major;
    int32_t minor;
    /** Current unused */
    int32_t power;
    /** Power in DB measured at 1m away */
    int32_t calibratedPower;
    int32_t rssi;
    /** BeconEventType */
    int32_t messageType;
    // std::time() value * 1000
    int64_t time;

public:

    Beacon()
            :scannerID("DEFAULT"),
    uuid("0123456789ABCDEF"){
    }

    Beacon(string scannerID)
            : scannerID(scannerID),
                uuid("0123456789ABCDEF"),
                messageType(BeconEventType::SCANNER_READ){
    }
    Beacon(const char * scannerID, const char * uuid, int32_t code, int32_t manufacturer, int32_t major, int32_t minor, int32_t power, int32_t calibratedPower, int32_t rssi, int64_t time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              calibratedPower(calibratedPower),
              rssi(rssi),
              time(time),
              messageType(BeconEventType::SCANNER_READ){
    }
    Beacon(string scannerID, string uuid, int32_t code, int32_t manufacturer, int32_t major, int32_t minor, int32_t power, int32_t calibratedPower, int32_t rssi, int64_t time)
            : scannerID(scannerID),
              uuid(uuid),
              code(code),
              manufacturer(manufacturer),
              major(major),
              minor(minor),
              power(power),
              calibratedPower(calibratedPower),
              rssi(rssi),
              time(time),
              messageType(BeconEventType::SCANNER_READ){
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

    int32_t getCode() const {
        return code;
    }

    void setCode(int32_t code) {
        Beacon::code = code;
    }

    int32_t getManufacturer() const {
        return manufacturer;
    }

    void setManufacturer(int32_t manufacturer) {
        Beacon::manufacturer = manufacturer;
    }

    int32_t getMajor() const {
        return major;
    }

    void setMajor(int32_t major) {
        Beacon::major = major;
    }

    int32_t getMinor() const {
        return minor;
    }

    void setMinor(int32_t minor) {
        Beacon::minor = minor;
    }

    int32_t getPower() const {
        return power;
    }

    void setPower(int32_t power) {
        Beacon::power = power;
    }

    int32_t getCalibratedPower() const {
        return calibratedPower;
    }

    void setCalibratedPower(int32_t calibratedPower) {
        Beacon::calibratedPower = calibratedPower;
    }

    int32_t getRssi() const {
        return rssi;
    }

    void setRssi(int32_t rssi) {
        Beacon::rssi = rssi;
    }

    int64_t getTime() const {
        return time;
    }

    void setTime(int64_t time) {
        Beacon::time = time;
    }


    int32_t getMessageType() const {
        return messageType;
    }

    void setMessageType(int32_t messageType) {
        Beacon::messageType = messageType;
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