include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCForceRegistryTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCForceRegistryTest ${SRC} ${MOC_SRC})
set_property(TARGET MCForceRegistryTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCForceRegistryTest MiniCore Qt5::OpenGL Qt5::Xml Qt5::Test ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCForceRegistryTest ${UNIT_TEST_BASE_DIR}/MCForceRegistryTest)
