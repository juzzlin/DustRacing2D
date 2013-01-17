set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCMeshLoaderTest.cpp)
set(MOC_HDRS MCMeshLoaderTest.hpp)

qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
link_libraries(MiniCore ${QT_LIBRARIES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCMeshLoaderTest ${SRC} ${MOC_SRC})
add_test(MCMeshLoaderTest ${CMAKE_SOURCE_DIR}/unittests/MCMeshLoaderTest)
