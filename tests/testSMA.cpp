#include "../src2/SMA.h"
#include <iostream>

using namespace std;

int main(int argc, char * * argv) {
    SMA foo(3);
    SMA bar(5);

    int data[] = { 1, 2, 3, 4, 5, 5, 4, 3, 2, 1 };
    for (int * itr = data; itr < data + 10; itr++) {
        foo.add(*itr);
        cout << "Added " << *itr << " avg: " << foo.avg() << endl;
    }
    cout << endl;
    for (int * itr = data; itr < data + 10; itr++) {
        bar.add(*itr);
        cout << "Added " << *itr << " avg: " << bar.avg() << endl;
    }

    return 0;
}
