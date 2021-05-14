#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "Constants.h"
#include "Move.h"
#include "TranspositionTable.h"
#include "Board.h"
#include "MoveGeneration.h"
#include "MakeMove.h"
#include "LegalityChecks.h"
#include "Utilities.h"
#include "Init.h"
#include "Perft.h"

Move g_selectedMove;
bool foundMove;
int searchedDepth;
int currentScore;

int ORIGINAL_GAMES_STATE_STACK_POINTER;
int ORIGINAL_MOVE_STACK_POINTER;

u64 nodeCount = 0;
u64 quiescentNodeCount = 0;
u64 transpositionCount = 0;

bool checkmate;
bool losingCheckMate;

int rootSideToMove;

int firstKiller;
int secondKiller;
int mateKiller;

// killer move heuristic
Move KILLER_MOVES[MAX_DEPTH][2];

// mate killer move heuristic
Move KILLER_MATE_MOVES[MAX_DEPTH];

void clear_killers() {
    KILLER_MATE_MOVES[g_ply] = create_move(0, 0, 0);
    KILLER_MOVES[g_ply][0] = create_move(0, 0, 0);
    KILLER_MOVES[g_ply][1] = create_move(0, 0, 0);
}

int MVV_LVA[5][6] = {
    {500, 400, 300, 200, 100, 600},
    {1100, 1000, 900, 800, 700, 1200},
    {1700, 1600, 1500, 1400, 1300, 1800},
    {2300, 2200, 2100, 2000, 1900, 2400},
    {2900, 2800, 2700, 2600, 2500, 3000}
};

int PROMOTIONS[4] = {
    -1000,
    -2000,
    -3000,
    1000
};

int pieceValues[6] = {
    PAWN_SCORE, KNIGHT_SCORE, BISHOP_SCORE, ROOK_SCORE, QUEEN_SCORE, KING_SCORE
};

int history[2][64][64];

int MOVE_SCORES[MAX_MOVES];
int QUIET_MOVE_SCORES[MAX_MOVES];


// debugging info
u64 betaCutOffs[100][5];
u64 betaCutOffsTotal[100];

void zero_beta_cutoff() {
    int i;
    int j;

    for (i = 0; i < 100; i++) {
        betaCutOffsTotal[i] = 0;

        for (j = 0; j < 5; j++) {
            betaCutOffs[i][j] = 0;
        }
    }
}

void zero_history() {
    int i;
    int j;

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            history[0][i][j] = 0;
            history[1][i][j] = 0;
        }
    }
}

