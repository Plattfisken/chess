#ifndef CHESS_H
#define CHESS_H
#include "root.h"

#define PAWN(color) ((color) == (PLAYER_WHITE) ? (WHITE_PAWN) : (BLACK_PAWN))
#define KNIGHT(color) ((color) == (PLAYER_WHITE) ? (WHITE_KNIGHT) : (BLACK_KNIGHT))
#define BISHOP(color) ((color) == (PLAYER_WHITE) ? (WHITE_BISHOP) : (BLACK_BISHOP))
#define ROOK(color) ((color) == (PLAYER_WHITE) ? (WHITE_ROOK) : (BLACK_ROOK))
#define QUEEN(color) ((color) == (PLAYER_WHITE) ? (WHITE_QUEEN) : (BLACK_QUEEN))
#define KING(color) ((color) == (PLAYER_WHITE) ? (WHITE_KING) : (BLACK_KING))

typedef enum {
    INVALID_MOVE_ATTEMPTED,
    CONTINUE,
    DRAW,
    BLACK_WIN,
    WHITE_WIN,
} GAME_STATE; // I want a better name for this, alternatively the ChessGameState struct...

typedef enum {
    BOT,
    LOCAL_PLAYER,
    REMOTE_PLAYER
} PLAYER_TYPE;

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
    Move *moveHistory;
    PLAYER_TYPE whiteType;
    PLAYER_TYPE blackType;
    TreeNode *tree;
    GAME_STATE state;
    udp_context udp_ctx;
} ChessGameState;

void initGame(ChessGameState *newGame, PLAYER_TYPE white, PLAYER_TYPE black);
void deleteGameResources(ChessGameState *game);
void playMove(ChessGameState *state, Move move, int shouldCheckKingInCheck);
GAME_STATE progressGame(ChessGameState *game, Move attemptedMove);
Move *calculateLegalMoves(ChessGameState *gameState, int shouldCheckKingInCheck);
GAME_STATE getStateForPosition(ChessGameState state);
// idk why this gives compiler error
Move getNextMove(ChessGameState *gameState, PLAYER_TYPE playerType, InputHandler input, Texture2D *pieceTextures);
#endif
