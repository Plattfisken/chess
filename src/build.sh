#!/bin/bash
PROGRAM_NAME="chess";
SOURCE_FILE_NAME="main.c";
LIBS="-L ../lib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ../lib/libraylib.a";
FLAGS="-g -O0 -Wall -Wextra"
mkdir -p ../build;
clang $SOURCE_FILE_NAME $LIBS $FLAGS -o ../build/$PROGRAM_NAME && echo "$PROGRAM_NAME";
