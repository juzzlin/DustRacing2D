# Qt project file to cross-compile the game for Windows with MXE

TEMPLATE = app
TARGET   = dustrac-game

DEFINES += DATA_PATH=\\\"./data\\\" VERSION=\\\"1.7.3\\\"
QMAKE_CXXFLAGS += -O3 -std=gnu++11 -fomit-frame-pointer -finline-functions -ffast-math

# Qt version check
message("Building for Qt version $${QT_VERSION}.")
contains(QT_VERSION, ^5\\..*) {
QT += opengl widgets xml
DEFINES += USE_QT5
} else {
QT += opengl gui xml
}

DEFINES += GLEW_STATIC GLEW_NO_GLU

# Sound libraries
CONFIG += link_pkgconfig
PKGCONFIG += openal vorbisfile

CONFIG += console

INCLUDEPATH += . \
    MiniCore/Core \
    MiniCore/Physics \
    MiniCore/Graphics \
    MiniCore/Text \
    MiniCore/Asset \
    MiniCore/Particles \
    MTFH \
    STFH \

# Input
HEADERS += \
    ../common/config.hpp \
    ../common/mapbase.hpp \
    ../common/objectbase.hpp \
    ../common/objects.hpp \
    ../common/route.hpp \
    ../common/targetnodebase.hpp \
    ../common/trackdatabase.hpp \
    ../common/tracktilebase.hpp \
    ai.hpp \
    audiosource.hpp \
    audioworker.hpp \
    bridge.hpp \
    car.hpp \
    carfactory.hpp \
    carparticleeffectmanager.hpp \
    carsoundeffectmanager.hpp \
    carstatusview.hpp \
    checkeredflag.hpp \
    confirmationmenu.hpp \
    credits.hpp \
    eventhandler.hpp \
    fadeanimation.hpp \
    game.hpp \
    graphicsfactory.hpp \
    help.hpp \
    inputhandler.hpp \
    intro.hpp \
    keycodes.hpp \
    keyconfigmenu.hpp \
    layers.hpp \
    mainmenu.hpp \
    map.hpp \
    messageoverlay.hpp \
    offtrackdetector.hpp \
    openaldata.hpp \
    openaldevice.hpp \
    openaloggdata.hpp \
    openalsource.hpp \
    openalwavdata.hpp \
    overlaybase.hpp \
    particlefactory.hpp \
    pit.hpp \
    race.hpp \
    renderable.hpp \
    renderer.hpp \
    resolutionmenu.hpp \
    scene.hpp \
    settings.hpp \
    settingsmenu.hpp \
    shaders.h \
    shaders30.h \
    slidefrictiongenerator.hpp \
    startlights.hpp \
    startlightsoverlay.hpp \
    statemachine.hpp \
    surfacemenu.hpp \
    textmenuitemview.hpp \
    timing.hpp \
    timingoverlay.hpp \
    track.hpp \
    trackdata.hpp \
    trackloader.hpp \
    trackobject.hpp \
    trackobjectfactory.hpp \
    trackselectionmenu.hpp \
    tracktile.hpp \
    treeview.hpp \
    updateableif.hpp \
    vsyncmenu.hpp \
    MTFH/menu.hpp \
    MTFH/menuitem.hpp \
    MTFH/menuitemaction.hpp \
    MTFH/menuitemview.hpp \
    MTFH/menumanager.hpp \
    MiniCore/Asset/mcassetmanager.hh \
    MiniCore/Core/mcbbox.hh \
    MiniCore/Core/mccast.hh \
    MiniCore/Core/mcevent.hh \
    MiniCore/Core/mcexception.hh \
    MiniCore/Core/mclogger.hh \
    MiniCore/Core/mcmacros.hh \
    MiniCore/Core/mcmathutil.hh \
    MiniCore/Core/mcobbox.hh \
    MiniCore/Core/mcobject.hh \
    MiniCore/Core/mcobjectdata.hh \
    MiniCore/Core/mcobjectfactory.hh \
    MiniCore/Core/mcrandom.hh \
    MiniCore/Core/mcrecycler.hh \
    MiniCore/Core/mctimerevent.hh \
    MiniCore/Core/mctrigonom.hh \
    MiniCore/Core/mctypes.hh \
    MiniCore/Core/mcvector2d.hh \
    MiniCore/Core/mcvector3d.hh \
    MiniCore/Core/mcvectoranimation.hh \
    MiniCore/Core/mcworld.hh \
    MiniCore/Graphics/mccamera.hh \
    MiniCore/Graphics/mcglambientlight.hh \
    MiniCore/Graphics/mcglcolor.hh \
    MiniCore/Graphics/mcgldiffuselight.hh \
    MiniCore/Graphics/mcglmaterial.hh \
    MiniCore/Graphics/mcglobjectbase.hh \
    MiniCore/Graphics/mcglpointparticlerenderer.hh \
    MiniCore/Graphics/mcglscene.hh \
    MiniCore/Graphics/mcglshaderprogram.hh \
    MiniCore/Graphics/mcgltexcoord.hh \
    MiniCore/Graphics/mcglvertex.hh \
    MiniCore/Graphics/mcmesh.hh \
    MiniCore/Graphics/mcmeshconfigloader.hh \
    MiniCore/Graphics/mcmeshloader.hh \
    MiniCore/Graphics/mcmeshmanager.hh \
    MiniCore/Graphics/mcmeshmetadata.hh \
    MiniCore/Graphics/mcmeshobjectdata.hh \
    MiniCore/Graphics/mcmeshview.hh \
    MiniCore/Graphics/mcshaders.hh \
    MiniCore/Graphics/mcshaders30.hh \
    MiniCore/Graphics/mcshadersGLES.hh \
    MiniCore/Graphics/mcshapeview.hh \
    MiniCore/Graphics/mcsurface.hh \
    MiniCore/Graphics/mcsurfaceconfigloader.hh \
    MiniCore/Graphics/mcsurfacemanager.hh \
    MiniCore/Graphics/mcsurfacemetadata.hh \
    MiniCore/Graphics/mcsurfaceobjectdata.hh \
    MiniCore/Graphics/mcsurfaceview.hh \
    MiniCore/Graphics/mcworldrenderer.hh \
    MiniCore/Particles/mcglpointparticle.hh \
    MiniCore/Particles/mcglrectparticle.hh \
    MiniCore/Particles/mcparticle.hh \
    MiniCore/Particles/mcsurfaceparticle.hh \
    MiniCore/Physics/mccircleshape.hh \
    MiniCore/Physics/mccollisiondetector.hh \
    MiniCore/Physics/mccollisionevent.hh \
    MiniCore/Physics/mccontact.hh \
    MiniCore/Physics/mcdragforcegenerator.hh \
    MiniCore/Physics/mcedge.hh \
    MiniCore/Physics/mcforcegenerator.hh \
    MiniCore/Physics/mcforceregistry.hh \
    MiniCore/Physics/mcfrictiongenerator.hh \
    MiniCore/Physics/mcgravitygenerator.hh \
    MiniCore/Physics/mcimpulsegenerator.hh \
    MiniCore/Physics/mcobjecttree.hh \
    MiniCore/Physics/mcoutofboundariesevent.hh \
    MiniCore/Physics/mcrectshape.hh \
    MiniCore/Physics/mcsegment.hh \
    MiniCore/Physics/mcshape.hh \
    MiniCore/Physics/mcspringforcegenerator.hh \
    MiniCore/Physics/mcspringforcegenerator2dfast.hh \
    MiniCore/Text/mctexturefont.hh \
    MiniCore/Text/mctexturefontconfigloader.hh \
    MiniCore/Text/mctexturefontdata.hh \
    MiniCore/Text/mctexturefontmanager.hh \
    MiniCore/Text/mctextureglyph.hh \
    MiniCore/Text/mctexturetext.hh \
    STFH/data.hpp \
    STFH/device.hpp \
    STFH/listener.hpp \
    STFH/location.hpp \
    STFH/source.hpp \

