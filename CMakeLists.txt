project(DustRacing2D)

cmake_minimum_required(VERSION 2.8.8)
cmake_policy(VERSION 2.8.8)

# Global game version
set(VERSION_MAJOR "1")
set(VERSION_MINOR "8")
set(VERSION_PATCH "3")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

option(ReleaseBuild "This should be used when creating DEB and RPM packages." OFF)

option(StaticVorbisLibs "Try to find static version of vorbis libs." OFF)

option(StaticVorbisLibsPath "Optional path hint for static versions of vorbis libs." "")

option(USC "Enable install layout required by Ubuntu Software Center i.e. installs to /opt." OFF)

option(DATA_PATH "Optional DATA_PATH for Linux release build." "")

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(DOC_PATH "Optional DOC_PATH for Linux release build." "")

option(GLES "Build for OpenGL ES 2.0" OFF)

option(NO_GLEW "Don't use GLEW to resolve OpenGL extensions if enabled." ON)

option(QOpenGLFunctions "Use QOpenGLFunctions to resolve OpenGL extensions if enabled." ON)

if(GLES)
    add_definitions(-D__MC_GLES__)
    message(STATUS "Compiling for OpenGL ES 2.0")
else()
    message(STATUS "Compiling for OpenGL 2.1")
endif()

if(NO_GLEW)
    add_definitions(-D__MC_NO_GLEW__)
endif()

if(QOpenGLFunctions)
    message(STATUS "Using QOpenGLFunctions")
    add_definitions(-D__MC_QOPENGLFUNCTIONS__)
    add_definitions(-D__MC_NO_GLEW__)
endif()

add_definitions(-DGLEW_STATIC)
add_definitions(-DGLEW_NO_GLU)

if(UNIX)
    include("InstallLinux.cmake")
elseif(WIN32)
    include("InstallWindows.cmake")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -O3 -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")
endif()

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_MIN_VER 5.2.1)
find_package(Qt5Core ${QT_MIN_VER} REQUIRED)
find_package(Qt5OpenGL ${QT_MIN_VER} REQUIRED)
find_package(Qt5Xml ${QT_MIN_VER} REQUIRED)
find_package(Qt5Widgets ${QT_MIN_VER} REQUIRED)
find_package(Qt5LinguistTools ${QT_MIN_VER} REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# OpenAL for sounds. OpenAL directory can be given by -DOPENAL_DIR=...
set(ENV{OPENALDIR} ${OpenALDir})
find_package(OpenAL REQUIRED)

# Vorbis libs for reading Ogg files
if(StaticVorbisLibs)
    find_library(VORBISFILE_STATIC_LIB NAMES libvorbisfile.a libvorbisfile_static.lib HINTS ${StaticVorbisLibsPath} REQUIRED)
    find_library(VORBIS_STATIC_LIB NAMES libvorbis.a libvorbis_static.lib HINTS ${StaticVorbisLibsPath} REQUIRED)
    find_library(OGG_STATIC_LIB NAMES libogg.a libogg_static.lib HINTS ${StaticVorbisLibsPath} REQUIRED)
    message(STATUS "Using static versions of vorbis libs:")
    message(STATUS "  ${VORBISFILE_STATIC_LIB}")
    message(STATUS "  ${VORBIS_STATIC_LIB}")
    message(STATUS "  ${OGG_STATIC_LIB}")
else()
    include(FindPkgConfig)
    pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
endif()

# Enable CMake's unit test framework
enable_testing()

# Install paths depend on the build type and target platform
resolve_install_paths()

add_subdirectory(src/editor)
add_subdirectory(src/game)
