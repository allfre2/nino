/*--------------------------------------------------------------*
 | Engine name - thread.c - thread wrapper functions.           |
 | Description: Threads for pondering moves in opponents time   |
 | wrapper for custom compile in windows and linux              |
 | License                                                      |
 | Written by: Alfredo LLuzon                                   |
 | Contact info: @hotmail.com, @gmail.com, website.com          |
 | last modifications by: <guy #n> : <modifications>            |
 *--------------------------------------------------------------*/

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
 /* Remember maybe initialize all mutexes */

 THREAD_t IterativeDeepening;
 THREAD_t PonderThread;
 THREAD_t TimerThread;
 int flags;
 int exitXboard;
int Think(void * data){
  int ret;
  double * time;
 flags = 0;
  //time = &((Game *)data) -> available_time;
  //ret = (int) THREAD_CREATE(TimerThread,NULL,Timer,(void*)time);
  /* haha good bug. Note to self to never forget it. I'm passing a pointer to a variable that disapears
 *   when the function returns. cool ... =D
 *
 *   code was:
 *   double time = ((Game *)data) -> available_time;
 *   ret = THREAD_CREATE(TimerThread,NULL,Timer,(double *)&time); // <--- lololol
*/
 // if(ret) return ret;

  return (int) THREAD_CREATE(IterativeDeepening,NULL,Iterate,data);
}

int Ponder(void * data){

}

THREAD_RETURN_VAL Iterate(void * data){

 int exit,turn,alpha,beta;
 Game * game = data;
 current_depth = 1;
 exit = 0;
 alpha = Galpha = -INFINITY;
 beta = Gbeta =  +INFINITY;
 turn = game->Position.turn;
 Line pvTMP[MAX_DEPTH];
 int window;
 while( current_depth < MAX_DEPTH - 1){
/* if(alpha > beta ) window = alpha - beta; else*/ window = 30;
 GlobalNodeCount = 0;
/* alpha = Galpha - window;
 beta =  Gbeta  + window;
 Scores[current_depth] = Search(current_depth, &game->Position,  alpha ,  beta );
 //printBoardinfo(&game->Position);
 if( Galpha <= alpha ||  Gbeta >= beta) {
  if(turn == WHITE && Galpha <= alpha) alpha = Galpha, beta = +INFINITY;
  if(turn == BLACK && Gbeta >= beta) beta = Gbeta, alpha = -INFINITY;
  printf("\noutside window\n");
  GlobalNodeCount = 0;*/
  Scores[current_depth] = Search(current_depth, &game->Position, alpha , beta );
 //}
GNodes[current_depth] = GlobalNodeCount;
MUTEX_LOCK(StopIterate);
 if(flags != 0){/*printf("\nfrom Iterate thread flags != 0\n");*/ exit = 1;}
MUTEX_UNLOCK(StopIterate);
 if (exit == 0 && current_depth < MAX_DEPTH &&
	Scores[current_depth -1] < (INFINITY - MAX_DEPTH) &&
	 Scores[current_depth -1] > (-INFINITY + MAX_DEPTH) &&
	  BEAST.flags != NOMOVE){
 //printf("\n at depth => %d\n",current_depth);
memcpy(pvTMP,pv,sizeof(Line) * MAX_DEPTH);
//memcpy(&SortPv,&pv[current_depth],sizeof(Line) );
memcpy(&BEST,&BEAST,sizeof(Move));
//printPV(&pv[current_depth],&game->Position);
if(post || !xboard){
 Post(&game->Position);
}
current_depth++;
 continue;
  }
 else{
  MUTEX_LOCK(StopIterate);
  flags |= FINISH;
  //printf("\nFrom Iterate thread: finish?\n");
  MUTEX_UNLOCK(StopIterate);
  break;
 }
 }
//printmove(&BEST,&game->Position,666);
/* Only when Xboard ???*/
//	MakeMove(&BEST,&game->Position);
/**/
  //MakeMove(&BEST,&game->Position);
//  printBoardinfo(&game->Position);
  //printmove(&BEST,&game->Position,_SHORT);
 //
 //printmove(&BEST,&game->Position,_SHORT);
//fflush(stdout);
 // printf("\n\n");
  thinking = 0;
THREAD_EXIT(IterativeDeepening);
}

THREAD_RETURN_VAL Timer(void * data){

/* Add a semaphore to leave the thread alive and signal it when want it to work
   and just free the thread and wait at program exit
*/

double Time = *(double *)data;
int exit;
exit = 0;
 while(exit == 0){
/* maybe sleep some time */
MUTEX_LOCK(StopIterate);
  if(clock() >= Time) /*printf("\nfrom Timer thread: clock >= time\n"),*/
   flags |= TIMEOUT;

 if(flags != 0) /*printf("\nfrom Timer thread: flags != 0\n"),*/ exit = 1;
MUTEX_UNLOCK(StopIterate);

 }

THREAD_EXIT(TimerThread);
}

