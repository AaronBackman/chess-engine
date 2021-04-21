#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "Constants.h"
#include "Move.h"
#include "MoveGeneration.h"
#include "MakeMove.h"
#include "LegalityChecks.h"
#include "Utilities.h"
#include "Init.h"

Move SELECTED_MOVE;
int SEARCHED_DEPTH;

int ORIGINAL_GAMES_STATE_STACK_POINTER;
int ORIGINAL_MOVE_STACK_POINTER;

u64 nodeCount = 0;
u64 quiescentNodeCount = 0;

bool checkmate;

// parameters are the count of each piece type and color on the board (eg. wp == number white pawns)
// used to calculate the tapered eval
int calcPhase(int wp, int bp, int wn, int bn, int wb, int bb, int wr, int br, int wq, int bq) {
  int phase;
  int pawnPhase = 0;
  int knightPhase = 1;
  int bishopPhase = 1;
  int rookPhase = 2;
  int queenPhase = 4;
  int totalPhase = pawnPhase * 16 + knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

  phase = totalPhase;

  phase -= wp * pawnPhase;
  phase -= bp * pawnPhase;
  phase -= wn * knightPhase;
  phase -= bn * knightPhase;
  phase -= wb * bishopPhase;
  phase -= bb * bishopPhase;
  phase -= wr * rookPhase;
  phase -= br * rookPhase;
  phase -= wq * queenPhase;
  phase -= bq * queenPhase;

  phase = (phase * 256 + (totalPhase / 2)) / totalPhase;

  return phase;
}

// white tries to checkmate the black king
int blackKingToCorners() {
    // center manhattan distance of the black king
    int cmd;
    int whiteKingIndex;
    int blackKingIndex;

    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 whiteKings = gameState[6];
    u64 blackKings = gameState[13];

    whiteKingIndex = bitScanForward(whiteKings);
    blackKingIndex = bitScanForward(blackKings);

    return 47 * CMD[blackKingIndex] + 18 * (14 - getManhattanDistance(whiteKingIndex, blackKingIndex));
}

