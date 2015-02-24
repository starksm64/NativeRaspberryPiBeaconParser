#ifndef QpidPublisher_H
#define QpidPublisher_H

// QpidPublisher uses Qpid Messaging to publish messages to
// a topic on an ActiveMQ AMQP Broker.

#include "MsgPublisher.h"

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Message.h>

class QpidPublisher : public MsgPublisher {
private:
    qpid::messaging::Connection connection;
    qpid::messaging::Session session;
    qpid::messaging::Sender sender;

public:

    QpidPublisher(string brokerUrl, string clientID, string userName, string password)
            : MsgPublisher(brokerUrl, clientID, userName, password) {}

    virtual void start(bool asyncMode);

    virtual void stop();

    virtual void queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string topicName, MqttQOS qos, byte *payload, size_t len);
};
#endif
