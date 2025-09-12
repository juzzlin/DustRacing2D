include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
set_property(TARGET MCObjectTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCObjectTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCObjectTest ${UNIT_TEST_BASE_DIR}/MCObjectTest)
