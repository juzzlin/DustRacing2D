include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Core)

set(SRC MCForceRegistryTest.cpp)

if(NOT UseQt5)
    set(MOC_HDRS MCForceRegistryTest.hpp)
    set(QT_USE_QTXML TRUE)
    set(QT_USE_QTOPENGL TRUE)
    set(QT_USE_QTTEST TRUE)
    qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(MCForceRegistryTest ${SRC} ${MOC_SRC})
target_link_libraries(MCForceRegistryTest MiniCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
add_test(MCForceRegistryTest ${CMAKE_SOURCE_DIR}/unittests/MCForceRegistryTest)

if(UseQt5)
    qt5_use_modules(MCForceRegistryTest OpenGL Xml Test)
else()
    target_link_libraries(MCForceRegistryTest ${QT_LIBRARIES})
endif()
