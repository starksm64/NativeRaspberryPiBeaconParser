#ifndef MsgPublisher_H
#define MsgPublisher_H

#include <string>
#include "QOS.h"

using namespace std;
using byte = unsigned char;

enum MsgPublisherType {
    PAHO_MQTT,
    AMQP_PROTON,
    AMQP_CMS,
    INVALID=-1
} ;

/**
* An abstraction for the various message publishing apis
*/
class MsgPublisher {
private:

protected:
    string brokerUrl;
    string clientID;
    string userName;
    string password;
    string topicName;

public:

    MsgPublisher(string brokerUrl, string clientID, string userName, string password)
        : brokerUrl(brokerUrl),
          clientID(clientID),
          userName(userName),
          password(password),
          topicName("beaconEvents") {
    }

    string getTopicName() {
        return topicName;
    }

    virtual void start(bool asyncMode) = 0;
    virtual void stop() = 0;
    virtual void queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) = 0;
    virtual void publish(string topicName, MqttQOS qos, byte *payload, size_t len) = 0;
    virtual const char *toString() {
        int length = brokerUrl.length() + clientID.length();
        char *str = new char[length+128];
        sprintf(str, "%s[%s]", brokerUrl.c_str(), clientID.c_str());
        return str;
    }

    // Factory method
    static MsgPublisher* create(MsgPublisherType type, string brokerUrl, string clientID, string userName, string password);
};
#endif
