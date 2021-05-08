#ifndef UTILITIES_H
#define UTILITIES_H

#include <assert.h>

#include "Move.h"
#include "Constants.h"

inline Move create_move(u16 from, u16 to, u16 code) {
    Move move = {.from = from, .to = to, .code = code};

    return move;
}

inline bool square_occupied(u64 board, int index) {
    u64 bitIndex = SINGLE_BIT_LOOKUP[index];

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

inline u64 empty_square(u64 board, int index) {
    return board & ~SINGLE_BIT_LOOKUP[index];
}

inline u64 fill_square(u64 board, int index) {
    return board | SINGLE_BIT_LOOKUP[index];
}

bool can_white_castle_short(u64 meta);
bool can_white_castle_long(u64 meta);
bool can_black_castle_short(u64 meta);
bool can_black_castle_long(u64 meta);
u64 remove_white_castle_short(u64 meta);
u64 remove_white_castle_long(u64 meta);
u64 remove_black_castle_short(u64 meta);
u64 remove_black_castle_long(u64 meta);

u64 set_white_castle_short(u64 meta);
u64 set_white_castle_long(u64 meta);
u64 set_black_castle_short(u64 meta);
u64 set_black_castle_long(u64 meta);

bool is_enpassant_allowed(u64 meta);
u64 set_enpassant_allowed(u64 meta, bool enPassantAllowed);
u64 set_enpassant_square(u64 meta, u64 square);
u64 get_enpassant_square(u64 meta);

int get_side_to_play(u64 meta);
u64 set_side_to_play(u64 meta, int side);

void move_to_string(char *str, Move move);

const extern int index64Forward[64];
inline int bitscan_forward(u64 board) {
    const u64 debruijn64 = 0x03f79d71b4cb0a89;
    assert (board != 0);
    return index64Forward[((board & -board) * debruijn64) >> 58];
}

const extern int index64Reverse[64];
inline int bitscan_reverse(u64 board) {
   const u64 debruijn64 = 0x03f79d71b4cb0a89;
   assert (board != 0);
   board |= board >> 1; 
   board |= board >> 2;
   board |= board >> 4;
   board |= board >> 8;
   board |= board >> 16;
   board |= board >> 32;
   return index64Reverse[(board * debruijn64) >> 58];
}

int get_manhattan_distance(int sq1, int sq2);

void print_board();

void add_tt_entry(u64 zobristKey, int score, Move hashMove, u16 nodeType, u16 depth);

// very inefficient, only used in testing
u64 get_zobrist(Board board);

void print_in_binary(u64 n);

#endif