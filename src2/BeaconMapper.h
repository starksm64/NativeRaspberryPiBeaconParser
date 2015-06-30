//
// Created by Scott Stark on 6/12/15.
//

#ifndef NATIVESCANNER_BEACONMAPPER_H
#define NATIVESCANNER_BEACONMAPPER_H

#include <map>
#include <string>
#include "AbstractBeaconMapper.h"

using namespace std;

/**
 * Map a beacon id to the registered user by querying the application registration rest api
 *
 * Relies on:
 * git clone https://github.com/open-source-parsers/jsoncpp.git
 * cd jsoncpp/
 * mkdir build
 * cd build/
 * cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON ..
 * make
 * make install
 *
 * git clone https://github.com/mrtazz/restclient-cpp.git
 * apt-get install autoconf
 * apt-get install libtool
 * apt-get install libcurl4-openssl-dev
 * cd restclient-cpp/
 * ./autogen.sh
 * ./configure
 * make
 * make install
 */
class BeaconMapper : public AbstractBeaconMapper {
private:
    map<int, string> beaconToUser;

public:
    /**
     * Query the current user registrations to update the beacon minorID to name mappings
     */
    virtual void refresh();

    virtual string lookupUser(int minorID);
};


#endif //NATIVESCANNER_BEACONMAPPER_H
