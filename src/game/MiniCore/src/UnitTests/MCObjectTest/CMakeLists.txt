include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
target_link_libraries(MCObjectTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCObjectTest ${CMAKE_SOURCE_DIR}/unittests/MCObjectTest)

qt5_use_modules(MCObjectTest OpenGL Xml Test)
