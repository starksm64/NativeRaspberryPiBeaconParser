#include <iostream>
#include "proton/message.h"
#include "proton/messenger.h"
#include "../src/Beacon.h"

using namespace std;


void die(const char *file, int line, const char *message) {
    fprintf(stderr, "%s:%i: %s\n", file, line, message);
    exit(1);
}

inline void check(pn_messenger_t *messenger) {
    if(pn_messenger_errno(messenger)) {
      die(__FILE__, __LINE__, pn_error_text(pn_messenger_error(messenger)));
    }
}

/*
 * Testing sending a message to a topic using the proton api
 */
int main(int argc, char **argv) {

    pn_message_t *message;
    pn_messenger_t *messenger;
    pn_data_t *body;
    const char *brokerURL = "amqp://192.168.1.107:5672/topic://beaconEvents";


    messenger = pn_messenger(NULL);

    pn_messenger_start(messenger);
    pn_messenger_subscribe(messenger, brokerURL);

    while(true) {
        pn_messenger_recv(messenger, 1024);
        check(messenger);

        while(pn_messenger_incoming(messenger))
        {
            pn_messenger_get(messenger, message);
            check(messenger);
            {
                pn_data_t *properties = pn_message_properties(message);
                char buffer[1024];
                size_t buffsize = sizeof(buffer);
                const char* subject = pn_message_get_subject(message);
                pn_data_t *body = pn_message_properties(message);
                pn_data_format(body, buffer, &buffsize);

                printf("Address: %s\n", pn_message_get_address(message));
                printf("Subject: %s\n", subject ? subject : "(no subject)");
                printf("Content: %s\n", buffer);
            }
        }
    }
}
