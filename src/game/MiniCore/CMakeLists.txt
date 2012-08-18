project(MiniCore)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Particles")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Text")

set(MiniCoreSRC
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
Graphics/mccamera.cc
Graphics/mcglobjectdata.cc
Graphics/mcglscene.cc
Graphics/mcglshaderprogram.cc
Graphics/mcsurface.cc
Graphics/mcsurfaceobjectdata.cc
Graphics/mcsurfaceview.cc
Graphics/mctextureconfigloader.cc
Graphics/mctexturemanager.cc
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
Physics/mcworld.cc
Text/mctexturefont.cc
Text/mctexturefontconfigloader.cc
Text/mctexturefontdata.cc
Text/mctexturefontmanager.cc
Text/mctextureglyph.cc
Text/mctexturetext.cc
)

link_libraries(
#   ${SDL_LIBRARY}
#   ${SDLMIXER_LIBRARY}
#   ${OPENGL_gl_LIBRARY}
#   ${OPENGL_glu_LIBRARY}
#   ${QT_QTGUI_LIBRARY}
   ${QT_QTCORE_LIBRARY}
   ${QT_QTOPENGL_LIBRARY}
   ${QT_QTXML_LIBRARY}
)

add_library(MiniCore ${MiniCoreSRC})
add_subdirectory(UnitTests)

