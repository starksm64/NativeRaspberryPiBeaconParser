#include <fstream>
#include <iostream>

using namespace std;

int main() {
    string os;
    ifstream pidora("/etc/redhat-release");
    ifstream debian("/etc/debian_version");
    if(pidora.good()) {
        pidora >> os;
    }
    else if(debian.good()) {
        debian >> os;
        os = "Debian " + os;
    }

    cout << os << endl;
}
