//
// Created by Scott Stark on 6/12/15.
//

#ifndef NATIVESCANNER_BEACONMAPPER_H
#define NATIVESCANNER_BEACONMAPPER_H

#include <map>
#include <string>

using namespace std;

/**
 * Map a beacon id to the registered user by querying the application registration rest api
 */
class BeaconMapper {
private:
    map<int, string> beaconToUser;

public:
    /**
     * Query the current user registrations to update the beacon minorID to name mappings
     */
    void refresh();

    string lookupUser(int minorID);
};


#endif //NATIVESCANNER_BEACONMAPPER_H
