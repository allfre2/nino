#include "common.h"
#include "search.h"
#include "xboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notation.h"
#include "thread.h"
#include "tm.h"
#include "move.h"
#include "debug.h"
#include "gen.h"

int force,post;
/*signal(SIGTERM, SIG_IGN);
signal(SIGINT, SIG_IGN);
*/int Xboard(){
setbuf(stdout,NULL);
setbuf(stdin,NULL);
  force = 0;
  post = 1;
  FILE * input, * xlog;
  char commands[255],san[10];
  Boardmap Position;
  Game game;
  float nseconds = 1;
  input = stdin;
  thinking = 0;
  Move moves[MAXMOVES],m;
  int nmoves;
	INIT_MUTEX(StopIterate);
printf("feature sigint=0\n");
printf("feature done=1\n");
while(1){
//fflush(stdin);
  if(fgets(commands,254,stdin) == NULL){
   xboard = 0; return 0;
  }
/*   xlog  = fopen("xlog","a");
if(xlog != NULL) {fprintf(xlog,"%s",commands);
   fclose(xlog);
}*/
   if(strcmp(commands,"new\n") == 0){
    if(thinking){
	MUTEX_LOCK(StopIterate);
	   flags |= INTERRUPT;
	MUTEX_UNLOCK(StopIterate);
    }
	force = 0;
    SetfromFEN((char *)StartPositionFEN,&game.Position);
//    memcpy(&Position,&game.Position,sizeof(Boardmap));
//printf("\nstarted new board\n");
   }/*
   else if(strcmp(commands,"time\n") == 0){
   scanf("%f",&nseconds);
 }*/
   else if(strcmp(commands,"stop\n") == 0 || strcmp(commands,"?\n") == 0){

	MUTEX_LOCK(StopIterate);
	   flags |= INTERRUPT;
	MUTEX_UNLOCK(StopIterate);
   }
   else if(strcmp(commands,"go\n") == 0){
 movie:
        force = 0;
	/*LOCK! see if engine is already thinking*/
        if(thinking){
 //         printf("\nAlready thinking =)\n");
        }
        else{
//	printf("\nstarted to think\n");
	//ManageTime(&game,NULL);
	flags = 0;
	game.MaxDepth = MAX_DEPTH;
	available_time = game.available_time = clock() + (nseconds*CLOCKS_PER_SEC);
	thinking = 1;
	Think(&game);
	WAIT(IterativeDeepening);
printf("move "); printM(&BEST,&game.Position); printf("\n");
  MakeMove(&BEST,&game.Position);
 // fflush(stdout);

/*   xlog  = fopen("xlog","a");
if(xlog != NULL) {fprintf(xlog,"\nmove: %s\n",san);
   fclose(xlog);
}*/
	//WAIT(TimerThread);
  //memcpy(&BEST,&BEAST,sizeof(Move));
       }
   }
   else if(strcmp(commands,"level") == 0){

   }
   else if(strcmp(commands,"post\n") == 0){
    post = 1;
   }
   else if(strcmp(commands,"nopost\n") == 0){
    post = 0;
   }
   else if(strcmp(commands,"force\n") == 0){
    force = 1;
   }
   else if(strcmp(commands,"hard") == 0){

   }
   else if(strcmp(commands,"easy\n") == 0){

   }
   else if(strcmp(commands,"quit\n") == 0){
     exit(0);
   }/*
   else if(strcmp(commands,"/\n") == 0){
	printBoardinfo(&game.Position);
   }*/
   else{
  nmoves = GenMoves(&game.Position,moves);
 if(Parsemove(commands,&m,&game.Position) || ValidateMove(&m,moves,&game.Position,nmoves)){
/*   xlog  = fopen("xlog","a");
if(xlog != NULL) {fprintf(xlog,"\nInvalid move: %s\n",commands);
   fclose(xlog);
}*/
 }
 else{ if(!thinking ) MakeMove(&m,&game.Position); if(!force) goto movie; }

   }
  }
}

int Post(Boardmap * Position){

    printf("%d\t%d\t%.0f\t%d\t",current_depth,Scores[current_depth ] > 999999 ? 999999 : Scores[current_depth] < -9999999 ? -999999 : Scores[current_depth],(available_time - clock())/10000,GlobalNodeCount);
    printPV(&pv[current_depth],Position);
    printf("\n");

}
