# Translation files in src/editor/translations (without .ts)
set(TS dustrac-editor_fi dustrac-editor_it dustrac-editor_cs dustrac-editor_fr dustrac-editor_de)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/editor/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/src/editor/${TS_FILE}.qm)
endforeach()
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
    targetnodesizedlg.cpp
    tileanimator.cpp
    trackdata.cpp
    trackio.cpp
    tracktile.cpp
    trackpropertiesdialog.cpp
    undostack.cpp
    ../common/config.cpp
    ../common/mapbase.cpp
    ../common/objectbase.cpp
    ../common/objects.cpp
    ../common/route.cpp
    ../common/targetnodebase.cpp
    ../common/trackdatabase.cpp
    ../common/tracktilebase.cpp)

set(RCS ${CMAKE_SOURCE_DIR}/data/images/editor.qrc ${CMAKE_SOURCE_DIR}/data/icons/icons.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)

qt5_add_resources(RC_SRC ${RCS})
qt5_add_translation(QM ${TS_FILES})
qt5_wrap_ui(UI_HDRS ${UIS})

# We need this to be able to include headers produced by uic in our code
include_directories(${CMAKE_BINARY_DIR})

# Resource compilation for MinGW
if(MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o
        COMMAND
            ${CMAKE_RC_COMPILER}
            -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/data/icons/WindowsEditor.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
endif()

# Copy desktop file
if(ReleaseBuild AND UNIX)
    set(DesktopFileSourcePath)
    if(USC)
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/dustrac-editor.desktop.opt.in)
    else()
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/dustrac-editor.desktop.in)
    endif()
    add_custom_target(desktop-file-editor ALL
        COMMAND cmake -E copy ${DesktopFileSourcePath}
            ${CMAKE_BINARY_DIR}/dustrac-editor.desktop
    DEPENDS ${EDITOR_BINARY_NAME})
endif()

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${EDITOR_BINARY_NAME} WIN32 ${SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})

target_link_libraries(${EDITOR_BINARY_NAME} Qt5::Widgets Qt5::Xml)

foreach(TS_FILE ${TS})
    # Make targets to copy generated qm files to data dir. This is done the hard
    # way, because qt4_add_translation() generates the qm files to ${CMAKE_CURRENT_SOURCE_DIR}
    add_custom_target(copy-translation-file-${TS_FILE} ALL
        COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/src/editor/${TS_FILE}.qm
            ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm
        DEPENDS ${EDITOR_BINARY_NAME})
endforeach()
