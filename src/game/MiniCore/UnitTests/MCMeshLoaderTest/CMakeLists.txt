include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCMeshLoaderTest.cpp)

if(NOT UseQt5)
    set(MOC_HDRS MCMeshLoaderTest.hpp)
    set(QT_USE_QTTEST TRUE)
    qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCMeshLoaderTest ${SRC} ${MOC_SRC})
target_link_libraries(MCMeshLoaderTest MiniCore)
add_test(MCMeshLoaderTest ${CMAKE_SOURCE_DIR}/unittests/MCMeshLoaderTest)

if(UseQt5)
    qt5_use_modules(MCMeshLoaderTest Test)
else()
    target_link_libraries(MCMeshLoaderTest ${QT_LIBRARIES})
endif()
