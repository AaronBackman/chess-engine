#include <stdbool.h>
#include <stdlib.h>

#include "Constants.h"
#include "Utilities.h"

u64 SINGLE_BIT_LOOKUP[64];
u64 NORTH_EAST_LOOKUP_PATTERN[64];
u64 NORTH_WEST_LOOKUP_PATTERN[64];
u64 SOUTH_EAST_LOOKUP_PATTERN[64];
u64 SOUTH_WEST_LOOKUP_PATTERN[64];

u64 NORTH_LOOKUP_PATTERN[64];
u64 WEST_LOOKUP_PATTERN[64];
u64 SOUTH_LOOKUP_PATTERN[64];
u64 EAST_LOOKUP_PATTERN[64];

u64 KING_LOOKUP_PATTERN[64];
u64 KNIGHT_LOOKUP_PATTERN[64];
u64 WHITE_PAWN_ATTACK_LOOKUP_PATTERN[64];
u64 WHITE_PAWN_MOVE_LOOKUP_PATTERN[64];
u64 BLACK_PAWN_ATTACK_LOOKUP_PATTERN[64];
u64 BLACK_PAWN_MOVE_LOOKUP_PATTERN[64];

int BLACK_PAWN_PIECE_SQUARE_TABLE[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  50, 50, 50, 50, 50, 50, 50, 50,
  10, 10, 20, 30, 30, 20, 10, 10,
  5,  5, 10, 25, 25, 10,  5,  5,
  0,  0,  0, 20, 20,  0,  0,  0,
  5, -5,-10,  0,  0,-10, -5,  5,
  5, 10, 10,-20,-20, 10, 10,  5,
  0,  0,  0,  0,  0,  0,  0,  0
};

int WHITE_PAWN_PIECE_SQUARE_TABLE[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10,-20,-20, 10, 10,  5,
  5, -5,-10,  0,  0,-10, -5,  5,
  0,  0,  0, 20, 20,  0,  0,  0,
  5,  5, 10, 25, 25, 10,  5,  5,
  10, 10, 20, 30, 30, 20, 10, 10,
  50, 50, 50, 50, 50, 50, 50, 50,
  0,  0,  0,  0,  0,  0,  0,  0
};

int BLACK_KNIGHT_PIECE_SQUARE_TABLE[64] =
{
  -50,-40,-30,-30,-30,-30,-40,-50,
  -40,-20,  0,  0,  0,  0,-20,-40,
  -30,  0, 10, 15, 15, 10,  0,-30,
  -30,  5, 15, 20, 20, 15,  5,-30,
  -30,  0, 15, 20, 20, 15,  0,-30,
  -30,  5, 10, 15, 15, 10,  5,-30,
  -40,-20,  0,  5,  5,  0,-20,-40,
  -50,-40,-30,-30,-30,-30,-40,-50
};

int WHITE_KNIGHT_PIECE_SQUARE_TABLE[64] =
{
  -50,-40,-30,-30,-30,-30,-40,-50,
  -40,-20,  0,  5,  5,  0,-20,-40,
  -30,  5, 10, 15, 15, 10,  5,-30,
  -30,  0, 15, 20, 20, 15,  0,-30,
  -30,  5, 15, 20, 20, 15,  5,-30,
  -30,  0, 10, 15, 15, 10,  0,-30,
  -40,-20,  0,  0,  0,  0,-20,-40,
  -50,-40,-30,-30,-30,-30,-40,-50
};

int BLACK_BISHOP_PIECE_SQUARE_TABLE[64] =
{
  -20,-10,-10,-10,-10,-10,-10,-20,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -10,  0,  5, 10, 10,  5,  0,-10,
  -10,  5,  5, 10, 10,  5,  5,-10,
  -10,  0, 10, 10, 10, 10,  0,-10,
  -10, 10, 10, 10, 10, 10, 10,-10,
  -10,  5,  0,  0,  0,  0,  5,-10,
  -20,-10,-10,-10,-10,-10,-10,-20
};

int WHITE_BISHOP_PIECE_SQUARE_TABLE[64] =
{
  -20,-10,-10,-10,-10,-10,-10,-20,
  -10,  5,  0,  0,  0,  0,  5,-10,
  -10, 10, 10, 10, 10, 10, 10,-10,
  -10,  0, 10, 10, 10, 10,  0,-10,
  -10,  5,  5, 10, 10,  5,  5,-10,
  -10,  0,  5, 10, 10,  5,  0,-10,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -20,-10,-10,-10,-10,-10,-10,-20
};

int BLACK_ROOK_PIECE_SQUARE_TABLE[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
};

