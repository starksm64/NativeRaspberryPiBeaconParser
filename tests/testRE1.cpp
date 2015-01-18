#include <iostream>
#include <string>
#include <regex>

using namespace std;

static std::string trim(std::string str) {
    size_t endpos = str.find_last_not_of(" \t");
    if( string::npos != endpos )
    {
        str = str.substr( 0, endpos+1 );
    }
    return str;
}

int main() {
    printf("Begin testRE1\n");
    std::string line = "> 04 3E 2A 02 01 03 01 92 EF 0D 06 40 30 1E 02 01 06 1A FF 4C ";
    //const char *pattern = ">\\s04((\\s[0-9A-F]{2}){6}).*1A FF 4C";
    const char *pattern = ">\\s04.*((?:\\s[[:xdigit:]]{2}){6})((?:\\s[[:xdigit:]]{2}){4}) 1A FF 4C";
    std::regex exp(pattern);
    line = trim(line);
    cout << "Checking '" << line << "'" << endl;
    if(regex_match(line, exp)) {
        cout << "Line matches pattern\n";
    } else {
        cout << "No match\n";
    }
    cout << flush;

    std::cmatch cm;
    std::regex_match(line.c_str(), cm, exp);
    std::cout << "string literal with " << cm.size() << " matches\n";
    for(int n = 0; n < cm.size(); n ++) {
        cout << "Match[" << n << "] = " << cm[n] << endl;
    }
    /*
    Assert.assertTrue("There are matches", m.matches());
    for(int n = 0; n <= m.groupCount(); n ++)
        System.out.printf("group(%d): %s\n", n, m.group(n));
    System.out.printf("BDADDR: %s\n", m.group(1));
    */
    return 0;
}
