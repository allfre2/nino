#include "common.h"
#include "eval.h"
#include "gen.h"
#include "debug.h"

/*TODO:  make 12 values long so that it can be indexed by the piece in the board*/
const int Relative_Piece_Value[] = {100, 320, 330, 500, 900, INFINITY};
/* this one asumes middle game | opening stage */
short PieceSqValueT[][64] = {
   {
      0, 0, 0, 0, 0, 0, 0, 0,
      110, 110, 115, 115, 115, 115, 110, 110,
      70, 70, 75, 76, 76, 75, 70, 70,
      8, 10, 25, 27, 27, 25, 10, 8,
      6, 6, 7, 25, 25, 7, 6, 6,
      0, 0, 6, 7, 7, 6, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0
   },
   {
      -85, -50, -20, -15, -15, -20, -50, -85,
      -15, 0, 0, 0, 0, 0, 0, -15,
      -10, 10, 20, 30, 30, 20, 10, -10,
      -5, 15, 25, 40, 40, 25, 15, -5,
      -5, 15, 25, 40, 40, 25, 15, -5,
      -10, 10, 20, 30, 30, 20, 10, -10,
      -15, 0, 0, 0, 0, 0, 0, -15,
      -85, -50, -20, -15, -15, -20, -50, -85
   },
   {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 20, 10, 10, 10, 10, 20, 0,
      10, 20, 25, 29, 29, 25, 20, 10,
      15, 23, 30, 38, 38, 30, 23, 15,
      15, 23, 30, 38, 38, 30, 23, 15,
      10, 20, 25, 29, 29, 25, 20, 10,
      0, 20, 10, 10, 10, 10, 20, 0,
      0, 0, 0, 0, 0, 0, 0, 0
   },
   {
      0, 0, 7, 10, 10, 7, 0, 0,
      0, 0, 8, 13, 13, 8, 0, 0,
      0, 8, 9, 10, 10, 9, 8, 0,
      0, 0, 9, 9, 9, 9, 0, 0,
      0, 0, 9, 9, 9, 9, 0, 0,
      0, 8, 9, 10, 10, 9, 8, 0,
      0, 0, 8, 13, 13, 8, 0, 0,
      0, 0, 7, 10, 10, 7, 0, 0,
   },
   {
      0, 0, 0, 10, 10, 0, 0, 0,
      0, 10, 10, 13, 13, 10, 10, 0,
      8, 14, 25, 30, 30, 25, 14, 8,
      9, 21, 30, 50, 50, 30, 21, 9,
      9, 21, 30, 50, 50, 30, 21, 9,
      8, 14, 25, 30, 30, 25, 14, 8,
      0, 0, 10, 13, 13, 10, 0, 0,
      0, 0, 0, 10, 10, 0, 0, 0,
   },
   {
      20,  30,  20,  0, 0, -8, 30,  20,
      10,  10,  -7,  -10, -10, -7,  10,  10,
      -10, -15, -20, -30, -30, -20, -15, -10,
      -20, -30, -45, -50, -50, -45, -30, -20,
      -20, -30, -45, -50, -50, -45, -30, -20,
      -10, -15, -20, -30, -30, -20, -15, -10,
      10,  10,  -7,  -10, -10, -7,  10,  10,
      20,  30,  25,  0,   0,   -8,  30,  20
   }
};

int Eval(Boardmap *Position)
{

   /* Material evaluation, Maybe later take into account the game stage */
   int score = 0;
   uint64 pieceCount, Piece;

   for (int i = PAWN; i < KING; ++i)
   {
      Piece = Position->pieces[i][WHITE];
      PIECECOUNT(Piece, pieceCount);
      score += pieceCount * Relative_Piece_Value[i];
      Piece = Position->pieces[i][BLACK];
      PIECECOUNT(Piece, pieceCount);
      score -= pieceCount * Relative_Piece_Value[i];
   }

  /* Pawn Evaluation
   * Query Pawn Hash table to see if score for given position was already computed
   */

  /* Doubled pawns
   * TODO:
   * Piece - Sq , placement bonus
   */

   uint64 Pieces, nextpiece;
   int sq;
   
   for (int p = PAWN; p < 6; ++p)
   {
      Pieces = Position->pieces[p][WHITE];

      while (Pieces)
      {
         nextpiece = NEXTSQ(Pieces);
         LOG2(nextpiece, sq);
         score += PieceSqValueT[p][sq];
         LSBCLEAR(Pieces);
      }

      Pieces = Position->pieces[p][BLACK];

      while (Pieces)
      {
         nextpiece = NEXTSQ(Pieces);
         LOG2(nextpiece, sq);
         score -= PieceSqValueT[p][63 - sq];
         LSBCLEAR(Pieces);
      }
   }

   /* King attack and king safety */
   uint64 kings;
   int count;

   kings = Position->attackd[WHITE];
   PIECECOUNT(kings, count);
   score += count * 2;
   kings = Position->attackd[BLACK];
   PIECECOUNT(kings, count);
   score -= count * 2;

   return score;
}

int DrawEval(Boardmap *Position)
{
/* TODO: 
 * To return an eval score:
 * Check repeated positions
 * Check halfmove and full move clocks
 * Else return evaluations based on score of the opponent - the score of the side to move
 * this way if the side to move is better the score for a draw is negative and positive otherwise
 */
}

