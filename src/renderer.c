#pragma once
#include "root.h"

void _load_textures(Renderer *context) {
    context->piece_textures[WHITE_PAWN] = LoadTexture("../assets/WHITE_PAWN.png");
    context->piece_textures[WHITE_KNIGHT] = LoadTexture("../assets/WHITE_KNIGHT.png");
    context->piece_textures[WHITE_BISHOP] = LoadTexture("../assets/WHITE_BISHOP.png");
    context->piece_textures[WHITE_ROOK] = LoadTexture("../assets/WHITE_ROOK.png");
    context->piece_textures[WHITE_QUEEN] = LoadTexture("../assets/WHITE_QUEEN.png");
    context->piece_textures[WHITE_KING] = LoadTexture("../assets/WHITE_KING.png");

    context->piece_textures[BLACK_PAWN] = LoadTexture("../assets/BLACK_PAWN.png");
    context->piece_textures[BLACK_KNIGHT] = LoadTexture("../assets/BLACK_KNIGHT.png");
    context->piece_textures[BLACK_BISHOP] = LoadTexture("../assets/BLACK_BISHOP.png");
    context->piece_textures[BLACK_ROOK] = LoadTexture("../assets/BLACK_ROOK.png");
    context->piece_textures[BLACK_QUEEN] = LoadTexture("../assets/BLACK_QUEEN.png");
    context->piece_textures[BLACK_KING] = LoadTexture("../assets/BLACK_KING.png");

    for(int i = 0; i < ARRAY_LENGTH(context->piece_textures); ++i) {
        if(!IsTextureValid(context->piece_textures[i])) {
            printf("Failed to load texture at index: %d\n", i);
        }
    }
}

void _sort_components(RenderingComponent *components) {
    // lol bubble sort
    for(int i = 0; i < arrlen(components); ++i) {
        for(int u = i + 1; u < arrlen(components); ++u) {
            if(components[i].type > components[u].type) {
                RenderingComponent tmp = components[i];
                components[i] = components[u];
                components[u] = tmp;
            }
        }
    }
}

void _draw_square(DrawSettings *settings) {
    DrawRectangle(settings->x, settings->y, settings->width, settings->height, settings->color);
}

void _draw_chess_board() {
    for(int i = 0; i < BOARD_SIZE; ++i) {
        Color color = (RANK(i) + FILE(i)) % 2 ? DARKGREEN : BEIGE;
        DrawRectangle(FILE(i) * SQUARE_SIZE, RANK(i) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
    }
}

void _draw_piece(DrawSettings *settings) {
    DrawTexture(((Renderer *)settings->renderer)->piece_textures[settings->texture_index], settings->x, settings->y, settings->color);
}

void _draw_button(DrawSettings *settings) {
    DrawRectangle(settings->x, settings->y, settings->width, settings->height, settings->color);
    if(settings->texture_index > -1)
        DrawTexture(((Renderer *)settings->renderer)->piece_textures[settings->texture_index], settings->x, settings->y, WHITE);
    if(settings->text)
        DrawText(settings->text, settings->x, settings->y, settings->height, BLACK);
}

void _draw_menu_background_with_text(DrawSettings *settings) {
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){ 200, 200, 200, 200});
    int font_size = 24;
    int text_width = MeasureText(settings->text, font_size);
    DrawText(settings->text, WINDOW_WIDTH/2 - text_width/2, WINDOW_HEIGHT/4, font_size, BLACK);
}

void _draw_centered_text(DrawSettings *settings) {
    int text_width = MeasureText(settings->text, settings->height);
    DrawText(settings->text, settings->x - text_width/2, settings->y, settings->height, BLACK);
}

void renderer_init(Renderer *renderer) {
    renderer->components = NULL;
    _load_textures(renderer);
}

void renderer_draw_next_frame(Renderer *renderer) {
    _sort_components(renderer->components);
    BeginDrawing();
    ClearBackground(WHITE);
    for(int i = 0; i < arrlen(renderer->components); ++i) {
        renderer->components[i].draw_function(&renderer->components[i].settings);
    }
    DrawFPS(20, 20);
    EndDrawing();
    arrdeln(renderer->components, 0, arrlen(renderer->components));
}

void renderer_add_chess_board(Renderer *renderer) {
    RenderingComponent board = {
        .type = CT_BOARD,
        .draw_function = _draw_chess_board
    };
    arrpush(renderer->components, board);
}

