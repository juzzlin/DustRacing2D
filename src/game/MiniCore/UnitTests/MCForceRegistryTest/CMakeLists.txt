include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCForceRegistryTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCForceRegistryTest ${SRC} ${MOC_SRC})
target_link_libraries(MCForceRegistryTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCForceRegistryTest ${CMAKE_SOURCE_DIR}/unittests/MCForceRegistryTest)

qt5_use_modules(MCForceRegistryTest OpenGL Xml Test)
