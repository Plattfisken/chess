#include "root.h"

#define TREE_VIEW_WIDTH ((SQUARE_SIZE) * (4))

int main(int argc, char **argv) {
    assert(argc == 2 && "need args");
    char *color = argv[1];
    // int my_port = atoi(argv[2]);
    // int peer_port = atoi(argv[3]);
    PLAYER_COLOR playerPicked = strcmp(color, "w") == 0 ? PLAYER_WHITE : PLAYER_BLACK;

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

    ChessGameState gameState = {0};
    if(playerPicked == PLAYER_WHITE)
        initGame(&gameState, LOCAL_PLAYER, REMOTE_PLAYER);
    else
        initGame(&gameState, REMOTE_PLAYER, LOCAL_PLAYER);

    InputHandler input = initInputHandler();
    int boardToDraw = 0;
    PLAYER_TYPE playerToPlay = gameState.board.colorToPlay == PLAYER_WHITE ? gameState.whiteType : gameState.blackType;
    while(!WindowShouldClose()) {
        if(boardToDraw == arrlen(gameState.positionHistory) - 1 && playerToPlay == LOCAL_PLAYER)
            input = processInput(input);

        // Move this into process input
        if(IsKeyPressed(KEY_R)) {
            deleteGameResources(&gameState);
            initGame(&gameState, LOCAL_PLAYER, REMOTE_PLAYER);
            boardToDraw = arrlen(gameState.positionHistory) - 1;
        }
        if(IsKeyPressed(KEY_LEFT)) {
            if(boardToDraw > 0)
                boardToDraw--;
        }
        if(IsKeyPressed(KEY_RIGHT)) {
            if(boardToDraw < arrlen(gameState.positionHistory) - 1)
                boardToDraw++;
        }

        // update and render
        BeginDrawing();
        ClearBackground(WHITE);

        // drawing board
        for(int i = 0; i < BOARD_SIZE; ++i) {
            Color color = (RANK(i) + FILE(i)) % 2 ? DARKGREEN : BEIGE;
            DrawRectangle(FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
        }

        // drawing pieces
        for(int i = 0; i < BOARD_SIZE; ++i) {
            if(gameState.positionHistory[boardToDraw].position[i] != EMPTY) {
                PIECE_TYPE pieceType = gameState.positionHistory[boardToDraw].position[i];
                if(i != input.selectedPieceIdx && i != input.selectedPieceIdx)
                    DrawTexture(pieceTextures[pieceType], FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, RAYWHITE);
            }
        }

        // NOTE: the state INVALID_MOVE_ATTEMPTED is returned if progressGame is called with a moveToPlay that is not found in legalMoves,
        // but since getNextMove already matches the move from input with a move from legal moves then currently INVALID_MOVE_ATTEMPTED should
        // never occur from local input. I believe originally i intended whatever move that was inputted to be passed to progressGame, but it
        // became a bit difficult with Move.type, since you don't really input the type normally, the type can be inferred from the
        // start- and end-indeces that you input. This is making drawing selected pieces a little bit awkward, when choosing which piece to promote to.

        // get and play move
        if(gameState.state == CONTINUE || gameState.state == INVALID_MOVE_ATTEMPTED) {
            Move moveToPlay = getNextMove(&gameState, playerToPlay, input, pieceTextures);
            if(moveToPlay.type != NO_MOVE) {
                gameState.state = progressGame(&gameState, moveToPlay);
                playerToPlay = gameState.board.colorToPlay == PLAYER_WHITE ? gameState.whiteType : gameState.blackType;
                if(playerToPlay == REMOTE_PLAYER) {
                    sendMove(&gameState.udp_ctx, gameState.moveHistory[arrlen(gameState.moveHistory) - 1],
                             gameState.positionHistory[arrlen(gameState.positionHistory) - 2]);
                }
                boardToDraw = arrlen(gameState.positionHistory) - 1;
                // resetting values
                input = initInputHandler();
            }
        }

        // drawing legal squares to move
        if(input.selectedPieceIdx != -1) {
            for(int i = 0; i < arrlen(gameState.legalMoves); ++i) {
                if(gameState.legalMoves[i].startIndex == input.selectedPieceIdx) {
                    DrawRectangle(FILE(gameState.legalMoves[i].destIndex) * SQUARE_SIZE,
                                  RANK(gameState.legalMoves[i].destIndex) * SQUARE_SIZE,
                                  SQUARE_SIZE, SQUARE_SIZE, (Color){ 255, 0, 0, 200 });
                }
            }
        }

        // drawing selected piece and highlighting square that mouse is hovering over
        if(input.selectedPieceIdx != -1) {
            Vector2 mousePos = GetMousePosition();
            mousePos = (Vector2){ mousePos.x - 32, mousePos.y - 32 };
            DrawRectangle(FILE(getMouseSquareIdx()) * SQUARE_SIZE, RANK(getMouseSquareIdx()) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, (Color){ 200, 200, 200, 200 });
            DrawTextureV(pieceTextures[gameState.board.position[input.selectedPieceIdx]], mousePos, RAYWHITE);
        }

        if(gameState.state == INVALID_MOVE_ATTEMPTED) {
            DrawText("Invalid move attempted", 24, 24, 18, WHITE);
            // input = initInputHandler();
        }

        // drawing tree view
        if(gameState.tree) {
            for(int i = 0; i < gameState.tree[0].childCount; ++i) {
                char moveBuf[8];
                DrawText(convertMoveToShortAlgebraicNotation(gameState.positionHistory[arrlen(gameState.positionHistory) - 2], gameState.tree[gameState.tree[0].firstChildIdx + i].move, moveBuf), SQUARE_SIZE * ROW_SIZE + 12, 24 * i, 12, BLACK);

                char valBuf[8];
                sprintf(valBuf, "%4.2f\n", gameState.tree[gameState.tree[0].firstChildIdx + i].value);
                DrawText(valBuf, SQUARE_SIZE * ROW_SIZE + 60, 24 * i, 12, BLACK);
            }
        }
        // show move history
        if(arrlen(gameState.positionHistory) > 1) {
            for(int i = 0; i < arrlen(gameState.moveHistory); ++i) {
                char moveBuf[8];
                DrawText(serialize_move_long_alg(gameState.positionHistory[i], gameState.moveHistory[i], moveBuf),
                         SQUARE_SIZE * ROW_SIZE + 12, 24 * i, 12, BLACK);
            }
        }

        // drawing end state
        if(gameState.state != CONTINUE && gameState.state != INVALID_MOVE_ATTEMPTED) {
            DrawRectangle(0, 0, windowWidth, windowHeight, (Color){ 200, 200, 200, 200});
            switch(gameState.state) {
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
}
