
#include <restclient-cpp/restclient.h>
#include "json/json.h"

/*
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
 * cd restclient-cpp/
 * ./autogen.sh
 * ./configure
 * make
 * make install
 */
// https://summitdemo-540ty4j5jnfp0dusuik5kldm-rht-summit-prod.mbaas2.rht.feedhenry.com/registration

int main() {
    RestClient::response r = RestClient::get("https://summitdemo-540ty4j5jnfp0dusuik5kldm-rht-summit-prod.mbaas2.rht.feedhenry.com/registration");
    printf("Request response code=%d\n", r.code);
    printf("Request response body=%s\n", r.body.c_str());

    /*
    RestClient::response r2 = RestClient::get("https://summitdemo-540ty4j5jnfp0dusuik5kldm-rht-summit-prod.mbaas2.rht.feedhenry.com/registration?beaconId=50");
    printf("Request response code=%d\n", r2.code);
    printf("Request response body=%s\n", r2.body.c_str());
    */

    /* Example response body:
     [
     {"type":"registration","guid":"55759d9fb787bfc10500af4e","fields":{"beaconId":"50","name":"Burr Sutter","showName":true,"__fh":{"cuid":"49DA907BA0AC4862B86A181FA1E76134","cuidMap":null,"destination":"iphone","sdk_version":"FH_JS_SDK/2.6.0+21","appid":"dbmxt6f5wnnvwzsaljx6qvxs","appkey":"378c73d8b74852439f8408181ec571dc3b486212","projectid":"54_tyE_DE2NMHNm-ErWgvAjC","connectiontag":"0.0.113","init":{"trackId":"jh63yjf5cua7wgutq6gsaft3"}}}},
     {"type":"registration","guid":"55759dd9b787bfc10500af54","fields":{"beaconId":"100","name":"Mark Little","showName":true,"__fh":{"cuid":"49DA907BA0AC4862B86A181FA1E76134","cuidMap":null,"destination":"iphone","sdk_version":"FH_JS_SDK/2.6.0+21","appid":"dbmxt6f5wnnvwzsaljx6qvxs","appkey":"378c73d8b74852439f8408181ec571dc3b486212","projectid":"54_tyE_DE2NMHNm-ErWgvAjC","connectiontag":"0.0.113","init":{"trackId":"jh63yjf5cua7wgutq6gsaft3"}}}},
     {"type":"registration","guid":"55759f9ef0f31dbf05006338","fields":{"beaconId":"70","name":"Jay Balunas","showName":true,"__fh":{"cuid":"49DA907BA0AC4862B86A181FA1E76134","cuidMap":null,"destination":"iphone","sdk_version":"FH_JS_SDK/2.6.0+21","appid":"dbmxt6f5wnnvwzsaljx6qvxs","appkey":"378c73d8b74852439f8408181ec571dc3b486212","projectid":"54_tyE_DE2NMHNm-ErWgvAjC","connectiontag":"0.0.113","init":{"trackId":"jh63yjf5cua7wgutq6gsaft3"}}}},
     ...
     ]
     */
    Json::Value deserializeRoot;
    Json::Reader reader;

    if ( !reader.parse(r.body, deserializeRoot) )
        printf("Failed to parse json body\n");
    Json::ArrayIndex count = deserializeRoot.size();
    printf("root value has %d elements\n", count);
    for(Json::ArrayIndex n = 0; n < count; n ++) {
        Json::Value value = deserializeRoot[n];
        Json::Value fields = value["fields"];
        const char* minorIDCstr = fields["beaconId"].asCString();
        int minorID = std::stoi(minorIDCstr);
        std::string name = fields["name"].asString().c_str();
        printf("[%d]: beaconId: %d, name: %s\n", n, minorID, name.c_str());
    }
}