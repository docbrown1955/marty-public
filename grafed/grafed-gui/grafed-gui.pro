#-------------------------------------------------
#
# Project created by QtCreator 2020-08-03T15:48:11
#
#-------------------------------------------------


QT             += core gui widgets
#QMAKE_CXX = g++-7
#QMAKE_LINK = g++-7
QMAKE_CXXFLAGS += -std=c++17

TARGET = grafed-gui
TEMPLATE = app

INCLUDEPATH += /usr/local/include

LIBS += -L"/usr/local/lib" -lgsl -lgslcblas

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mainwidget.cpp \
    diagramrenderer.cpp \
    diagramwidget.cpp \
    node.cpp \
    edge.cpp \
    curvature.cpp \
    htmlconverter.cpp \
    dialoglatex.cpp \
    toolbar.cpp \
    grid.cpp \
    diagram.cpp \
    edgeform.cpp \
    nodeform.cpp \
    latexcompiler.cpp \
    jsonLoader.cpp \
    planargraph.cpp \
    latexLink.cpp

HEADERS += \
    mainwindow.h \
    mainwidget.h \
    diagramrenderer.h \
    diagramwidget.h \
    node.h \
    edge.h \
    curvature.h \
    htmlconverter.h \
    dialoglatex.h \
    toolbar.h \
    grid.h \
    diagram.h \
    border.h \
    edgeform.h \
    nodeform.h \
    latexcompiler.h \
    resourcehandler.h \
    jsonObject.h \
    jsonLoader.h \
    planargraph.h \
    latexLink.h \
    adjacencyMatrix.h

FORMS += \
        mainwindow.ui \
    mainwidget.ui \
    diagramrenderer.ui \
    dialoglatex.ui \
    toolbar.ui \
    edgeform.ui \
    nodeform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += app.qrc
RC_FILE = app.qrc