/* Maybe there's some use for just returning the diference beetwen the pieces as a exchange value
 * ie. if the piece is defended return CapturedPiece - attacking piece else return CapturedPiece value*/

int SEE(Move *move, Boardmap *Position)
{
   int score, turn, piece, sq;
   uint64 Attackers, Defenders, tosqBit, nextPiece, bitSpace, piecesBits[2];

   piece = NPIECE(Position->board[move->to]);

   if (Position->board[move->to] == EMPTY)
      score = 0;
   else
      score = Relative_Piece_Value[piece];

   tosqBit = BIT(move->to);

   if ((tosqBit & Position->attackd[Position->turn ^ 1]) == 0)
   {
      return score; /* Was a hanging piece */
   }
   else
   {
      piece = NPIECE(Position->board[move->from]);
      score -= Relative_Piece_Value[piece];
      if (score > 0)
      {
         return score;
      }
   }

   turn = Position->turn;
   Attackers = (AllRays[move->to] & (Position->empty[WHITE] | Position->empty[BLACK]));

   /* Loop to see wich pieces are actually attacking the move->to square */
   Defenders = Attackers; /* Use defenders as a holder for temporary copy of Attakers*/
  
   while (Defenders)
   {
      nextPiece = NEXTSQ(Defenders);
      LOG2(nextPiece, sq);

      if (!(Position->attacks[sq] & tosqBit))
         Attackers ^= nextPiece;

      LSBCLEAR(Defenders);
   }

   Attackers |= (KnightSqs[move->to] &
                 (Position->pieces[KNIGHT][WHITE] | Position->pieces[KNIGHT][BLACK]));
   Attackers ^= BIT(move->from); /* Exclude piece that makes the capture */
   piecesBits[turn] = Attackers & Position->empty[turn];
   piecesBits[turn ^ 1] = Attackers & Position->empty[turn ^ 1];

   nextPiece = BIT(move->from);
   /* Start swapping the pieces*/
   turn ^= 1;

   while (piecesBits[turn] && piecesBits[turn ^ 1])
   {
      /* Check Xrays */
      if (LONG_RANGE(piece) || piece == PAWN)
      {
         /* If previous piece is bishop, rook, queen or pawn check behind it */
         /* Calcular el bitmap de los bits entre la casilla de captura y la pieza*/
         bitSpace = BITSPACE(tosqBit, nextPiece); 
         LOG2(nextPiece, sq);
         uint64 discoveredPiece;
         discoveredPiece = 0;

         switch (piece)
         {
         case BISHOP:
         case PAWN:
            discoveredPiece = (~bitSpace & AllBishopSqs[move->to]) & AllBishopSqs[sq];
            break;
         case ROOK:
            discoveredPiece = (~bitSpace & AllRookSqs[move->to]) & AllRookSqs[sq];
            break;
         default: /* QUEEN */
            /* Has to be done in two parts?? the bishop rays and then the rook rays ...*/
            discoveredPiece = (~bitSpace & AllRookSqs[move->to]) & AllRookSqs[sq];
            if (discoveredPiece & AllRookSqs[move->to] == 0)
               discoveredPiece = (~bitSpace & AllBishopSqs[move->to]) & AllBishopSqs[sq];
            break;
         }

         discoveredPiece &= KingSqs[sq];
         if (discoveredPiece &
             (Position->pieces[BISHOP][turn ^ 1] | Position->pieces[ROOK][turn ^ 1] | Position->pieces[QUEEN][turn ^ 1]))
         {
            piecesBits[turn ^ 1] |= discoveredPiece;
         }
      }

      if (nextPiece = (piecesBits[turn] & Position->pieces[PAWN][turn]))
      {
         piece = PAWN;
      }
      else if (nextPiece = (piecesBits[turn] & Position->pieces[KNIGHT][turn]))
      {
         piece = KNIGHT;
      }
      else if (nextPiece = (piecesBits[turn] & Position->pieces[BISHOP][turn]))
      {
         piece = BISHOP;
      }
      else if (nextPiece = (piecesBits[turn] & Position->pieces[ROOK][turn]))
      {
         piece = ROOK;
      }
      else if (nextPiece = (piecesBits[turn] & Position->pieces[QUEEN][turn]))
      {
         piece = QUEEN;
      }
      else
      {
         piece = KING;
         nextPiece = piecesBits[turn] & Position->pieces[KING][turn];

         /* 
          * King stuff ... actually, do nothing. the code that follows will remove the previows
          * capture since it was an invalid move to consider. (king captures a defended piece) 
          */
      }

      nextPiece = NEXTSQ(nextPiece);
      piecesBits[turn] ^= nextPiece;

      if (turn == Position->turn)
      {
         score -= Relative_Piece_Value[piece];
         if (score > 0)
            break;
      }
      else
      {
         score += Relative_Piece_Value[piece];
         if (score < 0)
            break;
      }

      if (piece > KNIGHT && piece < KING)
      {
         // When pawns get out of way also goes ?
         /* SLIDING PIECE: check behind to se if there is another piece forming a battery */
      }
      turn ^= 1;
   }

   return score;
}
