#pragma once
#include "root.h"
#include <stdio.h>

#include "chess.h"


ChessGameState setupStartingPosition() {
    ChessGameState newGame;
    for(int i = 0; i < ROW_SIZE; ++i) {
        newGame.board.position[IDX_2D(i,1)] = BLACK_PAWN;
        newGame.board.position[IDX_2D(i,6)] = WHITE_PAWN;

        for(int u = 2; u < 6; ++u) {
            newGame.board.position[IDX_2D(i,u)] = EMPTY;
        }
    }
    newGame.board.position[IDX_2D(0,0)] = BLACK_ROOK;
    newGame.board.position[IDX_2D(7,0)] = BLACK_ROOK;

    newGame.board.position[IDX_2D(1,0)] = BLACK_KNIGHT;
    newGame.board.position[IDX_2D(6,0)] = BLACK_KNIGHT;

    newGame.board.position[IDX_2D(2,0)] = BLACK_BISHOP;
    newGame.board.position[IDX_2D(5,0)] = BLACK_BISHOP;

    newGame.board.position[IDX_2D(3,0)] = BLACK_QUEEN;
    newGame.board.position[IDX_2D(4,0)] = BLACK_KING;

    newGame.board.position[IDX_2D(0,7)] = WHITE_ROOK;
    newGame.board.position[IDX_2D(7,7)] = WHITE_ROOK;

    newGame.board.position[IDX_2D(1,7)] = WHITE_KNIGHT;
    newGame.board.position[IDX_2D(6,7)] = WHITE_KNIGHT;

    newGame.board.position[IDX_2D(2,7)] = WHITE_BISHOP;
    newGame.board.position[IDX_2D(5,7)] = WHITE_BISHOP;

    newGame.board.position[IDX_2D(3,7)] = WHITE_QUEEN;
    newGame.board.position[IDX_2D(4,7)] = WHITE_KING;

    newGame.board.colorToPlay = PLAYER_WHITE;
    newGame.legalMoves = NULL;
    newGame.positionHistory = NULL;
    return newGame;
}

PLAYER_COLOR getPieceColor(PIECE_TYPE piece) {
    if(piece == EMPTY) return -1;
    if(piece >= WHITE_PAWN && piece <= WHITE_KING) return PLAYER_WHITE;
    return PLAYER_BLACK;
}

Board makeMove(Board board, Move move) {
    board.position[move.destIndex] = board.position[move.startIndex];
    board.position[move.startIndex] = EMPTY;
    switch(move.type) {
        case REGULAR: {
            break;
        }
        case SHORT_CASTLE: {
            board.position[IDX_2D(5, RANK(move.startIndex))] = ROOK(board.colorToPlay);
            board.position[IDX_2D(7, RANK(move.startIndex))] = EMPTY;
            break;
        }
        case LONG_CASTLE: {
            board.position[EAST(move.destIndex, 1)] = ROOK(board.colorToPlay);
            board.position[IDX_2D(0, RANK(move.startIndex))] = EMPTY;
            break;
        }
        case EN_PASSANT: {
            if(board.colorToPlay == PLAYER_WHITE)
                board.position[SOUTH(move.destIndex, 1)] = EMPTY;
            else
                board.position[NORTH(move.destIndex, 1)] = EMPTY;
            break;
        }
        case PROMOTION_TO_KNIGHT: {
            board.position[move.destIndex] = KNIGHT(board.colorToPlay);
            break;
        }
        case PROMOTION_TO_BISHOP: {
            board.position[move.destIndex] = BISHOP(board.colorToPlay);
            break;
        }
        case PROMOTION_TO_ROOK: {
            board.position[move.destIndex] = ROOK(board.colorToPlay);
            break;
        }
        case PROMOTION_TO_QUEEN: {
            board.position[move.destIndex] = QUEEN(board.colorToPlay);
            break;
        }
    }
    return board;
}

