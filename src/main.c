#include "root.h"

#define TREE_VIEW_WIDTH ((SQUARE_SIZE) * (4))

int draw_button(int x_pos, int y_pos, int width, int height, Color idle_color, Color highligt_color, const char *text) {
    Vector2 mousePos = GetMousePosition();
    int mouse_on_button = mousePos.x >= x_pos         &&
                          mousePos.x <= x_pos + width &&
                          mousePos.y >= y_pos         &&
                          mousePos.y <= y_pos + height;
    if(!mouse_on_button)
        DrawRectangle(x_pos, y_pos, width, height, idle_color);
    else {
        DrawRectangle(x_pos, y_pos, width, height, highligt_color);
    }
    if(text) {
        DrawText(text, x_pos, y_pos, height, BLACK);
    }
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_on_button) {
        return 1;
    }
    return 0;
}

void draw_chess_board() {
    for(int i = 0; i < BOARD_SIZE; ++i) {
        Color color = (RANK(i) + FILE(i)) % 2 ? DARKGREEN : BEIGE;
        DrawRectangle(FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
    }
}

void draw_start_menu(GameSettings *settings, int width, int height, Texture2D white_pawn, Texture2D black_pawn) {
    // choosing color to play
    PLAYER_TYPE *opponent_type = NULL;
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        draw_chess_board();
        DrawRectangle(0, 0, width, height, (Color){ 200, 200, 200, 200});

        char *str = "Choose color to play";
        int text_width = MeasureText(str, 24);
        DrawText(str, width/2 - text_width/2, height/4, 24, BLACK);

        int x_offset = width/8;
        int white_pawn_x_pos = width/2 - white_pawn.width/2 - x_offset;
        int black_pawn_x_pos = width/2 - black_pawn.width/2 + x_offset;
        int pawns_y_pos = height/3;

        if(draw_button(white_pawn_x_pos, pawns_y_pos, white_pawn.width, white_pawn.height,
                    (Color){ 200, 200, 200, 0 }, (Color){ 150, 150, 150, 200}, NULL)) {
            settings->white_type = LOCAL_PLAYER;
            opponent_type = &settings->black_type;

            EndDrawing();
            break;
        }
        DrawTexture(white_pawn, white_pawn_x_pos, pawns_y_pos, WHITE);

        if(draw_button(black_pawn_x_pos, pawns_y_pos, black_pawn.width, black_pawn.height,
                    (Color){ 200, 200, 200, 0 }, (Color){ 150, 150, 150, 200}, NULL)) {
            settings->black_type = LOCAL_PLAYER;
            opponent_type = &settings->white_type;

            EndDrawing();
            break;
        }
        DrawTexture(black_pawn, black_pawn_x_pos, pawns_y_pos, WHITE);

        EndDrawing();
    }
    // choosing opponent type
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        draw_chess_board();
        DrawRectangle(0, 0, width, height, (Color){ 200, 200, 200, 200});

        int gap = 12;
        {
            char *str = "Choose your opponent";
            int text_width = MeasureText(str, 24);
            DrawText(str, width/2 - text_width/2, height/4, 24, BLACK);
        }

        int btn_count = 0;
        int btn_height = 24;
        {
            char *str = "Local player";
            int text_width = MeasureText(str, btn_height);
            ++btn_count;
            if(draw_button(width/2 - text_width/2, height/4 + btn_height * btn_count + gap * btn_count, text_width,
                        btn_height, (Color){ 200, 200, 200, 200 }, (Color){ 150, 150, 150, 200 }, str)) {
                *opponent_type = LOCAL_PLAYER;

                EndDrawing();
                break;
            }
        }
        {
            char *str = "Remote player";
            int text_width = MeasureText(str, btn_height);
            ++btn_count;
            if(draw_button(width/2 - text_width/2, height/4 + btn_height * btn_count + gap * btn_count, text_width,
                        btn_height, (Color){ 200, 200, 200, 200 }, (Color){ 150, 150, 150, 200 }, str)) {
                *opponent_type = REMOTE_PLAYER;

                EndDrawing();
                break;
            }
        }
        {
            char *str = "Bot";
            int text_width = MeasureText(str, btn_height);
            ++btn_count;
            if(draw_button(width/2 - text_width/2, height/4 + btn_height * btn_count + gap * btn_count, text_width,
                        btn_height, (Color){ 200, 200, 200, 200 }, (Color){ 150, 150, 150, 200 }, str)){
                *opponent_type = BOT;
                EndDrawing();
                break;
            }
        }
        EndDrawing();
    }
    if(*opponent_type != REMOTE_PLAYER)
        return;

    #define buf_len 16
    char ip_buf[buf_len];
    memset(ip_buf, 0, sizeof ip_buf);
    int pos = 0;
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        draw_chess_board();
        DrawRectangle(0, 0, width, height, (Color){ 200, 200, 200, 200});

        if(IsKeyPressed(KEY_ENTER)) {
            strcpy(settings->ip_address, ip_buf);
            EndDrawing();
            break;
        }
        else if(IsKeyPressed(KEY_BACKSPACE)) {
            if(pos > 0)
                ip_buf[--pos] = 0;
        }

        // leave space for nul-terminator
        else if(pos < buf_len - 1){
            int character = GetCharPressed();
            if(character) {
                ip_buf[pos++] = character;
            }
        }
        int gap = 12;
        {
            char *str = "Enter IP-Address";
            int text_width = MeasureText(str, 24);
            DrawText(str, width/2 - text_width/2, height/4, 24, BLACK);
        }
        {
            int text_width = MeasureText(ip_buf, 24);
            DrawText(ip_buf, width/2 - text_width/2, height/4 + 24 + gap, 24, BLACK);
        }
        EndDrawing();
    }
}

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

    ChessGameState gameState = {0};
    GameSettings settings = {0};
    draw_start_menu(&settings, windowWidth, windowHeight, pieceTextures[WHITE_PAWN], pieceTextures[BLACK_PAWN]);
    initGame(&gameState, &settings);

    InputHandler input = initInputHandler();
    int boardToDraw = 0;
    PLAYER_TYPE playerToPlay = gameState.board.colorToPlay == PLAYER_WHITE ? gameState.white_type : gameState.black_type;
    while(!WindowShouldClose()) {
        if(boardToDraw == arrlen(gameState.position_history) - 1 && playerToPlay == LOCAL_PLAYER)
            input = processInput(input);

        // Move this into process input
        if(IsKeyPressed(KEY_R)) {
            deleteGameResources(&gameState);
            draw_start_menu(&settings, windowWidth, windowHeight, pieceTextures[WHITE_PAWN], pieceTextures[BLACK_PAWN]);
            initGame(&gameState, &settings);
        }
        if(IsKeyPressed(KEY_LEFT)) {
            if(boardToDraw > 0)
                boardToDraw--;
        }
        if(IsKeyPressed(KEY_RIGHT)) {
            if(boardToDraw < arrlen(gameState.position_history) - 1)
                boardToDraw++;
        }

        // update and render
        BeginDrawing();
        ClearBackground(WHITE);

        draw_chess_board();

        // drawing pieces
        for(int i = 0; i < BOARD_SIZE; ++i) {
            if(gameState.position_history[boardToDraw].position[i] != EMPTY) {
                PIECE_TYPE pieceType = gameState.position_history[boardToDraw].position[i];
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
                playerToPlay = gameState.board.colorToPlay == PLAYER_WHITE ? gameState.white_type : gameState.black_type;
                if(playerToPlay == REMOTE_PLAYER) {
                    sendMove(&gameState.udp_ctx, gameState.move_history[arrlen(gameState.move_history) - 1],
                             gameState.position_history[arrlen(gameState.position_history) - 2]);
                }
                boardToDraw = arrlen(gameState.position_history) - 1;
                // resetting values
                input = initInputHandler();
            }
        }

        // drawing legal squares to move
        if(input.selectedPieceIdx != -1) {
            for(int i = 0; i < arrlen(gameState.legal_moves); ++i) {
                if(gameState.legal_moves[i].startIndex == input.selectedPieceIdx) {
                    DrawRectangle(FILE(gameState.legal_moves[i].destIndex) * SQUARE_SIZE,
                                  RANK(gameState.legal_moves[i].destIndex) * SQUARE_SIZE,
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
                DrawText(convertMoveToShortAlgebraicNotation(gameState.position_history[arrlen(gameState.position_history) - 2], gameState.tree[gameState.tree[0].firstChildIdx + i].move, moveBuf), SQUARE_SIZE * ROW_SIZE + 12, 24 * i, 12, BLACK);

                char valBuf[8];
                sprintf(valBuf, "%4.2f\n", gameState.tree[gameState.tree[0].firstChildIdx + i].value);
                DrawText(valBuf, SQUARE_SIZE * ROW_SIZE + 60, 24 * i, 12, BLACK);
            }
        }
        // show move history
        if(arrlen(gameState.position_history) > 1) {
            for(int i = 0; i < arrlen(gameState.move_history); ++i) {
                char moveBuf[8];
                DrawText(serialize_move_long_alg(gameState.position_history[i], gameState.move_history[i], moveBuf),
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
