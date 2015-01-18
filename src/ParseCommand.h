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
};
#endif