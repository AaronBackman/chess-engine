#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "Constants.h"
#include "Move.h"
#include "Utilities.h"

int addPawnMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    const int row = boardIndex / 8;
    const int column = boardIndex % 8;

    // white
    if (side == 1) {
        if (!squareOccupied(whitePieces, boardIndex + 8) && !squareOccupied(blackPieces, boardIndex + 8)) {
            movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 0, 0, 0);
            moveIndex++;

            if (row == 6) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 4, 0, 0);
                moveIndex++;
            }

            if ((row == 1) && (!squareOccupied(whitePieces, boardIndex + 16) && !squareOccupied(blackPieces, boardIndex + 16))) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 16, 0, 0, 0);
                moveIndex++;
            }
        }

        if (row == 6) {
            // take top left piece and promote
            if (column != 0 && squareOccupied(blackPieces, boardIndex + 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 4, 0, 0);
                moveIndex++;
            }

            // take top right piece and promote
            if (column != 7 && squareOccupied(blackPieces, boardIndex + 9)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 4, 0, 0);
                moveIndex++;
            }
        }

        else {
            // take top left piece
            if (column != 0 && squareOccupied(blackPieces, boardIndex + 7)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 0, 0, 0);
                moveIndex++;
            }

            // take top right piece
            if (column != 7 && squareOccupied(blackPieces, boardIndex + 9)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // black
    else if (side == -1) {
        if (!squareOccupied(blackPieces, boardIndex - 8) && !squareOccupied(whitePieces, boardIndex - 8)) {
            movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 0, 0, 0);
            moveIndex++;

            if (row == 1) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 4, 0, 0);
                moveIndex++;
            }

            if ((row == 6) && (!squareOccupied(blackPieces, boardIndex - 16) && !squareOccupied(whitePieces, boardIndex - 16))) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 16, 0, 0, 0);
                moveIndex++;
            }
        }

        if (row == 1) {
            // take bottom left piece and promote
            if (column != 0 && squareOccupied(whitePieces, boardIndex - 9)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 4, 0, 0);
                moveIndex++;
            }

            // take bottom right piece and promote
            if (column != 7 && squareOccupied(whitePieces, boardIndex - 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 4, 0, 0);
                moveIndex++;
            }
        }

        else {
            // take bottom left piece
            if (column != 0 && squareOccupied(whitePieces, boardIndex - 9)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 0, 0, 0);
                moveIndex++;
            }

            // take bottom right piece
            if (column != 7 && squareOccupied(whitePieces, boardIndex - 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    return moveIndex;
}

int addKnightMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    destinationSquare = boardIndex + i * 8 + j;

                    // cant move outside the board
                    if (destinationSquare > 63 || destinationSquare < 0) continue;

                    if (!squareOccupied(whitePieces, destinationSquare)) {
                        movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                        moveIndex++;
                    }
                }
            }
        }
    }

    // black
    else if (side == -1) {
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    destinationSquare = boardIndex + i * 8 + j;

                    // cant move outside the board
                    if (destinationSquare > 63 || destinationSquare < 0) continue;

                    if (!squareOccupied(blackPieces, destinationSquare)) {
                        movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                        moveIndex++;
                    }
                }
            }
        }
    }

    return moveIndex;
}

int addBishopMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        j = 1;
        // top right diagonal
        for (i = 9; boardIndex + i < 64 && boardIndex % 8 + j < 8; i += 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // top left diagonal
        for (i = 7; boardIndex + i < 64 && boardIndex % 8 + j >= 0; i += 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }

        j = 1;
        // bottom right diagonal
        for (i = -7; boardIndex + i >= 0 && boardIndex % 8 + j < 8; i -= 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // bottom left diagonal
        for (i = -9; boardIndex + i >= 0 && boardIndex % 8 + j >= 0; i -= 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }
    }

    // black
    else if (side == -1) {
        j = 1;
        // top right diagonal
        for (i = 9; boardIndex + i < 64 && boardIndex % 8 + j < 8; i += 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // top left diagonal
        for (i = 7; boardIndex + i < 64 && boardIndex % 8 + j >= 0; i += 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }

        j = 1;
        // bottom right diagonal
        for (i = -7; boardIndex + i >= 0 && boardIndex % 8 + j < 8; i -= 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // bottom left diagonal
        for (i = -9; boardIndex + i >= 0 && boardIndex % 8 + j >= 0; i -= 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }
    }

    return moveIndex;
}

int addRookMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        // top
        for (i = 8; boardIndex + i < 64; i += 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // right
        for (i = 1; boardIndex % 8 + i < 8; i += 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // bottom
        for (i = -8; boardIndex + i >= 0; i -= 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // left
        for (i = -1; boardIndex % 8 + i >= 0; i -= 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // black
    else if (side == -1) {
        // top
        for (i = 8; boardIndex + i < 64; i += 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // right
        for (i = 1; boardIndex % 8 + i < 8; i += 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // bottom
        for (i = -8; boardIndex + i >= 0; i -= 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // left
        for (i = -1; boardIndex % 8 + i >= 0; i -= 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    return moveIndex;
}

int addKingMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                // would not moving anywhere
                if (i == 0 && j == 0) continue;

                destinationSquare = boardIndex + i * 8 + j;

                // outside the board
                if (destinationSquare < 0 || destinationSquare > 63) continue;

                if (!squareOccupied(whitePieces, destinationSquare)) {
                    movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                    moveIndex++;
                }
             }
        }
    }

    // black
    else if (side == -1) {
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                // would not moving anywhere
                if (i == 0 && j == 0) continue;

                destinationSquare = boardIndex + i * 8 + j;

                // outside the board
                if (destinationSquare < 0 || destinationSquare > 63) continue;

                if (!squareOccupied(blackPieces, destinationSquare)) {
                    movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                    moveIndex++;
                }
             }
        }
    }

    return moveIndex;
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
int generateMoves(u64 *gameState, Move *movesArr, int side) {
    int i;
    int isKingInDanger;

    // next free index in moves array
    int moveIndex = 0;
    u64 bitIndex;


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

    // white
    if (side == 1) {
        for (i = 0; i < BOARD_SIZE; i++) {
            bitIndex = 1ULL << i;

            // check if square doesnt have a white piece
            if (!squareOccupied(whitePieces, i)) continue;

            // remove piece to see if it causes king to be in check and do extra checking later
            whitePieces &= ~bitIndex;
            // is white king in danger
            if (kingInDanger(gameState, side)) {
                isKingInDanger = 1;
            } else {
                isKingInDanger = 0;
            }
            // add the piece back
            whitePieces |= bitIndex;

            // check if square has a white pawn
            if (squareOccupied(whitePawns, i)) {
                moveIndex = addPawnMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, pawn gen\n", moveIndex);
            }

            else if (squareOccupied(whiteKnights, i)) {
                moveIndex = addKnightMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, knight gen\n", moveIndex);
            }

            else if (squareOccupied(whiteBishops, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, bishop gen\n", moveIndex);
            }

            else if (squareOccupied(whiteRooks, i)) {
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, rook gen\n", moveIndex);
            }

            else if (squareOccupied(whiteQueens, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, queen gen\n", moveIndex);
            }
            
            else if (squareOccupied(whiteKings, i)) {
                moveIndex = addKingMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, king gen\n", moveIndex);
            }
        }
    }
    // black
    else {
        for (i = 0; i < BOARD_SIZE; i++) {
            // check if square doesnt have a black piece
            if (!blackPieces & (1ULL << i)) continue;

            // remove piece to see if it causes king to be in check and do extra checking later
            blackPieces &= ~bitIndex;
            // is white king in danger
            if (kingInDanger(gameState, side)) {
                isKingInDanger = 1;
            } else {
                isKingInDanger = 0;
            }
            // add the piece back
            blackPieces |= bitIndex;

            // check if square has a white pawn
            if (squareOccupied(blackPawns, i)) {
                moveIndex = addPawnMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackKnights, i)) {
                moveIndex = addKnightMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackBishops, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackRooks, i)) {
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackQueens, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
            }
            
            else if (squareOccupied(blackKings, i)) {
                moveIndex = addKingMoves(gameState, i, movesArr, moveIndex, side);
            }
        }
    }

    //printf("end gen\n\n");

    return moveIndex;
}