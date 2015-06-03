#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void getMACAddress(std::string _iface,unsigned char MAC[6]) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , _iface.c_str() , IFNAMSIZ-1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    for(unsigned int i=0;i<6;i++)
        MAC[i] = ifr.ifr_hwaddr.sa_data[i];
    close(fd);
}

int main() {
    char hostIPAddress[128];
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }
    for (ifa = ifaddr, n = 0; ifa != nullptr; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == nullptr)
            continue;
        family = ifa->ifa_addr->sa_family;
        printf("%s, %d\n", ifa->ifa_name, family);
        if(family == AF_INET) {
            char tmp[sizeof(hostIPAddress)];
            int err = getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), tmp, sizeof(tmp), nullptr, 0, NI_NUMERICHOST);
            if(err != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
            }
            printf("Found hostIPAddress of: %s\n", tmp);
            if(strncasecmp(tmp, "127.0", 5) == 0) {
                printf("Skipping localhost address\n");
                continue;
            }
            strcpy(hostIPAddress, tmp);
            unsigned char MAC[6];
            char macaddr[16];
            getMACAddress(ifa->ifa_name, MAC);
            sprintf(macaddr, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
            printf("MAC: %s\n", macaddr);
        }
    }

    freeifaddrs(ifaddr);

}