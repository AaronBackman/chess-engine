#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char BYTE;
typedef struct move Move;

#define DEPTH 4
#define QUIESCENT_DEPTH 10
#define MAX_DEPTH DEPTH + QUIESCENT_DEPTH
#define BOARD_SIZE 64
// average possible moves per turn
#define BRANCHING_FACTOR 35
#define MAX_TURNS 1024
#define MAX_MOVES 500
// affects likelihood of accepting draws, negative for avoiding draws
#define CONTEMPT_FACTOR -100

int nodes = 0;
int quiescentNodes = 0;
int ENDGAME = 0;



int WHITE_PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, -20, -20, 10, 10, 5,
    5, -5, -10, 0, 0, -10, -5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, 5, 10, 25, 25, 10, 5, 5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0
};

int WHITE_PAWN_TABLE_ENDGAME[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    -30, -30, -30, -30, -30, -30, -30, -30,
    -10, -10, -10, -10, -10, -10, -10, -10,
    10, 10, 10, 10, 10, 10, 10, 10,
    20, 20, 20, 20, 20, 20, 20, 20,
    30, 30, 30, 30, 30, 30, 30, 30,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0
};

int BLACK_PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};

int BLACK_PAWN_TABLE_ENDGAME[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    30, 30, 30, 30, 30, 30, 30, 30,
    20, 20, 20, 20, 20, 20, 20, 20,
    10, 10, 10, 10, 10, 10, 10, 10,
    -10, -10, -10, -10, -10, -10, -10, -10,
    -30, -30, -30, -30, -30, -30, -30, -30,
    0, 0, 0, 0, 0, 0, 0, 0,
};

int WHITE_KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

int BLACK_KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

