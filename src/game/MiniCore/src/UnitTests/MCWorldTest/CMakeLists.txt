include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCWorldTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCWorldTest ${SRC} ${MOC_SRC})
set_property(TARGET MCWorldTest PROPERTY CXX_STANDARD 11)

target_link_libraries(MCWorldTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} Qt5::OpenGL Qt5::Xml Qt5::Test)
add_test(MCWorldTest ${CMAKE_SOURCE_DIR}/unittests/MCWorldTest)

