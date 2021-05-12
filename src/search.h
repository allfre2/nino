#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H
#endif

#define UPDATEPV() \
	()

typedef struct
{
	unsigned int key;
	int nmoves;
} MoveRecord;

typedef struct
{
	int Nodes;
	int Captures;
	int eps;
	int Castles;
	int Promotions;
	int Checks;
	int CheckMates;

} SearchStats;

int Quiescence(int, Boardmap *, int, int);
int Search(int, Boardmap *, int, int);

static inline void UpdatePV(int, Move *);

extern Move PV[MAX_DEPTH]; // Principal Variation Heuristic
extern int PVScores[MAX_DEPTH];
extern Move BEST, TMP_BEST;
extern short Mhistory[64][64];
extern Move Killers[2];
extern int Scores[MAX_DEPTH];
extern int Galpha, Gbeta;
extern int GNodes[MAX_DEPTH];
extern int GlobalNodeCount;
extern int GlobalQuiescenceNodes;
extern SearchStats Statistics[MAX_DEPTH];
extern Line _PV;
extern Line pv[];