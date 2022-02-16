 /* in *//* gv_location: 6501-7000 *//*
******************************************************************** *
file: info.c , Implem. of commands. Part of DIKUMUD * * Usage :
Informative commands.  * * Copyright (C) 1990, 1991 - see 'license.doc'
for complete infor.  *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "modify.h"
#include "ansi.h"
#include "weather.h"
#include "globals.h"
#include "func.h"

#define EXP_ACROSS 5
#define RACE_ACROSS 4
#define CLASS_ACROSS 3

void in1000_do_show_races(struct char_data * ch, char *arg, int cmd)
{
	int lv_row, lv_number, idx, lv_level, jdx;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	lv_row = (MAX_RACES / RACE_ACROSS);
	if (MAX_RACES % RACE_ACROSS)
		lv_row++;
	for (; isspace(*arg); arg++);

	if (!*arg) {
		send_to_char("\r\nRaces:\r\n------\r\n", ch);
		bzero(buf, sizeof(buf));

		for (idx = 0; idx < lv_row; idx++) {
			bzero(buf2, sizeof(buf));
			for (jdx = 0; jdx < RACE_ACROSS; jdx++) {
				lv_number = 1 + idx + (jdx * lv_row);
				if (lv_number >= MAX_RACES || !*races[lv_number].name ||
				    !(strncmp(races[lv_number].name, "undefined", 9))) {
					sprintf(buf2, "                   ");
				}
				else {
					sprintf(buf2, "%-2d. %-15s",
						lv_number,
						races[lv_number].name);
				}
				strcat(buf, buf2);
			}
			strcat(buf, "\r\n");
		}
		send_to_char(buf, ch);
		if (cmd != CMD_ADMIN)
			send_to_char("Enter HELP RACE <race> for details on that race\r\n", ch);
		return;
	}			/* END  OF NO ARGUMENT */

	/* IS VALUE NUMERIC */
	lv_number = 0;
	if (is_number(arg)) {
		lv_number = atoi(arg);
	}

	/* LOOK FOR THE RACE SPECIFIED IN ARGUMENT */
	if (lv_number == 0) {
		for (idx = 1; idx < MAX_RACES; idx++) {
			/* CONVERT TO LOWER CASE */
			bzero(buf, sizeof(buf));
			strcpy(buf, races[idx].name);
			for (jdx = 0; jdx < sizeof(buf); jdx++) {
				buf[jdx] = LOWER(buf[jdx]);
			}
			if (is_abbrev(arg, buf)) {
				lv_number = idx;
				idx = MAX_RACES;	/* so we'll leave the
							 * loop */
			}
		}
	}			/* END OF lv_number equal to zero */

	if (lv_number == 0 || lv_number >= MAX_RACES) {
		sprintf(buf, "Sorry, but I don't recognize %s as a race.\r\n",
			arg);
		send_to_char(buf, ch);
		return;
	}

	if (!(strncmp(races[lv_number].name, "undefined", 9))) {
		sprintf(buf, "Sorry, but %s is an undefined race.\r\n",
			arg);
		send_to_char(buf, ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "\r\nRACE: %s       Desc: %s    Size: ",
		races[lv_number].name,
		races[lv_number].desc);
	if (IS_SET(races[lv_number].flag, RFLAG_SIZE_SMALL))
		strcat(buf, "SMALL");
	else if (IS_SET(races[lv_number].flag, RFLAG_SIZE_NORMAL))
		strcat(buf, "NORMAL");
	else
		strcat(buf, "LARGE");
	strcat(buf, "\r\n\r\n");
	send_to_char(buf, ch);

	for (idx = 0; idx < 4; idx++) {
		jdx = strlen(races[lv_number].text[idx]);
		if (jdx > 0 && (strncmp(races[lv_number].text[idx], "none", 9))) {
			send_to_char(races[lv_number].text[idx], ch);
			send_to_char("\r\n", ch);
		}
	}
	send_to_char("\r\n", ch);
	sprintf(buf, "              STR INT WIS DEX CON CHA    HIT  |   MANA    MOVES\r\n");
	send_to_char(buf, ch);

	sprintf(buf, "Maximum      |%3d|%3d|%3d|%3d|%3d|%3d| %7d| %7d| %7d|\r\n",
		races[lv_number].max_str,
		races[lv_number].max_int,
		races[lv_number].max_wis,
		races[lv_number].max_dex,
		races[lv_number].max_con,
		races[lv_number].max_cha,
		races[lv_number].max_hit,
		races[lv_number].max_mana,
		races[lv_number].max_move);
	send_to_char(buf, ch);

	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));
	sprintf(buf, "Adjustments: ");
	if (races[lv_number].adj_str == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_str);
	strcat(buf, buf2);
	if (races[lv_number].adj_int == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_int);
	strcat(buf, buf2);
	if (races[lv_number].adj_wis == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_wis);
	strcat(buf, buf2);
	if (races[lv_number].adj_dex == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_dex);
	strcat(buf, buf2);
	if (races[lv_number].adj_con == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_con);
	strcat(buf, buf2);
	if (races[lv_number].adj_cha == 0)
		sprintf(buf2, "|   ");
	else
		sprintf(buf2, "|%+3d", races[lv_number].adj_cha);
	strcat(buf, buf2);

	if (races[lv_number].adj_hit == 0)
		sprintf(buf2, "|        ");
	else
		sprintf(buf2, "|%+3d/lev", races[lv_number].adj_hit);
	strcat(buf, buf2);
	if (races[lv_number].adj_mana == 0)
		sprintf(buf2, "|        ");
	else
		sprintf(buf2, "|%+3d/lev", races[lv_number].adj_mana);
	strcat(buf, buf2);
	if (races[lv_number].adj_move == 0)
		sprintf(buf2, "|        ");
	else
		sprintf(buf2, "|%+3d/lev", races[lv_number].adj_move);
	strcat(buf, buf2);
	strcat(buf, "|\r\n");
	send_to_char(buf, ch);

	sprintf(buf, "Minimum/Start|  3|  3|  3|  3|  3|  3| %7d| %7d| %7d|\r\n",
		races[lv_number].base_hit,
		races[lv_number].base_mana,
		races[lv_number].base_move);
	send_to_char(buf, ch);

	if (races[lv_number].adj_hitroll != 0) {
		sprintf(buf, "Hitroll is adjusted by %+5d\r\n",
			races[lv_number].adj_hitroll);
		send_to_char(buf, ch);
	}

	if (races[lv_number].adj_dmgroll != 0) {
		sprintf(buf, "Dmgroll is adjusted by %+5d\r\n",
			races[lv_number].adj_dmgroll);
		send_to_char(buf, ch);
	}

	if (races[lv_number].adj_ac != 0) {
		sprintf(buf, "AC is adjusted by %+5d\r\n",
			races[lv_number].adj_ac);
		send_to_char(buf, ch);
	}

	if (races[lv_number].regen_hit == 0) {
		sprintf(buf, "This race doesn't regen hits.\r\n");
	}
	else {
		sprintf(buf, "Hitpoint regeneration is %d%% of normal.\r\n",
			races[lv_number].regen_hit);
	}
	send_to_char(buf, ch);

	if (races[lv_number].regen_mana == 0) {
		sprintf(buf, "This race doesn't regen mana.\r\n");
	}
	else {
		sprintf(buf, "Mana regeneration is %d%% of normal.\r\n",
			races[lv_number].regen_mana);
	}
	send_to_char(buf, ch);

	if (races[lv_number].regen_move == 0) {
		sprintf(buf, "This race doesn't regen moves.\r\n");
	}
	else {
		sprintf(buf, "Movement regeneration is %d%% of normal.\r\n",
			races[lv_number].regen_move);
	}
	send_to_char(buf, ch);

	if (races[lv_number].adj_food == 0) {
		sprintf(buf, "This race doesn't require food.\r\n");
	}
	else {
		sprintf(buf, "Food usage is %d%% of normal with a max of %d.\r\n",
			races[lv_number].adj_food,
			races[lv_number].max_food);
	}
	send_to_char(buf, ch);

	if (races[lv_number].adj_thirst == 0) {
		sprintf(buf, "This race doesn't require drink.\r\n");
	}
	else {
		sprintf(buf, "Thirst usage is %d%% of normal with a max of %d.\r\n",
			races[lv_number].adj_thirst,
			races[lv_number].max_thirst);
	}
	send_to_char(buf, ch);

	if (races[lv_number].adj_drunk == 0) {
		sprintf(buf, "This race is always sober.\r\n");
	}
	else {
		sprintf(buf, "sobering is %d%% of normal with a max of %d.\r\n",
			races[lv_number].adj_drunk,
			races[lv_number].max_drunk);
	}
	send_to_char(buf, ch);

	send_to_char("Race has the following default spells:\r\n", ch);
	jdx = 0;
	for (idx = 0; idx < MAX_RACE_ATTRIBUTES; idx++) {
		if (races[lv_number].perm_spell[idx] > 0) {
			sprintf(buf, "    %s\r\n",
			   spell_names[races[lv_number].perm_spell[idx] - 1]);
			send_to_char(buf, ch);
			jdx++;
		}
	}
	if (jdx == 0) {
		send_to_char("    none\r\n", ch);
	}

	send_to_char("Race gets the following skills/spells:\r\n", ch);
	jdx = 0;
	for (lv_level = 1; lv_level < IMO_LEV; lv_level++) {
		for (idx = 1; idx < MAX_SKILLS; idx++) {
			if (races[lv_number].skill_min_level[idx] == lv_level) {
				sprintf(buf, "  %d.  %-20s  level: %d\r\n",
					idx,
					spell_names[idx - 1],
					races[lv_number].skill_min_level[idx]);
				send_to_char(buf, ch);
				jdx++;
			}
		}
	}
	if (jdx == 0) {
		send_to_char("    none\r\n", ch);
	}

	send_to_char("Race has the following limits on skills/spells:\r\n", ch);
	jdx = 0;
	for (idx = 1; idx < MAX_SKILLS; idx++) {
		if (races[lv_number].skill_max[idx] != 99) {
			sprintf(buf, "    %15s  proficiency: %d\r\n",
			spell_names[idx - 1], races[lv_number].skill_max[idx]);
			send_to_char(buf, ch);
			jdx++;
		}
	}
	if (jdx == 0) {
		send_to_char("    none\r\n", ch);
	}

	return;

}				/* END OF in1000_do_show_races() */


void in1100_do_show_classes(struct char_data * ch, char *arg, int cmd)
{

	int lv_row, lv_number, lv_level, lv_race, idx, jdx;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];



	/* HOW MANY CAN WE PRINT DOWN? */
	lv_row = (MAX_CLASSES / CLASS_ACROSS);
	lv_race = GET_RACE(ch);

	for (; isspace(*arg); arg++);

	if (!*arg) {
		send_to_char("Classes:\r\n--------\r\n", ch);
		bzero(buf, sizeof(buf));

		for (idx = 0; idx < lv_row; idx++) {
			bzero(buf2, sizeof(buf));
			for (jdx = 0; jdx < CLASS_ACROSS; jdx++) {
				lv_number = 1 + idx + (jdx * lv_row);
				if (!*classes[lv_number].name ||
				    !(strncmp(classes[lv_number].name, "undefined", 9))) {
					sprintf(buf2, "                          ");
				}
				else {
					if (!check_class_race(lv_number, lv_race))
						sprintf(buf2, "    &r%-20s&n  ", classes[lv_number].name);
					else
						sprintf(buf2, "%-2d. &g%-20s&n  ", lv_number, classes[lv_number].name);
				}
				strcat(buf, buf2);
			}
			strcat(buf, "\r\n");
		}
		send_to_char(buf, ch);
		if (cmd != CMD_ADMIN)
			send_to_char("Enter HELP CLASS <class> for details on that class.\r\n", ch);
		return;
	}			/* END  OF NO ARGUMENT */

	/* IS VALUE NUMERIC */
	lv_number = 0;
	if (is_number(arg)) {
		lv_number = atoi(arg);
	}

	/* LOOK FOR THE CLASS SPECIFIED IN ARGUMENT */
	if (lv_number == 0) {
		for (idx = 1; idx < MAX_CLASSES; idx++) {
			/* CONVERT TO LOWER CASE */
			bzero(buf, sizeof(buf));
			strcpy(buf, classes[idx].name);
			for (jdx = 0; jdx < sizeof(buf); jdx++) {
				buf[jdx] = LOWER(buf[jdx]);
			}
			if (is_abbrev(arg, buf)) {
				lv_number = idx;
				idx = MAX_CLASSES;	/* so we'll leave the
							 * loop */
			}
		}
	}			/* END OF lv_number equal to zero */

	if (lv_number == 0 || lv_number >= MAX_CLASSES) {
		sprintf(buf, "Sorry, but I don't recognize %s as a class.\r\n",
			arg);
		send_to_char(buf, ch);
		return;
	}

	if (!(strncmp(classes[lv_number].name, "undefined", 9))) {
		sprintf(buf, "Sorry, but %s is an undefined class.\r\n",
			arg);
		send_to_char(buf, ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "\r\nCLASS: %s       Desc: %s\r\n\r\n",
		classes[lv_number].name, classes[lv_number].desc);
	send_to_char(buf, ch);

	for (idx = 0; idx < 4; idx++) {
		jdx = strlen(classes[lv_number].text[idx]);
		if (jdx > 0 && (strncmp(classes[lv_number].text[idx], "none", 9))) {
			send_to_char(classes[lv_number].text[idx], ch);
			send_to_char("\r\n", ch);
		}
	}
	send_to_char("\r\nStat adjustments (gained per level):\r\n", ch);
	sprintf(buf, "Hit : %3d%%\r\n", classes[lv_number].adj_hit);
	send_to_char(buf, ch);
	sprintf(buf, "Mana: %3d%%\r\n", classes[lv_number].adj_mana);
	send_to_char(buf, ch);
	sprintf(buf, "Move: %3d%%\r\n\r\n", classes[lv_number].adj_move);
	send_to_char(buf, ch);

	sprintf(buf, "THAC0 = 30.5 - (LEVEL * %d/%d)\r\n",
		classes[lv_number].thaco_numerator,
		classes[lv_number].thaco_denominator);
	send_to_char(buf, ch);

	send_to_char("\r\nClass has the following proficiency(max prof):\r\n", ch);
	send_to_char("  LVL SPELL/SKILL           LVL SPELL/SKILL           LVL SPELL/SKILL\r\n", ch);

	jdx = 0;
	lv_row = 0;
	bzero(buf2, sizeof(buf2));
	for (lv_level = 1; lv_level < IMO_LEV; lv_level++) {
		for (idx = 1; idx < MAX_SKILLS; idx++) {
			if (classes[lv_number].skill_min_level[idx] == lv_level) {
				bzero(buf, sizeof(buf));
				bzero(buf3, sizeof(buf));
				strcpy(buf3, spell_names[idx - 1]);
				buf3[13] = 0;
				sprintf(buf, "  %-2d  %s(%-2d%%)                     ",
				      classes[lv_number].skill_min_level[idx],
					buf3,
					classes[lv_number].skill_max[idx]);
				buf[26] = 0;
				lv_row++;
				strcat(buf2, buf);
				if (lv_row == 3) {
					strcat(buf2, "\r\n");
					send_to_char(buf2, ch);
					bzero(buf2, sizeof(buf2));
					lv_row = 0;
				}
				jdx++;
			}
		}		/* END OF idx loop */
	}			/* END OF lv_level loop */
	if (jdx > 0) {
		strcat(buf2, "\r\n");
		send_to_char(buf2, ch);
	}
	else {
		send_to_char("    none\r\n", ch);
	}
	return;
}				/* END OF in1100_do_show_classes() */


void in1200_do_color(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];



	bzero(buf, sizeof(buf));

	sprintf(buf, "%sBLACK %sRED %sGREEN %sBROWN %sBLUE %sPURPLE%s\r\n",
		BLACK, RED, GREEN, BROWN, BLUE, PURPLE, END);
	send_to_char(buf, ch);
	sprintf(buf, "%sCYAN %sDGRAY %sLRED %sLGREEN %sYELLOW%s\r\n",
		CYAN, DGRAY, LRED, LGREEN, YELLOW, END);
	send_to_char(buf, ch);
	sprintf(buf, "%sGRAY %sLBLUE %sLPURPLE %sLCYAN %sWHITE%s\r\n",
		GRAY, LBLUE, LPURPLE, LCYAN, WHITE, END);
	send_to_char(buf, ch);

	return;

}				/* END OF in1200_do_color() */


