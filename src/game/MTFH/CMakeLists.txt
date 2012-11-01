project(MTFH)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(MTFHSRC
    menu.cpp
    menuitem.cpp
    menuitemaction.cpp
    menumanager.cpp
    menuitemview.cpp)

add_library(MTFH ${MTFHSRC})

