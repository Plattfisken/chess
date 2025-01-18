#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H
#include "root.h"

typedef struct {
    int selected_square_index; // actual index of position of the selected piece
    int dropped_at_square_index; // index where selected piece was droppedmain
    Move attemptedMove;
} InputHandler;

void init_input_handler(InputHandler *Input_handler);
void process_input(InputHandler *input_handler);
int get_mouse_square_index();
#endif //INPUT_HANDLING_H
