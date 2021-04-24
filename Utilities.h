#ifndef UTILITIES_H
#define UTILITIES_H

#include "Move.h"

Move create_move(int from, int to, int promotion, int castle, bool enPassant);
bool square_occupied(u64 board, int index);
u64 empty_square(u64 board, int index);
u64 fill_square(u64 board, int index);

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
int bitscan_forward(u64 board);
int bitscan_reverse(u64 board);

int get_manhattan_distance(int sq1, int sq2);

void print_board();

#endif