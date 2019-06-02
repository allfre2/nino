#ifndef BOOK_H
#define BOOK_H
#endif

#define OPENING_NAME_LEN 30
#define MAXOPENINGMOVES 20
#define MAXBOOKSIZE 1024
typedef struct {

	int * BookMoves;
	short nmoves;
	unsigned int key;

}OpeningBookEntry;

int SaveOpeningBook(OpeningBookEntry *, char *, int);
void * ReadBook(char *, int *);
int freeBookMem(OpeningBookEntry *, int);
int ImproveBook(OpeningBookEntry *, Pgn *);
int toBookMove(Move *); /* Pack a Move struct into an int*/
int fromBookMove(int , Move *);

/* Opening Book binary format:
	int - > describing how many entries there are in the book (n_entries)
	int - > describing size of book in bytes (?? maybe)

	Then n_entries times this:

	int  - >  hashkey
	char - >  nmoves (moves to come in the file for that position)
	int [nmoves] - > actual moves in order
*/
/*
e4
	e5
		Nf3
		Bc4
		d4
		Nc3
	d5
		exd5
		Nc3
	Nf6

*/
/*
move in int format: `[]` == 1 byte
[flag][castle][from][to]

flags can be:
	0000 0001	!! (Brilliant move)	1111 1110	Full Engine Strength
	0000 0010	!  (Excelent move)	1111 1101	Very High Level Play
	0000 0100	!? (Interesting move)	1111 1011	Mikail Tal's disciple
	0000 1000	=  (Good move)		1111 0111	Good chess player
	0001 0000	   (Mediocre move)	1110 1111	Mediocre Chess Player
	0010 0000	?! (Jugada dudosa)	1101 1111	Weak Chess Player
	0100 0000	?  (Bad move)		1011 1111	Bad Chess Player

	1000 0000	White move
	0000 0000	Black move

*/
