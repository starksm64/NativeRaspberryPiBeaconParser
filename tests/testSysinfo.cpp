//
// Created by starksm on 4/4/15.
//


#include <stdio.h>
#include <string>
#include <sys/sysinfo.h>

int main(int argc, char **argv) {
    struct sysinfo info;
    if(sysinfo(&info)) {
        perror("Failed to read sysinfo");
        return -1;
    }
    int mb = 1024*1024;
    int days = info.uptime / (24*3600);
    int hours = (info.uptime - days * 24*3600) / 3600;
    int minute = (info.uptime - days * 24*3600 - hours*3600) / 60;
    char uptime[128];
    sprintf(uptime, "uptime: %ld, days:%d, hrs: %d, min: %d", info.uptime, days, hours, minute);
    printf("%s\n", uptime);
    printf("load average: %.2f, %.2f, %.2f\n", info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0);
    printf("mem_unit=%d\n", info.mem_unit);
    printf("Procs: %d\n", info.procs);
    printf("TotalRam: %ldMb\n", info.totalram*info.mem_unit / mb);
    printf("AvailableRam: %ldMb\n", info.freeram*info.mem_unit / mb);
    printf("FreeHigh: %ldMb\n", info.freehigh*info.mem_unit / mb);
    printf("TotalHigh: %ldMb\n", info.totalhigh*info.mem_unit / mb);
    printf("SharedRam: %ldMb\n", info.sharedram*info.mem_unit / mb);
    printf("FreeSwap: %ldMb\n", info.freeswap*info.mem_unit / mb);
    printf("TotalSwap: %ldMb\n", info.totalswap*info.mem_unit / mb);

    printf("--- Unscaled:\n");
    printf("TotalRam: %ld\n", info.totalram*info.mem_unit);
    printf("AvailableRam: %ld\n", info.freeram*info.mem_unit);
    printf("FreeHigh: %ld\n", info.freehigh*info.mem_unit);
    printf("TotalHigh: %ld\n", info.totalhigh*info.mem_unit);
    printf("SharedRam: %ld\n", info.sharedram*info.mem_unit);
    printf("FreeSwap: %ld\n", info.freeswap*info.mem_unit);
    printf("TotalSwap: %ld\n", info.totalswap*info.mem_unit);

    printf("--- to_string:\n");
    printf("TotalRam: %s\n", std::to_string(info.totalram*info.mem_unit / mb).c_str());
    printf("AvailableRam: %s\n", std::to_string(info.freeram*info.mem_unit / mb).c_str());

    return 0;
}