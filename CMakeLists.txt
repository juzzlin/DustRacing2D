cmake_minimum_required(VERSION 2.8.7)

project(DustRacing)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

# Enable release build by giving e.g. -DReleaseBuild=1 to cmake.
option(ReleaseBuild "ReleaseBuild" OFF)

# Set install paths
set(BIN_PATH bin)

if(ReleaseBuild)
    message(STATUS "Release build.")
    set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/data)
else()
    message(STATUS "NOTE!!: This is a development build without install targets!")
    set(DATA_PATH ./data)
endif()

# This is the main data path given to the game and editor binaries.
add_definitions(-DDATA_PATH="${DATA_PATH}")
add_definitions(-DGL_GLEXT_PROTOTYPES)

# Find Qt4 and needed additional components.
find_package(Qt4 4.7.0 REQUIRED COMPONENTS QtXml QtOpenGL)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# Find OpenGL (this is for some glu functions used by the
# game binary)
find_package(OpenGL REQUIRED)

# Enable CMake's unit test framework
enable_testing()

# Sub build: editor
add_subdirectory(src/editor)
# Sub build: game binary
add_subdirectory(src/game)

# Install binaries & data
if(ReleaseBuild)
    install(PROGRAMS dustrac-game DESTINATION ${BIN_PATH})
    install(PROGRAMS dustrac-editor DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf data/textures.conf data/fonts.conf DESTINATION ${DATA_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.fsh")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.vsh")

    # Install icons and the .desktop file
    install(FILES dustrac-game.desktop DESTINATION share/applications)
    install(FILES dustrac-editor.desktop DESTINATION share/applications)
    install(FILES data/icons/DustRacing.png DESTINATION share/pixmaps)
    install(FILES data/icons/DustRacing.png DESTINATION share/icons/hicolor/64x64/apps)
endif()

