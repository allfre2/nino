#include "common.h"
#include "hash.h"
#include "hashNumbers.c"
#include <stdlib.h>
#include <time.h>

unsigned int HashKey(Boardmap * Position){

	unsigned int key = 0;
	for(int i = 0; i < 64; ++i){
	 if(Position->board[i] != EMPTY)
	  key ^= RandomNumbers[Position->board[i]][i];
	}
	key ^= epRandomNumbers[Position->ep];
	key ^= castleRandomNumbers[Position->castle];
 return key ;//& 0x7fffffff; /* Get rid of sign bit */
}

unsigned int PawnHash(Boardmap * Position){

	unsigned int key = 0;

	for(short i = 0; i < 64; ++i){
	 if(Position->board[i] != EMPTY && NPIECE(Position->board[i]) == PAWN)
	  key ^= RandomNumbers[Position->board[i]][i];
	}

 return key ;//&  0x7fffffff;
}

int InitRandomTable(int Default){

	srand(time(NULL) /*+ getpid()*/);

	for(int p = PAWN; p < 12; ++p)
	 for(int i = 0; i < 64; ++i)
	 if(p == 12) RandomNumbers[p][i] = 0; else
	  if (Default) RandomNumbers[p][i] = DefaultRandomNumbers[p][i];
	  else RandomNumbers[p][i] = rand()%0xffffffff;

	for(int i = 0; i < 64; ++i)
	  if(i == 0) epRandomNumbers[i] = 0;
	  else
	  if (Default) epRandomNumbers[i] = DefaultepRandomNumbers[i];
	  else epRandomNumbers[i] = rand()%0xffffffff;

	for(int i = 0; i < 0x10; ++i)
	  if (Default) castleRandomNumbers[i] = DefaultcastleRandomNumbers[i];
	  else castleRandomNumbers[i] = rand()%0xffffffff;
}
/*
int PrintRandomTable(){
	srand(time(NULL) /*+ getpid());

	for(int p = PAWN; p < 12; ++p,printf("\n\n"))
	 for(int i = 0; i < 64; ++i)
;

	for(int i = 0; i < 64; ++i)
;

	for(int i = 0; i < 0x10; ++i)
	  printf("0x%x, ",rand()%0xffffffff);
}*/
