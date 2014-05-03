#!/bin/bash
#
# Script to clean up & rebuild CuteSync.
# This will build our application in RELEASE mode.

# Build CuteSync
qmake CONFIG+=csrelease CONFIG+=cutesync
make clean
rm -fr ./CuteSync
make

# Build mkipod
qmake CONFIG+=csrelease CONFIG+=mkipod
make clean
rm -fr ./mkipod
make

# Build ifsck
qmake CONFIG+=csrelease CONFIG+=ifsck
make clean
rm -fr ./ifsck
make

# Build id3ck
qmake CONFIG+=csrelease CONFIG+=id3ck
make clean
rm -fr ./id3ck
make
