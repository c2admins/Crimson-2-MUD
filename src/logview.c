/* *********************************************************************
*  file: logview.c , special module.               Part of crimsonMUD *
*  Usage: Procedures for viewing and searching the logs online        *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
*  Shalira 26.07.01                                                   *
********************************************************************* */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "constants.h"
#include "ansi.h"
#include "func.h"
#include "globals.h"

/* Log view is originaly by Tyler Barnes<tyler.barnes@crcn.net>
* Who put it up on the Circlemud site. All credits goes to him
*
* Modified to work with C2 code by Shalira at 26.07.01
*
*/

struct log_struct {
	char *name;
	const char *path;
	int minlev;
};

struct log_struct loginf[] = {
	{"mainlog", MAIN_LOG, IMO_IMP},
	{"mainlog1", MAIN_LOG1, IMO_IMM},
	{"mainlog2", MAIN_LOG2, IMO_IMM},
	{"steals", STEAL_LOG, IMO_IMM},
	{"firsthit", FIRSTHIT_LOG, IMO_IMM},
	{"pkill", PKILL_LOG, IMO_IMM},
	{"badpws", WRONG_PW_LOG, IMO_IMM},
	{"dts", DT_LOG, IMO_IMM},
	{"errors", ERROR_LOG, IMO_IMP},
	{"godcmds", GOD_COMMAND_LOG, IMO_IMP},
	{"connect", CONNECT_LOG, IMO_IMP},
	{"new", NEW_PLAYER_LOG, IMO_IMM},
	{"delete", DELETE_LOG, IMO_IMM},
	{"system", SYSTEM_LOG, IMO_IMP},
	{"deaths", DEATH_LOG, IMO_IMM},
	{"purges", PURGE_LOG, IMO_IMM},
	{"equipment", EQUIPMENT_LOG, IMO_IMM},
	{"nohelp", NO_HELP_LOG, IMO_IMM},
	{"spells", SPELL_LOG, IMO_IMM},
	{"fun", FUN_LOG, IMO_IMP},
	{"bounty", BOUNTY_LOG, IMO_IMM},
	{"\0", 0, 0}
};

