#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "hash.h"
#include "tt.h"

TTEntry * TTable;
PTEntry * PTable;
unsigned int TTSize;

int InitTTable(int size){
 TTable = malloc(size * sizeof(TTEntry));
// if(TTable == NULL) printf("\nMIERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRDAAAAAAAAAAAAAAAAA\n");
 memset(TTable,-1,size * sizeof(TTEntry));
 return (int)TTable;
}

int freeTTable(){
  free(TTable);
}

int TTscore(unsigned int key, int ply){

 key = (key ) % TTSize;

 return TTable[key].score;

}

int TTmove(unsigned int key, Move * move){

 key = (key ) % TTSize;

 move->from = TTable[key].move[0].from;
 move->to = TTable[key].move[0].to;
 move->castle = TTable[key].move[0].castle;
 move->promote = TTable[key].move[0].promote;
 move->flags = TTable[key].move[0].flags;

}

int TTprobe(unsigned int key, int ply){

 unsigned int ikey;
 ikey = (key ) % TTSize;
 //if(key > TTSize) printf("\n\tDO'h\n");
 if(TTable[ikey].depth > -1 && TTable[ikey].depth >= ply && key == TTable[ikey].key) return 1; else return 0;

}

int TTstore(unsigned int key, Move * move, int ply, int score){

 unsigned int ikey;

 ikey = (key ) % TTSize;
 TTable[ikey].score = score;
 TTable[ikey].key = key;
 TTable[ikey].depth = ply;
 memcpy(&TTable[ikey].move[0],move,sizeof(Move));

}

