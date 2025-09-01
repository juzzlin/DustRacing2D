cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

project(DustRacing2D)

# Use cmake --help-policy [POLICY] for more information about these:

if(POLICY CMP0005)
    cmake_policy(SET CMP0005 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

# Default to GLVND
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 OLD)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

# Global game version
set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("\n${CMAKE_PROJECT_NAME} ${VERSION}\n")
message(STATUS "System name       : ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "System version    : ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler identity : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version  : ${CMAKE_CXX_COMPILER_VERSION}")

# Some common CPack variables
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

set(CPACK_PACKAGE_NAME dustracing2d)
set(CPACK_PACKAGE_VENDOR Juzzlin)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/data/images/about.png)

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

option(ReleaseBuild "This should be used when creating a build with install targets" OFF)

option(VorbisLibsPath "Optional path hint for vorbis libs (libvorbis, libvorbisfile)." "")

option(OggLibPath "Optional path hint for ogg lib (libogg)." "")

option(DATA_PATH "Optional DATA_PATH for Linux release build." "")

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(DOC_PATH "Optional DOC_PATH for Linux release build." "")

option(GLES "Build for OpenGL ES 2.0" OFF)

option(NO_GLEW "Don't use GLEW to resolve OpenGL extensions if enabled." ON)

option(QOpenGLFunctions "Use QOpenGLFunctions to resolve OpenGL extensions if enabled." ON)

option(UnlockAllTracks "Unlock all tracks (for development purposes)" OFF)

option(SystemFonts "Assume all the fonts are available in the system." OFF)

option(DisableFramebufferBlits "Render fake shadows without framebuffer blits. Will result in bad shadows." OFF)

option(USE_CCACHE "Auto detect and use ccache during compilation" ON)

# Default to release C++ flags if CMAKE_BUILD_TYPE not set
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

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

if (UnlockAllTracks)
    message(STATUS "All tracks unlocked")
    add_definitions(-DUNLOCK_ALL_TRACKS)
endif()

if(SystemFonts)
    message(STATUS "Assuming system fonts")
    add_definitions(-DSYSTEM_FONTS)
endif()

if (DisableFramebufferBlits)
    message(STATUS "Framebuffer blits disabled")
    add_definitions(-DDISABLE_FRAMEBUFFER_BLITS)
endif()

add_definitions(-DGLEW_STATIC)
add_definitions(-DGLEW_NO_GLU)

# Enable CMake's unit test framework
include(CTest)

if(UNIX)
    include("InstallLinux.cmake")
elseif(WIN32)
    include("InstallWindows.cmake")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options("$<$<CONFIG:RELEASE>:-W;-Wall;-O3;-pedantic;-fomit-frame-pointer>")
    add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-pedantic>")

    if(USE_CCACHE)
        # Automatically use ccache if found
        find_program(CCACHE_FOUND ccache)
        if(CCACHE_FOUND)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
        endif(CCACHE_FOUND)
    endif()
elseif(MSVC)
    add_definitions(-DNOMINMAX)
endif()

set(GAME_BINARY_NAME "dustrac-game")
set(EDITOR_BINARY_NAME "dustrac-editor")

add_definitions(-DVERSION="${VERSION}")

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 6.4 COMPONENTS Core OpenGL Xml Widgets LinguistTools Sql Test REQUIRED)
if(QT_VERSION_MAJOR EQUAL 6 AND QT_VERSION_MINOR GREATER_EQUAL 5)
    qt_standard_project_setup(REQUIRES 6.5)
    if(QT_KNOWN_POLICY_QTP0001)
        qt_policy(SET QTP0001 NEW)
    endif()
    if(QT_KNOWN_POLICY_QTP0004)
        qt_policy(SET QTP0004 NEW)
    endif()
else()
    qt_standard_project_setup(REQUIRES 6.4)
endif()
message(STATUS "Qt version found  : ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}\n")

# Find OpenGL
find_package(OpenGL REQUIRED)
if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(DUSTRAC_OPENGL_LIBS ${OPENGL_gl_LIBRARY})
else()
    set(DUSTRAC_OPENGL_LIBS OpenGL::GL)
endif()

# OpenAL for sounds. OpenAL directory can be given by -DOPENALDIR=...
set(ENV{OPENALDIR} ${OpenALDir})
find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR}/..)
if(PACKAGE_TYPE)
    message(STATUS "Package type: ${PACKAGE_TYPE}")
    # https://github.com/mosra/magnum/issues/412
    if (${PACKAGE_TYPE} MATCHES "NSIS")
        add_compile_definitions(AL_LIBTYPE_STATIC)
    endif()
endif()

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

add_subdirectory(src/contrib/SimpleLogger EXCLUDE_FROM_ALL)
include_directories(src/contrib/SimpleLogger/src)

add_subdirectory(src/contrib/Argengine EXCLUDE_FROM_ALL)
include_directories(src/contrib/Argengine/src)

# Install paths depend on the build type and target platform
resolve_install_paths()

add_subdirectory(src/editor)
add_subdirectory(src/game)
