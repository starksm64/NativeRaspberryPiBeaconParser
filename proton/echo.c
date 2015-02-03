/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "proton/message.h"
#include "proton/messenger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#define check(messenger)                                                     \
  {                                                                          \
    if(pn_messenger_errno(messenger))                                        \
    {                                                                        \
      die(__FILE__, __LINE__, pn_error_text(pn_messenger_error(messenger))); \
    }                                                                        \
  }                                                                          \

void die(const char *file, int line, const char *message)
{
    fprintf(stderr, "%s:%i: %s\n", file, line, message);
    exit(1);
}

void usage(void)
{
    printf("Usage: echo [options] <addr>\n");
    printf("-c    \tPath to the certificate file.\n");
    printf("-k    \tPath to the private key file.\n");
    printf("-p    \tPassword for the private key.\n");
    printf("addr  \tThe target address [amqp[s]://domain[/name]]\n");
    exit(0);
}

int main(int argc, char** argv)
{
    char* certificate = NULL;
    char* privatekey = NULL;
    char* password = NULL;
    int c;
    char * address = (char *) "amqp://192.168.1.107:5672/echo";
    char * msgtext = (char *) "Hello World!";
    opterr = 0;

    while((c = getopt(argc, argv, "h")) != -1)
    {
        switch(c)
        {
            case 'h': usage(); break;

            case 'c': certificate = optarg; break;
            case 'k': privatekey = optarg; break;
            case 'p': password = optarg; break;

            case '?':
                if(optopt == 'c' ||
                        optopt == 'k' ||
                        optopt == 'p')
                {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if(isprint(optopt))
                {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                else
                {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }

    if (optind < argc)
    {
        address = argv[optind];
    }

    {
        pn_message_t * messageSend;
        pn_message_t * messageRecv;
        pn_messenger_t * messengerSend;
        pn_messenger_t * messengerRecv;
        pn_data_t * body;

        /* load the various command line options if they're set */
        if(certificate)
        {
            pn_messenger_set_certificate(messengerSend, certificate);
            pn_messenger_set_certificate(messengerRecv, certificate);
        }

        if(privatekey)
        {
            pn_messenger_set_private_key(messengerSend, privatekey);
            pn_messenger_set_private_key(messengerRecv, privatekey);
        }

        if(password)
        {
            pn_messenger_set_password(messengerSend, password);
            pn_messenger_set_password(messengerRecv, password);
        }

        messageSend = pn_message();
        messageRecv = pn_message();
        messengerSend = pn_messenger(NULL);
        messengerRecv = pn_messenger(NULL);

        // Give the outgoing message some properties
        pn_data_t *properties = pn_message_properties(messageSend);
        pn_data_put_map(properties);

        char * keyFirst = "First";
        char * keySecond = "Second";

        pn_data_enter(properties);
        pn_data_put_string(properties, pn_bytes(strlen(keyFirst), keyFirst));
        pn_data_put_int(   properties, 1);
        pn_data_put_string(properties, pn_bytes(strlen(keySecond), keySecond));
        pn_data_put_int(   properties, 2222);
        pn_data_exit(properties);

        pn_messenger_start(messengerSend);
        check(messengerSend);
        pn_messenger_start(messengerRecv);
        check(messengerRecv);

        pn_message_set_address(messageSend, address);
        body = pn_message_body(messageSend);
        pn_data_put_string(body, pn_bytes(strlen(msgtext), msgtext));

        printf("Echo subscribing to address : %s\n", address);
        pn_messenger_subscribe(messengerRecv, address);
        check(messengerRecv);

        int i;
        for (i=0; i<1000; ++i)
        {
            pn_messenger_put(messengerSend, messageSend);
            check(messengerSend);

            pn_messenger_send(messengerSend, -1);
            check(messengerSend);

            pn_messenger_recv(messengerRecv, 1024);
            check(messengerRecv);

            while(pn_messenger_incoming(messengerRecv))
            {
                pn_messenger_get(messengerRecv, messageRecv);
                check(messengerRecv);

                if (messageRecv != 0)
                {
                    char buffer[1024];
                    size_t buffsize = sizeof(buffer);
                    const char * subject = pn_message_get_subject(messageRecv);
                    pn_data_t * bodyRecv = pn_message_body(messageRecv);
                    pn_data_format(bodyRecv, buffer, &buffsize);

                    printf("Message: %d, Address: %s, Subject: %s, Content: %s\n",
                            i, pn_message_get_address(messageRecv),
                            (subject ? subject : "(no subject)"),
                            buffer);
                    // Print received properties
                    pn_data_t *properties = pn_message_properties(messageRecv);
                    pn_data_next(properties);
                    // assert that it's a map
                    assert(pn_data_type(properties) == PN_MAP);

                    {
                        size_t mapsize = pn_data_get_map(properties);
                        pn_data_enter(properties);
                        int x=0;
                        for(x=0; x<mapsize/2; x++)
                        {
                            pn_data_next(properties);
                            assert(pn_data_type(properties) == PN_STRING);
                            pn_bytes_t string  = pn_data_get_string(properties);

                            pn_data_next(properties);
                            assert(pn_data_type(properties) == PN_INT);
                            int integer = pn_data_get_int(properties);

                            printf("%*s, %i\n", string.size, string.start, integer);
                        }
                        pn_data_exit(properties);
                    }
                }
            }
            sleep(5);
        }

        pn_messenger_stop(messengerSend);
        pn_messenger_free(messengerSend);
        pn_messenger_stop(messengerRecv);
        pn_messenger_free(messengerRecv);
        pn_message_free(messageSend);
        pn_message_free(messageRecv);
    }

    return 0;
}
