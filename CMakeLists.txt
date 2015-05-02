cmake_minimum_required(VERSION 2.8.9)
project(NativeScanner)

# For profiling
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Libraries
add_library(CMS STATIC IMPORTED)
set_property(TARGET CMS PROPERTY IMPORTED_LOCATION /usr/local/lib/libactivemq-cpp.a)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} usr/local/lib)

# The eclipse paho mqtt client libs
add_library(MqttCLib SHARED IMPORTED)
set_property(TARGET MqttCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3c.so)
add_library(MqttAsyncCLib SHARED IMPORTED)
set_property(TARGET MqttAsyncCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3as.so)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)

# Proton and Qpid Messaging library in /opt/local
find_package(Proton REQUIRED PATHS /opt/local/lib/cmake/Proton /opt/local/lib64/cmake/Proton)
MESSAGE(STATUS "Found Proton: ${Proton_Libraries} (version \"${Proton_VERSION}\")")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    MESSAGE(STATUS "Skipping Qpid check for now")
    include_directories(/opt/local/include)
else()
    find_package(Qpid REQUIRED PATHS /opt/local/lib/cmake/Qpid /opt/local/lib64/cmake/Qpid)
    MESSAGE(STATUS "Found Qpid: ${Qpid_Libraries} (version \"${Qpid_VERSION}\")")
endif()

find_library(WiringPi_Libs NAMES wiringPi)
if(WiringPi_Libs)
    MESSAGE(STATUS "Found WiringPi_Libs: ${WiringPi_Libs}")
endif()
find_library(WiringPiDev_Libs NAMES wiringPiDev)
if(WiringPiDev_Libs)
    MESSAGE(STATUS "Found WiringPi_Libs: ${WiringPiDev_Libs}")
endif()
if(WiringPi_Libs AND WiringPiDev_Libs)
    add_subdirectory(lcd)
    set(WiringPi_LIBRARIES ${WiringPiDev_Libs} ${WiringPi_Libs})
    set(WiringPi_FOUND true)
    ADD_DEFINITIONS(-DHAVE_LCD_DISPLAY)
    MESSAGE(STATUS "Using WiringPi: ${WiringPi_LIBRARIES}")
    MESSAGE(STATUS "Added lcd source to build")
endif()

# Source directories
add_subdirectory(src)
add_subdirectory(src2)
add_subdirectory(proton)
add_subdirectory(socket)
add_subdirectory(cms)
add_subdirectory(tests)
