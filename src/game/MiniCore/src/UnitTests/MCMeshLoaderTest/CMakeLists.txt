include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCMeshLoaderTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCMeshLoaderTest ${SRC} ${MOC_SRC})
set_property(TARGET MCMeshLoaderTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCMeshLoaderTest MiniCore Qt6::Test)
add_test(MCMeshLoaderTest ${UNIT_TEST_BASE_DIR}/MCMeshLoaderTest)
