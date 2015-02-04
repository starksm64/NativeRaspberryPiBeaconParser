#include "CMSPublisher.h"


void CMSPublisher::start(bool asyncMode) {

    // Create a ConnectionFactory
    decaf::net::URI uri(brokerUrl.c_str());
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory(brokerUrl));

    // Create a Connection
    connection = connectionFactory->createConnection();
    connection->start();
    printf("Started CMS connection\n");

    // Create a Session
    session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    session->start();

    // Create the destination (Topic)
    destination = session->createTopic(topicName);

    producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
}

void CMSPublisher::stop() {
    producer->close();
    session->close();
    connection->stop();
    connection->close();
}

void CMSPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {

}

void CMSPublisher::publish(string topicName, MqttQOS qos, byte *payload, size_t len) {

    Destination *dest = destination;
    if(topicName.length() > 0)
        dest = session->createTopic(topicName);

    std::auto_ptr<BytesMessage> message(session->createBytesMessage(payload, len));
    producer->send(message.get());

    if(dest != destination)
        delete dest;
}