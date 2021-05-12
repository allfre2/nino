/* Time Control functions */

#include "common.h"
#include "tm.h"
#include "time.h"

int ManageTime(Game *game, TimeControl *Time)
{
    /* Allocate time for making a move */
    game->available_time = clock() + (5 * CLOCKS_PER_SEC);

    return 0;
}

void Reset(StopWatch *stopWatch)
{
    stopWatch->start = 0;
    stopWatch->end = 0;
    stopWatch->elapsed = 0;
}

void Start(StopWatch *stopWatch) 
{
    stopWatch->start = clock();
    stopWatch->end = 0;
    stopWatch->elapsed = 0;
}

void Stop(StopWatch *stopWatch)
{
    stopWatch->end = clock();
    stopWatch->elapsed = (stopWatch->end - stopWatch->start) / (CLOCKS_PER_SEC);
}
