include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCImpulseGeneratorTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCImpulseGeneratorTest ${SRC} ${MOC_SRC})
set_property(TARGET MCImpulseGeneratorTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCImpulseGeneratorTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCImpulseGeneratorTest ${UNIT_TEST_BASE_DIR}/MCImpulseGeneratorTest)
