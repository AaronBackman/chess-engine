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
#include "Uci.h"
#include "Random.h"

void main(void) {
    uci_listen();
}

// compile with gcc -pthread -O3 -o Program *.c