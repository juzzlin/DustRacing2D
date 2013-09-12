# **** Resolve install and data paths ****
function(resolve_install_paths)

    message(STATUS "Windows build.")
    
    set(BIN_PATH .)
    set(DATA_PATH ./data)
    set(DOC_PATH .)

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

    # Add target to copy runtime files to the binary dir.
    add_custom_target(runtimeData ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README)

    if (NOT UseQt5)
        add_custom_target(runtimeQt4 ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/deps/win32/qt4/graphicssystems ${CMAKE_BINARY_DIR}/graphicssystems
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/deps/win32/qt4/imageformats ${CMAKE_BINARY_DIR}/imageformats
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/qt4/QtCore4.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/qt4/QtGui4.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/qt4/QtOpenGL4.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/qt4/QtXml4.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/mingw32/libgcc_s_dw2-1.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/mingw32/libstdc++-6.dll ${CMAKE_BINARY_DIR}
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/deps/win32/mingw32/mingwm10.dll ${CMAKE_BINARY_DIR})        
    endif()

    # Copy NSIS script for manual packaging
    add_custom_target(nsis ALL
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/packaging/windows/dustrac.nsi ${CMAKE_BINARY_DIR})

endfunction()

# **** Instal targets for Windows ****
function(setup_install_targets)

    # Install binaries and game data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${GAME_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${EDITOR_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/textures.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")

    set(QT_VER_STR "qt4")
    if(UseQt5)
        set(QT_VER_STR "qt5")
    else()
        # Runtime libraries for Qt 4.8
        install(DIRECTORY deps/win32/qt4/graphicssystems DESTINATION ${BIN_PATH})
        install(DIRECTORY deps/win32/qt4/imageformats DESTINATION ${BIN_PATH})
        install(FILES deps/win32/qt4/QtCore4.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/qt4/QtGui4.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/qt4/QtOpenGL4.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/qt4/QtXml4.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/mingw32/libgcc_s_dw2-1.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/mingw32/libstdc++-6.dll DESTINATION ${BIN_PATH})
        install(FILES deps/win32/mingw32/mingwm10.dll DESTINATION ${BIN_PATH})
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_ZIP ON)
    set(CPACK_BINARY_NSIS OFF)

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-windows-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

    include(CPack)

endfunction()

