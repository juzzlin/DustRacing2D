# Project file for the game
TEMPLATE         = app
TARGET           =
DEPENDPATH      += MiniWidgets MiniCore .
INCLUDEPATH     += MiniWidgets MiniCore .
QMAKE_CXXFLAGS  += -std=c++0x
QT              += opengl

# Input
SOURCES += main.cpp mwscene.cpp \
    car.cpp \
    mainwindow.cpp \
    track.cpp \
    game.cpp \
    trackloader.cpp \
    gamewidgetabs.cpp

HEADERS += mwscene.h \
    car.h \
    mainwindow.h \
    track.h \
    game.h \
    trackloader.h \
    gamewidgetabs.h
