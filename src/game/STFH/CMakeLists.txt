project(STFH)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(STFHSRC
    data.cpp
    datamanager.cpp
    device.cpp
    listener.cpp
    location.cpp
    source.cpp
    world.cpp
)

add_library(STFH ${STFHSRC})

