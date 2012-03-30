cmake_minimum_required(VERSION 2.8)

project(DustRacing)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -O3 -pedantic -ffast-math")

# Find Qt4
find_package(Qt4 4.7.0 REQUIRED)

# By default only QtCore and QtGui modules are enabled
set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

# Find OpenGL (this is for some glu functions used by the
# game binary)
find_package(OpenGL REQUIRED)

enable_testing()

# Sub build: editor
add_subdirectory(src/editor)
# Sub build: MiniCore engine
add_subdirectory(src/game/MiniCore)
# Sub build: game binary
add_subdirectory(src/game)

