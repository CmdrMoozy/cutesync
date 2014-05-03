#!/bin/bash
#
# This script runs the Valgrind leak-checker on our application, using our predefined
# suppression file.

LD_LIBRARY_PATH="/media/Krypton/Workshop/valgrind/prefix/lib:libquadra/release/lib:libreuleaux/release/lib:3rdparty/prefix/debug/lib" valgrind --leak-check=full --suppressions=./CuteSync.supp ./CuteSync
