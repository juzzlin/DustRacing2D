#!/bin/bash

# Builds NSIS installer for Windows in Docker

DUSTRAC_RELEASE_VERSION=2.0.1
QMAKE=/opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5
MAKENSIS=/opt/mxe/usr/bin/i686-w64-mingw32.static-makensis

CMD="export LANG=en_US.UTF-8 && \
     export LC_ALL=en_US.UTF-8 && \
     export PATH='$PATH':/opt/mxe/usr/bin && \
     cd /dr2d && DUSTRAC_RELEASE_VERSION=${DUSTRAC_RELEASE_VERSION} ./scripts/mxeWindowsBuild.sh"

if [ -f /.dockerenv ]; then
    echo "Script inside Docker"
    bash -c "${CMD}"
else
    echo "Script outside Docker"
    docker run --privileged -t -v $(pwd):/dr2d juzzlin/mxe-qt5:latest bash -c "${CMD}"
fi

