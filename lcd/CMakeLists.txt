cmake_minimum_required(VERSION 2.8.9)
project(NativeScanner)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    MESSAGE(STATUS "Skipping wiringPi check on Darwin")
    set(WIRING_PI_HOME /Users/starksm/Dev/IoT/wiringPi)
    include_directories(${WIRING_PI_HOME}/wiringPi ${WIRING_PI_HOME}/devLib)
else()
    find_package(Qpid REQUIRED PATHS /usr/local/lib)
    MESSAGE(STATUS "Found Qpid: ${Qpid_Libraries} (version \"${Qpid_VERSION}\")")
endif()

add_library (lcdDisplayObjs OBJECT LcdDisplay.cpp lcd.c)
add_library (lcdDisplay SHARED $<TARGET_OBJECTS:lcdDisplayObjs>)