void do_xyzzy(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	int idx;

	/* if this is an IMP, and they've used LOOP as an argument twice */
	/* then enter one, else increment counter */
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg2);

	co2900_send_to_char(ch, "This command is dangerous and has been disabled!\r\n");
	return;

	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_IMP) {

		if (!(strcmp(arg2, "free"))) {
			send_to_char("Done.\r\n", ch);
			db7100_free_char(ch, 16);
			return;
		}

		/* SHOULD WE GO OFF AND REBUILD EQUIPMENT? */
		if (!(strcmp(arg2, "equipment"))) {
			ut9000_update_player_file(ch, "  ", cmd);
			return;
		}		/* END OF report */

		if (!(strcmp(arg2, "rebuild"))) {
			send_to_char("Rebuild is in progress.\r\n", ch);

			/* ROOMS */
			main_log("UPDATING ROOMS: ");
			for (idx = 0; idx <= top_of_zone_table; idx++) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "         rzone: %d %s.", idx, zone_table[idx].filename);
				main_log(buf);
				db9100_save_wld(idx);
			}

			/* MOBILES */
			main_log("UPDATING MOBILES: ");
			for (idx = 0; idx <= top_of_zone_table; idx++) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "         mzone: %d %s.", idx, zone_table[idx].filename);
				main_log(buf);
				db9300_save_mob(idx);
			}

			/* OBJECTS */
			main_log("UPDATING OBJECTS: ");
			for (idx = 0; idx <= top_of_zone_table; idx++) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "         ozone: %d %s.", idx, zone_table[idx].filename);
				main_log(buf);
				db9500_save_obj(idx);
			}

			/* ZONES */
			main_log("UPDATING ZONES: ");
			for (idx = 0; idx <= top_of_zone_table; idx++) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "         ozone: %d %s.", idx, zone_table[idx].filename);
				main_log(buf);
				db9000_save_zon(idx);
			}

			/* WE DON'T NEED THE SPECIAL ANYMORE */
			gv_rebuild = 0;
			return;
		}
		if (!(strcmp(arg2, "loop"))) {
			if (gv_force_infinate_loop != TRUE) {
				gv_force_infinate_loop = TRUE;
				main_log("At DEFCON 5 for infinite loop-next pass is WAR!");
				do_sys("At DEFCON 5 for infinite loop-next pass is WAR!", 1, ch);
			}
			else {
				main_log("Entering infinite loop");
			}
			return;
		}
		if (!(strcmp(arg2, "reporto"))) {
			do_obj_report(ch, arg, cmd);
			return;
		}		/* END OF report */

		if (!(strcmp(arg2, "reportm"))) {
			do_mob_report(ch, arg, cmd);
			return;
		}		/* END OF report */

		/* RESET DEFCON IF WE DIDN'T ENTER ANYTHING AND ITS SET */
		if (gv_force_infinate_loop == TRUE) {
			send_to_char("Returning to DEFCON 1 - safety.\r\n", ch);
			gv_force_infinate_loop = FALSE;
		}
	}

	if (gv_force_infinate_loop == TRUE)
		send_to_char("Nothing happens. But we are at DEFCON 5!\r\n", ch);
	else
		send_to_char("Nothing happens.\r\n", ch);

	return;

}				/* END OF do_xyzzy() */

void do_who(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char mortal_buf[MAX_STRING_LENGTH];
	char avat_buf[MAX_STRING_LENGTH];
	char imm_buf[MAX_STRING_LENGTH];
	char info_buf[MAX_STRING_LENGTH];
	char final_buf[MAX_STRING_LENGTH * 5];

	int looking_for_name = 0;
	int ch_level;
	int visible_count = 0;
	
	int have_mortals = 0, have_avats = 0, have_imms = 0;

	struct char_data *c;
	struct descriptor_data *d;

	/* Abort if called with no ch or a character with no descriptor */
	if (!ch || !ch->desc)
		return;

	/* Get the player's level, so imms can't switch into mobs and bypass
	 * invis/incog */
	ch_level = (ch->desc->original) ? GET_LEVEL(ch->desc->original) : GET_LEVEL(ch);

	/* Find arguments */
	while (isspace(*arg))
		arg++;

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (is_abbrev(buf, "name"))
		looking_for_name = 1;

	/* Setup the buffers */
	bzero(mortal_buf, sizeof(mortal_buf));
	strncat(mortal_buf,
			"&wPlayers\r\n"
			"-------&n\r\n",
			sizeof(mortal_buf));
	bzero(avat_buf, sizeof(avat_buf));
	strncat(avat_buf,
			"&YAvatars\r\n"
			"-------&n\r\n",
			sizeof(avat_buf));
	bzero(imm_buf, sizeof(imm_buf));
	strncat(imm_buf,
			"&CAdministrators\r\n"
			"--------------&n\r\n",
			sizeof(imm_buf));	
	bzero(final_buf, sizeof(final_buf));
	
	/* Loop time! */
	for (d = descriptor_list; d; d = d->next) {
		if (d->connected == CON_PLAYING || d->connected == CON_LINK_DEAD) {

			/* Get the character (keeping in mind switched
			 * characters) */
			c = (d->original) ? d->original : d->character;

			if (ch_level >= GET_VISIBLE(c) && ch_level >= GET_INCOGNITO(c)) {
				bzero(buf, sizeof(buf));
				visible_count++;

				switch (GET_LEVEL(c)) {
				case MAX_LEV:
					snprintf(buf, sizeof(buf), "%s", CLR_MAX "[------DM-----");
					break;
				case IMO_IMP:
					snprintf(buf, sizeof(buf), "%s", CLR_IMP "[-----God-----");
					break;
				case IMO_IMM:
					snprintf(buf, sizeof(buf), "%s", CLR_MEDIATOR "[--Immortal---");
					break;
				case IMO_SPIRIT:
					if (gv_port == ZONING_PORT) {
						snprintf(buf, sizeof(buf), "%s", "&C[&c*&C*&c*&CCreator&c*&C*&c*&C");
					}
					else {
						snprintf(buf, sizeof(buf), "%s", "&W[&w----&WS&wp&Wir&wi&Wt&w---&W");
					}
					break;
				case IMO_LEV:
					snprintf(buf, sizeof(buf), "%s", "&Y[&y----&YA&yv&Yat&ya&Yr&y---&Y");
					break;
				case PK_LEV:
					snprintf(buf, sizeof(buf), "%s", "&K[&wX&Kx&wX&RP&rkiller&wX&Kx&wX&K");
					break;
				default:
					snprintf(buf, sizeof(buf), "[%-2d %-5s %-4s", GET_LEVEL(c),
						 races[GET_RACE(c)].desc, classes[GET_CLASS(c)].desc);
				}

				/* Wiztitles? */
				char *wiztitle = NULL;
				wiztitle = getwiztitle(GET_NAME(c));
				if (GET_LEVEL(c) >= IMO_SPIRIT && wiztitle) {
					snprintf(buf, sizeof(buf), "%s", wiztitle);
				}

				strncat(buf, "]&n ", sizeof(buf));

				if (looking_for_name)
					strncat(buf, GET_NAME(c), sizeof(buf));
				else {
					char transform_buf[MAX_STRING_LENGTH];
					if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_DRAGON)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&R%s &rthe fearsome Dragon&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_WOLF)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&W%s &wthe lanky Wolf&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_BEAR)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&Y%s &ythe sturdy Bear&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_NINJA)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&w%s &Kthe sneaky Ninja&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_MONKEY)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&B%s &bthe cheeky Monkey&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_MANTICORE)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&P%s &pthe noble Manticore&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_CHIMERA)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&G%s &gthe tricky Chimera&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_WOMBLE)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&w%s &gthe curious Womble&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else if (ha1375_affected_by_spell(c, SPELL_TRANSFORM_COW)) {
						snprintf(transform_buf, MAX_STRING_LENGTH, "&W%s &Kth&We m&Koo &WC&Ko&Ww&n", GET_NAME(c));
						strncat(buf, transform_buf, sizeof(buf));
					}
					else {
						strncat(buf, GET_TITLE(c), sizeof(buf));
					}
				}

				/* Add some info */
				int count = 0;
				int len = strlen(buf);

				if (GET_VISIBLE(c) > 0 && ch_level >= GET_VISIBLE(c) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &R<&W%d&R>", GET_VISIBLE(c));
					if (count >= 0)
						len += count;
				}

				if (GET_INCOGNITO(c) > 0 && ch_level >= GET_INCOGNITO(c) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &w<INCOG:&r %d&w>", GET_INCOGNITO(c));
					if (count >= 0)
						len += count;
				}

				if (GET_GHOST(c) > 0 && ch_level >= GET_GHOST(c) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &w<GHOST:&r %d&w>", GET_GHOST(c));
					if (count >= 0)
						len += count;
				}
				
				if (IS_SET(GET_ACT2(c), PLR2_QUEST) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &g<&YQUEST&g>");
					if (count >=0)
						len += count;
				}

				if (IS_SET(GET_ACT2(c), PLR2_AFK) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &r<AFK>");
					if (count >= 0)
						len += count;
				}

				if (IS_SET(GET_ACT2(c), PLR2_AFW) && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &r<AFW>");
					if (count >= 0)
						len += count;
				}

				if (c->specials.timer > 4 && len < sizeof(buf)) {
					count = snprintf(buf + len, sizeof(buf) - len, " &W{IDLE: %d}", c->specials.timer);
					if (count >= 0)
						len += count;
				}

				/* Add the newline */
				strncat(buf, "&n\r\n", sizeof(buf));

				if (GET_LEVEL(c) >= IMO_IMM) {
					strncat(imm_buf, buf, sizeof(imm_buf));
					have_imms = 1;
				}
				else if (GET_LEVEL(c) == IMO_LEV) {
					strncat(avat_buf, buf, sizeof(avat_buf));
					have_avats = 1;
				}
				else {
					strncat(mortal_buf, buf, sizeof(mortal_buf));
					have_mortals = 1;
				}
			}
		}
	}/* End of huge for() loop. */
	
	/* After that loop, we three buffers to put together into one large
	 * one.  Joy :) */
	
	strncat(final_buf,
			"\r\n"
			"&WOnline Players\r\n"
			"--------------&n\r\n"
			"\r\n",
			 sizeof(final_buf));
	if (have_mortals == 1) {
		strncat(final_buf, mortal_buf, sizeof(final_buf));
		strncat(final_buf, "\r\n", sizeof(final_buf));
	}
	if (have_avats == 1) {
		strncat(final_buf, avat_buf, sizeof(final_buf));
		strncat(final_buf, "\r\n", sizeof(final_buf));
	}
	if (have_imms == 1) {
		strncat(final_buf, imm_buf, sizeof(final_buf));
		strncat(final_buf, "\r\n", sizeof(final_buf));
	}
	
	if (visible_count > gv_boottime_high) {
		gv_boottime_high = visible_count;
	}
	
	snprintf(info_buf, sizeof(info_buf),
		END "&WCurrent visible players: &R%d&W. Max so far: &R%d&W.&n\r\n",
		visible_count, gv_boottime_high);
	strncat(final_buf, info_buf, sizeof(final_buf));
	
	if (ch->desc) {
		page_string(ch->desc, final_buf, 1);
	}
}

void in1500_do_afk(struct char_data * ch, char *arg, int cmd)
{

	if (IS_SET(GET_ACT2(ch), PLR2_AFK)) {
		REMOVE_BIT(GET_ACT2(ch), PLR2_AFK);
		send_to_char("You're no longer AFK.\r\n", ch);
		act("$n is no longer AFK.", TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		SET_BIT(GET_ACT2(ch), PLR2_AFK);
		send_to_char("You're AFK.\r\n", ch);
		act("$n is AFK.", TRUE, ch, 0, 0, TO_ROOM);
	}
	if (IS_SET(GET_ACT2(ch), PLR2_AFW)) {
		REMOVE_BIT(GET_ACT2(ch), PLR2_AFW);
		send_to_char("You're no longer AFW.\r\n", ch);
		act("$n is no longer AFW.", TRUE, ch, 0, 0, TO_ROOM);
	}

	return;

}				/* END OF in1500_do_afk() */


void do_afw(struct char_data * ch, char *arg, int cmd)
{

	if (IS_SET(GET_ACT2(ch), PLR2_AFW)) {
		REMOVE_BIT(GET_ACT2(ch), PLR2_AFW);
		send_to_char("You're no longer AFW.\r\n", ch);
		act("$n is no longer AFW.", TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		SET_BIT(GET_ACT2(ch), PLR2_AFW);
		send_to_char("You're AFW.\r\n", ch);
		act("$n is AFW.", TRUE, ch, 0, 0, TO_ROOM);
	}
	if (IS_SET(GET_ACT2(ch), PLR2_AFK)) {
		REMOVE_BIT(GET_ACT2(ch), PLR2_AFK);
		send_to_char("You're no longer AFK.\r\n", ch);
		act("$n is no longer AFK.", TRUE, ch, 0, 0, TO_ROOM);
	}

	return;

}				/* END OF do_afw() */

void in1600_do_diag(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int percent;
	struct char_data *victim;

	if (!ch->desc)
		return;

	if ((IS_DARK(ch, ch->in_room)) &&
	    !(IS_AFFECTED(ch, AFF_DARKSIGHT)) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		send_to_char("It is pitch black...\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (*buf)
		victim = ha2125_get_char_in_room_vis(ch, buf);
	else
		victim = ch;

	if (!victim) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "You can't seem to locate %s.", buf);
		send_to_char(buf2, ch);
		return;
	}

	/* CAN WE SEE THE OTHER CHAR? */
	if (IS_AFFECTED(victim, AFF_HIDE) || !(CAN_SEE(ch, victim))) {
		if (IS_AFFECTED(ch, AFF_SENSE_LIFE) &&
		    GET_LEVEL(ch) >= GET_VISIBLE(victim) &&
		    GET_LEVEL(ch) >= GET_GHOST(victim)) {
			//Incognito & ghost Shalira 22.07 .01
				ansi_act("You sense a hidden life form in the room.",
					 FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);
			return;
		}
	}

	if (GET_HIT_LIMIT(victim) > 0)
		percent = (100 * GET_HIT(victim)) / GET_MAX_HIT(victim);
	else
		percent = -1;	/* How could MAX_HIT be < 1?? */

	if (IS_NPC(victim))
		strcpy(buf, victim->player.short_descr);
	else
		strcpy(buf, GET_NAME(victim));

	if (percent >= 100)
		strcat(buf, " is in an excellent condition.");
	else if (percent >= 90)
		strcat(buf, " has a few scratches.");
	else if (percent >= 75)
		strcat(buf, " has some small wounds and bruises.");
	else if (percent >= 50)
		strcat(buf, " has quite a few wounds.");
	else if (percent >= 30)
		strcat(buf, " has some big nasty wounds and scratches.");
	else if (percent >= 15)
		strcat(buf, " looks pretty hurt.");
	else if (percent >= 0)
		strcat(buf, " is in an awful condition.");
	else
		strcat(buf, " is bleeding awfully from big wounds.");

	ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);

	if (ch != victim) {
		act("$n checks your health.", TRUE, ch, 0, victim, TO_VICT);
		act("$n checks $N's health.", TRUE, ch, 0, victim, TO_NOTVICT);
	}

}				/* END OF in1600_do_diag() */


/* ROUTINE RETURNS gv_str WITH A VALUE OF YOUR OPONENT'S HEALTH */
char *in1650_do_short_diag(struct char_data * ch, char *arg, int cmd)
{

	int percent;
	struct char_data *victim;

	if ((IS_DARK(ch, ch->in_room)) &&
	    !(IS_AFFECTED(ch, AFF_DARKSIGHT)) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		return ("TO_DARK");
	}

	victim = GET_FIGHTING(ch);

	if (!victim) {
		return ("NOT_FIGHTING");
	}

	/* CAN WE SEE THE OTHER CHAR? */
	if (IS_AFFECTED(victim, AFF_HIDE) || !(CAN_SEE(ch, victim))) {
		if (!IS_AFFECTED(ch, AFF_SENSE_LIFE) ||
		    GET_LEVEL(ch) < GET_VISIBLE(victim) ||
		    GET_LEVEL(ch) < GET_GHOST(victim)) {
			//Incognito & ghost shalira 22.07 .01
				return ("HIDDEN");
		}
	}

	if (GET_HIT_LIMIT(victim) > 0)
		percent = (100 * GET_HIT(victim)) / GET_HIT_LIMIT(victim);
	else
		percent = -1;	/* How could MAX_HIT be < 1?? */

	if (percent >= 100)
		return ("EXCELLENT");
	else if (percent >= 90)
		return ("SCRATCHES");
	else if (percent >= 75)
		return ("BRUISES");
	else if (percent >= 50)
		return ("WOUNDS");
	else if (percent >= 30)
		return ("NASTY");
	else if (percent >= 15)
		return ("HURT");
	else if (percent >= 0)
		return ("AWFUL");
	else
		return ("BLEEDING");

}				/* END OF in1650_do_short_diag() */


void in1700_do_attribute(struct char_data * ch, char *arg, int cmd)
{

	int lv_ac;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	struct char_data *k;
	struct time_info_data real_time_passed(time_t t2, time_t t1);



	k = 0;
	bzero(buf3, sizeof(buf3));
	arg = one_argument(arg, buf3);
	/* no arg so default to self */
	if (!*buf3) {
		k = ch;
	}
	else {
		/* MORTALS AREN'T ALLOWED TO STAT ANYONE ELSE */
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are only allowed to get attributes on yourself.\r\n", ch);
			send_to_char("Enter ATTRIBUTE without arguments.\r\n", ch);
			return;
		}
	}

	if (cmd == CMD_ADMIN)
		k = ch->desc->admin;

	/* LOOK FOR VICTIM */
	if (!k) {
		k = ha3100_get_char_vis(ch, buf3);
		if (!k) {
			send_to_char("Unable to find that person.\r\n", ch);
			return;
		}
	}

	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));

	switch (k->player.sex) {
	case SEX_NEUTRAL:
		strcpy(buf, "NEUTER");
		break;
	case SEX_MALE:
		strcpy(buf, "MALE");
		break;
	case SEX_FEMALE:
		strcpy(buf, "FEMALE");
		break;
	default:
		strcpy(buf, "ILLEGAL-SEX!!");
		break;
	}

	sprintf(buf2, " %s - Name : &W%s&n  Level: &W%d&n\r\n",
		(!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
		GET_NAME(k),
		GET_LEVEL(k));
	strcat(buf, buf2);
	send_to_char(buf, ch);

	sprintf(buf, "&wRace: &W%s&w  Class: &W%s&n\r\n",
		races[GET_RACE(k)].name, classes[GET_CLASS(k)].name);
	send_to_char(buf, ch);

	if (IS_MOB(k)) {
		sprintf(buf, "V-Number &W[&w%d&W]&n\r\n", mob_index[k->nr].virtual);
		send_to_char(buf, ch);
	}

	strcpy(buf, "&wTitle:&W ");
	strcat(buf, (k->player.title ? k->player.title : "None"));
	strcat(buf, "&n\r\n");
	send_to_char(buf, ch);

	if (CLAN_NUMBER(ch) != NO_CLAN) {
		if (CLAN_RANK(ch) == CLAN_APPLY_RANK)
			sprintf(buf, "&wYou have applied for clan &W%s&w.&n\r\n", GET_CLAN_NAME(CLAN_NUMBER(ch)));
		else
			sprintf(buf, "&wYou are &W%s&w in clan &W%s&w.&n\r\n",
			     GET_CLAN_RANK_NAME(CLAN_NUMBER(k), CLAN_RANK(k)),
				GET_CLAN_NAME(CLAN_NUMBER(k)));
		send_to_char(buf, ch);
	}


	bzero(buf, sizeof(buf));
	sprintf(buf, "&wYou have &W%d&w rotting corpses on your pyre.&n\r\n", GET_DEATHS(k));
	send_to_char(buf, ch);
	bzero(buf, sizeof(buf));
	sprintf(buf, "&wYou have slaughtered &W%d&w victims.&n\r\n", GET_KILLS(k));
	send_to_char(buf, ch);
	/* STATS */
	sprintf(buf, "&wBase/adj STR:[&W%d&w/&W%d&w] INT:[&W%d&w/&W%d&w] WIS:[&W%d&w/&W%d&w] DEX:[&W%d&w/&W%d&w] CON:[&W%d&w/&W%d&w] CHA:[&W%d&w/&W%d&w]&n\r\n",
		GET_REAL_STR(k) + GET_BONUS_STR(k),
		GET_STR(k) + GET_BONUS_STR(k),
		GET_REAL_INT(k),
		GET_INT(k),
		GET_REAL_WIS(k),
		GET_WIS(k),
		GET_REAL_DEX(k) + GET_BONUS_DEX(k),
		GET_DEX(k) + GET_BONUS_DEX(k),
		GET_REAL_CON(k),
		GET_CON(k),
		GET_REAL_CHA(k) + GET_BONUS_CHA(k),
		GET_CHA(k) + GET_BONUS_CHA(k));
	send_to_char(buf, ch);

	sprintf(buf, "&wSaving throws PARA:[&W%d&w] ROD:[&W%d&w] PETRI:[&W%d&w] BREATH:[&W%d&w] SPELL:[&W%d&w]&n\r\n",
		k->specials.apply_saving_throw[0],
		k->specials.apply_saving_throw[1],
		k->specials.apply_saving_throw[2],
		k->specials.apply_saving_throw[3],
		k->specials.apply_saving_throw[4]);
	send_to_char(buf, ch);

	if (GET_LEVEL(ch) > 10) {
		sprintf(buf, "&wBonus: Damage stat+eqp[&W%d%+d&w=&W%d&w],  Hit stat+eqp[&W%d%+d&w=&W%d&w]  AC dex+race[&W%d%+d&w=&W%d&w]&n\r\n",
		    li9700_adjust_damage_bonus(GET_STR(k) + GET_BONUS_STR(k)),
			GET_DAMROLL(k),
			(GET_DAMROLL(k) + li9700_adjust_damage_bonus(GET_STR(k) + GET_BONUS_STR(k))),
		     li9725_adjust_tohit_bonus(GET_STR(k) + GET_BONUS_STR(k)),
			GET_HITROLL(k),
			(GET_HITROLL(k) + li9725_adjust_tohit_bonus(GET_STR(k) + GET_BONUS_STR(k))),
			li9750_ac_bonus(GET_DEX(k) + GET_BONUS_DEX(k)),
			races[GET_RACE(k)].adj_ac,
			(li9750_ac_bonus(GET_DEX(k) + GET_BONUS_DEX(k)) + races[GET_RACE(k)].adj_ac));
		send_to_char(buf, ch);
	}

	/* THACO / AC / CARRY */
	bzero(buf, sizeof(buf));
	lv_ac = GET_AC(k) + li9750_ac_bonus(GET_DEX(k) + GET_BONUS_DEX(k)) +
		races[GET_RACE(k)].adj_ac;
	/* if (lv_ac > 400) lv_ac = 400; if (lv_ac < -400) lv_ac = -400; */
	sprintf(buf, "&wTHAC0 &W%d&w, and AC &W%d&w, You're carrying &W%d&w/&W%d&w pounds.&n\r\n",
		li9675_thaco(k),
		lv_ac,
		IS_CARRYING_W(k),
		CAN_CARRY_W(k));
	send_to_char(buf, ch);

	/* FOOD/THIRST */
	sprintf(buf, "&wHunger: &W%d&w/&W%d&w  Thirst: &W%d&w/&W%d&w  Drunk: &W%d&w/&W%d&n\r\n",
		GET_COND(k, FOOD),
		races[GET_RACE(k)].max_food,
		GET_COND(k, THIRST),
		races[GET_RACE(k)].max_thirst,
		GET_COND(k, DRUNK),
		races[GET_RACE(k)].max_drunk);
	send_to_char(buf, ch);

	return;

}				/* END OF in1700_do_attribute() */


