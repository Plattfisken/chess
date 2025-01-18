#pragma once
#include "root.h"

void init_input_handler(InputHandler *input_handler) {
    *input_handler = (InputHandler){
        .selected_square_index = -1,
        .dropped_at_square_index = -1,
        .attemptedMove = (Move){-1, -1, NO_MOVE}
    };
}

void process_input(InputHandler *input_handler) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        input_handler->selected_square_index = get_mouse_square_index();
    }

    if(input_handler->selected_square_index != -1)
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            input_handler->dropped_at_square_index = get_mouse_square_index();

            input_handler->attemptedMove.startIndex = input_handler->selected_square_index;
            input_handler->attemptedMove.destIndex = input_handler->dropped_at_square_index;

            input_handler->selected_square_index = -1;
            input_handler->dropped_at_square_index = -1;
        }
}

int get_mouse_square_index() {
    Vector2 mousePos = GetMousePosition();
    if(mousePos.x >= 0 && mousePos.x <= SQUARE_SIZE * ROW_SIZE &&
       mousePos.y >= 0 && mousePos.y <= SQUARE_SIZE * COL_SIZE)
        return IDX_2D((int)(mousePos.x / SQUARE_SIZE), (int)(mousePos.y / SQUARE_SIZE));
    return -1;
}
