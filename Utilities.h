#ifndef UTILITIES_H
#define UTILITIES_H

#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, bool enPassant);
bool squareOccupied(u64 board, int index);
u64 emptySquare(u64 board, int index);
u64 fillSquare(u64 board, int index);

bool canWhiteCastleShort(u64 meta);
bool canWhiteCastleLong(u64 meta);
bool canBlackCastleShort(u64 meta);
bool canBlackCastleLong(u64 meta);
u64 removeWhiteCastleShort(u64 meta);
u64 removeWhiteCastleLong(u64 meta);
u64 removeBlackCastleShort(u64 meta);
u64 removeBlackCastleLong(u64 meta);

bool isEnPassantAllowed(u64 meta);
u64 setEnPassantAllowed(u64 meta, bool enPassantAllowed);
u64 setEnPassantSquare(u64 meta, u64 square);
u64 getEnPassantSquare(u64 meta);

int getSideToPlay(u64 meta);
u64 setSideToPlay(u64 meta, int side);

u64 *parseFen(u64 *gameState, char *fenStr);
int bitScanForward(u64 board);

#endif