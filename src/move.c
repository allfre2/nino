#include "common.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "search.h"
#include "debug.h"
#include <stdio.h>
#include "notation.h"
#include <string.h>

int SortPly;

int MakeMove(Move *move, Boardmap *Position)
{
	Boardmap TMP;
	char ep_xsquare, delta, kingSq, rookSq;

	uint64 kingBit, rookBit, ep_xsquareBit, toBit, fromBit, affectedSqs;

	affectedSqs = 0;
	fromBit = BIT(move->from);
	toBit = BIT(move->to);

	move->ep = Position->ep;			  /*Save en passant square*/
	Position->ep = 0;					  /*Clear en passant square*/
	move->savedcastle = Position->castle; /*Save castling rights*/

	/*Update Zobrist Hash*/
	Position->key ^= castleRandomNumbers[move->savedcastle] ^ epRandomNumbers[move->ep];

	if (move->castle & SHORTCASTLE)
	{
		kingSq = Position->turn ? 60 : 4;
		rookSq = Position->turn ? 63 : 7;

		kingBit = BIT(kingSq);
		rookBit = BIT(rookSq);
		/*Update affectedSqs*/
		affectedSqs |= kingBit | rookBit;

		/* Update Zobrist Hash */

		Position->key ^= RandomNumbers[Position->board[kingSq]][kingSq] ^ RandomNumbers[Position->board[rookSq]][rookSq];

		/*Clear current castle squares*/
		Position->board[kingSq] = EMPTY;
		Position->board[rookSq] = EMPTY;

		/* Update bitmaps */
		Position->pieces[KING][Position->turn] &= ~kingBit;
		Position->pieces[ROOK][Position->turn] &= ~rookBit;

		Position->empty[Position->turn] &= ~(kingBit | rookBit);

		/*Set castled king and rook*/
		Position->board[kingSq + 2] = KING + (Position->turn ? 6 : 0);
		Position->board[kingSq + 1] = ROOK + (Position->turn ? 6 : 0);

		/*Update Zobrist Hash */
		Position->key ^= RandomNumbers[Position->board[kingSq + 2]][kingSq + 2] ^ RandomNumbers[Position->board[kingSq + 1]][kingSq + 1];

		/*Update bitmaps*/
		Position->pieces[KING][Position->turn] |= kingBit << 2;
		Position->pieces[ROOK][Position->turn] |= kingBit << 1;
		/*Update affectedSqs*/
		affectedSqs |= (kingBit << 2) | (kingBit << 1);

		Position->empty[Position->turn] |= (kingBit << 2) | (kingBit << 1);

		/*Clear corresponding castling rights*/
		Position->castle &= (Castling_rights[K] | Castling_rights[Q]) << Position->turn;
	}
	else if (move->castle & LONGCASTLE)
	{
		kingSq = Position->turn ? 60 : 4;
		rookSq = Position->turn ? 56 : 0;

		kingBit = BIT(kingSq);
		rookBit = BIT(rookSq);
		/*Update affectedSqs*/
		affectedSqs |= kingBit | rookBit;

		/*Update Zobrist Hash*/

		Position->key ^= RandomNumbers[Position->board[kingSq]][kingSq] ^ RandomNumbers[Position->board[rookSq]][rookSq];

		/*Clear current castle squares*/
		Position->board[kingSq] = EMPTY;
		Position->board[rookSq] = EMPTY;

		/*upsydatey bitmaps*/
		Position->pieces[KING][Position->turn] &= ~kingBit;
		Position->pieces[ROOK][Position->turn] &= ~rookBit;

		Position->empty[Position->turn] &= ~(kingBit | rookBit);

		/*Set castled king and rook*/
		Position->board[kingSq - 2] = KING + (Position->turn ? 6 : 0);
		Position->board[kingSq - 1] = ROOK + (Position->turn ? 6 : 0);

		/*Update Zobrist Hash */
		Position->key ^= RandomNumbers[Position->board[kingSq - 2]][kingSq - 2] ^ RandomNumbers[Position->board[kingSq - 1]][kingSq - 1];

		/*Update bitmaps*/
		Position->pieces[KING][Position->turn] |= kingBit >> 2;
		Position->pieces[ROOK][Position->turn] |= kingBit >> 1;
		/*Update affectedSqs*/
		affectedSqs |= (kingBit >> 2) | (kingBit >> 1);

		Position->empty[Position->turn] |= kingBit >> 2 | kingBit >> 1;

		/*Clear corresponding castling rights*/
		Position->castle &= (Castling_rights[K] | Castling_rights[Q]) << Position->turn;
	}
	else
	{
		switch (NPIECE(Position->board[move->from]))
		{
		case PAWN:
			delta = move->to - move->from;
			delta = ABS(delta);
			if (delta >= 16)
			{ /* move is two step by pawn */
				Position->ep = move->to + (Position->turn ? 8 : -8);
				/*Update Zobrist Hash*/
				Position->key ^= epRandomNumbers[Position->ep];
			}
			else if (move->to == move->ep && move->ep > 0)
			{ /* move is an en passant capture */

				ep_xsquare = EP_XSQ(move->ep, Position->turn);

				/*Update Zobrist Hash*/

				Position->key ^= RandomNumbers[Position->board[ep_xsquare]][ep_xsquare];
				Position->board[ep_xsquare] = EMPTY;
				ep_xsquareBit = BIT(ep_xsquare);
				Position->pieces[PAWN][Position->turn ^ 1] &= ~ep_xsquareBit;

				Position->empty[Position->turn ^ 1] &= ~ep_xsquareBit;
				/*Update affectedSqs*/
				affectedSqs |= ep_xsquareBit;
			}
			else if (move->promote > 0)
			{ /* move is promotion */

				/*Update Zobrist Hash*/
				Position->key ^= RandomNumbers[Position->board[move->from]][move->from] ^ RandomNumbers[move->promote][move->from];

				Position->board[move->from] = move->promote;
				Position->pieces[PAWN][Position->turn] &= ~fromBit;
			}
			break;
		case KING:
			/*take away castling rights*/
			Position->castle &= (Castling_rights[K] | Castling_rights[Q]) << Position->turn;
			break;
		case ROOK:
			/*find out from wich side is the rook that moved and take away those castling rights*/
			Position->castle &= ~(Castling_rights[WICHROOK(move->from)] >> Position->turn);
			break;
		}

		/*Update affectedSqs*/
		affectedSqs |= fromBit | toBit;
		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[move->from]][move->from] ^ RandomNumbers[Position->board[move->to]][move->to] ^ RandomNumbers[Position->board[move->from]][move->to];

		/*Update bitmaps*/
		Position->pieces[NPIECE(Position->board[move->from])][Position->turn] &= ~fromBit;
		Position->pieces[NPIECE(Position->board[move->to])][Position->turn ^ 1] &= ~toBit;
		Position->pieces[NPIECE(Position->board[move->from])][Position->turn] |= toBit;

		Position->empty[Position->turn ^ 1] &= ~toBit;
		Position->empty[Position->turn] &= ~fromBit;
		Position->empty[Position->turn] |= toBit;
		/* perform the exchange */
		move->saved = Position->board[move->to];
		Position->board[move->to] = Position->board[move->from];
		Position->board[move->from] = EMPTY;
	}
	Position->key ^= castleRandomNumbers[Position->castle];
	Position->turn ^= 1;
	/* Map attacked*/
	fastMap(Position, affectedSqs);
}

