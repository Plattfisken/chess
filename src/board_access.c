#pragma once
#include "root.h"


// provides utility functions and macros for navigating the chessboard.
int north(int startIdx, int steps) {
    return NORTH(startIdx, steps) < 0 ? OUT_OF_BOUNDS : NORTH(startIdx, steps);
}

int south(int startIdx, int steps) {
    return SOUTH(startIdx, steps) > BOARD_SIZE ? OUT_OF_BOUNDS : SOUTH(startIdx, steps);
}

int west(int startIdx, int steps) {
    return steps >= ROW_SIZE || FILE(WEST(startIdx, steps)) > FILE(startIdx) ? OUT_OF_BOUNDS : WEST(startIdx, steps);
}

int east(int startIdx, int steps) {
    return steps >= ROW_SIZE || FILE(EAST(startIdx, steps)) < FILE(startIdx) ? OUT_OF_BOUNDS : EAST(startIdx, steps);
}

int northWest(int startIdx, int stepsNorth, int stepsWest) {
    return north(startIdx, stepsNorth) == OUT_OF_BOUNDS || west(startIdx, stepsWest) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : NORTH_WEST(startIdx, stepsNorth, stepsWest);
}

int northEast(int startIdx, int stepsNorth, int stepsEast) {
    return north(startIdx, stepsNorth) == OUT_OF_BOUNDS || east(startIdx, stepsEast) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : NORTH_EAST(startIdx, stepsNorth, stepsEast);
}

int southWest(int startIdx, int stepsSouth, int stepsWest) {
    return south(startIdx, stepsSouth) == OUT_OF_BOUNDS || west(startIdx, stepsWest) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : SOUTH_WEST(startIdx, stepsSouth, stepsWest);
}

int southEast(int startIdx, int stepsSouth, int stepsEast) {
    return south(startIdx, stepsSouth) == OUT_OF_BOUNDS || east(startIdx, stepsEast) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : SOUTH_EAST(startIdx, stepsSouth, stepsEast);
}
