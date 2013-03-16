include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCObjectTest.cpp)

if(NOT UseQt5)
    set(MOC_HDRS MCObjectTest.hpp)
    set(QT_USE_QTXML TRUE)
    set(QT_USE_QTOPENGL TRUE)
    set(QT_USE_QTTEST TRUE)
    include(${QT_USE_FILE})
    qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCObjectTest ${SRC} ${MOC_SRC})
target_link_libraries(MCObjectTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCObjectTest ${CMAKE_SOURCE_DIR}/unittests/MCObjectTest)

if(UseQt5)
    qt5_use_modules(MCObjectTest OpenGL Xml Test)
else()
    target_link_libraries(MCObjectTest ${QT_LIBRARIES})
endif()
