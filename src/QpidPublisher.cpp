#include "QpidPublisher.h"
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Message.h>
#include <chrono>


using namespace std::chrono;

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
  /* Reconnect behaviour can be controlled through the following options:
    * - reconnect: true/false (enables/disables reconnect entirely)
    * - reconnect_timeout: seconds (give up and report failure after specified time)
    * - reconnect_limit: n (give up and report failure after specified number of attempts)
    * - reconnect_interval_min: seconds (initial delay between failed reconnection attempts)
    * - reconnect_interval_max: seconds (maximum delay between failed reconnection attempts)
    * - reconnect_interval: shorthand for setting the same reconnect_interval_min/max
    * - reconnect_urls: list of alternate urls to try when connecting
    *
    * The reconnect_interval is the time that the client waits for
    * after a failed attempt to reconnect before retrying. It starts
    * at the value of the min_retry_interval and is doubled every
    * failure until the value of max_retry_interval is reached.
  */
  // This does not work, reconnect fails to reestablish the flow of messages
  //connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0,reconnect:true,reconnect_interval:60}");
  connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0}");
  connection.open();
  disconnectCount = 0;
  connected = true;

  // Create session
  if(useTransactions) {
    session = connection.createTransactionalSession();
    printf("Created transacted session\n");
  }
  else {
    session = connection.createSession();
    printf("Created non-transacted session\n");
  }

  // Create sender with default topic destination address
  string destName = isUseTopics() ? AmqTopicName(destinationName) : AmqQueueName(destinationName);
  sender = session.createSender(destName);
}

void QpidPublisher::stop() {
  if(sender)
    sender.close();
  if(session)
    session.close();
  if(connection)
    connection.close();
  sender = messaging::Sender();
  session = messaging::Session();
  connection = messaging::Connection();
}

void QpidPublisher::queueForPublish(string const &topicName, MqttQOS qos, byte *payload, size_t len) {
  // HACK ALERT: tbd
}

void QpidPublisher::publish(string const &destName, MqttQOS qos, byte *payload, size_t len) {

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

void QpidPublisher::publish(string const &destName, Beacon &beacon) {

  // Use default destination unless a new one is specified
  messaging::Sender sndr = sender;
  if (destName.length() > 0) {
    string fullDestName = isUseTopics() ? AmqTopicName(destName) : AmqQueueName(destName);
    sndr = session.createSender(fullDestName);
  }

  doPublishProperties(sndr, beacon, BeconEventType::SCANNER_READ);

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

void QpidPublisher::publishStatus(Beacon &beacon) {
  doPublishProperties(sender, beacon, BeconEventType::SCANNER_HEARTBEAT);
}

void QpidPublisher::doPublishProperties(messaging::Sender sndr, Beacon &beacon, BeconEventType messageType) {
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
  message.setProperty("messageType", messageType);

  if (clientID.length() > 0)
    message.setUserId(clientID);

  // Send message
  try {
    if(isConnected())
      sndr.send(message);
    else {
      disconnectCount++;
      if(disconnectCount %100)
        fprintf(stderr, "disconnectCount=%d\n", disconnectCount);
      if(shouldReconnect(beacon.getTime())) {
        fprintf(stderr, "Trying to reconnect...\n");
        try {
          connection.reconnect();
        } catch(messaging::MessagingException& e2) {
          fprintf(stderr, "Failed, will retry...%s\n", e2.what());
          calculateReconnectTime(beacon.getTime());
        }
      }
    }
  } catch(messaging::MessagingException& e) {
    fprintf(stderr, "doPublishProperties, MessagingException: %s\n", e.what());
    connected = false;
    calculateReconnectTime(beacon.getTime());
  }
}

void QpidPublisher::publishProperties(string const &destName, map<string,string> const &properties) {
  messaging::Sender sndr = sender;
  if (destName.length() > 0) {
    string fullDestName = isUseTopics() ? AmqTopicName(destName) : AmqQueueName(destName);
    sndr = session.createSender(fullDestName);
  }

  messaging::Message message;
  for(map<string, string>::const_iterator iter = properties.begin(); iter != properties.end(); iter ++) {
    message.setProperty(iter->first, iter->second);

  }
  message.setProperty("messageType", SCANNER_STATUS);

  if (clientID.length() > 0)
    message.setUserId(clientID);

  // Send message
  sndr.send(message);
}

void QpidPublisher::calculateReconnectTime(int64_t now) {
  setNextReconnectTime(now + reconnectInterval*1000);
  milliseconds ms(now);
  seconds s = duration_cast<seconds>(ms);
  time_t t = s.count();
  fprintf(stderr, "Will attempt reconnect at: %s\n", ctime(&t));
}
