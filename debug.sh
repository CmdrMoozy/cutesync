#!/bin/bash
#
# Script to clean up & rebuild CuteSync.
# This will build our application in DEBUG mode.

# Build CuteSync
qmake CONFIG+=csdebug CONFIG+=cutesync
make clean
rm -fr ./CuteSync
make

# Build mkipod
qmake CONFIG+=csdebug CONFIG+=mkipod
make clean
rm -fr ./mkipod
make

# Build ifsck
qmake CONFIG+=csdebug CONFIG+=ifsck
make clean
rm -fr ./ifsck
make

# Build id3ck
qmake CONFIG+=csdebug CONFIG+=id3ck
make clean
rm -fr ./id3ck
make
