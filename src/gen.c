
#include "common.h"
#include "gen.h"
#include "Bitmaps.c"
	#include "debug.h"
#include <stdio.h>
#include <string.h>

const int AttackTable[][10] = {
			    {-11,-13,0,+11,+13,0},
			    {-25,-23,-14,-10,+10,+14,+23,+25,0},
			    {-13,-11,+11,+13,0},
			    {-12,-1,+1,+12,0},
			    {-13,-12,-11,-1,+1,+11,+12,+13,0},
			    {-13,-12,-11,-1,+1,+11,+12,+13,0}
			     };
/* Esta funcion inicializa los bitboards que indican las posibles jugadas de
   cada pieza. Deben quedar inicializados estaticamente con el programa
   compilado */
int initBitmaps(){

	int movesq,move;

	for(int sq = 0; sq < 64; ++sq){

	 RightSqs[sq] = 0;
	 LeftSqs [sq] = 0;
	 DownSqs [sq] = 0;
	 UpSqs   [sq] = 0;

	 BishopURSqs[sq] = 0;
	 BishopULSqs[sq] = 0;
	 BishopDRSqs[sq] = 0;
	 BishopDLSqs[sq] = 0;

	 KnightSqs[sq] = 0;

	 PwnSqs[WHITE][sq] = PwnSqs[BLACK][sq] = 0;
	 PwnFwdSqs[WHITE][sq] = PwnFwdSqs[BLACK][sq] = 0;
	 Pwn2StepSqs[WHITE][sq] = Pwn2StepSqs[BLACK][sq] = 0;
	 KingSqs[sq] = 0;

	 AllRays[sq] = 0;

	 movesq = MailBox[MailBoxNumber[sq] + 1];

	 while(movesq != -1){

	  RightSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] + 1];

	 }
	 movesq = MailBox[MailBoxNumber[sq] - 1];

	 while(movesq != -1){

	  LeftSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] - 1];

	 }
	 movesq = MailBox[MailBoxNumber[sq] + 12];

	 while(movesq != -1){

	  DownSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] + 12];

	 }
	 movesq = MailBox[MailBoxNumber[sq] - 12];

	 while(movesq != -1){

	  UpSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] - 12];

	 }

/* Fill Diagonals here */
	 movesq = MailBox[MailBoxNumber[sq] - 11];

	 while(movesq != -1){

	  BishopURSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] - 11];

	 }
	 movesq = MailBox[MailBoxNumber[sq] - 13];

	 while(movesq != -1){

	  BishopULSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] - 13];

	 }
	 movesq = MailBox[MailBoxNumber[sq] + 13];

	 while(movesq != -1){

	  BishopDRSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] + 13];

	 }
	 movesq = MailBox[MailBoxNumber[sq] + 11];

	 while(movesq != -1){

	  BishopDLSqs[sq] |= BIT(movesq);
	  movesq = MailBox[MailBoxNumber[movesq] + 11];

	 }
/***********************/
	 for(int i = 0; AttackTable[KNIGHT][i] != 0; ++i){
	  movesq = MailBox[MailBoxNumber[sq] + AttackTable[KNIGHT][i]];
	  if(movesq != -1) KnightSqs[sq] |= BIT(movesq);
	 }
	 for(int i = 0; AttackTable[KING][i] != 0; ++i){
	  movesq = MailBox[MailBoxNumber[sq] + AttackTable[KING][i]];
	  if(movesq != -1) KingSqs[sq] |= BIT(movesq);
	 }

	 if((movesq = (MailBox[MailBoxNumber[sq] + 11])) != -1) PwnSqs[BLACK][sq] |= BIT(movesq);
	 if((movesq = (MailBox[MailBoxNumber[sq] + 13])) != -1) PwnSqs[BLACK][sq] |= BIT(movesq);
	 if((movesq = (MailBox[MailBoxNumber[sq] - 11])) != -1) PwnSqs[WHITE][sq] |= BIT(movesq);
	 if((movesq = (MailBox[MailBoxNumber[sq] - 13])) != -1) PwnSqs[WHITE][sq] |= BIT(movesq);

	 PwnFwdSqs[WHITE][sq] |= BIT(sq-8);
	 if(BIT(sq) & PwnStartSqs[WHITE]){  Pwn2StepSqs[WHITE][sq] |= BIT(sq-16);}
	 PwnFwdSqs[BLACK][sq] |= BIT(sq+8);
	 if(BIT(sq) & PwnStartSqs[BLACK]){  Pwn2StepSqs[BLACK][sq] |= BIT(sq+16);}

	 AllBishopSqs[sq] =
	 BishopURSqs[sq] | BishopULSqs[sq] |
	 BishopDRSqs[sq] | BishopDLSqs[sq] ;

	 AllRookSqs[sq] =
	 RightSqs[sq]    | LeftSqs [sq] |
	 DownSqs [sq]    | UpSqs   [sq] ;

	 AllRays[sq] = 	 AllBishopSqs[sq] | AllRookSqs[sq];

	}
}

