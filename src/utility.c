/* ut */
/* gv_location: 19501-20000 */
/* *******************************************************************
*  file: utility.c, Utility module.                  Part of DIKUMUD *
*  Usage: Utility procedures                                         *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#if defined (DIKU_WINDOWS)
#include <winsock2.h>
#endif

#include "structs.h"
#include "utility.h"
#include "db.h"
#include "constants.h"
#include "globals.h"
#include "parser.h"
#include "spells.h"
#include "func.h"
#include "comm.h"

#define OLD_HEAD obj_file_u
#define NEW_HEAD obj_file_u
#define OLD_ELEM obj_file_elem
#define NEW_ELEM obj_file_elem

/* creates a random number in interval [from;to] */
int number(int from, int to)
{

	/* We don't to add to the overhead */

	return ((random() % (to - from + 1)) + from);
}
/* simulates dice roll */
int dice(int number, int size)
{
	int r;
	int sum = 0;

	/* We don't to add to the overhead */

	if (size < 1) {
		main_log("ERROR: dice() in utility.c negative size. Default to 1");
		spec_log("ERROR: dice() in utility.c negative size. Default to 1", ERROR_LOG);
		size = 1;
	}

	size = MAXV(1, size);

	for (r = 1; r <= number; r++)
		sum += ((random() % size) + 1);

	return (sum);
}				/* END OF dice() */


/* simulates dice roll */
int xx_dice(int lv_number, int lv_size)
{
	int lv_low, lv_hgh, lv_sum;



	if (lv_number < 1) {
		main_log("ERROR: dice() in utility.c negative lv_number.");
		spec_log("ERROR: dice() in utility.c negative lv_number.", ERROR_LOG);
		lv_number = 1;
	}
	if (lv_size < 1) {
		main_log("ERROR: dice() in utility.c negative size.");
		spec_log("ERROR: dice() in utility.c negative size.", ERROR_LOG);
		lv_size = 1;
	}

	lv_hgh = lv_number * lv_size;
	lv_low = lv_number;
	if (lv_hgh / 2 - lv_low > lv_low) {
		lv_low = lv_hgh / 2 - lv_low;
	}

	lv_sum = number(lv_low, lv_hgh);
	/*
   printf("num=%d, dice=%d, low=%d, hgh=%d, sum=%d\r\n",
					lv_number, lv_size, lv_low, lv_hgh, lv_sum);
	 */
	return (lv_sum);

}				/* END OF dice() */


/* Create a duplicate of a string */
char *str_alloc(char *source)
{

	/* We don't to add to the overhead */

	if (source) {
		return strdup(source);
	}
	else
		return NULL;
}

char *str_free(char *str)
{

	/* We don't to add to the overhead */

	if (str) {
		free(str);
	}
	return NULL;
}


/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int str_cmp(char *arg1, char *arg2)
{

	int chk, i;

	/* We don't to add to the overhead */

	for (i = 0; *(arg1 + i) || *(arg2 + i); i++)
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
			if (chk < 0)
				return (-1);
			else
				return (1);
		}
	return (0);

}				/* END OF str_cmp() */


/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int strn_cmp(char *arg1, char *arg2, int n)
{
	int chk, i;

	/* We don't to add to the overhead */

	for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--)
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
			if (chk < 0)
				return (-1);
			else
				return (1);
		}
	return (0);
}


/* writes a string to the log */
void main_log(char *str)
{
	long ct;
	char *tmstr;
	char *new_str;

	/* don't overwrite location with prints */

	/* Don't double it.        trace_log(str); */
	new_str = strdup(str);
	ct = time(0);
	tmstr = asctime(localtime(&ct));
	/* remove the NL char */
	*(tmstr + strlen(tmstr) - 1) = '\0';
	REMOVE_COLOR(new_str);
	fprintf(stderr, "%s :: %s\r\n", tmstr, new_str);
	free(new_str);
}
/* New log files, Shalira 23.07.01 */
/* writes a string to the log */
/* Define new logs in db.h */
void spec_log(char *str, char *logname)
{
	long ct;
	char *tmstr;
	char *new_str;
	FILE *logfile;
	FILE *mainlogfile;

	ct = time(0);
	tmstr = asctime(localtime(&ct));
	/* remove the NL char */
	*(tmstr + strlen(tmstr) - 1) = '\0';

	new_str = strdup(str);
	REMOVE_COLOR(new_str);

	if ((logfile = fopen(logname, "a")) == NULL) {
		fprintf(stderr, "%s :: ERROR: Cannot open %s\n", tmstr, logname);
		free(new_str);
		return;
	}

	fprintf(logfile, "%s :: %s\r\n", tmstr, new_str);
	fclose(logfile);
	if ((mainlogfile = fopen(MAIN_LOG, "a")) == NULL) {
		fprintf(stderr, "%s :: ERROR: Cannot open %s\n", tmstr, MAIN_LOG);
		free(new_str);
		return;
	}

	fprintf(mainlogfile, "%s :: %s\r\n", tmstr, new_str);
	fclose(mainlogfile);

	free(new_str);
}


