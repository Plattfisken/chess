#ifndef RENDERER_H
#define RENDERER_H
#include "root.h"

// NOTE: the order here is the order that's used to sort components, so the thing that should be drawn on top should be last
typedef enum {
    CT_BOARD,
    CT_LEGAL_SQUARE,
    CT_HIGHLIGHTED_SQUARE,
    CT_PIECE,
    CT_SELECTED_PIECE,
    CT_UI_BACKGROUND,
    CT_UI
} COMPONENT_TYPE;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    Color color;
    char *text;
    int texture_index;
    void *renderer;
} DrawSettings;

typedef struct RenderingComponent {
    COMPONENT_TYPE type;
    DrawSettings settings;
    void (* draw_function)(DrawSettings *);
} RenderingComponent;

typedef struct RenderingContext {
    Texture2D piece_textures[12];
    RenderingComponent *components;
} Renderer;

void      renderer_init(Renderer *renderer);
void      renderer_draw_next_frame(Renderer *renderer);

void      renderer_add_chess_board(Renderer *renderer);
void      renderer_add_pieces(Renderer *renderer, const PIECE_TYPE *pieces, const InputHandler *input);
void      renderer_add_hovered_square_highlighting(Renderer *renderer);
void      renderer_add_legal_squares_highlighting(Renderer *renderer, int piece_to_move_square_index, const Move *legal_moves);

void      renderer_add_interactive_color_selection_menu(Renderer *renderer, GameSettings *settings);
void      renderer_add_interactive_opponent_selection_menu(Renderer *renderer, GameSettings *settings);
int       renderer_add_interactive_ip_text_input_menu(Renderer *renderer, GameSettings *settings);

int       renderer_add_interactive_button
          (Renderer *renderer, int x, int y, int width, int height,
           Color idle_color, Color highligt_color, PIECE_TYPE texture_index, char *text);

MOVE_TYPE renderer_add_interactive_promotion_selection_menu
          (Renderer *renderer, int index_to_draw, PLAYER_COLOR color_to_draw);
#endif //RENDERER_H
