#ifndef CHESS_DEBUG_H
#define CHESS_DEBUG_H
#endif

#define NOTIMPLEMENTED printf("\n Not implemented :'(\n")

typedef struct
{
    Boardmap *Position;

    int (*makefn)(Move *, Boardmap *);
    int (*unmakefn)(Move *, Boardmap *);

    int (*genfn)(Boardmap *, Move *);
    int (*capturesfn)(Boardmap *, Move *);
} _perftArgs;

typedef struct
{
    int nodes;
    int captures;
    int ep;
    int castles;
    int promotions;
    int checks;
    int mates;

} _perftResults;

extern uint64 higligth;

void printBoardinfo(Boardmap *);
void printull(unsigned long long x);
int  printmoves(Boardmap *, int);
void printBitmap(uint64, int);
void printmove(Move *, Boardmap *, int);
int  printM(Move *, Boardmap *);
int  printPV(Line *, Boardmap *);
void printattckd(Boardmap *, char);

int fakeperft(int);
int _perft(int, _perftArgs *, _perftResults[]);

void debugFramework();
void debugSignals(int);
int epdTest(char *);