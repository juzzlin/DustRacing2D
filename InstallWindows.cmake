# **** Resolve install and data paths ****
function(resolve_install_paths)

    message(STATUS "Windows build.")
    
    set(BIN_PATH .)
    set(DATA_PATH ./data)
    set(DOC_PATH .)

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

endfunction()

# **** Instal targets for Windows ****
function(setup_install_targets)

    # Install binaries and game data
    install(PROGRAMS ${GAME_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(PROGRAMS ${EDITOR_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/textures.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_ZIP ON)
    set(CPACK_BINARY_NSIS OFF)

    set(QT_VER_STR "qt4")
    if(UseQt5)
        set(QT_VER_STR "qt5")
    endif()

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-windows-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

    include(CPack)

endfunction()

