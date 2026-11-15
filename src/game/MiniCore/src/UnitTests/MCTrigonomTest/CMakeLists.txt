include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCTrigonomTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCTrigonomTest ${SRC} ${MOC_SRC})
set_property(TARGET MCTrigonomTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCTrigonomTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCTrigonomTest ${UNIT_TEST_BASE_DIR}/MCTrigonomTest)