// TODO: Fix so the pawn that's about to promote isn't visible while selection menu is
// maybe look in the components in the renderer for the promotion menu, it should always be added before the pieces, i believe.
void renderer_add_pieces(Renderer *renderer, const PIECE_TYPE *pieces, const InputHandler *input) {
    for(int i = 0; i < BOARD_SIZE; ++i) {
        if(pieces[i] != EMPTY) {
            if(i != input->selected_square_index) {
                RenderingComponent piece = {
                    .type = CT_PIECE,
                    .settings = {
                        .x = FILE(i) * SQUARE_SIZE,
                        .y = RANK(i) * SQUARE_SIZE,
                        .color = WHITE,
                        .texture_index = pieces[i],
                        .renderer = renderer
                    },
                    .draw_function = _draw_piece
                };
                arrpush(renderer->components, piece);
            }
        }
    }
    if(input->selected_square_index != -1 && pieces[input->selected_square_index] != EMPTY) {
        RenderingComponent selected_piece = {
            .type = CT_SELECTED_PIECE,
            .settings = {
                .x = GetMouseX() - (renderer->piece_textures[pieces[input->selected_square_index]].width/2),
                .y = GetMouseY() - (renderer->piece_textures[pieces[input->selected_square_index]].height/2),
                .texture_index = pieces[input->selected_square_index],
                .color = WHITE,
                .renderer = renderer
            },
            .draw_function = _draw_piece
        };
        arrpush(renderer->components, selected_piece);
    }
}

int renderer_add_interactive_button(Renderer *renderer, int x, int y, int width, int height,
                                    Color idle_color, Color highligt_color, PIECE_TYPE texture_index, char *text) {
    Vector2 mousePos = GetMousePosition();
    int mouse_on_button = mousePos.x >= x         &&
                          mousePos.x <= x + width &&
                          mousePos.y >= y         &&
                          mousePos.y <= y + height;
    RenderingComponent interactive_button = {
        .type = CT_UI,
        .settings = {
            .x = x,
            .y = y,
            .height = height,
            .width = width,
            .texture_index = texture_index,
            .text = text,
            .renderer = renderer
        },
        .draw_function = _draw_button
    };

    if(mouse_on_button)
        interactive_button.settings.color = highligt_color;
    else
        interactive_button.settings.color = idle_color;
    arrpush(renderer->components, interactive_button);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_on_button) {
        return 1;
    }
    return 0;
}

MOVE_TYPE renderer_add_interactive_promotion_selection_menu(Renderer *renderer, int index_to_draw, PLAYER_COLOR color_to_draw) {
    Color idle_color = { 200, 200, 200, 200 };
    Color highligt_color = { 255, 255, 255, 200 };
    int iota = 0;
    int sign = color_to_draw == PLAYER_WHITE ? 1 : -1;
    if(renderer_add_interactive_button(renderer, FILE(index_to_draw) * SQUARE_SIZE,
                                       (RANK(index_to_draw) + iota++ * sign) * SQUARE_SIZE,
                                       SQUARE_SIZE, SQUARE_SIZE, idle_color, highligt_color,
                                       KNIGHT(color_to_draw), NULL))
        return PROMOTION_TO_KNIGHT;
    if(renderer_add_interactive_button(renderer, FILE(index_to_draw) * SQUARE_SIZE,
                                       (RANK(index_to_draw) + iota++ * sign) * SQUARE_SIZE,
                                       SQUARE_SIZE, SQUARE_SIZE, idle_color, highligt_color,
                                       BISHOP(color_to_draw), NULL))
        return PROMOTION_TO_BISHOP;
    if(renderer_add_interactive_button(renderer, FILE(index_to_draw) * SQUARE_SIZE,
                                       (RANK(index_to_draw) + iota++ * sign) * SQUARE_SIZE,
                                       SQUARE_SIZE, SQUARE_SIZE, idle_color, highligt_color,
                                       ROOK(color_to_draw), NULL))
        return PROMOTION_TO_ROOK;
    if(renderer_add_interactive_button(renderer, FILE(index_to_draw) * SQUARE_SIZE,
                                       (RANK(index_to_draw) + iota++ * sign) * SQUARE_SIZE,
                                       SQUARE_SIZE, SQUARE_SIZE, idle_color, highligt_color,
                                       QUEEN(color_to_draw), NULL))
        return PROMOTION_TO_QUEEN;
    return NO_MOVE;
}

void renderer_add_hovered_square_highlighting(Renderer *renderer) {
    int hovered_square_index = get_mouse_square_index();
    RenderingComponent highligted_square = {
        .type = CT_HIGHLIGHTED_SQUARE,
        .settings = {
            .x = FILE(hovered_square_index) * SQUARE_SIZE,
            .y = RANK(hovered_square_index) * SQUARE_SIZE,
            .width = SQUARE_SIZE,
            .height = SQUARE_SIZE,
            .color = (Color){ 200, 200, 200, 200 }
        },
        .draw_function = _draw_square
    };
    arrpush(renderer->components, highligted_square);
}

