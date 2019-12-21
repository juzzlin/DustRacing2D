include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Asset")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Text")

set(MiniCoreSRC
Asset/mcassetmanager.cc
Asset/mcmeshconfigloader.cc
Asset/mcmeshloader.cc
Asset/mcmeshmanager.cc
Asset/mcmeshobjectdata.cc
Asset/mcsurfaceobjectdata.cc
Asset/mcsurfaceconfigloader.cc
Asset/mcsurfacemanager.cc
Core/mcbbox.hh
Core/mcbbox3d.hh
Core/mcevent.cc
Core/mclogger.cc
Core/mcmathutil.cc
Core/mcmacros.hh
Core/mcobbox.hh
Core/mcobject.cc
Core/mcobjectcomponent.cc
Core/mcobjectdata.cc
Core/mcobjectfactory.cc
Core/mcrandom.cc
Core/mctimerevent.cc
Core/mctrigonom.cc
Core/mctyperegistry.cc
Core/mcvectoranimation.cc
Core/mcvector2d.hh
Core/mcvector3d.hh
Core/mcworld.cc
Graphics/mccamera.cc
Graphics/mcglambientlight.cc
Graphics/mcgldiffuselight.cc
Graphics/mcglmaterial.cc
Graphics/mcglobjectbase.cc
Graphics/mcglscene.cc
Graphics/mcglshaderprogram.cc
Graphics/mcmesh.cc
Graphics/mcmeshview.cc
Graphics/mcobjectrendererbase.cc
Graphics/mcparticle.cc
Graphics/mcparticlerendererbase.cc
Graphics/mcrenderlayer.cc
Graphics/mcshaders.hh
Graphics/mcshaders30.hh
Graphics/mcshadersGLES.hh
Graphics/mcshapeview.cc
Graphics/mcsurfaceparticle.cc
Graphics/mcsurfaceobjectrenderer.cc
Graphics/mcsurfaceobjectrendererlegacy.cc
Graphics/mcsurfaceparticlerenderer.cc
Graphics/mcsurfaceparticlerendererlegacy.cc
Graphics/mcsurface.cc
Graphics/mcsurfaceview.cc
Graphics/mcworldrenderer.cc
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
Physics/mcobjectgrid.cc
Physics/mcoutofboundariesevent.cc
Physics/mcphysicscomponent.cc
Physics/mcrectshape.cc
Physics/mcseparationevent.cc
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

if(NOT QOpenGLFunctions)
set(MiniCoreSRC ${MiniCoreSRC} Graphics/contrib/glew/glew.c)
endif()

set(MiniCoreTargetName MiniCore)
add_library(${MiniCoreTargetName} ${MiniCoreSRC})
target_link_libraries(${MiniCoreTargetName} Qt5::Core Qt5::OpenGL Qt5::Xml ${MINICORE_OPENGL_LIBS})
set_property(TARGET ${MiniCoreTargetName} PROPERTY CXX_STANDARD 14)

add_subdirectory(UnitTests)

