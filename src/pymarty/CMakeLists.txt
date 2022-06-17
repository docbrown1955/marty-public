set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    simple_py.cpp
)

pybind11_add_module(pymarty ${ALL_SOURCES})
target_link_libraries(pymarty PRIVATE marty)
