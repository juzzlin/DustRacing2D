include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(NAME gearboxtest)
set(SRC ${NAME}.cpp ../../gearbox.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 11)
target_link_libraries(${NAME} Qt5::Test)
add_test(${NAME} ${CMAKE_SOURCE_DIR}/unittests/${NAME})