int _GenMoves(Boardmap * Position, Move moves[]){

	int curr = 0;
	char piece,sq,msq,ksq,promotecolor = Position->turn ? 6 : 0;
	char turn = Position->turn,
	     op= Position->turn^1;
	uint64 pieces,square,allpieces,allmoves,movesq,KingSq,bitspace,xrays,kNight,AttackRays;
	pieces = Position->empty[Position->turn];
	KingSq = Position->pieces[KING][turn];
	LOG2(KingSq,ksq);
	if(INCHECK(Position)){
	 kNight = KingSqs[ksq] & Position->pieces[KNIGHT][op];
	}
	allpieces = Position->empty[turn] | Position->empty[op];
	while(pieces){
	 square = NEXTSQ(pieces);
	 LOG2(square,sq);
	 xrays = ~0;
//#ifdef ONLYLEGALGEN
/*
	 if(AllBishopSqs[ksq] & square){
	  xrays = Xray(KingSq,square,allpieces);
	  if((xrays & (Position->pieces[BISHOP][oponent] | Position->pieces[QUEEN][oponent])) == 0) xrays = ~0;
	 }else
	 if(AllRookSqs[ksq] & square){
	  xrays = Xray(KingSq,square,allpieces);
	  if((xrays & (Position->pieces[ROOK][oponent] | Position->pieces[QUEEN][oponent])) == 0) xrays = ~0;
	 }
//*/
//#endif
/*
if((AllRays[ksq] & square) == square){
printBitmap(GetBishopRay(square,Position->pieces[KING][turn]),-1);
printBitmap(GetRookRay(square,Position->pieces[KING][turn]),-1);
printBoardinfo(Position);
getchar();
}//*/

	 allmoves = Position->moves[sq];// & xrays;
/*	 if(square & Position->pieces[KING][turn])
          allmoves &= ~Position->attackd[op];
*/	 while(allmoves){
	  movesq = NEXTSQ(allmoves);
	  LOG2(movesq,msq);

 //if(square & (Position->pieces[KNIGHT][turn] |Position->pieces[KING][turn] ) == 0){
    /*     int npiece;
         npiece = NPIECE(Position->board[sq]);
         if(npiece == BISHOP)
	 printBitmap(GetBishopRay(square,movesq),-1);
         else
         if(npiece == ROOK)
	 printBitmap(GetRookRay(square,movesq),-1);
         else if(npiece == QUEEN){
          if(GetBishopRay(square,movesq))
	 printBitmap(GetBishopRay(square,movesq),-1);
          else
	 printBitmap(GetRookRay(square,movesq),-1);
	 }
         printBoardinfo(Position);
//}
//*/

	   if((square & Position->pieces[PAWN][Position->turn]) && (movesq & PROMOTESQMASK)){
	    moves[curr].castle = moves[curr+1].castle = moves[curr+2].castle = moves[curr+3].castle = 0;
	    moves[curr].from = moves[curr+1].from = moves[curr+2].from = moves[curr+3].from = sq;
	    moves[curr].to = moves[curr+1].to = moves[curr+2].to = moves[curr+3].to = msq;
	    moves[curr].promote = QUEEN+promotecolor;
	    moves[curr+1].promote = ROOK+promotecolor;
	    moves[curr+2].promote = BISHOP+promotecolor;
	    moves[curr+3].promote = KNIGHT+promotecolor;
	    curr += 4;

	   }
	   else{
	    moves[curr].castle = moves[curr].promote = 0;
	    moves[curr].from = sq;
	    moves[curr].to = msq;
	    ++curr;
   /*          printf("\n\n");
		printM(&moves[curr-1],Position);
	 getchar();//*/
	   }
	 if (curr >= MAXMOVES - 4) return curr;
	 LSBCLEAR(allmoves);
	}
	LSBCLEAR(pieces);
	}
	/*Handle castling*/ /* RE DO Entire castling mechanism of the program*/
	if((Position->castle & (Position->turn ? Castling_rights[K] : Castling_rights[k])) &&
		((Position->attackd[op] & CastleShortMask[ATTCKD][turn]) == 0) &&
		(((Position->empty[op] | Position->empty[turn]) & CastleShortMask[BEETWEN][turn]) == 0) &&
		((Position->pieces[KING][turn] & CastleShortMask[KINGSQ][turn]) && (Position->pieces[ROOK][turn] & CastleShortMask[ROOKSQ][turn]))) {
	 moves[curr++].castle = SHORTCASTLE;
}
	if((Position->castle & (Position->turn ? Castling_rights[Q] : Castling_rights[q])) &&
	  ((Position->attackd[op] & CastleLongMask[ATTCKD][turn]) == 0) &&
	  (((Position->empty[op] | Position->empty[turn]) & CastleLongMask[BEETWEN][turn]) == 0) &&
	  ((Position->pieces[KING][turn] & CastleLongMask[KINGSQ][turn]) && (Position->pieces[ROOK][turn] & CastleLongMask[ROOKSQ][turn]))){
	 moves[curr++].castle = LONGCASTLE;
}
/****************/
 return curr;
}

/*
CASTLING IMPROVEMENT: TODO

side mask:  white -> 0xff00000000000000
	    black -> 0x00000000000000ff
if(Position - > castle == SHORT)
Position-> attackd [oponent] & 0xf0000000000000f0      |
((Position->empty[0] | Position->empty[1]) & 0x6000000000000060 ) & side mask  == 0   can castle
*/

