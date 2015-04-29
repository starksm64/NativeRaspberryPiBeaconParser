project(ProtonIntegration)

# The eclipse paho mqtt client libs
set(FIND_LIBRARY_USE_LIB64_PATHS true)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib64 /usr/local/lib /opt/local/lib64 /opt/local/lib)

include_directories(${Proton_INCLUDE_DIRS})

set(echo_FILES echo.c)
add_executable(echo ${echo_FILES})
target_link_libraries (echo LINK_PUBLIC ${Proton_LIBRARIES})

set(testProtonSend_FILES testProtonSend.cpp)
add_executable(testProtonSend ${testProtonSend_FILES})
target_link_libraries (testProtonSend LINK_PUBLIC Beacon ${Proton_LIBRARIES})

set(testProtonPropsSend_FILES testProtonPropsSend.cpp)
add_executable(testProtonPropsSend ${testProtonPropsSend_FILES})
target_link_libraries (testProtonPropsSend LINK_PUBLIC Beacon ${Proton_LIBRARIES})

add_executable(testProtonBrowse testProtonBrowse.cpp)
target_link_libraries (testProtonBrowse LINK_PUBLIC ${Proton_LIBRARIES})
