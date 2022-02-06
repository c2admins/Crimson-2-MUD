/* ad */
/* *******************************************************************
*  file: admin.c , Special module.           Part of Crimson MUD     *
*  Usage: Admin system for Crimson MUD II                            *
*                  Written by Hercules                               *
******************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#if !defined (DIKU_WINDOWS)
#include <time.h>
#include <unistd.h>
#endif

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
#include "admin.h"
#include "repversion.h"


const char *wtypes[] = {
	"-",			/* 0 */
	"-",			/* 1 */
	"whip",			/* 2 */
	"slash",		/* 3 */
	"-",			/* 4 */
	"-",			/* 5 */
	"crush",		/* 6 */
	"pound",		/* 7 */
	"-",			/* 8 */
	"-",			/* 9 */
	"-",			/* 10 */
	"pierce",		/* 11 */
	"\n",
};

const char *lock_bits[] = {
	"CLOSABLE",		/* BIT0 */
	"PICKPROOF",		/* BIT1 */
	"CLOSED",		/* BIT2 */
	"LOCKABLE",		/* BIT3 */
	"\n"
};

extern struct txt_block *bufpool;
/* ***********************************************************
*  ADMIN - Startup	                                     *
*********************************************************** */


void ad1000_do_admin(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *k;
	struct follow_type *f;
	struct descriptor_data *d;
	char buf[MAX_STRING_LENGTH];
	int lv_level[MAX_LEV + 5];
	int idx;

	if (!ch->desc)
		return;

	if (IS_NPC(ch)) {
		send_to_char("Mobs aren't allowed to take over the mud ;)\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		act("$n disappears as $e goes into admin mode.", TRUE, ch, 0, 0, TO_ROOM);
		GET_PRIVATE(ch) = ch->in_room;
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, 0);
		STATE(ch->desc) = CON_ADMIN;
		ADMIN_STATE(ch->desc) = ADMIN_MAIN;
		ad1500_show_menu(ch->desc);
		OLD_STATE(ch->desc) = 0;
	}
	else {
		if (is_abbrev(buf, "show")) {
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);

			//**************SHOW WHAT PLAYERS ARE KILLING *********************

				if (is_abbrev(buf, "killing")) {
				send_to_char("\r\nPlayer            Target                     Time lasted since last hit.\r\n", ch);
				send_to_char("-------------------------------------------------------------------------------\r\n", ch);
				for (d = descriptor_list; d; d = d->next)
					if (d->character) {
						k = d->character;
						if ((d->connected == CON_PLAYING) &&
						    (k->in_room >= 0) &&
						    (d->last_target) &&
						    ((time(0) - d->last_hit_time) < 300)) {
							sprintf(buf, "%-15s - %-25s %2d %s %2d %s %2d %s\r\n",
							     GET_REAL_NAME(k),
								GET_REAL_NAME(d->last_target),
								get_hit_time(d->last_hit_time, 2),
								get_hit_time(d->last_hit_time, 2) == 1 ? "Hour" : "Hours",
								get_hit_time(d->last_hit_time, 3),
								get_hit_time(d->last_hit_time, 3) == 1 ? "Minute" : "Minutes",
								get_hit_time(d->last_hit_time, 4),
								get_hit_time(d->last_hit_time, 4) == 1 ? "Second" : "Seconds");
							send_to_char(buf, ch);
						}
					}
				send_to_char("\r\n", ch);
				return;
			}

			//***************SHOW PLAYERS ON QUESTS ******************

				if (is_abbrev(buf, "quests")) {
				send_to_char("\r\nPlayer            Quest Target                    Time Left\r\n", ch);
				send_to_char("-------------------------------------------------------------------------------\r\n", ch);
				for (d = descriptor_list; d; d = d->next)
					if (d->character) {
						k = d->character;
						if ((d->connected == CON_PLAYING) &&
						    (k->in_room >= 0)) {
							bzero(buf, sizeof(buf));
							sprintf(buf, "\r\n");
							if (k->countdown) {
								sprintf(buf, "%-15s - %-30s %2d %s %2d %s %2d %s\r\n",
									GET_REAL_NAME(k),
									k->questmob ? GET_REAL_NAME(k->questmob) : "Quest completed",
									get_jail_time(k->countdown, 2),
									get_jail_time(k->countdown, 2) == 1 ? "Hour" : "Hours",
									get_jail_time(k->countdown, 3),
									get_jail_time(k->countdown, 3) == 1 ? "Minute" : "Minutes",
									get_jail_time(k->countdown, 4),
									get_jail_time(k->countdown, 4) == 1 ? "Second" : "Seconds");
							}
							else {
								if (k->nextquest)
									sprintf(buf, "%-15s - %-30s %2d %s %2d %s %2d %s\r\n",
										GET_REAL_NAME(k),
										"Waiting for next.",
										get_jail_time(k->nextquest, 2),
										get_jail_time(k->nextquest, 2) == 1 ? "Hour" : "Hours",
										get_jail_time(k->nextquest, 3),
										get_jail_time(k->nextquest, 3) == 1 ? "Minute" : "Minutes",
										get_jail_time(k->nextquest, 4),
										get_jail_time(k->nextquest, 4) == 1 ? "Second" : "Seconds");
							}
							send_to_char(buf, ch);
						}
					}
				send_to_char("\r\n", ch);
				return;
			}
			//******************SHOW AMOUNT OF PLAYERS PER LEVEL ************************

				if (is_abbrev(buf, "Levels")) {
				for (idx = 0; idx <= (MAX_LEV + 4); idx++)
					lv_level[idx] = 0;
				for (idx = 0; idx <= top_of_p_table; idx++)
					lv_level[player_table[idx].pidx_level] += 1;
				send_to_char("Lev  Nr.   Lev  Nr.   Lev  Nr.   Lev  Nr.\r\n", ch);
				send_to_char("-----------------------------------------\r\n", ch);
				for (idx = 1; idx <= (MAX_LEV >> 2); idx++) {
					sprintf(buf, "%2d -%4d | %2d -%4d | %2d -%4d | %2d -%4d\r\n",
						idx,
						lv_level[idx],
						idx + (MAX_LEV >> 2),
						lv_level[idx + (MAX_LEV >> 2)],
						idx + 2 * (MAX_LEV >> 2),
					   lv_level[idx + 2 * (MAX_LEV >> 2)],
						idx + 3 * (MAX_LEV >> 2),
					  lv_level[idx + 3 * (MAX_LEV >> 2)]);
					send_to_char(buf, ch);
				}
				return;
			}

			//******************SHOW PLAYER GROUPS *****************

				if (is_abbrev(buf, "Followers")) {
				send_to_char("\r\nLeader                 Followers                     Grouped\r\n", ch);
				send_to_char("-------------------------------------------------------------\r\n", ch);
				for (d = descriptor_list; d; d = d->next)
					if (d->character) {
						k = d->character;
						if ((d->connected == CON_PLAYING) &&
						    (k->in_room >= 0) &&
						    (k->followers)) {
							sprintf(buf, "%-20s\r\n", GET_REAL_NAME(k));
							send_to_char(buf, ch);
							for (f = k->followers; f; f = f->next) {
								sprintf(buf, "%-20s   %-30s %3s\r\n", " -", GET_REAL_NAME(f->follower),
									IS_AFFECTED(f->follower, AFF_GROUP) ? "(*)" : " ");
								send_to_char(buf, ch);
							}
						}
					}
				send_to_char("\r\n", ch);
				return;
			}

			if (is_abbrev(buf, "Idle")) {
				send_to_char("\r\nPlayer                 Idle ticks\r\n", ch);
				send_to_char("----------------------------------\r\n", ch);
				for (d = descriptor_list; d; d = d->next)
					if (d->character) {
						k = d->character;
						if ((d->connected == CON_PLAYING) && (k->in_room >= 0)) {
							sprintf(buf, "%-20s   %d\r\n", GET_REAL_NAME(k), k->specials.timer);
							if (GET_LEVEL(ch) >= GET_VISIBLE(k))
								send_to_char(buf, ch);
						}
					}
				return;
			}

			if (is_abbrev(buf, "stats")) {
				struct txt_block *c_pointer;
				idx = 0;
				for (c_pointer = bufpool; c_pointer; c_pointer = c_pointer->next)
					idx++;
				co2900_send_to_char(ch, "Buffer switches: %d\r\n", buf_switches);
				co2900_send_to_char(ch, "Large buffers in the pool: %d (%d)\r\n", buf_largecount, idx);
				co2900_send_to_char(ch, "Buffer overflows: %d\r\n", buf_overflows);
				/* DO NOT MODIFIY THE LINE BELOW.  OR YOU WILL SUFFER MY WRATH! */
				co2900_send_to_char(ch, "Version: %s\r\n", C2_VERSION);
				return;
			}

			if (is_abbrev(buf, "debug")) {
				co2900_send_to_char(ch, "Current lunar phase: %d\r\n", weather_info.lunar_phase);
				co2900_send_to_char(ch, "Current lunar phase hour: %d\r\n", weather_info.current_hour_in_phase);
				return;
			}

			send_to_char("Valid options for \"admin show\" are: killing / Quests / followers / levels / Idle / stats\r\n", ch);
			return;
		}
		send_to_char("Valid options for \"admin\" are: show\r\n", ch);
	}


}				/* END OF ad1000_do_admin() */


