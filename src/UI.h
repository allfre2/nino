/*--------------------------------------------------------------*
 | Engine nam - UI.h - User Interface, command parser, options, |
 | arguments, etc ... symbolic declarations.                    |
 | License ++++++++++++++++++++++++                             |
 | posibly GPL or just put the code in the public domain        |
 | written by: Alfredo Luzon                                    |
 | Contact info: @hotmail.com, @gmail.com, website.com          |
 | last modifications by <guy #n> : <modifications>             |
 *--------------------------------------------------------------*/
#ifndef CHESS_UI_H
#define CHESS_UI_H
#endif
#define MAXALIASES 8
#define MAXCMDLEN 50 

enum {
	NOARGS,
	TAKESARGS,
	FLAG,
	OTHER /* ?? */

}CommandType;



typedef struct Command{

	char name[MAXALIASES][MAXCMDLEN];
	char type;

}Command;

extern const Command Arguments[];
extern const Command Commands[];
extern const Command XboardCommands[];
extern const Command UCICommands[];
int ParseArgs(int , char**);
int ParseCommand(char *);

