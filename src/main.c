/*--------------------------------------------------------------*
 | Engine name - main.c - program entry point.                  |
 | "Engine name" Is a chess engine that was written with bleh   |
 | objectives its caracteristics are blahblah and its advantages|
 | are bladablahda blah. Documentation on 'X' file.             |
 | Parameters:                                                  |
 | --uci	Enters UCI mode for use with a GUI              |
 | --debug	Very verbose mode for debug purposes            |
 | License                                                      |
 | Written by: Alfredo Luzon                                    |
 | Contact info: @hotmail.com, @gmail.com, website.com          |
 | last modifications by: <guy #n> : <modifications>            |
 *--------------------------------------------------------------*/

/* Celullar division in a few steps */

/*
  o
  O
  0
  8
  oo
  OO
  00
  88
 oo oo
 OO OO
 00 00
 88 88
oooooooo

*/

#include <stdio.h>
#include "common.h"
#include "gen.h"
#include "debug.h"
#include "hash.h"
#include "thread.h"
#include "UI.h"
#include "tt.h"
#include <string.h>
	#include "xboard.h"
int ponder = 0;
int xboard = 0;
int ttSize;
int debug;

int main(int argc, char ** argv){
debug = 0;
TTable = NULL;
printf("feature sigint=0\n");
initBitmaps();
InitRandomTable(1);
 if (xboard)
  Xboard();
 else
  debugFramework();
}
