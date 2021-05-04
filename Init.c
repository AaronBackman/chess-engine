#include <stdbool.h>
#include <stdlib.h>

#include "Constants.h"
#include "TranspositionTable.h"
#include "Utilities.h"
#include "Random.h"

TranspositionTableEntry tTable[TRANSPOSITION_TABLE_SIZE];

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


u64 WHITE_PAWN_ZOBRIST[64];
u64 WHITE_KNIGHT_ZOBRIST[64];
u64 WHITE_BISHOP_ZOBRIST[64];
u64 WHITE_ROOK_ZOBRIST[64];
u64 WHITE_QUEEN_ZOBRIST[64];
u64 WHITE_KING_ZOBRIST[64];
u64 BLACK_PAWN_ZOBRIST[64];
u64 BLACK_KNIGHT_ZOBRIST[64];
u64 BLACK_BISHOP_ZOBRIST[64];
u64 BLACK_ROOK_ZOBRIST[64];
u64 BLACK_QUEEN_ZOBRIST[64];
u64 BLACK_KING_ZOBRIST[64];
u64 BLACK_TO_MOVE_ZOBRIST;
u64 CASTLING_ZOBRIST[4];
u64 ENPASSANT_FILE_ZOBRIST[8];

// manhattan distance between 2 squares
int calc_manhattan_distance(int sq1, int sq2) {
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

void init_manhattan_distance_arr() {
    int i;
    int j;

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            MD[i][j] = calc_manhattan_distance(i, j);
        }
    }
}

void set_single_bit_lookup() {
  int i;

  for (i = 0; i < 64; i++) {
    SINGLE_BIT_LOOKUP[i] = 1LLU << i;
  }
}

void set_north_east_lookup() {
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

        pattern = fill_square(pattern, destinationSquare);

        j++;
    }

    NORTH_EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_north_west_lookup() {
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

        pattern = fill_square(pattern, destinationSquare);

        j--;
    }

    NORTH_WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_south_east_lookup() {
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

        pattern = fill_square(pattern, destinationSquare);

        j++;
    }

    SOUTH_EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_south_west_lookup() {
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

        pattern = fill_square(pattern, destinationSquare);

        j--;
    }

    SOUTH_WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}


void set_north_lookup() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from up
    for (i = 1; boardIndex + i * 8 < 64; i++) {
        destinationSquare = boardIndex + i * 8;
        pattern = fill_square(pattern, destinationSquare);
    }

    NORTH_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_west_lookup() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from left
    for (i = 1; boardIndex % 8 - i >= 0; i++) {
        destinationSquare = boardIndex - i;
        pattern = fill_square(pattern, destinationSquare);
    }

    WEST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_south_lookup() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from up
    for (i = 1; boardIndex - i * 8 >= 0; i++) {
        destinationSquare = boardIndex - i * 8;
        pattern = fill_square(pattern, destinationSquare);
    }

    SOUTH_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_east_lookup() {
  int boardIndex;

  for (boardIndex = 0; boardIndex < 64; boardIndex++) {
    int destinationSquare;
    int i;
    u64 pattern = 0;

    // check linear, non-diagonal attacks from left
    for (i = 1; boardIndex % 8 + i < 8; i++) {
        destinationSquare = boardIndex + i;
        pattern = fill_square(pattern, destinationSquare);
    }

    EAST_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_king_lookup() {
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
            pattern = fill_square(pattern, destinationSquare);
        }
    }
    KING_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_knight_lookup() {
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

              pattern = fill_square(pattern, destinationSquare);
          }
      }
    }
    KNIGHT_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_white_pawn_attack_lookup() {
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
        pattern = fill_square(pattern, boardIndex + 7);
      }
      if (column != 7) {
        pattern = fill_square(pattern, boardIndex + 9);
      }
    }
    
    WHITE_PAWN_ATTACK_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_white_pawn_move_lookup() {
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
        pattern = fill_square(pattern, boardIndex + 16);
      }
      // no pawns can be on first and last rows
      if (row != 0 && row != 7) {
        pattern = fill_square(pattern, boardIndex + 8);
      }
    }
    
    WHITE_PAWN_MOVE_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_black_pawn_attack_lookup() {
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
        pattern = fill_square(pattern, boardIndex - 9);
      }
      if (column != 7) {
        pattern = fill_square(pattern, boardIndex - 7);
      }
    }
    
    BLACK_PAWN_ATTACK_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void set_black_pawn_move_lookup() {
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
        pattern = fill_square(pattern, boardIndex - 16);
      }
      // no pawns can be on first and last rows
      if (row != 0 && row != 7) {
        pattern = fill_square(pattern, boardIndex - 8);
      }
    }
    
    BLACK_PAWN_MOVE_LOOKUP_PATTERN[boardIndex] = pattern;
  }
}

void initZobrist() {
    int i;

    for (i = 0; i < 64; i++) {
        WHITE_PAWN_ZOBRIST[i] = getRNG();
        WHITE_KNIGHT_ZOBRIST[i] = getRNG();
        WHITE_BISHOP_ZOBRIST[i] = getRNG();
        WHITE_ROOK_ZOBRIST[i] = getRNG();
        WHITE_QUEEN_ZOBRIST[i] = getRNG();
        WHITE_KING_ZOBRIST[i] = getRNG();
        BLACK_PAWN_ZOBRIST[i] = getRNG();
        BLACK_KNIGHT_ZOBRIST[i] = getRNG();
        BLACK_BISHOP_ZOBRIST[i] = getRNG();
        BLACK_ROOK_ZOBRIST[i] = getRNG();
        BLACK_QUEEN_ZOBRIST[i] = getRNG();
        BLACK_KING_ZOBRIST[i] = getRNG();
    }

    BLACK_TO_MOVE_ZOBRIST = getRNG();

    for (i = 0; i < 4; i++) {
        CASTLING_ZOBRIST[i] = getRNG();
    }

    for (i = 0; i < 8; i++) {
        ENPASSANT_FILE_ZOBRIST[i] = getRNG();
    }
}

void set_lookup_tables() {
    init_manhattan_distance_arr();
    set_single_bit_lookup();

    // set diagonal lookUptables
    set_north_east_lookup();
    set_north_west_lookup();
    set_south_east_lookup();
    set_south_west_lookup();

    // set straight lookUptables
    set_north_lookup();
    set_west_lookup();
    set_south_lookup();
    set_east_lookup();

    set_king_lookup();
    set_knight_lookup();
    set_white_pawn_attack_lookup();
    set_white_pawn_move_lookup();
    set_black_pawn_attack_lookup();
    set_black_pawn_move_lookup();

    initZobrist();
}