#ifndef CHESS_NOTATION_H
#define CHESS_NOTATION_H
#endif

#define INVALIDFEN -0xf3
#define FIELDELIMITER ' '
#define ROWDELIMITER '/'
#define EMPTYFIELD '-'
#define MAXFENLEN 100
#define MAXPGNFIELDS 20
#define MAXPGNGAMES 30

#define MAXPGNLINE 255
#define NTAGS 14
#define MAXTAGLEN 20

#define CHARSQINT(a, b) \
	((63 - ('h' - LOWERCASE(a))) - (((b) - '1') * 8))
#define COLUMNCHAR(n) \
	('a' + ((n)&7))
#define ROWCHAR(n) \
	('8' - ((n) >> 3))

//TODO SAN notation parsing

enum
{
	_SHORT,
	SAN,
	DESCRIPTIVE
};

enum
{
	BOARD,
	TURN,
	CASTLING,
	EP,
	HALFMOVECLOCK,
	FULLMOVENUM,
	END // end of fen

} FenFields;

enum
{
	EVENT,
	SITE,
	DATE_,
	ROUND,
	WHITEPLAYER,
	BLACKPLAYER,
	RESULT
} PgnFieldEnum;

typedef struct
{
	char FieldData[MAXPGNFIELDS][50];
	Move moves[GAMEMAXMOVES];
} Pgn;

enum
{
	COMMENT = 0x01,
	EOLCOMMENT = 0x02,
	BRACECOMMENT = 0x04,
	MOVE = 0X08,
	PERCENTSCAPE = 0x10,
	TAGPAIR = 0x20

} PgnParseFlags;

enum
{
	COMMENTERROR,
	EOLCOMMENTERROR,
	BRACECOMMEN

} PgnErrorCodes;

int ValidateFEN(char *);
int SetfromFEN(char *, Boardmap *);
int GetFENof(Boardmap *, char *);
int LoadPgnFromFile(void *, Pgn *, int);
int wichTag(char *);
int PrintPgn(Pgn *);
int SANmove(Move *, Boardmap *, char *);
int Parsemove(char *, Move *, Boardmap *);

extern const char castlingchars[];
extern const char PgnTags[][20];