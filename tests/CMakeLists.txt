cmake_minimum_required(VERSION 2.8.4)
project(NativeScannerTests)

enable_testing()
include_directories(. ../src)

#set(BEACON_FILES ../src/Beacon.cpp ../src/Beacon.h)

set(threadsTest_FILES threads/threadsTest.cpp threads/backgroundTask.cpp threads/backgroundTask.h)
add_executable(threadsTest ${threadsTest_FILES} threads/MessageExchanger.cpp threads/MessageExchanger.h)
add_test(threadsTest ${CMAKE_CURRENT_BINARY_DIR}/threadsTest)

set(testEventsWindow_FILES testEventsWindow.cpp ../src2/EventsWindow.cpp)
add_executable(testEventsWindow ${testEventsWindow_FILES})
add_test(testEventsWindow ${CMAKE_CURRENT_BINARY_DIR}/testEventsWindow)

set(testProducerConsumer_FILES threads/testProducerConsumer.cpp)
add_executable(testProducerConsumer ${testProducerConsumer_FILES} threads/MessageExchanger.cpp threads/MessageExchanger.h)
target_link_libraries (testProducerConsumer LINK_PUBLIC pthread)
add_test(testProducerConsumer ${CMAKE_CURRENT_BINARY_DIR}/testProducerConsumer)

set(testTime_FILES testTime.c)
add_executable(testTime ${testTime_FILES})
add_test(testTime ${CMAKE_CURRENT_BINARY_DIR}/testTime)

set(testProcStat_FILES testProcStat.cpp)
add_executable(testProcStat ${testProcStat_FILES})
add_test(testProcStat ${CMAKE_CURRENT_BINARY_DIR}/testProcStat)

set(testMap_FILES testMap.cpp)
add_executable(testMap ${testMap_FILES})
add_test(testMap ${CMAKE_CURRENT_BINARY_DIR}/testMap)

set(testCout_FILES testCout.cpp)
add_executable(testCout ${testCout_FILES})
add_test(testCout ${CMAKE_CURRENT_BINARY_DIR}/testCout)

set(testVector_FILES testVector.cpp)
add_executable(testVector ${testVector_FILES})
add_test(testVector ${CMAKE_CURRENT_BINARY_DIR}/testVector)

set(testParseNativeHCIDump_FILES testParseNativeHCIDump.c)
add_executable(testParseNativeHCIDump ${testParseNativeHCIDump_FILES})
add_test(testParseNativeHCIDump ${CMAKE_CURRENT_BINARY_DIR}/testParseNativeHCIDump)

set(testRE1_FILES testRE1.cpp)
add_executable(testRE1 ${testRE1_FILES})
add_test(testRE1 ${CMAKE_CURRENT_BINARY_DIR}/testRE1)

set(testParse_FILES testParse.cpp)
add_executable(testParse ${testParse_FILES})
target_link_libraries (testParse LINK_PUBLIC Beacon)
add_test(testParse ${CMAKE_CURRENT_BINARY_DIR}/testParse)

set(testParseHCIDump_FILES testParseHCIDump.cpp)
add_executable(testParseHCIDump ${testParseHCIDump_FILES})
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
add_test(testParserLogic ${CMAKE_CURRENT_BINARY_DIR}/testUnserializeBeacon)
