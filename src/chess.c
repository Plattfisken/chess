#pragma once
#include "root.h"

void setupStartingPosition(ChessGameState *state) {
    for(int i = 0; i < ROW_SIZE; ++i) {
        state->board.position[IDX_2D(i,1)] = BLACK_PAWN;
        state->board.position[IDX_2D(i,6)] = WHITE_PAWN;

        for(int u = 2; u < 6; ++u) {
            state->board.position[IDX_2D(i,u)] = EMPTY;
        }
    }
    state->board.position[IDX_2D(0,0)] = BLACK_ROOK;
    state->board.position[IDX_2D(7,0)] = BLACK_ROOK;

    state->board.position[IDX_2D(1,0)] = BLACK_KNIGHT;
    state->board.position[IDX_2D(6,0)] = BLACK_KNIGHT;

    state->board.position[IDX_2D(2,0)] = BLACK_BISHOP;
    state->board.position[IDX_2D(5,0)] = BLACK_BISHOP;

    state->board.position[IDX_2D(3,0)] = BLACK_QUEEN;
    state->board.position[IDX_2D(4,0)] = BLACK_KING;

    state->board.position[IDX_2D(0,7)] = WHITE_ROOK;
    state->board.position[IDX_2D(7,7)] = WHITE_ROOK;

    state->board.position[IDX_2D(1,7)] = WHITE_KNIGHT;
    state->board.position[IDX_2D(6,7)] = WHITE_KNIGHT;

    state->board.position[IDX_2D(2,7)] = WHITE_BISHOP;
    state->board.position[IDX_2D(5,7)] = WHITE_BISHOP;

    state->board.position[IDX_2D(3,7)] = WHITE_QUEEN;
    state->board.position[IDX_2D(4,7)] = WHITE_KING;

}

PLAYER_COLOR getPieceColor(PIECE_TYPE piece) {
    if(piece == EMPTY) return -1;
    if(piece >= WHITE_PAWN && piece <= WHITE_KING) return PLAYER_WHITE;
    return PLAYER_BLACK;
}

