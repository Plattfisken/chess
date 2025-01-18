#!/bin/bash
PROGRAM_NAME="chess";
SOURCE_FILE_NAME="main.c";
PKG="$(pkg-config --libs --cflags raylib)"
FLAGS="-g -O0 -Wall -Wextra -Werror"
mkdir -p ../build;
clang $SOURCE_FILE_NAME $PKG $FLAGS -o ../build/$PROGRAM_NAME && echo "$PROGRAM_NAME";