// black tries to checkmate the white king
int whiteKingToCorners() {
    // center manhattan distance of the white king
    int cmd;
    int whiteKingIndex;
    int blackKingIndex;

    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 whiteKings = gameState[6];
    u64 blackKings = gameState[13];

    whiteKingIndex = bitScanForward(whiteKings);
    blackKingIndex = bitScanForward(blackKings);

    return 47 * CMD[whiteKingIndex] + 18 * (14 - getManhattanDistance(whiteKingIndex, blackKingIndex));
}

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
  int openingScore;
  int endgameScore;
  int mopUpScore;
  int index;
  int whiteOpeningMaterial = 0;
  int blackOpeningMaterial = 0;
  int whiteEndgameMaterial = 0;
  int blackEndgameMaterial = 0;
  int phase;
  // weighted sum of pieces (not pawns and kings)
  int whitePieceSum;
  int blackPieceSum;

  // number of each piece type and color
  int wp = 0;
  int bp = 0;
  int wn = 0;
  int bn = 0;
  int wb = 0;
  int bb = 0;
  int wr = 0;
  int br = 0;
  int wq = 0;
  int bq = 0;


  // first calculate the opening score
  while (whitePawns != 0) {
      index = bitScanForward(whitePawns);
      whitePawns &= (whitePawns - 1);
      whiteOpeningMaterial += PAWN_SCORE + WHITE_PAWN_PIECE_SQUARE_TABLE[index];
      wp++;
  }
  while (whiteKnights!= 0) {
      index = bitScanForward(whiteKnights);
      whiteKnights &= (whiteKnights - 1);
      whiteOpeningMaterial += KNIGHT_SCORE + WHITE_KNIGHT_PIECE_SQUARE_TABLE[index];
      wn++;
  }
  while (whiteBishops != 0) {
      index = bitScanForward(whiteBishops);
      whiteBishops &= (whiteBishops - 1);
      whiteOpeningMaterial += BISHOP_SCORE + WHITE_BISHOP_PIECE_SQUARE_TABLE[index];
      wb++;
  }
  while (whiteRooks != 0) {
      index = bitScanForward(whiteRooks);
      whiteRooks &= (whiteRooks - 1);
      whiteOpeningMaterial += ROOK_SCORE + WHITE_ROOK_PIECE_SQUARE_TABLE[index];
      wr++;
  }
  while (whiteQueens != 0) {
      index = bitScanForward(whiteQueens);
      whiteQueens &= (whiteQueens - 1);
      whiteOpeningMaterial += QUEEN_SCORE + WHITE_QUEEN_PIECE_SQUARE_TABLE[index];
      wq++;
  }

  // endgame and opening evaluate similarly until now
  whiteEndgameMaterial = whiteOpeningMaterial;

  // only 1 king can exist
  index = bitScanForward(whiteKings);
  whiteOpeningMaterial += KING_SCORE + WHITE_KING_PIECE_SQUARE_TABLE_OPENING[index];
  whiteEndgameMaterial += KING_SCORE + WHITE_KING_PIECE_SQUARE_TABLE_ENDGAME[index];


  while (blackPawns != 0) {
      index = bitScanForward(blackPawns);
      blackPawns &= (blackPawns - 1);
      blackOpeningMaterial += PAWN_SCORE + BLACK_PAWN_PIECE_SQUARE_TABLE[index];
      bp++;
  }
  while (blackKnights!= 0) {
      index = bitScanForward(blackKnights);
      blackKnights &= (blackKnights - 1);
      blackOpeningMaterial += KNIGHT_SCORE+ BLACK_KNIGHT_PIECE_SQUARE_TABLE[index];
      bn++;
  }
  while (blackBishops != 0) {
      index = bitScanForward(blackBishops);
      blackBishops &= (blackBishops - 1);
      blackOpeningMaterial += BISHOP_SCORE+ BLACK_BISHOP_PIECE_SQUARE_TABLE[index];
      bb++;
  }
  while (blackRooks != 0) {
      index = bitScanForward(blackRooks);
      blackRooks &= (blackRooks - 1);
      blackOpeningMaterial += ROOK_SCORE+ BLACK_ROOK_PIECE_SQUARE_TABLE[index];
      br++;
  }
  while (blackQueens != 0) {
      index = bitScanForward(blackQueens);
      blackQueens &= (blackQueens - 1);
      blackOpeningMaterial += QUEEN_SCORE+ BLACK_QUEEN_PIECE_SQUARE_TABLE[index];
      bq++;
  }

  whitePieceSum = wn * 3 + wb * 3 + wr * 5 + wq * 9;
  blackPieceSum = bn * 3 + bb * 3 + br * 5 + bq * 9;
  // check draw by insufficient material
  if (wp + bp == 0) {
      //printf("insufficient material");
      if ((whitePieceSum - blackPieceSum) < 4 && (whitePieceSum - blackPieceSum) > -4) {
          return 0;
      }
  }

  // white can try to checkmate the opponent
  if ((whitePieceSum - blackPieceSum) >= 4 && blackPieceSum <= 9) {
      //printf("white mopup");
      mopUpScore = blackKingToCorners() * side;
  }
  // black can try to checkmate the opponent
  else if ((blackPieceSum - whitePieceSum) >= 4 && whitePieceSum <= 9) {
      //printf("black mopup");
      mopUpScore = -1 * whiteKingToCorners() * side;
  }
  else {
      mopUpScore = 0;
  }

  // endgame and opening evaluate similarly until now
  blackEndgameMaterial = blackOpeningMaterial;

  // only 1 king can exist
  index = bitScanForward(blackKings);
  blackOpeningMaterial += KING_SCORE + BLACK_KING_PIECE_SQUARE_TABLE_OPENING[index];;
  blackEndgameMaterial += KING_SCORE + BLACK_KING_PIECE_SQUARE_TABLE_ENDGAME[index];


  // tapered evaluation score
  phase = calcPhase(wp, bp, wn, bn, wb, bb, wr, br, wq, bq);
  openingScore = (whiteOpeningMaterial - blackOpeningMaterial) * side;
  endgameScore = (whiteEndgameMaterial - blackEndgameMaterial) * side;
  score = ((openingScore * (256 - phase)) + (endgameScore * phase)) / 256;
  score += mopUpScore;

  return score;
}

