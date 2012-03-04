# Set sources
set(SRC
aboutdlg.cpp
ailogic.cpp
car.cpp
centrifugalforcegenerator.cpp
checkerflag.cpp
game.cpp
inputhandler.cpp
main.cpp
mainwindow.cpp
map.cpp
overlaybase.cpp
race.cpp
radius.cpp
renderer.cpp
scene.cpp
slidefrictiongenerator.cpp
speedometer.cpp
startlights.cpp
startlightsoverlay.cpp
timing.cpp
timingoverlay.cpp
track.cpp
trackdata.cpp
trackloader.cpp
trackobject.cpp
tracktile.cpp
treeview.cpp
../common/config.cpp
../common/objectbase.cpp
../common/objects.cpp
../common/route.cpp
../common/trackdatabase.cpp
../common/tracktilebase.cpp
../common/mapbase.cpp
)

set(MOC_HDRS
aboutdlg.hpp
game.hpp
mainwindow.hpp
renderer.hpp
timing.hpp
)

# Add the resource file
set(RCS ../../data/images/images.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  
# This command will generate rules that will run rcc
qt4_add_resources(RC_SRC ${RCS})

qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

link_libraries(MiniCore ${QT_LIBRARIES} ${OPENGL_glu_LIBRARY})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/game)
add_executable(dustrac ${SRC} ${MOC_SRC} ${RC_SRC})

