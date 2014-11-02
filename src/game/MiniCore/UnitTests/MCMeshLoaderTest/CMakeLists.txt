include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCMeshLoaderTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCMeshLoaderTest ${SRC} ${MOC_SRC})
target_link_libraries(MCMeshLoaderTest MiniCore)
add_test(MCMeshLoaderTest ${CMAKE_SOURCE_DIR}/unittests/MCMeshLoaderTest)

qt5_use_modules(MCMeshLoaderTest Test)
