#ifndef BOOK_H
#define BOOK_H
#endif

#include <stdio.h>

// ABK (Arena)

typedef struct MoveEntry
{
	char from;      /* a1 0, b1 1, ..., h1 7, ... h8 63 */
	char to;        /* a1 0, b1 1, ..., h1 7, ... h8 63 */
	char promotion; /* 0 none, +-1 rook, +-2 knight, +-3 bishop, +-4 queen */
	char priority;
	int ngames;
	int nwon;
	int nlost;
	int plycount;
	int nextMove;
	int nextSibling;
} ABKMoveEntry;

typedef struct Book
{
	ABKMoveEntry * book;
	int size;
} ABKBook;

extern const int ABKStartIndex;

ABKBook * ReadOpeningBook (char *);
void CloseOpeningBook(ABKBook *);
void PrintMoveEntry(ABKMoveEntry *);
void FromBookMove(ABKMoveEntry *, Move *);