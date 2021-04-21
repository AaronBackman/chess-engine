#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "Utilities.h"
#include "MakeMove.h"
#include "Constants.h"

// parses a fen string into a gamestate
void parseUciPosition(u64 *gameState, char *fenStr) {
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

    u64 otherGameInfo = 0LLU;

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

                    otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                    otherGameInfo = setEnPassantSquare(otherGameInfo, enPassantPieceIndex);
                }
            }
        }
    }
    else if (strncmp(fenStr, "position startpos", 17) == 0) {
        char nextChar = fenStr[17];

        whitePieces = 0b0000000000000000000000000000000000000000000000001111111111111111LLU;
        whitePawns = 0b0000000000000000000000000000000000000000000000001111111100000000LLU;
        whiteKnights = 0b0000000000000000000000000000000000000000000000000000000001000010LLU;
        whiteBishops = 0b0000000000000000000000000000000000000000000000000000000000100100LLU;
        whiteRooks = 0b0000000000000000000000000000000000000000000000000000000010000001LLU;
        whiteQueens = 0b0000000000000000000000000000000000000000000000000000000000001000LLU;
        whiteKings = 0b0000000000000000000000000000000000000000000000000000000000010000LLU;

        blackPieces = 0b1111111111111111000000000000000000000000000000000000000000000000LLU;
        blackPawns = 0b0000000011111111000000000000000000000000000000000000000000000000LLU;
        blackKnights = 0b0100001000000000000000000000000000000000000000000000000000000000LLU;
        blackBishops = 0b0010010000000000000000000000000000000000000000000000000000000000LLU;
        blackRooks = 0b1000000100000000000000000000000000000000000000000000000000000000LLU;
        blackQueens = 0b0000100000000000000000000000000000000000000000000000000000000000LLU;
        blackKings = 0b0001000000000000000000000000000000000000000000000000000000000000LLU;

        otherGameInfo = setSideToPlay(otherGameInfo, 1);
        otherGameInfo = setWhiteCastleShort(otherGameInfo);
        otherGameInfo = setWhiteCastleLong(otherGameInfo);
        otherGameInfo = setBlackCastleShort(otherGameInfo);
        otherGameInfo = setBlackCastleLong(otherGameInfo);

        if (nextChar == ' ' && fenStr[17 + 1] == 'm' && fenStr[17 + 2] == 'o' && fenStr[17 + 3] == 'v' && fenStr[17 + 4] == 'e' && fenStr[17 + 5] == 's') {
            movesAfterPosition = true;
            moveStartIndex = 17 + 5 + 2;
        }
    }

    //printf("gamestate outside: %d\n", GAME_STATE_STACK_POINTER);

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

            gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
            u64 *newGameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER + 1];

            whitePieces = gameState[0];
            whitePawns = gameState[1];
            whiteKnights = gameState[2];
            whiteBishops = gameState[3];
            whiteRooks = gameState[4];
            whiteQueens = gameState[5];
            whiteKings = gameState[6];

            blackPieces = gameState[7];
            blackPawns = gameState[8];
            blackKnights = gameState[9];
            blackBishops = gameState[10];
            blackRooks = gameState[11];
            blackQueens = gameState[12];
            blackKings = gameState[13];

            otherGameInfo = gameState[14];

            enPassantSquare = getEnPassantSquare(otherGameInfo);
            enPassantAllowed = isEnPassantAllowed(otherGameInfo);

            side = getSideToPlay(otherGameInfo);

            //printf("gamestate: %d\n", GAME_STATE_STACK_POINTER);
            //printf("index: %d\n", i);

            // handle promotions
            if (fenStr[i + 4] == 'n') {
                nextMove = createMove(from, to, 1, 0, 0);
                i += 5;
            }
            else if (fenStr[i + 4] == 'b') {
                nextMove = createMove(from, to, 2, 0, 0);
                i += 5;
            }
            else if (fenStr[i + 4] == 'r') {
                nextMove = createMove(from, to, 3, 0, 0);
                i += 5;
            }
            else if (fenStr[i + 4] == 'q') {
                nextMove = createMove(from, to, 4, 0, 0);
                i += 5;
            }
            // handle white short castling
            else if (from == 4 && to == 6 && squareOccupied(whiteKings, from)) {
                nextMove = createMove(from, to, 0, 1, 0);
                i += 4;
            }
            // handle white long castling
            else if (from == 4 && to == 2 && squareOccupied(whiteKings, from)) {
                nextMove = createMove(from, to, 0, 2, 0);
                i += 4;
            }
            // handle black short castling
            else if (from == 60 && to == 62 && squareOccupied(blackKings, from)) {
                nextMove = createMove(from, to, 0, 3, 0);
                i += 4;
            }
            // handle black long castling
            else if (from == 60 && to == 58 && squareOccupied(blackKings, from)) {
                nextMove = createMove(from, to, 0, 4, 0);
                i += 4;
            }
            // handle en passant
            else if ((side == 1 && (to == enPassantSquare + 8) && enPassantAllowed && squareOccupied(whitePawns, from)) || (side == -1 && (to == enPassantSquare - 8) && enPassantAllowed && squareOccupied(blackPawns, from))) {
                nextMove = createMove(from, to, 0, 0, 1);
                i += 4;
            }
            // a normal move
            else {
                nextMove = createMove(from, to, 0, 0, 0);
                i += 4;
            }

            //printf("%d, %d, %d, %d, %d\n", nextMove.from, nextMove.to, nextMove.promotion, nextMove.castling, nextMove.enPassant);
            //printf ("enpassant allowed: %d, enpassant square: %d\n", enPassantAllowed, enPassantSquare);
            
            makeMove(nextMove);

            // in uci newline terminates commands
            if (fenStr[i] == '\n') {
              break;
            }
            else {
              i++;
            }
        }
    }
}