int UnMakeMove(Move *move, Boardmap *Position)
{
	Boardmap TMP;

	char ep_xsquare, kingSq, rookSq;

	uint64 kingBit, rookBit, ep_xsquareBit, fromBit, toBit, affectedSqs = 0;

	fromBit = BIT(move->from);
	toBit = BIT(move->to);
	char delete = Position->board[move->to];

	/*Update Zobrsit Hash*/

	Position->key ^= epRandomNumbers[Position->ep] ^ epRandomNumbers[move->ep] ^ castleRandomNumbers[Position->castle] ^ castleRandomNumbers[move->savedcastle];

	Position->turn ^= 1;				  /* First set the turn back to the previous player */
	Position->ep = move->ep;			  /* return previous ep square */
	Position->castle = move->savedcastle; /* return previous castling rights */

	if (move->castle & SHORTCASTLE)
	{
		kingSq = Position->turn ? 60 : 4;
		rookSq = Position->turn ? 63 : 7;

		kingBit = BIT(kingSq);
		rookBit = BIT(rookSq);

		/*Update affectedSqs*/
		affectedSqs |= kingBit | rookBit;
		/*Clear current castle squares*/
		Position->board[kingSq] = KING + (Position->turn ? 6 : 0);
		Position->board[rookSq] = ROOK + (Position->turn ? 6 : 0);
		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[kingSq]][kingSq] ^ RandomNumbers[Position->board[rookSq]][rookSq];

		/*Update bitmaps*/
		Position->pieces[KING][Position->turn] |= kingBit;
		Position->pieces[ROOK][Position->turn] |= rookBit;

		Position->empty[Position->turn] |= kingBit | rookBit;

		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[kingSq + 2]][kingSq + 2] ^ RandomNumbers[Position->board[kingSq + 1]][kingSq + 1];

		/*Set castled king and rook*/
		Position->board[kingSq + 2] = EMPTY;
		Position->board[kingSq + 1] = EMPTY;

		/*Update Bitmaps*/
		Position->pieces[KING][Position->turn] ^= kingBit << 2;
		Position->pieces[ROOK][Position->turn] ^= kingBit << 1;

		/*Update affectedSqs*/

		affectedSqs |= (kingBit << 2) | (kingBit << 1);

		Position->empty[Position->turn] ^= kingBit << 2 | kingBit << 1;
	}
	else if (move->castle & LONGCASTLE)
	{
		kingSq = Position->turn ? 60 : 4;
		rookSq = Position->turn ? 56 : 0;

		kingBit = BIT(kingSq);
		rookBit = BIT(rookSq);

		/*Update affectedSqs*/
		affectedSqs |= kingBit | rookBit;

		/*Clear current castle squares*/
		Position->board[kingSq] = KING + (Position->turn ? 6 : 0);
		Position->board[rookSq] = ROOK + (Position->turn ? 6 : 0);

		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[kingSq]][kingSq] ^ RandomNumbers[Position->board[rookSq]][rookSq];

		/*Update bitmaps*/
		Position->pieces[KING][Position->turn] |= kingBit;
		Position->pieces[ROOK][Position->turn] |= rookBit;

		Position->empty[Position->turn] |= kingBit | rookBit;

		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[kingSq - 2]][kingSq - 2] ^ RandomNumbers[Position->board[kingSq - 1]][kingSq - 1];

		/*Set castled king and rook*/
		Position->board[kingSq - 2] = EMPTY;
		Position->board[kingSq - 1] = EMPTY;

		/*Update bitmaps*/
		Position->pieces[KING][Position->turn] ^= kingBit >> 2;
		Position->pieces[ROOK][Position->turn] ^= kingBit >> 1;
		/*Update affectedSqs*/
		affectedSqs |= (kingBit >> 2) | (kingBit >> 1);

		Position->empty[Position->turn] ^= kingBit >> 2 | kingBit >> 1;
	}
	else
	{
		if (NPIECE(Position->board[move->to]) == PAWN)
		{
			/*This if moving piece is pawn*/
			if (move->to == Position->ep && Position->ep > 0)
			{
				ep_xsquare = EP_XSQ(Position->ep, Position->turn);
				Position->board[ep_xsquare] = PAWN + (Position->turn ? 0 : 6);
				ep_xsquareBit = BIT(ep_xsquare);
				/*Update affectedSqs*/
				affectedSqs |= ep_xsquareBit;
				/*Update Zobrist Hash*/
				Position->key ^= RandomNumbers[Position->board[ep_xsquare]][ep_xsquare];

				/*Update bitmaps*/
				Position->pieces[PAWN][Position->turn ^ 1] |= ep_xsquareBit;

				Position->empty[Position->turn ^ 1] |= ep_xsquareBit;
			}
		}
		else if (move->promote > 0)
		{
			Position->pieces[NPIECE(Position->board[move->to])][Position->turn] &= ~(BIT(move->to));
			Position->pieces[PAWN][Position->turn] |= BIT(move->to);
			/* un-promote to pawn again */
			Position->board[move->to] = PAWN + (Position->turn ? 6 : 0);
			/*Update Zobrist Hash*/
			Position->key ^= RandomNumbers[move->promote][move->to] ^ RandomNumbers[Position->board[move->to]][move->to];
		}

		/*Update Zobrist Hash*/
		Position->key ^= RandomNumbers[Position->board[move->to]][move->to] ^ RandomNumbers[Position->board[move->to]][move->from];
		if (move->saved != EMPTY)
			Position->key ^= RandomNumbers[move->saved][move->to];
		/*Update bitmaps*/
		Position->pieces[NPIECE(Position->board[move->to])][Position->turn] &= ~toBit;
		if (move->saved != EMPTY)
			Position->pieces[NPIECE(move->saved)][Position->turn ^ 1] |= toBit;
		Position->pieces[NPIECE(Position->board[move->to])][Position->turn] |= fromBit;
		/*Update affectedSqs*/
		affectedSqs |= fromBit | toBit;

		if (move->saved != EMPTY)
			Position->empty[Position->turn ^ 1] |= toBit;
		Position->empty[Position->turn] &= ~toBit;
		Position->empty[Position->turn] |= fromBit;
		/*Actual piece restore*/
		Position->board[move->from] = Position->board[move->to]; /*restore piece to original sq*/
		Position->board[move->to] = move->saved;				 /*return whatever was on the destination square*/
	}
	/* Map */
	fastMap(Position, affectedSqs);
}

