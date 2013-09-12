# **** Resolve install and data paths ****
function(resolve_install_paths)

    # Set default install paths
    set(BIN_PATH bin)
    set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/data)
    set(DOC_PATH ${CMAKE_INSTALL_PREFIX}/share/games/DustRacing/)

    if(ReleaseBuild)
        message(STATUS "Linux release build with system install targets.")
        if(Ubuntu)
            message(STATUS "Installing to /opt.")
            set(BIN_PATH /opt/dustrac)
            set(DATA_PATH /opt/dustrac/data)
            set(DOC_PATH /opt/dustrac/)
        endif()
    else()
        message(STATUS "Linux development build with local install targets.")
        set(BIN_PATH .)
        set(DATA_PATH ./data)
        set(DOC_PATH .)

        # Add target to copy runtime files to the binary dir.
        add_custom_target(runtime ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README)
    endif()

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

endfunction()

# **** Instal targets for Linux ****
function(setup_install_targets)

    # Install binaries and game data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${GAME_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${EDITOR_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/textures.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")

    if(ReleaseBuild)
        # Install .desktop files
        install(FILES dustrac-game.desktop DESTINATION share/applications)
        install(FILES dustrac-editor.desktop DESTINATION share/applications)

        # Install icons
        install(FILES data/icons/dustrac-game.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)

        # Create symlinks to targets
        if (Ubuntu)
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${GAME_BINARY_NAME} /usr/bin/${GAME_BINARY_NAME})
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${EDITOR_BINARY_NAME} /usr/bin/${EDITOR_BINARY_NAME})
        endif()
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ ON)
    set(CPACK_BINARY_TGZ ON)
    set(CPACK_BINARY_TZ OFF)

    set(QT_VER_STR "qt4")
    if(UseQt5)
        set(QT_VER_STR "qt5")
    endif()

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

    if(NOT ReleaseBuild)
        include(CPack)
    endif()
    
endfunction()
