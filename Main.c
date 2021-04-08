#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "Move.h"
#include "Utilities.h"
#include "MoveGeneration.h"
#include "Constants.h"

Move legalMoves[MAX_MOVES];
Move checkThreatMoves[MAX_MOVES];
Move checkMateMoves[MAX_MOVES];

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

    // previous en passant possibility ends with this move
    otherGameInfo = setEnPassantAllowed(otherGameInfo, false);
    
    // castling is handled separately because it is the only move that moves 2 pieces at the same time
    if (castle != 0) {
        // white moving
        if (squareOccupied(whiteKings, move.from)) {
            // set white castling to 0
            otherGameInfo = removeWhiteCastleShort(otherGameInfo);
            otherGameInfo = removeWhiteCastleLong(otherGameInfo);

            // kingside
            if (castle == 1) {
                whiteKings = emptySquare(whiteKings, 4);
                whitePieces = emptySquare(whitePieces, 4);
                whiteKings = fillSquare(whiteKings, 6);
                whitePieces = fillSquare(whitePieces, 6);

                whiteRooks = emptySquare(whiteRooks, 7);
                whitePieces = emptySquare(whitePieces, 7);
                whiteRooks = fillSquare(whiteRooks, 5);
                whitePieces = fillSquare(whitePieces, 5);
            }

            // queenside
            else if (castle == 2) {
                whiteKings = emptySquare(whiteKings, 4);
                whitePieces = emptySquare(whitePieces, 4);
                whiteKings = fillSquare(whiteKings, 2);
                whitePieces = fillSquare(whitePieces, 2);

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
                blackKings = emptySquare(blackKings, 60);
                blackPieces = emptySquare(blackPieces, 60);
                blackKings = fillSquare(blackKings, 62);
                blackPieces = fillSquare(blackPieces, 62);

                blackRooks = emptySquare(blackRooks, 63);
                blackPieces = emptySquare(blackPieces, 63);
                blackRooks = fillSquare(blackRooks, 61);
                blackPieces = fillSquare(blackPieces, 61);

            }

            // queenside
            else if (castle == 4) {
                blackKings = emptySquare(blackKings, 60);
                blackPieces = emptySquare(blackPieces, 60);
                blackKings = fillSquare(blackKings, 58);
                blackPieces = fillSquare(blackPieces, 58);

                blackRooks = emptySquare(blackRooks, 56);
                blackPieces = emptySquare(blackPieces, 56);
                blackRooks = fillSquare(blackRooks, 59);
                blackPieces = emptySquare(blackPieces, 59);
            }
        }
    }

    else if (enPassant) {
        int enPassantSquare = getEnPassantSquare(otherGameInfo);
        if (squareOccupied(whitePawns, from)) {
            whitePieces = emptySquare(whitePieces, from);
            whitePawns = emptySquare(whitePawns, from);

            whitePieces = fillSquare(whitePieces, to);
            whitePawns = fillSquare(whitePawns, to);

            blackPieces = emptySquare(blackPieces, enPassantSquare);
            blackPawns = emptySquare(blackPawns, enPassantSquare);
        }
        else if (squareOccupied(blackPawns, from)) {
            blackPieces = emptySquare(blackPieces, from);
            blackPawns = emptySquare(blackPawns, from);

            blackPieces = fillSquare(blackPieces, to);
            blackPawns = fillSquare(blackPawns, to);

            whitePieces = emptySquare(whitePieces, enPassantSquare);
            whitePawns = emptySquare(whitePawns, enPassantSquare);
        }
        else {
            printf("enpassant error\n");
        }
    }

    // a normal move or promotion
    else {
        if (squareOccupied(whitePieces, to)) {
            whitePieces = emptySquare(whitePieces, to);
        }

        if (squareOccupied(whitePawns, to)) {
            whitePawns = emptySquare(whitePawns, to);
        }

        if (squareOccupied(whiteKnights, to)) {
            whiteKnights = emptySquare(whiteKnights, to);
        }
        
        if (squareOccupied(whiteBishops, to)) {
            whiteBishops = emptySquare(whiteBishops, to);
        }
        
        if (squareOccupied(whiteRooks, to)) {
            whiteRooks = emptySquare(whiteRooks, to);
        }
        
        if (squareOccupied(whiteQueens, to)) {
            whiteQueens = emptySquare(whiteQueens, to);
        }
        
        if (squareOccupied(whiteKings, to)) {
            whiteKings = emptySquare(whiteKings, to);
        }



        if (squareOccupied(blackPieces, to)) {
            blackPieces = emptySquare(blackPieces, to);
        }

        if (squareOccupied(blackPawns, to)) {
            blackPawns = emptySquare(blackPawns, to);
        }

        if (squareOccupied(blackKnights, to)) {
            blackKnights = emptySquare(blackKnights, to);
        }
        
        if (squareOccupied(blackBishops, to)) {
            blackBishops = emptySquare(blackBishops, to);
        }
        
        if (squareOccupied(blackRooks, to)) {
            blackRooks = emptySquare(blackRooks, to);
        }
        
        if (squareOccupied(blackQueens, to)) {
            blackQueens = emptySquare(blackQueens, to);
        }
        
        if (squareOccupied(blackKings, to)) {
            blackKings = emptySquare(blackKings, to);
        }



        if (squareOccupied(whitePieces, from)) {
            whitePieces = emptySquare(whitePieces, from);
            whitePieces = fillSquare(whitePieces, to);
        }

        if (squareOccupied(whitePawns, from)) {
            whitePawns = emptySquare(whitePawns, from);
            whitePawns = fillSquare(whitePawns, to);

            // moved 2 squares, en passant possible on next move
            if (to - from == 16) {
                otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                otherGameInfo = setEnPassantSquare(otherGameInfo, to);
            }
        }

        if (squareOccupied(whiteKnights, from)) {
            whiteKnights = emptySquare(whiteKnights, from);
            whiteKnights = fillSquare(whiteKnights, to);
        }
        
        if (squareOccupied(whiteBishops, from)) {
            whiteBishops = emptySquare(whiteBishops, from);
            whiteBishops = fillSquare(whiteBishops, to);
        }
        
        if (squareOccupied(whiteRooks, from)) {
            whiteRooks = emptySquare(whiteRooks, from);
            whiteRooks = fillSquare(whiteRooks, to);
        }
        
        if (squareOccupied(whiteQueens, from)) {
            whiteQueens = emptySquare(whiteQueens, from);
            whiteQueens = fillSquare(whiteQueens, to);
        }
        
        if (squareOccupied(whiteKings, from)) {
            whiteKings = emptySquare(whiteKings, from);
            whiteKings = fillSquare(whiteKings, to);
        }



        if (squareOccupied(blackPieces, from)) {
            blackPieces = emptySquare(blackPieces, from);
            blackPieces = fillSquare(blackPieces, to);
        }

        if (squareOccupied(blackPawns, from)) {
            blackPawns = emptySquare(blackPawns, from);
            blackPawns = fillSquare(blackPawns, to);

            // moved 2 squares, en passant possible on next move
            if (to - from == -16) {
                otherGameInfo = setEnPassantAllowed(otherGameInfo, true);
                otherGameInfo = setEnPassantSquare(otherGameInfo, to);
            }
        }

        if (squareOccupied(blackKnights, from)) {
            blackKnights = emptySquare(blackKnights, from);
            blackKnights = fillSquare(blackKnights, to);
        }
        
        if (squareOccupied(blackBishops, from)) {
            blackBishops = emptySquare(blackBishops, from);
            blackBishops = fillSquare(blackBishops, to);
        }
        
        if (squareOccupied(blackRooks, from)) {
            blackRooks = emptySquare(blackRooks, from);
            blackRooks = fillSquare(blackRooks, to);
        }
        
        if (squareOccupied(blackQueens, from)) {
            blackQueens = emptySquare(blackQueens, from);
            blackQueens = fillSquare(blackQueens, to);
        }
        
        if (squareOccupied(blackKings, from)) {
            blackKings = emptySquare(blackKings, from);
            blackKings = fillSquare(blackKings, to);
        }

        // handle promotion of pawn
        if (promotion != 0) {
            if (squareOccupied(whitePawns, to)) {
                printf("promotion: %d, from: %d, to: %d\n", promotion, from, to);
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
                else if (promotion == 4) {
                    whiteQueens = fillSquare(whiteQueens, to);
                }

                else {
                    printf("promotion something went wrong\n");
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
                else if (promotion == 4) {
                    blackQueens = fillSquare(blackQueens, to);
                }

                else {
                    printf("promotion something went wrong");
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


// checks if king can be taken
bool checkIfCheckThreat(int side) {
    int i;
    int moveCount;
    Move generatedMove;
    u64 *gameState;
    u64 whiteKings;
    u64 blackKings;

    gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    whiteKings = gameState[6];
    blackKings = gameState[13];
    // generate moves for opponent side and check if any could take your king in the square you are moving (t0)
    moveCount = generateMoves(checkThreatMoves, -side);
    for (i = 0; i < moveCount; i++) {
        generatedMove = checkThreatMoves[i];

        if (side == 1 && squareOccupied(whiteKings, generatedMove.to)) {
            printf("attack from: %d, and king in: %d\n", generatedMove.from, generatedMove.to);
            return true;
        }
        else if (side == -1 && squareOccupied(blackKings, generatedMove.to)) {
            printf("attack from: %d, and king in: %d\n", generatedMove.from, generatedMove.to);
            return true;
        }
    }

    return false;
}

// filters out pseudo-legal moves, basically notices checkmates and draws by no moves
bool checkIfNoLegalMoves(int side) {
    int moveCount;
    int i;

    moveCount = generateMoves(checkMateMoves, side);

    // no possible pseudo-legal moves -> no legal moves either
    if (moveCount == 0) return true;

    bool noMoves = true;
    for (i = 0; i < moveCount; i++) {
        makeMove(checkMateMoves[i]);
        GAME_STATE_STACK_POINTER++;
        if (!checkIfCheckThreat(side)) {
            noMoves = false;
            GAME_STATE_STACK_POINTER--;
            break;
        }
        GAME_STATE_STACK_POINTER--;
    }

    return noMoves;
}

void printBoard() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    //printf("stackpointer: %d", GAME_STATE_STACK_POINTER);

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
    
    int i = 56;
    

    while (i >= 0 && i < 64) {
        printf(" ");

        if (!squareOccupied(whitePieces, i) && !squareOccupied(blackPieces, i)) {
            printf("__");
        }

        if (squareOccupied(whitePieces, i)) printf("+");
        if (squareOccupied(blackPieces, i)) printf("#");

        if (squareOccupied(whitePawns, i)) printf("P");

        if (squareOccupied(whiteBishops, i)) printf("B");

        if (squareOccupied(whiteKnights, i)) printf("N");

        if (squareOccupied(whiteRooks, i)) printf("R");

        if (squareOccupied(whiteQueens, i)) printf("Q");

        if (squareOccupied(whiteKings, i)) printf("K");

        if (squareOccupied(blackPawns, i)) printf("p");

        if (squareOccupied(blackBishops, i)) printf("b");

        if (squareOccupied(blackKnights, i)) printf("n");

        if (squareOccupied(blackRooks, i)) printf("r");

        if (squareOccupied(blackQueens, i)) printf("q");

        if (squareOccupied(blackKings, i)) printf("k");

        printf(" ");

        //printf("%d", i);

        i++;

        if (i % 8 == 0) {
            i -= 16;
            printf("\n");
        }
    }

    printf("\n\n\n");
}

void gameLoop() {
    int side = 1;
    Move bestMove;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    int i = 0;

    printBoard();

    while (true) {
        int inputFrom;
        int inputTo;
        int from;
        int to;

        int legalMoveCount;
        int i;
        Move legalMove;
        bool moveIsLegal;
        Move selectedMove;

        legalMoveCount = generateMoves(legalMoves, side);

        if (checkIfNoLegalMoves(side)) {
            if (checkIfCheckThreat(side)) {
                printf("checkmate\n");
            }
            else {
                printf("draw\n");
            }
            return;
        }

        printf("from:");
        scanf("%d", &inputFrom);
        printf("to:");
        scanf("%d", &inputTo);

        // translate inputs to coordinates
        from = inputFrom / 10 - 1 + (inputFrom % 10 - 1) * 8;
        to = inputTo / 10 - 1 + (inputTo % 10 - 1) * 8;

        moveIsLegal = false;
        for (i = 0; i < legalMoveCount; i++) {
            legalMove = legalMoves[i];

            if (legalMove.promotion != 0 && legalMove.promotion != 4) continue;

            if (legalMove.from != from) {
                continue;
            }

            if (legalMove.to != to) {
                continue;
            }

            moveIsLegal = true;
            selectedMove = legalMove;
            break;
        }

        if (!moveIsLegal) {
            printf("move is not legal\n");
            continue;
        }

        if (legalMoveCount == 0) {
            printf("draw\n");
            return;
        }

        makeMove(selectedMove);
        GAME_STATE_STACK_POINTER++;
        if (checkIfCheckThreat(side)) {
            printf("move is not legal, you are in check\n");
            GAME_STATE_STACK_POINTER--;
            continue;
        }

        printf("side: %d   from: %d,   to: %d\n", side, from, to);

        printBoard();

        // switch side after turn is completed
        side *= -1;

        i++;
    }
}


void main(void) {
    GAME_STATE_STACK_POINTER++;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    // all white pieces
    gameState[0] = 0b0000000000000000000000000000000000000000000000001111111111111111LLU;
    // white pawns
    gameState[1] = 0b0000000000000000000000000000000000000000000000001111111100000000LLU;
    // white knights
    gameState[2] = 0b0000000000000000000000000000000000000000000000000000000001000010LLU;
    // white bishops
    gameState[3] = 0b0000000000000000000000000000000000000000000000000000000000100100LLU;
    // white rooks
    gameState[4] = 0b0000000000000000000000000000000000000000000000000000000010000001LLU;
    // white queens
    gameState[5] = 0b0000000000000000000000000000000000000000000000000000000000001000LLU;
    // white king
    gameState[6] = 0b0000000000000000000000000000000000000000000000000000000000010000LLU;

    // all black pieces
    gameState[7] = 0b1111111111111111000000000000000000000000000000000000000000000000LLU;
    // black pawns
    gameState[8] = 0b0000000011111111000000000000000000000000000000000000000000000000LLU;
    // black knights
    gameState[9] = 0b0100001000000000000000000000000000000000000000000000000000000000LLU;
    // black bishops
    gameState[10] = 0b0010010000000000000000000000000000000000000000000000000000000000LLU;
    // black rooks
    gameState[11] = 0b1000000100000000000000000000000000000000000000000000000000000000LLU;
    // black queens
    gameState[12] = 0b0000100000000000000000000000000000000000000000000000000000000000LLU;
    // black kings
    gameState[13] = 0b0001000000000000000000000000000000000000000000000000000000000000LLU;

    // additional info (both colors can castle both ways), no en passant, no moves done (0th move)
    // of the 3-fold repetition and the 50-move rule
    gameState[14] = 0b0000000000000000000000000000000000000000000000000000000000001111LLU;

    gameLoop();
}

// compile with gcc -o Program *.c