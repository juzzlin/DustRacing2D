# Qt project file to cross-compile the editor for Windows with MXE

TEMPLATE = app
TARGET   = dustrac-editor

CONFIG += c++14
DEFINES += DATA_PATH=\\\"./data\\\" VERSION=\\\"2.1.0\\\"

# Qt version check
contains(QT_VERSION, ^5\\..*) {
    message("Building for Qt version $${QT_VERSION}.")
    QT += widgets xml
} else {
    error("Qt5 is required!")
}

INCLUDEPATH += . ../contrib/Argengine/src

# Input
HEADERS +=  \
    ../common/config.hpp \
    ../common/mapbase.hpp \
    ../common/objectbase.hpp \
    ../common/objects.hpp \
    ../common/route.hpp \
    ../common/targetnodebase.hpp \
    ../common/trackdatabase.hpp \
    ../common/tracktilebase.hpp \
    ../common/userexception.hpp \
    aboutdlg.hpp \
    application.hpp \
    draganddropstore.hpp \
    editordata.hpp \
    editorview.hpp \
    editormode.hpp \
    floodfill.hpp \
    mainwindow.hpp \
    map.hpp \
    mediator.hpp \
    newtrackdialog.hpp \
    object.hpp \
    objectfactory.hpp \
    objectmodel.hpp \
    objectmodelloader.hpp \
    rotatedialog.hpp \
    targetnode.hpp \
    targetnodesizedlg.hpp \
    tileanimator.hpp \
    trackdata.hpp \
    trackio.hpp \
    trackpropertiesdialog.hpp \
    tracktile.hpp \
    undostack.hpp \
    ../contrib/Argengine/src/argengine.hpp

SOURCES += \
    ../common/mapbase.cpp \
    ../common/objectbase.cpp \
    ../common/objects.cpp \
    ../common/route.cpp \
    ../common/targetnodebase.cpp \
    ../common/trackdatabase.cpp \
    ../common/tracktilebase.cpp \
    aboutdlg.cpp \
    application.cpp \
    draganddropstore.cpp \
    editordata.cpp \
    editorview.cpp \
    floodfill.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    mediator.cpp \
    newtrackdialog.cpp \
    object.cpp \
    objectfactory.cpp \
    objectmodelloader.cpp \
    rotatedialog.cpp \
    targetnode.cpp \
    targetnodesizedlg.cpp \
    tileanimator.cpp \
    trackdata.cpp \
    trackio.cpp \
    trackpropertiesdialog.cpp \
    tracktile.cpp \
    undostack.cpp \
    ../contrib/Argengine/src/argengine.cpp

RESOURCES += ../../data/icons/icons.qrc ../../data/images/editor.qrc
RC_FILE = ../../data/icons/WindowsEditor.rc
TRANSLATIONS += \
    translations/dustrac-editor_cs.ts \
    translations/dustrac-editor_de.ts \
    translations/dustrac-editor_fi.ts \
    translations/dustrac-editor_fr.ts \
    translations/dustrac-editor_it.ts \
    translations/dustrac-editor_nl.ts \
    translations/dustrac-editor_ru.ts

target.path = $$OUT_PWD/../..
INSTALLS += target

