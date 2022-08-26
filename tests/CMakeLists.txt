################################################
#   NetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

find_package (Threads REQUIRED)

add_executable(nettleTests
        AllNetTests.cpp
        main.cpp
)
target_link_libraries(nettleTests ${LIBRARY_NAME}

        Threads::Threads
        ${BSLCORE_LIBRARIES}
        ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET nettleTests COMMAND ./nettleTests POST_BUILD)