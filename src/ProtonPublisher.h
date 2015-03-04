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

    virtual void queueForPublish(string destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string destinationName, Beacon &beacon);
};
#endif
