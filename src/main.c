#include "root.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess");
    if(!IsWindowReady()) {
        printf("Failed to open window\n");
        return 1;
    }
    SetTargetFPS(60);

    Renderer renderer = {0};
    renderer_init(&renderer);

    InputHandler input_handler = {0};
    init_input_handler(&input_handler);

    ChessGameState game_state = {0};
    game_state.state = MENU;
    GameSettings settings = {0};

    int boardToDraw = 0;
    while(!WindowShouldClose()) {
        renderer_add_chess_board(&renderer);

        // TODO: Maybe move this into process input
        // TODO: there's a bug here causing a crash if you try to restart and then pick remote player
        // reset game
        if(IsKeyPressed(KEY_R)) {
            deleteGameResources(&game_state);
            memset(&settings, 0, sizeof settings);
            game_state.state = MENU;
        }
        switch(game_state.state) {
            case MENU: {
                if(settings.black_type == NOT_SET && settings.white_type == NOT_SET) {
                    renderer_add_interactive_color_selection_menu(&renderer, &settings);
                }
                else if(settings.white_type == NOT_SET || settings.black_type == NOT_SET) {
                    renderer_add_interactive_opponent_selection_menu(&renderer, &settings);
                }
                else if(settings.white_type != REMOTE_PLAYER && settings.black_type != REMOTE_PLAYER) {
                    init_game(&game_state, &settings);
                }
                else if(renderer_add_interactive_ip_text_input_menu(&renderer, &settings)) {
                    init_game(&game_state, &settings);
                }
            } break;

            case CONTINUE:
            case INVALID_MOVE_ATTEMPTED: {
                PLAYER_TYPE playerToPlay = game_state.board.colorToPlay == PLAYER_WHITE ? game_state.white_type : game_state.black_type;
                if(boardToDraw == arrlen(game_state.position_history) - 1 && playerToPlay == LOCAL_PLAYER)
                    process_input(&input_handler);

                if(IsKeyPressed(KEY_LEFT)) {
                    if(boardToDraw > 0)
                        boardToDraw--;
                }
                if(IsKeyPressed(KEY_RIGHT)) {
                    if(boardToDraw < arrlen(game_state.position_history) - 1)
                        boardToDraw++;
                }

                // NOTE: the state INVALID_MOVE_ATTEMPTED is returned if progressGame is called with a moveToPlay that is not found in legalMoves,
                // but since getNextMove already matches the move from input with a move from legal moves then currently INVALID_MOVE_ATTEMPTED should
                // never occur from local input. I believe originally i intended whatever move that was inputted to be passed to progressGame, but it
                // became a bit difficult with Move.type, since you don't really input the type normally, the type can be inferred from the
                // start- and end-indeces that you input. This is making drawing selected pieces a little bit awkward, when choosing which piece to promote to.

                Move moveToPlay = get_next_move(&game_state, playerToPlay, input_handler, &renderer);
                if(moveToPlay.type != NO_MOVE) {
                    game_state.state = progressGame(&game_state, moveToPlay);
                    playerToPlay = game_state.board.colorToPlay == PLAYER_WHITE ? game_state.white_type : game_state.black_type;
                    if(playerToPlay == REMOTE_PLAYER) {
                        sendMove(&game_state.udp_ctx, game_state.move_history[arrlen(game_state.move_history) - 1],
                                 game_state.position_history[arrlen(game_state.position_history) - 2]);
                    }
                    boardToDraw = arrlen(game_state.position_history) - 1;
                    // resetting values
                    // TODO: this will probably need to be done differently
                    init_input_handler(&input_handler);
                }

                renderer_add_pieces(&renderer, game_state.position_history[boardToDraw].position, &input_handler);

                if(input_handler.selected_square_index != -1 && game_state.board.position[input_handler.selected_square_index] != EMPTY) {
                    renderer_add_hovered_square_highlighting(&renderer);
                    renderer_add_legal_squares_highlighting(&renderer, input_handler.selected_square_index, game_state.legal_moves);
                }
            } break;

            case WHITE_WIN:
            case BLACK_WIN:
            case DRAW: {

            } break;
        }

        renderer_draw_next_frame(&renderer);

        // drawing tree view
        // if(game_state.tree) {
        //     char tree_count_buf[32];
        //     sprintf(tree_count_buf, "Tree nodes: %lu\n", arrlen(game_state.tree));
        //     DrawText(tree_count_buf, SQUARE_SIZE * ROW_SIZE + 12, 0, 12, BLACK);
        //     for(int i = 0; i < game_state.tree[0].childCount; ++i) {
        //         char moveBuf[8];
        //         convertMoveToShortAlgebraicNotation(game_state.position_history[arrlen(game_state.position_history) - 2],
        //                                             game_state.tree[game_state.tree[0].firstChildIdx + i].move, moveBuf);
        //         DrawText(moveBuf, SQUARE_SIZE * ROW_SIZE + 12, 24 * (i + 1), 12, BLACK);
        //
        //         char valBuf[8];
        //         sprintf(valBuf, "%4.2f\n", game_state.tree[game_state.tree[0].firstChildIdx + i].value);
        //         DrawText(valBuf, SQUARE_SIZE * ROW_SIZE + 60, 24 * (i + 1), 12, BLACK);
        //     }
        // }
        // show move history
        // if(arrlen(gameState.position_history) > 1) {
        //     for(int i = 0; i < arrlen(gameState.move_history); ++i) {
        //         char moveBuf[8];
        //         DrawText(serialize_move_long_alg(gameState.position_history[i], gameState.move_history[i], moveBuf),
        //                  SQUARE_SIZE * ROW_SIZE + 12, 24 * i, 12, BLACK);
        //     }
        // }

        // drawing end state
        // if(game_state.state != CONTINUE && game_state.state != INVALID_MOVE_ATTEMPTED) {
        //     DrawRectangle(0, 0, window_width, window_height, (Color){ 200, 200, 200, 200});
        //     switch(game_state.state) {
        //         case DRAW:
        //             DrawText("Draw... \nNobody wins.", window_width / 2 - 50, window_height / 2 - 50, 24, BLACK);
        //             break;
        //         case WHITE_WIN:
        //             DrawText("Checkmate! \nWhite wins.", window_width / 2 - 50, window_height / 2 - 50, 24, BLACK);
        //             break;
        //         case BLACK_WIN:
        //             DrawText("Checkmate! \nBlack wins.", window_width / 2 - 50, window_height / 2 - 50, 24, BLACK);
        //             break;
        //         default:
        //             break;
        //     }
        // }
        // EndDrawing();
    }
}
