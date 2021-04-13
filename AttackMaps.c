#include <stdbool.h>

#include "Constants.h"
#include "Init.h"
#include "Utilities.h"

// includes squares blocked by own pieces
u64 getDiagonalMaps(u64 *gameState, int square) {
    u64 blocker;
    int blockerSquare;
    u64 combinedMap;

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 occupied = whitePieces | blackPieces;
    u64 northEast = NORTH_EAST_LOOKUP_PATTERN[square];
    u64 northWest = NORTH_WEST_LOOKUP_PATTERN[square];
    u64 southEast = SOUTH_EAST_LOOKUP_PATTERN[square];
    u64 southWest = SOUTH_WEST_LOOKUP_PATTERN[square];

    blocker = occupied & northEast;
    if (blocker) {
        blockerSquare = bitScanForward(blocker);
        northEast ^= NORTH_EAST_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & northWest;
    if (blocker) {
        blockerSquare = bitScanForward(blocker);
        northWest ^= NORTH_WEST_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & southEast;
    if (blocker) {
        blockerSquare = bitScanReverse(blocker);
        southEast ^= SOUTH_EAST_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & southWest;
    if (blocker) {
        blockerSquare = bitScanReverse(blocker);
        southWest ^= SOUTH_WEST_LOOKUP_PATTERN[blockerSquare];
    }

    combinedMap = northEast | northWest | southEast | southWest;

    return combinedMap;
}

// includes squares blocked by own pieces
u64 getLinearMaps(u64 *gameState, int square) {
    u64 blocker;
    int blockerSquare;
    u64 combinedMap;

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 occupied = whitePieces | blackPieces;
    u64 north = NORTH_LOOKUP_PATTERN[square];
    u64 west = WEST_LOOKUP_PATTERN[square];
    u64 east = EAST_LOOKUP_PATTERN[square];
    u64 south = SOUTH_LOOKUP_PATTERN[square];

    blocker = occupied & north;
    if (blocker) {
        blockerSquare = bitScanForward(blocker);
        north ^= NORTH_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & east;
    if (blocker) {
        blockerSquare = bitScanForward(blocker);
        east ^= EAST_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & west;
    if (blocker) {
        blockerSquare = bitScanReverse(blocker);
        west ^= WEST_LOOKUP_PATTERN[blockerSquare];
    }

    blocker = occupied & south;
    if (blocker) {
        blockerSquare = bitScanReverse(blocker);
        south ^= SOUTH_LOOKUP_PATTERN[blockerSquare];
    }

    combinedMap = north | east | west | south;

    return combinedMap;
}

// gets squares where the pawn can move
u64 getWhitePawnMoveMaps(u64 *gameState, int square) {
    u64 blocker;
    int blockerSquare;

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 occupied = whitePieces | blackPieces;
    u64 pawnMovePattern = WHITE_PAWN_MOVE_LOOKUP_PATTERN[square];

    blocker = occupied & pawnMovePattern;
    
    if (blocker == 0) return pawnMovePattern;

    // no possible moves
    if (squareOccupied(occupied, square + 8)) return 0LLU;
    
    // otherwise is double pawn push blocked 2 squares forward -> one available move
    pawnMovePattern ^= blocker;

    return pawnMovePattern;
}

// gets squares where the pawn can move
u64 getBlackPawnMoveMaps(u64 *gameState, int square) {
    u64 blocker;
    int blockerSquare;

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 occupied = whitePieces | blackPieces;
    u64 pawnMovePattern = BLACK_PAWN_MOVE_LOOKUP_PATTERN[square];

    blocker = occupied & pawnMovePattern;
    
    if (blocker == 0) return pawnMovePattern;

    // no possible moves
    if (squareOccupied(occupied, square - 8)) return 0LLU;
    
    // otherwise is double pawn push blocked 2 squares forward -> one available move
    pawnMovePattern ^= blocker;

    return pawnMovePattern;
}

// gets squares where the pawn can attack, no en passant
u64 getWhitePawnAttackMaps(u64 *gameState, int square) {
    u64 blackPieces = gameState[7];
    u64 pawnAttackPattern = WHITE_PAWN_ATTACK_LOOKUP_PATTERN[square];

    pawnAttackPattern &= blackPieces;

    return pawnAttackPattern;
}

// gets squares where the pawn can attack, no en passant
u64 getBlackPawnAttackMaps(u64 *gameState, int square) {
    u64 whitePieces = gameState[0];
    u64 pawnAttackPattern = BLACK_PAWN_ATTACK_LOOKUP_PATTERN[square];

    pawnAttackPattern &= whitePieces;

    return pawnAttackPattern;
}

u64 getKnightMaps(u64 *gameState, int square) {
    return KNIGHT_LOOKUP_PATTERN[square];
}

u64 getKingMaps(u64 *gameState, int square) {
    return KING_LOOKUP_PATTERN[square];
}