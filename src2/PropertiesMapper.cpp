//
// Created by Scott Stark on 7/24/15.
//

#include "PropertiesMapper.h"

string PropertiesMapper::lookupUser(int minorID) {
    string key = std::to_string(minorID);
    string user("Unknown");
    bool found = false;
    if(mapping.find(key) != mapping.end()) {
        user = mapping[key];
        found = false;
    }
#ifdef PRINT_DEBUG
    printf("lookupUser(%d) = %s\n", minorID, user);
#endif
    return user;
}
