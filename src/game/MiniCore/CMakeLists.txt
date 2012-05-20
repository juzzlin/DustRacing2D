project(MiniCore)

set(MiniCoreSRC
Core/mccamera.cc
Core/mccircleshape.cc
Core/mccollisionevent.cc
Core/mcevent.cc
Core/mcexception.cc
Core/mcglobjectdata.cc
Core/mcglscene.cc
Core/mclogger.cc
Core/mcobject.cc
Core/mcobjectdata.cc
Core/mcobjectfactory.cc
Core/mcobjecttree.cc
Core/mcoutofboundariesevent.cc
Core/mcrandom.cc
Core/mcrectshape.cc
Core/mcshape.cc
Core/mcsurface.cc
Core/mcsurfaceobjectdata.cc
Core/mctextureconfigloader.cc
Core/mctexturemanager.cc
Core/mctimerevent.cc
Core/mctrigonom.cc
Core/mcvectoranimation.cc
Core/mcworld.cc
Core/Particles/mcparticle.cc
Core/Particles/mcglrectparticle.cc
Core/Particles/mcglrectparticlegroup.cc
Core/Particles/mcsurfaceparticle.cc
Core/Physics/mccollisiondetector.cc
Core/Physics/mccontact.cc
Core/Physics/mcdragforcegenerator.cc
Core/Physics/mcforcegenerator.cc
Core/Physics/mcforceregistry.cc
Core/Physics/mcfrictiongenerator.cc
Core/Physics/mcgravitygenerator.cc
Core/Physics/mcimpulsegenerator.cc
Core/Physics/mcspringforcegenerator.cc
Core/Physics/mcspringforcegenerator2dfast.cc
Core/Views/mcsurfaceview.cc
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

