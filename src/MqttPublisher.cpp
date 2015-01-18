#include "MqttPublisher.h"

MqttPublisher::MqttPublisher(string brokerUrl, string clientID)
        : brokerURL(brokerUrl),
          clientID(clientID)
{

}
MqttPublisher::MqttPublisher(string brokerUrl, string userName, string password, string clientID)
        : brokerURL(brokerUrl),
    userName(userName),
    password(password),
    clientID(clientID)
{

}

MqttPublisher::~MqttPublisher() {
    stop();
}

void MqttPublisher::start() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    std::cout << "Connecting..." << std::flush;
    MQTTClient_create(&client, brokerURL.c_str(), clientID.c_str(),
            MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
    } else {
        std::cout << "OK" << std::endl;
    }
}

void MqttPublisher::stop() {
    // Disconnect
    std::cout << "Disconnecting..." << std::flush;
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    client = nullptr;
    std::cout << "OK" << std::endl;
}

void MqttPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {
}
void MqttPublisher::publish(string topicName, MqttQOS qos, byte *payload, size_t len) {
    //std::cout << "Sending message..." << std::flush;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = payload;
    pubmsg.payloadlen = len;
    pubmsg.qos = qos;
    pubmsg.retained = 0;
    long timeout = 10000L;
    MQTTClient_publishMessage(client, topicName.c_str(), &pubmsg, &token);
#ifdef DEBUG
    printf("Waiting for up to %ld seconds for publication of %s\n"
                    "on topic %s for client with ClientID: %s\n",
            timeout/1000, "beacon", topicName.c_str(), clientID.c_str());
#endif
    int rc = MQTTClient_waitForCompletion(client, token, timeout);
    if(rc != 0)
        std::cout << "Failed to deliver message, rc=" << rc << endl;
}
