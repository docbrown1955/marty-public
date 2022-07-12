
if(ENABLE_COVERAGE)
    include(CodeCoverage.cmake)
endif()

add_subdirectory(units)
add_subdirectory(system)

if(ENABLE_COVERAGE)
    set(UNIT_TEST_LABEL "units")
    set(SYSTEM_TEST_LABEL "system")
    SETUP_TARGET_FOR_COVERAGE(${UNIT_TEST_LABEL})
    SETUP_TARGET_FOR_COVERAGE(${SYSTEM_TEST_LABEL})
endif()
