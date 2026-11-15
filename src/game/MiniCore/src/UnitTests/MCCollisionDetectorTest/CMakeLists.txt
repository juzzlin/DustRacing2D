include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCCollisionDetectorTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCCollisionDetectorTest ${SRC} ${MOC_SRC})
set_property(TARGET MCCollisionDetectorTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCCollisionDetectorTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCCollisionDetectorTest ${UNIT_TEST_BASE_DIR}/MCCollisionDetectorTest)
