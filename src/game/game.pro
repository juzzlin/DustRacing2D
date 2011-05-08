# Project file for the game
TEMPLATE         = app
TARGET           =
DEPENDPATH      += .
INCLUDEPATH     += .
QMAKE_CXXFLAGS  += -std=c++0x

# Input
SOURCES += main.cpp \
    MiniWidgets/mwobject.cpp \
    MiniWidgets/mwwidget.cpp \
    MiniWidgets/mwmenu.cpp \
    MiniWidgets/mwtext.cpp \
    MiniWidgets/mwscene.cpp

HEADERS += \
    MiniWidgets/mwobject.h \
    MiniWidgets/mwwidget.h \
    MiniWidgets/mwmenu.h \
    MiniWidgets/mwtext.h \
    MiniWidgets/mwscene.h