Board _makeMove(Board board, Move move) {
    board.position[move.destIndex] = board.position[move.startIndex];
    board.position[move.startIndex] = EMPTY;
    switch(move.type) {
        case NO_MOVE: {
            break;
        }
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

Move *addMovesInDirection(const Board *board, Move *legal_moves, int startIdx, int stepsToCheck, int stepsX, int stepsY, int (*dir)(int, int, int)) {
    int iterations = 0;
    for(int startingIdx = dir(startIdx, stepsY, stepsX);
        iterations < stepsToCheck && startingIdx != OUT_OF_BOUNDS;
        startingIdx = dir(startingIdx, stepsY, stepsX))
    {
        Move move = { startIdx, startingIdx, REGULAR };
        arrpush(legal_moves, move);
        if(board->position[startingIdx] != EMPTY) break;
        ++iterations;
    }
    return legal_moves;
}

int moveLeavesKingInCheck(ChessGameState state, Move move) {
    // simulate the move on the board, calculate the new moves to see if king is attacked,
    // when calling calculateLegalMoves() the flag shouldCheckKingInCheck needs to be set to zero to avoid infinite recursion.

    // make new list, so that it won't interfere with the actual game.
    state.legal_moves = NULL;
    playMove(&state, move, FALSE);

    for(int i = 0; i < arrlen(state.legal_moves); ++i) {
        int attackedSquareIdx = state.legal_moves[i].destIndex;
        if(state.board.position[attackedSquareIdx] == KING(!state.board.colorToPlay)) {
            arrfree(state.legal_moves);
            return TRUE;
        }
    }
    arrfree(state.legal_moves);
    return FALSE;
}

// board, legalMoves, positionHistory
Move *calculateLegalMoves(ChessGameState *gameState, int shouldCheckKingInCheck) {
    // clear list
    size_t legalMoveCount = arrlen(gameState->legal_moves);
    if(legalMoveCount > 0)
        arrdeln(gameState->legal_moves, 0, arrlen(gameState->legal_moves));

    PLAYER_COLOR colorToPlay = gameState->board.colorToPlay;
    PLAYER_COLOR enemyColor = !colorToPlay;
    for(int boardIdx = 0; boardIdx < BOARD_SIZE; ++boardIdx) {
        PIECE_TYPE piece = gameState->board.position[boardIdx];

        if(piece == PAWN(colorToPlay)) {
            int forward = colorToPlay == PLAYER_WHITE ? NORTH(boardIdx, 1) : SOUTH(boardIdx, 1);
            int forwardTwoStep = colorToPlay == PLAYER_WHITE ? NORTH(boardIdx, 2) : SOUTH(boardIdx, 2);

            int startingRank = colorToPlay == PLAYER_WHITE ? COL_SIZE - 2 : 1;
            if(RANK(boardIdx) == startingRank && gameState->board.position[forward] == EMPTY && gameState->board.position[forwardTwoStep] == EMPTY) {
                Move moveTwoStepForward = { boardIdx, forwardTwoStep, REGULAR };
                arrpush(gameState->legal_moves, moveTwoStepForward);
            }

            int promotionRank = colorToPlay == PLAYER_WHITE ? 0 : COL_SIZE - 1;
            int oneBelowPromotionRank = colorToPlay == PLAYER_WHITE ? promotionRank + 1 : promotionRank - 1;
            if(RANK(boardIdx) != promotionRank && gameState->board.position[forward] == EMPTY) {
                if(RANK(boardIdx) == oneBelowPromotionRank) {
                    Move promoteToKnight = { boardIdx, forward, PROMOTION_TO_KNIGHT };
                    arrpush(gameState->legal_moves, promoteToKnight);

                    Move promoteToBishop = { boardIdx, forward, PROMOTION_TO_BISHOP };
                    arrpush(gameState->legal_moves, promoteToBishop);

                    Move promoteToRook = { boardIdx, forward, PROMOTION_TO_ROOK };
                    arrpush(gameState->legal_moves, promoteToRook);

                    Move promoteToQueen = { boardIdx, forward, PROMOTION_TO_QUEEN };
                    arrpush(gameState->legal_moves, promoteToQueen);
                }
                else {
                    Move moveForward = { boardIdx, forward, REGULAR };
                    arrpush(gameState->legal_moves, moveForward);
                }
            }

            int forwardDiagonalWest = colorToPlay == PLAYER_WHITE ? NORTH_WEST(boardIdx,1,1) : SOUTH_WEST(boardIdx,1,1);
            if(getPieceColor(gameState->board.position[forwardDiagonalWest]) == enemyColor) {
                if(FILE(boardIdx) != 0) {
                    if(RANK(boardIdx) == oneBelowPromotionRank) {
                        Move promoteToKnight = { boardIdx, forwardDiagonalWest, PROMOTION_TO_KNIGHT };
                        arrpush(gameState->legal_moves, promoteToKnight);

                        Move promoteToBishop = { boardIdx, forwardDiagonalWest, PROMOTION_TO_BISHOP };
                        arrpush(gameState->legal_moves, promoteToBishop);

                        Move promoteToRook = { boardIdx, forwardDiagonalWest, PROMOTION_TO_ROOK };
                        arrpush(gameState->legal_moves, promoteToRook);

                        Move promoteToQueen = { boardIdx, forwardDiagonalWest, PROMOTION_TO_QUEEN };
                        arrpush(gameState->legal_moves, promoteToQueen);
                    }
                    else {
                        Move moveForwardDiagonalWest = { boardIdx, forwardDiagonalWest, REGULAR };
                        arrpush(gameState->legal_moves, moveForwardDiagonalWest);
                    }
                }
            }

            int forwardDiagonalEast = colorToPlay == PLAYER_WHITE ? NORTH_EAST(boardIdx,1,1) : SOUTH_EAST(boardIdx,1,1);
            if(getPieceColor(gameState->board.position[forwardDiagonalEast]) == enemyColor) {
                if(FILE(boardIdx) != ROW_SIZE - 1) {
                    if(RANK(boardIdx) == oneBelowPromotionRank) {
                        Move promoteToKnight = { boardIdx, forwardDiagonalEast, PROMOTION_TO_KNIGHT };
                        arrpush(gameState->legal_moves, promoteToKnight);

                        Move promoteToBishop = { boardIdx, forwardDiagonalEast, PROMOTION_TO_BISHOP };
                        arrpush(gameState->legal_moves, promoteToBishop);

                        Move promoteToRook = { boardIdx, forwardDiagonalEast, PROMOTION_TO_ROOK };
                        arrpush(gameState->legal_moves, promoteToRook);

                        Move promoteToQueen = { boardIdx, forwardDiagonalEast, PROMOTION_TO_QUEEN };
                        arrpush(gameState->legal_moves, promoteToQueen);
                    }
                    else {
                        Move moveForwardDiagonalEast = { boardIdx, forwardDiagonalEast, REGULAR };
                        arrpush(gameState->legal_moves, moveForwardDiagonalEast);
                    }
                }
            }

            int enPassantRank = colorToPlay == PLAYER_WHITE ? 3 : 4;
            int enemyPawnStartingRank = colorToPlay == PLAYER_WHITE ? 1 : COL_SIZE - 2;
            if(RANK(boardIdx) == enPassantRank && gameState->board.position[WEST(boardIdx, 1)] == PAWN(enemyColor)
                && gameState->position_history[arrlen(gameState->position_history) - 2]
                    .position[IDX_2D(FILE(boardIdx) - 1, enemyPawnStartingRank)] == PAWN(enemyColor)) {
                Move moveForwardDiagonalWest = { boardIdx, forwardDiagonalWest, EN_PASSANT };
                arrpush(gameState->legal_moves, moveForwardDiagonalWest);
            }

            if(RANK(boardIdx) == enPassantRank && gameState->board.position[EAST(boardIdx, 1)] == PAWN(enemyColor)
                && gameState->position_history[arrlen(gameState->position_history) - 2]
                    .position[IDX_2D(FILE(boardIdx) + 1, enemyPawnStartingRank)] == PAWN(enemyColor)) {
                Move moveForwardDiagonalEast = { boardIdx, forwardDiagonalEast, EN_PASSANT };
                arrpush(gameState->legal_moves, moveForwardDiagonalEast);
            }
        }
        else if(piece == KNIGHT(colorToPlay)) {
            /* NOTE: enums here are used as a constant to avoid compiler warnings,
             * using const does not remove compiler warnings and #define is global scope. */
            int (*directionsToCheck[]) (int, int, int) = { northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = 1;
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx, steps, 1, 2, directionsToCheck[i]);
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx, steps, 2, 1, directionsToCheck[i]);
            }
        }
        else if(piece == BISHOP(colorToPlay)) {
            int (*directionsToCheck[]) (int, int, int) = { northWest, northEast, southWest, southEast };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }
        }
        else if(piece == ROOK(colorToPlay)) {
            int (*directionsToCheck[]) (int, int) = { north, south, west, east };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx,
                                                            steps, 1, 1, (int(*)(int,int,int))directionsToCheck[i]);
            }
        }
        else if(piece == QUEEN(colorToPlay)) {
            int (*directionsToCheck[]) (int, int, int) = {
                (int(*)(int,int,int))north,
                (int(*)(int,int,int))south,
                (int(*)(int,int,int))west,
                (int(*)(int,int,int))east,
                northWest, northEast,
                southWest, southEast
            };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = ROW_SIZE;
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }
        }
        else if(piece == KING(colorToPlay)) {
            // regular moves
            int (*directionsToCheck[]) (int, int, int) = {
                (int(*)(int,int,int))north,
                (int(*)(int,int,int))south,
                (int(*)(int,int,int))west,
                (int(*)(int,int,int))east,
                northWest, northEast,
                southWest, southEast
            };
            for(int i = 0; i < ARRAY_LENGTH(directionsToCheck); ++i) {
                int steps = 1;
                gameState->legal_moves = addMovesInDirection(&gameState->board, gameState->legal_moves, boardIdx, steps, 1, 1, directionsToCheck[i]);
            }

            // castling
            int shortCastlingRights = TRUE;
            int longCastlingRights = TRUE;

            int kingOriginalPos = colorToPlay == PLAYER_WHITE ? IDX_2D(4, COL_SIZE - 1) : IDX_2D(4, 0);
            int westRookOriginalPos = IDX_2D(0, RANK(kingOriginalPos));
            int eastRookOriginalPos = IDX_2D(ROW_SIZE - 1, RANK(kingOriginalPos));

            for(int i = 0; i < arrlen(gameState->position_history); ++i) {
                if(gameState->position_history[i].position[kingOriginalPos] != KING(colorToPlay)) {
                    shortCastlingRights = FALSE;
                    longCastlingRights = FALSE;
                }
                if(gameState->position_history[i].position[westRookOriginalPos] != ROOK(colorToPlay))
                    longCastlingRights = FALSE;
                if(gameState->position_history[i].position[eastRookOriginalPos] != ROOK(colorToPlay))
                    shortCastlingRights = FALSE;
            }

            if(shortCastlingRights && gameState->board.position[EAST(boardIdx, 1)] == EMPTY
               && shouldCheckKingInCheck
               && !moveLeavesKingInCheck(*gameState, (Move){ boardIdx, boardIdx, REGULAR })
               && !moveLeavesKingInCheck(*gameState, (Move){ boardIdx, EAST(boardIdx, 1), REGULAR })) {
                Move shortCastle = { boardIdx, EAST(boardIdx, 2), SHORT_CASTLE};
                arrpush(gameState->legal_moves, shortCastle);
            }
            if(longCastlingRights && gameState->board.position[WEST(boardIdx, 1)] == EMPTY
               && gameState->board.position[WEST(boardIdx, 3)] == EMPTY
               && shouldCheckKingInCheck
               && !moveLeavesKingInCheck(*gameState, (Move){ boardIdx, boardIdx, REGULAR })
               && !moveLeavesKingInCheck(*gameState, (Move){ boardIdx, WEST(boardIdx, 1), REGULAR })) {
                Move longCastle = { boardIdx, WEST(boardIdx, 2), LONG_CASTLE };
                arrpush(gameState->legal_moves, longCastle);
            }
        }
    }

    for(int i = 0; i < arrlen(gameState->legal_moves); ++i) {
        Move move = gameState->legal_moves[i];
        // delete moves that takes a friendly piece
        if(getPieceColor(gameState->board.position[move.destIndex]) == colorToPlay) {
            arrdel(gameState->legal_moves, i);
            --i;
            continue;
        }
        // delete moves that leaves king in check
        if(shouldCheckKingInCheck && moveLeavesKingInCheck(*gameState, move)) {
            arrdel(gameState->legal_moves, i);
            --i;
            continue;
        }
    }
    return gameState->legal_moves;
}