// quiescent search comes after normal search and only looks for captures
int qsearch(int alpha, int beta, int side) {
    int moveCount;
    int i;
    int score;
    Move *movesArr = MOVE_STACK[MOVE_STACK_POINTER];
    int stand_pat = evaluate(side);

    quiescentNodeCount++;

    if (CANCEL_THREAD == 1) {
        printf("exit\n");
        CANCEL_THREAD = 0;
        pthread_exit(NULL);
    }

    if (stand_pat >= beta) {
        return beta;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    moveCount = generateCaptures(movesArr, side);
    MOVE_STACK_POINTER++;

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];

        makeMove(move);

        if (!isKingThreatened(side)) {
            score = -qsearch(-beta, -alpha, -side);

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
    
    return alpha;
}

int negaMax(int alpha, int beta, int depth, int side) {
    int moveCount;
    int legalMoveCount = 0;
    int i;
    int score;
    Move *movesArr = MOVE_STACK[MOVE_STACK_POINTER];

    if (CANCEL_THREAD == 1) {
        CANCEL_THREAD = 0;
        pthread_exit(NULL);
    }

    if (depth == 0) {
        nodeCount++;
        return qsearch(alpha, beta, side);
    }

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

    // checkmate or draw, depends if king is safe or not
    if (legalMoveCount == 0) {
        if (!isKingThreatened(side)) {
            // a draw if king is not threatened
            return 0;
        }
        else {
            checkmate = true;
            // loses if king is lost => return a very bad score
            return -100000;
        }
    }

    return alpha;
}


Move negaMaxRoot(int alpha, int beta, int depth, int side) {
    int moveCount;
    int i;
    Move bestMove;
    int score;
    Move *movesArr = MOVE_STACK[MOVE_STACK_POINTER];

    moveCount = generateMoves(movesArr, side);
    MOVE_STACK_POINTER++;

    // select first legal move to be the best move so that the engine always returns atleast some move
    for (i = 0; i < moveCount; i++) {
        Move move = movesArr[i];
        makeMove(move);

        if (!isKingThreatened(side)) {
            bestMove = move;
            unMakeMove();
            break;
        }
        unMakeMove();
    }


    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];
        makeMove(move);

        if (!isKingThreatened(side)) {
            score = -negaMax(-beta, -alpha, depth - 1, -side);

            if (score >= beta) {
                // printf("fail hard, score: %d\n", score);
                unMakeMove();
                MOVE_STACK_POINTER--;
                return bestMove;
            }

            //printf("score: %d\n", score);
            //printf("from: %d, to: %d\n", move.from, move.to);
            if (score > alpha) {
                bestMove = move;
                alpha = score;

                // printf("score rise: %d\n", score);
                // printf("from: %d, to: %d\n", move.from, move.to);
            }
        }
        unMakeMove();
    }
    MOVE_STACK_POINTER--;

    //printf("best score: %d\n", alpha);
    //printf("best move: from: %d, to: %d\n", bestMove.from, bestMove.to);

    //printf("nodes: %d\n", nodeCount);

    return bestMove;
}

// cleans up after this thread is terminated
void cleanUpHandler(void *vargp) {
    GAME_STATE_STACK_POINTER = ORIGINAL_GAMES_STATE_STACK_POINTER;
    MOVE_STACK_POINTER = ORIGINAL_MOVE_STACK_POINTER;
    printf("cleanup\n");
}

// a thread to calculate the best moves with iterative deepening
void *search(void *vargp) {
    SEARCHED_DEPTH = 0;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 otherGameInfo = gameState[14];
    int side = getSideToPlay(otherGameInfo);

    ORIGINAL_GAMES_STATE_STACK_POINTER = GAME_STATE_STACK_POINTER;
    ORIGINAL_MOVE_STACK_POINTER = MOVE_STACK_POINTER;

    pthread_cleanup_push(cleanUpHandler, vargp);

    printf("game: %d, move: %d\n", GAME_STATE_STACK_POINTER, MOVE_STACK_POINTER);

    checkmate = false;
    CANCEL_THREAD = 0;

    while (SEARCHED_DEPTH < MAX_DEPTH) {
        nodeCount = 0;
        quiescentNodeCount = 0;
        SELECTED_MOVE = negaMaxRoot(ALPHA_BETA_MIN, ALPHA_BETA_MAX, SEARCHED_DEPTH + 1, side);

        if (checkmate) {
            printf("checkmate\n");
            break;
        }

        printf("searched depth: %d\n", SEARCHED_DEPTH);
        //printf("nodes: %d\n", nodeCount);
        //printf("quiescent: %d\n", quiescentNodeCount);
        SEARCHED_DEPTH++;
    }

    pthread_cleanup_pop(1);
}