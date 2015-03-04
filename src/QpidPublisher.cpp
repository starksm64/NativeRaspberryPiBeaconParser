#include "QpidPublisher.h"
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Message.h>

using namespace qpid;

// Quote and prefix the project simple topic name.
// * The quotes pass the name to qpid messaging correctly
// * The prefix gets the AMQ broker to use a topic and not a queue
string AmqTopicName(const string name) {
  return "'topic://" + name + "'";
}
string AmqQueueName(const string name) {
  return "'queue://" + name + "'";
}


void QpidPublisher::start(bool asyncMode) {
  // Open connection
  connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0}");
  connection.open();

  // Create session
  session = connection.createTransactionalSession();

  // Create sender with default topic destination address
  string destName = isUseTopics() ? AmqTopicName(destinationName) : AmqQueueName(destinationName);
  sender = session.createSender(destName);
}

void QpidPublisher::stop() {
  sender.close();
  session.close();
  connection.close();
  sender = messaging::Sender();
  session = messaging::Session();
  connection = messaging::Connection();
}

void QpidPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {
  // HACK ALERT: tbd
}

void QpidPublisher::publish(string destName, MqttQOS qos, byte *payload, size_t len) {

  // Use default destination unless a new one is specified
  messaging::Sender sndr = sender;
  if (destName.length() > 0) {
    string fullDestName = isUseTopics() ? AmqTopicName(destName) : AmqQueueName(destName);
    sndr = session.createSender(fullDestName);
  }

  // Create message
  messaging::Message message((const char *)payload, len);
  if (clientID.length() > 0)
    message.setUserId(clientID);

  // Send message
  sndr.send(message);

  // Close temporary sender
  if (sndr != sender)
    sndr.close();
}

void QpidPublisher::publish(string destName, Beacon &beacon) {

  // Use default destination unless a new one is specified
  messaging::Sender sndr = sender;
  if (destName.length() > 0) {
    string fullDestName = isUseTopics() ? AmqTopicName(destName) : AmqQueueName(destName);
    sndr = session.createSender(fullDestName);
  }

  messaging::Message message;
  message.setProperty("uuid", beacon.getUuid());
  message.setProperty("scannerID", beacon.getScannerID());
  message.setProperty("major", beacon.getMajor());
  message.setProperty("minor", beacon.getMinor());
  message.setProperty("manufacturer", beacon.getManufacturer());
  message.setProperty("code", beacon.getCode());
  message.setProperty("power", beacon.getCalibratedPower());
  message.setProperty("rssi", beacon.getRssi());
  message.setProperty("time", beacon.getTime());

  if (clientID.length() > 0)
    message.setUserId(clientID);

  // Send message
  sndr.send(message);

  // Close temporary sender
  if (sndr != sender)
    sndr.close();
}

void QpidPublisher::publish(vector<Beacon> events) {
  for(int n = 0; n < events.size(); n ++) {
    Beacon& b = events.at(n);
    publish("", b);
  }
  session.commit();
}
