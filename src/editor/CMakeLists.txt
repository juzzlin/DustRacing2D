# Set sources
set(SRC
    aboutdlg.cpp
    editordata.cpp
    editorview.cpp
    editorscene.cpp
    main.cpp
    mainwindow.cpp
    map.cpp
    newtrackdialog.cpp
    object.cpp
    objectfactory.cpp
    objectmodelloader.cpp
    rotatedialog.cpp
    targetnode.cpp
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
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp)

# Set moc headers
set(MOC_HDRS
    aboutdlg.hpp
    editorview.hpp
    editorscene.hpp
    mainwindow.hpp
    newtrackdialog.hpp
    rotatedialog.hpp
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

# Resource compilation for MinGW
if(MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o
        COMMAND
            windres.exe
            -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/data/icons/Windows.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
endif(MINGW)

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
add_executable(${EDITOR_BINARY_NAME} WIN32 ${SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS})

# Link libraries
target_link_libraries(${EDITOR_BINARY_NAME} ${QT_LIBRARIES})