void in1800_do_score(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	int lv_elapsed, lv_hours, lv_minutes;

	struct char_data *k;
	struct time_info_data playing_time;
	struct time_info_data real_time_passed(time_t t2, time_t t1);

	k = 0;
	bzero(buf3, sizeof(buf3));
	arg = one_argument(arg, buf3);
	/* no arg so default to self */
	if (!(*buf3)) {
		k = ch;
	}
	else {
		/* MORTALS AREN'T ALLOWED TO STAT ANYONE ELSE */
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are only allowed to get attributes on yourself.\r\n", ch);
			send_to_char("Enter SCORE without arguments.\r\n", ch);
			return;
		}
	}
	if (cmd == CMD_ADMIN)
		k = ch->desc->admin;

	/* LOOK FOR VICTIM */
	if (!k) {
		k = ha3100_get_char_vis(ch, buf3);
		if (!k) {
			send_to_char("Unable to find that person.\r\n", ch);
			return;
		}
	}



	/* LOG ARCH and METH when they score others */
	if (GET_LEVEL(ch) < IMO_IMP) {
		if (ch != k) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s level %d entered SCORE %s",
			  GET_REAL_NAME(ch), GET_LEVEL(ch), GET_REAL_NAME(k));
			do_wizinfo(buf, IMO_IMP, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}
	}

	bzero(buf, sizeof(buf));
	if (IS_NPC(k)) {
		sprintf(buf, "You are: %s&n (level %d).\r\n",
			k->player.short_descr, GET_LEVEL(k));
	}
	else {
		sprintf(buf, "\r\nYou are: %s&n (level %d) %s %s.\r\n",
			GET_TITLE(k),
			GET_LEVEL(k),
			races[GET_RACE(k)].name,
			classes[GET_CLASS(k)].name);
		if (GET_LEVEL(k) == IMO_LEV || GET_LEVEL(k) == PK_LEV) {
			sprintf(buf2, "You are level %d in your current class.\r\n",
				wi3150_return_avatar_level(k));
			strcat(buf, buf2);
		}
	}
	ansi(CLR_TITLE, ch);
	send_to_char(buf, ch);
	ansi(END, ch);

	sprintf(buf, "You are &W%d&n years old.", GET_AGE(k));
	playing_time = real_time_passed((time(0) - k->player.time.logon) +
					k->player.time.played, 0);
	sprintf(buf, "You have been playing for &W%d&n days and &W%d&n hours. You are &W%d&n years old\r\n",
		playing_time.day,
		playing_time.hours,
		GET_AGE(k));
	send_to_char(buf, ch);

	if (IS_PC(k) &&
	    (z_age(k).month == 0) && (z_age(k).day == 0)) {
		send_to_char("&YIt's your birthday today!&n\r\n", ch);
	}

	sprintf(buf,
		"You have &g%d&n(&G%d&n) hit, &c%d&n(&C%d&n) mana, &y%d&n(&Y%d&n) movement points.\r\n",
		GET_HIT(k), GET_HIT_LIMIT(k),
		GET_MANA(k), GET_SPECIAL_MANA(k),
		GET_MOVE(k), GET_MOVE_LIMIT(k));
	send_to_char(buf, ch);
	sprintf(buf, "Your kills/death score is : &W%d&n.\r\n", GET_SCORE(k));
	send_to_char(buf, ch);
	sprintf(buf, "You have &W%d&n quest points.\r\n", k->questpoints);
	send_to_char(buf, ch);


	if (GET_LEVEL(k) < IMO_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You have scored &g%d&n/&G%d&n (&W%d&n to go) exp,\r\n",
			GET_EXP(k),
			LEVEL_EXP(GET_LEVEL(k)),
			LEVEL_EXP(GET_LEVEL(k)) - GET_EXP(k));
		send_to_char(buf, ch);
	}
	sprintf(buf, "You have &W%d&n exp left to train.\r\n", GET_AVAILABLE_EXP(k));
	send_to_char(buf, ch);
	if (GET_LEVEL(k) == IMO_LEV || GET_LEVEL(k) == PK_LEV) {
		sprintf(buf, "You have practiced &g%d&n/&G55&n foreign spells/skills.\r\n", k->points.fspells);
		send_to_char(buf, ch);
	}
	bzero(buf, sizeof(buf));
	sprintf(buf, "You have &W%d&n coins. (Balance in Bank: %d  Inn: %d)\r\n",
		GET_GOLD(k),
		GET_BANK_GOLD(k),
		GET_INN_GOLD(k));
	send_to_char(buf, ch);

	if (GET_ALIGNMENT(k) > 800)
		send_to_char("&GYou're so good you've developed a halo.&n\r\n", ch);
	else if (GET_ALIGNMENT(k) > 600)
		send_to_char("&gYou're a virtual saint.&n\r\n", ch);
	else if (GET_ALIGNMENT(k) > 300)
		send_to_char("You're a fairly good person.\r\n", ch);
	else if (GET_ALIGNMENT(k) > -300)
		send_to_char("You're as neutral as they come.\r\n", ch);
	else if (GET_ALIGNMENT(k) > -600)
		send_to_char("You're mean and spiteful.\r\n", ch);
	else if (GET_ALIGNMENT(k) > -800)
		send_to_char("&rYou're a malevolent fiend.&n\r\n", ch);
	else
		send_to_char("&RYou're so evil you make demons look like Paladins.&n\r\n", ch);

	if (ch->nextcast > time(0)) {
		sprintf(buf, "&CYou may transform again in %d hours, %d minutes and %d seconds.&n\r\n",
			get_jail_time(ch->nextcast, 2),
			get_jail_time(ch->nextcast, 3),
			get_jail_time(ch->nextcast, 4));
		send_to_char(buf, ch);
	}
	if ((ch->nextcast == 0) && (ch->skills[SPELL_BEAST_TRANSFORM].learned)) {
		send_to_char("&CYour body can safely handle a bestial transformation.&n\r\n", ch);
	}
	switch (GET_POS(k)) {
	case POSITION_DEAD:
		send_to_char("You are DEAD!\r\n", ch);
		break;
	case POSITION_MORTALLYW:
		send_to_char("You are mortally wounded!, you should seek help!\r\n", ch);
		break;
	case POSITION_INCAP:
		send_to_char("You are incapacitated, slowly fading away.\r\n", ch);
		break;
	case POSITION_STUNNED:
		send_to_char("You are stunned! You can't move.\r\n", ch);
		break;
	case POSITION_SLEEPING:
		send_to_char("You are sleeping.\r\n", ch);
		break;
	case POSITION_RESTING:
		send_to_char("You are resting.\r\n", ch);
		break;
	case POSITION_SITTING:
		send_to_char("You are sitting.\r\n", ch);
		break;
	case POSITION_FIGHTING:
		if (k->specials.fighting)
			act("You are fighting $N.\r\n", FALSE, ch, 0,
			    k->specials.fighting, TO_CHAR);
		else
			send_to_char("You are fighting thin air.\r\n", ch);
		break;
	case POSITION_STANDING:
		send_to_char("You are standing.\r\n", ch);
		break;
	default:
		send_to_char("You are floating.\r\n", ch);
		break;
	}

	/* MUZZLE STUFF? */
	if (IS_SET(GET_ACT2(k), PLR2_MUZZLE_SHOUT)) {
		bzero(buf, sizeof(buf));
		lv_elapsed = (time(0) - k->specials.time_of_muzzle);
		lv_hours = lv_elapsed / 3600;
		lv_minutes = lv_elapsed % 3600 / 60;

		if (lv_hours > 23 ||
		    k->specials.time_of_muzzle < 1) {
			REMOVE_BIT(GET_ACT2(k), PLR2_MUZZLE_SHOUT);
			k->specials.time_of_muzzle = 0;
		}
		else {
			sprintf(buf, "You have been muzzled from using channels. Expires in &W%d&n hours &W%d&n minutes.\r\n",
				23 - lv_hours,
				59 - lv_minutes);
			send_to_char(buf, ch);
		}
	}			/* END OF is muzzle_shout set */
	if (IS_SET(GET_ACT2(k), PLR2_JAILED)) {
		send_to_char("You are jailed for another ", ch);
		sprintf(buf, "&W%d&n months &W%d&n days &W%d&n hours &W%d&n minutes &W%d&n seconds.\r\n",
			get_jail_time(GET_JAILTIME(k), 0),
			get_jail_time(GET_JAILTIME(k), 1),
			get_jail_time(GET_JAILTIME(k), 2),
			get_jail_time(GET_JAILTIME(k), 3),
			get_jail_time(GET_JAILTIME(k), 4));
		send_to_char(buf, ch);
	}
	if (GET_LEVEL(ch) >= IMO_SPIRIT) {
		send_to_char("You have the following bits set:\r\n", ch);
		sprintbit(GET_ACT1(k), player_bits1, buf);
		send_to_char(buf, ch);
		sprintbit(GET_ACT2(k), player_bits2, buf);
		send_to_char(buf, ch);
		sprintbit(GET_ACT3(k), player_bits3, buf);
		send_to_char(buf, ch);
		sprintbit(GET_WIZ_PERM(k), wizperm_bits, buf);
		co2900_send_to_char(ch, buf);
		if (IS_PC(k)) {
			sprintbit(GET_DSC1(k), descriptor_bits1, buf);
			send_to_char(buf, ch);
		}
		send_to_char("\r\n", ch);
	}
	bzero(buf, sizeof(buf));
	strcpy(buf, do_how_left_text(k, GET_HOW_SAVE(k)));
	if (*buf) {
		send_to_char(buf, ch);
	}

}				/* END OF in1800_do_score() */


void in1900_do_system(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], *suf;
	int weekday, weeks, day, hours, minutes;
	long elapsed;
	extern long up_time;
	extern struct time_info_data time_info;

	time_t t;
	t = time(NULL);



	bzero(buf, sizeof(buf));
	sprintf(buf, "It is %d o'clock %s, on ",
		((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
		((time_info.hours >= 12) ? "pm" : "am"));

	weekday = ((35 * time_info.month) + time_info.day + 1) % 7;	/* 35 days in a month */

	strcat(buf, weekdays[weekday]);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	day = time_info.day + 1;/* day in [1..35] */

	if (day == 1)
		suf = "st";
	else if (day == 2)
		suf = "nd";
	else if (day == 3)
		suf = "rd";
	else if (day < 20)
		suf = "th";
	else if ((day % 10) == 1)
		suf = "st";
	else if ((day % 10) == 2)
		suf = "nd";
	else if ((day % 10) == 3)
		suf = "rd";
	else
		suf = "th";

	sprintf(buf, "The %d%s Day of the %s, Year %d.\r\n",
		day,
		suf,
		month_name[time_info.month],
		time_info.year);
	send_to_char(buf, ch);

	switch (weather_info.sky) {
	case SKY_CLOUDLESS:{
			send_to_char("Skies are clear.\r\n", ch);
			break;
		}
	case SKY_CLOUDY:{
			send_to_char("Skies are cloudy.\r\n", ch);
			break;
		}
	case SKY_RAINING:{
			send_to_char("Its raining.\r\n", ch);
			break;
		}
	case SKY_LIGHTNING:{
			send_to_char("Its lightning and thundering.\r\n", ch);
			break;
		}
	}			/* END OF switch() */

	sprintf(buf, "\r\nZones=%d      Rooms=%d\r\nMob's=%d   Current=%ld\r\nObj's=%d   Current=%ld\r\n\r\n",
		top_of_zone_table + 1,
		top_of_world + 1,
		top_of_mobt + 1,
		gv_total_mobiles,
		top_of_objt + 1,
		gv_total_objects);
	send_to_char(buf, ch);

	elapsed = (time(0) - up_time);
	weeks = 0;
	if (elapsed > 604800) {
		weeks = elapsed / 604800;
		elapsed = elapsed - (weeks * 604800);
	}
	day = 0;
	if (elapsed > 86400) {
		day = elapsed / 86400;
		elapsed = elapsed - (day * 86400);
	}
	hours = elapsed / 3600;
	minutes = elapsed % 3600 / 60;

	if (GET_LEVEL(ch) > IMO_SPIRIT) {
		sprintf(buf, "Up-time is: %d weeks %d days %d hours and %d minutes.\r\n",
			weeks, day, hours, minutes);
		send_to_char(buf, ch);
	}

	/* CURRENT TIME */
	sprintf(buf, "EST date/time: %s\r\n", ctime(&t));
	send_to_char(buf, ch);
	return;

}				/* END OF in1900_do_system */


void in2100_ot2900_do_auto_level(struct char_data * ch, char *arg, int cmd)
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
		send_to_char("Only numeric values are allowed.\r\n", ch);
		return;
	}

	lv_value = atoi(arg);

	/* IS VALUE HIGHER THAN YOUR LEVEL? */
	if (lv_value > GET_LEVEL(ch)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Value must be between 0 and %d.\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_value < 0 || lv_value > 45) {
		send_to_char("Value must be between 0 and 45.\r\n", ch);
		return;
	}

	gv_auto_level = lv_value;
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s has set AUTOLEVEL to %d.\r\n",
		GET_NAME(ch), gv_auto_level);
	do_sys(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);
	bzero(buf, sizeof(buf));
	sprintf(buf, "You have set AUTOLEVEL to %d.\r\n", gv_auto_level);
	send_to_char(buf, ch);
	send_to_char("New players logging onto system will be advanced to that level.\r\n", ch);

	return;

}				/* END OF in2100_ot2900_do_auto_level */


