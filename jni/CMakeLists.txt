cmake_minimum_required(VERSION 2.8.4)
project(NativeScannerJNI)

# Uncomment to build the stack with print debugging enabled
# Can also use: cmake -DPRINT_DEBUG=ON -DCMAKE_BUILD_TYPE=Debug ..
#add_definitions(-DPRINT_DEBUG)

set(CMAKE_VERBOSE_MAKEFILE ON)
# BluezHome is set in the parent CMakeLists.txt file
set(BluezToolsParser ${BluezHome}/tools/parser)
include_directories(. ${BluezHome} ${BluezHome}/tools /usr/include/apr-1)

# Set JAVA_HOME from environment, or default to
set(JAVA_HOME $ENV{JAVA_HOME})
if(!JAVA_HOME)
    message(STATUS "JAVA_HOME not set in envrionment, attempting to default...")
    # Check for the oracle jvm on our debian image
    if(EXISTS /usr/lib/jvm/jdk-8-oracle-arm-vfp-hflt)
        set(JAVA_HOME /usr/lib/jvm/jdk-8-oracle-arm-vfp-hflt)
    else()
    # Default to a download in my envrionment
        set(JAVA_HOME /root/Downloads/jdk1.8.0_45)
        message(STATUS "JAVA_HOME not set, defaulting to ${JAVA_HOME}")
    endif()
endif()
# Validate it has the jni headers
if(EXISTS ${JAVA_HOME}/include AND EXISTS ${JAVA_HOME}/include/linux)
    include_directories(${JAVA_HOME}/include ${JAVA_HOME}/include/linux ../src)
    message(STATUS "Using JDK=${JAVA_HOME}")
else()
    message(FATAL_ERROR "${JAVA_HOME}/include and ${JAVA_HOME}/include/linux do not exist. Check that JAVA_HOME points to a JDK.")
endif()

# The scannerJni shared library
add_library (scannerJni SHARED org_jboss_summit2015_beacon_bluez_HCIDump.cpp ../src2/hcidumpinternal.c ${BluezToolsParser}/parser.c)
target_link_libraries(scannerJni BluezLibInternal)
