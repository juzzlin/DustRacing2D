# Project file for the game
TEMPLATE     = app
TARGET       = ../../bin/game/dustrac
INCLUDEPATH += MiniCore .
QT          += opengl xml

# Doesn't compile on Windows with mingw32 without -U__STRICT_ANSI__
# if -std=c++0x is defined.
QMAKE_CXXFLAGS  += -std=c++0x -U__STRICT_ANSI__

# Input
SOURCES += main.cpp \
    car.cpp \
    game.cpp \
    inputhandler.cpp \
    mainwindow.cpp \
    race.cpp \
    renderer.cpp \
    scene.cpp \
    slidefrictiongenerator.cpp \
    timing.cpp \
    track.cpp \
    trackloader.cpp \
    tracktile.cpp \
    ../common/config.cpp \
    ../common/map.cpp \
    ../common/objects.cpp \
    ../common/route.cpp \
    ../common/trackdata.cpp \
    ../common/tracktilebase.cpp \

# Engine
SOURCES += MiniCore/Core/mccamera.cc \
    MiniCore/Core/mccircleshape.cc \
    MiniCore/Core/mccollisionevent.cc \
    MiniCore/Core/mccontact.cc \
    MiniCore/Core/mccontactresolver.cc \
    MiniCore/Core/mcdragforcegenerator.cc \
    MiniCore/Core/mcevent.cc \
    MiniCore/Core/mcexception.cc \
    MiniCore/Core/mcforcegenerator.cc \
    MiniCore/Core/mcforceregistry.cc \
    MiniCore/Core/mcfrictiongenerator.cc \
    MiniCore/Core/mcglcircleparticle.cc \
    MiniCore/Core/mcglobjectdata.cc \
    MiniCore/Core/mcglrectparticle.cc \
    MiniCore/Core/mcgravitygenerator.cc \
    MiniCore/Core/mclogger.cc \
    MiniCore/Core/mcobject.cc \
    MiniCore/Core/mcobjectdata.cc \
    MiniCore/Core/mcobjectfactory.cc \
    MiniCore/Core/mcoutofboundariesevent.cc \
    MiniCore/Core/mcparticle.cc \
    MiniCore/Core/mcquadtree.cc \
    MiniCore/Core/mcrandom.cc \
    MiniCore/Core/mcrectshape.cc \
    MiniCore/Core/mcshape.cc \
    MiniCore/Core/mcspringforcegenerator2dfast.cc \
    MiniCore/Core/mcspringforcegenerator.cc \
    MiniCore/Core/mcsurface.cc \
    MiniCore/Core/mcsurfaceobjectdata.cc \
    MiniCore/Core/mcsurfaceparticle.cc \
    MiniCore/Core/mcsurfaceview.cc \
    MiniCore/Core/mctrigonom.cc \
    MiniCore/Core/mcworld.cc \
    MiniCore/Util/mctextureconfigloader.cc \
    MiniCore/Util/mctexturemanager.cc \

# Game headers
HEADERS += car.h \
    game.h \
    inputhandler.h \
    layers.h \
    mainwindow.h \
    race.h \
    renderer.h \
    scene.h \
    slidefrictiongenerator.h \
    timing.h \
    track.h \
    trackloader.h \
    tracktile.h \
    ../common/config.h \
    ../common/map.h \
    ../common/objects.h \
    ../common/route.h \
    ../common/trackdata.h \
    ../common/tracktilebase.h \

# Engine headers
HEADERS += MiniCore/Core/mccamera.hh \
    MiniCore/Core/mccircleshape.hh \
    MiniCore/Core/mccollisionevent.hh \
    MiniCore/Core/mccontact.hh \
    MiniCore/Core/mccontactresolver.hh \
    MiniCore/Core/mcdragforcegenerator.hh \
    MiniCore/Core/mcevent.hh \
    MiniCore/Core/mcexception.hh \
    MiniCore/Core/mcforcegenerator.hh \
    MiniCore/Core/mcforceregistry.hh \
    MiniCore/Core/mcfrictiongenerator.hh \
    MiniCore/Core/mcglcircleparticle.hh \
    MiniCore/Core/mcglobjectdata.hh \
    MiniCore/Core/mcglrectparticle.hh \
    MiniCore/Core/mcgravitygenerator.hh \
    MiniCore/Core/mclogger.hh \
    MiniCore/Core/mcmathutil.hh \
    MiniCore/Core/mcobject.hh \
    MiniCore/Core/mcobjectfactory.hh \
    MiniCore/Core/mcobjectdata.hh \
    MiniCore/Core/mcoutofboundariesevent.hh \
    MiniCore/Core/mcparticle.hh \
    MiniCore/Core/mcquadtree.hh \
    MiniCore/Core/mcrandom.hh \
    MiniCore/Core/mcrectshape.hh \
    MiniCore/Core/mcrecycler.hh \
    MiniCore/Core/mcshape.hh \
    MiniCore/Core/mcspringforcegenerator2dfast.hh \
    MiniCore/Core/mcspringforcegenerator.hh \
    MiniCore/Core/mcsurface.hh \
    MiniCore/Core/mcsurfaceobjectdata.hh \
    MiniCore/Core/mcsurfaceparticle.hh \
    MiniCore/Core/mcsurfaceview.hh \
    MiniCore/Core/mctrigonom.hh \
    MiniCore/Core/mcworld.hh \
    MiniCore/Util/mctexturedata.hh \
    MiniCore/Util/mctextureconfigloader.hh \
    MiniCore/Util/mctexturemanager.hh \
