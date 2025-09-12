cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

project(MiniCore)

# Automoc for .hh
if(POLICY CMP0100)
    cmake_policy(SET CMP0100 NEW)
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

# Find OpenGL
find_package(OpenGL REQUIRED)
if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(MINICORE_OPENGL_LIBS ${OPENGL_gl_LIBRARY})
else()
    set(MINICORE_OPENGL_LIBS OpenGL::GL)
endif()

add_subdirectory(src)

