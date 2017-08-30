#!/bin/bash

# Simple build script that can be run from Jenkins

CORES=$(cat /proc/cpuinfo | grep processor | wc -l)

(mkdir -p jenkins-build-cmake && cd jenkins-build-cmake && cmake .. && make -j${CORES} && ctest) || exit 1

(mkdir -p jenkins-build-qmake && cd jenkins-build-qmake && qmake .. && make -j${CORES}) || exit 1

