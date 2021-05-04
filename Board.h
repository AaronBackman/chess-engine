#ifndef BOARD_H
#define BOARD_H

typedef unsigned long long u64;

typedef struct board {
    u64 whitePieces;
    u64 blackPieces;
    u64 pawns;
    u64 knights;
    u64 bishops;
    u64 rooks;
    u64 queens;
    u64 kings;
    u64 meta;
} Board;

#endif