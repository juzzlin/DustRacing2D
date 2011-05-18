# Project file for the game
TEMPLATE         = app
TARGET           =
DEPENDPATH      += MiniWidgets .
INCLUDEPATH     += MiniWidgets .
QMAKE_CXXFLAGS  += -std=c++0x

# Input
SOURCES += main.cpp mwscene.cpp
HEADERS += mwscene.h
