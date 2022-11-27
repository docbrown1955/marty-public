if ((NOT PYMARTY_PYTHON_VERSION))
    set(PYMARTY_PYTHON_VERSION 3.9)
endif()

find_package(Python3 EXACT ${PYMARTY_PYTHON_VERSION} REQUIRED COMPONENTS Interpreter Development)
set(PYBIND11_PYTHON_VERSION ${PYMARTY_PYTHON_VERSION} CACHE STRING "")
find_package(pybind11 CONFIG REQUIRED)

set(ALL_SOURCES
    pymartyc.cpp
)

add_definitions(-DPYBIND11)
pybind11_add_module(pymartyc ${ALL_SOURCES} MODULE NO_EXTRAS)
target_link_libraries(pymartyc PRIVATE marty pybind11::module)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/pymartyc.so
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:pymartyc>" "${CMAKE_BINARY_DIR}/pymartyc.so"
    DEPENDS pymartyc
)
add_custom_target(pymarty_generic_so ALL 
    DEPENDS ${CMAKE_BINARY_DIR}/pymartyc.so)