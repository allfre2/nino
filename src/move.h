/*--------------------------------------------------------------*
 | Engine name - move.h - Move code symbolic declarations.      |
 | License                                                      |
 | Written by: Alfredo LLuzon                                   |
 | Contact info: @hotmail.com, @gmail.com, website.com          |
 | last modifications by: <guy #n> : <modifications>            |
 *--------------------------------------------------------------*/
#ifndef MOVE_H
#define MOVE_H
#endif
#define EP_CAPTURE_SQ(p,sq) \
 ((sq)+ ((p) ? 8 : -8))
#define EP_XSQ(ep,turn) \
 ((ep) + ((turn) ? 8 : -8))
#define EP_SQ(Bmap,sq) \
 ()
#define MAX_MOVE_LEN 8

#define WICHROOK(n) \
 (((n) == 0 || (n) == 56)? q : ((n) == 7 || (n) == 63)? k : NONE)

extern Line SortPv;
extern int SortPly;
int MakeMove(Move * , Boardmap * );
int UnMakeMove(Move * , Boardmap * );

int SortMoves(int , Move [], Boardmap *, int);
int MoveIsCheck(Move * , Boardmap * );
int SortMs(int, Move [], Boardmap *, int);
int NextMove(int *, int );
int ValidateMove(Move *, Move [], Boardmap *, int);
int MoveCmp(Move * , Move *);
