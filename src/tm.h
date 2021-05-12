#ifndef TM_H
#define TM_H
#endif

typedef struct
{
	double time;
	double increment;
	double available_time;

} TimeControl;

typedef struct
{
	double start;
	double end;
	double elapsed;

} StopWatch;

int ManageTime(Game *, TimeControl *);
void Reset(StopWatch *);
void Start(StopWatch *);
void Stop(StopWatch *);