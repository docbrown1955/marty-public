if(ENABLE_COVERAGE)
    include(CodeCoverage.cmake)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    endif()
endif()

function(SETUP_MARTY_TEST_TARGET TARGET)
    target_compile_definitions(${TARGET} PRIVATE
        MARTY_LIBRARY_PATH="${CMAKE_BINARY_DIR}/src"
        MARTY_INCLUDE_PATH="${CMAKE_SOURCE_DIR}/src"
    )
    target_link_libraries(${TARGET} marty)
endfunction()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

FetchContent_MakeAvailable(googletest)

function (SETUP_GTEST_TARGET TARGET LABEL)
    target_link_libraries(${TARGET} GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(${TARGET} PROPERTIES LABELS ${LABEL})
endfunction()

add_subdirectory(units)
add_subdirectory(integration)
add_subdirectory(system)

if(ENABLE_COVERAGE)
    set(UNIT_TEST_LABEL "units")
    set(INTEGRATION_TEST_LABEL "integration")
    set(SYSTEM_TEST_LABEL "system")
    SETUP_TARGET_FOR_COVERAGE(${UNIT_TEST_LABEL})
    SETUP_TARGET_FOR_COVERAGE(${INTEGRATION_TEST_LABEL})
    SETUP_TARGET_FOR_COVERAGE(${SYSTEM_TEST_LABEL})
endif()
