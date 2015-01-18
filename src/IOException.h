#ifndef IOException_H
#define IOException_H
#include <exception>
using namespace std;

class IOException : public exception {
    const char *msg;


public:
    IOException(const char *msg) : msg(msg) {}

    ~IOException() {
        delete msg;
    }

    const char* what() const noexcept {
        return msg;
    }
};
#endif


