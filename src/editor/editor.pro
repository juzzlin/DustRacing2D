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
    route.cpp \
    map.cpp \
    ../common/trackdata.cpp \
    ../common/trackio.cpp \
    ../common/tracktile.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    editordata.h \
    editorview.h \
    editorscene.h \
    tiletypedialog.h \
    tileanimator.h \
    route.h \
    map.h \
    version.h \
    ../common/trackdata.h \
    ../common/trackio.h \
    ../common/tracktile.h

RESOURCES += \
    ../../data/images/images.qrc
