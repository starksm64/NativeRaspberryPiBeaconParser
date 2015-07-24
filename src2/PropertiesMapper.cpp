//
// Created by Scott Stark on 7/24/15.
//

#include "PropertiesMapper.h"

string PropertiesMapper::lookupUser(int minorID) {
    string key = std::to_string(minorID);
    string user("Unknown");
    if(mapping.find(key) != mapping.end())
        user = mapping[key];
    return user;
}
