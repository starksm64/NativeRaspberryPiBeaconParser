
#include <map>

using namespace std;


int main() {
    map<int, int> testMap;

    testMap[1] = 1;
    testMap[2] = 4;
    testMap[3] = 9;
    testMap[4] ++;
    testMap[4] ++;
    testMap[4] ++;

    if(testMap.find(2) != testMap.end()) {
        printf("map[2] = %d\n", testMap[2]);
    }
    printf("map[4] = %d\n", testMap[4]);

    for(map<int32_t, int32_t>::iterator iter = testMap.begin(); iter != testMap.end(); iter++) {
        printf("\t%2d: %2d\n", iter->first, iter->second);
    }
}