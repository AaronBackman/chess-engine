#include <stdbool.h>

#include "Constants.h"
#include "Move.h"

Move createMove(int from, int to, int promotion, int castle, bool enPassant) {
    Move move = {from, to, promotion, castle, enPassant};

    return move;
}

bool squareOccupied(u64 board, int index) {
    u64 bitIndex = (1ULL << index);

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

u64 emptySquare(u64 board, int index) {
    return board & ~(1LLU << index);
}

u64 fillSquare(u64 board, int index) {
    return board | (1LLU << index);
}

bool canWhiteCastleShort(u64 meta) {
    return (meta & 1LLU) > 0;
}

bool canWhiteCastleLong(u64 meta) {
    return (meta & (1LLU << 1)) > 0;
}

bool canBlackCastleShort(u64 meta) {
    return (meta & (1LLU << 2)) > 0;
}

bool canBlackCastleLong(u64 meta) {
    return (meta & (1LLU << 3)) > 0;
}

u64 removeWhiteCastleShort(u64 meta) {
    return meta & ~(1LLU);
}

u64 removeWhiteCastleLong(u64 meta) {
    return meta & ~(1LLU << 1);
}

u64 removeBlackCastleShort(u64 meta) {
    return meta & ~(1LLU << 2);
}

u64 removeBlackCastleLong(u64 meta) {
    return meta &=~(1LLU << 3);
}

bool isEnPassantAllowed(u64 meta) {
    return (meta & (1LLU << 4)) > 0;
}

u64 setEnPassantAllowed(u64 meta, bool enPassantAllowed) {
    if (enPassantAllowed) {
        meta |= (1LLU << 4);
    }
    else {
        meta &= ~(1LLU << 4);
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