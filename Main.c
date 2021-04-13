#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "Move.h"
#include "MoveGeneration.h"
#include "Constants.h"
#include "MakeMove.h"
#include "LegalityChecks.h"
#include "Utilities.h"
#include "Perft.h"
#include "Init.h"

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
    Move bestMove;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    int i = 0;
    Move legalMoves[MAX_MOVES];

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
        int side;

        gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
        side = getSideToPlay(gameState[14]);

        legalMoveCount = generateMoves(legalMoves, side);

        if (legalMoveCount == 0) {
            printf("no moves generated\n");
        }

        if (checkIfNoLegalMoves(side)) {
            // difference between a stalemate and a checkmate
            if (isKingThreatened(side)) {
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

            printf("some move: from: %d, to: %d\n", legalMoves[i].from, legalMoves[i].to);

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
        if (isKingThreatened(side)) {
            printf("move is not legal, you are in check\n");
            unMakeMove();
            continue;
        }

        printf("side: %d   from: %d,   to: %d\n", side, from, to);
        printf("move: castle: %d, promotion: %d, enpassant: %d\n", legalMove.castling, legalMove.promotion, legalMove.enPassant);

        printBoard();

        i++;
    }
}


void main(void) {
    int leastBit;
    GAME_STATE_STACK_POINTER++;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    char fenStr[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";

    // some important initializations
    setLookUpTables();

    gameState = parseFen(gameState, fenStr);

    leastBit = bitScanForward(KNIGHT_LOOKUP_PATTERN[1]);

    //printf("least bit: %d\n", leastBit);

    //gameLoop();
    perftDivide(5);
}

// compile with gcc -o Program *.c