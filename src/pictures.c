/* pi */
/* *******************************************************************
*  file: pictures.c , Special module.        Part of Crimson MUD     *
*  Usage: Procedures handling ingame pictures                        *
*                     Written by Hercules                            *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

void pi1000_show_rip(struct char_data * ch)
{

	send_to_char("   &K /\"\"\"\"\"/\"\\\"\"\"\"\"..\n\r", ch);
	send_to_char("  &K /     /  /\\      \\            &y__\n\r", ch);
	send_to_char("  &K/     /       &w /\\  &K\\           &y||\n\r", ch);
	send_to_char(" &K/____ / &w /\\  |  |/&K   \\          &y||\n\r", ch);
	send_to_char("&K|     |  &w|_/  | .| &K   |          &y||\n\r", ch);
	send_to_char("&K|     |  &w| \\ .|    &K   |          &y||\n\r", ch);
	send_to_char("&K|  &w _/&K|  &w|           &K |          &y||\n\r", ch);
	send_to_char("&K|&w _/&K  |    &w         &K  |          &y||\n\r", ch);
	send_to_char("&K|&w/ \\_&K |     &w* *   * * &K|         &K_&y||&K_\n\r", ch);
	send_to_char("&K|    &w\\&K|     &w*\\/* *\\/* &K|        &K| TT |\n\r", ch);
	send_to_char("&K|     |     &w*_\\_  /   &y...&g\"\"\"\"\"\"&K| || |&y.&g\"\"&y....&g\"\"\"\"\"\"\"\"&y.&g\"\"\n\r", ch);
	send_to_char("&K|     |    &w*    \\/&y..&g\"\"\"\"\"&y...&g\"\"\"&K\\ || /\n\r", ch);
	send_to_char("&K|&y....&g\"\"\"\"\"\"\"&y........&g\"\"\"\"\"\"&y^^^^&g\"&y.......&g\"\"\"\"\"\"\"\"&y..&g\"\n\r", ch);
	send_to_char("&K|&y......&g\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"&y........&g\"\"\"\"\"&y....&g\"\"\"\"\"&y..&g\"\"\n\r", ch);
	send_to_char("&E\n\r\n\r", ch);
}

void do_request(struct char_data * ch, char *arg, int cmd)
{

	int i, choice, lv_nr;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct extra_descr_data *desc;
	int zone, start, end, location;
	struct char_data *mob;

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);
	if (!*buf)
		return;
	if (!is_number(buf))
		return;

	choice = atoi(buf);

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);
	if (!*buf)
		return;
	if (!is_number(buf))
		return;

	zone = atoi(buf);
	if (zone > top_of_zone_table)
		return;

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);
	if (!*buf)
		return;
	if (!is_number(buf))
		return;

	lv_nr = atoi(buf);

	sprintf(buf, "@S%d@", choice);
	send_to_char(buf, ch);

	start = zone_table[zone].bottom;
	end = zone_table[zone].top;

	//Get zone list
		if (choice == 1) {
		for (i = 0; i <= top_of_zone_table; i++) {
			sprintf(buf, "%d,%s;",
				i, zone_table[i].filename);
			send_to_char(buf, ch);
		}
	}


	//Get mob list
		if (choice == 2) {
		for (i = zone_table[zone].bot_of_mobt; (i >= 0) && (i <= top_of_mobt) && (mob_index[i].virtual <= end); i = mob_index[i].next) {
			if (mob_index[i].virtual >= start) {
				sprintf(buf, "%d,%s;", mob_index[i].virtual, mob_index[i].name);
				send_to_char(buf, ch);
			}
		}
	}

	//Get obj list
		if (choice == 3) {
		for (i = zone_table[zone].bot_of_objt; (i >= 0) && (i <= top_of_objt) && (obj_index[i].virtual <= end); i = obj_index[i].next) {
			if (obj_index[i].virtual >= start) {
				sprintf(buf, "%d,%s;", obj_index[i].virtual, obj_index[i].name);
				send_to_char(buf, ch);
			}
		}

	}

	//Get room list
		if (choice == 4) {
		for (i = 0; i < carved_world; i++) {
			if (world[i].number >= start && world[i].number <= end && world[i].name) {
				sprintf(buf, "%d,%s;", world[i].number, world[i].name);
				send_to_char(buf, ch);
			}
		}
	}

	//Get Room info
		if (choice == 100) {
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == lv_nr)
				break;
			else if (location == top_of_world)
				return;
		sprinttype(world[location].sector_type, sector_types, buf2);
		sprintf(buf, "%d,%s,%s,%s,",
			world[location].number, world[location].name, buf2,
			(world[location].funct) ? "Y" : "N");
		send_to_char(buf, ch);

		sprintf(buf, "%ld,%ld", world[location].room_flags, world[location].room_flags2);
		send_to_char(buf, ch);

		sprintf(buf, "%s,", world[location].description);
		send_to_char(buf, ch);

		for (desc = world[location].ex_description; desc; desc = desc->next) {
			sprintf(buf, "%s&", desc->keyword);
			send_to_char(buf, ch);
		}

		send_to_char(",", ch);

		/*
		       for (i=0; i<=5; i++) {
			       if (world[location].dir_option[i]) {
				       sprintbit(world[location].dir_option[i]->exit_info, exit_bits, buf2);
				       sprintf(buf, "[%s] to room:[%d], flags:[%s]\r\n",
								       dirs[i], world[world[location].dir_option[i]->to_room].number, buf2);
				       send_to_char(buf, ch);
				       if (world[location].dir_option[i]->keyword) {
					       sprintf(buf, "Key:[%d] Keywords:[%s]\r\n",
									       world[location].dir_option[i]->key, world[location].dir_option[i]->keyword);
					       send_to_char(buf, ch);
				       }
				      
				       if (world[location].dir_option[i]->general_description) {
					       send_to_char(world[location].dir_option[i]->general_description, ch);
				       }
			       }
		       }
		      
		       sprintf(buf, "Minimum level %d maximum level %d.\r\n",
						       world[ch->in_room].min_level,
						       world[ch->in_room].max_level);
		       send_to_char(buf, ch);
		       */
	}

	//Get mobile info
		if (choice == 101) {
		if ((mob = db5000_read_mobile(lv_nr, PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
			//0 - Number, keyword, Sex
				sprintf(buf, "%d|%s|%d|", mob_index[mob->nr].virtual, (mob->player.name + 4), mob->player.sex);
			send_to_char(buf, ch);

			//3 - Short desc, Name, Long desc, Desc
				sprintf(buf, "%s|%s|%s|%s|",
					((mob->player.short_descr) ? mob->player.short_descr : "None"),
					ha1100_fname(mob_index[mob->nr].name),
					((mob->player.long_descr) ? mob->player.long_descr : "None"),
					((mob->player.description) ? mob->player.description : "None"));
			send_to_char(buf, ch);

			//7 - Act1, Act2, Act3, Aff
				sprintf(buf, "%ld|%ld|%ld|%ld|",
					GET_ACT1(mob),
					GET_ACT2(mob),
					GET_ACT3(mob),
					mob->specials.affected_by);
			send_to_char(buf, ch);

			//11 - Race, Class
				sprintf(buf, "%d|%d|",
					GET_RACE(mob),
					GET_CLASS(mob));
			send_to_char(buf, ch);

			//13 - STR, INT, WIS, DEX, CON, CHA
				sprintf(buf, "%d|%d|%d|%d|%d|%d|",
					GET_REAL_STR(mob),
					GET_REAL_INT(mob),
					GET_REAL_WIS(mob),
					GET_REAL_DEX(mob),
					GET_REAL_CON(mob),
					GET_REAL_CHA(mob));
			send_to_char(buf, ch);

			//19 - Align, Level, Gold, Exp
				sprintf(buf, "%d|%d|%d|%d|",
					mob->specials.alignment, GET_LEVEL(mob), mob->points.gold, GET_EXP(mob));
			send_to_char(buf, ch);

			//AC, HPr1, HPr2, HP + HitR, Damr1, Damr2, Damr +
				sprintf(buf, "%d|%d|%d|%d|%d|%d|%d|%d|",
					mob->points.armor,
					mob->points.max_hit,
					mob->player.weight,
					mob->points.hit,
					mob->points.hitroll,
					mob->specials.damnodice,
					mob->specials.damsizedice,
					mob->points.damroll);
			send_to_char(buf, ch);

			//special attacks nr, type, skill1, skill2, skill3(3 x)
				for (i = 0; i < MAX_SPECIAL_ATTACK; i++) {
				if (mob_index[mob->nr].attack_type[i] != 0) {
					sprintf(buf, "%d|%d|%d|%d|%d|",
						i + 1,
					    mob_index[mob->nr].attack_type[i],
					mob_index[mob->nr].attack_skill[i][0],
					mob_index[mob->nr].attack_skill[i][1],
					mob_index[mob->nr].attack_skill[i][2]);
					send_to_char(buf, ch);
				}
				else {
					sprintf(buf, "-1|-1|-1|-1|-1|");
					send_to_char(buf, ch);
				}
			}
		}
	}

	//Get Race list
		if (choice == 102) {
		for (i = 0; i < MAX_RACES; i++) {
			sprintf(buf, "%s|", races[i].name ? races[i].name : "undefined");
			send_to_char(buf, ch);
		}
	}

	//Get Class list
		if (choice == 103) {
		for (i = 0; i < MAX_CLASSES; i++) {
			sprintf(buf, "%s|", classes[i].name ? classes[i].name : "undefined");
			send_to_char(buf, ch);
		}
	}

	sprintf(buf, "@E%d@", choice);
	send_to_char(buf, ch);

}
