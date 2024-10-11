add_library(marty-boltzmann-solver-core OBJECT
    libjsondata.cpp
    processdata.cpp
    quantumnumberdata.cpp
)
set_property(TARGET marty-boltzmann-solver-core PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(marty-boltzmann-solver SHARED)
target_link_libraries(marty-boltzmann-solver PRIVATE marty-boltzmann-solver-core)
target_link_libraries(marty-boltzmann-solver PUBLIC marty)

add_library(marty-boltzmann-solver-static STATIC)
target_link_libraries(marty-boltzmann-solver-static PRIVATE marty-boltzmann-solver-core)
target_link_libraries(marty-boltzmann-solver-static PUBLIC marty-static)
