cmake_minimum_required(VERSION 3.22)

# Placeholder until MARTY has a release version.
set(PYMARTY_VERSION "0.1.0.dev0")

set(PYBIND11_FINDPYTHON ON)
find_package(Python 3.10 COMPONENTS Interpreter Development.Module REQUIRED)

include(FetchContent)
find_package(pybind11 2.11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.2
    )
    FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(_pymarty src/module.cpp)
target_link_libraries(_pymarty PRIVATE marty-static)
target_compile_definitions(_pymarty PRIVATE PYMARTY_VERSION="${PYMARTY_VERSION}")
set_target_properties(_pymarty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymarty"
)

# Copy the pure Python part next to the extension so that the build directory
# is importable (PYTHONPATH=<build>/python).
file(GLOB PYMARTY_PACKAGE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/*.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/*.pyi"
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/py.typed")
foreach(file ${PYMARTY_PACKAGE_FILES})
    get_filename_component(name ${file} NAME)
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/pymarty/${name}" COPYONLY)
endforeach()

# Python tests (L2) and parity check (L3). Needs pytest in the interpreter
# found above; point Python_EXECUTABLE to a virtual environment to provide it.
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import pytest"
    RESULT_VARIABLE PYMARTY_HAS_PYTEST
    OUTPUT_QUIET ERROR_QUIET)
if(NOT PYMARTY_HAS_PYTEST EQUAL 0)
    message(WARNING "pytest is not available in ${Python_EXECUTABLE}: the "
        "'python' tests will fail. Configure with -DPython_EXECUTABLE=<venv python>.")
endif()

add_test(NAME pymarty.pytest
    COMMAND "${Python_EXECUTABLE}" -m pytest -p no:cacheprovider
        -c "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml"
        --rootdir "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests" -m "not slow"
    # The build directory holds the importable package; the source directory
    # holds a pymarty/ without the compiled module and must not be on sys.path.
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME pymarty.parity
    COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_parity.py")
set_tests_properties(pymarty.pytest pymarty.parity PROPERTIES
    LABELS python
    ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}")

# Wheel layout (scikit-build-core sets SKBUILD): the extension and the pure
# Python files go to <site-packages>/pymarty, the C++ headers and libraries
# installed by the top-level CMakeLists.txt go to pymarty/_data
# (wheel.install-dir in pyproject.toml).
if(SKBUILD)
    install(TARGETS _pymarty DESTINATION "${SKBUILD_PLATLIB_DIR}/pymarty")
    install(FILES ${PYMARTY_PACKAGE_FILES}
        DESTINATION "${SKBUILD_PLATLIB_DIR}/pymarty")
endif()
