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

# REST
find_library(restcpp_Libs NAMES restclient-cpp)
if(restcpp_Libs)
    ADD_DEFINITIONS(-DHAVE_REST_LIB)
    MESSAGE(STATUS "Found restcpp_Libs: ${restcpp_Libs}")
else()
    set(restcpp_Libs "")
    MESSAGE(STATUS "No restcpp_Libs")
endif()

# JSON
find_library(jsoncpp_Libs NAMES jsoncpp)
if(jsoncpp_Libs)
    ADD_DEFINITIONS(-DHAVE_JSON_LIB)
    MESSAGE(STATUS "Found jsoncpp_Libs: ${jsoncpp_Libs}")
else()
    set(jsoncpp_Libs "")
    MESSAGE(STATUS "No jsoncpp_Libs")
endif()

# Conditional settings on activemq-cpp lib
find_library(ActiveMQ_CPP_Libs NAMES activemq-cpp)
if(ActiveMQ_CPP_Libs)
    add_subdirectory(cms)
    set(ActiveMQ_CPP_LIBRARIES ${ActiveMQ_CPP_Libs})
    set(ActiveMQ_CPP_FOUND true)
    ADD_DEFINITIONS(-DHAVE_ActiveMQ_CPP)
    MESSAGE(STATUS "Using ActiveMQ_CPP: ${ActiveMQ_CPP_LIBRARIES}")
    MESSAGE(STATUS "Added cms source to build")
else()
    MESSAGE(STATUS "Skipping ActiveMQ_CPP publisher")
endif()

# Conditional settings on MqttCLib
find_library(Mqtt_Libs NAMES paho-mqtt3c)
find_library(MqttAsync_Libs NAMES paho-mqtt3as)
if(Mqtt_Libs AND MqttAsync_Libs)
    MESSAGE(STATUS "Found Mqtt_Libs: ${Mqtt_Libs}, ${MqttAsync_Libs}")
    ADD_DEFINITIONS(-DHAVE_MQTT)
    set(MQTT_LIBRARIES ${Mqtt_Libs} ${MqttAsync_Libs})
    set(MQTT_FOUND true)
else()
    MESSAGE(STATUS "Skipping MQTT publisher")
    set(MQTT_LIBRARIES "")
endif()

# Source directories
add_subdirectory(src)
add_subdirectory(src2)
add_subdirectory(proton)
add_subdirectory(socket)
add_subdirectory(tests)
