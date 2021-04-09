#include <stdbool.h>
#include <stdio.h>

#include "Move.h"
#include "MakeMove.h"
#include "Constants.h"
#include "MoveGeneration.h"
#include "Utilities.h"

Move checkThreatMoves[MAX_MOVES];
Move checkMateMoves[MAX_MOVES];

// checks if king can be taken
bool checkIfCheckThreat(int side) {
    int i;
    int moveCount;
    Move generatedMove;
    u64 *gameState;
    u64 whiteKings;
    u64 blackKings;

    gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    whiteKings = gameState[6];
    blackKings = gameState[13];
    // generate moves for opponent side and check if any could take your king in the square you are moving (t0)
    moveCount = generateMoves(checkThreatMoves, -side);
    for (i = 0; i < moveCount; i++) {
        generatedMove = checkThreatMoves[i];

        if (side == 1 && squareOccupied(whiteKings, generatedMove.to)) {
            // printf("attack from: %d, and king in: %d\n", generatedMove.from, generatedMove.to);
            return true;
        }
        else if (side == -1 && squareOccupied(blackKings, generatedMove.to)) {
            // printf("attack from: %d, and king in: %d\n", generatedMove.from, generatedMove.to);
            return true;
        }
    }

    return false;
}

// filters out pseudo-legal moves, basically notices checkmates and draws by no moves
bool checkIfNoLegalMoves(int side) {
    int moveCount;
    int i;

    moveCount = generateMoves(checkMateMoves, side);

    // no possible pseudo-legal moves -> no legal moves either
    if (moveCount == 0) return true;

    bool noMoves = true;
    for (i = 0; i < moveCount; i++) {
        makeMove(checkMateMoves[i]);
        GAME_STATE_STACK_POINTER++;
        if (!checkIfCheckThreat(side)) {
            noMoves = false;
            GAME_STATE_STACK_POINTER--;
            break;
        }
        GAME_STATE_STACK_POINTER--;
    }

    return noMoves;
}