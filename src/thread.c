#include "thread.h"
#include "common.h"
#include "search.h"
#include "notation.h"
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "move.h"
#include "gen.h"
#include "xboard.h"

MUTEX_t StopIterate; /* Global Mutex for stoping iterative deepening */
MUTEX_t InputMutex;

THREAD_t IterativeDeepening;
THREAD_t PonderThread;
THREAD_t TimerThread;
int flags;
int exitXboard;

int Think(void *data)
{
  int ret;
  double *time;
  flags = 0;
  /* haha good bug. Note to self to never forget it. I'm passing a pointer to a variable that disapears
 *   when the function returns. cool ... =D
 *
 *   code was:
 *   double time = ((Game *)data) -> available_time;
 *   ret = THREAD_CREATE(TimerThread,NULL,Timer,(double *)&time); // <--- lololol
*/

  return (int)THREAD_CREATE(IterativeDeepening, NULL, Iterate, data);
}

int Ponder(void *data)
{
}

THREAD_RETURN_VAL Iterate(void *data)
{
  int exit, turn, alpha, beta;
  Game *game = data;
  current_depth = 1;
  exit = 0;
  alpha = Galpha = -INFINITY;
  beta = Gbeta = +INFINITY;
  turn = game->Position.turn;
  Line pvTMP[MAX_DEPTH];
  int window;

  while (current_depth < MAX_DEPTH - 1)
  {
    window = 30;
    GlobalNodeCount = 0;
    Scores[current_depth] = Search(current_depth, &game->Position, alpha, beta);
    GNodes[current_depth] = GlobalNodeCount;
    MUTEX_LOCK(StopIterate);

    if (flags != 0)
    {
      exit = 1;
    }
    MUTEX_UNLOCK(StopIterate);

    if (exit == 0 && current_depth < MAX_DEPTH &&
        Scores[current_depth - 1] < (INFINITY - MAX_DEPTH) &&
        Scores[current_depth - 1] > (-INFINITY + MAX_DEPTH) &&
        BEAST.flags != NOMOVE)
    {
      memcpy(pvTMP, pv, sizeof(Line) * MAX_DEPTH);
      memcpy(&BEST, &BEAST, sizeof(Move));
      if (post || !xboard)
      {
        Post(&game->Position);
      }
      current_depth++;
      continue;
    }
    else
    {
      MUTEX_LOCK(StopIterate);
      flags |= FINISH;
      MUTEX_UNLOCK(StopIterate);
      break;
    }
  }
  thinking = 0;
  THREAD_EXIT(IterativeDeepening);
}

THREAD_RETURN_VAL Timer(void *data)
{
  /* Add a semaphore to leave the thread alive and signal it when want it to work
   and just free the thread and wait at program exit
  */

  double Time = *(double *)data;
  int exit;
  exit = 0;

  while (exit == 0)
  {
    MUTEX_LOCK(StopIterate);
    if (clock() >= Time)
      flags |= TIMEOUT;

    if (flags != 0)
      exit = 1;

    MUTEX_UNLOCK(StopIterate);
  }

  THREAD_EXIT(TimerThread);
}