/* cl */
/* *******************************************************************
*  file: clans.c , Special module.           Part of Crimson MUD     *
*  Usage: Procedures handling special procedures                     *
*         for clans.  Written by Hercules                            *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "modify.h"
#include "ansi.h"
#include "globals.h"
#include "func.h"

void cl1000_do_clan(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char buf4[MAX_STRING_LENGTH];
	char buf5[MAX_STRING_LENGTH];
	char buf6[MAX_STRING_LENGTH];
	char sbuf[MAX_STRING_LENGTH * 2];
	char old_arg[MAX_STRING_LENGTH];
	char save_position;
	int lv_choice, lv_clan_nr, lv_clan_rank, lv_not_found, lv_offline_player, idx;
	struct char_data *vict;
	struct obj_data *obj;
	struct descriptor_data *d;
	const char *lv_clan_cmds[] = {
		"tell",		/* 1 */
		"advance",	/* 2 */
		"demote",	/* 3 */
		"apply",	/* 4 */
		"info",		/* 5 */
		"leave",	/* 6 */
		"where",	/* 7 */
		"who",		/* 8 */
		"donate",	/* 9 */
		"accept",	/* 10 */
		"deny",		/* 11 */
		"applicants",	/* 12 */
		"list",		/* 13 */
		"kick",		/* 14 */
		"enable",	/* 15 */
		"leader",	/* 16 */
		"war",		/* 17 */
		"save",		/* 18 */
		"create",	/* 19 */
		"\n",
	};
	int cmd_levels[] = {
		0,		/* 1 tell	 */
		10,		/* 2 advance 	 */
		11,		/* 3 demote	 */
		0,		/* 4 apply	 */
		0,		/* 5 info	 */
		0,		/* 6 leave	 */
		1,		/* 7 where	 */
		1,		/* 8 who	 */
		1,		/* 9 donate	 */
		10,		/* 10 allow	 */
		10,		/* 11 deny	 */
		10,		/* 12 applicants	 */
		0,		/* 13 list	 */
		10,		/* 14 kick	 */
		0,		/* 15 enable	 */
		0,		/* 16 leader	 */
		0,		/* 17 war	 */
		11,		/* 18 save       */
		11,		/* 19 create	 */
	};

