#ifndef TM_H
#define TM_H
#endif

typedef struct {

	double time;
	double increment;
	double available_time;

}TimeControl;

/* Must (QUICKLY) return fraction of the time based on the situation on the board and available time, time controls ,etc*/
int ManageTime(Game *, TimeControl *);
