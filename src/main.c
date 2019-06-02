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

int main(int argc, char **argv)
{
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