void ad1010_do_oedit(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *obj;
	int number, zone, r_num;

	arg = one_argument(arg, buf);

	if (!is_number(buf)) {
		send_to_char("Try ostat <obj>\r\n", ch);
		return;
	}

	number = atoi(buf);

	if ((r_num = db8200_real_object(number)) < 0) {
		send_to_char("There is no object with that number.\r\n", ch);
		return;
	}

	zone = obj_index[r_num].zone;
	CAN_PERSON_UPDATE(IMO_IMP);

	obj = db5100_read_object(r_num, REAL);
	ha2100_obj_to_room(obj, ch->in_room);

	ch->desc->admin_obj = obj;
	ch->desc->admin_zone = zone;

	ad1420_display_object(ch);

	ha2700_extract_obj(ch->desc->admin_obj);

	ch->desc->admin_obj = 0;
	ch->desc->admin_zone = 0;

}				/* END OF ad1010_do_oedit() */


/* ***********************************************************
*  ADMIN - Basic menu nanny                                  *
*********************************************************** */


void ad1100_admin_nanny(struct descriptor_data * d, char *arg)
{

	int lv_number, lv_lines;
	struct char_data *ch, *victim;
	char buf[MAX_STRING_LENGTH];

	ch = d->character;
	victim = d->admin;


	if (arg)
		for (; isspace(*arg); arg++);

	/* GENERAL CHECK FOR ALL MENUS Make sure the menu is between
	 * ADMIN_EDIT_START and ADMIN_EDIT_END */
	if ((ADMIN_STATE(d) > ADMIN_EDIT_START) && (ADMIN_STATE(d) < ADMIN_EDIT_END)) {
		if (!*arg) {
			send_to_char("Enter choice: ", ch);
			return;
		}
		if (!is_number(arg)) {
			ad1500_show_menu(d);
			return;
		}
		lv_number = atoi(arg);
		send_to_char("\r\n", ch);
	}

	switch (ADMIN_STATE(d)) {
	case ADMIN_PRESS_ENTER:
		ADMIN_STATE(d) = OLD_STATE(d);
		OLD_STATE(d) = 0;
		ad1500_show_menu(d);
		break;

		/* ***********************************************************
		 * ADMIN - Config menu options                               * ********************************************************** */

	case ADMIN_CONFIG:
		switch (lv_number) {
		case 0:
			send_to_char("Returning to game.\r\n", ch);
			ad2100_end_config(ch);
			break;
		case 1:
			ADMIN_STATE(d) = ADMIN_CONFIG_PROMPT;
			OLD_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			break;
		case 2:
			ADMIN_STATE(d) = ADMIN_CONFIG_DISPLAY;
			OLD_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			break;
		case 3:
			ADMIN_STATE(d) = ADMIN_CONFIG_CHANNEL;
			OLD_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			break;
		case 4:
			ADMIN_STATE(d) = ADMIN_CONFIG_FLAG;
			OLD_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			break;
		case 5:
			ADMIN_STATE(d) = ADMIN_CONFIG_GENERAL;
			OLD_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


/* ***********************************************************
*  ADMIN - Config prompt options			     *
*********************************************************** */


	case ADMIN_CONFIG_PROMPT:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_CONFIG;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2100_end_config(ch);
			}
			break;
		case 1:
			TOGGLE_BIT2(PLR2_SHOW_NAME);
			ad1500_show_menu(d);
			break;
		case 2:
			TOGGLE_BIT3(PLR3_SHOW_ROOM);
			ad1500_show_menu(d);
			break;
		case 3:
			TOGGLE_BIT3(PLR3_SHOW_HP);
			ad1500_show_menu(d);
			break;
		case 4:
			TOGGLE_BIT3(PLR3_SHOW_MANA);
			ad1500_show_menu(d);
			break;
		case 5:
			TOGGLE_BIT3(PLR3_SHOW_MOVE);
			ad1500_show_menu(d);
			break;
		case 6:
			TOGGLE_BIT3(PLR3_SHOW_EXITS);
			ad1500_show_menu(d);
			break;
		case 7:
			TOGGLE_BIT1(PLR1_SHOW_DIAG);
			ad1500_show_menu(d);
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;

/* ***********************************************************
*  ADMIN - Config display options			     *
*********************************************************** */


	case ADMIN_CONFIG_DISPLAY:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_CONFIG;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2100_end_config(ch);
			}
			break;
		case 1:
			TOGGLE_BIT3(PLR3_ANSI);
			ad1500_show_menu(d);
			break;
		case 2:
			TOGGLE_BIT2(PLR2_NEWGRAPHICS);
			ad1500_show_menu(d);
			break;
		case 3:
	//todo:	add question for lines
				for (lv_lines = 100; lv_lines >= 0; lv_lines--) {
					sprintf(buf, "- %d\r\n", lv_lines);
					send_to_char(buf, ch);
				}
			send_to_char("\r\nNow enter the top number on your screen: ", ch);
			ADMIN_STATE(d) = ADMIN_CONFIG_SET_LINES;
			break;
		case 4:
			TOGGLE_BIT3(PLR3_BRIEF);
			ad1500_show_menu(d);
			break;
		case 5:
			TOGGLE_BIT3(PLR3_COMPACT);
			ad1500_show_menu(d);
			break;

		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;



/* ***********************************************************
*  ADMIN - Config Channel options			     *
*********************************************************** */


	case ADMIN_CONFIG_CHANNEL:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_CONFIG;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2100_end_config(ch);
			}
			break;
		case 1:
			TOGGLE_BIT3(PLR3_NOTELL);
			ad1500_show_menu(d);
			break;
		case 2:
			TOGGLE_BIT3(PLR3_NOGOSSIP);
			ad1500_show_menu(d);
			break;
		case 3:
			TOGGLE_BIT3(PLR3_NOAVATAR);
			ad1500_show_menu(d);
			break;
		case 4:
			TOGGLE_BIT3(PLR3_NOSHOUT);
			ad1500_show_menu(d);
			break;
		case 5:
			TOGGLE_BIT3(PLR3_NOAUCTION);
			ad1500_show_menu(d);
			break;
		case 6:
			TOGGLE_BIT3(PLR3_NOINFO);
			ad1500_show_menu(d);
			break;
		case 7:
			TOGGLE_BIT3(PLR3_NOCLAN);
			ad1500_show_menu(d);
			break;
		case 8:
			TOGGLE_BIT3(PLR3_NOSYSTEM);
			ad1500_show_menu(d);
			break;
		case 9:
			TOGGLE_BIT3(PLR3_NOIMM);
			ad1500_show_menu(d);
			break;
		//case 10:
			//TOGGLE_BIT3(PLR3_NOANTE);
			//ad1500_show_menu(d);
			//break;
		case 11:
			TOGGLE_BIT3(PLR3_NOPKFLAME);
			ad1500_show_menu(d);
			break;
		case 12:
			TOGGLE_BIT2(PLR2_NOSTAFF);
			ad1500_show_menu(d);
			break;
		case 13:
			TOGGLE_BIT2(PLR2_NOBOSS);
			ad1500_show_menu(d);
			break;
		case 14:
			TOGGLE_BIT3(PLR3_NOWIZINFO);
			ad1500_show_menu(d);
			break;
		case 15:
			TOGGLE_BIT3(PLR3_NOMUSIC);
			ad1500_show_menu(d);
			break;

		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;



