#include "common.h"
#include "notation.h"
#include "book.h"
#include <stdio.h>
#include <stdlib.h>

int SaveOpeningBook(OpeningBookEntry * OpeningBook, char * filename, int nEntries){

	FILE * book = fopen(filename,"wb");
	ASSERT(book != NULL);
	fwrite(&(nEntries),sizeof(int),1,book);  /*The Number of entries is the fisrt int of the file*/
	for(int i = 0; i < nEntries; ++i){
	 fwrite(&(OpeningBook[i].key),sizeof(unsigned int),1,book); // Key of the first position
	 fwrite(&(OpeningBook[i].nmoves),sizeof(short),1,book); // Number of moves to come
	  fwrite(OpeningBook[i].BookMoves,sizeof(int),OpeningBook[i].nmoves,book);
	}
	fclose(book);
}

void * ReadBook(char * BookFileName, int * nEntries){

	int result;

	FILE * book = fopen(BookFileName,"rb");

	if(book == NULL ) return NULL;

	 result = fread(nEntries,sizeof(int),1,book); // how many entries are in the book

	OpeningBookEntry * OpeningBook = malloc(sizeof(OpeningBookEntry) * (*nEntries));

	if(OpeningBook == NULL){ fclose(book); return NULL;}

	for(int i = 0 ; i < (*nEntries); ++i){
	 result = fread(&(OpeningBook[i].key),sizeof(unsigned int),1,book); // read positions hash key
	 result = fread(&(OpeningBook[i].nmoves),sizeof(short),1,book); // read nmoves[BLACK] and nmoves[WHITE]
	 OpeningBook[i].BookMoves = malloc(sizeof(int) * OpeningBook[i].nmoves);

	 if(OpeningBook[i].BookMoves == NULL){ fclose(book); free(OpeningBook); return NULL;}

	  fread((OpeningBook[i].BookMoves),sizeof(int),OpeningBook[i].nmoves,book);
	}

	fclose(book);
 return (void * )OpeningBook;
}

int freeBookMem(OpeningBookEntry * OpeningBook, int nEntries){

	for(int i = 0; i < nEntries && i < MAXBOOKSIZE; ++i){
	 free(OpeningBook[i].BookMoves);
	}

	free(OpeningBook);
}

int toBookMove(Move * move){

	int BookMove = 0;
/* Without handling puntuation or flags for the move for now*/
	if(move->castle){
	 BookMove |= (int)(move->castle) << 0x10;
	}else{
	 BookMove |= (int)(move->from) << 0x08;
	 BookMove |= (int)(move->to);
	 /* if the move is not castle and is promotion the castle part of the int holds the promotiong piece */
	 if(move->promote) BookMove |= (int)(move->promote) << 0x10;
	}
 return BookMove;
}

int fromBookMove(int BookMove, Move * move){

		     /* define this magic number 0x0000ffff  */
	if(BookMove & 0x0000ffff ){ /* If it has a from or to field then it is not a castling move*/

	// printf("\nBook has a castling move! FUUUUUUUUUUUCKKKKKKKKK\n");
	 move->to = (char) BookMove;
	 move->from = (char) (BookMove >> 0x08);
	 move->promote = (char) (BookMove >> 0x10);
	}else{
	 move->castle = (char) (BookMove >> 0x10);
	}
}
