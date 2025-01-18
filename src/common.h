#ifndef COMMON_H
#define COMMON_H
#include "root.h"

#define TRUE 1
#define FALSE 0
#define ARRAY_LENGTH(arr) ((int)((sizeof(arr)) / (sizeof(arr[0]))))

#define SQUARE_SIZE 64
#define ROW_SIZE 8
#define COL_SIZE 8
#define BOARD_SIZE ((ROW_SIZE)*(COL_SIZE))
#define TREE_VIEW_WIDTH ((SQUARE_SIZE) * (4))
#define WINDOW_WIDTH ((SQUARE_SIZE) * (ROW_SIZE) + (TREE_VIEW_WIDTH))
#define WINDOW_HEIGHT ((SQUARE_SIZE) * (COL_SIZE))

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

typedef enum {
    NO_MOVE = 0,
    REGULAR,
    SHORT_CASTLE,
    LONG_CASTLE,
    EN_PASSANT,
    PROMOTION_TO_KNIGHT,
    PROMOTION_TO_BISHOP,
    PROMOTION_TO_ROOK,
    PROMOTION_TO_QUEEN
} MOVE_TYPE;

typedef struct {
    int startIndex;
    int destIndex;
    MOVE_TYPE type;
} Move;

typedef enum {
    EMPTY = -1,

    WHITE_PAWN = 0,
    WHITE_KNIGHT = 1,
    WHITE_BISHOP = 2,
    WHITE_ROOK = 3,
    WHITE_QUEEN = 4,
    WHITE_KING = 5,

    BLACK_PAWN = 6,
    BLACK_KNIGHT = 7,
    BLACK_BISHOP = 8,
    BLACK_ROOK = 9,
    BLACK_QUEEN = 10,
    BLACK_KING = 11,
} PIECE_TYPE;

typedef enum {
    PLAYER_BLACK = 0,
    PLAYER_WHITE = 1
} PLAYER_COLOR;

typedef enum {
    NOT_SET = 0,
    BOT,
    LOCAL_PLAYER,
    REMOTE_PLAYER
} PLAYER_TYPE;

typedef struct {
    PLAYER_COLOR colorToPlay;
    PIECE_TYPE position[BOARD_SIZE];
} Board;

typedef struct {
    PLAYER_TYPE white_type;
    PLAYER_TYPE black_type;
    char ip_address[16];
} GameSettings;

#endif //COMMON_H