int WHITE_ROOK_PIECE_SQUARE_TABLE[64] =
{
  0,  0,  0,  5,  5,  0,  0,  0,
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,  0,  0,  0,  0,  0,  0, -5,
  5, 10, 10, 10, 10, 10, 10,  5,
  0,  0,  0,  0,  0,  0,  0,  0
};

int BLACK_QUEEN_PIECE_SQUARE_TABLE[64] =
{
  -20,-10,-10, -5, -5,-10,-10,-20,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -10,  0,  5,  5,  5,  5,  0,-10,
  -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
  -10,  5,  5,  5,  5,  5,  0,-10,
  -10,  0,  5,  0,  0,  0,  0,-10,
  -20,-10,-10, -5, -5,-10,-10,-20
};

int WHITE_QUEEN_PIECE_SQUARE_TABLE[64] =
{
  -20,-10,-10, -5, -5,-10,-10,-20,
  -10,  0,  5,  0,  0,  0,  0,-10,
  -10,  5,  5,  5,  5,  5,  0,-10,
  0,  0,  5,  5,  5,  5,  0, -5,
  -5,  0,  5,  5,  5,  5,  0, -5,
  -10,  0,  5,  5,  5,  5,  0,-10,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -20,-10,-10, -5, -5,-10,-10,-20
};

int BLACK_KING_PIECE_SQUARE_TABLE_OPENING[64] =
{
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -20,-30,-30,-40,-40,-30,-30,-20,
  -10,-20,-20,-20,-20,-20,-20,-10,
  20, 20,  0,  0,  0,  0, 20, 20,
  20, 30, 10,  0,  0, 10, 30, 20
};

int WHITE_KING_PIECE_SQUARE_TABLE_OPENING[64] =
{
  20, 30, 10,  0,  0, 10, 30, 20,
  20, 20,  0,  0,  0,  0, 20, 20,
  -10,-20,-20,-20,-20,-20,-20,-10,
  -20,-30,-30,-40,-40,-30,-30,-20,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30
};

int BLACK_KING_PIECE_SQUARE_TABLE_ENDGAME[64] =
{
  -50,-40,-30,-20,-20,-30,-40,-50,
  -30,-20,-10,  0,  0,-10,-20,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-30,  0,  0,  0,  0,-30,-30,
  -50,-30,-30,-30,-30,-30,-30,-50
};

int WHITE_KING_PIECE_SQUARE_TABLE_ENDGAME[64] =
{
  -50,-30,-30,-30,-30,-30,-30,-50,
  -30,-30,  0,  0,  0,  0,-30,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-20,-10,  0,  0,-10,-20,-30,
  -50,-40,-30,-20,-20,-30,-40,-50
};

unsigned char CMD[64] = {
    6, 5, 4, 3, 3, 4, 5, 6,
    5, 4, 3, 2, 2, 3, 4, 5,
    4, 3, 2, 1, 1, 2, 3, 4,
    3, 2, 1, 0, 0, 1, 2, 3,
    3, 2, 1, 0, 0, 1, 2, 3,
    4, 3, 2, 1, 1, 2, 3, 4,
    5, 4, 3, 2, 2, 3, 4, 5,
    6, 5, 4, 3, 3, 4, 5, 6
};

unsigned char MD[64][64];

// manhattan distance between 2 squares
int calcManhattanDistance(int sq1, int sq2) {
   int file1, file2, rank1, rank2;
   int rankDistance, fileDistance;

   file1 = sq1  & 7;
   file2 = sq2  & 7;
   rank1 = sq1 >> 3;
   rank2 = sq2 >> 3;
   rankDistance = abs(rank2 - rank1);
   fileDistance = abs(file2 - file1);

   return rankDistance + fileDistance;
}

void initManhattanDistanceArr() {
    int i;
    int j;

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            MD[i][j] = calcManhattanDistance(i, j);
        }
    }
}

void setSingleBitLookUp() {
  int i;

  for (i = 0; i < 64; i++) {
    SINGLE_BIT_LOOKUP[i] = 1LLU << i;
  }
}

void setNorthEastLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    j = 1;
    // top right diagonal
    for (i = 9; boardIndex + i < 64 && boardIndex % 8 + j < 8; i += 9) {
        destinationSquare = boardIndex + i;

        pattern = fillSquare(pattern, destinationSquare);

        j++;
    }

    NORTH_EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setNorthWestLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    j = -1;
    // top left diagonal
    for (i = 7; boardIndex + i < 64 && boardIndex % 8 + j >= 0; i += 7) {
        destinationSquare = boardIndex + i;

        pattern = fillSquare(pattern, destinationSquare);

        j--;
    }

    NORTH_WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setSouthEastLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    j = 1;
    // top right diagonal
    for (i = -7; boardIndex + i >= 0 && boardIndex % 8 + j < 8; i -= 7) {
        destinationSquare = boardIndex + i;

        pattern = fillSquare(pattern, destinationSquare);

        j++;
    }

    SOUTH_EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setSouthWestLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    j = -1;
    // top left diagonal
    for (i = -9; boardIndex + i >= 0 && boardIndex % 8 + j >= 0; i -= 9) {
        destinationSquare = boardIndex + i;

        pattern = fillSquare(pattern, destinationSquare);

        j--;
    }

    SOUTH_WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}


void setNorthLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from up
    for (i = 1; boardIndex + i * 8 < 64; i++) {
        destinationSquare = boardIndex + i * 8;
        pattern = fillSquare(pattern, destinationSquare);
    }

    NORTH_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setWestLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from left
    for (i = 1; boardIndex % 8 - i >= 0; i++) {
        destinationSquare = boardIndex - i;
        pattern = fillSquare(pattern, destinationSquare);
    }

    WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setSouthLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from up
    for (i = 1; boardIndex - i * 8 >= 0; i++) {
        destinationSquare = boardIndex - i * 8;
        pattern = fillSquare(pattern, destinationSquare);
    }

    SOUTH_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setEastLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from left
    for (i = 1; boardIndex % 8 + i < 8; i++) {
        destinationSquare = boardIndex + i;
        pattern = fillSquare(pattern, destinationSquare);
    }

    EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setKingLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            if ((boardIndex % 8 + j < 0) || (boardIndex % 8 + j > 7)) continue;
            if ((boardIndex + i * 8 < 0) || (boardIndex + i * 8 > 63)) continue;

            destinationSquare = boardIndex + j + i * 8;
            pattern = fillSquare(pattern, destinationSquare);
        }
    }
    KING_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setKnightLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int j;
    u64 pattern = 0;

    for (i = -2; i < 3; i++) {
      for (j = -2; j < 3; j++) {
          if (boardIndex % 8 + j > 7 || boardIndex % 8 + j < 0) continue;

          if (j * i == 2 || j * i == -2) {
              destinationSquare = boardIndex + i * 8 + j;

              // cant move outside the board
              if (destinationSquare > 63 || destinationSquare < 0) continue;

              pattern = fillSquare(pattern, destinationSquare);
          }
      }
    }
    KNIGHT_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setWhitePawnAttackLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int row;
    int column;
    u64 pattern = 0;
    
    row = boardIndex / 8;
    column = boardIndex % 8;

    if (row != 7) {
      if (column != 0) {
        pattern = fillSquare(pattern, boardIndex + 7);
      }
      if (column != 7) {
        pattern = fillSquare(pattern, boardIndex + 9);
      }
    }
    
    WHITE_PAWN_ATTACK_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setWhitePawnMoveLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int row;
    int column;
    u64 pattern = 0;
    
    row = boardIndex / 8;
    column = boardIndex % 8;

    if (row != 7) {
      // first pawn move can be 2 squares
      if (row == 1) {
        pattern = fillSquare(pattern, boardIndex + 16);
      }
      // no pawns can be on first and last rows
      if (row != 0 && row != 7) {
        pattern = fillSquare(pattern, boardIndex + 8);
      }
    }
    
    WHITE_PAWN_MOVE_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setBlackPawnAttackLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int row;
    int column;
    u64 pattern = 0;
    
    row = boardIndex / 8;
    column = boardIndex % 8;

    if (row != 0) {
      if (column != 0) {
        pattern = fillSquare(pattern, boardIndex - 9);
      }
      if (column != 7) {
        pattern = fillSquare(pattern, boardIndex - 7);
      }
    }
    
    BLACK_PAWN_ATTACK_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setBlackPawnMoveLookUp() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    int row;
    int column;
    u64 pattern = 0;
    
    row = boardIndex / 8;
    column = boardIndex % 8;

    if (row != 0) {
      // first pawn move can be 2 squares
      if (row == 6) {
        pattern = fillSquare(pattern, boardIndex - 16);
      }
      // no pawns can be on first and last rows
      if (row != 0 && row != 7) {
        pattern = fillSquare(pattern, boardIndex - 8);
      }
    }
    
    BLACK_PAWN_MOVE_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void setLookUpTables() {
    initManhattanDistanceArr();
    setSingleBitLookUp();

    // set diagonal lookUptables
    setNorthEastLookUp();
    setNorthWestLookUp();
    setSouthEastLookUp();
    setSouthWestLookUp();

    // set straight lookUptables
    setNorthLookUp();
    setWestLookUp();
    setSouthLookUp();
    setEastLookUp();

    setKingLookUp();
    setKnightLookUp();
    setWhitePawnAttackLookUp();
    setWhitePawnMoveLookUp();
    setBlackPawnAttackLookUp();
    setBlackPawnMoveLookUp();
}