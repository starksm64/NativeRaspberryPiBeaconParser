#ifndef MqttPublisher_H
#define MqttPublisher_H

#include <string>
#include <iostream>
#include <MQTTAsync.h>
#include "MQTTClient.h"
#include "MsgPublisher.h"
#include "QOS.h"

using namespace std;

class MqttPublisher : public MsgPublisher {
private:
    MQTTClient client;
    MQTTAsync asyncClient;

    string brokerURL;
    bool quietMode;
    bool clean;
    bool asyncMode;
    string password;
    string userName;
    string clientID;

    void setupClient();
    void setupAsyncClient();

public:
    MqttPublisher(string brokerUrl, string clientID);
    MqttPublisher(string brokerUrl, string userName, string password, string clientID);
    ~MqttPublisher();

    void start(bool asyncMode);
    void stop();
    void queueForPublish(string destinationName, MqttQOS qos, byte *payload, size_t len);
    void publish(string destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string destinationName, Beacon &beacon);
    virtual void publish(vector<Beacon> events) {}
};
#endif
