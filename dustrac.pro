# Top-level Qt project file

TEMPLATE = subdirs
SUBDIRS += src/editor src/game

data.path = $$OUT_PWD/
data.files = data AUTHORS CHANGELOG COPYING README

INSTALLS += data