/* ***********************************************************
*  ADMIN - Config flag options			     *
*********************************************************** */


	case ADMIN_CONFIG_FLAG:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_CONFIG;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2100_end_config(ch);
			}
			break;
		case 1:
			TOGGLE_BIT1(PLR1_WIMPY);
			ad1500_show_menu(d);
			break;
		case 2:
			TOGGLE_BIT2(PLR2_NO_OPPONENT_FMSG);
			ad1500_show_menu(d);
			break;
		case 3:
			TOGGLE_BIT2(PLR2_NO_PERSONAL_FMSG);
			ad1500_show_menu(d);
			break;
		case 4:
			TOGGLE_BIT2(PLR2_NO_BYSTANDER_FMSG);
			ad1500_show_menu(d);
			break;
		case 5:
			TOGGLE_BIT3(PLR3_NOSUMMON);
			ad1500_show_menu(d);
			break;
		case 6:
			TOGGLE_BIT3(PLR3_ASSIST);
			ad1500_show_menu(d);
			break;
		case 7:
			TOGGLE_BIT3(PLR3_AUTOSPLIT);
			ad1500_show_menu(d);
			break;
		case 8:
			TOGGLE_BIT3(PLR3_AUTOGOLD);
			ad1500_show_menu(d);
			break;
		case 9:
			TOGGLE_BIT3(PLR3_AUTOLOOT);
			ad1500_show_menu(d);
			break;

		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;



/* ***********************************************************
*  ADMIN - Config general options			     *
*********************************************************** */


	case ADMIN_CONFIG_GENERAL:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_CONFIG;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2100_end_config(ch);
			}
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;



/* ***********************************************************
*  ADMIN - Main menu options                                 *
*********************************************************** */


	case ADMIN_MAIN:
		switch (lv_number) {
		case 0:
			send_to_char("Returning to game.\r\n", ch);
			ad2000_clear_admin(ch);
			break;
		case 1:
			ADMIN_STATE(d) = ADMIN_EDIT;
			OLD_STATE(d) = ADMIN_MAIN;
			ad1500_show_menu(d);
			break;
		case 2:
			send_to_char("\r\nEnter player name: ", ch);
			ADMIN_STATE(d) = ADMIN_MAIN_NAME;
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;

	case ADMIN_MAIN_NAME:
		if (ad1300_select_player(ch, arg)) {
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
			OLD_STATE(d) = ADMIN_MAIN;
			ad1500_show_menu(d);
		}
		else {
			ADMIN_STATE(d) = ADMIN_MAIN;
			PRESS_ENTER(ch);
		}
		break;


		/* ***********************************************************
		 * ADMIN - Main player edit menu options		     * ********************************************************** */


	case ADMIN_EDIT:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = OLD_STATE(d);
				OLD_STATE(d) = 0;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2000_clear_admin(ch);
			}
			break;
		case 2:
			ADMIN_STATE(d) = ADMIN_OBJECTS;
			ad1500_show_menu(d);
			break;
		case 3:
			ADMIN_STATE(d) = ADMIN_MOBILES;
			ad1500_show_menu(d);
			break;
		case 5:
			ADMIN_STATE(d) = ADMIN_ZONES;
			ad1500_show_menu(d);
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


/* ***********************************************************
*  ADMIN - Objects menu	options				     *
*********************************************************** */


	case ADMIN_OBJECTS:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_EDIT;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2000_clear_admin(ch);
			}
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


/* ***********************************************************
*  ADMIN - Mobiles menu	options				     *
*********************************************************** */


	case ADMIN_MOBILES:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_EDIT;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2000_clear_admin(ch);
			}
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


/* ***********************************************************
*  ADMIN - Zones menu options				     *
*********************************************************** */


	case ADMIN_ZONES:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = ADMIN_EDIT;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2000_clear_admin(ch);
			}
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


