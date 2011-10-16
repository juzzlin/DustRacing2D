# Project file for the level editor
TEMPLATE     = app
TARGET       = ../../bin/editor/editor
INCLUDEPATH += .
QT          += xml

# Doesn't compile on Windows with mingw32 without -U__STRICT_ANSI__
# if -std=c++0x is defined.
QMAKE_CXXFLAGS += -std=c++0x -U__STRICT_ANSI__

SOURCES += main.cpp mainwindow.cpp \
    newtrackdialog.cpp \
    editordata.cpp \
    editorview.cpp \
    editorscene.cpp \
    objectloader.cpp \
    tiletypedialog.cpp \
    tileanimator.cpp \
    trackio.cpp \
    tracktile.cpp \
    version.cpp \
    ../common/config.cpp \
    ../common/map.cpp \
    ../common/route.cpp \
    ../common/trackdata.cpp \
    ../common/tracktilebase.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    editordata.h \
    editorview.h \
    editorscene.h \
    objectloader.h \
    tiletypedialog.h \
    tileanimator.h \
    trackio.h \
    tracktile.h \
    version.h \
    ../common/config.h \
    ../common/map.h \
    ../common/route.h \
    ../common/trackdata.h \
    ../common/tracktilebase.h \
    objectdata.h

RESOURCES += \
    ../../data/images/images.qrc
