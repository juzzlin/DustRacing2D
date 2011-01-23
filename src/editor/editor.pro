# Project file for the level editor
TEMPLATE     = app
TARGET       = 
DEPENDPATH  += .
INCLUDEPATH += .
DEFINES     += "QSETTINGS_COMPANY_NAME=\\\"Dust\\\""
DEFINES     += "QSETTINGS_SOFTWARE_NAME=\\\"Editor\\\""
DEFINES     += "EDITOR_NAME=\\\"Dust\ Track\ Editor\\\""
DEFINES     += "EDITOR_VERSION=\\\"v0.1\\\""

SOURCES += main.cpp mainwindow.cpp editor.cpp \
    newtrackdialog.cpp \
    trackdata.cpp \
    editorview.cpp \
    editorscene.cpp \
    trackio.cpp \
    tracktile.cpp

HEADERS += mainwindow.h \
    newtrackdialog.h \
    trackdata.h \
    editorview.h \
    editorscene.h \
    trackio.h
