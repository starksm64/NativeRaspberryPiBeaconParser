#include "MsgPublisher.h"
#include "MqttPublisher.h"
#include "ProtonPublisher.h"
#include "CMSPublisher.h"

MsgPublisher* MsgPublisher::create(MsgPublisherType type, string brokerUrl, string clientID, string userName, string password) {
    MsgPublisher* publisher = nullptr;
    switch (type) {
        case PAHO_MQTT:
            publisher = new MqttPublisher(brokerUrl, clientID, userName, password);
            break;
        case AMQP_PROTON:
            publisher = new ProtonPublisher(brokerUrl, clientID, userName, password);
            break;
        case AMQP_CMS:
            publisher = new CMSPublisher(brokerUrl, clientID, userName, password);
            printf("Created CMSPublisher instance\n");
            break;
    }
    return publisher;
}
