set(VERBOSE 1)

include_directories(. /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.c/src ${BluezHome} ${BluezHome}/tools /usr/include/apr-1 /usr/local/include/activemq-cpp-3.8.3/ /home/qpid/git/paho.mqtt.c/src ${Qpid_INCLUDE_DIRS})

# Define the non-main classes to build a reusable libBeacon.a for use with the tests
set(SOURCE_FILES Beacon.h Beacon.cpp ByteBuffer.h ByteBuffer.cpp MqttPublisher.h ParserLogic.h ParserLogic.cpp
    MsgPublisher.cpp CMSPublisher.cpp MqttPublisher.cpp ProtonPublisher.cpp QpidPublisher.cpp
)

add_library (BeaconObjs OBJECT ${SOURCE_FILES} SocketPublisher.cpp SocketPublisher.h)
add_library (Beacon STATIC $<TARGET_OBJECTS:BeaconObjs>)

add_executable(NativeScanner main.cpp)
target_link_libraries (NativeScanner LINK_PUBLIC Beacon MqttCLib MqttAsyncCLib CMS ssl crypto z apr-1 ${Qpid_LIBRARIES})

# Tests
set(testCmsFactory_FILES testCmsFactory.cpp)
add_executable(testCmsFactory ${testCmsFactory_FILES})
target_link_libraries (testCmsFactory LINK_PUBLIC Beacon MqttCLib MqttAsyncCLib CMS ssl crypto z apr-1 pthread ${Qpid_LIBRARIES})

set(testQpidFactory_FILES testQpidFactory.cpp)
add_executable(testQpidFactory ${testQpidFactory_FILES})
target_link_libraries (testQpidFactory LINK_PUBLIC Beacon MqttCLib MqttAsyncCLib CMS ssl crypto z apr-1 pthread Socket ${Qpid_LIBRARIES})
