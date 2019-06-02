#include "common.h"
#include "notation.h"
#include "gen.h"
#include "move.h"
#include "hash.h"
#include "tt.h"
#include "eval.h"
#include "search.h"
#include "thread.h"
#include "xboard.h"
	#include "debug.h"
	#include <string.h>
/*DELETE ME*/
	int GlobalNodeCount;
	int GlobalQuiescenceNodes;
	int GNodes[MAX_DEPTH];
	SearchStats Statistics[MAX_DEPTH];
/***************/
short Mhistory[64][64]; // History heuristic
Move  Killers[2];
Move PV[MAX_DEPTH]; // Principal Variation Heuristic
int PVdepth;
int Galpha,Gbeta;
Line pv[MAX_DEPTH];
Move BEST,BEAST;
int Scores[MAX_DEPTH];
int Extendedplies;
Move MovesBuffer[MAX_DEPTH][MAXMOVES];
Move QMovesBuffer[MAX_DEPTH][MAXMOVES];
int nMovesBuffer[MAX_DEPTH];
int nQMovesBuffer[MAX_DEPTH];;
char NastyFenGlobal[MAXFENLEN];
int nullflag;
int Quiescence(int ply, Boardmap * Position, int alpha , int beta){
/* TODO:
	Add Pruning, some sort of standpat score or whatever,
	speeup search , test throughoutly ... :(
	Add promotions and maybe checks ...
*/
	int * nmoves,score,see,delta,Best;
	Move * moves;
Best = -1;
/*
MUTEX_LOCK(StopIterate);
if(thinking && clock() >= available_time){
 flags |= TIMEOUT;
}
score = flags;
MUTEX_UNLOCK(StopIterate);
if(score != 0) return Position->turn == WHITE ? alpha : beta;*/
			return Eval(Position/*??*/);
	 score = Eval(Position); //standpat

	 if(Position->turn == WHITE){
	  if(score > alpha){
           if(score >= beta) return beta;
           if(score > alpha) alpha = score;
          }
	 }
         else{
	  if(score < beta){
           if(score <= alpha) return alpha;
           if (score < beta) beta = score;
          }
	 }
//	 printf("\nQuiescence depth reached: %d => alpha: %d, beta: %d . %d\n",ply,alpha,beta,Position->turn);
//printBoardinfo(Position);
/*
	 if(Position->turn == WHITE){
	  if(score <= alpha) {//printf("\ncutted before anything\n");
           return score;
          }
	  else alpha = score;
	 }
	 else{
	  if(score >= beta) {//printf("\ncutted before anything\n");
           return score;
          }
	  else beta = score;
	 }*/
//printf("\n**************88\n");
	moves = QMovesBuffer[ply];
	nmoves = &nQMovesBuffer[ply];
	if(INCHECK(Position)) *nmoves = GenMoves(Position,moves);
	 else *nmoves = GenCaptures(Position,moves);

	if(ply <= 0 || *nmoves <= 0){
	// printf("\nQuiescence depth reached: %d => alpha: %d, beta: %d . %d\n",ply,alpha,beta,Position->turn);getchar();
	 GlobalQuiescenceNodes++;
//	 printBoardinfo(Position,-1);
	 return Eval(Position);
    	// return Position->turn == WHITE ? alpha : beta ;
	}

	/* Maybe Sort capture moves */

	//printf("\n%d captures at ply %d\n",*nmoves,ply);printBoardinfo(Position);getchar();
	for(int i = 0; i < *nmoves; ++i){
	 /*Filter moves by SEE()*/
//	 printf("\n AAAAAAAAAAAAAAAAAAAAA \n");
         see = SEE(&moves[i],Position);
//	 printf("\nmove quiescent => %d\n",i);
//printBoardinfo(Position);
         /* FIXTHIS: tried to add some sort of delta prunning but not sure
 	   also added not skip promotions */
/*	if(Position->turn == WHITE){
	 delta = alpha + see + 200 > beta;
 	}
	else{
	 delta = beta - see - 200 < alpha;
	}
	 if(see < 0 || ! delta && moves[i].promote < 0) continue;*/
/*         if(Position->turn == WHITE && ((alpha + see) <= beta)) continue;
         else if(Position->turn == BLACK && ((beta - see) >= alpha)) continue;*/
	 if(see < 0) continue;
	 MakeMove(&moves[i],Position);
	  if(!CHECK(Position)){ /// wontbe needded since i generate only legal moves
	   if(Best < 0) Best = i;
	   score = Quiescence(ply-1,Position,alpha,beta);
	  }
	  else {UnMakeMove(&moves[i],Position);continue;}
	 UnMakeMove(&moves[i],Position);

	 if(Position->turn == WHITE){
	  if(score > alpha){
//printf("HERE!"); getchar();
	    alpha = score;
	   if(alpha >= beta){ return alpha;/*printf("\nalpha cut here!\n");*/}
	  }

	 }else{
	  if(score < beta){
 //printf("HERE!");getchar();
	    beta = score;
	   if(beta <= alpha){return beta; /*printf("\nbeta cut here!\n");*/}
	  }
	 }
	}
/*	 if(Best < 0) {
	  if(INCHECK(Position)) return Position->turn == WHITE ? -INFINITY +current_depth - ply:INFINITY - current_depth + ply;
	  else return Position->turn == WHITE ? alpha - beta : beta + alpha; // DrawEval ...
	 }else*/
    	 return Position->turn == WHITE ? alpha : beta ;
}