Move *addMovesInDirection(Board board, Move *legalMoves, int startIdx, int stepsToCheck, int stepsX, int stepsY, int (*dir)(int, int, int)) {
    int iterations = 0;
    for(int startingIdx = dir(startIdx, stepsY, stepsX);
        iterations < stepsToCheck && startingIdx != OUT_OF_BOUNDS;
        startingIdx = dir(startingIdx, stepsY, stepsX))
    {
        Move move = { startIdx, startingIdx, REGULAR };
        arrpush(legalMoves, move);
        if(board.position[startingIdx] != EMPTY) break;
        ++iterations;
    }
    return legalMoves;
}

int moveLeavesKingInCheck(ChessGameState gameState, Move move) {
    // simulate the move on the board, calculate the new moves to see if king is attacked,
    // when calling calculateLegalMoves() the flag shouldCheckKingInCheck needs to be set to zero to avoid infinite recursion.

    // make new list, so that it won't interfere with the actual game.
    gameState.legalMoves = NULL;
    gameState = playMove(gameState, move, FALSE);

    for(int i = 0; i < arrlen(gameState.legalMoves); ++i) {
        int attackedSquareIdx = gameState.legalMoves[i].destIndex;
        if(gameState.board.position[attackedSquareIdx] == KING(!gameState.board.colorToPlay)) {
            arrfree(gameState.legalMoves);
            return TRUE;
        }
    }
    arrfree(gameState.legalMoves);
    return FALSE;
}

