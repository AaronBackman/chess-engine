#ifndef INIT_H
#define INIT_H

#include "Move.h"
#include "TranspositionTable.h"

// precalculated bit shifts, for example singleBitLookUp[15] == 1LLU << 15
extern u64 SINGLE_BIT_LOOKUP[64];
// pattern used to calculate north-east diagonal moves with bitboards
extern u64 NORTH_EAST_LOOKUP_PATTERN[64];
extern u64 NORTH_WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_EAST_LOOKUP_PATTERN[64];
extern u64 SOUTH_WEST_LOOKUP_PATTERN[64];

extern u64 NORTH_LOOKUP_PATTERN[64];
extern u64 WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_LOOKUP_PATTERN[64];
extern u64 EAST_LOOKUP_PATTERN[64];

extern u64 KING_LOOKUP_PATTERN[64];
extern u64 KNIGHT_LOOKUP_PATTERN[64];
extern u64 WHITE_PAWN_ATTACK_LOOKUP_PATTERN[64];
extern u64 WHITE_PAWN_MOVE_LOOKUP_PATTERN[64];
extern u64 BLACK_PAWN_ATTACK_LOOKUP_PATTERN[64];
extern u64 BLACK_PAWN_MOVE_LOOKUP_PATTERN[64];

extern int BLACK_PAWN_PIECE_SQUARE_TABLE[64];
extern int WHITE_PAWN_PIECE_SQUARE_TABLE[64];

extern int BLACK_KNIGHT_PIECE_SQUARE_TABLE[64];
extern int WHITE_KNIGHT_PIECE_SQUARE_TABLE[64];

extern int BLACK_BISHOP_PIECE_SQUARE_TABLE[64];
extern int WHITE_BISHOP_PIECE_SQUARE_TABLE[64];

extern int BLACK_ROOK_PIECE_SQUARE_TABLE[64];
extern int WHITE_ROOK_PIECE_SQUARE_TABLE[64];

extern int BLACK_QUEEN_PIECE_SQUARE_TABLE[64];
extern int WHITE_QUEEN_PIECE_SQUARE_TABLE[64];

extern int BLACK_KING_PIECE_SQUARE_TABLE_OPENING[64];
extern int WHITE_KING_PIECE_SQUARE_TABLE_OPENING[64];

extern int BLACK_KING_PIECE_SQUARE_TABLE_ENDGAME[64];
extern int WHITE_KING_PIECE_SQUARE_TABLE_ENDGAME[64];


extern u64 WHITE_PAWN_ZOBRIST[64];
extern u64 WHITE_KNIGHT_ZOBRIST[64];
extern u64 WHITE_BISHOP_ZOBRIST[64];
extern u64 WHITE_ROOK_ZOBRIST[64];
extern u64 WHITE_QUEEN_ZOBRIST[64];
extern u64 WHITE_KING_ZOBRIST[64];
extern u64 BLACK_PAWN_ZOBRIST[64];
extern u64 BLACK_KNIGHT_ZOBRIST[64];
extern u64 BLACK_BISHOP_ZOBRIST[64];
extern u64 BLACK_ROOK_ZOBRIST[64];
extern u64 BLACK_QUEEN_ZOBRIST[64];
extern u64 BLACK_KING_ZOBRIST[64];
extern u64 BLACK_TO_MOVE_ZOBRIST;
extern u64 CASTLING_ZOBRIST[4];
extern u64 ENPASSANT_FILE_ZOBRIST[8];

extern TranspositionTableEntry tTable[TRANSPOSITION_TABLE_SIZE];

// the center manhattan distance
extern unsigned char CMD[64];

// the center manhattan distance
extern unsigned char MD[64][64];

void set_lookup_tables();

#endif