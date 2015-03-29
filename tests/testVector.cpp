//
// Created by starksm on 3/29/15.
//

#include <map>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

using namespace std;

void toTimeWindowString(vector<char>& output) {
    char timestr[256];
    char *ptr = timestr;
    struct timeval tv;
    tv.tv_sec = time(nullptr);
    tv.tv_usec = 0;
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    int count = strftime(timestr, 128, "%R:%S", &tm);
    output.insert(output.end(), ptr, ptr+count);
    ptr += count;
    tv.tv_sec += 5;
    localtime_r(&tv.tv_sec, &tm);
    count = strftime(ptr, 128, "-%R:%S", &tm);
    output.insert(output.end(), ptr, ptr+count);
    output.push_back('\0');
}

int main() {
    vector<char> output;

    char timestr[256];
    char tmp[256];
    char *ptr = tmp;
    struct timeval tv;
    tv.tv_sec = time(nullptr);
    tv.tv_usec = 0;
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    strftime(timestr, sizeof(timestr), "%r", &tm);
    int count = snprintf(tmp, sizeof(tmp), "+++ Beacon counts for window(%ld,%d): %s\n", 100, 5, timestr);
    output.insert(output.end(), ptr, ptr+count);
    output.push_back('\t');
    map<int32_t, int32_t> data;
    for(int n = 0; n < 100; n ++) {
        data[n] = n*n;
    }
    map<int32_t, int32_t >::const_iterator iter = data.begin();
    while(iter != data.end()) {
        count = snprintf(tmp, sizeof(tmp), "+%d: %2d; ", iter->first, iter->second);
        output.insert(output.end(), ptr, ptr+count);
        iter ++;
    }
    printf("output=%s\n", output.data());

    output.resize(0);
    //output.push_back('\0');
    toTimeWindowString(output);
    printf("toTimeWindowString=%s\n", output.data());
}