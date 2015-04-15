#ifndef MsgPublisher_H
#define MsgPublisher_H

#include <map>
#include <string>
#include "QOS.h"
#include "Beacon.h"

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
    /** The default message destination name */
    string destinationName;
    /** Interval in seconds between reconnection attempts */
    int reconnectInterval;
    /** Should reconnection be attempted on failure */
    bool reconnectOnFailure;
    /** Is the publisher connected */
    bool connected;
    /** Should the destination type be a topic(true) or a queue(false) */
    bool useTopics;
    /** Should transacted sessions be used */
    bool useTransactions;

public:

    MsgPublisher(string brokerUrl, string clientID, string userName, string password)
        : brokerUrl(brokerUrl),
          clientID(clientID),
          userName(userName),
          password(password),
          destinationName("beaconEvents"),
          useTopics(true),
          useTransactions(false) {
    }

    virtual ~MsgPublisher() {
    }

    void setDestinationName(const string& name) {
        destinationName = name;
    }
    string getDestinationName() {
        return destinationName;
    }

    int getReconnectInterval() const {
        return reconnectInterval;
    }

    void setReconnectInterval(int reconnectInterval) {
        MsgPublisher::reconnectInterval = reconnectInterval;
    }

    bool isReconnectOnFailure() const {
        return reconnectOnFailure;
    }

    void setReconnectOnFailure(bool reconnectOnFailure) {
        MsgPublisher::reconnectOnFailure = reconnectOnFailure;
    }

    bool isConnected() const {
        return connected;
    }

    void setConnected(bool connected) {
        MsgPublisher::connected = connected;
    }

/**
    * Set whether the destinationName represents a topic.
    * @param flag - true for a topic, false for a queue
    */
    void setUseTopics(bool flag) {
        useTopics = flag;
    }
    bool isUseTopics() {
        return useTopics;
    }

    bool isUseTransactions() const {
        return useTransactions;
    }
    void setUseTransactions(bool useTransactions) {
        MsgPublisher::useTransactions = useTransactions;
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
    virtual void queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len) = 0;

    /**
    * Send a message to the broker synchronously
    * @param - destinationName, a possibly empty name for the message destination
    * @param - qos, the delivery quality of service
    * @param - payload, the serialized form of the Beacon object to send
    * @param - len, the length of payload
    */
    virtual void publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len) = 0;

    /**
    * Send a beacon event to the broker as a collection of message properties
    * @param - destinationName, a possibly empty name for the message destination
    * @param - beacon, the beacon event to publish
    */
    virtual void publish(string const &destinationName, Beacon &beacon) = 0;

    /**
    * Send a collection of beacon events in batch to the broker, with each message consisting of the beacon
    * properties
    */
    virtual void publish(vector<Beacon> events) = 0;

    /**
    * Called to publish a beacon event from the beacon associated with the scanner heartbeat
    */
    virtual void publishStatus(Beacon& beacon) = 0;

    /**
     * Called to publish a set of properties as a message
     */
    virtual void publishProperties(string const &destinationName, map<string,string> const &properties) = 0;

    virtual const char *toString() {
        int length = brokerUrl.length() + clientID.length();
        char *str = new char[length + 128];
        sprintf(str, "%s[%s]", brokerUrl.c_str(), clientID.c_str());
        return str;
    }

    // Factory method
    static MsgPublisher* create(MsgPublisherType type, string brokerUrl, string clientID, string userName, string password);
};
#endif
