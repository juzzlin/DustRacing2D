cmake_minimum_required(VERSION 2.8.7)

project(DustRacing2D)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

# Enable release build by giving e.g. -DReleaseBuild=1 to cmake.
option(ReleaseBuild "ReleaseBuild" OFF)

# Enable install layout required by USC.
option(Ubuntu "Ubuntu" OFF)

# Set install paths
set(BIN_PATH bin)

if(ReleaseBuild)
    message(STATUS "Release build.")
    if(Ubuntu)
        message(STATUS "Installing to /opt.")
        set(BIN_PATH /opt/dustrac)
        set(DATA_PATH /opt/dustrac/data)
        set(CMAKE_INSTALL_PREFIX /usr)
    else()
        set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/data)
    endif()
else()
    message(STATUS "NOTE!!: This is a development build without install targets!")
    set(DATA_PATH ./data)
endif()

# This is the main data path given to the game and editor binaries.
add_definitions(-DDATA_PATH="${DATA_PATH}")

# This is not needed with GLEW (built-in with MiniCore)
#add_definitions(-DGL_GLEXT_PROTOTYPES) 
add_definitions(-DGLEW_STATIC)

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
    # Install binaries and game data
    install(PROGRAMS dustrac-game DESTINATION ${BIN_PATH})
    install(PROGRAMS dustrac-editor DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/textures.conf DESTINATION ${DATA_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.fsh")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.vsh")

    # Install .desktop files
    install(FILES dustrac-game.desktop DESTINATION share/applications)
    install(FILES dustrac-editor.desktop DESTINATION share/applications)
    
    # Install icons
    install(FILES data/icons/dustrac-game.png DESTINATION share/pixmaps)
    install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
    install(FILES data/icons/dustrac-editor.png DESTINATION share/pixmaps)
    install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)
endif()