int MoveCmp(Move *a, Move *b)
{
	if (a->castle == 0 && b->castle == 0)
	{
		if (a->from == b->from && a->to == b->to && a->promote == b->promote)
			return 0;
	}
	else if (a->castle == b->castle)
		return 0;
	return 1;
}

int SortMoves(int nmoves, Move moves[], Boardmap *Position, int ply)
{
	int scores[MAXMOVES] = {0};
	int index = 0;
	
	Move tmp, PvMove;
	memcpy(&PvMove, &pv[SortPly].moves[0], sizeof(Move));
	
	int kingsq, npiece;
	kingsq = BIT(Position->pieces[KING][Position->turn]); /* asume there is only one king right ? ...*/
	
	for (int i = 0; i < nmoves; ++i)
	{
		npiece = NPIECE(Position->board[moves[i].to]);

		if (thinking && MoveCmp(&moves[i], &PvMove) == 0)
		{
			memcpy(&tmp, &moves[0], sizeof(Move));
			memcpy(&moves[0], &moves[i], sizeof(Move));
			memcpy(&moves[i], &tmp, sizeof(Move));
			if (index == 0)
				index = 1;
		}
		else if (Position->board[moves[i].to] != EMPTY)
		{
			scores[i] += 20;
			memcpy(&tmp, &moves[i], sizeof(Move));
			memcpy(&moves[i], &moves[index], sizeof(Move));
			memcpy(&moves[index], &tmp, sizeof(Move));
			++index;
		}
		else if ((npiece == KNIGHT && (KnightSqs[moves[i].to] & Position->pieces[KING][Position->turn ^ 1])) || AllRays[moves[i].to] & Position->pieces[KING][Position->turn ^ 1])
		{ // will be check
			memcpy(&tmp, &moves[i], sizeof(Move));
			memcpy(&moves[i], &moves[index], sizeof(Move));
			memcpy(&moves[index], &tmp, sizeof(Move));
			++index;
		}
	}
	/* Implement some form of sorting */
}

