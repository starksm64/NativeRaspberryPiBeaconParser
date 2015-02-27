#include <MQTTAsync.h>
#include "MqttPublisher.h"

MqttPublisher::MqttPublisher(string brokerUrl, string clientID)
        : MsgPublisher(brokerUrl, clientID, "", "")
{
    client = nullptr;
    asyncClient = nullptr;
}
MqttPublisher::MqttPublisher(string brokerUrl, string userName, string password, string clientID)
        : MsgPublisher(brokerUrl, userName, password, clientID)
{
    client = nullptr;
    asyncClient = nullptr;
}

MqttPublisher::~MqttPublisher() {
    stop();
}

void MqttPublisher::start(bool asyncMode) {
    MqttPublisher::asyncMode = asyncMode;
    client = nullptr;
    asyncClient = nullptr;
    if(asyncMode)
        setupAsyncClient();
    else
        setupClient();
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
}

void MqttPublisher::stop() {
    // Disconnect
    std::cout << "Disconnecting..." << std::flush;
    if(client != nullptr) {
        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
    } else if(asyncClient != nullptr) {
        MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
        int rc;

        opts.onSuccess = onDisconnect;
        opts.context = client;

        if ((rc = MQTTAsync_disconnect(asyncClient, &opts)) != MQTTASYNC_SUCCESS)
        {
            printf("Failed to start sendMessage, return code %d\n", rc);
        }
        MQTTAsync_destroy(&asyncClient);
    }
    client = nullptr;
    asyncClient = nullptr;
    std::cout << "OK" << std::endl;
}

extern "C" void onSend(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;
#ifdef PRINT_DEBUG
    printf("Message with token value %d delivery confirmed\n", response->token);
#endif
}
extern "C" void onSendFailure(void* context,  MQTTAsync_failureData* response) {
    printf("delivery failed, token: %d \n", response->token);
}

void MqttPublisher::queueForPublish(string topicName, MqttQOS qos, byte *payload, size_t len) {
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = asyncClient;

    pubmsg.payload = payload;
    pubmsg.payloadlen = len;
    pubmsg.qos = qos;
    pubmsg.retained = 0;
    if ((rc = MQTTAsync_sendMessage(asyncClient, topicName.c_str(), &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start sendMessage, return code %d\n", rc);
    } else {
#ifdef PRINT_DEBUG
        printf("MQTTAsync_sendMessage\n");
#endif
    }

}
void MqttPublisher::publish(string topicName, MqttQOS qos, byte *payload, size_t len) {
    if(asyncMode) {
        queueForPublish(topicName, qos, payload, len);
        return;
    }

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
            timeout/1000, "beacon", destinationName.c_str(), clientID.c_str());
#endif
    int rc = MQTTClient_waitForCompletion(client, token, timeout);
    if(rc != 0)
        printf("Failed to start sendMessage, return code %d\n", rc);
}

void MqttPublisher::setupClient() {
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

extern "C" void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response ? response->code : 0);
}

extern "C" void onConnect(void* context, MQTTAsync_successData* response)
{
    printf("onConnect\n");
}

extern "C" void onConnectionLost(void *context, char *cause)
{
    MQTTAsync asyncClient = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    printf("Connection lost, cause: %s\n", cause);
    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(asyncClient, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
    }
}

void MqttPublisher::setupAsyncClient() {
    MQTTAsync_create(&asyncClient, brokerURL.c_str(), clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    MQTTAsync_setCallbacks(client, NULL, onConnectionLost, NULL, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(asyncClient, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
    }
}