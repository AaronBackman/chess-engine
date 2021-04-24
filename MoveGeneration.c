#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "Constants.h"
#include "Move.h"
#include "Utilities.h"
#include "LegalityChecks.h"
#include "AttackMaps.h"

// includes moves, not captures
typedef struct moveTable {
    u32 from : 6;
    u64 moveBitMap;
    u32 pieceType : 2; // 2 == king, 1 == pawn, 0 == other
} MoveTable;

// includes captures, not moves
typedef struct captureTable {
    u32 from : 6;
    u64 captureBitMap;
    u32 pieceType : 2; // 2 == king, 1 == pawn, 0 == other
} CaptureTable;

// max 18 pieces per side
MoveTable moveTables[18];
CaptureTable captureTables[18];


void add_white_pawn_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 1};
    MoveTable moveTable = {square, 0, 1};
    u64 blackPieces = gameState[7];
    u64 movePattern = get_white_pawn_move_maps(gameState, square);
    u64 attackPattern = get_white_pawn_attack_maps(gameState, square);

    attackPattern &= blackPieces;

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_black_pawn_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 1};
    MoveTable moveTable = {square, 0, 1};
    u64 whitePieces = gameState[0];
    u64 movePattern = get_black_pawn_move_maps(gameState, square);
    u64 attackPattern = get_black_pawn_attack_maps(gameState, square);

    attackPattern &= whitePieces;

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_white_knight_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_knight_maps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_black_knight_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_knight_maps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_white_king_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 2};
    MoveTable moveTable = {square, 0, 2};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_king_maps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_black_king_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 2};
    MoveTable moveTable = {square, 0, 2};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_king_maps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_white_diagonal_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_diagonal_maps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_black_diagonal_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_diagonal_maps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_white_linear_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_linear_maps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void add_black_linear_patterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = get_linear_maps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}



bool short_white_castling_squares_empty() {
    u64 *gameState = g_gameStateStack[g_root + g_ply];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (square_occupied(whitePieces, 5) || square_occupied(blackPieces, 5)) return false;

    if (square_occupied(whitePieces, 6) || square_occupied(blackPieces, 6)) return false;

    return true;
}

bool long_white_castling_squares_empty() {
    u64 *gameState = g_gameStateStack[g_root + g_ply];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (square_occupied(whitePieces, 1) || square_occupied(blackPieces, 1)) return false;

    if (square_occupied(whitePieces, 2) || square_occupied(blackPieces, 2)) return false;

    if (square_occupied(whitePieces, 3) || square_occupied(blackPieces, 3)) return false;

    return true;
}

bool short_black_castling_squares_empty() {
    u64 *gameState = g_gameStateStack[g_root + g_ply];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (square_occupied(whitePieces, 61) || square_occupied(blackPieces, 61)) return false;

    if (square_occupied(whitePieces, 62) || square_occupied(blackPieces, 62)) return false;

    return true;
}

bool long_black_castling_squares_empty() {
    u64 *gameState = g_gameStateStack[g_root + g_ply];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (square_occupied(whitePieces, 57) || square_occupied(blackPieces, 57)) return false;

    if (square_occupied(whitePieces, 58) || square_occupied(blackPieces, 58)) return false;

    if (square_occupied(whitePieces, 59) || square_occupied(blackPieces, 59)) return false;

    return true;
}


// board coordinates are from 0 to 63 and start from bottom left from
// whites point of view, they go left to right, bottom to top

// a single move: 32-bit unsigned int, 8 bits: original location,
// 8 bits destination location,
// 3 bits promotion (queen, rook, bishop, knight or none)
// 2 bits for castling (queenside, kingside or none)
// in case of castling, the castling king is determined by the original location

// game state: an array consisting of 13 unsigned 64-bit integers (long long)
// first 12 of those use used to contain location of a certain piece
// such as white pawn in the 64 squares of the board
// so if for example 45:th bit is 1, the 44:th (0-index) square of
// the board has a white pawn

// the game state contains pawns, knights, bishops, rooks, queens and king
// in that order first for white then for black
// the last number (13:th) is used to store other useful data about the game
// such as castling rights (2 bits for both sides), en passant (8 bits) and information concerning
// the 3-fold repetition (4 bits) and the 50-move rule (9 bits)

