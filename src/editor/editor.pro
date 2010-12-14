# Project file for the level editor
TEMPLATE     = app
TARGET       = 
DEPENDPATH  += .
INCLUDEPATH += .

DEFINES += "QSETTINGS_COMPANY_NAME=\\\"Dust\\\""
DEFINES += "QSETTINGS_SOFTWARE_NAME=\\\"Editor\\\""

# Input
SOURCES += main.cpp mainwindow.cpp editor.cpp
HEADERS += mainwindow.h

