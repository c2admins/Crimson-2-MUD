/* sg */
/* gv_location: 15001-15500 */
/* *******************************************************************
*  file: signals.c , trapping of signals from Unix.  Part of DIKUMUD *
*  Usage : Signal Trapping.                                          *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <signal.h>
#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <strings.h>

#include "structs.h"
#include "utility.h"
#include "signals.h"
#include "func.h"
#include "globals.h"
#include "db.h"

void logsig(int lv_signal_number);
void warning_signal(int i);

#if defined (DIKU_WINDOWS)
void signal_setup(void)
{
	return;
}
#else
void signal_setup(void)
{
	/* look for defines in /usr/include/sys/signal.h */
	signal(SIGBUS, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, stop_sig);
	signal(SIGALRM, infinate_loop_check);
	signal(SIGTERM, stop_sig);
	signal(52, terminate_request);
	signal(53, warning_signal);
}
#endif

void warning_signal(int i)
{
	char buf[MAX_INPUT_LENGTH];



	co3000_send_to_all("Reboot coming up in 5 minutes.\r\n");

	bzero(buf, sizeof(buf));
	sprintf(buf, "Recieved signal %d. Shutting down in 5 min.\r\n", i);
	printf(buf);
	return;

}/* END OF warning_signal() */

void terminate_request(int i)
{
	char buf[MAX_INPUT_LENGTH];

	co3000_send_to_all("Operations is shutting down the computer\r\n");

	bzero(buf, sizeof(buf));
	sprintf(buf, "Recieved signal %d.  Shutting down(1).\r\n", i);
	printf(buf);
	main_log(buf);
	spec_log(buf, SYSTEM_LOG);
	wi4100_do_crashsave(0, 0, 0);
	gv_terminate_count = 1;
	return;
}/* END OF terminate_request() */

/* kick out players etc */
void stop_sig(int i)
{
	char buf[MAX_INPUT_LENGTH];

	wi4100_do_crashsave(0, 0, 0);
	bzero(buf, sizeof(buf));
	sprintf(buf, "Recieved signal %d.  Shutting down(2).\r\n", i);
	printf(buf);
	main_log(buf);
	perror(buf);
	ABORT_PROGRAM();
}

void infinate_loop_check(int lv_signal_number)
{
	char buf[MAX_INPUT_LENGTH];
	bzero(buf, sizeof(buf));
	sprintf(buf, "Signal %d received. time var: %d.",
		lv_signal_number, (int) gv_infinate_loop_count);
	main_log(buf);

	bzero(buf, sizeof(buf));
	sprintf(buf, "GVLOC1: %d  GVLOC2: %d  GVLOC3: %d",
		gv_location1, gv_location2, gv_location3);
	main_log(buf);

	bzero(buf, sizeof(buf));
	sprintf(buf, "GVLOC4: %d  GVLOC5: %d  GVLOC6: %d",
		gv_location4, gv_location5, gv_location6);
	main_log(buf);

	bzero(buf, sizeof(buf));
	sprintf(buf, "GVLOC7: %d  GVLOC8: %d  GVLOC9: %d",
		gv_location7, gv_location8, gv_location9);
	main_log(buf);
	bzero(buf, sizeof(buf));
	sprintf(buf, "GVLOC10: %d  GVLOC11: %d  GVLOC12: %d",
		gv_location10, gv_location11, gv_location12);
	main_log(buf);
	/* IF WE GOT HERE, GAME DIDN't RESET ALARM TIME SO ITS IN A LOOP.
	 * LETS GO FATAL---- YIPPY YAY */
	printf("Aborting from infinate loop\r\n");
	trace_log("Aborting from infinate loop\r\n");
	main_log("Aborting from infinate loop\r\n");

	sprintf(buf, "ERROR: Aborting from infinate loop\r\n");
	ABORT_PROGRAM();

	/* SHOULD NEVER MAKE IT HERE... */
	sleep(120);
	assert(0);
	exit(1);
	abort();
	return;
}/* END OF infinate_loop_check() */

void logsig(int lv_signal_number)
{
	char buf[MAX_INPUT_LENGTH];

	bzero(buf, sizeof(buf));
	sprintf(buf, "Signal %d received. Ignoring.", lv_signal_number);
	main_log(buf);

}
