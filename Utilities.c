#include <stdbool.h>

#include "Constants.h"
#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, int checkmate) {
    Move move = {from, to, promotion, castle, checkmate};

    return move;
}

bool squareOccupied(u64 board, int index) {
    u64 bitIndex = (1ULL << index);

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

bool kingInDanger(u64 *gameState, int side) {
    // king not in danger
    return false;
}

u64 emptySquare(u64 board, int index) {
    u64 bitIndex = (1LLU << index);

    board &= ~bitIndex;

    return board;
}

u64 fillSquare(u64 board, int index) {
    u64 bitIndex = (1LLU << index);

    board |= bitIndex;

    return board;
}