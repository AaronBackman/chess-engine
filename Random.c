#include "Constants.h"

const int SEED = 42;

typedef struct xorshift64s_state {
  u64 a;
} Xorshift64s_state;

Xorshift64s_state rngState = {.a = SEED};

u64 xorshift64s(Xorshift64s_state *state) {
	u64 x = state->a;	/* The state must be seeded with a nonzero value. */
	x ^= x >> 12; // a
	x ^= x << 25; // b
	x ^= x >> 27; // c
	state->a = x;
	return x * 0x2545F4914F6CDD1D;
}

u64 getRNG() {
    return xorshift64s(&rngState);
}