#include <stdio.h>
#include "common.h"
#include "notation.h"

const char castlingchars[] = {"-KkQq"};

const char PgnTags[NTAGS][MAXTAGLEN] = {"Event", "Site", "Date", "Round", "White", "Black", "Result",
										"Opening", "Variation", "ECO", "FEN", "PlyCount", "Time", ""};

const char PgnErrorMsgs[][50] = {"C"};

int ValidateFEN(char *fen)
{
	int field = BOARD;

	for (int i = 0; i < MAXFENLEN; ++i)
	{
		if (fen[i] == FIELDELIMITER)
		{
			while (fen[i] == FIELDELIMITER && i < MAXFENLEN)
				++i;
			++field;
		}
		
		switch (field)
		{
		case BOARD:

			break;
		case TURN:
			break;
		case CASTLING:
			break;
		case EP:
			break;
		case HALFMOVECLOCK:
			break;
		case FULLMOVENUM:
			break;
		case END:
			return 0;
			break;
		}
	}
}

/* THis function needs the extra espaces removed from the fen string and other validations before applying it*/
int SetfromFEN(char *fen, Boardmap *Bmap)
{
	int field = BOARD,
		index = 0;
	char digit;

	Initboard(Bmap, NULL);

	for (int i = 0; fen[i] && i < MAXFENLEN && field != END; ++i)
	{
		if (fen[i] == FIELDELIMITER)
		{
			field++;
			continue;
		}

		switch (field)
		{
		case BOARD:
			if (index > 63)
				break;
			if (ISDIGIT(fen[i]))
			{
				digit = '0';
				while (fen[i] > digit && digit < '9')
				{
					++index;
					++digit;
				}
			}
			else if (fen[i] != ROWDELIMITER)
				Bmap->board[index++] = GetpieceN(fen[i]);
			break;
		case TURN:
			if (LOWERCASE(fen[i]) == 'w')
				Bmap->turn = WHITE;
			else
				Bmap->turn = BLACK;
			break;
		case CASTLING:
			if (fen[i] == EMPTYFIELD)
				Bmap->castle = Castling_rights[NONE];
			else
				Bmap->castle |= Castling_rights[Select(fen[i], "-KkQq")];
			break;
		case EP:
			if (fen[i] == EMPTYFIELD)
				break;
			if (fen[i + 1] && fen[i + 1] != FIELDELIMITER)
				Bmap->ep = CHARSQINT(fen[i], fen[i + 1]);
			break;
		case HALFMOVECLOCK:
			break;
		case FULLMOVENUM:
			break;
		}
	}

	Map(Bmap);
	Bmap->key = HashKey(Bmap);
	return field;
}

int GetFENof(Boardmap *Bmap, char *fen)
{
	/*--> BOARD FIELD */
	int index = 0;
	char digit = '0';

	for (int i = 0; i <= 64 && index < MAXFENLEN - 3; ++i)
	{
		if (i > 0 && (i % 8) == 0)
		{
			if (digit > '0')
			{
				fen[index++] = digit;
				digit = '0';
			}
			if (i != 64)
				fen[index++] = ROWDELIMITER;
			else
				break;
		}
		if (Bmap->board[i] != EMPTY)
		{
			if (digit > '0')
			{
				fen[index++] = digit;
				digit = '0';
			}
			fen[index++] = Pieces[Bmap->board[i]];
		}
		else
			digit++;
	}
	/*--> TURN FIELD */

	fen[index++] = FIELDELIMITER;
	fen[index++] = Bmap->turn ? 'w' : 'b';

	/*--> CASTLING */

	fen[index++] = FIELDELIMITER;

	if (Bmap->castle == Castling_rights[NONE])
		fen[index++] = castlingchars[NONE];
	else
	{
		if (Bmap->castle & Castling_rights[K])
			fen[index++] = castlingchars[K];
		if (Bmap->castle & Castling_rights[Q])
			fen[index++] = castlingchars[Q];
		if (Bmap->castle & Castling_rights[k])
			fen[index++] = castlingchars[k];
		if (Bmap->castle & Castling_rights[q])
			fen[index++] = castlingchars[q];
	}

	/*--> ENPASSANT */

	if (MAXFENLEN - index > 3)
	{
		fen[index++] = FIELDELIMITER;
		if (Bmap->ep != 0)
		{
			fen[index++] = COLUMNCHAR(Bmap->ep);
			fen[index++] = ROWCHAR(Bmap->ep);
		}
		else
			fen[index++] = EMPTYFIELD;
	}

	/*--> HALFMOVECLOCK */

	/*--> FULLMOVENUM */

	fen[index] = 0;

	return 0;
}

