include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(NAME gearboxtest)
set(SRC ${NAME}.cpp ../../gearbox.cpp)
set(EXECUTABLE_OUTPUT_PATH ${UNIT_TEST_BASE_DIR})
add_executable(${NAME} ${SRC} ${MOC_SRC})
set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${NAME} Qt6::Test)
add_test(${NAME} ${UNIT_TEST_BASE_DIR}/${NAME})
