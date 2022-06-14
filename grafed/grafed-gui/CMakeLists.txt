cmake_minimum_required(VERSION 3.5)

project(grafed-gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

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

target_link_libraries(grafed-gui PRIVATE GSL::gsl GSL::gslcblas Qt5::Widgets Qt5::PrintSupport)
