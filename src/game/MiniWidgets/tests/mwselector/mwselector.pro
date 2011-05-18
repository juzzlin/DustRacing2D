TEMPLATE        = app
TARGET          =
DEPENDPATH     += . ../..
INCLUDEPATH    += . ../..
QMAKE_CXXFLAGS += -std=c++0x

# Input
HEADERS += mwselector.h mwscene.h mwwidget.h mwobject.h
SOURCES += main.cpp mwselector.cpp mwscene.cpp mwwidget.cpp mwobject.cpp
