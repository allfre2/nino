#ifndef TM_H
#define TM_H
#endif

typedef struct
{
	double time;
	double increment;
	double available_time;

} TimeControl;

int ManageTime(Game *, TimeControl *);