/* ***********************************************************
*  ADMIN - Main player edit menu options		     *
*********************************************************** */


	case ADMIN_EDIT_CHOOSE:
		switch (lv_number) {
		case 0:
			if (OLD_STATE(d)) {
				ADMIN_STATE(d) = OLD_STATE(d);
				OLD_STATE(d) = 0;
				ad1500_show_menu(d);
			}
			else {
				send_to_char("\r\nReturning to game.\r\n", ch);
				ad2000_clear_admin(ch);
			}
			break;
		case 1:
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE_PLAYER;
			send_to_char("\r\nEnter player name: ", ch);
			break;
		case 2:
			ADMIN_STATE(d) = ADMIN_EDIT_GENERAL;
			ad1500_show_menu(d);
			break;
		case 3:
			ADMIN_STATE(d) = ADMIN_EDIT_ATT;
			ad1500_show_menu(d);
			break;
		case 4:
			in1800_do_score(ch, GET_REAL_NAME(victim), CMD_ADMIN);
			PRESS_ENTER(ch);
			break;
		case 5:
			in1700_do_attribute(ch, GET_REAL_NAME(victim), CMD_ADMIN);
			PRESS_ENTER(ch);
			break;
		case 6:
			ot2000_do_practice(ch, GET_REAL_NAME(victim), CMD_ADMIN);
			PRESS_ENTER(ch);
			break;
		case 7:
			in3000_do_look(ch, "at admin", CMD_ADMIN);
			PRESS_ENTER(ch);
			break;
		case 8:
			r3400_del_objs(ch);
			r2400_load_char_objs(ch, TRUE);
			send_to_char("\r\nBackup rentfile loaded. Choose save to copy over current.\r\n", ch);
			PRESS_ENTER(ch);
			break;
		case 9:
			ad2100_save_player(ch);
			send_to_char("\r\nPlayer data saved.\r\n", ch);
			PRESS_ENTER(ch);
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;

	case ADMIN_EDIT_CHOOSE_PLAYER:
		if (ad1300_select_player(ch, arg)) {
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
			ad1500_show_menu(d);
		}
		else {
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
			PRESS_ENTER(ch);
		}
		break;



		/* ***********************************************************
		 * ADMIN - Player edit general data options                  * ********************************************************** */


	case ADMIN_EDIT_GENERAL:
		switch (lv_number) {
		case 0:
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
			ad1500_show_menu(d);
			break;
		case 1:
			sprintf(buf, "&nOld Name: &G%s&n\r\nNew name: (Sorry, this has been disabled)", GET_REAL_NAME(victim));
			send_to_char(buf, ch);
			//ADMIN_STATE(d) = ADMIN_GEN_NAME;
			ADMIN_STATE(d) = ADMIN_EDIT_GENERAL;
			break;
		case 2:
			co2900_send_to_char(ch, "&nOld title: &G%s&n\r\nNew title: ", GET_TITLE(victim));
			ADMIN_STATE(d) = ADMIN_GEN_TITLE;
			break;
		case 3:
			sprintf(buf, "&nOld level: &R%d&n\r\nNew level: ", GET_LEVEL(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_LEVEL;
			break;
		case 4:
			sprintf(buf, "&nOld experience value: &R%d&n\r\nNew experience value: ", GET_EXP(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_EXP;
			break;
		case 5:
			sprintf(buf, "&nOld gold value: &R%d&n\r\nNew gold value: ", GET_GOLD(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_GOLD;
			break;
		case 6:
			sprintf(buf, "&nOld score: &R%d&n\r\nNew score: ", GET_SCORE(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_SCORE;
			break;
		case 7:
			sprintf(buf, "&nOld deaths value: &R%d&n\r\nNew deaths value: ", GET_DEATHS(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_DEATHS;
			break;
		case 8:
			sprintf(buf, "&nOld kills value: &R%d&n\r\nNew kills value: ", GET_KILLS(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_KILLS;
			break;
		case 9:
			cl1000_do_clan(ch, "list", CMD_ADMIN);
			sprintf(buf, "\r\n&nOld clan: &R%d&n\r\nNew clan: ", CLAN_NUMBER(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_CLAN;
			break;
		case 10:
			sprintf(buf, "info %d", CLAN_NUMBER(victim));
			cl1000_do_clan(ch, buf, CMD_ADMIN);
			sprintf(buf, "\r\n&nOld clan rank: &R%d&n\r\nNew clan rank: ", CLAN_RANK(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_RANK;
			break;
		case 11:
			sprintf(buf, "&nOld quest points value: &R%d&n\r\nNew quest points value: ", victim->questpoints);
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_QPOINTS;
			break;
		case 12:
			sprintf(buf, "&nOld poofin: %s&n\r\nNew poofin: ", victim->player.immortal_enter);
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_POOFIN;
			break;
		case 13:
			sprintf(buf, "&nOld poofout: %s&n\r\nNew poofout: ", victim->player.immortal_exit);
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_POOFOUT;
			break;
		case 14:
			sprintf(buf, "&nOld jail time: -&n\r\nNew jail time: ");
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_JAIL;
			break;
		case 15:
			sprintf(buf, "&nOld muzzle time: -&n\r\nNew muzzle time:");
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_GEN_MUZZLE;
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;


		/* ***********************************************************
		 * ADMIN - Player edit attributes options                    * ********************************************************** */


	case ADMIN_EDIT_ATT:
		switch (lv_number) {
		case 0:
			ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
			ad1500_show_menu(d);
			break;
		case 1:
			in1100_do_show_classes(ch, "", CMD_ADMIN);
			sprintf(buf, "\r\n&nOld class: &R%d&n\r\nNew class: ", GET_CLASS(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_CLASS;
			break;
		case 2:
			in1000_do_show_races(ch, "", CMD_ADMIN);
			sprintf(buf, "\r\n&nOld race: &R%d&n\r\nNew race: ", GET_RACE(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_RACE;
			break;
		case 3:
			sprintf(buf, "&nOld sex: &R%d&n\r\nNew sex: ", GET_SEX(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_SEX;
			break;
		case 4:
			sprintf(buf, "&nOld age: &R%d&n\r\nNew age: ", GET_EXP(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_AGE;
			break;
		case 5:
			sprintf(buf, "&nOld strength: &R%d&n\r\nNew strength value: ", GET_STR(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_STR;
			break;
		case 6:
			sprintf(buf, "&nOld intelligence: &R%d&n\r\nNew intelligence: ", GET_INT(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_INT;
			break;
		case 7:
			sprintf(buf, "&nOld wisdom: &R%d&n\r\nNew wisdom: ", GET_WIS(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_WIS;
			break;
		case 8:
			sprintf(buf, "&nOld dex: &R%d&n\r\nNew dex: ", GET_DEX(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_DEX;
			break;
		case 9:
			sprintf(buf, "\r\n&nOld con: &R%d&n\r\nNew con: ", GET_CON(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_CON;
			break;
		case 10:
			sprintf(buf, "\r\n&nOld charisma: &R%d&n\r\nNew charisma: ", GET_CHA(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_CHA;
			break;
		case 11:
			sprintf(buf, "&nOld practices value: &R%d&n\r\nNew practices: ", GET_PRACTICES(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_PRAC;
			break;
		case 12:
			sprintf(buf, "&nOld max hit: &R%d&n\r\nNew max hit: ", GET_MAX_HIT(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_HIT;
			break;
		case 13:
			sprintf(buf, "&nOld max mana: &R%d&n\r\nNew max mana: ", GET_MAX_MANA(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_MANA;
			break;
		case 14:
			sprintf(buf, "&nOld max move: &R%d&n\r\nNew max move: ", GET_MAX_MOVE(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_MOVE;
			break;
		case 15:
			send_to_char("&R-1000 &wEvil\r\n&R0     &wNeutral\r\n&R+1000 &wGood&n\r\n\r\n", ch);
			sprintf(buf, "&nOld alignment: &R%d&n\r\nNew alignment:", GET_ALIGNMENT(victim));
			send_to_char(buf, ch);
			ADMIN_STATE(d) = ADMIN_ATT_ALIGN;
			break;
		default:
			ad1500_show_menu(d);
			break;
		}		/* end of lv_number */
		break;

		/* ***********************************************************
		 * ADMIN - CONFIG Out of menu states                         * ********************************************************** */
	case ADMIN_CONFIG_SET_LINES:
		do_set_lines(ch, arg, 0);
		ADMIN_STATE(d) = ADMIN_CONFIG_DISPLAY;
		PRESS_ENTER(ch);
		break;

		/* ***********************************************************
		 * ADMIN - Out of menu states                                * ********************************************************** */


	default:
		if ((ADMIN_STATE(d) > ADMIN_GEN_START) && (ADMIN_STATE(d) < ADMIN_GEN_END)) {
			ad1110_admin_nanny_general(d, arg);
			ADMIN_STATE(d) = ADMIN_EDIT_GENERAL;
			ad1500_show_menu(d);
			return;
		}
		if ((ADMIN_STATE(d) > ADMIN_ATT_START) && (ADMIN_STATE(d) < ADMIN_ATT_END)) {
			ad1110_admin_nanny_general(d, arg);
			ADMIN_STATE(d) = ADMIN_EDIT_ATT;
			ad1500_show_menu(d);
			return;
		}
		ADMIN_STATE(d) = ADMIN_EDIT_CHOOSE;
		ad1500_show_menu(d);
		break;

	}			/* end of ADMIN_STATE(d) */

}				/* END OF ad1100_admin_nanny() */


/* ***********************************************************
*  ADMIN - Execution of commands			     *
*********************************************************** */

void ad1110_admin_nanny_general(struct descriptor_data * d, char *arg)
{

	struct char_data *ch, *victim;
	char buf[MAX_STRING_LENGTH], *lv_name;
	int lv_number = 0;

	if (!*arg)
		return;
	ch = d->character;
	victim = d->admin;

	lv_name = GET_REAL_NAME(victim);

	switch (ADMIN_STATE(d)) {
	case ADMIN_GEN_SCORE:
	case ADMIN_GEN_KILLS:
	case ADMIN_GEN_CLAN:
	case ADMIN_GEN_RANK:
	case ADMIN_GEN_QPOINTS:{
			if (!is_number(arg))
				return;
			lv_number = atoi(arg);
		}
	}

	switch (ADMIN_STATE(d)) {


		/* ***********************************************************
		 * ADMIN - Player general setting commands                   * ********************************************************** */


	case ADMIN_GEN_NAME:
		//sprintf(buf, "char %s name %s", lv_name, arg);
		//do_string(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_TITLE:
		sprintf(buf, "char %s title %s", lv_name, arg);
		do_string(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_LEVEL:
		sprintf(buf, "char %s level %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_EXP:
		sprintf(buf, "char %s exp %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_GOLD:
		sprintf(buf, "char %s gold %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_SCORE:
		GET_SCORE(victim) = lv_number;
		break;
	case ADMIN_GEN_DEATHS:
		sprintf(buf, "char %s deaths %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_GEN_KILLS:
		GET_KILLS(victim) = lv_number;
		break;
	case ADMIN_GEN_CLAN:
		CLAN_NUMBER(victim) = lv_number;
		break;
	case ADMIN_GEN_RANK:
		CLAN_RANK(victim) = lv_number;
		break;
	case ADMIN_GEN_QPOINTS:
		victim->questpoints = lv_number;
		break;
	case ADMIN_GEN_POOFIN:
		wi4000_do_poofin(victim, arg, CMD_ADMIN);
		break;
	case ADMIN_GEN_POOFOUT:
		do_poofout(victim, arg, CMD_ADMIN);
		break;
	case ADMIN_GEN_JAIL:
		/* not done yet */
		break;
	case ADMIN_GEN_MUZZLE:
		/* not done yet */
		break;


		/* ***********************************************************
		 * ADMIN - Player attribute setting commands       	     * ********************************************************** */


	case ADMIN_ATT_CLASS:
		sprintf(buf, "char %s class %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_RACE:
		sprintf(buf, "char %s race %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_SEX:
		sprintf(buf, "char %s sex %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_AGE:
		sprintf(buf, "char %s age %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_STR:
		sprintf(buf, "char %s str %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_INT:
		sprintf(buf, "char %s int %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_WIS:
		sprintf(buf, "char %s wis %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_DEX:
		sprintf(buf, "char %s dex %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_CON:
		sprintf(buf, "char %s con %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_CHA:
		sprintf(buf, "char %s cha %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_PRAC:
		sprintf(buf, "char %s prac %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_HIT:
		sprintf(buf, "char %s max_hit %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_MANA:
		sprintf(buf, "char %s max_mana %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_MOVE:
		sprintf(buf, "char %s max_move %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	case ADMIN_ATT_ALIGN:
		sprintf(buf, "char %s align %s", lv_name, arg);
		do_set(ch, buf, CMD_ADMIN);
		break;
	}			/* END OF ADMIN_STATE(d) */
}				/* END OF ad1110_admin_nanny_general() */



/* ***********************************************************
*  ADMIN - Standard menus                                    *
*********************************************************** */


void ad1200_display_menu(struct char_data * ch, int lv_menu)
{

	char buf[MAX_STRING_LENGTH];

	switch (lv_menu) {
	case MENU_EDIT_PLAYER:
		CLEAR_SCREEN(ch)
			sprintf(buf, "\r\n&wCRIMSON ][ Edit Player: &y%s&n\r\n", GET_REAL_NAME(ch->desc->admin));
		send_to_char(buf, ch);
		send_to_char("\r\n"
			     "\r\n"
			     "&W1&w)&c Select player\r\n"
			     "&W2&w)&c Edit General data\r\n"
			     "&W3&w)&c Edit attributes\r\n"
			     "\r\n"
			     "&W4&w)&c Show score\r\n"
			     "&W5&w)&c Show attributes\r\n"
			     "&W6&w)&c Show practice\r\n"
			     "&W7&w)&c Show inventory\r\n"
			     "\r\n"
			     "&W8&w)&c Restore backup rentfile\r\n"
			     "&W9&w)&c Save data\r\n"
			     "\r\n"
			     "&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_ADMIN:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][  ADMIN MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w)&c Edit zones\r\n"
				     "&W2&w)&c Edit players\r\n"
				     "&W3&w)&c Edit clans\r\n"
				     "&W4&w)&c General settings\r\n"
				     "&W5&w)&c Statistics \r\n"
				     "\r\n"
				     "&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_EDIT:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][  EDIT MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w)&c Select zone\r\n"
				     "&W2&w)&c Objects\r\n"
				     "&W3&w)&c Mobiles\r\n"
				     "&W4&w)&c Rooms\r\n"
				     "&W5&w)&c Zones\r\n"
				     "\r\n"
				     "&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_OBJECTS:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSTON ][ OBJECTS MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w)&c Create\r\n"
				     "&W2&w)&c Copy\r\n"
				     "&W3&w)&c Edit\r\n"
				     "\r\n"
				     "&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_MOBILES:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][ MOBILES MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w)&c Create\r\n"
				     "&W2&w)&c Copy\r\n"
				     "&W3&w)&c Edit\r\n"
				     "\r\n"
				     "n&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_ZONES:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][ ZONES MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w) &cEdit\r\n"
				     "&W2&w) &cReboot\r\n"
				     "&W3&w) &cReset\r\n"
				     "\r\n"
				     "&W0&w)&r Quit&n\r\n", ch);
		break;
	case MENU_CONFIG:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][ CONFIG MENU\r\n"
				     "\r\n"
				     "\r\n"
				     "&W1&w) &cPrompt settings\r\n"
				     "&W2&w) &cDisplay settings\r\n"
				     "&W3&w) &cChannel settings\r\n"
				     "&W4&w) &cFlag settings\r\n"
				     "&W5&w) &cGeneral settings\r\n"
				     "\r\n"
				     "&W0&w)&r Quit&n\r\n", ch);
		break;
	default:
		CLEAR_SCREEN(ch)
			send_to_char("\r\n&wCRIMSON ][ UNKNOWN MENU!\r\n", ch);
		return;
	}			/* END OF lv_menu */

	send_to_char("\r\nEnter choice: ", ch);

}				/* END OF ad1200_display_menu() */


/* ***********************************************************
*  ADMIN - Player selection for edit player menus            *
*********************************************************** */


int ad1300_select_player(struct char_data * ch, char *arg)
{

	struct char_file_u char_element;
	struct char_data *victim;
	char buf[MAX_STRING_LENGTH];
	int lv_player_nr, idx;
	FILE *fl;

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!*buf) {
		send_to_char("Please provide a name.\r\n", ch);
		return (FALSE);
	}

	lv_player_nr = 0;
	for (idx = 0; (*(buf + idx) = LOWER(*(buf + idx))); idx++);
	for (idx = 0; idx <= top_of_p_table; idx++)
		if (strcmp(player_table[idx].pidx_name, buf) == 0)
			lv_player_nr = idx;

	if (!lv_player_nr) {
		for (idx = 0; idx <= top_of_p_table; idx++)
			if (is_abbrev(buf, player_table[idx].pidx_name))
				lv_player_nr = idx;
	}

	if (!lv_player_nr) {
		send_to_char("Unknown player.\r\n", ch);
		return (FALSE);
	}

	if (!(fl = fopen(PLAYER_FILE, "rb"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Opening player file for reading. (admin.c, qu1000_do_admin)");
		perror(buf);
		send_to_char("ERROR: Opening player file for reading. (admin.c, qu1000_do_admin)\r\n", ch);
		return (FALSE);
	}

	fseek(fl, (long) (player_table[lv_player_nr].pidx_nr *
			  sizeof(struct char_file_u)), 0);

	fread(&char_element, sizeof(struct char_file_u), 1, fl);
	fclose(fl);

	if (char_element.level > GET_LEVEL(ch)) {
		send_to_char("You can't edit chars with a higher level than yourself!\r\n", ch);
		return (FALSE);
	}

	/* If there already is a character, free memory */
	if (ch->desc->admin) {
		r3400_del_objs(ch->desc->admin);
		db7100_free_char(ch->desc->admin, 0);
		ch->desc->admin = 0;
	}

	CREATE(victim, struct char_data, 1);
	db7500_clear_char(victim);
	db6100_store_to_char(&char_element, victim);
	r2400_load_char_objs(victim, FALSE);
	ch->desc->admin = victim;

	return (TRUE);
}				/* END OF ad1300_select_player() */


/* ***********************************************************
*  ADMIN - Player Edit general Menu			     *
*********************************************************** */


void ad1400_display_general(struct char_data * ch)
{

	//char buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	victim = ch->desc->admin;

	CLEAR_SCREEN(ch)
		co2900_send_to_char(ch, "\r\n&wCRIMSON ][ Edit General data: &y%s&n\r\n", GET_REAL_NAME(victim));
	co2900_send_to_char(ch, "&W\r\n\r\n 1&w) &cName (Disabled)  &w: &G%s&n\r\n", GET_REAL_NAME(victim));
	co2900_send_to_char(ch, "&W 2&w) &cTitle            &w: &G%s&n\r\n", GET_TITLE(victim));
	co2900_send_to_char(ch, "&W 3&w) &cLevel            &w: &R%d&n\r\n", GET_LEVEL(victim));
	co2900_send_to_char(ch, "&W 4&w) &cExperience       &w: &R%d&n\r\n", GET_EXP(victim));
	co2900_send_to_char(ch, "&W 5&w) &cGold             &w: &R%d&n\r\n", GET_GOLD(victim));
	co2900_send_to_char(ch, "\r\n&W 6&w) &cScore            &w: &R%d&n\r\n", GET_SCORE(victim));
	co2900_send_to_char(ch, "&W 7&w) &cDeaths           &w: &R%d&n\r\n", GET_DEATHS(victim));
	co2900_send_to_char(ch, "&W 8&w) &cKills            &w: &R%d&n\r\n", GET_KILLS(victim));
	co2900_send_to_char(ch, "&W 9&w) &cClan             &w: &G%s &w(&R%d&w)&n\r\n",
		     GET_CLAN_NAME(CLAN_NUMBER(victim)), CLAN_NUMBER(victim));
	co2900_send_to_char(ch, "&W10&w) &cClan Rank        &w: &G%s &w(&R%d&w)&n\r\n",
			    GET_CLAN_RANK_NAME(CLAN_NUMBER(victim), CLAN_RANK(victim)), CLAN_RANK(victim));
	co2900_send_to_char(ch, "\r\n&W11&w) &cQuest Points     &w: &R%d&n\r\n", victim->questpoints);
	co2900_send_to_char(ch, "&W12&w) &cPoofin           &w: %s&n\r\n", victim->player.immortal_enter);
	co2900_send_to_char(ch, "&W13&w) &cPoofout          &w: %s&n\r\n", victim->player.immortal_exit);
	co2900_send_to_char(ch, "&W14&w) &cJailtime         &w: &R-&n\r\n");
	co2900_send_to_char(ch, "&W15&w) &cMuzzletime       &w: &R-&n\r\n");
	co2900_send_to_char(ch, "\r\n&W 0) &rReturn.&n");
	co2900_send_to_char(ch, "\r\n\r\nEnter choice: ");

}				/* END OF ad1400_display_general() */


/* ***********************************************************
*  ADMIN - Player edit attributes menu			     *
*********************************************************** */


void ad1410_display_att(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	victim = ch->desc->admin;

	CLEAR_SCREEN(ch)
		sprintf(buf, "\r\n&wCRIMSON ][ Edit Attributes: &y%s&n\r\n", GET_REAL_NAME(victim));
	send_to_char(buf, ch);
	sprintf(buf, "\r\n\r\n&W 1&w) &cClass            &w: &G%s &w(&R%d&w)&n\r\n", classes[GET_CLASS(victim)].name, GET_CLASS(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w) &cRace             &w: &G%s &w(&R%d&w)&n\r\n", races[GET_RACE(victim)].name, GET_RACE(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w) &cSex              &w: &R%d&n\r\n", GET_SEX(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 4&w) &cAge              &w: &R%d&n\r\n", GET_AGE(victim));
	send_to_char(buf, ch);
	sprintf(buf, "\r\n&W 5&w) &cStrength         &w: &R%d&n\r\n", GET_REAL_STR(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 6&w) &cIntelligence     &w: &R%d&n\r\n", GET_REAL_INT(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 7&w) &cWisdom           &w: &R%d&n\r\n", GET_REAL_WIS(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 8&w) &cDex              &w: &R%d&n\r\n", GET_REAL_DEX(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W 9&w) &cCon              &w: &R%d&n\r\n", GET_REAL_CON(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W10&w) &cCharisma         &w: &R%d&n\r\n", GET_REAL_CHA(victim));
	send_to_char(buf, ch);
	sprintf(buf, "\r\n&W11&w) &cPractices        &w: &R%d&n\r\n", GET_PRACTICES(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W12&w) &cMax Hit          &w: &R%d&n\r\n", GET_MAX_HIT(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W13&w) &cMax Mana         &w: &R%d&n\r\n", GET_MAX_MANA(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W14&w) &cMax Move         &w: &R%d&n\r\n", GET_MAX_MOVE(victim));
	send_to_char(buf, ch);
	sprintf(buf, "&W15&w) &cAlignment        &w: &R%d&n\r\n", GET_ALIGNMENT(victim));
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w) &rReturn.&n", ch);
	send_to_char("\r\n\r\nEnter choice: ", ch);

}				/* END OF ad1410_display_att() */


/* ***********************************************************
*  ADMIN - Player edit object menu			     *
*********************************************************** */


void ad1420_display_object(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	struct obj_data *obj, *tmp_obj = 0;
	struct extra_descr_data *desc;
	int lv_type, lv_value, lv_skill_lookup;

	obj = ch->desc->admin_obj;
	lv_type = GET_ITEM_TYPE(obj);

	CLEAR_SCREEN(ch);
	sprintf(buf, "\r\n&wCRIMSON ][  Edit object: &y%s &w(&Y%d&w)\r\n", GET_OSDESC(obj), obj_index[obj->item_number].virtual);
	send_to_char(buf, ch);
	sprintf(buf, "\r\n\r\n&W 1&w)&c Keywords            &w: &G%s\r\n", obj->name);
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w)&c Short description   &w: %s\r\n", GET_OSDESC(obj));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w)&c Long description    &w: %s\r\n", obj->description);
	send_to_char(buf, ch);
	sprinttype(lv_type, item_types, buf2);
	sprintf(buf, "\r\n&W 4&w)&c Type                &w: &Y%s\r\n", buf2);
	send_to_char(buf, ch);
	sprintf(buf, "&W 5&w)&c Reset percentage    &w: &R%d\r\n", obj_index[obj->item_number].maximum);
	send_to_char(buf, ch);
	send_to_char("&W 6&w)&c Worn flags          &w: &Y", ch);
	sprintbit(obj->obj_flags.wear_flags, wear_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	send_to_char("&W 7&w)&c Extra flags         &w: &Y", ch);
	sprintbit(obj->obj_flags.flags1, oflag1_bits, buf);
	sprintbit(obj->obj_flags.flags2, oflag2_bits, buf);
	send_to_char(buf, ch);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	/* OVALUE 1 */
	lv_value = obj->obj_flags.value[0];
	switch (lv_type) {
	case ITEM_WEAPON:
		sprintbit(lv_value, weapon_bits, buf2);
		sprintf(buf3, "&Y%s", buf2);
		break;
	case ITEM_KEY:
		if (lv_value > 0) {
			if (!(tmp_obj = db5100_read_object(lv_value, PROTOTYPE)))
				sprintf(buf3, "&YIllegal object &w(&R%d&w)", lv_value);
			else {
				if ((GET_ITEM_TYPE(tmp_obj) != ITEM_CONTAINER) &&
				    (GET_ITEM_TYPE(tmp_obj) != ITEM_QSTCONT))
					sprintf(buf3, "&G%s &w(&R%d&w) &YERROR&w:&W Not a container", GET_OSDESC(tmp_obj), lv_value);
				else
					sprintf(buf3, "&G%s &w(&R%d&w)", GET_OSDESC(tmp_obj), lv_value);
			}
		}
		else
			sprintf(buf3, "&GDoor &w(&R0&w)");
		break;
	default:
		sprintf(buf3, "&R%d", lv_value);
		break;
	}
	sprintf(buf, "\r\n&W 8&w)&c %-20s&w: %s\r\n", ovalues[lv_type - 1].value1, buf3);
	send_to_char(buf, ch);

	/* OVALUE 2 */
	lv_value = obj->obj_flags.value[1];
	switch (lv_type) {
	case ITEM_SCROLL:
	case ITEM_POTION:
		lv_skill_lookup = -1;
		if (lv_value > 0) {
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == lv_value) {
					break;
				}
			}
			if (skill_numbers[lv_skill_lookup] == 0)
				sprintf(buf2, "%-20s", spell_names[lv_value - 1]);
			else
				sprintf(buf2, "%-20s", skill_names[lv_skill_lookup]);
		}
		sprintf(buf3, "&G%s&w (&R%d&w)", ((lv_value > 0) ? buf2 : "None"), lv_value);
		break;
	case ITEM_CONTAINER:
		sprintbit(lv_value, lock_bits, buf2);
		sprintf(buf3, "&Y%s", buf2);
		break;
	case ITEM_KEY:
		if (lv_value == 0)
			sprintf(buf3, "&GUnlimited. &w(&R0&w)");
		else
			sprintf(buf3, "&R%d", lv_value);
		break;
	default:
		sprintf(buf3, "&R%d", lv_value);
		break;
	}
	sprintf(buf, "&W 9&w)&c %-20s&w: %s\r\n", ovalues[lv_type - 1].value2, buf3);
	send_to_char(buf, ch);

	/* OVALUE 3 */
	lv_value = obj->obj_flags.value[2];
	switch (lv_type) {
	case ITEM_SCROLL:
	case ITEM_POTION:
		lv_skill_lookup = -1;
		if (lv_value > 0) {
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == lv_value) {
					break;
				}
			}
			if (skill_numbers[lv_skill_lookup] == 0)
				sprintf(buf2, "%-20s", spell_names[lv_value - 1]);
			else
				sprintf(buf2, "%-20s", skill_names[lv_skill_lookup]);
		}
		sprintf(buf3, "&G%s&w (&R%d&w)", ((lv_value > 0) ? buf2 : "None"), lv_value);
		break;
	case ITEM_DRINKCON:
		sprinttype(lv_value, drinks, buf2);
		sprintf(buf3, "&G%s &w(&R%d&w)", buf2, lv_value);
		break;
	default:
		sprintf(buf3, "&R%d", lv_value);
		break;
	}
	sprintf(buf, "&W10&w)&c %-20s&w: %s\r\n", ovalues[lv_type - 1].value3, buf3);
	send_to_char(buf, ch);

	/* OVALUE 4 */
	lv_value = obj->obj_flags.value[3];
	switch (lv_type) {
	case ITEM_WAND:
	case ITEM_STAFF:
	case ITEM_ARMOR:
	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_SPELL:
		lv_skill_lookup = -1;
		if (lv_value > 0) {
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == lv_value) {
					break;
				}
			}
			if (skill_numbers[lv_skill_lookup] == 0)
				sprintf(buf2, "%-20s", spell_names[lv_value - 1]);
			else
				sprintf(buf2, "%-20s", skill_names[lv_skill_lookup]);
		}
		sprintf(buf3, "&G%s&w (&R%d&w)", ((lv_value > 0) ? buf2 : "None"), lv_value);
		break;
	case ITEM_WEAPON:
		sprinttype(lv_value, wtypes, buf2);
		sprintf(buf3, "&G%s&w (&R%d&w)", buf2, lv_value);
		break;
	case ITEM_CONTAINER:
	case ITEM_DRINKCON:
	case ITEM_FOOD:
		sprintf(buf3, "&G%s&w (&R%d&w)", (lv_value ? "Yes" : "No"), lv_value);
		break;
	default:
		sprintf(buf3, "&R%d", lv_value);
		break;
	}
	sprintf(buf, "&W11&w)&c %-20s&w: %s\r\n", ovalues[lv_type - 1].value4, buf3);
	send_to_char(buf, ch);
	sprinttype(obj->affected[0].location, apply_types, buf2);
	sprintf(buf, "\r\n&W12&w)&c Affect 0            &w: &G%s &wby &R%d\r\n", buf2, obj->affected[0].modifier);
	send_to_char(buf, ch);
	sprinttype(obj->affected[1].location, apply_types, buf2);
	sprintf(buf, "&W13&w)&c Affect 1            &w: &G%s &wby &R%d\r\n", buf2, obj->affected[1].modifier);
	send_to_char(buf, ch);
	sprinttype(obj->affected[2].location, apply_types, buf2);
	sprintf(buf, "&W14&w)&c Affect 2            &w: &G%s &wby &R%d\r\n", buf2, obj->affected[2].modifier);
	send_to_char(buf, ch);
	sprinttype(obj->affected[3].location, apply_types, buf2);
	sprintf(buf, "&W15&w)&c Affect 3            &w: &G%s &wby &R%d\r\n", buf2, obj->affected[3].modifier);
	send_to_char(buf, ch);
	send_to_char("\r\n&W16&w)&c Extra desc keywords &w: ", ch);
	if (obj->ex_description) {
		strcpy(buf, "&Y");
		for (desc = obj->ex_description; desc; desc = desc->next) {
			strcat(buf, desc->keyword);
			strcat(buf, "\r\n");
		}
	}
	else {
		strcpy(buf, "&wNone\r\n");
	}
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w)&r Quit\r\n", ch);


}				/* END OF ad1420_display_object() */


/* ***********************************************************
*  ADMIN - Config menu's				     *
*********************************************************** */

void ad1430_display_prompt(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch)
		send_to_char("\r\n&wCRIMSON ][ CONFIGURE PROMPT&n\r\n", ch);
	sprintf(buf, "\r\n\r\n&W 1&w) &cShow name        &w: &W%s&n\r\n", BIT2_YES_NO(PLR2_SHOW_NAME));
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w) &cShow room        &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_SHOW_ROOM));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w) &cShow hit         &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_SHOW_HP));
	send_to_char(buf, ch);
	sprintf(buf, "&W 4&w) &cShow mana        &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_SHOW_MANA));
	send_to_char(buf, ch);
	sprintf(buf, "&W 5&w) &cShow move        &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_SHOW_MOVE));
	send_to_char(buf, ch);
	sprintf(buf, "&W 6&w) &cShow exits       &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_SHOW_EXITS));
	send_to_char(buf, ch);
	sprintf(buf, "&W 7&w) &cShow diagnostics &w: &W%s&n\r\n", BIT1_YES_NO(PLR1_SHOW_DIAG));
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w) &rReturn.&n", ch);
	send_to_char("\r\n\r\nEnter choice: ", ch);

}				/* END OF ad1430_display_prompt() */

void ad1431_display_display(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch)
		send_to_char("\r\n&wCRIMSON ][ CONFIGURE DISPLAY&n\r\n", ch);
	sprintf(buf, "\r\n\r\n&W 1&w) &cUse colors         &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_ANSI));
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w) &cUse new graphics   &w: &W%s&n\r\n", BIT2_YES_NO(PLR2_NEWGRAPHICS));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w) &cLines per screen   &w: &G%d&n\r\n", (ch->screen_lines == 0) ? 18 : ch->screen_lines);
	send_to_char(buf, ch);
	sprintf(buf, "&W 4&w) &cBrief Descriptions &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_BRIEF));
	send_to_char(buf, ch);
	sprintf(buf, "&W 5&w) &cCompact display    &w: &W%s&n\r\n", BIT3_YES_NO(PLR3_COMPACT));
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w) &rReturn.&n", ch);
	send_to_char("\r\n\r\nEnter choice: ", ch);

}				/* END OF ad1430_display_display() */

void ad1432_display_channel(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch)
		send_to_char("\r\n&wCRIMSON ][ CONFIGURE CHANNELS&n\r\n", ch);
	sprintf(buf, "\r\n\r\n&W 1&w) &cTell        &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOTELL));
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w) &cGossip      &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOGOSSIP));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w) &cAvatar      &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOAVATAR));
	send_to_char(buf, ch);
	sprintf(buf, "&W 4&w) &cShout       &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOSHOUT));
	send_to_char(buf, ch);
	sprintf(buf, "&W 5&w) &cAuction     &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOAUCTION));
	send_to_char(buf, ch);
	sprintf(buf, "&W 6&w) &cInfo        &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOINFO));
	send_to_char(buf, ch);
	sprintf(buf, "&W 7&w) &cClan        &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOCLAN));
	send_to_char(buf, ch);
	sprintf(buf, "&W 8&w) &cSystem      &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOSYSTEM));
	send_to_char(buf, ch);
	sprintf(buf, "&W 9&w) &cImmortal    &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOIMM));
	send_to_char(buf, ch);
	//sprintf(buf, "&W10&w) &cAnte        &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOANTE));
	//send_to_char(buf, ch);
	sprintf(buf, "&W11&w) &cPkflame     &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOPKFLAME));
	send_to_char(buf, ch);
	sprintf(buf, "&W12&w) &cSTAFF   &w: &W%s&n\r\n", BIT2_ON_OFF(PLR2_NOSTAFF));
	send_to_char(buf, ch);
	sprintf(buf, "&W13&w) &cBOSS       &w: &W%s&n\r\n", BIT2_ON_OFF(PLR2_NOBOSS));
	send_to_char(buf, ch);
	sprintf(buf, "&W14&w) &cWizinfo     &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOWIZINFO));
	send_to_char(buf, ch);
	sprintf(buf, "&W15&w) &cMusic       &w: &W%s&n\r\n", BIT3_ON_OFF(PLR3_NOMUSIC));
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w) &rReturn.&n", ch);
	send_to_char("\r\n\r\nEnter choice: ", ch);


}				/* END OF ad1430_display_channel() */

void ad1433_display_flag(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch)
		send_to_char("\r\n&wCRIMSON ][ CONFIGURE FLAGS&n\r\n", ch);
	sprintf(buf, "\r\n\r\n&W 1&w) &cWimpy            &w: &W%s&n\r\n", BIT1_OFF_ON(PLR1_WIMPY));
	send_to_char(buf, ch);
	sprintf(buf, "&W 2&w) &cOpponent Message &w: &W%s&n\r\n", BIT2_ON_OFF(PLR2_NO_OPPONENT_FMSG));
	send_to_char(buf, ch);
	sprintf(buf, "&W 3&w) &cPersonal Message &w: &W%s&n\r\n", BIT2_ON_OFF(PLR2_NO_PERSONAL_FMSG));
	send_to_char(buf, ch);
	sprintf(buf, "&W 4&w) &cBystander Message&w: &W%s&n\r\n", BIT2_ON_OFF(PLR2_NO_BYSTANDER_FMSG));
	send_to_char(buf, ch);
	sprintf(buf, "&W 5&w) &cCan be summoned  &w: &W%s&n\r\n", BIT3_NO_YES(PLR3_NOSUMMON));
	send_to_char(buf, ch);
	sprintf(buf, "&W 6&w) &cAuto Assist      &w: &W%s&n\r\n", BIT3_OFF_ON(PLR3_ASSIST));
	send_to_char(buf, ch);
	sprintf(buf, "&W 7&w) &cAuto split coins &w: &W%s&n\r\n", BIT3_OFF_ON(PLR3_AUTOSPLIT));
	send_to_char(buf, ch);
	sprintf(buf, "&W 8&w) &cAuto loot gold   &w: &W%s&n\r\n", BIT3_OFF_ON(PLR3_AUTOGOLD));
	send_to_char(buf, ch);
	sprintf(buf, "&W 9&w) &cAuto loot items  &w: &W%s&n\r\n", BIT3_OFF_ON(PLR3_AUTOLOOT));
	send_to_char(buf, ch);
	send_to_char("\r\n&W 0&w) &rReturn.&n", ch);
	send_to_char("\r\n\r\nEnter choice: ", ch);

}				/* END OF ad1430_display_flag() */

void ad1434_display_general(struct char_data * ch)
{
	//char buf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch)
		co2900_send_to_char(ch, "\r\n&wCRIMSON ][ CONFIGURE GENERAL&n\r\n");
	co2900_send_to_char(ch, "\r\n\r\n&W 1&w) &cTitle       &w: &W%s&n\r\n", GET_TITLE(ch));
	co2900_send_to_char(ch, "&W 2&w) &cPoofin      &w: &n%s\r\n",
	   ch->player.immortal_enter ? ch->player.immortal_enter : "(empty)");
	co2900_send_to_char(ch, "&W 3&w) &cPoofout     &w: &n%s\r\n",
	     ch->player.immortal_exit ? ch->player.immortal_exit : "(empty)");
	co2900_send_to_char(ch, "&W 4&w) &cDescription &w: \r\n&B%s&n\r\n\r\n",
		   ch->player.long_descr ? ch->player.long_descr : "(empty)");
	co2900_send_to_char(ch, "\r\n&W 0&w) &rReturn.&n");
	co2900_send_to_char(ch, "\r\n\r\nEnter choice: ");
}				/* END OF ad1430_display_general() */


/* ***********************************************************
*  ADMIN - Show menu's according to state		     *
*********************************************************** */


/* Show menu according to current state of character */
void ad1500_show_menu(struct descriptor_data * d)
{

	struct char_data *ch;

	ch = d->character;

	switch (ADMIN_STATE(d)) {
	case ADMIN_MAIN:
		ad1200_display_menu(ch, MENU_ADMIN);
		break;
	case ADMIN_EDIT_CHOOSE:
		ad1200_display_menu(ch, MENU_EDIT_PLAYER);
		break;
	case ADMIN_EDIT_GENERAL:
		ad1400_display_general(ch);
		break;
	case ADMIN_EDIT_ATT:
		ad1410_display_att(ch);
		break;
	case ADMIN_EDIT:
		ad1200_display_menu(ch, MENU_EDIT);
		break;
	case ADMIN_OBJECTS:
		ad1200_display_menu(ch, MENU_OBJECTS);
		break;
	case ADMIN_MOBILES:
		ad1200_display_menu(ch, MENU_MOBILES);
		break;
	case ADMIN_ZONES:
		ad1200_display_menu(ch, MENU_ZONES);
		break;
	case ADMIN_CONFIG:
		ad1200_display_menu(ch, MENU_CONFIG);
		break;
	case ADMIN_CONFIG_PROMPT:
		ad1430_display_prompt(ch);
		break;
	case ADMIN_CONFIG_DISPLAY:
		ad1431_display_display(ch);
		break;
	case ADMIN_CONFIG_CHANNEL:
		ad1432_display_channel(ch);
		break;
	case ADMIN_CONFIG_FLAG:
		ad1433_display_flag(ch);
		break;
	case ADMIN_CONFIG_GENERAL:
		ad1434_display_general(ch);
		break;

	}
}				/* END OF ad1500_show_menu() */


/* ***********************************************************
*  ADMIN - Clear admin data                                  *
*********************************************************** */


void ad2000_clear_admin(struct char_data * ch)
{

	if (ch->desc->admin) {
		r3400_del_objs(ch->desc->admin);
		db7100_free_char(ch->desc->admin, 0);
		ch->desc->admin = 0;
	}
	STATE(ch->desc) = CON_PLAYING;
	ADMIN_STATE(ch->desc) = 0;
	OLD_STATE(ch->desc) = 0;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, GET_PRIVATE(ch));
	act("$n suddenly appears in the middle of the room.", TRUE, ch, 0, 0, TO_ROOM);
	GET_PRIVATE(ch) = 0;

}				/* END OF ad2000_clear_admin */

void ad2100_end_config(struct char_data * ch)
{

	char colorbuf[MAX_STRING_LENGTH];

	CLEAR_SCREEN(ch);
	ADMIN_STATE(ch->desc) = 0;
	OLD_STATE(ch->desc) = 0;
	sprintf(colorbuf, MENU);
	SEND_TO_Q(colorbuf, ch->desc);
	STATE(ch->desc) = CON_GET_MENU_SELECTION;
}


/* ***********************************************************
*  ADMIN - Save player data                                  *
*********************************************************** */

void ad2100_save_player(struct char_data * ch)
{

	struct char_file_u st;
	struct char_data *vict;
	FILE *fl;
	char mode[4];
	char buf[MAX_STRING_LENGTH];


	strcpy(mode, "r+b");

	vict = ch->desc->admin;
	vict->nr = pa2000_find_name(GET_REAL_NAME(vict));

	fl = fopen(PLAYER_FILE, mode);
	if (!fl) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: db6400 saving player %d.\r\n", errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	if (vict->nr == 0) {
		send_to_char("Error, players vnr was 0!\r\n", ch);
		return;
	}

	//sprintf(buf, "Updating char %s (%d)", GET_REAL_NAME(vict), vict->nr);
	//send_to_char(buf, ch);

	fseek(fl, vict->nr * sizeof(struct char_file_u), 0);
	fread(&st, sizeof(struct char_file_u), 1, fl);

	db6200_char_to_store(vict, &st, FALSE);

	fseek(fl, vict->nr * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);

	fclose(fl);

}				/* END OF ad2100_save_player */
