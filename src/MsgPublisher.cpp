#include "MsgPublisher.h"
#include "MqttPublisher.h"
#include "ProtonPublisher.h"
#include "CMSPublisher.h"
#include "QpidPublisher.h"

void MsgPublisherFactory::create(MsgPublisher& newPublisher, MsgPublisherType type, string brokerUrl, string clientID, string userName, string password) {

    switch (type) {
        case PAHO_MQTT:
        {
            MqttPublisher publisher(brokerUrl, clientID, userName, password);
            newPublisher = publisher;
            break;
        }
        case AMQP_PROTON:
        {
            ProtonPublisher publisher(brokerUrl, clientID, userName, password);
            newPublisher = publisher;
            break;
        }
        case AMQP_CMS:
        {
            CMSPublisher publisher(brokerUrl, clientID, userName, password);
            printf("Created CMSPublisher instance\n");
            newPublisher = publisher;
            break;
        }
        case AMQP_QPID:
        {
            QpidPublisher publisher(brokerUrl, clientID, userName, password);
            printf("Created QpidPublisher instance\n");
            newPublisher = publisher;
            break;
        }
    }
}
