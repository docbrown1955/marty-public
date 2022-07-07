if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

function(ADD_UNIT_TEST NAME)
    add_executable(${NAME}.x ${NAME}.cpp)
    target_link_libraries(${NAME}.x marty gtest_main)
    include(GoogleTest)
    gtest_discover_tests(${NAME}.x)
endfunction()

ADD_UNIT_TEST(test_equality)