int GenMoves(Boardmap * Position, Move moves[]){
/* TODO: very important add ep exceptions asap*/
 /* Cuando en jaque no se generan peon al paso */
	int curr = 0,pinSq;
	char piece,sq,msq,ksq,promotecolor = Position->turn ? 6 : 0;
	char turn = Position->turn,
	     op= Position->turn^1;
	uint64 pieces,square,allpieces,allmoves,movesq,KingBit,bitspace,xrays,ray,Pinned,Check,Xpiece,BnQ,RnQ,kNight,AllCheckRays;
	KingBit = Position->pieces[KING][turn];
	LOG2(KingBit,ksq);

	allpieces = Position->empty[turn] | Position->empty[op];
// NEW CODE
BnQ = Position->pieces[QUEEN][op] | Position->pieces[BISHOP][op];
RnQ = Position->pieces[QUEEN][op] | Position->pieces[ROOK][op];
pieces = AllRays[ksq] &
	 (Position->pieces[QUEEN][op] |
	  Position->pieces[ROOK][op]  |
	  Position->pieces[BISHOP][op]);
Pinned = AllCheckRays = Check = 0;

while(pieces){

	 square = NEXTSQ(pieces);
	 LOG2(square,sq);
	 ray = 0;
	 if((square & Position->pieces[ROOK][op]) == 0 && square & AllBishopSqs[ksq]){
	  ray = GetBishopRay(KingBit,square);
	 }else
	 if((square & Position->pieces[BISHOP][op]) == 0  && square & AllRookSqs[ksq]){
	  ray = GetRookRay(KingBit,square);
	 }
	 if(ray){
	  Xpiece = (ray ^ square) & allpieces;
/*if(INCHECK(Position)){
	  printBoardinfo(Position);
	  printBitmap(ray,-1);
	  printBitmap(Xpiece,-1);
	  getchar();
}*/
	  if(Xpiece == 0){
	   Check |= square;
	   CheckRays[sq] = ray;
	   AllCheckRays |= ray;
	  }else
	  if(PowerOfTwo(Xpiece) && Xpiece & Position->empty[turn]){
  LOG2(Xpiece,pinSq);
  CheckRays[pinSq] = ray;
  Pinned |= Xpiece;
}
	}

	LSBCLEAR(pieces);
}

//
Check |= PwnSqs[turn][ksq] & Position->pieces[PAWN][op];
kNight = KnightSqs[ksq] & Position->pieces[KNIGHT][op];
Check |= kNight;
AllCheckRays |= PwnSqs[turn][ksq] & Position->pieces[PAWN][op];
AllCheckRays |= kNight;
LOG2(kNight,pinSq);
CheckRays[pinSq] = 0;

if(0 && Pinned && !thinking){
	printBoardinfo(Position);
	printBitmap(Pinned,-1);
	printBitmap(Check,-1);
	getchar();
}/*else if(INCHECK(Position)){

 printf("\nshiiiiiiiiiiiiiiiiiiiiitttttttttttttttt\n");
 printBoardinfo(Position);
 printBitmap(Check,-1);

}//*/
//
//
/*
	if(Check){
	 if(!INCHECK(Position)) printf("NOOOOOOOOOOOOOOOOO\n"),printBoardinfo(Position),getchar();
	 if(!PowerOfTwo(Check)) pieces = KingBit; // It's a Double Check
	 else{
	  if(kNight) AllCheckRays = kNight;
	 }
	}else{
	   pieces = Position->empty[Position->turn];
	   AllCheckRays = ~0;
	 }*/
	 /*if(Check && !PowerOfTwo(Check)) pieces = KingBit; // It's a Double Check
	   else*/ pieces = Position->empty[Position->turn];
	while(pieces){
	 square = NEXTSQ(pieces);
	 LOG2(square,sq);
/*
if((AllRays[ksq] & square) == square){
printBitmap(GetBishopRay(square,Position->pieces[KING][turn]),-1);
printBitmap(GetRookRay(square,Position->pieces[KING][turn]),-1);
printBoardinfo(Position);
getchar();
}//*/
	 allmoves = Position->moves[sq] ;
	 if(square & KingBit)
          allmoves &= ~Position->attackd[op];
	 else{

	  if(square & Pinned) allmoves &= CheckRays[sq];
	  if(Check) allmoves &= AllCheckRays;

	 }
/*	 else if(Check)
	   allmoves &= AllCheckRays;
*/
	 while(allmoves){
	  movesq = NEXTSQ(allmoves);
	  LOG2(movesq,msq);

 //if(square & (Position->pieces[KNIGHT][turn] |Position->pieces[KING][turn] ) == 0){
    /*     int npiece;
         npiece = NPIECE(Position->board[sq]);
         if(npiece == BISHOP)
	 printBitmap(GetBishopRay(square,movesq),-1);
         else
         if(npiece == ROOK)
	 printBitmap(GetRookRay(square,movesq),-1);
         else if(npiece == QUEEN){
          if(GetBishopRay(square,movesq))
	 printBitmap(GetBishopRay(square,movesq),-1);
          else
	 printBitmap(GetRookRay(square,movesq),-1);
	 }
         printBoardinfo(Position);
//}
//*/

	   if((square & Position->pieces[PAWN][Position->turn]) && (movesq & PROMOTESQMASK)){
	    moves[curr].castle = moves[curr+1].castle = moves[curr+2].castle = moves[curr+3].castle = 0;
	    moves[curr].from = moves[curr+1].from = moves[curr+2].from = moves[curr+3].from = sq;
	    moves[curr].to = moves[curr+1].to = moves[curr+2].to = moves[curr+3].to = msq;
	    moves[curr].promote = QUEEN+promotecolor;
	    moves[curr+1].promote = ROOK+promotecolor;
	    moves[curr+2].promote = BISHOP+promotecolor;
	    moves[curr+3].promote = KNIGHT+promotecolor;
	    curr += 4;

	   }
	   else{
	    moves[curr].castle = moves[curr].promote = 0;
	    moves[curr].from = sq;
	    moves[curr].to = msq;
	    ++curr;
   /*          printf("\n\n");
		printM(&moves[curr-1],Position);
	 getchar();//*/
	   }
	 if (curr >= MAXMOVES - 4) return curr;
	 LSBCLEAR(allmoves);
	}
	LSBCLEAR(pieces);
	}
	/*Handle castling*/ /* RE DO Entire castling mechanism of the program*/
	if((Position->castle & (Position->turn ? Castling_rights[K] : Castling_rights[k])) &&
		((Position->attackd[op] & CastleShortMask[ATTCKD][turn]) == 0) &&
		(((Position->empty[op] | Position->empty[turn]) & CastleShortMask[BEETWEN][turn]) == 0) &&
		((Position->pieces[KING][turn] & CastleShortMask[KINGSQ][turn]) && (Position->pieces[ROOK][turn] & CastleShortMask[ROOKSQ][turn]))) {
	 moves[curr++].castle = SHORTCASTLE;
}
	if((Position->castle & (Position->turn ? Castling_rights[Q] : Castling_rights[q])) &&
	  ((Position->attackd[op] & CastleLongMask[ATTCKD][turn]) == 0) &&
	  (((Position->empty[op] | Position->empty[turn]) & CastleLongMask[BEETWEN][turn]) == 0) &&
	  ((Position->pieces[KING][turn] & CastleLongMask[KINGSQ][turn]) && (Position->pieces[ROOK][turn] & CastleLongMask[ROOKSQ][turn]))){
	 moves[curr++].castle = LONGCASTLE;
}
/****************/
 if(0 && debug && Check && Check & Position->pieces[PAWN][op]){
   printBitmap(Check,-1);
   printBoardinfo(Position);
   printBitmap(AllCheckRays,-1);
  for(int i =0; i < curr; i++){
   printf(", "); printM(&moves[i],Position);
  }
   if(getchar() == 'q'){
    printf("\nDebug is now off.\n");
    getchar();
   }
 }//*/
 return curr;
}
int GenCaptures(Boardmap * Position, Move moves[]){
/* TODO:
	Maybe add flag for generating promotions too , or very tactical aggresive moves (checks,etc)*/

	int curr = 0;
	char sq,xsq,promotecolor = Position->turn ? 6 : 0;
	uint64 pieces,square,captures,capturesq,ep,pawns;
	pieces = Position->empty[Position->turn];
	ep = BIT(Position->ep) & epSqs[Position->turn];
	pawns = Position->pieces[PAWN][Position->turn];
	while(pieces){
	 square = NEXTSQ(pieces);
	 LOG2(square,sq);

	 captures = Position->moves[sq] & (Position->empty[Position->turn^1] | (pawns & square ? ep : 0));

	  while(captures){
	   capturesq = NEXTSQ(captures);

	   LOG2(capturesq,xsq);

	   if((square & pawns) && (capturesq & PROMOTESQMASK)){
	    moves[curr].castle = moves[curr+1].castle = moves[curr+2].castle = moves[curr+3].castle = 0;
	    moves[curr].from = moves[curr+1].from = moves[curr+2].from = moves[curr+3].from = sq;
	    moves[curr].to = moves[curr+1].to = moves[curr+2].to = moves[curr+3].to = xsq;
	    moves[curr].promote = QUEEN+promotecolor;
	    moves[curr+1].promote = ROOK+promotecolor;
	    moves[curr+2].promote = BISHOP+promotecolor;
	    moves[curr+3].promote = KNIGHT+promotecolor;
	    curr += 4;

	   }
	   else{
	    moves[curr].castle = moves[curr].promote = 0;
	    moves[curr].from = sq;
	    moves[curr].to = xsq;
	    ++curr;
	   }
	 if (curr >= MAXMOVES - 4) return curr;
	   LSBCLEAR(captures);
	  }
	 LSBCLEAR(pieces);
	}
 return curr;
}

