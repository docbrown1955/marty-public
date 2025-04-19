cmake_minimum_required(VERSION 3.24)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(GSL REQUIRED)

QT5_ADD_RESOURCES(RC_SRC "app.qrc")

set(ALL_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    diagram.h
    diagram.cpp
    curvature.h
    curvature.cpp
    adjacencyMatrix.h
    border.h
    diagramrenderer.h
    diagramrenderer.cpp
    diagramwidget.h
    diagramwidget.cpp
    dialoglatex.h
    dialoglatex.cpp
    edge.h
    edge.cpp
    edgeform.h
    edgeform.cpp
    grid.h
    grid.cpp
    htmlconverter.h
    htmlconverter.cpp
    jsonLoader.h
    jsonLoader.cpp
    jsonObject.h
    latexcompiler.h
    latexcompiler.cpp
    latexLink.h
    latexLink.cpp
    mainwidget.h
    mainwidget.cpp
    node.h
    node.cpp
    nodeform.h
    nodeform.cpp
    pdfoption.h
    pdfsetupdialog.h
    pdfsetupdialog.cpp
    planargraph.h
    planargraph.cpp
    resourcehandler.h
    toolbar.h
    toolbar.cpp
    diagramrenderer.ui
    dialoglatex.ui
    edgeform.ui
    nodeform.ui
    mainwidget.ui
    mainwindow.ui
    pdfsetupdialog.ui
    toolbar.ui
)

if(ANDROID)
    add_library(grafed-gui SHARED ${RC_SRC} ${ALL_SOURCES})
else()
    add_executable(grafed-gui ${RC_SRC} ${ALL_SOURCES})
endif()

target_link_libraries(grafed-gui PRIVATE 
    GSL::gsl 
    GSL::gslcblas 
    Qt5::Widgets 
    Qt5::PrintSupport
)
