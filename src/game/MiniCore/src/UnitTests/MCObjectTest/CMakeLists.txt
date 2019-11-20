include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
set_property(TARGET MCObjectTest PROPERTY CXX_STANDARD 11)

target_link_libraries(MCObjectTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} Qt5::OpenGL Qt5::Xml Qt5::Test)
add_test(MCObjectTest ${CMAKE_SOURCE_DIR}/unittests/MCObjectTest)

