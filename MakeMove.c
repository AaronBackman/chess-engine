#include <stdbool.h>
#include <stdio.h>

#include "Constants.h"
#include "Move.h"
#include "Utilities.h"

void make_move(Move move, bool permanent) {
    int from = move.from;
    int to = move.to;
    int promotion = move.promotion;
    int castle = move.castling;
    bool enPassant = move.enPassant;

    u64 *gameState = g_gameStateStack[g_root + g_ply];
    u64 *newGameState = g_gameStateStack[g_root + g_ply + 1];

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

    int side = get_side_to_play(otherGameInfo);

    if (permanent) {
        g_root++;
    }
    else {
        g_ply++;
    }

    // previous en passant possibility ends with this move
    otherGameInfo = set_enpassant_allowed(otherGameInfo, false);
    // switch side to play
    otherGameInfo = set_side_to_play(otherGameInfo, -side);
    
    // castling is handled separately because it is the only move that moves 2 pieces at the same time
    if (castle != 0) {
        // white moving
        if (side == 1) {
            // set white castling to 0
            otherGameInfo = remove_white_castle_short(otherGameInfo);
            otherGameInfo = remove_white_castle_long(otherGameInfo);

            // kingside
            if (castle == 1) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = empty_square(whiteRooks, 7);
                whitePieces = empty_square(whitePieces, 7);
                whiteRooks = fill_square(whiteRooks, 5);
                whitePieces = fill_square(whitePieces, 5);
            }

            // queenside
            else if (castle == 2) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = empty_square(whiteRooks, 0);
                whitePieces = empty_square(whitePieces, 0);
                whiteRooks = fill_square(whiteRooks, 3);
                whitePieces = fill_square(whitePieces, 3);
            }
        }

        // black moving
        else {
            // set black castling to 0
            otherGameInfo = remove_black_castle_short(otherGameInfo);
            otherGameInfo = remove_black_castle_long(otherGameInfo);

            // kingside
            if (castle == 3) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = empty_square(blackRooks, 63);
                blackPieces = empty_square(blackPieces, 63);
                blackRooks = fill_square(blackRooks, 61);
                blackPieces = fill_square(blackPieces, 61);

            }

            // queenside
            else if (castle == 4) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = empty_square(blackRooks, 56);
                blackPieces = empty_square(blackPieces, 56);
                blackRooks = fill_square(blackRooks, 59);
                blackPieces = fill_square(blackPieces, 59);
            }
        }
    }

    else if (enPassant) {
        u64 enPassantBit = SINGLE_BIT_LOOKUP[get_enpassant_square(otherGameInfo)];
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
        if (square_occupied(whitePieces, to)) {
            whitePieces = empty_square(whitePieces, to);

            if (square_occupied(whitePawns, to)) {
                whitePawns = empty_square(whitePawns, to);
            }

            else if (square_occupied(whiteKnights, to)) {
                whiteKnights = empty_square(whiteKnights, to);
            }
            
            else if (square_occupied(whiteBishops, to)) {
                whiteBishops = empty_square(whiteBishops, to);
            }
            
            else if (square_occupied(whiteRooks, to)) {
                whiteRooks = empty_square(whiteRooks, to);

                // cant castle anymore if the castling took is taken
                if (to == 0 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                }
                // cant castle anymore if the castling took is taken
                else if (to == 7 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                }
            }
            
            else if (square_occupied(whiteQueens, to)) {
                whiteQueens = empty_square(whiteQueens, to);
            }
            
            else {
                whiteKings = empty_square(whiteKings, to);
            }
        }


        else if (square_occupied(blackPieces, to)) {
            blackPieces = empty_square(blackPieces, to);

            if (square_occupied(blackPawns, to)) {
                blackPawns = empty_square(blackPawns, to);
            }

            else if (square_occupied(blackKnights, to)) {
                blackKnights = empty_square(blackKnights, to);
            }
            
            else if (square_occupied(blackBishops, to)) {
                blackBishops = empty_square(blackBishops, to);
            }
            
            else if (square_occupied(blackRooks, to)) {
                blackRooks = empty_square(blackRooks, to);

                // cant castle anymore if the castling took is taken
                if (to == 56 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                }
                // cant castle anymore if the castling took is taken
                else if (to == 63 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                }
            }
            
            else if (square_occupied(blackQueens, to)) {
                blackQueens = empty_square(blackQueens, to);
            }
            
            else {
                blackKings = empty_square(blackKings, to);
            }
        }

        if (side == 1) {
            whitePieces ^= movePattern;

            if (square_occupied(whitePawns, from)) {
                whitePawns ^= movePattern;

                // moved 2 squares, en passant possible on next move
                if (to - from == 16) {
                    otherGameInfo = set_enpassant_allowed(otherGameInfo, true);
                    otherGameInfo = set_enpassant_square(otherGameInfo, to);
                }
            }

            else if (square_occupied(whiteKnights, from)) {
                whiteKnights ^= movePattern;
            }
            
            else if (square_occupied(whiteBishops, from)) {
                whiteBishops ^= movePattern;
            }
            
            else if (square_occupied(whiteRooks, from)) {
                whiteRooks ^= movePattern;

                // remove castling rights, if rook moves
                if (from == 0 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                }

                if (from == 7 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                }
            }
            
            else if (square_occupied(whiteQueens, from)) {
                whiteQueens ^= movePattern;
            }
            
            else {
                whiteKings ^= movePattern;

                // remove both castling rights, if king moves
                if (from == 4 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                }

                if (from == 4 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                }
            }
        }


        else {
            blackPieces ^= movePattern;

            if (square_occupied(blackPawns, from)) {
                blackPawns ^= movePattern;

                // moved 2 squares, en passant possible on next move
                if (to - from == -16) {
                    otherGameInfo = set_enpassant_allowed(otherGameInfo, true);
                    otherGameInfo = set_enpassant_square(otherGameInfo, to);
                }
            }

            else if (square_occupied(blackKnights, from)) {
                blackKnights ^= movePattern;
            }
            
            else if (square_occupied(blackBishops, from)) {
                blackBishops ^= movePattern;
            }
            
            else if (square_occupied(blackRooks, from)) {
                blackRooks ^= movePattern;

                // remove castling rights, if rook moves
                if (from == 56 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                }

                if (from == 63 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                }
            }
            
            else if (square_occupied(blackQueens, from)) {
                blackQueens ^= movePattern;
            }
            
            else {
                blackKings ^= movePattern;

                // remove both castling rights, if king moves
                if (from == 60 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                }

                if (from == 60 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                }
            }
        }


        // handle promotion of pawn
        if (promotion != 0) {
            if (side == 1) {
                whitePawns = empty_square(whitePawns, to);

                // promote to knight
                if (promotion == 1) {
                    whiteKnights = fill_square(whiteKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    whiteBishops = fill_square(whiteBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    whiteRooks = fill_square(whiteRooks, to);
                }
                // promote to queen
                else {
                    whiteQueens = fill_square(whiteQueens, to);
                }
            }
            else {
                blackPawns = empty_square(blackPawns, to);

                // promote to knight
                if (promotion == 1) {
                    blackKnights = fill_square(blackKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    blackBishops = fill_square(blackBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    blackRooks = fill_square(blackRooks, to);
                }
                // promote to queen
                else {
                    blackQueens = fill_square(blackQueens, to);
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

void unmake_move() {
    g_ply--;
}