#include <iostream>
#include <chrono>

using namespace std::chrono;

int main()
{
    high_resolution_clock::time_point p = high_resolution_clock::now();
    std::cout << "high_resolution_clock: " << p.time_since_epoch().count() << std::endl;
    milliseconds ms = duration_cast<milliseconds>(p.time_since_epoch());
    std::cout << "high_resolution_clock; milliseconds since epoch: " << ms.count() << std::endl;
    int64_t years = 2015 - 1970;
    int64_t days = 107;
    int64_t hours = 21;
    int64_t mins = 45;
    int64_t estimate = years*365*24*3600*1000 + days*24*3600*1000 + hours*3600*1000 + mins*60*1000;
    std::cout << "estimated milliseconds since epoch: " << estimate << std::endl;
    seconds s = duration_cast<seconds>(ms);
    std::time_t t = s.count();
    std::cout << "seconds since epoch: " << t << std::endl;

    std::cout << std::ctime(&t) << std::endl;

    system_clock::time_point p2 = system_clock::now();
    std::time_t today_time = system_clock::to_time_t(p2);
    std::cout << "today: " << ctime(&today_time) << std::endl;

    ms = duration_cast<milliseconds>(p2.time_since_epoch());
    std::cout << "system_clock; milliseconds since epoch: " << ms.count() << std::endl;
}
