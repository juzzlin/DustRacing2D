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
        set(DOC_PATH ${DEFAULT_DATA_PATH_BASE}/)
    endif()

    if(ReleaseBuild)
        message(STATUS "Linux release build with system install targets.")
        setup_install_targets(${BIN_PATH} ${DATA_PATH} ${DOC_PATH})

        set(QT_VER_STR "qt5")

        set(CPACK_PACKAGE_FILE_NAME "dustracing2d-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
        set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
        set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

        set(CPACK_DEBIAN_PACKAGE_NAME "dustracing2d")
        set(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jussi Lind <jussi.lind@iki.fi>")
        set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Dust Racing 2D is a traditional top-down car racing game including a level editor. ")
        set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

        set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

        if(DISTRO_VERSION)
            message(STATUS "Distro version: ${DISTRO_VERSION}")
            set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${DISTRO_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
        else()
            set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
        endif()

        include(CPack)

    else()
        message(STATUS "Linux development build without install targets.")
        message(STATUS "Use -DReleaseBuild=ON to enable install targets.")

        set(BIN_PATH .)
        set(DATA_PATH ./data)
        set(DOC_PATH .)

        # Add target to copy runtime files to the binary dir.
        add_custom_target(runtime ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.md
            DEPENDS ${GAME_BINARY_NAME} ${QM_FILES})
    endif()

    message(STATUS "BIN_PATH: " ${BIN_PATH})
    message(STATUS "DATA_PATH: " ${DATA_PATH})
    message(STATUS "DOC_PATH:" ${DOC_PATH})

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

endfunction()

# **** Install targets for Linux ****
function(setup_install_targets BIN_PATH DATA_PATH DOC_PATH)

    # Install binaries and game data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${GAME_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${EDITOR_BINARY_NAME} DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/surfaces.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README.md DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")
    install(DIRECTORY data/sounds DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ogg")
    if(NOT SystemFonts)
        install(DIRECTORY data/fonts DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ttf")
        install(DIRECTORY data/fonts DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.txt")
    endif()
    install(DIRECTORY ${CMAKE_BINARY_DIR}/data/translations DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.qm")

    # Install .desktop files
    install(FILES ${CMAKE_BINARY_DIR}/dustrac-game.desktop DESTINATION share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/dustrac-editor.desktop DESTINATION share/applications)

    # Install app store meta data
    install(FILES src/dustrac.appdata.xml DESTINATION share/metainfo)

    # Install icons
    install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
    install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)

endfunction()
