#include "common.h"
#include "debug.h"
#include "gen.h"
#include "move.h"
#include "notation.h"
#include "book.h"
#include "hash.h"
#include "tt.h"
#include "eval.h"
#include "search.h"
#include "tm.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)

#else
	#include <pthread.h>
#endif

#include <signal.h>

int verbose;
uint64 higligth;
_perftResults results[MAX_DEPTH];
char moveOutput[10];
char fen[50];
StopWatch stopWatch;

int _perft(int ply, _perftArgs *args, _perftResults results[])
{
	int _;
	int count = 0, nmoves, turn, ksq;
	Move moves[MAXMOVES];
	turn = args->Position->turn;
	LOG2(args->Position->pieces[KING][turn], ksq);
	Boardmap TMP;

	memcpy(&TMP, args->Position, sizeof(Boardmap));

	if (INCHECK(args->Position))
	{
		nmoves = GenCheckEscapes(args->Position, moves);
	}
	else
		nmoves = args->genfn(args->Position, moves);

	#ifdef ONLYLEGALGEN
	
	if (ply <= 1)
	{
		uint64 allpieces = args->Position->empty[turn] | args->Position->empty[turn],
			   KingSq = args->Position->pieces[KING][turn];
		results[current_depth].nodes += nmoves;

		for (int i = 0; i < nmoves; ++i)
		{
			_ = 0;
			args->makefn(&moves[i], args->Position);
			if (CHECK(args->Position))
			{
				_ = 1;
				printf("ALERT!\n\n\n\n");
			}
			args->unmakefn(&moves[i], args->Position);
			if (_)
			{
				printBoardinfo(args->Position);
				printM(&moves[i], args->Position);
				GetFENof(args->Position, fen);
				printf("\n%s", fen);
				getchar();
			}
		}
		return nmoves;
	}

	for (int i = 0; i < nmoves; ++i)
	{
		args->makefn(&moves[i], args->Position);
		_perft(ply - 1, args, results);
		args->unmakefn(&moves[i], args->Position);
	}
	return nmoves;

	#else
	
	if (ply <= 0)
		return 1;

	for (int i = 0; i < nmoves; ++i)
	{
		args->makefn(&moves[i], args->Position);
		int check;
		check = INCHECK(args->Position);
		if (!check || !CHECK(args->Position))
		{
			results[ply].nodes++;
			if (args->Position->pieces[KING][args->Position->turn] & args->Position->attackd[args->Position->turn ^ 1])
			{
				results[ply].checks++;
			}

			if (moves[i].castle)
				results[ply].castles++;
			else
			{
				if (moves[i].saved != EMPTY)
					results[ply].captures++;
				if (moves[i].promote > 0)
					results[ply].promotions++;
				if (NPIECE(args->Position->board[moves[i].to]) == PAWN)
				{
					if (moves[i].ep == moves[i].to)
						results[ply].ep++, results[ply].captures++;
				}
			}
			_perft(ply - 1, args, results);
		}
		else
		{
			higligth = 0;
		}
		args->unmakefn(&moves[i], args->Position);
	}

	return results[ply].nodes;

	#endif
}

