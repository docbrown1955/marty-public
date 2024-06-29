add_library(boltzmann_solver-core OBJECT
    libjsondata.cpp
    processdata.cpp
    quantumnumberdata.cpp
)
set_property(TARGET boltzmann_solver-core PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(boltzmann_solver SHARED)
target_link_libraries(boltzmann_solver boltzmann_solver-core)

add_library(boltzmann_solver-static STATIC)
target_link_libraries(boltzmann_solver-static boltzmann_solver-core)