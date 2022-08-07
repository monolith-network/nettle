################################################
#   NetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslNetTests
        AllNetTests.cpp
        main.cpp
)
target_link_libraries(bslNetTests ${LIBRARY_NAME}
        ${BSLCORE_LIBRARIES}
        ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET bslNetTests COMMAND ./bslNetTests POST_BUILD)