/* writes a string to the tracefile log */
void trace_log(char *str)
{
	long ct;
	char *tmstr;
	char *new_str;

	/* don't overwrite location with prints */

	if (!trace_file) {
		main_log("ERROR: tracefile not open\r\n");
		spec_log("ERROR: tracefile not open.", ERROR_LOG);
		return;
	}

	new_str = strdup(str);
	REMOVE_COLOR(new_str);

	ct = time(0);
	tmstr = asctime(localtime(&ct));
	/* remove the NL char */
	*(tmstr + strlen(tmstr) - 1) = '\0';
	fprintf(trace_file, "%s :: %s\r\n", tmstr, new_str);
	fflush(trace_file);

	free(new_str);
	//free(tmstr);
}


void sprintbit(long vektor, const char *names[], char *result)
{
	long nr;



	*result = '\0';

	for (nr = 0; vektor; vektor >>= 1) {
		if (IS_SET(1, vektor)) {
			if (*names[nr] != '\n') {
				strcat(result, names[nr]);
				strcat(result, " ");
			}
			else {
				strcat(result, "UNDEFINED");
				strcat(result, " ");
			}
		}
		if (*names[nr] != '\n')
			nr++;
		/* IF THE LAST BIT WAS USED, DIVIDING BY 2 WONT SHIFT IT */
		if (vektor == -1 || vektor == 1)
			vektor = 0;
	}

	if (!*result)
		strcat(result, "NOBITS ");
}



void sprinttype(int type, const char *names[], char *result)
{
	int nr;



	for (nr = 0; (*names[nr] != '\n'); nr++);
	if (type < nr)
		strcpy(result, names[type]);
	else
		strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries(secs) */
struct time_info_data real_time_passed(time_t t2, time_t t1)
{
	long secs;

	struct time_info_data now;



	secs = (long) (t2 - t1);

	now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_REAL_HOUR * now.hours;

	now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
	secs -= SECS_PER_REAL_DAY * now.day;

	now.month = -1;
	now.year = -1;

	return now;
}				/* END OF read_time_passed() */


/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;



	secs = (long) (t2 - t1);

	now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_MUD_HOUR * now.hours;

	now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
	secs -= SECS_PER_MUD_DAY * now.day;

	now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
	secs -= SECS_PER_MUD_MONTH * now.month;

	now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */
	secs -= SECS_PER_MUD_MONTH * now.year;

	return now;
}				/* END OF mud_time_passed() */


long adjust_time(time_t time_in,
		   int lv_year, int lv_month, int lv_day, int lv_hours)
{
	long lv_current_time, secs;
	int lv_remainder, lv_year_new, lv_month_new, lv_day_new, lv_hours_new;



	lv_current_time = time(0);
	secs = (long) (lv_current_time - time_in);
	lv_year_new = secs / SECS_PER_MUD_YEAR;
	secs = secs - (lv_year_new * SECS_PER_MUD_YEAR);
	lv_month_new = secs / SECS_PER_MUD_MONTH;
	secs = secs - (lv_month_new * SECS_PER_MUD_MONTH);
	lv_day_new = secs / SECS_PER_MUD_DAY;
	secs = secs - (lv_day_new * SECS_PER_MUD_DAY);
	lv_hours_new = secs / SECS_PER_MUD_HOUR;
	lv_remainder = secs - (lv_hours_new * SECS_PER_MUD_HOUR);

	secs = SECS_PER_MUD_HOUR * lv_hours;
	secs += SECS_PER_MUD_DAY * lv_day;
	secs += SECS_PER_MUD_MONTH * lv_month;
	secs += SECS_PER_MUD_YEAR * (lv_year - 17);

	secs = (long) (lv_current_time - secs);
	return (secs);

}				/* END OF adjust_time() */

