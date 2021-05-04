#ifndef ATTACK_MAPS_H
#define ATTACK_MAPS_H

u64 get_diagonal_maps(Board gameState, int square);
u64 get_linear_maps(Board gameState, int square);
u64 get_white_pawn_move_maps(Board gameState, int square);
u64 get_black_pawn_move_maps(Board gameState, int square);
u64 get_white_pawn_attack_maps(Board gameState, int square);
u64 get_black_pawn_attack_maps(Board gameState, int square);
u64 get_knight_maps(Board gameState, int square);
u64 get_king_maps(Board gameState, int square);

#endif