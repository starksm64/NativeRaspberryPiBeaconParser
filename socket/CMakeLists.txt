
# Define the non-main classes to build a reusable libSocket.a for use with the tests
set(SOURCE_FILES PracticalSocket.h PracticalSocket.cpp)
add_library (SocketObjs OBJECT ${SOURCE_FILES})
add_library (Socket STATIC $<TARGET_OBJECTS:SocketObjs>)