/* Procedures related to 'look' */
void in2200_argument_split_2(char *arg, char *first_arg, char *second_arg)
{
	int look_at, found, begin;



	found = begin = 0;

	/* Find first non blank */
	for (; *(arg + begin) == ' '; begin++);

	/* Find length of first word */
	for (look_at = 0; *(arg + begin + look_at) > ' '; look_at++)
		/* Make all letters lower case, AND copy them to first_arg */
		*(first_arg + look_at) = LOWER(*(arg + begin + look_at));
	*(first_arg + look_at) = '\0';
	begin += look_at;

	/* Find first non blank */
	for (; *(arg + begin) == ' '; begin++);

	/* Find length of second word */
	for (look_at = 0; *(arg + begin + look_at) > ' '; look_at++)
		/* Make all letters lower case, AND copy them to second_arg */
		*(second_arg + look_at) = LOWER(*(arg + begin + look_at));
	*(second_arg + look_at) = '\0';
	begin += look_at;
}

struct obj_data *in2300_get_object_in_equip_vis(struct char_data * ch,
			     char *arg, struct obj_data * equipment[], int *j)
{



	for ((*j) = 0; (*j) < MAX_WEAR; (*j)++)
		if (equipment[(*j)])
			if (CAN_SEE_OBJ(ch, equipment[(*j)]))
				if (ha1150_isname(arg, equipment[(*j)]->name))
					return (equipment[(*j)]);

	return (0);
}

char *in2400_find_ex_description(char *word, struct extra_descr_data * list)
{
	struct extra_descr_data *i;



	for (i = list; i; i = i->next)
		if (ha1150_isname(word, i->keyword))
			return (i->description);

	return (0);
}

void in2500_show_obj_to_char(struct obj_data * object, struct char_data * ch, int mode, int num)
{
	char buffer[MAX_STRING_LENGTH];
	bool found;



	bzero(buffer, sizeof(buffer));
	if (IS_OBJ_STAT(object, OBJ1_NO_SEE))
		if ((GET_LEVEL(ch) < IMO_IMP) &&
		    (!ha1175_isexactname(GET_NAME(ch),
				   zone_table[world[ch->in_room].zone].lord)))
			return;	/* hide no_see items from lesser mortals */

	/* ZERO */
	if ((mode == 0) && object->description) {
		if (GET_LEVEL(ch) > IMO_IMM)
			sprintf(buffer, "%s &R(%d)&E", object->description,
				(object->item_number >= 0) ? obj_index[object->item_number].virtual : 0);
		else
			strcat(buffer, object->description);
	}

	/* ONE, TWO, THREE, FOUR */
	if ((mode == 1) || (mode == 2) || (mode == 3) || (mode == 4)) {
		if (object->short_description) {
			if (GET_LEVEL(ch) > IMO_IMM)
				sprintf(buffer, "%s &R(%d)&E", object->short_description,
					(object->item_number >= 0) ? obj_index[object->item_number].virtual : 0);
			else
				strcat(buffer, object->short_description);
		}
	}

	/* FIVE */
	if (mode == 5) {
		if (object->obj_flags.type_flag == ITEM_NOTE) {
			if (object->action_description) {
				strcat(buffer,
				"There is something written upon it:\r\n\r\n");
				strcat(buffer, object->action_description);
				if (ch->desc) {
					page_string(ch->desc, buffer, 1);
				}
			}
			else {
				strcat(buffer, "It's blank.");
				co2900_send_to_char(ch, "%s", buffer);
			}
			return;
		}
		else {
			if ((object->obj_flags.type_flag != ITEM_DRINKCON)) {
				/* strcat(buffer,"        nothing special.."); */
				strcat(buffer, "        ");
				strcat(buffer, object->short_description);
			}
			else {	/* ITEM_TYPE == ITEM_DRINKCON */
				strcat(buffer, "It looks like a drink container.");
			}
			co2900_send_to_char(ch, "%s", buffer);
			return;
		}
	}

	if (mode != 3) {
		found = FALSE;
		if (IS_OBJ_STAT(object, OBJ1_INVISIBLE)) {
			strcat(buffer, "(invisible)");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, OBJ1_MAGIC) &&
		    IS_AFFECTED(ch, AFF_DETECT_MAGIC)) {
			strcat(buffer, "..It glows blue!");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, OBJ1_GLOW)) {
			strcat(buffer, "..it glows brightly!");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, OBJ1_HUM)) {
			strcat(buffer, "..it emits a faint hum!");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, OBJ1_DARK)) {
			strcat(buffer, "..clouded in darkness!");
			found = TRUE;
		}
	}

	if (num > 1) {
		/* if list mode collect multiples */
		sprintf(buffer + strlen(buffer), " (%-2d)", num);
	}

	if (ch->desc) {
		strcat(buffer, "\r\n");
		ansi(CLR_OBJ, ch);
		page_string(ch->desc, buffer, 1);
		ansi(END, ch);
	}

}				/* END OF in2500_show_obj_to_char() */


/* PRINTS ITEM (n) like when you look at a room */
void in2600_list_obj_to_char(struct obj_data * list, struct char_data * ch, int mode,
			       bool show)
{
	struct obj_data *list_obj;
	bool found;
	int num;



	found = FALSE;
	for (list_obj = list; list_obj; list_obj = list_obj->next_content) {
		if (CAN_SEE_OBJ(ch, list_obj)) {
			num = 1;/* one found so far, look for more of the same */
			if (GET_ITEM_TYPE(list_obj) != ITEM_ZCMD) {
				while ((list_obj->next_content)
				&& !strcmp(list_obj->next_content->description,
					   list_obj->description)) {
					num++;
					list_obj = list_obj->next_content;
				}
			}
			in2500_show_obj_to_char(list_obj, ch, mode, num);
			found = TRUE;
		}
	}
	if ((!found) && (show))
		send_to_char("Nothing.\r\n", ch);
}

void in2650_list_all_obj_to_char(struct obj_data * list, struct char_data * ch, int mode,
				   bool show)
{
	struct obj_data *list_obj;
	bool found;
	int num;



	found = FALSE;
	for (list_obj = list; list_obj; list_obj = list_obj->next_content) {
		if (CAN_SEE_OBJ(ch, list_obj)) {
			num = 1;/* one found so far, look for more of the same */
			if (GET_ITEM_TYPE(list_obj) != ITEM_ZCMD) {
				while ((list_obj->next_content)
				&& !strcmp(list_obj->next_content->description,
					   list_obj->description)) {
					num++;
					list_obj = list_obj->next_content;
				}
			}
			if (GET_ITEM_TYPE(list_obj) == ITEM_CONTAINER || GET_ITEM_TYPE(list_obj) == ITEM_QSTCONT) {
				in2600_list_obj_to_char(list_obj->contains, ch, 2, TRUE);
			}
			in2500_show_obj_to_char(list_obj, ch, mode, num);
			found = TRUE;
		}
	}
	if ((!found) && (show))
		send_to_char("Nothing.\r\n", ch);
}

