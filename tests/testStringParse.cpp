//
// Created by Scott Stark on 7/24/15.
//

#include <iostream>
#include <string>
#include <utility>

using namespace std;

std::pair<string,string> splitUser(const string& pair) {
    size_t equals = pair.find("=");
    std::pair<string,string> values(pair.substr(0, equals), pair.substr(equals+1));
    return values;
}
int main() {
    size_t last = 0;
    size_t next = 0;
    string mappings("id1=user1,id2=user2,id3=user3");
    while ((next = mappings.find(",", last)) != std::string::npos) {
        string pair = mappings.substr(last, next-last);
        std::pair<string,string>  info = splitUser(pair);
        std::cout << info.first << ';' << info.second << std::endl;
        last = next + 1;
    }
    std::pair<string,string>  info = splitUser(mappings.substr(last));
    std::cout << info.first << ';' << info.second << std::endl;
}