#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Search.h"
#include "Constants.h"
#include "Init.h"
#include "Utilities.h"
#include "ParseUciPosition.h"

void uciListen() {
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

    printf("gamestack %d movestack %d\n", GAME_STATE_STACK_POINTER, MOVE_STACK_POINTER);


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
        setLookUpTables();
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
      GAME_STATE_STACK_POINTER = 0;
      parseUciPosition(GAME_STATE_STACK[GAME_STATE_STACK_POINTER], "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n");
      positionReady = true;

      free(input);
      continue;
    }

    if (strncmp(input, "position", 8) == 0) {
      positionReady = false;
      GAME_STATE_STACK_POINTER = 0;
      parseUciPosition(GAME_STATE_STACK[GAME_STATE_STACK_POINTER], input);
      positionReady = true;

      printBoard();
      
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
      CANCEL_THREAD = 1;
      moveStr = (char*) malloc(8 * sizeof(char));
      moveToString(moveStr, SELECTED_MOVE);

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
      int side = getSideToPlay(GAME_STATE_STACK[GAME_STATE_STACK_POINTER][14]);
      int i;
      
      
      argsFilled = sscanf(input, "go wtime %d btime %d winc %d binc %d", &whiteTime, &blackTime, &whiteIncrement, &blackIncrement);

      // no increment
      if (argsFilled == 2) {
        if (side == 1) {
          calcTime = whiteTime * 1000 / 40;
        }
        else {
          calcTime = blackTime * 1000 / 40;
        }
      }
      // increment
      else if (argsFilled == 4) {
        if (side == 1) {
          calcTime = whiteTime * 1000 / 40;
          calcTime += whiteIncrement * 1000;
        }
        else {
          calcTime = blackTime * 1000 / 40;
          calcTime += blackIncrement * 1000;
        }
      }
      else {
        printf("error in args\n");
        fflush(stdout);
        return;
      }

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

      pthread_cancel(threadId);
      CANCEL_THREAD = 1;
      moveStr = (char*) malloc(8 * sizeof(char));
      moveToString(moveStr, SELECTED_MOVE);

      printf("bestmove %s\n", moveStr);
      fflush(stdout);

      free(moveStr);
      free(input);
      continue;
    }

    free(input);
  }
  /*
  pthread_create(&threadId, NULL, search, NULL);
  pthread_detach(threadId);
  sleep(5);
  pthread_cancel(threadId);

  // some important initializations
  setLookUpTables();
  */
}