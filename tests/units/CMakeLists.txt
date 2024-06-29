function(ADD_UNIT_TEST NAME)
    add_executable(${NAME}.x ${NAME}.cpp)
    target_link_libraries(${NAME}.x marty)
    SETUP_GTEST_TARGET(${NAME}.x units)
endfunction()

add_subdirectory(csl)
add_subdirectory(marty)
add_subdirectory(marty-plugins)