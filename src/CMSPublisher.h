#ifndef CMSPublisher_H
#define CMSPublisher_H

#include "MsgPublisher.h"
#include "MqttPublisher.h"
#ifdef HAVE_ActiveMQ_CPP
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <decaf/lang/System.h>
#include <activemq/library/ActiveMQCPP.h>
#endif

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

class CMSPublisher : public MsgPublisher {
private:
#ifdef HAVE_ActiveMQ_CPP
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
#endif

public:
    CMSPublisher(string brokerUrl, string clientID, string userName, string password)
            : MsgPublisher(brokerUrl, clientID, userName, password) {
        activemq::library::ActiveMQCPP::initializeLibrary();
    }

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


