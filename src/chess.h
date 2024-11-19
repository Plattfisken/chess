#ifndef CHESS_H
#define CHESS_H

#include "root.h"

#define PAWN(color) ((color) == (PLAYER_WHITE) ? (WHITE_PAWN) : (BLACK_PAWN))
#define KNIGHT(color) ((color) == (PLAYER_WHITE) ? (WHITE_KNIGHT) : (BLACK_KNIGHT))
#define BISHOP(color) ((color) == (PLAYER_WHITE) ? (WHITE_BISHOP) : (BLACK_BISHOP))
#define ROOK(color) ((color) == (PLAYER_WHITE) ? (WHITE_ROOK) : (BLACK_ROOK))
#define QUEEN(color) ((color) == (PLAYER_WHITE) ? (WHITE_QUEEN) : (BLACK_QUEEN))
#define KING(color) ((color) == (PLAYER_WHITE) ? (WHITE_KING) : (BLACK_KING))

#define NO_MOVE ((Move){0})

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
    INVALID_MOVE_ATTEMPTED,
    CONTINUE,
    DRAW,
    BLACK_WIN,
    WHITE_WIN,
} GAME_STATE; // I want a better name for this, alternatively the ChessGameState struct...

typedef enum {
    REGULAR,
    SHORT_CASTLE,
    LONG_CASTLE,
    EN_PASSANT,
    PROMOTION_TO_KNIGHT,
    PROMOTION_TO_BISHOP,
    PROMOTION_TO_ROOK,
    PROMOTION_TO_QUEEN
} MOVE_TYPE;

typedef enum {
    BOT,
    HUMAN
} PLAYER_TYPE;

typedef struct {
    size_t startIndex;
    size_t destIndex;
    MOVE_TYPE type;
} Move;

typedef struct {
    PLAYER_COLOR colorToPlay;
    PIECE_TYPE position[BOARD_SIZE];
} Board;

typedef struct {
    int parentIdx;
    int firstChildIdx;
    int childCount;
    Move move;
    float value;
} TreeNode;
TreeNode *newTreeNode(TreeNode *tree, int parentIdx, Move move, float val);

typedef struct {
    Board board;
    Move *legalMoves;
    Board *positionHistory;
    PLAYER_TYPE whiteType;
    PLAYER_TYPE blackType;
    TreeNode *tree;
} ChessGameState;

ChessGameState initGame(PLAYER_TYPE white, PLAYER_TYPE black);
ChessGameState playMove(ChessGameState state, Move move, int shouldCheckKingInCheck);
GAME_STATE progressGame(ChessGameState *game, Move attemptedMove);
Move *calculateLegalMoves(ChessGameState gameState, int shouldCheckKingInCheck);

#endif
