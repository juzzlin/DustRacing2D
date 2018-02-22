#!/bin/bash

# Simple build script that can be run from Jenkins

MXE_PATH=/opt/mxe/

export PATH=${MXE_PATH}/usr/bin:${PATH}
export PKG_CONFIG_PATH=${MXE_PATH}/usr/i686-pc-mingw32/lin/pkgconfig

(mkdir -p build && cd build && source ../scripts/mxeEnv.sh && ../scripts/mxeWindowsBuild.sh ..) || exit 1

