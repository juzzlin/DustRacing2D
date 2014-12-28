include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCWorldTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCWorldTest ${SRC} ${MOC_SRC})
target_link_libraries(MCWorldTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCWorldTest ${CMAKE_SOURCE_DIR}/unittests/MCWorldTest)

qt5_use_modules(MCWorldTest OpenGL Xml Test)

