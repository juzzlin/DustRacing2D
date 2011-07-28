# Project file for the level editor
TEMPLATE        = app
TARGET          =
DEPENDPATH     += .
INCLUDEPATH    += .
QT             += xml
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp mainwindow.cpp \
    newtrackdialog.cpp \
    trackdata.cpp \
    editorview.cpp \
    editorscene.cpp \
    trackio.cpp \
    tracktile.cpp \
    tiletypedialog.cpp \
    tileanimator.cpp \
    route.cpp \
    map.cpp \
    editor.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    trackdata.h \
    editorview.h \
    editorscene.h \
    trackio.h \
    tracktile.h \
    tiletypedialog.h \
    tileanimator.h \
    route.h \
    map.h \
    editor.h \
    version.h

RESOURCES += \
    ../data/images/images.qrc
