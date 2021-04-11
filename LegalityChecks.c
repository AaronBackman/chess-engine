#include <stdbool.h>
#include <stdio.h>

#include "Move.h"
#include "MakeMove.h"
#include "Constants.h"
#include "MoveGeneration.h"
#include "Utilities.h"

bool isSquareThreatened(int originSquare, int side) {
    int i;
    int j;
    int squareIndex;
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

    if (side == 1) {
        int column = originSquare % 8;
        int row = originSquare / 8;

        // check linear, non-diagonal attacks from up
        for (i = 1; originSquare + i * 8 < 64; i++) {
            squareIndex = originSquare + i * 8;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(blackRooks, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from down
        for (i = 1; originSquare - i * 8 >= 0; i++) {
            squareIndex = originSquare - i * 8;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(blackRooks, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from left
        for (i = 1; originSquare % 8 - i >= 0; i++) {
            squareIndex = originSquare - i;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(blackRooks, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from right
        for (i = 1; originSquare % 8 + i < 8; i++) {
            squareIndex = originSquare + i;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(blackRooks, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }


        // check diagonal attacks from top left
        j = 1;
        for (i = 1; (originSquare + i * 8 < 64) && (originSquare % 8 - j >= 0); i++) {
            squareIndex = originSquare + i * 8 - j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(blackBishops, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from top right
        j = 1;
        for (i = 1; (originSquare + i * 8 < 64) && (originSquare % 8 + j < 8); i++) {
            squareIndex = originSquare + i * 8 + j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(blackBishops, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from bottom left
        j = 1;
        for (i = 1; (originSquare - i * 8 >= 0) && (originSquare % 8 - j >= 0); i++) {
            squareIndex = originSquare - i * 8 - j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(blackBishops, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from bottom right
        j = 1;
        for (i = 1; (originSquare - i * 8 >= 0) && (originSquare % 8 + j < 8); i++) {
            squareIndex = originSquare - i * 8 + j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(whitePieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(blackBishops, squareIndex)) return true;
            if (squareOccupied(blackQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        if (column != 0) {
            // a pawn attack
            if (squareOccupied(blackPawns, originSquare + 7)) {
                return true;
            }
        }

        if (column != 7) {
            // a pawn attack
            if (squareOccupied(blackPawns, originSquare + 9)) {
                return true;
            }
        }

        // check king attacks
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if ((originSquare % 8 + j < 0) || (originSquare % 8 + j > 7)) continue;
                if ((originSquare + i * 8 < 0) || (originSquare + i * 8 > 63)) continue;

                squareIndex = originSquare + j + i * 8;
                if (squareOccupied(blackKings, squareIndex)) return true;
            }
        }

        // check knight attacks
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (originSquare % 8 + j > 7 || originSquare % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    squareIndex = originSquare + i * 8 + j;

                    // cant move outside the board
                    if (squareIndex > 63 || squareIndex < 0) continue;

                    if (squareOccupied(blackKnights, squareIndex)) return true;
                }
            }
        }
    }
    else {
        int column = originSquare % 8;
        int row = originSquare / 8;

        // check linear, non-diagonal attacks from up
        for (i = 1; originSquare + i * 8 < 64; i++) {
            squareIndex = originSquare + i * 8;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(whiteRooks, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from down
        for (i = 1; originSquare - i * 8 >= 0; i++) {
            squareIndex = originSquare - i * 8;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(whiteRooks, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from left
        for (i = 1; originSquare % 8 - i >= 0; i++) {
            squareIndex = originSquare - i;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(whiteRooks, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check linear, non-diagonal attacks from right
        for (i = 1; originSquare % 8 + i < 8; i++) {
            squareIndex = originSquare + i;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only rook and queen move linearly
            if (squareOccupied(whiteRooks, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }


        // check diagonal attacks from top left
        j = 1;
        for (i = 1; (originSquare + i * 8 < 64) && (originSquare % 8 - j >= 0); i++) {
            squareIndex = originSquare + i * 8 - j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(whiteBishops, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from top right
        j = 1;
        for (i = 1; (originSquare + i * 8 < 64) && (originSquare % 8 + j < 8); i++) {
            squareIndex = originSquare + i * 8 + j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(whiteBishops, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from bottom left
        j = 1;
        for (i = 1; (originSquare - i * 8 >= 0) && (originSquare % 8 - j >= 0); i++) {
            squareIndex = originSquare - i * 8 - j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(whiteBishops, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        // check diagonal attacks from bottom right
        j = 1;
        for (i = 1; (originSquare - i * 8 >= 0) && (originSquare % 8 + j < 8); i++) {
            squareIndex = originSquare - i * 8 + j;
            j++;
            // cant be threatened by an empty square
            if (!squareOccupied(whitePieces | blackPieces, squareIndex)) continue;

            // cant be threatened by own pieces
            if(squareOccupied(blackPieces, squareIndex)) break;

            // only bishop and queen move linearly
            if (squareOccupied(whiteBishops, squareIndex)) return true;
            if (squareOccupied(whiteQueens, squareIndex)) return true;

            // other black piece, blocks any further attacks
            break;
        }

        if (column != 0) {
            // a pawn attack
            if (squareOccupied(whitePawns, originSquare - 9)) {
                return true;
            }
        }

        if (column != 7) {
            // a pawn attack
            if (squareOccupied(whitePawns, originSquare - 7)) {
                return true;
            }
        }

        // check king attacks
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if ((originSquare % 8 + j < 0) || (originSquare % 8 + j > 7)) continue;
                if ((originSquare + i * 8 < 0) || (originSquare + i * 8 > 63)) continue;

                squareIndex = originSquare + j + i * 8;
                if (squareOccupied(whiteKings, squareIndex)) return true;
            }
        }

        // check knight attacks
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (originSquare % 8 + j > 7 || originSquare % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    squareIndex = originSquare + i * 8 + j;

                    // cant move outside the board
                    if (squareIndex > 63 || squareIndex < 0) continue;

                    if (squareOccupied(whiteKnights, squareIndex)) return true;
                }
            }
        }
    }

    return false;
}

bool isKingThreatened(int side) {
    int i;
    int kingIndex;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 whiteKings = gameState[6];
    u64 blackKings = gameState[13];

    if (side == 1) {
        // find the kings square first
        kingIndex = bitScanForward(whiteKings);
        return isSquareThreatened(kingIndex, side);
    }
    else {
        // find the kings square first
        kingIndex = bitScanForward(blackKings);
        return isSquareThreatened(kingIndex, side);
    }
}

// filters out pseudo-legal moves, basically notices checkmates and draws by no moves
bool checkIfNoLegalMoves(int side) {
    int moveCount;
    int i;
    Move checkMateMoves[MAX_MOVES];

    moveCount = generateMoves(checkMateMoves, side);

    // no possible pseudo-legal moves -> no legal moves either
    if (moveCount == 0) return true;

    bool noMoves = true;
    for (i = 0; i < moveCount; i++) {
        makeMove(checkMateMoves[i]);
        if (!isKingThreatened(side)) {
            noMoves = false;
            unMakeMove();
            break;
        }
        unMakeMove();
    }

    return noMoves;
}