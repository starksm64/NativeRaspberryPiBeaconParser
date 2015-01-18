set(VERBOSE 1)

include_directories(.
    /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.c/src
    /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.cpp/src)

set(SOURCE_FILES Beacon.h Beacon.cpp ByteBuffer.h ByteBuffer.cpp MqttPublisher.h MqttPublisher.cpp ParserLogic.h ParserLogic.cpp)

add_library (BeaconObjs OBJECT ${SOURCE_FILES})
add_library (Beacon STATIC $<TARGET_OBJECTS:BeaconObjs>)

set(MqttCLib_SOURCE /Users/starksm/Dev/IoT/BLE/org.eclipse.paho.mqtt.c)
add_library(MqttCLib SHARED IMPORTED)
set_property(TARGET MqttCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3c.so.1.0)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)

add_executable(NativeScanner main.cpp ${SOURCE_FILES})
target_link_libraries (NativeScanner LINK_PUBLIC Beacon MqttCLib)
