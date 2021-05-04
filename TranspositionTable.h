#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

typedef unsigned long long u64;
typedef unsigned short u16;

typedef struct transpositionTableEntry {
    u64 zobristKey;
    int score;
    Move hashMove;
    u16 nodeType : 8;
    u16 depth : 8;
} TranspositionTableEntry;

#endif