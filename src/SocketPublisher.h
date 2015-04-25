//
// Created by Scott Stark on 4/15/15.
//

#ifndef NATIVESCANNER_SOCKETPUBLISHER_H
#define NATIVESCANNER_SOCKETPUBLISHER_H


#include "MsgPublisher.h"
#include "ByteBuffer.h"
#include <memory>

class TCPSocket;

/**
 * A simple implementation of the MsgPublisher that sends binary data over a clientSocket using the ByteBuffer class.
 */
class SocketPublisher : public MsgPublisher {
    unique_ptr<TCPSocket> clientSocket;
    ByteBuffer buffer;
    int64_t nextReconnectTime = 0;
    vector<vector<byte>> backlog;

protected:
    void doSend(const vector<byte>& msg, int64_t now);
    void calculateReconnectTime(int64_t now);

    int64_t getNextReconnectTime() const {
        return nextReconnectTime;
    }
    void setNextReconnectTime(int64_t nextReconnectTime) {
        SocketPublisher::nextReconnectTime = nextReconnectTime;
    }
    bool shouldReconnect(int64_t now) {
        return now >= nextReconnectTime;
    }

public:

    SocketPublisher(string brokerUrl, string clientID, string userName, string password);

    bool isConnected() const {
        return connected;
    }

    virtual ~SocketPublisher();

    virtual void start(bool asyncMode);

    virtual void stop();

    virtual void queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len);

    virtual void publish(string const &destinationName, Beacon &beacon);

    virtual void publish(vector<Beacon> events);

    virtual void publishStatus(Beacon &beacon);

    virtual void publishProperties(string const &destinationName, map<string, string> const &properties);
};


#endif //NATIVESCANNER_SOCKETPUBLISHER_H
