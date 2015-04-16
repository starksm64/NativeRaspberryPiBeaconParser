#include <stdio.h>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    int port;
    char host[64];

    int count = sscanf("192.168.1.107:12345", "%s:%d", host, &port);
    printf("Parsed(%d) host: %s, port: %d\n", count, host, port);

    string brokerURL("192.168.1.107:12345");
    size_t colon = brokerURL.find(':');
    string hostip = brokerURL.substr(0, colon);
    string portStr = brokerURL.substr(colon+1, brokerURL.size());
    printf("hostip: %s, portStr: %s\n", hostip.c_str(), portStr.c_str());

    port = stoi(portStr.c_str());
    printf("Parsed host: %s, port: %d\n", hostip.c_str(), port);
}
