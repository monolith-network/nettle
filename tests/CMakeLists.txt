################################################
#   NetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

find_package (Threads REQUIRED)

add_executable(bslNetTests
        AllNetTests.cpp
        main.cpp
)
target_link_libraries(bslNetTests ${LIBRARY_NAME}

        Threads::Threads
        ${BSLCORE_LIBRARIES}
        ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET bslNetTests COMMAND ./bslNetTests POST_BUILD)