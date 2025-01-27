#pragma once
#include "root.h"

float getPieceSquareValue(PIECE_TYPE piece, int squareIdx) {
    float piece_table[12][BOARD_SIZE] = {
        // white pawn
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
            0.1, 0.1, 0.2, 0.3, 0.3, 0.2, 0.1, 0.1,
            0.05, 0.05, 0.1, 0.25, 0.25, 0.1, 0.05, 0.05,
            0, 0, 0, 0.2, 0.2, 0, 0, 0,
            0.05, -0.05, -0.1, 0, 0, -0.1, -0.05, 0.05,
            0.05, 0.1, 0.1, -0.2, -0.2, 0.1, 0.1, 0.05,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        // white knight
        {
            -0.50, -0.40, -0.30, -0.30, -0.30, -0.30, -0.40, -0.50, 
            -0.40, -0.20, 0, 0, 0, 0, -0.20, -0.40, 
            -0.30, 0, 0.10, 0.15, 0.15, 0.10, 0, -0.30, 
            -0.30, 0.05, 0.15, 0.20, 0.20, 0.15, 0.05, -0.30, 
            -0.30, 0, 0.15, 0.20, 0.20, 0.15, 0, -0.30, 
            -0.30, 0.05, 0.10, 0.15, 0.15, 0.10, 0.05, -0.30, 
            -0.40, -0.20, 0, 0.05, 0.05, 0, -0.20, -0.40, 
            -0.50, -0.40, -0.30, -0.30, -0.30, -0.30, -0.40, -0.50, 
        },
        // white bishop
        {
            -0.20, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.20, 
            -0.10, 0, 0, 0, 0, 0, 0, -0.10, 
            -0.10, 0, 0.05, 0.10, 0.10, 0.05, 0, -0.10, 
            -0.10, 0.05, 0.05, 0.10, 0.10, 0.05, 0.05, -0.10, 
            -0.10, 0, 0.10, 0.10, 0.10, 0.10, 0, -0.10, 
            -0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, -0.10, 
            -0.10, 0.05, 0, 0, 0, 0, 0.05, -0.10, 
            -0.20, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.20, 
        },
        // white rook
        {
            0, 0, 0, 0, 0, 0, 0, 0, 
            0.05, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.05, 
            -0.05, 0, 0, 0, 0, 0, 0, -0.05, 
            -0.05, 0, 0, 0, 0, 0, 0, -0.05, 
            -0.05, 0, 0, 0, 0, 0, 0, -0.05, 
            -0.05, 0, 0, 0, 0, 0, 0, -0.05, 
            -0.05, 0, 0, 0, 0, 0, 0, -0.05, 
            0, 0, 0, 0.05, 0.05, 0, 0, 0, 
        },
        // white queen
        {
            -0.20, -0.10, -0.10, -0.05, -0.05, -0.10, -0.10, -0.20, 
            -0.10, 0, 0, 0, 0, 0, 0, -0.10, 
            -0.10, 0, 0.05, 0.05, 0.05, 0.05, 0, -0.10, 
            -0.05, 0, 0.05, 0.05, 0.05, 0.05, 0, -0.05, 
            0, 0, 0.05, 0.05, 0.05, 0.05, 0, -0.05, 
            -0.10, 0.05, 0.05, 0.05, 0.05, 0.05, 0, -0.10, 
            -0.10, 0, 0.05, 0, 0, 0, 0, -0.10, 
            -0.20, -0.10, -0.10, -0.05, -0.05, -0.10, -0.10, -0.20, 
        },
        // white king
        {
            -0.30, -0.40, -0.40, -0.50, -0.50, -0.40, -0.40, -0.30, 
            -0.30, -0.40, -0.40, -0.50, -0.50, -0.40, -0.40, -0.30, 
            -0.30, -0.40, -0.40, -0.50, -0.50, -0.40, -0.40, -0.30, 
            -0.30, -0.40, -0.40, -0.50, -0.50, -0.40, -0.40, -0.30, 
            -0.20, -0.30, -0.30, -0.40, -0.40, -0.30, -0.30, -0.20, 
            -0.10, -0.20, -0.20, -0.20, -0.20, -0.20, -0.20, -0.10, 
            0.20, 0.20, 0, 0, 0, 0, 0.20, 0.20, 
            0.20, 0.30, 0.10, 0, 0, 0.10, 0.30, 0.20, 
        },
        // black pawn
        {
            0, 0, 0, 0, 0, 0, 0, 0, 
            -0.05, -0.10, -0.10, 0.20, 0.20, -0.10, -0.10, -0.05, 
            -0.05, 0.05, 0.10, 0, 0, 0.10, 0.05, -0.05, 
            0, 0, 0, -0.20, -0.20, 0, 0, 0, 
            -0.05, -0.05, -0.10, -0.25, -0.25, -0.10, -0.05, -0.05, 
            -0.10, -0.10, -0.20, -0.30, -0.30, -0.20, -0.10, -0.10, 
            -0.50, -0.50, -0.50, -0.50, -0.50, -0.50, -0.50, -0.50, 
            0, 0, 0, 0, 0, 0, 0, 0, 
        },
        // black knight
        {
            0.50, 0.40, 0.30, 0.30, 0.30, 0.30, 0.40, 0.50, 
            0.40, 0.20, 0, -0.05, -0.05, 0, 0.20, 0.40, 
            0.30, -0.05, -0.10, -0.15, -0.15, -0.10, -0.05, 0.30, 
            0.30, 0, -0.15, -0.20, -0.20, -0.15, 0, 0.30, 
            0.30, -0.05, -0.15, -0.20, -0.20, -0.15, -0.05, 0.30, 
            0.30, 0, -0.10, -0.15, -0.15, -0.10, 0, 0.30, 
            0.40, 0.20, 0, 0, 0, 0, 0.20, 0.40, 
            0.50, 0.40, 0.30, 0.30, 0.30, 0.30, 0.40, 0.50, 
        },
        // black bishop
        {
            0.20, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.20, 
            0.10, -0.05, 0, 0, 0, 0, -0.05, 0.10, 
            0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, 0.10, 
            0.10, 0, -0.10, -0.10, -0.10, -0.10, 0, 0.10, 
            0.10, -0.05, -0.05, -0.10, -0.10, -0.05, -0.05, 0.10, 
            0.10, 0, -0.05, -0.10, -0.10, -0.05, 0, 0.10, 
            0.10, 0, 0, 0, 0, 0, 0, 0.10, 
            0.20, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.20, 
        },
        // black rook
        {
            0, 0, 0, -0.05, -0.05, 0, 0, 0, 
            0.05, 0, 0, 0, 0, 0, 0, 0.05, 
            0.05, 0, 0, 0, 0, 0, 0, 0.05, 
            0.05, 0, 0, 0, 0, 0, 0, 0.05, 
            0.05, 0, 0, 0, 0, 0, 0, 0.05, 
            0.05, 0, 0, 0, 0, 0, 0, 0.05, 
            -0.05, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.05, 
            0, 0, 0, 0, 0, 0, 0, 0, 
        },
        // black queen
        {
            0.20, 0.10, 0.10, 0.05, 0.05, 0.10, 0.10, 0.20, 
            0.10, 0, 0, 0, 0, -0.05, 0, 0.10, 
            0.10, 0, -0.05, -0.05, -0.05, -0.05, -0.05, 0.10, 
            0.05, 0, -0.05, -0.05, -0.05, -0.05, 0, 0, 
            0.05, 0, -0.05, -0.05, -0.05, -0.05, 0, 0.05, 
            0.10, 0, -0.05, -0.05, -0.05, -0.05, 0, 0.10, 
            0.10, 0, 0, 0, 0, 0, 0, 0.10, 
            0.20, 0.10, 0.10, 0.05, 0.05, 0.10, 0.10, 0.20, 
        },
        // black king
        {
            -0.20, -0.30, -0.10, 0, 0, -0.10, -0.30, -0.20, 
            -0.20, -0.20, 0, 0, 0, 0, -0.20, -0.20, 
            0.10, 0.20, 0.20, 0.20, 0.20, 0.20, 0.20, 0.10, 
            0.20, 0.30, 0.30, 0.40, 0.40, 0.30, 0.30, 0.20, 
            0.30, 0.40, 0.40, 0.50, 0.50, 0.40, 0.40, 0.30, 
            0.30, 0.40, 0.40, 0.50, 0.50, 0.40, 0.40, 0.30, 
            0.30, 0.40, 0.40, 0.50, 0.50, 0.40, 0.40, 0.30, 
            0.30, 0.40, 0.40, 0.50, 0.50, 0.40, 0.40, 0.30, 
        }
    };
    switch(piece) {
        case EMPTY:
            return 0;
            break;
        case WHITE_PAWN:
            return piece_table[WHITE_PAWN][squareIdx];
            break;
        case WHITE_KNIGHT:
            return piece_table[WHITE_KNIGHT][squareIdx];
            break;
        case WHITE_BISHOP:
            return piece_table[WHITE_BISHOP][squareIdx];
            break;
        case WHITE_ROOK:
            return piece_table[WHITE_ROOK][squareIdx];
            break;
        case WHITE_QUEEN:
            return piece_table[WHITE_QUEEN][squareIdx];
            break;
        case WHITE_KING:
            return piece_table[WHITE_KING][squareIdx];
            break;

        case BLACK_PAWN:
            return piece_table[BLACK_PAWN][squareIdx];
            break;
        case BLACK_KNIGHT:
            return piece_table[BLACK_KNIGHT][squareIdx];
            break;
        case BLACK_BISHOP:
            return piece_table[BLACK_BISHOP][squareIdx];
            break;
        case BLACK_ROOK:
            return piece_table[BLACK_ROOK][squareIdx];
            break;
        case BLACK_QUEEN:
            return piece_table[BLACK_QUEEN][squareIdx];
            break;
        case BLACK_KING:
            return piece_table[BLACK_KING][squareIdx];
            break;
    }
}

