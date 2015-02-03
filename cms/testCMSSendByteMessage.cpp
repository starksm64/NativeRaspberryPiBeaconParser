
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include "../src/Beacon.h"

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

Beacon testBeacon() {
    const char *scannerID = "testBytesMessage";
    const char *uuid = "15DAF246CE836311E4B116123B93F75C";
    int code = 2;
    int manufacturer = 3852;
    int major = 47616;
    int minor = 12345;
    int power = -253;
    int rssi = -62;
    time_t time = ::time(nullptr);
    Beacon beacon(scannerID, uuid, code, manufacturer, major, minor, 0, power, rssi, time);
    return beacon;
}

int main(int argc, char* argv[]) {

    activemq::library::ActiveMQCPP::initializeLibrary();
    {
        std::cout << "=====================================================\n";
        std::cout << "Starting the example:" << std::endl;
        std::cout << "-----------------------------------------------------\n";


        // Set the URI to point to the IP Address of your broker.
        // add any optional params to the url to enable things like
        // tightMarshalling or tcp logging etc.  See the CMS web site for
        // a full list of configuration options.
        //
        //  http://activemq.apache.org/cms/
        //
        // Wire Format Options:
        // =========================
        // Use either stomp or openwire, the default ports are different for each
        //
        // Examples:
        //    tcp://127.0.0.1:61616                      default to openwire
        //    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
        //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
        //
        // SSL:
        // =========================
        // To use SSL you need to specify the location of the trusted Root CA or the
        // certificate for the broker you want to connect to.  Using the Root CA allows
        // you to use failover with multiple servers all using certificates signed by
        // the trusted root.  If using client authentication you also need to specify
        // the location of the client Certificate.
        //
        //     System::setProperty( "decaf.net.ssl.keyStore", "<path>/client.pem" );
        //     System::setProperty( "decaf.net.ssl.keyStorePassword", "password" );
        //     System::setProperty( "decaf.net.ssl.trustStore", "<path>/rootCA.pem" );
        //
        // The you just specify the ssl transport in the URI, for example:
        //
        //     ssl://localhost:61617
        //
        std::string brokerURI =
                "failover:(tcp://localhost:61616"
//        "?wireFormat=openwire"
//        "&transport.useInactivityMonitor=false"
//        "&connection.alwaysSyncSend=true"
//        "&connection.useAsyncSend=true"
//        "?transport.commandTracingEnabled=true"
//        "&transport.tcpTracingEnabled=true"
//        "&wireFormat.tightEncodingEnabled=true"
                        ")";

        //============================================================
        // set to true to use topics instead of queues
        // Note in the code above that this causes createTopic or
        // createQueue to be used in both consumer an producer.
        //============================================================
        bool useTopics = true;
        bool sessionTransacted = false;
        int numMessages = 2000;

        long long startTime = System::currentTimeMillis();

        Connection* connection;
        Session* session;
        Destination* destination;
        MessageProducer* producer;
        bool useTopic = true;

        try {

            // Create a ConnectionFactory
            auto_ptr<ConnectionFactory> connectionFactory(
                    ConnectionFactory::createCMSConnectionFactory(brokerURI));

            // Create a Connection
            connection = connectionFactory->createConnection();
            connection->start();

            // Create a Session
            if (sessionTransacted) {
                session = connection->createSession(Session::SESSION_TRANSACTED);
            } else {
                session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
            }

            // Create the destination (Topic or Queue)
            if (useTopic) {
                destination = session->createTopic("TEST.beaconEvents");
            } else {
                destination = session->createQueue("TEST.beaconEvents");
            }

            // Create a MessageProducer from the Session to the Topic or Queue
            producer = session->createProducer(destination);
            producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

            // Create a messages
            Beacon beacon = testBeacon();
            for (int ix = 0; ix < numMessages; ++ix) {
                int64_t now = System::currentTimeMillis();
                beacon.setTime(now);
                vector<byte> data = beacon.toByteMsg();
                std::auto_ptr<BytesMessage> message(session->createBytesMessage(data.data(), data.size()));

                printf("Sent message #%d from thread\n", ix + 1);
                producer->send(message.get());

            }

        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }
}
