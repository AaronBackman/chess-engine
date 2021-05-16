#include <stdbool.h>
#include <stdio.h>

#include "Constants.h"
#include "Init.h"
#include "Move.h"
#include "Board.h"
#include "Utilities.h"

void make_move(Move move, bool permanent) {
    u16 code = move.code;
    u16 from = move.from;
    u16 to = move.to;

    Board gameState = g_gameStateStack[g_root + g_ply];
    Board newGameState = g_gameStateStack[g_root + g_ply + 1];
    u64 zobristKey = g_zobristStack[g_root + g_ply];

    u64 whitePieces = gameState.whitePieces;
    u64 whitePawns = gameState.pawns & whitePieces;
    u64 whiteKnights = gameState.knights & whitePieces;
    u64 whiteBishops = gameState.bishops & whitePieces;
    u64 whiteRooks = gameState.rooks & whitePieces;
    u64 whiteQueens = gameState.queens & whitePieces;
    u64 whiteKings = gameState.kings & whitePieces;

    u64 blackPieces = gameState.blackPieces;
    u64 blackPawns = gameState.pawns & blackPieces;
    u64 blackKnights = gameState.knights & blackPieces;
    u64 blackBishops = gameState.bishops & blackPieces;
    u64 blackRooks = gameState.rooks & blackPieces;
    u64 blackQueens = gameState.queens & blackPieces;
    u64 blackKings = gameState.kings & blackPieces;

    u64 otherGameInfo = gameState.meta;
    u64 movePattern = SINGLE_BIT_LOOKUP[from] | SINGLE_BIT_LOOKUP[to];

    int side = get_side_to_play(otherGameInfo);

    if (permanent) {
        g_root++;
    }
    else {
        g_ply++;
    }

    if (is_enpassant_allowed(otherGameInfo)) {
        // previous en passant possibility ends with this move
        otherGameInfo = set_enpassant_allowed(otherGameInfo, false);
        zobristKey ^= ENPASSANT_FILE_ZOBRIST[get_enpassant_square(otherGameInfo) % 8];
    }
    // switch side to play
    otherGameInfo = set_side_to_play(otherGameInfo, -side);
    zobristKey ^= BLACK_TO_MOVE_ZOBRIST;
    
    // castling is handled separately because it is the only move that moves 2 pieces at the same time
    if (code == KING_CASTLE_MOVE || code == QUEEN_CASTLE_MOVE) {
        // white moving
        if (side == WHITE) {
            // set white castling to 0
            if (can_white_castle_short(otherGameInfo)) {
                otherGameInfo = remove_white_castle_short(otherGameInfo);
                zobristKey ^= CASTLING_ZOBRIST[0];
            }
            if (can_white_castle_long(otherGameInfo)) {
                otherGameInfo = remove_white_castle_long(otherGameInfo);
                zobristKey ^= CASTLING_ZOBRIST[1];
            }

            // kingside
            if (code == KING_CASTLE_MOVE) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = empty_square(whiteRooks, 7);
                whitePieces = empty_square(whitePieces, 7);
                whiteRooks = fill_square(whiteRooks, 5);
                whitePieces = fill_square(whitePieces, 5);

                zobristKey ^= WHITE_KING_ZOBRIST[from];
                zobristKey ^= WHITE_KING_ZOBRIST[to];
                zobristKey ^= WHITE_ROOK_ZOBRIST[7];
                zobristKey ^= WHITE_ROOK_ZOBRIST[5];
            }

            // queenside
            else if (code == QUEEN_CASTLE_MOVE) {
                whiteKings ^= movePattern;
                whitePieces ^= movePattern;

                whiteRooks = empty_square(whiteRooks, 0);
                whitePieces = empty_square(whitePieces, 0);
                whiteRooks = fill_square(whiteRooks, 3);
                whitePieces = fill_square(whitePieces, 3);

                zobristKey ^= WHITE_KING_ZOBRIST[from];
                zobristKey ^= WHITE_KING_ZOBRIST[to];
                zobristKey ^= WHITE_ROOK_ZOBRIST[0];
                zobristKey ^= WHITE_ROOK_ZOBRIST[3];
            }
        }

        // black moving
        else {
            // set black castling to 0
            if (can_black_castle_short(otherGameInfo)) {
                otherGameInfo = remove_black_castle_short(otherGameInfo);
                zobristKey ^= CASTLING_ZOBRIST[2];
            }
            if (can_black_castle_long(otherGameInfo)) {
                otherGameInfo = remove_black_castle_long(otherGameInfo);
                zobristKey ^= CASTLING_ZOBRIST[3];
            }

            // kingside
            if (code == KING_CASTLE_MOVE) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = empty_square(blackRooks, 63);
                blackPieces = empty_square(blackPieces, 63);
                blackRooks = fill_square(blackRooks, 61);
                blackPieces = fill_square(blackPieces, 61);

                zobristKey ^= BLACK_KING_ZOBRIST[from];
                zobristKey ^= BLACK_KING_ZOBRIST[to];
                zobristKey ^= BLACK_ROOK_ZOBRIST[63];
                zobristKey ^= BLACK_ROOK_ZOBRIST[61];

            }

            // queenside
            else if (code == QUEEN_CASTLE_MOVE) {
                blackKings ^= movePattern;
                blackPieces ^= movePattern;

                blackRooks = empty_square(blackRooks, 56);
                blackPieces = empty_square(blackPieces, 56);
                blackRooks = fill_square(blackRooks, 59);
                blackPieces = fill_square(blackPieces, 59);

                zobristKey ^= BLACK_KING_ZOBRIST[from];
                zobristKey ^= BLACK_KING_ZOBRIST[to];
                zobristKey ^= BLACK_ROOK_ZOBRIST[56];
                zobristKey ^= BLACK_ROOK_ZOBRIST[59];
            }
        }
    }

    else if (code == EP_CAPTURE_MOVE) {
        int enpassantSquare = get_enpassant_square(otherGameInfo);
        u64 enPassantBit = SINGLE_BIT_LOOKUP[enpassantSquare];
        
        if (side == WHITE) {
            whitePieces ^= movePattern;
            whitePawns ^= movePattern;

            zobristKey ^= WHITE_PAWN_ZOBRIST[from];
            zobristKey ^= WHITE_PAWN_ZOBRIST[to];

            blackPieces ^= enPassantBit;
            blackPawns ^= enPassantBit;

            zobristKey ^= BLACK_PAWN_ZOBRIST[enpassantSquare];
        }
        else {
            blackPieces ^= movePattern;
            blackPawns ^= movePattern;

            zobristKey ^= BLACK_PAWN_ZOBRIST[from];
            zobristKey ^= BLACK_PAWN_ZOBRIST[to];

            whitePieces ^= enPassantBit;
            whitePawns ^= enPassantBit;

            zobristKey ^= WHITE_PAWN_ZOBRIST[enpassantSquare];
        }
    }
    else if (code == NULL_MOVE) {
        // do nothing
    }

    // a normal move or promotion
    else {
        if ((side == BLACK) && (code == CAPTURE_MOVE || code >= KNIGHT_PROMOTION_CAPTURE_MOVE)) {
            whitePieces = empty_square(whitePieces, to);

            if (square_occupied(whitePawns, to)) {
                whitePawns = empty_square(whitePawns, to);
                zobristKey ^= WHITE_PAWN_ZOBRIST[to];
            }

            else if (square_occupied(whiteKnights, to)) {
                whiteKnights = empty_square(whiteKnights, to);
                zobristKey ^= WHITE_KNIGHT_ZOBRIST[to];
            }
            
            else if (square_occupied(whiteBishops, to)) {
                whiteBishops = empty_square(whiteBishops, to);
                zobristKey ^= WHITE_BISHOP_ZOBRIST[to];
            }
            
            else if (square_occupied(whiteRooks, to)) {
                whiteRooks = empty_square(whiteRooks, to);
                zobristKey ^= WHITE_ROOK_ZOBRIST[to];

                // cant castle anymore if the castling took is taken
                if (to == 0 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[1];
                }
                // cant castle anymore if the castling took is taken
                else if (to == 7 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[0];
                }
            }
            
            else if (square_occupied(whiteQueens, to)) {
                whiteQueens = empty_square(whiteQueens, to);
                zobristKey ^= WHITE_QUEEN_ZOBRIST[to];
            }
            
            else {
                whiteKings = empty_square(whiteKings, to);
                zobristKey ^= WHITE_KING_ZOBRIST[to];
            }
        }


        else if ((side == WHITE) && (code == CAPTURE_MOVE || code >= KNIGHT_PROMOTION_CAPTURE_MOVE)) {
            blackPieces = empty_square(blackPieces, to);

            if (square_occupied(blackPawns, to)) {
                blackPawns = empty_square(blackPawns, to);
                zobristKey ^= BLACK_PAWN_ZOBRIST[to];
            }

            else if (square_occupied(blackKnights, to)) {
                blackKnights = empty_square(blackKnights, to);
                zobristKey ^= BLACK_KNIGHT_ZOBRIST[to];
            }
            
            else if (square_occupied(blackBishops, to)) {
                blackBishops = empty_square(blackBishops, to);
                zobristKey ^= BLACK_BISHOP_ZOBRIST[to];
            }
            
            else if (square_occupied(blackRooks, to)) {
                blackRooks = empty_square(blackRooks, to);
                zobristKey ^= BLACK_ROOK_ZOBRIST[to];

                // cant castle anymore if the castling took is taken
                if (to == 56 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[3];
                }
                // cant castle anymore if the castling took is taken
                else if (to == 63 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[2];
                }
            }
            
            else if (square_occupied(blackQueens, to)) {
                blackQueens = empty_square(blackQueens, to);
                zobristKey ^= BLACK_QUEEN_ZOBRIST[to];
            }
            
            else {
                blackKings = empty_square(blackKings, to);
                zobristKey ^= BLACK_KING_ZOBRIST[to];
            }
        }

        if (side == WHITE) {
            whitePieces ^= movePattern;

            if (square_occupied(whitePawns, from)) {
                whitePawns ^= movePattern;
                zobristKey ^= WHITE_PAWN_ZOBRIST[from];
                zobristKey ^= WHITE_PAWN_ZOBRIST[to];

                // moved 2 squares, en passant possible on next move
                if (to - from == 16) {
                    otherGameInfo = set_enpassant_allowed(otherGameInfo, true);
                    otherGameInfo = set_enpassant_square(otherGameInfo, to);
                    zobristKey ^= ENPASSANT_FILE_ZOBRIST[to % 8];
                }
            }

            else if (square_occupied(whiteKnights, from)) {
                whiteKnights ^= movePattern;
                zobristKey ^= WHITE_KNIGHT_ZOBRIST[from];
                zobristKey ^= WHITE_KNIGHT_ZOBRIST[to];
            }
            
            else if (square_occupied(whiteBishops, from)) {
                whiteBishops ^= movePattern;
                zobristKey ^= WHITE_BISHOP_ZOBRIST[from];
                zobristKey ^= WHITE_BISHOP_ZOBRIST[to];
            }
            
            else if (square_occupied(whiteRooks, from)) {
                whiteRooks ^= movePattern;
                zobristKey ^= WHITE_ROOK_ZOBRIST[from];
                zobristKey ^= WHITE_ROOK_ZOBRIST[to];

                // remove castling rights, if rook moves
                if (from == 0 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[1];
                }

                if (from == 7 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[0];
                }
            }
            
            else if (square_occupied(whiteQueens, from)) {
                whiteQueens ^= movePattern;
                zobristKey ^= WHITE_QUEEN_ZOBRIST[from];
                zobristKey ^= WHITE_QUEEN_ZOBRIST[to];
            }
            
            else {
                whiteKings ^= movePattern;
                zobristKey ^= WHITE_KING_ZOBRIST[from];
                zobristKey ^= WHITE_KING_ZOBRIST[to];

                // remove both castling rights, if king moves
                if (from == 4 && can_white_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[1];
                }

                if (from == 4 && can_white_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_white_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[0];
                }
            }
        }


        else {
            blackPieces ^= movePattern;

            if (square_occupied(blackPawns, from)) {
                blackPawns ^= movePattern;
                zobristKey ^= BLACK_PAWN_ZOBRIST[from];
                zobristKey ^= BLACK_PAWN_ZOBRIST[to];

                // moved 2 squares, en passant possible on next move
                if (to - from == -16) {
                    otherGameInfo = set_enpassant_allowed(otherGameInfo, true);
                    otherGameInfo = set_enpassant_square(otherGameInfo, to);
                    zobristKey ^= ENPASSANT_FILE_ZOBRIST[to % 8];
                }
            }

            else if (square_occupied(blackKnights, from)) {
                blackKnights ^= movePattern;
                zobristKey ^= BLACK_KNIGHT_ZOBRIST[from];
                zobristKey ^= BLACK_KNIGHT_ZOBRIST[to];
            }
            
            else if (square_occupied(blackBishops, from)) {
                blackBishops ^= movePattern;
                zobristKey ^= BLACK_BISHOP_ZOBRIST[from];
                zobristKey ^= BLACK_BISHOP_ZOBRIST[to];
            }
            
            else if (square_occupied(blackRooks, from)) {
                blackRooks ^= movePattern;
                zobristKey ^= BLACK_ROOK_ZOBRIST[from];
                zobristKey ^= BLACK_ROOK_ZOBRIST[to];

                // remove castling rights, if rook moves
                if (from == 56 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[3];
                }

                if (from == 63 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[2];
                }
            }
            
            else if (square_occupied(blackQueens, from)) {
                blackQueens ^= movePattern;
                zobristKey ^= BLACK_QUEEN_ZOBRIST[from];
                zobristKey ^= BLACK_QUEEN_ZOBRIST[to];
            }
            
            else {
                blackKings ^= movePattern;
                zobristKey ^= BLACK_KING_ZOBRIST[from];
                zobristKey ^= BLACK_KING_ZOBRIST[to];

                // remove both castling rights, if king moves
                if (from == 60 && can_black_castle_long(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[3];
                }

                if (from == 60 && can_black_castle_short(otherGameInfo)) {
                    otherGameInfo = remove_black_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[2];
                }
            }
        }


        // handle promotion of pawn
        if (code >= KNIGHT_PROMOTION_MOVE) {
            if (side == 1) {
                whitePawns = empty_square(whitePawns, to);
                zobristKey ^= WHITE_PAWN_ZOBRIST[to];

                // promote to knight
                if (code == KNIGHT_PROMOTION_MOVE || code == KNIGHT_PROMOTION_CAPTURE_MOVE) {
                    whiteKnights = fill_square(whiteKnights, to);
                    zobristKey ^= WHITE_KNIGHT_ZOBRIST[to];
                }
                // promote to bishop
                else if (code == BISHOP_PROMOTION_MOVE || code == BISHOP_PROMOTION_CAPTURE_MOVE) {
                    whiteBishops = fill_square(whiteBishops, to);
                    zobristKey ^= WHITE_BISHOP_ZOBRIST[to];
                }
                // promote to rook
                else if (code == ROOK_PROMOTION_MOVE || code == ROOK_PROMOTION_CAPTURE_MOVE) {
                    whiteRooks = fill_square(whiteRooks, to);
                    zobristKey ^= WHITE_ROOK_ZOBRIST[to];
                }
                // promote to queen
                else {
                    whiteQueens = fill_square(whiteQueens, to);
                    zobristKey ^= WHITE_QUEEN_ZOBRIST[to];
                }
            }
            else {
                blackPawns = empty_square(blackPawns, to);
                zobristKey ^= BLACK_PAWN_ZOBRIST[to];

                // promote to knight
                if (code == KNIGHT_PROMOTION_MOVE || code == KNIGHT_PROMOTION_CAPTURE_MOVE) {
                    blackKnights = fill_square(blackKnights, to);
                    zobristKey ^= BLACK_KNIGHT_ZOBRIST[to];
                }
                // promote to bishop
                else if (code == BISHOP_PROMOTION_MOVE || code == BISHOP_PROMOTION_CAPTURE_MOVE) {
                    blackBishops = fill_square(blackBishops, to);
                    zobristKey ^= BLACK_BISHOP_ZOBRIST[to];
                }
                // promote to rook
                else if (code == ROOK_PROMOTION_MOVE || code == ROOK_PROMOTION_CAPTURE_MOVE) {
                    blackRooks = fill_square(blackRooks, to);
                    zobristKey ^= BLACK_ROOK_ZOBRIST[to];
                }
                // promote to queen
                else {
                    blackQueens = fill_square(blackQueens, to);
                    zobristKey ^= BLACK_QUEEN_ZOBRIST[to];
                }
            }
        }
    }

    newGameState.whitePieces = whitePieces;
    newGameState.blackPieces = blackPieces;

    newGameState.pawns = whitePawns | blackPawns;
    newGameState.knights = whiteKnights | blackKnights;
    newGameState.bishops = whiteBishops | blackBishops;
    newGameState.rooks = whiteRooks | blackRooks;
    newGameState.queens = whiteQueens | blackQueens;
    newGameState.kings = whiteKings | blackKings;

    newGameState.meta = otherGameInfo;

    g_gameStateStack[g_root + g_ply] = newGameState;

    g_zobristStack[g_root + g_ply] = zobristKey;
}

void unmake_move() {
    g_ply--;
}