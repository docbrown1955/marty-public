set(ALL_SOURCES
    gamma_api.cpp
)

add_definitions(-DPYBIND11)
pybind11_add_module(_pymarty ${ALL_SOURCES} MODULE NO_EXTRAS)
target_link_libraries(_pymarty PRIVATE marty-static pybind11::module)

set_target_properties(_pymarty
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python/src/pymarty"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python/src/pymarty"
)