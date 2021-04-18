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
#define DEPTH 8

#define PAWN_SCORE 100
#define KNIGHT_SCORE 300
#define BISHOP_SCORE 350
#define ROOK_SCORE 500
#define QUEEN_SCORE 900
#define ALPHA_BETA_MIN -1000000000
#define ALPHA_BETA_MAX 1000000000

extern u64 GAME_STATE_STACK[MAX_DEPTH + MAX_TURNS][15];
extern int GAME_STATE_STACK_POINTER;

extern int MOVE_STACK_POINTER;
extern Move MOVE_STACK[MAX_DEPTH][MAX_MOVES];

// precalculated bit shifts, for example singleBitLookUp[15] == 1LLU << 15
extern u64 SINGLE_BIT_LOOKUP[64];
// pattern used to calculate north-east diagonal moves with bitboards
extern u64 NORTH_EAST_LOOKUP_PATTERN[64];
extern u64 NORTH_WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_EAST_LOOKUP_PATTERN[64];
extern u64 SOUTH_WEST_LOOKUP_PATTERN[64];

void setLookUpTables();

#endif