uint64 GetMoveAttacks(Boardmap *Position, Move *move)
{
	/* before making a move see which squares are going to be attacked by that move */
	int npiece, turn, to;
	to = move->to;
	turn = Position->turn;
	npiece = NPIECE(Position->board[move->from]);
	uint64 Attacks;
	
	switch (npiece)
	{
	case PAWN:
		Attacks = PwnSqs[turn][to];
		break;
	case KNIGHT:
		Attacks = KnightSqs[to];
		break;
	case BISHOP:
		Attacks = AllBishopSqs[to];
		break;
	case ROOK:
		Attacks = AllRookSqs[to];
		break;
	case QUEEN:
		Attacks = AllRays[to];
		break;
	case KING:
		Attacks = KingSqs[to];
		break;
	}
	return Attacks;
}

int MoveIsCheck(Move *move, Boardmap *Position)
{
	int npiece, turn, ret;
	ret = 0;
	turn = Position->turn;
	npiece = NPIECE(Position->board[move->from]);
	uint64 oking = Position->pieces[KING][turn ^ 1];
	uint64 Attacks, allpieces, toBit;
	allpieces = Position->empty[turn] | Position->empty[turn ^ 1];
	Attacks = 0;
	toBit = BIT(move->to);
	switch (npiece)
	{
	case PAWN:
		Attacks = PwnSqs[turn][move->to];
		break;
	case KNIGHT:
		Attacks = KnightSqs[move->to];
		break;
	case BISHOP:
		Attacks = GetBishopRay(toBit, oking);
		break;
	case ROOK:
		Attacks = GetRookRay(toBit, oking);
		break;
	case QUEEN:
		Attacks = GetRookRay(toBit, oking);
		if (!Attacks)
			Attacks = GetBishopRay(toBit, oking);
		break;
	}
	if ((Attacks & allpieces) == oking)
		return 1;
	else
		return 0;
}

