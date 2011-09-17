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
    game.cpp \
    mainwindow.cpp \
    race.cpp \
    renderer.cpp \
    scene.cpp \
    timing.cpp \
    track.cpp \
    trackloader.cpp \

HEADERS += mwscene.h \
    car.h \
    game.h \
    mainwindow.h \
    race.h \
    renderer.h \
    scene.h \
    timing.h \
    track.h \
    trackloader.h \
