cmake_minimum_required(VERSION 2.8.9)
project(NativeScanner)

include_directories(/usr/local/include ../src ../src2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    MESSAGE(STATUS "Skipping wiringPi check on Darwin")
    set(WIRING_PI_HOME /Users/starksm/Dev/IoT/wiringPi)
    include_directories(${WIRING_PI_HOME}/wiringPi ${WIRING_PI_HOME}/devLib)
else()
endif()

add_library (lcdDisplayObjs OBJECT ../src2/ScannerView.h LcdDisplay.cpp)
add_library (lcdDisplay SHARED $<TARGET_OBJECTS:lcdDisplayObjs>)
