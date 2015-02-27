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
    AMQP_QPID,
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
    string destinationName;
    bool useTopics;

public:

    MsgPublisher(string brokerUrl, string clientID, string userName, string password)
        : brokerUrl(brokerUrl),
          clientID(clientID),
          userName(userName),
          password(password),
          destinationName("beaconEvents"),
          useTopics(true) {
    }

    void setDestinationName(const string& name) {
        destinationName = name;
    }
    string getDestinationName() {
        return destinationName;
    }

    /**
    * Set whether the destinationName represents a topic.
    * @param flag - true for a topic, false for a queue
    */
    void setUseTopics(bool flag) {
        useTopics = false;
    }
    bool isUseTopics() {
        return useTopics;
    }

    // Interface methods ---
    virtual void start(bool asyncMode) = 0;
    virtual void stop() = 0;

    /**
    * Enque a msg for publishing asynchronounsly
    * @param - destinationName, a possibly empty name for the message destination
    * @param - qos, the delivery quality of service
    * @param - payload, the serialized form of the Beacon object to send
    * @param - len, the length of payload
    */
    virtual void queueForPublish(string destinationName, MqttQOS qos, byte *payload, size_t len) = 0;

    /**
    * Send a message to the broker synchronously
    * @param - destinationName, a possibly empty name for the message destination
    * @param - qos, the delivery quality of service
    * @param - payload, the serialized form of the Beacon object to send
    * @param - len, the length of payload
    */
    virtual void publish(string destinationName, MqttQOS qos, byte *payload, size_t len) = 0;

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
