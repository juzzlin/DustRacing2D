project(SFX)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/src/game/MiniCore/Core")

set(SFX_SRC
    device.cpp
    music.cpp
    sound.cpp
    soundconfigloader.cpp
    soundmanager.cpp)

add_library(SFX ${SFX_SRC})

