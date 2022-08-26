cmake_minimum_required(VERSION 3.8)

project(Nettle)

find_package (Threads REQUIRED)

##################################################
# Options
##################################################

option(COMPILE_TESTS    "Compile the tests"    ON)

##################################################
# Find CPPU tests, and set project env
##################################################


include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Threads.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

set(CMAKE_CXX_STANDARD 20)

##################################################
# Glob modules
##################################################

set(HEADERS
        lib/ConnectionHandler.hpp
        lib/HostPort.hpp
        lib/Socket.hpp
        lib/Writer.hpp
        lib/TcpServer.hpp
        lib/UdpServerN.hpp
        )

set(SOURCES
        lib/HostPort.cpp
        lib/Socket.cpp
        lib/Writer.cpp
        lib/TcpServer.cpp
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

##################################################
# Configure Library
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)

##################################################
# Tests and Examples 
##################################################

if(COMPILE_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

if(COMPILE_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

##################################################
# Configure Install
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)


