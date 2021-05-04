#ifndef MOVE_H
#define MOVE_H

typedef unsigned short u16;

typedef struct move {
    u16 from : 6;
    u16 to : 6;
    u16 code: 4;
} Move;

#endif