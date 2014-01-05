#!/bin/bash

PREFIX='/home/jussil/qt5'
./configure \
            -opensource \
            -confirm-license \
            -force-pkg-config \
            -fontconfig \
            -release \
            -static \
            -prefix $PREFIX \
            -no-icu \
            -opengl desktop \
            -no-glib \
            -accessibility \
            -nomake examples \
            -nomake tests \
            -qt-zlib \
            -qt-libpng \
            -qt-libjpeg \
            -qt-sql-sqlite \
            -qt-xcb \
            -qt-pcre \
            -v

