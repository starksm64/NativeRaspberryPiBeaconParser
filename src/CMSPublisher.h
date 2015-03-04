#ifndef CMSPublisher_H
#define CMSPublisher_H

#include "MsgPublisher.h"
#include "MqttPublisher.h"
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <decaf/lang/System.h>
#include <activemq/library/ActiveMQCPP.h>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

class CMSPublisher : public MsgPublisher {
private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;

public:
    CMSPublisher(string brokerUrl, string clientID, string userName, string password)
            : MsgPublisher(brokerUrl, clientID, userName, password) {
        activemq::library::ActiveMQCPP::initializeLibrary();
    }

    virtual void start(bool asyncMode);

    virtual void stop();

    virtual void queueForPublish(string destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string destinationName, Beacon &beacon);
    virtual void publish(vector<Beacon> events) {}
};
#endif