int fakeperft(int testcases)
{
	Boardmap Position;
	Move moves[MAXMOVES];

	double
		genfinal = 0,
		capturefinal = 0,
		mapfinal = 0,
		makefinal = 0,
		evalfinal = 0,
		engine_cicle = 0,
		quiescencefinal = 0,
		seefinal = 0;

	memset(moves, 0, sizeof(Move) * MAXMOVES);
	SetfromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", &Position);
	printBoardinfo(&Position);
	int nmoves = 0;
	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		nmoves = GenMoves(&Position, moves);
	}
	Stop(&stopWatch);
	printf("\n--> Generated %d moves in %f seconds.\n", nmoves * testcases, stopWatch.elapsed);

	genfinal = (nmoves * testcases) / stopWatch.elapsed;
	printf("==> Generates ~ %.2f moves per second", genfinal);

	for (int i = 0; i < nmoves; i++)
	{
		printf("\n%d. ", i + 1);
		printmove(&moves[i], &Position, _SHORT);
	}

	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		nmoves = GenCaptures(&Position, moves);
	}
	Stop(&stopWatch);
	printf("\n--> Generated %d captures in %f seconds.\n", nmoves * testcases, stopWatch.elapsed);

	capturefinal = (nmoves * testcases) / stopWatch.elapsed;
	printf("==> Generates ~ %.2f captures per second", capturefinal);

	for (int i = 0; i < nmoves; i++)
	{
		printf("\n%d. ", i + 1);
		printmove(&moves[i], &Position, _SHORT);
	}

	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		fastMap(&Position, 0x000f);
	}
	Stop(&stopWatch);
	printf("\n--> fastMap'd %d positions in %f seconds.\n", testcases, stopWatch.elapsed);
	mapfinal = (testcases) / stopWatch.elapsed;
	printf("==> fastMaps ~ %.2f Positions per second", mapfinal);

	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		for (int j = 0; j < nmoves; ++j)
		{
			MakeMove(&(moves[j]), &Position);
			UnMakeMove(&(moves[j]), &Position);
		}
	}
	Stop(&stopWatch);

	printf("\n--> maked and unmaked %d moves in %f seconds.\n", nmoves * testcases, stopWatch.elapsed);
	makefinal = (nmoves * testcases) / stopWatch.elapsed;
	printf("==> makes and un-makes %.2f moves per second", makefinal);
	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		Eval(&Position);
	}
	Stop(&stopWatch);
	printf("\n--> evaluated %d positions in %f seconds.\n", testcases, stopWatch.elapsed);
	evalfinal = testcases / stopWatch.elapsed;
	printf("==> Evaluates ~ %.2f Positions per second", evalfinal);

	Move m;

	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		for (int sq = 0; sq < 64; ++sq)
		{
			m.to = sq;
			SEE(&m, &Position);
		}
	}
	Stop(&stopWatch);
	printf("\n--> see()'d %d positions in %f seconds.\n", testcases * 64, stopWatch.elapsed);
	seefinal = (testcases * 64) / stopWatch.elapsed;
	printf("==> see()'s ~ %.2f Positions per second", seefinal);

	Start(&stopWatch);
	for (int i = 0; i < testcases; ++i)
	{
		Quiescence(20, &Position, -INFINITY, INFINITY);
	}
	Stop(&stopWatch);
	printf("\n--> Quiescence'd %d times in %f seconds.\n", testcases, stopWatch.elapsed);

	quiescencefinal = testcases / stopWatch.elapsed;
	printf("==> Quiescences ~ %.2f times per second", quiescencefinal);

	engine_cicle = (evalfinal + makefinal + ((genfinal))) / 3;

	printf("\n\nOverall performance pronostic : ~ %.2f nodes per second\n", engine_cicle);
}

int printmoves(Boardmap *Bmap, int sq)
{
	printf("\u2654 ");
#if defined(_WIN32) || defined(_WIN64)
	char color[][2] = {"", "", "", ""};
	int nux = 0;
#else
	char color[][10] = {"\e[30m", "\e[1m\e[37m", "\e[43m", "\e[42m"};
	int nux = 1;
#endif
	unsigned long long sqc = 1;
	unsigned long long bleh = 1;
	for (long long i = 0; i < 64; ++i, sqc ^= 0x1)
	{
#if defined(_WIN32) || defined(_WIN64)
#else
		printf("\e[0m");
#endif
		if (i % 8 == 0)
			sqc ^= 1;
		printf("%s%c%c%s%s%s ", 
			i % 8 ? "" : "\n ", 
			i % 8 ? '\0' : (char)('8' - (i / 8)), i % 8 ? '\0' : ' ', 
			color[sqc + 2], color[COLOR(Bmap->board[i])],
			Bmap->moves[sq] & 0 ? "*" : (Bmap->board[i] == EMPTY) && nux ? higligth & (BIT(i)) ? "*" : " " : wPieces[Bmap->board[i]]);
	}

#if defined(_WIN32) || defined(_WIN64)
	printf("\n   a   b   c   d   e   f   g   h\n");
#else
	printf("\e[0m");
	printf("\n   a b c d e f g h\n");
#endif
}

void printULL(unsigned long long x)
{
	for (unsigned long long i = 0; i < 64; ++i)
	{
		printf("%c%c%s", 
			i % 4 ? '\0' : ' ', 
			x & (one << i) ? '1' : '0', 
			(i + 1) % 32 == 0 ? "\n" : ""
		);
	}
}

void printBitmap(uint64 bitmap, int sq)
{
	printf("\n");
	for (uint64 bit = 0x1, c = 0; bit; bit <<= 1, ++c)
	{
		printf("%c%c", c % 8 ? ' ' : '\n', c == sq ? 'x' : bitmap & bit ? '*' : '.');
	}
}

void printAttackedSquares(Boardmap *Bmap, char side)
{
	for (uint64 i = 0; i < 64; ++i)
	{
		printf("%s%c ", i % 8 ? "" : "\n",
			   Bmap->attackd[side] & (one << i) ? '*' : Pieces[Bmap->board[i]]);
	}
}