Move *calculateLegalMoves(ChessGameState gameState, int shouldCheckKingInCheck) {
    // clear list
    size_t legalMoveCount = arrlen(gameState.legalMoves);
    if(legalMoveCount > 0)
        arrdeln(gameState.legalMoves, 0, arrlen(gameState.legalMoves));

    PLAYER_COLOR colorToPlay = gameState.board.colorToPlay;
    PLAYER_COLOR enemyColor = !colorToPlay;
    for(int boardIdx = 0; boardIdx < BOARD_SIZE; ++boardIdx) {
        PIECE_TYPE piece = gameState.board.position[boardIdx];

        if(piece == PAWN(colorToPlay)) {
            int forward = colorToPlay == PLAYER_WHITE ? NORTH(boardIdx, 1) : SOUTH(boardIdx, 1);
            int forwardTwoStep = colorToPlay == PLAYER_WHITE ? NORTH(boardIdx, 2) : SOUTH(boardIdx, 2);

            int startingRank = colorToPlay == PLAYER_WHITE ? COL_SIZE - 2 : 1;
            if(RANK(boardIdx) == startingRank && gameState.board.position[forward] == EMPTY && gameState.board.position[forwardTwoStep] == EMPTY) {
                Move moveTwoStepForward = { boardIdx, forwardTwoStep, REGULAR };
                arrpush(gameState.legalMoves, moveTwoStepForward);
            }

            int promotionRank = colorToPlay == PLAYER_WHITE ? 0 : COL_SIZE - 1;
            int oneBelowPromotionRank = colorToPlay == PLAYER_WHITE ? promotionRank + 1 : promotionRank - 1;
            if(RANK(boardIdx) != promotionRank && gameState.board.position[forward] == EMPTY) {
                if(RANK(boardIdx) == oneBelowPromotionRank) {
                    Move promoteToKnight = { boardIdx, forward, PROMOTION_TO_KNIGHT };
                    arrpush(gameState.legalMoves, promoteToKnight);

                    Move promoteToBishop = { boardIdx, forward, PROMOTION_TO_BISHOP };
                    arrpush(gameState.legalMoves, promoteToBishop);

                    Move promoteToRook = { boardIdx, forward, PROMOTION_TO_ROOK };
                    arrpush(gameState.legalMoves, promoteToRook);

                    Move promoteToQueen = { boardIdx, forward, PROMOTION_TO_QUEEN };
                    arrpush(gameState.legalMoves, promoteToQueen);
                }
                else {
                    Move moveForward = { boardIdx, forward, REGULAR };
                    arrpush(gameState.legalMoves, moveForward);
                }
            }

            int forwardDiagonalWest = colorToPlay == PLAYER_WHITE ? NORTH_WEST(boardIdx,1,1) : SOUTH_WEST(boardIdx,1,1);
            if(getPieceColor(gameState.board.position[forwardDiagonalWest]) == enemyColor) {
                if(FILE(boardIdx) != 0) {
                    if(RANK(boardIdx) == oneBelowPromotionRank) {
                        Move promoteToKnight = { boardIdx, forwardDiagonalWest, PROMOTION_TO_KNIGHT };
                        arrpush(gameState.legalMoves, promoteToKnight);

                        Move promoteToBishop = { boardIdx, forwardDiagonalWest, PROMOTION_TO_BISHOP };
                        arrpush(gameState.legalMoves, promoteToBishop);

                        Move promoteToRook = { boardIdx, forwardDiagonalWest, PROMOTION_TO_ROOK };
                        arrpush(gameState.legalMoves, promoteToRook);

                        Move promoteToQueen = { boardIdx, forwardDiagonalWest, PROMOTION_TO_QUEEN };
                        arrpush(gameState.legalMoves, promoteToQueen);
                    }
                    else {
                        Move moveForwardDiagonalWest = { boardIdx, forwardDiagonalWest, REGULAR };
                        arrpush(gameState.legalMoves, moveForwardDiagonalWest);
                    }
                }
            }

            int forwardDiagonalEast = colorToPlay == PLAYER_WHITE ? NORTH_EAST(boardIdx,1,1) : SOUTH_EAST(boardIdx,1,1);
            if(getPieceColor(gameState.board.position[forwardDiagonalEast]) == enemyColor) {
                if(FILE(boardIdx) != ROW_SIZE - 1) {
                    if(RANK(boardIdx) == oneBelowPromotionRank) {
                        Move promoteToKnight = { boardIdx, forwardDiagonalEast, PROMOTION_TO_KNIGHT };
                        arrpush(gameState.legalMoves, promoteToKnight);

                        Move promoteToBishop = { boardIdx, forwardDiagonalEast, PROMOTION_TO_BISHOP };
                        arrpush(gameState.legalMoves, promoteToBishop);

                        Move promoteToRook = { boardIdx, forwardDiagonalEast, PROMOTION_TO_ROOK };
                        arrpush(gameState.legalMoves, promoteToRook);

                        Move promoteToQueen = { boardIdx, forwardDiagonalEast, PROMOTION_TO_QUEEN };
                        arrpush(gameState.legalMoves, promoteToQueen);
                    }
                    else {
                        Move moveForwardDiagonalEast = { boardIdx, forwardDiagonalEast, REGULAR };
                        arrpush(gameState.legalMoves, moveForwardDiagonalEast);
                    }
                }
            }

            int enPassantRank = colorToPlay == PLAYER_WHITE ? 3 : 4;
            int enemyPawnStartingRank = colorToPlay == PLAYER_WHITE ? 1 : COL_SIZE - 2;
            if(RANK(boardIdx) == enPassantRank && gameState.board.position[WEST(boardIdx, 1)] == PAWN(enemyColor)
                && gameState.positionHistory[arrlen(gameState.positionHistory) - 2]
                    .position[IDX_2D(FILE(boardIdx) - 1, enemyPawnStartingRank)] == PAWN(enemyColor)) {
                Move moveForwardDiagonalWest = { boardIdx, forwardDiagonalWest, EN_PASSANT };
                arrpush(gameState.legalMoves, moveForwardDiagonalWest);
            }

            if(RANK(boardIdx) == enPassantRank && gameState.board.position[EAST(boardIdx, 1)] == PAWN(enemyColor)
                && gameState.positionHistory[arrlen(gameState.positionHistory) - 2]
                    .position[IDX_2D(FILE(boardIdx) + 1, enemyPawnStartingRank)] == PAWN(enemyColor)) {
                Move moveForwardDiagonalEast = { boardIdx, forwardDiagonalEast, EN_PASSANT };
                arrpush(gameState.legalMoves, moveForwardDiagonalEast);
            }
        }
        else if(piece == KNIGHT(colorToPlay)) {
            /* NOTE: enums here are used as a constant to avoid compiler warnings,
             * using const does not remove compiler warnings and #define is global scope. */
            int (*directionsToCheck[]) (int, int, int) = { northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = 1;
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 1, 2, directionsToCheck[i]);
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 2, 1, directionsToCheck[i]);
            }
        }
        else if(piece == BISHOP(colorToPlay)) {
            int (*directionsToCheck[]) (int, int, int) = { northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }
        }
        else if(piece == ROOK(colorToPlay)) {
            int (*directionsToCheck[]) (int, int, int) = { north, south, west, east };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }
        }
        else if(piece == QUEEN(colorToPlay)) {
            int (*directionsToCheck[]) (int, int, int) = { north, south, west, east, northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }
        }
        else if(piece == KING(colorToPlay)) {
            // regular moves
            int (*directionsToCheck[]) (int, int, int) = { north, south, west, east, northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = 1;
                gameState.legalMoves = addMovesInDirection(gameState.board, gameState.legalMoves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }

            // castling
            int shortCastlingRights = TRUE;
            int longCastlingRights = TRUE;

            int kingOriginalPos = colorToPlay == PLAYER_WHITE ? IDX_2D(4, COL_SIZE - 1) : IDX_2D(4, 0);
            int westRookOriginalPos = IDX_2D(0, RANK(kingOriginalPos));
            int eastRookOriginalPos = IDX_2D(ROW_SIZE - 1, RANK(kingOriginalPos));

            for(int i = 0; i < arrlen(gameState.positionHistory); ++i) {
                if(gameState.positionHistory[i].position[kingOriginalPos] != KING(colorToPlay)) {
                    shortCastlingRights = FALSE;
                    longCastlingRights = FALSE;
                }
                if(gameState.positionHistory[i].position[westRookOriginalPos] != ROOK(colorToPlay))
                    longCastlingRights = FALSE;
                if(gameState.positionHistory[i].position[eastRookOriginalPos] != ROOK(colorToPlay))
                    shortCastlingRights = FALSE;
            }

            if(shortCastlingRights && gameState.board.position[EAST(boardIdx, 1)] == EMPTY
               && shouldCheckKingInCheck
               && !moveLeavesKingInCheck(gameState, (Move){ boardIdx, boardIdx, REGULAR })
               && !moveLeavesKingInCheck(gameState, (Move){ boardIdx, EAST(boardIdx, 1), REGULAR })) {
                Move shortCastle = { boardIdx, EAST(boardIdx, 2), SHORT_CASTLE};
                arrpush(gameState.legalMoves, shortCastle);
            }
            if(longCastlingRights && gameState.board.position[WEST(boardIdx, 1)] == EMPTY
               && gameState.board.position[WEST(boardIdx, 3)] == EMPTY
               && shouldCheckKingInCheck
               && !moveLeavesKingInCheck(gameState, (Move){ boardIdx, boardIdx, REGULAR })
               && !moveLeavesKingInCheck(gameState, (Move){ boardIdx, WEST(boardIdx, 1), REGULAR })) {
                Move longCastle = { boardIdx, WEST(boardIdx, 2), LONG_CASTLE };
                arrpush(gameState.legalMoves, longCastle);
            }
        }
    }

    for(int i = 0; i < arrlen(gameState.legalMoves); ++i) {
        Move move = gameState.legalMoves[i];
        // delete moves that takes a friendly piece
        if(getPieceColor(gameState.board.position[move.destIndex]) == colorToPlay) {
            arrdel(gameState.legalMoves, i);
            --i;
            continue;
        }
        // delete moves that leaves king in check
        if(shouldCheckKingInCheck && moveLeavesKingInCheck(gameState, move)) {
            arrdel(gameState.legalMoves, i);
            --i;
            continue;
        }
    }
    return gameState.legalMoves;
}

