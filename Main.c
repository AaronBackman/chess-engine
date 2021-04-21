#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "Move.h"
#include "MoveGeneration.h"
#include "Constants.h"
#include "MakeMove.h"
#include "LegalityChecks.h"
#include "Utilities.h"
#include "Perft.h"
#include "Uci.h"

void main(void) {
    uciListen();
    //perftDivide(4);
}

// compile with gcc -pthread -o Program *.c