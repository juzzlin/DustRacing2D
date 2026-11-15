include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCVectorTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(MCVectorTest ${SRC} ${MOC_SRC})
set_property(TARGET MCVectorTest PROPERTY CXX_STANDARD 17)
target_link_libraries(MCVectorTest MiniCore Qt6::OpenGL Qt6::Xml Qt6::Test)
add_test(MCVectorTest ${UNIT_TEST_BASE_DIR}/MCVectorTest)
