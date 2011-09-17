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
    renderer.cpp \
    track.cpp \
    trackloader.cpp \
    scene.cpp \
    game.cpp

HEADERS += mwscene.h \
    car.h \
    mainwindow.h \
    renderer.h \
    track.h \
    trackloader.h \
    scene.h \
    game.h
