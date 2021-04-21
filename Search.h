#ifndef SEARCH_H
#define SEARCH_H

#include "Move.h"

extern Move SELECTED_MOVE;
extern int SEARCHED_DEPTH;

void *search(void *vargp);

#endif