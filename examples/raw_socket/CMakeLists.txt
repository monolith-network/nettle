cmake_minimum_required(VERSION 3.8)

if (MSVC)
    set(LIBS ${LIBS} ws2_32.lib)
else()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
endif(MSVC)

find_package (Threads REQUIRED)

find_package(Nettle REQUIRED)
include_directories(${NETTLE_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
        )

set(CMAKE_CXX_STANDARD 17)

add_executable(rawSocket
        main.cpp
        )

target_link_libraries(rawSocket
        PRIVATE
        ${ORBIT_LIBRARIES}
        ${NETTLE_LIBRARIES}
        Threads::Threads
        )
