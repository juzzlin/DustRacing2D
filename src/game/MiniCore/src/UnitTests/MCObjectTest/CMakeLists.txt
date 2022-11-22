include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
set_property(TARGET MCObjectTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCObjectTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} Qt5::OpenGL Qt5::Xml Qt5::Test)
add_test(MCObjectTest ${UNIT_TEST_BASE_DIR}/MCObjectTest)
