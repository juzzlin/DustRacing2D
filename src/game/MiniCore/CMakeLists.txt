project(MiniCore)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Asset")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Particles")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Text")

set(MiniCoreSRC
Asset/mcassetmanager.cc
Core/mcevent.cc
Core/mcexception.cc
Core/mclogger.cc
Core/mcobject.cc
Core/mcobjectdata.cc
Core/mcobjectfactory.cc
Core/mcrandom.cc
Core/mctimerevent.cc
Core/mctrigonom.cc
Core/mcvectoranimation.cc
Core/mcworld.cc
Graphics/glew/glew.c
Graphics/mccamera.cc
Graphics/mcglambientlight.cc
Graphics/mcgldiffuselight.cc
Graphics/mcglpointparticlerenderer.cc
Graphics/mcglscene.cc
Graphics/mcglshaderprogram.cc
Graphics/mcmesh.cc
Graphics/mcmeshconfigloader.cc
Graphics/mcmeshloader.cc
Graphics/mcmeshmanager.cc
Graphics/mcmeshobjectdata.cc
Graphics/mcmeshview.cc
Graphics/mcsurface.cc
Graphics/mcsurfaceobjectdata.cc
Graphics/mcsurfaceview.cc
Graphics/mcsurfaceconfigloader.cc
Graphics/mcsurfacemanager.cc
Graphics/mcworldrenderer.cc
Particles/mcparticle.cc
Particles/mcglpointparticle.cc
Particles/mcglrectparticle.cc
Particles/mcsurfaceparticle.cc
Physics/mccircleshape.cc
Physics/mccollisiondetector.cc
Physics/mccollisionevent.cc
Physics/mccontact.cc
Physics/mcdragforcegenerator.cc
Physics/mcforcegenerator.cc
Physics/mcforceregistry.cc
Physics/mcfrictiongenerator.cc
Physics/mcgravitygenerator.cc
Physics/mcimpulsegenerator.cc
Physics/mcobjecttree.cc
Physics/mcoutofboundariesevent.cc
Physics/mcrectshape.cc
Physics/mcshape.cc
Physics/mcspringforcegenerator.cc
Physics/mcspringforcegenerator2dfast.cc
Text/mctexturefont.cc
Text/mctexturefontconfigloader.cc
Text/mctexturefontdata.cc
Text/mctexturefontmanager.cc
Text/mctextureglyph.cc
Text/mctexturetext.cc
)

add_library(MiniCore ${MiniCoreSRC})

if(UseQt5)
    qt5_use_modules(MiniCore Core OpenGL Xml)
else()
    target_link_libraries(MiniCore ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTXML_LIBRARY})
endif()

add_subdirectory(UnitTests)

