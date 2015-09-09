project(NativeScannerBlueZ)

OPTION(PRINT_DEBUG
  "Build the project using print debugging enabled"
  OFF)
IF(PRINT_DEBUG)
  MESSAGE("Adding print debug flag...")
  ADD_DEFINITIONS(-DPRINT_DEBUG)
  SET(CMAKE_BUILD_TYPE Debug)
  MESSAGE("Build type is " ${CMAKE_BUILD_TYPE})
ENDIF(PRINT_DEBUG)

add_definitions(-DUSE_YIELD_LOOP)

include_directories(. ../src ${BluezHome} ${BluezHome}/tools /usr/include/apr-1 /usr/local/include/activemq-cpp-3.8.3/)

set(tools_hcidump_SOURCES ${BluezToolsParser}/parser.h ${BluezToolsParser}/parser.c)


find_library(Apr_Libs NAMES apr-1 apr-1.0)
if(Apr_Libs)
    set(apr_LIBRARIES ${Apr_Libs})
    set(apr_FOUND true)
    MESSAGE(STATUS "Using Apr_Libs: ${Apr_Libs}")
endif()

set(SCANNER_LIBS Beacon ${ActiveMQ_CPP_LIBRARIES} ${MQTT_LIBRARIES} pthread ssl crypto z ${Apr_Libs} BluezLibInternal ${Qpid_LIBRARIES} Socket ${restcpp_Libs} ${jsoncpp_Libs})

if(WiringPi_FOUND)
    set(SCANNER_LIBS ${SCANNER_LIBS} lcdDisplay wiringPiDev wiringPi)
endif()

message("SCANNER_LIBS=${SCANNER_LIBS}")
# The NativeScannerBlueZ executable depends on our local source and the tools_hcidump_SOURCES from the bluez source tree
add_executable(NativeScannerBlueZ main.cpp HCIDumpCommand.h HCIDumpParser.h HCIDumpParser.cpp HealthStatus.cpp
    hcidumpinternal.c hcidumpinternal.h ${tools_hcidump_SOURCES} EventsWindow.cpp EventsWindow.h
    EventExchanger.h EventExchanger.cpp BeaconEventConsumer.cpp BeaconEventConsumer.h BeaconMapper.cpp EventsBucket.h PropertiesMapper.cpp PropertiesMapper.h)
target_link_libraries (NativeScannerBlueZ LINK_PUBLIC ${SCANNER_LIBS})