// side: 0 == white, 1 == black
int generate_moves(Move *movesArr, int side) {
    int i;
    // next free index in moves array
    int patternIndex = 0;
    int moveIndex = 0;
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
    u64 whiteDiagonals = whiteQueens | whiteBishops;
    u64 blackDiagonals = blackQueens | blackBishops;
    u64 whiteLinears = whiteQueens | whiteRooks;
    u64 blackLinears = blackQueens | blackRooks;

    bool isEnPassant = is_enpassant_allowed(otherGameInfo);

    u64 attackSet = g_attackSets[g_ply];

    if (side == 1) {
        while(whitePawns != 0) {
            int index = bitscan_forward(whitePawns);
            whitePawns &= (whitePawns - 1); // reset the least significant bit
            add_white_pawn_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKnights != 0) {
            int index = bitscan_forward(whiteKnights);
            whiteKnights &= (whiteKnights - 1); // reset the least significant bit
            add_white_knight_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteDiagonals != 0) {
            int index = bitscan_forward(whiteDiagonals);
            whiteDiagonals &= (whiteDiagonals - 1); // reset the least significant bit
            add_white_diagonal_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteLinears != 0) {
            int index = bitscan_forward(whiteLinears);
            whiteLinears &= (whiteLinears - 1); // reset the least significant bit
            add_white_linear_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKings != 0) {
            int index = bitscan_forward(whiteKings);
            whiteKings &= (whiteKings - 1); // reset the least significant bit
            add_white_king_patterns(gameState, index, patternIndex);
            patternIndex++;
        }
    }
    else {
        while(blackPawns != 0) {
            int index = bitscan_forward(blackPawns);
            blackPawns &= (blackPawns - 1); // reset the least significant bit
            add_black_pawn_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKnights != 0) {
            int index = bitscan_forward(blackKnights);
            blackKnights &= (blackKnights - 1); // reset the least significant bit
            add_black_knight_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackDiagonals != 0) {
            int index = bitscan_forward(blackDiagonals);
            blackDiagonals &= (blackDiagonals - 1); // reset the least significant bit
            add_black_diagonal_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackLinears != 0) {
            int index = bitscan_forward(blackLinears);
            blackLinears &= (blackLinears - 1); // reset the least significant bit
            add_black_linear_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKings != 0) {
            int index = bitscan_forward(blackKings);
            blackKings &= (blackKings - 1); // reset the least significant bit
            add_black_king_patterns(gameState, index, patternIndex);
            patternIndex++;
        }
    }

    // first create capturing moves
    for (i = 0; i < patternIndex; i++) {
        CaptureTable captureTable;
        int from;
        int to;
        u64 pattern;
        int type;

        captureTable = captureTables[i];
        from = captureTable.from;
        pattern = captureTable.captureBitMap;
        type = captureTable.pieceType;

        attackSet |= pattern;

        // pawn move
        if (type == 1) {
            int row;
            // if row is 6 or 1, there is a possibility of promotion
            row = from / 8;
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                // promotion
                if ((row == 6 && side == 1) || (row == 1 && side == -1)) {
                    movesArr[moveIndex] = create_move(from, to, 1, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 2, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 3, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 4, 0, 0);
                    moveIndex++;
                }
                // no promotion
                else {
                    movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                    moveIndex++;
                }
            }
        }
        else {
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    g_attackSets[g_ply] = attackSet;

    // then create non-capturing moves
    for (i = 0; i < patternIndex; i++) {
        MoveTable moveTable;
        int from;
        int to;
        u64 pattern;
        int type;

        moveTable = moveTables[i];
        from = moveTable.from;
        pattern = moveTable.moveBitMap;
        type = moveTable.pieceType;

        // pawn move
        if (type == 1) {
            // if row is 6 or 1, there is a possibility of promotion
            int row = from / 8;
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                // promotion
                if ((row == 6 && side == 1) || (row == 1 && side == -1)) {
                    movesArr[moveIndex] = create_move(from, to, 1, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 2, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 3, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 4, 0, 0);
                    moveIndex++;
                }
                // no promotion
                else {
                    movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                    moveIndex++;
                }
            }
        }
        else {
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // handle enPassant
    if (isEnPassant) {
        int enPassantSquare = get_enpassant_square(otherGameInfo);
        int enPassantColumn = enPassantSquare % 8;

        if (side == 1) {
            whitePawns = gameState[1];

            if (enPassantColumn != 0 && square_occupied(whitePawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare - 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && square_occupied(whitePawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare + 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }
        }
        else {
            blackPawns = gameState[8];

            if (enPassantColumn != 0 && square_occupied(blackPawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare - 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && square_occupied(blackPawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare + 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }
        }
    }

    // castling move generation
    if (side == 1) {
        if (can_white_castle_short(otherGameInfo)) {
            if (short_white_castling_squares_empty()) {
                // king cant be or move through any threatened squares
                if (!is_square_threatened(4, side) && !is_square_threatened(5, side) && !is_square_threatened(5, side)) {
                    movesArr[moveIndex] = create_move(4, 6, 0, 1, 0);
                    moveIndex++;
                }
            }
        }

        if (can_white_castle_long(otherGameInfo)) {
            if (long_white_castling_squares_empty()) {
                // king cant be or move through any threatened squares
                if (!is_square_threatened(2, side) && !is_square_threatened(3, side) && !is_square_threatened(4, side)) {
                    movesArr[moveIndex] = create_move(4, 2, 0, 2, 0);
                    moveIndex++;
                }
            }
        }
    }
    else {
        if (can_black_castle_short(otherGameInfo)) {
            if (short_black_castling_squares_empty()) {
                // king cant be or move through any threatened squares
                if (!is_square_threatened(60, side) && !is_square_threatened(61, side) && !is_square_threatened(62, side)) {
                    movesArr[moveIndex] = create_move(60, 62, 0, 3, 0);
                    moveIndex++;
                }
            }
        }

        if (can_black_castle_long(otherGameInfo)) {
            if (long_black_castling_squares_empty()) {
                // king cant be or move through any threatened squares
                if (!is_square_threatened(58, side) && !is_square_threatened(59, side) && !is_square_threatened(60, side)) {
                    movesArr[moveIndex] = create_move(60, 58, 0, 4, 0);
                    moveIndex++; 
                }
            }
        }
    }

    return moveIndex;
}

// only generate capturing moves (pseudolegal), used in qsearch
int generate_captures(Move *movesArr, int side) {
    int i;
    // next free index in moves array
    int patternIndex = 0;
    int moveIndex = 0;
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
    u64 whiteDiagonals = whiteQueens | whiteBishops;
    u64 blackDiagonals = blackQueens | blackBishops;
    u64 whiteLinears = whiteQueens | whiteRooks;
    u64 blackLinears = blackQueens | blackRooks;

    bool isEnPassant = is_enpassant_allowed(otherGameInfo);
    u64 attackSet = g_attackSets[g_ply];

    if (side == 1) {
        while(whitePawns != 0) {
            int index = bitscan_forward(whitePawns);
            whitePawns &= (whitePawns - 1); // reset the least significant bit
            add_white_pawn_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKnights != 0) {
            int index = bitscan_forward(whiteKnights);
            whiteKnights &= (whiteKnights - 1); // reset the least significant bit
            add_white_knight_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteDiagonals != 0) {
            int index = bitscan_forward(whiteDiagonals);
            whiteDiagonals &= (whiteDiagonals - 1); // reset the least significant bit
            add_white_diagonal_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteLinears != 0) {
            int index = bitscan_forward(whiteLinears);
            whiteLinears &= (whiteLinears - 1); // reset the least significant bit
            add_white_linear_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKings != 0) {
            int index = bitscan_forward(whiteKings);
            whiteKings &= (whiteKings - 1); // reset the least significant bit
            add_white_king_patterns(gameState, index, patternIndex);
            patternIndex++;
        }
    }
    else {
        while(blackPawns != 0) {
            int index = bitscan_forward(blackPawns);
            blackPawns &= (blackPawns - 1); // reset the least significant bit
            add_black_pawn_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKnights != 0) {
            int index = bitscan_forward(blackKnights);
            blackKnights &= (blackKnights - 1); // reset the least significant bit
            add_black_knight_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackDiagonals != 0) {
            int index = bitscan_forward(blackDiagonals);
            blackDiagonals &= (blackDiagonals - 1); // reset the least significant bit
            add_black_diagonal_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackLinears != 0) {
            int index = bitscan_forward(blackLinears);
            blackLinears &= (blackLinears - 1); // reset the least significant bit
            add_black_linear_patterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKings != 0) {
            int index = bitscan_forward(blackKings);
            blackKings &= (blackKings - 1); // reset the least significant bit
            add_black_king_patterns(gameState, index, patternIndex);
            patternIndex++;
        }
    }

    // create capturing moves
    for (i = 0; i < patternIndex; i++) {
        CaptureTable captureTable;
        int from;
        int to;
        u64 pattern;
        int type;

        captureTable = captureTables[i];
        from = captureTable.from;
        pattern = captureTable.captureBitMap;
        type = captureTable.pieceType;

        attackSet |= pattern;

        // pawn move
        if (type == 1) {
            int row;
            // if row is 6 or 1, there is a possibility of promotion
            row = from / 8;
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                // promotion
                if ((row == 6 && side == 1) || (row == 1 && side == -1)) {
                    movesArr[moveIndex] = create_move(from, to, 1, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 2, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 3, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = create_move(from, to, 4, 0, 0);
                    moveIndex++;
                }
                // no promotion
                else {
                    movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                    moveIndex++;
                }
            }
        }
        else {
            while (pattern != 0) {
                to = bitscan_forward(pattern);
                pattern &= (pattern - 1);

                movesArr[moveIndex] = create_move(from, to, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    g_attackSets[g_ply] = attackSet;

    // handle enPassant
    if (isEnPassant) {
        int enPassantSquare = get_enpassant_square(otherGameInfo);
        int enPassantColumn = enPassantSquare % 8;

        if (side == 1) {
            whitePawns = gameState[1];

            if (enPassantColumn != 0 && square_occupied(whitePawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare - 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && square_occupied(whitePawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare + 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }
        }
        else {
            blackPawns = gameState[8];

            if (enPassantColumn != 0 && square_occupied(blackPawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare - 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && square_occupied(blackPawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = create_move(enPassantSquare + 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }
        }
    }

    return moveIndex;
}