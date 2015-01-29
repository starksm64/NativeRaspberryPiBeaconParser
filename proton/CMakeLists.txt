project(ProtonIntegration)

# The eclipse paho mqtt client libs
set(FIND_LIBRARY_USE_LIB64_PATHS true)
#find_package(Proton REQUIRED)
add_library(Proton SHARED IMPORTED)
set_property(TARGET Proton PROPERTY IMPORTED_LOCATION /usr/local/lib64/libqpid-proton.dylib)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib64 /usr/local/lib)

set(testProtonSend_FILES testProtonSend.cpp)
add_executable(testProtonSend ${testProtonSend_FILES})
target_link_libraries (testProtonSend LINK_PUBLIC Beacon Proton)
