#include "ProtonPublisher.h"
#include "proton/message.h"

void ProtonPublisher::start(bool asyncMode) {

}

void ProtonPublisher::stop() {

}

void ProtonPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {

}

void ProtonPublisher::publish(string topicName, MqttQOS qos, byte *payload, size_t len) {

}

void ProtonPublisher::publish(string destinationName, Beacon &beacon) {

}
