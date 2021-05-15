#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "Utilities.h"
#include "Board.h"
#include "Init.h"
#include "MakeMove.h"
#include "Constants.h"

// parses a fen string into a gamestate
void parse_uci_position(char *fenStr) {
    int i;
    // fen starts from top left of the board (index 56 in our board representation)
    int index = 56;
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = 0LLU;
    u64 blackPieces = 0LLU;

    u64 pawns = 0LLU;
    u64 knights = 0LLU;
    u64 bishops = 0LLU;
    u64 rooks = 0LLU;
    u64 queens = 0LLU;
    u64 kings = 0LLU;

    u64 otherGameInfo = 0LLU;

    u64 zobristKey = 0LLU;

    int fenStrLength = strlen(fenStr);

    // is the board part in fen finished
    bool isBoardFinished = false;

    // is the side part in fen finished
    bool isSideFinished = false;

    // is the castling part in fen finished
    bool isCastlingFinished = false;

    // is the en passant part in fen finished
    bool isEnPassantFinished = false;

    // is the halfmove part (50-move rule) in fen finished (TODO)
    bool isHalfMoveFinished = false;

    // is the fullmove part in fen finished (TODO)
    bool isFullMoveFinished = false;

    bool movesAfterPosition = false;
    int moveStartIndex;

    if (strncmp(fenStr, "position fen ", 13) == 0) {
        for (i = 13; i < fenStrLength; i++) {
            char fenChar = fenStr[i];

            if (fenChar == '\n') {
                break;
            }
            else if (fenChar == ' ' && fenStr[i + 1] == 'm' && fenStr[i + 2] == 'o' && fenStr[i + 3] == 'v' && fenStr[i + 4] == 'e' && fenStr[i + 5] == 's') {
                movesAfterPosition = true;
                moveStartIndex = i + 5 + 2;
                break;
            }

            if (!isBoardFinished) {
                if (fenChar == 'p') {
                    pawns = fill_square(pawns, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_PAWN_ZOBRIST[index];
                }
                else if (fenChar == 'n') {
                    knights = fill_square(knights, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_KNIGHT_ZOBRIST[index];
                }
                else if (fenChar == 'b') {
                    bishops = fill_square(bishops, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_BISHOP_ZOBRIST[index];
                }
                else if (fenChar == 'r') {
                    rooks = fill_square(rooks, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_ROOK_ZOBRIST[index];
                }
                else if (fenChar == 'q') {
                    queens = fill_square(queens, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_QUEEN_ZOBRIST[index];
                }
                else if (fenChar == 'k') {
                    kings = fill_square(kings, index);
                    blackPieces = fill_square(blackPieces, index);

                    zobristKey ^= BLACK_KING_ZOBRIST[index];
                }

                else if (fenChar == 'P') {
                    pawns = fill_square(pawns, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_PAWN_ZOBRIST[index];
                }
                else if (fenChar == 'N') {
                    knights = fill_square(knights, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_KNIGHT_ZOBRIST[index];
                }
                else if (fenChar == 'B') {
                    bishops = fill_square(bishops, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_BISHOP_ZOBRIST[index];
                }
                else if (fenChar == 'R') {
                    rooks = fill_square(rooks, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_ROOK_ZOBRIST[index];
                }
                else if (fenChar == 'Q') {
                    queens = fill_square(queens, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_QUEEN_ZOBRIST[index];
                }
                else if (fenChar == 'K') {
                    kings = fill_square(kings, index);
                    whitePieces = fill_square(whitePieces, index);

                    zobristKey ^= WHITE_KING_ZOBRIST[index];
                }

                if (fenChar == '/') {
                    index -= 16;
                }

                // means there are that many spaces in the chess board
                else if (isdigit(fenChar)) {
                    // adds the character after it is converted to an integer
                    index += fenChar - '0';
                }
                // stop putting pieces after the first space
                else if (fenChar == ' ') {
                    isBoardFinished = true;
                }
                // is a chess piece
                else {
                    index++;
                }
            }
            else if (!isSideFinished) {
                // white to move
                if (fenChar == 'w') {
                    otherGameInfo = set_side_to_play(otherGameInfo, 1);
                }
                // black to move
                else if (fenChar == 'b') {
                    otherGameInfo = set_side_to_play(otherGameInfo, -1);
                    zobristKey ^= BLACK_TO_MOVE_ZOBRIST;
                }

                if (fenChar == ' ') {
                    isSideFinished = true;
                }
            }
            else if (!isCastlingFinished) {
                // white castles kingside (short)
                if (fenChar == 'K') {
                    otherGameInfo = set_white_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[0];
                }
                // white castles queenside (long)
                else if (fenChar == 'Q') {
                    otherGameInfo = set_white_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[1];
                }
                // black castles kingside (short)
                else if (fenChar == 'k') {
                    otherGameInfo = set_black_castle_short(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[2];
                }
                // black castles queenside (long)
                else if (fenChar == 'q') {
                    otherGameInfo = set_black_castle_long(otherGameInfo);
                    zobristKey ^= CASTLING_ZOBRIST[3];
                }

                if (fenChar == ' ') {
                    isCastlingFinished = true;
                }
            }
            else if (!isEnPassantFinished) {
                // no en passant
                if (fenChar == '-') {
                    continue;
                }
                // second character in a move
                else if (isdigit(fenChar) != 0) {
                    continue;
                }
                else if (fenChar == ' ') {
                    isEnPassantFinished = true;
                }
                // first character in a move (eg. e3)
                else {
                    // convert character to column index: example: d -> 3
                    int column = fenChar - 'a';
                    zobristKey ^= ENPASSANT_FILE_ZOBRIST[column];
                    // column number is the next character
                    int row = fenStr[i + 1] - '1';
                    // index used by fen is the square that is behind the moved pawn
                    int enPassantIndex = row * 8 + column;
                    // index used by this program is the location of the moved pawn
                    int enPassantPieceIndex;

                    //printf("char: %c, column: %d, row: %d\n", fenChar, column, row);

                    // white moved 2 squares with a pawn
                    if (row == 2) {
                        enPassantPieceIndex = enPassantIndex + 8;
                    }
                    // black moved 2 squares with a pawn
                    else {
                        enPassantPieceIndex = enPassantIndex - 8;
                    }

                    otherGameInfo = set_enpassant_allowed(otherGameInfo, true);
                    otherGameInfo = set_enpassant_square(otherGameInfo, enPassantPieceIndex);
                }
            }
        }
    }
    else if (strncmp(fenStr, "position startpos", 17) == 0) {
        char nextChar = fenStr[17];

        whitePieces = 0b0000000000000000000000000000000000000000000000001111111111111111LLU;
        blackPieces = 0b1111111111111111000000000000000000000000000000000000000000000000LLU;
        
        pawns =   0b0000000011111111000000000000000000000000000000001111111100000000LLU;
        knights = 0b0100001000000000000000000000000000000000000000000000000001000010LLU;
        bishops = 0b0010010000000000000000000000000000000000000000000000000000100100LLU;
        rooks =   0b1000000100000000000000000000000000000000000000000000000010000001LLU;
        queens =  0b0000100000000000000000000000000000000000000000000000000000001000LLU;
        kings =   0b0001000000000000000000000000000000000000000000000000000000010000LLU;

        zobristKey ^= WHITE_PAWN_ZOBRIST[8];
        zobristKey ^= WHITE_PAWN_ZOBRIST[9];
        zobristKey ^= WHITE_PAWN_ZOBRIST[10];
        zobristKey ^= WHITE_PAWN_ZOBRIST[11];
        zobristKey ^= WHITE_PAWN_ZOBRIST[12];
        zobristKey ^= WHITE_PAWN_ZOBRIST[13];
        zobristKey ^= WHITE_PAWN_ZOBRIST[14];
        zobristKey ^= WHITE_PAWN_ZOBRIST[15];

        zobristKey ^= WHITE_KNIGHT_ZOBRIST[1];
        zobristKey ^= WHITE_KNIGHT_ZOBRIST[6];

        zobristKey ^= WHITE_BISHOP_ZOBRIST[2];
        zobristKey ^= WHITE_BISHOP_ZOBRIST[5];

        zobristKey ^= WHITE_ROOK_ZOBRIST[0];
        zobristKey ^= WHITE_ROOK_ZOBRIST[7];

        zobristKey ^= WHITE_QUEEN_ZOBRIST[3];
        zobristKey ^= WHITE_KING_ZOBRIST[4];

        zobristKey ^= BLACK_PAWN_ZOBRIST[48];
        zobristKey ^= BLACK_PAWN_ZOBRIST[49];
        zobristKey ^= BLACK_PAWN_ZOBRIST[50];
        zobristKey ^= BLACK_PAWN_ZOBRIST[51];
        zobristKey ^= BLACK_PAWN_ZOBRIST[52];
        zobristKey ^= BLACK_PAWN_ZOBRIST[53];
        zobristKey ^= BLACK_PAWN_ZOBRIST[54];
        zobristKey ^= BLACK_PAWN_ZOBRIST[55];

        zobristKey ^= BLACK_KNIGHT_ZOBRIST[57];
        zobristKey ^= BLACK_KNIGHT_ZOBRIST[62];

        zobristKey ^= BLACK_BISHOP_ZOBRIST[58];
        zobristKey ^= BLACK_BISHOP_ZOBRIST[61];

        zobristKey ^= BLACK_ROOK_ZOBRIST[56];
        zobristKey ^= BLACK_ROOK_ZOBRIST[63];

        zobristKey ^= BLACK_QUEEN_ZOBRIST[59];
        zobristKey ^= BLACK_KING_ZOBRIST[60];

        zobristKey ^= CASTLING_ZOBRIST[0];
        zobristKey ^= CASTLING_ZOBRIST[1];
        zobristKey ^= CASTLING_ZOBRIST[2];
        zobristKey ^= CASTLING_ZOBRIST[3];

        otherGameInfo = set_side_to_play(otherGameInfo, 1);
        otherGameInfo = set_white_castle_short(otherGameInfo);
        otherGameInfo = set_white_castle_long(otherGameInfo);
        otherGameInfo = set_black_castle_short(otherGameInfo);
        otherGameInfo = set_black_castle_long(otherGameInfo);

        if (nextChar == ' ' && fenStr[17 + 1] == 'm' && fenStr[17 + 2] == 'o' && fenStr[17 + 3] == 'v' && fenStr[17 + 4] == 'e' && fenStr[17 + 5] == 's') {
            movesAfterPosition = true;
            moveStartIndex = 17 + 5 + 2;
        }
    }

    //printf("gamestate outside: %d\n", GAME_STATE_STACK_POINTER);

    gameState.whitePieces = whitePieces;
    gameState.blackPieces = blackPieces;
    
    gameState.pawns = pawns;
    gameState.knights = knights;
    gameState.bishops = bishops;
    gameState.rooks = rooks;
    gameState.queens = queens;
    gameState.kings = kings;

    gameState.meta = otherGameInfo;

    g_gameStateStack[g_root + g_ply] = gameState;

    g_zobristStack[0] = zobristKey;

    //printf("zobrist after parse: ");
    //print_in_binary(zobristKey);

    if (movesAfterPosition) {
        int i = moveStartIndex;
        while (true) {
            Move nextMove;
            // calculate the indexes based on column and row
            int from = (fenStr[i] - 'a') + 8 * (fenStr[i + 1] - '1');
            int to = (fenStr[i + 2] - 'a') + 8 * (fenStr[i + 3] - '1');
            int enPassantSquare;
            bool enPassantAllowed;
            int side;

            gameState = g_gameStateStack[g_root + g_ply];

            whitePieces = gameState.whitePieces;
            blackPieces = gameState.blackPieces;

            pawns = gameState.pawns;
            knights = gameState.knights;
            bishops = gameState.bishops;
            rooks = gameState.rooks;
            queens = gameState.queens;
            kings = gameState.kings;

            otherGameInfo = gameState.meta;

            enPassantSquare = get_enpassant_square(otherGameInfo);
            enPassantAllowed = is_enpassant_allowed(otherGameInfo);

            side = get_side_to_play(otherGameInfo);

            //printf("gamestate: %d\n", GAME_STATE_STACK_POINTER);
            //printf("index: %d\n", i);

            // handle promotions
            if (fenStr[i + 4] == 'n') {
                if (square_occupied(whitePieces | blackPieces, to)) {
                    nextMove = create_move(from, to, KNIGHT_PROMOTION_CAPTURE_MOVE);
                }
                else {
                    nextMove = create_move(from, to, KNIGHT_PROMOTION_MOVE);
                }
                i += 5;
            }
            else if (fenStr[i + 4] == 'b') {
                if (square_occupied(whitePieces | blackPieces, to)) {
                    nextMove = create_move(from, to, BISHOP_PROMOTION_CAPTURE_MOVE);
                }
                else {
                    nextMove = create_move(from, to, BISHOP_PROMOTION_MOVE);
                }
                i += 5;
            }
            else if (fenStr[i + 4] == 'r') {
                if (square_occupied(whitePieces | blackPieces, to)) {
                    nextMove = create_move(from, to, ROOK_PROMOTION_CAPTURE_MOVE);
                }
                else {
                    nextMove = create_move(from, to, ROOK_PROMOTION_MOVE);
                }
                i += 5;
            }
            else if (fenStr[i + 4] == 'q') {
                if (square_occupied(whitePieces | blackPieces, to)) {
                    nextMove = create_move(from, to, QUEEN_PROMOTION_CAPTURE_MOVE);
                }
                else {
                    nextMove = create_move(from, to, QUEEN_PROMOTION_MOVE);
                }
                i += 5;
            }
            // handle white short castling
            else if (from == 4 && to == 6 && square_occupied(kings, from)) {
                nextMove = create_move(from, to, KING_CASTLE_MOVE);
                i += 4;
            }
            // handle white long castling
            else if (from == 4 && to == 2 && square_occupied(kings, from)) {
                nextMove = create_move(from, to, QUEEN_CASTLE_MOVE);
                i += 4;
            }
            // handle black short castling
            else if (from == 60 && to == 62 && square_occupied(kings, from)) {
                nextMove = create_move(from, to, KING_CASTLE_MOVE);
                i += 4;
            }
            // handle black long castling
            else if (from == 60 && to == 58 && square_occupied(kings, from)) {
                nextMove = create_move(from, to, QUEEN_CASTLE_MOVE);
                i += 4;
            }
            // handle en passant
            else if ((side == 1 && (to == enPassantSquare + 8) && enPassantAllowed && square_occupied(pawns & whitePieces, from)) || (side == -1 && (to == enPassantSquare - 8) && enPassantAllowed && square_occupied(pawns & blackPieces, from))) {
                nextMove = create_move(from, to, EP_CAPTURE_MOVE);
                i += 4;
            }
            // a normal move
            else {
                if (square_occupied(whitePieces | blackPieces, to)) {
                    nextMove = create_move(from, to, CAPTURE_MOVE);
                }
                else {
                    nextMove = create_move(from, to, QUIET_MOVE);
                }
                i += 4;
            }

            //printf("%d, %d, %d, %d, %d\n", nextMove.from, nextMove.to, nextMove.promotion, nextMove.castling, nextMove.enPassant);
            //printf ("enpassant allowed: %d, enpassant square: %d\n", enPassantAllowed, enPassantSquare);
            
            make_move(nextMove, true);

            //printf("zobrist: %llu\n", g_zobristStack[g_root + g_ply]);

            // in uci newline terminates commands
            if (fenStr[i] == '\n') {
              break;
            }
            else {
              i++;
            }
        }
    }

    //printf("zobrist: %llu\n", g_zobristStack[g_root + g_ply]);
}
