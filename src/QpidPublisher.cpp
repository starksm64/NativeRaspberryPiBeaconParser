#include "QpidPublisher.h"
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
  printf("QpidPublisher, start, this=%p\n", this);
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
  //connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0}");
  connection = messaging::Connection(brokerUrl, "{protocol:amqp1.0,reconnect:true,reconnect_interval:30}");
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
  printf("Created new sender for: %s\n", destName.c_str());
  senders[destName] = sender;
}

void QpidPublisher::stop() {
  running = false;
  if(sender)
    sender.close();
  if(session)
    session.close();
  if(connection)
    connection.close();
  if(heartbeatMonitorThread) {
    heartbeatMonitorThread->detach();
    heartbeatMonitorThread.reset(nullptr);
  }
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
    sndr = senders[fullDestName];
    if(!sndr) {
      sndr = session.createSender(fullDestName);
      printf("Created new sender for: %s\n", fullDestName.c_str());
      senders[fullDestName] = sndr;
    }
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
    sndr = senders[fullDestName];
    if(!sndr) {
      sndr = session.createSender(fullDestName);
      printf("Created new sender for: %s\n", fullDestName.c_str());
      senders[fullDestName] = sndr;
    }
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
  message.setProperty("power", beacon.getPower());
  message.setProperty("calibratedPower", beacon.getCalibratedPower());
  message.setProperty("rssi", beacon.getRssi());
  message.setProperty("time", beacon.getTime());
  message.setProperty("messageType", messageType);
  message.setProperty("scannerSeqNo", beacon.getScannerSequenceNo());

  if (clientID.length() > 0)
    message.setUserId(clientID);

  // Send message
  sndr.send(message);
}

void QpidPublisher::publishProperties(string const &destName, map<string,string> const &properties) {
  messaging::Sender sndr = sender;
  if (destName.length() > 0) {
    string fullDestName = isUseTopics() ? AmqTopicName(destName) : AmqQueueName(destName);
    sndr = senders[fullDestName];
    if(!sndr) {
      sndr = session.createSender(fullDestName);
      printf("Created new sender for: %s\n", fullDestName.c_str());
      senders[fullDestName] = sndr;
    }
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

void QpidPublisher::monitorHeartbeats(string const &destinationName, heartbeatReceived callback) {
  thread *t = new thread(&QpidPublisher::doMonitorHeartbeats, this, destinationName, callback);
  this->heartbeatMonitorThread.reset(t);
}

void QpidPublisher::doMonitorHeartbeats(string const &destinationName, heartbeatReceived callback) {
  string destName = isUseTopics() ? AmqTopicName(destinationName) : AmqQueueName(destinationName);
  messaging::Receiver receiver = session.createReceiver(destName);
  int missedCount = 0;
  int receivedCount = 0;
  while(running) {
    messaging::Message message;
    try {
      receiver.fetch(message, messaging::Duration::MINUTE);
      receivedCount ++;
      missedCount = 0;
      callback(true, receivedCount, missedCount);
    } catch(messaging::NoMessageAvailable& e) {
      fprintf(stderr, "Failed to receive own heartbeat for 1 minute, %s\n", e.what());
      missedCount ++;
      callback(false, receivedCount, missedCount);
    }
  }
}
