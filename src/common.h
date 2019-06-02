#ifndef COMMON_H
# define COMMON_H
#endif
#if defined(_WIN32) || defined(_WIN64)
#  define NEWLINE 0x0a0d
#else
#  define NEWLINE 0x000a
#endif

//#define ONLYLEGALGEN
/*??????????????????*/
#define ASSERT(cond) \
 if(!(cond)) return -1;
#define INFINITY 0x50000000
#define oo INFINITY
#define INVALID -1
#define WHITE 1
#define BLACK 0
#define SHORTCASTLE 0x02
#define LONGCASTLE 0x04
#define EMPTY 12
#define EMPTYCHAR '.'
#ifndef NULL
#define NULL (void *)0
#endif
#define MAX_DEPTH 30
#define GAMEMAXMOVES 200
#define ISDIGIT(n) \
 ((n) >= '0' && (n) <= '9')
#define ISLOWER(c) \
 ((c) & 0x20)
#define LOWERCASE(c) \
 ((c) | 0x20)
#define ABS(n) \
 (((n) > 0)?(n):(-(n)))
#define BIT(n) \
 ((one) << (n))
#define FLIPBIT(n,bitmap) \
 ((bitmap) |= ((one) << (n)))
#define LONG_RANGE(Piece) \
 ((Piece) > 1 && (Piece) < 5)
#define PAWNSTARTSQ(sq,side) \
 ((((side) == BLACK) && (sq) < 16 && (sq) >= 8) \
	|| (((side) == WHITE) && (sq) < 56 && (sq) > 47))
#define PROMOTESQ(sq) \
 ((sq) < 8 || ((sq) > 55 && (sq) < 64))
#define SIDE(p) \
 (((p) < 6)?BLACK:WHITE)
#define COLOR(p) \
 (((p) < 6)?BLACK:WHITE)
#define NPIECE(p) \
 ((p)%6)
#define FRONTSQ(sq,side) \
 ((sq) + ((side) ? -8 : 8))
#define MYEPSQ(sq,side) \
 (((side) == WHITE && ((sq) < 24 && (sq) > 15 )) ||((side == BLACK) && ((sq) < 48 && (sq) > 39)))
#define CASTLESHORT(rights) \
 ((rights) & ((Castling_rights[K] | Castling_rights[k])))
#define CASTLELONG(rights) \
 ((rights) & ((Castling_rights[Q] | Castling_rights[q])))
#define CHECK(Bmaptr) \
 ((Bmaptr)->pieces[KING][(Bmaptr)->turn^1] & (Bmaptr)->attackd[(Bmaptr)->turn])
#define INCHECK(Bmaptr) \
 ((Bmaptr)->pieces[KING][(Bmaptr)->turn] & (Bmaptr)->attackd[(Bmaptr)->turn^1])
#define BITCOUNT(value,count) \
 for((count) = 0; (value); ++(count)) \
  value &= (value -1);
#define PIECECOUNT(value,count) \
 BITCOUNT((value),(count))
#define NEXTSQ(piece) \
 ((piece) & (~((piece)-1)))
#define LSBCLEAR(value) \
 ((value) &= (value) - 1)
#define LSB(var) \
 ((var) & (~((var)-1)))
#define MSB(var,l) \
 (l) = 0x1; \
 uint64 _t_ = (var); while( _t_ >>= 1) (l) <<= 1;
#define BITSPACE(a,b) \
 ((((a)-1) ^ ((b)-1)) | (a) | (b))
/* Just plain Google'd this idea and found this method here :
http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
*/
#define LOG2(v,l) \
 {(l) = 64;\
if ((v) & 0x00000000FFFFFFFF) (l) -= 32;\
if ((v) & 0x0000FFFF0000FFFF) (l) -= 16;\
if ((v) & 0x00FF00FF00FF00FF) (l) -= 8;\
if ((v) & 0x0F0F0F0F0F0F0F0F) (l) -= 4;\
if ((v) & 0x3333333333333333) (l) -= 2;\
if ((v) & 0x5555555555555555) (l) -= 1;\
(l) = l-1;\
}

#define PROMOTESQMASK 0xff000000000000ff

typedef unsigned long long uint64;

enum {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};

enum {
	NONE,
	K,
	k,
	Q,
	q,
	ALL
};

enum {
	ATTCKD,
	BEETWEN,
	KINGSQ,
	ROOKSQ
}CastleMask;

enum {
	NOMOVE = -1,
	VALID = 0,
}MoveFlags;

typedef char int8;

typedef struct {

char board[64];

char castle;
char ep;
char turn;

uint64 pieces[6][2];
uint64 Pieces[12]; /* -->  will take a long time to replace all the code*/
uint64 empty[2];/* TODO: change name to occupied */
uint64 attackd[2];
uint64 attacks[64];
uint64 moves[64];

unsigned int key;

}Boardmap;

typedef struct {
char from;
char to;
char saved;
char savedcastle;
char promote;
char ep;
char castle;
char flags;
}Move;

typedef struct{
	Move moves[MAX_DEPTH];
	int length;
}Line;

typedef struct {

Boardmap Position;
Move moves[GAMEMAXMOVES];
double available_time;
int MaxDepth;

}Game;

void Initboard(Boardmap *,char *);
int ValidatePosition(Boardmap *);//<------------------
int GetpieceN(char );//??????
int Select(char, char *);//????
int Mirror(Boardmap *);
int PVString(char *,Move [],int);

char Log2(uint64);
extern int debug;
extern int xboard;
extern const uint64 one;
extern const uint64 zero;
extern const char Pieces[];
extern const char* wPieces[];
extern const char Castling_rights[];
extern const uint64 epSqs[];
extern const uint64 promoteSqs[];
extern const uint64 CenterSqs;
extern const char StartPositionFEN[]; // ???
extern char algebraic[64][4];
extern const uint64 CastleShortMask[][2];
extern const uint64 CastleLongMask[][2];
extern const uint64 PwnStartSqs[2];
extern unsigned int RandomNumbers[13][64];
extern unsigned int epRandomNumbers[64];
extern unsigned int castleRandomNumbers[0x10];
extern int Map(Boardmap *);
extern int fastMap(Boardmap *);
extern int _fastMap(Boardmap *,uint64 );
extern uint64 PowerOfTwo(uint64);
extern unsigned int HashKey(Boardmap *);
extern uint64 PwnFwdSqs[2][64];
extern uint64 Pwn2StepSqs[2][64];
extern int current_depth;
extern int thinking;
extern double available_time;
