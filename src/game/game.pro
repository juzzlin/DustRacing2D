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
    track.cpp \
    trackloader.cpp \

HEADERS += mwscene.h \
    car.h \
    game.h \
    mainwindow.h \
    track.h \
    trackloader.h \
