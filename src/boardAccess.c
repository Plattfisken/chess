// provides utility functions and macros for navigating the chessboard.
#pragma once
#include "root.h"

#define ROW_SIZE 8
#define COL_SIZE 8
#define BOARD_SIZE ((ROW_SIZE)*(COL_SIZE))

#define IDX_2D(x,y) ((x)+(y)*(ROW_SIZE))
#define RANK(i) ((i) < (0) ? ((i)-(1)) / (ROW_SIZE) : (i) / (ROW_SIZE))
#define FILE(i) ((i) % (ROW_SIZE))

#define NORTH(i, steps) ((i) - ((ROW_SIZE) * (steps)))
#define SOUTH(i, steps) ((i) + ((ROW_SIZE) * (steps)))
#define WEST(i, steps) ((i) - (steps))
#define EAST(i, steps) ((i) + (steps))

#define NORTH_WEST(i, stepsNorth, stepsWest) (NORTH(WEST(i, stepsWest), stepsNorth))
#define NORTH_EAST(i, stepsNorth, stepsEast) (NORTH(EAST(i, stepsEast), stepsNorth))
#define SOUTH_WEST(i, stepsSouth, stepsWest) (SOUTH(WEST(i, stepsWest), stepsSouth))
#define SOUTH_EAST(i, stepsSouth, stepsEast) (SOUTH(EAST(i, stepsEast), stepsSouth))

#define OUT_OF_BOUNDS -1

// NOTE: last argument is ignored, and only defined so the functions can be treated equally with the diagonal directional functions
int north(int startIdx, int steps, int _) {
    return NORTH(startIdx, steps) < 0 ? OUT_OF_BOUNDS : NORTH(startIdx, steps);
}

int south(int startIdx, int steps, int _) {
    return SOUTH(startIdx, steps) > BOARD_SIZE ? OUT_OF_BOUNDS : SOUTH(startIdx, steps);
}

int west(int startIdx, int steps, int _) {
    return steps >= ROW_SIZE || FILE(WEST(startIdx, steps)) > FILE(startIdx) ? OUT_OF_BOUNDS : WEST(startIdx, steps);
}

int east(int startIdx, int steps, int _) {
    return steps >= ROW_SIZE || FILE(EAST(startIdx, steps)) < FILE(startIdx) ? OUT_OF_BOUNDS : EAST(startIdx, steps);
}

int northWest(int startIdx, int stepsNorth, int stepsWest) {
    return north(startIdx, stepsNorth, 0) == OUT_OF_BOUNDS || west(startIdx, stepsWest, 0) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : NORTH_WEST(startIdx, stepsNorth, stepsWest);
}

int northEast(int startIdx, int stepsNorth, int stepsEast) {
    return north(startIdx, stepsNorth, 0) == OUT_OF_BOUNDS || east(startIdx, stepsEast, 0) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : NORTH_EAST(startIdx, stepsNorth, stepsEast);
}

int southWest(int startIdx, int stepsSouth, int stepsWest) {
    return south(startIdx, stepsSouth, 0) == OUT_OF_BOUNDS || west(startIdx, stepsWest, 0) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : SOUTH_WEST(startIdx, stepsSouth, stepsWest);
}

int southEast(int startIdx, int stepsSouth, int stepsEast) {
    return south(startIdx, stepsSouth, 0) == OUT_OF_BOUNDS || east(startIdx, stepsEast, 0) == OUT_OF_BOUNDS
    ? OUT_OF_BOUNDS : SOUTH_EAST(startIdx, stepsSouth, stepsEast);
}
