#ifndef ATTACK_MAPS_H
#define ATTACK_MAPS_H

u64 get_diagonal_maps(u64 *gameState, int square);
u64 get_linear_maps(u64 *gameState, int square);
u64 get_white_pawn_move_maps(u64 *gameState, int square);
u64 get_black_pawn_move_maps(u64 *gameState, int square);
u64 get_white_pawn_attack_maps(u64 *gameState, int square);
u64 get_black_pawn_attack_maps(u64 *gameState, int square);
u64 get_knight_maps(u64 *gameState, int square);
u64 get_king_maps(u64 *gameState, int square);

#endif