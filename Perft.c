#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Move.h"
#include "Board.h"
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

  moveCount = generate_moves(moves, side);

  //print_board();

  for (i = 0; i < moveCount; i++) {
    make_move(moves[i], false);
    if (!is_king_threatened(side)) {
      nodes += perft(depth - 1, -side);
    }
    unmake_move();
  }
  return nodes;
}


void perft_divide(int depth) {
  Move moves[MAX_MOVES];
  int moveCount;
  int i;
  u64 nodes = 0;
  int side = get_side_to_play(g_gameStateStack[g_root].meta);

  if (depth == 0) 
    return;

  moveCount = generate_moves(moves, side);
  for (i = 0; i < moveCount; i++) {
    u64 dividedNodes = 0;
    Move move = moves[i];

    make_move(move, false);
    if (!is_king_threatened(side)) {
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

      printf("%s: %llu\n", moveNotation, dividedNodes);
      //printf("code: %d\n\n", move.code);

      free(moveNotation);
    }
    unmake_move();
  }

  printf("total nodes: %llu\n\n", nodes);
}