void do_logview(struct char_data * ch, char *arg, int cmd)
{

	char shname[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	unsigned long l = 0, match = 0, start = 0, lines = 15;
	char *temp;
	//struct log_struct loginf;
	FILE *f;
	int i;

	temp = one_argument(one_argument(arg, shname), buf);

	if (!*arg) {
		send_to_char("\r\n", ch);
		for (i = 0; *loginf[i].name; i++)
			if (GET_LEVEL(ch) >= loginf[i].minlev) {
				sprintf(buf1, "%-12s    [ %-20s ]\r\n", loginf[i].name, loginf[i].path);
				send_to_char(buf1, ch);
			}
		return;
	}



	if ((*buf) && (!isdigit(*buf))) {
		send_to_char("syntax is: logview <file alias> [the number of lines to display]\r\n", ch);
		send_to_char("where <file alias> can be:\r\n", ch);
		for (i = 0; *loginf[i].name; i++)
			if (GET_LEVEL(ch) >= loginf[i].minlev) {
				sprintf(buf1, "%-12s    [ %-20s ]\r\n", loginf[i].name, loginf[i].path);
				send_to_char(buf1, ch);
			}
		return;
	}
	else if (*buf)
		lines = atol(buf);

	for (i = 0; *loginf[i].name && !is_abbrev(shname, loginf[i].name); i++);

	if (!*loginf[i].name)
		send_to_char("No such log file.\r\n", ch);
	else if (GET_LEVEL(ch) < loginf[i].minlev)
		send_to_char("You don't have access to that log file.\r\n", ch);
	else if (!(f = fopen(loginf[i].path, "r")))
		send_to_char("Error opening log file.\r\n", ch);
	else {
		/* count lines */
		do {
			l += get_line(f, buf);
		} while (!feof(f));
		rewind(f);

		/* sync if necessary */
		if (lines > l)
			l = 1;
		else {
			start = (l - lines) + 1;
			for (l = 1; l < start; l++)
				get_line(f, buf);

		}
		l = 1;

		for (; match < lines; l++) {

			get_line(f, buf);

			if (feof(f))
				break;

			sprintf(buf1, "[%ld] %s\r\n", l, buf);
			send_to_char(buf1, ch);
			match++;


		}
		if (!match)
			send_to_char("No lines in Log file.\r\n", ch);

		fclose(f);

	}

}
/* Log search is originaly by Tyler Barnes<tyler.barnes@crcn.net>
* Who put it up on the Circlemud site. All credits goes to him
*
* Modified to work with C2 code by Shalira at 26.07.01
*
*/
void do_logsearch(struct char_data * ch, char *arg, int cmd)
{

	char shname[MAX_INPUT_LENGTH], searchstr[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	unsigned long l = 0, match = 0, lines = 15;
	//char *temp;
	FILE *f;
	int i;

	if (!*arg) {
		send_to_char("\r\n", ch);
		for (i = 0; *loginf[i].name; i++)
			if (GET_LEVEL(ch) >= loginf[i].minlev) {
				sprintf(buf, "%-12s    [ %-20s ]\r\n", loginf[i].name, loginf[i].path);
				send_to_char(buf, ch);
			}
		return;
	}

	//temp = one_argument(one_argument(arg, shname), buf);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, shname);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, searchstr);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (isdigit(*buf))
		lines = atoi(buf);

	/*
	 if (!isdigit(*buf))
	 half_chop(arg, shname, searchstr);
	 else {
		 lines = atol(buf);
		 //        skip_spaces(&temp);
		 strcpy(searchstr, temp);
	 } */

	if (!*searchstr)
		send_to_char("Please, at minimum, specify a valid log name and a search string.\r\n", ch);
	else {

		for (i = 0; *loginf[i].name && !is_abbrev(shname, loginf[i].name); i++);

		if (!*loginf[i].name)
			send_to_char("No such log file.\r\n", ch);
		else if (GET_LEVEL(ch) < loginf[i].minlev)
			send_to_char("You don't have access to that log file.\r\n", ch);
		else if (!(f = fopen(loginf[i].path, "r")))
			send_to_char("Error opening log file.\r\n", ch);
		else {


			l = 1;

			for (; match < lines; l++) {

				get_line(f, buf);

				if (feof(f))
					break;

				if (str_stra(buf, searchstr)) {
					sprintf(buf1, "[%ld] %s\r\n", l, buf);
					send_to_char(buf1, ch);
					match++;
				}

			}
			if (!match)
				send_to_char("No lines matched your criteria.\r\n", ch);

			fclose(f);
		}
	}
}
/*
 * get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 *
 * Returns the number of lines advanced in the file.
 *
 * Part of CircleMUD
 *
 * Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University
 * CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.
 *
 */
int get_line(FILE * fl, char *buf)
{
	char temp[256];
	int lines = 0;
	int sl;

	do {
		if (!fgets(temp, 256, fl))
			return (0);
		lines++;
	} while (*temp == '*' || *temp == '\n');

	/* Last line of file doesn't always have a \n, but it should. */
	sl = strlen(temp);
	if (sl > 0 && temp[sl - 1] == '\n')
		temp[sl - 1] = '\0';

	strcpy(buf, temp);
	return (lines);
}
/*
 * str_stra: a case-insensitive version of strstr().
 *
 * Part of CircleMUD
 *
 * Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University
 * CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.
 *
 * Modified (very litle) by Shalira
 */

char *str_stra(const char *str, const char *txt)
{

	int chk = 0, i;
	char buf[MAX_STRING_LENGTH];

	if (str == NULL || txt == NULL) {
		sprintf(buf, "SYSERR: str_stra() passed a NULL pointer, %p or %p.", str, txt);
		spec_log(buf, SYSTEM_LOG);
	}
	else if (strlen(str) >= strlen(txt)) {

		for (i = 0; str[i]; i++) {
			if (LOWER(txt[chk]) == LOWER(str[i]))
				chk++;
			else {
				i -= chk;
				chk = 0;
			}
			if (chk == strlen(txt))
				return ((char *) (str) + i - chk + 1);
		}
	}

	return (NULL);
}
