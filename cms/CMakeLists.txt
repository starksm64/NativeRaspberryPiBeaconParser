project(CMSIntegration)

include_directories(. /usr/include/apr-1 /usr/local/include/activemq-cpp-3.8.3/)

add_library(CMS STATIC IMPORTED)
set_property(TARGET CMS PROPERTY IMPORTED_LOCATION /usr/local/lib/libactivemq-cpp.a)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} usr/local/lib)

set(testCMSSend_FILES testCMSSend.cpp)
add_executable(testCMSSend ${testCMSSend_FILES})
target_link_libraries (testCMSSend LINK_PUBLIC CMS ssl crypto z apr-1.0)

set(testCMSSendByteMessage_FILES testCMSSendByteMessage.cpp)
add_executable(testCMSSendByteMessage ${testCMSSendByteMessage_FILES})
target_link_libraries (testCMSSendByteMessage LINK_PUBLIC Beacon CMS ssl crypto z apr-1.0)
