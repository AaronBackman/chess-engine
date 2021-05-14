#include <stdbool.h>
#include <stdio.h>

#include "Move.h"
#include "Board.h"
#include "MakeMove.h"
#include "Constants.h"
#include "MoveGeneration.h"
#include "Utilities.h"
#include "AttackMaps.h"

u64 get_attack_defend_maps(int originSquare) {
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = gameState.whitePieces;
    u64 blackPieces = gameState.blackPieces;
    
    u64 pawns = gameState.pawns;
    u64 knights = gameState.knights;
    u64 diagonals = gameState.bishops | gameState.queens;
    u64 linears = gameState.rooks | gameState.queens;
    u64 kings = gameState.kings;

    u64 attackDefendMaps = 0;
    u64 pawnPattern;
    u64 knightPattern;
    u64 diagonalPattern;
    u64 linearPattern;
    u64 kingPattern;
    
    pawnPattern = get_white_pawn_attack_maps(gameState, originSquare);
    attackDefendMaps |= (pawnPattern & pawns & blackPieces);

    pawnPattern = get_black_pawn_attack_maps(gameState, originSquare);
    attackDefendMaps |= (pawnPattern & pawns & whitePieces);

    knightPattern = get_knight_maps(gameState, originSquare);
    attackDefendMaps |= (knightPattern & knights);

    diagonalPattern = get_diagonal_maps(gameState, originSquare);
    attackDefendMaps |= (diagonalPattern & diagonals);

    linearPattern = get_linear_maps(gameState, originSquare);
    attackDefendMaps |= (linearPattern & linears);

    kingPattern = get_king_maps(gameState, originSquare);
    attackDefendMaps |= (kingPattern & kings);

    return attackDefendMaps;
}

u64 get_threat_map(int originSquare, int side) {
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = gameState.whitePieces;
    u64 blackPieces = gameState.blackPieces;

    u64 attackDefendMaps = get_attack_defend_maps(originSquare);
    
    if (side == WHITE) {
        return attackDefendMaps & blackPieces;
    }
    else {
        return attackDefendMaps & whitePieces;
    }
}

bool is_square_threatened(int originSquare, int side) {
    u64 threatMap;

    threatMap = get_threat_map(originSquare, side);

    // if threats are not an empty set, square is threatened
    return threatMap != 0;
}

bool is_king_threatened(int side) {
    int i;
    int kingIndex;
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whiteKings = gameState.kings & gameState.whitePieces;
    u64 blackKings = gameState.kings & gameState.blackPieces;

    if (side == 1) {
        // find the kings square first
        kingIndex = bitscan_forward(whiteKings);
        return is_square_threatened(kingIndex, side);
    }
    else {
        // find the kings square first
        kingIndex = bitscan_forward(blackKings);
        return is_square_threatened(kingIndex, side);
    }
}

// filters out pseudo-legal moves, basically notices checkmates and draws by no moves
bool check_if_no_legal_moves(int side) {
    int moveCount;
    int i;
    Move checkMateMoves[MAX_MOVES];

    moveCount = generate_moves(checkMateMoves, side);

    // no possible pseudo-legal moves -> no legal moves either
    if (moveCount == 0) return true;

    bool noMoves = true;
    for (i = 0; i < moveCount; i++) {
        make_move(checkMateMoves[i], false);
        if (!is_king_threatened(side)) {
            noMoves = false;
            unmake_move();
            break;
        }
        unmake_move();
    }

    return noMoves;
}

bool is_repeating() {
    u64 currentZobrist = g_zobristStack[g_root + g_ply];
    u64 oldZobrist;
    int ply = 4;
    int start = g_root + g_ply;

    while (true) {
        if (start - ply < 0) {
            return false;
        }

        oldZobrist = g_zobristStack[start - ply];
        if (currentZobrist == oldZobrist) {
            return true;
        }


        ply += 2;
    }
}