struct time_info_data z_age(struct char_data * ch)
{
	struct time_info_data player_age;
	
	player_age = mud_time_passed(time(0), ch->player.time.birth);
	player_age.year += 17;	/* All players start at 17 */

	return player_age;
}

void check_reboot(void)
{

	char buf[MAX_STRING_LENGTH];
	time_t lv_time;



	lv_time = time(0);

	gv_terminate_count--;

	if (gv_terminate_count > 30) {
		if (!(gv_terminate_count % 60)) {
			sprintf(buf, "Reboot in %ld hour(s)!\r\n",
				gv_terminate_count / 60);
			co3000_send_to_all(buf);
		}
		return;
	}

	if (gv_terminate_count < 1) {
		co3000_send_to_all("Rebooting system \r\n");
		SET_BIT(gv_run_flag, RUN_FLAG_TERMINATE);
		gv_highest_login_level = IMO_IMM;
		spec_log("Rebooting system.", SYSTEM_LOG);
		return;
	}

	/* WE ARE DOWN TO THE LAST HOUR, LET THEM KNOW EVERY SO OFTEN */
	if (gv_terminate_count == 1) {
		co3000_send_to_all("Reboot in 1 minute!\r\n");
		return;
	}

	if (gv_terminate_count == 5) {
		co3000_send_to_all("Reboot in 5 minutes!\r\n");
		return;
	}

	if (!(gv_terminate_count % 10)) {
		sprintf(buf, "Reboot in %d minutes!\r\n", (int) gv_terminate_count);
		co3000_send_to_all(buf);
		return;
	}

	return;

}				/* END OF check_reboot() */

