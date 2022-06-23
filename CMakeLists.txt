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

enable_testing()

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

add_compile_definitions(MARTY_CXX=${CMAKE_CXX_COMPILER})
add_compile_definitions(MARTY_CC=${CMAKE_C_COMPILER})
add_compile_definitions(MARTY_FC=${CMAKE_Fortran_COMPILER})
add_compile_definitions(MARTY_INSTALL_PATH=${CMAKE_INSTALL_PREFIX})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(dep)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)

add_library(csl SHARED)
target_link_libraries(csl csl-core)
target_link_libraries(csl GSL::gsl GSL::gslcblas)

add_library(csl-static STATIC)
target_link_libraries(csl-static csl-core)
target_link_libraries(csl-static GSL::gsl GSL::gslcblas)

add_library(marty-lha SHARED)
target_link_libraries(marty-lha marty-lha-obj)
target_link_libraries(marty-lha marty-lha-obj)

add_library(marty-lha-static STATIC)
target_link_libraries(marty-lha-static marty-lha-obj)
target_link_libraries(marty-lha-static marty-lha-obj)

add_dependencies(marty-core lt_header)
add_library(marty SHARED)
target_link_libraries(marty csl-core grafed-core jsonparser marty-lha-obj marty-core sgl marty-models)
target_link_libraries(marty GSL::gsl GSL::gslcblas)
target_link_libraries(marty ${PROJECT_BINARY_DIR}/libooptools.so)
add_dependencies(marty lt_lib)

add_library(marty-static STATIC)
target_link_libraries(marty-static csl-core grafed-core jsonparser marty-lha-obj marty-core sgl marty-models)
target_link_libraries(marty-static GSL::gsl GSL::gslcblas)
target_link_libraries(marty-static ${PROJECT_BINARY_DIR}/libooptools.a)
add_dependencies(marty-static lt_lib_static)

install(TARGETS marty marty-static csl csl-static marty-lha marty-lha-static
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
