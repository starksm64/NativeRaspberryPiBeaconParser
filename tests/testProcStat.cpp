//
// Created by starksm on 3/30/15.
//

/* Show per core CPU utilization of the system
 * This is a part of the post http://phoxis.org/2013/09/05/finding-overall-and-per-core-cpu-utilization
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#define BUF_MAX 1024
#define MAX_CPU 128

using namespace std;

enum class ProcStatFields {
    User,
    Nice,
    System,
    Idle,
    Iowait,
    Irq,
    Softirq,
    Steal,
    Quest,
    QuestNice,
    COUNT
};

int getCpuCount() {
    FILE *cpuinfo  = fopen ("/proc/cpuinfo", "r");
    if (cpuinfo == nullptr)  {
        perror ("/proc/cpuinfo");
    }
    char tmp[80];
    int count = 0;
    while(!fgets(tmp, sizeof(tmp), cpuinfo)) {
        if(strncmp(tmp, "processor", 9))
            count ++;
    }
    fclose(cpuinfo);
    return count;
}

int read_fields (FILE *fp, unsigned long long int *fields)
{
    int retval;
    char buffer[BUF_MAX];


    if (!fgets (buffer, BUF_MAX, fp))
    { perror ("Error"); }
    /* line starts with c and a string. This is to handle cpu, cpu[0-9]+ */
    retval = sscanf (buffer, "c%*s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                     &fields[0],
                     &fields[1],
                     &fields[2],
                     &fields[3],
                     &fields[4],
                     &fields[5],
                     &fields[6],
                     &fields[7],
                     &fields[8],
                     &fields[9]);
    if (retval == 0)
    { return -1; }
    if (retval < 4) /* Atleast 4 fields is to be read */
    {
        fprintf (stderr, "Error reading /proc/stat cpu field\n");
        return 0;
    }
    return 1;
}

int getCpuUsage(FILE *procStat, vector<double>& usage) {
    unsigned long long int fields[10], total_tick[MAX_CPU], total_tick_old[MAX_CPU], idle[MAX_CPU], idle_old[MAX_CPU], del_total_tick[MAX_CPU], del_idle[MAX_CPU];
    int update_cycle = 0, i, cpus = 0, count;
    double percent_usage;


    while (read_fields(procStat, fields) != -1) {
        for (i = 0, total_tick[cpus] = 0; i < 10; i++) { total_tick[cpus] += fields[i]; }
        idle[cpus] = fields[3]; /* idle ticks index */
        cpus++;
    }

    sleep(1);
    fseek(procStat, 0, SEEK_SET);
    fflush(procStat);
    for (count = 0; count < cpus; count++) {
        total_tick_old[count] = total_tick[count];
        idle_old[count] = idle[count];

        if (!read_fields(procStat, fields)) { return cpus; }

        for (i = 0, total_tick[count] = 0; i < 10; i++) { total_tick[count] += fields[i]; }
        idle[count] = fields[3];

        del_total_tick[count] = total_tick[count] - total_tick_old[count];
        del_idle[count] = idle[count] - idle_old[count];

        percent_usage = ((del_total_tick[count] - del_idle[count]) / (double) del_total_tick[count]) * 100;
        usage.push_back(percent_usage);
    }
    return cpus;
}

void getCpuUsageLoop(FILE *procStat) {
    vector<double> usage;
    while (1)
    {
        sleep (1);
        getCpuUsage(procStat, usage);
        printf ("Total CPU Usage: %3.2lf%%\n", usage[0]);
        for(int n = 1; n < usage.size(); n ++) {
            printf ("\tCPU%d Usage: %3.2lf%%\n", n-1, usage[n]);
        }
        printf ("\n");
        usage.clear();
    }
}

int main (void)
{
    FILE *procStat;
    FILE *procMeminfo;
    FILE *procLoadavg;

    int cpuCount = getCpuCount();
    printf("Found %d CPUs\n", cpuCount);
    procStat = fopen ("/proc/stat", "r");
    if (procStat == NULL) {
        perror ("Error");
    }
    getCpuUsageLoop(procStat);


    /* Ctrl + C quit, therefore this will not be reached. We rely on the kernel to close this file */
    fclose (procStat);

    return 0;
}
