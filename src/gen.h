#ifndef GEN_H
#define GEN_H
#endif

#define INVALIDSQ -1

#define MAXMOVES 100

#define SHIFTUR(v) \
	(((v) >> 7) | ((v) >> 14) | ((v) >> 21) | ((v) >> 28) | ((v) >> 35) | ((v) >> 42))
#define SHIFTUL(v) \
	(((v) >> 9) | ((v) >> 18) | ((v) >> 27) | ((v) >> 36) | ((v) >> 45) | ((v) >> 54))
#define SHIFTDR(v) \
	(((v) << 9) | ((v) << 18) | ((v) << 27) | ((v) << 36) | ((v) << 45) | ((v) << 54))
#define SHIFTDL(v) \
	(((v) << 7) | ((v) << 14) | ((v) << 21) | ((v) << 28) | ((v) << 35) | ((v) << 42))

#define SHIFTRIGHT(v) \
	(((v) << 1) | ((v) << 2) | ((v) << 3) | ((v) << 4) | ((v) << 5) | ((v) << 6))
#define SHIFTLEFT(v) \
	(((v) >> 1) | ((v) >> 2) | ((v) >> 3) | ((v) >> 4) | ((v) >> 5) | ((v) >> 6))
#define SHIFTUP(v) \
	(((v) >> 8) | ((v) >> 16) | ((v) >> 24) | ((v) >> 32) | ((v) >> 40) | ((v) >> 48))
#define SHIFTDOWN(v) \
	(((v) << 8) | ((v) << 16) | ((v) << 24) | ((v) << 32) | ((v) << 40) | ((v) << 48))

#define ROOKMOVES(sq, moves, allPieces, Bmap, side)              \
	{                                                            \
                                                                 \
		(moves) = RightSqs[(sq)] & (allPieces);                  \
		(moves) = SHIFTRIGHT((moves));                           \
		(moves) = (((moves)&RightSqs[(sq)]) ^ RightSqs[(sq)]);   \
		(Bmap)->attacks[sq] |= (moves);                          \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)]; \
                                                                 \
		(moves) = LeftSqs[(sq)] & (allPieces);                   \
		(moves) = SHIFTLEFT(moves);                              \
		(moves) = (((moves)&LeftSqs[(sq)]) ^ LeftSqs[(sq)]);     \
		(Bmap)->attacks[sq] |= (moves);                          \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)]; \
                                                                 \
		(moves) = UpSqs[(sq)] & allPieces;                       \
		(moves) = SHIFTUP((moves));                              \
		(moves) = (((moves)&UpSqs[(sq)]) ^ UpSqs[(sq)]);         \
		(Bmap)->attacks[sq] |= (moves);                          \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)]; \
                                                                 \
		(moves) = DownSqs[(sq)] & (allPieces);                   \
		(moves) = SHIFTDOWN((moves));                            \
		(moves) = (((moves)&DownSqs[(sq)]) ^ DownSqs[(sq)]);     \
		(Bmap)->attacks[sq] |= (moves);                          \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)]; \
	}

#define BISHOPMOVES(sq, moves, allPieces, Bmap, side)                \
	{                                                                \
                                                                     \
		(moves) = BishopURSqs[(sq)] & (allPieces);                   \
		(moves) = SHIFTUR((moves));                                  \
		(moves) = (((moves)&BishopURSqs[(sq)]) ^ BishopURSqs[(sq)]); \
		(Bmap)->attacks[sq] |= (moves);                              \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)];     \
                                                                     \
		(moves) = BishopULSqs[(sq)] & (allPieces);                   \
		(moves) = SHIFTUL((moves));                                  \
		(moves) = (((moves)&BishopULSqs[(sq)]) ^ BishopULSqs[(sq)]); \
		(Bmap)->attacks[sq] |= (moves);                              \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)];     \
                                                                     \
		(moves) = BishopDRSqs[(sq)] & (allPieces);                   \
		(moves) = SHIFTDR(moves);                                    \
		(moves) = (((moves)&BishopDRSqs[(sq)]) ^ BishopDRSqs[(sq)]); \
		(Bmap)->attacks[sq] |= (moves);                              \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)];     \
                                                                     \
		(moves) = BishopDLSqs[sq] & (allPieces);                     \
		(moves) = SHIFTDL((moves));                                  \
		(moves) = (((moves)&BishopDLSqs[(sq)]) ^ BishopDLSqs[(sq)]); \
		(Bmap)->attacks[sq] |= (moves);                              \
		(Bmap)->moves[(sq)] |= (moves) & ~(Bmap)->empty[(side)];     \
	}

enum
{
	PSEUDOLEGAL,
	LEGAL,
	CHECK,
	CAPTURE,
	PROMOTION
} MoveType;

/* Got this idea from TSCP :P thanks to the autor. */
static char MailBox[145] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
	-1, -1, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1,
	-1, -1, 16, 17, 18, 19, 20, 21, 22, 23, -1, -1,
	-1, -1, 24, 25, 26, 27, 28, 29, 30, 31, -1, -1,
	-1, -1, 32, 33, 34, 35, 36, 37, 38, 39, -1, -1,
	-1, -1, 40, 41, 42, 43, 44, 45, 46, 47, -1, -1,
	-1, -1, 48, 49, 50, 51, 52, 53, 54, 55, -1, -1,
	-1, -1, 56, 57, 58, 59, 60, 61, 62, 63, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static char MailBoxNumber[64] = {
	26, 27, 28, 29, 30, 31, 32, 33,
	38, 39, 40, 41, 42, 43, 44, 45,
	50, 51, 52, 53, 54, 55, 56, 57,
	62, 63, 64, 65, 66, 67, 68, 69,
	74, 75, 76, 77, 78, 79, 80, 81,
	86, 87, 88, 89, 90, 91, 92, 93,
	98, 99, 100, 101, 102, 103, 104, 105,
	110, 111, 112, 113, 114, 115, 116, 117};

int initBitmaps();
uint64 Xray(uint64, uint64, uint64);
uint64 GetRookRay(uint64, uint64);
uint64 GetBishopRay(uint64, uint64);
int GenMoves(Boardmap *, Move[]);
int GenCaptures(Boardmap *, Move[]);
int GenCheckEscapes(Boardmap *, Move[]);
int Map(Boardmap *);
int _fastMap(Boardmap *, uint64);

extern const int AttackTable[][10];
extern uint64 Pwns[2];

extern uint64 RightSqs[64];
extern uint64 LeftSqs[64];
extern uint64 UpSqs[64];
extern uint64 DownSqs[64];

extern uint64 BishopURSqs[64];
extern uint64 BishopULSqs[64];
extern uint64 BishopDRSqs[64];
extern uint64 BishopDLSqs[64];

extern uint64 KnightSqs[64];

extern uint64 PwnSqs[2][64];
extern uint64 KingSqs[64];

extern uint64 AllRays[64];
extern uint64 CheckRays[64];
extern uint64 AllBishopSqs[64];
extern uint64 AllRookSqs[64];