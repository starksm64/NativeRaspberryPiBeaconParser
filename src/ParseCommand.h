#ifndef ParseCommand_H
#define ParseCommand_H

#include <string>

using namespace std;

class ParseCommand {
public:

    ParseCommand(string scannerID, string brokerURL, string clientID, string topicName)
            : scannerID(scannerID), brokerURL(brokerURL), clientID(clientID), topicName(topicName) {
    }

    string scannerID;
    string brokerURL;
    string clientID;
    string topicName;
    bool skipPublish;


    string getScannerID() const {
        return scannerID;
    }

    void setScannerID(string &scannerID) {
        ParseCommand::scannerID = scannerID;
    }

    string getBrokerURL() const {
        return brokerURL;
    }

    void setBrokerURL(string &brokerURL) {
        ParseCommand::brokerURL = brokerURL;
    }

    string getClientID() const {
        return clientID;
    }

    void setClientID(string &clientID) {
        ParseCommand::clientID = clientID;
    }

    string getTopicName() const {
        return topicName;
    }

    void setTopicName(string &topicName) {
        ParseCommand::topicName = topicName;
    }

    bool isSkipPublish() const {
        return skipPublish;
    }

    void setSkipPublish(bool skipPublish) {
        ParseCommand::skipPublish = skipPublish;
    }
};
#endif