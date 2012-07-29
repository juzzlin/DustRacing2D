# Set sources
set(SRC
    aboutdlg.cpp
    ailogic.cpp
    car.cpp
    centrifugalforcegenerator.cpp
    checkeredflag.cpp
    game.cpp
    inputhandler.cpp
    main.cpp
    mainwindow.cpp
    map.cpp
    menu.cpp
    menuitem.cpp
    menuitemview.cpp
    menumanager.cpp
    offtrackdetector.cpp
    overlaybase.cpp
    race.cpp
    radius.cpp
    renderer.cpp
    scene.cpp
    shaderprogram.cpp
    slidefrictiongenerator.cpp
    startlights.cpp
    startlightsoverlay.cpp
    statemachine.cpp
    timing.cpp
    timingoverlay.cpp
    track.cpp
    trackdata.cpp
    trackloader.cpp
    trackobject.cpp
    trackselectionmenu.cpp
    tracktile.cpp
    treeview.cpp
    wall.cpp
    ../common/config.cpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp
    ../common/mapbase.cpp)

set(MOC_HDRS
    aboutdlg.hpp
    game.hpp
    mainwindow.hpp
    renderer.hpp
    timing.hpp)

# Add the resource file
set(RCS ${CMAKE_SOURCE_DIR}/data/images/images.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  
# This command will generate rules that will run rcc
qt4_add_resources(RC_SRC ${RCS})
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

# Add target to copy runtime files to the binary dir
add_custom_target(runtime ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)

# The main game executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
add_executable(${GAME_BINARY_NAME} ${SRC} ${MOC_SRC} ${RC_SRC})
target_link_libraries(${GAME_BINARY_NAME}
    MiniCore ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
