# Project file for the game
TEMPLATE         = app
TARGET           =
DEPENDPATH      += MiniWidgets MiniCore .
INCLUDEPATH     += MiniWidgets MiniCore .
QMAKE_CXXFLAGS  += -std=c++0x

# Input
SOURCES += main.cpp mwscene.cpp \
    car.cpp \
    track.cpp \
    game.cpp
HEADERS += mwscene.h \
    car.h \
    track.h \
    game.h
