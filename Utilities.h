#ifndef UTILITIES_H
#define UTILITIES_H

#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, int checkmate);
bool squareOccupied(u64 board, int index);
bool kingInDanger(u64 *gameState, int side);
u64 emptySquare(u64 board, int index);
u64 fillSquare(u64 board, int index);

#endif