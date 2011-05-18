TEMPLATE        = app
TARGET          =
DEPENDPATH     += . ../..
INCLUDEPATH    += . ../..
QMAKE_CXXFLAGS += -std=c++0x

# Input
HEADERS += mwtext.h mwscene.h mwwidget.h mwobject.h
SOURCES += main.cpp mwtext.cpp mwscene.cpp mwwidget.cpp mwobject.cpp
