cmake_minimum_required(VERSION 2.8.4)
project(NativeScannerTests)

enable_testing()
include_directories(. ../src ../src2 /usr/local/include/activemq-cpp-3.8.3/ /opt/local/include/ /usr/local/include)

#set(BEACON_FILES ../src/Beacon.cpp ../src/Beacon.h)

set(threadsTest_FILES threads/threadsTest.cpp threads/backgroundTask.cpp threads/backgroundTask.h)
add_executable(threadsTest ${threadsTest_FILES} threads/MessageExchanger.cpp threads/MessageExchanger.h)
add_test(threadsTest ${CMAKE_CURRENT_BINARY_DIR}/threadsTest)

add_executable(testThreadSleep threads/testThreadSleep.cpp)
target_link_libraries (testThreadSleep LINK_PUBLIC pthread)
add_test(threadsTest ${CMAKE_CURRENT_BINARY_DIR}/testThreadSleep )

add_executable(testEventsWindow testEventsWindow.cpp ../src2/EventsWindow.cpp)
add_test(testEventsWindow ${CMAKE_CURRENT_BINARY_DIR}/testEventsWindow)

add_executable(testMedFilter medfilter/medflt.h medfilter/testMedFilter.cpp)
add_test(testProducerConsumer ${CMAKE_CURRENT_BINARY_DIR}/testProducerConsumer)

add_executable(testScannerIDs testScannerIDs.cpp)
add_test(testScannerIDs ${CMAKE_CURRENT_BINARY_DIR}/testScannerIDs)

set(testProducerConsumer_FILES threads/testProducerConsumer.cpp)
add_executable(testProducerConsumer ${testProducerConsumer_FILES} threads/MessageExchanger.cpp threads/MessageExchanger.h)
target_link_libraries (testProducerConsumer LINK_PUBLIC pthread)
add_test(testProducerConsumer ${CMAKE_CURRENT_BINARY_DIR}/testProducerConsumer)

add_executable(testAtomic testAtomic.cpp)
add_test(testAtomic ${CMAKE_CURRENT_BINARY_DIR}/testAtomic)

add_executable(testParseMapping testParseMapping.cpp)
add_test(testParseMapping ${CMAKE_CURRENT_BINARY_DIR}/testParseMapping)

add_executable(testStringParse testStringParse.cpp)
add_test(testStringParse ${CMAKE_CURRENT_BINARY_DIR}/testStringParse)

add_executable(testChrono testChrono.cpp)
add_test(testChrono ${CMAKE_CURRENT_BINARY_DIR}/testChrono)

add_executable(testTime testTime.cpp)
add_test(testTime ${CMAKE_CURRENT_BINARY_DIR}/testTime)

set(testProcStat_FILES testProcStat.cpp)
add_executable(testProcStat ${testProcStat_FILES})

add_executable(testBeaconMapping testBeaconMapping.cpp)
target_link_libraries (testBeaconMapping LINK_PUBLIC restclient-cpp ${jsoncpp_Libs})

set(testMap_FILES testMap.cpp)
add_executable(testMap ${testMap_FILES})
add_test(testMap ${CMAKE_CURRENT_BINARY_DIR}/testMap)

set(testCout_FILES testCout.cpp)
add_executable(testCout ${testCout_FILES})
add_test(testCout ${CMAKE_CURRENT_BINARY_DIR}/testCout)

add_executable(testOSType testOSType.cpp)

set(testVector_FILES testVector.cpp)
add_executable(testVector ${testVector_FILES})
add_test(testVector ${CMAKE_CURRENT_BINARY_DIR}/testVector)

set(testSMA_FILES testSMA.cpp)
add_executable(testSMA ${testSMA_FILES})
add_test(testSMA ${CMAKE_CURRENT_BINARY_DIR}/testSMA)

set(testSysinfo_FILES testSysinfo.cpp)
add_executable(testSysinfo ${testSysinfo_FILES})
add_test(testSysinfo ${CMAKE_CURRENT_BINARY_DIR}/testSysinfo)

add_executable(testParseNativeHCIDump testParseNativeHCIDump.c)
add_test(testParseNativeHCIDump ${CMAKE_CURRENT_BINARY_DIR}/testParseNativeHCIDump)

add_executable(testRE1 testRE1.cpp)
add_test(testRE1 ${CMAKE_CURRENT_BINARY_DIR}/testRE1)

add_executable(testSscanf testSscanf.cpp)
add_test(testSscanf ${CMAKE_CURRENT_BINARY_DIR}/testSscanf)

add_executable(testGetifaddrs testGetifaddrs.cpp)
add_test(testGetifaddrs ${CMAKE_CURRENT_BINARY_DIR}/testGetifaddrs)

add_executable(testSscanf2 testSScanf2.cpp)
add_test(testSscanf2 ${CMAKE_CURRENT_BINARY_DIR}/testSscanf2)

add_executable(testParse testParse.cpp)
target_link_libraries (testParse LINK_PUBLIC Beacon)
add_test(testParse ${CMAKE_CURRENT_BINARY_DIR}/testParse)

