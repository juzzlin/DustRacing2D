# Sub build: MiniCore engine
add_subdirectory(MiniCore)

# Sub build: Menu Toolkit From Hell
add_subdirectory(MTFH)

# Engine include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Asset")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Particles")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Text")

# Menu toolkit include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MTFH")

# Set sources
set(SRC
    aboutdlg.cpp
    ai.cpp
    car.cpp
    carparticleeffectmanager.cpp
    carstatusview.cpp
    checkeredflag.cpp
    confirmationmenu.cpp
    credits.cpp
    eventhandler.cpp
    fadeanimation.cpp
    game.cpp
    graphicsfactory.cpp
    help.cpp
    inputhandler.cpp
    intro.cpp
    keyconfigmenu.cpp
    main.cpp
    mainmenu.cpp
    map.cpp
    messageoverlay.cpp
    particlefactory.cpp
    offtrackdetector.cpp
    overlaybase.cpp
    race.cpp
    renderer.cpp
    resolutionmenu.cpp
    scene.cpp
    shaderprogram.cpp
    slidefrictiongenerator.cpp
    settings.cpp
    settingsmenu.cpp
    startlights.cpp
    startlightsoverlay.cpp
    statemachine.cpp
    surfacemenu.cpp
    textmenuitemview.cpp
    timing.cpp
    timingoverlay.cpp
    track.cpp
    trackdata.cpp
    trackloader.cpp
    trackobject.cpp
    trackobjectfactory.cpp
    trackselectionmenu.cpp
    tracktile.cpp
    treeview.cpp
    ../common/config.cpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp
    ../common/mapbase.cpp)

set(RCS ${CMAKE_SOURCE_DIR}/data/icons/icons.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (UseQt5)
    qt5_add_resources(RC_SRC ${RCS})
else()
    set(MOC_HDRS
        aboutdlg.hpp
        eventhandler.hpp
        fadeanimation.hpp
        mainmenu.hpp
        game.hpp
        race.hpp
        renderer.hpp
        statemachine.hpp
        startlights.hpp
        timing.hpp)
    qt4_add_resources(RC_SRC ${RCS})
    qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
endif()

# Add target to copy runtime files to the binary dir.
add_custom_target(runtime ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)

# Copy desktop file
if(ReleaseBuild AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DesktopFileSourcePath)
    if(Ubuntu)
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/dustrac-game.desktop.opt.in)
    else()
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/dustrac-game.desktop.in)
    endif()
    add_custom_target(desktop-file-game ALL
        COMMAND cmake -E copy ${DesktopFileSourcePath}
            ${CMAKE_BINARY_DIR}/dustrac-game.desktop
    DEPENDS ${GAME_BINARY_NAME})
endif()

# Resource compilation for MinGW
if(MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o
        COMMAND
            windres.exe
            -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/data/icons/WindowsGame.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
endif()

# The main game executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${GAME_BINARY_NAME} WIN32 ${SRC} ${MOC_SRC} ${RC_SRC})

set(COMMON_LIBS ${GAME_BINARY_NAME} MiniCore MTFH ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
if(UseQt5)
    qt5_use_modules(${GAME_BINARY_NAME} OpenGL Widgets Xml)
    target_link_libraries(${COMMON_LIBS})
else()
    target_link_libraries(${COMMON_LIBS} ${QT_LIBRARIES})
endif()