uint64 Xray(uint64 hidden, uint64 pinned, uint64 allpieces){
/* This function will output bitmap with the piece that is right
 * behind the pinned piece in a ray that touches the hidden
 * piece. If to use for discover pins it will have to be tested
 * to see if the ray beetwen the xray piece and the hidden piece
 * is not interfiered by any other than the pinned piece */
 uint64 ray,xray;
 int hsq,psq;
/*
 for(int i = 0; i < 64; i++){
  printf("\nbeetwen %d and %d\n",ksq,i);
  printf("\nBitspace\n");
  printBitmap(BITSPACE(KingSq,BIT(i)),ksq);getchar();
  printf("\n bishopray\n");
  printBitmap(GetBishopRay(KingSq,BIT(i)),-1);getchar();
  printf("\n rookray\n");
  printBitmap(GetRookRay(KingSq,BIT(i)),-1);getchar();
  printf("\n ~bitspace\n");
  printBitmap(~(BITSPACE(KingSq,BIT(i))),ksq);getchar();
 }
*/
 xray = 0;
 LOG2(hidden,hsq);
 LOG2(pinned,psq);
 if(ray = GetBishopRay(hidden,pinned)){
  if((ray & allpieces) == pinned){
    ray |= hidden;

    if(BishopURSqs[psq] & ray){
     xray = BishopDLSqs[psq] & allpieces;
     xray = SHIFTDL(xray);
     xray =( xray & BishopDLSqs[psq]) ^ BishopDLSqs[psq];
    }
    else
    if(BishopULSqs[psq] & ray){
     xray = BishopDRSqs[psq] & allpieces;
     xray = SHIFTDR(xray);
     xray =( xray & BishopDRSqs[psq]) ^ BishopDRSqs[psq];
    }
    else
    if(BishopDRSqs[psq] & ray){
     xray = BishopULSqs[psq] & allpieces;
     xray = SHIFTUL(xray);
     xray =( xray & BishopULSqs[psq]) ^ BishopULSqs[psq];
    }
    else
    if(BishopDLSqs[psq] & ray){
     xray = BishopURSqs[psq] & allpieces;
     xray = SHIFTUR(xray);
     xray =( xray & BishopURSqs[psq]) ^ BishopURSqs[psq];
    }
  }else xray = 0;
 }else
 if(ray = GetRookRay(hidden,pinned)){
  if((ray & allpieces) == pinned){
    ray |= hidden;
    if(UpSqs[psq] & ray){
     xray = DownSqs[psq] & allpieces;
     xray = SHIFTDOWN(xray);
     xray = (xray & DownSqs[psq]) ^ DownSqs[psq];
    }
    else
    if(DownSqs[psq] & ray){
     xray = UpSqs[psq] & allpieces;
     xray = SHIFTUP(xray);
     xray = (xray & UpSqs[psq]) ^ UpSqs[psq];
    }
    else
    if(LeftSqs[psq] & ray){
     xray = RightSqs[psq] & allpieces;
     xray = SHIFTRIGHT(xray);
     xray = (xray & RightSqs[psq]) ^ RightSqs[psq];
    }
    else
    if(RightSqs[psq] & ray){
     xray = LeftSqs[psq] & allpieces;
     xray = SHIFTLEFT(xray);
     xray = (xray & LeftSqs[psq]) ^ LeftSqs[psq];
    }

  }else xray = 0;
 }
 return xray;
}

