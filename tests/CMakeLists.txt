include(CodeCoverage.cmake)
add_subdirectory(units)
add_subdirectory(system)

set(UNIT_TEST_LABEL "units")
set(SYSTEM_TEST_LABEL "system")

SETUP_TARGET_FOR_COVERAGE(${UNIT_TEST_LABEL})
SETUP_TARGET_FOR_COVERAGE(${SYSTEM_TEST_LABEL})