SOURCES += \
    ../common/config.cpp \
    ../common/mapbase.cpp \
    ../common/objectbase.cpp \
    ../common/objects.cpp \
    ../common/route.cpp \
    ../common/targetnodebase.cpp \
    ../common/trackdatabase.cpp \
    ../common/tracktilebase.cpp \
    ai.cpp \
    audiosource.cpp \
    audioworker.cpp \
    bridge.cpp \
    car.cpp \
    carfactory.cpp \
    carparticleeffectmanager.cpp \
    carsoundeffectmanager.cpp \
    carstatusview.cpp \
    checkeredflag.cpp \
    confirmationmenu.cpp \
    credits.cpp \
    eventhandler.cpp \
    fadeanimation.cpp \
    game.cpp \
    graphicsfactory.cpp \
    help.cpp \
    inputhandler.cpp \
    intro.cpp \
    keyconfigmenu.cpp \
    main.cpp \
    mainmenu.cpp \
    map.cpp \
    messageoverlay.cpp \
    offtrackdetector.cpp \
    openaldata.cpp \
    openaldevice.cpp \
    openaloggdata.cpp \
    openalsource.cpp \
    openalwavdata.cpp \
    overlaybase.cpp \
    particlefactory.cpp \
    pit.cpp \
    race.cpp \
    renderer.cpp \
    resolutionmenu.cpp \
    scene.cpp \
    settings.cpp \
    settingsmenu.cpp \
    slidefrictiongenerator.cpp \
    startlights.cpp \
    startlightsoverlay.cpp \
    statemachine.cpp \
    surfacemenu.cpp \
    textmenuitemview.cpp \
    timing.cpp \
    timingoverlay.cpp \
    track.cpp \
    trackdata.cpp \
    trackloader.cpp \
    trackobject.cpp \
    trackobjectfactory.cpp \
    trackselectionmenu.cpp \
    tracktile.cpp \
    treeview.cpp \
    vsyncmenu.cpp \
    MTFH/menu.cpp \
    MTFH/menuitem.cpp \
    MTFH/menuitemaction.cpp \
    MTFH/menuitemview.cpp \
    MTFH/menumanager.cpp \
    MiniCore/Asset/mcassetmanager.cc \
    MiniCore/Core/mcevent.cc \
    MiniCore/Core/mcexception.cc \
    MiniCore/Core/mclogger.cc \
    MiniCore/Core/mcobject.cc \
    MiniCore/Core/mcobjectdata.cc \
    MiniCore/Core/mcobjectfactory.cc \
    MiniCore/Core/mcrandom.cc \
    MiniCore/Core/mctimerevent.cc \
    MiniCore/Core/mctrigonom.cc \
    MiniCore/Core/mcvectoranimation.cc \
    MiniCore/Core/mcworld.cc \
    MiniCore/Graphics/mccamera.cc \
    MiniCore/Graphics/mcglambientlight.cc \
    MiniCore/Graphics/mcgldiffuselight.cc \
    MiniCore/Graphics/mcglmaterial.cc \
    MiniCore/Graphics/mcglobjectbase.cc \
    MiniCore/Graphics/mcglpointparticlerenderer.cc \
    MiniCore/Graphics/mcglscene.cc \
    MiniCore/Graphics/mcglshaderprogram.cc \
    MiniCore/Graphics/mcmesh.cc \
    MiniCore/Graphics/mcmeshconfigloader.cc \
    MiniCore/Graphics/mcmeshloader.cc \
    MiniCore/Graphics/mcmeshmanager.cc \
    MiniCore/Graphics/mcmeshobjectdata.cc \
    MiniCore/Graphics/mcmeshview.cc \
    MiniCore/Graphics/mcsurface.cc \
    MiniCore/Graphics/mcsurfaceconfigloader.cc \
    MiniCore/Graphics/mcsurfacemanager.cc \
    MiniCore/Graphics/mcsurfaceobjectdata.cc \
    MiniCore/Graphics/mcsurfaceview.cc \
    MiniCore/Graphics/mcworldrenderer.cc \
    MiniCore/Particles/mcglpointparticle.cc \
    MiniCore/Particles/mcglrectparticle.cc \
    MiniCore/Particles/mcparticle.cc \
    MiniCore/Particles/mcsurfaceparticle.cc \
    MiniCore/Physics/mccircleshape.cc \
    MiniCore/Physics/mccollisiondetector.cc \
    MiniCore/Physics/mccollisionevent.cc \
    MiniCore/Physics/mccontact.cc \
    MiniCore/Physics/mcdragforcegenerator.cc \
    MiniCore/Physics/mcforcegenerator.cc \
    MiniCore/Physics/mcforceregistry.cc \
    MiniCore/Physics/mcfrictiongenerator.cc \
    MiniCore/Physics/mcgravitygenerator.cc \
    MiniCore/Physics/mcimpulsegenerator.cc \
    MiniCore/Physics/mcobjecttree.cc \
    MiniCore/Physics/mcoutofboundariesevent.cc \
    MiniCore/Physics/mcrectshape.cc \
    MiniCore/Physics/mcshape.cc \
    MiniCore/Physics/mcspringforcegenerator.cc \
    MiniCore/Physics/mcspringforcegenerator2dfast.cc \
    MiniCore/Text/mctexturefont.cc \
    MiniCore/Text/mctexturefontconfigloader.cc \
    MiniCore/Text/mctexturefontdata.cc \
    MiniCore/Text/mctexturefontmanager.cc \
    MiniCore/Text/mctextureglyph.cc \
    MiniCore/Text/mctexturetext.cc \
    MiniCore/Graphics/contrib/glew/glew.c \
    STFH/data.cpp \
    STFH/device.cpp \
    STFH/listener.cpp \
    STFH/location.cpp \
    STFH/source.cpp \

RESOURCES += ../../data/icons/icons.qrc ../../data/images/editor.qrc
RC_FILE = ../../data/icons/WindowsGame.rc
TRANSLATIONS += \
    translations/dustrac-game_fi.ts \
    translations/dustrac-game_fr.ts \
    translations/dustrac-game_it.ts \
    translations/dustrac-game_cs.ts

target.path = $$OUT_PWD/../..
INSTALLS += target

