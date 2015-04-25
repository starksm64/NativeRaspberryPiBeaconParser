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

    void setupClient();
    void setupAsyncClient();

public:
    MqttPublisher(string brokerUrl, string clientID);
    MqttPublisher(string brokerUrl, string userName, string password, string clientID);
    ~MqttPublisher();

    void start(bool asyncMode);
    void stop();
    void queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);
    void publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, Beacon &beacon);
    // TODO
    virtual void publish(vector<Beacon> events) {}
    virtual void publishStatus(Beacon& beacon) {}

    virtual void publishProperties(string const &destinationName, map<string,string> const &properties);
};
#endif