void do_disable(struct char_data *ch, char *arg, int cmd)
{
	int look_at, number_disabled, temp_cmd;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_ch;

	/* DID WE GET A COMMAND TO DISABLE? */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*buf) {
		number_disabled = 0;
		temp_cmd = 0;
		if (IS_SET(gv_run_flag, RUN_FLAG_DISABLE_FIGHTING)) {
			send_to_char("FIGHTING: DISABLED\r\n", ch);
		}
		else {
			send_to_char("FIGHTING: ENABLED\r\n", ch);
		}
		while (*(command[temp_cmd]) != '\n') {
			if (!(IS_SET(cmd_info[temp_cmd + 1].cmd_flag, ENABLED_CMD))) {
				if (number_disabled == 0)
					send_to_char("Disabled commands:\r\n", ch);
				bzero(buf, sizeof(buf));
				sprintf(buf, "     %s\r\n",
					command[temp_cmd]);
				send_to_char(buf, ch);
				number_disabled++;
			}
			temp_cmd++;
		}		/* END OF WHILE */
		if (number_disabled == 0)
			send_to_char("There aren't any disabled commands.  Which command do you wish to disable?\r\n", ch);
		return;
	}

	/* convert to lower case */
	for (look_at = 0; *(buf + look_at) > 0; look_at++)
		*(buf + look_at) = LOWER(*(buf + look_at));

	/* DO THEY WANT SPELLS? */
	if (!strcmp(buf, "spell")) {
		do_spell_disable(ch, arg, cmd);
		return;
	}

	/* DISABLE ALL COMMANDS? */
	if (!strcmp(buf, "all")) {
		/* IF THE FIRST COMMAND IS ENABLED, WE'LL ASSUME THE */
		/* ALL MEANS TO DISABLE ALL COMMANDS                 */
		if (IS_SET(cmd_info[1].cmd_flag, ENABLED_CMD)) {
			for (temp_cmd = 1; temp_cmd < MAX_CMD_LIST; temp_cmd++) {
				REMOVE_BIT(cmd_info[temp_cmd].cmd_flag, ENABLED_CMD);
			}
			/* MAKE SURE DISABLE COMAND IS ACTIVE */
			SET_BIT(cmd_info[CMD_DISABLE].cmd_flag, ENABLED_CMD);
			SET_BIT(cmd_info[CMD_HELP].cmd_flag, ENABLED_CMD);
			send_to_char("All commands disabled.\r\n", ch);
		}
		else {
			/* ENABLE ALL COMMANDS */
			for (temp_cmd = 1; temp_cmd < MAX_CMD_LIST; temp_cmd++) {
				SET_BIT(cmd_info[temp_cmd].cmd_flag, ENABLED_CMD);
			}
			send_to_char("All commands enabled.\r\n", ch);
		}
		return;
	}			/* END OF disable ALL */

	/* DISABLE FIGHTING? */
	if (!strcmp(buf, "fighting")) {
		if (IS_SET(gv_run_flag, RUN_FLAG_DISABLE_FIGHTING)) {
			REMOVE_BIT(gv_run_flag, RUN_FLAG_DISABLE_FIGHTING);
			send_to_char("Fighting enabled.\r\n", ch);
		}
		else {
			SET_BIT(gv_run_flag, RUN_FLAG_DISABLE_FIGHTING);
			while ((tmp_ch = combat_list)) {
				ft1400_stop_fighting(tmp_ch, 0);
				send_to_char("All fighting has been temporarily suspended.\r\n", tmp_ch);
			}
			send_to_char("Fighting disabled.\r\n", ch);
		}
		return;
	}			/* END OF disable fighting */

	/* find command */
	temp_cmd = old_search_block(buf, 0, look_at, command, 0);

	if (temp_cmd < 1) {
		send_to_char("I couldn't find that command.\r\n", ch);
		return;
	}

	if (temp_cmd == CMD_DISABLE) {
		send_to_char("I'm sorry but that is one command you can't disable.\r\n", ch);
		return;
	}

	if ((IS_SET(cmd_info[temp_cmd].cmd_flag, ENABLED_CMD))) {
		REMOVE_BIT(cmd_info[temp_cmd].cmd_flag, ENABLED_CMD);
		bzero(buf, sizeof(buf));
		sprintf(buf, "Command %s disabled.\r\n",
			command[temp_cmd - 1]);
		send_to_char(buf, ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has disabled command %s.",
			GET_NAME(ch), command[temp_cmd - 1]);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, SYSTEM_LOG);
		spec_log(buf, GOD_COMMAND_LOG);
	}
	else {
		SET_BIT(cmd_info[temp_cmd].cmd_flag, ENABLED_CMD);
		bzero(buf, sizeof(buf));
		sprintf(buf, "Command %s enabled.\r\n",
			command[temp_cmd - 1]);
		send_to_char(buf, ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has enabled command %s.",
			GET_NAME(ch), command[temp_cmd - 1]);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, SYSTEM_LOG);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	return;

}				/* END OF do_disable() */


void do_spell_disable(struct char_data * ch, char *arg, int cmd)
{

	int lv_spell, tmp_spell, lv_number_disabled;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* ************ */
	/* SKILL TYPE  */
	/* ************ */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*buf) {
		lv_number_disabled = 0;
		tmp_spell = 1;
		while (*(spell_names[tmp_spell]) != '\n') {
			if (spell_info[tmp_spell].spell_pointer &&
			    !IS_SET(spell_info[tmp_spell].spell_flag, ENABLED_SPELL)) {
				if (lv_number_disabled == 0)
					send_to_char("Disabled spells/skills:\r\n", ch);
				bzero(buf, sizeof(buf));
				sprintf(buf, "     %s\r\n",
					spell_names[tmp_spell - 1]);
				send_to_char(buf, ch);
				lv_number_disabled++;
			}
			tmp_spell++;
		}		/* END OF WHILE */

		if (lv_number_disabled == 0)
			send_to_char("There aren't any disabled spells.  Which spell do you wish to disable?\r\n", ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (is_number(buf)) {
		lv_spell = atoi(buf);
	}
	else {
		lv_spell = old_search_block(buf, 0, strlen(buf),
					    spell_names, 0);
		if (lv_spell < 0) {
			sprintf(buf2, "Sorry, but spell type: %s is invalid.\r\n",
				buf);
			send_to_char(buf2, ch);
			send_to_char("Format: disable spell <spell>.\r\n", ch);
			return;
		}
	}

	/* IS SKILL IN CORRECT RANGE? */
	if (lv_spell < 1 || lv_spell >= MAX_SKILLS) {
		sprintf(buf, "Spell must be between 0 and %d.\r\n",
			MAX_SKILLS);
		send_to_char(buf, ch);
		return;
	}


	bzero(buf, sizeof(buf));
	if (IS_SET(spell_info[lv_spell].spell_flag, ENABLED_SPELL)) {
		REMOVE_BIT(spell_info[lv_spell].spell_flag, ENABLED_SPELL);
		sprintf(buf, "Disabling spell %s.\r\n",
			spell_names[lv_spell - 1]);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has disabled spell %s.",
			GET_NAME(ch), spell_names[lv_spell - 1]);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, SYSTEM_LOG);
		spec_log(buf, GOD_COMMAND_LOG);
	}
	else {
		SET_BIT(spell_info[lv_spell].spell_flag, ENABLED_SPELL);
		sprintf(buf, "Enabling spell %s.\r\n",
			spell_names[lv_spell - 1]);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has enabled spell %s.",
			GET_NAME(ch), spell_names[lv_spell - 1]);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, SYSTEM_LOG);
		spec_log(buf, GOD_COMMAND_LOG);

	}
	send_to_char(buf, ch);

	return;

}				/* END OF do_spell_disable() */


