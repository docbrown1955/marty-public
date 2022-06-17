cmake_minimum_required(VERSION 3.5)

project(marty LANGUAGES CXX C Fortran)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GNU_COMPILERS 0)
set(REQUIRED_COMPILER_VERSION 0)
set(IDENTICAL_COMPILER_VERSION 0)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    set(GNU_COMPILERS 1)
endif()

if("g++-7" VERSION_LESS CMAKE_CXX_COMPILER_VERSION AND "gcc-7" VERSION_LESS CMAKE_C_COMPILER_VERSION AND "gfortran-7" VERSION_LESS CMAKE_Fortran_COMPILER_VERSION)
    set(REQUIRED_COMPILER_VERSION 1)
endif()

if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL CMAKE_C_COMPILER_VERSION AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL CMAKE_Fortran_COMPILER_VERSION)
    set(IDENTICAL_COMPILER_VERSION 1)
endif()

if(REQUIRED_COMPILER_VERSION AND GNU_COMPILERS AND IDENTICAL_COMPILER_VERSION)
    message("Successful compiler configuration")
    message("> Using CXX: ${CMAKE_CXX_COMPILER_VERSION}")
    message("> Using CC : ${CMAKE_C_COMPILER_VERSION}")
    message("> Using FC : ${CMAKE_Fortran_COMPILER_VERSION}")
else()
    if(${GNU_COMPILERS} EQUAL 0)
        set(error "The build system requires GNU compilers (gcc/g++/gfortran)")
    elseif(${REQUIRED_COMPILER_VERSION} EQUAL 0)
        set(error "The build system requires compilers versions >= 7")
    elseif(${IDENTICAL_COMPILER_VERSION} EQUAL 0)
        set(error "The build system requires identical gcc/g++/gfortran versions to link programs correctly")
    endif()

    message(FATAL_ERROR
        "${error}\n"
        "An exemple of valid compiler setup is (with X >= 7):\n"
        "    gcc-X / g++-X / gfortran-X\n"
        "You should make sure to install the required compilers and set the full paths before calling cmake:\n"
        "    $ export CXX=/usr/bin/g++-10\n"
        "    $ export CC=/usr/bin/gcc-10\n"
        "    $ export FC=/usr/bin/gfortran-10\n"
        "    $ rm CMakeCache.txt  # resets the wrong compiler configurations\n"
        "    $ cmake ..\n"
        "The current defined compilers are:\n"
        "    CXX: ${CMAKE_CXX_COMPILER} (version ${CMAKE_CXX_COMPILER_VERSION})\n"
        "    CC : ${CMAKE_C_COMPILER} (version ${CMAKE_C_COMPILER_VERSION})\n"
        "    FC : ${CMAKE_Fortran_COMPILER} (version ${CMAKE_Fortran_COMPILER_VERSION})\n"
    )
endif()

find_package(GSL REQUIRED)

if(NOT DEFINED PYMARTY)
    set(PYMARTY 1)
endif()

if(PYMARTY)
    find_package(Python3 REQUIRED)
    find_package(pybind11 CONFIG REQUIRED)
endif()

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

add_compile_definitions(MARTY_CXX=${CMAKE_CXX_COMPILER})
add_compile_definitions(MARTY_CC=${CMAKE_C_COMPILER})
add_compile_definitions(MARTY_FC=${CMAKE_Fortran_COMPILER})

add_subdirectory(dep)
add_subdirectory(src)
add_subdirectory(examples)

add_library(csl SHARED)
target_link_libraries(csl csl-core)
target_link_libraries(csl GSL::gsl GSL::gslcblas)

add_library(csl-static STATIC)
target_link_libraries(csl-static csl-core)
target_link_libraries(csl-static GSL::gsl GSL::gslcblas)

add_dependencies(marty-core lt_header)
add_library(marty SHARED)
target_link_libraries(marty csl-core grafed-core jsonparser marty-core sgl marty-models)
target_link_libraries(marty GSL::gsl GSL::gslcblas)
target_link_libraries(marty ${PROJECT_BINARY_DIR}/libooptools.so)
add_dependencies(marty lt_lib)

add_library(marty-static STATIC)
target_link_libraries(marty-static csl-core grafed-core jsonparser marty-core sgl marty-models)
target_link_libraries(marty-static GSL::gsl GSL::gslcblas)
target_link_libraries(marty-static ${PROJECT_BINARY_DIR}/libooptools.a)
add_dependencies(marty-static lt_lib_static)

install(TARGETS marty marty-static csl csl-static
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
