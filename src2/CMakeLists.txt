project(NativeScannerBlueZ)

# The location of a local bluez source tree
set(BluezHome /home/starksm/bluez-5.25)
# We use the tools/parser contents from the bluez source tree as is
set(BluezToolsParser ${BluezHome}/tools/parser)
include_directories(. ../src ${BluezHome} ${BluezHome}/tools)
set(tools_hcidump_SOURCES ${BluezToolsParser}/parser.h ${BluezToolsParser}/parser.c)
# The bluez internal lib from a local bluez build
add_library(BluezLibInternal SHARED IMPORTED)
set_property(TARGET BluezLibInternal PROPERTY IMPORTED_LOCATION ${BluezHome}/lib/.libs/libbluetooth-internal.a)

# The eclipse paho mqtt client libs
add_library(MqttCLib SHARED IMPORTED)
set_property(TARGET MqttCLib PROPERTY IMPORTED_LOCATION /usr/local/lib/libpaho-mqtt3c.so)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)

# The NativeScannerBlueZ executable depends on our local source and the tools_hcidump_SOURCES from the bluez source tree
add_executable(NativeScannerBlueZ main.cpp HCIDumpCommand.h HCIDumpParser.h HCIDumpParser.cpp
    hcidumpinternal.c hcidumpinternal.h ${tools_hcidump_SOURCES})
target_link_libraries (NativeScannerBlueZ LINK_PUBLIC Beacon MqttCLib BluezLibInternal)
