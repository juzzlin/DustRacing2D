project(MiniCore)

cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

if(POLICY CMP0005)
    cmake_policy(SET CMP0005 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

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

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_MIN_VER 5.2.1)
find_package(Qt5Core ${QT_MIN_VER} REQUIRED)
find_package(Qt5OpenGL ${QT_MIN_VER} REQUIRED)
find_package(Qt5Xml ${QT_MIN_VER} REQUIRED)
find_package(Qt5Widgets ${QT_MIN_VER} REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)
if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(MINICORE_OPENGL_LIBS ${OPENGL_gl_LIBRARY})
else()
    set(MINICORE_OPENGL_LIBS OpenGL::GL)
endif()

# Enable CMake's unit test framework
enable_testing()

add_subdirectory(src)

