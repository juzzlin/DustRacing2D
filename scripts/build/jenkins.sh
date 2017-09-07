#!/bin/bash

# Simple build script that can be run from Jenkins

CORES=$(cat /proc/cpuinfo | grep processor | wc -l)

(mkdir -p jenkins-build-cmake && cd jenkins-build-cmake && cmake .. && make -j${CORES} && ctest) || exit 1

(mkdir -p jenkins-build-qmake && cd jenkins-build-qmake && qmake .. && make -j${CORES}) || exit 1

MXE_PATH=/opt/mxe/

export PATH=${MXE_PATH}/usr/bin:${PATH}
export PKG_CONFIG_PATH=${MXE_PATH}/usr/i686-pc-mingw32/lin/pkgconfig

(mkdir -p jenkins-build-mxe && cd jenkins-build-mxe && source ../scripts/mxeEnv.sh && ../scripts/mxeWindowsBuild.sh ..) || exit 1