char _getFileLetterFromIndex(int fileIndex) {
    switch(fileIndex) {
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return 'c';
        case 3:
            return 'd';
        case 4:
            return 'e';
        case 5:
            return 'f';
        case 6:
            return 'g';
        case 7:
            return 'h';
        default:
            return 0;
    }
}
char *convertMoveToShortAlgebraicNotation(Board board, Move move, char *buf) {
    int pos = 0;
    if(move.type == SHORT_CASTLE) {
        memcpy(buf, "0-0", sizeof("0-0"));
        return buf;
    }
    if(move.type == LONG_CASTLE) {
        memcpy(buf, "0-0-0", sizeof("0-0-0"));
        return buf;
    }

    // piece
    switch(board.position[move.startIndex]) {
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            buf[pos++] = 'N';
            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            buf[pos++] = 'B';
            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            buf[pos++] = 'R';
            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            buf[pos++] = 'Q';
            break;
        case WHITE_KING:
        case BLACK_KING:
            buf[pos++] = 'K';
            break;
        default:
            break;
    }

    // capture
    if(board.position[move.destIndex] != EMPTY) {
        if(board.position[move.startIndex] == PAWN(board.colorToPlay))
            buf[pos++] = _getFileLetterFromIndex(FILE(move.startIndex));
        buf[pos++] = 'x';
    }

    // file
    buf[pos++] = _getFileLetterFromIndex(FILE(move.destIndex));

    // rank
    buf[pos++] = '0' + (COL_SIZE - RANK(move.destIndex));

    // null-terminate
    buf[pos] = 0;
    return buf;
}

