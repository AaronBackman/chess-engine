#include <stdbool.h>
#include <stdio.h>

#include "Constants.h"
#include "Move.h"
#include "Utilities.h"

void makeMove(Move move) {
    int from = move.from;
    int to = move.to;
    int promotion = move.promotion;
    int castle = move.castling;
    bool enPassant = move.enPassant;

    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 *newGameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER + 1];

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
    u64 movePattern = SINGLE_BIT_LOOKUP[from] | SINGLE_BIT_LOOKUP[to];

    int side = getSideToPlay(otherGameInfo);

    GAME_STATE_STACK_POINTER++;

    // previous en passant possibility ends with this move
    otherGameInfo = setEnPassantAllowed(otherGameInfo, false);
    // switch side to play
    otherGameInfo = setSideToPlay(otherGameInfo, -side);
    
    // castling is handled separately because it is the only move that moves 2 pieces at the same time
    if (castle != 0) {
        // white moving
        if (side == 1) {
            // set white castling to 0
            otherGameInfo = removeWhiteCastleShort(otherGameInfo);
            otherGameInfo = removeWhiteCastleLong(otherGameInfo);

            // kingside
            if (castle == 1) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = emptySquare(whiteRooks, 7);
                whitePieces = emptySquare(whitePieces, 7);
                whiteRooks = fillSquare(whiteRooks, 5);
                whitePieces = fillSquare(whitePieces, 5);
            }

            // queenside
            else if (castle == 2) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = emptySquare(whiteRooks, 0);
                whitePieces = emptySquare(whitePieces, 0);
                whiteRooks = fillSquare(whiteRooks, 3);
                whitePieces = fillSquare(whitePieces, 3);
            }
        }

        // black moving
        else {
            // set black castling to 0
            otherGameInfo = removeBlackCastleShort(otherGameInfo);
            otherGameInfo = removeBlackCastleLong(otherGameInfo);

            // kingside
            if (castle == 3) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = emptySquare(blackRooks, 63);
                blackPieces = emptySquare(blackPieces, 63);
                blackRooks = fillSquare(blackRooks, 61);
                blackPieces = fillSquare(blackPieces, 61);

            }

            // queenside
            else if (castle == 4) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = emptySquare(blackRooks, 56);
                blackPieces = emptySquare(blackPieces, 56);
                blackRooks = fillSquare(blackRooks, 59);
                blackPieces = fillSquare(blackPieces, 59);
            }
        }
    }

    else if (enPassant) {
        u64 enPassantBit = SINGLE_BIT_LOOKUP[getEnPassantSquare(otherGameInfo)];
        if (side == 1) {
            whitePieces ^= movePattern;
            whitePawns ^= movePattern;

            blackPieces ^= enPassantBit;
            blackPawns ^= enPassantBit;
        }
        else {
            blackPieces ^= movePattern;
            blackPawns ^= movePattern;

            whitePieces ^= enPassantBit;
            whitePawns ^= enPassantBit;
        }
    }

    // a normal move or promotion
    else {
        if (squareOccupied(whitePieces, to)) {
            whitePieces = emptySquare(whitePieces, to);

            if (squareOccupied(whitePawns, to)) {
                whitePawns = emptySquare(whitePawns, to);
            }

            else if (squareOccupied(whiteKnights, to)) {
                whiteKnights = emptySquare(whiteKnights, to);
            }
            
            else if (squareOccupied(whiteBishops, to)) {
                whiteBishops = emptySquare(whiteBishops, to);
            }
            
            else if (squareOccupied(whiteRooks, to)) {
                whiteRooks = emptySquare(whiteRooks, to);

                // cant castle anymore if the castling took is taken
                if (to == 0 && canWhiteCastleLong(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleLong(otherGameInfo);
                }
                // cant castle anymore if the castling took is taken
                else if (to == 7 && canWhiteCastleShort(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleShort(otherGameInfo);
                }
            }
            
            else if (squareOccupied(whiteQueens, to)) {
                whiteQueens = emptySquare(whiteQueens, to);
            }
            
            else {
                whiteKings = emptySquare(whiteKings, to);
            }
        }


        else if (squareOccupied(blackPieces, to)) {
            blackPieces = emptySquare(blackPieces, to);

            if (squareOccupied(blackPawns, to)) {
                blackPawns = emptySquare(blackPawns, to);
            }

            else if (squareOccupied(blackKnights, to)) {
                blackKnights = emptySquare(blackKnights, to);
            }
            
            else if (squareOccupied(blackBishops, to)) {
                blackBishops = emptySquare(blackBishops, to);
            }
            
            else if (squareOccupied(blackRooks, to)) {
                blackRooks = emptySquare(blackRooks, to);

                // cant castle anymore if the castling took is taken
                if (to == 56 && canBlackCastleLong(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleLong(otherGameInfo);
                }
                // cant castle anymore if the castling took is taken
                else if (to == 63 && canBlackCastleShort(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleShort(otherGameInfo);
                }
            }
            
            else if (squareOccupied(blackQueens, to)) {
                blackQueens = emptySquare(blackQueens, to);
            }
            
            else {
                blackKings = emptySquare(blackKings, to);
            }
        }

        if (side == 1) {
            whitePieces ^= movePattern;

            if (squareOccupied(whitePawns, from)) {
                whitePawns ^= movePattern;

                // moved 2 squares, en passant possible on next move
                if (to - from == 16) {
                    otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                    otherGameInfo = setEnPassantSquare(otherGameInfo, to);
                }
            }

            else if (squareOccupied(whiteKnights, from)) {
                whiteKnights ^= movePattern;
            }
            
            else if (squareOccupied(whiteBishops, from)) {
                whiteBishops ^= movePattern;
            }
            
            else if (squareOccupied(whiteRooks, from)) {
                whiteRooks ^= movePattern;

                // remove castling rights, if rook moves
                if (from == 0 && canWhiteCastleLong(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleLong(otherGameInfo);
                }

                if (from == 7 && canWhiteCastleShort(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleShort(otherGameInfo);
                }
            }
            
            else if (squareOccupied(whiteQueens, from)) {
                whiteQueens ^= movePattern;
            }
            
            else {
                whiteKings ^= movePattern;

                // remove both castling rights, if king moves
                if (from == 4 && canWhiteCastleLong(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleLong(otherGameInfo);
                }

                if (from == 4 && canWhiteCastleShort(otherGameInfo)) {
                    otherGameInfo = removeWhiteCastleShort(otherGameInfo);
                }
            }
        }


        else {
            blackPieces ^= movePattern;

            if (squareOccupied(blackPawns, from)) {
                blackPawns ^= movePattern;

                // moved 2 squares, en passant possible on next move
                if (to - from == -16) {
                    otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                    otherGameInfo = setEnPassantSquare(otherGameInfo, to);
                }
            }

            else if (squareOccupied(blackKnights, from)) {
                blackKnights ^= movePattern;
            }
            
            else if (squareOccupied(blackBishops, from)) {
                blackBishops ^= movePattern;
            }
            
            else if (squareOccupied(blackRooks, from)) {
                blackRooks ^= movePattern;

                // remove castling rights, if rook moves
                if (from == 56 && canBlackCastleLong(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleLong(otherGameInfo);
                }

                if (from == 63 && canBlackCastleShort(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleShort(otherGameInfo);
                }
            }
            
            else if (squareOccupied(blackQueens, from)) {
                blackQueens ^= movePattern;
            }
            
            else {
                blackKings ^= movePattern;

                // remove both castling rights, if king moves
                if (from == 60 && canBlackCastleLong(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleLong(otherGameInfo);
                }

                if (from == 60 && canBlackCastleShort(otherGameInfo)) {
                    otherGameInfo = removeBlackCastleShort(otherGameInfo);
                }
            }
        }


        // handle promotion of pawn
        if (promotion != 0) {
            if (side == 1) {
                whitePawns = emptySquare(whitePawns, to);

                // promote to knight
                if (promotion == 1) {
                    whiteKnights = fillSquare(whiteKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    whiteBishops = fillSquare(whiteBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    whiteRooks = fillSquare(whiteRooks, to);
                }
                // promote to queen
                else {
                    whiteQueens = fillSquare(whiteQueens, to);
                }
            }
            else {
                blackPawns = emptySquare(blackPawns, to);

                // promote to knight
                if (promotion == 1) {
                    blackKnights = fillSquare(blackKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    blackBishops = fillSquare(blackBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    blackRooks = fillSquare(blackRooks, to);
                }
                // promote to queen
                else {
                    blackQueens = fillSquare(blackQueens, to);
                }
            }
        }
    }

    newGameState[0] = whitePieces;
    newGameState[1] = whitePawns;
    newGameState[2] = whiteKnights;
    newGameState[3] = whiteBishops;
    newGameState[4] = whiteRooks;
    newGameState[5] = whiteQueens;
    newGameState[6] = whiteKings;

    newGameState[7] = blackPieces;
    newGameState[8] = blackPawns;
    newGameState[9] = blackKnights;
    newGameState[10] = blackBishops;
    newGameState[11] = blackRooks;
    newGameState[12] = blackQueens;
    newGameState[13] = blackKings;

    newGameState[14] = otherGameInfo;
}

void unMakeMove() {
    GAME_STATE_STACK_POINTER--;
}