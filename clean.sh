#!/bin/bash
#
# This script cleans up the build files produced from "./release.sh" or "./debug.sh".

rm -fr ./CuteSync ./ifsck ./mkipod ./id3ck
rm -fr ./*.o
rm -fr ./moc_*.cpp
rm -fr ./qrc_*.cpp
rm -fr ./Makefile
