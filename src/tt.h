#ifndef TTABLE_H
#define TTABLE_H
#endif
/*  32+mb     16mb     8mb    4mb      2mb    1mb   500+k  260+k    Need More? :P  */
/* 2097152  1048576  524288  262144  131072  65536  32768  16384                */
/*  2^21      2^20    2^19    ...                          2^14                 */

#define TTABLESIZE 0x040000 /* Default => 131072 => 4+mb of memory. */
#define PTABLESIZE 0x040000 /* Default Pawn Table size. 262144 entries*/

enum
{
	LAZY,
	AlPHA,
	BETA,

} NodeType;

typedef struct
{
	Move move[2]; /* One Move for white's turn and another for black's turn*/
	int score;
	char nodeType;
	int depth;		  /* If depth is  < 0 it means that the positions hasn't been seen before */
	unsigned int key; /* key for detecting collisions */

} TTEntry;

typedef struct
{
	int score;
	unsigned int key;
	char nodeType;
} PTEntry;

extern TTEntry *TTable;
extern PTEntry *PTable;
extern unsigned int TTSize;
int InitTTable(int);
int freeTTable();
int TTscore(unsigned int, int);
int TTmove(unsigned int, Move *);
int TTprobe(unsigned int, int);
int TTstore(unsigned int, Move *, int, int);