int ut9000_update_player_file(struct char_data * ch, char *arg, int cmd)
{

	FILE *fl_player;
	struct char_file_u dummy;
	char buf[MAX_STRING_LENGTH];

	int obj_loc[32767], rc, idx;
	rc = 0;

	/* DID THEY APPLY THE REBUILD EQUIPMENT SWITCH? */
	if (!IS_SET(gv_run_flag, RUN_FLAG_REBUILD_EQUIPMENT)) {
		send_to_char("To use this option, you  must use the -E switch when starting program.", ch);
		return (rc);
	}

	/* Load obj_loc array with virtual numbers so we can */
	/* locate objects faster                             */
	for (idx = 0; idx < 32767; idx++) {
		obj_loc[idx] = 0;
	}
	for (idx = 0; idx < top_of_objt && idx < 32767; idx++) {
		obj_loc[obj_index[idx].virtual] = idx;
	}

	fl_player = fopen(PLAYER_FILE, "rb+");
	if (!fl_player) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: unable to open player index %d.\r\n",
			errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	idx = 0;
	fread(&dummy, sizeof(struct char_file_u), 1, fl_player);
	while (!feof(fl_player)) {
		if (*dummy.name != 0 &&	/* THROW OUT NULLS   */
		    *dummy.name != 32 &&	/* THROW OUT BLANKS  */
		    *dummy.name != '1' &&	/* THROW OUT DELETES */
		    *dummy.name != '2') {	/* THROW OUT SELF DELETS */
			idx++;
			printf("Read char: %s\r\n", dummy.name);
			rc = ut9500_rebuild_equipment_file(dummy.name, obj_loc);
		}
		fread(&dummy, sizeof(struct char_file_u), 1, fl_player);
	}
	fclose(fl_player);

	send_to_char("Finished rebuilding equipment\r\n", ch);
	bzero(buf, sizeof(buf));
	sprintf(buf, "Processed %d players\r\n", idx);
	send_to_char(buf, ch);
	return (rc);

}				/* END OF ut9000_update_player_file() */


