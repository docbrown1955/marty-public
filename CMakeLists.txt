cmake_minimum_required(VERSION 3.22)

project(marty LANGUAGES CXX C Fortran)

set(CMAKE_CXX_STANDARD 23)

set(GNU_COMPILERS 0)
set(CLANG_COMPILERS 0)
set(LLVM_COMPILERS 0)
set(REQUIRED_COMPILER_VERSION 0)
set(IDENTICAL_COMPILER_VERSION 0)

find_package(GSL REQUIRED)

# Dependencies for GRAFED
option(MARTY_REQUIRE_LATEX_TOOLS
    "Require lualatex, latex, convert and dvipng at configure time" ON)
option(MARTY_BUILD_GRAFED_GUI "Build the GRAFED Qt 5 GUI (grafed-gui)" ON)

if(MARTY_REQUIRE_LATEX_TOOLS)
    set(MARTY_TOOL_REQUIRED REQUIRED)
else()
    set(MARTY_TOOL_REQUIRED "")
endif()
find_program(LUALATEX lualatex ${MARTY_TOOL_REQUIRED})
find_program(CONVERT convert ${MARTY_TOOL_REQUIRED})
find_program(LATEX latex ${MARTY_TOOL_REQUIRED})
find_program(DVIPNG dvipng)

if(MARTY_REQUIRE_LATEX_TOOLS)
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

    if(NOT CONVERT)
        message(FATAL_ERROR
            "The 'convert' program is not found but necessary for GRAFED."
            "The 'convert' program is part of the ImageMagick package."
            "Install it before retrying the installation.")
    endif()
else()
    # Library-only builds (for example Python wheels): missing tools are only
    # reported. The MARTY_HAS_* definitions tell the code which ones exist.
    foreach(tool LUALATEX CONVERT LATEX DVIPNG)
        if(NOT ${tool})
            message(WARNING "The '${tool}' program is not found: the "
                "corresponding GRAFED features are unavailable.")
        endif()
    endforeach()
endif()

foreach(tool LUALATEX CONVERT LATEX DVIPNG)
    if(${tool})
        set(MARTY_HAS_${tool} 1)
    else()
        set(MARTY_HAS_${tool} 0)
    endif()
endforeach()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(ENABLE_COVERAGE "Instrument the code with gcov for coverage reports (combine with Debug build type)" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage")
endif()

enable_testing()

add_compile_definitions(MARTY_CXX=${CMAKE_CXX_COMPILER})
add_compile_definitions(MARTY_CC=${CMAKE_C_COMPILER})
add_compile_definitions(MARTY_FC=${CMAKE_Fortran_COMPILER})
add_compile_definitions(MARTY_INSTALL_PATH=${CMAKE_INSTALL_PREFIX})
add_compile_definitions(MARTY_HAS_LUALATEX=${MARTY_HAS_LUALATEX})
add_compile_definitions(MARTY_HAS_CONVERT=${MARTY_HAS_CONVERT})
add_compile_definitions(MARTY_HAS_LATEX=${MARTY_HAS_LATEX})
add_compile_definitions(MARTY_HAS_DVIPNG=${MARTY_HAS_DVIPNG})

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

if (UNIX AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(dep)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)

option(MARTY_BUILD_PYTHON "Build the pymarty Python extension" OFF)
if(MARTY_BUILD_PYTHON)
    add_subdirectory(python)
endif()

install(TARGETS marty marty-static csl csl-static marty-lha marty-lha-static marty-gamma-api marty-boltzmann-solver marty-boltzmann-solver-static
    DESTINATION lib)
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN *.h)
install(FILES ${PROJECT_BINARY_DIR}/libooptools.so ${PROJECT_BINARY_DIR}/libooptools-quad.so
    DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/libooptools-static.a ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    DESTINATION lib)
if(MARTY_BUILD_GRAFED_GUI)
    install(TARGETS grafed-gui
        DESTINATION bin)
endif()