void in2700_show_char_to_char(struct char_data * i, struct char_data * ch, int mode)
{

	char buffer[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int j, lv_show, found, percent;



	if (FAILS_SANITY(ch)) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "ERROR: *ch user fails sanity check!");
		do_sys(buf2, IMO_IMM, gv_ch_ptr);
		spec_log(buf, ERROR_LOG);
	}

	if (FAILS_SANITY(i)) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "ERROR: *i user fails sanity check in r%d room %d!",
			ch->in_room, world[ch->in_room].number);
		do_sys(buf2, IMO_IMM, ch);
		spec_log(buf, ERROR_LOG);
	}

	if (mode == 0) {
		lv_show = TRUE;
		if (ch == i)
			lv_show = FALSE;

		if (!(CAN_SEE(ch, i)))
			lv_show = FALSE;

		if ((IS_AFFECTED(i, AFF_HIDE))) {
			if (!(IS_AFFECTED(ch, AFF_SENSE_LIFE)))
				if (GET_LEVEL(ch) < IMO_IMM)
					lv_show = FALSE;
		}

		if (GET_LEVEL(ch) < IMO_IMM) {
			if (IS_AFFECTED(i, AFF_HIDE) || !CAN_SEE(ch, i)) {
				if (IS_AFFECTED(ch, AFF_SENSE_LIFE) &&
				    GET_LEVEL(ch) >= GET_VISIBLE(i) &&
				    GET_LEVEL(ch) >= GET_GHOST(i)) {
					//Incognito & ghost Shalira 22.07 .01
						ansi_act("You sense a hidden life form in the room.", FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);
				}
				return;
			}
		}

		/* check if mob is a quest target */
		if (IS_NPC(i) && ((ch->questmob && ch->questmob == i) ||
				  (i->questmob && i->questmob == ch)))
			if (IS_PC(ch))
				send_to_char("&W<&RQuest&W>&n ", ch);


		/* if the person in room is being ridden or being ridden by the
		 * character Odin ch = looker i = who are we looking at */

		if (RIDDEN_BY(i) && ch != RIDDEN_BY(i) &&
		    i->in_room == RIDDEN_BY(i)->in_room) {
			sprintf(buf, "&g%s is here mounted upon %s.\r\n",
				GET_REAL_NAME(RIDDEN_BY(i)), GET_REAL_NAME(i));
			send_to_char(buf, ch);

			return;
		}

		if (RIDING(i) == ch && ch->in_room == i->in_room) {
			sprintf(buf, "&gYou are being ridden by %s.\r\n", GET_REAL_NAME(i));
			send_to_char(buf, ch);
			return;
		}

		if (RIDDEN_BY(i) == ch && i->in_room == ch->in_room) {
			sprintf(buf, "&gYou are mounted on %s.\r\n", GET_REAL_NAME(i));
			send_to_char(buf, ch);
			return;
		}

		if (RIDING(i) || RIDDEN_BY(i))
			return;



		if (!(i->player.long_descr) || (GET_POS(i) != i->specials.default_pos)) {
			/* A player char or a mobile without long descr */
			if (IS_PC(i)) {
				if (!(i->player.title)) {
					bzero(buf2, sizeof(buf2));
					sprintf(buf2, "ERROR: *i user doesn't have a title in r%d room %d!",
						ch->in_room, world[ch->in_room].number);
					do_sys(buf2, IMO_IMM, ch);
					spec_log(buf, ERROR_LOG);
					return;
				}
				strcpy(buffer, GET_TITLE(i));
			}
			else {
				strcpy(buffer, i->player.short_descr);
				CAP(buffer);
			}



			if (IS_PC(i)) {
				if (i->desc) {
					if (i->desc->connected == CON_LINK_DEAD) {
						strcat(buffer, " (Linkdead)");
					}
				}
				else {
					/* IF THERE IS NO DESC, WE ARE LINKDEAD */
					strcat(buffer, " (LinkDead)");
				}
			}

			if (IS_AFFECTED(i, AFF_HOLD_PERSON))
				strcat(buffer, " &p(Held)&n");

			if (IS_AFFECTED(i, AFF_INVISIBLE))
				strcat(buffer, " &K(Invis)&n");

			if (ha1375_affected_by_spell(ch, SPELL_DETECT_UNDEAD)) {
				if (IS_UNDEAD(i)) {
					strcat(buffer, " &Y(&wUndead&Y)&n");
				}
			}

			if (ha1375_affected_by_spell(ch, SPELL_DETECT_ANIMALS)) {
				if (IS_ANIMAL(i)) {
					strcat(buffer, " &W(&GAnimal&W)&n");
				}
			}
			
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_DRAGONS)) {
				if (IS_DRAGON(i)) {
					strcat(buffer, " &W(&RD&Br&Ga&Cg&Yo&Pn&W)&n");
				}
			}
			
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_DEMONS)) {
				if (IS_DEMON(i)) {
					strcat(buffer, " &W(&RD&Kemo&Rn&W)&n");
				}
			}
			
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_GIANTS)) {
				if (IS_GIANT(i)) {
					strcat(buffer, " &W(&BG&Ci&Wa&Cn&Bt&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_SHADOWS)) {
				if (IS_SHADOW(i)) {
					strcat(buffer, " &W(&KSh&wad&Kow&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_VAMPIRES)) {
				if (IS_VAMPIRE(i)) {
					strcat(buffer, " &W(&KV&Ra&wm&Kp&Ri&wr&Ke&W)&n");
				}
			}
			if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
				if (IS_EVIL(i)) {
					strcat(buffer, " &r(Evil)&n");
				}
			}

			if (ha1375_affected_by_spell(ch, SPELL_DETECT_GOOD)) {
				if (IS_GOOD(i))
					strcat(buffer, " &W(Good)&n");
			}

			if (IS_AFFECTED(i, AFF_CHARM)) {
				strcat(buffer, " &P(Charmed)&n");
			}

			if (IS_SET(GET_ACT2(i), PLR2_PKILLABLE)) {
				strcat(buffer, " <PK>");
			}

			if (IS_NPC(i) && GET_LEVEL(ch) > IMO_IMM)
				sprintf(buffer + strlen(buffer), " &R(%d)&E", (i->nr >= 0) ? mob_index[i->nr].virtual : 0);

			switch (GET_POS(i)) {
			case POSITION_STUNNED:
				strcat(buffer, " is lying here, stunned.");
				break;
			case POSITION_INCAP:
				strcat(buffer, " is lying here, incapacitated.");
				break;
			case POSITION_MORTALLYW:
				strcat(buffer, " is lying here, mortally wounded.");
				break;
			case POSITION_DEAD:
				strcat(buffer, " is lying here, dead.");
				break;
			case POSITION_STANDING:
				strcat(buffer, " is standing here.");
				break;
			case POSITION_SITTING:
				strcat(buffer, " is sitting here.");
				break;
			case POSITION_RESTING:
				strcat(buffer, " is resting here.");
				break;
			case POSITION_SLEEPING:
				strcat(buffer, " is sleeping here.");
				break;
			case POSITION_FIGHTING:
				if (i->specials.fighting) {
					strcat(buffer, " is here, fighting ");
					if (i->specials.fighting == ch)
						strcat(buffer, " YOU!");
					else {
						if (i->in_room == i->specials.fighting->in_room)
							if (IS_NPC(i->specials.fighting))
								strcat(buffer, i->specials.fighting->player.short_descr);
							else
								strcat(buffer, GET_NAME(i->specials.fighting));
						else
							strcat(buffer, "someone who has already left.");
					}
				}
				else	/* NIL fighting pointer */
					strcat(buffer, " is here struggling with thin air.");
				break;
			default:
				strcat(buffer, " is floating here.");
				break;
			}

			ansi_act(buffer, FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);
		}
		else {		/* npc with long */
			strcpy(buffer, i->player.long_descr);
			buffer[strlen(buffer) - 2] = '\0';

			if (IS_AFFECTED(i, AFF_HOLD_PERSON))
				strcat(buffer, " &p(Held)&n");

			if (IS_AFFECTED(i, AFF_INVISIBLE))
				strcat(buffer, " &K(Invis)&n");

			if (ha1375_affected_by_spell(ch, SPELL_DETECT_UNDEAD)) {
				if (IS_UNDEAD(i)) {
					strcat(buffer, " &Y(&wUndead&Y)&n");
				}
			}
			
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_ANIMALS)) {
				if (IS_ANIMAL(i)) {
					strcat(buffer, " &W(&GAnimal&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_DRAGONS)) {
				if (IS_DRAGON(i)) {
					strcat(buffer, " &W(&RD&Br&Ga&Cg&Yo&Pn&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_DEMONS)) {
				if (IS_DEMON(i)) {
					strcat(buffer, " &W(&RD&Kemo&Rn&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_GIANTS)) {
				if (IS_GIANT(i)) {
					strcat(buffer, " &W(&BG&Ci&Wa&Cn&Bt&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_SHADOWS)) {
				if (IS_SHADOW(i)) {
					strcat(buffer, " &W(&KSh&wad&Kow&W)&n");
				}
			}
			if (ha1375_affected_by_spell(ch, SPELL_DETECT_VAMPIRES)) {
				if (IS_VAMPIRE(i)) {
					strcat(buffer, " &W(&KV&Ra&wm&Kp&Ri&wr&Ke&W)&n");
				}
			}
			if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
				if (IS_EVIL(i))
					strcat(buffer, " &r(Evil)&n");
			}

			if (ha1375_affected_by_spell(ch, SPELL_DETECT_GOOD)) {
				if (IS_GOOD(i))
					strcat(buffer, " &W(Good)&n");
			}

			if (ha1375_affected_by_spell(i, SPELL_CHARM_PERSON)) {
				strcat(buffer, " &P(Charmed)&n");
			}

			if (IS_SET(GET_ACT2(i), PLR2_PKILLABLE)) {
				strcat(buffer, " <PK>");
			}

			if (IS_NPC(i) && GET_LEVEL(ch) > IMO_IMM)
				sprintf(buffer + strlen(buffer), " &R(%d)&E", (i->nr >= 0) ? mob_index[i->nr].virtual : 0);

			ansi_act(buffer, FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);
		}

		if (IS_AFFECTED(i, AFF_SANCTUARY))
			act("&W..$n &Wglows with a bright white light!&n", FALSE, i, 0, ch, TO_VICT);
		if (IS_CASTED_ON(i, SPELL_FAERIE_FIRE))
			act("&W..$n &Wis outlined in a &Pbright pink&W aura!&n", FALSE, i, 0, ch, TO_VICT);
		if (IS_CASTED_ON(i, SPELL_FAERIE_FOG))
			act("&W..$n &Wis outlined in a &pdeep purple&W aura!&n", FALSE, i, 0, ch, TO_VICT);

	}
	else if (mode == 1) {

		if (i->player.description) {
			ansi(CLR_DESC, ch);
			co2900_send_to_char(ch, "%s", i->player.description);
			ansi(END, ch);
		}
		else {
			act("You see nothing special about $m.", FALSE, i, 0, ch, TO_VICT);
		}

		/* Show a character to another */

		if (GET_HIT_LIMIT(i) > 0)
			percent = (100 * GET_HIT(i)) / GET_HIT_LIMIT(i);
		else
			percent = -1;	/* How could MAX_HIT be < 1?? */

		if (IS_NPC(i))
			strcpy(buffer, i->player.short_descr);
		else
			strcpy(buffer, GET_NAME(i));

		if (percent >= 100)
			strcat(buffer, " is in an excellent condition.");
		else if (percent >= 90)
			strcat(buffer, " has a few scratches.");
		else if (percent >= 75)
			strcat(buffer, " has some small wounds and bruises.");
		else if (percent >= 50)
			strcat(buffer, " has quite a few wounds.");
		else if (percent >= 30)
			strcat(buffer, " has some big nasty wounds and scratches.");
		else if (percent >= 15)
			strcat(buffer, " looks pretty hurt.");
		else if (percent >= 0)
			strcat(buffer, " is in an awful condition.");
		else
			strcat(buffer, " is bleeding awfully from big wounds.");

		ansi_act(buffer, FALSE, ch, 0, 0, TO_CHAR, CLR_MOB, 0);

		found = FALSE;
		for (j = 0; j < MAX_WEAR; j++) {
			if (i->equipment[j]) {
				if (CAN_SEE_OBJ(ch, i->equipment[j])) {
					found = TRUE;
				}
			}
		}
		if (found) {
			act("\r\n$n is using:", FALSE, i, 0, ch, TO_VICT);
			for (j = 0; j < MAX_WEAR; j++) {
				if (i->equipment[j]) {
					if (CAN_SEE_OBJ(ch, i->equipment[j])) {
						send_to_char((char *) where[j], ch);
						in2500_show_obj_to_char(i->equipment[j], ch, 1, -1);
					}
				}
			}
		}
		/* DEFAULT TO CAN SEE */
		lv_show = TRUE;
		/* IF YOU LOOK AT YOURSELF, DON'T SHOW INVENTORY */
		if (ch == i) {
			lv_show = FALSE;
		}
		/* IF LOOKING AT AN IMMORT, YOU HAVE TO BE SAME LEVEL OR HIGHER */
		if (GET_LEVEL(i) > PK_LEV) {
			if (GET_LEVEL(ch) < GET_LEVEL(i)) {
				lv_show = FALSE;
			}
		}
		else {
			/* IF NOT A THIEF, FORGET IT */
			if (GET_CLASS(ch) != CLASS_THIEF) {
				lv_show = FALSE;
			}
		}

		if (lv_show == TRUE) {
			found = FALSE;
			send_to_char("\r\nYou attempt to peek at the inventory:\r\n", ch);
			/*
			       for(tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
				       if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0,20) < GET_LEVEL(ch))) {
					       in2500_show_obj_to_char(tmp_obj, ch, 1, -1);
					       found = TRUE;
				       }
			       }
			       if (!found)
			       send_to_char("You can't see anything.\r\n", ch);
			       */
			in2600_list_obj_to_char(i->carrying, ch, 1, TRUE);
		}		/* END OF lv_show == TRUE; */

	}
	else {
		if (mode == 2) {

			/* Lists inventory */
			act("$n is carrying:", FALSE, i, 0, ch, TO_VICT);
			in2600_list_obj_to_char(i->carrying, ch, 1, TRUE);
		}
	}

	return;

}				/* END OF in2700_show_char_to_char() */


void in2800_list_people_to_char(struct char_data * list, struct char_data * ch, int mode)
{
	char buf[MAX_STRING_LENGTH];
	int lv_number, lv_show;
	struct char_data *i;



	lv_number = 0;
	for (i = list; i; i = i->next_in_room) {
		lv_show = TRUE;
		if (ch == i)
			lv_show = FALSE;




		if (!(CAN_SEE(ch, i)))
			lv_show = FALSE;

		if ((IS_AFFECTED(i, AFF_HIDE))) {
			if (!(IS_AFFECTED(ch, AFF_SENSE_LIFE)))
				if (GET_LEVEL(ch) < IMO_IMM)
					lv_show = FALSE;
		}

		if (lv_show == TRUE) {
			lv_number++;
			/* SAFTY CHECK */
			if (lv_number > 100) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: excessive mobs in room %d\r\n",
					world[ch->in_room].number);
				do_sys(buf, MAXV(IMO_IMM, GET_LEVEL(ch)), ch);
				spec_log(buf, ERROR_LOG);
				send_to_char("There are more mobs but I ran out of buffer space.\r\n", ch);
				break;
			}
			in2700_show_char_to_char(i, ch, 0);
		}
	}
}
#define OBJ_MIRROR_SHIELD 1757
#define OBJ_VANITY_MIRROR 15532
#define OBJ_SHARD_PARADIGM 183

void in3000_do_look(struct char_data * ch, char *arg, int cmd)
{

	char buffer[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	int keyword_no;
	int was_in, lv_veh_room, lv_save_riding, j, bits, temp;
	bool found;
	struct obj_data *tmp_object, *found_object;
	struct char_data *tmp_char;
	char *tmp_desc;
	signed char percent;
	const char *keywords[] = {
		"north",
		"east",
		"south",
		"west",
		"up",
		"down",
		"in",
		"at",
		"",		/* Look at '' case */
	"\n"};


	if (!ch)
		return;

	if (!ch->desc)		/* mud crashes here */
		return;

	if (GET_POS(ch) < POSITION_SLEEPING) {
		send_to_char("You can't see anything but stars!\r\n", ch);
	}
	else if (GET_POS(ch) == POSITION_SLEEPING)
		send_to_char("You can't see anything, you're sleeping!\r\n", ch);
	else if (GET_LEVEL(ch) < IMO_SPIRIT && IS_AFFECTED(ch, AFF_BLIND))
		send_to_char("You can't see a damn thing, you're blinded!\r\n", ch);
	else if (IS_DARK(ch, ch->in_room) && !IS_AFFECTED(ch, AFF_DARKSIGHT) && GET_LEVEL(ch) < IMO_LEV)
		send_to_char("It is pitch black...\r\n", ch);
	else {
		in2200_argument_split_2(arg, arg1, arg2);
		keyword_no = search_block(arg1, keywords, FALSE);

		if ((keyword_no == -1) && *arg1) {
			keyword_no = 7;
			strcpy(arg2, arg1);
		}

		found = FALSE;
		tmp_object = 0;
		tmp_char = 0;
		tmp_desc = 0;

		/* Check if PEEK is used instead of LOOK */
		if (cmd == CMD_PEEK) {
			if (keyword_no != 7) {
				send_to_char("What do you want to peek at?\r\n", ch);
				return;
			}
			if (ch->skills[SKILL_PEEK].learned == 0) {
				send_to_char("You have no idea how to do that!\r\n", ch);
				return;
			}
			percent = number(1, 101);
			if (percent > ch->skills[SKILL_PEEK].learned) {
				send_to_char("You fail to peek!\r\n", ch);
				return;
			}
		}
		/* end of PEEK part */

		switch (keyword_no) {
			/* look <dir> */
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:{

				if (EXIT(ch, keyword_no)) {

					sprintf(buffer, "Looking %s, you see:\r\n",
						dirs[keyword_no]);
					send_to_char(buffer, ch);
					if (IS_SET(EXIT(ch, keyword_no)->exit_info, EX_CLOSED) &&
					    (EXIT(ch, keyword_no)->keyword)) {
						sprintf(buffer, "The %s is closed.\r\n",
							ha1100_fname(EXIT(ch, keyword_no)->keyword));
						send_to_char(buffer, ch);
					}
					else {
						if (IS_SET(EXIT(ch, keyword_no)->exit_info, EX_ISDOOR) &&
						    EXIT(ch, keyword_no)->keyword) {
							sprintf(buffer, "The %s is open.\r\n",
								ha1100_fname(EXIT(ch, keyword_no)->keyword));
							send_to_char(buffer, ch);
						}
						if (EXIT(ch, keyword_no)->general_description) {
							send_to_char(EXIT(ch, keyword_no)->general_description, ch);
						}
						else {
							send_to_char("You see nothing special.\r\n", ch);
						}
					}
				}
				else {
					send_to_char("Nothing special there..\r\n", ch);
				}
			}
			break;

			/* look 'in'   */
		case 6:{
				if (*arg2) {
					/* Item carried */

					bits = ha3500_generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM |
								   FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

					if (bits) {	/* Found something */
						if (GET_ITEM_TYPE(tmp_object) == ITEM_DRINKCON) {
							if (tmp_object->obj_flags.value[1] <= 0) {
								act("It is empty.", FALSE, ch, 0, 0, TO_CHAR);
							}
							else {
								temp = ((tmp_object->obj_flags.value[1] * 3) / tmp_object->obj_flags.value[0]);
								if (temp > 3)
									temp = 3;
								sprintf(buffer, "It's%sfull of a %s liquid.\r\n",
									fullness[temp], color_liquid[tmp_object->obj_flags.value[2]]);
								send_to_char(buffer, ch);
							}
						}
						else if (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER ||
							 GET_ITEM_TYPE(tmp_object) == ITEM_QSTCONT) {
							if (!IS_SET(tmp_object->obj_flags.value[1], CONT_CLOSED) ||
							    tmp_object->obj_flags.value[1] == ch->nr
								) {
								send_to_char(ha1100_fname(tmp_object->name), ch);
								switch (bits) {
								case FIND_OBJ_INV:
									send_to_char(" (carried) : \r\n", ch);
									break;
								case FIND_OBJ_ROOM:
									send_to_char(" (here) : \r\n", ch);
									break;
								case FIND_OBJ_EQUIP:
									send_to_char(" (used) : \r\n", ch);
									break;
								}
								in2600_list_obj_to_char(tmp_object->contains, ch, 2, TRUE);
								if (ch->desc && ch->desc->snoop.snoop_by)
									send_to_char("\r\n", ch);
							}
							else {
								send_to_char("It is closed.\r\n", ch);
							}
						}
						else {
							if (GET_ITEM_TYPE(tmp_object) == ITEM_VEHICLE) {
								send_to_char("Inside the vehicle you see:\r\n", ch);
								lv_veh_room = vh500_get_vehicle_room(ch, "\0", 0, tmp_object);
								if (lv_veh_room < 0) {
									send_to_char("    nothing.\r\n", ch);
								}
								else {
									lv_save_riding = ch->riding;
									was_in = ch->in_room;
									ha1500_char_from_room(ch);
									ha1600_char_to_room(ch, lv_veh_room);
									in3000_do_look(ch, "\0", cmd);
									ha1500_char_from_room(ch);
									ha1600_char_to_room(ch, was_in);
									ch->riding = lv_save_riding;
								}
							}
							else {
								send_to_char("That is not a container.\r\n", ch);
							}
						}
					}
					else {	/* wrong argument */
						send_to_char("You do not see that item here.\r\n", ch);
					}
				}
				else {	/* no argument */
					send_to_char("Look in what?!\r\n", ch);
				}
			}
			break;

			/* look 'at'   */
		case 7:{


				if (*arg2) {

					if (cmd == CMD_ADMIN)
						tmp_char = ch->desc->admin;
					else
						bits = ha3500_generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM |
									   FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &tmp_char, &found_object);
					if (tmp_char) {
						/* Odin:	Code for
						 * Onslaughts Mirror Shield 02
						 * / 01 / 01-- update rlum 12.3
						 * .04 */
						if (tmp_char->equipment[WEAR_SHIELD])
							if (obj_index[tmp_char->equipment[WEAR_SHIELD]->item_number].virtual == OBJ_MIRROR_SHIELD) {
								if (GET_LEVEL(ch) < IMO_IMM) {
									send_to_char("You see your reflection!\r\n", ch);
									in2700_show_char_to_char(ch, ch, 1);
									return;
								}
							}
						if (tmp_char->equipment[HOLD])
							if (obj_index[tmp_char->equipment[HOLD]->item_number].virtual == OBJ_VANITY_MIRROR) {
								if (GET_LEVEL(ch) < IMO_IMM) {
									send_to_char("You see your reflection!\r\n", ch);
									in2700_show_char_to_char(ch, ch, 1);
									return;
								}
							}


						in2700_show_char_to_char(tmp_char, ch, 1);
						if (ch != tmp_char) {

							if (cmd != CMD_PEEK) {
								act("$n looks at you.", TRUE, ch, 0, tmp_char, TO_VICT);
								act("$n looks at $N.", TRUE, ch, 0, tmp_char, TO_NOTVICT);
							}
						}
						return;
					}


					/* Search for Extra Descriptions in
					 * room and items */

					/* Extra description in room?? */
					if (!found) {
						tmp_desc = in2400_find_ex_description(arg2,
										      world[ch->in_room].ex_description);
						if (tmp_desc) {
							if (ch->desc) {
								ansi(CLR_DESC, ch);
								page_string(ch->desc, tmp_desc, 1);
								ansi(END, ch);
							}
							return;	/* RETURN SINCE IT WAS A
								 * ROOM DESCRIPTION */
							/* Old system was:
							 * found = TRUE; */
						}
					}

					/* Search for extra descriptions in
					 * items */

					/* Equipment Used */

					if (!found) {
						for (j = 0; j < MAX_WEAR && !found; j++) {
							if (ch->equipment[j]) {
								if (CAN_SEE_OBJ(ch, ch->equipment[j])) {
									tmp_desc = in2400_find_ex_description(arg2,
													      ch->equipment[j]->ex_description);
									if (tmp_desc &&
									    ch->desc) {
										ansi(CLR_DESC, ch);
										page_string(ch->desc, tmp_desc, 1);
										found = TRUE;
										ansi(END, ch);
									}
								}
							}
						}
					}

					/* In inventory */

					if (!found) {
						for (tmp_object = ch->carrying;
						     tmp_object && !found;
						     tmp_object = tmp_object->next_content) {
							if CAN_SEE_OBJ
								(ch, tmp_object) {
								tmp_desc = in2400_find_ex_description(arg2,
												      tmp_object->ex_description);
								if (tmp_desc &&
								    ch->desc) {
									ansi(CLR_DESC, ch);
									send_to_char(tmp_desc, ch);
									found = TRUE;
									ansi(END, ch);
								}
								}
						}
					}

					/* Object In room */

					if (!found) {
						for (tmp_object = world[ch->in_room].contents;
						     tmp_object && !found;
						     tmp_object = tmp_object->next_content) {
							if CAN_SEE_OBJ
								(ch, tmp_object) {
								tmp_desc = in2400_find_ex_description(arg2,
												      tmp_object->ex_description);
								if (tmp_desc &&
								    ch->desc) {
									ansi(CLR_DESC, ch);
									page_string(ch->desc, tmp_desc, 1);
									found = TRUE;
									ansi(END, ch);
								}
								}
						}
					}
					/* wrong argument */

					if (bits) {	/* If an object was
							 * found */
						if (!found)
							in2500_show_obj_to_char(found_object, ch, 5, -1);	/* Show no-description */
						else
							in2500_show_obj_to_char(found_object, ch, 6, -1);	/* Find hum, glow etc */
					}
					else if (!found) {
						send_to_char("You do not see that here.\r\n", ch);
					}
				}
				else {
					/* no argument */

					send_to_char("Look at what?!\r\n", ch);
				}
			}
			break;


/* look ''     */
		case 8:{
				ansi(CYAN, ch);
				sprintf(buffer, "\r\n%s", world[ch->in_room].name);
				send_to_char(buffer, ch);
				ansi(END, ch);
				send_to_char("\r\n", ch);

				if (!IS_SET(GET_ACT3(ch), PLR3_BRIEF) ||
				    cmd == CMD_LOOK) {
					ansi(CLR_DESC, ch);
					send_to_char(world[ch->in_room].description, ch);
					ansi(END, ch);
				}

				in2600_list_obj_to_char(world[ch->in_room].contents, ch, 0, FALSE);
				in2800_list_people_to_char(world[ch->in_room].people, ch, 0);
			}
			break;

/* wrong arg   */
		case -1:
			send_to_char("Sorry, I didn't understand that!\r\n", ch);
			break;
		}
	}

}				/* END OF in3000_do_look() */


int in3050_do_vehicle_look(struct char_data * ch, char *arg, int cmd)
{



	if (ch->in_room < 0)
		return (FALSE);

	/* ARE WE ON A VEHICLE? */
	if (cmd == CMD_LOOK &&
	    world[ch->in_room].number >= VEHICLE_BEG_ROOM &&
	    world[ch->in_room].number <= VEHICLE_END_ROOM) {
		send_to_char("Looking outside you see:\r\n", ch);
		in3000_do_look(ch, arg, cmd);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF in3050_do_vehicle_look() */


void in3100_do_read(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];



	sprintf(buf, "at %s", arg);
	buf[99] = 0;
	in3000_do_look(ch, buf, 0);

}				/* END OF in3100_do_read() */


void in3200_do_examine(struct char_data * ch, char *arg, int cmd)
{
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int bits;
	struct char_data *tmp_char;
	struct obj_data *tmp_object;



	sprintf(buf, "at %s", arg);
	in3000_do_look(ch, buf, 0);

	arg = one_argument(arg, name);

	if (!*name) {
		send_to_char("Examine what?\r\n", ch);
		return;
	}

	bits = ha3500_generic_find(name, FIND_OBJ_INV | FIND_OBJ_ROOM |
				   FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

	if (tmp_object) {
		if ((GET_ITEM_TYPE(tmp_object) == ITEM_DRINKCON) ||
		    (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER) ||
		    (GET_ITEM_TYPE(tmp_object) == ITEM_QSTCONT)) {
			send_to_char("When you look inside, you see:\r\n", ch);
			sprintf(buf, "in %s", arg);
			in3000_do_look(ch, buf, 0);
		}
	}
}				/* END OF in3200_do_examine() */


void in3300_do_exits(struct char_data * ch, char *arg, int cmd)
{

	int door;
	char buf[MAX_STRING_LENGTH];
	char *exits[] = {
		"North",
		"East ",
		"South",
		"West ",
		"Up   ",
		"Down "
	};



	*buf = '\0';

	if (ch->in_room > 0 &&
	    world[ch->in_room].number >= VEHICLE_BEG_ROOM &&
	    world[ch->in_room].number <= VEHICLE_END_ROOM) {
		vh1300_exit_from_vehicle(ch, arg, cmd);
		return;
	}

	for (door = 0; door <= 5; door++)
		if (EXIT(ch, door))
			if (EXIT(ch, door)->to_room != NOWHERE &&
			    !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
				if (IS_DARK(ch, EXIT(ch, door)->to_room) &&
				    GET_LEVEL(ch) < IMO_IMP && !IS_AFFECTED(ch, AFF_DARKSIGHT))
					sprintf(buf + strlen(buf), "%s - &KToo dark to tell.&n\r\n", exits[door]);
				else if (GET_LEVEL(ch) >= IMO_IMP) {
					if (IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
						sprintf(buf + strlen(buf), "%s - &Rr[&W%d&R] &c<HIDDEN> &w%s&n\r\n", exits[door],
							world[EXIT(ch, door)->to_room].number,
							world[EXIT(ch, door)->to_room].name);
					else
						sprintf(buf + strlen(buf), "%s - &Rr[&W%d&R]&w %s&n\r\n", exits[door],
							world[EXIT(ch, door)->to_room].number,
							world[EXIT(ch, door)->to_room].name);
				}
				else if (!IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
					sprintf(buf + strlen(buf), "%s - &w%s&n\r\n", exits[door],
					 world[EXIT(ch, door)->to_room].name);
			}
	send_to_char("Obvious exits:\r\n", ch);

	if (*buf) {
		ansi(WHITE, ch);
		send_to_char(buf, ch);
		ansi(END, ch);
	}
	else
		send_to_char("None.\r\n", ch);
}

void in3400_do_weather(struct char_data * ch, char *arg, int cmd)
{

	char buf[128];		/* messages are short here, only small buf
				 * needed */
	static char *sky_look[4] = {
		"cloudless",
		"cloudy",
		"rainy",
	"lit by flashes of lightning"};



	if (OUTSIDE(ch)) {
		sprintf(buf,
			"The sky is %s and %s.\r\n",
			sky_look[weather_info.sky],
			(weather_info.change >= 0 ? "You feel a warm wind from south." :
			 "Your foot tells you bad weather is due."));
		send_to_char(buf, ch);
	}
	else
		send_to_char("You have no feeling about the weather at all.\r\n", ch);
}


void in3500_do_help(struct char_data * ch, char *arg, int cmd)
{

	extern int top_of_helpt;
	extern struct help_index_element *help_index;
	extern FILE *help_fl;

	int lv_full_partial, chk, bot, top, mid, minlen;
	char buf[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];



	if (!ch->desc)
		return;

	for (; isspace(*arg); arg++);
	one_argument(arg, buf);

	if (!(*arg)) {
		send_to_char(help, ch);
		return;
	}

	/* IF HELP RACE OR CLASS, CALL ROUTINES */
	if (is_abbrev(buf, "races")) {
		arg = one_argument(arg, buf);
		in1000_do_show_races(ch, arg, CMD_HELP);
		return;
	}
	if (is_abbrev(buf, "classes")) {
		arg = one_argument(arg, buf);
		in1100_do_show_classes(ch, arg, CMD_HELP);
		return;
	}
	if (!help_index) {
		send_to_char("No help available.\r\n", ch);
		return;
	}

	for (lv_full_partial = 0; lv_full_partial < 2; lv_full_partial++) {
		bot = 0;
		top = top_of_helpt;
		for (; bot <= top;) {
			mid = (bot + top) / 2;
			minlen = strlen(arg);

			/* IF FIRST PASS, LOOK FOR EXACT MATCH ONLY */
			if (lv_full_partial == 0)
				chk = strcmp(arg, help_index[mid].keyword);
			else
				chk = strn_cmp(arg, help_index[mid].keyword, minlen);
			if (!chk) {
				fseek(help_fl, help_index[mid].pos, 0);
				*buffer = '\0';
				for (;;) {
					fgets(buf, 80, help_fl);
					if (*buf == '#')
						break;
					strcat(buffer, buf);
					strcat(buffer, "\r");
				}
				if (ch->desc) {
					page_string(ch->desc, buffer, 1);
				}
				return;
			}	/* END OF !chk */
			else if (bot >= top) {
				if (lv_full_partial == 0) {
					bot = top + 1;	/* FORCE FOR LOOP EXIT */
					continue;
				}
				else {
					send_to_char("There is no help on that word.\r\n", ch);
					//Log missing help entries.Shalira 23.07 .01
						sprintf(buf, "HELP: %s tried to get help on %s.", GET_REAL_NAME(ch), arg);
					main_log(buf);
					spec_log(buf, NO_HELP_LOG);
					return;
				}
			}
			else {
				if (chk > 0)
					bot = ++mid;
				else
					top = --mid;
			}
		}		/* END OF for (;;) loop */

	}			/* END OF lv_full_partial loop */


	send_to_char("Sorry, there is no help on that word.\r\n", ch);
	//Log missing help entries.Shalira 23.07 .01
		sprintf(buf, "HELP: %s tries to get help(2) on %s.", GET_REAL_NAME(ch), arg);
	main_log(buf);
	spec_log(buf, NO_HELP_LOG);
	return;

}				/* END OF in3500_do_help() */


void in3600_do_wizhelp(struct char_data * ch, char *arg, int cmd)
{
	char mediator_buf[MAX_STRING_LENGTH], questor_buf[MAX_STRING_LENGTH],
		ruler_buf[MAX_STRING_LENGTH], tmpbuf[MAX_STRING_LENGTH], allimm_buf[MAX_STRING_LENGTH],
		godonly_buf[MAX_STRING_LENGTH], builder_buf[MAX_STRING_LENGTH];
	int i;
	
	// Clear the spaces off the arg:
    for ( ; isspace(*arg); arg++);
	
	// If we're an DEI/IMP:
	if (GET_LEVEL(ch) == IMO_IMP) 
	{
	    if (*arg)
            co2900_send_to_char(ch, "Sorry, arguments are not yet implemented.\r\n");

        /* Initialize */
    	i = 0;
		bzero(mediator_buf, sizeof(mediator_buf));
		bzero(questor_buf, sizeof(questor_buf));
		bzero(ruler_buf, sizeof(ruler_buf));
		bzero(tmpbuf, sizeof(tmpbuf));
		bzero(allimm_buf, sizeof(allimm_buf));
		bzero(godonly_buf, sizeof(godonly_buf));
		bzero(builder_buf, sizeof(builder_buf));

    	while (*(command[i]) != '\n') {
    		snprintf(tmpbuf, MAX_STRING_LENGTH-1, "%s\r\n", command[i]);
    		if ((IS_SET(cmd_info[i+1].wiz_flags, WIZ_MEDIATOR_PERM))) {
    			strncat(mediator_buf, tmpbuf, sizeof(mediator_buf) - strlen(mediator_buf) - 1);
    		}
    		if ((IS_SET(cmd_info[i+1].wiz_flags, WIZ_QUESTOR_PERM))) {
    			strncat(questor_buf, tmpbuf, sizeof(questor_buf) - strlen(questor_buf) - 1);
    		}
    		if ((IS_SET(cmd_info[i+1].wiz_flags, WIZ_RULER_PERM))) {
    			strncat(ruler_buf, tmpbuf, sizeof(ruler_buf) - strlen(ruler_buf) - 1);
    		}
    		if ((IS_SET(cmd_info[i+1].wiz_flags, WIZ_BUILDER_PERM))) {
    			strncat(builder_buf, tmpbuf, sizeof(builder_buf) - strlen(builder_buf) - 1);
    		}
    		if (!(IS_SET(cmd_info[i+1].wiz_flags, WIZ_RULER_PERM)) &&
    			!(IS_SET(cmd_info[i+1].wiz_flags, WIZ_QUESTOR_PERM)) &&
    			!(IS_SET(cmd_info[i+1].wiz_flags, WIZ_MEDIATOR_PERM)) &&
    			!(IS_SET(cmd_info[i+1].wiz_flags, WIZ_BUILDER_PERM)) &&
    			cmd_info[i+1].minimum_level == IMO_IMM) {
    			strncat(allimm_buf, tmpbuf, sizeof(allimm_buf) - strlen(allimm_buf) - 1);
    		}
    		if (cmd_info[i+1].minimum_level == IMO_IMP) {
    			strncat(godonly_buf, tmpbuf, sizeof(godonly_buf) - strlen(godonly_buf) - 1);
    		}
    		i++;
    	}
    	co2900_send_to_char(ch, "Wizcommands without permission bits set (avail to all lvl 48s):\r\n%s\r\n"
    							"Wizcommands with permission bits set:\r\n"
    							"Mediator Commands:\r\n%s\r\n"
    							"Quest Commands:\r\n%s\r\n"
    							"Builder Commands:\r\n%s\r\n"
    							"Ruler Commands:\r\n%s\r\n"
    							"Wizcommands for Gods:\r\n%s\r\n",
    							allimm_buf, mediator_buf, questor_buf, builder_buf, ruler_buf, godonly_buf);
							
    	co2900_send_to_char(ch, "You have the following wiz permission bits set: (%ld)\r\n%s%s%s%s\r\n", GET_WIZ_PERM(ch),
    						IS_SET(GET_WIZ_PERM(ch), WIZ_MEDIATOR_PERM) ? "MEDIATOR " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_QUESTOR_PERM) ? "QUESTOR " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_RULER_PERM) ? "RULER " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_BUILDER_PERM) ? "BUILDER " : "");
	}
	else // Not an IMP/Dei, only show them what they can do/what they are!
	{
        bzero(tmpbuf, sizeof(tmpbuf));
        int no;
        for (i = 0, no = 1; *(command[i]) != '\n'; i++)
        {
            if (cmd_info[i+1].minimum_level == IMO_IMM &&
                (cmd_info[i+1].wiz_flags == 0 ||
                (IS_SET(GET_WIZ_PERM(ch), WIZ_MEDIATOR_PERM) &&
                IS_SET(cmd_info[i+1].wiz_flags, WIZ_MEDIATOR_PERM)) ||
                (IS_SET(GET_WIZ_PERM(ch), WIZ_QUESTOR_PERM) &&
                IS_SET(cmd_info[i+1].wiz_flags, WIZ_QUESTOR_PERM)) ||
                (IS_SET(GET_WIZ_PERM(ch), WIZ_RULER_PERM) &&
                IS_SET(cmd_info[i+1].wiz_flags, WIZ_RULER_PERM)) ||
                (IS_SET(GET_WIZ_PERM(ch), WIZ_BUILDER_PERM) &&
                IS_SET(cmd_info[i+1].wiz_flags, WIZ_BUILDER_PERM))))
            {
                
                snprintf(tmpbuf + strlen(tmpbuf), sizeof(tmpbuf)-1, "%-12s", command[i]);
                
                if (!(no % 7))
                    strncat(tmpbuf, "\r\n", sizeof(tmpbuf)-1);
                
                no++;
            }
        }
        co2900_send_to_char(ch, "You have access to the following commands:\r\n%s\r\n", tmpbuf);
        co2900_send_to_char(ch, "\r\nYou have the following wiz permission bits set: %s%s%s%s\r\n",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_MEDIATOR_PERM) ? "MEDIATOR " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_QUESTOR_PERM) ? "QUESTOR " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_RULER_PERM) ? "RULER " : "",
    						IS_SET(GET_WIZ_PERM(ch), WIZ_BUILDER_PERM) ? "BUILDER " : "");
	}
}


void in3700_do_commands(struct char_data * ch, char *arg, int cmd)
{



	if (ch->desc) {
		page_string(ch->desc, commands, 1);
	}
}

void in3800_do_spells(struct char_data * ch, char *arg, int cmd)
{
	struct affected_type *aff;

	char buf[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];

	struct char_data *k;
	ubyte prev_type;
	sh_int prev_duration;

	k = 0;
	bzero(buf3, sizeof(buf3));
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf3);
	/* no arg so default to self */
	if (!(*buf3)) {
		k = ch;
	}
	else {
		/* MORTALS AREN'T ALLOWED TO STAT ANYONE ELSE */
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are only allowed to get attributes on yourself.\r\n", ch);
			send_to_char("Enter SPELL without arguments.\r\n", ch);
			return;
		}
	}

	/* LOOK FOR VICTIM */
	if (!k) {
		k = ha3100_get_char_vis(ch, buf3);
		if (!k) {
			send_to_char("Unable to find that person.\r\n", ch);
			return;
		}
	}

	/* did we get the clear command? */
	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_IMP) {
		for (; isspace(*arg); arg++);
		if (is_abbrev(arg, "clear")) {
			if (IS_NPC(k) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("You can't do that to mobs.\r\n", ch);
				return;
			}
			else {
				ha1370_remove_all_spells(k);
			}
		}
		else {
			send_to_char("Enter SPELL <char> CLEAR to remove spells.\r\n", ch);
		}
	}

	prev_duration = prev_type = 0;
	/* Routine to show what spells a char is affected by */
	send_to_char("\r\nAffecting Spells:\r\n-----------------\r\n", ch);
	if (k->affected) {
		for (aff = k->affected; aff; aff = aff->next) {
			if (prev_type != aff->type) {
				if (aff->type == SPELL_TRANSFORM_DRAGON ||
				    aff->type == SPELL_SANCTUARY ||
				    aff->type == SPELL_TRANSFORM_BEAR ||
				    aff->type == SPELL_TRANSFORM_WOLF ||
				    aff->type == SPELL_TRANSFORM_MONKEY ||
				    aff->type == SPELL_TRANSFORM_NINJA ||
				    aff->type == SPELL_TRANSFORM_MANTICORE ||
				    aff->type == SPELL_TRANSFORM_WOMBLE ||
					aff->type == SPELL_TRANSFORM_CHIMERA ||
				    aff->type == SPELL_TRANSFORM_COW) {
					sprintf(buf, "&WSpell : '%s' Time left:[&R%3d&W]&n\r\n",
						spell_names[aff->type - 1],
						aff->duration);
				}
				else {
					sprintf(buf, "Spell : '%s' Time left:[%3d]\r\n",
						spell_names[aff->type - 1],
						aff->duration);
				}
				send_to_char(buf, ch);
				prev_type = aff->type;
			}
		}
	}
	else
		send_to_char("None.\r\n", ch);
}

