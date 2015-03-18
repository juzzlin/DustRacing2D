project(DustRacing2D)

cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

if(POLICY CMP0005)
    cmake_policy(SET CMP0005 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

# Global game version
set(VERSION_MAJOR "1")
set(VERSION_MINOR "9")
set(VERSION_PATCH "2")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

option(ReleaseBuild "This should be used when creating DEB and RPM packages." OFF)

option(VorbisLibsPath "Optional path hint for vorbis libs (libvorbis, libvorbisfile)." "")

option(OggLibPath "Optional path hint for ogg lib (libogg)." "")

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
    add_compile_options(-std=c++11 -W -Wall -O3 -pedantic)
    add_compile_options(-fomit-frame-pointer -finline-functions -ffast-math)
elseif(MSVC)
    add_definitions(-DNOMINMAX)
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

# OpenAL for sounds. OpenAL directory can be given by -DOPENALDIR=...
set(ENV{OPENALDIR} ${OpenALDir})
find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR}/..)

# Vorbis libs for reading Ogg files
if(MSVC)
    find_library(VORBISFILE_LIB NAMES libvorbisfile.a libvorbisfile_static.lib HINTS ${VorbisLibsPath} REQUIRED)
    find_library(VORBIS_LIB NAMES libvorbis.a libvorbis_static.lib HINTS ${VorbisLibsPath} REQUIRED)
    include_directories(${VorbisLibsPath}/include)
    find_library(OGG_LIB NAMES libogg.a libogg_static.lib HINTS ${OggLibPath} REQUIRED)
    include_directories(${OggLibPath}/include)
    message(STATUS "Using static versions of vorbis libs:")
    message(STATUS "  ${VORBISFILE_LIB}")
    message(STATUS "  ${VORBIS_LIB}")
    message(STATUS "  ${OGG_LIB}")
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
