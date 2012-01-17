project(MiniCore)

set(MiniCoreSRC
Core/mcforcegenerator.cc
Core/mcexception.cc
Core/mccollisionevent.cc
Core/mcobjectdata.cc
Core/mcobjectfactory.cc
Core/mcsurfaceobjectdata.cc
Core/mcworld.cc
Core/mcgravitygenerator.cc
Core/mcshape.cc
Core/mcfrictiongenerator.cc
Core/mccamera.cc
Core/mcobjecttree.cc
Core/mcoutofboundariesevent.cc
Core/mcspringforcegenerator2dfast.cc
Core/mctexturemanager.cc
Core/mctextureconfigloader.cc
Core/mcevent.cc
Core/mcdragforcegenerator.cc
Core/mclogger.cc
Core/mcobject.cc
Core/mcparticle.cc
Core/mcforceregistry.cc
Core/mcsurface.cc
Core/mcspringforcegenerator.cc
Core/mccircleshape.cc
Core/mctrigonom.cc
Core/mcsurfaceparticle.cc
Core/mccontact.cc
Core/mcrandom.cc
Core/mcsurfaceview.cc
Core/mccontactresolver.cc
Core/mcglcircleparticle.cc
Core/mcrectshape.cc
Core/mcglobjectdata.cc
Core/mcglrectparticle.cc
Text/mctexturefont.cc
Text/mctexturefontconfigloader.cc
Text/mctexturefontdata.cc
Text/mctexturefontmanager.cc
Text/mctextureglyph.cc
Text/mctexturetext.cc
)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIR})

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

