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
