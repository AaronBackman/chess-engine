#include <stdbool.h>

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

void setLookUpTables() {
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
}