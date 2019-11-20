include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCMeshLoaderTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCMeshLoaderTest ${SRC} ${MOC_SRC})
set_property(TARGET MCMeshLoaderTest PROPERTY CXX_STANDARD 11)

target_link_libraries(MCMeshLoaderTest MiniCore Qt5::Test)
add_test(MCMeshLoaderTest ${CMAKE_SOURCE_DIR}/unittests/MCMeshLoaderTest)

