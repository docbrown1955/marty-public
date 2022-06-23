add_executable(C7_SM.x src/C7_SM.cpp)
target_link_libraries(C7_SM.x marty)

add_test(NAME TEST_C7_SM COMMAND C7_SM.x)