uint64 GetBishopRay(uint64 Bsq, uint64 sq){

 uint64 ray;
 ray = 0;
 int Bnsq;
 LOG2(Bsq,Bnsq);

 if(BishopURSqs[Bnsq] & sq)
  ray = BITSPACE(Bsq,sq) & BishopURSqs[Bnsq];
 else
 if(BishopULSqs[Bnsq] & sq)
  ray = BITSPACE(Bsq,sq) & BishopULSqs[Bnsq];
 else
 if(BishopDRSqs[Bnsq] & sq)
  ray = BITSPACE(Bsq,sq) & BishopDRSqs[Bnsq];
 else
 if(BishopDLSqs[Bnsq] & sq)
  ray = BITSPACE(Bsq,sq) & BishopDLSqs[Bnsq];

 return ray;
}

uint64 GetRookRay(uint64 Rsq, uint64 sq){

 uint64 ray;
 ray = 0;
 int Rnsq;
 LOG2(Rsq,Rnsq);

 if(UpSqs[Rnsq] & sq)
  ray = BITSPACE(Rsq,sq) & UpSqs[Rnsq];
 else
 if(DownSqs[Rnsq] & sq)
  ray = BITSPACE(Rsq,sq) & DownSqs[Rnsq];
 else
 if(RightSqs[Rnsq] & sq)
  ray = BITSPACE(Rsq,sq) & RightSqs[Rnsq];
 else
 if(LeftSqs[Rnsq] & sq)
  ray = BITSPACE(Rsq,sq) & LeftSqs[Rnsq];

 return ray;

}

