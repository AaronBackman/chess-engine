#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "Constants.h"
#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, bool enPassant) {
    Move move = {from, to, promotion, castle, enPassant};

    return move;
}

bool squareOccupied(u64 board, int index) {
    u64 bitIndex = (1ULL << index);

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

u64 emptySquare(u64 board, int index) {
    return board & ~(1LLU << index);
}

u64 fillSquare(u64 board, int index) {
    return board | (1LLU << index);
}

bool canWhiteCastleShort(u64 meta) {
    return (meta & 1LLU) > 0;
}

bool canWhiteCastleLong(u64 meta) {
    return (meta & (1LLU << 1)) > 0;
}

bool canBlackCastleShort(u64 meta) {
    return (meta & (1LLU << 2)) > 0;
}

bool canBlackCastleLong(u64 meta) {
    return (meta & (1LLU << 3)) > 0;
}

u64 removeWhiteCastleShort(u64 meta) {
    return meta & ~(1LLU);
}

u64 removeWhiteCastleLong(u64 meta) {
    return meta & ~(1LLU << 1);
}

u64 removeBlackCastleShort(u64 meta) {
    return meta & ~(1LLU << 2);
}

u64 removeBlackCastleLong(u64 meta) {
    return meta &=~(1LLU << 3);
}

u64 setWhiteCastleShort(u64 meta) {
    return meta |= (1LLU);
}

u64 setWhiteCastleLong(u64 meta) {
    return meta |= (1LLU << 1);
}

u64 setBlackCastleShort(u64 meta) {
    return meta |= (1LLU << 2);
}

u64 setBlackCastleLong(u64 meta) {
    return meta |= (1LLU << 3);
}

bool isEnPassantAllowed(u64 meta) {
    return (meta & (1LLU << 4)) > 0;
}

u64 setEnPassantAllowed(u64 meta, bool enPassantAllowed) {
    if (enPassantAllowed) {
        meta |= (1LLU << 4);
    }
    else {
        meta &= ~(1LLU << 4);
    }

    return meta;
}

u64 setEnPassantSquare(u64 meta, u64 square) {
    meta &= ~(0b111111LLU << 5);
    meta |= (square << 5);

    return meta;
}

u64 getEnPassantSquare(u64 meta) {
    return (meta & (0b111111LLU << 5)) >> 5;
}

// 1 == white plays, -1 == black plays
int getSideToPlay(u64 meta) {
    if ((meta & (1LLU << 11)) > 0) {
        return 1;
    }
    else {
        return -1;
    }
}

// 1 == white plays, -1 == black plays
u64 setSideToPlay(u64 meta, int side) {
    if (side == 1) {
        // set 12th bit to 1
        meta |= (1LLU << 11);
    }
    else {
        // set 12th bit to 0
        meta &= ~(1LLU << 11);
    }

    return meta;
}

// parses a fen string into a gamestate
u64 *parseFen(u64 *gameState, char *fenStr) {
    int i;
    // fen starts from top left of the board (index 56 in our board representation)
    int index = 56;
    u64 whitePieces = 0LLU;
    u64 whitePawns = 0LLU;
    u64 whiteKnights = 0LLU;
    u64 whiteBishops = 0LLU;
    u64 whiteRooks = 0LLU;
    u64 whiteQueens = 0LLU;
    u64 whiteKings = 0LLU;

    u64 blackPieces = 0LLU;
    u64 blackPawns = 0LLU;
    u64 blackKnights = 0LLU;
    u64 blackBishops = 0LLU;
    u64 blackRooks = 0LLU;
    u64 blackQueens = 0LLU;
    u64 blackKings = 0LLU;

    u64 otherGameInfo = 0;

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

    for (i = 0; i < fenStrLength; i++) {
        char fenChar = fenStr[i];

        if (!isBoardFinished) {
            if (fenChar == 'p') {
                blackPawns = fillSquare(blackPawns, index);
                blackPieces = fillSquare(blackPieces, index);
            }
            else if (fenChar == 'n') {
                blackKnights = fillSquare(blackKnights, index);
                blackPieces = fillSquare(blackPieces, index);
            }
            else if (fenChar == 'b') {
                blackBishops = fillSquare(blackBishops, index);
                blackPieces = fillSquare(blackPieces, index);
            }
            else if (fenChar == 'r') {
                blackRooks = fillSquare(blackRooks, index);
                blackPieces = fillSquare(blackPieces, index);
            }
            else if (fenChar == 'q') {
                blackQueens = fillSquare(blackQueens, index);
                blackPieces = fillSquare(blackPieces, index);
            }
            else if (fenChar == 'k') {
                blackKings = fillSquare(blackKings, index);
                blackPieces = fillSquare(blackPieces, index);
            }

            else if (fenChar == 'P') {
                whitePawns = fillSquare(whitePawns, index);
                whitePieces = fillSquare(whitePieces, index);
            }
            else if (fenChar == 'N') {
                whiteKnights = fillSquare(whiteKnights, index);
                whitePieces = fillSquare(whitePieces, index);
            }
            else if (fenChar == 'B') {
                whiteBishops = fillSquare(whiteBishops, index);
                whitePieces = fillSquare(whitePieces, index);
            }
            else if (fenChar == 'R') {
                whiteRooks = fillSquare(whiteRooks, index);
                whitePieces = fillSquare(whitePieces, index);
            }
            else if (fenChar == 'Q') {
                whiteQueens = fillSquare(whiteQueens, index);
                whitePieces = fillSquare(whitePieces, index);
            }
            else if (fenChar == 'K') {
                whiteKings = fillSquare(whiteKings, index);
                whitePieces = fillSquare(whitePieces, index);
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
                otherGameInfo = setSideToPlay(otherGameInfo, 1);
            }
            // black to move
            else if (fenChar == 'b') {
                otherGameInfo = setSideToPlay(otherGameInfo, -1);
            }

            if (fenChar == ' ') {
                isSideFinished = true;
            }
        }
        else if (!isCastlingFinished) {
            // white castles kingside (short)
            if (fenChar == 'K') {
                otherGameInfo = setWhiteCastleShort(otherGameInfo);
            }
            // white castles queenside (long)
            else if (fenChar == 'Q') {
                otherGameInfo = setWhiteCastleLong(otherGameInfo);
            }
            // black castles kingside (short)
            else if (fenChar == 'k') {
                otherGameInfo = setBlackCastleShort(otherGameInfo);
            }
            // black castles queenside (long)
            else if (fenChar == 'q') {
                otherGameInfo = setBlackCastleLong(otherGameInfo);
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
                // column number is the next character, minus one to get zero-based index
                int row = fenStr[i + 1] - '0' - 1;
                // index used by fen is the square that is behind the moved pawn
                int enPassantIndex = row * 8 + column;
                // index used by this program is the location of the moved pawn
                int enPassantPieceIndex;

                printf("char: %c, column: %d, row: %d\n", fenChar, column, row);

                // white moved 2 squares with a pawn
                if (row == 2) {
                    enPassantPieceIndex = enPassantIndex + 8;
                }
                // black moved 2 squares with a pawn
                else {
                    enPassantPieceIndex = enPassantIndex - 8;
                }

                otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                otherGameInfo = setEnPassantSquare(otherGameInfo, enPassantPieceIndex);
            }
        }
    }

    gameState[0] = whitePieces;
    gameState[1] = whitePawns;
    gameState[2] = whiteKnights;
    gameState[3] = whiteBishops;
    gameState[4] = whiteRooks;
    gameState[5] = whiteQueens;
    gameState[6] = whiteKings;

    gameState[7] = blackPieces;
    gameState[8] = blackPawns;
    gameState[9] = blackKnights;
    gameState[10] = blackBishops;
    gameState[11] = blackRooks;
    gameState[12] = blackQueens;
    gameState[13] = blackKings;

    gameState[14] = otherGameInfo;

    return gameState;
}