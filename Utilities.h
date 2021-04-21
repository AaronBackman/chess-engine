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

u64 setWhiteCastleShort(u64 meta);
u64 setWhiteCastleLong(u64 meta);
u64 setBlackCastleShort(u64 meta);
u64 setBlackCastleLong(u64 meta);

bool isEnPassantAllowed(u64 meta);
u64 setEnPassantAllowed(u64 meta, bool enPassantAllowed);
u64 setEnPassantSquare(u64 meta, u64 square);
u64 getEnPassantSquare(u64 meta);

int getSideToPlay(u64 meta);
u64 setSideToPlay(u64 meta, int side);

void moveToString(char *str, Move move);
void parseUciPosition(u64 *gameState, char *fenStr);
int bitScanForward(u64 board);
int bitScanReverse(u64 board);

void printBoard();

#endif