int GenCheckEscapes(Boardmap * Position, Move moves[]){
/* TODO: when its a king check see that if the king moves
 *       in the same ray that is is given check wont move
 *       back and believe its a valid move ... ://*/
 int curr,hit;
 char piece,sq,msq,turn,op,ksq,promotecolor = Position->turn ? 6 : 0;
 uint64 pieces,square,allmoves,movesq,RayAttacks,kAttacks,kNight,KingBit,ray,xray,allpieces,epBit;
 turn = Position->turn,
 op   = Position->turn^1,
 curr = 0;
 KingBit = Position->pieces[KING][turn];
 pieces = Position->empty[turn];
 allpieces = Position->empty[op] | pieces;
 LOG2(KingBit,ksq);
 kNight = (KnightSqs[ksq] & Position->pieces[KNIGHT][op]);
 pieces = (Position->pieces[BISHOP][op] | Position->pieces[QUEEN][op]) & AllBishopSqs[ksq];
 pieces |= (Position->pieces[ROOK][op] | Position->pieces[QUEEN][op]) & AllRookSqs[ksq];
 pieces &= AllRays[ksq];
 pieces |= KingSqs[ksq] & Position->pieces[PAWN][op];
 hit = kNight ? 1 : 0;
 kAttacks = 0;
 RayAttacks = 0;
 kAttacks |= kNight;
 uint64 BnQ,RnQ;
 BnQ = Position->pieces[BISHOP][op] | Position->pieces[QUEEN][op];
 RnQ = Position->pieces[ROOK][op] | Position->pieces[QUEEN][op];
  while(pieces && hit < 2){ /* no more than two pieces give check at the same time */

   square = NEXTSQ(pieces);
   LOG2(square,sq);
    switch(NPIECE(Position->board[sq])){
     case QUEEN:
       if(AllBishopSqs[sq] & KingBit){
        ray = GetBishopRay(square,KingBit);
        if((ray & allpieces) == KingBit){
         kAttacks |= square;
         kAttacks |= ray;
	 RayAttacks |= kAttacks;
         hit++;
	}
      }else
       if(AllRookSqs[sq] & KingBit){
        ray = GetRookRay(square,KingBit);
        if((ray & allpieces) == KingBit){
         kAttacks |= square;
         kAttacks |= ray;
	 RayAttacks |= kAttacks;
         hit++;
	}
      }
     break;
     case BISHOP:
       if(AllBishopSqs[sq] & KingBit){
        ray = GetBishopRay(square,KingBit);
        if((ray & allpieces) == KingBit){
         kAttacks |= square;
         kAttacks |= ray;
	 RayAttacks |= kAttacks;
         hit++;
	}
      }
     break;
     case ROOK:
       if(AllRookSqs[sq] & KingBit){
        ray = GetRookRay(square,KingBit);
        if((ray & allpieces) == KingBit){
         kAttacks |= square;
         kAttacks |= ray;
	 RayAttacks |= kAttacks; /* Really pieces that attack in rays*/
         hit++;
	}
      }
     break;
     case PAWN:
      if(PwnSqs[op][sq] & KingBit){
       kAttacks |= square;
       hit++;
      }
     break;
    }
    LSBCLEAR(pieces);
  }
 if(hit > 1) /* if it's double check just move the king */
  pieces = KingBit;
 else pieces = Position->empty[turn];
 RayAttacks &= Position->empty[op];
 RayAttacks &= ~kNight;
/*
 printBoardinfo(Position);
 printf("\nhit = %d\n",hit);
 printBitmap(RayAttacks,-1);
 getchar();
//*/
 while(pieces){

	 square = NEXTSQ(pieces);
	 LOG2(square,sq);
	 allmoves  = Position->moves[sq];
/*
	 xray = Xray(KingBit,square,allpieces);
	 //Check if the piece is pinned
	 if(xray & BnQ || xray & RnQ){
	  allmoves &= xray;
	 }*/

	 if(square & KingBit){
	  allmoves &= ~Position->attackd[op]; /* If its king not go to attacked squares*/
	 }
	 else allmoves &= kAttacks;
/*
             if(square & Position->pieces[PAWN][turn] && Position->ep){
                 epBit = BIT(Position->ep);
               if((kAttacks & Position->pieces[PAWN][op]) && Position->moves[sq] & epBit){
	         allmoves |= epBit;
                 printBoardinfo(Position);
                 printf("\nhit = %d\n",hit);
                 printBitmap(epBit,-1);
                 getchar();
               }
	     }//*/

	 while(allmoves){
	  movesq = NEXTSQ(allmoves);
	  LOG2(movesq,msq);

	   if(!(square & KingBit && (AllBishopSqs[msq] & (RayAttacks & BnQ) || AllRookSqs[msq] & (RayAttacks & RnQ)))){

	   if((square & Position->pieces[PAWN][Position->turn]) && (movesq & PROMOTESQMASK)){
	    moves[curr].castle = moves[curr+1].castle = moves[curr+2].castle = moves[curr+3].castle = 0;
	    moves[curr].from = moves[curr+1].from = moves[curr+2].from = moves[curr+3].from = sq;
	    moves[curr].to = moves[curr+1].to = moves[curr+2].to = moves[curr+3].to = msq;
	    moves[curr].promote = QUEEN+promotecolor;
	    moves[curr+1].promote = ROOK+promotecolor;
	    moves[curr+2].promote = BISHOP+promotecolor;
	    moves[curr+3].promote = KNIGHT+promotecolor;
	    curr += 4;

	   }
	   else{
	    moves[curr].castle = moves[curr].promote = 0;
	    moves[curr].from = sq;
	    moves[curr].to = msq;
	    ++curr;
	   }
	 if (curr >= MAXMOVES - 4) return curr;
	   }
	 LSBCLEAR(allmoves);
	}
	LSBCLEAR(pieces);

 }
 return curr;
}

