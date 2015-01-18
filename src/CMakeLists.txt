set(VERBOSE 1)

include_directories(.
    /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.c/src
    /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.cpp/src)

# Define the non-main classes to build a reusable libBeacon.a for use with the tests
set(SOURCE_FILES Beacon.h Beacon.cpp ByteBuffer.h ByteBuffer.cpp MqttPublisher.h MqttPublisher.cpp ParserLogic.h ParserLogic.cpp)

add_library (BeaconObjs OBJECT ${SOURCE_FILES})
add_library (Beacon STATIC $<TARGET_OBJECTS:BeaconObjs>)

# The eclipse paho mqtt client libs
add_library(MqttCLib SHARED IMPORTED)
set_property(TARGET MqttCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3c.so.1.0)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)

add_executable(NativeScanner main.cpp)
target_link_libraries (NativeScanner LINK_PUBLIC Beacon MqttCLib)
