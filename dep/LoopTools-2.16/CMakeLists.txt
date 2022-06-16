cmake_minimum_required(VERSION 3.5)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/clooptools.h ${PROJECT_BINARY_DIR}/libooptools.so ${PROJECT_BINARY_DIR}/libooptools-static.a
    COMMAND export FFLAGS=-fPIC
    COMMAND export CFLAGS=-fPIC
    COMMAND export CXXFLAGS=-fPIC
    COMMAND rm -rf build
    COMMAND ./configure FFLAGS="-fPIC"
    COMMAND make
    COMMAND cp build/libooptools.a ${PROJECT_BINARY_DIR}/libooptools-static.a
    COMMAND cp build/clooptools.h ${PROJECT_BINARY_DIR}/clooptools.h
    COMMAND ${CMAKE_Fortran_COMPILER} -shared build/*.o -o ${PROJECT_BINARY_DIR}/libooptools.so
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/libooptools-quad.so ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    COMMAND export FFLAGS=-fPIC
    COMMAND export CFLAGS=-fPIC
    COMMAND export CXXFLAGS=-fPIC
    COMMAND rm -rf build-quad
    COMMAND ./configure --quad FFLAGS="-fpic"
    COMMAND make
    COMMAND cp build-quad/libooptools-quad.a ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
    COMMAND ${CMAKE_Fortran_COMPILER} -shared build-quad/*.o -o ${PROJECT_BINARY_DIR}/libooptools-quad.so
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(lt_header
    DEPENDS ${PROJECT_BINARY_DIR}/clooptools.h
)
add_custom_target(lt_lib
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools.so
)
add_custom_target(lt_qlib
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-quad.so
)
add_custom_target(lt_lib_static
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-static.a
)
add_custom_target(lt_qlib_static
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
)

add_custom_target(looptools ALL
    DEPENDS ${PROJECT_BINARY_DIR}/clooptools.h
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools.so
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-quad.so
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-static.a
    DEPENDS ${PROJECT_BINARY_DIR}/libooptools-quad-static.a
)