int _GenCheckEscapes(Boardmap * Position, Move moves[]){
 /* TODO: if its attacked by more than one piece must be a king move */
 /* TODO: if its attacked by knight only king moves and captures of the knight */
 /* with a sligth modification this could generate checks */
 int curr;
 char piece,sq,msq,turn,op,ksq,promotecolor = Position->turn ? 6 : 0;
 uint64 pieces,square,allmoves,movesq,kAttacks,kNight,BApieces,RApieces;
 turn = Position->turn,
 op   = Position->turn^1,
 curr = 0;
 pieces = Position->empty[turn];
 LOG2(Position->pieces[KING][turn],ksq);
 kNight = (KnightSqs[ksq] & Position->pieces[KNIGHT][turn^1]);

 BApieces = (Position->pieces[BISHOP][op] | (Position->pieces[PAWN][op] & KingSqs[ksq]) | Position->pieces[QUEEN][op]);
 RApieces = (Position->pieces[ROOK][op] | Position->pieces[QUEEN][op]);

		kAttacks = KingSqs[ksq];

	 if(kNight) /* there is a knight check */
	  kAttacks |= kNight;
	 else{

	 	if(BishopURSqs[ksq] & BApieces)
		 kAttacks |= BishopURSqs[ksq];

	 	if(BishopULSqs[ksq] & BApieces)
		 kAttacks |= BishopULSqs[ksq];

	 	if(BishopDRSqs[ksq] & BApieces)
		 kAttacks |= BishopDRSqs[ksq];

	 	if(BishopDLSqs[ksq] & BApieces)
		 kAttacks |= BishopDLSqs[ksq];

	 	if(RightSqs[ksq] & RApieces)
		 kAttacks |= RightSqs[ksq];

	 	if(LeftSqs[ksq] & RApieces)
		 kAttacks |= LeftSqs[ksq];

	 	if(UpSqs[ksq] & RApieces)
		 kAttacks |= UpSqs[ksq];

	 	if(DownSqs[ksq] & RApieces)
		 kAttacks |= DownSqs[ksq];
	 }

//printBitmap(kAttacks,-1);
 while(pieces){

	 square = NEXTSQ(pieces);
	 LOG2(square,sq);
	 allmoves  = Position->moves[sq];
	 if(square & Position->pieces[KING][turn]) allmoves &= ~Position->attackd[op]; /* If its king not go do atttackerd swuares*/
	 else allmoves &= Position->attackd[op] | Position->empty[op];
	 allmoves &= kAttacks;
	 while(allmoves){
	  movesq = NEXTSQ(allmoves);
	  LOG2(movesq,msq);
	   if((square & Position->pieces[PAWN][Position->turn]) && (movesq & PROMOTESQMASK)){
	    moves[curr].castle = moves[curr+1].castle = moves[curr+2].castle = moves[curr+3].castle = 0;
	    moves[curr].from = moves[curr+1].from = moves[curr+2].from = moves[curr+3].from = sq;
	    moves[curr].to = moves[curr+1].to = moves[curr+2].to = moves[curr+3].to = msq;
	    moves[curr].promote = QUEEN+promotecolor;
	    moves[curr+1].promote = ROOK+promotecolor;
	    moves[curr+2].promote = BISHOP+promotecolor;
	    moves[curr+3].promote = KNIGHT+promotecolor;
	    curr += 4;

	   }
	   else{
	    moves[curr].castle = moves[curr].promote = 0;
	    moves[curr].from = sq;
	    moves[curr].to = msq;
	    ++curr;
	   }
	 if (curr >= MAXMOVES - 4) return curr;
	 LSBCLEAR(allmoves);
	}
	LSBCLEAR(pieces);

 }
 return curr;
}