void in3900_do_users(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], line[300], state[30], host[140], 
		name[20], name1[50], level[10], time_connected[50];
	int has_level;
	int invisible_level = 0, incognito_level = 0,
	  lv_count, lv_char_level, lv_user_to_clear;
	struct descriptor_data *d, *next_d;

	if (ch->desc->original) {
		lv_char_level = GET_LEVEL(ch->desc->original);
	}
	else {
		lv_char_level = GET_LEVEL(ch);
	}
	lv_user_to_clear = 0;
	for (; *arg && *arg == ' '; arg++);
	if (lv_char_level >= IMO_IMM) {
		if (*arg) {
			if (is_abbrev(arg, "clear")) {
				lv_user_to_clear = 32767;
			}
			if (is_number(arg)) {
				lv_user_to_clear = atoi(arg);
			}
		}
	}

	strcpy(buf, "\r\nNo. Name               State             Host                                       Time Connected\r\n");
	strcat(buf, "-----------------------------------------------------------------------------------------------------------\r\n");

	lv_count = 1;
	for (d = descriptor_list; d; d = next_d) {
		
		//Incase we're clearing.
		next_d = d->next;

		if (d->character && d->character->player.name) {
			invisible_level = GET_VISIBLE(d->character);
			incognito_level = GET_INCOGNITO(d->character);
		}

		/* GET HOST AND STATE, FIRST */

		bzero(state, sizeof(state));
		if (d->original)
			sprintf(state, "Switched"); //22
		else if (d->character && (STATE(d) == CON_PLAYING))
			snprintf(state, sizeof(state), position_types[GET_POS(d->character)]); //22
		else
			snprintf(state, sizeof(state), connected_types[STATE(d)]);

		bzero(host, sizeof(host));
		if (d->host)
			snprintf(host, sizeof(host), "[%s]", d->host);

		/* GET NAME */

		has_level = 0;

		if (lv_user_to_clear == lv_count) {
			sprintf(name, "CLEARING");
			co2500_close_single_socket(d);
		}
		else if (!d->character) {
			if (lv_user_to_clear == 32767) {
				sprintf(name, "CLEARING"); //16
				co2500_close_single_socket(d);
			}
			else
				sprintf(name, "UNDEFINED");
		}
		else if (d->original) {
			sprintf(name, d->original->player.name);
			if (GET_LEVEL(d->original)) {
				sprintf(level, "(%d)", GET_LEVEL(d->original));
				has_level = 1;
			}
		}
		else if (d->character->player.name) {
			snprintf(name, sizeof(name), d->character->player.name);
			if (GET_LEVEL(d->character)) {
				snprintf(level, sizeof(level), "(%d)", GET_LEVEL(d->character));
				has_level = 1;
			}
		}
		else
			sprintf(name, "UNDEFINED");

		if (has_level)
			snprintf(name1, sizeof(name1), "%s %s", name, level);
		else
			snprintf(name1, sizeof(name1), "%s", name);
			
		/* GET THE CONNECTED TIME */
        
        int minutes, hours, days, seconds;
        
        seconds = time(0) - d->connected_at;
        
        minutes = seconds / 60;
        seconds = seconds % 60;
        hours = minutes / 60;
        minutes = minutes % 60;
        days = hours / 24;
        hours = hours % 24;
        
        if (days)
            snprintf(time_connected, sizeof(time_connected), 
                     "%d days, %d:%.2d:%.2d", days, hours, minutes, seconds);
        else if (hours)
            snprintf(time_connected, sizeof(time_connected), 
                     "%d:%.2d:%.2d hours", hours, minutes, seconds);
        else if (minutes)
            snprintf(time_connected, sizeof(time_connected), 
                     "%.2d:%.2d minutes", minutes, seconds);
        else
            snprintf(time_connected, sizeof(time_connected), 
                     "%d second%s", seconds, (seconds != 1) ? "s" : "");
            
		/* ASSEMBLE THE LINE */

		snprintf(line, sizeof(line), "%3d %-18s %-17s %-42s %s\r\n",
				lv_count, name1, state, host, time_connected);

		if (lv_char_level >= invisible_level &&
		    lv_char_level >= incognito_level) {	/* Incognito Shalira
							 * 21.08.01 */
			strncat(buf, line, sizeof(buf));
			lv_count++;
		}
	}
	send_to_char(buf, ch);

}/* END OF do_user() */


