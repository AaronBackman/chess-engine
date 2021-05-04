#ifndef LEGALITY_CHECKS_H
#define LEGALITY_CHECKS_H

u64 get_threat_map(int originSquare, int side);
bool is_square_threatened(int originSquare, int side);
bool is_king_threatened();
bool check_if_no_legal_moves(int side);

bool is_repeating();

#endif