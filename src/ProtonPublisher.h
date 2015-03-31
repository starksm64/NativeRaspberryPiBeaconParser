#ifndef ProtonPublisher_H
#define ProtonPublisher_H

#include "MsgPublisher.h"
#include "proton/messenger.h"

class ProtonPublisher : public MsgPublisher {
private:
    pn_messenger_t * messenger;

public:

    ProtonPublisher(string brokerUrl, string clientID, string userName, string password)
            : MsgPublisher(brokerUrl, clientID, userName, password) {}

    virtual void start(bool asyncMode);

    virtual void stop();

    virtual void queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, Beacon &beacon);
    // TODO
    virtual void publish(vector<Beacon> events) {}
    virtual void publishStatus(Beacon& beacon) {}

    virtual void publishProperties(string const &destinationName, map<string,string> const &properties);
};
#endif
