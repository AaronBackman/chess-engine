#include "Constants.h"

u64 g_gameStateStack[MAX_DEPTH + MAX_TURNS][15];
int g_root = -1;

Move g_moveStack[MAX_DEPTH][MAX_MOVES];

int g_cancelThread = 0;

int g_ply = 0;

u64 g_attackSets[MAX_DEPTH];