#include "root.h"
#include <stdio.h>
#include "../include/raylib.h"

#define SQUARE_SIZE 64
#define TREE_VIEW_WIDTH ((SQUARE_SIZE) * (4))

int main(void) {
    int windowWidth = SQUARE_SIZE * ROW_SIZE + TREE_VIEW_WIDTH;
    int windowHeight = SQUARE_SIZE * COL_SIZE;
    InitWindow(windowWidth, windowHeight, "Chess");
    if(!IsWindowReady()) {
        printf("Failed to open window\n");
        return 1;
    }

    Texture2D pieceTextures[12];
    {
        pieceTextures[WHITE_PAWN] = LoadTexture("../assets/WHITE_PAWN.png");
        pieceTextures[WHITE_KNIGHT] = LoadTexture("../assets/WHITE_KNIGHT.png");
        pieceTextures[WHITE_BISHOP] = LoadTexture("../assets/WHITE_BISHOP.png");
        pieceTextures[WHITE_ROOK] = LoadTexture("../assets/WHITE_ROOK.png");
        pieceTextures[WHITE_QUEEN] = LoadTexture("../assets/WHITE_QUEEN.png");
        pieceTextures[WHITE_KING] = LoadTexture("../assets/WHITE_KING.png");

        pieceTextures[BLACK_PAWN] = LoadTexture("../assets/BLACK_PAWN.png");
        pieceTextures[BLACK_KNIGHT] = LoadTexture("../assets/BLACK_KNIGHT.png");
        pieceTextures[BLACK_BISHOP] = LoadTexture("../assets/BLACK_BISHOP.png");
        pieceTextures[BLACK_ROOK] = LoadTexture("../assets/BLACK_ROOK.png");
        pieceTextures[BLACK_QUEEN] = LoadTexture("../assets/BLACK_QUEEN.png");
        pieceTextures[BLACK_KING] = LoadTexture("../assets/BLACK_KING.png");
    }
    for(int i = 0; i < ARRAY_LENGTH(pieceTextures); ++i) {
        if(!IsTextureReady(pieceTextures[i])) {
            printf("Failed to load texture at index: %d\n", i);
        }
    }

    ChessGameState gameState = initGame(HUMAN, BOT);
    GAME_STATE endState = CONTINUE;

    int selectedPieceIndex = -1;
    int mouseSquareIdx = -1;
    int promotionSquareIdx = -1;
    int boardToDraw = 0;
    int showWinner = FALSE;
    Move attemptedMove = NO_MOVE;
    int shouldPlay = TRUE;
    while(shouldPlay) {
        if(IsKeyPressed(KEY_Q)) shouldPlay = FALSE;
        if(IsKeyPressed(KEY_R)) {
            arrfree(gameState.legalMoves);
            arrfree(gameState.positionHistory);
            gameState.positionHistory = NULL;
            ChessGameState gameState = initGame(HUMAN, HUMAN);
            endState = CONTINUE;
            boardToDraw = arrlen(gameState.positionHistory) - 1;
            showWinner = FALSE;
        }
        if(IsKeyPressed(KEY_LEFT)) {
            if(boardToDraw > 0)
                boardToDraw--;
        }
        if(IsKeyPressed(KEY_RIGHT)) {
            if(boardToDraw < arrlen(gameState.positionHistory) - 1)
                boardToDraw++;
        }

        PLAYER_TYPE playerToPlay = gameState.board.colorToPlay == PLAYER_WHITE ? gameState.whiteType : gameState.blackType;
        if(playerToPlay == HUMAN && (endState == CONTINUE || endState == INVALID_MOVE_ATTEMPTED) &&
           boardToDraw == arrlen(gameState.positionHistory) - 1 && promotionSquareIdx == -1) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if(mouseSquareIdx != -1 && gameState.board.position[mouseSquareIdx] != EMPTY) selectedPieceIndex = mouseSquareIdx;
            }

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                attemptedMove = (Move){ selectedPieceIndex, mouseSquareIdx };
                if(gameState.board.position[selectedPieceIndex] == PAWN(gameState.board.colorToPlay) &&
                   (RANK(attemptedMove.destIndex) == 0 || RANK(attemptedMove.destIndex) == COL_SIZE - 1)) {
                    for(int i = 0; i < arrlen(gameState.legalMoves); ++i) {
                        if(gameState.legalMoves[i].startIndex == attemptedMove.startIndex &&
                           gameState.legalMoves[i].destIndex == attemptedMove.destIndex) {
                            promotionSquareIdx = attemptedMove.destIndex;
                        }
                    }
                }
                if(promotionSquareIdx == -1) {
                    endState = progressGame(&gameState, attemptedMove);
                    boardToDraw = arrlen(gameState.positionHistory) - 1;
                }
                selectedPieceIndex = -1;
            }
        }

        if(promotionSquareIdx != -1) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if(mouseSquareIdx == promotionSquareIdx) {
                    attemptedMove.type = PROMOTION_TO_KNIGHT;
                    endState = progressGame(&gameState, attemptedMove);
                    boardToDraw = arrlen(gameState.positionHistory) - 1;
                    promotionSquareIdx = -1;
                }
                else if(mouseSquareIdx == (gameState.board.colorToPlay == PLAYER_WHITE ? SOUTH(promotionSquareIdx, 1) : NORTH(promotionSquareIdx, 1))) {
                    attemptedMove.type = PROMOTION_TO_BISHOP;
                    endState = progressGame(&gameState, attemptedMove);
                    boardToDraw = arrlen(gameState.positionHistory) - 1;
                    promotionSquareIdx = -1;
                }
                else if(mouseSquareIdx == (gameState.board.colorToPlay == PLAYER_WHITE ? SOUTH(promotionSquareIdx, 2) : NORTH(promotionSquareIdx, 2))) {
                    attemptedMove.type = PROMOTION_TO_ROOK;
                    endState = progressGame(&gameState, attemptedMove);
                    boardToDraw = arrlen(gameState.positionHistory) - 1;
                    promotionSquareIdx = -1;
                }
                else if(mouseSquareIdx == (gameState.board.colorToPlay == PLAYER_WHITE ? SOUTH(promotionSquareIdx, 3) : NORTH(promotionSquareIdx, 3))) {
                    attemptedMove.type = PROMOTION_TO_QUEEN;
                    endState = progressGame(&gameState, attemptedMove);
                    boardToDraw = arrlen(gameState.positionHistory) - 1;
                    promotionSquareIdx = -1;
                }
            }
        }

        // update
        if(playerToPlay == BOT && endState == CONTINUE) {
            attemptedMove = calculateMoveToPlay(&gameState);
            endState = progressGame(&gameState, attemptedMove);
            boardToDraw = arrlen(gameState.positionHistory) - 1;
        }

        if(endState != CONTINUE && endState != INVALID_MOVE_ATTEMPTED) {
            showWinner = TRUE;
        }
        Vector2 mousePos = GetMousePosition();
        mouseSquareIdx = IDX_2D((int)(mousePos.x / SQUARE_SIZE), (int)(mousePos.y / SQUARE_SIZE));

        // render
        BeginDrawing();
        ClearBackground(WHITE);

        // drawing board
        for(int i = 0; i < BOARD_SIZE; ++i) {
            Color color = (RANK(i) + FILE(i)) % 2 ? DARKGREEN : BEIGE;
            DrawRectangle(FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
        }

        // drawing legal squares to move
        if(selectedPieceIndex != -1) {
            for(int i = 0; i < arrlen(gameState.legalMoves); ++i) {
                if(gameState.legalMoves[i].startIndex == selectedPieceIndex) {
                    DrawRectangle(FILE(gameState.legalMoves[i].destIndex) * SQUARE_SIZE,
                                  RANK(gameState.legalMoves[i].destIndex) * SQUARE_SIZE,
                                   SQUARE_SIZE, SQUARE_SIZE, RED);
                }
            }
        }

        // drawing pieces
        for(int i = 0; i < BOARD_SIZE; ++i) {
            if(gameState.positionHistory[boardToDraw].position[i] != EMPTY) {
                PIECE_TYPE pieceType = gameState.positionHistory[boardToDraw].position[i];
                if(i != selectedPieceIndex && (i != attemptedMove.startIndex || promotionSquareIdx == -1))
                    DrawTexture(pieceTextures[pieceType], FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, RAYWHITE);
            }
        }

        // drawing selected piece and highlighting square that mouse is hovering over
        if(selectedPieceIndex != -1) {
            Vector2 mousePos = GetMousePosition();
            mousePos = (Vector2){ mousePos.x - 32, mousePos.y - 32 };
            DrawRectangle(FILE(mouseSquareIdx) * SQUARE_SIZE, RANK(mouseSquareIdx) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, (Color){ 200, 200, 200, 200 });
            DrawTextureV(pieceTextures[gameState.board.position[selectedPieceIndex]], mousePos, RAYWHITE);
        }

        // drawing selection menu for piece promotion
        if(promotionSquareIdx != -1) {
            DrawRectangle(FILE(promotionSquareIdx) * SQUARE_SIZE,
                          gameState.board.colorToPlay == PLAYER_WHITE ? RANK(promotionSquareIdx) * SQUARE_SIZE : RANK(NORTH(promotionSquareIdx, 3)) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE * 4, GRAY);
            for(int i = KNIGHT(gameState.board.colorToPlay); i <= QUEEN(gameState.board.colorToPlay); ++i) {
                DrawTexture(pieceTextures[i], FILE(promotionSquareIdx) * SQUARE_SIZE,
                            gameState.board.colorToPlay == PLAYER_WHITE ?
                            RANK(SOUTH(promotionSquareIdx, i - KNIGHT(gameState.board.colorToPlay))) * SQUARE_SIZE :
                            RANK(NORTH(promotionSquareIdx, i - KNIGHT(gameState.board.colorToPlay))) * SQUARE_SIZE, WHITE);
            }
        }

        if(endState == INVALID_MOVE_ATTEMPTED) {
            DrawText("Invalid move attempted", 24, 24, 18, WHITE);
        }

        // drawing tree view
        if(gameState.tree) {
            for(int i = 0; i < gameState.tree[0].childCount; ++i) {
                char moveBuf[8];
                DrawText(convertMoveToChessNotation(gameState.positionHistory[arrlen(gameState.positionHistory) - 2], gameState.tree[gameState.tree[0].firstChildIdx + i].move, moveBuf), SQUARE_SIZE * ROW_SIZE + 12, 24 * i, 12, BLACK);

                char valBuf[8];
                sprintf(valBuf, "%4.2f\n", gameState.tree[gameState.tree[0].firstChildIdx + i].value);
                DrawText(valBuf, SQUARE_SIZE * ROW_SIZE + 60, 24 * i, 12, BLACK);
            }
        }

        // drawing end state
        if(showWinner) {
            DrawRectangle(0, 0, windowWidth, windowHeight, (Color){ 200, 200, 200, 200});
            switch(endState) {
                case DRAW:
                    DrawText("Draw... \nNobody wins.", windowWidth / 2 - 50, windowHeight / 2 - 50, 24, BLACK);
                    break;
                case WHITE_WIN:
                    DrawText("Checkmate! \nWhite wins.", windowWidth / 2 - 50, windowHeight / 2 - 50, 24, BLACK);
                    break;
                case BLACK_WIN:
                    DrawText("Checkmate! \nBlack wins.", windowWidth / 2 - 50, windowHeight / 2 - 50, 24, BLACK);
                    break;
                default:
                    break;
            }
        }

        EndDrawing();
    }
    return 1;
}
