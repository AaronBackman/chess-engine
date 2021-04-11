#ifndef INIT_H
#define INIT_H

#include "Move.h"

// precalculated bit shifts, for example singleBitLookUp[15] == 1LLU << 15
extern u64 SINGLE_BIT_LOOKUP[64];
// pattern used to calculate north-east diagonal moves with bitboards
extern u64 NORTH_EAST_LOOKUP_PATTERN[64];
extern u64 NORTH_WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_EAST_LOOKUP_PATTERN[64];
extern u64 SOUTH_WEST_LOOKUP_PATTERN[64];

extern u64 NORTH_LOOKUP_PATTERN[64];
extern u64 WEST_LOOKUP_PATTERN[64];
extern u64 SOUTH_LOOKUP_PATTERN[64];
extern u64 EAST_LOOKUP_PATTERN[64];

void setLookUpTables();

#endif