# Qt project file to cross-compile the game for Windows with MXE

TEMPLATE = app
TARGET   = dustrac-game

DEFINES += DATA_PATH=\\\"./data\\\" VERSION=\\\"1.13.0\\\"
QMAKE_CXXFLAGS += -O3 -std=gnu++11 -fomit-frame-pointer -finline-functions -ffast-math

# Qt version check
contains(QT_VERSION, ^5\\..*) {
    message("Building for Qt version $${QT_VERSION}.")
    QT += opengl widgets xml
} else {
    error("Qt5 is required!")
}

DEFINES += GLEW_STATIC GLEW_NO_GLU
DEFINES += __MC_NO_GLEW__ __MC_QOPENGLFUNCTIONS__

# Sound libraries
CONFIG += link_pkgconfig
PKGCONFIG += openal vorbisfile

CONFIG += console

INCLUDEPATH += . \
    MiniCore/src/Core \
    MiniCore/src/Physics \
    MiniCore/src/Graphics \
    MiniCore/src/Text \
    MiniCore/src/Asset \
    MiniCore/src/Particles \
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
    application.hpp \
    audiosource.hpp \
    audioworker.hpp \
    bridge.hpp \
    bridgetrigger.hpp \
    car.hpp \
    carfactory.hpp \
    carparticleeffectmanager.hpp \
    carphysicscomponent.hpp \
    carsoundeffectmanager.hpp \
    carstatusview.hpp \
    checkeredflag.hpp \
    confirmationmenu.hpp \
    crashoverlay.hpp \
    credits.hpp \
    difficultyprofile.hpp \
    eventhandler.hpp \
    fadeanimation.hpp \
    fontfactory.hpp \
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
    minimap.hpp \
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
    startlights.hpp \
    startlightsoverlay.hpp \
    statemachine.hpp \
    surfacemenu.hpp \
    textmenuitemview.hpp \
    timing.hpp \
    timingoverlay.hpp \
    tire.hpp \
    track.hpp \
    trackdata.hpp \
    trackloader.hpp \
    trackobject.hpp \
    trackobjectfactory.hpp \
    trackselectionmenu.hpp \
    tracktile.hpp \
    tree.hpp \
    updateableif.hpp \
    userexception.hpp \
    vsyncmenu.hpp \
    MTFH/menu.hpp \
    MTFH/menuitem.hpp \
    MTFH/menuitemaction.hpp \
    MTFH/menuitemview.hpp \
    MTFH/menumanager.hpp \
    MiniCore/src/Asset/mcassetmanager.hh \
    MiniCore/src/Asset/mcmeshconfigloader.hh \
    MiniCore/src/Asset/mcmeshloader.hh \
    MiniCore/src/Asset/mcmeshmanager.hh \
    MiniCore/src/Asset/mcmeshmetadata.hh \
    MiniCore/src/Asset/mcmeshobjectdata.hh \
    MiniCore/src/Asset/mcsurfaceconfigloader.hh \
    MiniCore/src/Asset/mcsurfacemanager.hh \
    MiniCore/src/Asset/mcsurfacemetadata.hh \
    MiniCore/src/Asset/mcsurfaceobjectdata.hh \
    MiniCore/src/Core/mcbbox.hh \
    MiniCore/src/Core/mccast.hh \
    MiniCore/src/Core/mcevent.hh \
    MiniCore/src/Core/mclogger.hh \
    MiniCore/src/Core/mcmacros.hh \
    MiniCore/src/Core/mcmathutil.hh \
    MiniCore/src/Core/mcobbox.hh \
    MiniCore/src/Core/mcobject.hh \
    MiniCore/src/Core/mcobjectcomponent.hh \
    MiniCore/src/Core/mcobjectdata.hh \
    MiniCore/src/Core/mcobjectfactory.hh \
    MiniCore/src/Core/mcrandom.hh \
    MiniCore/src/Core/mcrecycler.hh \
    MiniCore/src/Core/mctimerevent.hh \
    MiniCore/src/Core/mctrigonom.hh \
    MiniCore/src/Core/mctypes.hh \
    MiniCore/src/Core/mctyperegistry.hh \
    MiniCore/src/Core/mcvector2d.hh \
    MiniCore/src/Core/mcvector3d.hh \
    MiniCore/src/Core/mcvectoranimation.hh \
    MiniCore/src/Core/mcworld.hh \
    MiniCore/src/Graphics/mccamera.hh \
    MiniCore/src/Graphics/mcglambientlight.hh \
    MiniCore/src/Graphics/mcglcolor.hh \
    MiniCore/src/Graphics/mcgldiffuselight.hh \
    MiniCore/src/Graphics/mcglmaterial.hh \
    MiniCore/src/Graphics/mcglobjectbase.hh \
    MiniCore/src/Graphics/mcglscene.hh \
    MiniCore/src/Graphics/mcglshaderprogram.hh \
    MiniCore/src/Graphics/mcgltexcoord.hh \
    MiniCore/src/Graphics/mcglvertex.hh \
    MiniCore/src/Graphics/mcmesh.hh \
    MiniCore/src/Graphics/mcmeshview.hh \
    MiniCore/src/Graphics/mcshaders.hh \
    MiniCore/src/Graphics/mcshaders30.hh \
    MiniCore/src/Graphics/mcshadersGLES.hh \
    MiniCore/src/Graphics/mcrenderlayer.hh \
    MiniCore/src/Graphics/mcshapeview.hh \
    MiniCore/src/Graphics/mcsurface.hh \
    MiniCore/src/Graphics/mcsurfaceobjectrenderer.hh \
    MiniCore/src/Graphics/mcsurfaceview.hh \
    MiniCore/src/Graphics/mcobjectrendererbase.hh \
    MiniCore/src/Graphics/mcparticle.hh \
    MiniCore/src/Graphics/mcparticlerendererbase.hh \
    MiniCore/src/Graphics/mcsurfaceparticle.hh \
    MiniCore/src/Graphics/mcsurfaceparticlerenderer.hh \
    MiniCore/src/Graphics/mcworldrenderer.hh \
    MiniCore/src/Physics/mccircleshape.hh \
    MiniCore/src/Physics/mccollisiondetector.hh \
    MiniCore/src/Physics/mccollisionevent.hh \
    MiniCore/src/Physics/mccontact.hh \
    MiniCore/src/Physics/mcdragforcegenerator.hh \
    MiniCore/src/Physics/mcedge.hh \
    MiniCore/src/Physics/mcforcegenerator.hh \
    MiniCore/src/Physics/mcforceregistry.hh \
    MiniCore/src/Physics/mcfrictiongenerator.hh \
    MiniCore/src/Physics/mcgravitygenerator.hh \
    MiniCore/src/Physics/mcimpulsegenerator.hh \
    MiniCore/src/Physics/mcobjectgrid.hh \
    MiniCore/src/Physics/mcoutofboundariesevent.hh \
    MiniCore/src/Physics/mcphysicscomponent.hh \
    MiniCore/src/Physics/mcrectshape.hh \
    MiniCore/src/Physics/mcsegment.hh \
    MiniCore/src/Physics/mcshape.hh \
    MiniCore/src/Physics/mcspringforcegenerator.hh \
    MiniCore/src/Physics/mcspringforcegenerator2dfast.hh \
    MiniCore/src/Text/mctexturefont.hh \
    MiniCore/src/Text/mctexturefontconfigloader.hh \
    MiniCore/src/Text/mctexturefontdata.hh \
    MiniCore/src/Text/mctexturefontmanager.hh \
    MiniCore/src/Text/mctextureglyph.hh \
    MiniCore/src/Text/mctexturetext.hh \
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
    application.cpp \
    audiosource.cpp \
    audioworker.cpp \
    bridge.cpp \
    bridgetrigger.cpp \
    car.cpp \
    carfactory.cpp \
    carparticleeffectmanager.cpp \
    carphysicscomponent.cpp \
    carsoundeffectmanager.cpp \
    carstatusview.cpp \
    checkeredflag.cpp \
    confirmationmenu.cpp \
    crashoverlay.cpp \
    credits.cpp \
    difficultyprofile.cpp \
    eventhandler.cpp \
    fadeanimation.cpp \
    fontfactory.cpp \
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
    minimap.cpp \
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
    startlights.cpp \
    startlightsoverlay.cpp \
    statemachine.cpp \
    surfacemenu.cpp \
    textmenuitemview.cpp \
    timing.cpp \
    timingoverlay.cpp \
    tire.cpp \
    track.cpp \
    trackdata.cpp \
    trackloader.cpp \
    trackobject.cpp \
    trackobjectfactory.cpp \
    trackselectionmenu.cpp \
    tracktile.cpp \
    tree.cpp \
    vsyncmenu.cpp \
    MTFH/menu.cpp \
    MTFH/menuitem.cpp \
    MTFH/menuitemaction.cpp \
    MTFH/menuitemview.cpp \
    MTFH/menumanager.cpp \
    MiniCore/src/Asset/mcassetmanager.cc \
    MiniCore/src/Asset/mcmeshconfigloader.cc \
    MiniCore/src/Asset/mcmeshloader.cc \
    MiniCore/src/Asset/mcmeshmanager.cc \
    MiniCore/src/Asset/mcmeshobjectdata.cc \
    MiniCore/src/Asset/mcsurfaceconfigloader.cc \
    MiniCore/src/Asset/mcsurfacemanager.cc \
    MiniCore/src/Asset/mcsurfaceobjectdata.cc \
    MiniCore/src/Core/mcmathutil.cc \
    MiniCore/src/Core/mcevent.cc \
    MiniCore/src/Core/mclogger.cc \
    MiniCore/src/Core/mcobject.cc \
    MiniCore/src/Core/mcobjectcomponent.cc \
    MiniCore/src/Core/mcobjectdata.cc \
    MiniCore/src/Core/mcobjectfactory.cc \
    MiniCore/src/Core/mcrandom.cc \
    MiniCore/src/Core/mctimerevent.cc \
    MiniCore/src/Core/mctrigonom.cc \
    MiniCore/src/Core/mctyperegistry.cc \
    MiniCore/src/Core/mcvectoranimation.cc \
    MiniCore/src/Core/mcworld.cc \
    MiniCore/src/Graphics/mccamera.cc \
    MiniCore/src/Graphics/mcglambientlight.cc \
    MiniCore/src/Graphics/mcgldiffuselight.cc \
    MiniCore/src/Graphics/mcglmaterial.cc \
    MiniCore/src/Graphics/mcglobjectbase.cc \
    MiniCore/src/Graphics/mcglscene.cc \
    MiniCore/src/Graphics/mcglshaderprogram.cc \
    MiniCore/src/Graphics/mcmesh.cc \
    MiniCore/src/Graphics/mcmeshview.cc \
    MiniCore/src/Graphics/mcrenderlayer.cc \
    MiniCore/src/Graphics/mcshapeview.cc \
    MiniCore/src/Graphics/mcsurface.cc \
    MiniCore/src/Graphics/mcsurfaceobjectrenderer.cc \
    MiniCore/src/Graphics/mcsurfaceview.cc \
    MiniCore/src/Graphics/mcobjectrendererbase.cc \
    MiniCore/src/Graphics/mcparticle.cc \
    MiniCore/src/Graphics/mcparticlerendererbase.cc \
    MiniCore/src/Graphics/mcsurfaceparticle.cc \
    MiniCore/src/Graphics/mcsurfaceparticlerenderer.cc \
    MiniCore/src/Graphics/mcworldrenderer.cc \
    MiniCore/src/Physics/mccircleshape.cc \
    MiniCore/src/Physics/mccollisiondetector.cc \
    MiniCore/src/Physics/mccollisionevent.cc \
    MiniCore/src/Physics/mccontact.cc \
    MiniCore/src/Physics/mcdragforcegenerator.cc \
    MiniCore/src/Physics/mcforcegenerator.cc \
    MiniCore/src/Physics/mcforceregistry.cc \
    MiniCore/src/Physics/mcfrictiongenerator.cc \
    MiniCore/src/Physics/mcgravitygenerator.cc \
    MiniCore/src/Physics/mcimpulsegenerator.cc \
    MiniCore/src/Physics/mcobjectgrid.cc \
    MiniCore/src/Physics/mcoutofboundariesevent.cc \
    MiniCore/src/Physics/mcphysicscomponent.cc \
    MiniCore/src/Physics/mcrectshape.cc \
    MiniCore/src/Physics/mcshape.cc \
    MiniCore/src/Physics/mcspringforcegenerator.cc \
    MiniCore/src/Physics/mcspringforcegenerator2dfast.cc \
    MiniCore/src/Text/mctexturefont.cc \
    MiniCore/src/Text/mctexturefontconfigloader.cc \
    MiniCore/src/Text/mctexturefontdata.cc \
    MiniCore/src/Text/mctexturefontmanager.cc \
    MiniCore/src/Text/mctextureglyph.cc \
    MiniCore/src/Text/mctexturetext.cc \
    MiniCore/src/Graphics/contrib/glew/glew.c \
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

