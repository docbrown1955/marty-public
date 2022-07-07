include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

function(ADD_UNIT_TEST NAME)
    add_executable(${NAME}.x ${NAME}.cpp)
    target_link_libraries(${NAME}.x marty gtest_main)
    include(GoogleTest)
    gtest_discover_tests(${NAME}.x PROPERTIES LABELS unit)
endfunction()

add_subdirectory(csl)
