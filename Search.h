#ifndef SEARCH_H
#define SEARCH_H

#include "Move.h"

extern Move g_selectedMove;
extern bool checkmate;

void *search(void *vargp);

#endif