project(MiniCore)

set(MiniCoreSRC
Core/mccamera.cc
Core/mccircleshape.cc
Core/mccollisionevent.cc
Core/mccontactresolver.cc
Core/mccontact.cc
Core/mcdragforcegenerator.cc
Core/mcevent.cc
Core/mcexception.cc
Core/mcforcegenerator.cc
Core/mcforceregistry.cc
Core/mcfrictiongenerator.cc
Core/mcglcircleparticle.cc
Core/mcglobjectdata.cc
Core/mcglrectparticle.cc
Core/mcglrectparticlegroup.cc
Core/mcgravitygenerator.cc
Core/mclogger.cc
Core/mcobject.cc
Core/mcobjectdata.cc
Core/mcobjectfactory.cc
Core/mcobjecttree.cc
Core/mcoutofboundariesevent.cc
Core/mcparticle.cc
Core/mcrandom.cc
Core/mcrectshape.cc
Core/mcshape.cc
Core/mcspringforcegenerator.cc
Core/mcspringforcegenerator2dfast.cc
Core/mcsurface.cc
Core/mcsurfaceobjectdata.cc
Core/mcsurfaceparticle.cc
Core/mcsurfaceview.cc
Core/mctextureconfigloader.cc
Core/mctexturemanager.cc
Core/mctrigonom.cc
Core/mcvectoranimation.cc
Core/mcworld.cc
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

