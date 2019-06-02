#ifndef EVAL_H
#define EVAL_H
#endif

int Eval(Boardmap *); /* Lazy evaluation */
int Material(Boardmap *);
int DrawEval(Boardmap *);    /* Eval position for a draw */
int SEE(Move *, Boardmap *); /* Static exchange evaluation */
int Lazyflag;

extern const int Relative_Piece_Value[6];
/* Piece square tables */

extern short PieceSqValueT[6][64];