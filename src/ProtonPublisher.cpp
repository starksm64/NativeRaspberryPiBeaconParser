#include "ProtonPublisher.h"
#include "proton/message.h"

void ProtonPublisher::start(bool asyncMode) {

}

void ProtonPublisher::stop() {

}

void ProtonPublisher::queueForPublish(string const &topicName, MqttQOS qos, byte *payload, size_t len) {

}

void ProtonPublisher::publish(string const &topicName, MqttQOS qos, byte *payload, size_t len) {

}

void ProtonPublisher::publish(string const &destinationName, Beacon &beacon) {

}

void ProtonPublisher::publishProperties(string const &destinationName, map<string,string> const &properties) {

}
