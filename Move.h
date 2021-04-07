#ifndef MOVE_H
#define MOVE_H

typedef unsigned long long u64;
typedef unsigned int u32;

typedef struct move {
    u32 from : 6;
    u32 to : 6;
    u32 promotion : 3;
    u32 castling : 2;
    u32 checkmate : 1;
} Move;

#endif