add_executable(testSocket testSocket.cpp)
target_link_libraries (testSocket LINK_PUBLIC Beacon Socket)
add_test(testSocket ${CMAKE_CURRENT_BINARY_DIR}/testSocket)

add_executable(testStatusBcast testStatusBcast.cpp ../src2/EventsWindow.cpp ../src2/HealthStatus.cpp)
target_link_libraries (testStatusBcast LINK_PUBLIC Beacon Socket pthread)

add_executable(testStatusBcastServer testStatusBcastServer.cpp ../src2/EventsWindow.cpp ../src2/HealthStatus.cpp)
target_link_libraries (testStatusBcastServer LINK_PUBLIC Beacon Socket pthread)

add_executable(testSocketReconnect testSocketReconnect.cpp)
target_link_libraries (testSocketReconnect LINK_PUBLIC Beacon Socket)
add_test(testSocketReconnect ${CMAKE_CURRENT_BINARY_DIR}/testSocketReconnect)

add_executable(testParseHCIDump testParseHCIDump.cpp)
target_link_libraries (testParseHCIDump LINK_PUBLIC Beacon)
add_test(testParseHCIDump ${CMAKE_CURRENT_BINARY_DIR}/testParseHCIDump)

set(testSerializeBeacon_FILES testSerializeBeacon.cpp)
add_executable(testSerializeBeacon ${testSerializeBeacon_FILES})
target_link_libraries (testSerializeBeacon LINK_PUBLIC Beacon)
add_test(testSerializeBeacon ${CMAKE_CURRENT_BINARY_DIR}/testSerializeBeacon)

set(testUnserializeBeacon_FILES testUnserializeBeacon.cpp)
add_executable(testUnserializeBeacon ${testUnserializeBeacon_FILES})
target_link_libraries (testUnserializeBeacon LINK_PUBLIC Beacon)
add_test(testUnserializeBeacon ${CMAKE_CURRENT_BINARY_DIR}/testUnserializeBeacon)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)
set(testParserLogic_FILES testParserLogic.cpp)
add_executable(testParserLogic ${testParserLogic_FILES})
target_link_libraries (testParserLogic LINK_PUBLIC Beacon MqttCLib  MqttAsyncCLib)
add_test(testParserLogic ${CMAKE_CURRENT_BINARY_DIR}/testParserLogic)

add_executable(testMiniLcdPCD8544 testMiniLcdPCD8544.cpp)
target_link_libraries (testMiniLcdPCD8544 LINK_PUBLIC Beacon lcdDisplay wiringPiDev wiringPi)

add_executable(testWiringPiLcdDisplay testWiringPiLcdDisplay.cpp)
target_link_libraries (testWiringPiLcdDisplay LINK_PUBLIC Beacon lcdDisplay wiringPiDev wiringPi)

add_executable(testButtonInterrupt testButtonInterrupt.cpp)
target_link_libraries (testButtonInterrupt LINK_PUBLIC wiringPi)
add_test(testButtonInterrupt ${CMAKE_CURRENT_BINARY_DIR}/testButtonInterrupt)

set(testDisplayStatus_FILES testDisplayStatus.cpp ../src2/EventsWindow.cpp ../src2/HealthStatus.cpp)
add_executable(testDisplayStatus ${testDisplayStatus_FILES})
target_link_libraries (testDisplayStatus LINK_PUBLIC Beacon lcdDisplay wiringPiDev wiringPi)
add_test(testDisplayStatus ${CMAKE_CURRENT_BINARY_DIR}/testDisplayStatus)

set(testMsgProperties_FILES testMsgProperties.cpp)
add_executable(testMsgProperties ${testMsgProperties_FILES})
target_link_libraries (testMsgProperties LINK_PUBLIC ${Qpid_LIBRARIES})
add_test(testParserLogic ${CMAKE_CURRENT_BINARY_DIR}/testMsgProperties)

add_executable(testQpidHeartbeat testQpidHeartbeat.cpp)
target_link_libraries (testQpidHeartbeat LINK_PUBLIC Beacon MqttCLib MqttAsyncCLib CMS ssl crypto z apr-1 pthread Socket ${Qpid_LIBRARIES})

add_executable(testQpidReconnect testQpidReconnect.cpp)
target_link_libraries (testQpidReconnect LINK_PUBLIC Beacon ${ActiveMQ_CPP_LIBRARIES} ${MQTT_LIBRARIES} pthread ssl crypto z ${Apr_Libs} ${Qpid_LIBRARIES} Socket)

add_executable(testSystemD testSystemD.cpp)

add_executable(testBeaconBuffer testBeaconBuffer.cpp ../src2/hcidumpinternal.h)

# Create a library for testing jni access to the beacon_info data structure
add_library (beaconBufferObjs OBJECT testBeaconBuffer.cpp ../src2/hcidumpinternal.h)
add_library (beaconBuffer STATIC $<TARGET_OBJECTS:beaconBufferObjs>)