int Search(int ply, Boardmap * Position, int alpha, int beta){

	int * nmoves,score,Best = -1,eval,turn,nullcut;
	Move * moves;
	turn = Position->turn;
	/*Add a mutex lock | unlock mechanism to recursively exit the search if a flag is set*/
int moveOrder[MAX_DEPTH];
MUTEX_LOCK(StopIterate);
if(thinking && clock() >= available_time){
 flags |= TIMEOUT;
}
score = flags;
MUTEX_UNLOCK(StopIterate);
if(score != 0) return Position->turn == WHITE ? alpha : beta;
//printBoardinfo(Position);getchar();
	//if(ply == 1) {GetFENof(Position,NastyFenGlobal); printf("\nfen: %s\n",NastyFenGlobal);getchar();}
	pv[ply].length = ply;
	if(ply == 0){
	/**/ GlobalNodeCount++;

	// printf("\ni'm evaluating!");
	/* if some conditions: != MAX_DEPTH, != checkmate or stalemate, other*/
	// return Quiescence(ply/*+ ?*/, Position, alpha, beta);
//	 return Eval(Position);
	 return Quiescence(MAX_DEPTH - current_depth - 1,Position,alpha,beta);
	}
	else {/* See if have moves of this position for the side to play ... else */ //printf("\ni'm in some node");
	/* Check if we've already generated moves for this position --?????*/
Best = -1;
score = 0;
	 moves = MovesBuffer[ply];/* === &MovesBuffer[ply][0]*/
	 nmoves = &nMovesBuffer[ply];
/*#ifndef ONLYLEGALGEN
	 if(INCHECK(Position)){
	  *nmoves = GenCheckEscapes(Position,moves);
#endif*/
/*if(*nmoves <= 0){
  for(int ii = 0; ii < *nmoves; ++ii){
   printf("\n%d) ",ii); printmove(&moves[ii],&Position,_SHORT);
  }
	   printf("\nGenerated %d check escapes\n",*nmoves);
	   printBoardinfo(Position); getchar();}//*/
/*#ifndef ONLYLEGALGEN
	 }else
#endif*/
	 *nmoves = GenMoves(Position,moves);
	 /*if(*nmoves <= 0){ // checkmate or stalemate
	  if(CHECK(Position)) return Position->turn ? -INFINITY : INFINITY;
	 }*/
/*
	   if(!(INCHECK(Position)) && ply > 4 ){
	     Position->turn ^= 1; // Null move, just switch which side is to move
	     score =  Search(ply-(ply > 6 ? 4 : 3),Position, alpha, beta);
		//printf("\nMade null move\n");
	     Position->turn ^= 1 ;
	     if((turn == WHITE  && score >= beta) || (turn == BLACK  && score <= alpha))
	      { nullcut = 1;
		if(turn == WHITE ) alpha = score;
		else if(turn == BLACK ) beta = score;
		 //printf("\nnull cut !!\n");
	         //return turn == WHITE ? alpha : beta; //break;// cut!
	         ply -= 4; // Null move reductions
		 if(ply <= 0) return Quiescence(MAX_DEPTH - current_depth - 1,Position,alpha,beta);
	      }
	   }*//*
 printf("\nGenerated moves:\n");
  for(int i = 0; i < *nmoves && i < 6; ++i){
   printf("\n%d) ",i); printmove(&moves[i],Position,_SHORT);
  }getchar();*/
	//SortMoves(*nmoves,moves,Position,ply); /* Sort using PV, killer moves heuristic, history heuristic, Ttable, etc ...*/
	SortMs(*nmoves,moves,Position,ply);
	 //printf("\nFisrt move in ply %d is: ",SortPly); printM(&moves[0],Position); printf("\n");
 /* for(int i = 0; i < *nmoves; ++i){
   printf("\n%d) ",i); printmove(&moves[i],&Position,_SHORT);
  }
  getchar();
  */
/*
printBoardinfo(Position);

 printf("\nSorted moves:\n");
  for(int i = 0; i < *nmoves && i < 6; ++i){
   printf("\n%d) ",i); printmove(&moves[i],Position,_SHORT);
  }getchar();*/
	 int check;
	 check = INCHECK(Position);
	 for(int i = 0; i < *nmoves; ++i){
//	 printf("\nmove => %d at depth %d\n",i,ply);
         /* Some sort of null move attempt ... mmm lest see */
	   nullcut = 0;
	  MakeMove(&moves[i],Position);// printBoardinfo(Position);getchar();
//#ifndef ONLYLEGALGEN
	  if(!CHECK(Position)){
//#endif
	  /* Search move in TT */
           int plyy;
       /*    plyy = ply > 3 ? 2 : 1;
           score = Search(ply - plyy,Position, alpha, beta);
	   if(((Position->turn^1 == WHITE && score> beta -200) || (Position->turn^1 == BLACK && score< alpha +200)) && ply > 3) //printf("\nFull search\n"),*/
/*	   if(TTprobe(Position->key,ply)){score = TTscore(Position->key,ply);}
	    else{ */
/* PV search window experiment*/
	     if(thinking && i > 0){
	      if(turn == WHITE) score = Search(ply-1,Position,alpha+1 ,alpha );
		else score = Search(ply-1,Position,beta ,beta -1);
	      if((turn == WHITE && score >= alpha) || (turn == BLACK && score <= beta))
	       score = Search(ply-1,Position, alpha, beta);
	     }else{// lmr try = (
	/*	if(thinking && i > 6)
	       score = Search(ply > 4 ? ply - 3 : ply-2,Position, alpha, beta); //*/
	//       if(score >= alpha || score <= beta)
	       score = Search(ply-1,Position, alpha, beta);

	      }
	     //TTstore(Position->key,&moves[i],ply,score);
	     /*printf("\ntake store\n");*/
	   /* }*/
//	if(ply == current_depth) printBoardinfo(Position),printf("\n\n score: %d\n",score);
	  if(Best < 0){
	   Best = i;
	   UpdatePV(ply,&moves[Best]);
	  }
	  /* Store in TT */
	  UnMakeMove(&moves[i],Position);
/*Minimax with alpha-beta prunning*/
	  if(Position->turn == WHITE){
	   if(score > alpha){ //printf("\n\t\tNO WAY~!");getchar();
	    Best = i;
	    alpha = score;
	   UpdatePV(ply,&moves[Best]);

	    if(!xboard && ply > current_depth - 2){

#if defined(_WIN32) ||defined( _WIN64)
#else
 printf("\e[F\e[J");
if(debug) printf("%d\t\t%d\t\t",alpha % 1000000,beta < 0 ? -((beta * -1) % 1000000) : beta % 1000000);
		Post(Position);
#endif
	    }
	   }
	   if(alpha >= beta){
	   /*add to killers, to history, to transposition table ,etc*/
	        memcpy(&Killers[turn],&moves[i],sizeof(Move));
	if(moves[i].castle == 0)  Mhistory[moves[Best].from][moves[Best].to] += ply;
		break; /* alpha cutoff */
	   }
	  }
	  else{
	   if(score < beta){ //printf("\n\t\tNO WAY 22222222~!");getchar();
	    Best = i;
	    beta = score;
	   UpdatePV(ply,&moves[Best]);

	    if(!xboard && ply > current_depth - 2){
#if defined(_WIN32) ||defined( _WIN64)
#else
 printf("\e[F\e[J");
if(debug) printf("%d\t\t%d\t\t",alpha % 1000000,beta < 0 ? -((beta * -1) % 1000000) : beta % 1000000);
Post(Position);
#endif
	    }
	   }
	   if(beta <= alpha){
	        memcpy(&Killers[turn],&moves[i],sizeof(Move));
	        if(moves[i].castle == 0) Mhistory[moves[Best].from][moves[Best].to] += ply;
	        break; /* beta cutoff */
	   }
	  }
//#ifndef ONLYLEGALGEN
	 }else{
		UnMakeMove(&moves[i],Position);

	   if(0 && debug){
		printf("\nIlegal move generated => \n");
		printM(&moves[i],Position);
	        printBoardinfo(Position);
	        if(getchar() == 'q') debug = 0;
	   }
	  }
//#endif
	}
	}
	if(Best < 0){
	  pv[ply].moves[0].flags = PV[ply].flags = NOMOVE;
	  pv[ply].length = 0;
	  if(ply == current_depth) BEAST.flags = NOMOVE;
	 if(INCHECK(Position)){
	  /*printf("\n found mate at ply = %d score = %d\n",ply,turn ? (-INFINITY)-ply : INFINITY + ply);
	  printBoardinfo(Position);
	  getchar();//*/
	  //return Position->turn ? (-INFINITY) - ply: INFINITY + ply;
	   turn ? (alpha = -INFINITY + (current_depth - ply)) : (beta = INFINITY - (current_depth - ply));
	 }
	 else return 0; //Position->turn ? -alpha : -beta; /* Some sort of DrawEvaluation */
	}
	 else{
	  if(ply == current_depth) memcpy(&BEAST,&moves[Best],sizeof(Move)), BEAST.flags = VALID;
/*	  PV[ply].from = moves[Best].from;
	  PV[ply].to = moves[Best].to;
	  PV[ply].castle = moves[Best].castle;
	  PV[ply].promote = moves[Best].promote;
	  PV[ply].flags = VALID;*/

	 }
	 if(ply == current_depth){
	  Galpha = alpha;
	  Gbeta = beta;
	 }
    return Position->turn == WHITE ? alpha : beta ;
}

static inline void UpdatePV(int ply, Move * move){
  /* TODO: Optimize the way i update the PV */

 memcpy(&pv[ply].moves[0],move,sizeof(Move));
 memcpy(&pv[ply].moves[1],&pv[ply-1].moves[0],pv[ply-1].length * sizeof(Move));
 pv[ply].length = pv[ply-1].length+1;
/*
if(post && ply == current_depth){
 printf("%d\t%d\t%.0f\t%d\t",current_depth,Scores[current_depth ],(available_time - clock())/10000,GlobalNodeCount);
 printPV(&pv[current_depth],NULL);
 printf("\n");
}*/
}
