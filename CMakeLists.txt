project(DustRacing2D)

# Global game version
set(VERSION_MAJOR "1")
set(VERSION_MINOR "4")
set(VERSION_PATCH "4")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Enable Qt5 support
option(UseQt5 "UseQt5" OFF)

# Enable release build by giving e.g. -DReleaseBuild=1 to cmake.
# This should be used when creating DEB and RPM packages.
option(ReleaseBuild "ReleaseBuild" OFF)

# Enable debug build.
option(DebugBuild "DebugBuild" OFF)

# Enable install layout required by Ubuntu Software Center i.e. installs to /opt.
option(USC "USC" OFF)

# Enable MiniCore build for GLES.
option(GLES "GLES" OFF)

# Don't use GLEW if enabled.
option(NO_GLEW "NO_GLEW" OFF)

# Enable OpenGL 3.0 shaders (default is 2.1)
option(GL30 "GL30" OFF)

if(UseQt5)
    message(STATUS "Using Qt5.")
    cmake_minimum_required(VERSION 2.8.8)
    cmake_policy(VERSION 2.8.8)
else()
    message(STATUS "Using Qt4. Add -DUseQt5=ON to build for Qt5.")
    cmake_minimum_required(VERSION 2.8.7)
    cmake_policy(VERSION 2.8.7)
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

# Install paths depend on the build type and target platform
resolve_install_paths()

add_subdirectory(src/editor)
add_subdirectory(src/game)
