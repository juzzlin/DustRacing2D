# Sub build: MiniCore engine
add_subdirectory(MiniCore)

# Sub build: Menu Toolkit From Hell
add_subdirectory(MTFH)

# Sub build: Sound Toolkit From Hell
add_subdirectory(STFH)

# Engine include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Asset")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Core")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Particles")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Physics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/src/Text")

# Menu toolkit and menu include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/MTFH")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/menu")

# Sound toolkit include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/STFH")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/audio")

# Translation files in src/game/translations (without .ts)
set(TS dustrac-game_fi dustrac-game_it dustrac-game_cs dustrac-game_fr dustrac-game_de dustrac-game_ru dustrac-game_nl)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/game/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm)
endforeach()
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/data/translations)

# Set headers that don't have a corresponding cpp file
set(HDR
    layers.hpp
    shaders.h
    shaders30.h
    ../common/userexception.hpp
)

# Set sources
set(SRC
    ai.cpp
    application.cpp
    bridge.cpp
    car.cpp
    carfactory.cpp
    carparticleeffectmanager.cpp
    carphysicscomponent.cpp
    carsoundeffectmanager.cpp
    carstatusview.cpp
    checkeredflag.cpp
    crashoverlay.cpp
    database.cpp
    difficultyprofile.cpp
    eventhandler.cpp
    fadeanimation.cpp
    fontfactory.cpp
    game.cpp
    gearbox.cpp
    graphicsfactory.cpp
    inputhandler.cpp
    intro.cpp
    main.cpp
    map.cpp
    messageoverlay.cpp
    minimap.cpp
    particlefactory.cpp
    pit.cpp
    offtrackdetector.cpp
    overlaybase.cpp
    race.cpp
    renderer.cpp
    scene.cpp
    settings.cpp
    startlights.cpp
    startlightsoverlay.cpp
    statemachine.cpp
    timing.cpp
    timingoverlay.cpp
    tire.cpp
    track.cpp
    trackdata.cpp
    trackloader.cpp
    trackobject.cpp
    trackobjectfactory.cpp
    tracktile.cpp
    tree.cpp
    ../common/config.hpp
    ../common/datakeywords.hpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp
    ../common/mapbase.cpp
    audio/audioworker.cpp
    audio/audiosource.cpp
    audio/openaldata.cpp
    audio/openaldevice.cpp
    audio/openalsource.cpp
    audio/openaloggdata.cpp
    audio/openalwavdata.cpp
    menu/confirmationmenu.cpp
    menu/credits.cpp
    menu/difficultymenu.cpp
    menu/help.cpp
    menu/keyconfigmenu.cpp
    menu/lapcountmenu.cpp
    menu/mainmenu.cpp
    menu/resolutionmenu.cpp
    menu/settingsmenu.cpp
    menu/surfacemenu.cpp
    menu/textmenuitemview.cpp
    menu/trackselectionmenu.cpp
    menu/vsyncmenu.cpp
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
    ${OPENAL_LIBRARY}
    ${VORBISFILE_LIBRARIES}
    ${VORBISFILE_LIB} # Valid only with MSVC
    ${VORBIS_LIB}     # Valid only with MSVC
    ${OGG_LIB})       # Valid only with MSVC

target_link_libraries(${GAME_BINARY_NAME} ${COMMON_LIBS} Qt5::OpenGL Qt5::Sql Qt5::Xml ${DUSTRAC_OPENGL_LIBS} SimpleLogger_static Argengine_static)
set_property(TARGET ${GAME_BINARY_NAME} PROPERTY CXX_STANDARD 14)

add_subdirectory(unittests)
