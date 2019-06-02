#ifndef CHESS_DEBUG_H
# define CHESS_DEBUG_H
#endif

#define NOTIMPLEMENTED printf(":'( \nNot implemented\n")

int printmoves(Boardmap *, int );
void printull(unsigned long long x);
void printBitmap(uint64, int);
void debugSignals(int);
int fakeperft(int);

typedef struct{

 Boardmap * Position;

 int (*makefn)    (Move *, Boardmap *);
 int (*unmakefn)  (Move *, Boardmap *);

 int (*genfn)     (Boardmap *, Move *);
 int (*capturesfn)(Boardmap *, Move *);
}_perftArgs;

typedef struct{
 int nodes;
 int captures;
 int ep;
 int castles;
 int promotions;
 int checks;
 int mates;

}_perftResults;

int _perft(int, _perftArgs *, _perftResults []);
void printBoardinfo(Boardmap *);
void printmove(Move *, Boardmap *, int);
int printM(Move *,Boardmap *);
int printPV(Line *, Boardmap *);
void printattckd(Boardmap *, char );
void debugFramework(); /* A console like state that parses many useful commands for debuging the chess engine */
int epdTest(char *);
extern uint64 higligth;

