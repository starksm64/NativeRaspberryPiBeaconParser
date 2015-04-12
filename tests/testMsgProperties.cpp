#include <map>
#include <stdio.h>
#include <string>
#include <sys/sysinfo.h>
#include <qpid/messaging/Message.h>
#include <iostream>

using namespace qpid;
using namespace std;

int main(int argc, char **argv) {
    struct sysinfo info;
    if(sysinfo(&info)) {
        perror("Failed to read sysinfo");
        return -1;
    }

    int mb = 1024*1024;
    messaging::Message message;
    map<string, string> statusProperties;
    string MemTotal("MemTotal");
    string MemActive("MemActive");
    string MemFree("MemFree");
    statusProperties[MemTotal] = to_string(info.totalram*info.mem_unit / mb);
    statusProperties[MemActive] = to_string((info.totalram - info.freeram)*info.mem_unit / mb);
    statusProperties[MemFree] = to_string(info.freeram*info.mem_unit / mb);
    printf("TotalRam: %s\n", statusProperties[MemTotal].c_str());
    printf("MemActive: %s\n", statusProperties[MemActive].c_str());
    printf("MemFree: %s\n", statusProperties[MemFree].c_str());

    for(map<string, string>::const_iterator iter = statusProperties.begin(); iter != statusProperties.end(); iter ++) {
        message.setProperty(iter->first, iter->second);
    }
    cout << message.getProperties() << endl;
}