void in4000_do_inventory(struct char_data * ch, char *arg, int cmd)
{
	send_to_char("\r\nYou are carrying:\r\n", ch);
	in2600_list_obj_to_char(ch->carrying, ch, 1, TRUE);
}


void in4100_do_equipment(struct char_data * ch, char *arg, int cmd)
{
	int j;
	bool found;



	send_to_char("\r\nYou are using:\r\n", ch);
	found = FALSE;
	for (j = 0; j < MAX_WEAR; j++) {
		if (ch->equipment[j]) {
			if (CAN_SEE_OBJ(ch, ch->equipment[j])) {
				send_to_char((char *) where[j], ch);
				in2500_show_obj_to_char(ch->equipment[j], ch, 1, -1);
				found = TRUE;
			}
			else {
				send_to_char((char *) where[j], ch);
				send_to_char("Something.\r\n", ch);
				found = TRUE;
			}
		}
	}
	if (!found) {
		send_to_char(" Nothing.\r\n", ch);
	}
}


void in4200_do_credits(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, credits, 1);
	}
}

void in4250_do_changelog(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, changelog, 1);
	}
}

void in4300_do_news(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, news, 1);
	}
}

void in4350_do_todo(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, todo, 1);
	}
}

void in4400_do_wipe(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, wipe, 1);
	}
}


void in4500_do_view_info(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, info, 1);
	}
}


void in4600_do_syslog(struct char_data * ch, char *arg, int cmd)
{
	if (ch->desc) {
		page_string(ch->desc, gv_syslog, 1);
	}
}

void in4700_do_wizlist(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];


	strcpy(buf, wizlist);

	if (ch->desc) {
		page_string(ch->desc, buf, 1);
	}
}


void in4800_do_where(struct char_data * ch, char *arg, int cmd)
{

	char name[MAX_STRING_LENGTH], name2[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	register struct char_data *i;
	struct descriptor_data *d;
	struct char_data *vict;
	int lv_number, lv_number_found;



	lv_number_found = 0;

	if (ch->in_room < 0) {
		send_to_char("Hrmmm your NOWHERE.\r\n", ch);
		return;
	}

	arg = one_argument(arg, name);
	if (!*name) {
		lv_number = 1;	/* START AT ONE BECAUSE WE CAN'T INCREMENT FOR */
		/* THOSE WE FAIL TO PRINT BECAUSE OF INVIS.    */
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_SPIRIT) {
			strcpy(buf, "\r\nPlayers:\r\n--------\r\n");
			for (d = descriptor_list; d; d = d->next) {
				if (d->character &&
				    (d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0) &&
				    CAN_SEE(ch, d->character) &&
				    !(d->original) &&
				    (!(IS_SET(GET_ACT2(d->character), PLR2_NO_LOCATE))
				     || GET_LEVEL(ch) >= IMO_IMM) &&
				    (world[ch->in_room].zone == world[d->character->in_room].zone)) {
					sprintf(buf, "%d %-20s - %s\r\n",
						lv_number,
						GET_REAL_NAME(d->character),
					   world[d->character->in_room].name);
					send_to_char(buf, ch);
					lv_number++;
					lv_number_found++;
				}
			}	/* END OF for loop */
		}
		else {
			strcpy(buf, "\r\nPlayers:\r\n--------\r\n");
			for (d = descriptor_list; d; d = d->next) {
				if (d->character &&
				    (d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0)) {
					if (d->original) {	/* If switched */
						vict = d->original;
						sprintf(buf, "%d. %-20s - r[%d] %s. In body of %s\r\n",
							lv_number,
							GET_REAL_NAME(vict),
						  world[vict->in_room].number,
						    world[vict->in_room].name,
						 GET_REAL_NAME(d->character));
					}
					else {
						vict = d->character;
						sprintf(buf, "%d. %-20s - r[%d] %s\r\n",
							lv_number,
							GET_REAL_NAME(vict),
							world[d->character->in_room].number,
							world[d->character->in_room].name);
					}

					if (GET_LEVEL(ch) >= GET_VISIBLE(vict) &&
					GET_LEVEL(ch) >= GET_INCOGNITO(vict) &&
					    GET_LEVEL(ch) >= GET_GHOST(vict)) {
						//Incognito & ghost Shalira 22.07 .01
							lv_number++;
						lv_number_found++;
						send_to_char(buf, ch);
					}
				}
			}	/* END of for loop */

		}		/* END OF else level > */

		if (lv_number_found < 1) {
			send_to_char("Couldn't find anything.\r\n", ch);
		}
		return;
	}			/* END OF NO NAME SPECIFIED */
	if ((strcmp(name, "mob") || strcmp(name, "Mob")) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		send_to_char("You can't do that!\r\n", ch);
		return;
	}
	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_IMM &&
	    is_number(name)) {
		/* LOOK FOR OBJECTS */
		in4950_where_object_argument(ch, name, cmd, 1);
		return;
	}
	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_SPIRIT &&
	    (!strcmp(name, "board") || !strcmp(name, "BOARD"))) {
		in4950_where_object_argument(ch, "board", cmd, 2);
		return;
	}
	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_SPIRIT &&
	    (!strcmp(name, "corpse") || !strcmp(name, "corpse"))) {
		in4950_where_object_argument(ch, "corpse", cmd, 2);
		return;
	}


	if (IS_PC(ch) &&
	    GET_LEVEL(ch) >= IMO_IMM &&
	    (!strcmp(name, "obj") || !strcmp(name, "OBJ"))) {
		/* LOOK FOR OBJECTS */
		in4950_where_object_argument(ch, arg, cmd, 2);
		if (GET_LEVEL(ch) < IMO_IMM) {
			sprintf(buf, "%s entered where obj%s", GET_REAL_NAME(ch), arg);
			do_wizinfo(buf, IMO_IMM, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}		/* log it */
		sprintf(buf, "%s entered command where obj%s", GET_REAL_NAME(ch), arg);
		do_wizinfo(buf, IMO_IMM, ch);
		spec_log(buf, GOD_COMMAND_LOG);
		return;
	}

	/* WE GOT AN ARGUMENT */
	bzero(buf, sizeof(buf));

	arg = one_argument(arg, name2);

	lv_number = 0;
	for (i = character_list; i; i = i->next) {
		if ((ha1150_isname(name, i->player.name) && ((!*name2) || ha1150_isname(name2, i->player.name))) &&
		    (!(IS_SET(GET_ACT2(i), PLR2_NO_LOCATE))
		     || GET_LEVEL(ch) >= IMO_IMM) &&
		    CAN_SEE(ch, i)) {
			if ((i->in_room >= 0) &&
			    ((IS_PC(ch) && GET_LEVEL(ch) > PK_LEV) ||
			(world[i->in_room].zone == world[ch->in_room].zone))) {

				lv_number++;
				lv_number_found++;
				if (GET_LEVEL(ch) > PK_LEV) {
					sprintf(buf, "%d. %-30s - r[%d] %s\r\n",
						lv_number,
						GET_REAL_NAME(i),
						world[i->in_room].number,
						world[i->in_room].name);
				}
				else {
					sprintf(buf, "%d. %-30s - %s\r\n",
						lv_number,
						GET_REAL_NAME(i),
						world[i->in_room].name);
				}
				send_to_char(buf, ch);


				if (GET_LEVEL(ch) < IMO_LEV)
					break;
			}
		}
	}			/* END OF for loop */

	if (lv_number_found == 0) {
		send_to_char("Couldn't find any such thing.\r\n", ch);
	}

	return;

}				/* END OF in4800_do_where() */