float getPositionEvaluation(const Board *board, GAME_STATE endState) {
    if(endState == WHITE_WIN) {
        return 1000;
    }
    else if(endState == BLACK_WIN) {
        return -1000;
    }
    if(endState == DRAW) {
        return 0;
    }
    float result = 0;
    for(int i = 0; i < BOARD_SIZE; ++i) {
        result += getPieceSquareValue(board->position[i], i);
        if(board->position[i] == WHITE_PAWN) result += 1;
        if(board->position[i] == WHITE_KNIGHT) result += 3.2;
        if(board->position[i] == WHITE_BISHOP) result += 3.3;
        if(board->position[i] == WHITE_ROOK) result += 5;
        if(board->position[i] == WHITE_QUEEN) result += 9;

        if(board->position[i] == BLACK_PAWN) result += -1;
        if(board->position[i] == BLACK_KNIGHT) result += -3.2;
        if(board->position[i] == BLACK_BISHOP) result += -3.3;
        if(board->position[i] == BLACK_ROOK) result += -5;
        if(board->position[i] == BLACK_QUEEN) result += -9;
    }
    return result;
}

TreeNode *createMoveTree(ChessGameState gameState, TreeNode *tree, int depth) {
    if(tree) {
        arrfree(tree);
        tree = NULL;
    }

    tree = newTreeNode(tree, 0, (Move){0, 0, NO_MOVE}, 0);

    Move moves[depth];
    // initialize to zero
    for(int i = 0; i < depth; ++i) {
        moves[i] = (Move){0, 0, 0};
    }

    for(int nodeIdx = 0; nodeIdx < arrlen(tree); ++nodeIdx) {
        // copy gameState to be able to use progressGame without affecting the game, 
        // the pointers to dynamic arrays have to be reset and copied manually so the actual lists do not get modified
        ChessGameState gameStateCopy = gameState;

        gameStateCopy.legal_moves = NULL;
        gameStateCopy.position_history = NULL;
        gameStateCopy.move_history = NULL;
        arrsetlen(gameStateCopy.legal_moves, arrlen(gameState.legal_moves));
        arrsetlen(gameStateCopy.position_history, arrlen(gameState.position_history));
        arrsetlen(gameStateCopy.move_history, arrlen(gameState.move_history));
        memcpy(gameStateCopy.legal_moves, gameState.legal_moves, arrlen(gameState.legal_moves) * sizeof(Move));
        memcpy(gameStateCopy.position_history, gameState.position_history, arrlen(gameState.position_history) * sizeof(Board));
        memcpy(gameStateCopy.move_history, gameState.move_history, arrlen(gameState.move_history) * sizeof(Move));

        // count how many generations there are from current node until root node, and store each parents move
        int generationCount = 0;
        {
            TreeNode parent = tree[nodeIdx];
            while(parent.parentIdx != -1) {
                moves[generationCount++] = parent.move;
                parent = tree[parent.parentIdx];
            }
        }
        // break out if treedepth has been reach
        // (this will probably leave a single node at one depth lower than the rest, right? How to fix this?)
        if(generationCount >= depth) {
            arrfree(gameStateCopy.legal_moves);
            arrfree(gameStateCopy.position_history);
            break;
        }
        // progress game with each parent nodes move which calculates all the next legal moves
        for(int i = generationCount - 1; i >= 0; --i) {
            assert(nodeIdx > 0 && "NodeIdx is zero");
            GAME_STATE state = progressGame(&gameStateCopy, moves[i]);
            assert(state != INVALID_MOVE_ATTEMPTED && "Invalid move attempted while creating tree");
        }
        // NOTE: shit's almost certainly buggy af
        // NOTE: calculateLegalMoves gets called inside playMove, it seems to cause a significnant slowdown but is necessary for getStateForPosition
        //       to work properly, perhaps a better way is required.
        // add new node for each legal move from the position arrived at
        for(int i = 0; i < arrlen(gameStateCopy.legal_moves); ++i) {
            ChessGameState gameStateCopyCopy = gameStateCopy;
            gameStateCopyCopy.legal_moves = NULL;
            playMove(&gameStateCopyCopy, gameStateCopy.legal_moves[i], TRUE);
            tree = newTreeNode(tree, nodeIdx, gameStateCopy.legal_moves[i], getPositionEvaluation(&gameStateCopyCopy.board,
                                                                                                  getStateForPosition(&gameStateCopyCopy)));
            arrfree(gameStateCopyCopy.legal_moves);
        }

        arrfree(gameStateCopy.legal_moves);
        arrfree(gameStateCopy.position_history);
        arrfree(gameStateCopy.move_history);
    }

    return tree;
}

Move _calculateMoveToPlay(ChessGameState *gameState) {

    // things get very slow at a depth value higher than three, optimizations will be necessary
    const int moveDepth = 3;
    gameState->tree = createMoveTree(*gameState, gameState->tree, moveDepth);

    TreeNode bestChild = gameState->tree[gameState->tree[0].firstChildIdx];
    for(int i = 0; i < gameState->tree[0].childCount; ++i) {
        TreeNode child = gameState->tree[gameState->tree[0].firstChildIdx + i];
        if(gameState->board.colorToPlay == PLAYER_WHITE) {
            if(child.value > bestChild.value)
                bestChild = child;
        }
        else {
            if(child.value < bestChild.value)
                bestChild = child;
        }
    }
    return bestChild.move;
}
