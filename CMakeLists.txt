cmake_minimum_required(VERSION 2.8.4)
project(NativeScanner)

# For profiling
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(src)
add_subdirectory(src2)
add_subdirectory(proton)
add_subdirectory(cms)
add_subdirectory(tests)
