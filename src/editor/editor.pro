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
    object.cpp \
    objectloader.cpp \
    tiletypedialog.cpp \
    tileanimator.cpp \
    trackio.cpp \
    tracktile.cpp \
    ../common/config.cpp \
    ../common/map.cpp \
    ../common/objectbase.cpp \
    ../common/objects.cpp \
    ../common/route.cpp \
    ../common/trackdata.cpp \
    ../common/tracktilebase.cpp \

HEADERS += mainwindow.h \
    newtrackdialog.h \
    editordata.h \
    editorview.h \
    editorscene.h \
    object.h \
    objectdata.h \
    objectloader.h \
    tiletypedialog.h \
    tileanimator.h \
    trackio.h \
    tracktile.h \
    ../common/config.h \
    ../common/map.h \
    ../common/objectbase.h \
    ../common/objects.h \
    ../common/route.h \
    ../common/trackdata.h \
    ../common/tracktilebase.h \

RESOURCES += \
    ../../data/images/images.qrc
