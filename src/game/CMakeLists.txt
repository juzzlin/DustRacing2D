# Sub build: MiniCore engine
add_subdirectory(MiniCore)

# Sub build: Menu Toolkit From Hell
add_subdirectory(MTFH)

# Sub build: Sound Toolkit From Hell
add_subdirectory(STFH)

# Engine include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Asset")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Particles")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Text")

# Menu toolkit include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MTFH")

# Sound toolkit include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/STFH")

# Translation files in src/game/translations (without .ts)
set(TS dustrac-game_fi dustrac-game_it dustrac-game_cs dustrac-game_fr dustrac-game_de)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/game/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/src/game/${TS_FILE}.qm)
endforeach()

# Set headers that don't have a corresponding cpp file
set(HDR
    layers.hpp
    shaders.h
    shaders30.h
    userexception.hpp
)

# Set sources
set(SRC
    ai.cpp
    application.cpp
    audioworker.cpp
    audiosource.cpp
    bridge.cpp
    bridgetrigger.cpp
    car.cpp
    carfactory.cpp
    carparticleeffectmanager.cpp
    carphysicscomponent.cpp
    carsoundeffectmanager.cpp
    carstatusview.cpp
    checkeredflag.cpp
    confirmationmenu.cpp
    crashoverlay.cpp
    credits.cpp
    difficultyprofile.cpp
    eventhandler.cpp
    fadeanimation.cpp
    fontfactory.cpp
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
    pit.cpp
    offtrackdetector.cpp
    openaldata.cpp
    openaldevice.cpp
    openalsource.cpp
    openaloggdata.cpp
    openalwavdata.cpp
    overlaybase.cpp
    race.cpp
    renderer.cpp
    resolutionmenu.cpp
    scene.cpp
    settings.cpp
    settingsmenu.cpp
    startlights.cpp
    startlightsoverlay.cpp
    statemachine.cpp
    surfacemenu.cpp
    textmenuitemview.cpp
    timing.cpp
    timingoverlay.cpp
    tire.cpp
    track.cpp
    trackdata.cpp
    trackloader.cpp
    trackobject.cpp
    trackobjectfactory.cpp
    trackselectionmenu.cpp
    tracktile.cpp
    treeview.cpp
    vsyncmenu.cpp
    ../common/config.cpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp
    ../common/mapbase.cpp
    )

set(RCS ${CMAKE_SOURCE_DIR}/data/icons/icons.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

qt5_add_resources(RC_SRC ${RCS})
qt5_add_translation(QM ${TS_FILES})

# Copy desktop file
if(ReleaseBuild AND UNIX)
    set(DesktopFileSourcePath)
    if(USC)
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
            ${CMAKE_RC_COMPILER}
            -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/data/icons/WindowsGame.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
endif()

# The main game executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${GAME_BINARY_NAME} WIN32 ${HDR} ${SRC} ${MOC_SRC} ${RC_SRC} ${QM})

set(COMMON_LIBS
    MiniCore
    MTFH
    STFH
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${OPENAL_LIBRARY}
    ${VORBISFILE_LIBRARIES}
    ${VORBISFILE_LIB} # Valid only with MSVC
    ${VORBIS_LIB}     # Valid only with MSVC
    ${OGG_LIB})       # Valid only with MSVC

target_link_libraries(${GAME_BINARY_NAME} ${COMMON_LIBS} Qt5::OpenGL Qt5::Xml)

foreach(TS_FILE ${TS})
    # Make targets to copy generated qm files to data dir. This is done the hard
    # way, because qt4_add_translation() generates the qm files to ${CMAKE_CURRENT_SOURCE_DIR}
    add_custom_target(copy-translation-file-${TS_FILE} ALL
        COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/src/game/${TS_FILE}.qm
            ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm
        DEPENDS ${GAME_BINARY_NAME})
endforeach()