int ut9500_rebuild_equipment_file(char *lv_name, int obj_loc[32767])
{

	FILE *file_in, *file_out;
	int idx, size_of_old_head, size_of_old_elem, size_of_new_head, size_of_new_elem, lv_eqp_loop, number_written, rc;

	char *ptr1, *ptr2, buf[MAX_STRING_LENGTH], filename[256];

	struct obj_data *tmp_obj;

	struct OLD_HEAD old_head;
	struct OLD_ELEM old_elem;
	struct NEW_HEAD new_head;
	struct NEW_ELEM new_elem;

	size_of_old_head = sizeof(struct OLD_HEAD);
	size_of_old_elem = sizeof(struct OLD_ELEM);
	size_of_new_head = sizeof(struct NEW_HEAD);
	size_of_new_elem = sizeof(struct NEW_ELEM);

	number_written = 0;

	if (lv_name[0] >= 'a' && lv_name[0] <= 'z')
		lv_name[0] -= 32;

	/* GET EQUIPMENT FILE NAME */
	rc = r1000_rent_get_filename(lv_name, filename);
	if (!rc) {
		return (rc);
	}

	/* OPEN FILE */
	file_in = fopen(filename, "rb");
	if (!file_in) {
		rc = errno;
		return (rc);
	}

	/* READ HEADER */
	ptr1 = (char *) &old_head;
	bzero(ptr1, size_of_old_head);
	fread(&old_head, size_of_old_head, 1, file_in);
	if (feof(file_in)) {
		printf("ERROR: Blank or invalid equipment header\r\n");
		fclose(file_in);
		return (0);
	}

	/* READ FIRST EQUIPMENT RECORD FROM FILE */
	ptr1 = (char *) &old_elem;
	bzero(ptr1, size_of_old_elem);
	fread(&old_elem, size_of_old_elem, 1, file_in);
	if (feof(file_in)) {
		printf("ERROR: Eqp file with only header - no eqp.\r\n");
		fclose(file_in);
		return (0);
	}

	/* OPEN NEW RENT FILE */
	file_out = fopen(filename, "wb+");
	if (!file_out) {
		bzero(buf, sizeof(buf));
		rc = errno;
		sprintf(buf, "ERROR: %d opening equipment file %s.\r\n",
			rc, filename);
		printf(buf);
		fclose(file_in);
		return (rc);
	}

	/* SET UP NEW HEADER */
	ptr1 = (char *) &new_head;
	bzero(ptr1, size_of_new_head);
	strncpy(new_head.owner, old_head.owner, 19);
	new_head.total_cost = old_head.total_cost;
	new_head.last_update = old_head.last_update;
	new_head.flags = old_head.flags;

	/* WRITE NEW HEADER */
	fwrite(&new_head, size_of_new_head, 1, file_out);
	rc = errno;
	if (rc > 0 && rc != 2) {
		printf("Error %d writing eqp head file %s.\r\n",
		       rc, filename);
		fclose(file_in);
		fclose(file_out);
		return (rc);
	}
	number_written++;

	for (lv_eqp_loop = 0; (!feof(file_in)); lv_eqp_loop++) {

		/* MOVE STUFF FROM OLD STRUCT TO NEW STRUCT */
		ptr1 = (char *) &old_elem;
		ptr2 = (char *) &new_elem;

		/* MOVE DATA FROM OLD STRUCT TO NEW STRUCT */
		memcpy(ptr2, ptr1, size_of_new_elem);

		/* IF WE DON'T HAVE AN OBJECT (its zero) AND ITS NOT THE */
		/* WEDDING RING, ITS INVALID */
		if (obj_loc[new_elem.item_number] == 0 &&
		    new_elem.item_number != 0) {
			bzero(buf, sizeof(buf));
			sprintf("%s has invalid equipment %d Default to 0.\r\n",
				lv_name, new_elem.item_number);
			new_elem.item_number = 0;
		}

		/* LOAD PROTOTYPE */
		tmp_obj = 0;
		tmp_obj = db5100_read_object(new_elem.item_number, PROTOTYPE);
		if (!tmp_obj) {
			bzero(buf, sizeof(buf));
			sprintf("%s has unknown equipment %d Default to 0.\r\n",
				lv_name, new_elem.item_number);
			new_elem.item_number = 0;
			tmp_obj = db5100_read_object(new_elem.item_number, PROTOTYPE);
		}

		/* MAKE CHANGES */
		new_elem.value[0] = tmp_obj->obj_flags.value[0];
		new_elem.value[1] = tmp_obj->obj_flags.value[1];
		new_elem.value[2] = tmp_obj->obj_flags.value[2];
		new_elem.value[3] = tmp_obj->obj_flags.value[3];
		new_elem.flags1 = tmp_obj->obj_flags.flags1;
		new_elem.flags2 = tmp_obj->obj_flags.flags2;
		new_elem.timer = tmp_obj->obj_flags.timer;
		new_elem.bitvector = tmp_obj->obj_flags.bitvector;
		for (idx = 0; idx < MAX_OBJ_AFFECT; idx++) {
			new_elem.affected[idx].location =
				tmp_obj->affected[idx].location;
			new_elem.affected[idx].modifier =
				tmp_obj->affected[idx].modifier;
		}

		fwrite(&new_elem, size_of_new_elem, 1, file_out);
		number_written++;
		rc = errno;
		if (rc > 0 && rc != 2) {
			printf("Error %d writing eqp elem file %s.\r\n",
			       rc, filename);
			fclose(file_in);
			fclose(file_out);
		}

		ptr1 = (char *) &old_elem;
		bzero(ptr1, size_of_old_elem);
		fread(&old_elem, size_of_old_elem, 1, file_in);

	}			/* END OF for loop to read old equipment */

	fclose(file_in);
	fclose(file_out);

	return (0);

}				/* END OF ut9500_rebuild_equipment_file() */



