cmake_minimum_required(VERSION 2.8.4)
project(NativeScannerTests)

enable_testing()
include_directories(. ../src)

#set(BEACON_FILES ../src/Beacon.cpp ../src/Beacon.h)

set(testCout_FILES testCout.cpp)
add_executable(testCout ${testCout_FILES})
add_test(testCout ${CMAKE_CURRENT_BINARY_DIR}/testCout)

set(testRE1_FILES testRE1.cpp)
add_executable(testRE1 ${testRE1_FILES})
add_test(testRE1 ${CMAKE_CURRENT_BINARY_DIR}/testRE1)

set(testParse_FILES testParse.cpp)
add_executable(testParse ${testParse_FILES})
target_link_libraries (testParse LINK_PUBLIC Beacon)
add_test(testParse ${CMAKE_CURRENT_BINARY_DIR}/testParse)

set(testParseHCIDump_FILES testParseHCIDump.cpp)
add_executable(testParseHCIDump ${testParseHCIDump_FILES})
target_link_libraries (testParseHCIDump LINK_PUBLIC Beacon)
add_test(testParseHCIDump ${CMAKE_CURRENT_BINARY_DIR}/testParseHCIDump)

set(testSerializeBeacon_FILES testSerializeBeacon.cpp)
add_executable(testSerializeBeacon ${testSerializeBeacon_FILES})
target_link_libraries (testSerializeBeacon LINK_PUBLIC Beacon)
add_test(testSerializeBeacon ${CMAKE_CURRENT_BINARY_DIR}/testSerializeBeacon)

set(testUnserializeBeacon_FILES testUnserializeBeacon.cpp)
add_executable(testUnserializeBeacon ${testUnserializeBeacon_FILES})
target_link_libraries (testUnserializeBeacon LINK_PUBLIC Beacon)
add_test(testUnserializeBeacon ${CMAKE_CURRENT_BINARY_DIR}/testUnserializeBeacon)

# The eclipse paho mqtt client libs
add_library(MqttCLib SHARED IMPORTED)
set_property(TARGET MqttCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3c.so.1.0)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)
set(testParserLogic_FILES testParserLogic.cpp)
add_executable(testParserLogic ${testParserLogic_FILES})
target_link_libraries (testParserLogic LINK_PUBLIC Beacon MqttCLib)
add_test(testParserLogic ${CMAKE_CURRENT_BINARY_DIR}/testUnserializeBeacon)
