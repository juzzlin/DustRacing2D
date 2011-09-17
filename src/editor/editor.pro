# Project file for the level editor
TEMPLATE        = app
TARGET          =
DEPENDPATH     += .
INCLUDEPATH    += .
QT             += xml
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp mainwindow.cpp \
    newtrackdialog.cpp \
    editordata.cpp \
    editorview.cpp \
    editorscene.cpp \
    map.cpp \
    route.cpp \
    tiletypedialog.cpp \
    tileanimator.cpp \
    trackdata.cpp \
    trackio.cpp \
    tracktile.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    editordata.h \
    editorview.h \
    editorscene.h \
    map.h \
    route.h \
    tiletypedialog.h \
    tileanimator.h \
    trackdata.h \
    trackio.h \
    tracktile.h \
    version.h

RESOURCES += \
    ../../data/images/images.qrc
