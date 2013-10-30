# Top-level Qt project file

!contains(QT_VERSION, ^5\\.[0-9]\\..*) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.1.")
} else {
    message("Qt version $${QT_VERSION}.")
}

TEMPLATE = subdirs
SUBDIRS += src/editor src/game

data.path = $$OUT_PWD/
data.files = data AUTHORS CHANGELOG COPYING README

INSTALLS += data

