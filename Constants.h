#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Move.h"

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char BYTE;

#define BOARD_SIZE 64
#define MAX_TURNS 1024
#define MAX_MOVES 500
#define MAX_DEPTH 30

extern u64 GAME_STATE_STACK[MAX_DEPTH + MAX_TURNS][15];
extern int GAME_STATE_STACK_POINTER;

extern int MOVE_STACK_POINTER;
extern Move MOVE_STACK[MAX_DEPTH][MAX_MOVES];

#endif