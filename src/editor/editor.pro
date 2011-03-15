# Project file for the level editor
TEMPLATE     = app
TARGET       = 
DEPENDPATH  += .
INCLUDEPATH += .
DEFINES     += "QSETTINGS_COMPANY_NAME=\\\"DustRAC\\\""
DEFINES     += "QSETTINGS_SOFTWARE_NAME=\\\"Editor\\\""
DEFINES     += "EDITOR_NAME=\\\"DustRAC\ Track\ Editor\\\""
DEFINES     += "EDITOR_VERSION=\\\"0.1\\\""
QT          += xml

SOURCES += main.cpp mainwindow.cpp editor.cpp \
    newtrackdialog.cpp \
    trackdata.cpp \
    editorview.cpp \
    editorscene.cpp \
    trackio.cpp \
    tracktile.cpp \
    tiletypedialog.cpp \
    tileanimator.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    trackdata.h \
    editorview.h \
    editorscene.h \
    trackio.h \
    tiletypedialog.h \
    tileanimator.h

RESOURCES += \
    ../common.qrc
