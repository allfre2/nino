#include "common.h"
#include "notation.h"
#include "book.h"
#include <stdio.h>
#include <stdlib.h>

const int ABKStartIndex = 900;

ABKBook * ReadOpeningBook (char * filename)
{
	ABKBook * abk = malloc(sizeof(ABKBook));
	FILE * file = fopen(filename, "r");
	printf("%x", file);

	fseek(file, 0, SEEK_END);
	int size = (ftell(file)/sizeof(ABKMoveEntry)) - ABKStartIndex;

	printf("\n%x\nsize: %d", file, size); getchar();
	
	fseek(file, 0, SEEK_SET); // rewind
	fseek(file, ABKStartIndex*sizeof(ABKMoveEntry), 0); // Start to read moves From ABKStartIndex

	ABKMoveEntry * book = malloc(sizeof(ABKMoveEntry) * size);

	for (int i = 0; i < size; ++i)
	{
		fread(&(book[i]), sizeof(ABKMoveEntry), 1, file);
	}

	for (int i = 0; i < size; ++i)
	{
		if (book[i].to != 0)
		{
			PrintMoveEntry(&(book[i]));
			int from = (56 - ((book[i].from/8)*8)) + (book[i].from % 8);
			int to = (56 - ((book[i].to/8)*8)) + (book[i].to % 8);
			printf("\n%s%s\n", algebraic[from], algebraic[to]);
			getchar();
		}
	}

	abk->book = book;
	abk->size = size;

	free(book);
	free(abk);
	fclose(file);

	return NULL;
}

void CloseOpeningBook(ABKBook * abk)
{
	free(abk->book);
	free(abk);
}

void PrintMoveEntry(ABKMoveEntry * book)
{
	printf("\nfrom: %i\n to: %i\n nextMove: %i\n nextSibling: %i\n, ngames: %i\n, nlost: %i\n nwon: %i\n plycount: %i\n priority: %i\n promotion: %i\n",
		book->from,
		book->to,
		book->nextMove,
		book->nextSibling,
		book->ngames,
		book->nlost,
		book->nwon,
		book->plycount,
		book->priority,
		book->promotion);
}

void FromBookMove(ABKMoveEntry * book, Move * move)
{
	move->from = (56 - ((book->from/8)*8)) + (book->from % 8);
	move->to = (56 - ((book->to/8)*8)) + (book->to % 8);
	move->promote = book->promotion; // ?
}