#include "common.h"
#include "gen.h"
#include "hash.h"
#include "tm.h"
#include "thread.h"
#include <string.h>
#include <stdio.h>

int thinking = 0;
double available_time;
int current_depth;
const uint64 one = 1;

char algebraic[64][4] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

const char Pieces[] = {"pnbrqkPNBRQK."};

#if defined(_WIN32) || defined(_WIN64)
    const char *wPieces[] = {
        "p", "n", "b", "r", "q", "k",
        "P", "N", "B", "R", "Q", "K",
        ".", 0};
#else
    const char *wPieces[] = {
        "\u265F", "\u265E", "\u265D", "\u265C", "\u265B", "\u265A",
        "\u2659", "\u2658", "\u2657", "\u2656", "\u2655", "\u2654",
        ".", 0};
#endif

const char Castling_rights[] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x0f};

const uint64 CastleShortMask[][2] = {
    {0x00000000000000070, 0x7000000000000000},
    {0x0000000000000060, 0x6000000000000000},
    {0x0000000000000010, 0x1000000000000000},
    {0x0000000000000080, 0x8000000000000000}};

const uint64 CastleLongMask[][2] = {
    {0x000000000000001c, 0x1c00000000000000},
    {0x000000000000000e, 0x0e00000000000000},
    {0x0000000000000010, 0x1000000000000000},
    {0x0000000000000001, 0x0100000000000000}};

const uint64 epSqs[2] = {0x0000ff0000000000, 0x0000000000ff0000};
const uint64 PwnStartSqs[2] = {0x000000000000ff00, 0x00ff000000000000};
const uint64 CenterSqs = 0x00007c3c3c3e0000;

const char StartPositionFEN[] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - "};

void Initboard(Boardmap *Bmap, char *Board)
{
    memset((void *)Bmap, 0, sizeof(Boardmap));

    for (int i = 0; i < 64; ++i)
        if (Board == NULL)
            Bmap->board[i] = EMPTY;
        else
            Bmap->board[i] = GetpieceN(Board[i]);
    
    Map(Bmap);
    Bmap->key = HashKey(Bmap);
}

int GetpieceN(char c)
{
    for (int i = 0; i < 13; ++i)
        if (c == Pieces[i])
            return i;
    return EMPTY;
}

int Select(char c, char *options)
{
    for (int i = 0; options[i]; ++i)
        if (c == options[i])
            return i;
    return 0;
}

inline uint64 PowerOfTwo(uint64 x)
{
    return ((x) && ((x) & ((x)-1)) == 0) ? 1 : 0;
}