void renderer_add_legal_squares_highlighting(Renderer *renderer, int piece_to_move_square_index, const Move *legal_moves) {
    for(int i = 0; i < arrlen(legal_moves); ++i) {
        if(legal_moves[i].startIndex == piece_to_move_square_index) {
            RenderingComponent legal_square = {
                .type = CT_LEGAL_SQUARE,
                .settings = {
                    .x = FILE(legal_moves[i].destIndex) * SQUARE_SIZE,
                    .y = RANK(legal_moves[i].destIndex) * SQUARE_SIZE,
                    .width = SQUARE_SIZE,
                    .height = SQUARE_SIZE,
                    .color = RED
                },
                .draw_function = _draw_square
            };
            arrpush(renderer->components, legal_square);
        }
    }
}

void renderer_add_interactive_color_selection_menu(Renderer *renderer, GameSettings *settings) {
    RenderingComponent menu_background = {
        .type = CT_UI,
        .settings = {
            .text = "Choose your color"
        },
        .draw_function = _draw_menu_background_with_text
    };
    arrpush(renderer->components, menu_background);
    int button_offset = WINDOW_WIDTH/8;
    int button_width = renderer->piece_textures[WHITE_PAWN].width;
    int button_height = button_width;
    if(renderer_add_interactive_button(renderer, WINDOW_WIDTH/2 - button_width/2 - button_offset,
                                    WINDOW_HEIGHT * 2/5, button_width, button_height,
                                    (Color){ .a = 0 }, (Color){ 100, 100, 100, 200 }, WHITE_PAWN, NULL)) {
        settings->white_type = LOCAL_PLAYER;
    }
    if(renderer_add_interactive_button(renderer, WINDOW_WIDTH/2 - button_width/2 + button_offset,
                                    WINDOW_HEIGHT * 2/5, button_width, button_height,
                                    (Color){ .a = 0 }, (Color){ 100, 100, 100, 200 }, BLACK_PAWN, NULL)) {
        settings->black_type = LOCAL_PLAYER;
    }
}

void renderer_add_interactive_opponent_selection_menu(Renderer *renderer, GameSettings *settings) {
    RenderingComponent menu_background = {
        .type = CT_UI,
        .settings = {
            .text = "Choose your opponent"
        },
        .draw_function = _draw_menu_background_with_text
    };
    arrpush(renderer->components, menu_background);

    PLAYER_TYPE *opponent = settings->white_type == NOT_SET ? &settings->white_type : &settings->black_type;
    char *strings[] = { "Local player", "Remote player", "Bot" };
    for(int i = 0; i < ARRAY_LENGTH(strings); ++i) {
        int font_size = 24;
        int button_gap = 12;
        int text_width = MeasureText(strings[i], font_size);
        if(renderer_add_interactive_button(renderer, WINDOW_WIDTH/2 - text_width/2, WINDOW_HEIGHT/3 + (font_size + button_gap) * i,
                                           text_width, font_size, (Color){ .a = 0 }, (Color){ 100, 100, 100, 200 }, -1, strings[i])) {
            if(strcmp("Local player", strings[i]) == 0) {
                *opponent = LOCAL_PLAYER;
            }
            else if(strcmp("Remote player", strings[i]) == 0) {
                *opponent = REMOTE_PLAYER;
            }
            else if(strcmp("Bot", strings[i]) == 0) {
                *opponent = BOT;
            }
        }
    }
}

void renderer_add_interactive_text_input_box(Renderer *renderer, char *buf, int x, int y, int font_size) {
    RenderingComponent input_box = {
        .type = CT_UI,
        .settings = {
            .x = x,
            .y = y,
            .height = font_size,
            .text = buf
        },
        .draw_function = _draw_centered_text
    };
    arrpush(renderer->components, input_box);
}

int renderer_add_interactive_ip_text_input_menu(Renderer *renderer, GameSettings *settings) {
    RenderingComponent menu_background = {
        .type = CT_UI,
        .settings = {
            .text = "Enter IP-Address"
        },
        .draw_function = _draw_menu_background_with_text
    };
    arrpush(renderer->components, menu_background);
    static int pos = 0;

    if(IsKeyPressed(KEY_ENTER))
        return 1;

    if(IsKeyPressed(KEY_BACKSPACE)){
        if(pos > 0)
            settings->ip_address[--pos] = 0;
    }
    else {
        char character = GetKeyPressed();
        if(character && pos < ARRAY_LENGTH(settings->ip_address) - 1) {
            settings->ip_address[pos++] = character;
        }
    }

    int font_size = 24;
    renderer_add_interactive_text_input_box(renderer, settings->ip_address, WINDOW_WIDTH/2, WINDOW_HEIGHT/3, font_size);
    return 0;
}
