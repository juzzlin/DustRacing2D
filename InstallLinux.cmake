# **** Resolve install and data paths ****
function(resolve_install_paths)

    # Set default install paths for release builds.
    set(DEFAULT_DATA_PATH_BASE share/games/DustRacing2D)
    if(NOT DATA_PATH)
        set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/data)
    endif()
    if(NOT BIN_PATH)
        set(BIN_PATH bin)
    endif()
    if(NOT DOC_PATH)
        set(DOC_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/)
    endif()

    if(ReleaseBuild)
        message(STATUS "Linux release build with system install targets.")
        if(USC)
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
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README
            DEPENDS ${GAME_BINARY_NAME} ${QM_FILES})
    endif()

    message(STATUS "BIN_PATH: " ${BIN_PATH})
    message(STATUS "DATA_PATH: " ${DATA_PATH})
    message(STATUS "DOC_PATH:" ${DOC_PATH})

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

    setup_install_targets(${BIN_PATH} ${DATA_PATH} ${DOC_PATH})

endfunction()

# **** Install targets for Linux ****
function(setup_install_targets BIN_PATH DATA_PATH DOC_PATH)

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
    install(DIRECTORY data/sounds DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ogg")
    install(DIRECTORY data/fonts DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ttf")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/data/translations DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.qm")

    if(ReleaseBuild)
        # Install .desktop files
        install(FILES ${CMAKE_BINARY_DIR}/dustrac-game.desktop DESTINATION share/applications)
        install(FILES ${CMAKE_BINARY_DIR}/dustrac-editor.desktop DESTINATION share/applications)

        # Install icons
        install(FILES data/icons/dustrac-game.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)

        # Create symlinks to targets
        if(USC)
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${GAME_BINARY_NAME} /usr/bin/${GAME_BINARY_NAME})
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${EDITOR_BINARY_NAME} /usr/bin/${EDITOR_BINARY_NAME})
        endif()
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ ON)
    set(CPACK_BINARY_TGZ ON)
    set(CPACK_BINARY_TZ OFF)

    set(QT_VER_STR "qt5")

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

    if(NOT ReleaseBuild)
        include(CPack)
    endif()
    
endfunction()
