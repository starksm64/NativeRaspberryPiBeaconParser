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

# The location of a local bluez source tree
if(EXISTS $ENV{HOME}/bluez-5.28)
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

# The NativeScannerBlueZ executable depends on our local source and the tools_hcidump_SOURCES from the bluez source tree
add_executable(NativeScannerBlueZ main.cpp HCIDumpCommand.h HCIDumpParser.h HCIDumpParser.cpp
    hcidumpinternal.c hcidumpinternal.h ${tools_hcidump_SOURCES} EventsWindow.cpp EventsWindow.h)
target_link_libraries (NativeScannerBlueZ LINK_PUBLIC Beacon CMS pthread ssl crypto z apr-1 MqttCLib MqttAsyncCLib BluezLibInternal ${Qpid_LIBRARIES})
