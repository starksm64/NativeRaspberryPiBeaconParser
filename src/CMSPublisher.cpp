#include "CMSPublisher.h"


void CMSPublisher::start(bool asyncMode) {

    // Create a ConnectionFactory
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory(brokerUrl));

    // Create a Connection
    connection = connectionFactory->createConnection();
    connection->start();
    printf("Started CMS connection\n");

    // Create a Session
    session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    session->start();

    // Create the destination (Topic)
    if(isUseTopics())
        destination = session->createTopic(destinationName);
    else
        destination = session->createQueue(destinationName);

    producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
}

void CMSPublisher::stop() {
    producer->close();
    session->close();
    connection->stop();
    connection->close();
}

void CMSPublisher::queueForPublish(string const &topicName, MqttQOS qos, byte *payload, size_t len) {

}

void CMSPublisher::publish(string const &destName, MqttQOS qos, byte *payload, size_t len) {

    Destination *dest = destination;
    if(destName.length() > 0) {
        if(isUseTopics())
            dest = session->createTopic(destName);
        else
            dest = session->createQueue(destName);
    }

    std::auto_ptr<BytesMessage> message(session->createBytesMessage(payload, len));
    producer->send(message.get());

    if(dest != destination)
        delete dest;
}

void CMSPublisher::publish(string const &destName, Beacon &beacon) {
    Destination *dest = destination;
    if(destName.length() > 0) {
        if(isUseTopics())
            dest = session->createTopic(destName);
        else
            dest = session->createQueue(destName);
    }

    TextMessage *message = session->createTextMessage();
    message->setStringProperty("uuid", beacon.getUuid());
    message->setStringProperty("scannerID", beacon.getScannerID());
    message->setIntProperty("major", beacon.getMajor());
    message->setIntProperty("minor", beacon.getMinor());
    message->setIntProperty("manufacturer", beacon.getManufacturer());
    message->setIntProperty("code", beacon.getCode());
    message->setIntProperty("power", beacon.getCalibratedPower());
    message->setIntProperty("rssi", beacon.getRssi());
    message->setLongProperty("time", beacon.getTime());
    producer->send(message);
    delete message;

    if(dest != destination)
        delete dest;
}

void CMSPublisher::publishProperties(string const &destinationName, map<string,string> const &properties) {

}
