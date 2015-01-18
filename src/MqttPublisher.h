#ifndef MqttPublisher_H
#define MqttPublisher_H

#include <string>
#include <iostream>
#include "MQTTClient.h"

using namespace std;
using byte = unsigned char;

enum MqttQOS {
    // QOS = 0
            AT_MOST_ONCE,
    // OQS = 1
            AT_LEAST_ONCE,
    // QOS = 2
            EXACTLY_ONCE
};

class MqttPublisher {
private:
    MQTTClient client;
    string brokerURL;
    bool quietMode;
    bool clean;
    string password;
    string userName;
    string clientID;

public:
    MqttPublisher(string brokerUrl, string clientID);
    MqttPublisher(string brokerUrl, string userName, string password, string clientID);
    ~MqttPublisher();

    void start();
    void stop();
    void queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len);
    void publish(string topicName, MqttQOS qos, byte *payload, size_t len);

};
#endif
