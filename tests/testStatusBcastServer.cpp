#include <ByteBuffer.h>
#include <iostream>            // For cout and cerr
#include "../socket/PracticalSocket.h"

const int MAXDATA = 65534;

int main(int argc, char *argv[]) {

    unsigned short echoServPort = 12345;

    try {
        UDPSocket sock(echoServPort);
        uint8_t data[MAXDATA];
        string sourceAddress;              // Address of datagram source
        unsigned short sourcePort;         // Port of datagram source
        int bytesRcvd = sock.recvFrom(data, MAXDATA, sourceAddress, sourcePort);
        while(bytesRcvd > 0) {
            ByteBuffer buffer(data, bytesRcvd);
            int count = buffer.readInt();
            printf("Received buffer with %d properties\n", count);
            for (int n = 0; n < count; n++) {
                string name = buffer.readString();
                string value = buffer.readString();
                printf("%s=%s\n", name.c_str(), value.c_str());
            }
            bytesRcvd = sock.recvFrom(data, MAXDATA, sourceAddress, sourcePort);
        }
    } catch (SocketException &e) {
        cerr << e.what() << endl;
        exit(1);
    }

    return 0;
}