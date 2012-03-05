set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)
set(MOC_HDRS MCObjectTest.hpp)

qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
link_libraries(MiniCore ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/tests)
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
add_test(MCObjectTest ${CMAKE_BINARY_DIR}/bin/tests/MCObjectTest)
