#include "common.h"
#include "UI.h"


const Command Arguments[] = {
	{{"","-h","--help",0}, NOARGS},
	{{"","","--xboard",0}, NOARGS},
	{{"","","--uci",0}, NOARGS},

	{{"","-d","--depth",0}, TAKESARGS},
	{{"","--ttsize","--TTSIZE",0},TAKESARGS},

	{{"b","B","--use-book", "--no-book",0},FLAG},
	{{"d","D","--debug-on", "--debug-off",0},FLAG},
	{{"t","T","--use-transposition-table", "--no-transposition-table",0},FLAG},
	{{"p","P","--pondering-on", "--pondering-off",0},FLAG},
	{{"v","V","--show-pv","",},FLAG}
};

const Command Commands[] = {
	{{"go"},NOARGS},
	{{"help"},NOARGS}
};

const Command XboardCommands[] = {
	{{"go"},NOARGS},
	{{"help"},NOARGS}
};

const Command UCICommands[] = {
	{{"go"},NOARGS},
	{{"help"},NOARGS}
};

/* NOARGS type Commands: 
-->
	They don't have a short alias and do a single independent task
	(might not have a second alias either)
*/