#define CLAN_TELL	 1
#define CLAN_ADVANCE	 2
#define CLAN_DEMOTE	 3
#define CLAN_APPLY	 4
#define CLAN_INFO	 5
#define CLAN_LEAVE	 6
#define CLAN_WHERE	 7
#define CLAN_WHO	 8
#define CLAN_DONATE	 9
#define CLAN_ALLOW	 10
#define CLAN_DENY	 11
#define CLAN_APPLICANTS	 12
#define CLAN_LIST	 13
#define CLAN_KICK	 14
#define CLAN_ENABLE	 15
#define CLAN_LEADER	 16
#define CLAN_WAR	 17
#define CLAN_SAVE	 18
#define CLAN_CREATE	 19
#define CLAN_NOT_DEFINED 100

	lv_offline_player = 0;

	if (GET_LEVEL(ch) < 10) {
		send_to_char("Your level isn't high enough to use this command.\r\n", ch);
		return;
	}


	if ((IS_SET(GET_ACT2(ch), PLR2_JAILED)) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You can't tell to your clan while jailed.\r\n", ch);
		return;
	}

	/* if ct is used ass 'tell' to the argument */
	if (cmd == CMD_CLAN_TELL) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "tell %s", arg);
		strcpy(arg, buf);
	}

	/* Strip first argument */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	strcpy(buf6, arg);
	lv_choice = 0;
	if (*buf)
		lv_choice = old_search_block(buf, 0, strlen(buf), lv_clan_cmds, 0);

	if (cmd == CMD_CLAN_TELL)
		lv_choice = CLAN_TELL;

	if (lv_choice < 1) {
		send_to_char("None or invalid argument passed to set.\r\n", ch);
		send_to_char("Valid options are:\r\n\r\n", ch);
		send_to_char(" accept  <name>\r\n", ch);
		send_to_char(" advance <name> <clan rank>\r\n", ch);
		send_to_char(" applicants\r\n", ch);
		send_to_char(" apply   <clan number>\r\n", ch);
		send_to_char(" demote  <name> <clan rank>\r\n", ch);
		send_to_char(" deny    <name>\r\n", ch);
		send_to_char(" donate  <object>\r\n", ch);
		send_to_char(" info    <clan number>\r\n", ch);
		send_to_char(" leave   <clan number>\r\n", ch);
		send_to_char(" list\r\n", ch);
		send_to_char(" kick    <name>\r\n", ch);
		send_to_char(" tell    <message>\r\n", ch);
		send_to_char(" where\r\n", ch);
		send_to_char(" who\r\n", ch);
		if (GET_LEVEL(ch) >= IMO_IMM) {
			send_to_char("\r\nSpecial commands for your level:\r\n", ch);
			send_to_char(" enable <clan number>\r\n", ch);
			send_to_char(" leader <name> <clan number>\r\n", ch);
		}
		return;
	}
	for (; isspace(*arg); arg++);
	bzero(old_arg, sizeof(old_arg));
	strcpy(old_arg, arg);
	bzero(buf2, sizeof(buf2));
	arg = one_argument(arg, buf2);
	for (; isspace(*arg); arg++);
	bzero(buf3, sizeof(buf3));
	arg = one_argument(arg, buf3);


	/* Check if all needed arguments are given */
	switch (lv_choice) {
		/* commands that need minlevel */
	case CLAN_CREATE:
	case CLAN_ENABLE:
	case CLAN_LEADER:{
			if (GET_LEVEL(ch) < IMO_IMM) {
				send_to_char("Sorry your level isn't high enough.\r\n", ch);
				return;
			}
		} break;

	}
	switch (lv_choice) {
		/* commands that need you to be in a clan */
	case CLAN_SAVE:
	case CLAN_TELL:
	case CLAN_KICK:
	case CLAN_ALLOW:
	case CLAN_DENY:
	case CLAN_LEAVE:
	case CLAN_ADVANCE:
	case CLAN_DEMOTE:
	case CLAN_WHO:
	case CLAN_DONATE:
	case CLAN_WHERE:{
			if (CLAN_NUMBER(ch) == NO_CLAN) {
				send_to_char("But you are the member of no clan!\r\n", ch);
				return;
			}
			if (!(CLAN_EXISTS(CLAN_NUMBER(ch))) && CLAN_NUMBER(ch) != 10) {
				//*gasp * Magic number....
					send_to_char("Sorry your clan has been disbanded or disabled.\r\n", ch);
				return;
			}
			break;
		}
	}
	switch (lv_choice) {
		/* commands that need a victim as first argument */
	case CLAN_KICK:
	case CLAN_ADVANCE:
	case CLAN_DEMOTE:
	case CLAN_ALLOW:
	case CLAN_LEADER:
	case CLAN_DENY:{
			if (!*buf2) {
				send_to_char("Need a <name> for this command!\r\n", ch);
				return;
			}
			vict = 0;
			vict = ha3100_get_char_vis(ch, buf2);
			if (vict) {
				if (!IS_PC(vict))
					vict = 0;
			}
			if (!vict) {
				/*
				 bzero(sbuf, sizeof(sbuf));
				 sprintf(sbuf, "Unable to locate %s.\r\n", buf2);
				 send_to_char(sbuf, ch);
				 return;
				 */
				ad1300_select_player(ch, buf2);
				if (!ch->desc->admin)
					return;
				vict = ch->desc->admin;
				lv_offline_player = 1;
			}
			bzero(buf2, sizeof(buf2));
			strcpy(buf2, buf3);
			break;
		}
	}
	switch (lv_choice) {
		/* Commands that need clan number as first/second argument */
	case CLAN_LEAVE:
	case CLAN_ENABLE:
	case CLAN_INFO:
	case CLAN_LEADER:
	case CLAN_APPLY:{
			if (!*buf2) {
				if ((lv_choice != CLAN_INFO) &&
				    (lv_choice != CLAN_LEAVE)) {
					send_to_char("Need a <clan number> for this command!\r\n", ch);
					if (lv_offline_player)
						cl1100_clear_admin(ch);
					return;
				}
				else {
					lv_clan_nr = CLAN_NUMBER(ch);
					break;
				}
			}
			if (!is_number(buf2)) {
				send_to_char("<clan number> must be numeric.\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			lv_clan_nr = MAXV(0, atoi(buf2));
			if ((!CLAN_EXISTS(lv_clan_nr)) &&
			    (lv_choice != CLAN_ENABLE)) {
				sprintf(buf2, "Clan number %d does not exist or has been disabled!\r\n", lv_clan_nr);
				send_to_char(buf2, ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			break;
		}
	}
	switch (lv_choice) {
		/* Commands that need an object as first param */
	case CLAN_NOT_DEFINED:{
			if (!*buf2) {
				send_to_char("Need an <obj> for this command!\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			/* assuming the obj needs to be in inventory */
			obj = ha2075_get_obj_list_vis(ch, buf2, ch->carrying);
			if (!obj) {
				send_to_char("You do not have that item.\n\r", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			break;
		}
	}
	switch (lv_choice) {
		/* Commands that need <clan rank> as 2nd argument */
	case CLAN_ADVANCE:
	case CLAN_DEMOTE:{
			if (!*buf3) {
				send_to_char("Need a <clan rank> for this command!\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			if (!is_number(buf3)) {
				send_to_char("<clan rank> must be numeric.\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			lv_clan_rank = MAXV(0, atoi(buf3));
			if (lv_clan_rank > 11) {
				send_to_char("<clan rank> must be a value between 0 and 11!\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			break;
		}
	}
	switch (lv_choice) {
		/* Commands that need any first param */
	case CLAN_DONATE:
	case CLAN_TELL:{
			if (!*buf2) {
				send_to_char("Need a parameter for this command!\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			bzero(buf, sizeof(buf2));
			strcpy(buf2, old_arg);
			break;
		}
	}
	switch (lv_choice) {
		/* Commands that need victim to be in Clan */
	case CLAN_KICK:
	case CLAN_ADVANCE:
	case CLAN_DEMOTE:
	case CLAN_ALLOW:
	case CLAN_DENY:{
			if (!vict) {
				send_to_char("Error couldn't find a victim! please report this\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			if (CLAN_NUMBER(ch) != CLAN_NUMBER(vict)) {
				send_to_char("But they are not in your clan!\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			if (CLAN_RANK(ch) < CLAN_RANK(vict)) {
				send_to_char("Your rank is not high enough.\r\n", ch);
				return;
			}
			break;
		}
	}			/* end of argument check */

	if (cmd_levels[lv_choice - 1] > CLAN_RANK(ch)) {
		send_to_char("Your rank is not high enough to use this command.\r\n", ch);
		if (lv_offline_player)
			cl1100_clear_admin(ch);
		return;
	}


/* All arguments are ok, proceed with command */
/* no need to check arguments from here on    */
	switch (lv_choice) {
		/* Clan tell */
	case CLAN_TELL:{
			/* Tell - ch, buf2 (as message) */
			if ((CLAN_RANK(ch) == CLAN_APPLY_RANK) &&
			    (CLAN_NUMBER(ch) != 0)) {
				send_to_char("Sorry you are not in the clan yet.\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			save_position = GET_POS(ch);
			GET_POS(ch) = POSITION_STANDING;
			if (IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
				sprintf(buf, "You tell the clan '%s%s%s'", WHITE, buf2, CLR_CLAN_TELL);
			}
			else {
				sprintf(buf, "You tell the clan '%s'", buf2);
			}
			ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_CLAN_TELL, 0);
			GET_POS(ch) = save_position;
			for (d = descriptor_list; d; d = d->next) {
				if (d->character &&
				    (d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0)) {
					vict = d->character;
					if ((CLAN_NUMBER(ch) == CLAN_NUMBER(vict)) &&
					    (ch != vict) &&
					(CLAN_RANK(vict) > CLAN_APPLY_RANK)) {
						save_position = GET_POS(vict);
						GET_POS(vict) = POSITION_STANDING;
						if (IS_SET(GET_ACT3(vict), PLR3_ANSI)) {
							sprintf(buf, "%s tells the clan '%s%s%s'", GET_REAL_NAME(ch), WHITE, buf2, CLR_CLAN_TELL);
						}
						else {
							sprintf(buf, "%s tells the clan '%s'", GET_REAL_NAME(ch), buf2);
						}
						ansi_act(buf, FALSE, vict, 0, 0, TO_CHAR, CLR_CLAN_TELL, 0);
						GET_POS(vict) = save_position;
					}
				}
			}
			return;
		}
	case CLAN_ADVANCE:{
			if (CLAN_RANK(vict) > lv_clan_rank) {
				send_to_char("Please use 'clan demote' for demoting a clan member.\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			break;
		}
	case CLAN_DEMOTE:{
			if (CLAN_RANK(vict) < lv_clan_rank) {
				send_to_char("Please use 'clan advance' for advancing a clan member.\r\n", ch);
				if (lv_offline_player)
					cl1100_clear_admin(ch);
				return;
			}
			break;
		}
	}
	switch (lv_choice) {
	case CLAN_CREATE:{
			db10200_create_clan(ch);
			break;
		}
	case CLAN_SAVE:{
			db10100_save_clans();
			send_to_char("Clan data saved.\r\n", ch);
			break;
		}
	case CLAN_DEMOTE:
	case CLAN_ADVANCE:{
			/* advance - ch, vict, lv_clan_rank */

			if ((lv_choice == CLAN_ADVANCE) && (ch == vict)) {
				send_to_char("You can't advance yourself!\r\n", ch);
				return;
			}

			if (lv_clan_rank > CLAN_RANK(ch)) {
				send_to_char("You can not make anyone that level.\r\n", ch);
				return;
			}

			if (CLAN_RANK(vict) > CLAN_RANK(ch)) {
				send_to_char("You can not demote anyone higher than you.\r\n", ch);
				return;
			}
			send_to_char("Your rank in your clan has changed.\r\n", vict);
			co2900_send_to_char(ch, "Your new rank is: %d %s\r\n", lv_clan_rank,
			 GET_CLAN_RANK_NAME(CLAN_NUMBER(vict), lv_clan_rank));
			co2900_send_to_char(ch, "You changed %s's rank from %d %s to %d %s\r\n",
					    GET_NAME(vict),
					    CLAN_RANK(vict),
			GET_CLAN_RANK_NAME(CLAN_NUMBER(vict), CLAN_RANK(vict)),
					    lv_clan_rank,
			 GET_CLAN_RANK_NAME(CLAN_NUMBER(vict), lv_clan_rank));
			CLAN_RANK(vict) = lv_clan_rank;
			player_table[vict->nr].pidx_clan_rank = lv_clan_rank;
			if (CLAN_RANK(vict) == 0) {
				send_to_char("You have been put on the reapply list.\r\n", vict);
			}
		} break;
	case CLAN_APPLY:{
			/* apply - ch, lv_clan_nr */
			CLAN_NUMBER(ch) = lv_clan_nr;
			player_table[ch->nr].pidx_clan_number = lv_clan_nr;
			CLAN_RANK(ch) = CLAN_APPLY_RANK;
			player_table[ch->nr].pidx_clan_rank = CLAN_APPLY_RANK;
			co2900_send_to_char(ch, "Ok, you've applied for clan %s%s%s.\r\n", GET_CLAN_COLOR(ch, lv_clan_nr), GET_CLAN_NAME(lv_clan_nr), END_COLOR(ch));
			co2900_send_to_char(ch, "The clan leader, %s, will let you know if your application is successful.\r\n", GET_CLAN_LEADER(lv_clan_nr));
			for (d = descriptor_list; d; d = d->next) {
				if (d->character &&
				    (d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0) &&
				    (CLAN_NUMBER(ch) == CLAN_NUMBER(d->character)) &&
				    (CLAN_RANK(d->character) >= 10)) {
					send_to_char("Applicants list updated, please check.\r\n", d->character);
				}
			}
		} break;
	case CLAN_INFO:{
			/* info - ch, lv_clan_nr */
			if (cmd != CMD_ADMIN) {
				co2900_send_to_char(ch, "Clan Nr     : %d\r\n", lv_clan_nr);
				co2900_send_to_char(ch, "Clan Name   : %s%s%s\r\n", GET_CLAN_COLOR(ch, lv_clan_nr), GET_CLAN_NAME(lv_clan_nr), END_COLOR(ch));
				co2900_send_to_char(ch, "Clan Leader : %s\r\n", GET_CLAN_LEADER(lv_clan_nr));
				if (lv_clan_nr == CLAN_NUMBER(ch))
					sprintf(buf, "Your rank   : %s\r\n\r\n", GET_CLAN_RANK_NAME(lv_clan_nr, CLAN_RANK(ch)));
				else
					sprintf(buf, "\r\n");
				strcat(sbuf, buf);
				strcat(sbuf, "Clan Ranks :\r\n------------\r\n");
			}
			else
				strcpy(sbuf, "Clan Ranks :\r\n------------\r\n");
			for (idx = MAX_CLAN_RANKS; idx > 0; idx--) {
				if (idx == MAX_CLAN_RANKS)
					sprintf(buf, "Rank Leader : %s%s%s\r\n", GET_RANK_COLOR(ch, idx),
						GET_CLAN_RANK_NAME(lv_clan_nr, idx), END_COLOR(ch));
				else
					sprintf(buf, "Rank %2d : %s%s%s\r\n",
						idx,
						GET_RANK_COLOR(ch, idx),
					  GET_CLAN_RANK_NAME(lv_clan_nr, idx),
						END_COLOR(ch));
				strcat(sbuf, buf);
			}
			if (cmd != CMD_ADMIN) {
				strcat(sbuf, "\r\n\r\n\r\nClan Description:\r\n------------------\r\n");
				for (idx = 0; idx < MAX_CLAN_DESC; idx++) {
					sprintf(buf, "%s\r\n", GET_CLAN_DESC(lv_clan_nr, idx));
					if (buf[0] != '.')
						strcat(sbuf, buf);
				}
				if (ch->desc) {
					page_string(ch->desc, sbuf, 1);
				}
			}
			else
				send_to_char(sbuf, ch);
			bzero(sbuf, sizeof(sbuf));
		} break;
	case CLAN_LEAVE:{
			 /* leave - ch, lv_clan_nr */ ;
			sprintf(buf, "You have left clan %s%s%s.\r\n", GET_CLAN_COLOR(ch, lv_clan_nr), GET_CLAN_NAME(lv_clan_nr), END_COLOR(ch));
			send_to_char(buf, ch);
			CLAN_NUMBER(ch) = NO_CLAN;
			CLAN_RANK(ch) = 0;
			player_table[ch->nr].pidx_clan_number = NO_CLAN;
			player_table[ch->nr].pidx_clan_rank = 0;
		} break;
	case CLAN_WHERE:{
			/* where - ch */
			send_to_char("\r\nClan members:\r\n--------------\r\n", ch);
			for (d = descriptor_list; d; d = d->next) {
				if (d->character &&
				    (d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0)) {
					vict = d->character;
					if ((CLAN_NUMBER(ch) == CLAN_NUMBER(vict)) &&
					(GET_VISIBLE(vict) <= GET_LEVEL(ch)) &&
					    (GET_INCOGNITO(vict) <= GET_LEVEL(ch)) && //Incognito & ghost Shalira 22.07 .01
					 (GET_GHOST(vict) <= GET_LEVEL(ch))) {
						sprintf(buf, "%-20s - r[%d] %s",
							GET_REAL_NAME(vict),
						  world[vict->in_room].number,
						   world[vict->in_room].name);
						if ((d->last_target) && ((time(0) - d->last_hit_time) < 300)) {
							strcat(buf, " killing ");
							strcat(buf, GET_REAL_NAME(d->last_target));
						}
						strcat(buf, "\r\n");
						send_to_char(buf, ch);
					}
				}
			}
		} break;
	case CLAN_WHO:{
			/* who - ch */
			send_to_char("\r\nClan Members:\r\n-------------\r\n", ch);
			for (d = descriptor_list; d; d = d->next) {
				if (d->character && (d->connected == CON_PLAYING) && (d->character->in_room >= 0)) {
					vict = d->character;
					sprintf(buf, " %s ", GET_CLAN_RANK_NAME(CLAN_NUMBER(vict), CLAN_RANK(vict)));
					strcpy(buf2, "-----------");
					buf2[11 - (strlen(buf) / 2)] = 0;
					strcpy(buf3, "-----------");
					buf3[22 - strlen(buf2) - strlen(buf)] = 0;
					if (IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
						sprintf(buf4, "%s[%s%s%s%s%s]%s %-16s [%-2d]",
							WHITE, buf2, rank_color[CLAN_RANK(vict)],
							buf, WHITE, buf3,
							END, GET_REAL_NAME(vict), CLAN_RANK(vict));
					}
					else
						sprintf(buf4, "[%s%s%s] %-16s [%-2d]", buf2, buf, buf3, GET_REAL_NAME(vict), CLAN_RANK(vict));
					if ((CLAN_NUMBER(ch) == CLAN_NUMBER(vict)) &&
					(GET_VISIBLE(vict) <= GET_LEVEL(ch)) &&
					    (GET_INCOGNITO(vict) <= GET_LEVEL(ch))) {
						//incognito & ghost Shalira 22.07 .01
							ansi_act(buf4, FALSE, ch, 0, 0, TO_CHAR, WHITE, 0);
					}
				}
			}
			send_to_char("\r\nOffline Clan Members:\r\n---------------------\r\n", ch);
			for (idx = 0; idx < top_of_p_table; idx++) {

				/* Let's let them see how long it's been -- *
				 * relic March 20, 2003                     */

				int lv_seconds_gone = time(0) - player_table[idx].pidx_last_on;
				int lv_months = 0;
				int lv_days = 0;
				int lv_hours = 0;
				int lv_minutes = 0;

				if (lv_seconds_gone > 2592000)
					lv_months = (int) (lv_seconds_gone / 2592000);
				lv_seconds_gone = lv_seconds_gone - (lv_months * 2592000);


				if (lv_seconds_gone > 86400)
					lv_days = (int) (lv_seconds_gone / 86400);
				lv_seconds_gone = lv_seconds_gone - (lv_days * 86400);

				if (lv_seconds_gone > 3600)
					lv_hours = (int) (lv_seconds_gone / 3600);
				lv_seconds_gone = lv_seconds_gone - (lv_hours * 3600);


				if (lv_seconds_gone > 60)
					lv_minutes = (int) (lv_seconds_gone / 60);
				lv_seconds_gone = lv_seconds_gone - (lv_minutes * 60);

				if ((player_table + idx)->pidx_clan_number == CLAN_NUMBER(ch)) {
					sprintf(buf, " %s ",
						GET_CLAN_RANK_NAME((player_table + idx)->pidx_clan_number,
					(player_table + idx)->pidx_clan_rank));
					strcpy(buf2, "-----------");
					buf2[11 - (strlen(buf) / 2)] = 0;
					strcpy(buf3, "-----------");
					buf3[22 - strlen(buf2) - strlen(buf)] = 0;
					strcpy(buf5, (player_table + idx)->pidx_name);
					buf5[0] = UPPER(buf5[0]);
					if (IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
						sprintf(buf4, "%s[%s%s%s%s%s]%s %-16s [%-2d] Gone %d months, %d days, %d:%d:%d.",
							WHITE, buf2,
							rank_color[(player_table + idx)->pidx_clan_rank],
							buf, WHITE, buf3,
							END, buf5, (player_table + idx)->pidx_clan_rank,
						 lv_months, lv_days, lv_hours,
						 lv_minutes, lv_seconds_gone);
					}
					else
						sprintf(buf4, "[%s%s%s] %-16s [%-2d] Gone %d months, %d days, %d:%d:%d",
							buf2, buf, buf3, buf5,
							(player_table + idx)->pidx_clan_rank,
						 lv_months, lv_days, lv_hours,
						 lv_minutes, lv_seconds_gone);
					lv_not_found = 1;
					for (d = descriptor_list; d; d = d->next) {
						if (d->character &&
						    (d->connected == CON_PLAYING) &&
						(d->character->in_room >= 0)) {
							if (d->character->nr == idx) {
								lv_not_found = 0;
								break;
							}
						}
					}
					if
						(lv_not_found)
						ansi_act(buf4, FALSE, ch, 0, 0, TO_CHAR, WHITE, 0);
				}
			}
			return;
		}
	case CLAN_DONATE:{
			/* donate - ch, buf2 */
			do_donate(ch, buf2, CMD_CLAN_DONATE);

		} break;
	case CLAN_ALLOW:{
			/* accept - ch, vict */
			if (CLAN_RANK(vict) != CLAN_APPLY_RANK) {
				sprintf(buf, "%s is already in your clan!\r\n", GET_REAL_NAME(vict));
				send_to_char(buf, ch);
				break;
			}
			lv_clan_nr = CLAN_NUMBER(vict);
			sprintf(buf, "You have accepted %s to the clan.\r\n", GET_REAL_NAME(vict));
			send_to_char(buf, ch);
			send_to_char("New member set to first Rank.\r\n", ch);
			sprintf(buf, "You are accepted into clan %s%s%s.\r\n", GET_CLAN_COLOR(ch, lv_clan_nr), GET_CLAN_NAME(lv_clan_nr), END_COLOR(ch));
			send_to_char(buf, vict);
			CLAN_RANK(vict) = CLAN_FIRST_RANK;
			player_table[vict->nr].pidx_clan_rank = CLAN_FIRST_RANK;
		} break;
	case CLAN_DENY:{
			/* deny - ch, vict */
			if (CLAN_RANK(vict) != CLAN_APPLY_RANK) {
				sprintf(buf, "%s is already in your clan!\r\n", GET_REAL_NAME(vict));
				send_to_char(buf, ch);
				break;
			}
			lv_clan_nr = CLAN_NUMBER(vict);
			sprintf(buf, "You have denied %s's request to join the clan.\r\n", GET_REAL_NAME(vict));
			send_to_char(buf, ch);
			sprintf(buf, "Your request to join %s%s%s has been denied.\r\n", GET_CLAN_COLOR(ch, lv_clan_nr), GET_CLAN_NAME(lv_clan_nr), END_COLOR(ch));
			send_to_char(buf, vict);
			CLAN_NUMBER(vict) = NO_CLAN;
			player_table[vict->nr].pidx_clan_number = NO_CLAN;
		} break;
	case CLAN_LIST:{
			/* list - ch */
			send_to_char("Current clans:\r\n-------------\r\n", ch);
			for (idx = 0; idx < gv_last_clan; idx++) {
				if (CLAN_EXISTS(idx)) {
					sprintf(buf, "%3d %s%s%s\r\n", idx, GET_CLAN_COLOR(ch, idx), GET_CLAN_NAME(idx), END_COLOR(ch));
					send_to_char(buf, ch);

				}
			}
		} break;
	case CLAN_KICK:{
			/* kick - ch, vict */
			sprintf(buf, "%s has kicked you out of the clan!\r\n", GET_REAL_NAME(ch));
			send_to_char(buf, vict);
			sprintf(buf, "You have kicked %s out of the clan!\r\n", GET_REAL_NAME(vict));
			send_to_char(buf, ch);
			CLAN_RANK(vict) = 0;
			player_table[vict->nr].pidx_clan_rank = 0;
			CLAN_NUMBER(vict) = NO_CLAN;
			player_table[vict->nr].pidx_clan_number = NO_CLAN;
		} break;
	case CLAN_ENABLE:{
			/* enable - ch, lv_clan_nr */
			if (GET_CLAN_ENABLED(lv_clan_nr)) {
				sprintf(buf, "Clan %d - %s disabled.\r\n", lv_clan_nr, GET_CLAN_NAME(lv_clan_nr));
				send_to_char(buf, ch);
				GET_CLAN_ENABLED(lv_clan_nr) = 0;
				break;
			}
			else {
				sprintf(buf, "Clan %d - %s enabled.\r\n", lv_clan_nr, GET_CLAN_NAME(lv_clan_nr));
				send_to_char(buf, ch);
				GET_CLAN_ENABLED(lv_clan_nr) = 1;
				break;
			}
		}
	case CLAN_LEADER:{
			/* leader - ch, vict, lv_clan_nr */
			sprintf(buf, "You have made %s leader of clan %d - %s\r\n",
				GET_REAL_NAME(vict),
				lv_clan_nr,
				GET_CLAN_NAME(lv_clan_nr));
			send_to_char(buf, ch);
			CLAN_RANK(vict) = 11;
			player_table[vict->nr].pidx_clan_rank = 11;
			CLAN_NUMBER(vict) = lv_clan_nr;
			player_table[vict->nr].pidx_clan_number = lv_clan_nr;
		} break;
	case CLAN_APPLICANTS:{
			send_to_char("Applicants:\r\n-----------\r\n", ch);
			for (idx = 0; idx <= top_of_p_table; idx++) {
				if (((player_table + idx)->pidx_clan_number == CLAN_NUMBER(ch)) &&
				    ((player_table + idx)->pidx_clan_rank == CLAN_APPLY_RANK)) {
					sprintf(buf, "%s\r\n", (player_table + idx)->pidx_name);
					if (buf[0] > 96)
						buf[0] -= 32;
					send_to_char(buf, ch);
				}
			}
		} break;
	case CLAN_WAR:
		cl1800_do_war(ch, buf6, cmd);
		break;

	default:{
			send_to_char("Someone made a mistake while coding :P\r\n", ch);
		} break;
	}

	if (lv_offline_player) {
		ad2100_save_player(ch);
		cl1100_clear_admin(ch);
	}

}				/* end of cl1000_do_clan() */


void cl1100_clear_admin(struct char_data * ch)
{

	if (ch->desc->admin) {
		r3400_del_objs(ch->desc->admin);
		db7100_free_char(ch->desc->admin, 0);
		ch->desc->admin = 0;
	}
}				/* end of cl1100_clear_admin() */


void cl1200_start_war(struct char_data * ch, int enemy)
{
	struct clan_war_data *war;
	int clan;
	char buf[MAX_STRING_LENGTH];

	clan = CLAN_NUMBER(ch);

	if (enemy == clan) {
		send_to_char("You can't start a war against yourself!\r\n", ch);
		return;
	}

	war = cl1400_get_war(clan, enemy);
	if (war) {
		send_to_char("You are already at war with them.\r\n", ch);
		return;
	}

	CREATE(war, struct clan_war_data, 1);
	bzero(war, sizeof(war));

	war->enemy_clan = enemy;
	war->start_time = time(0);
	war->next = clans[clan].war;
	clans[clan].war = war;

	war = cl1400_get_war(enemy, clan);
	if (!war) {
		sprintf(buf, "You declare war with %s, they have not responded yet.\r\n", GET_CLAN_NAME(enemy));
		send_to_char(buf, ch);
		return;
	}

	war->start_time = time(0);
	sprintf(buf, "You declare war with %s, let the fighting begin!\r\n", GET_CLAN_NAME(enemy));
	send_to_char(buf, ch);
}				/* end of cl1200_start_war */


void cl1300_stop_war(struct char_data * ch, int enemy)
{
	struct clan_war_data *war;
	struct clan_war_data *nme;
	int clan;
	int lv_time;
	char buf[MAX_STRING_LENGTH];

	clan = CLAN_NUMBER(ch);

	if (enemy == clan) {
		send_to_char("You stop fighting with yourself, even though you enjoyed it so much.\r\n", ch);
		return;
	}

	war = cl1400_get_war(clan, enemy);
	nme = cl1400_get_war(enemy, clan);

	if (!war && !nme) {
		send_to_char("You are not at war with them.\r\n", ch);
		return;
	}

	if (war) {
		lv_time = (int) ((time(0) - (!war->attack_time ? war->start_time : war->attack_time)) / 3600);
		if ((lv_time < WAR_MIN_HOURS) && (GET_LEVEL(ch) < IMO_IMP)) {
			sprintf(buf, "You can't stop this war yet (%d hours left).\r\n", WAR_MIN_HOURS - lv_time);
			send_to_char(buf, ch);
			return;
		}
	}

	if (war)
		cl1350_remove_war(clan, war);
	if (nme)
		cl1350_remove_war(enemy, nme);

	if (!war && nme)
		sprintf(buf, "You decline the war with %s.\r\n", GET_CLAN_NAME(enemy));
	else
		sprintf(buf, "You end the war with %s.\r\n", GET_CLAN_NAME(enemy));
	send_to_char(buf, ch);

}				/* end of cl1300_stop_war */


void cl1350_remove_war(int clan, struct clan_war_data * war)
{
	struct clan_war_data *warloop;

	if (clans[clan].war == war) {
		clans[clan].war = war->next;
		free(war);
		return;
	}

	for (warloop = clans[clan].war; warloop; warloop = warloop->next) {
		if (warloop->next == war) {
			warloop->next = war->next;
			free(war);
			break;
		}
	}
}				/* end of cl1350_get_war() */


struct clan_war_data *cl1400_get_war(int clan, int enemy)
{
	struct clan_war_data *war;

	if ((clan >= gv_last_clan) || (enemy >= gv_last_clan))
		return (0);

	for (war = clans[clan].war; war; war = war->next)
		if (war->enemy_clan == enemy)
			break;

	return (war);
}				/* end of cl1400_get_war() */


void cl1500_attack_player(struct char_data * ch, struct char_data * vict)
{
	struct clan_war_data *war;

	war = cl1400_get_war(CLAN_NUMBER(ch), CLAN_NUMBER(vict));

	if (!war)
		return;

	war->attack_time = time(0);
	war->attacks += 1;

}				/* end of cl1500_attack_player */


void cl1600_kill_player(struct char_data * ch, struct char_data * vict)
{
	struct clan_war_data *war;
	struct clan_war_data *nme;

	war = cl1400_get_war(CLAN_NUMBER(ch), CLAN_NUMBER(vict));
	nme = cl1400_get_war(CLAN_NUMBER(vict), CLAN_NUMBER(ch));

	if (!war || !nme)
		return;

	war->kills += 1;
	war->kill_time = time(0);

	nme->deaths += 1;

}				/* end of cl1600_kill_player */


void cl1700_show_war_stats(struct char_data * ch, int clan)
{
	struct clan_war_data *war;
	char buf[MAX_STRING_LENGTH];

	war = clans[clan].war;

	if (!war) {
		send_to_char("This clan is not involved in any wars.\r\n", ch);
		return;
	}

	sprintf(buf, "War data for clan %s.\r\n\r\n", GET_CLAN_NAME(clan));
	send_to_char(buf, ch);
	send_to_char("Enemy                  Kills  Deaths Attacks Escapes\r\n", ch);
	send_to_char("----------------------------------------------------\r\n", ch);

	for (; war; war = war->next) {
		sprintf(buf, "%s%-20s &G%7ld &R%7ld &g%7ld &r%7ld&n\r\n",
			GET_CLAN_COLOR(ch, war->enemy_clan),
			GET_CLAN_NAME(war->enemy_clan),
			war->kills,
			war->deaths,
			war->attacks,
			war->escapes);
		send_to_char(buf, ch);
	}


}				/* end of cl1700_show_war_stats */

void cl1800_do_war(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int lv_choice, lv_clan_nr;

	const char *lv_war_cmds[] = {
		"start",	/* 1 */
		"stop",		/* 2 */
		"accept",	/* 3 */
		"decline",	/* 4 */
		"info",		/* 5 */
		"\n",
	};
	int cmd_levels[] = {
		10,		/* 1 start	 */
		10,		/* 2 stop 	 */
		10,		/* 3 accept	 */
		10,		/* 4 decline	 */
		0,		/* 5 info	 */
	};

#define WAR_START	1
#define WAR_STOP	2
#define WAR_ACCEPT	3
#define WAR_DECLINE	4
#define WAR_INFO	5

	if (GET_LEVEL(ch) < 10) {
		send_to_char("Your level isn't high enough to use this command.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	lv_choice = 0;
	if (*buf)
		lv_choice = old_search_block(buf, 0, strlen(buf), lv_war_cmds, 0);

	if (lv_choice < 1) {
		send_to_char("None or invalid argument passed to set.\r\n", ch);
		send_to_char("Valid options are:\r\n\r\n", ch);
		send_to_char(" start   <clan number>\r\n", ch);
		send_to_char(" stop    <clan number>\r\n", ch);
		send_to_char(" accept  <clan number>\r\n", ch);
		send_to_char(" decline <clan number>\r\n", ch);
		send_to_char(" info    <clan number>\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);

	switch (lv_choice) {
		/* Commands that need clan number as first/second argument */
	case WAR_START:
	case WAR_STOP:
	case WAR_ACCEPT:
	case WAR_DECLINE:
	case WAR_INFO:{
			if (!*arg) {
				if (lv_choice != WAR_INFO) {
					send_to_char("Need a <clan number> for this command!\r\n", ch);
					return;
				}
				else {
					lv_clan_nr = CLAN_NUMBER(ch);
					break;
				}
			}
			if (!is_number(arg)) {
				send_to_char("<clan number> must be numeric.\r\n", ch);
				return;
			}
			lv_clan_nr = MAXV(0, atoi(arg));
			if ((!CLAN_EXISTS(lv_clan_nr))) {
				sprintf(buf, "Clan number %d does not exist or has been disabled!\r\n", lv_clan_nr);
				send_to_char(buf, ch);
				return;
			}
			break;
		}
	}

	if (cmd_levels[lv_choice - 1] > CLAN_RANK(ch)) {
		send_to_char("Your rank is not high enough to use this command.\r\n", ch);
		return;
	}


	switch (lv_choice) {
	case WAR_START:
	case WAR_ACCEPT:
		cl1200_start_war(ch, lv_clan_nr);
		break;
	case WAR_STOP:
	case WAR_DECLINE:
		cl1300_stop_war(ch, lv_clan_nr);
		break;
	case WAR_INFO:
		cl1700_show_war_stats(ch, lv_clan_nr);
		break;
	}
}				/* void cl1800_do_war(struct char_data *ch,
				 * char *arg, int cmd) */

int cl1900_at_war(struct char_data * ch, struct char_data * vict)
{
	int ch_clan, vict_clan;

	if (!ch || !vict)
		return (FALSE);

	if (IS_NPC(ch) || IS_NPC(vict))
		return (FALSE);

	ch_clan = CLAN_NUMBER(ch);
	vict_clan = CLAN_NUMBER(vict);

	if (ch_clan == vict_clan)
		return (FALSE);

	if (!ch_clan || !vict_clan)
		return (FALSE);

	if (CLAN_RANK(ch) == CLAN_APPLY_RANK)
		return (FALSE);

	if (!cl1400_get_war(ch_clan, vict_clan))
		return (FALSE);
	if (!cl1400_get_war(vict_clan, ch_clan))
		return (FALSE);

	return (TRUE);
}				/* void cl1900_at_war (struct char_data *ch,
				 * struct char_data *vict) */

void cl2000_player_escape(struct char_data * ch, struct char_data * vict)
{
	struct clan_war_data *war;

	if (!ch || !vict)
		return;

	war = cl1400_get_war(CLAN_NUMBER(ch), CLAN_NUMBER(vict));

	if (!war)
		return;

	war->escapes += 1;
}				/* void cl2000_player_eascape () */

void cl2100_do_set_clan(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char *vict;
	int lv_choice, lv_clan, location, lv_char_nr, lv_argv;
	const char *lv_set_cmds[] = {
		"entrance",	/* 1 */
		"donation",	/* 2 */
		"board",	/* 3 */
		"rank",		/* 4 */
		"description",	/* 5 */
		"leader",	/* 6 */
		"name",		/* 7 */
		"color",	/* 8 */
		"\n",
	};

#define SET_ENTRANCE	1
#define SET_DONATION	2
#define SET_BOARD	3
#define SET_RANK	4
#define SET_DESC	5
#define SET_LEADER	6
#define SET_NAME	7
#define SET_COLOR	8

	if (IS_NPC(ch))
		return;

	/* Strip out first argument */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	for (; isspace(*arg); arg++);

	lv_clan = CLAN_NUMBER(ch);

	if ((CLAN_RANK(ch) < MAX_CLAN_RANKS) || (!lv_clan)) {
		send_to_char("You are not allowed to use this command.\r\n", ch);
		return;
	}

	lv_choice = 0;
	if (*buf)
		lv_choice = old_search_block(buf, 0, strlen(buf), lv_set_cmds, 0);

	/* Check level restrictions and commands */
		if ((lv_choice < 1) || ((GET_LEVEL(ch) < IMO_IMM) && (lv_choice != SET_RANK) && (lv_choice != SET_DESC))) {
		send_to_char("None or invalid argument passed to set.\r\n", ch);
		send_to_char("Valid options are:\r\n\r\n", ch);
		send_to_char(" rank        <rank> <description>\r\n", ch);
		send_to_char(" description <line> <text>\r\n", ch);
		send_to_char(" color       <color>\r\n", ch);
		if (GET_LEVEL(ch) >= IMO_IMM) {
			send_to_char(" name     <name>\r\n", ch);
			send_to_char(" leader   <name>\r\n", ch);
			send_to_char(" entrance <room vnum>\r\n", ch);
			send_to_char(" donation <room vnum>\r\n", ch);
			send_to_char(" board    <obj vnum>&n\r\n", ch);
		}
		return;
	}

	/* Check if first parameter is numeric if needed */
	if ((lv_choice != SET_NAME) && (lv_choice != SET_LEADER) && (lv_choice != SET_COLOR)) {

		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
		for (; isspace(*arg); arg++);

		if (!*buf) {
			send_to_char("Please enter a value.\r\n", ch);
			return;
		}
		if (!is_number(buf)) {
			send_to_char("Please enter a numeric value.\r\n", ch);
			return;
		}
		lv_argv = MAXV(0, atoi(buf));

		for (; isspace(*arg); arg++);
	}

	/* Check if there's a second parameter when command needs it */
	if (((lv_choice == SET_RANK) ||
	     (lv_choice == SET_DESC) ||
	     (lv_choice == SET_NAME) ||
	     (lv_choice == SET_LEADER) ||
	     (lv_choice == SET_DESC)) && (!*arg)) {
		send_to_char("Please enter a value.\r\n", ch);
		return;
	}

	/* Set a clan rank */
	if (lv_choice == SET_RANK) {
		/* Check if entered value is between limits */
		if ((lv_argv < 1) || (lv_argv > MAX_CLAN_RANKS)) {
			sprintf(buf, "Invalid rank, please enter a value between 1 and %d.\r\n", MAX_CLAN_RANKS - 1);
			send_to_char(buf, ch);
			return;
		}

		REMOVE_COLOR(arg);

		/* Check length of entered string */
		if (strlen(arg) > 20) {
			send_to_char("Rank name to long, maximum length is 20 characters.\r\n", ch);
			return;
		};

		co2900_send_to_char(ch, "Old rank description: %s\r\n", clans[lv_clan].rank[lv_argv]);
		strcpy(clans[lv_clan].rank[lv_argv], arg);
		co2900_send_to_char(ch, "New rank description: %s\r\n", clans[lv_clan].rank[lv_argv]);
		return;
	}

	/* Set a clan description */
	if (lv_choice == SET_DESC) {
		/* Check if entered value is between limits */
		if ((lv_argv < 0) || (lv_argv >= MAX_CLAN_DESC)) {
			sprintf(buf, "Invalid description number, please enter a value between 0 and %d.\r\n", MAX_CLAN_DESC - 1);
			send_to_char(buf, ch);
			return;
		}

		/* Check length of entered string */
		if (strlen(arg) > 79) {
			send_to_char("Description to long, maximum length is 80 characters.\r\n", ch);
			return;
		};

		sprintf(buf, "Old description: %s\r\n", clans[lv_clan].desc[lv_argv]);
		send_to_char(buf, ch);
		strcpy(clans[lv_clan].desc[lv_argv], arg);
		sprintf(buf, "New description: %s\r\n", clans[lv_clan].desc[lv_argv]);
		send_to_char(buf, ch);
		return;
	}

	/* Set the clans name */
	if (lv_choice == SET_NAME) {
		REMOVE_COLOR(arg);
		if (strlen(arg) >= sizeof(clans[lv_clan].name)) {
			send_to_char("The name you entered is to long.\r\n", ch);
			return;
		}

		strcpy(clans[lv_clan].name, arg);
		sprintf(buf, "Clan name changed to %s\r\n", arg);
		send_to_char(buf, ch);
		return;
	}

	/* Set the clan color */
	if (lv_choice == SET_COLOR) {
		if ((strlen(arg) != 2) || (*arg != '&')) {
			send_to_char("Please use color codes as mentioned in 'help colors'\r\n", ch);
			return;
		}

		strcpy(clans[lv_clan].color, arg);
		sprintf(buf, "Clan color changed to &%s\r\n", arg);
		send_to_char(buf, ch);
		return;
	}

	/* Check if room exists for donation and entrance */
	if ((lv_choice == SET_DONATION) || (lv_choice == SET_ENTRANCE)) {
		for (location = 0; location <= top_of_world; location++) {
			if (world[location].number == lv_argv) {
				break;
			}
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\r\n", ch);
				return;
			}
		}
	}

	/* Set the donation room */
	if (lv_choice == SET_DONATION) {
		if (!lv_argv) {
			sprintf(buf, "Donation room reset to Midgaard's donation room.\r\n");
			lv_argv = gv_donate_room;
		}
		else
			sprintf(buf, "Clans donation room set to R#-%d.\r\n", lv_argv);
		clans[lv_clan].don_room = lv_argv;
		send_to_char(buf, ch);
		return;
	}

	/* Set the clans teleportation room */
	if (lv_choice == SET_ENTRANCE) {
		clans[lv_clan].room = lv_argv;
		sprintf(buf, "Clans teleportation room set to R#-%d.\r\n", lv_argv);
		send_to_char(buf, ch);
		return;
	}

	/* Set the clans leader */
	if (lv_choice == SET_LEADER) {
		lv_char_nr = pa2050_find_name_approx(arg);
		if (lv_char_nr < 0) {
			send_to_char("Unknown player name.\r\n", ch);
			return;
		}
		vict = player_table[lv_char_nr].pidx_name;

		if (player_table[lv_char_nr].pidx_clan_number) {
			sprintf(buf, "%s is already member of another clan.\r\n", vict);
			send_to_char(buf, ch);
			return;
		}

		strcpy(clans[lv_clan].leader, vict);
		*(clans[lv_clan].leader) -= 32;
		sprintf(buf, "Clan leader set to %s.\r\n", vict);
		send_to_char(buf, ch);
		sprintf(buf, " leader %s %d", vict, lv_clan);
		cl1000_do_clan(ch, buf, CMD_CLAN);
		return;
	}

	/* Set the clans board */
	if (lv_choice == SET_BOARD) {
		clans[lv_clan].board_num = lv_argv;
		send_to_char("Warning! the number you entered is *NOT* checked, please make sure it is correct.\r\n", ch);
		sprintf(buf, "Clans board v-num set to %d.\r\n", lv_argv);
		send_to_char(buf, ch);
		return;
	}

	send_to_char("If you read this, something went wrong...", ch);
}