int LoadPgnFromFile(void *fileptr, Pgn *Pgns, int limit)
{
	char line[MAXPGNLINE];
	FILE *file = (FILE *)fileptr;
	if (file == NULL)
		return -1;
	int games = 0, tagindex;
	char errorflags, tag[MAXTAGLEN], tagn;

	while (games < limit && fgets(line, MAXPGNLINE, file) != NULL)
	{
		for (int i = 0; line[i]; ++i)
		{
			if (errorflags)
				return -errorflags;
			if (line[i] == '[')
			{
				tagindex = 0;
				while (line[i] && Select(line[i], "- \t") != 0)
					++i;
				while (line[i] && Select(line[i], "- \t") == 0)
					tag[tagindex++] = line[i++];
				while (line[i] && Select(line[i], "-\"") == 0)
					++i;
				tagn = wichTag(tag);
				if (tagn != -1)
				{
					Pgns[games];
				}
			}
			else if (line[i] == '(')
			{
			}
			else if (line[i] == '{')
			{ /* braquet comment */
			}
			else if ((line[i] == '%' && i == 0) || line[i] == ';')
				continue; /* end of line comment*/
			else if (line[i])
			{
			}
		}
	}
	return games;
}

int wichTag(char *str)
{
}

int SANmove(Move *move, Boardmap *Position, char *SAN)
{
	char piece;
	int index = 0;

	if (move->castle)
	{
		SAN[0] = 'O';
		SAN[1] = '-';
		SAN[2] = 'O';
		index = 3;
		if (move->castle & LONGCASTLE)
		{
			SAN[3] = '-';
			SAN[4] = 'O';
			index = 5;
		}
	}
	else
	{
		piece = Position->board[move->from];

		if (NPIECE(piece) != PAWN)
			SAN[index++] = Pieces[piece];

		/*check if there is ambiguity for this piece or if move is enpassant */
		/* use ISPOWEROFTWO on the bitmap of the pieces of the same type that can attack the `to` square */
		/* then if it is a pawn just put the column char there*/

		if (Position->board[move->to] != EMPTY)
			SAN[index++] = 'x'; /*check if move is capture. (put an `x` there)*/

		SAN[index++] = COLUMNCHAR(move->to);
		SAN[index++] = ROWCHAR(move->to);

		if (move->promote)
		{
			SAN[index++] = '=';
			SAN[index++] = Pieces[move->promote];
		}
	}

	if (Position->pieces[KING][Position->turn ^ 1] & Position->attackd[Position->turn])
		SAN[index++] = '+';

	SAN[index] = '\0';

	return index;
}

int Parsemove(char *input, Move *move, Boardmap *Position)
{
	/* 
    	Reads stdin and tries to parse the input to a Move (for now )
    	Returns 0 if everything Ok else returns -1 
 	*/
	int count = 0, i;

	for (i = 0; input[i] && i < 255; ++i)
	{
		input[i] |= 0x20; /* Make it Lower case*/

		if (input[i] < 123 && input[i] > 96)
		{
			break;
		}
	}

	if (i > 240)
		return -1;

	move->from = CHARSQINT((input[i] | 0x20), (input[i + 1] | 0x20));
	move->to = CHARSQINT((input[i + 2] | 0x20), (input[i + 3] | 0x20));

	if (move->from < 0 || move->from > 63 || move->to < 0 || move->to > 63)
		return -1;

	if (NPIECE(Position->board[move->from]) == KING &&
		((move->from == 60 && (move->to == 62 || move->to == 58)) || (move->from == 4 && (move->to == 6 || move->to == 2))))
	{
		move->castle = (move->to - move->from) > 0 ? SHORTCASTLE : LONGCASTLE;
	}
	else
	{
		move->castle = 0;

		if (input[i + 4] && (input[i + 4] | 0x20) < 123 && (input[i + 4] | 0x20) > 96)
		{
			move->promote = GetpieceN((input[i + 4] | 0x20));
			if (move->promote != EMPTY)
			{
				move->promote += Position->turn == WHITE ? 6 : 0;
			}
			else
				return -1;
		}
		else
			move->promote = 0;
	}

	return 0;
}