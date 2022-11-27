set(ALL_SOURCES
    charge.h
    colorcoupling.h
    diracmatrix.h
    electriccharge.cpp
    electriccharge.h
    field.h
    lorentzcoupling.h
    marty-ufo.h
    parsefactors.cpp
    parsefactors.h
    simplifiedvertex.cpp
    simplifiedvertex.h
    ufosmdata.h
    vertexinfo.h
)

add_library(marty-ufo OBJECT ${ALL_SOURCES})