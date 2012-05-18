# Set sources
set(SRC
    main.cpp
    mainwindow.cpp
    map.cpp
    newtrackdialog.cpp
    editordata.cpp
    editorview.cpp
    editorscene.cpp
    object.cpp
    objectloader.cpp
    tiletypedialog.cpp
    tileanimator.cpp
    trackdata.cpp
    trackio.cpp
    tracktile.cpp
    trackpropertiesdialog.cpp
    ../common/config.cpp
    ../common/mapbase.cpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp)

# Set moc headers
set(MOC_HDRS
    mainwindow.hpp
    newtrackdialog.hpp
    editorview.hpp
    editorscene.hpp
    tiletypedialog.hpp
    tileanimator.hpp
    trackpropertiesdialog.hpp)

# Some .ui files
#set(UIS ui/about.ui ui/settings.ui)

# Add the resource file
set(RCS ${CMAKE_SOURCE_DIR}/data/images/images.qrc)

# Add include dir for the common sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)

# NOTE!!: Already in the main configure
#find_package(Qt4 REQUIRED)

# Add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
include(${QT_USE_FILE})

qt4_add_resources(RC_SRC ${RCS})
qt4_wrap_ui(UI_HDRS ${UIS})
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

# We need this to be able to include headers produced by uic in our code
include_directories(${CMAKE_BINARY_DIR})

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/editor)
add_executable(editor ${SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS})

# Link libraries
target_link_libraries(editor ${QT_LIBRARIES})
