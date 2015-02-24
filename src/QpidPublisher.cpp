#include "QpidPublisher.h"
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Message.h>

using namespace qpid;

// Quote and prefix the project simple topic name.
// * The quotes pass the name to qpid messaging correctly
// * The prefix gets the AMQ broker to use a topic and not a queue
string AmqTopicName(const string topicName) {
  return "'topic://" + topicName + "'";
}

void QpidPublisher::start(bool asyncMode) {
  // Open connection
  connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0}");
  connection.open();

  // Create session
  session = connection.createSession();

  // Create sender with default topic destination address
  sender = session.createSender(AmqTopicName(topicName));
}

void QpidPublisher::stop() {
  sender.close();
  session.close();
  connection.close();
}

void QpidPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {
  // HACK ALERT: tbd
}

void QpidPublisher::publish(string topicName, MqttQOS qos, byte *payload, size_t len) {

  // Use default topic unless a new one is specified
  messaging::Sender sndr = sender;
  if (topicName.length() > 0)
    sndr = session.createSender(AmqTopicName(topicName));

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
