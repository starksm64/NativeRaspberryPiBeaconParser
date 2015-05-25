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

# The location of a local bluez source tree
if(EXISTS $ENV{HOME}/bluez-5.30)
    set(BluezHome $ENV{HOME}/bluez-5.30)
elseif(EXISTS $ENV{HOME}/bluez-5.28)
    set(BluezHome $ENV{HOME}/bluez-5.28)
else()
    set(BluezHome ../../BlueZ/bluez-5.28)
endif()

# We use the tools/parser contents from the bluez source tree as is
set(BluezToolsParser ${BluezHome}/tools/parser)
include_directories(. ../src ${BluezHome} ${BluezHome}/tools /usr/include/apr-1 /usr/local/include/activemq-cpp-3.8.3/)

set(tools_hcidump_SOURCES ${BluezToolsParser}/parser.h ${BluezToolsParser}/parser.c)
# The bluez internal lib from a local bluez build
add_library(BluezLibInternal SHARED IMPORTED)
set_property(TARGET BluezLibInternal PROPERTY IMPORTED_LOCATION ${BluezHome}/lib/.libs/libbluetooth-internal.a)


find_library(Apr_Libs NAMES apr-1 apr-1.0)
if(Apr_Libs)
    set(apr_LIBRARIES ${Apr_Libs})
    set(apr_FOUND true)
    MESSAGE(STATUS "Using Apr_Libs: ${Apr_Libs}")
endif()

set(SCANNER_LIBS Beacon ${ActiveMQ_CPP_LIBRARIES} ${MQTT_LIBRARIES} pthread ssl crypto z ${Apr_Libs} BluezLibInternal ${Qpid_LIBRARIES} Socket)

if(WiringPi_FOUND)
    set(SCANNER_LIBS ${SCANNER_LIBS} lcdDisplay wiringPiDev wiringPi)
endif()
# The NativeScannerBlueZ executable depends on our local source and the tools_hcidump_SOURCES from the bluez source tree
add_executable(NativeScannerBlueZ main.cpp HCIDumpCommand.h HCIDumpParser.h HCIDumpParser.cpp HealthStatus.cpp
    hcidumpinternal.c hcidumpinternal.h ${tools_hcidump_SOURCES} EventsWindow.cpp EventsWindow.h EventExchanger.h EventExchanger.cpp BeaconEventConsumer.cpp BeaconEventConsumer.h EventsBucket.h)
target_link_libraries (NativeScannerBlueZ LINK_PUBLIC ${SCANNER_LIBS})
