#ifndef HCIDumpCommand_H
#define HCIDumpCommand_H

#include <string>

using namespace std;

class HCIDumpCommand {
private:
    string scannerID;
    string brokerURL;
    string clientID;
    string topicName;
    string hciDev = "hci0";
    bool skipPublish;
    bool asyncMode;

public:

    HCIDumpCommand(string scannerID, string brokerURL, string clientID, string topicName)
            : scannerID(scannerID), brokerURL(brokerURL), clientID(clientID), topicName(topicName) {
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

    string getTopicName() const {
        return topicName;
    }

    void setTopicName(string &topicName) {
        HCIDumpCommand::topicName = topicName;
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
};

#endif