// parameters are the count of each piece type and color on the board (eg. wp == number white pawns)
// used to calculate the tapered eval
int calc_phase(int wp, int bp, int wn, int bn, int wb, int bb, int wr, int br, int wq, int bq) {
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
int black_king_to_corners() {
    // center manhattan distance of the black king
    int cmd;
    int whiteKingIndex;
    int blackKingIndex;

    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whiteKings = gameState.kings & gameState.whitePieces;
    u64 blackKings = gameState.kings & gameState.blackPieces;

    whiteKingIndex = bitscan_forward(whiteKings);
    blackKingIndex = bitscan_forward(blackKings);

    return 47 * CMD[blackKingIndex] + 18 * (14 - get_manhattan_distance(whiteKingIndex, blackKingIndex));
}

// black tries to checkmate the white king
int white_king_to_corners() {
    // center manhattan distance of the white king
    int cmd;
    int whiteKingIndex;
    int blackKingIndex;

    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whiteKings = gameState.kings & gameState.whitePieces;
    u64 blackKings = gameState.kings & gameState.blackPieces;

    whiteKingIndex = bitscan_forward(whiteKings);
    blackKingIndex = bitscan_forward(blackKings);

    return 47 * CMD[whiteKingIndex] + 18 * (14 - get_manhattan_distance(whiteKingIndex, blackKingIndex));
}

u64 considerXrays(u64 attackersDefenders, int squareAttacked, u64 squareRemovedBB) {
    u64 xray;

    if ((squareRemovedBB & NORTH_EAST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & NORTH_EAST_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & NORTH_WEST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & NORTH_WEST_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & SOUTH_EAST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & SOUTH_EAST_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & SOUTH_WEST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & SOUTH_WEST_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & NORTH_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & NORTH_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & WEST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & WEST_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & SOUTH_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & SOUTH_LOOKUP_PATTERN[squareAttacked];
    }

    else if ((squareRemovedBB & EAST_LOOKUP_PATTERN[squareAttacked]) != 0) {
        xray = attackersDefenders & EAST_LOOKUP_PATTERN[squareAttacked];
    }
    else {
        return 0LLU;
    }

    return xray & -xray;
}

u64 getLeastValuablePiece(u64 attackersDefenders, int side, Board *board, int *attackPiece) {
    u64 leastValuablePiece;

    if (side == WHITE) {
        attackersDefenders &= board -> whitePieces;
    }
    else {
        attackersDefenders &= board -> blackPieces;
    }

    if ((board -> pawns & attackersDefenders) != 0) {
        leastValuablePiece = board -> pawns & attackersDefenders;
        *attackPiece = 0;
    }
    else if ((board -> knights & attackersDefenders) != 0) {
        leastValuablePiece = board -> knights & attackersDefenders;
        *attackPiece = 1;
    }
    else if ((board -> bishops & attackersDefenders) != 0) {
        leastValuablePiece = board -> bishops & attackersDefenders;
        *attackPiece = 2;
    }
    else if ((board -> rooks & attackersDefenders) != 0) {
        leastValuablePiece = board -> rooks & attackersDefenders;
        *attackPiece = 3;
    }
    else if ((board -> queens & attackersDefenders) != 0) {
        leastValuablePiece = board -> queens & attackersDefenders;
        *attackPiece = 4;
    }
    else if ((board -> kings & attackersDefenders) != 0) {
        leastValuablePiece = board -> kings & attackersDefenders;
        *attackPiece = 5;
    }
    else {
        return 0LLU;
    }

    return leastValuablePiece & -leastValuablePiece;
}

// static exchange evaluation (SEE)
int see(int from, int to, Board *board, int side, int targetPiece, int attackPiece) {
    int gain[32], d = 0;
    u64 mayXray = board -> pawns | board -> bishops | board -> rooks | board -> queens;
    u64 fromSet = SINGLE_BIT_LOOKUP[from];
    u64 occupied = board -> whitePieces | board -> blackPieces;
    u64 attackersDefenders = get_attack_defend_maps(to);
    gain[d] = pieceValues[targetPiece];
    do {
        d++; // next depth and side
        side *= -1;
        gain[d] = pieceValues[attackPiece] - gain[d-1]; // speculative store, if defended

        if (max(-gain[d-1], gain[d]) < 0) break; // pruning does not influence the result

        attackersDefenders ^= fromSet; // reset bit in set to traverse
        occupied ^= fromSet; // reset bit in temporary occupancy (for x-Rays)

        if ( fromSet & mayXray ) {
            attackersDefenders |= considerXrays(occupied, to, fromSet);
        }

        fromSet = getLeastValuablePiece(attackersDefenders, side, board, &attackPiece);

    } while (fromSet);

    while (--d) {
        gain[d-1] = -max(-gain[d-1], gain[d]);
    }

    return gain[0];
}

int evaluate(int side) {
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = gameState.whitePieces;
    u64 whitePawns = gameState.pawns & whitePieces;
    u64 whiteKnights = gameState.knights & whitePieces;
    u64 whiteBishops = gameState.bishops & whitePieces;
    u64 whiteRooks = gameState.rooks & whitePieces;
    u64 whiteQueens = gameState.queens & whitePieces;
    u64 whiteKings = gameState.kings & whitePieces;

    u64 blackPieces = gameState.blackPieces;
    u64 blackPawns = gameState.pawns & blackPieces;
    u64 blackKnights = gameState.knights & blackPieces;
    u64 blackBishops = gameState.bishops & blackPieces;
    u64 blackRooks = gameState.rooks & blackPieces;
    u64 blackQueens = gameState.queens & blackPieces;
    u64 blackKings = gameState.kings & blackPieces;
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

    u64 originalWhitePawns = whitePawns;
    u64 originalBlackPawns = blackPawns;

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
    u64 mask;


    // first calculate the opening score
    while (whitePawns != 0) {
        index = bitscan_forward(whitePawns);
        whitePawns &= (whitePawns - 1);
        whiteOpeningMaterial += PAWN_SCORE + WHITE_PAWN_PIECE_SQUARE_TABLE[index];
        
        mask = IS_PASSED_PAWN[0][index];
        // passed pawn
        if ((mask & originalBlackPawns) == 0) {
            whiteOpeningMaterial += 20;
        }

        mask = IS_ISOLATED_PAWN[index & 7];
        // isolated pawns
        if ((mask & originalWhitePawns) == 0) {
            whiteOpeningMaterial -= 20;
        }

        mask = IS_CONNECTED_PAWN[0][index];
        // unconnected pawns
        if ((mask & originalWhitePawns) == 0) {
            whiteOpeningMaterial -= 10;
        }

        mask = IS_STACKED_PAWN[0][index];
        // doubled, triple, etc pawns
        if ((mask & originalWhitePawns) != 0) {
            whiteOpeningMaterial -= 20;
        }

        wp++;
    }
    while (whiteKnights!= 0) {
        index = bitscan_forward(whiteKnights);
        whiteKnights &= (whiteKnights - 1);
        whiteOpeningMaterial += KNIGHT_SCORE + WHITE_KNIGHT_PIECE_SQUARE_TABLE[index];
        wn++;
    }
    while (whiteBishops != 0) {
        index = bitscan_forward(whiteBishops);
        whiteBishops &= (whiteBishops - 1);
        whiteOpeningMaterial += BISHOP_SCORE + WHITE_BISHOP_PIECE_SQUARE_TABLE[index];
        wb++;
    }
    while (whiteRooks != 0) {
        index = bitscan_forward(whiteRooks);
        whiteRooks &= (whiteRooks - 1);
        whiteOpeningMaterial += ROOK_SCORE + WHITE_ROOK_PIECE_SQUARE_TABLE[index];

        mask = FILE_MASK[index & 7];

        if ((mask & originalWhitePawns) == 0) {
            // open file
            if ((mask & originalBlackPawns) == 0) {
                whiteOpeningMaterial += 20;
            }
            // semi-open file
            else {
                whiteOpeningMaterial += 10;
            }
        }

        wr++;
    }
    while (whiteQueens != 0) {
        index = bitscan_forward(whiteQueens);
        whiteQueens &= (whiteQueens - 1);
        whiteOpeningMaterial += QUEEN_SCORE + WHITE_QUEEN_PIECE_SQUARE_TABLE[index];
        wq++;
    }

    // endgame and opening evaluate similarly until now
    whiteEndgameMaterial = whiteOpeningMaterial;

    // only 1 king can exist
    index = bitscan_forward(whiteKings);
    whiteOpeningMaterial += KING_SCORE + WHITE_KING_PIECE_SQUARE_TABLE_OPENING[index];
    whiteEndgameMaterial += KING_SCORE + WHITE_KING_PIECE_SQUARE_TABLE_ENDGAME[index];


    while (blackPawns != 0) {
        index = bitscan_forward(blackPawns);
        blackPawns &= (blackPawns - 1);
        blackOpeningMaterial += PAWN_SCORE + BLACK_PAWN_PIECE_SQUARE_TABLE[index];

        mask = IS_PASSED_PAWN[1][index];

        if ((mask & originalWhitePawns) == 0) {
            blackOpeningMaterial += 20;
        }

        mask = IS_ISOLATED_PAWN[index & 7];

        if ((mask & originalBlackPawns) == 0) {
            blackOpeningMaterial -= 20;
        }

        mask = IS_CONNECTED_PAWN[1][index];

        if ((mask & originalBlackPawns) == 0) {
            blackOpeningMaterial -= 10;
        }

        mask = IS_STACKED_PAWN[1][index & 7];
        // doubled, triple, etc pawns
        if ((mask & originalBlackPawns) != 0) {
            blackOpeningMaterial -= 20;
        }

        bp++;
    }
    while (blackKnights!= 0) {
        index = bitscan_forward(blackKnights);
        blackKnights &= (blackKnights - 1);
        blackOpeningMaterial += KNIGHT_SCORE+ BLACK_KNIGHT_PIECE_SQUARE_TABLE[index];
        bn++;
    }
    while (blackBishops != 0) {
        index = bitscan_forward(blackBishops);
        blackBishops &= (blackBishops - 1);
        blackOpeningMaterial += BISHOP_SCORE+ BLACK_BISHOP_PIECE_SQUARE_TABLE[index];
        bb++;
    }
    while (blackRooks != 0) {
        index = bitscan_forward(blackRooks);
        blackRooks &= (blackRooks - 1);
        blackOpeningMaterial += ROOK_SCORE+ BLACK_ROOK_PIECE_SQUARE_TABLE[index];

        mask = FILE_MASK[index & 7];

        if ((mask & originalBlackPawns) == 0) {
            // open file
            if ((mask & originalWhitePawns) == 0) {
                blackOpeningMaterial += 20;
            }
            // semi-open file
            else {
                blackOpeningMaterial += 10;
            }
        }

        br++;
    }
    while (blackQueens != 0) {
        index = bitscan_forward(blackQueens);
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
            return DRAW;
        }
    }

    // white can try to checkmate the opponent
    if ((whitePieceSum - blackPieceSum) >= 4 && blackPieceSum <= 9) {
        //printf("white mopup");
        mopUpScore = black_king_to_corners() * side;
    }
    // black can try to checkmate the opponent
    else if ((blackPieceSum - whitePieceSum) >= 4 && whitePieceSum <= 9) {
        //printf("black mopup");
        mopUpScore = -1 * white_king_to_corners() * side;
    }
    else {
        mopUpScore = 0;
    }

    // endgame and opening evaluate similarly until now
    blackEndgameMaterial = blackOpeningMaterial;

    // only 1 king can exist
    index = bitscan_forward(blackKings);
    blackOpeningMaterial += KING_SCORE + BLACK_KING_PIECE_SQUARE_TABLE_OPENING[index];;
    blackEndgameMaterial += KING_SCORE + BLACK_KING_PIECE_SQUARE_TABLE_ENDGAME[index];


    // tapered evaluation score
    phase = calc_phase(wp, bp, wn, bn, wb, bb, wr, br, wq, bq);
    openingScore = (whiteOpeningMaterial - blackOpeningMaterial) * side;
    endgameScore = (whiteEndgameMaterial - blackEndgameMaterial) * side;
    score = ((openingScore * (256 - phase)) + (endgameScore * phase)) / 256;
    score += mopUpScore;

    return score;
}

// assigns a score for each move so that they can later be sorted
void sort_moves_score(Move *movesArr, int end, int side, Move hashMove, bool foundHashMove) {
    int i;
    int j;
    int start = 0;
    int score;
    int quietScore;
    int MVV_Index;
    int LVA_Index;
    Move move;
    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = gameState.whitePieces;
    u64 blackPieces = gameState.blackPieces;

    u64 pawns = gameState.pawns;
    u64 knights = gameState.knights;
    u64 bishops = gameState.bishops;
    u64 rooks = gameState.rooks;
    u64 queens = gameState.queens;
    u64 kings = gameState.kings;

    // score moves
    for (i = start; i < end; i++) {
        move = movesArr[i];
        score = 0;
        quietScore = 0;

        // hash moves always first
        if (move.from == hashMove.from && move.to == hashMove.to && move.code == hashMove.code) {
            score += 10000;
        }

        if (move.code == CAPTURE_MOVE || move.code == KNIGHT_PROMOTION_CAPTURE_MOVE || move.code == BISHOP_PROMOTION_CAPTURE_MOVE || move.code == ROOK_PROMOTION_CAPTURE_MOVE || move.code == QUEEN_PROMOTION_CAPTURE_MOVE || move.code == EP_CAPTURE_MOVE) {
            if (square_occupied(pawns, move.from)) {
                LVA_Index = 0;
            }
            else if (square_occupied(knights, move.from)) {
                LVA_Index = 1;
            }
            else if (square_occupied(bishops, move.from)) {
                LVA_Index = 2;
            }
            else if (square_occupied(rooks, move.from)) {
                LVA_Index = 3;
            }
            else if (square_occupied(queens, move.from)) {
                LVA_Index = 4;
            }

            else if (square_occupied(kings, move.from)) {
                LVA_Index = 5;
            }

            if (square_occupied(pawns, move.to)) {
                MVV_Index = 0;
            }
            else if (square_occupied(knights, move.to)) {
                MVV_Index = 1;
            }
            else if (square_occupied(bishops, move.to)) {
                MVV_Index = 2;
            }
            else if (square_occupied(rooks, move.to)) {
                MVV_Index = 3;
            }
            else if (square_occupied(queens, move.to)) {
                MVV_Index = 4;
            }
            else {
                FILE *fp;
                fp = fopen("error.txt", "w");
                fprintf(fp, "error: move from: %d, to: %d, code: %d", move.from, move.to, move.code);
                fclose(fp);
                assert(0 != 0);
            }

            score = MVV_LVA[MVV_Index][LVA_Index];

            if (move.code == KNIGHT_PROMOTION_CAPTURE_MOVE) {
                score += PROMOTIONS[0];
            }
            else if (move.code == BISHOP_PROMOTION_CAPTURE_MOVE) {
                score += -PROMOTIONS[1];
            }
            else if (move.code == ROOK_PROMOTION_CAPTURE_MOVE) {
                score += PROMOTIONS[2];
            }
            else if (move.code == QUEEN_PROMOTION_CAPTURE_MOVE) {
                score += PROMOTIONS[3];
            }
        }
        else if (move.code == KNIGHT_PROMOTION_MOVE || move.code == BISHOP_PROMOTION_MOVE || move.code == ROOK_PROMOTION_MOVE || move.code == QUEEN_PROMOTION_MOVE) {
            if (move.code == KNIGHT_PROMOTION_MOVE) {
                score += PROMOTIONS[0];
            }
            else if (move.code == BISHOP_PROMOTION_MOVE) {
                score += PROMOTIONS[1];
            }
            else if (move.code == ROOK_PROMOTION_MOVE) {
                score += PROMOTIONS[2];
            }
            else if (move.code == QUEEN_PROMOTION_MOVE) {
                score += PROMOTIONS[3];
            }
            else {
                assert(0 != 0);
            }
        }
        else if (move.code == QUIET_MOVE) {
            if (g_ply > 0) {
                Move firstKillerMove = KILLER_MOVES[g_ply - 1][0];
                Move secondKillerMove = KILLER_MOVES[g_ply - 1][1];
                Move mateKillerMove = KILLER_MATE_MOVES[g_ply - 1];

                // killer moves before other quiet moves
                if (move.from == mateKillerMove.from && move.to == mateKillerMove.to && move.code == mateKillerMove.code) {
                    quietScore += INT_MAX;
                    mateKiller++;
                }
                else if (move.from == firstKillerMove.from && move.to == firstKillerMove.to && move.code == firstKillerMove.code) {
                    quietScore += INT_MAX - 1;
                    firstKiller++;
                }
                else if (move.from == secondKillerMove.from && move.to == secondKillerMove.to && move.code == secondKillerMove.code) {
                    quietScore += INT_MAX - 2;
                    secondKiller++;
                }

                else {
                    if (side == WHITE) {
                        quietScore = history[0][move.from][move.to];
                    }
                    else {
                        quietScore = history[1][move.from][move.to];
                    }
                }
            }
        }


        MOVE_SCORES[i] = score;
        QUIET_MOVE_SCORES[i] = quietScore;
    }
}

void selection_sort_one_move(Move *movesArr, int start, int end) {
    int maxScoreIndex = start;
    int maxQuietIndex = start;

    Move startMove = movesArr[start];
    int startQuetScore = QUIET_MOVE_SCORES[start];
    int startScore = MOVE_SCORES[start];

    int maxScore = startScore;
    int maxQuietScore = startQuetScore;
    int i;

    for (i = start + 1; i < end; i++) {
        if (MOVE_SCORES[i] > maxScore) {
            maxScore = MOVE_SCORES[i];
            maxScoreIndex = i;
        }
        else if (startMove.code == QUIET_MOVE && maxScoreIndex == start && QUIET_MOVE_SCORES[i] > maxQuietScore) {
            maxQuietScore = QUIET_MOVE_SCORES[i];
            maxQuietIndex = i;
        }
    }

    if (maxScoreIndex == start && maxQuietIndex == start) {
        return;
    }


    // swap best remaining move to the beginning
    if (maxScoreIndex != start) {
        movesArr[start] = movesArr[maxScoreIndex];
        movesArr[maxScoreIndex] = startMove;

        QUIET_MOVE_SCORES[start] = QUIET_MOVE_SCORES[maxScoreIndex];
        QUIET_MOVE_SCORES[maxScoreIndex] = startQuetScore;

        MOVE_SCORES[start] = MOVE_SCORES[maxScoreIndex];
        MOVE_SCORES[maxScoreIndex] = startScore;
    }
    else if (maxQuietIndex != start) {
        movesArr[start] = movesArr[maxQuietIndex];
        movesArr[maxQuietIndex] = startMove;

        QUIET_MOVE_SCORES[start] = QUIET_MOVE_SCORES[maxQuietIndex];
        QUIET_MOVE_SCORES[maxQuietIndex] = startQuetScore;

        MOVE_SCORES[start] = MOVE_SCORES[maxQuietIndex];
        MOVE_SCORES[maxQuietIndex] = startScore;
    }
    else {
        assert(0 != 0);
    }
}

// quiescent search comes after normal search and only looks for captures
int q_search(int alpha, int beta, int side) {
    int moveCount;
    int i;
    int score;
    Move *movesArr = g_moveStack[g_ply];
    u64 attackSet;
    int stand_pat = evaluate(side);
    //assert(evaluate(side) == -evaluate(-side));

    Board gameState = g_gameStateStack[g_root + g_ply];
    u64 whitePieces = gameState.whitePieces;
    u64 blackPieces = gameState.blackPieces;

    u64 pawns = gameState.pawns;
    u64 knights = gameState.knights;
    u64 bishops = gameState.bishops;
    u64 rooks = gameState.rooks;
    u64 queens = gameState.queens;
    u64 kings = gameState.kings;

    quiescentNodeCount++;

    if (g_cancelThread == 1) {
        printf("cancel\n");
        g_cancelThread = 0;
        pthread_exit(NULL);
    }

    if (stand_pat >= beta) {
        return beta;
    }

    // none of the moves can pass delta pruning
    if (stand_pat + QUEEN_SCORE < alpha - DELTA) {
        return alpha;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    moveCount = generate_captures(movesArr, side);
    sort_moves_score(movesArr, moveCount, side, create_move(0, 0, 0), false);

    for (i = 0; i < moveCount; i++)  {
        Move move;
        int attackPiece;
        int targetPiece;

        selection_sort_one_move(movesArr, i, moveCount);
        move = movesArr[i];

        if (square_occupied(pawns, move.from)) {
            attackPiece = 0;
        }
        else if (square_occupied(knights, move.from)) {
            attackPiece = 1;
        }
        else if (square_occupied(bishops, move.from)) {
            attackPiece = 2;
        }
        else if (square_occupied(rooks, move.from)) {
            attackPiece = 3;
        }
        else if (square_occupied(queens, move.from)) {
            attackPiece = 4;
        }
        else if (square_occupied(kings, move.from)) {
            attackPiece = 5;
        }
        else {
            goto afterCapturePruning;
        }


        if (square_occupied(pawns, move.to)) {
            targetPiece = 0;
        }
        else if (square_occupied(knights, move.to)) {
            targetPiece = 1;
        }
        else if (square_occupied(bishops, move.to)) {
            targetPiece = 2;
        }
        else if (square_occupied(rooks, move.to)) {
            targetPiece = 3;
        }
        else if (square_occupied(queens, move.to)) {
            targetPiece = 4;
        }
        else {
            goto afterCapturePruning;
        }

        // delta pruning
        if (pieceValues[targetPiece] < alpha - DELTA) {
            continue;
        }

        // prune moves with negative SEE
        if (attackPiece != 5 && targetPiece - attackPiece < 0 && see(move.from, move.to, &gameState, side, targetPiece, attackPiece) < 0) {
            continue;
        }

        afterCapturePruning:

        make_move(move, false);

        if (!is_king_threatened(side)) {
            score = -q_search(-beta, -alpha, -side);

            if (score >= beta) {
                unmake_move();
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

        unmake_move();
    }
    
    return alpha;
}

int negamax(int alpha, int beta, int depth, int side) {
    int moveCount;
    int legalMoveCount = 0;
    int i;
    int score;
    int bestScore;
    Move *movesArr = g_moveStack[g_ply];
    u64 attackSet;
    Move bestMove;
    bool alphaRise = false;
    //bool isInCheck = is_king_threatened(side);

    Move hashMove = create_move(0, 0, 0);
    bool foundHashMove = false;
    TranspositionTableEntry ttEntry;
    u64 zobristKey = g_zobristStack[g_root + g_ply];
    u64 ttIndex = zobristKey % TRANSPOSITION_TABLE_SIZE;

    ttEntry = tTable[ttIndex];

    if (g_cancelThread == 1) {
        g_cancelThread = 0;
        printf("cancel\n");
        pthread_exit(NULL);
    }

    // must be before transposition table probing
    if (is_repeating()) {
        score = DRAW;

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }

        return alpha;
    }

    if (ttEntry.zobristKey == zobristKey) {
        int ttDepth = ttEntry.depth;
        int ttNodeType = ttEntry.nodeType;
        hashMove = ttEntry.hashMove;

        if (ttNodeType == PV_NODE) {
            foundHashMove = true;
        }
        if (ttNodeType == CUT_NODE) {
            foundHashMove = true;
        }

        // if entry is deep enough, result can be used directly
        if (ttDepth >= depth) {
            transpositionCount++;

            // checkmate ply re-adjustment
            if (ttEntry.score > 100000) {
                ttEntry.score -= g_ply;
            }
            else if (ttEntry.score < -100000) {
                ttEntry.score += g_ply;
            }

            // score is exact
            if (ttNodeType == PV_NODE) {
                return ttEntry.score;
            }
            // score is lower bound
            else if (ttNodeType == CUT_NODE) {
                if (ttEntry.score > alpha) {
                    alpha = ttEntry.score;
                }
            }
            // score is upper bound
            else if (ttNodeType == ALL_NODE) {
                if (ttEntry.score < beta) {
                    beta = ttEntry.score;
                }
            }

            if (alpha >= beta) {
                return alpha;
            }
        }
    }

    if (depth <= 0) {
        nodeCount++;
        return q_search(alpha, beta, side);
    }

    moveCount = generate_moves(movesArr, side);
    sort_moves_score(movesArr, moveCount, side, hashMove, foundHashMove);

    for (i = 0; i < moveCount; i++)  {
        Move move;

        selection_sort_one_move(movesArr, i, moveCount);
        move = movesArr[i];
        make_move(move, false);

        if (!is_king_threatened(side)) {
            legalMoveCount++;

            score = -negamax(-beta, -alpha, depth - 1, -side);

            if (score > bestScore) {
                bestScore = score;
            }

            if (score >= beta) {
                clear_killers();
                unmake_move();
                add_tt_entry(zobristKey, score, move, CUT_NODE, depth);

                if (move.code == QUIET_MOVE) {
                    if (g_ply < 10) {
                        if (side == WHITE) {
                            history[0][move.from][move.to] = depth * depth;
                        }
                        else {
                            history[1][move.from][move.to] = depth * depth;
                        }
                    }

                    if (score > 100000) {
                        KILLER_MATE_MOVES[g_ply - 1] = move;
                    }
                    else {
                        if (KILLER_MOVES[g_ply - 1][0].from == 0 && KILLER_MOVES[g_ply - 1][0].to == 0) {
                            KILLER_MOVES[g_ply - 1][0] = move;
                        }
                        else {
                            KILLER_MOVES[g_ply - 1][1] = move;
                        }
                    }
                }

                betaCutOffsTotal[g_ply]++;
                if (legalMoveCount < 6) {
                    betaCutOffs[g_ply][legalMoveCount - 1]++;
                }


                return beta;
            }

            if (score > alpha) {
                alpha = score;
                bestMove = move;
                alphaRise = true;
            }
        }
        clear_killers();
        unmake_move();
    }

    // checkmate or draw, depends if king is safe or not
    if (legalMoveCount == 0) {
        if (!is_king_threatened(side)) {
            // a draw if king is not threatened
            score = DRAW;
        }
        else {
            // loses if king is lost => return a very bad score
            score = -CHECKMATE + g_ply;
        }

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }

        return alpha;
    }

    // PV node
    if (alphaRise) {
        add_tt_entry(zobristKey, alpha, bestMove, PV_NODE, depth);
    }
    // ALL node
    else {
        add_tt_entry(zobristKey, bestScore, create_move(0, 0, INVALID_MOVE), ALL_NODE, depth);
    }

    return alpha;
}


Move negamax_root(int alpha, int beta, int depth, int side) {
    int moveCount;
    int legalMoveCount = 0;
    int i;
    Move bestMove;
    bool alphaRise = false;
    bool foundHashMove = false;
    int aspirationWindow;
    //bool isInCheck = is_king_threatened(side);
    int score;
    int originalAlpha = alpha;
    int originalBeta = beta;
    Move *movesArr = g_moveStack[g_ply];
    u64 attackSet;

    Move hashMove = create_move(0, 0, 0);
    TranspositionTableEntry ttEntry;
    u64 zobristKey = g_zobristStack[g_root + g_ply];
    u64 ttIndex = zobristKey % TRANSPOSITION_TABLE_SIZE;

    ttEntry = tTable[ttIndex];

    if (g_cancelThread == 1) {
        g_cancelThread = 0;
        printf("cancel\n");
        pthread_exit(NULL);
    }

    printf("root\n");
    printf("alpha: %d, beta: %d\n", alpha, beta);
    fflush(stdout);

    if (ttEntry.zobristKey == zobristKey) {
        int ttDepth = ttEntry.depth;
        int ttNodeType = ttEntry.nodeType;
        hashMove = ttEntry.hashMove;

        if (ttNodeType == PV_NODE) {
            foundHashMove = true;
        }
        if (ttNodeType == CUT_NODE) {
            foundHashMove = true;
        }
    }

    moveCount = generate_moves(movesArr, side);
    sort_moves_score(movesArr, moveCount, side, hashMove, foundHashMove);

    rootSideToMove = get_side_to_play(g_gameStateStack[g_root + g_ply].meta);

    for (i = 0; i < moveCount; i++)  {
        Move move;

        selection_sort_one_move(movesArr, i, moveCount);
        move = movesArr[i];

        make_move(move, false);

        if (!is_king_threatened(side)) {
            legalMoveCount++;

            score = -negamax(-beta, -alpha, depth - 1, -side);

            // fail high aspiration window, have to re-search with a wider window
            if (score >= beta) {
                clear_killers();
                unmake_move();

                aspirationWindow = beta - currentScore;
                printf("aspiration window size fail high: %d\n", aspirationWindow);
                fflush(stdout);
                aspirationWindow *= 4;

                assert(aspirationWindow != 0);

                if (aspirationWindow > 100) {
                    return negamax_root(originalAlpha, ALPHA_BETA_MAX, depth, side);
                }

                return negamax_root(originalAlpha, currentScore + aspirationWindow, depth, side);
            }

            //printf("score: %d\n", score);
            //printf("from: %d, to: %d\n", move.from, move.to);
            if (score > alpha) {
                bestMove = move;
                alpha = score;
                alphaRise = true;

                // printf("score rise: %d\n", score);
                // printf("from: %d, to: %d\n", move.from, move.to);
            }
        }

        clear_killers();
        unmake_move();
    }

    //printf("best score: %d\n", alpha);
    //printf("best move: from: %d, to: %d\n", bestMove.from, bestMove.to);

    //printf("nodes: %d\n", nodeCount);

    // PV-node
    if (alphaRise) {
        add_tt_entry(zobristKey, alpha, bestMove, PV_NODE, depth);
        currentScore = alpha;

        if (alpha < -100000) {
            int distanceToMate = alpha + CHECKMATE;

            if (depth >= distanceToMate) {
                losingCheckMate = true;
            }
        }
        else if (alpha > 100000) {
            int distanceToMate = CHECKMATE - alpha;

            if (depth >= distanceToMate) {
                checkmate = true;
            }
        }
    }
    // fail low aspiration window, have to re-search with a wider window
    else {
        aspirationWindow = currentScore - alpha;
        printf("aspiration window size fail low: %d\n", aspirationWindow);
        fflush(stdout);
        aspirationWindow *= 4;

        assert(aspirationWindow != 0);

        if (aspirationWindow > 100) {
            return negamax_root(ALPHA_BETA_MIN, originalBeta, depth, side);
        }

        return negamax_root(currentScore - aspirationWindow, originalBeta, depth, side);
    }

    return bestMove;
}

// cleans up after this thread is terminated
void clean_up_handler(void *vargp) {
    g_ply = 0;
    printf("cleanup\n");
}

// a thread to calculate the best moves with iterative deepening
void *search(void *vargp) {
    searchedDepth = 0;
    Board gameState = g_gameStateStack[g_root];
    u64 otherGameInfo = gameState.meta;
    int side = get_side_to_play(otherGameInfo);
    clock_t start;
    clock_t end;
    int timeInMillis;
    int i;
    char *moveStr;

    printf("start search\n");
    fflush(stdout);

    pthread_cleanup_push(clean_up_handler, vargp);

    // used for debugging purposes
    zero_beta_cutoff();

    start = clock();

    currentScore = 0;

    checkmate = false;
    losingCheckMate = false;
    g_cancelThread = 0;

    foundMove = false;

    while (searchedDepth < MAX_DEPTH) {
        Move previousMove = g_selectedMove;
        nodeCount = 0;
        quiescentNodeCount = 0;
        transpositionCount = 0;

        mateKiller = 0;
        firstKiller = 0;
        secondKiller = 0;

        if (searchedDepth == 0) {
            // full search
            g_selectedMove = negamax_root(ALPHA_BETA_MIN, ALPHA_BETA_MAX, searchedDepth + 1, side);
        }
        else {
            // aspiration window
            printf("window around: %d and %d\n", currentScore - 25, currentScore + 25);
            fflush(stdout);
            g_selectedMove = negamax_root(currentScore - 25, currentScore + 25, searchedDepth + 1, side);
        }
        foundMove = true;

        end = clock();
        timeInMillis = (int)(((double) (end - start)) / CLOCKS_PER_SEC * 1000);

        printf("searched depth: %d ", searchedDepth);
        printf("nodes: %d ", nodeCount);
        printf("quiescent: %d ", quiescentNodeCount);
        printf("transpositions: %d ", transpositionCount);
        // time in seconds, millisecond precision
        printf("time: %d.%d ", timeInMillis / 1000, timeInMillis % 1000);
        printf("score: %d\n", currentScore);
        moveStr = (char*) malloc(8 * sizeof(char));
        move_to_string(moveStr, g_selectedMove);

        printf("pv: %s\n\n", moveStr);

        free(moveStr);

        for (i = 0; i <= searchedDepth; i++) {
            printf("beta-cut-offs: %f, %f, %f, total: %d\n", (float)betaCutOffs[i][0] * 100 / (float)betaCutOffsTotal[i], (float)betaCutOffs[i][1] * 100 / (float)betaCutOffsTotal[i], (float)betaCutOffs[i][2] * 100 / (float)betaCutOffsTotal[i], betaCutOffsTotal[i]);
        }
        
        printf("\nmate killers: %d, 1. killers: %d, 2. killers: %d\n\n", mateKiller, firstKiller, secondKiller);


        fflush(stdout);
        searchedDepth++;

        if (checkmate) {
            printf("checkmate\n");
            break;
        }
        else if (losingCheckMate) {
            checkmate = true;
            printf("losing checkmate\n");
            break;
        }

        zero_beta_cutoff();

        if (g_cancelThread == 1) {
            g_cancelThread = 0;
            printf("cancel\n");
            pthread_exit(NULL);
        }
    }

    pthread_cleanup_pop(1);
}