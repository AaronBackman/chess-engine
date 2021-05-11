#ifndef SEARCH_H
#define SEARCH_H

#include "Move.h"
// #include "Board.h"

extern Move g_selectedMove;
extern bool foundMove;
extern bool checkmate;

void zero_history();

// int see(int from, int to, Board *board, int side, int targetPiece, int attackPiece);

void *search(void *vargp);

#endif