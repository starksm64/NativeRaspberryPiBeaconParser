//
// Created by Scott Stark on 7/24/15.
//

#ifndef NATIVESCANNER_PROPERTIESMAPPER_H
#define NATIVESCANNER_PROPERTIESMAPPER_H

#include <map>
#include <string>
#include <utility>

#include "AbstractBeaconMapper.h"

using namespace std;
using Properties = map<string, string>;

class PropertiesMapper : public AbstractBeaconMapper {
private:
    Properties mapping;
    std::pair<string,string> splitUser(const string& pair) {
        size_t equals = pair.find("=");
        std::pair<string,string> values(pair.substr(0, equals), pair.substr(equals+1));
        return values;
    }
public:
    /**
     * The value is a series of id1=userid1,id2=userid2,... values
     */
    PropertiesMapper(const string& mappings) {
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
        printf("\t%s=%s\n", info.first.c_str(), info.second.c_str());
    }


    virtual string lookupUser(int minorID) override;
};


#endif //NATIVESCANNER_PROPERTIESMAPPER_H