GAME_STATE getStateForPosition(const ChessGameState *state) {
    // Check position for checkmate, stalemate or continue. Assumes the caller has calculated legal moves
    if(arrlen(state->legal_moves) == 0) {
        if(moveLeavesKingInCheck(*state, (Move){0,0,NO_MOVE}))
            return state->board.colorToPlay == PLAYER_WHITE ? BLACK_WIN : WHITE_WIN;
        else
            return DRAW;
    }
    else
        return CONTINUE;
}

void playMove(ChessGameState *state, Move move, int shouldCheckKingInCheck) {
    // function to do all necessary steps in right order (first makeMove, then switch color, then calculateLegalMoves)
    // to make sure it is always done in the right order, chessbots should use this function to simulate moves??
    state->board = _makeMove(state->board, move);
    state->board.colorToPlay = !state->board.colorToPlay;
    state->legal_moves = calculateLegalMoves(state, shouldCheckKingInCheck);
}

void init_game(ChessGameState *newGame, const GameSettings *settings) {
    setupStartingPosition(newGame);
    newGame->board.colorToPlay = PLAYER_WHITE;
    newGame->white_type = settings->white_type;
    newGame->black_type = settings->black_type;
    newGame->state = CONTINUE;
    newGame->legal_moves = NULL;
    newGame->position_history = NULL;
    newGame->move_history = NULL;
    newGame->tree = NULL;
    newGame->legal_moves = calculateLegalMoves(newGame, 1);
    arrpush(newGame->position_history, newGame->board);

    newGame->udp_ctx = (UdpContext){0};
    if(settings->white_type == REMOTE_PLAYER) {
        initRemotePlay(&(newGame->udp_ctx), 8181, 8080, settings->ip_address);
    }
    else if(settings->black_type == REMOTE_PLAYER) {
        initRemotePlay(&(newGame->udp_ctx), 8080, 8181, settings->ip_address);
    }
}

