#pragma once
#include "root.h"

InputHandler initInputHandler() {
    return (InputHandler){
        .selectedPieceIdx = -1,
        .droppedPieceIdx = -1,
        .attemptedMove = (Move){-1, -1, NO_MOVE}
    };
}

InputHandler processInput(InputHandler input) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        input.selectedPieceIdx = getMouseSquareIdx();
    }

    if(input.selectedPieceIdx != -1)
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            input.droppedPieceIdx = getMouseSquareIdx();

            input.attemptedMove.startIndex = input.selectedPieceIdx;
            input.attemptedMove.destIndex = input.droppedPieceIdx;

            input.selectedPieceIdx = -1;
            input.droppedPieceIdx = -1;
        }
    return input;
}

int getMouseSquareIdx() {
    Vector2 mousePos = GetMousePosition();
    return IDX_2D((int)(mousePos.x / SQUARE_SIZE), (int)(mousePos.y / SQUARE_SIZE));
}
