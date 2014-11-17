project(DustRacing2D)

# Global game version
set(VERSION_MAJOR "1")
set(VERSION_MINOR "8")
set(VERSION_PATCH "2")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

option(ReleaseBuild "This should be used when creating DEB and RPM packages." OFF)

option(StaticVorbisLibs "Try to find static version of vorbis libs." OFF)

option(StaticVorbisLibsPath "Optional path hint for static versions of vorbis libs." "")

option(USC "Enable install layout required by Ubuntu Software Center i.e. installs to /opt." OFF)

option(GLES "Build for OpenGL ES 2.0" OFF)

option(NO_GLEW "Don't use GLEW to resolve OpenGL extensions if enabled." OFF)

option(GL30 "Build for OpenGL 3.0 shaders (default is 2.1)" OFF)

option(DATA_PATH "Optional DATA_PATH for Linux release build." "")

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(DOC_PATH "Optional DOC_PATH for Linux release build." "")

cmake_minimum_required(VERSION 2.8.8)
cmake_policy(VERSION 2.8.8)

if(UNIX)
    include("InstallLinux.cmake")
elseif(WIN32)
    include("InstallWindows.cmake")
endif()

# GCC-specific flags
if(CMAKE_COMPILER_IS_GNUCXX OR MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -O3 -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -ffast-math")
endif()

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

if(GLES)
    add_definitions(-D__MC_GLES__)
    message(STATUS "Compiling for OpenGL ES 2.0")
else()
    if(GL30)
        add_definitions(-D__MC_GL30__)
        message(STATUS "Compiling for OpenGL 3.0")
    else()
        message(STATUS "Compiling for OpenGL 2.1")
    endif()
endif()

if(NO_GLEW)
    add_definitions(-D__MC_NO_GLEW__)
endif()

add_definitions(-DGLEW_STATIC)
add_definitions(-DGLEW_NO_GLU)
add_definitions(-DVERSION="${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

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
