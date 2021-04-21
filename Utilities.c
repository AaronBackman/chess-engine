#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "Constants.h"
#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, bool enPassant) {
    Move move = {from, to, promotion, castle, enPassant};

    return move;
}

bool squareOccupied(u64 board, int index) {
    u64 bitIndex = SINGLE_BIT_LOOKUP[index];

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

u64 emptySquare(u64 board, int index) {
    return board & ~SINGLE_BIT_LOOKUP[index];
}

u64 fillSquare(u64 board, int index) {
    return board | SINGLE_BIT_LOOKUP[index];
}

bool canWhiteCastleShort(u64 meta) {
    return (meta & 1LLU) > 0;
}

bool canWhiteCastleLong(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[1]) > 0;
}

bool canBlackCastleShort(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[2]) > 0;
}

bool canBlackCastleLong(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[3]) > 0;
}

u64 removeWhiteCastleShort(u64 meta) {
    return meta & ~(1LLU);
}

u64 removeWhiteCastleLong(u64 meta) {
    return meta & ~SINGLE_BIT_LOOKUP[1];
}

u64 removeBlackCastleShort(u64 meta) {
    return meta & ~SINGLE_BIT_LOOKUP[2];
}

u64 removeBlackCastleLong(u64 meta) {
    return meta &=~SINGLE_BIT_LOOKUP[3];
}

u64 setWhiteCastleShort(u64 meta) {
    return meta |= (1LLU);
}

u64 setWhiteCastleLong(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[1];
}

u64 setBlackCastleShort(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[2];
}

u64 setBlackCastleLong(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[3];
}

bool isEnPassantAllowed(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[4]) > 0;
}

u64 setEnPassantAllowed(u64 meta, bool enPassantAllowed) {
    if (enPassantAllowed) {
        meta |= SINGLE_BIT_LOOKUP[4];
    }
    else {
        meta &= ~SINGLE_BIT_LOOKUP[4];
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
    if ((meta & SINGLE_BIT_LOOKUP[11]) > 0) {
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
        meta |= SINGLE_BIT_LOOKUP[11];
    }
    else {
        // set 12th bit to 0
        meta &= ~SINGLE_BIT_LOOKUP[11];
    }

    return meta;
}

void moveToString(char *str, Move move) {
    str[0] = 'a' + move.from % 8;
    str[1] = '1' + move.from / 8;
    str[2] = 'a' + move.to % 8;
    str[3] = '1' + move.to / 8;

    if (move.promotion == 1) {
        str[4] = 'n';
        str[5] = '\0';
    }
    else if (move.promotion == 2) {
        str[4] = 'b';
        str[5] = '\0';
    }
    else if (move.promotion == 3) {
        str[4] = 'r';
        str[5] = '\0';
    }
    else if (move.promotion == 4) {
        str[4] = 'q';
        str[5] = '\0';
    }
    else {
        str[4] = '\0';
    }
}

const int index64Forward[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

// gets the least significant bit using de brujn bitscan algorithm
int bitScanForward(u64 board) {
    const u64 debruijn64 = 0x03f79d71b4cb0a89;
    assert (board != 0);
    return index64Forward[((board & -board) * debruijn64) >> 58];
}

const int index64Reverse[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

// gets the most significant bit using de brujn bitscan algorithm
int bitScanReverse(u64 board) {
   const u64 debruijn64 = 0x03f79d71b4cb0a89;
   assert (board != 0);
   board |= board >> 1; 
   board |= board >> 2;
   board |= board >> 4;
   board |= board >> 8;
   board |= board >> 16;
   board |= board >> 32;
   return index64Reverse[(board * debruijn64) >> 58];
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