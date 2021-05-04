#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Search.h"
#include "Board.h"
#include "Constants.h"
#include "Init.h"
#include "Utilities.h"
#include "ParseUciPosition.h"
#include "Perft.h"

void uci_listen() {
  bool initReady = false;
  // false if position is being changed based on input
  bool positionReady = true;
  while (true) {
    char *input;
    // includes the terminating null character
    int maxInputLength = 2048;
    pthread_t threadId;

    input = (char*) malloc(maxInputLength * sizeof(char));
    fgets(input, maxInputLength, stdin);

    printf("root %d ply %d\n", g_root, g_ply);


    if (strcmp(input, "uci\n") == 0) {
      printf("id name RockFish\n");
      printf("id author MrFish\n");
      printf("uciok\n");
      fflush(stdout);

      free(input);
      continue;
    }

    if (strcmp(input, "quit\n") == 0) {
      free(input);
      exit(0);
      return;
    }

    if (strcmp(input, "isready\n") == 0) {
      if (!initReady) {
        set_lookup_tables();
        initReady = true;
      }
      if (!positionReady) {
        free(input);
        continue;
      }
      printf("readyok\n");
      fflush(stdout);

      free(input);
      continue;
    }

    if (strcmp(input, "ucinewgame\n") == 0) {
      positionReady = false;
      g_root = 0;
      parse_uci_position("position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n");
      positionReady = true;

      free(input);
      continue;
    }

    if (strncmp(input, "position", 8) == 0) {
      positionReady = false;
      g_root = 0;
      parse_uci_position(input);
      positionReady = true;

      //print_board();
      
      free(input);
      continue;
    }

    if (strcmp(input, "go infinite\n") == 0) {
      pthread_create(&threadId, NULL, search, NULL);
      pthread_detach(threadId);

      free(input);
      continue;
    }

    if (strcmp(input, "stop\n") == 0) {
      char *moveStr;

      pthread_cancel(threadId);
      g_cancelThread = 1;
      moveStr = (char*) malloc(8 * sizeof(char));
      move_to_string(moveStr, g_selectedMove);

      printf("bestmove %s\n", moveStr);
      fflush(stdout);

      free(moveStr);
      free(input);
      continue;
    }

    if (strncmp(input, "go wtime", 8) == 0) {
      char *moveStr;
      int whiteTime;
      int blackTime;
      int whiteIncrement;
      int blackIncrement;
      int argsFilled;
      // based on remaining time for the side and the increment
      int calcTime;
      int side = get_side_to_play(g_gameStateStack[g_root].meta);
      int i;
      
      
      argsFilled = sscanf(input, "go wtime %d btime %d winc %d binc %d", &whiteTime, &blackTime, &whiteIncrement, &blackIncrement);

      // no increment
      if (argsFilled == 2) {
        if (side == 1) {
          calcTime = whiteTime * 1000 / 50;
        }
        else {
          calcTime = blackTime * 1000 / 50;
        }
      }
      // increment
      else if (argsFilled == 4) {
        if (side == 1) {
          calcTime = whiteTime * 1000 / 50;
          calcTime += whiteIncrement * 1000;
        }
        else {
          calcTime = blackTime * 1000 / 50;
          calcTime += blackIncrement * 1000;
        }
      }
      else {
        printf("error in args\n");
        fflush(stdout);
        return;
      }

      printf("calctime: %d\n", calcTime);

      pthread_create(&threadId, NULL, search, NULL);
      pthread_detach(threadId);

      // more than 1 second
      if (calcTime >= 1000000) {
          for (i = 0; i < calcTime / 1000000; i++) {
              usleep(1000000);

              // checkmate was found, no need to search further
              if (checkmate) {
                  break;
              }
          }
      }
      else {
          usleep(calcTime);
      }

      g_cancelThread = 1;
      moveStr = (char*) malloc(8 * sizeof(char));
      move_to_string(moveStr, g_selectedMove);

      printf("bestmove %s\n", moveStr);
      fflush(stdout);

      free(moveStr);
      free(input);
      continue;
    }

    if (strncmp(input, "go perft", 8) == 0) {
      int depth;

      sscanf(input, "go perft %d", &depth);

      perft_divide(depth);

      free(input);
      continue;
    }

    free(input);
  }
}