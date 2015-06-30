//
// Created by Scott Stark on 6/29/15.
//

#ifndef NATIVESCANNER_ABSTRACTBEACONMAPPER_H
#define NATIVESCANNER_ABSTRACTBEACONMAPPER_H
#include <string>
using namespace std;

/**
 * No op base class for BeaconMapper that allows one to avoid dependency on rest/json libs used by the
 * BeaconMapper.cpp implementation
 *
 */
class AbstractBeaconMapper {
public:
    /**
     * Query the current user registrations to update the beacon minorID to name mappings
     */
    virtual void refresh() {}

    virtual string lookupUser(int minorID) {
        return "Unknown";
    }
};
#endif //NATIVESCANNER_ABSTRACTBEACONMAPPER_H