char *convertMoveToChessNotation(Board board, Move move, char *buf) {
    int pos = 0;

    // piece
    switch(board.position[move.startIndex]) {
        case WHITE_KNIGHT:
            buf[pos++] = 'N';
            break;
        case WHITE_BISHOP:
            buf[pos++] = 'B';
            break;
        case WHITE_ROOK:
            buf[pos++] = 'R';
            break;
        case WHITE_QUEEN:
            buf[pos++] = 'Q';
            break;
        case WHITE_KING:
            buf[pos++] = 'K';
            break;
        case BLACK_KNIGHT:
            buf[pos++] = 'N';
            break;
        case BLACK_BISHOP:
            buf[pos++] = 'B';
            break;
        case BLACK_ROOK:
            buf[pos++] = 'R';
            break;
        case BLACK_QUEEN:
            buf[pos++] = 'Q';
            break;
        case BLACK_KING:
            buf[pos++] = 'K';
            break;
        default:
            break;
    }

    // capture
    if(board.position[move.destIndex] != EMPTY)
        buf[pos++] = 'x';

    // file
    switch(FILE(move.destIndex)) {
        case 0:
            buf[pos++] = 'a';
            break;
        case 1:
            buf[pos++] = 'b';
            break;
        case 2:
            buf[pos++] = 'c';
            break;
        case 3:
            buf[pos++] = 'd';
            break;
        case 4:
            buf[pos++] = 'e';
            break;
        case 5:
            buf[pos++] = 'f';
            break;
        case 6:
            buf[pos++] = 'g';
            break;
        case 7:
            buf[pos++] = 'h';
            break;
    }

    // rank
    buf[pos++] = '0' + (8 - RANK(move.destIndex));

    buf[pos] = '\0';
    return buf;
}

