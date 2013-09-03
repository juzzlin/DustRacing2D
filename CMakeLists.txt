project(DustRacing2D)

# Global game version
set(VERSION "1.4.2")

# Enable Qt5 support
option(UseQt5 "UseQt5" OFF)

# Enable release build by giving e.g. -DReleaseBuild=1 to cmake.
option(ReleaseBuild "ReleaseBuild" OFF)

# Enable debug build.
option(DebugBuild "DebugBuild" OFF)

# Enable install layout required by USC.
option(Ubuntu "Ubuntu" OFF)

# Enable MiniCore build for GLES.
option(GLES "GLES" OFF)

# Don't use GLEW if enabled.
option(NO_GLEW "NO_GLEW" OFF)

if(UseQt5)
    message(STATUS "Using Qt5.")
    cmake_minimum_required(VERSION 2.8.8)
else()
    message(STATUS "Using Qt4. Add -DUseQt5=ON to build for Qt5.")
    cmake_minimum_required(VERSION 2.8.7)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include("InstallLinux.cmake")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include("InstallWindows.cmake")
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

if(GLES)
    add_definitions(-D__MC_GLES__)
endif()

if(NO_GLEW)
    add_definitions(-D__MC_NO_GLEW__)
endif()

add_definitions(-DGLEW_STATIC)
add_definitions(-DGLEW_NO_GLU)
add_definitions(-DVERSION="${VERSION}")

if(UseQt5)
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

if(DebugBuild)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    add_definitions(-DDEBUG)
endif()

# Find OpenGL (this is for some glu functions used by the
# game binary)
find_package(OpenGL REQUIRED)

# Enable CMake's unit test framework
enable_testing()

# Set default install paths
set(BIN_PATH .)
set(DATA_PATH ./data)
set(DOC_PATH .)

# Install paths depend on the build type and target platform
resolve_install_paths()
setup_install_targets()

add_subdirectory(src/editor)
add_subdirectory(src/game)

