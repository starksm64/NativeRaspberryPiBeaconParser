#include "../socket/PracticalSocket.h"
#include "../src2/HealthStatus.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ByteBuffer.h>

int main() {

    try {
        UDPSocket bcastSocket;
        HealthStatus healthStatus;
        StatusInformation status;
        status.setScannerID("testStatusBcast");
        healthStatus.calculateStatus(status);
        ByteBuffer buffer;
        vector<StatusProperties> enums;
        enums.push_back(StatusProperties::HostIPAddress);
        enums.push_back(StatusProperties::MACAddress);
        enums.push_back(StatusProperties::ScannerID);
        enums.push_back(StatusProperties::LoadAverage);
        enums.push_back(StatusProperties::MemTotal);
        enums.push_back(StatusProperties::MemFree);
        enums.push_back(StatusProperties::SystemTime);
        enums.push_back(StatusProperties::SystemType);
        enums.push_back(StatusProperties::SystemUptime);
        enums.push_back(StatusProperties::Uptime);
        vector<string> names = HealthStatus::getStatusPropertyNames(enums);
        status.encodeLastStatus(buffer, names);
        vector<uint8_t>& data = buffer.getData();
        bcastSocket.sendTo(data.data(), data.size(), "192.168.1.255", 12345);
        printf("Sent bcast to 192.168.1.255, size=%d\n", data.size());
    } catch (SocketException &e) {
        cerr << e.what() << endl;
        exit(1);
    }
}
