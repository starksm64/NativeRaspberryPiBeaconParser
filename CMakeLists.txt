cmake_minimum_required(VERSION 2.8.4)
project(NativeScanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_subdirectory(src)
add_subdirectory(src2)
add_subdirectory(tests)