int SortMs(int nmoves, Move moves[], Boardmap *Position, int ply)
{
	if (nmoves <= 1)
		return 0;

	int list[MAXMOVES];
	int see;
	int ksq, oksq, turn, npiece;

	Move pvMove;
	uint64 bitto;
	turn = Position->turn;
	LOG2(Position->pieces[KING][turn], ksq);
	LOG2(Position->pieces[KING][turn ^ 1], oksq);
	memcpy(&pvMove, &pv[ply].moves[0], sizeof(Move));

	for (int i = 0; i < nmoves; ++i)
	{
		npiece = NPIECE(Position->board[moves[i].from]);
		see = SEE(&moves[i], Position);
		bitto = BIT(moves[i].to);
		list[i] = 1000 + see;
		if (Position->board[moves[i].to] != EMPTY)
		{
			if (see > 0)
				list[i] += 10000000;
			else if (see > -200)
				list[i] += 1000000;
			else
				list[i] += 500000;
		}

		if (thinking && MoveCmp(&moves[i], &pvMove) == 0)
		{
			list[i] += 40000000;
		}
		else
		{
			if (MoveIsCheck(&moves[i], Position))
				list[i] += 22000000;
			else
			{
				if (GetMoveAttacks(Position, &moves[i]) & KingSqs[oksq])
					list[i] += see > -200 ? 30000 : 10000;
				if (PieceSqValueT[npiece][moves[i].to] > PieceSqValueT[npiece][moves[i].from])
					list[i] += 100;
				if (moves[i].promote != 0)
					list[i] += see > 0 ? 7000000 : 7000;
				else
				{
					if (bitto & CenterSqs && see > 0)
						list[i] += 500;
				}
			}
		}
		if (moves[i].castle == 0)
			list[i] += Mhistory[moves[i].from][moves[i].to];
	}

	int max, maxi, tmpp;
	Move tmp;

	for (int j = 0; j < nmoves; ++j)
	{
		max = -INFINITY;
		for (int k = j; k < nmoves; ++k)
		{
			if (list[k] > max)
				max = list[k], maxi = k;
		}
		if (max > -INFINITY)
		{
			tmpp = list[maxi];
			list[maxi] = list[j];
			list[j] = tmpp;
			memcpy(&tmp, &moves[maxi], sizeof(Move));
			memcpy(&moves[maxi], &moves[j], sizeof(Move));
			memcpy(&moves[j], &tmp, sizeof(Move));
		}
	}
}

int NextMove(int *list, int len)
{
	int max, n;
	max = n = -INFINITY;

	for (int i = 0; i < len; i++)
	{
		if (list[i] > max)
		{
			max = list[i];
			n = i;
		}
	}

	if (n > -1)
		list[n] = -1;
	return n;
}

int ValidateMove(Move *move, Move moves[], Boardmap *Position, int nmoves)
{
	int ret = -1;

	if (Position->board[move->from] == EMPTY)
		return -1;

	int i = 0;

	for (i = 0; i < nmoves; ++i)
	{
		if (moves[i].castle == move->castle || (moves[i].from == move->from && moves[i].to == move->to))
		{
			if (move->promote != 0 && move->promote == moves[i].promote || move->promote == 0)
				break;
		}
	}

	if (i < nmoves)
	{
		MakeMove(move, Position);
		if (!CHECK(Position))
			ret = 0;
		UnMakeMove(move, Position);
	}
	return ret;
}