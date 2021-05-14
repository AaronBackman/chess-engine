#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "Constants.h"
#include "TranspositionTable.h"
#include "Move.h"
#include "Board.h"
#include "Init.h"
#include "Utilities.h"

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

extern inline Move create_move(u16 from, u16 to, u16 code);

extern inline bool square_occupied(u64 board, int index);

extern inline u64 empty_square(u64 board, int index);

extern inline u64 fill_square(u64 board, int index);

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

    if (move.code == KNIGHT_PROMOTION_MOVE || move.code == KNIGHT_PROMOTION_CAPTURE_MOVE) {
        str[4] = 'n';
        str[5] = '\0';
    }
    else if (move.code == BISHOP_PROMOTION_MOVE || move.code == BISHOP_PROMOTION_CAPTURE_MOVE) {
        str[4] = 'b';
        str[5] = '\0';
    }
    else if (move.code == ROOK_PROMOTION_MOVE || move.code == ROOK_PROMOTION_CAPTURE_MOVE) {
        str[4] = 'r';
        str[5] = '\0';
    }
    else if (move.code == QUEEN_PROMOTION_MOVE || move.code == QUEEN_PROMOTION_CAPTURE_MOVE) {
        str[4] = 'q';
        str[5] = '\0';
    }
    else {
        str[4] = '\0';
    }
}

// gets the least significant bit using de brujn bitscan algorithm
extern inline int bitscan_forward(u64 board);


// gets the most significant bit using de brujn bitscan algorithm
extern inline int bitscan_reverse(u64 board);

void print_board() {
    Board gameState = g_gameStateStack[g_root + g_ply];
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

void add_tt_entry(u64 zobristKey, int score, Move hashMove, u16 nodeType, u16 depth) {
    TranspositionTableEntry ttEntry = (TranspositionTableEntry){.zobristKey = zobristKey, .score = score, .hashMove = hashMove, .nodeType = nodeType, .depth = depth};
    
    // checkmate ply adjustment
    if (ttEntry.score > 100000) {
        ttEntry.score += g_ply;
    }
    else if (ttEntry.score < -100000) {
        ttEntry.score -= g_ply;
    }

    tTable[zobristKey % TRANSPOSITION_TABLE_SIZE] = ttEntry;
}

u64 get_zobrist(Board board) {
    u64 zobrist = 0;
    int i;

    for (i = 0; i < 64; i++) {
        if (square_occupied(board.whitePieces | board.blackPieces, i)) {
            if (square_occupied(board.pawns & board.whitePieces, i)) zobrist ^= WHITE_PAWN_ZOBRIST[i];
            if (square_occupied(board.pawns & board.blackPieces, i)) zobrist ^= BLACK_PAWN_ZOBRIST[i];

            if (square_occupied(board.knights & board.whitePieces, i)) zobrist ^= WHITE_KNIGHT_ZOBRIST[i];
            if (square_occupied(board.knights & board.blackPieces, i)) zobrist ^= BLACK_KNIGHT_ZOBRIST[i];

            if (square_occupied(board.bishops & board.whitePieces, i)) zobrist ^= WHITE_BISHOP_ZOBRIST[i];
            if (square_occupied(board.bishops & board.blackPieces, i)) zobrist ^= BLACK_BISHOP_ZOBRIST[i];

            if (square_occupied(board.rooks & board.whitePieces, i)) zobrist ^= WHITE_ROOK_ZOBRIST[i];
            if (square_occupied(board.rooks & board.blackPieces, i)) zobrist ^= BLACK_ROOK_ZOBRIST[i];

            if (square_occupied(board.queens & board.whitePieces, i)) zobrist ^= WHITE_QUEEN_ZOBRIST[i];
            if (square_occupied(board.queens & board.blackPieces, i)) zobrist ^= BLACK_QUEEN_ZOBRIST[i];

            if (square_occupied(board.kings & board.whitePieces, i)) zobrist ^= WHITE_KING_ZOBRIST[i];
            if (square_occupied(board.kings & board.blackPieces, i)) zobrist ^= BLACK_KING_ZOBRIST[i];
        }
    }

    if (get_side_to_play(board.meta) == BLACK) zobrist ^= BLACK_TO_MOVE_ZOBRIST;

    if (can_white_castle_short(board.meta)) zobrist ^= CASTLING_ZOBRIST[0];
    if (can_white_castle_long(board.meta)) zobrist ^= CASTLING_ZOBRIST[1];
    if (can_black_castle_short(board.meta)) zobrist ^= CASTLING_ZOBRIST[2];
    if (can_black_castle_long(board.meta)) zobrist ^= CASTLING_ZOBRIST[3];

    if (is_enpassant_allowed(board.meta)) zobrist ^= ENPASSANT_FILE_ZOBRIST[get_enpassant_square(board.meta) % 8];

    return zobrist;
}

void print_in_binary(u64 n) {
    int i;

    for (i = 63; i >= 0; i--) {
        if ((n & SINGLE_BIT_LOOKUP[i]) == 0) printf("0");
        else printf("1");
    }

    printf("\n");
}

void print_bitmap_binary(u64 bb) {
    int i = 7;
    int j = 0;

    printf("\n");

    while (i * 8 + j >= 0) {
        if (square_occupied(bb, i * 8 + j)) {
            printf("1 ");
        }
        else {
            printf("0 ");
        }

        if (j == 7) {
            printf("\n");
            j = 0;
            i--;
        }
        else {
            j++;
        }
    }

    printf("\n");
}


int max(int a, int b) {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}