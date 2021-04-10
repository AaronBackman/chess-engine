#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
      char *moveNotation;
      char *moveNotationFrom;
      char *moveNotationTo;

      dividedNodes = perft(depth - 1, -side);
      nodes += dividedNodes;

      // allocate space for the string, +1 for the null character
      moveNotation = (char*) malloc((4 + 1) * sizeof(char));

      moveNotation[0] = 'a' + (move.from % 8);
      moveNotation[1] = '1' + (move.from / 8);
      moveNotation[2] = 'a' + (move.to % 8);
      moveNotation[3] = '1' + (move.to / 8);
      moveNotation[4] = '\0';

      printf("%s: %d\n", moveNotation, dividedNodes);

      free(moveNotation);
    }
    unMakeMove();
  }

  printf("total nodes: %d\n\n", nodes);
}