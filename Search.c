#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "Constants.h"
#include "Move.h"
#include "MoveGeneration.h"
#include "MakeMove.h"
#include "LegalityChecks.h"

u64 nodeCount = 0;

int evaluate(int side) {
  u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
  u64 whitePieces = gameState[0];
  u64 whitePawns = gameState[1];
  u64 whiteKnights = gameState[2];
  u64 whiteBishops = gameState[3];
  u64 whiteRooks = gameState[4];
  u64 whiteQueens = gameState[5];
  u64 whiteKings = gameState[6];

  u64 blackPieces = gameState[7];
  u64 blackPawns = gameState[8];
  u64 blackKnights = gameState[9];
  u64 blackBishops = gameState[10];
  u64 blackRooks = gameState[11];
  u64 blackQueens = gameState[12];
  u64 blackKings = gameState[13];
  int score;
  int whiteMaterial = 0;
  int blackMaterial = 0;

  nodeCount++;

  while (whitePawns != 0) {
      whitePawns &= (whitePawns - 1);
      whiteMaterial += PAWN_SCORE;
  }
  while (whiteKnights!= 0) {
      whiteKnights &= (whiteKnights - 1);
      whiteMaterial += KNIGHT_SCORE;
  }
  while (whiteBishops != 0) {
      whiteBishops &= (whiteBishops - 1);
      whiteMaterial += BISHOP_SCORE;
  }
  while (whiteRooks != 0) {
      whiteRooks &= (whiteRooks - 1);
      whiteMaterial += ROOK_SCORE;
  }
  while (whiteQueens != 0) {
      whiteQueens &= (whiteQueens - 1);
      whiteMaterial += QUEEN_SCORE;
  }


  while (blackPawns != 0) {
      blackPawns &= (blackPawns - 1);
      blackMaterial += PAWN_SCORE;
  }

  while (blackKnights!= 0) {
      blackKnights &= (blackKnights - 1);
      blackMaterial += KNIGHT_SCORE;
  }

  while (blackBishops != 0) {
      blackBishops &= (blackBishops - 1);
      blackMaterial += BISHOP_SCORE;
  }

  while (blackRooks != 0) {
      blackRooks &= (blackRooks - 1);
      blackMaterial += ROOK_SCORE;
  }

  while (blackQueens != 0) {
      blackQueens &= (blackQueens - 1);
      blackMaterial += QUEEN_SCORE;
  }

  score = (whiteMaterial - blackMaterial) * side;

  return score;
}

int negaMax(int alpha, int beta, int depth, int side) {
    int moveCount;
    int legalMoveCount = 0;
    int i;
    int score;
    Move *movesArr = MOVE_STACK[MOVE_STACK_POINTER];

    if (depth == 0) return evaluate(side);

    moveCount = generateMoves(movesArr, side);
    MOVE_STACK_POINTER++;

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];
        makeMove(move);

        if (!isKingThreatened(side)) {
          legalMoveCount++;
          score = -negaMax(-beta, -alpha, depth - 1, -side);

          if (score >= beta) {
            unMakeMove();
            MOVE_STACK_POINTER--;
            return beta;
          }

          if (score > alpha) {
            alpha = score;
          }
        }
        unMakeMove();
    }
    MOVE_STACK_POINTER--;

    // checkmate or draw, depends if king is safe or nat
    if (legalMoveCount == 0) {
      if (!isKingThreatened(side)) {
        // a draw if king is not threatened
        return 0;
      }
    }

    return alpha;
}


Move negaMaxRoot(int alpha, int beta, int depth, int side) {
    int moveCount;
    int legalMoveCount = 0;
    int i;
    Move bestMove;
    int score;
    Move *movesArr = MOVE_STACK[MOVE_STACK_POINTER];

    moveCount = generateMoves(movesArr, side);
    MOVE_STACK_POINTER++;

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];
        makeMove(move);

        if (!isKingThreatened(side)) {
          legalMoveCount++;
          score = -negaMax(-beta, -alpha, depth - 1, -side);

          if (score >= beta) {
            printf("fail hard, score: %d\n", score);
            unMakeMove();
            MOVE_STACK_POINTER--;
            return bestMove;
          }

          printf("score: %d\n", score);
          printf("from: %d, to: %d\n", move.from, move.to);
          if (score > alpha) {
            bestMove = move;
            alpha = score;

            printf("score rise: %d\n", score);
            printf("from: %d, to: %d\n", move.from, move.to);
          }
        }
        unMakeMove();
    }
    MOVE_STACK_POINTER--;

    printf("best score: %d\n", alpha);
    printf("best move: from: %d, to: %d\n", bestMove.from, bestMove.to);

    printf("nodes: %d\n", nodeCount);

    printf("movesstack: %d\n", MOVE_STACK_POINTER);
    
    return bestMove;
}