#include <string.h>
#include "org_jboss_summit2015_beacon_bluez_HCIDump.h"
#include "../src2/hcidumpinternal.h"
#include <chrono>
#include <thread>
using namespace std;

static JavaVM *theVM;

// the beacon_info pointer shared with java as a direct ByteBuffer
static beacon_info *javaBeaconInfo;
static jobject byteBufferObj;
// a cached object handle to the org.jboss.summit2015.beacon.bluez.HCIDump class
static jclass hcidumpClass;
// The JNIEnv passed to the initBuffer native method
static JNIEnv *javaEnv = nullptr;
static jmethodID eventNotification;

extern "C" bool beacon_event_callback_to_java(beacon_info * info);

/**
 * The shared library load callback to set the JavaVM pointer
 */
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    theVM = vm;
    printf("JNI_OnLoad, requesting JNI_VERSION_1_8\n");
    return JNI_VERSION_1_8;
}

/**
 * Called by the scanner thread entry points to attach the thread to the JavaVM and allocate the
 * direct ByteBuffer to javaBeaconInfo mapping.
 */
static void attachToJavaVM() {
    if(javaEnv == nullptr) {
        // We need to attach this thread the to jvm
        int status;
        if ((status = theVM->GetEnv((void**)&javaEnv, JNI_VERSION_1_8)) < 0) {
            if ((status = theVM->AttachCurrentThreadAsDaemon((void**)&javaEnv, nullptr)) < 0) {
                return;
            }
        }
        javaBeaconInfo = (beacon_info *) javaEnv->GetDirectBufferAddress(byteBufferObj);
        memset(javaBeaconInfo, 0, sizeof(beacon_info));
    }
}

/**
 * Simple test event generator function to validate callback into java
 */
static void eventGenerator() {
    attachToJavaVM();
    beacon_info info;
    // Initialize common data
    strcpy(info.uuid, "DAF246CEF20111E4B116123B93F75CBA");
    info.calibrated_power = -38;
    info.code = 1;
    info.isHeartbeat = false;
    info.count = 0;
    info.major = 12345;
    info.minor = 11111;
    info.power = -48;
    info.rssi = -50;
    for(int n = 0; n < 100; n ++) {
        chrono::milliseconds now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
        info.count ++;
        info.time = now.count();
        beacon_event_callback_to_java(&info);
    }
}

// Flag to cause wait loop for debugger to attach to the java process
static bool waiting = false;

/**
 * The thread entry point for running the hcidump scanning loop
 * @param device the numeric value of the host controller interface instance to scan
 */
static void runScanner(int device) {
    // Connect this thread to the JavaVM instance
    attachToJavaVM();
    while(waiting)
        this_thread::yield();
    scan_frames(device, beacon_event_callback_to_java);
}

JNIEXPORT void JNICALL Java_org_jboss_summit2015_beacon_bluez_HCIDump_allocScanner
(JNIEnv *env, jclass clazz, jobject bb, jint device) {
    printf("begin Java_org_jboss_summit2015_beacon_bluez_HCIDump_allocScanner(%x,%x,%x)\n", env, clazz, bb);
    // Create global references to the ByteBuffer and HCIDump class for use in other native threads
    byteBufferObj = (jobject) env->NewGlobalRef(bb);
    hcidumpClass = (jclass) env->NewGlobalRef(clazz);
    eventNotification = env->GetStaticMethodID(hcidumpClass, "eventNotification", "()V");
    if(eventNotification == nullptr) {
        fprintf(stderr, "Failed to lookup eventNotification()V on: jclass=%s", clazz);
        exit(1);
    }
    printf("Found eventNotification=%x\n", eventNotification);

    thread::id tid;
#if 0
    // Simple test thread
    thread t(eventGenerator, device);
#else
    // Run the scanner
    thread t(runScanner, device);
#endif
    tid = t.get_id();
    t.detach();
    printf("end Java_org_jboss_summit2015_beacon_bluez_HCIDump_allocScanner, tid=%x, hcidumpClassRef=%x, eventNotification=%x\n", tid, hcidumpClass, eventNotification);
}

JNIEXPORT void JNICALL Java_org_jboss_summit2015_beacon_bluez_HCIDump_freeScanner
        (JNIEnv *env, jclass clazz) {
    printf("begin Java_org_jboss_summit2015_beacon_bluez_HCIDump_freeScanner(%x,%x)\n", env, clazz);
    javaEnv->DeleteGlobalRef(hcidumpClass);
    javaEnv->DeleteGlobalRef(byteBufferObj);
}

/**
* Callback invoked by the hdidumpinternal.c code when a LE_ADVERTISING_REPORT event is seen on the stack. This
 * passes the event beacon_info back to java via the javaBeaconInfo pointer and returns the stop flag
 * indicator as returned by the event notification callback return value.
*/
static long eventCount = 0;
extern "C" bool beacon_event_callback_to_java(beacon_info * info) {
#if PRINT_DEBUG
    printf("beacon_event_callback_to_java(%ld: %s, code=%d, time=%lld)\n", eventCount, info->uuid, info->code, info->time);
#endif
    eventCount ++;
    // Copy the event data to javaBeaconInfo
    memcpy(javaBeaconInfo, info, sizeof(*info));
    // Notify java that the buffer has been updated
    jboolean stop = javaEnv->CallStaticBooleanMethod(hcidumpClass, eventNotification);
    return stop == JNI_TRUE;
}
