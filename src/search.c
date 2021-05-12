#include "common.h"
#include "notation.h"
#include "gen.h"
#include "move.h"
#include "hash.h"
#include "tt.h"
#include "eval.h"
#include "search.h"
#include "thread.h"
#include "xboard.h"
#include "debug.h"
#include <string.h>

/*DELETE ME*/
int GlobalNodeCount;
int GlobalQuiescenceNodes;
int GNodes[MAX_DEPTH];
SearchStats Statistics[MAX_DEPTH];
/***************/
short Mhistory[64][64]; // History heuristic
Move Killers[2];
Move PV[MAX_DEPTH]; // Principal Variation Heuristic
int PVdepth;
int Galpha, Gbeta;
Line pv[MAX_DEPTH];
Move BEST, TMP_BEST;
int Scores[MAX_DEPTH];
int Extendedplies;
Move MovesBuffer[MAX_DEPTH][MAXMOVES];
Move QMovesBuffer[MAX_DEPTH][MAXMOVES];
int nMovesBuffer[MAX_DEPTH];
int nQMovesBuffer[MAX_DEPTH];
char NastyFenGlobal[MAXFENLEN];
int nullflag;

int Quiescence(int ply, Boardmap *Position, int alpha, int beta)
{
	int *nmoves, score, see, delta, Best;
	Move *moves;
	Best = -1;
	return Eval(Position);
	score = Eval(Position);

	if (Position->turn == WHITE)
	{
		if (score > alpha)
		{
			if (score >= beta)
				return beta;
			if (score > alpha)
				alpha = score;
		}
	}
	else
	{
		if (score < beta)
		{
			if (score <= alpha)
				return alpha;
			if (score < beta)
				beta = score;
		}
	}
	moves = QMovesBuffer[ply];
	nmoves = &nQMovesBuffer[ply];

	if (INCHECK(Position))
		*nmoves = GenMoves(Position, moves);
	else
		*nmoves = GenCaptures(Position, moves);

	if (ply <= 0 || *nmoves <= 0)
	{
		GlobalQuiescenceNodes++;
		return Eval(Position);
	}

	for (int i = 0; i < *nmoves; ++i)
	{
		see = SEE(&moves[i], Position);

		if (see < 0)
			continue;

		MakeMove(&moves[i], Position);

		if (!CHECK(Position))
		{
			if (Best < 0)
				Best = i;
			score = Quiescence(ply - 1, Position, alpha, beta);
		}
		else
		{
			UnMakeMove(&moves[i], Position);
			continue;
		}
		UnMakeMove(&moves[i], Position);

		if (Position->turn == WHITE)
		{
			if (score > alpha)
			{
				alpha = score;
				if (alpha >= beta)
				{
					return alpha;
				}
			}
		}
		else
		{
			if (score < beta)
			{
				beta = score;
				if (beta <= alpha)
				{
					return beta;
				}
			}
		}
	}

	return Position->turn == WHITE ? alpha : beta;
}

