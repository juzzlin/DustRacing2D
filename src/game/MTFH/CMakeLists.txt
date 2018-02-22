project(MTFH)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(MTFHSRC
    animationcurve.cpp
    menu.cpp
    menuitem.cpp
    menuitemaction.cpp
    menumanager.cpp
    menuitemview.cpp)

add_library(MTFH ${MTFHSRC})
set_property(TARGET MTFH PROPERTY CXX_STANDARD 11)
