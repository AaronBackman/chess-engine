#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "Constants.h"
#include "Move.h"
#include "Init.h"

Move create_move(int from, int to, int promotion, int castle, bool enPassant) {
    Move move = {from, to, promotion, castle, enPassant};

    return move;
}

bool square_occupied(u64 board, int index) {
    u64 bitIndex = SINGLE_BIT_LOOKUP[index];

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

u64 empty_square(u64 board, int index) {
    return board & ~SINGLE_BIT_LOOKUP[index];
}

u64 fill_square(u64 board, int index) {
    return board | SINGLE_BIT_LOOKUP[index];
}

bool can_white_castle_short(u64 meta) {
    return (meta & 1LLU) > 0;
}

bool can_white_castle_long(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[1]) > 0;
}

bool can_black_castle_short(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[2]) > 0;
}

bool can_black_castle_long(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[3]) > 0;
}

u64 remove_white_castle_short(u64 meta) {
    return meta & ~(1LLU);
}

u64 remove_white_castle_long(u64 meta) {
    return meta & ~SINGLE_BIT_LOOKUP[1];
}

u64 remove_black_castle_short(u64 meta) {
    return meta & ~SINGLE_BIT_LOOKUP[2];
}

u64 remove_black_castle_long(u64 meta) {
    return meta &=~SINGLE_BIT_LOOKUP[3];
}

u64 set_white_castle_short(u64 meta) {
    return meta |= (1LLU);
}

u64 set_white_castle_long(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[1];
}

u64 set_black_castle_short(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[2];
}

u64 set_black_castle_long(u64 meta) {
    return meta |= SINGLE_BIT_LOOKUP[3];
}

bool is_enpassant_allowed(u64 meta) {
    return (meta & SINGLE_BIT_LOOKUP[4]) > 0;
}

u64 set_enpassant_allowed(u64 meta, bool enPassantAllowed) {
    if (enPassantAllowed) {
        meta |= SINGLE_BIT_LOOKUP[4];
    }
    else {
        meta &= ~SINGLE_BIT_LOOKUP[4];
    }

    return meta;
}

u64 set_enpassant_square(u64 meta, u64 square) {
    meta &= ~(0b111111LLU << 5);
    meta |= (square << 5);

    return meta;
}

u64 get_enpassant_square(u64 meta) {
    return (meta & (0b111111LLU << 5)) >> 5;
}

// 1 == white plays, -1 == black plays
int get_side_to_play(u64 meta) {
    if ((meta & SINGLE_BIT_LOOKUP[11]) > 0) {
        return 1;
    }
    else {
        return -1;
    }
}

// 1 == white plays, -1 == black plays
u64 set_side_to_play(u64 meta, int side) {
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

void move_to_string(char *str, Move move) {
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
int bitscan_forward(u64 board) {
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
int bitscan_reverse(u64 board) {
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

void print_board() {
    u64 *gameState = g_gameStateStack[g_root + g_ply];

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

        if (!square_occupied(whitePieces, i) && !square_occupied(blackPieces, i)) {
            printf("__");
        }

        if (square_occupied(whitePieces, i)) printf("+");
        if (square_occupied(blackPieces, i)) printf("#");

        if (square_occupied(whitePawns, i)) printf("P");

        if (square_occupied(whiteBishops, i)) printf("B");

        if (square_occupied(whiteKnights, i)) printf("N");

        if (square_occupied(whiteRooks, i)) printf("R");

        if (square_occupied(whiteQueens, i)) printf("Q");

        if (square_occupied(whiteKings, i)) printf("K");

        if (square_occupied(blackPawns, i)) printf("p");

        if (square_occupied(blackBishops, i)) printf("b");

        if (square_occupied(blackKnights, i)) printf("n");

        if (square_occupied(blackRooks, i)) printf("r");

        if (square_occupied(blackQueens, i)) printf("q");

        if (square_occupied(blackKings, i)) printf("k");

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

int get_manhattan_distance(int sq1, int sq2) {
    return MD[sq1][sq2];
}