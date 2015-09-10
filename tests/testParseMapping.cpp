#include <map>
#include <string>
#include <utility>

using namespace std;
using Properties = map<string, string>;

int main (void)
{
    string mappings("300=Scott,301=Tony");
        size_t last = 0;
        size_t next = 0;
        printf("PropertiesMapper():\n");
        while ((next = mappings.find(",", last)) != std::string::npos) {
            string pair = mappings.substr(last, next-last);
            std::pair<string,string> info = splitUser(pair);
            mapping[info.first] = info.second;
            printf("\t%s=%s\n", info.first.c_str(), info.second.c_str());
            last = next + 1;
        }
        std::pair<string,string> info = splitUser(mappings.substr(last));
        mapping[info.first] = info.second;
}