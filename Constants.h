#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Move.h"
#include "Board.h"

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char BYTE;

#define BOARD_SIZE 64
#define MAX_TURNS 1024
#define MAX_MOVES 500
#define MAX_DEPTH 100
// with 16 byte hash entry is equal to 256 megabytes
#define TRANSPOSITION_TABLE_SIZE 16777216

#define PAWN_SCORE 100
#define KNIGHT_SCORE 320
#define BISHOP_SCORE 330
#define ROOK_SCORE 500
#define QUEEN_SCORE 900
#define KING_SCORE 20000

#define QUIET_MOVE 0
// #define DOUBLE_PAWN_MOVE 1
#define KING_CASTLE_MOVE 2
#define QUEEN_CASTLE_MOVE 3
#define CAPTURE_MOVE 4
#define EP_CAPTURE_MOVE 5
// #define NULL_MOVE 6
#define INVALID_MOVE 7
#define KNIGHT_PROMOTION_MOVE 8
#define BISHOP_PROMOTION_MOVE 9
#define ROOK_PROMOTION_MOVE 10
#define QUEEN_PROMOTION_MOVE 11
#define KNIGHT_PROMOTION_CAPTURE_MOVE 12
#define BISHOP_PROMOTION_CAPTURE_MOVE 13
#define ROOK_PROMOTION_CAPTURE_MOVE 14
#define QUEEN_PROMOTION_CAPTURE_MOVE 15

#define WHITE 1
#define BLACK -1

#define DELTA 200

#define PV_NODE 1
#define ALL_NODE 2
#define CUT_NODE 3

#define ALPHA_BETA_MIN -1000000000
#define ALPHA_BETA_MAX 1000000000
#define DRAW 0
#define CONTEMPT 50
#define CHECKMATE 10000000

extern Board g_gameStateStack[MAX_DEPTH + MAX_TURNS];
// zobrist hash keys of positions
extern u64 g_zobristStack[MAX_DEPTH + MAX_TURNS];
extern int g_root;
extern int g_cancelThread;
extern int g_ply;

extern Move g_moveStack[MAX_DEPTH][MAX_MOVES];
extern u64 g_attackSets[MAX_DEPTH];

// precalculated bit shifts, for example singleBitLookUp[15] == 1LLU << 15
extern u64 SINGLE_BIT_LOOKUP[64];
// pattern used to calculate north-east diagonal moves with bitboards
extern u64 NORTH_EAST_LOOKUP_PATTERN[64];
extern u64 NORTH_WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_EAST_LOOKUP_PATTERN[64];
extern u64 SOUTH_WEST_LOOKUP_PATTERN[64];

#endif