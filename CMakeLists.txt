cmake_minimum_required(VERSION 2.8.4)
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

# Source directories
add_subdirectory(src)
add_subdirectory(src2)
add_subdirectory(proton)
add_subdirectory(cms)
add_subdirectory(tests)
