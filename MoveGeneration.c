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

int getNumberOfKingAttackers(int kingIndex, int side) {
    u64 threatMap;
    int leastBit;
    
    threatMap = getThreatMap(kingIndex, side);

    if (threatMap == 0) return threatMap;

    leastBit = bitScanForward(threatMap);

    // remove the found bit
    threatMap ^= SINGLE_BIT_LOOKUP[leastBit];

    // threatmap is empty after removing 1 attacker
    if (threatMap == 0) return 1;

    // king can only have 2 attackers in legal positions
    return 2;
}


void addWhitePawnPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 1};
    MoveTable moveTable = {square, 0, 1};
    u64 blackPieces = gameState[7];
    u64 movePattern = getWhitePawnMoveMaps(gameState, square);
    u64 attackPattern = getWhitePawnAttackMaps(gameState, square);

    attackPattern &= blackPieces;

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addBlackPawnPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 1};
    MoveTable moveTable = {square, 0, 1};
    u64 whitePieces = gameState[0];
    u64 movePattern = getBlackPawnMoveMaps(gameState, square);
    u64 attackPattern = getBlackPawnAttackMaps(gameState, square);

    attackPattern &= whitePieces;

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addWhiteKnightPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getKnightMaps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addBlackKnightPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getKnightMaps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addWhiteKingPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 2};
    MoveTable moveTable = {square, 0, 2};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getKingMaps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addBlackKingPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 2};
    MoveTable moveTable = {square, 0, 2};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getKingMaps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addWhiteDiagonalPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getDiagonalMaps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addBlackDiagonalPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getDiagonalMaps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addWhiteLinearPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getLinearMaps(gameState, square);

    attackPattern = pattern & blackPieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~whitePieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}

void addBlackLinearPatterns(u64 *gameState, int square, int patternIndex) {
    CaptureTable captureTable = {square, 0, 0};
    MoveTable moveTable = {square, 0, 0};
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    u64 attackPattern;
    u64 movePattern;
    u64 pattern = getLinearMaps(gameState, square);

    attackPattern = pattern & whitePieces;
    movePattern = pattern ^ attackPattern;
    movePattern &= (~blackPieces);

    captureTable.captureBitMap = attackPattern;
    moveTable.moveBitMap = movePattern;
    captureTables[patternIndex] = captureTable;
    moveTables[patternIndex] = moveTable;
}



bool shortWhiteCastlingSquaresEmpty() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (squareOccupied(whitePieces, 5) || squareOccupied(blackPieces, 5)) return false;

    if (squareOccupied(whitePieces, 6) || squareOccupied(blackPieces, 6)) return false;

    return true;
}

bool longWhiteCastlingSquaresEmpty() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (squareOccupied(whitePieces, 1) || squareOccupied(blackPieces, 1)) return false;

    if (squareOccupied(whitePieces, 2) || squareOccupied(blackPieces, 2)) return false;

    if (squareOccupied(whitePieces, 3) || squareOccupied(blackPieces, 3)) return false;

    return true;
}

bool shortBlackCastlingSquaresEmpty() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (squareOccupied(whitePieces, 61) || squareOccupied(blackPieces, 61)) return false;

    if (squareOccupied(whitePieces, 62) || squareOccupied(blackPieces, 62)) return false;

    return true;
}