/* check_class_race is used to see if a class/race combo is valid */
int check_class_race(int class, int race)
{


	//04.13 .05
	// 1 mage
	// 2 cleric
	// 3 thief
	// 4 warrior
	// 5 bard
	// 6 priest
	// 7 paladin
	// 8 druid
	// 9 eldritch knight
	// 10 monk

	switch (race) {
	case 1://Human
		return 1;
	case 2:
		//Dwarf
		return 1;
		//Old Class Restrictions
		/*	switch (class) {
		case 1:
		case 5:
		case 8:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 3:
		//Gnome
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 4:
		//Halfling
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 5:
		//Half Giant
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 2:
		case 6:
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 6:
		//Pixie
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 4:
		case 7:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 7:
		//Demon
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 6:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 8:
		//Snotling
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 4:
		case 7:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 9:
		//Feline
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 10:
		//Troll
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 2:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 11:
		//Dragon Kin
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
		case 5:
		case 7:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 12:
		//Centaur
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 13:
		//Merman
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 5:
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 14:
		//High Elf
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
			return 0;
		default:
			return 1;
		}
		*/
	case 15:
		//Half Elf
			return 1;
	case 16:
		//Drow Elf
			return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 17:
		//Orc
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 7:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 18:
		//Half Orc
		return 1;
	case 19:
		//Darkling
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 20:
		//Ogre
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 21:
		//Goblin
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 4:
		case 6:
		case 7:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 22:
		//Minotaur
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 3:
		case 5:
		case 6:
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 23:
		//Bugbear
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		//case 4:
		//case 7:
		case 8:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 24:
		//Giant
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 25:
		//Ravshi
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 2:
		case 4:
		case 5:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 26:
		//Irda
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
		case 7:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 27:
		//Thri - Kreen
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 28:
		//Bullywug
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 6:
		case 7:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 29:
		//Gargoyle
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 2:
		case 5:
		case 6:
		case 7:
		case 8:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	case 30:
		//Imp
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 6:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 31:
		//Saurian
		return 1;
	case 32:
		//Lizardman
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 1:
		case 5:
		case 6:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 33:
		//Sahaugin
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
		case 5:
		case 7:
		case 8:
			return 0;
		default:
			return 1;
		}
		*/
	case 34:
		//Satyr
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 3:
		case 6:
		case 9:
			return 0;
		default:
			return 1;
		}
		*/
	case 35:
		//Yuan - Ti
		return 1;
		//Old Class Restrictions
		/*
			switch (class) {
		case 5:
		case 6:
		case 7:
		case 10:
			return 0;
		default:
			return 1;
		}
		*/
	default:
		return 0;
	}
}
#if defined (DIKU_WINDOWS)
void bzero(void *buf, int length)
{
	while (length > 0) {
		length--;
		*((char *) buf + length) = '\0';
	}
}


char *crypt(char *source, char *target)
{
	return source;
}

char *index(char *name, char search)
{
	return strchr(name, search);
}

void gettimeofday(struct timeval * t, struct timezone * dummy)
{
	unsigned int millisec = GetTickCount();

	t->tv_sec = (int) (millisec / 1000);
	t->tv_usec = (millisec % 1000) * 1000;
}

int random(void)
{
	return rand();
}
void srandom(unsigned int seed)
{
	srand(seed);
}
int strcasecmp(char *a, char *b)
{
	return stricmp(a, b);
}

int strncasecmp(char *a, char *b, size_t c)
{
	return strnicmp(a, b, c);
}
#endif

/* Taken from CircleMUD http://www.circlemud.org -relic */
/* Basically just create files that aren't there.  (Or, that's what we'll use it for.) */
int touch(const char *path)
{
	FILE *fl;
	char buf[MAX_STRING_LENGTH];

	if (!(fl = fopen(path, "a"))) {
		sprintf(buf, "SYSERR: %s: %s", path, strerror(errno));
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (-1);
	}
	else {
		fclose(fl);
		return (0);
	}
}
