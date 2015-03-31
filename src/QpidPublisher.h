#ifndef QpidPublisher_H
#define QpidPublisher_H

// QpidPublisher uses Qpid Messaging to publish messages to
// a topic on an ActiveMQ AMQP Broker.

#include "MsgPublisher.h"

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Message.h>

using namespace qpid;

class QpidPublisher : public MsgPublisher {
private:
    qpid::messaging::Connection connection;
    qpid::messaging::Session session;
    qpid::messaging::Sender sender;

protected:
    /**
    * Publish a beacon event as a collection of message properties with the given messageType
    * @param - sndr encapsulation of publisher and destination
    * @param - beacon, the beacon event to publish
    * @param - messageType, 0=beacon event, 1=scanner heartbeat event
    */
    void doPublishProperties(messaging::Sender sndr, Beacon &beacon, BeconEventType messageType);

public:

    QpidPublisher(string brokerUrl, string clientID, string userName, string password)
            : MsgPublisher(brokerUrl, clientID, userName, password) {}

    ~QpidPublisher() {}

    virtual void start(bool asyncMode);

    virtual void stop();

    virtual void queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, Beacon &beacon);

    virtual void publish(vector<Beacon> events);

    virtual void publishStatus(Beacon& beacon);

    virtual void publishProperties(string const &destinationName, map<string,string> const &properties);
};
#endif
