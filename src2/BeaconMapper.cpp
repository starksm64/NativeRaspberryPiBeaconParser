#include "BeaconMapper.h"
#include <restclient-cpp/restclient.h>
#include <json/json.h>

void BeaconMapper::refresh() {
    beaconToUser.clear();
    // Query the registration api
    RestClient::response r = RestClient::get("https://summitdemo-540ty4j5jnfp0dusuik5kldm-rht-summit-prod.mbaas2.rht.feedhenry.com/registration");
    if(r.code != 200) {
        fprintf(stderr, "Failed to query the registration api, code=%d\n", r.code);
        return;
    }
    Json::Value deserializeRoot;
    Json::Reader reader;

    if( !reader.parse(r.body, deserializeRoot) )
        fprintf(stderr, "Failed to parse json body: %s\n", r.body.c_str());

    Json::ArrayIndex count = deserializeRoot.size();
    printf("Registration response has %d elements\n", count);
    for(Json::ArrayIndex n = 0; n < count; n ++) {
        Json::Value value = deserializeRoot[n];
        Json::Value fields = value["fields"];
        const char* minorIDCstr = fields["beaconId"].asCString();
        int minorID = std::stoi(minorIDCstr);
        string name = fields["name"].asString().c_str();
        beaconToUser[minorID] = name;
        printf("[%d]: beaconId: %d, name: %s\n", n, minorID, name.c_str());
    }
}

string BeaconMapper::lookupUser(int minorID) {
    string user = beaconToUser[minorID];
    return user;
}