bool longBlackCastlingSquaresEmpty() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    if (squareOccupied(whitePieces, 57) || squareOccupied(blackPieces, 57)) return false;

    if (squareOccupied(whitePieces, 58) || squareOccupied(blackPieces, 58)) return false;

    if (squareOccupied(whitePieces, 59) || squareOccupied(blackPieces, 59)) return false;

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
int generateMoves(Move *movesArr, int side) {
    int i;
    // next free index in moves array
    int patternIndex = 0;
    int moveIndex = 0;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
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

    bool isEnPassant = isEnPassantAllowed(otherGameInfo);

    assert(MOVE_STACK_POINTER < MAX_DEPTH);

    if (side == 1) {
        while(whitePawns != 0) {
            int index = bitScanForward(whitePawns);
            whitePawns &= (whitePawns - 1); // reset the least significant bit
            addWhitePawnPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKnights != 0) {
            int index = bitScanForward(whiteKnights);
            whiteKnights &= (whiteKnights - 1); // reset the least significant bit
            addWhiteKnightPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteDiagonals != 0) {
            int index = bitScanForward(whiteDiagonals);
            whiteDiagonals &= (whiteDiagonals - 1); // reset the least significant bit
            addWhiteDiagonalPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteLinears != 0) {
            int index = bitScanForward(whiteLinears);
            whiteLinears &= (whiteLinears - 1); // reset the least significant bit
            addWhiteLinearPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(whiteKings != 0) {
            int index = bitScanForward(whiteKings);
            whiteKings &= (whiteKings - 1); // reset the least significant bit
            addWhiteKingPatterns(gameState, index, patternIndex);
            patternIndex++;
        }
    }
    else {
        while(blackPawns != 0) {
            int index = bitScanForward(blackPawns);
            blackPawns &= (blackPawns - 1); // reset the least significant bit
            addBlackPawnPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKnights != 0) {
            int index = bitScanForward(blackKnights);
            blackKnights &= (blackKnights - 1); // reset the least significant bit
            addBlackKnightPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackDiagonals != 0) {
            int index = bitScanForward(blackDiagonals);
            blackDiagonals &= (blackDiagonals - 1); // reset the least significant bit
            addBlackDiagonalPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackLinears != 0) {
            int index = bitScanForward(blackLinears);
            blackLinears &= (blackLinears - 1); // reset the least significant bit
            addBlackLinearPatterns(gameState, index, patternIndex);
            patternIndex++;
        }

        while(blackKings != 0) {
            int index = bitScanForward(blackKings);
            blackKings &= (blackKings - 1); // reset the least significant bit
            addBlackKingPatterns(gameState, index, patternIndex);
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

        // pawn move
        if (type == 1) {
            int row;
            // if row is 6 or 1, there is a possibility of promotion
            row = from / 8;
            while (pattern != 0) {
                to = bitScanForward(pattern);
                pattern &= (pattern - 1);

                // promotion
                if ((row == 6 && side == 1) || (row == 1 && side == -1)) {
                    movesArr[moveIndex] = createMove(from, to, 1, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 2, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 3, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 4, 0, 0);
                    moveIndex++;
                }
                // no promotion
                else {
                    movesArr[moveIndex] = createMove(from, to, 0, 0, 0);
                    moveIndex++;
                }
            }
        }
        else {
            while (pattern != 0) {
                to = bitScanForward(pattern);
                pattern &= (pattern - 1);

                movesArr[moveIndex] = createMove(from, to, 0, 0, 0);
                moveIndex++;
            }
        }
    }

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
                to = bitScanForward(pattern);
                pattern &= (pattern - 1);

                // promotion
                if ((row == 6 && side == 1) || (row == 1 && side == -1)) {
                    movesArr[moveIndex] = createMove(from, to, 1, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 2, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 3, 0, 0);
                    moveIndex++;
                    movesArr[moveIndex] = createMove(from, to, 4, 0, 0);
                    moveIndex++;
                }
                // no promotion
                else {
                    movesArr[moveIndex] = createMove(from, to, 0, 0, 0);
                    moveIndex++;
                }
            }
        }
        else {
            while (pattern != 0) {
                to = bitScanForward(pattern);
                pattern &= (pattern - 1);

                movesArr[moveIndex] = createMove(from, to, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // handle enPassant
    if (isEnPassant) {
        int enPassantSquare = getEnPassantSquare(otherGameInfo);
        int enPassantColumn = enPassantSquare % 8;

        if (side == 1) {
            whitePawns = gameState[1];

            if (enPassantColumn != 0 && squareOccupied(whitePawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = createMove(enPassantSquare - 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && squareOccupied(whitePawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = createMove(enPassantSquare + 1, enPassantSquare + 8, 0, 0, 1);
                moveIndex++;
            }
        }
        else {
            blackPawns = gameState[8];

            if (enPassantColumn != 0 && squareOccupied(blackPawns, enPassantSquare - 1)) {
                movesArr[moveIndex] = createMove(enPassantSquare - 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }

            if (enPassantColumn != 7 && squareOccupied(blackPawns, enPassantSquare + 1)) {
                movesArr[moveIndex] = createMove(enPassantSquare + 1, enPassantSquare - 8, 0, 0, 1);
                moveIndex++;
            }
        }
    }

    // castling move generation
    if (side == 1) {
        if (canWhiteCastleShort(otherGameInfo)) {
            if (shortWhiteCastlingSquaresEmpty()) {
                // king cant be or move through any threatened squares
                if (!isSquareThreatened(4, side) && !isSquareThreatened(5, side) && !isSquareThreatened(5, side)) {
                    movesArr[moveIndex] = createMove(4, 6, 0, 1, 0);
                    moveIndex++;
                }
            }
        }

        if (canWhiteCastleLong(otherGameInfo)) {
            if (longWhiteCastlingSquaresEmpty()) {
                // king cant be or move through any threatened squares
                if (!isSquareThreatened(2, side) && !isSquareThreatened(3, side) && !isSquareThreatened(4, side)) {
                    movesArr[moveIndex] = createMove(4, 2, 0, 2, 0);
                    moveIndex++;
                }
            }
        }
    }
    else {
        if (canBlackCastleShort(otherGameInfo)) {
            if (shortBlackCastlingSquaresEmpty()) {
                // king cant be or move through any threatened squares
                if (!isSquareThreatened(60, side) && !isSquareThreatened(61, side) && !isSquareThreatened(62, side)) {
                    movesArr[moveIndex] = createMove(60, 62, 0, 3, 0);
                    moveIndex++;
                }
            }
        }

        if (canBlackCastleLong(otherGameInfo)) {
            if (longBlackCastlingSquaresEmpty()) {
                // king cant be or move through any threatened squares
                if (!isSquareThreatened(58, side) && !isSquareThreatened(59, side) && !isSquareThreatened(60, side)) {
                    movesArr[moveIndex] = createMove(60, 58, 0, 4, 0);
                    moveIndex++; 
                }
            }
        }
    }

    return moveIndex;
}