
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

static string scannerID("Default");
static vector<string> scannerIDs;
static int currentID = 0;
static bool cyclesScannerIDs;
static string getScannerID() {
    const string* myID = &scannerID;
    if(cyclesScannerIDs) {
        myID = scannerIDs.data()+currentID;
        currentID = (currentID + 1) % scannerIDs.size();
    }
    return *myID;
}

int main() {

    // If the scannerID is a comma separated list of ids, then getScannerID() will cycle through the ids
    string scannerID("Ballroom,Ballroomx,BoothL,BoothR,Devzone,Entrance,General,Generalx,GenStageL,GenStageR,Lounge,LunchL,LunchR,Room200,Room201,Room202,Room204,Room206,HallLx,HallCx,HallRx");
    stringstream ss(scannerID);
    string item;
    while (getline(ss, item, ',')) {
        scannerIDs.push_back(item);
    }
    cyclesScannerIDs = scannerIDs.size() > 1;
    if(cyclesScannerIDs) {
        printf("Running with %ld scanner IDs: ", scannerIDs.size());
        std::copy(scannerIDs.begin(), scannerIDs.end(),
                  std::ostream_iterator<string>(std::cout, " "));
        printf("\n");
    }

    for(int n = 0; n < 2*scannerIDs.size(); n ++) {
        printf("getScannerID(%d) = %s\n", n, getScannerID().c_str());
    }
}
