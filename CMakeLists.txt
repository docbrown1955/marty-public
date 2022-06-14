cmake_minimum_required(VERSION 3.5)

project(marty LANGUAGES CXX C Fortran)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GNU_COMPILERS 0)
set(REQUIRED_COMPILER_VERSION 0)
set(IDENTICAL_COMPILER_VERSION 0)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    set(GNU_COMPILERS 1)
endif()
if ("g++-7" VERSION_LESS CMAKE_CXX_COMPILER_VERSION AND "gcc-7" VERSION_LESS CMAKE_C_COMPILER_VERSION AND "gfortran-7" VERSION_LESS CMAKE_Fortran_COMPILER_VERSION)
    set(REQUIRED_COMPILER_VERSION 1)
endif()
if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL CMAKE_C_COMPILER_VERSION AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL CMAKE_Fortran_COMPILER_VERSION)
    set(IDENTICAL_COMPILER_VERSION 1)
endif()
if(REQUIRED_COMPILER_VERSION AND GNU_COMPILERS AND IDENTICAL_COMPILER_VERSION)
  message("Successful compiler configuration")
  message("> Using CXX: ${CMAKE_CXX_COMPILER_VERSION}")
  message("> Using CC : ${CMAKE_C_COMPILER_VERSION}")
  message("> Using FC : ${CMAKE_Fortran_COMPILER_VERSION}")
else()
    if (${GNU_COMPILERS} EQUAL 0)
        set(error "The build system requires GNU compilers (gcc/g++/gfortran)")
    elseif(${REQUIRED_COMPILER_VERSION} EQUAL 0)
        set(error "The build system requires compilers versions >= 7")
    elseif (${IDENTICAL_COMPILER_VERSION} EQUAL 0)
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
        "    CXX: ${CMAKE_CXX_COMPILER}\n"
        "    CC : ${CMAKE_C_COMPILER}\n"
        "    FC : ${CMAKE_Fortran_COMPILER}\n"
    )
endif()

add_subdirectory(dep)
add_subdirectory(src)

add_library(marty INTERFACE)

target_link_libraries(marty INTERFACE csl grafed marty-core)
