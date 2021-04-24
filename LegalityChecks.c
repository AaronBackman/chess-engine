#include <stdbool.h>
#include <stdio.h>

#include "Move.h"
#include "MakeMove.h"
#include "Constants.h"
#include "MoveGeneration.h"
#include "Utilities.h"
#include "AttackMaps.h"

u64 get_threat_map(int originSquare, int side) {
    int i;
    int j;
    int squareIndex;
    u64 *gameState = g_gameStateStack[g_root + g_ply];

    u64 whitePieces = gameState[0];
    u64 whitePawns = gameState[1];
    u64 whiteKnights = gameState[2];
    u64 whiteBishops = gameState[3];
    u64 whiteRooks = gameState[4];
    u64 whiteQueens = gameState[5];
    u64 whiteKings = gameState[6];

    u64 blackPieces = gameState[7];
    u64 blackPawns = gameState[8];
    u64 blackKnights = gameState[9];
    u64 blackBishops = gameState[10];
    u64 blackRooks = gameState[11];
    u64 blackQueens = gameState[12];
    u64 blackKings = gameState[13];

    u64 otherGameInfo = gameState[14];
    u64 whiteDiagonals = whiteBishops | whiteQueens;
    u64 whiteLinears = whiteRooks | whiteQueens;
    u64 blackDiagonals = blackBishops | blackQueens;
    u64 blackLinears = blackRooks | blackQueens;

    u64 threatMap = 0;
    u64 pawnPattern;
    u64 knightPattern;
    u64 diagonalPattern;
    u64 linearPattern;
    u64 kingPattern;

    if (side == 1) {
        // pawns can always attack each other
        pawnPattern = get_white_pawn_attack_maps(gameState, originSquare);
        threatMap |= (pawnPattern & blackPawns);

        knightPattern = get_knight_maps(gameState, originSquare);
        threatMap |= (knightPattern & blackKnights);

        diagonalPattern = get_diagonal_maps(gameState, originSquare);
        threatMap |= (diagonalPattern & blackDiagonals);

        linearPattern = get_linear_maps(gameState, originSquare);
        threatMap |= (linearPattern & blackLinears);

        kingPattern = get_king_maps(gameState, originSquare);
        threatMap |= (kingPattern & blackKings);
    }
    else {
        // pawns can always attack each other
        pawnPattern = get_black_pawn_attack_maps(gameState, originSquare);
        threatMap |= (pawnPattern & whitePawns);

        knightPattern = get_knight_maps(gameState, originSquare);
        threatMap |= (knightPattern & whiteKnights);

        diagonalPattern = get_diagonal_maps(gameState, originSquare);
        threatMap |= (diagonalPattern & whiteDiagonals);

        linearPattern = get_linear_maps(gameState, originSquare);
        threatMap |= (linearPattern & whiteLinears);

        kingPattern = get_king_maps(gameState, originSquare);
        threatMap |= (kingPattern & whiteKings);
    }

    return threatMap;
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
    u64 *gameState = g_gameStateStack[g_root + g_ply];
    u64 whiteKings = gameState[6];
    u64 blackKings = gameState[13];

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