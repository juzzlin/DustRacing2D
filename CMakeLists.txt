project(DustRacing2D)

# Global game version
set(VERSION "1.3.0")

# Enable Qt5 support
option(UseQt5 "UseQt5" OFF)

# Enable release build by giving e.g. -DReleaseBuild=1 to cmake.
option(ReleaseBuild "ReleaseBuild" OFF)

# Enable debug build.
option(DebugBuild "DebugBuild" OFF)

# Enable install layout required by USC.
option(Ubuntu "Ubuntu" OFF)

if(UseQt5)
    message(STATUS "Using Qt5.")
    cmake_minimum_required(VERSION 2.8.8)
else()
    message(STATUS "Using Qt4. Add -DUseQt5=ON to build for Qt5.")
    cmake_minimum_required(VERSION 2.8.7)
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

# This is not needed with GLEW (built-in with MiniCore)
#add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DGLEW_STATIC)

if (UseQt5)
    add_definitions(-DUSE_QT5)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Widgets REQUIRED)
else()
    # Find Qt4 and needed additional components.
    find_package(Qt4 4.7.0 REQUIRED COMPONENTS QtXml QtOpenGL)
    include(${QT_USE_FILE})
    include_directories(${QT_INCLUDES})
endif()

# Find OpenGL (this is for some glu functions used by the
# game binary)
find_package(OpenGL REQUIRED)

# Enable CMake's unit test framework
enable_testing()

# Set default install paths
set(BIN_PATH bin)
set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/data)
set(DOC_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/)

if(ReleaseBuild)
    message(STATUS "Release build with system install targets.")
    if(Ubuntu)
        message(STATUS "Installing to /opt.")
        set(BIN_PATH /opt/dustrac)
        set(DATA_PATH /opt/dustrac/data)
        set(DOC_PATH /opt/dustrac/)
        set(CMAKE_INSTALL_PREFIX /usr)
    endif()
else()
    message(STATUS "Development build.")
    set(BIN_PATH .)
    set(DATA_PATH ./data)
    set(DOC_PATH .)

    if (DebugBuild)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        add_definitions(-DDEBUG)
    endif()
endif()

# This is the main data path given to the game and editor binaries.
add_definitions(-DDATA_PATH="${DATA_PATH}")
add_definitions(-DVERSION="${VERSION}")

# Sub build: editor
add_subdirectory(src/editor)
# Sub build: game binary
add_subdirectory(src/game)

# Install binaries & data
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    # Install binaries and game data
    install(PROGRAMS ${GAME_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(PROGRAMS ${EDITOR_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/textures.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.fsh")
    install(DIRECTORY data/shaders DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.vsh")

    if(ReleaseBuild)
        # Install .desktop files
        install(FILES dustrac-game.desktop DESTINATION share/applications)
        install(FILES dustrac-editor.desktop DESTINATION share/applications)

        # Install icons
        install(FILES data/icons/dustrac-game.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)

        # Create symlinks to targets
        if (Ubuntu)
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${GAME_BINARY_NAME} /usr/bin/${GAME_BINARY_NAME})
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${EDITOR_BINARY_NAME} /usr/bin/${EDITOR_BINARY_NAME})
        endif()
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ "ON")
    set(CPACK_BINARY_TGZ "ON")
    set(CPACK_BINARY_TZ "OFF")
    
    set(QT_VER_STR "qt4")
    if(UseQt5)
        set(QT_VER_STR "qt5")
    endif()

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE "COPYING")
    set(CPACK_RESOURCE_FILE_README "README")
    include(CPack)

endif()

