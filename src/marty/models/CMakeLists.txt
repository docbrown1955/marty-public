set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    QED.h QED.cpp
    QCD.h QCD.cpp
    SM.h SM.cpp
    2HDM.h 2HDM.cpp
    GTHDM.h GTHDM.cpp
    MSSM.h MSSMData.h MSSM.cpp
    PMSSM.h PMSSM.cpp
    NMFV.h NMFV.cpp

    PMSSM_LEM.h
    pmssm_lem/pmssm_content.cpp
    pmssm_lem/pmssm_data.h
    pmssm_lem/pmssm_interaction.cpp
    pmssm_lem/pmssm_kinetic.cpp
    pmssm_lem/pmssm_lem.h
    pmssm_lem/pmssm_mass.cpp
)

SET_SOURCE_FILES_PROPERTIES(
    pmssm_lem/pmssm_content.cpp
    pmssm_lem/pmssm_interaction.cpp
    pmssm_lem/pmssm_kinetic.cpp
    pmssm_lem/pmssm_mass.cpp
    PROPERTIES COMPILE_FLAGS -O0)

add_library(marty-models OBJECT ${ALL_SOURCES})
set_property(TARGET marty-models PROPERTY POSITION_INDEPENDENT_CODE 1)
