#ifndef ATTACK_MAPS_H
#define ATTACK_MAPS_H

u64 getDiagonalMaps(u64 *gameState, int square);
u64 getLinearMaps(u64 *gameState, int square);
u64 getWhitePawnMoveMaps(u64 *gameState, int square);
u64 getBlackPawnMoveMaps(u64 *gameState, int square);
u64 getWhitePawnAttackMaps(u64 *gameState, int square);
u64 getBlackPawnAttackMaps(u64 *gameState, int square);
u64 getKnightMaps(u64 *gameState, int square);
u64 getKingMaps(u64 *gameState, int square);

#endif