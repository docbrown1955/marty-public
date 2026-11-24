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

pybind11_add_module(_pymarty src/module.cpp src/symbolic.cpp src/diracology.cpp src/group_theory.cpp src/modeling.cpp src/calculation.cpp)
target_link_libraries(_pymarty PRIVATE marty-static)
target_compile_definitions(_pymarty PRIVATE PYMARTY_VERSION="${PYMARTY_VERSION}")
set_target_properties(_pymarty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymarty"
    # Installed wheel: libooptools.so is shipped in pymarty/_data/lib (see
    # wheel.install-dir in pyproject.toml). The build tree keeps its own RPATH.
    INSTALL_RPATH "$ORIGIN/_data/lib"
)

# Copy the pure Python part next to the extension so that the build directory
# is importable (PYTHONPATH=<build>/python).
file(GLOB_RECURSE PYMARTY_PACKAGE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/*.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/*.pyi"
    "${CMAKE_CURRENT_SOURCE_DIR}/pymarty/py.typed")
foreach(file ${PYMARTY_PACKAGE_FILES})
    file(RELATIVE_PATH relative_path "${CMAKE_CURRENT_SOURCE_DIR}/pymarty" ${file})
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/pymarty/${relative_path}" COPYONLY)
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

# Workaround: the full run segfaults in symbolic/test_parity.py when it shares
# a process with the earlier tests (see docs/architecture/pymarty/phase-4/
# 04-implementation-log.md, "Known issues"). That file is run on its own below.
add_test(NAME pymarty.pytest
    COMMAND "${Python_EXECUTABLE}" -m pytest -p no:cacheprovider
        -c "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml"
        --rootdir "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        "--ignore=${CMAKE_CURRENT_SOURCE_DIR}/tests/symbolic/test_parity.py"
        -m "not slow"
    # The build directory holds the importable package; the source directory
    # holds a pymarty/ without the compiled module and must not be on sys.path.
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME pymarty.pytest.symbolic_parity
    COMMAND "${Python_EXECUTABLE}" -m pytest -p no:cacheprovider
        -c "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml"
        --rootdir "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/symbolic/test_parity.py" -m "not slow"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME pymarty.parity
    COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_parity.py")
set_tests_properties(pymarty.pytest pymarty.pytest.symbolic_parity pymarty.parity PROPERTIES
    LABELS python
    ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}")

# Wheel layout (scikit-build-core sets SKBUILD): the extension and the pure
# Python files go to <site-packages>/pymarty, the C++ headers and libraries
# installed by the top-level CMakeLists.txt go to pymarty/_data
# (wheel.install-dir in pyproject.toml).
if(SKBUILD)
    install(TARGETS _pymarty DESTINATION "${SKBUILD_PLATLIB_DIR}/pymarty")
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymarty/"
        DESTINATION "${SKBUILD_PLATLIB_DIR}/pymarty"
        FILES_MATCHING PATTERN "*.py" PATTERN "*.pyi" PATTERN "py.typed")
endif()
