#include <stdbool.h>
#include <stdio.h>

#include "Move.h"
#include "Constants.h"
#include "MakeMove.h"
#include "LegalityChecks.h"
#include "MoveGeneration.h"
#include "MakeMove.h"
#include "Utilities.h"

u64 perft(int depth, int side) {
  Move moves[MAX_MOVES];
  int moveCount;
  int i;
  u64 nodes = 0;

  if (depth == 0) 
    return 1ULL;

  moveCount = generateMoves(moves, side);
  for (i = 0; i < moveCount; i++) {
    makeMove(moves[i]);
    if (!checkIfCheckThreat(side)) {
      nodes += perft(depth - 1, -side);
    }
    unMakeMove();
  }
  return nodes;
}


void perftDivide(int depth) {
  Move moves[MAX_MOVES];
  int moveCount;
  int i;
  u64 nodes = 0;
  int side = getSideToPlay(GAME_STATE_STACK[GAME_STATE_STACK_POINTER][14]);

  if (depth == 0) 
    return;

  moveCount = generateMoves(moves, side);
  for (i = 0; i < moveCount; i++) {
    u64 dividedNodes = 0;
    Move move = moves[i];
    makeMove(move);
    if (!checkIfCheckThreat(side)) {
      dividedNodes = perft(depth - 1, -side);
      nodes += dividedNodes;
      printf("from: %d, to: %d, nodes: %d\n", move.from, move.to, dividedNodes);
    }
    unMakeMove();
  }

  printf("total nodes: %d\n\n", nodes);
}