void in4950_where_object_argument(struct char_data * ch, char *arg, int cmd, int lv_flag)
{

	char name[MAX_STRING_LENGTH], name2[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	register struct obj_data *k;
	int lv_search, lv_number_found, lv_room;



	if (ch->in_room < 0) {
		send_to_char("Hrmmm your NOWHERE!\r\n", ch);
		return;
	}

	arg = one_argument(arg, name);
	arg = one_argument(arg, name2);
	lv_search = 0;
	lv_search = atoi(name);

	lv_number_found = 0;
	for (k = object_list; k; k = k->next) {
		if (((lv_flag == 1 &&
		      obj_index[k->item_number].virtual == lv_search) ||
		     ((ha1150_isname(name, k->name)) && ((!*name2) || ha1150_isname(name2, k->name)))) &&
		    CAN_SEE_OBJ_NOLOCATE(ch, k)) {
			if (k->carried_by) {
				if (CAN_SEE(ch, k->carried_by)) {
					lv_room = k->carried_by->in_room;
					if (lv_room > 0) {
						lv_room = world[lv_room].number;
					}
					lv_number_found++;
					bzero(buf, sizeof(buf));
					sprintf(buf, "%d. %s carried by %s r[%d].\r\n",
						lv_number_found,
						k->short_description, PERS(k->carried_by, ch),
						lv_room);
					send_to_char(buf, ch);
				}
			}
			else {
				if (k->in_obj) {
					lv_room = k->in_obj->in_room;
					if (lv_room > 0) {
						lv_room = world[lv_room].number;
					}
					lv_number_found++;
					bzero(buf, sizeof(buf));
					sprintf(buf, "%d. %s%s.\r\n",
						lv_number_found,
						k->short_description,
					 in4960_who_has_the_object(ch, k, 0));
					send_to_char(buf, ch);
				}
				else {
					lv_number_found++;
					bzero(buf, sizeof(buf));
					if (k->in_room >= 0) {
						sprintf(buf, "%d. %-30s - r[%d] %s\r\n",
							lv_number_found,
							k->short_description,
						     world[k->in_room].number,
						      world[k->in_room].name);
						send_to_char(buf, ch);
					}
					else {
						lv_number_found++;
						sprintf(buf, "%d. %s in NOWHERE.\r\n",
							lv_number_found,
							k->short_description);
						send_to_char(buf, ch);
					}
				}
			}
		}
	}

	if (lv_number_found == 0) {
		send_to_char("Couldn't find any such thing.\r\n", ch);
	}
	return;

}				/* END OF in4950_where_object_argument() */


char *in4960_who_has_the_object(struct char_data * ch, struct obj_data * arg_obj, int lv_flag)
{

	int lv_room;
	struct obj_data *obj;
	char buf[MAX_STRING_LENGTH];



	bzero(gv_str, sizeof(gv_str));
	obj = arg_obj;

	while (obj->in_obj) {
		sprintf(buf, " in %s",
			obj->in_obj->short_description);
		strcat(gv_str, buf);
		obj = obj->in_obj;
	}

	/* DO WE KNOW WHO HAS IT NOW? */
	if (obj->carried_by) {
		if (CAN_SEE(ch, obj->carried_by)) {
			lv_room = obj->carried_by->in_room;
			if (lv_room > 0) {
				lv_room = world[lv_room].number;
			}
			bzero(buf, sizeof(buf));
			sprintf(buf, " carried by %s [%d]",
				PERS(obj->carried_by, ch),
				lv_room);
			strcat(gv_str, buf);
		}		/* END OF can see */
		else {
			strcat(gv_str, "in NOWHERE");
		}
	}			/* END OF CARRIED BY */
	else {
		bzero(buf, sizeof(buf));
		if (obj->in_room >= 0) {
			sprintf(buf, " in %s [%d]",
				world[obj->in_room].name,
				world[obj->in_room].number);
			strcat(gv_str, buf);
		}
		else {
			strcat(gv_str, " in NOWHERE");
		}
	}			/* END OF else */

	/* CHOP IF OFF FOR SAFETY */
	gv_str[250] = 0;
	return (gv_str);

}				/* END OF in4960_who_has_the_object() */


void in4975_whererent(struct char_data * ch, char *arg, int cmd)
{

	FILE *file_in;
	struct obj_file_u eqp_head;
	struct obj_file_elem eqp_elem;
	int size_of_header, size_of_element, lv_search, lv_file_count, lv_find_count, rc,
	  idx;
	char *ptr, filename[256], buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	size_of_header = sizeof(struct obj_file_u);
	size_of_element = sizeof(struct obj_file_elem);

	for (; *arg == ' '; arg++);
	if (is_number(arg)) {
		lv_search = atoi(arg);
	}
	else {
		send_to_char("Sorry, I need a numeric item to search for.\r\n",
			     ch);
		return;
	}

	lv_file_count = 0;
	lv_find_count = 0;

	for (idx = 0; idx < top_of_p_table; idx++) {
		bzero(buf, sizeof(buf));
		strcpy(buf, player_table[idx].pidx_name);
		if (*buf == '1' || *buf == '2') {	/* CHAR NOT DELETED */
			continue;
		}
		rc = r1000_rent_get_filename(buf, filename);
		file_in = fopen(filename, "rb+");
		if (!file_in)
			continue;
		fread(&eqp_head, size_of_header, 1, file_in);
		if (feof(file_in)) {
			fclose(file_in);
			continue;
		}
		lv_file_count++;
		ptr = (char *) &eqp_elem;
		bzero(ptr, sizeof(eqp_elem));
		fread(&eqp_elem, size_of_element, 1, file_in);
		while (!feof(file_in)) {
			if (eqp_elem.item_number == lv_search) {
				sprintf(buf2, "    %s has eqp: %d.\r\n", buf, lv_search);
				send_to_char(buf2, ch);
				lv_find_count++;
			}
			fread(&eqp_elem, size_of_element, 1, file_in);
		}
		fclose(file_in);
	}
	if (lv_find_count) {
		sprintf(buf2, "Found %d occurrences in %d files.\r\n",
			lv_find_count, lv_file_count);
	}
	else {
		sprintf(buf2, "Unable to locate %d in any rent files.\r\n",
			lv_search);
	}
	send_to_char(buf2, ch);

	return;

}				/* END OF in4975_whererent_argument() */

void in5000_do_levels(struct char_data * ch, char *arg, int cmd)
{

	int lv_row, lv_level, idx, jdx;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	for (; *arg == ' '; arg++);

	/* HOW MANY CAN WE PRINT DOWN? */
	if (GET_LEVEL(ch) < IMO_LEV)
		lv_row = (IMO_LEV / EXP_ACROSS);
	else
		lv_row = (MAX_LEV / EXP_ACROSS);

	bzero(buf, sizeof(buf));
	sprintf(buf, "You're a level %d %s %s\r\n",
		GET_LEVEL(ch),
		races[GET_RACE(ch)].name,
		classes[GET_CLASS(ch)].name);
	strcat(buf, "Amount beside level + 1 required to advance to next level.\r\n");

	for (idx = 0; idx < lv_row; idx++) {
		bzero(buf2, sizeof(buf));
		for (jdx = 0; jdx < EXP_ACROSS; jdx++) {
			lv_level = 1 + idx + (jdx * lv_row);
			sprintf(buf2, "%-2d. %-10d  ",
				lv_level,
				LEVEL_EXP(lv_level));
			strcat(buf, buf2);
		}
		strcat(buf, "\r\n");
	}
	send_to_char(buf, ch);
	return;

}				/* END in5000_do_level() */


void in5100_do_consider(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *victim;
	char name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int lv_ch_dmg, lv_vict_dmg, dam, diff;



	arg = one_argument(arg, name);
	if (!*name) {
		send_to_char("Consider who?\r\n", ch);
		return;
	}

	victim = ha2125_get_char_in_room_vis(ch, name);
	if (!victim) {
		send_to_char("Consider who?\r\n", ch);
		return;
	}

	send_to_char("Enter HELP CONSIDER for information on this command.\r\n", ch);
	send_to_char("Comparison          You      Opponent\r\n", ch);
	send_to_char("--------------      -------  --------\r\n", ch);

	if (victim == ch) {
		send_to_char("Easy! Very easy indeed!\r\n", ch);
		return;
	}

	diff = (GET_LEVEL(victim) - GET_LEVEL(ch));
	if (diff <= -25)
		sprintf(buf, "Would you like to borrow a cross and shovel?");
	else if (diff <= -20)
		sprintf(buf, "Very unsporting of you!");
	else if (diff <= -15)
		sprintf(buf, "Now where did the chickens go?");
	else if (diff <= -10)
		sprintf(buf, "Say the word and its history...");
	else if (diff <= -5)
		sprintf(buf, "You could do it with a needle!");
	else if (diff <= -2)
		sprintf(buf, "Easy.");
	else if (diff <= -1)
		sprintf(buf, "Fairly easy.");
	else if (diff == 0)
		sprintf(buf, "The perfect match!");
	else if (diff <= 1)
		sprintf(buf, "Slightly more skilled than you.");
	else if (diff <= 2)
		sprintf(buf, "A bit more skilled than you.");
	else if (diff <= 3)
		sprintf(buf, "Much more skilled than you!");
	else if (diff <= 5)
		sprintf(buf, "Do you feel lucky, punk? ");
	else if (diff <= 10)
		sprintf(buf, "You should Bring some friends!");
	else if (diff <= 15)
		sprintf(buf, "Bring *LOTS* of friends!?");
	else if (diff <= 20)
		sprintf(buf, "You should not mess with them ");
	else if (diff <= 35)
		sprintf(buf, "They are practically a god compared to you");
	else
		sprintf(buf, "Some guys with straight jackets rushing over here!");

	send_to_char("Level          :     ", ch);
	sprintf(buf2, "%6d  %s\r\n", GET_LEVEL(ch), buf);
	send_to_char(buf2, ch);

	/*
	 0%-5%  A good match
	 6%-20% About the same
	 21-49% less than you
	 50%+   Far less than you
	 */
	/* HIT POINTS */
	if (GET_HIT_LIMIT(victim) < GET_HIT_LIMIT(ch)) {
		if (GET_HIT_LIMIT(victim) == 0)
			diff = GET_HIT_LIMIT(ch);
		else
			diff = (GET_HIT_LIMIT(ch) / GET_HIT_LIMIT(victim)) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 21)
			sprintf(buf, "Slightly less than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit less than you.");
		else
			sprintf(buf, "Way less than you.");
	}
	else {
		if (GET_HIT_LIMIT(ch) == 0)
			diff = GET_HIT_LIMIT(victim);
		else
			diff = (GET_HIT_LIMIT(victim) / GET_HIT_LIMIT(ch)) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 21)
			sprintf(buf, "Slightly more than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit higher than you.");
		else
			sprintf(buf, "Way higher than you.");
	}
	send_to_char("Hits           :     ", ch);
	sprintf(buf2, "%6d  %s\r\n", GET_HIT_LIMIT(ch), buf);
	send_to_char(buf2, ch);
	if (GET_LEVEL(ch) == IMO_IMP) {
		sprintf(buf, "percent        :      %5d  %5d\r\n", diff, GET_HIT_LIMIT(victim));
		send_to_char(buf, ch);
	}

	/* MANA POINTS */
	if (GET_MANA_LIMIT(victim) < GET_MANA_LIMIT(ch)) {
		if (GET_MANA_LIMIT(victim) == 0)
			diff = GET_MANA_LIMIT(ch);
		else
			diff = (GET_MANA_LIMIT(ch) / GET_MANA_LIMIT(victim)) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 21)
			sprintf(buf, "Slightly less than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit less than you.");
		else
			sprintf(buf, "Way less than you.");
	}
	else {
		if (GET_MANA_LIMIT(ch) == 0)
			diff = GET_MANA_LIMIT(victim);
		else
			diff = (GET_MANA_LIMIT(victim) / GET_MANA_LIMIT(ch)) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 21)
			sprintf(buf, "Slightly higher than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit higher than you.");
		else
			sprintf(buf, "Way higher than you.");
	}
	send_to_char("Magic          :     ", ch);
	sprintf(buf2, "%6d  %s\r\n", GET_MANA_LIMIT(ch), buf);
	send_to_char(buf2, ch);
	if (GET_LEVEL(ch) == IMO_IMP) {
		sprintf(buf, "percent        :      %5d  %5d\r\n",
			diff,
			GET_MANA_LIMIT(victim));
		send_to_char(buf, ch);
	}

	/* DAMAGE */
	if (ch->equipment[WIELD] &&
	    (ch->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON ||
	     ch->equipment[WIELD]->obj_flags.type_flag == ITEM_QSTWEAPON)) {
		dam = (ch->equipment[WIELD]->obj_flags.value[2] / 2 + .5) *
			ch->equipment[WIELD]->obj_flags.value[1];
		/*
		 dam = dice(ch->equipment[WIELD]->obj_flags.value[1],
								ch->equipment[WIELD]->obj_flags.value[2]);
		 */
	}
	else {
		if (IS_PC(ch))
			dam = number(0, 2);
		else
			dam = dice(ch->specials.damnodice,
				   ch->specials.damsizedice);
	}
	lv_ch_dmg = dam +
		GET_DAMROLL(ch) +
		li9700_adjust_damage_bonus(GET_STR(ch) + GET_BONUS_STR(ch));

	if (victim->equipment[WIELD] &&
	    (victim->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON ||
	   victim->equipment[WIELD]->obj_flags.type_flag == ITEM_QSTWEAPON)) {
		dam = (victim->equipment[WIELD]->obj_flags.value[2] / 2 + .5) *
			victim->equipment[WIELD]->obj_flags.value[1];
	}
	else {
		if (IS_PC(victim))
			dam = number(0, 2);
		else
			dam = (victim->specials.damsizedice / 2 + .5) * victim->specials.damnodice;
	}
	lv_vict_dmg = dam +
		GET_DAMROLL(victim) +
		li9700_adjust_damage_bonus(GET_STR(victim) + GET_BONUS_STR(victim));

	if (lv_vict_dmg < lv_ch_dmg) {
		if (lv_vict_dmg == 0)
			diff = lv_ch_dmg;
		else
			diff = (lv_ch_dmg / lv_vict_dmg) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 16)
			sprintf(buf, "Slightly less than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit less than you.");
		else
			sprintf(buf, "Way less than you.");
	}
	else {
		if (lv_ch_dmg == 0)
			diff = lv_vict_dmg;
		else
			diff = (lv_vict_dmg / lv_ch_dmg) * 100;
		if (diff < 6)
			sprintf(buf, "A good match.");
		else if (diff < 16)
			sprintf(buf, "Slightly higher than you.");
		else if (diff < 50)
			sprintf(buf, "A good bit higher than you.");
		else
			sprintf(buf, "Way higher than you.");
	}
	send_to_char("Damage         :     ", ch);
	sprintf(buf2, "%6d  %s\r\n", lv_ch_dmg, buf);
	send_to_char(buf2, ch);
	if (GET_LEVEL(ch) == IMO_IMP) {
		sprintf(buf, "percent        :      %5d  %5d\r\n",
			diff,
			lv_vict_dmg);
		send_to_char(buf, ch);
	}

	return;

}				/* END OF do_consider()  */


void in5200_do_ansi(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_INPUT_LENGTH];



	arg = one_argument(arg, buf);

	if (IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
		send_to_char("ANSI disabled.\r\n", ch);
		REMOVE_BIT(GET_ACT3(ch), PLR3_ANSI);
	}
	else {
		send_to_char("ANSI graphics enabled.\r\n", ch);
		SET_BIT(GET_ACT3(ch), PLR3_ANSI);
	}
}				/* END OF in5200_do_ansi() */


void in5300_update_top_list(struct char_data * ch, int list)
{

	int idx, jdx, top_size, lv_found, lv_score;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct top_stats *tlist;

	bzero(buf, sizeof(buf));

	if (GET_LEVEL(ch) > PK_LEV)
		return;

	bzero(buf, sizeof(buf));

	/* for every top list : lv_score - new score tlist    - Address of
	 * topx_ list top_size - Size of topx_ list buf      - Text to display
	 * must have 2 %d where 1st = new position in the list 2nd = size of
	 * the top list */

	switch (list) {
	case top_kills:{
			lv_score = GET_KILLS(ch);
			tlist = topx_kills;
			top_size = size_top_kills;
			strcpy(buf, "Nice kill, it made you nr. %d in the top %d killers list!\r\n");
			break;
		}
	case top_deaths:{
			lv_score = GET_DEATHS(ch);
			tlist = topx_deaths;
			top_size = size_top_deaths;
			strcpy(buf, "Sorry you died, but it did make you nr. %d in the top %d deaths list :)\r\n");
			break;
		}
	case top_avats:
	case top_mortals:
	case top_scores:{
			lv_score = GET_SCORE(ch);
			tlist = topx_scores;
			top_size = size_top_scores;
			strcpy(buf, "test %d %d\r\n");
			break;
		}
	default:
		return;
	}
	if (lv_score < 0)
		return;
	for (idx = 0; idx < top_size; idx++) {
		if (lv_score > tlist[idx].score) {
			if (strcmp(tlist[idx].name, GET_REAL_NAME(ch)) == 0) {
				tlist[idx].score = lv_score;
				return;
			}
			lv_found = 0;
			for (jdx = idx + 1; jdx < top_size; jdx++) {
				if ((strcmp(tlist[jdx].name, GET_REAL_NAME(ch)) == 0)) {
					tlist[jdx].score = tlist[idx].score;
					strcpy(tlist[jdx].name, tlist[idx].name);
					lv_found = 1;
				}
			}
			if (!lv_found) {
				for (jdx = top_size - 1; jdx > idx; jdx--) {
					tlist[jdx].score = tlist[jdx - 1].score;
					strcpy(tlist[jdx].name, tlist[jdx - 1].name);
				}
			}
			tlist[idx].score = lv_score;
			strcpy(tlist[idx].name, GET_REAL_NAME(ch));
			if (ch->desc) {
				sprintf(buf2, buf, idx + 1, top_size);
				send_to_char(buf2, ch);
			}
			return;
		}
	}
	return;
}				/* end of in5300_update_top_list() */

void in5400_show_top_list(struct char_data * ch, int list)
{

	char buf[MAX_STRING_LENGTH];
	int idx, half_size;

	switch (list) {
	case top_kills:{
			sprintf(buf, "Top %d killers:\r\n", size_top_avats);
			send_to_char(buf, ch);
			half_size = size_top_kills >> 1;
			for (idx = 0; idx < half_size; idx++) {
				sprintf(buf, "%3d - %-20s %6ld   %3d - %-20s %ld\r\n",
					idx + 1,
					topx_kills[idx].name,
					topx_kills[idx].score,
					idx + half_size + 1,
					topx_kills[idx + half_size].name,
					topx_kills[idx + half_size].score);
				send_to_char(buf, ch);
			}
			return;
		}
	case top_deaths:{
			sprintf(buf, "Top %d deaths:\r\n", size_top_avats);
			send_to_char(buf, ch);
			half_size = size_top_deaths >> 1;
			for (idx = 0; idx < half_size; idx++) {
				sprintf(buf, "%3d - %-20s %6ld   %3d - %-20s %ld\r\n",
					idx + 1,
					topx_deaths[idx].name,
					topx_deaths[idx].score,
					idx + half_size + 1,
					topx_deaths[idx + half_size].name,
					topx_deaths[idx + half_size].score);
				send_to_char(buf, ch);
			}
			return;
		}
	case top_avats:
	case top_mortals:
		 /* case top_scores  : */ {
			if (list == top_scores)
				in5700_build_top_score_list(1, 41);
			if (list == top_mortals)
				in5700_build_top_score_list(1, 40);
			if (list == top_avats)
				in5700_build_top_score_list(41, 41);
			sprintf(buf, "Top %d scores:\r\n", size_top_scores);
			send_to_char(buf, ch);
			half_size = size_top_scores >> 1;
			for (idx = 0; idx < half_size; idx++) {
				sprintf(buf, "%3d - %-20s %7ld   %3d - %-20s %ld\r\n",
					idx + 1,
					topx_scores[idx].name,
					topx_scores[idx].score,
					idx + half_size + 1,
					topx_scores[idx + half_size].name,
					topx_scores[idx + half_size].score);
				send_to_char(buf, ch);
			}
			return;
		}
	}
	return;
}				/* end of in5400_show_top_list() */

void in5500_build_top_lists(void)
{

	struct char_file_u st;
	struct char_data ch;
	FILE *fl;
	int lv_player;

	trace_log("Building top scores list");

	/* ch.player.name = str_alloc ("Undefined name......"); */
	fl = fopen(PLAYER_FILE, "r");
	for (lv_player = 0; lv_player <= top_of_p_file; lv_player++) {
		fread(&st, sizeof(struct char_file_u), 1, fl);
		ch.points.stat_gold = st.points.stat_gold;	/* deaths */
		ch.points.fate = st.points.fate;	/* kills  */
		ch.points.score = st.points.score;
		ch.player.level = st.level;
		ch.desc = 0;
		ch.player.name = st.name;
		if (ch.player.name[0] <= '2')
			continue;
		ch.player.short_descr = st.name;
		in5300_update_top_list(&ch, top_kills);
		in5300_update_top_list(&ch, top_deaths);
	}
	fclose(fl);
	return;
}

void in5600_do_top(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int lv_choice;
	const char *lv_top_cmds[] = {
		"avats",
		"mortals",
		"kills",
		"deaths",
		"scores",
		"\n",
	};

#define TOP_AVATS   	1
#define TOP_MORTALS 	2
#define TOP_KILLS	3
#define TOP_DEATHS	4
#define TOP_SCORES	5


	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	lv_choice = 0;
	if (*buf)
		lv_choice = old_search_block(buf, 0, strlen(buf), lv_top_cmds, 0);

	if (lv_choice < 1) {
		send_to_char("Use: top <list>\r\n", ch);
		send_to_char("Available lists: mortals, avats, kills, deaths\r\n", ch);
		return;
	}

	switch (lv_choice) {
	case TOP_KILLS:{
			in5400_show_top_list(ch, top_kills);
			break;
		}
	case TOP_DEATHS:{
			in5400_show_top_list(ch, top_deaths);
			break;
		}
	case TOP_SCORES:{
			/* in5400_show_top_list(ch, top_scores); break; */
			send_to_char("Top scores replaced with 'Top mortals' and 'Top avats'.\r\n", ch);
			return;
		}
	case TOP_AVATS:{
			in5400_show_top_list(ch, top_avats);
			break;
		}
	case TOP_MORTALS:{
			in5400_show_top_list(ch, top_mortals);
			break;
		}
	default:{
			send_to_char("Sorry this list hasn't been implemented yet.\r\n", ch);
			return;
		}
	}
	return;
}				/* end of in5600_do_top() */

void in5700_build_top_score_list(int min_level, int max_level)
{

	int idx;
	char buf[MAX_STRING_LENGTH];
	struct char_data ch;

	/* ch.player.name = str_alloc ("Undefined name......"); */
	for (idx = 0; idx < size_top_scores; idx++) {
		topx_scores[idx].score = 0;
		strcpy(topx_scores[idx].name, "Empty.");
	}
	for (idx = 0; idx <= top_of_p_table; idx++) {
		if (player_table[idx].pidx_level > max_level)
			continue;
		if (player_table[idx].pidx_level < min_level)
			continue;
		ch.desc = 0;
		ch.points.score = player_table[idx].pidx_score;
		bzero(buf, sizeof(buf));
		strcpy(buf, player_table[idx].pidx_name);
		if (!*buf)
			continue;
		if (buf[0] >= 'a' && buf[0] <= 'z')
			buf[0] = buf[0] - 32;
		else
			continue;
		ch.player.name = buf;
		ch.player.short_descr = buf;
		ch.player.level = player_table[idx].pidx_level;
		in5300_update_top_list(&ch, top_scores);
	}
	return;
}				/* end of in5700_build_top_score_list(); */