void deleteGameResources(ChessGameState *game) {
    // NOTE: This won't call free on the actual pointer passed in as I may wanna keep it stack allocated.
    if(game->legal_moves) arrfree(game->legal_moves);
    if(game->position_history) arrfree(game->position_history);
    if(game->move_history) arrfree(game->move_history);
    if(game->tree) arrfree(game->tree);
}

GAME_STATE progressGame(ChessGameState *state, Move attemptedMove) {
    assert(state->position_history && "Chess game not initialized");
    Move moveToPlay = {0};
    int foundMove = FALSE;
    for(int i = 0; i < arrlen(state->legal_moves); ++i) {
        if(state->legal_moves[i].startIndex == attemptedMove.startIndex &&
           state->legal_moves[i].destIndex == attemptedMove.destIndex) {
            foundMove = TRUE;
            moveToPlay = state->legal_moves[i];
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
        playMove(state, moveToPlay, TRUE);
        arrpush(state->position_history, state->board);
        arrpush(state->move_history, moveToPlay);
        GAME_STATE endState = getStateForPosition(state);
        if(endState != CONTINUE)
            arrfree(state->legal_moves);
        return endState;
    }

    // TODO: three-fold repetetion

    // TODO: 50 move rule...

    return INVALID_MOVE_ATTEMPTED;
}

TreeNode *newTreeNode(TreeNode *tree, int parentIdx, Move move, float val) {
    // create a new tree node with an associated move, the value of the position after this move will be calculated later
    TreeNode newNode;
    newNode.move = move;
    newNode.value = val;
    newNode.childCount = 0;
    newNode.firstChildIdx = -1;
    // set the root node's parentIdx to -1 if there are no nodes in the tree
    if(arrlen(tree) == 0)
        newNode.parentIdx = -1;
    else {
        newNode.parentIdx = parentIdx;
        TreeNode *parent = &tree[parentIdx];

        // TODO: insertion
        // checck the assertion logic, some stuff isnt working here
        {
            size_t treeLen = arrlen(tree);
            TreeNode lastNode = tree[treeLen - 1];
            assert(lastNode.parentIdx <= parentIdx && "insertion attempted");
        }

        if(parent->childCount == 0) {
            parent->firstChildIdx = arrlen(tree);
        }
        ++parent->childCount;
    }
    arrpush(tree, newNode);
    return tree;
}

int getMatchingMoveCount(Move moveWithoutType, Move *moveList, int moveListCount) {
    int matchingMoves = 0;
    for(int i = 0; i < moveListCount; ++i)
        if(moveWithoutType.startIndex == moveList[i].startIndex && moveWithoutType.destIndex == moveList[i].destIndex)
            ++matchingMoves;
    return matchingMoves;
}

Move getFirstMatchingMove(Move moveWithoutType, Move *moveList, int moveListCount) {
    for(int i = 0; i < moveListCount; ++i)
        if(moveWithoutType.startIndex == moveList[i].startIndex && moveWithoutType.destIndex == moveList[i].destIndex)
            return moveList[i];
    return (Move){0, 0, NO_MOVE};
}

Move _getLocalPlayerMove(ChessGameState *gameState, InputHandler input, Renderer *renderer) {

    int matchingMoveCount = getMatchingMoveCount(input.attemptedMove, gameState->legal_moves, arrlen(gameState->legal_moves));
    if(matchingMoveCount == 0)
        return input.attemptedMove;
    if(matchingMoveCount == 1) {
        Move moveToPlay = getFirstMatchingMove(input.attemptedMove, gameState->legal_moves, arrlen(gameState->legal_moves));
        assert(moveToPlay.type != NO_MOVE && "Move not found in getFirstMatchingMove");
        return moveToPlay;
    }

    // multiple valid movetypes means promotion
    input.attemptedMove.type = renderer_add_interactive_promotion_selection_menu(renderer, input.attemptedMove.destIndex, gameState->board.colorToPlay);
    return input.attemptedMove;
}

Move get_next_move(ChessGameState *gameState, PLAYER_TYPE playerType, InputHandler input, Renderer *renderer) {
    switch(playerType) {
        case BOT:
            return _calculateMoveToPlay(gameState);
            break;
        case LOCAL_PLAYER:
            return _getLocalPlayerMove(gameState, input, renderer);
            break;
        case REMOTE_PLAYER:
            return _getRemotePlayerMove(&(gameState->udp_ctx), gameState->legal_moves);
            break;
        default:
            assert(0 && "Should not be here...");
            break;
    }
}