int WHITE_BISHOP_TABLE[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

int BLACK_BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

int WHITE_ROOK_TABLE[64] = {
    0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

int BLACK_ROOK_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

int WHITE_QUEEN_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
    0,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
};

int BLACK_QUEEN_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

int WHITE_KING_TABLE[64] = {
    20, 30, 10,  0,  0, 10, 30, 20,
    20, 20,  0,  0,  0,  0, 20, 20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30
};

int WHITE_KING_TABLE_ENDGAME[64] = {
    -10, -10, -10,  -10,  -10, -10, -10, -10,
    -10, 10, 10,  10,  10, 10, 10, -10,
    -10, 10, 30,  30,  30, 30, 10, -10,
    -10, 10, 30,  50,  50, 30, 10, -10,
    -10, 10, 30,  50,  50, 30, 10, -10,
    -10, 10, 30,  30,  30, 30, 10, -10,
    -10, 10, 10,  10,  10, 10, 10, -10,
    -10, -10, -10,  -10,  -10, -10, -10, -10
};

int BLACK_KING_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

int BLACK_KING_TABLE_ENDGAME[64] = {
    -10, -10, -10,  -10,  -10, -10, -10, -10,
    -10, 10, 10,  10,  10, 10, 10, -10,
    -10, 10, 30,  30,  30, 30, 10, -10,
    -10, 10, 30,  50,  50, 30, 10, -10,
    -10, 10, 30,  50,  50, 30, 10, -10,
    -10, 10, 30,  30,  30, 30, 10, -10,
    -10, 10, 10,  10,  10, 10, 10, -10,
    -10, -10, -10,  -10,  -10, -10, -10, -10
};



u64 GAME_STATE_STACK[MAX_DEPTH + MAX_TURNS][15];
int GAME_STATE_STACK_POINTER = -1;

struct move {
    u32 from : 6;
    u32 to : 6;
    u32 promotion : 3;
    u32 castling : 2;
    u32 checkmate : 1;
};

int MOVE_STACK_POINTER = 0;
Move MOVE_STACK[MAX_DEPTH][MAX_MOVES];

Move createMove(int from, int to, int promotion, int castle, int checkmate) {
    Move move = {from, to, promotion, castle, checkmate};

    return move;
}

bool squareOccupied(u64 board, int index) {
    u64 bitIndex = (1ULL << index);

    if ((board & bitIndex) == 0LLU) {
        return false;
    }
    else return true;
}

bool kingInDanger(u64 *gameState, int side) {
    // king not in danger
    return false;
}

u64 emptySquare(u64 board, int index) {
    u64 bitIndex = (1LLU << index);

    board &= ~bitIndex;

    return board;
}

u64 fillSquare(u64 board, int index) {
    u64 bitIndex = (1LLU << index);

    board |= bitIndex;

    return board;
}

void makeMove(Move move) {
    int from = move.from;
    int to = move.to;
    int promotion = move.promotion;
    int castle = move.castling;

    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    u64 *newGameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER + 1];

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

    u64 otherGameInfo = gameState[14];
    

    // castling is handled separately because it is the only move that moves 2 pieces at the same time
    if (castle != 0) {
        // white moving
        if (squareOccupied(whiteKings, 4)) {
            // set white castling to 0
            otherGameInfo &= ~(11LLU);

            // kingside
            if (castle == 1) {
                whiteKings = emptySquare(whiteKings, 4);
                whiteKings = fillSquare(whiteKings, 6);

                whiteRooks = emptySquare(whiteRooks, 7);
                whiteRooks = fillSquare(whiteRooks, 5);
            }

            // queenside
            else if (castle == 2) {
                whiteKings = emptySquare(whiteKings, 4);
                whiteKings = fillSquare(whiteKings, 2);

                whiteRooks = emptySquare(whiteRooks, 0);
                whiteRooks = fillSquare(whiteRooks, 3);
            }
        }

        // black moving
        else {
            // set black castling to 0
            otherGameInfo &= ~(11LLU << 2);

            // kingside
            if (castle == 1) {
                blackKings = emptySquare(blackKings, 60);
                blackKings = fillSquare(blackKings, 62);

                blackRooks = emptySquare(blackRooks, 63);
                blackRooks = fillSquare(blackRooks, 61);
            }

            // queenside
            else if (castle == 2) {
                blackKings = emptySquare(blackKings, 60);
                blackKings = fillSquare(blackKings, 58);

                blackRooks = emptySquare(blackRooks, 56);
                blackRooks = fillSquare(blackRooks, 59);
            }
        }
    }

    // a normal move or promotion or en passant
    else {
        if (squareOccupied(whitePieces, to)) {
            whitePieces = emptySquare(whitePieces, to);
        }

        if (squareOccupied(whitePawns, to)) {
            whitePawns = emptySquare(whitePawns, to);
        }

        if (squareOccupied(whiteKnights, to)) {
            whiteKnights = emptySquare(whiteKnights, to);
        }
        
        if (squareOccupied(whiteBishops, to)) {
            whiteBishops = emptySquare(whiteBishops, to);
        }
        
        if (squareOccupied(whiteRooks, to)) {
            whiteRooks = emptySquare(whiteRooks, to);
        }
        
        if (squareOccupied(whiteQueens, to)) {
            whiteQueens = emptySquare(whiteQueens, to);
        }
        
        if (squareOccupied(whiteKings, to)) {
            whiteKings = emptySquare(whiteKings, to);
        }



        if (squareOccupied(blackPieces, to)) {
            blackPieces = emptySquare(blackPieces, to);
        }

        if (squareOccupied(blackPawns, to)) {
            blackPawns = emptySquare(blackPawns, to);
        }

        if (squareOccupied(blackKnights, to)) {
            blackKnights = emptySquare(blackKnights, to);
        }
        
        if (squareOccupied(blackBishops, to)) {
            blackBishops = emptySquare(blackBishops, to);
        }
        
        if (squareOccupied(blackRooks, to)) {
            blackRooks = emptySquare(blackRooks, to);
        }
        
        if (squareOccupied(blackQueens, to)) {
            blackQueens = emptySquare(blackQueens, to);
        }
        
        if (squareOccupied(blackKings, to)) {
            blackKings = emptySquare(blackKings, to);
        }



        if (squareOccupied(whitePieces, from)) {
            whitePieces = emptySquare(whitePieces, from);
            whitePieces = fillSquare(whitePieces, to);
        }

        if (squareOccupied(whitePawns, from)) {
            whitePawns = emptySquare(whitePawns, from);
            whitePawns = fillSquare(whitePawns, to);
        }

        if (squareOccupied(whiteKnights, from)) {
            whiteKnights = emptySquare(whiteKnights, from);
            whiteKnights = fillSquare(whiteKnights, to);
        }
        
        if (squareOccupied(whiteBishops, from)) {
            whiteBishops = emptySquare(whiteBishops, from);
            whiteBishops = fillSquare(whiteBishops, to);
        }
        
        if (squareOccupied(whiteRooks, from)) {
            whiteRooks = emptySquare(whiteRooks, from);
            whiteRooks = fillSquare(whiteRooks, to);
        }
        
        if (squareOccupied(whiteQueens, from)) {
            whiteQueens = emptySquare(whiteQueens, from);
            whiteQueens = fillSquare(whiteQueens, to);
        }
        
        if (squareOccupied(whiteKings, from)) {
            whiteKings = emptySquare(whiteKings, from);
            whiteKings = fillSquare(whiteKings, to);
        }



        if (squareOccupied(blackPieces, from)) {
            blackPieces = emptySquare(blackPieces, from);
            blackPieces = fillSquare(blackPieces, to);
        }

        if (squareOccupied(blackPawns, from)) {
            blackPawns = emptySquare(blackPawns, from);
            blackPawns = fillSquare(blackPawns, to);
        }

        if (squareOccupied(blackKnights, from)) {
            blackKnights = emptySquare(blackKnights, from);
            blackKnights = fillSquare(blackKnights, to);
        }
        
        if (squareOccupied(blackBishops, from)) {
            blackBishops = emptySquare(blackBishops, from);
            blackBishops = fillSquare(blackBishops, to);
        }
        
        if (squareOccupied(blackRooks, from)) {
            blackRooks = emptySquare(blackRooks, from);
            blackRooks = fillSquare(blackRooks, to);
        }
        
        if (squareOccupied(blackQueens, from)) {
            blackQueens = emptySquare(blackQueens, from);
            blackQueens = fillSquare(blackQueens, to);
        }
        
        if (squareOccupied(blackKings, from)) {
            blackKings = emptySquare(blackKings, from);
            blackKings = fillSquare(blackKings, to);
        }

        // handle promotion of pawn
        if (promotion != 0) {
            //printf("promotion: %d, from: %d, to: %d\n", promotion, from, to);
            if (squareOccupied(whitePawns, to)) {
                whitePawns = emptySquare(whitePawns, to);

                // promote to knight
                if (promotion == 1) {
                    whiteKnights = fillSquare(whiteKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    whiteBishops = fillSquare(whiteBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    whiteRooks == fillSquare(whiteRooks, to);
                }
                // promote to queen
                else if (promotion == 4) {
                    whiteQueens == fillSquare(whiteQueens, to);
                }

                else {
                    printf("promotion something went wrong");
                }
            }
            else {
                blackPawns = emptySquare(blackPawns, to);

                // promote to knight
                if (promotion == 1) {
                    blackKnights = fillSquare(blackKnights, to);
                }
                // promote to bishop
                else if (promotion == 2) {
                    blackBishops = fillSquare(blackBishops, to);
                }
                // promote to rook
                else if (promotion == 3) {
                    blackRooks == fillSquare(blackRooks, to);
                }
                // promote to queen
                else if (promotion == 4) {
                    blackQueens == fillSquare(blackQueens, to);
                }

                else {
                    printf("promotion something went wrong");
                }
            }
        }
    }

    newGameState[0] = whitePieces;
    newGameState[1] = whitePawns;
    newGameState[2] = whiteKnights;
    newGameState[3] = whiteBishops;
    newGameState[4] = whiteRooks;
    newGameState[5] = whiteQueens;
    newGameState[6] = whiteKings;

    newGameState[7] = blackPieces;
    newGameState[8] = blackPawns;
    newGameState[9] = blackKnights;
    newGameState[10] = blackBishops;
    newGameState[11] = blackRooks;
    newGameState[12] = blackQueens;
    newGameState[13] = blackKings;

    newGameState[14] = otherGameInfo;
}

int addPawnMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];
    const int row = boardIndex / 8;
    const int column = boardIndex % 8;

    // white
    if (side == 1) {
        if (!squareOccupied(whitePieces, boardIndex + 8) && !squareOccupied(blackPieces, boardIndex + 8)) {
            movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 0, 0, 0);
            moveIndex++;

            if (row == 6) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 8, 4, 0, 0);
                moveIndex++;
            }

            if ((row == 1) && (!squareOccupied(whitePieces, boardIndex + 16) && !squareOccupied(blackPieces, boardIndex + 16))) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 16, 0, 0, 0);
                moveIndex++;
            }
        }

        if (row == 6) {
            // take top left piece and promote
            if (column != 0 && squareOccupied(blackPieces, boardIndex + 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 4, 0, 0);
                moveIndex++;
            }

            // take top right piece and promote
            if (column != 7 && squareOccupied(blackPieces, boardIndex + 9)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 4, 0, 0);
                moveIndex++;
            }
        }

        else {
            // take top left piece
            if (column != 0 && squareOccupied(blackPieces, boardIndex + 7)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 7, 0, 0, 0);
                moveIndex++;
            }

            // take top right piece
            if (column != 7 && squareOccupied(blackPieces, boardIndex + 9)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex + 9, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // black
    else if (side == -1) {
        if (!squareOccupied(blackPieces, boardIndex - 8) && !squareOccupied(whitePieces, boardIndex - 8)) {
            movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 0, 0, 0);
            moveIndex++;

            if (row == 1) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 8, 4, 0, 0);
                moveIndex++;
            }

            if ((row == 6) && (!squareOccupied(blackPieces, boardIndex - 16) && !squareOccupied(whitePieces, boardIndex - 16))) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 16, 0, 0, 0);
                moveIndex++;
            }
        }

        if (row == 1) {
            // take bottom left piece and promote
            if (column != 0 && squareOccupied(whitePieces, boardIndex - 9)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 4, 0, 0);
                moveIndex++;
            }

            // take bottom right piece and promote
            if (column != 7 && squareOccupied(whitePieces, boardIndex - 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 1, 0, 0);
                moveIndex++;

                // promote to bishop
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 2, 0, 0);
                moveIndex++;

                // promote to rook
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 3, 0, 0);
                moveIndex++;

                // promote to queen
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 4, 0, 0);
                moveIndex++;
            }
        }

        else {
            // take bottom left piece
            if (column != 0 && squareOccupied(whitePieces, boardIndex - 9)) {
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 9, 0, 0, 0);
                moveIndex++;
            }

            // take bottom right piece
            if (column != 7 && squareOccupied(whitePieces, boardIndex - 7)) {
                // promote to knight
                movesArr[moveIndex] = createMove(boardIndex, boardIndex - 7, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    return moveIndex;
}

int addKnightMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    destinationSquare = boardIndex + i * 8 + j;

                    // cant move outside the board
                    if (destinationSquare > 63 || destinationSquare < 0) continue;

                    if (!squareOccupied(whitePieces, destinationSquare)) {
                        movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                        moveIndex++;
                    }
                }
            }
        }
    }

    // black
    else if (side == -1) {
        for (i = -2; i < 3; i++) {
            for (j = -2; j < 3; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                if (j * i == 2 || j * i == -2) {
                    destinationSquare = boardIndex + i * 8 + j;

                    // cant move outside the board
                    if (destinationSquare > 63 || destinationSquare < 0) continue;

                    if (!squareOccupied(blackPieces, destinationSquare)) {
                        movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                        moveIndex++;
                    }
                }
            }
        }
    }

    return moveIndex;
}

int addBishopMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        j = 1;
        // top right diagonal
        for (i = 9; boardIndex + i < 64 && boardIndex % 8 + j < 8; i += 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // top left diagonal
        for (i = 7; boardIndex + i < 64 && boardIndex % 8 + j >= 0; i += 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }

        j = 1;
        // bottom right diagonal
        for (i = -7; boardIndex + i >= 0 && boardIndex % 8 + j < 8; i -= 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // bottom left diagonal
        for (i = -9; boardIndex + i >= 0 && boardIndex % 8 + j >= 0; i -= 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }
    }

    // black
    else if (side == -1) {
        j = 1;
        // top right diagonal
        for (i = 9; boardIndex + i < 64 && boardIndex % 8 + j < 8; i += 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // top left diagonal
        for (i = 7; boardIndex + i < 64 && boardIndex % 8 + j >= 0; i += 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }

        j = 1;
        // bottom right diagonal
        for (i = -7; boardIndex + i >= 0 && boardIndex % 8 + j < 8; i -= 7) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j++;
        }

        j = -1;
        // bottom left diagonal
        for (i = -9; boardIndex + i >= 0 && boardIndex % 8 + j >= 0; i -= 9) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }

            j--;
        }
    }

    return moveIndex;
}

int addRookMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        // top
        for (i = 8; boardIndex + i < 64; i += 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // right
        for (i = 1; boardIndex % 8 + i < 8; i += 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // bottom
        for (i = -8; boardIndex + i >= 0; i -= 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // left
        for (i = -1; boardIndex % 8 + i >= 0; i -= 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(whitePieces, destinationSquare)) break;

            else if (squareOccupied(blackPieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    // black
    else if (side == -1) {
        // top
        for (i = 8; boardIndex + i < 64; i += 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // right
        for (i = 1; boardIndex % 8 + i < 8; i += 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // bottom
        for (i = -8; boardIndex + i >= 0; i -= 8) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }

        // left
        for (i = -1; boardIndex % 8 + i >= 0; i -= 1) {
            destinationSquare = boardIndex + i;

            if (squareOccupied(blackPieces, destinationSquare)) break;

            else if (squareOccupied(whitePieces, destinationSquare)) {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
                break;
            }

            else {
                movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                moveIndex++;
            }
        }
    }

    return moveIndex;
}

int addKingMoves(u64 *gameState, int boardIndex, Move *movesArr, int moveIndex, int side) {
    int i;
    int j;
    int destinationSquare;
    u64 whitePieces = gameState[0];
    u64 blackPieces = gameState[7];

    // white
    if (side == 1) {
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                // would not moving anywhere
                if (i == 0 && j == 0) continue;

                destinationSquare = boardIndex + i * 8 + j;

                // outside the board
                if (destinationSquare < 0 || destinationSquare > 63) continue;

                if (!squareOccupied(whitePieces, destinationSquare)) {
                    movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                    moveIndex++;
                }
             }
        }
    }

    // black
    else if (side == -1) {
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

                // would not moving anywhere
                if (i == 0 && j == 0) continue;

                destinationSquare = boardIndex + i * 8 + j;

                // outside the board
                if (destinationSquare < 0 || destinationSquare > 63) continue;

                if (!squareOccupied(blackPieces, destinationSquare)) {
                    movesArr[moveIndex] = createMove(boardIndex, destinationSquare, 0, 0, 0);
                    moveIndex++;
                }
             }
        }
    }

    return moveIndex;
}

// board coordinates are from 0 to 63 and start from bottom left from
// whites point of view, they go left to right, bottom to top

// a single move: 32-bit unsigned int, 8 bits: original location,
// 8 bits destination location,
// 3 bits promotion (queen, rook, bishop, knight or none)
// 2 bits for castling (queenside, kingside or none)
// in case of castling, the castling king is determined by the original location

// game state: an array consisting of 13 unsigned 64-bit integers (long long)
// first 12 of those use used to contain location of a certain piece
// such as white pawn in the 64 squares of the board
// so if for example 45:th bit is 1, the 44:th (0-index) square of
// the board has a white pawn

// the game state contains pawns, knights, bishops, rooks, queens and king
// in that order first for white then for black
// the last number (13:th) is used to store other useful data about the game
// such as castling rights (2 bits for both sides), en passant (8 bits) and information concerning
// the 3-fold repetition (4 bits) and the 50-move rule (9 bits)

// side: 0 == white, 1 == black
int generateMoves(u64 *gameState, Move *movesArr, int side) {
    int i;
    int isKingInDanger;

    // next free index in moves array
    int moveIndex = 0;
    u64 bitIndex;


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

    u64 otherGameInfo = gameState[14];

    // white
    if (side == 1) {
        for (i = 0; i < BOARD_SIZE; i++) {
            bitIndex = 1ULL << i;

            // check if square doesnt have a white piece
            if (!squareOccupied(whitePieces, i)) continue;

            // remove piece to see if it causes king to be in check and do extra checking later
            whitePieces &= ~bitIndex;
            // is white king in danger
            if (kingInDanger(gameState, side)) {
                isKingInDanger = 1;
            } else {
                isKingInDanger = 0;
            }
            // add the piece back
            whitePieces |= bitIndex;

            // check if square has a white pawn
            if (squareOccupied(whitePawns, i)) {
                moveIndex = addPawnMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, pawn gen\n", moveIndex);
            }

            else if (squareOccupied(whiteKnights, i)) {
                moveIndex = addKnightMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, knight gen\n", moveIndex);
            }

            else if (squareOccupied(whiteBishops, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, bishop gen\n", moveIndex);
            }

            else if (squareOccupied(whiteRooks, i)) {
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, rook gen\n", moveIndex);
            }

            else if (squareOccupied(whiteQueens, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, queen gen\n", moveIndex);
            }
            
            else if (squareOccupied(whiteKings, i)) {
                moveIndex = addKingMoves(gameState, i, movesArr, moveIndex, side);
                //printf("moveindex :%d, king gen\n", moveIndex);
            }
        }
    }
    // black
    else {
        for (i = 0; i < BOARD_SIZE; i++) {
            // check if square doesnt have a black piece
            if (!blackPieces & (1ULL << i)) continue;

            // remove piece to see if it causes king to be in check and do extra checking later
            blackPieces &= ~bitIndex;
            // is white king in danger
            if (kingInDanger(gameState, side)) {
                isKingInDanger = 1;
            } else {
                isKingInDanger = 0;
            }
            // add the piece back
            blackPieces |= bitIndex;

            // check if square has a white pawn
            if (squareOccupied(blackPawns, i)) {
                moveIndex = addPawnMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackKnights, i)) {
                moveIndex = addKnightMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackBishops, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackRooks, i)) {
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
            }

            else if (squareOccupied(blackQueens, i)) {
                moveIndex = addBishopMoves(gameState, i, movesArr, moveIndex, side);
                moveIndex = addRookMoves(gameState, i, movesArr, moveIndex, side);
            }
            
            else if (squareOccupied(blackKings, i)) {
                moveIndex = addKingMoves(gameState, i, movesArr, moveIndex, side);
            }
        }
    }

    //printf("end gen\n\n");

    return moveIndex;
}

// checks if a move would take a king
int checkIfCheckMate(Move move, int side) {
    int i;
    int moveCount;
    Move generatedMove;
    u64 *gameState;
    u64 whiteKings;
    u64 blackKings;
    Move *movesArr;

    makeMove(move);
    MOVE_STACK_POINTER++;
    GAME_STATE_STACK_POINTER++;
    gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
    whiteKings = gameState[6];
    blackKings = gameState[13];
    GAME_STATE_STACK_POINTER--;

    movesArr = MOVE_STACK[MOVE_STACK_POINTER];
    // generate moves for opponent side and check if any could take your king in the square you are moving (t0)
    moveCount = generateMoves(gameState, movesArr, -side);
    for (i = 0; i < moveCount; i++) {
        generatedMove = movesArr[i];

        if (side == 1 && squareOccupied(whiteKings, generatedMove.to)) {
            printf("attack from: %d, and king in: %d\n", generatedMove.from, i);
            MOVE_STACK_POINTER--;
            return 1;
        }
        else if (side == -1 && squareOccupied(blackKings, generatedMove.to)) {
            MOVE_STACK_POINTER--;
            return 1;
        }
    }

    MOVE_STACK_POINTER--;
    return 0;
}

void printBoard() {
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    //printf("stackpointer: %d", GAME_STATE_STACK_POINTER);

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

    u64 otherGameInfo = gameState[14];
    
    int i = 56;
    

    while (i >= 0 && i < 64) {
        printf(" ");

        if (!squareOccupied(whitePieces, i) && !squareOccupied(blackPieces, i)) {
            printf("__");
        }

        if (squareOccupied(whitePieces, i)) printf("+");
        if (squareOccupied(blackPieces, i)) printf("#");

        if (squareOccupied(whitePawns, i)) printf("P");

        if (squareOccupied(whiteBishops, i)) printf("B");

        if (squareOccupied(whiteKnights, i)) printf("N");

        if (squareOccupied(whiteRooks, i)) printf("R");

        if (squareOccupied(whiteQueens, i)) printf("Q");

        if (squareOccupied(whiteKings, i)) printf("K");

        if (squareOccupied(blackPawns, i)) printf("p");

        if (squareOccupied(blackBishops, i)) printf("b");

        if (squareOccupied(blackKnights, i)) printf("n");

        if (squareOccupied(blackRooks, i)) printf("r");

        if (squareOccupied(blackQueens, i)) printf("q");

        if (squareOccupied(blackKings, i)) printf("k");

        printf(" ");

        //printf("%d", i);

        i++;

        if (i % 8 == 0) {
            i -= 16;
            printf("\n");
        }
    }

    printf("\n\n\n");
}

void gameLoop() {
    int playingSide = 1;
    int side = 1;
    Move bestMove;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    int i = 0;

    int playerSide = 1;

    Move legalMoves[MAX_MOVES];

    printBoard();

    while (true) {
        int from;
        int to;

        int legalMoveCount;
        int i;
        Move legalMove;
        int moveIsLegal;

        nodes = 0;
        quiescentNodes = 0;

        printf("from:");
        scanf("%d", &from);
        printf("to:");
        scanf("%d", &to);

        bestMove = createMove(from, to, 0, 0, 0);

        legalMoveCount = generateMoves(gameState, legalMoves, side);

        moveIsLegal = 0;
        for (i = 0; i < legalMoveCount; i++) {
            legalMove = legalMoves[i];

            if (legalMove.from != bestMove.from) {
                continue;
            }

            if (legalMove.to != bestMove.to) {
                continue;
            }

            moveIsLegal = 1;
            break;
        }

        if (moveIsLegal == 0) {
            printf("move is not legal\n");
            continue;
        }

        // invalid promotion number is used to signify that no move is available (to save space in move struct)
        if (bestMove.promotion == 7) {
            printf("draw");
            return;
        }

        if (checkIfCheckMate(bestMove, side) == 1) {
            printf("checkmate");
            return;
        }
        printf("side: %d   from: %d,   to: %d\n", side, from, to);
        printf("nodes: %d :::  quiescent: %d\n", nodes, quiescentNodes);

        gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];
        // modifies the gamestate array
        makeMove(bestMove);
        GAME_STATE_STACK_POINTER++;

        printBoard();

        // switch side after turn is completed
        side *= -1;

        i++;
    }
}


