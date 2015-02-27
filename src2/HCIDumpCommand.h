#ifndef HCIDumpCommand_H
#define HCIDumpCommand_H

#include <string>
#include <MsgPublisher.h>

using namespace std;

class HCIDumpCommand {
private:
    string scannerID;
    string brokerURL;
    string clientID;
    string destinationName;
    string hciDev = "hci0";
    bool skipPublish;
    bool asyncMode;
    bool useQueues;
    MsgPublisherType pubType;

public:

    HCIDumpCommand(string scannerID, string brokerURL, string clientID, string destinationName)
            : scannerID(scannerID),
              brokerURL(brokerURL),
              clientID(clientID),
              destinationName(destinationName),
              pubType(MsgPublisherType::PAHO_MQTT) {
    }


    string getHciDev() const {
        return hciDev;
    }

    void setHciDev(string &hciDev) {
        HCIDumpCommand::hciDev = hciDev;
    }

    string getScannerID() const {
        return scannerID;
    }

    void setScannerID(string &scannerID) {
        HCIDumpCommand::scannerID = scannerID;
    }

    string getBrokerURL() const {
        return brokerURL;
    }

    void setBrokerURL(string &brokerURL) {
        HCIDumpCommand::brokerURL = brokerURL;
    }

    string getClientID() const {
        return clientID;
    }

    void setClientID(string &clientID) {
        HCIDumpCommand::clientID = clientID;
    }

    string getDestinationName() const {
        return destinationName;
    }

    void setDestinationName(string &topicName) {
        HCIDumpCommand::destinationName = topicName;
    }

    bool isSkipPublish() const {
        return skipPublish;
    }

    void setSkipPublish(bool skipPublish) {
        HCIDumpCommand::skipPublish = skipPublish;
    }

    bool isAsyncMode() const {
        return asyncMode;
    }

    void setAsyncMode(bool asyncMode) {
        HCIDumpCommand::asyncMode = asyncMode;
    }


    MsgPublisherType const &getPubType() const {
        return pubType;
    }


    bool isUseQueues() const {
        return useQueues;
    }

    void setUseQueues(bool useQueues) {
        HCIDumpCommand::useQueues = useQueues;
    }

    void setPubType(MsgPublisherType const &pubType) {
        HCIDumpCommand::pubType = pubType;
    }
};

#endif