void printmove(Move *move, Boardmap *Position, int n)
{
	char san[10];

	if (move->castle)
	{
		if (move->castle & SHORTCASTLE)
		{
			moveOutput[0] = 'O';
			moveOutput[1] = '-';
			moveOutput[2] = 'O';
			moveOutput[3] = '\0';
		}
		else
		{
			moveOutput[0] = 'O';
			moveOutput[1] = '-';
			moveOutput[2] = 'O';
			moveOutput[3] = '-';
			moveOutput[4] = 'O';
			moveOutput[5] = '\0';
		}
	}
	else
	{
		moveOutput[0] = COLUMNCHAR(move->from);
		moveOutput[1] = ROWCHAR(move->from);
		moveOutput[2] = COLUMNCHAR(move->to);
		moveOutput[3] = ROWCHAR(move->to);
		moveOutput[4] = move->promote > 0 ? Pieces[move->promote] : '\0';
		moveOutput[5] = '\0';
	}
	if (move->castle)
	{
		if (move->castle & SHORTCASTLE)
			printf("%s\n", Position->turn == WHITE ? "move e1g1" : "move e8g8");

		else if (move->castle & LONGCASTLE)
			printf("%s\n", Position->turn == WHITE ? "move e1c1" : "move e8c8");
	}
	else if (n == _SHORT)
	{
		printf("move %c%c%c%c%c\n",
			   COLUMNCHAR(move->from),
			   ROWCHAR(move->from),
			   COLUMNCHAR(move->to),
			   ROWCHAR(move->to),
			   move->promote > 0 ? Pieces[move->promote] : '\0');
	}
	else if (n == SAN && Position != NULL)
	{
		SANmove(move, Position, san);
		printf("%s", san);
	}
	else if (n == 666)
	{
		if (move->castle)
		{
			if (move->castle & SHORTCASTLE)
			{
				printf("%s\n", Position->turn == WHITE ? "e1g1" : "e8g8");
			}
			else if (move->castle & LONGCASTLE)
			{
				printf("%s\n", Position->turn == WHITE ? "e1c1" : "e8c8");
			}
		}
		else
		{
			printf("%c%c%c%c%c\n",
				   COLUMNCHAR(move->from),
				   ROWCHAR(move->from),
				   COLUMNCHAR(move->to),
				   ROWCHAR(move->to),
				   move->promote > 0 ? (Pieces[move->promote] ^ 0x20) : '\0');
		}
	}
	else;
	// TODO: Implement Descriptive Notation
}

void printBoardinfo(Boardmap *Bmap)
{
	if (verbose)
	{
		printf("\nside to move: %s\n", Bmap->turn ? "white" : "black");
		printf("enpassant square: %s\n", algebraic[Bmap->ep]);
		printf("castle rights: %x\n", Bmap->castle);
		printf("Position hash: %x\n", Bmap->key);
	}
	printmoves(Bmap, (CHARSQINT('h', '2')));
}

int printM(Move *move, Boardmap *Position)
{
	if (move->castle)
	{
		if (move->castle & SHORTCASTLE)
		{
			printf("%s", Position->turn == WHITE ? "e1g1" : "e8g8");
		}
		else if (move->castle & LONGCASTLE)
		{
			printf("%s", Position->turn == WHITE ? "e1c1" : "e8c8");
		}
	}
	else
	{
		printf("%c%c%c%c%c",
			   COLUMNCHAR(move->from),
			   ROWCHAR(move->from),
			   COLUMNCHAR(move->to),
			   ROWCHAR(move->to),
			   move->promote > 0 ? (Pieces[move->promote]) : '\0');
	}
}

int printPV(Line *pv, Boardmap *Position)
{
	Boardmap TMP;
	memcpy(&TMP, Position, sizeof(Boardmap));
	for (int i = 0; i < pv->length; ++i)
	{
		printM(&pv->moves[i], &TMP);
		if (MoveIsCheck(&pv->moves[i], &TMP))
			printf("+");
		printf(" ");
		MakeMove(&pv->moves[i], &TMP);
	}
}

