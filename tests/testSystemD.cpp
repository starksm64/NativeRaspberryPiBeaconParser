/**
 * A little program that will exit with different exit codes when /tmp/testSystemD.exit file is seen to test
 * the restart feature of systemd services. To test:
 * 1. cp testSystemD.service /lib/systemd/system
 * 2. systemctl enable testSystemD.service
 * 3. systemctl start testSystemD.service
 * 4. touch /tmp/testSystemD.exit0
 * ## systemd Restart option used will not restart a service that exited with a zero code, so restart manually
 * 5. systemctl start testSystemD.service
 * 6. touch /tmp/testSystemD.ex
 * 7. touch /tmp/testSystemD.npe
 * 8. touch /tmp/testSystemD.segv
 */

#include <exception>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <unistd.h>
#include <execinfo.h>
#include <sys/stat.h>
#include <stdexcept>

using namespace std;

void printStacktrace() {
    void *array[20];
    int size = backtrace(array, sizeof(array) / sizeof(array[0]));
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}

static void terminateHandler() {
    exception_ptr exptr = std::current_exception();
    if (exptr != nullptr) {
        // the only useful feature of std::exception_ptr is that it can be rethrown...
        try {
            rethrow_exception(exptr);
        } catch (std::exception &ex) {
            fprintf(stderr, "Terminated due to exception: %s\n", ex.what());
        }
        catch (...) {
            fprintf(stderr, "Terminated due to unknown exception\n");
        }
    }
    else {
        fprintf(stderr, "Terminated due to unknown reason :(\n");
    }
    printStacktrace();
    exit(100);
}

inline bool markerExists(const char* marker) {
    struct stat buffer;
    bool exists = (stat (marker, &buffer) == 0);
    return exists;
}

/**
 * Look for a /tmp/testSystemD.exit file marker and return its content code if found
 */
static int readExitCode() {
    int code = 0;
    if(markerExists("/tmp/testSystemD.exit")) {
        fstream exit("/tmp/testSystemD.exit", std::ios_base::in);
        exit >> code;
    }
    return code;
}


int main(int argc, char** argv) {

    // Install default terminate handler to make sure we exit with non-zero status
    std::set_terminate(terminateHandler);

    // Clear any existing marker files
    std::remove("/tmp/testSystemD.exit");
    std::remove("/tmp/testSystemD.exit0");
    std::remove("/tmp/testSystemD.ex");
    std::remove("/tmp/testSystemD.npe");
    std::remove("/tmp/testSystemD.segv");

    while(true) {
        // Look for exit code
        int exitCode = readExitCode();
        if(exitCode) {
            printf("Exiting with code=%d\n", exitCode);
            exit(exitCode);
        }
        // Look for 0 exit code
        if(markerExists("/tmp/testSystemD.exit0")) {
            printf("Exiting with success code\n");
            exit(0);
        }
        // Look for exception
        if(markerExists("/tmp/testSystemD.ex")) {
            printf("Exiting with c++ exception\n");
            throw std::runtime_error("Exiting due to /tmp/testSystemD.ex");
        }
        // Look for null pointer reference
        if(markerExists("/tmp/testSystemD.npe")) {
            printf("Exiting with null pointer reference\n");
            string* sptr = nullptr;
            sptr->length();
        }
        // Look for segv pointer reference
        if(markerExists("/tmp/testSystemD.segv")) {
            printf("Exiting with bad pointer reference\n");
            string* sptr = (string *) 0xdeadbeef;
            sptr->data();
        }
        sleep(1);
    }
}
