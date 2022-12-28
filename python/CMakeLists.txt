find_package(Python EXACT ${PYMARTY_PYTHON_VERSION} COMPONENTS Interpreter Development)
enable_testing()

include(Pytest.cmake)

pytest_discover_tests(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    TEST_PREFIX "PYUNITTEST_"
    TEST_SUFFIX ""
    XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/report
)