void debugFramework()
{
	int ntokens = 0;
	exitXboard = 1;
	int no_promt = 0;

	int Nodes[MAX_DEPTH];

	char commands[255];
	char tokens[5][MAXFENLEN];

	FILE *inputFile = stdin;

	Boardmap Position;
	Move moves[MAXMOVES];
	int nmoves;

	Move moveHistory[100];
	int nhistory = 0;

	char fen[MAXFENLEN];

	/*mapping functions*/

	int (*mapfn[2])(Boardmap *);

	mapfn[0] = Map;
	mapfn[1] = Map;

	/*make and unmake functions ptrs*/

	int (*makefn[2])(Move *, Boardmap *);
	int (*unmakefn[2])(Move *, Boardmap *);

	makefn[0] = MakeMove;
	unmakefn[0] = UnMakeMove;

	makefn[1] = MakeMove;
	unmakefn[1] = UnMakeMove;

	while (1)
	{
		printf("\n%s", no_promt ? "" : ">");

		if (fgets(commands, 254, inputFile) == NULL)
		{
			if (inputFile != stdin)
			{
				fclose(inputFile);
				inputFile = stdin;
			}
			else
				break;
		}
		if (strcmp(commands, "\n") == 0 || strcmp(commands, "") == 0)
		{
			if (tokens[0][0] == 0)
				continue;
			else
				goto doit;
		}

		ntokens = 0;
		int token_char;
		for (int i = 0; commands[i] && i < 255; ++i)
		{
			if ((commands[i] == ' ' || commands[i] == '\t' || commands[i] == '\n'))
				continue;

			for (token_char = 0; !(commands[i] == ' ' || commands[i] == '\t' || commands[i] == '\n') && i < MAXFENLEN; ++token_char)
				tokens[ntokens][token_char] = commands[i++];

			tokens[ntokens][token_char] = 0;
			if (++ntokens >= 5)
				break;
		}

	doit:
		if (tokens[0][0] == '#')
			continue;
		if (strcmp(tokens[0], "exit") == 0 || strcmp(tokens[0], "bye") == 0 || strcmp(tokens[0], "quit") == 0)
		{
			exit(0);
		}
		else if (strcmp(tokens[0], "continue") == 0)
		{
			break;
		}
		else if (strcmp(tokens[0], "verbose") == 0)
		{
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "perft") == 0)
		{
			/*initiates a thread running perft to a given depth*/

			int depth;

			depth = ntokens > 1 ? atoi(tokens[1]) : 1;
			current_depth = depth;
			_perftArgs args = {
				&Position,
				makefn[1],
				unmakefn[1],
				GenMoves,
				GenCaptures};

			memset(results, 0, sizeof(_perftResults) * MAX_DEPTH);

			printf("\n => Calculating perft(%d) ...\n", depth);
			Start(&stopWatch);
			_perft(depth, &args, results);
			Stop(&stopWatch);
#ifndef ONLYLEGALGEN
			for (int i = depth; i > 0; --i)
			{
				printf("\n\n => perft(%d):\n", (current_depth - i) + 1);
				printf("\tnodes: %d\n\tcaptures: %d\n\te.p: %d\n\tcastles: %d\n\tpromotions: %d\n\tchecks: %d\n",
					   results[i].nodes, results[i].captures, results[i].ep, results[i].castles, results[i].promotions, results[i].checks);
			}
			printf(" Completed in : %f\n", stopWatch.elapsed);

#else

			printf("\n\n => perft(%d):\n", current_depth);
			printf("\tnodes: %d\n\tcaptures: %d\n\te.p: %d\n\tcastles: %d\n\tpromotions: %d\n\tchecks: %d\n",
				   results[current_depth].nodes, results[current_depth].captures, results[current_depth].ep, results[current_depth].castles, results[current_depth].promotions, results[current_depth].checks);
			printf(" Completed in : %f\n", stopWatch.elapsed);
#endif
		}
		else if (strcmp(tokens[0], "stop-perft") == 0)
		{
			/*stops the current perft count and gives the results*/
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "clear") == 0)
		{
			printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		}
		else if (strcmp(tokens[0], "reset") == 0)
		{ /* reset board position */
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "show") == 0)
		{
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "boardinfo") == 0 || strcmp(tokens[0], "/") == 0)
		{
			printBoardinfo(&Position);
		}
		else if (strcmp(tokens[0], "+escape") == 0)
		{
			nmoves = GenCheckEscapes(&Position, moves);
			SortMs(nmoves, moves, &Position, 0);
			for (int i = 0; i < nmoves; ++i)
			{
				printf(" %d) ,", i);
				printM(&moves[i], &Position);
			}
		}
		else if (strcmp(tokens[0], "order") == 0)
		{
			nmoves = GenMoves(&Position, moves);
			SortMs(nmoves, moves, &Position, 0);
			for (int i = 0; i < nmoves; ++i)
			{
				printf(" %d) ,", i);
				printM(&moves[i], &Position);
			}
		}
		else if (strcmp(tokens[0], "moves") == 0 || strcmp(tokens[0], ",") == 0)
		{
			if (ntokens > 1)
			{
				int sq;
				sq = atoi(tokens[1]);
				printBitmap(Position.moves[sq], sq);
			}
			else
			{
#ifdef ONLYLEGALGEN
				printf("\ncompiled with only legal moves generation\n\n\n");

				if (INCHECK(&Position))
					nmoves = GenCheckEscapes(&Position, moves);
				else
#endif
					nmoves = GenMoves(&Position, moves);

				for (int i = 0; i < nmoves; ++i)
				{
					printf(" %d) ", i);
					printM(&moves[i], &Position);
				}
			}
		}
		else if (strcmp(tokens[0], "captures") == 0 || strcmp(tokens[0], "<") == 0)
		{
			nmoves = GenCaptures(&Position, moves);
			for (int i = 0; i < nmoves; ++i)
			{
				printf("\n%d) ", i);
				printmove(&moves[i], &Position, _SHORT);
			}
		}
		else if (strcmp(tokens[0], "eval") == 0)
		{
			printf("\n => %d", Eval(&Position));
		}
		else if (strcmp(tokens[0], "see") == 0)
		{
			int see_score, sq;

			Move m[MAXMOVES];

			if (ntokens > 1)
			{
				if (tokens[1][0] > 96 && tokens[1][0] < 123 && tokens[1][1] != 0)
				{
					sq = CHARSQINT(tokens[1][0], tokens[1][1]);
				}
				else
				{
					sq = atoi(tokens[1]);
					sq = sq > 63 ? 64 : sq < 0 ? 0 : sq;
				}

				int ncaptures = GenMoves(&Position, m);

				for (int n = 0; n < ncaptures; ++n)
				{
					see_score = SEE(&m[n], &Position);
					printBoardinfo(&Position);
					printf("\n => SEE score for move: ");
					printmove(&m[n], &Position, _SHORT);
					printf(" -> %d\n", see_score);
					getchar();
				}
			}
			else
				printf("\n must enter a sq number\n");
		}
		else if (strcmp(tokens[0], "debug") == 0)
		{
			debug ^= 1;
			if (debug)
				printf("\nDebug is on.\n");
			else
				printf("\nDebug is off.\n");
		}
		else if (strcmp(tokens[0], "hash") == 0 || strcmp(tokens[0], "h") == 0)
		{
			printf("\n => %x ", HashKey(&Position));
		}
		else if (strcmp(tokens[0], "key") == 0)
		{
			printf("\n => %x ", Position.key);
		}
		else if (strcmp(tokens[0], "ls") == 0)
		{
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "attacked") == 0)
		{
			printAttackedSquares(&Position, Position.turn);
		}
		else if (strcmp(tokens[0], "fen") == 0)
		{
			GetFENof(&Position, fen);
			printf("\n%s", fen);
		}
		else if (strcmp(tokens[0], "set") == 0 || strcmp(tokens[0], "=") == 0)
		{
			/*if has arguments tries to use the argument as a fen position else sets the stating pos*/
			memset(Nodes, 0, sizeof(int) * MAX_DEPTH);
			if (ntokens > 1)
			{
				nhistory = 0;
				nmoves = 0;
				int index = 0;
				printf("\n%d\n", ntokens);
				fen[0] = 0;
				for (int t = 1; t < ntokens; ++t)
					strcat(fen, tokens[t]), strcat(fen, " ");
				printf("\n%s", fen);
				getchar();
				SetfromFEN(fen, &Position);
			}
			else
				SetfromFEN(StartPositionFEN, &Position);
		}
		else if (strcmp(tokens[0], "move") == 0)
		{
			int n;
			if (ntokens > 1)
			{
				if (strcmp(tokens[1], "null") == 0)
				{
					memset(Nodes, 0, sizeof(int) * MAX_DEPTH);
					Position.turn ^= 1;
					nhistory = 0;
					nmoves = 0;
					printf("\n made a null move => history deleted.");
				}
				else
				{
					n = atoi(tokens[1]);
					n = n >= nmoves ? nmoves - 1 : n < 0 ? 0 : n;
					makefn[1](&moves[n], &Position);

					memcpy(&moveHistory[nhistory], &moves[n], sizeof(Move));
					nhistory += (nhistory >= 99 ? 0 : 1);
				}
				printf("\n Move made data: \n from: %d\n to: %d\n saved: %d\n savedcastle: 0x%x\n ep: %d\n promote: %d\n castle: 0x%x\n",
					   moves[n].from, moves[n].to, moves[n].saved, moves[n].savedcastle, moves[n].ep, moves[n].promote, moves[n].castle);
			}
			else
				printf("\nSelect a move to make. (type `moves` to see available moves).\n");
		}
		else if (strcmp(tokens[0], "undo") == 0 || strcmp(tokens[0], "-") == 0)
		{
			if (nhistory > 0)
			{
				nhistory--;
				unmakefn[1](&moveHistory[nhistory], &Position);
			}
			else
				printf("\n => no moves have been made.\n");
		}
		else if (strcmp(tokens[0], "allsqmoves") == 0)
		{

			int nbitmap;
			if (ntokens > 1)
			{
				nbitmap = atoi(tokens[1]);
			}
			else
				nbitmap = 0;
			printf("nbitmap: %d, ntokens: %d", nbitmap, ntokens);
			getchar();
			switch (nbitmap)
			{
			case 1:
				printf("\nwhite attacked:");
				printAttackedSquares(&Position, WHITE);
				printBoardinfo(&Position);
				getchar();
				printf("\nblack attacked:");
				printAttackedSquares(&Position, BLACK);
				printBoardinfo(&Position);
				break;
			case 2:
				printf("WHITE");
				printBoardinfo(&Position);
				printBitmap(Position.empty[WHITE], -1);
				getchar();
				printf("\nBLACK");
				printBoardinfo(&Position);
				printBitmap(Position.empty[BLACK], -1);

				break;
			case 3:

				for (int p = 0; p < 6; ++p)
				{
					printf("\nWHITE pieces");
					printBitmap(Position.pieces[p][WHITE], -1);
					printBoardinfo(&Position);
					getchar();
					printf("\nBLACK pieces");
					printBitmap(Position.pieces[p][BLACK], -1);
					printBoardinfo(&Position);
					getchar();
				}
				break;
			case 4:

				for (int p = 0; p < 64; ++p)
				{
					printBoardinfo(&Position);
					printBitmap(Position.attacks[p], p);
					getchar();
				}
				break;
			case 0:
			default:
				for (int i = 0; i < 64; ++i)
				{
					printBoardinfo(&Position);
					printBitmap(Position.moves[i], i);
					getchar();
				}
				break;
			}
		}
		else if (strcmp(tokens[0], "book") == 0)
		{
			ABKBook * abkBook = ReadOpeningBook(tokens[1]);
		}
		else if (strcmp(tokens[0], "file") == 0)
		{
			if (ntokens > 1)
			{
				inputFile = fopen(tokens[1], "r");
				if (inputFile == NULL)
				{
					printf("\n => Could not open file %s.", tokens[1]);
					inputFile = stdin;
				}
			}
			else
				printf("\n => Must supply a file to read.");
		}
		else if (strcmp(tokens[0], "pause") == 0)
		{
			printf("\n--more--");
			getchar();
		}
		else if (strcmp(tokens[0], "no-promt") == 0)
		{
			no_promt ^= 1;
		}
		else if (strcmp(tokens[0], "ttalloc") == 0)
		{
			if (TTable != NULL)
			{
				freeTTable();
				TTable = NULL;
			}
			int sze = TTABLESIZE;
			if (ntokens > 1)
			{
				sscanf(tokens[1], "%d", &sze);
			}
			TTSize = sze;
			InitTTable(TTSize);
			printf("\n => sizeof TTEntry = %d\n", sizeof(TTEntry));
			printf("\n => size of TTable = %d bytes, %d mibs\n", TTSize * sizeof(TTEntry), (TTSize * sizeof(TTEntry)) / 1000000);
			printf("\n => address of last TTable element = %p\n", &TTable[TTSize - 1]);
			printf("\n => address of first TTable element = %p\n", &TTable[0]);
			printf("\n => difference of addresses = %d\n", &TTable[TTSize - 1] - &TTable[0]);
			printf("\nCareful!\n");
		}
		else if (strcmp(tokens[0], "ttfree") == 0)
		{
			if (TTable != NULL)
			{
				freeTTable();
				TTable = NULL;
			}
		}
		else if (strcmp(tokens[0], "quiescence") == 0)
		{

			int what = Eval(&Position);
			printf("\n => Quiescence score: %d\n", Quiescence(MAX_DEPTH - current_depth, &Position, -INFINITY, INFINITY));
		}
		else if (strcmp(tokens[0], "search") == 0 || 
		         strcmp(tokens[0], "play") == 0 || 
				 strcmp(tokens[0], ";") == 0 || 
				 strcmp(tokens[0], "'") == 0)
		{
			double EBF; /*Effective Branching Factor*/

			flags = 0;
			memset(Scores, 0, sizeof(int) * MAX_DEPTH);
			int ply = 3;
			if (ntokens > 1)
			{
				ply = atoi(tokens[1]);
			}
			current_depth = ply;
			GlobalNodeCount = 0;
			memset(Mhistory, 0, sizeof(short) * 64 * 64);
			Start(&stopWatch);
			int score = Search(ply, &Position, -INFINITY - MAX_DEPTH, INFINITY + MAX_DEPTH);
			Stop(&stopWatch);
			Nodes[ply] = GlobalNodeCount;
			EBF = ply > 0 ? Nodes[ply - 1] > 0 ? Nodes[ply] / Nodes[ply - 1] : Nodes[ply] : 0;

			if (strcmp(tokens[0], "play") == 0 || strcmp(tokens[0], "'") == 0)
			{
				if (TMP_BEST.flags != NOMOVE)
				{
					makefn[1](&TMP_BEST, &Position);
					memcpy(&moveHistory[nhistory], &TMP_BEST, sizeof(Move));
					nhistory += (nhistory >= 99 ? 0 : 1);
				}
			}

			printBoardinfo(&Position);
			printf("\n");
			printPV(&pv[current_depth], &Position);

			if (TMP_BEST.flags != NOMOVE)
			{
				printf("\n => move: ");
				printmove(&TMP_BEST, &Position, 666);
			}
			else
				printf("\n => There aren't any legal moves in this position.");
			char matestr;
			matestr = '\0';
			int sc = score, turn;
			turn = Position.turn;
			if (turn == WHITE)
			{
				if (INFINITY - sc < MAX_DEPTH)
				{
					matestr = '#';
					sc = INFINITY - sc;
					sc++;
					sc /= 2;
				}
			}
			else
			{
				if (-INFINITY + sc > -MAX_DEPTH)
				{
					matestr = '#';
					sc = -INFINITY + sc;
					sc *= -1;
					sc++;
					sc /= 2;
				}
			}
			printf("\n => Score: %c%d\n", matestr, sc);
			printf(" => Concluded ply %d search in %.2f. (%d Nodes)", ply, stopWatch.elapsed, Nodes[ply]);
			printf("\n => EBF: %.2f", EBF);
		}
		else if (strcmp(tokens[0], "verbose") == 0 || strcmp(tokens[0], "v") == 0)
		{
			if (verbose)
			{
				verbose = 0;
				printf("\nverbose mode off\n");
			}
			else
			{
				verbose = 1;
				printf("\nverbose mode on\n");
			}
		}
		else if (strcmp(tokens[0], "think") == 0 || strcmp(tokens[0], "\\") == 0)
		{
			char turn;
			double EBF; /*effective branching factor*/
			Game game;
			float TimeToThink;
			TimeToThink = 5;
			memset(Mhistory, 0, sizeof(short) * 64 * 64);
			if (ntokens > 1)
				sscanf(tokens[1], "%f", &TimeToThink);
			memcpy(&game.Position, &Position, sizeof(Boardmap));
			thinking = 1;
			available_time = game.available_time = clock() + (TimeToThink * CLOCKS_PER_SEC);
			game.MaxDepth = MAX_DEPTH;
			double t;
			turn = game.Position.turn;
			flags = 0;
			INIT_MUTEX(StopIterate);
			if (!xboard)
				printf("%sply\tscore\ttime\tnodes\tPV\n", debug ? "alpha\t\tbeta\t\t" : "");
			printf("%s---\t-----\t----\t-----\t--\n\n", debug ? "-----\t\t----\t\t" : "");
			Start(&stopWatch);
			Think(&game);
			WAIT(IterativeDeepening);
			Stop(&stopWatch);
			if (TMP_BEST.flags != NOMOVE)
			{
				makefn[1](&BEST, &Position);
				higligth = 0;
				if (nhistory < 99)
				{
					memcpy(&moveHistory[nhistory], &BEST, sizeof(Move));
					nhistory++;
				}
			}
			printBoardinfo(&Position);
			///*calculate EBF*/
			if (current_depth > 1)
				EBF = GNodes[1];
			else
				EBF = (GNodes[current_depth - 1] + GNodes[current_depth]) / 2;

			if (TMP_BEST.flags != NOMOVE)
			{
				printf("\n move: ");
				printmove(&BEST, &game.Position, 666);
			}
			else
				printf("\n %s", INCHECK(&game.Position) ? turn == BLACK ? "White Mates 1-0." : "Black Mates 0-1." : "Stalemate 1/2-1/2.");
			char matestr;
			matestr = '\0';
			int sc = Scores[current_depth - 1];
			if (sc > (INFINITY - MAX_DEPTH) || sc < (-(INFINITY - MAX_DEPTH)))
			{
				matestr = '#';
				sc = INFINITY - abs(sc);
				sc++;
				sc /= 2;
			}
			printf("\n => Score: %c%d\n", matestr, sc);
			printf(" => Concluded ply %d search in ~ %.2f sec. EBF=%.2f", current_depth - 1, stopWatch.elapsed, EBF);
			printf("\nQuiescent Nodes: %d\n", GlobalQuiescenceNodes);
		}
		else if (strcmp(tokens[0], "help") == 0 || strcmp(tokens[0], "?") == 0)
		{
		}
		else if (strcmp(tokens[0], "epdtest") == 0)
		{
			if (ntokens > 1)
			{
				int ntests = epdTest(tokens[1]);
			}
			else
				printf("\n => Must provide a file name\n");
		}
		else if (strcmp(tokens[0], "match") == 0)
		{
			NOTIMPLEMENTED;
		}
		else if (strcmp(tokens[0], "mirror") == 0)
		{
			char temp;

			if (Position.ep != 0)
			{
				Position.ep = (63 - Position.ep);
			}
			for (int sq = 0; sq < 32; ++sq)
			{
				temp = Position.board[sq];
				Position.board[sq] = Position.board[63 - sq];
				Position.board[63 - sq] = temp;
			}

			for (int sq = 0; sq < 64; ++sq)
				if (Position.board[sq] != EMPTY)
					Position.board[sq] += SIDE(Position.board[sq]) == WHITE ? -6 : +6;
			Position.turn ^= 1;

			temp = Position.castle & 0x5; /*white catling rights*/

			Position.castle ^= temp; /* Just black rights*/

			Position.castle >>= 1;

			Position.castle |= temp << 1;

			mapfn[0](&Position);
		}
		else if (strcmp(tokens[0], "fakeperft") == 0)
		{
			int testcases;
			testcases = 1000000;
			if (ntokens > 1)
				testcases = atoi(tokens[1]);
			printf("\n => Doing a fakeperft with %d testcases", testcases);

			fakeperft(testcases);
		}
		else if (strcmp(tokens[0], "map") == 0)
		{
			int nfn;
			if (ntokens > 1)
			{
				nfn = atoi(tokens[1]);
				nfn > 1 ? nfn = 1 : nfn < 0 ? nfn = 0 : nfn;
			}
			else
				nfn = 0;
			printf("\n => used mapping function : %d", nfn);
			mapfn[nfn](&Position);
		}
		else if (strcmp(tokens[0], "system") == 0 || strcmp(tokens[0], "!") == 0)
		{
			if (ntokens > 1)
			{
				char syscommands[255] = {0};
				memset(syscommands, 0, 255);
				for (int t = 1; t < ntokens; ++t)
					strcat(syscommands, tokens[t]), strcat(syscommands, " ");
				system(syscommands);
			}
			else
				printf("\nSystem => Need a command to execute.\n");
		}
		else if (strcmp(tokens[0], "xboard") == 0)
		{
			xboard = 1;
			Xboard();
		}
		else if (strcmp(tokens[0], "xrays") == 0)
		{

			char piece, sq, msq, ksq;
			char turn = Position.turn,
				 oponent = Position.turn ^ 1;
			uint64 pieces,
				square,
				allpieces,
				allmoves,
				movesq,
				KingSqRays,
				bitspace,
				sliders;

			pieces = Position.empty[Position.turn];
			LOG2(Position.pieces[KING][turn], ksq);

			allpieces = Position.empty[turn] | Position.empty[oponent];
			while (pieces)
			{
				square = NEXTSQ(pieces);
				LOG2(square, sq);
				allmoves = Position.moves[sq];
				KingSqRays = Xray(Position.pieces[KING][turn], square, allpieces);
				printf("\n %s", algebraic[sq]);
				printBoardinfo(&Position);
				printBitmap(KingSqRays, -1);
				printBitmap(KingSqRays ^ 63, -1);
				getchar();
				LSBCLEAR(pieces);
			}
		}
		else if (strcmp(tokens[0], "^") == 0)
		{
			int sq, p;
			char btmp[64];
			if (ntokens > 1)
			{
				sq = CHARSQINT(tokens[1][0], tokens[1][1]);
				if (sq > 63 || sq < 0)
					printf("\n\t(!) Invalid square. \n");
				else
				{
					if (ntokens > 2)
						Position.board[sq] = GetpieceN(tokens[2][0]);
					else
						Position.board[sq] = EMPTY;

					mapfn[0](&Position);
				}
			}

			else
				printf("\n\t Must enter a square\n");
		}
		else
		{
			Move m;
			nmoves = GenMoves(&Position, moves);
			if (Parsemove(tokens[0], &m, &Position) || ValidateMove(&m, moves, &Position, nmoves))
			{
				printf("\nInvalid command.");
			}
			else
			{
				MakeMove(&m, &Position);
				memcpy(&moveHistory[nhistory], &m, sizeof(Move));
				nhistory += (nhistory >= 99 ? 0 : 1);
			}
		}
	}
}

