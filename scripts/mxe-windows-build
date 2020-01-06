#!/bin/bash

# Assumes, that you have built and installed qt5, openal and vorbis via MXE.

ZIP=zip
if ! which ${ZIP}; then
    echo "${ZIP} not found."
    exit 1
fi

UNIX2DOS=unix2dos
if ! which ${UNIX2DOS}; then
    echo "${UNIX2DOS} not found."
    exit 1
fi

QMAKE=/mxe/usr/i686-w64-mingw32.static/qt5/bin/qmake
if ! which ${QMAKE}; then
    echo "${QMAKE} not found."
    exit 1
fi

MAKENSIS=/mxe/usr/bin/i686-w64-mingw32.static-makensis
if ! which ${MAKENSIS}; then
    echo "${MAKENSIS} not found."
    exit 1
fi

LRELEASE=/mxe/usr/i686-w64-mingw32.static/qt5/bin/lrelease
if ! which ${LRELEASE}; then
    echo "${LRELEASE} not found."
    exit 1
fi

NUM_CPUS=$(cat /proc/cpuinfo | grep processor | wc -l)

# Package naming

NAME=dustracing2d
VERSION=${DUSTRAC_RELEASE_VERSION}
if [[ -z ${VERSION} ]]; then
    echo "DUSTRAC_RELEASE_VERSION not set."
    exit 1
fi

ARCH=windows-x86

# Build

PROJECT_DIR=.
if [[ $1 ]]; then
    PROJECT_DIR=$1
    echo "Project dir: ${PROJECT_DIR}"
fi

${QMAKE} ${PROJECT_DIR} && make clean && make -j${NUM_CPUS} || exit 1

# Update translations

${LRELEASE} ${PROJECT_DIR}/src/game/game.pro && ${LRELEASE} ${PROJECT_DIR}/src/editor/editor.pro || exit 1

# Install to packaging dir

PACKAGE_PATH=${NAME}-${VERSION}-${ARCH}

rm -rf ${PACKAGE_PATH}
mkdir ${PACKAGE_PATH}

mkdir -p data/translations &&
cp -v ${PROJECT_DIR}/src/game/translations/*.qm data/translations &&
cp -v ${PROJECT_DIR}/src/editor/translations/*.qm data/translations &&

# These are output files
cp -v src/game/release/dustrac-game.exe ${PACKAGE_PATH} &&
cp -v src/editor/release/dustrac-editor.exe ${PACKAGE_PATH} &&

cp -rv ${PROJECT_DIR}/data ${PACKAGE_PATH} || exit 1

TEXT_FILES="AUTHORS CHANGELOG COPYING README.md"
for i in ${TEXT_FILES}; do
    cp -v ${PROJECT_DIR}/$i ${PACKAGE_PATH} || exit 1
done

cd ${PACKAGE_PATH}
${UNIX2DOS} ${TEXT_FILES}
cd ..

# Create zip archive

ZIP_ARCHIVE=${NAME}-${VERSION}-${ARCH}.zip
rm -f ${ZIP_ARCHIVE}
$ZIP -rv ${ZIP_ARCHIVE} ${PACKAGE_PATH}

# Create NSIS installer

cp ${PROJECT_DIR}/packaging/windows/dustrac.nsi ${PACKAGE_PATH} && cd ${PACKAGE_PATH} && ${MAKENSIS} dustrac.nsi || exit 1
cd .. && mv ${PACKAGE_PATH}/*setup.exe . || exit 1

ls -ltr

echo "Done."

