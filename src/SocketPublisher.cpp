//
// Created by Scott Stark on 4/15/15.
//

#include "SocketPublisher.h"
#include "../socket/PracticalSocket.h"

SocketPublisher::SocketPublisher(string brokerUrl, string clientID, string userName, string password) : MsgPublisher(
        brokerUrl, clientID, userName, password) {

}

SocketPublisher::~SocketPublisher() {

}

void SocketPublisher::start(bool asyncMode) {
    if(asyncMode)
        printf("Ignoring asyncMode for now\n");

    int port = 12345;
    string host;
    size_t colon = brokerUrl.find(':');
    if(colon > 0) {
        size_t colon = brokerUrl.find(':');
        host = brokerUrl.substr(0, colon);
        string portStr = brokerUrl.substr(colon+1, brokerUrl.size());
        port = stoi(portStr.c_str());
    } else {
        host = brokerUrl;
    }

    printf("Connecting to: %s, %d\n", host.c_str(), port);
    TCPSocket *client = new TCPSocket(host, port);
    clientSocket.reset(client);
}

void SocketPublisher::stop() {
    if(clientSocket) {
        clientSocket->close();
    }
}

void SocketPublisher::queueForPublish(string const &destinationName, MqttQOS qos, byte *payload, size_t len) {

}

void SocketPublisher::publish(string const &destinationName, MqttQOS qos, byte *payload, size_t len) {

}

void SocketPublisher::publish(vector<Beacon> events) {

}

void SocketPublisher::publish(string const &destinationName, Beacon &beacon) {
    vector<byte> msg = beacon.toByteMsg();
    buffer.clear();
    buffer.writeBytes(destinationName);
    buffer.writeInt(msg.size());
    vector<byte> prefix = buffer.getData();
    msg.insert(msg.begin(), prefix.begin(), prefix.end());
    clientSocket->send(msg.data(), msg.size());
}

void SocketPublisher::publishStatus(Beacon &beacon) {
    beacon.setMessageType(BeconEventType::SCANNER_HEARTBEAT);
    publish("beaconEvents", beacon);
}

void SocketPublisher::publishProperties(string const &destinationName, map<string, string> const &properties) {
    buffer.clear();
    buffer.writeBytes(destinationName);
    // Save room for the msg size
    uint32_t sizePos = buffer.reserveBytes(4);
    uint32_t start = buffer.getWritePos();
    // Write the key count
    buffer.writeInt(properties.size());
    for(map<string,string>::const_iterator iter = properties.begin(); iter != properties.end(); iter ++) {
        buffer.writeBytes(iter->first);
        buffer.writeBytes(iter->second);
    }
    uint32_t end = buffer.getWritePos();
    // Write the length of th properties data
    buffer.setWritePos(sizePos);
    buffer.writeInt(end - start);
    vector<byte> msg = buffer.getData();
    clientSocket->send(msg.data(), msg.size());
}
