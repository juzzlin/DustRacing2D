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
    tiletypedialog.cpp \
    tileanimator.cpp \
    trackio.cpp \
    tracktile.cpp \
    ../common/map.cpp \
    ../common/route.cpp \
    ../common/trackdata.cpp \
    ../common/tracktilebase.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    editordata.h \
    editorview.h \
    editorscene.h \
    tiletypedialog.h \
    tileanimator.h \
    trackio.h \
    tracktile.h \
    version.h \
    ../common/map.h \
    ../common/route.h \
    ../common/trackdata.h \
    ../common/tracktilebase.h

RESOURCES += \
    ../../data/images/images.qrc
