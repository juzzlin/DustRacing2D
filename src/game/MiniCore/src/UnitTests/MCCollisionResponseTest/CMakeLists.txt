include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCCollisionResponseTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCCollisionResponseTest ${SRC} ${MOC_SRC})
set_property(TARGET MCCollisionResponseTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCCollisionResponseTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCCollisionResponseTest ${UNIT_TEST_BASE_DIR}/MCCollisionResponseTest)
