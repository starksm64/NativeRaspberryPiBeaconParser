cmake_minimum_required(VERSION 2.8.4)
project(NativeScanner)

set(CMAKE_BINARY_DIR out)
SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(src)
add_subdirectory(tests)


