#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H
#include "root.h"

typedef struct {
    int selectedPieceIdx; // actual index of position of the selected piece
    int droppedPieceIdx; // index where selected piece was dropped
    Move attemptedMove;
} InputHandler;

InputHandler createInputHandler();
InputHandler processInput(InputHandler input);
int getMouseSquareIdx();
#endif //INPUT_HANDLING_H