void main(void) {
    GAME_STATE_STACK_POINTER++;
    u64 *gameState = GAME_STATE_STACK[GAME_STATE_STACK_POINTER];

    // all white pieces
    gameState[0] = 0b0000000000000000000000000000000000000000000000001111111111111111LLU;
    // white pawns
    gameState[1] = 0b0000000000000000000000000000000000000000000000001111111100000000LLU;
    // white knights
    gameState[2] = 0b0000000000000000000000000000000000000000000000000000000001000010LLU;
    // white bishops
    gameState[3] = 0b0000000000000000000000000000000000000000000000000000000000100100LLU;
    // white rooks
    gameState[4] = 0b0000000000000000000000000000000000000000000000000000000010000001LLU;
    // white queens
    gameState[5] = 0b0000000000000000000000000000000000000000000000000000000000001000LLU;
    // white king
    gameState[6] = 0b0000000000000000000000000000000000000000000000000000000000010000LLU;

    // all black pieces
    gameState[7] = 0b1111111111111111000000000000000000000000000000000000000000000000LLU;
    // black pawns
    gameState[8] = 0b0000000011111111000000000000000000000000000000000000000000000000LLU;
    // black knights
    gameState[9] = 0b0100001000000000000000000000000000000000000000000000000000000000LLU;
    // black bishops
    gameState[10] = 0b0010010000000000000000000000000000000000000000000000000000000000LLU;
    // black rooks
    gameState[11] = 0b1000000100000000000000000000000000000000000000000000000000000000LLU;
    // black queens
    gameState[12] = 0b0000100000000000000000000000000000000000000000000000000000000000LLU;
    // black kings
    gameState[13] = 0b0001000000000000000000000000000000000000000000000000000000000000LLU;

    // additional info (both colors can castle both ways), no en passant, no moves done (0th move)
    // of the 3-fold repetition and the 50-move rule
    gameState[14] = 0b0000000000000000000000000000000000000000000000000000000000001111LLU;

    gameLoop();
}
