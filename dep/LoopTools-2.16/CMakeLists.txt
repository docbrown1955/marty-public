cmake_minimum_required(VERSION 3.5)

if (LOOPTOOLS_DEBUG)
    set(LOOPTOOLS_OUTPUT_REDIRECT "")
else()
    set(LOOPTOOLS_OUTPUT_REDIRECT > /dev/null 2>&1)
endif()

add_custom_command(
    OUTPUT
    ${PROJECT_BINARY_DIR}/src/clooptools.h
    ${PROJECT_BINARY_DIR}/libooptools.so
    ${PROJECT_BINARY_DIR}/libooptools-static.a
    ${PROJECT_SOURCE_DIR}/src/clooptools.h
    COMMAND export FFLAGS=-fPIC
    COMMAND export CFLAGS=-fPIC
    COMMAND export CXXFLAGS=-fPIC
    COMMAND rm -rf build
    COMMAND ./configure FFLAGS="-fPIC" ${LOOPTOOLS_OUTPUT_REDIRECT}
    COMMAND make ${LOOPTOOLS_OUTPUT_REDIRECT}
    COMMAND cp build/libooptools.a ${PROJECT_BINARY_DIR}/libooptools-static.a
    COMMAND cp build/clooptools.h ${PROJECT_BINARY_DIR}/src
    COMMAND cp build/clooptools.h ${PROJECT_SOURCE_DIR}/src/
    COMMAND ${CMAKE_Fortran_COMPILER} -shared build/*.o -o ${PROJECT_BINARY_DIR}/libooptools.so ${LOOPTOOLS_OUTPUT_REDIRECT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/libooptools-quad.so ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    COMMAND export FFLAGS=-fPIC
    COMMAND export CFLAGS=-fPIC
    COMMAND export CXXFLAGS=-fPIC
    COMMAND rm -rf build-quad
    COMMAND ./configure --quad FFLAGS="-fpic" ${LOOPTOOLS_OUTPUT_REDIRECT}
    COMMAND make ${LOOPTOOLS_OUTPUT_REDIRECT}
    COMMAND cp build-quad/libooptools-quad.a ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    COMMAND ${CMAKE_Fortran_COMPILER} -shared build-quad/*.o -o ${PROJECT_BINARY_DIR}/libooptools-quad.so ${LOOPTOOLS_OUTPUT_REDIRECT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${PROJECT_BINARY_DIR}/clooptools.h
)

add_custom_target(looptools ALL
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools.so
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-quad.so
)
