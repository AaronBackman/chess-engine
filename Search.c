#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

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
        index = bitscan_forward(whitePawns);
        whitePawns &= (whitePawns - 1);
        whiteOpeningMaterial += PAWN_SCORE + WHITE_PAWN_PIECE_SQUARE_TABLE[index];
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

void sort_moves(Move *movesArr, u64 attackSet, int end, int side) {
    int i;
    int start = 0;
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

    while (attackSet != 0) {
        // most valuable victim - least valuable attacker, is used to sort attacking moves
        int MVV_INDEX = -1;
        int LVA_INDEX = -1;

        u64 victims;
        u64 attackers;
        u64 leastValuedAttacker = 0LLU;

        if (side == 1) {
            if ((attackSet & blackQueens) != 0) {
                victims = attackSet & blackQueens;
            }
            else if ((attackSet & blackRooks) != 0) {
                victims = attackSet & blackRooks;
            }
            else if ((attackSet & blackBishops) != 0) {
                victims = attackSet & blackBishops;
            }
            else if ((attackSet & blackKnights) != 0) {
                victims = attackSet & blackKnights;
            }
            else if ((attackSet & blackPawns) != 0) {
                victims = attackSet & blackPawns;
            }
            // dont need to check for kings because king cant ever be captured before game ends in checkmate
            else {
                // no victims left to be attacked
                break;
            }

            MVV_INDEX = bitscan_forward(victims);
            attackers = get_threat_map(MVV_INDEX, -side);

            assert(MVV_INDEX != -1);

            while (attackers != 0) {
                if ((attackers & whitePawns) != 0) {
                    leastValuedAttacker = attackers & whitePawns;
                }
                else if ((attackers & whiteKnights) != 0) {
                    leastValuedAttacker = attackers & whiteKnights;
                }
                else if ((attackers & whiteBishops) != 0) {
                    leastValuedAttacker = attackers & whiteBishops;
                }
                else if ((attackers & whiteRooks) != 0) {
                    leastValuedAttacker = attackers & whiteRooks;
                }
                else if ((attackers & whiteQueens) != 0) {
                    leastValuedAttacker = attackers & whiteQueens;
                }
                else if ((attackers & whiteKings) != 0) {
                    leastValuedAttacker = attackers & whiteKings;
                }

                LVA_INDEX = bitscan_forward(leastValuedAttacker);
                attackers = empty_square(attackers, LVA_INDEX);

                assert(LVA_INDEX != -1);


                for (i = start; i < end; i++) {
                    Move move = movesArr[i];

                    // if found the MVV-LVA move, sort it to the beginning
                    if (move.from == LVA_INDEX && move.to == MVV_INDEX) {
                        Move m = movesArr[start];
                        movesArr[start] = move;
                        movesArr[i] = m;
                        start++;
                        break;
                    }
                }
            }

            attackSet = empty_square(attackSet, MVV_INDEX);
        }
        else {
            if ((attackSet & whiteQueens) != 0) {
                victims = attackSet & whiteQueens;
            }
            else if ((attackSet & whiteRooks) != 0) {
                victims = attackSet & whiteRooks;
            }
            else if ((attackSet & whiteBishops) != 0) {
                victims = attackSet & whiteBishops;
            }
            else if ((attackSet & whiteKnights) != 0) {
                victims = attackSet & whiteKnights;
            }
            else if ((attackSet & whitePawns) != 0) {
                victims = attackSet & whitePawns;
            }
            // dont need to check for kings because king cant ever be captured before game ends in checkmate
            else {
                // no victims left to be attacked
                break;
            }

            MVV_INDEX = bitscan_forward(victims);
            attackers = get_threat_map(MVV_INDEX, -side);

            assert(MVV_INDEX != -1);

            while (attackers != 0) {
                if ((attackers & blackPawns) != 0) {
                    leastValuedAttacker = attackers & blackPawns;
                }
                else if ((attackers & blackKnights) != 0) {
                    leastValuedAttacker = attackers & blackKnights;
                }
                else if ((attackers & blackBishops) != 0) {
                    leastValuedAttacker = attackers & blackBishops;
                }
                else if ((attackers & blackRooks) != 0) {
                    leastValuedAttacker = attackers & blackRooks;
                }
                else if ((attackers & blackQueens) != 0) {
                    leastValuedAttacker = attackers & blackQueens;
                }
                else if ((attackers & blackKings) != 0) {
                    leastValuedAttacker = attackers & blackKings;
                }

                LVA_INDEX = bitscan_forward(leastValuedAttacker);
                attackers = empty_square(attackers, LVA_INDEX);

                assert(LVA_INDEX != -1);


                for (i = start; i < end; i++) {
                    Move move = movesArr[i];

                    // if found the MVV-LVA move, sort it to the beginning
                    if (move.from == LVA_INDEX && move.to == MVV_INDEX) {
                        Move m = movesArr[start];
                        movesArr[start] = move;
                        movesArr[i] = m;
                        start++;
                        break;
                    }
                }
            }

            attackSet = empty_square(attackSet, MVV_INDEX);
        }
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

    quiescentNodeCount++;

    if (g_cancelThread == 1) {
        printf("cancel\n");
        g_cancelThread = 0;
        pthread_exit(NULL);
    }

    if (stand_pat >= beta) {
        return beta;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    moveCount = generate_captures(movesArr, side);
    // attackset is generated alongside the moves
    attackSet = g_attackSets[g_ply];
    sort_moves(movesArr, attackSet, moveCount, side);

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];

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

    Move hashMove;
    bool foundHashMove = false;
    TranspositionTableEntry ttEntry;
    u64 zobristKey = g_zobristStack[g_root + g_ply];
    u64 ttIndex = zobristKey % TRANSPOSITION_TABLE_SIZE;

    ttEntry = tTable[ttIndex];

    if (ttEntry.zobristKey == zobristKey) {
        int ttDepth = ttEntry.depth;
        int ttNodeType = ttEntry.nodeType;
        hashMove = ttEntry.hashMove;

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

    if (g_cancelThread == 1) {
        g_cancelThread = 0;
        printf("cancel\n");
        pthread_exit(NULL);
    }

    /*
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
    */

    if (depth <= 0) {
        nodeCount++;
        return q_search(alpha, beta, side);
    }

    moveCount = generate_moves(movesArr, side);
    // attackset is generated alongside the moves
    attackSet = g_attackSets[g_ply];
    sort_moves(movesArr, attackSet, moveCount, side);

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];
        make_move(move, false);

        if (!is_king_threatened(side)) {
            legalMoveCount++;

            score = -negamax(-beta, -alpha, depth - 1, -side);

            if (score > bestScore) {
                bestScore = score;
            }

            if (score >= beta) {
                unmake_move();
                add_tt_entry(zobristKey, score, move, CUT_NODE, depth);
                return beta;
            }

            if (score > alpha) {
                alpha = score;
                bestMove = move;
                alphaRise = true;
            }
        }
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
    int illegalMoveCount = 0;
    int i;
    Move bestMove;
    bool alphaRise = false;
    bool foundHashMove = false;
    int score;
    Move *movesArr = g_moveStack[g_ply];
    u64 attackSet;

    Move hashMove;
    TranspositionTableEntry ttEntry;
    u64 zobristKey = g_zobristStack[g_root + g_ply];
    u64 ttIndex = zobristKey % TRANSPOSITION_TABLE_SIZE;

    ttEntry = tTable[ttIndex];

    moveCount = generate_moves(movesArr, side);
    // attackset is generated alongside the moves
    attackSet = g_attackSets[g_ply];
    sort_moves(movesArr, attackSet, moveCount, side);

    rootSideToMove = get_side_to_play(g_gameStateStack[g_root + g_ply].meta);

    for (i = 0; i < moveCount; i++)  {
        Move move = movesArr[i];

        make_move(move, false);

        if (!is_king_threatened(side)) {
            score = -negamax(-beta, -alpha, depth - 1, -side);

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

        unmake_move();
    }

    //printf("best score: %d\n", alpha);
    //printf("best move: from: %d, to: %d\n", bestMove.from, bestMove.to);

    //printf("nodes: %d\n", nodeCount);

    // PV-node
    if (alphaRise) {
        add_tt_entry(zobristKey, alpha, bestMove, PV_NODE, depth);
    }

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

    currentScore = alpha;

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

    pthread_cleanup_push(clean_up_handler, vargp);

    currentScore = 0;

    checkmate = false;
    losingCheckMate = false;
    g_cancelThread = 0;

    while (searchedDepth < MAX_DEPTH) {
        Move previousMove = g_selectedMove;
        nodeCount = 0;
        quiescentNodeCount = 0;
        transpositionCount = 0;

        g_selectedMove = negamax_root(ALPHA_BETA_MIN, ALPHA_BETA_MAX, searchedDepth + 1, side);

        printf("searched depth: %d ", searchedDepth);
        printf("nodes: %d ", nodeCount);
        printf("quiescent: %d ", quiescentNodeCount);
        printf("transpositions: %d ", transpositionCount);
        printf("score: %d\n", currentScore);
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
    }

    pthread_cleanup_pop(1);
}