GAME_STATE getStateForPosition(ChessGameState state) {
    // Check position for checkmate, stalemate or continue. Assumes the caller has calculated legal moves
    if(arrlen(state.legalMoves) == 0) {
        if(moveLeavesKingInCheck(state, NO_MOVE))
            return state.board.colorToPlay == PLAYER_WHITE ? BLACK_WIN : WHITE_WIN;
        else
            return DRAW;
    }
    else
        return CONTINUE;
}

ChessGameState playMove(ChessGameState state, Move move, int shouldCheckKingInCheck) {
    // function to do all necessary steps in right order (first makeMove, then switch color, then calculateLegalMoves)
    // to make sure it is always done in the right order, chessbots should use this function to simulate moves??
    state.board = makeMove(state.board, move);
    state.board.colorToPlay = !state.board.colorToPlay;
    state.legalMoves = calculateLegalMoves(state, shouldCheckKingInCheck);
    return state;
}

GAME_STATE progressGame(ChessGameState *state, Move attemptedMove) {
    if(!state->positionHistory) {
        *state = setupStartingPosition();
        state->legalMoves = calculateLegalMoves(*state, 1);
        arrpush(state->positionHistory, state->board);
    }

    Move moveToPlay = {0};
    int foundMove = FALSE;
    for(int i = 0; i < arrlen(state->legalMoves); ++i) {
        if(state->legalMoves[i].startIndex == attemptedMove.startIndex &&
           state->legalMoves[i].destIndex == attemptedMove.destIndex) {
            foundMove = TRUE;
            moveToPlay = state->legalMoves[i];
            if(moveToPlay.type >= PROMOTION_TO_KNIGHT) {
                if(attemptedMove.type != moveToPlay.type) {
                    foundMove = FALSE;
                }
            }
            if(foundMove)
                break;
        }
    }
    if(foundMove) {
        *state = playMove(*state, moveToPlay, TRUE);
        arrpush(state->positionHistory, state->board);
        GAME_STATE endState = getStateForPosition(*state);
        if(endState != CONTINUE)
            arrfree(state->legalMoves);
        return endState;
    }

    // TODO: three-fold repetetion

    // TODO: 50 move rule...

    return INVALID_MOVE_ATTEMPTED;
}
