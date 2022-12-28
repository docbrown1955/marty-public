cmake_minimum_required(VERSION 3.5)

project(marty LANGUAGES CXX C Fortran)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GNU_COMPILERS 0)
set(CLANG_COMPILERS 0)
set(LLVM_COMPILERS 0)
set(REQUIRED_COMPILER_VERSION 0)
set(IDENTICAL_COMPILER_VERSION 0)

find_package(GSL REQUIRED)

# Dependencies for GRAFED
find_program(LUALATEX lualatex REQUIRED)
find_program(CONVERT convert REQUIRED)
find_program(LATEX latex REQUIRED)
find_program(DVIPNG dvipng)

if(NOT LUALATEX)
    message(FATAL_ERROR
        "The 'lualatex' program is not found but necessary for GRAFED."
        "Install it before retrying the installation.")
endif()

if(NOT DVIPNG)
    message(FATAL_ERROR
        "The 'dvipng' program is not found but necessary for GRAFED."
        "Install it before retrying the installation.")
endif()

if(NOT LUALATEX)
    message(FATAL_ERROR
        "The 'lualatex' program is not found but necessary for GRAFED."
        "Install it before retrying the installation.")
endif()

if(NOT CONVERT)
    message(FATAL_ERROR
        "The 'convert' program is not found but necessary for GRAFED."
        "The 'convert' program is part of the ImageMagick package."
        "Install it before retrying the installation.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE)

enable_testing()

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(ENABLE_COVERAGE 1)
else()
    set(ENABLE_COVERAGE 0)
endif()

if ((NOT PYMARTY_PYTHON_VERSION))
    set(PYMARTY_PYTHON_VERSION 3.9)
endif()

add_compile_definitions(MARTY_CXX=${CMAKE_CXX_COMPILER})
add_compile_definitions(MARTY_CC=${CMAKE_C_COMPILER})
add_compile_definitions(MARTY_FC=${CMAKE_Fortran_COMPILER})
add_compile_definitions(MARTY_INSTALL_PATH=${CMAKE_INSTALL_PREFIX})

if((NOT CMAKE_COMPILER_IS_GNUCXX) OR(NOT CMAKE_COMPILER_IS_GNUCC))
    message(WARNING
        "Quadruple precision for numerical library generation can only be used with GNU compilers for now."
        " The compilers defined are not GNU (${CMAKE_C_COMPILER} and ${CMAKE_CXX_COMPILER}), change the C/C++ compilers to GNU to enable quadruple precision."
        " The build procedure will continue normally but the quadruple precision feature has been disabled."
    )
    add_compile_definitions(MARTY_QUADRUPLE_ENABLED=0)
    set(QUADRUPLE_ENABLED 0)
else()
    add_compile_definitions(MARTY_QUADRUPLE_ENABLED=1)
    set(QUADRUPLE_ENABLED 1)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(dep)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(python)
add_subdirectory(tests)

install(TARGETS marty marty-static csl csl-static marty-lha marty-lha-static marty-gamma-api
    DESTINATION lib)
install(PROGRAMS "$<TARGET_FILE:_pymarty>"
    DESTINATION lib)
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN *.h)
install(FILES ${PROJECT_BINARY_DIR}/libooptools.so ${PROJECT_BINARY_DIR}/libooptools-quad.so
    DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/libooptools-static.a ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    DESTINATION lib)
install(TARGETS grafed-gui
    DESTINATION bin)