int Search(int ply, Boardmap *Position, int alpha, int beta)
{
	int *nmoves, score, Best = -1, eval, turn, nullcut;
	Move *moves;
	turn = Position->turn;
	/*Add a mutex lock | unlock mechanism to recursively exit the search if a flag is set*/
	int moveOrder[MAX_DEPTH];
	MUTEX_LOCK(StopIterate);

	if (thinking && clock() >= available_time)
	{
		flags |= TIMEOUT;
	}

	score = flags;
	MUTEX_UNLOCK(StopIterate);

	if (score != 0)
		return Position->turn == WHITE ? alpha : beta;
	pv[ply].length = ply;

	if (ply == 0)
	{
		GlobalNodeCount++;
		return Quiescence(MAX_DEPTH - current_depth - 1, Position, alpha, beta);
	}
	else
	{
		Best = -1;
		score = 0;
		moves = MovesBuffer[ply];
		nmoves = &nMovesBuffer[ply];

		*nmoves = GenMoves(Position, moves);

		SortMs(*nmoves, moves, Position, ply);

		int check;
		check = INCHECK(Position);

		for (int i = 0; i < *nmoves; ++i)
		{
			nullcut = 0;
			MakeMove(&moves[i], Position);

			if (!CHECK(Position))
			{
				int plyy;
				if (thinking && i > 0)
				{
					if (turn == WHITE)
						score = Search(ply - 1, Position, alpha + 1, alpha);
					else
						score = Search(ply - 1, Position, beta, beta - 1);
					if ((turn == WHITE && score >= alpha) || (turn == BLACK && score <= beta))
						score = Search(ply - 1, Position, alpha, beta);
				}
				else
				{
					score = Search(ply - 1, Position, alpha, beta);
				}

				if (Best < 0)
				{
					Best = i;
					UpdatePV(ply, &moves[Best]);
				}
				/* Store in TT */
				UnMakeMove(&moves[i], Position);
				/*Minimax with alpha-beta prunning*/
				if (Position->turn == WHITE)
				{
					if (score > alpha)
					{
						Best = i;
						alpha = score;
						UpdatePV(ply, &moves[Best]);

						if (!xboard && ply > current_depth - 2)
						{
						#if defined(_WIN32) || defined(_WIN64)

						#else
							printf("\e[F\e[J");
							if (debug)
								printf("%d\t\t%d\t\t", alpha % 1000000, beta < 0 ? -((beta * -1) % 1000000) : beta % 1000000);
							Post(Position);
						#endif
						}
					}

					if (alpha >= beta)
					{
						/*add to killers, to history, to transposition table ,etc*/
						memcpy(&Killers[turn], &moves[i], sizeof(Move));
						if (moves[i].castle == 0)
							Mhistory[moves[Best].from][moves[Best].to] += ply;
						break; /* alpha cutoff */
					}
				}
				else
				{
					if (score < beta)
					{
						Best = i;
						beta = score;
						UpdatePV(ply, &moves[Best]);

						if (!xboard && ply > current_depth - 2)
						{
						
						#if defined(_WIN32) || defined(_WIN64)

						#else
							printf("\e[F\e[J");
							if (debug)
								printf("%d\t\t%d\t\t", alpha % 1000000, beta < 0 ? -((beta * -1) % 1000000) : beta % 1000000);
							Post(Position);
						#endif
						}
					}
					if (beta <= alpha)
					{
						memcpy(&Killers[turn], &moves[i], sizeof(Move));
						if (moves[i].castle == 0)
							Mhistory[moves[Best].from][moves[Best].to] += ply;
						break; /* beta cutoff */
					}
				}
			}
			else
			{
				UnMakeMove(&moves[i], Position);

				if (0 && debug)
				{
					printf("\nIlegal move generated => \n");
					printM(&moves[i], Position);
					printBoardinfo(Position);
					if (getchar() == 'q')
						debug = 0;
				}
			}
		}
	}

	if (Best < 0)
	{
		pv[ply].moves[0].flags = PV[ply].flags = NOMOVE;
		pv[ply].length = 0;

		if (ply == current_depth)
			TMP_BEST.flags = NOMOVE;

		if (INCHECK(Position))
		{
			turn ? (alpha = -INFINITY + (current_depth - ply)) : (beta = INFINITY - (current_depth - ply));
		}
		else
			return 0;
	}
	else
	{
		if (ply == current_depth)
			memcpy(&TMP_BEST, &moves[Best], sizeof(Move)), TMP_BEST.flags = VALID;
	}

	if (ply == current_depth)
	{
		Galpha = alpha;
		Gbeta = beta;
	}

	return Position->turn == WHITE ? alpha : beta;
}

static inline void UpdatePV(int ply, Move *move)
{
	/* TODO: Optimize the way i update the PV */
	memcpy(&pv[ply].moves[0], move, sizeof(Move));
	memcpy(&pv[ply].moves[1], &pv[ply - 1].moves[0], pv[ply - 1].length * sizeof(Move));
	pv[ply].length = pv[ply - 1].length + 1;
}