void debugSignals(int sig)
{
	if (sig == SIGINT)
		exit(0);
	(void)signal(SIGINT, SIG_DFL);
}

int epdTest(char *filename)
{
	Game game;
	int nepd = 0;
	FILE *epdfile = fopen(filename, "r");

	if (epdfile == NULL)
	{
		printf("\nCan't Open file: %s \n", filename);
		return -1;
	}
	else
	{
		char buffer[255];
		char Answer[10];

		while (fgets(buffer, 254, epdfile) != NULL)
		{
			for (int i = 0; buffer[i] != 0 && buffer[i] != '\n'; ++i)
			{
				if (buffer[i] == ';')
				{
					int back = i;
					while (back > 0 && buffer[back] != ' ')
					{
						--back;
					}
					int ansi = 0;
					while (ansi < 9 && back < i)
						Answer[ansi++] = buffer[back++];
					Answer[ansi] = '\0'; /* Null Terminator */
					break;
				}
			}
			/* Prepare for thinking */
			memset(Scores, 0, sizeof(int) * MAX_DEPTH);
			SetfromFEN(buffer, &game.Position);
			printBoardinfo(&game.Position);
			game.MaxDepth = 9;
			available_time = game.available_time = clock() + (5 * CLOCKS_PER_SEC);
			flags = 0;
			thinking = 1;
			INIT_MUTEX(StopIterate);
			printf("\n => thinking ...\n");
			Start(&stopWatch);
			Think(&game);
			WAIT(IterativeDeepening);
			Stop(&stopWatch);

			printBoardinfo(&game.Position);
			printf("\n => Score: %d\n", Scores[current_depth - 1]);
			printf(" => Concluded ply %d search in ~ %.2f sec.", current_depth - 1, stopWatch.elapsed);
			printf("\n move: ");
			printmove(&BEST, &game.Position, SAN);
			printf("\n Correct Move: %s\n", Answer);
			printf("\nepd #%d engine move: %s, Correct move: %s", nepd++, moveOutput, Answer);
		}
	}
	fclose(epdfile);
}
