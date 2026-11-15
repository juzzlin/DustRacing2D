include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCPhysicsComponentTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCPhysicsComponentTest ${SRC} ${MOC_SRC})
set_property(TARGET MCPhysicsComponentTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCPhysicsComponentTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCPhysicsComponentTest ${UNIT_TEST_BASE_DIR}/MCPhysicsComponentTest)
