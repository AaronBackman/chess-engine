#include "Constants.h"

u64 GAME_STATE_STACK[MAX_DEPTH + MAX_TURNS][15];
int GAME_STATE_STACK_POINTER = -1;

int MOVE_STACK_POINTER = 0;
Move MOVE_STACK[MAX_DEPTH][MAX_MOVES];
u64 SINGLE_BIT_LOOKUP[64];

void setSingleBitLookUp() {
  int i;

  for (i = 0; i < 64; i++) {
    SINGLE_BIT_LOOKUP[i] = 1LLU << i;
  }
}

void setLookUpTables() {
  setSingleBitLookUp();
}