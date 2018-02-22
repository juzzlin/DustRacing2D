project(STFH)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(STFHSRC
    data.cpp
    device.cpp
    listener.cpp
    location.cpp
    source.cpp
)

add_library(STFH ${STFHSRC})
set_property(TARGET STFH PROPERTY CXX_STANDARD 11)
