#ifndef HCIDumpCommand_H
#define HCIDumpCommand_H

#include <algorithm>
#include <string>
#include <MsgPublisher.h>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

/**
 * See the main.cpp TCLAP::* definitions for the meaning of these command options.
 *
 */
class HCIDumpCommand {
private:
    string scannerID;
    string brokerURL;
    string clientID;
    string destinationName;
    string hciDev = "hci0";
    string statusQueue;
    string username;
    string password;
    vector<string> scannerIDs;
    int currentID = 0;
    int statusInterval = 30;
    int analyzeWindow = 1;
    int rebootAfterNoReply = -1;
    bool analyzeMode = false;
    bool generateTestData = false;
    bool noBrokerReconnect = true;
    bool batteryTestMode = false;
    bool skipPublish = false;
    bool asyncMode = false;
    bool useQueues = false;
    bool skipHeartbeat = false;
    bool cyclesScannerIDs = false;
    MsgPublisherType pubType = MsgPublisherType::AMQP_QPID;

public:

    HCIDumpCommand(){}
    HCIDumpCommand(string scannerID, string brokerURL, string clientID, string destinationName)
            : scannerID(scannerID),
              brokerURL(brokerURL),
              clientID(clientID),
              destinationName(destinationName),
              pubType(MsgPublisherType::PAHO_MQTT) {
        // If the scannerID is a comma separated list of ids, then getScannerID() will cycle through the ids
        stringstream ss(scannerID);
        string item;
        while (getline(ss, item, ',')) {
            scannerIDs.push_back(item);
        }
        cyclesScannerIDs = scannerIDs.size() > 1;
        if(cyclesScannerIDs) {
            printf("Running with %ld scanner IDs: ", scannerIDs.size());
            std::copy(scannerIDs.begin(), scannerIDs.end(),
                      std::ostream_iterator<string>(std::cout, " "));
            printf("\n");
        }
    }

    string getHciDev() const {
        return hciDev;
    }

    void setHciDev(string &hciDev) {
        HCIDumpCommand::hciDev = hciDev;
    }

    string getScannerID() {
        const string* myID = &scannerID;
        if(cyclesScannerIDs) {
            myID = scannerIDs.data()+currentID;
            currentID = (currentID + 1) % scannerIDs.size();
        }
        return *myID;
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

    const string &getUsername() const {
        return username;
    }

    void setUsername(const string &username) {
        HCIDumpCommand::username = username;
    }

    const string &getPassword() const {
        return password;
    }

    void setPassword(const string &password) {
        HCIDumpCommand::password = password;
    }

    bool isSkipPublish() const {
        return skipPublish;
    }

    void setSkipPublish(bool skipPublish) {
        HCIDumpCommand::skipPublish = skipPublish;
    }


    bool isSkipHeartbeat() const {
        return skipHeartbeat;
    }

    void setSkipHeartbeat(bool skipHeartbeat) {
        HCIDumpCommand::skipHeartbeat = skipHeartbeat;
    }

    bool isAsyncMode() const {
        return asyncMode;
    }

    void setAsyncMode(bool asyncMode) {
        HCIDumpCommand::asyncMode = asyncMode;
    }

    int getAnalyzeWindow() const {
        return analyzeWindow;
    }

    void setAnalyzeWindow(int analyzeWindow) {
        HCIDumpCommand::analyzeWindow = analyzeWindow;
    }

    bool isAnalyzeMode() const {
        return analyzeMode;
    }

    void setAnalyzeMode(bool analyzeMode) {
        HCIDumpCommand::analyzeMode = analyzeMode;
    }

    bool isBatteryTestMode() const {
        return batteryTestMode;
    }

    void setBatteryTestMode(bool batteryTestMode) {
        HCIDumpCommand::batteryTestMode = batteryTestMode;
    }

    int getRebootAfterNoReply() const {
        return rebootAfterNoReply;
    }

    void setRebootAfterNoReply(int rebootAfterNoReply) {
        HCIDumpCommand::rebootAfterNoReply = rebootAfterNoReply;
    }

    bool isGenerateTestData() const {
        return generateTestData;
    }

    void setGenerateTestData(bool generateTestData) {
        HCIDumpCommand::generateTestData = generateTestData;
    }
    bool isNoBrokerReconnect() const {
        return noBrokerReconnect;
    }

    void setNoBrokerReconnect(bool noBrokerReconnect) {
        HCIDumpCommand::noBrokerReconnect = noBrokerReconnect;
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
    const string& getStatusQueue() const {
        return statusQueue;
    }

    void setStatusQueue(string &statusQueue) {
        HCIDumpCommand::statusQueue = statusQueue;
    }
    int getStatusInterval() const {
        return statusInterval;
    }

    void setStatusInterval(int statusInterval) {
        HCIDumpCommand::statusInterval = statusInterval;
    }
};

#endif
