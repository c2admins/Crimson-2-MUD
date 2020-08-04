/* md */
/* ********************************************************************
*  file: modify.c                                     Part of DIKUMUD *
*  Usage: Run-time modification (by users) of game variables          *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform.  *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "globals.h"
#include "modify.h"
#include "constants.h"
#include "func.h"
#include "spells.h"
#include "ansi.h"

#define SET_SKILL_FORMAT "format: set <char> skill <skill_type> <skill>.\r\n"
#define REBOOT_AT    10		/* 0-23, time of optional reboot if -e
				 * lib/reboot */

#define TP_MOB    0
#define TP_OBJ	   1
#define TP_ERROR  2

const char *string_fields[] = {
	"name",
	"short",
	"long",
	"description",
	"title",
	"delete-description",
	"\n"
};

/* maximum length for text field x+1 */
const int length[] =
{
	48,
	72,
	256,
	4096,
	72
};

const char *skill_fields[] =
{
	"learned",
	"affected",
	"duration",
	"recognize",
	"\n"
};

int max_value[] =
{
	255,
	255,
	10000,
	1
};
/* **************************************************************** *
*  modification using set command                                  *
****************************************************************** */
void do_set(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (is_abbrev(buf, "autolevel")) {
		CHECK_LEVEL(IMO_IMP);
		/* sys msg written in routine */
		do_set_auto_level(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "char") && GET_LEVEL(ch) > IMO_IMM) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s entered set char%s.",
			GET_REAL_NAME(ch), arg);
		if (GET_LEVEL(ch) <= IMO_IMP)
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		do_set_char(ch, arg, cmd);
		spec_log(buf, GOD_COMMAND_LOG);
		return;
	}

	if (is_abbrev(buf, "clan") && GET_LEVEL(ch) > 20) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s entered set clan%s.",
			GET_REAL_NAME(ch), arg);
		if (GET_LEVEL(ch) <= IMO_IMP)
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		cl2100_do_set_clan(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "channel")) {
		/* sys msg written in routine */
		do_set_channel(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "class")) {
		//NOTE WE ARE PASSING ch AS ARG 1 AND 4. THIS IS
			// BECAUSE THE PLAYER IS LOOKING AT HIS / HER CLASS
			CHECK_LEVEL(IMO_IMM);
		do_set_char_class(ch, arg, cmd, ch);
		return;
	}

	if (is_abbrev(buf, "deathroom")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_deathroom(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "lines")) {
		/* SET THE NUMBER OF LINES THAT FIT ON YOUR SCREEN */
		do_set_lines(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "prompt")) {
		do_set_prompt(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "graphics")) {
		/* SET NEW GRAPHICS / OLD GRAPHICS */
		do_set_graphics(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "flag")) {
		/* NOTE WE ARE PASSING ch AS ARG 1 AND 4.  THIS IS BECAUSE THE
		 * PLAYER IS LOOKING AT HIS/HER OWN FLAGS */
		do_set_flag(ch, arg, cmd, ch);
		return;
	}

	if (is_abbrev(buf, "nameserver")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_nameserver(ch, arg);
		return;
	}

	if (is_abbrev(buf, "pklevel")) {
		do_set_pklevel(ch, arg);
		return;
	}

	if ((is_abbrev(buf, "login")) ||
	    (is_abbrev(buf, "logon"))) {
		CHECK_LEVEL(IMO_IMM);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s entered set login%s.",
			GET_REAL_NAME(ch), arg);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
		do_set_login(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "password")) {
		CHECK_LEVEL(IMO_IMP)
			db6600_set_password(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "start")) {
		do_set_start(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "uptime")) {
		do_set_uptime(ch, arg, cmd);
		return;
	}

	if (is_abbrev(buf, "trace")) {
		CHECK_LEVEL(IMO_IMP);
		do_set_trace(ch, arg, cmd);
		return;
	}

	/* WE DIDN'T FIND A VALID SET ITEM */
	send_to_char("None or invalid argument passed to set.\r\n", ch);
	send_to_char("  FORMAT: set start     <room #>\r\n", ch);
	send_to_char("              channel   <channel> ON or OFF\r\n", ch);
	send_to_char("              flag      <flag>    ON or OFF\r\n", ch);
	send_to_char("              lines     <# lines>\r\n", ch);
	send_to_char("              graphics            OLD or NEW\r\n", ch);

	if (GET_LEVEL(ch) >= IMO_IMM) {
		send_to_char("              char      <player> <FIELDS> <value>\r\n", ch);
		send_to_char("              login     <level>  <optional msg\r\n", ch);
	}

	if (GET_LEVEL(ch) >= IMO_IMP) {
		send_to_char("              autolevel <level #>\r\n", ch);
		send_to_char("              deathroom <room #>\r\n", ch);
		send_to_char("              nameserver ON or OFF\r\n", ch);
		send_to_char("              password <password>\r\n", ch);
		send_to_char("              skill <skill_type> <#>\r\n", ch);
		send_to_char("              uptime <time>\r\n", ch);
	}

	return;
}
/* **************************************************************** *
* SET CHAR                                                         *
****************************************************************** */
void do_set_char(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct char_data *vict;

	/* STRIP OUT PLAYER TO MODIFY */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	CHECK_LEVEL(IMO_IMM)
		if (!(*buf)) {
		send_to_char("    FORMAT:   set char      <player> <FIELDS> <value>\n\n", ch);
		send_to_char("             FIELDS: str,  hit,   max_hit\n", ch);
		send_to_char("                     int,  mana,  max_mana\n", ch);
		send_to_char("                     wis,  move,  max_move\n", ch);
		send_to_char("                     dex,  con,   cha\n", ch);
		send_to_char("                     start, flag, private\n", ch);
		send_to_char("                     flag, gold, bank, inn\n", ch);
		send_to_char("                     exp,  level, usedexp\n", ch);
		send_to_char("                     drunk, hunger, thirst\n", ch);
		send_to_char("                     race, class, alignment\n", ch);
		send_to_char("                     hitroll, dmgroll, age\n", ch);
		send_to_char("                     deaths, skill(spell)\n", ch);
		send_to_char("                     position, invisible, sex\n", ch);
		send_to_char("                     extrahits\n", ch);
		return;
	}

	vict = 0;
	if (cmd != CMD_ADMIN)
		vict = ha3100_get_char_vis(ch, buf);
	else
		vict = ch->desc->admin;
	if (!vict) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "Unable to locate %s.\r\n", buf);
		send_to_char(buf2, ch);
		return;
	}

	/* ONLY IMP CAN DO NPC CHARS */
	/* if (GET_LEVEL(ch) < IMO_IMP) { if (IS_NPC(vict)) { send_to_char("You
	 * can't modify an NPC.\r\n", ch); return; } } */
	/* VICTIM HIGHER IN LEVEL? */
	if (GET_LEVEL(ch) <= GET_LEVEL(vict) &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    ch != vict) {
		send_to_char("That person is to awesome for you to mess with!\r\n", ch);
		return;
	}

	/* STRIP OUT FIELD TO MODIFY */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* BLOCK A COUPLE MINIMUM ABBREVIATIONS */
	if (is_abbrev(buf, "act") ||
	    is_abbrev(buf, "carry") ||
	    is_abbrev(buf, "max_") ||
	    is_abbrev(buf, "passwor")) {
		send_to_char("You didn't specify which field to modify. Enter SET for help", ch);
		return;
	}

	if (is_abbrev(buf, "flag")) {
		do_set_flag(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "age")) {
		do_set_char_age(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "str")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_str(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_str(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "int")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_int(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_int(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "wis")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_wis(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_wis(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "dex")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_dex(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_dex(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "cha")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_cha(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_cha(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "con")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_con(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_con(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "hit")) {
		do_set_char_hit(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "max_hit")) {
		do_set_char_max_hit(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "move")) {
		do_set_char_move(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "max_move")) {
		do_set_char_max_move(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "mana")) {
		do_set_char_mana(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "max_mana")) {
		do_set_char_max_mana(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "exp")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_exp(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP);
			do_set_char_exp(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "usedexp")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_usedexp(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP);
			do_set_char_usedexp(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "level")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_level(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "goldgold")) {
		do_set_char_gold(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "bankgold")) {
		do_set_char_bank(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "inngold")) {
		do_set_char_inn(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "skill")) {
		do_set_char_skill(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "statcount")) {
		CHECK_LEVEL(IMO_IMP);
		do_set_char_statcount(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "statgold")) {
		/* do_set_char_statgold(ch, arg, cmd, vict); *//* I  cheated
		 * here as well */
		return;
	}
	if (is_abbrev(buf, "deaths")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_deaths(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "start")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_start(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "private")) {	/* private room */
		CHECK_LEVEL(IMO_IMP)
			do_set_char_private(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "practices")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_practices(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "alignment")) {
		do_set_char_alignment(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "drunk")) {
		if (GET_LEVEL(vict) < IMO_IMM &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't do this to mortals.\r\n", ch);
			return;
		}
		do_set_char_drunk(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "food") ||
	    is_abbrev(buf, "hunger")) {
		if (GET_LEVEL(vict) < IMO_IMM &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't do this to mortals.\r\n", ch);
			return;
		}
		do_set_char_hunger(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "thirst")) {
		if (GET_LEVEL(vict) < IMO_IMM &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't do this to mortals.\r\n", ch);
			return;
		}
		do_set_char_thirst(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "act1")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_act1(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "act2")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_act2(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "act3")) {
		CHECK_LEVEL(IMO_IMP);
		do_set_char_act3(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "carry_weight")) {
		CHECK_LEVEL(IMO_IMP);
		do_set_char_carry_weight(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "carry_items")) {
		CHECK_LEVEL(IMO_IMP);
		do_set_char_carry_items(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "sex")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_sex(ch, arg, cmd, vict);
		}
		else {
			do_set_char_sex(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "race")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_race(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_race(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "class")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_class(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_class(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "hitroll")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_hitroll(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_hitroll(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "dmgroll")) {
		if ((IS_NPC(vict) && GET_LEVEL(ch) < IMO_IMP) || vict == ch) {
			do_set_char_dmgroll(ch, arg, cmd, vict);
		}
		else {
			CHECK_LEVEL(IMO_IMP)
				do_set_char_dmgroll(ch, arg, cmd, vict);
		}
		return;
	}
	if (is_abbrev(buf, "position")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_position(ch, arg, cmd, vict);
		return;
	}
	if (is_abbrev(buf, "invisible")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_visible(ch, arg, cmd, vict);
		return;
	}

	if (is_abbrev(buf, "questpoints")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_questpoints(ch, arg, cmd, vict);
		return;
	}

	if (is_abbrev(buf, "extrahits")) {
		CHECK_LEVEL(IMO_IMP)
			do_set_char_extrahits(ch, arg, cmd, vict);
		return;
	}

	send_to_char("You didn't specify which field to modify.  Enter SET for help", ch);
	return;

}				/* END OF do_set_char */


/* **************************************************************** *
* SET CHAR                                                        *
* **************************************************************** */
void do_set_char_age(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	INT32 lv_new_year, lv_new_month, lv_new_day, lv_new_hours, lv_old_year,
	  lv_old_month, lv_old_day, lv_old_hours;

	char buf[MAX_STRING_LENGTH];

	lv_old_year = z_age(ch).year;
	lv_old_month = z_age(ch).month;
	lv_old_day = z_age(ch).day;
	lv_old_hours = z_age(ch).hours;

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("Format: age <year> <month> <day> <hours>\r\n",
			     ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is %ld years %ld months, %ld days %ld hours old.\r\n",
			GET_REAL_NAME(vict),
			lv_old_year, lv_old_month,
			lv_old_day, lv_old_hours);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric year values are allowed\r\n", ch);
		return;
	}
	lv_new_year = atoi(buf);
	if (lv_new_year < 17 || lv_new_year > 500) {
		send_to_char("Year must be between 17 and 500\r\n", ch);
		return;
	}

	/* STRIP OUT MONTH */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	if (!(*buf)) {
		sprintf(buf, "%ld", lv_old_month);
	}
	if (!is_number(buf)) {
		send_to_char("Only numeric month values are allowed\r\n", ch);
		return;
	}
	lv_new_month = atoi(buf);
	if (lv_new_month < 0 || lv_new_month > 16) {
		send_to_char("month must be between 0 and 16\r\n", ch);
		return;
	}

	/* STRIP OUT DAYS */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	if (!(*buf)) {
		sprintf(buf, "%ld", lv_old_day);
	}
	if (!is_number(buf)) {
		send_to_char("Only numeric day values are allowed\r\n", ch);
		return;
	}
	lv_new_day = atoi(buf);
	if (lv_new_day < 0 || lv_new_day > 34) {
		send_to_char("day value must be between 0 and 34\r\n", ch);
		return;
	}

	/* STRIP OUT HOURS */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	if (!(*buf)) {
		sprintf(buf, "%ld", lv_old_hours);
	}
	if (!is_number(buf)) {
		send_to_char("Only numeric hour values are allowed\r\n", ch);
		return;
	}
	lv_new_hours = atoi(buf);
	if (lv_new_hours < 0 || lv_new_hours > 23) {
		send_to_char("hour must be between 0 and 23\r\n", ch);
		return;
	}

	/* CHANGE AGE */
	vict->player.time.birth = adjust_time(vict->player.time.birth,
					      lv_new_year,
					      lv_new_month,
					      lv_new_day,
					      lv_new_hours);

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's age changed from y%ld-%ld m%ld-%ld d%ld-%ld h%ld-%ld\r\n",
		GET_REAL_NAME(vict),
		lv_old_year, lv_new_year,
		lv_old_month, lv_new_month,
		lv_old_day, lv_new_day,
		lv_old_hours, lv_new_hours);

	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_() */

void do_set_char_extrahits(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d extra hits\r\n",
			GET_REAL_NAME(vict),
			vict->points.extra_hits);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 100) {
		send_to_char("Value must be between 0 and 100 (hits above 9 show up as x)\r\n", ch);
		return;
	}

	lv_old_value = vict->points.extra_hits;
	vict->points.extra_hits = lv_value;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's extra hits changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		vict->points.extra_hits);
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_extrahits() */


void do_set_char_questpoints(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d questpoints\r\n",
			GET_REAL_NAME(vict),
			vict->questpoints);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 1000000) {
		send_to_char("Value must be between 0 and 1000000\r\n", ch);
		return;
	}

	lv_old_value = vict->questpoints;
	vict->questpoints = lv_value;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's questpoints changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		vict->questpoints);
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_questpoints() */


 /* **************************************************************** * SET CHAR
  * STR                                                    * **************************************************************** */
void do_set_char_str(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d str\r\n",
			GET_REAL_NAME(vict),
			GET_STR(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_STR(vict);
	GET_STR(vict) = lv_value;
	GET_REAL_STR(vict) = lv_value;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's str changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_STR(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_str() */




/* **************************************************************** *
 * SET CHAR INT                                                    *
 * **************************************************************** */
void do_set_char_int(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d int\r\n",
			GET_REAL_NAME(vict),
			GET_INT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_INT(vict);
	GET_INT(vict) = lv_value;
	GET_REAL_INT(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's int changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_INT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_int() */
/* **************************************************************** *
 * SET CHAR WIS                                                    *
 * **************************************************************** */
void do_set_char_wis(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d wis\r\n",
			GET_REAL_NAME(vict),
			GET_WIS(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_WIS(vict);
	GET_WIS(vict) = lv_value;
	GET_REAL_WIS(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's wis changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_WIS(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_wis() */
/* **************************************************************** *
 * SET CHAR DEX                                                    *
 * **************************************************************** */
void do_set_char_dex(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d dex\r\n",
			GET_REAL_NAME(vict),
			GET_DEX(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_DEX(vict);
	GET_DEX(vict) = lv_value;
	GET_REAL_DEX(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's dex changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_DEX(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_dex() */
/* **************************************************************** *
 * SET CHAR CON                                                    *
 * **************************************************************** */
void do_set_char_con(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d con\r\n",
			GET_REAL_NAME(vict),
			GET_CON(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_CON(vict);
	GET_CON(vict) = lv_value;
	GET_REAL_CON(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's con changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_CON(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_con() */


/* **************************************************************** *
 * SET CHAR CHA                                                    *
 * **************************************************************** */
void do_set_char_cha(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d cha\r\n",
			GET_REAL_NAME(vict),
			GET_CHA(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 26) {
		send_to_char("Value must be between 1 and 26\r\n", ch);
		return;
	}

	lv_old_value = GET_CHA(vict);
	GET_CHA(vict) = lv_value;
	GET_REAL_CHA(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's cha changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_CHA(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_cha() */



/* **************************************************************** *
 * SET CHAR HIT                                                    *
 * **************************************************************** */
void do_set_char_hit(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d hit points\r\n",
			GET_REAL_NAME(vict),
			GET_HIT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < -10 || lv_value > 100000000) {
		send_to_char("Value must be between -10 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_HIT(vict);
	GET_HIT(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's hit points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_HIT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_hit() */


/* **************************************************************** *
 * SET CHAR MAX HIT                                                *
 * **************************************************************** */
void do_set_char_max_hit(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d max hit points\r\n",
			GET_REAL_NAME(vict),
			GET_MAX_HIT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < 1 || lv_value > 100000000) {
		send_to_char("Value must be between 1 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_MAX_HIT(vict);
	GET_MAX_HIT(vict) = lv_value;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's max_hit points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_MAX_HIT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_max_hit() */
/* **************************************************************** *
 * SET CHAR MOVE                                                   *
 * **************************************************************** */
void do_set_char_move(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d move points\r\n",
			GET_REAL_NAME(vict),
			GET_MOVE(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < 1 || lv_value > 100000000) {
		send_to_char("Value must be between 1 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_MOVE(vict);
	GET_MOVE(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's move points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_MOVE(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_move() */
/* **************************************************************** *
 * SET CHAR MAX MOVE                                               *
 * **************************************************************** */
void do_set_char_max_move(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d max move points\r\n",
			GET_REAL_NAME(vict),
			GET_MAX_MOVE(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < 1 || lv_value > 100000000) {
		send_to_char("Value must be between 1 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_MAX_MOVE(vict);
	GET_MAX_MOVE(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's max_move points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_MAX_MOVE(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_max_move() */


/* **************************************************************** *
 * SET CHAR MANA                                                   *
 * **************************************************************** */
void do_set_char_mana(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d mana points\r\n",
			GET_REAL_NAME(vict),
			GET_MANA(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < 1 || lv_value > 100000000) {
		send_to_char("Value must be between 1 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_MANA(vict);
	GET_MANA(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's mana points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_MANA(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_mana() */
/* **************************************************************** *
 * SET CHAR MAX MANA                                               *
 * **************************************************************** */
void do_set_char_max_mana(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d max mana points\r\n",
			GET_REAL_NAME(vict),
			GET_MAX_MANA(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 1000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 1000000\r\n", ch);
		return;
	}

	if (lv_value < 1 || lv_value > 100000000) {
		send_to_char("Value must be between 1 and 100000000\r\n", ch);
		return;
	}

	lv_old_value = GET_MAX_MANA(vict);
	GET_MAX_MANA(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's max mana points changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_MAX_MANA(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_max_mana() */
/* **************************************************************** *
 * SET CHAR EXP                                                    *
 * **************************************************************** */
void do_set_char_exp(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d exp\r\n",
			GET_REAL_NAME(vict),
			GET_EXP(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 100000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 100million\r\n", ch);
		return;
	}

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 1000000000) {
		send_to_char("Value must be between 1 and 1billion\r\n", ch);
		return;
	}

	lv_old_value = GET_EXP(vict);
	GET_EXP(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's exp changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_EXP(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_exp() */

void do_set_char_usedexp(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has used %d exp\r\n",
			GET_REAL_NAME(vict),
			GET_USEDEXP(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_value < 1 || lv_value > 100000000) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Value must be between 1 and 100million\r\n", ch);
		return;
	}

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 1000000000) {
		send_to_char("Value must be between 1 and 1billion\r\n", ch);
		return;
	}

	lv_old_value = GET_USEDEXP(vict);
	GET_USEDEXP(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's used exp changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_USEDEXP(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_exp() */


/* **************************************************************** *
 * SET CHAR LEVEL                                                  *
 * **************************************************************** */
void do_set_char_level(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is level %d\r\n",
			GET_REAL_NAME(vict),
			GET_LEVEL(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 ||
	    lv_value > MAX_LEV ||
	    ((lv_value > GET_LEVEL(ch)) && (GET_LEVEL(ch) < MAX_LEV))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 1 and %d\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = GET_LEVEL(vict);
	GET_LEVEL(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's level changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_LEVEL(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_level() */
/* **************************************************************** *
 * SET CHAR GOLD                                                   *
 * **************************************************************** */
void do_set_char_gold(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d gold\r\n",
			GET_REAL_NAME(vict),
			GET_GOLD(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (GET_LEVEL(ch) < IMO_IMP) {
		if (lv_value < 0 || lv_value > 500000000) {
			send_to_char("Value must be between 0 and 500 million\r\n", ch);
			return;
		}
		else {
			if (lv_value < 0 || lv_value > 2000000000) {
				send_to_char("Value must be between 0 and 2 billion\r\n", ch);
				return;
			}
		}
	}
	lv_old_value = GET_GOLD(vict);
	GET_GOLD(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's gold changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_GOLD(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_gold() */
/* **************************************************************** *
 * SET CHAR BANK BANK_GOLD                                              *
 * **************************************************************** */
void do_set_char_bank(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d bank gold\r\n",
			GET_REAL_NAME(vict),
			GET_BANK_GOLD(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 500000000) {
		send_to_char("Value must be between 0 and 500 million\r\n", ch);
		return;
	}

	lv_old_value = GET_BANK_GOLD(vict);
	GET_BANK_GOLD(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's bank gold changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_BANK_GOLD(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_bank() */


/* **************************************************************** *
 * SET CHAR STAT_COUNT                                              *
 * **************************************************************** */
void do_set_char_statcount(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d stat count\r\n",
			GET_REAL_NAME(vict),
			GET_STAT_COUNT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 500) {
		send_to_char("Value must be between 0 and 500.\r\n", ch);
		return;
	}

	lv_old_value = GET_STAT_COUNT(vict);
	GET_STAT_COUNT(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's stat count changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_STAT_COUNT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_statgold() */



/* **************************************************************** *
 * SET CHAR STAT_GOLD                                               *
 * **************************************************************** */
void do_set_char_statgold(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d stat gold\r\n",
			GET_REAL_NAME(vict),
			GET_STAT_GOLD(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 500000000) {
		send_to_char("Value must be between 0 and 500 million\r\n", ch);
		return;
	}

	lv_old_value = GET_STAT_GOLD(vict);
	GET_STAT_GOLD(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's stat gold changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_STAT_GOLD(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_statgold() */
/* **************************************************************** *
 * SET CHAR INN_GOLD                                                *
 * **************************************************************** */
void do_set_char_inn(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d inn gold\r\n",
			GET_REAL_NAME(vict),
			GET_INN_GOLD(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 500000000) {
		send_to_char("Value must be between 0 and 500 million\r\n", ch);
		return;
	}

	lv_old_value = GET_INN_GOLD(vict);
	GET_INN_GOLD(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's inn gold changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_INN_GOLD(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_inn() */


/* **************************************************************** *
 * SET CHAR INN_SKILL                                               *
 * **************************************************************** */
void do_set_char_skill(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_skill_type, lv_skill_value, lv_sign;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	/* ************ */
	/* SKILL TYPE  */
	/* ************ */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("Sorry, but a skill type is required.\r\n", ch);
		send_to_char("Type is <skill>, <spell> or <#>.\r\n", ch);
		send_to_char(buf2, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (is_number(buf)) {
		lv_skill_type = atoi(buf);
	}
	else {
		lv_skill_type = old_search_block(buf, 0, strlen(buf),
						 spell_names, 0);
		if (lv_skill_type < 0) {
			sprintf(buf2, "Sorry, but skill type: %s is invalid.\r\n",
				buf);
			send_to_char(buf2, ch);
			send_to_char("Type is <skill>, <spell> or <#>.\r\n", ch);
			return;
		}
	}

	/* IS SKILL IN CORRECT RANGE?  DON'T EXCEED ARRAY BOUNDARIS */
	if (lv_skill_type < 1 || lv_skill_type >= MAX_SKILLS) {
		sprintf(buf, "Skill type must be between 0 and %d.\r\n",
			MAX_SKILLS);
		send_to_char(buf, ch);
		send_to_char(SET_SKILL_FORMAT, ch);
		return;
	}

	/* CAN MOBS CAST IT? */
	if (IS_NPC(vict) &&
	    !IS_SET(spell_info[lv_skill_type].spell_flag, MOB_SPELL) &&
	    !IS_SET(spell_info[lv_skill_type].spell_flag, MOB_SKILL)) {
		send_to_char("Sorry, but you can't set this spell/skill for mobs.", ch);
		return;
	}

	/* ************* */
	/* SKILL VALUE  */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		strcpy(buf, "0");
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Skill value must be numeric.\r\n", ch);
		send_to_char(SET_SKILL_FORMAT, ch);
		return;
	}

	lv_skill_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_skill_value < 0 || lv_skill_value > 99)) {
		send_to_char("Skill value must be between 0 and 99.\r\n", ch);
		return;
	}

	/* UPDATE CHAR */
	bzero(buf, sizeof(buf));
	sprintf(buf, "Setting %s %s[%d] from %d to %d.\r\n",
		GET_REAL_NAME(vict),
		sk_sp_defines[lv_skill_type - 1],
		lv_skill_type,
		vict->skills[lv_skill_type - 1].learned,
		lv_skill_value);
	send_to_char(buf, ch);
	vict->skills[lv_skill_type].learned = lv_skill_value;

	return;

}				/* END OF void do_set_char_skill() */


/* **************************************************************** *
 * SET CHAR DEATH                                                   *
 * **************************************************************** */
void do_set_char_deaths(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has %d deaths.\r\n",
			GET_REAL_NAME(vict),
			GET_DEATHS(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);


	lv_old_value = GET_DEATHS(vict);
	GET_DEATHS(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's deaths changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_DEATHS(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_deaths() */




/* **************************************************************** *
 * SET CHAR START                                                  *
 * **************************************************************** */
void do_set_char_start(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, location;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s's start room is %d\r\n",
			GET_REAL_NAME(vict),
			GET_START(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS ROOM NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* MAKE SURE ROOM EXIST */
	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == lv_value)
			break;
		else if (location == top_of_world) {
			send_to_char("No room exists with that number.\r\n",
				     ch);
			return;
		}

	GET_START(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's start room is %d\r\n",
		GET_REAL_NAME(vict),
		GET_START(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_start() */
/* **************************************************************** *
 * SET CHAR PRIVATE                                                *
 * **************************************************************** */
void do_set_char_private(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, location;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s's private room is %d\r\n",
			GET_REAL_NAME(vict),
			GET_PRIVATE(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS ROOM NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* MAKE SURE ROOM EXIST */
	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == lv_value)
			break;
		else if (location == top_of_world) {
			send_to_char("No room exists with that number.\r\n",
				     ch);
			return;
		}

	GET_PRIVATE(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's private room is %d\r\n",
		GET_REAL_NAME(vict),
		GET_PRIVATE(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_private() */
/* **************************************************************** *
 * SET CHAR PRACTICES                                              *
 * **************************************************************** */
void do_set_char_practices(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d practices\r\n",
			GET_REAL_NAME(vict),
			GET_PRACTICES(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 200) {
		send_to_char("Value must be between 1 and 200\r\n", ch);
		return;
	}

	lv_old_value = GET_PRACTICES(vict);
	GET_PRACTICES(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's practices changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_PRACTICES(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_practices() */




/* **************************************************************** *
 *  SET GRAPHICS                                                    *
 * **************************************************************** */
void do_set_graphics(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		sprintf(buf, "You currently display %s graphics.\r\n", GET_NEWGRAPHICS(ch) ? "new" : "old");
		send_to_char(buf, ch);
		return;
	}

	if (is_abbrev(buf, "old")) {
		if (!GET_NEWGRAPHICS(ch)) {
			send_to_char("You are already using old graphics!\r\n", ch);
			return;
		}
		else {
			send_to_char("Switching from new to old graphics.\r\n", ch);
			REMOVE_BIT(GET_ACT2(ch), PLR2_NEWGRAPHICS);
			return;
		}
	}

	if (is_abbrev(buf, "new")) {
		if (GET_NEWGRAPHICS(ch)) {
			send_to_char("You are already using new graphics!\r\n", ch);
			return;
		}
		else {
			send_to_char("Switching from old to new graphics.\r\n", ch);
			SET_BIT(GET_ACT2(ch), PLR2_NEWGRAPHICS);
			return;
		}
	}

	send_to_char("Please use:\r\n\r\n", ch);
	send_to_char("Set graphics old  - Switch to old graphics\r\n", ch);
	send_to_char("Set graphics new  - Switch to new graphics\r\n", ch);
	return;
}
/* **************************************************************** *
*  SET NAMESERVER                                                  *
* **************************************************************** */
void do_set_nameserver(struct char_data * ch, char *arg)
{

	char buf[MAX_STRING_LENGTH];

	for (; isspace(*arg); arg++);

	if (!*arg) {
		sprintf(buf, "Currently the nameserver is %s.\r\n", gv_use_nameserver ? "on" : "off");
		send_to_char(buf, ch);
		return;
	}

	if (is_abbrev(arg, "on")) {
		if (gv_use_nameserver) {
			send_to_char("Already using the nameserver.\r\n", ch);
			return;
		}
		else {
			send_to_char("Turning on nameserver.\r\n", ch);
			gv_use_nameserver = 1;
			return;
		}
	}

	if (is_abbrev(arg, "off")) {
		if (!gv_use_nameserver) {
			send_to_char("Nameserver is already off.\r\n", ch);
			return;
		}
		else {
			send_to_char("Turning off nameserver.\r\n", ch);
			gv_use_nameserver = 0;
			return;
		}
	}

	send_to_char("Wrong option.\r\n", ch);
}
/* **************************************************************** *
*  SET PKLEVEL                                                     *
* **************************************************************** */
void do_set_pklevel(struct char_data * ch, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int lv_pklevel;

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		sprintf(buf, "Current PKill level is set at %d.\r\n", gv_pkill_level);
		send_to_char(buf, ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("Your level isn't high enough to do this!\r\n", ch);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed\r\nType 'set pklevel' to see current level\r\n", ch);
		return;
	}

	lv_pklevel = atoi(buf);
	if ((lv_pklevel < 1) || (lv_pklevel > 50)) {
		send_to_char("Must be a value between 1 and 50!\r\n", ch);
		return;
	}

	if (gv_pkill_level > lv_pklevel) {
		sprintf(buf, "Warning PKill level changed from %d to %d !!!!!", gv_pkill_level, lv_pklevel);
	}
	if (gv_pkill_level < lv_pklevel) {
		sprintf(buf, "PKill level raised from %d to %d.", gv_pkill_level, lv_pklevel);
	}
	if (gv_pkill_level == lv_pklevel)
		return;

	do_info_noch(buf, 1, 99);
	sprintf(buf, "PKILL: %s changed pkill level from %d to %d.",
		GET_REAL_NAME(ch), gv_pkill_level, lv_pklevel);
	spec_log(buf, GOD_COMMAND_LOG);
	gv_pkill_level = lv_pklevel;
}
/* **************************************************************** *
*  SET START                                                       *
* **************************************************************** */
void do_set_deathroom(struct char_data * ch, char *arg, int cmd)
{

	int new_death, location, rc;
	char buf[MAX_STRING_LENGTH];

	/* LIMIT WHO CAN DO THIS */
	if (IS_NPC(ch)) {
		send_to_char("Sorry, but mobs can't do this.\r\n", ch);
		return;
	}

	/* STRIP OUT ROOM NUMBER */
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "death room is %d.\r\n", gv_deathroom);
		send_to_char(buf, ch);
		return;
	}

	/* IS ROOM NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed.\r\n", ch);
		return;
	}

	new_death = atoi(buf);

	/* MAKE SURE ROOM EXIST */
	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == new_death)
			break;
		else if (location == top_of_world) {
			send_to_char("No room exists with that number.\r\n",
				     ch);
			return;
		}

	/* DO THEY HAVE ACCESS TO ROOM */
	rc = li3000_is_blocked_from_room(ch, location, 0);
	if (rc) {
		send_to_char("You don't have access to that room.\r\n", ch);
		return;
	}

	sprintf(buf, "Death room changed from %d to %d.\r\n",
		gv_deathroom, new_death);
	send_to_char(buf, ch);

	gv_deathroom = new_death;
	return;

}				/* END OF do_set_deathroom() */


/* **************************************************************** *
 *  SET LINES                                                       *
 * **************************************************************** */
void do_set_lines(struct char_data * ch, char *arg, int cmd)
{

	int lv_lines;
	char buf[MAX_STRING_LENGTH];

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		sprintf(buf, "You currently display %d lines a page.\r\n", ch->screen_lines);
		send_to_char(buf, ch);
		return;
	}
	if (is_abbrev(buf, "list")) {
		for (lv_lines = 100; lv_lines >= 0; lv_lines--) {
			sprintf(buf, "- %d\r\n", lv_lines);
			send_to_char(buf, ch);
		}
		send_to_char("Now enter the top number on your screen in 'set lines <#>'\r\n", ch);
		return;
	}


	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed\r\nUse 'Set lines 0' for unlimited lines.\r\nUse 'Set lines list' to see a list.\r\n", ch);
		return;
	}

	lv_lines = atoi(buf);

	if ((lv_lines != 0) && (lv_lines < 15)) {
		send_to_char("You can't have less than 15 lines a page!\r\n", ch);
		return;
	}


	sprintf(buf, "You now display %d lines a page.\r\n", lv_lines);
	send_to_char(buf, ch);
	ch->screen_lines = lv_lines;

}
/* **************************************************************** *
*  SET PROMPT                                                      *
* **************************************************************** */
void do_set_prompt(struct char_data * ch, char *arg, int cmd)
{

	int lv_prompt, idx;
	char buf[MAX_STRING_LENGTH];

	const char *lv_prompts[] = {
		"&nstandard",	/* 1 */
		"&Rd&ra&Cr&ck &Yr&ya&wi&Pn&pb&Ro&rw&n",	/* 2 */
		"&Wl&Ri&rg&Wh&Ct &cr&Wa&Yi&yn&Wb&Po&pw&n",	/* 3 */
		"&Wc&Cool&n",	/* 4 */
		"&Rh&rot&n",	/* 5 */
		"\n",
	};

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if ((ch->points.prompt < 1) || (ch->points.prompt > 5))
		ch->points.prompt = 1;

	if (!(*buf)) {
		sprintf(buf, "You are currently using prompt '%s'.\r\n\r\nAvailable prompts: \r\n", lv_prompts[ch->points.prompt - 1]);
		send_to_char(buf, ch);
		for (idx = 0; idx < 5; idx++) {
			sprintf(buf, "&n%-2d. %s\r\n", idx + 1, lv_prompts[idx]);
			send_to_char(buf, ch);
		}
		return;
	}

	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed.\r\n", ch);
		return;
	}

	lv_prompt = atoi(buf);

	if ((lv_prompt < 1) || (lv_prompt > 5)) {
		send_to_char("That's not a valid option!\r\nType 'set prompt' for a list.\r\n", ch);
		return;
	}

	sprintf(buf, "You are now using '%s' as prompt.\r\n", lv_prompts[lv_prompt - 1]);
	send_to_char(buf, ch);
	ch->points.prompt = lv_prompt;

}


/* **************************************************************** *
*  SET START                                                       *
* **************************************************************** */
void do_set_start(struct char_data * ch, char *arg, int cmd)
{

	int new_start, location, rc;
	char buf[MAX_STRING_LENGTH];

	/* LIMIT WHO CAN DO THIS */
	if (IS_NPC(ch)) {
		send_to_char("Sorry, but mobs can't do this.\r\n", ch);
		return;
	}
	if (GET_LEVEL(ch) < IMO_SPIRIT) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You must be level %d to set a start room\r\n",
			IMO_IMM);
		send_to_char(buf, ch);
		return;
	}

	/* STRIP OUT ROOM NUMBER */
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Your starting room is %d\r\n",
			GET_START(ch));
		send_to_char(buf, ch);
		return;
	}

	/* IS ROOM NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric arguments are allowed\r\n", ch);
		return;
	}

	new_start = atoi(buf);

	/* MAKE SURE ROOM EXIST */
	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == new_start)
			break;
		else if (location == top_of_world) {
			send_to_char("No room exists with that number.\r\n",
				     ch);
			return;
		}

	/* DO THEY HAVE ACCESS TO ROOM */
	rc = li3000_is_blocked_from_room(ch, location, 0);
	if (rc) {
		send_to_char("I don't think so.\r\n", ch);
		return;
	}

	GET_START(ch) = new_start;
	bzero(buf, sizeof(buf));
	sprintf(buf, "Your new start room is %d\r\n",
		GET_START(ch));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_start() */
/* **************************************************************** *
 * SET UPTIME                                                      *
 * **************************************************************** */
void do_set_uptime(struct char_data * ch, char *arg, int cmd)
{

	extern long up_time;

	long lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	CHECK_LEVEL(IMO_IMP);

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "uptime is %ld\r\n", up_time);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > time(0)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 1 and %ld\r\n", (long) time(0));;
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = up_time;
	up_time = lv_value;

	bzero(buf, sizeof(buf));
	sprintf(buf, "Uptime changed from %ld to %ld\r\n",
		lv_old_value,
		up_time);
	send_to_char(buf, ch);

	/* REPORT TO LOG */
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s has changed uptime from %ld to %ld.\r\n",
		GET_REAL_NAME(ch),
		lv_old_value,
		up_time);
	do_sys(buf, GET_LEVEL(ch) + 1, ch);

	return;

}				/* END OF do_set_char_str() */



/* **************************************************************** *
 * SET CHAR ALIGNMENT                                              *
 * **************************************************************** */
void do_set_char_alignment(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d alignment\r\n",
			GET_REAL_NAME(vict),
			GET_ALIGNMENT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < -1000 || lv_value > 1000) {
		send_to_char("Value must be between -1000 and 1000\r\n", ch);
		return;
	}

	lv_old_value = GET_ALIGNMENT(vict);
	GET_ALIGNMENT(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's alignment changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_ALIGNMENT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_alignment() */
/* **************************************************************** *
 * SET CHAR DRUNK                                                  *
 * **************************************************************** */
void do_set_char_drunk(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d drunk\r\n",
			GET_REAL_NAME(vict),
			GET_COND(vict, DRUNK));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < -1 || lv_value > races[GET_RACE(ch)].max_drunk) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between -1 and %d.\r\n",
			races[GET_RACE(ch)].max_drunk);
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = GET_COND(vict, DRUNK);
	GET_COND(vict, DRUNK) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's drunk changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_COND(vict, DRUNK));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_drunk() */
/* **************************************************************** *
 * SET CHAR HUNGER                                                 *
 * **************************************************************** */
void do_set_char_hunger(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d hunger\r\n",
			GET_REAL_NAME(vict),
			GET_COND(vict, FOOD));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < -1 || lv_value > races[GET_RACE(ch)].max_food) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between -1 and %d.\r\n",
			races[GET_RACE(ch)].max_food);
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = GET_COND(vict, FOOD);
	GET_COND(vict, FOOD) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's hunger changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_COND(vict, FOOD));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_hunger() */
/* **************************************************************** *
 * SET CHAR THIRST                                                 *
 * **************************************************************** */
void do_set_char_thirst(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d thirst\r\n",
			GET_REAL_NAME(vict),
			GET_COND(vict, THIRST));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < -1 || lv_value > races[GET_RACE(ch)].max_thirst) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between -1 and %d.\r\n",
			races[GET_RACE(ch)].max_thirst);
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = GET_COND(vict, THIRST);
	GET_COND(vict, THIRST) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's thirst changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_COND(vict, THIRST));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_thirst() */
/* **************************************************************** *
 * SET CHAR ACT1                                                   *
 * **************************************************************** */
void do_set_char_act1(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %ld act\r\n",
			GET_REAL_NAME(vict),
			GET_ACT1(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	/*
	 if (lv_value < 0 || lv_value > 0) {
		 send_to_char("Value must be between 0 and 0\r\n", ch);
		 return;
	 }
	 */
	lv_old_value = GET_ACT1(vict);
	GET_ACT1(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's act changed from %d to %ld\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_ACT1(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_act1() */
/* **************************************************************** *
 * SET CHAR ACT2                                                   *
 * **************************************************************** */
void do_set_char_act2(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %ld act2\r\n",
			GET_REAL_NAME(vict),
			GET_ACT2(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	/*
	 if (lv_value < 0 || lv_value > 0) {
		 send_to_char("Value must be between 0 and 0\r\n", ch);
		 return;
	 }
	 */
	lv_old_value = GET_ACT2(vict);
	GET_ACT2(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's act2 changed from %d to %ld\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_ACT3(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_act2() */
/* **************************************************************** *
 * SET CHAR ACT3                                                   *
 * **************************************************************** */
void do_set_char_act3(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %ld act3\r\n",
			GET_REAL_NAME(vict),
			GET_ACT3(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	/*
	 if (lv_value < 0 || lv_value > 0) {
		 send_to_char("Value must be between 0 and 0\r\n", ch);
		 return;
	 }
	 */
	lv_old_value = GET_ACT3(vict);
	GET_ACT3(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's act3 changed from %d to %ld\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_ACT3(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_act3() */
/* **************************************************************** *
 * SET CHAR CARRY WEIGHT                                           *
 * **************************************************************** */
void do_set_char_carry_weight(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d carry_weight\r\n",
			GET_REAL_NAME(vict),
			GET_CARRY_WEIGHT(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 1000) {
		send_to_char("Value must be between 0 and 1000\r\n", ch);
		return;
	}

	lv_old_value = GET_CARRY_WEIGHT(vict);
	GET_CARRY_WEIGHT(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's carry_weight changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_CARRY_WEIGHT(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_carry_weight() */
/* **************************************************************** *
 * SET CHAR CARRY ITEMS                                            *
 * **************************************************************** */
void do_set_char_carry_items(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d carry_items\r\n",
			GET_REAL_NAME(vict),
			GET_CARRY_ITEMS(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 1000) {
		send_to_char("Value must be between 0 and 1000\r\n", ch);
		return;
	}

	lv_old_value = GET_CARRY_ITEMS(vict);
	GET_CARRY_ITEMS(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's carry_items changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_CARRY_ITEMS(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_carry_items() */


/* **************************************************************** *
 * SET CHAR RACE                                                     *
 * **************************************************************** */
void do_set_char_race(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is a (%d)%s race.\r\n",
			GET_REAL_NAME(vict),
			GET_RACE(vict),
			races[GET_RACE(vict)].name);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value >= MAX_RACES) {
		sprintf(buf, "Value must be between 1 and %d\r\n", MAX_RACES - 1);
		send_to_char(buf, ch);
		return;
	}

	/* Is it defined? */
	if (!(strncmp(races[lv_value].name, "undefined", 9))) {
		send_to_char("Sorry, that race is undefined.\r\n", ch);
		return;
	}

	lv_old_value = GET_RACE(vict);
	GET_RACE(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's race changed from (%d)%s to (%d)%s\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		races[lv_old_value].name,
		GET_RACE(vict),
		races[GET_RACE(vict)].name);
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_race() */


/* **************************************************************** *
 * SET CHAR CLASS                                                    *
 * **************************************************************** */
void do_set_char_class(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is a (%d)%s class.\r\n",
			GET_REAL_NAME(vict),
			GET_CLASS(vict),
			classes[GET_CLASS(vict)].name);
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value >= MAX_CLASSES) {
		sprintf(buf, "Value must be between 1 and %d\r\n", MAX_CLASSES - 1);
		send_to_char(buf, ch);
		return;
	}

	/* Is it defined? */
	if (!(strncmp(classes[lv_value].name, "undefined", 9))) {
		send_to_char("Sorry, that class is undefined.\r\n", ch);
		return;
	}

	if ((ch == vict) && (!check_class_race(lv_value, GET_RACE(vict)))) {
		send_to_char("Your race does not allow you to chose this class.\r\n", ch);
		return;
	}

	lv_old_value = GET_CLASS(vict);
	GET_CLASS(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's class changed from (%d)%s to (%d)%s\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		classes[lv_old_value].name,
		GET_CLASS(vict),
		classes[GET_CLASS(vict)].name);
	send_to_char(buf, ch);

	/* IF THIS IS AN AVATAR, SET EXPERIECE TO ZERO */
	if (ch == vict && (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV)) {
		GET_EXP(ch) = 0;
		GET_USEDEXP(ch) = 0;
		send_to_char("You've been set to level 1 in that class.\r\n",
			     ch);
	}
	return;

}				/* END OF do_set_char_class() */




/* **************************************************************** *
 * SET CHAR SEX                                                     *
 * **************************************************************** */
void do_set_char_sex(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d SEX\r\n",
			GET_REAL_NAME(vict),
			GET_SEX(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 2) {
		send_to_char("Value must be between 1 and 2\r\n", ch);
		return;
	}

	lv_old_value = GET_SEX(vict);
	GET_SEX(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's sex changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_SEX(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_sex() */


/* **************************************************************** *
 * SET CHAR AC                                                    *
 * **************************************************************** */
void do_set_char_ac(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d ac\r\n",
			GET_REAL_NAME(vict),
			GET_AC(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 25) {
		send_to_char("Value must be between 1 and 25\r\n", ch);
		return;
	}

	lv_old_value = GET_AC(vict);
	GET_AC(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's ac changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_AC(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_ac() */
/* **************************************************************** *
 * SET CHAR HITROLL                                                    *
 * **************************************************************** */
void do_set_char_hitroll(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d hitroll\r\n",
			GET_REAL_NAME(vict),
			GET_HITROLL(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 99) {
		send_to_char("Value must be between 1 and 99\r\n", ch);
		return;
	}

	lv_old_value = GET_HITROLL(vict);
	GET_HITROLL(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's hitroll changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_HITROLL(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_hitroll() */
/* **************************************************************** *
 * SET CHAR DMGROLL                                                    *
 * **************************************************************** */
void do_set_char_dmgroll(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d dmgroll\r\n",
			GET_REAL_NAME(vict),
			GET_DAMROLL(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 1 || lv_value > 99) {
		send_to_char("Value must be between 1 and 99\r\n", ch);
		return;
	}

	lv_old_value = GET_DAMROLL(vict);
	GET_DAMROLL(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's dmgroll changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_DAMROLL(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_dmgroll() */
 /* **************************************************************** * SET CHAR
  * POS                                                    * **************************************************************** */
void do_set_char_position(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has a %d position\r\n",
			GET_REAL_NAME(vict),
			GET_POS(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < POSITION_MORTALLYW || lv_value > POSITION_STANDING) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between %d and %d\r\n",
			POSITION_MORTALLYW, POSITION_STANDING);
		send_to_char(buf, ch);
		return;
	}

	/* YOU CAN'T PLACE A CHAR IN A FIGHT OR OUT OF ONE */
	if (lv_value == POSITION_FIGHTING ||
	    GET_POS(vict) == POSITION_FIGHTING) {
		if (GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't start/stop fights this way!\r\n", ch);
			return;
		}
		send_to_char("Warning!  Starting/stoping fights this way could have unpredictable results\r\n", ch);
	}

	lv_old_value = GET_POS(vict);
	GET_POS(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's position changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_POS(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_position() */
/* **************************************************************** *
 * SET CHAR VISIBLE                                                    *
 * **************************************************************** */
void do_set_char_visible(struct char_data * ch, char *arg, int cmd, struct char_data * vict)
{

	int lv_value, lv_sign, lv_old_value;
	char buf[MAX_STRING_LENGTH];

	/* STRIP OUT NEW VALUE */
	for (; isspace(*arg); arg++);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s invisible level is %d\r\n",
			GET_REAL_NAME(vict),
			GET_VISIBLE(vict));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > GET_LEVEL(ch)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 0 and %d\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}

	lv_old_value = GET_VISIBLE(vict);
	GET_VISIBLE(vict) = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s's invisible changed from %d to %d\r\n",
		GET_REAL_NAME(vict),
		lv_old_value,
		GET_VISIBLE(vict));
	send_to_char(buf, ch);

	return;

}				/* END OF do_set_char_visible() */

void do_set_login(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int idx, lv_value;

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));

	/* ROUTINE, "one_argument", WON'T RETURN "on" */
	strcpy(buf, arg);
	/* FIND FIRST SPACE */
	for (idx = 0; buf[idx] != ' ' &&
	     buf[idx] != 0 &&
	     idx < sizeof(buf); idx++);

	/* NULL TERMINATE AT END OF FIRST ARGUMENT AND ADJUST ARG */
	if (buf[idx] == ' ' || buf[idx] == 0) {
		buf[idx] = 0;
		strcpy(arg, arg + idx);
	}

	if (!(*buf)) {
		if (gv_highest_login_level == 0) {
			sprintf(buf, "Logins enabled.  Use set login <level> <optional msg>.\r\n");
		}
		else {
			sprintf(buf, "Logins under level %d disabled.  Use set login <level> <optional msg>.\r\n", gv_highest_login_level);
		}
		if (*gv_login_message) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Login msg: %s\r\n", gv_login_message);
		}
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Only numeric values are allowed.\r\n", ch);
		return;
	}

	lv_value = atoi(buf);

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value >= IMO_IMP) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 0 and %d\r\n", IMO_IMP);
		send_to_char(buf, ch);
		return;
	}

	gv_highest_login_level = lv_value;

	/* IS THERE AN OPTIONAL DISABLE MESSAGE? */
	bzero(gv_login_message, sizeof(gv_login_message));
	if (*arg) {
		strncpy(gv_login_message, arg,
			sizeof(gv_login_message) - 2);
	}


	bzero(buf, sizeof(buf));
	if (gv_highest_login_level == 0) {
		sprintf(buf, "Logins enabled for all.\r\n");
	}
	else {
		sprintf(buf, "Logins under level %d disabled.  Use set login <level> <optional msg>.\r\n", gv_highest_login_level);
	}

	if (*gv_login_message) {
		sprintf(buf, "Login msg: %s\r\n", gv_login_message);
	}

	send_to_char(buf, ch);
	return;

}				/* END OF do_set_login */


void do_set_trace(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You are not authorized to use set in this way.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);

	if (!*arg) {
		bzero(buf, sizeof(buf));
		if (gv_trace_commands)
			sprintf(buf, "Trace command currently at: %d. Use set trace ON or OFF\r\n",
				gv_trace_commands);
		else
			strcpy(buf, "Trace command not turned on.  Use set trace ON or OFF.\r\n");
		send_to_char(buf, ch);
		return;
	}

	if (is_abbrev(arg, "on")) {
		if (gv_trace_commands)
			send_to_char("Trace is already turned on.  Resetting to 1\r\n", ch);
		else
			ha9000_open_tracefile(2);
		if (!trace_file) {
			sprintf(buf, "Error %d opening tracefile - trace not started.\r\n",
				errno);
			send_to_char(buf, ch);
			return;
		}
		send_to_char("Trace started.\r\n", ch);
		gv_trace_commands = 1;
		return;
	}

	if (is_abbrev(arg, "off")) {
		if (gv_trace_commands) {
			fclose(trace_file);
			send_to_char("Trace turned off.\r\n", ch);
		}
		else
			send_to_char("Trace isn't on.\r\n", ch);
		gv_trace_commands = 0;
		return;
	}

	send_to_char("Please enter ON or OFF\r\n", ch);
	return;

}				/* END OF do_set_trace */

void do_set_auto_level(struct char_data * ch, char *arg, int cmd)
{

	int lv_value;
	char buf[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You are not authorized to use set in this way.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);

	if (!*arg) {
		bzero(buf, sizeof(buf));
		if (gv_auto_level)
			sprintf(buf, "AUTOLEVEL currently at: %d. Use set autolevel <number>",
				gv_auto_level);
		else
			strcpy(buf, "AUTOLEVEL is not turned on.  Use set autolevel <number>.");
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(arg)) {
		send_to_char("Only numeric values are allowed\r\n", ch);
		return;
	}

	lv_value = atoi(arg);

	/* IS VALUE HIGHER THAN YOUR LEVEL? */
	if (lv_value > GET_LEVEL(ch)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 0 and %d\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 45) {
		send_to_char("Value must be between 0 and 45\r\n", ch);
		return;
	}

	gv_auto_level = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s has set AUTOLEVEL to %d.\r\n",
		GET_REAL_NAME(ch), gv_auto_level);
	do_sys(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);
	bzero(buf, sizeof(buf));
	sprintf(buf, "You have set AUTOLEVEL to %d.\r\n", gv_auto_level);
	send_to_char(buf, ch);
	if (gv_auto_level)
		send_to_char("New players logging onto system will be advanced to that level.\r\n", ch);
	else
		send_to_char("Autolevel turned off.\r\n", ch);

	return;

}				/* END OF do_set_auto_level */


void do_set_channel(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	unsigned long int lv_channel_flag;
	int user_wants_all;
	int idx;

	/* STRIP THE CHANNEL NAME */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	lv_channel_flag = 0;
	user_wants_all = FALSE;
	if ((*buf)) {
		if (is_abbrev(buf, "all")) {
			user_wants_all = TRUE;
		}
		if (is_abbrev(buf, "avatar")) {
			lv_channel_flag = PLR3_NOAVATAR;
		}
		if (is_abbrev(buf, "clan")) {
			lv_channel_flag = PLR3_NOCLAN;
		}
		if (is_abbrev(buf, "connect")) {
			lv_channel_flag = PLR3_NOCONNECT;
		}
		if (is_abbrev(buf, "auction")) {
			lv_channel_flag = PLR3_NOAUCTION;
		}
		if (is_abbrev(buf, "pkflame")) {
			lv_channel_flag = PLR3_NOPKFLAME;
		}
		if (is_abbrev(buf, "gossip")) {
			lv_channel_flag = PLR3_NOGOSSIP;
		}
		if (is_abbrev(buf, "spirit")) {
			lv_channel_flag = PLR4_NOSPIRIT;
		}
		if (is_abbrev(buf, "imm")) {
			lv_channel_flag = PLR3_NOIMM;
		}
		if (is_abbrev(buf, "info")) {
			lv_channel_flag = PLR3_NOINFO;
		}
		if (is_abbrev(buf, "shout")) {
			lv_channel_flag = PLR3_NOSHOUT;
		}
		if (is_abbrev(buf, "system")) {
			lv_channel_flag = PLR3_NOSYSTEM;
		}
		if (is_abbrev(buf, "tell")) {
			lv_channel_flag = PLR3_NOTELL;
		}
		if (is_abbrev(buf, "wizinfo")) {
			lv_channel_flag = PLR3_NOWIZINFO;
		}
		if (is_abbrev(buf, "music")) {
			lv_channel_flag = PLR3_NOMUSIC;
		}
	}

	/* DID WE GET A VALID CHANNEL? */
	if (!(lv_channel_flag) && user_wants_all != TRUE) {
		send_to_char("Which channel do you want? (ALL is supported)\r\n", ch);
		send_to_char("Valid channels:\r\n    AVATAR, AUCTION, BOSS, CLAN, MUSIC,\r\n    CONNECT, PKFLAME, STAFF, GOSSIP, IMMTALK\r\n    INFO, SHOUT, SYSTEM, TELL, WIZINFO, and SPIRIT.\r\n", ch);
		return;
	}

	/* LOOK FOR ON OR OFF */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	/* DAMN ROUTINE one_argument WON'T RETURN "on" */
	strcpy(buf, arg);
	/* FIND FIRST SPACE */
	for (idx = 0; buf[idx] != ' ' &&
	     buf[idx] != 0 &&
	     idx < sizeof(buf); idx++);

	/* NULL TERMINATE AT END OF FIRST ARGUMENT AND ADJUST ARG */
	if (buf[idx] == ' ') {
		buf[idx] = 0;
		strcpy(arg, arg + idx);
	}

	if (!(*buf)) {
		if (user_wants_all == TRUE) {
			bzero(buf, sizeof(buf));
			if (!IS_SET(GET_ACT3(ch), PLR3_NOAVATAR))
				strcat(buf, "AVATAR ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOCLAN))
				strcat(buf, "CLAN ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOCONNECT))
				strcat(buf, "CONNECT ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOAUCTION))
				strcat(buf, "AUCTION ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOPKFLAME))
				strcat(buf, "PKFLAME ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOGOSSIP))
				strcat(buf, "GOSSIP ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOIMM))
				strcat(buf, "IMMORT ");
			if (!IS_SET(GET_ACT4(ch), PLR4_NOSPIRIT))
				strcat(buf, "SPIRIT");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOINFO))
				strcat(buf, "INFO ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOSHOUT))
				strcat(buf, "SHOUT ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOSYSTEM))
				strcat(buf, "SYSTEM ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOTELL))
				strcat(buf, "TELL ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOWIZINFO))
				strcat(buf, "WIZINFO ");
			if (!IS_SET(GET_ACT3(ch), PLR3_NOMUSIC))
				strcat(buf, "MUSIC ");

			if (!(*buf))
				strcpy(buf, "none.");

			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Enabled channels: %s\r\n", buf);
			send_to_char(buf2, ch);
			return;
		}

		if (lv_channel_flag == PLR4_NOSPIRIT) {
			if ((IS_SET(GET_ACT4(ch), lv_channel_flag))) {
				sprintf(buf, "Channel disabled.  Use set channel <channel>  ON or OFF.\r\n");
			}
			else {
				sprintf(buf, "Channel enabled.  Use set channel <channel> ON or OFF.\r\n");
			}
			send_to_char(buf, ch);
			return;
		}
		else {
			if ((IS_SET(GET_ACT3(ch), lv_channel_flag))) {
				sprintf(buf, "Channel disabled. Use set channel <channel> ON or OFF.\r\n");
			}
			else {
				sprintf(buf, "Channel enabled. Use set channel <channel> ON or OFF.\r\n");
			}
			send_to_char(buf, ch);
			return;
		}
	}


	if (is_abbrev(buf, "on")) {
		if (user_wants_all == TRUE) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOCONNECT);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOCLAN);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOAVATAR);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOAUCTION);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOPKFLAME);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOGOSSIP);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOIMM);
			REMOVE_BIT(GET_ACT4(ch), PLR4_NOSPIRIT);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOINFO);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOSYSTEM);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOTELL);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOWIZINFO);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOMUSIC);
			send_to_char("All channels turned on.\r\n", ch);
			return;
		}
		if (user_wants_all == TRUE) {
			return;
		}

		if (lv_channel_flag == PLR4_NOSPIRIT) {
			if ((IS_SET(GET_ACT4(ch), lv_channel_flag))) {
				send_to_char("Channel enabled.\r\n", ch);
			}
			else
				send_to_char("Channel already enabled.\r\n", ch);
			REMOVE_BIT(GET_ACT4(ch), lv_channel_flag);
			return;
		}
		else {
			if ((IS_SET(GET_ACT3(ch), lv_channel_flag))) {
				send_to_char("Channel enabled.\r\n", ch);
			}
			else
				send_to_char("Channel already enabled.\r\n", ch);
			/* KEEP IN MIND THESE ARE NEGATIVE AND WE REVERSED FOR
			 * USERS */
			REMOVE_BIT(GET_ACT3(ch), lv_channel_flag);
			return;
		}


	}

	if (is_abbrev(buf, "off")) {
		if (user_wants_all == TRUE) {
			SET_BIT(GET_ACT3(ch), PLR3_NOAVATAR);
			SET_BIT(GET_ACT3(ch), PLR3_NOCLAN);
			SET_BIT(GET_ACT3(ch), PLR3_NOCONNECT);
			SET_BIT(GET_ACT3(ch), PLR3_NOAUCTION);
			SET_BIT(GET_ACT3(ch), PLR3_NOPKFLAME);
			SET_BIT(GET_ACT3(ch), PLR3_NOGOSSIP);
			SET_BIT(GET_ACT3(ch), PLR3_NOIMM);
			SET_BIT(GET_ACT4(ch), PLR4_NOSPIRIT);
			SET_BIT(GET_ACT3(ch), PLR3_NOINFO);
			SET_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
			SET_BIT(GET_ACT3(ch), PLR3_NOSYSTEM);
			SET_BIT(GET_ACT3(ch), PLR3_NOTELL);
			SET_BIT(GET_ACT3(ch), PLR3_NOWIZINFO);
			SET_BIT(GET_ACT3(ch), PLR3_NOMUSIC);
			send_to_char("All channels turned off.\r\n", ch);
			return;
		}

		if (lv_channel_flag == PLR4_NOSPIRIT) {
			if ((IS_SET(GET_ACT4(ch), lv_channel_flag))) {
				send_to_char("Channel already disabled.\r\n", ch);
			}
			else {
				send_to_char("Channel disabled.\r\n", ch);
				SET_BIT(GET_ACT4(ch), lv_channel_flag);
				return;
			}
		}
		else {
			if ((IS_SET(GET_ACT3(ch), lv_channel_flag))) {
				send_to_char("Channel already disabled.\r\n", ch);
			}
			else {
				send_to_char("Channel disabled.\r\n", ch);
				/* KEEP IN MIND THESE ARE NEGATIVE AND WE
				 * REVERSED FOR USERS */
				SET_BIT(GET_ACT3(ch), lv_channel_flag);
				return;
			}
		}
	}

	send_to_char("Please enter set channel <channel> ON or OFF\r\n", ch);
	return;

}				/* END OF do_set_channel() */


void do_set_flag(struct char_data * ch, char *arg, int cmd, struct char_data * victim)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], flag_name[80];

	unsigned long int lv_orig_flag, lv_temp_flag, lv_act1_flag, lv_act2_flag,
	  lv_act3_flag, lv_dsc1_flag, lv_wizperm_flag;
	int min_lev = 0, idx;

	if (IS_NPC(victim) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You cant set flags on NPC's\r\n", ch);
		return;
	}

	/* CHANGING ANOTHER PLAYER? */
	if (ch != victim) {
		/* VICTIM HIGHER IN LEVEL? */
		if (GET_LEVEL(ch) <= GET_LEVEL(victim) &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("That person is to awesome for you to mess with!\r\n", ch);
			return;
		}

		CHECK_LEVEL(IMO_IMM)
	}

	/* STRIP THE FLAG NAME */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

    lv_orig_flag = 0;
    lv_temp_flag = 0;
	lv_act1_flag = 0;
	lv_act2_flag = 0;
	lv_act3_flag = 0;
	lv_dsc1_flag = 0;
    lv_wizperm_flag = 0;

	if ((*buf)) {
		if (is_abbrev(buf, "ansi")) {
			lv_act3_flag = PLR3_ANSI;
			min_lev = 1;
			strcpy(flag_name, "ANSI");
		}
		if (is_abbrev(buf, "assist")) {
			lv_act3_flag = PLR3_ASSIST;
			min_lev = 1;
			strcpy(flag_name, "ASSIST");
			if (GET_LEVEL(victim) < IMO_IMM &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
		}
		if (is_abbrev(buf, "attacker")) {
			lv_act2_flag = PLR2_ATTACKER;
			min_lev = MAX_LEV + 1;
			strcpy(flag_name, "ATTACKER");
		}
		if (is_abbrev(buf, "autoaggr")) {
			lv_act3_flag = PLR3_AUTOAGGR;
			min_lev = 1;
			strcpy(flag_name, "AUTOAGGR");
			if (GET_LEVEL(victim) < IMO_IMM &&
			    GET_LEVEL(ch) < IMO_IMP &&
			    ch != victim) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
		}
		if (is_abbrev(buf, "autoassist")) {
			lv_act3_flag = PLR3_ASSIST;
			min_lev = 1;
			strcpy(flag_name, "AUTOASSIST");
		}
		if (is_abbrev(buf, "autogold")) {
			lv_act3_flag = PLR3_AUTOGOLD;
			min_lev = MAX_LEV + 1;
			strcpy(flag_name, "AUTOGOLD");
		}
		if (is_abbrev(buf, "autoloot")) {
			lv_act3_flag = PLR3_AUTOLOOT;
			min_lev = 1;
			strcpy(flag_name, "AUTOLOOT");
		}
		if (is_abbrev(buf, "autosplit")) {
			lv_act3_flag = PLR3_AUTOSPLIT;
			min_lev = 1;
			strcpy(flag_name, "AUTOSPLIT");
		}
		if (is_abbrev(buf, "brief")) {
			lv_act3_flag = PLR3_BRIEF;
			min_lev = 1;
			strcpy(flag_name, "BRIEF");
		}
		if (is_abbrev(buf, "can_finger")) {
			lv_dsc1_flag = DFLG1_CAN_FINGER;
			min_lev = 1;
			strcpy(flag_name, "CAN FINGER");
		}
		if (is_abbrev(buf, "canthunt")) {
			min_lev = IMO_IMP;
			lv_act1_flag = PLR1_CANT_HUNT;
			min_lev = IMO_IMP;
			strcpy(flag_name, "CANTHUNT");
		}
		if (is_abbrev(buf, "compact")) {
			lv_act3_flag = PLR3_COMPACT;
			min_lev = 1;
			strcpy(flag_name, "COMPACT");
		}

		/* NEW FLAG FOR PKILLERS */

		if (is_abbrev(buf, "pkillable")) {
			lv_act2_flag = PLR2_PKILLABLE;
			min_lev = IMO_IMP;
			strcpy(flag_name, "PKILLABLE");
		}

		if (is_abbrev(buf, "kickme")) {
			lv_act2_flag = PLR2_KICKME;
			min_lev = IMO_IMP;
			strcpy(flag_name, "KICK_ME");
		}


		if (is_abbrev(buf, "enforcer")) {
			lv_act3_flag = PLR3_ENFORCER;
			min_lev = IMO_IMM;
			strcpy(flag_name, "ENFORCER");
			if (GET_LEVEL(victim) < IMO_IMM &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
		}
		if (is_abbrev(buf, "freeze")) {
			lv_act2_flag = PLR2_FREEZE;
			min_lev = IMO_IMM;
			strcpy(flag_name, "FREEZE");
			/* YOU CAN'T DO THIS TO YOURSELF */
			if (ch == victim && GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("That wouldn't be wise.\r\n", ch);
				return;
			}
		}
		if (is_abbrev(buf, "nofollow")) {
			min_lev = IMO_IMP;
			lv_act1_flag = PLR1_NOFOLLOW;
			min_lev = IMO_IMP;
			strcpy(flag_name, "NOFOLLOW");
		}
		if (is_abbrev(buf, "nokill")) {
			lv_act1_flag = PLR1_NOKILL;
			min_lev = IMO_SPIRIT;
			strcpy(flag_name, "NOKILL");
		}
		if (is_abbrev(buf, "unfilter")) {
			lv_act1_flag = PLR1_UNFILTER;
			min_lev = 1;
			strcpy(flag_name, "UNFILTER");
		}
		if (is_abbrev(buf, "jailed")) {
			lv_act2_flag = PLR2_JAILED;
			min_lev = IMO_IMP;
			strcpy(flag_name, "JAILED");
		}
		if (is_abbrev(buf, "shownoexp")) {
			lv_act2_flag = PLR2_SHOWNOEXP;
			min_lev = 1;
			strcpy(flag_name, "SHOWNOEXP");
		}
		if (is_abbrev(buf, "muzzle_all")) {
			lv_act2_flag = PLR2_MUZZLE_ALL;
			min_lev = IMO_IMP;
			strcpy(flag_name, "MUZZLE_ALL");
		}
		if (is_abbrev(buf, "muzzle_beep")) {
			lv_act2_flag = PLR2_MUZZLE_BEEP;
			min_lev = IMO_IMP;
			strcpy(flag_name, "MUZZLE_BEEP");
		}
		if (is_abbrev(buf, "muzzle_shout")) {
			lv_act2_flag = PLR2_MUZZLE_SHOUT;
			min_lev = IMO_IMP;
			strcpy(flag_name, "MUZZLE_SHOUT");
		}
		if (is_abbrev(buf, "noavatar")) {
			lv_act3_flag = PLR3_NOAVATAR;
			min_lev = 1;
			strcpy(flag_name, "NOAVATAR");
		}
		if (is_abbrev(buf, "noauction")) {
			lv_act3_flag = PLR3_NOAUCTION;
			min_lev = 1;
			strcpy(flag_name, "NOAUCTION");
		}
		if (is_abbrev(buf, "noconnect")) {
			lv_act3_flag = PLR3_NOCONNECT;
			min_lev = 1;
			strcpy(flag_name, "NOCONNECT");
		}
		if (is_abbrev(buf, "noemote")) {
			lv_act2_flag = PLR2_NO_EMOTE;
			min_lev = IMO_IMM;
			strcpy(flag_name, "NO_EMOTE");
		}
		if (is_abbrev(buf, "nopkflame")) {
			lv_act3_flag = PLR3_NOPKFLAME;
			min_lev = 1;
			strcpy(flag_name, "NOPKFLAME");
		}
		if (is_abbrev(buf, "nostaff")) {
			lv_act2_flag = PLR2_NOSTAFF;
			min_lev = 1;
			strcpy(flag_name, "NOSTAFF");
		}
		if (is_abbrev(buf, "noboss")) {
			lv_act2_flag = PLR2_NOBOSS;
			min_lev = 1;
			strcpy(flag_name, "NOBOSS");
		}
		if (is_abbrev(buf, "nogossip")) {
			lv_act3_flag = PLR3_NOGOSSIP;
			min_lev = 1;
			strcpy(flag_name, "NOGOSSIP");
		}
		if (is_abbrev(buf, "nomusic")) {
			lv_act3_flag = PLR3_NOMUSIC;
			min_lev = 1;
			strcpy(flag_name, "NOMUSIC");
		}
		if (is_abbrev(buf, "nohassle")) {
			lv_act3_flag = PLR3_NOHASSLE;
			min_lev = IMO_SPIRIT;
			if ((GET_LEVEL(ch) > PK_LEV) ||
			    (ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[ch->in_room].zone].lord))) {
				min_lev = 1;
			}
			/* DON'T LET IMMS SET THIS ON MORTALS */
			if (GET_LEVEL(victim) < IMO_SPIRIT &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
			strcpy(flag_name, "NOHASSLE");
		}
		if (is_abbrev(buf, "noimm")) {
			lv_act3_flag = PLR3_NOIMM;
			min_lev = 1;
			strcpy(flag_name, "NOIMM");
		}
		if (is_abbrev(buf, "noinfo")) {
			lv_act3_flag = PLR3_NOINFO;
			min_lev = 1;
			strcpy(flag_name, "NOINFO");
		}
		if (is_abbrev(buf, "no_opponent_fmsg")) {
			lv_act2_flag = PLR2_NO_OPPONENT_FMSG;
			min_lev = 1;
			strcpy(flag_name, "NO_OPPONENT_FMSG");
		}
		if (is_abbrev(buf, "no_personal_fmsg")) {
			lv_act2_flag = PLR2_NO_PERSONAL_FMSG;
			min_lev = 1;
			strcpy(flag_name, "NO_PERSONAL_FMSG");
		}
		if (is_abbrev(buf, "no_bystander_fmsg")) {
			lv_act2_flag = PLR2_NO_BYSTANDER_FMSG;
			min_lev = 1;
			strcpy(flag_name, "NO_BYSTANDER_FMSG");
		}
		if (is_abbrev(buf, "noshout")) {
			lv_act3_flag = PLR3_NOSHOUT;
			min_lev = 0;
			strcpy(flag_name, "NOSHOUT");
		}
		if (is_abbrev(buf, "nosummon")) {
			lv_act3_flag = PLR3_NOSUMMON;
			min_lev = 1;
			strcpy(flag_name, "NOSUMMON");
		}
		if (is_abbrev(buf, "nosystem")) {
			lv_act3_flag = PLR3_NOSYSTEM;
			min_lev = 1;
			strcpy(flag_name, "NOSYSTEM");
		}
		if (is_abbrev(buf, "notell")) {
			lv_act3_flag = PLR3_NOTELL;
			min_lev = 1;
			strcpy(flag_name, "NOTELL");
		}
		if (is_abbrev(buf, "nosummon")) {
			lv_act3_flag = PLR3_NOSUMMON;
			min_lev = 1;
			strcpy(flag_name, "NOSUMMON");
		}
		if (is_abbrev(buf, "notell")) {
			lv_act3_flag = PLR3_NOTELL;
			min_lev = 1;
			strcpy(flag_name, "NOTELL");
		}
		if (is_abbrev(buf, "nowizinfo")) {
			lv_act3_flag = PLR3_NOWIZINFO;
			min_lev = 1;
			strcpy(flag_name, "NOWIZINFO");
		}
		if (is_abbrev(buf, "quest")) {
			lv_act2_flag = PLR2_QUEST;
			min_lev = 1;
			strcpy(flag_name, "QUEST");
		}
		if (is_abbrev(buf, "show_diag")) {
			lv_act1_flag = PLR1_SHOW_DIAG;
			min_lev = 2;
			strcpy(flag_name, "SHOW_DIAG");
		}
		if (is_abbrev(buf, "show_room")) {
			lv_act3_flag = PLR3_SHOW_ROOM;
			if (GET_LEVEL(victim) < IMO_LEV &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
			min_lev = IMO_LEV;
			strcpy(flag_name, "SHOW_ROOM");
		}
		if (is_abbrev(buf, "show_hp")) {
			lv_act3_flag = PLR3_SHOW_HP;
			min_lev = 1;
			strcpy(flag_name, "SHOW_HP");
		}
		if (is_abbrev(buf, "show_mana")) {
			lv_act3_flag = PLR3_SHOW_MANA;
			min_lev = 1;
			strcpy(flag_name, "SHOW_MANA");
		}
		if (is_abbrev(buf, "show_move")) {
			lv_act3_flag = PLR3_SHOW_MOVE;
			min_lev = 1;
			strcpy(flag_name, "SHOW_MOVE");
		}
		if (is_abbrev(buf, "show_name")) {
			lv_act2_flag = PLR2_SHOW_NAME;
			min_lev = 1;
			strcpy(flag_name, "SHOW_NAME");
		}
		if (is_abbrev(buf, "show_exits")) {
			lv_act3_flag = PLR3_SHOW_EXITS;
			min_lev = 1;
			strcpy(flag_name, "SHOW_EXITS");
		}
		if (is_abbrev(buf, "show_tick")) {
			lv_act2_flag = PLR2_SHOW_TICK;
			min_lev = 1;
			strcpy(flag_name, "SHOW_TICK");
		}
		if (is_abbrev(buf, "wimpy")) {
			if (GET_LEVEL(victim) < IMO_IMM &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't set this on mortals.\r\n", ch);
				return;
			}
			lv_act3_flag = PLR3_WIMPY;
			min_lev = 1;
			strcpy(flag_name, "WIMPY");
		}
		if (is_abbrev(buf, "mediator")) {
			lv_wizperm_flag = WIZ_MEDIATOR_PERM;
			if (victim == ch && GET_LEVEL(ch) < IMO_IMP) {
				co2900_send_to_char(ch, "You cannot set this on yourself.\r\n");
				return;
			}
			if (GET_LEVEL(victim) < IMO_IMM) {
				co2900_send_to_char(ch, "You cannot set this on mortals/players.\r\n");
			}
			min_lev = IMO_IMM;
			strcpy(flag_name, "MEDIATOR");
		}
		if (is_abbrev(buf, "builder")) {
			lv_wizperm_flag = WIZ_BUILDER_PERM;
			if (victim == ch && GET_LEVEL(ch) < IMO_IMP) {
				co2900_send_to_char(ch, "You cannot set this on yourself.\r\n");
				return;
			}
			if (GET_LEVEL(victim) < IMO_IMM) {
				co2900_send_to_char(ch, "You cannot set this on mortals/players.\r\n");
			}
			min_lev = IMO_IMM;
			strcpy(flag_name, "BUILDER");
		}
		if (is_abbrev(buf, "imm_questor")) {
			lv_wizperm_flag = WIZ_QUESTOR_PERM;
			if (victim == ch && GET_LEVEL(ch) < IMO_IMP) {
				co2900_send_to_char(ch, "You cannot set this on yourself.\r\n");
				return;
			}
			if (GET_LEVEL(victim) < IMO_IMM) {
				co2900_send_to_char(ch, "You cannot set this on mortals/players.\r\n");
			}
			min_lev = IMO_IMM;
			strcpy(flag_name, "IMM_QUESTOR");
		}
		if (is_abbrev(buf, "ruler")) {
			lv_wizperm_flag = WIZ_RULER_PERM;
			if (victim == ch && GET_LEVEL(ch) < IMO_IMP) {
				co2900_send_to_char(ch, "You cannot set this on yourself.\r\n");
				return;
			}
			if (GET_LEVEL(victim) < IMO_IMM) {
				co2900_send_to_char(ch, "You cannot set this on mortals/players.\r\n");
			}
			min_lev = IMO_IMM;
			strcpy(flag_name, "RULER");
		}
	}

	/* WHICH FLAG ARE WE COMPARING AGAINST */
	lv_temp_flag = 0;
	if (lv_act1_flag) {
		lv_orig_flag = GET_ACT1(victim);
		lv_temp_flag = lv_act1_flag;
	}
	if (lv_act2_flag) {
		lv_orig_flag = GET_ACT2(victim);
		lv_temp_flag = lv_act2_flag;
	}
	if (lv_act3_flag) {
		lv_orig_flag = GET_ACT3(victim);
		lv_temp_flag = lv_act3_flag;
	}
	if (lv_dsc1_flag) {
		if (IS_NPC(victim)) {
			send_to_char("You can't set this on NPC chars.\r\n", ch);
			return;
		}
		lv_orig_flag = GET_DSC1(victim);
		lv_temp_flag = lv_dsc1_flag;
	}
	if (lv_wizperm_flag) {
		lv_orig_flag = GET_WIZ_PERM(victim);
		lv_temp_flag = lv_wizperm_flag;
	}

	/* DID WE GET A VALID FLAG? */
	if (!(lv_temp_flag)) {
		send_to_char("Which flag do you want?\r\n", ch);
		bzero(buf, sizeof(buf));

		sprintf(buf, "%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n",
		 "Valid flags  ANSI          NOCONNECT          NOTELL      ",
		 "*KICKME      ASSIST       *NO_EMOTE           NOWIZINFO   ",
		 " COMPACT     AUTOAGGR      NOGOSSIP          *PKILLABLE   ",
		 " NOSUMMON    AUTOASSIST   *NOHASSLE           SHOW_TICK   ",
		 " SHOW_HP     AUTOLOOT      NOIMM             *SHOW_ROOM   ",
		 " SHOW_MANA   AUTOSPLIT     NOINFO             SHOW_MOVE   ",
		 " SHOW_EXITS  BRIEF         NO_OPPONENT_FMSG   SHOW_NAME   ",
		 " AUTOGOLD   *FREEZE        NO_PERSONAL_FMSG   SHOW_EXITS  ",
		 " *SHOW_DIAG  NOAUCTION     NO_BYSTANDER_FMSG  TOGGLE      ",
		 " QUEST      *MUZZLE_ALL    NOSHOUT            WIMPY       ",
		 " NOSTAFF    *MUZZLE_SHOUT  NOSYSTEM          *JAILED      ",
		 " NOBOS      *MUZZLE_BEEP   NOMUSIC           *NOFOLLOW    ",
		 "*NOKILL     *CANTHUNT      NOAVATAR           NOPKFLAME   ",
		 "*MEDIATOR   *IMM_QUESTOR  *RULER              CAN_FINGER  ",
		 "*BUILDER     *flags preceeded by a * are priviledged      ");

		send_to_char(buf, ch);
		return;
	}

	/* CAN WE SET THE FLAG? */
	if (GET_LEVEL(ch) < min_lev) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "Sorry, but you must be level %d to set this flag\r\n",
			min_lev);
		send_to_char(buf2, ch);
		return;
	}

	/* LOOK FOR ON OR OFF */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	/* DAMN ROUTINE one_argument WON'T RETURN "on" */
	strcpy(buf, arg);
	/* FIND FIRST SPACE */
	for (idx = 0; buf[idx] != ' ' &&
	     buf[idx] != 0 &&
	     idx < sizeof(buf); idx++);

	/* NULL TERMINATE AT END OF FIRST ARGUMENT AND ADJUST ARG */
	if (buf[idx] == ' ') {
		buf[idx] = 0;
		strcpy(arg, arg + idx);
	}

	if (!(*buf)) {
		bzero(buf2, sizeof(buf2));
		if ((IS_SET(lv_orig_flag, lv_temp_flag))) {
			sprintf(buf2, "Flag %s set.  Use set flag <flag>  ON or OFF.\r\n", flag_name);
		}
		else {
			sprintf(buf2, "Flag %s is NOT set. Use set flag <flag>  ON or OFF.\r\n", flag_name);
		}
		send_to_char(buf2, ch);
		return;
	}

	if (is_abbrev(buf, "on")) {
		bzero(buf2, sizeof(buf2));
		if ((IS_SET(lv_orig_flag, lv_temp_flag))) {
			sprintf(buf2, "Flag %s is already set", flag_name);
		}
		else {
			sprintf(buf2, "Flag %s has been set", flag_name);
		}
		if (ch != victim) {
			strcat(buf2, " for ");
			strcat(buf2, GET_REAL_NAME(victim));
		}
		strcat(buf2, ".\r\n");
		send_to_char(buf2, ch);

		SET_BIT(lv_orig_flag, lv_temp_flag);
		/* SET FLAG */
		if (lv_act1_flag)
			GET_ACT1(victim) = lv_orig_flag;
		if (lv_act2_flag)
			GET_ACT2(victim) = lv_orig_flag;
		if (lv_act3_flag)
			GET_ACT3(victim) = lv_orig_flag;
		if (lv_dsc1_flag)
			GET_DSC1(victim) = lv_orig_flag;
		if (lv_wizperm_flag)
			GET_WIZ_PERM(victim) = lv_orig_flag;
		return;
	}

	if (is_abbrev(buf, "off")) {
		if ((IS_SET(lv_orig_flag, lv_temp_flag))) {
			sprintf(buf2, "Flag %s has been removed", flag_name);
		}
		else {
			sprintf(buf2, "Flag %s has already been removed", flag_name);
		}
		if (ch != victim) {
			strcat(buf2, " for ");
			strcat(buf2, GET_REAL_NAME(victim));
		}
		strcat(buf2, ".\r\n");
		send_to_char(buf2, ch);

		REMOVE_BIT(lv_orig_flag, lv_temp_flag);
		/* SET FLAG */
		if (lv_act1_flag)
			GET_ACT1(victim) = lv_orig_flag;
		if (lv_act2_flag)
			GET_ACT2(victim) = lv_orig_flag;
		if (lv_act3_flag)
			GET_ACT3(victim) = lv_orig_flag;
		if (lv_dsc1_flag)
			GET_DSC1(victim) = lv_orig_flag;
		if (lv_wizperm_flag)
			GET_WIZ_PERM(victim) = lv_orig_flag;

		return;
	}

	send_to_char("Please enter set flag <flag> ON or OFF\r\n", ch);
	return;

}				/* END OF do_set_flag() */


/* Add user input to the 'current' string (as defined by d->str) */
void string_add(struct descriptor_data * d, char *str)
{

	char *scan, buf[MAX_STRING_LENGTH];
	int terminator = 0;
	char colorbuf[MAX_STRING_LENGTH];

	/* determine if this is the terminal string, and truncate if so */
	for ((scan = str); *scan; scan++) {
		if (*scan == '@' && strlen(str) == 1)
			terminator = TRUE;
		else
			terminator = FALSE;

		if (terminator) {
			*scan = '\0';
			break;
		}
	}			/* END OF for loop */

	if (!(*d->str)) {
		if (strlen(str) > d->max_str) {
			send_to_char("String too long - Truncated.\r\n",
				     d->character);
			*(str + d->max_str) = '\0';
			terminator = 1;
		}
		CREATE(*d->str, char, strlen(str) + 3);
		strcpy(*d->str, str);
	}
	else {
		if (strlen(str) + strlen(*d->str) > d->max_str) {
			send_to_char("String too long. Last line skipped.\r\n",
				     d->character);
			terminator = 1;
		}
		else {
			if (!(*d->str = (char *) realloc(*d->str, strlen(*d->str) +
							 strlen(str) + 3))) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR:string_add\r\n");
				perror(buf);
				ABORT_PROGRAM();
			}
			strcat(*d->str, str);
		}
	}

	if (terminator) {
		d->str = 0;
		if (d->connected == CON_GET_DESCRIPTION) {
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			d->connected = CON_GET_MENU_SELECTION;
		}
	}
	else
		strcat(*d->str, "\r\n");
}


#undef MAX_STR

/* interpret an argument for do_string */
void quad_arg(char *arg, int *type, char *name, int *field, char *string)
{
	char buf[MAX_STRING_LENGTH];

	/* determine type */
	arg = one_argument(arg, buf);
	if (is_abbrev(buf, "char"))
		*type = TP_MOB;
	else if (is_abbrev(buf, "obj"))
		*type = TP_OBJ;
	else {
		*type = TP_ERROR;
		return;
	}

	/* find name */
	arg = one_argument(arg, name);

	/* field name and number */
	arg = one_argument(arg, buf);
	if (!(*field = old_search_block(buf, 0, strlen(buf), string_fields, 0)))
		return;

	/* string */
	for (; isspace(*arg); arg++);
	for (; (*string = *arg); arg++, string++);

	return;
}


void do_string(struct char_data * ch, char *arg, int cmd)
{
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH],
	  string[MAX_STRING_LENGTH];
	int field, type;
	struct char_data *mob;
	struct obj_data *obj;
	struct extra_descr_data *ed, *tmp;

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s entered string %s.", GET_NAME(ch), arg);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

	quad_arg(arg, &type, name, &field, string);

	if (type == TP_ERROR) {
		send_to_char("Syntax:\r\nstring ('obj'|'char') <name> <field> [<string>].", ch);
		return;
	}

	if (!field) {
		send_to_char("No field by that name. Try 'help string'.\r\n",
			     ch);
		return;
	}

	if (type == TP_MOB) {

		/* locate the beast */
		if (cmd != CMD_ADMIN) {
			if (!(mob = ha3100_get_char_vis(ch, name))) {
				send_to_char("I don't know anyone by that name...\r\n", ch);
				return;
			}
		}
		else
			mob = ch->desc->admin;
		if (!mob)
			return;

		switch (field) {
		case 1:
			if (!IS_NPC(mob)) {
				//&&GET_LEVEL(ch) < IMO_IMP
					send_to_char("You can't change that field for players.", ch);
				return;
			}
			ch->desc->str = IS_NPC(mob) ? &(mob->player.name) : &(mob->player.name);
			if (!IS_NPC(mob))
				send_to_char("WARNING: You have changed the name of a player.\r\n", ch);
			break;
		case 2:
			if (!IS_NPC(mob)) {
				send_to_char("That field is for monsters only.\r\n", ch);
				return;
			}
			ch->desc->str = &mob->player.short_descr;
			break;
		case 3:
			if (!IS_NPC(mob)) {
				send_to_char("That field is for monsters only.\r\n", ch);
				return;
			}
			ch->desc->str = &mob->player.long_descr;
			break;
		case 4:
			if (!IS_NPC(mob) && GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't change that field for players.", ch);
				return;
			}
			ch->desc->str = &mob->player.description;
			break;
		case 5:
			if (IS_NPC(mob)) {
				send_to_char("Monsters have no titles.\r\n",
					     ch);
				return;
			}
			ch->desc->str = &mob->player.title;
			break;
		default:
			send_to_char("That field is undefined for monsters.\r\n", ch);
			return;
			break;
		}
	}
	else {			/* type == TP_OBJ */
		/* locate the object */
		if (!(obj = ha3200_get_obj_vis(ch, name))) {
			send_to_char("Can't find such a thing here..\r\n", ch);
			return;
		}
		if ((obj = ha2075_get_obj_list_vis(ch, name, ch->carrying)) == NULL) {

			send_to_char("You do not have that item.\n\r", ch);
			return;
		}
		switch (field) {
		case 1:
			ch->desc->str = &obj->name;
			break;
		case 2:
			ch->desc->str = &obj->short_description;
			break;
		case 3:
			ch->desc->str = &obj->description;
			break;
		case 4:
			if (!*string) {
				send_to_char("You have to supply a keyword.\r\n", ch);
				return;
			}
			/* try to locate extra description */
			for (ed = obj->ex_description;; ed = ed->next)
				if (!ed) {	/* the field was not found.
						 * create a new one. */
					CREATE(ed, struct extra_descr_data, 1);
					ed->next = obj->ex_description;
					obj->ex_description = ed;
					CREATE(ed->keyword, char, strlen(string) + 1);
					strcpy(ed->keyword, string);
					ed->description = 0;
					ch->desc->str = &ed->description;
					send_to_char("New field.\r\n", ch);
					break;
				}
				else if (!str_cmp(ed->keyword, string)) {	/* the field exists */
					free(ed->description);
					ha9900_sanity_check(0, "FREE91", "SYSTEM");
					ed->description = 0;
					ch->desc->str = &ed->description;
					send_to_char(
					    "Modifying description.\r\n", ch);
					break;
				}
			ch->desc->max_str = MAX_STRING_LENGTH;
			return;	/* the stndrd (see below) procedure does not
				 * apply here */
			break;
		case 6:
			if (!*string) {
				send_to_char("You must supply a field name.\r\n", ch);
				return;
			}
			/* try to locate field */
			for (ed = obj->ex_description;; ed = ed->next)
				if (!ed) {
					send_to_char("No field with that keyword.\r\n", ch);
					return;
				}
				else if (!str_cmp(ed->keyword, string)) {
					free(ed->keyword);
					if (ed->description) {
						free(ed->description);
					}
					/* there are two frees covered by this
					 * one */
					ha9900_sanity_check(0, "FREE92", "SYSTEM");

					/* delete the entry in the desr list */
					if (ed == obj->ex_description)
						obj->ex_description = ed->next;
					else {
						for (tmp = obj->ex_description; tmp->next != ed;
						     tmp = tmp->next);
						tmp->next = ed->next;
					}
					free(ed);
					ha9900_sanity_check(0, "FREE93", "SYSTEM");

					send_to_char("Field deleted.\r\n", ch);
					return;
				}
			break;
		default:
			send_to_char(
			      "That field is undefined for objects.\r\n", ch);
			return;
			break;
		}
	}

	if (*ch->desc->str) {
		free(*ch->desc->str);
		ha9900_sanity_check(0, "FREE94", "SYSTEM");
	}

	if (*string) {		/* there was a string in the argument array */
		bzero(buf2, sizeof(buf2));
		strncpy(buf2, string, sizeof(buf2)-1);
		if (strlen(buf2) > length[field - 1]) {
			send_to_char("String too long - truncated.\r\n", ch);
			*(string + length[field - 1]) = '\0';
		}
		CREATE(*ch->desc->str, char, strlen(string) + 1);
		strcpy(*ch->desc->str, string);
		ch->desc->str = 0;
		send_to_char("Ok.\r\n", ch);
	}
	else {			/* there was no string. enter string mode */
		send_to_char("Enter string. terminate with '@'.\r\n", ch);
		*ch->desc->str = 0;
		ch->desc->max_str = length[field - 1];
	}
}







/* **********************************************************************
*  Modification of character skills                                     *
********************************************************************** */

/* db stuff *********************************************** */

/* One_Word is like one_argument, execpt that words in quotes "" are */
/* regarded as ONE word                                              */

char *one_word(char *argument, char *first_arg)
{
	int found, begin, look_at;

	found = begin = 0;

	do {
		for (; isspace(*(argument + begin)); begin++);

		if (*(argument + begin) == '\"') {	/* is it a quote */

			begin++;

			for (look_at = 0; (*(argument + begin + look_at) >= ' ') &&
			   (*(argument + begin + look_at) != '\"'); look_at++)
				*(first_arg + look_at) = LOWER(*(argument + begin + look_at));

			if (*(argument + begin + look_at) == '\"')
				begin++;

		}
		else {

			for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
				*(first_arg + look_at) = LOWER(*(argument + begin + look_at));

		}

		*(first_arg + look_at) = '\0';
		begin += look_at;
	}
	while (fill_word(first_arg));

	return (argument + begin);
}


struct help_index_element *build_help_index(FILE * fl, int *num)
{
	int nr = -1, issorted, i;
	struct help_index_element *list = 0, mem;
	char buf[81], tmp[81], *scan;
	long pos;

	for (;;) {
		pos = ftell(fl);
		fgets(buf, 81, fl);
		*(buf + strlen(buf) - 1) = '\0';
		scan = buf;
		for (;;) {
			/* extract the keywords */
			scan = one_word(scan, tmp);

			if (!*tmp)
				break;

			if (!list) {
				CREATE(list, struct help_index_element, 1);
				nr = 0;
			}
			else
				RECREATE(list, struct help_index_element, ++nr + 1);

			list[nr].pos = pos;
			CREATE(list[nr].keyword, char, strlen(tmp) + 1);
			strcpy(list[nr].keyword, tmp);
		}
		/* skip the text */
		do
			fgets(buf, 81, fl);
		while (*buf != '#');
		if (*(buf + 1) == '~')
			break;
	}
	/* we might as well sort the stuff */
	do {
		issorted = 1;
		for (i = 0; i < nr; i++)
			if (str_cmp(list[i].keyword, list[i + 1].keyword) > 0) {
				mem = list[i];
				list[i] = list[i + 1];
				list[i + 1] = mem;
				issorted = 0;
			}
	}
	while (!issorted);

	*num = nr;
	return (list);
}


/*********************************************************************
* New Pagination Code
* Michael Buselli submitted the following code for an enhanced pager
* for CircleMUD.  All functions below are his.  --JE 8 Mar 96
*
*********************************************************************/

/* Traverse down the string until the begining of the next page has been
* reached.  Return NULL if this is the last page of the string.
*/
char *next_page(char *str, struct descriptor_data * d)
{
	int col = 1, line = 1, spec_code = FALSE;

	for (;; str++) {
		/* If end of string, return NULL. */
		int temp = (d->character && d->character->screen_lines > 0) ?
		d->character->screen_lines : 50;
		if (*str == '\0')
			return (NULL);

		/* If we're at the start of the next page, return this fact. */
		else if (line > temp)
			return (str);

		/* Check for the begining of an ANSI color code block. */
		else if (*str == '\x1B' && !spec_code)
			spec_code = TRUE;

		/* Check for the end of an ANSI color code block. */
		else if (*str == 'm' && spec_code)
			spec_code = FALSE;

		/* Check for everything else. */
		else if (!spec_code) {
			/* Carriage return puts us in column one. */
			if (*str == '\r')
				col = 1;
			/* Newline puts us on the next line. */
			else if (*str == '\n')
				line++;

			/* We need to check here and see if we are over the
			 * page width, and if so, compensate by going to the
			 * begining of the next line. */
			else if (col++ > 80) {
				col = 1;
				line++;
			}
		}
	}
}


/* Function that returns the number of pages in the string. */
int count_pages(char *str, struct descriptor_data * d)
{
	int pages;

	for (pages = 1; (str = next_page(str, d)); pages++);
	return (pages);
}


/* This function assigns all the pointers for showstr_vector for the
* page_string function, after showstr_vector has been allocated and
* showstr_count set.
*/
void paginate_string(char *str, struct descriptor_data * d)
{
	int i;

	if (d->showstr_count)
		*(d->showstr_vector) = str;

	for (i = 1; i < d->showstr_count && str; i++)
		str = d->showstr_vector[i] = next_page(str, d);

	d->showstr_page = 0;
}


/* The call that gets the paging ball rolling... */
void page_string(struct descriptor_data * d, char *str, int keep_internal)
{
	char actbuf[MAX_INPUT_LENGTH] = "";

	if (!d)
		return;

	if (!str || !*str)
		return;

	d->showstr_count = count_pages(str, d);
	CREATE(d->showstr_vector, char *, d->showstr_count);

	if (keep_internal) {
		d->showstr_head = strdup(str);
		paginate_string(d->showstr_head, d);
	}
	else
		paginate_string(str, d);

	show_string(d, actbuf);
}


/* The call that displays the next page. */
void show_string(struct descriptor_data * d, char *input)
{
	char buffer[MAX_STRING_LENGTH], buf[MAX_INPUT_LENGTH];
	int diff;

	any_one_arg(input, buf);

	/* Q is for quit. :) */
	if (LOWER(*buf) == 'q') {
		free(d->showstr_vector);
		d->showstr_vector = NULL;
		d->showstr_count = 0;
		if (d->showstr_head) {
			free(d->showstr_head);
			d->showstr_head = NULL;
		}
		return;
	}
	/* R is for refresh, so back up one page internally so we can display
	 * it again. */
	else if (LOWER(*buf) == 'r')
		d->showstr_page = MAXV(0, d->showstr_page - 1);

	/* B is for back, so back up two pages internally so we can display the
	 * correct page here. */
	else if (LOWER(*buf) == 'b')
		d->showstr_page = MAXV(0, d->showstr_page - 2);

	/* Feature to 'goto' a page.  Just type the number of the page and you
	 * are there! */
	else if (isdigit(*buf))
		d->showstr_page = MAXV(0, MINV(atoi(buf) - 1, d->showstr_count - 1));

	else if (*buf) {
		co2900_send_to_char(d->character,
				    "Valid commands while paging are RETURN, Q, R, B, or a numeric value.\r\n");
		return;
	}
	/* If we're displaying the last page, just send it to the character,
	 * and then free up the space we used. */
	if (d->showstr_page + 1 >= d->showstr_count) {
		co2900_send_to_char(d->character, "%s", d->showstr_vector[d->showstr_page]);
		free(d->showstr_vector);
		d->showstr_vector = NULL;
		d->showstr_count = 0;
		if (d->showstr_head) {
			free(d->showstr_head);
			d->showstr_head = NULL;
		}
	}
	/* Or if we have more to show.... */
	else {
		diff = d->showstr_vector[d->showstr_page + 1] - d->showstr_vector[d->showstr_page];
		if (diff > MAX_STRING_LENGTH - 3)	/* 3=\r\n\0 */
			diff = MAX_STRING_LENGTH - 3;
		strncpy(buffer, d->showstr_vector[d->showstr_page], diff);	/* strncpy: OK (size
										 * truncated above) */
		/*
	         * Fix for prompt overwriting last line in compact mode submitted by
	         * Peter Ajamian <peter@pajamian.dhs.org> on 04/21/2001
	         */
		if (buffer[diff - 2] == '\r' && buffer[diff - 1] == '\n')
			buffer[diff] = '\0';
		else if (buffer[diff - 2] == '\n' && buffer[diff - 1] == '\r')
			/* This is backwards.  Fix it. */
			strcpy(buffer + diff - 2, "\r\n");	/* strcpy: OK (size
								 * checked) */
		else if (buffer[diff - 1] == '\r' || buffer[diff - 1] == '\n')
			/* Just one of \r\n.  Overwrite it. */
			strcpy(buffer + diff - 1, "\r\n");	/* strcpy: OK (size
								 * checked) */
		else
			/* Tack \r\n onto the end to fix bug with prompt
			 * overwriting last line. */
			strcpy(buffer + diff, "\r\n");	/* strcpy: OK (size
							 * checked) */
		co2900_send_to_char(d->character, "%s", buffer);
		d->showstr_page++;
	}
}


//Ignore a player
void do_ignore(struct char_data * ch, char *arg, int cmd)
{

	int idx;
	char buf[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];

	if (!IS_PC(ch))
		return;

	one_argument(arg, name);

	if (!*name) {
		send_to_char("Currently ignored players:\r\n", ch);
		send_to_char("--------------------------\r\n", ch);
		for (idx = 0; idx < 20; idx++) {
			if (ch->desc->ignore[idx]) {
				sprintf(buf, "%s\r\n", ch->desc->ignore[idx]);
				send_to_char(buf, ch);
			}
		}
		send_to_char("\r\n", ch);
		return;
	}

	if (!is_ignored(ch, name)) {
		for (idx = 0; (idx < 20) && ch->desc->ignore[idx]; idx++);

		if (idx == 20) {
			send_to_char("You can't ignore more than 20 players at a time..\r\n", ch);
			return;
		}

		ch->desc->ignore[idx] = strdup(name);
		sprintf(buf, "%s is now being ignored.\r\n", name);
		send_to_char(buf, ch);
	}
	else {
		for (idx = 0; idx < 20; idx++) {
			if (ch->desc->ignore[idx]) {
				if (ha1175_isexactname(ch->desc->ignore[idx], name)) {
					sprintf(buf, "%s removed from ignore list.\r\n", name);
					send_to_char(buf, ch);
					free(ch->desc->ignore[idx]);
					ch->desc->ignore[idx] = 0;
					return;
				}
			}
		}

		send_to_char("modify.c (do_ignore) - Shouldn't be here....\r\n", ch);
	}

	return;
}


bool is_ignored(struct char_data * ch, char *name)
{

	int idx;
	if (IS_NPC(ch))
		return FALSE;

	for (idx = 0; idx < 20; idx++) {
		if (ch->desc->ignore[idx]) {
			if (ha1175_isexactname(ch->desc->ignore[idx], name))
				return TRUE;
		}
	}

	return FALSE;
}

void do_wiztitle(struct char_data * ch, char *arg, int cmd)
{

	int i, index, errorlvl, length, pos, exist = 0;

	char *title, buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], name[MAX_STRING_LENGTH];

	struct wiztitle_data *titles;

	const char *toggle[] = {
		"add",
		"set",
		"show",
		"remove",
		"list",
		"\n"
	};
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	for (; isspace(*arg); arg++);
	bzero(name, sizeof(name));
	arg = one_argument(arg, name);

	exist = does_player_exist(name);
	if (!exist) {
		send_to_char("No player around with that name.\r\n", ch);
		return;
	}
	if (*buf)
		i = (old_search_block(buf, 0, strlen(buf), toggle, 0));
	else
		i = -1;

	switch (i) {
	case 1:
		arg++;
		errorlvl = addwiztitle(name, arg);
		if (errorlvl == -1) {
			savewiztitles();
			send_to_char("Done.", ch);
			break;
		}
		else {
			send_to_char("not enough space in the string, or too many wiztitles.\n", ch);
			return;
		}
	case 2:
		arg++;
		errorlvl = setwiztitle(name, arg);
		if (errorlvl == -1) {
			savewiztitles();
			send_to_char("Done.", ch);
			break;
		}
		else {
			send_to_char("not enough space in the string, or too many wiztitles.\n", ch);
			return;
		}
	case 3:
		length = wiztitlecount(name);
		if (!length) {
			sprintf(buf1, "%s dosn't have a wiztitle set\r\n", name);
			send_to_char(buf1, ch);
			return;
		}
		send_to_char("id wiztitle\n", ch);
		send_to_char("__ ______________\n\n", ch);

		for (pos = 0; pos < length; pos++) {


			title = getwiztitleindex(name, pos);

			sprintf(buf1, "%2d %-14s%s\r\n&n", pos, title, "]");

			send_to_char(buf1, ch);
		}
		break;
	case 4:
		arg++;
		if (!is_number(arg)) {
			send_to_char("Only numeric values are allowed\r\n", ch);
			break;
		}

		index = atoi(arg);
		removewiztitle(name, index);
		savewiztitles();
		break;
	case 5:
		send_to_char("name                  # wiztitle\n", ch);
		send_to_char("____________________ __ ______________\n\n", ch);

		for (titles = wiztitles; titles != NULL; titles = titles->next) {
			length = wiztitlecount(titles->name);
			title = getwiztitleindex(titles->name, 0);
			sprintf(buf, "%-20s %2d %-14s%s", titles->name, length, title, "]&n\n");
			send_to_char(buf, ch);
		}
		break;
	case -1:
		send_to_char("options are: 'add', 'set', 'show', and 'remove'.\n", ch);
		send_to_char("Usage is: wiztitle <option> <name> <title>\n", ch);
		break;
	default:
		send_to_char("options are: 'add', 'set', 'show', and 'remove'.\n", ch);
		send_to_char("Usage is: wiztitle <option> <name> <title>\n", ch);
		break;
	}
}