int Map(Boardmap * Bmap){
 /* COmpletely innefficient. TODO: REMOVE!!*/
	char side,piece,npiece;
	int move,movesq;
	// Think a more efficient way of zeroing the 64 bit (uint64) maps
	memset(Bmap->pieces,0,sizeof(uint64)*6*2);
	memset(Bmap->empty,0,sizeof(uint64)*2);
	memset(Bmap->attackd,0,sizeof(uint64)*2);
	memset(Bmap->moves,0,sizeof(uint64)*64);

	for(int sq = 0; sq < 64; ++sq){

	 piece = Bmap->board[sq];

	 if(piece == EMPTY) continue;
	 side = SIDE(piece);
	 npiece= NPIECE(piece);
		FLIPBIT( sq, Bmap->pieces[npiece][side] );
		FLIPBIT( sq, Bmap->empty[side] );
	 for(int i = 0; AttackTable[npiece][i] != 0; ++i){

		if(npiece == PAWN && side == BLACK)
		move = AttackTable[npiece][i+3]; else
		move = AttackTable[npiece][i];

		   movesq = sq;
		   movesq = MailBox[MailBoxNumber[movesq] + move];
		   while(movesq != -1){
			// movesq += move;
			/* MailBox ... Y U NO WORK!?*/
			/* Challenge accepted */
			/* 10 hours later ...  THIS ... */
			/* Close Enough ...  : |  */
			 FLIPBIT( movesq, Bmap->attackd[side]);
			 FLIPBIT( movesq, Bmap->attacks[sq]);
		         if(npiece != PAWN ){
			 if(Bmap->board[movesq] == EMPTY || SIDE(Bmap->board[movesq]) != side)
			 FLIPBIT( movesq, Bmap->moves[sq]);
			 }
			 else{
			  if((Bmap->board[movesq] != EMPTY && SIDE(Bmap->board[movesq]) != side)
			   || (movesq == Bmap->ep && MYEPSQ(movesq,side)))
			   FLIPBIT( movesq, Bmap->moves[sq]);
 			   char frontsq = FRONTSQ(sq,side);
			   if(Bmap->board[frontsq] == EMPTY){
				FLIPBIT(frontsq,Bmap->moves[sq]);
				frontsq = FRONTSQ(frontsq,side); /*the front of the front sq*/
				if(PAWNSTARTSQ(sq,side) && Bmap->board[frontsq] == EMPTY)
				FLIPBIT(frontsq,Bmap->moves[sq]);
			 }
			 }

			 if(Bmap->board[movesq] != EMPTY) break;
			 if(!LONG_RANGE(npiece)) break;

		   movesq = MailBox[MailBoxNumber[movesq] + move];
		   }

	 }
	}

}
int _fastMap(Boardmap * Bmap, uint64 affectedSqs){ /* Employs shifted boards like Nagasaki chess engine*/
/* TODO: Can be faster?*/
	uint64 Pieces = Bmap->empty[WHITE] | Bmap->empty[BLACK],
	       allPieces = Pieces,
	       empty = ~allPieces,
	       nextpiece,
	       moves,
	       PwnFSq;
	char sq,side,c = 0;
	 Bmap->attackd[WHITE] = Bmap->attackd[BLACK] = 0;
	//printBitmap(affectedSqs,-1);getchar();
	Pieces |= affectedSqs;
	//if (Pieces & ~allPieces) printf("\nEmpty and affected by move\n"),printBitmap(Pieces & ~allPieces,-1),getchar();
	while(Pieces){
	 nextpiece = NEXTSQ(Pieces);
	 LOG2(nextpiece,sq);

	 if(nextpiece & ~allPieces){
	 // printf("\nI'm in default with square: %d\n",sq);getchar();
	 Bmap->moves[sq] = Bmap->attacks[sq] = 0;
	 }
	else{
	 side = SIDE(Bmap->board[sq]);
	 switch(NPIECE(Bmap->board[sq])){
	  case PAWN:
	   if((PwnSqs[side][sq] | PwnFwdSqs[side][sq] | Pwn2StepSqs[side][sq]) & affectedSqs || nextpiece & affectedSqs || BIT(Bmap->ep) & epSqs[side] & PwnSqs[side][sq] || (Bmap->moves[sq] & PwnSqs[side][sq]) & ~allPieces){
	   //c++;
	   Bmap->moves[sq] = Bmap->attacks[sq] =  0;
	    Bmap->moves[sq] = PwnSqs[side][sq] & (Bmap->empty[side^1] | (BIT(Bmap->ep) & epSqs[side]));
	    PwnFSq = PwnFwdSqs[side][sq] & (~allPieces);
/*
	   printf("\nfrom fastMap\n");
	   printBitmap(PwnFwdSqs[side][sq],sq);
	   printf("\n");getchar();
	   printBitmap(allPieces,-1);
	   printf("\n");
	   printBitmap(PwnFSq,-1);
	   printf("\n");getchar();
*/
	    Bmap->moves[sq] |= PwnFSq;
	    Bmap->moves[sq] |= (PwnFSq ? Pwn2StepSqs[side][sq] & (~allPieces): 0);
	    Bmap->attackd[side] |=  Bmap->attacks[sq] = PwnSqs[side][sq];
	   }

	  break;
	  case KNIGHT:
	   if(KnightSqs[sq] & affectedSqs|| nextpiece & affectedSqs){ //c++;
	   Bmap->moves[sq] = Bmap->attacks[sq] =  0;
	    Bmap->moves[sq] = KnightSqs[sq] & ~Bmap->empty[side];
	    Bmap->attackd[side] |= Bmap->attacks[sq] = KnightSqs[sq];
	   }
	  break;
	  case BISHOP:
	   if(Bmap->attacks[sq] & affectedSqs|| nextpiece & affectedSqs){//c++;
	   Bmap->moves[sq] = Bmap->attacks[sq] =  0;
	    BISHOPMOVES(sq,moves,allPieces,Bmap,side);
	   }
	  break;
	  case ROOK:
	   if(Bmap->attacks[sq] & affectedSqs|| nextpiece & affectedSqs){//c++;
	   Bmap->moves[sq] = Bmap->attacks[sq] =  0;
	    ROOKMOVES(sq,moves,allPieces,Bmap,side);
	   }
	  break;
	  case QUEEN:
	   if(Bmap->attacks[sq] & affectedSqs|| nextpiece & affectedSqs){//c++;
	    Bmap->moves[sq] = Bmap->attacks[sq] = 0;
	    BISHOPMOVES(sq,moves,allPieces,Bmap,side);
	    ROOKMOVES(sq,moves,allPieces,Bmap,side);
	   }
	  break;
	  case KING:
	   if(KingSqs[sq] & affectedSqs|| nextpiece & affectedSqs){//c++;
	   Bmap->moves[sq] = Bmap->attacks[sq] =  0;
	    Bmap->moves[sq] = KingSqs[sq] & ~Bmap->empty[side];
	    Bmap->attackd[side] |=  Bmap->attacks[sq] = KingSqs[sq];
	   }
	  break;
	 }
	}
	 Bmap->attackd[side] |= Bmap->attacks[sq];
	 LSBCLEAR(Pieces);
	}
//printf("\nIterated over %d pieces\n",c); getchar();
}


