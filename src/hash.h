#ifndef CHESS_HASH_H
#define CHESS_HASH_H
#endif
#define NPRIMES 11

unsigned int HashKey(Boardmap *);
unsigned int PawnHash(Boardmap *);
int InitRandomTable(int);
int PrintRandomTable();
unsigned int RandomNumbers [13][64];
unsigned int epRandomNumbers[64];
unsigned int castleRandomNumbers[0x10];

