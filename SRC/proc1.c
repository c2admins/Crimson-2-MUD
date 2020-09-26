/* pr */
/* gv_location: 13001-13500 */
/* ****************************************************************** *
*  file: proc1.c , Special module.                    Part of DIKUMUD *
*  Usage: Procedures handling special procs for object/room/mobile    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "limits.h"
#include "db.h"
#include "constants.h"
#include "spells.h"
#include "func.h"
#include "globals.h"
#include "ansi.h"

#define MAX_PROFICIENCY 60

/* Data declarations */

struct social_type {
	char *cmd;
	int next_line;
};


/* ********************************************************************
*  Special procedures for rooms                                       *
******************************************************************** */

void pr1000_lightning_strikes(int lv_flag)
{

	struct char_data *ch, *last_dude;
	char buf[MAX_STRING_LENGTH];
	int lv_dice_roll, lv_hit_loss;



	last_dude = 0;
	for (ch = character_list; ch; ch = ch->next) {
		lv_dice_roll = number(0, 999);
		/* IF PLAYER GOT HIT */
		if (!(lv_dice_roll) &&
		    (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) &&
		    IS_PC(ch) &&
		    GET_LEVEL(ch) > 5 &&
		    GET_LEVEL(ch) < 47) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s has been struck by lightning!",
				GET_REAL_NAME(ch));
			do_info(buf, 1, MAX_LEV, ch);
			sprintf(buf, "A lightning bolt flashes down from the sky and strikes %s!",
				GET_REAL_NAME(ch));
			act(buf, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char("Lightning strikes knocking you unconscious!\r\n", ch);
			lv_hit_loss = 5;
			if (GET_HIT(ch) > 1) {
				lv_hit_loss = GET_HIT(ch) / 2;
			}
			GET_HIT(ch) -= lv_hit_loss;
			GET_POS(ch) = POSITION_STUNNED;
			if (GET_HIT(ch) < 11) {
				GET_HOW_LEFT(ch) = LEFT_BY_LIGHTNING_DEATH;
				ft1200_update_pos(ch);
				ft2000_die(ch, ch);
			}
		}		/* END OF WE CAN STRIKE THIS CHAR */
	}
	return;

}				/* pr1000_lightning_strikes() */

void pr1005_strike_char(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *vict;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	int lv_hit_loss;



	one_argument(arg, name);
	if (!*name)
		send_to_char("Who hath angered thee?\r\n", ch);
	else if (!(vict = ha3100_get_char_vis(ch, name)))
		send_to_char("No one by that name around.\r\n", ch);
	else if (GET_LEVEL(vict) >= GET_LEVEL(ch))
		send_to_char("Somehow, I doubt it.\r\n", ch);
	else {

		bzero(buf, sizeof(buf));
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "You strike %s with all your rage!\r\n",
			GET_REAL_NAME(vict));
		send_to_char(buf2, ch);
		sprintf(buf, "%s has angered the Gods!",
			GET_REAL_NAME(vict));
		do_info(buf, 1, MAX_LEV, vict);
		sprintf(buf, "A lightning bolt from the Gods strikes %s!",
			GET_REAL_NAME(vict));
		act(buf, FALSE, vict, 0, 0, TO_ROOM);
		send_to_char("Lightning from the Gods strikes you unconscious!\r\n",
			     vict);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s strikes %s.", GET_NAME(ch), GET_NAME(vict));
		do_wizinfo(buf, IMO_IMP, ch);
		spec_log(buf, GOD_COMMAND_LOG);

		lv_hit_loss = 5;
		if (GET_HIT(vict) > 16) {
			lv_hit_loss = GET_HIT(vict) / 2;
		}
		GET_HIT(vict) -= lv_hit_loss;
		GET_POS(vict) = POSITION_STUNNED;
		if (GET_HIT(vict) < 11) {
			GET_HOW_LEFT(vict) = LEFT_BY_LIGHTNING_DEATH;
			ft1200_update_pos(vict);
			ft2000_die(vict, vict);
		}
	}
	return;

}				/* pr1005_strike_char() */


char *pr1100_how_good(int percent)
{
	static char buf[MAX_STRING_LENGTH];



	if (percent == 0)
		strcpy(buf, " (not learned)");
	else if (percent <= 10)
		strcpy(buf, " (awful)");
	else if (percent <= 20)
		strcpy(buf, " (pathetic)");
	else if (percent <= 30)
		strcpy(buf, " (bad)");
	else if (percent <= 40)
		strcpy(buf, " (poor)");
	else if (percent <= 50)
		strcpy(buf, " (below average)");
	else if (percent <= 60)
		strcpy(buf, " (average)");
	else if (percent <= 70)
		strcpy(buf, " (above average)");
	else if (percent <= 80)
		strcpy(buf, " (good)");
	else if (percent <= 90)
		strcpy(buf, " (very good)");
	else
		strcpy(buf, " (Superb)");
	return (buf);
}				/* END OF pr1100_how_good() */

void pr1200_do_the_guild_no_arg(struct char_data * ch, struct char_data * victim, char *arg, int cmd)
{

	int idx, jdx, kdx, lv_row, lv_skill_lookup, lv_can_we_practice, lv_char_level,
	  lv_color;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH],
	  fbuf[MAX_STRING_LENGTH * 10];



	if (!ch->desc) {
		send_to_char("ERROR: No buffer.", ch);
		return;
	}

	if (GET_LEVEL(victim) == IMO_LEV || GET_LEVEL(victim) == PK_LEV)
		lv_char_level = wi3150_return_avatar_level(victim);
	else
		lv_char_level = GET_LEVEL(victim);

	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));
	bzero(buf3, sizeof(buf3));
	sprintf(buf2, "                                      ");	/* 38 spcs */


	if (GET_NEWGRAPHICS(ch)) {

		/* ****** */
		/* SKILLS */
		/* ****** */
		sprintf(fbuf, "\r\nYou can practice any of the following spells/skills:\r\n");
		jdx = 0;
		lv_row = 0;
		bzero(buf2, sizeof(buf2));
		for (idx = 1; idx < MAX_SKILLS; idx++) {
			/* HAVE WE EXCEEDED VALID SKILLS? */
			if (spell_names[idx - 1][0] == '\n') {
				idx = MAX_SKILLS;
				continue;
			}

			lv_can_we_practice = FALSE;

			/* CAN THE CLASS PRACTICE THIS SKILL? */
			if (lv_char_level >=
			    classes[GET_CLASS(victim)].skill_min_level[idx] &&
			 classes[GET_CLASS(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* CAN THE RACE PRACTICE THIS SKILL? */
			if (lv_char_level >=
			    races[GET_RACE(victim)].skill_min_level[idx] &&
			    races[GET_CLASS(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* IF THIS IS AN NPC, AND ITS NOT PRACTICED, DON'T SHOW
			 * IT */
			if (IS_NPC(victim)) {
				lv_can_we_practice = FALSE;
			}

			/* IF WE ALREADY HAVE SOME PROFICIENCY, ALLOW IT */
			if (GET_SKILL(victim, idx))
				lv_can_we_practice = TRUE;

			/* LOOK FOR THIS SKILL IN THE LOOKUP TABLE */
			lv_skill_lookup = -1;
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == idx) {
					break;
				}
			}

			if (lv_can_we_practice == TRUE) {

				bzero(buf, sizeof(buf));
				bzero(buf3, sizeof(buf));

				/* Check for SKILL or SPELL */
				if (skill_numbers[lv_skill_lookup] == 0)
					sprintf(buf3, "&w%-20s", spell_names[idx - 1]);
				else
					sprintf(buf3, "&c%-20s", skill_names[lv_skill_lookup]);
				buf3[20] = 0;

				/* new test view */

				sprintf(buf, "&W%3d) %s &W[", idx, buf3);
				lv_row = GET_SKILL(victim, idx);
				for (kdx = 0; kdx < 21; kdx++) {
					if (kdx % 7 == 0)
						if ((kdx * 5) < lv_row) {
							if ((kdx / 7) == 0)
								strcat(buf, "&R");
							if ((kdx / 7) == 1)
								strcat(buf, "&Y");
							if ((kdx / 7) == 2)
								strcat(buf, "&G");
							lv_color = 1;
						}
					if (((kdx * 5) > (lv_row + 1)) && lv_color) {
						strcat(buf, "&K");
						lv_color = 0;
					}
					strcat(buf, "=");
				}
				sprintf(buf2, "&W] &R%3d%%&n\r\n", lv_row);
				strcat(buf, buf2);
				strcat(fbuf, buf);
				bzero(buf2, sizeof(buf2));
				jdx++;
				if (jdx % 5 == 4)
					strcat(fbuf, "\r\n");
			}
		}		/* END OF idx loop */
		if (jdx > 0)
			strcat(fbuf, "\r\n");
		else
			strcat(fbuf, "    none\r\n");

		bzero(buf, sizeof(buf));

		if (ch->desc) {
			page_string(ch->desc, fbuf, 1);
		}

		/* END OF NEW GRAPHICS */
	}
	else {
		/* ****** */
		/* SPELLS */
		/* ****** */
		send_to_char("\r\nYou can practice any of the following spells:\r\n", ch);

		/*
		 send_to_char("  No_ SPELL_____________    No_ SPELL_____________    No_ SPELL_____________\r\n", ch);
		
		 */
		jdx = 0;
		lv_row = 0;
		bzero(buf2, sizeof(buf2));
		for (idx = 1; idx < MAX_SKILLS; idx++) {
			/* HAVE WE EXCEEDED VALID SKILLS? */
			if (spell_names[idx - 1][0] == '\n') {
				idx = MAX_SKILLS;
				continue;
			}

			lv_can_we_practice = FALSE;

			/* CAN THEY PRACTICE THIS SPELL BY CLASS? */
			if (lv_char_level >=
			    classes[GET_CLASS(victim)].skill_min_level[idx] &&
			 classes[GET_CLASS(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* CAN THEY PRACTICE THIS SPELL BY RACE? */
			if (lv_char_level >=
			    races[GET_RACE(victim)].skill_min_level[idx] &&
			    races[GET_RACE(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* IF THIS IS AN NPC, AND ITS NOT PRACTICED, DON'T SHOW
			 * IT */
			if (IS_NPC(victim)) {
				lv_can_we_practice = FALSE;
			}

			/* IF WE ALREADY HAVE SOME PROFICIENCY, ALLOW IT */
			if (GET_SKILL(victim, idx))
				lv_can_we_practice = TRUE;

			/* LOOK FOR SKILL IN LOOKUP TABLE, IF THERE DON'T OFFER
			 * IT */
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == (int) idx) {
					lv_can_we_practice = FALSE;
					break;
				}
			}

			if (lv_can_we_practice == TRUE) {
				bzero(buf, sizeof(buf));
				bzero(buf3, sizeof(buf));
				strcpy(buf3, spell_names[idx - 1]);
				buf3[16] = 0;
				sprintf(buf, "%2d. %s(%2d%%)                       ",
					idx,
					buf3,
					GET_SKILL(victim, idx));
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
		if (jdx > 0) {
			strcat(buf2, "\r\n");
			send_to_char(buf2, ch);
		}
		else {
			send_to_char("    none\r\n", ch);
		}


		/* ****** */
		/* SKILLS */
		/* ****** */
		send_to_char("\r\nYou can practice any of the following skills:\r\n", ch);
		jdx = 0;
		lv_row = 0;
		bzero(buf2, sizeof(buf2));
		for (idx = 1; idx < MAX_SKILLS; idx++) {
			/* HAVE WE EXCEEDED VALID SKILLS? */
			if (spell_names[idx - 1][0] == '\n') {
				idx = MAX_SKILLS;
				continue;
			}

			lv_can_we_practice = FALSE;

			/* CAN THE CLASS PRACTICE THIS SKILL? */
			if (lv_char_level >=
			    classes[GET_CLASS(victim)].skill_min_level[idx] &&
			 classes[GET_CLASS(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* CAN THE RACE PRACTICE THIS SKILL? */
			if (lv_char_level >=
			    races[GET_RACE(victim)].skill_min_level[idx] &&
			    races[GET_CLASS(victim)].skill_min_level[idx] != 0)
				lv_can_we_practice = TRUE;

			/* IF THIS IS AN NPC, AND ITS NOT PRACTICED, DON'T SHOW
			 * IT */
			if (IS_NPC(victim)) {
				lv_can_we_practice = FALSE;
			}

			/* IF WE ALREADY HAVE SOME PROFICIENCY, ALLOW IT */
			if (GET_SKILL(victim, idx))
				lv_can_we_practice = TRUE;

			/* LOOK FOR THIS SKILL IN THE LOOKUP TABLE */
			lv_skill_lookup = -1;
			for (lv_skill_lookup = 0;
			     skill_numbers[lv_skill_lookup] != 0; lv_skill_lookup++) {
				if (skill_numbers[lv_skill_lookup] == idx) {
					break;
				}
			}

			if (lv_can_we_practice == TRUE) {

				bzero(buf, sizeof(buf));
				bzero(buf3, sizeof(buf));

				/* IF WE DIDN'T FIND THE SKILL, DON'T OFFER IT */
				if (skill_numbers[lv_skill_lookup] == 0) {
					continue;
				}

				strcpy(buf3, skill_names[lv_skill_lookup]);
				buf3[13] = 0;
				sprintf(buf, "  %2d. %s(%2d%%)                     ",
					idx,
					buf3,
					GET_SKILL(victim, idx));
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
		if (jdx > 0) {
			strcat(buf2, "\r\n");
			send_to_char(buf2, ch);
		}
		else {
			send_to_char("    none\r\n", ch);
		}

		//bzero(buf, sizeof(buf));
		//sprintf(buf, "\r\nYou have %d practice sessions.\r\n",
			  //victim->specials.spells_to_learn);
		//send_to_char(buf, ch);
	}			/* END OF OLD GRAPHICS */



	return;

}				/* END OF pr1200_do_the_guild_no_arg() */
/* NOTE:  If we are with a guild master, the mobs function will
 *        execute the pr1400_do_guild() function.  Otherwise, ot2000_do_practice
 *        calls this routine without an argument */
void pr1300_do_the_guild_with_arg(struct char_data * ch, struct char_data * victim, char *arg, int cmd)
{

	int lv_what_to_practice, lv_cost, slevel, plevel, lv_vict_level;
	char buf[MAX_STRING_LENGTH];

	lv_vict_level = GET_LEVEL(victim);

	for (; (arg && *arg == ' '); arg++);

	/* FIND WHICH SPELL THEY WANT TO PRACTICE */
	/* IS ARGUMENT NUMERIC? */
	lv_what_to_practice = 0;
	if (is_number(arg)) {
		lv_what_to_practice = atoi(arg);
		if (lv_what_to_practice < 0 || lv_what_to_practice > MAX_SKILLS ||
		!(strncmp(spell_names[lv_what_to_practice], "undefined", 9))) {
			send_to_char("Sorry, but that is not a valid spell/skill.\r\n",
				     ch);
			return;
		}
	}
	else {
		/* SEE IF WE CAN FIND IT BY NAME */
		lv_what_to_practice =
			old_search_block(arg, 0, strlen(arg), skill_names, FALSE);

		if (lv_what_to_practice > 0) {
			/* WE FOUND IT IN THE SKILL LIST SO CONVERT TO REAL
			 * NUMBER */
			lv_what_to_practice = skill_numbers[lv_what_to_practice - 1];
		}
		else {
			/* IT WASN'T A SKILL SO SEARCH FOR A SPELL */
			lv_what_to_practice =
				old_search_block(arg, 0, strlen(arg), spell_names, FALSE);
		}
	}

	/* ARE WE ALREADY LEARNED IN THIS AREA? */
	if (GET_SKILL(ch, lv_what_to_practice) >= 99) {
		send_to_char("Your guildmaster tells you that your expert in that area.\r\n", ch);
		return;
	}

	/* IS IT A VALID SKILL/SPELL? */
	if (lv_what_to_practice < 0) {
		send_to_char("You do not know a skill/spell like that.\r\n", ch);
		return;
	}

	/* ARE WE PREVENTED FROM PRACTICING THIS SPELL/SKILL? */
	if (lv_vict_level <
	    classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice] &&
	    lv_vict_level <
	    races[GET_RACE(ch)].skill_min_level[lv_what_to_practice] &&
	    GET_SKILL(ch, lv_what_to_practice) == 0) {
		bzero(buf, sizeof(buf));
		if (classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice] >
		    races[GET_RACE(ch)].skill_min_level[lv_what_to_practice]) {
			sprintf(buf, "You need to be level %d to practice %s.\r\n",
				races[GET_RACE(ch)].skill_min_level[lv_what_to_practice],
				spell_names[lv_what_to_practice - 1]);
		}
		else {
			sprintf(buf, "You need to be level %d to practice %s.\r\n",
				classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice],
				spell_names[lv_what_to_practice - 1]);
		}
		send_to_char(buf, ch);
		return;
	}

	/* IS THERE A RACE RESTRICTION? */
	if (GET_SKILL(ch, lv_what_to_practice) >=
	    races[GET_RACE(ch)].skill_max[lv_what_to_practice]) {
		send_to_char("Your guildmaster apologizes but tells you there are racial limits.\r\n", ch);
		return;
	}

	/* IS THERE A CLASS RESTRICTION? */
	if (GET_SKILL(ch, lv_what_to_practice) >=
	    classes[GET_CLASS(ch)].skill_max[lv_what_to_practice]) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Your guildmaster apologizes but tells you there are class limits.\r\n");
		send_to_char(buf, ch);
		return;
	}
	if (classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice] > races[GET_RACE(ch)].skill_min_level[lv_what_to_practice])
		slevel = races[GET_RACE(ch)].skill_min_level[lv_what_to_practice];
	else
		slevel = classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice];

	plevel = ch->skills[lv_what_to_practice].learned;

	if (!plevel && (slevel == IMO_LEV || slevel == PK_LEV) &&
	    (ch->points.fspells > 49)) {
		send_to_char("You can only learn 50 spells from other classes.\r\n", ch);
		return;
	};

	lv_cost = ((slevel * slevel * slevel * slevel) / 175 + 1) * (plevel + 1);

	if (lv_what_to_practice == SPELL_BEAST_TRANSFORM && (GET_CLASS(ch) != CLASS_RANGER && GET_CLASS(ch) != CLASS_DRUID)) {
		send_to_char("Your guildmaster apologizes, but only Rangers and Druids may practice that spell.\r\n", ch);
		return;
	}
	if ((lv_what_to_practice == SPELL_MORDEN_SWORD) && (GET_CLASS(ch) != CLASS_MAGIC_USER && GET_CLASS(ch) != CLASS_ELDRITCHKNIGHT)) {
		send_to_char("Your guildmaster apologizes, but only Mages and Eldritch Knights may practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice == SKILL_ENVENOM_WEAPON && GET_RACE(ch) != RACE_THRI_KREEN) {
		send_to_char("Your guildmaster apologizes, but only Thri-Kreen can learn that skill.\r\n", ch);
		return;
	}
	if (lv_what_to_practice == SPELL_UNDETECT_INVIS ||
		lv_what_to_practice == SPELL_SPIRIT_HAMMER ||
		lv_what_to_practice == SPELL_DREAMSIGHT ||
		lv_what_to_practice == SPELL_ANIMATE_DEAD ||
		lv_what_to_practice == SPELL_IRRESISTABLE_DANCE ||
		lv_what_to_practice == SPELL_WIZARD_LOCK ||
		lv_what_to_practice == SPELL_CLAWED_HANDS ||
		lv_what_to_practice == SPELL_FLYING_DISK ||
		lv_what_to_practice == SKILL_ASSESS ||
		lv_what_to_practice == SKILL_SWITCH_OPPONENT ||
		lv_what_to_practice == SKILL_SWITCH ||
		lv_what_to_practice == SPELL_FEAR ||
		lv_what_to_practice == SPELL_DIN_MAK ||
		lv_what_to_practice == SKILL_TSUGIASHI ||
		lv_what_to_practice == SKILL_BERSERK ||
		lv_what_to_practice == SKILL_PARRY ||
		lv_what_to_practice == SPELL_TREEWALK ||
		lv_what_to_practice == SPELL_TRANSFORM_CHIMERA ||
		lv_what_to_practice == SKILL_TRACK ||
	    lv_what_to_practice == SPELL_TRANSFORM_COW) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice >= SPELL_DEATHSHADOW && lv_what_to_practice <= SPELL_ENTANGLE) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice >= SPELL_CALM && lv_what_to_practice <= SPELL_WEB) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice > SPELL_ANIMAL_SUMMONING && lv_what_to_practice <= SPELL_EARTHMAW) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}	
	if (lv_what_to_practice > SPELL_FAERIE_FIRE && lv_what_to_practice < SPELL_METEOR_SWARM) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}	
	if (lv_what_to_practice >= SPELL_TSUNAMI && lv_what_to_practice < SKILL_AIRWALK) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice >= SPELL_LIFETAP && lv_what_to_practice <= SPELL_ENCHANT_ARMOR) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell.\r\n", ch);
		return;
	}
	if (lv_what_to_practice >= SPELL_SANCTUARY_MINOR && lv_what_to_practice < SPELL_GUSHER) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell yet.\r\n", ch);
		return;
	}
	if (lv_what_to_practice >= SPELL_TRANSFORM_MANTICORE && lv_what_to_practice <= SPELL_SLEEP_IMMUNITY) {
		send_to_char("Your guildmaster apologizes but you cannot practice that spell yet.\r\n", ch);
		return;
	}	
	if (plevel > 50)
		lv_cost += (plevel - 50) * 1000000;

	if (GET_AVAILABLE_EXP(ch) < lv_cost) {
		sprintf(buf, "You need %d experience to practice that.\r\n", lv_cost);
		send_to_char(buf, ch);
		return;
	}

	if ((slevel == IMO_LEV || slevel == PK_LEV) && !plevel)
		ch->points.fspells++;

	ch->skills[lv_what_to_practice].learned += 1;

	/* MAX PROFICIENCY? */
	if (GET_SKILL(ch, lv_what_to_practice) > 98) {
		GET_SKILL(ch, lv_what_to_practice) = 99;
		send_to_char("You are now learned in this area.\r\n", ch);
	}
	/* MAX RACIAL LIMITS */
	else if (GET_SKILL(ch, lv_what_to_practice) >
		 races[GET_RACE(ch)].skill_max[lv_what_to_practice]) {
		GET_SKILL(ch, lv_what_to_practice) =
			races[GET_RACE(ch)].skill_max[lv_what_to_practice];
		send_to_char("You are now learned in this area.\r\n", ch);
	}
	/* MAX CLASS LIMITS */
	else if (GET_SKILL(ch, lv_what_to_practice) >
		 classes[GET_CLASS(ch)].skill_max[lv_what_to_practice]) {
		GET_SKILL(ch, lv_what_to_practice) =
			classes[GET_CLASS(ch)].skill_max[lv_what_to_practice];
		send_to_char("You are now learned in this area.\r\n", ch);
	}
	else {
		co2900_send_to_char(ch, "You now have a %d" "%% proficiency in that area.\r\n",
				    ch->skills[lv_what_to_practice].learned);
	}

	if (GET_LEVEL(ch) < IMO_LEV)
		GET_USEDEXP(ch) += lv_cost;
	else
		GET_EXP(ch) -= lv_cost;

	bzero(buf, sizeof(buf));
	sprintf(buf, "You have %d experience left.\r\n",
		GET_AVAILABLE_EXP(ch));
	send_to_char(buf, ch);

	return;

}				/* END OF pr1200_do_the_guild_no_arg() */

int pr1400_do_guild(struct char_data * ch, int cmd, char *arg)
{



	if ((cmd != CMD_PRACTICE))
		return (FALSE);

	/* MAKE SURE PRACTICE SESSIONS ARE POSITIVE */
	if (ch->specials.spells_to_learn < 1)
		ch->specials.spells_to_learn = 0;

	for (; (arg && *arg == ' '); arg++);
	if (!arg || !*arg) {
		pr1200_do_the_guild_no_arg(ch, ch, arg, cmd);
	}
	else {
		pr1300_do_the_guild_with_arg(ch, ch, arg, cmd);

	}

	return (TRUE);

}				/* END OF pr1400_do_guild() */


int pr1500_do_arena(struct char_data * ch, int cmd, char *arg)
{

	int lv_dice, idx;



	lv_dice = number(1, 100);
	switch (world[ch->in_room].number) {
	case 2691:{
			if (lv_dice == 50) {
				idx = GET_MAX_HIT(ch) >> 2;
				if (GET_HIT(ch) + idx < li1500_hit_limit(ch)) {
					GET_HIT(ch) += idx;
				}
			}
			if (lv_dice == 51) {
				idx = GET_MAX_HIT(ch) >> 2;
				if (GET_HIT(ch) - idx > 0) {
					GET_HIT(ch) -= idx;
				}
			}
			return (FALSE);
		}
	case 2692:{
			return (FALSE);
		}
	case 2693:{
			if (lv_dice == 50) {
				idx = GET_MAX_MANA(ch) >> 2;
				if (GET_MANA(ch) + idx < li1300_mana_limit(ch)) {
					GET_MANA(ch) += idx;
				}
			}
			if (lv_dice == 51) {
				idx = GET_MAX_MANA(ch) >> 2;
				if (GET_MANA(ch) - idx > 0) {
					GET_MANA(ch) -= idx;
				}
			}
			return (FALSE);
		}
	case 2694:{
			return (FALSE);
		}
	case 2695:{
			if (lv_dice == 50) {
				idx = GET_MAX_MOVE(ch) >> 2;
				if (GET_MOVE(ch) + idx < li1600_move_limit(ch)) {
					GET_MOVE(ch) += idx;
				}
			}
			if (lv_dice == 51) {
				idx = GET_MAX_MOVE(ch) >> 2;
				if (GET_MOVE(ch) - idx > 0) {
					GET_MOVE(ch) -= idx;
				}
			}
			return (FALSE);
		}
	case 2696:{
			return (FALSE);
		}
	case 2697:{
			/* REMOVE SPELLS */
			for (idx = 0; idx < MAX_AFFECT; idx++) {
				if (ch->affected) {
					ha1325_affect_remove(ch, ch->affected, REMOVE_FOLLOWER);
				}
			}
			return (FALSE);
		}
	case 2698:{
			return (FALSE);
		}
	case 2699:{
			return (FALSE);
		}
	}

	return (FALSE);

}				/* END OF pr1500_do_arena() */


int pr1600_do_dump(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *k;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_char;
	int value = 0;

	char *ha1100_fname(char *namelist);



	for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
		sprintf(buf, "The %s vanishes in a puff of smoke.\n\r", ha1100_fname(k->name));
		for (tmp_char = world[ch->in_room].people; tmp_char;
		     tmp_char = tmp_char->next_in_room)
			if (CAN_SEE_OBJ(tmp_char, k))
				send_to_char(buf, tmp_char);
		ha2700_extract_obj(k);
	}

	if (cmd != CMD_DROP)
		return (FALSE);

	do_drop(ch, arg, cmd);
	value = 0;

	for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
		sprintf(buf, "The %s vanishes in a puff of smoke.\n\r", ha1100_fname(k->name));
		for (tmp_char = world[ch->in_room].people; tmp_char;
		     tmp_char = tmp_char->next_in_room)
			if (CAN_SEE_OBJ(tmp_char, k))
				send_to_char(buf, tmp_char);
		value += MAXV(1, MINV(50, k->obj_flags.cost / 10));

		ha2700_extract_obj(k);
	}

	if (value) {
		act("You are awarded for outstanding performance.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n has been awarded for being a good citizen.", TRUE, ch, 0, 0, TO_ROOM);

		if (GET_LEVEL(ch) < 3)
			li2200_gain_exp(ch, value);
		else
			GET_GOLD(ch) += value;
	}
	return (TRUE);
}				/* END OF pr1600_do_dump() */


int pr1700_mayor(struct char_data * ch, int cmd, char *arg)
{

	static char close_path[] =
	"T2322332215S.";

	static char open_path[] =
	"W43ab001100db333O111c000100ef00O22221222111O33330gR.";

	static char *path;
	static int index;
	static bool move = FALSE;



	if (!move) {
		if (time_info.hours == 6) {
			move = TRUE;
			path = open_path;
			index = 0;
		}
		else if (time_info.hours == 20) {
			move = TRUE;
			path = close_path;
			index = 0;
		}
	}

	if (cmd || !move || (GET_POS(ch) < POSITION_SLEEPING) ||
	    (GET_POS(ch) == POSITION_FIGHTING))
		return FALSE;

	switch (path[index]) {
	case '0':
	case '1':
	case '2':
	case '3':
		mo1500_do_move(ch, "", path[index] - '0' + 1);
		break;

	case 'W':
		GET_POS(ch) = POSITION_STANDING;
		act("$n wakes up.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'S':
		GET_POS(ch) = POSITION_SLEEPING;
		act("$n goes to sleep.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'R':
		GET_POS(ch) = POSITION_SITTING;
		act("$n sits down on a barstool.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'T':
		GET_POS(ch) = POSITION_STANDING;
		act("$n gets up and claps his hands over face.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'a':
		act("$n growls, 'Aargh..., the light hurts my eyes!'", FALSE, ch, 0, 0, TO_ROOM);
		act("$n grumbles about the good weather.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'b':
		act("$n gets a pair of dark shades from his bag.", FALSE, ch, 0, 0, TO_ROOM);
		act("$n wears a pair of dark shades over his eyes.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'c':
		act("$n grabs a bleck and gives it to the wall.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'd':
		act("$n sighs longingly as he glances at the bar.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'e':
		act("$n tips his shades at the executioner.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'f':
		act("$n chuckles as he looks at the pathetic midgaard man.", FALSE, ch, 0, 0, TO_ROOM);;
		break;

	case 'g':
		act("$n says I'll have a beer!", FALSE, ch, 0, 0, TO_ROOM);
		act("$n buys a beer.", FALSE, ch, 0, 0, TO_ROOM);
		break;

	case 'O':
		mo2200_do_unlock(ch, "gate", 0);
		act("$n says, I do this for a living, can you believe it?", FALSE, ch, 0, 0, TO_ROOM);
		mo1700_do_open(ch, "gate", 0);
		break;

	case 'C':
		mo1800_do_close(ch, "gate", 0);
		act("$n says, I do this for a living, can you believe it?", FALSE, ch, 0, 0, TO_ROOM);
		mo2100_do_lock(ch, "gate", 0);
		break;

	case '.':
		move = FALSE;
		break;

	}

	index++;
	return FALSE;
}				/* END OF pr1700_mayor() */


/* ********************************************************************
 *  General special procedures for mobiles                            *
 ******************************************************************** */

void pr2100_npc_steal(struct char_data * ch, struct char_data * victim)
{

	int gold;



	if (IS_NPC(victim))
		return;
	if (GET_LEVEL(victim) > 20)
		return;

	if (AWAKE(victim) && (number(0, GET_LEVEL(ch)) == 0)) {
		act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, victim, TO_VICT);
		act("$n tries to steal gold from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
	}
	else {
		/* Steal some gold coins */
		gold = (int) ((GET_GOLD(victim) * number(1, 10)) / 100);
		if (gold > 0) {
			GET_GOLD(ch) += gold;
			GET_GOLD(victim) -= gold;
		}
	}
}				/* END OF pr2100_npc_stea() */


int pr2200_snake(struct char_data * ch, int cmd, char *arg)
{



	if (cmd)
		return FALSE;

	if (GET_POS(ch) != POSITION_FIGHTING)
		return FALSE;

	if (ch->specials.fighting &&
	    (ch->specials.fighting->in_room == ch->in_room) &&
	    (number(0, 32 - GET_LEVEL(ch)) == 0)) {
		act("$n bites $N!", 1, ch, 0, ch->specials.fighting, TO_NOTVICT);
		act("$n bites you!", 1, ch, 0, ch->specials.fighting, TO_VICT);
		cast_poison(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL,
			    ch->specials.fighting, 0);
		return TRUE;
	}
	return FALSE;
}				/* END OF pr2200_snake() */


int pr2300_thief(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *cons;



	if (cmd)
		return FALSE;

	if (GET_POS(ch) != POSITION_STANDING)
		return FALSE;

	for (cons = world[ch->in_room].people; cons; cons = cons->next_in_room)
		if ((!IS_NPC(cons)) && (GET_LEVEL(cons) < IMO_LEV) && (number(1, 5) == 1))
			pr2100_npc_steal(ch, cons);

	return TRUE;
}				/* END OF pr2300_thief() */


/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */

int pr2600_high_priest(struct char_data * ch, int cmd, char *arg)
{

	const char *options[] = {	/* text */
		"Strength      ",
		"Intelligence  ",
		"Wisdom        ",
		"Dexterity     ",
		"Constitution  ",
		"Charisma      ",
		"Hit points (1)",
		"Mana points(1)",
		"Move points(1)",
		"Heal hits",
		"Heal Mana",
		"Heal Move",
		"Heal All ",
		"Cure (blind, poison)",
		"\n"
	};

	unsigned long cost[15], lv_A, lv_B, lv_C, lv_max_stat, number, i;

	char buf[MAX_STRING_LENGTH];	/* not input length dependant */
	char temp_str[MAX_STRING_LENGTH];
	struct char_data *temp_char, *save_char;
	static struct char_data *priest;
	int idx;



	save_char = ch;

	priest = 0;
	for (temp_char = world[ch->in_room].people;
	     (temp_char) && (!priest); temp_char = temp_char->next_in_room) {
		if (IS_MOB(temp_char))
			if (mob_index[temp_char->nr].func == pr2600_high_priest)
				priest = temp_char;
	}

	/* DID WE FIND THE PRIEST? */
	/* THIS SHOULDN'T HAPPEN AS WE ARE HERE BECAUSE OF THE FUNCTION */
	/* ON THE HIGHPRIEST CHAR STRUCTURE */
	if (priest == 0) {
		send_to_char("Couldn't locate the priest.\r\n", ch);
		return (FALSE);
	}

	/* IF THE HIGHPRIEST IS ASLEEP */
	if (GET_POS(priest) == POSITION_SLEEPING) {
		return (FALSE);
	}

	if (!ch)
		return (FALSE);

	/* THE FOLLOWING IS FOR HELL NIGHT.  IF USER ENTERS HEAL */
	/* AND AUTO LEVEL IS TURNED ON USER CAN GET FREE HEALS!! */
	if (cmd == CMD_HEAL) {
		if (gv_auto_level > 1) {
			if (!(IS_NPC(ch))) {
				if (ha1375_affected_by_spell(ch, SPELL_BLINDNESS))
					spell_cure_blind(40, priest, ch, 0);
				spell_remove_poison(40, priest, ch, 0);
				if ((li1500_hit_limit(ch) > ch->points.hit) ||
				  (li1300_mana_limit(ch) > ch->points.mana) ||
				  (li1600_move_limit(ch) > ch->points.move)) {
					ch->points.hit = li1500_hit_limit(ch);
					ch->points.mana = li1300_mana_limit(ch);
					ch->points.move = li1600_move_limit(ch);
					send_to_char("You have been healed.\r\n", ch);
				}
				else {
					send_to_char("You're in perfect health already.\r\n", ch);
				}
			}
		}
		return (TRUE);
	}

	if (cmd != CMD_BUY)
		return (FALSE);

	if (IS_NPC(ch)) {
		send_to_char("Mobs can't buy stats...\r\n", ch);
		return (TRUE);
	}

	/*
	 bzero(temp_str, sizeof(temp_str));
	 sprintf(temp_str,"A001 high_priest routine. char %s %d %d\r\n",
					 GET_NAME(save_char), ch, save_char);
	 main_log (temp_str);
	 */

	if (ch != save_char) {
		bzero(temp_str, sizeof(temp_str));
		sprintf(temp_str, "A002 ERROR ch changed char: %s pointer in high_priest routine\r\n",
			GET_NAME(save_char));

		main_log(temp_str);
		spec_log(temp_str, ERROR_LOG);
		return (TRUE);
	}

	if (ch != save_char) {
		main_log("A003 ERROR ch changed in high_priest routine\r\n");
		spec_log("A003 ERROR ch changed in high_priest routine\r\n", ERROR_LOG);
		return (TRUE);
	}

	/* STRENGTH */
	lv_A = GET_REAL_STR(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[0] = lv_A * (lv_B + lv_C);

	/* INTELLIGENCE */
	lv_A = GET_REAL_INT(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[1] = lv_A * (lv_B + lv_C);

	/* WISDOM */
	lv_A = GET_REAL_WIS(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[2] = lv_A * (lv_B + lv_C);

	/* DEXTERITY */
	lv_A = GET_REAL_DEX(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[3] = lv_A * (lv_B + lv_C);

	/* CONSTITUTION */
	lv_A = GET_REAL_CON(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[4] = lv_A * (lv_B + lv_C);

	/* CHARISMA */
	lv_A = GET_REAL_CHA(ch);
	lv_B = GET_STAT_COUNT(ch) * GET_STAT_COUNT(ch) * 800;
	lv_C = lv_A * 800;
	cost[5] = lv_A * (lv_B + lv_C);

	/* HIT POINTS */
	if (GET_HIT_LIMIT(ch) > GET_HIT(ch))
		lv_A = GET_HIT_LIMIT(ch);
	else
		lv_A = GET_HIT(ch);
	if (GET_LEVEL(ch) == PK_LEV)
		lv_A += (PK_LEV - GET_LEVEL(ch)) * 30;
	else
		lv_A += (IMO_LEV - GET_LEVEL(ch)) * 30;
	cost[6] = pr2650_stat_cost(lv_A);

	/* MANA POINTS */
	if (GET_MANA_LIMIT(ch) > GET_MANA(ch))
		lv_A = GET_MANA_LIMIT(ch);
	else
		lv_A = GET_MANA(ch);
	if (GET_LEVEL(ch) == PK_LEV)
		lv_A += (PK_LEV - GET_LEVEL(ch)) * 30;
	else
		lv_A += (IMO_LEV - GET_LEVEL(ch)) * 30;
	cost[7] = pr2650_stat_cost(lv_A);

	/* MOVE POINTS */
	if (GET_MOVE_LIMIT(ch) > GET_MOVE(ch))
		lv_A = GET_MOVE_LIMIT(ch);
	else
		lv_A = GET_MOVE(ch);
	if (GET_LEVEL(ch) == PK_LEV)
		lv_A += (PK_LEV - GET_LEVEL(ch)) * 30;
	else
		lv_A += (IMO_LEV - GET_LEVEL(ch)) * 30;
	cost[8] = pr2650_stat_cost(lv_A);

	/* HEAL HITS */
	cost[9] = (li1500_hit_limit(ch) + 500) * li1500_hit_limit(ch) / 25;
	if (GET_HIT(ch) >= li1500_hit_limit(ch)) {
		cost[9] = 0;
	}

	/* HEAL MANA */
	cost[10] = (li1300_mana_limit(ch) + 500) * li1300_mana_limit(ch) / 25;
	if (GET_MANA(ch) >= GET_SPECIAL_MANA(ch)) {
		cost[10] = 0;
	}

	/* HEAL MOVE */
	cost[11] = (li1600_move_limit(ch) + 500) * li1600_move_limit(ch) / 25;
	if (GET_MOVE(ch) >= li1600_move_limit(ch)) {
		cost[11] = 0;
	}

	/* HEAL ALL HIT MANA MOVE */
	cost[12] = ((li1500_hit_limit(ch) + 500) * li1500_hit_limit(ch) +
		    (li1300_mana_limit(ch) + 500) * li1300_mana_limit(ch) +
		  (li1600_move_limit(ch) + 500) * li1600_move_limit(ch)) / 25;
	if ((GET_HIT(ch) >= li1500_hit_limit(ch)) &&
	    (GET_MANA(ch) >= GET_SPECIAL_MANA(ch)) &&
	    (GET_MOVE(ch) >= li1600_move_limit(ch))) {
		cost[12] = 0;
	}

	/* CURE */
	cost[13] = 100;

	number = -1;
	if (arg)
		for (; *arg == ' '; arg++);
	if (!arg || !*arg) {
		send_to_char("You can buy any of the following:\n\r", ch);
		for (i = 0; *options[i] != '\n'; i++) {
			send_to_char((char *) options[i], ch);
			bzero(buf, sizeof(buf));
			if (i < 9)
				sprintf(buf, " &C%9ld&n exp.\r\n", cost[i]);
			else
				sprintf(buf, " &G%9ld&n coins.\r\n", cost[i]);
			send_to_char(buf, ch);
		}
		return (TRUE);
	}

	number = old_search_block(arg, 0, strlen(arg), options, -1);
	if (number < 1) {
		send_to_char("Sorry, but I'm not selling that!\r\n", ch);
		return (TRUE);
	}
	number--;

	if (ch != save_char) {
		main_log("A004 ch changed in high_priest routine\r\n");
		spec_log("A004 ch changed in high_priest routine\r\n", ERROR_LOG);
		return (TRUE);
	}

	if (number < 9) {
		if (GET_AVAILABLE_EXP(ch) < cost[number]) {
			send_to_char("Come back when you are more experienced.\r\n", ch);
			return (TRUE);
		}
	}
	else {
		if (GET_GOLD(ch) < cost[number]) {
			send_to_char("Come back when you can afford it.\r\n", ch);
			return (TRUE);
		}
	}

	if (number < 9 && GET_LEVEL(ch) < 11) {
		send_to_char("Come back when you're a little bit wiser!\r\n",
			     ch);
		return (TRUE);
	}

	bzero(buf, sizeof(buf));
	switch (number) {
	case 0:
		lv_max_stat = races[GET_RACE(ch)].max_str;
		if (ch->abilities.str >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.str++;
		ch->tmpabilities.str++;
		sprintf(buf, "Gained 1 STR for %ld experience. Base STR now: %d\r\n",
			cost[number], GET_STR(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 1:
		lv_max_stat = races[GET_RACE(ch)].max_int;
		if (ch->abilities.intel >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.intel++;
		ch->tmpabilities.intel++;
		sprintf(buf, "Gained 1 INT for %ld experience. Base INT now: %d\r\n",
			cost[number], GET_INT(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 2:
		lv_max_stat = races[GET_RACE(ch)].max_wis;
		if (ch->abilities.wis >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.wis++;
		ch->tmpabilities.wis++;
		sprintf(buf, "Gained 1 WIS for %ld experience. Base WIS now: %d\r\n",
			cost[number], GET_WIS(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 3:
		lv_max_stat = races[GET_RACE(ch)].max_dex;
		if (ch->abilities.dex >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.dex++;
		ch->tmpabilities.dex++;
		sprintf(buf, "Gained 1 DEX for %ld experience. Base DEX now: %d\r\n",
			cost[number], GET_DEX(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 4:
		lv_max_stat = races[GET_RACE(ch)].max_con;
		if (ch->abilities.con >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.con++;
		ch->tmpabilities.con++;
		sprintf(buf, "Gained 1 CON for %ld experience. Base CON now: %d\r\n",
			cost[number], GET_CON(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 5:
		lv_max_stat = races[GET_RACE(ch)].max_cha;
		if (ch->abilities.cha >= lv_max_stat) {
			send_to_char("The priest tells you that to advance in this area you must look elsewhere.\r\n", ch);
			return (TRUE);
		}
		ch->abilities.cha++;
		ch->tmpabilities.cha++;
		sprintf(buf, "Gained 1 CHA purchased for %ld experience. Base CHA now: %d\r\n",
			cost[number], GET_CHA(ch));
		GET_STAT_COUNT(ch)++;
		break;
	case 6:
		/* IF WEARING SOMETHING TELL EM TOUGH LUCK */
		for (idx = 0; idx < MAX_WEAR; idx++) {
			if (ch->equipment[idx]) {
				send_to_char("The priest tells you that you have to remove all equipment to buy hits.\r\n", ch);
				return (TRUE);
			}
		}
		if (ch->points.max_hit >= races[GET_RACE(ch)].max_hit) {
			send_to_char("The priest tells you that you've reached a racial limit.\r\n", ch);
			return (TRUE);
		}
		ch->points.max_hit += 1;
		sprintf(buf, "Gained 1 hit for %ld experience. Hits now: %d\r\n",
			cost[number], li1500_hit_limit(ch));
		break;
	case 7:
		/* IF WEARING SOMETHING TELL EM TOUGH LUCK */
		if (IS_CASTED_ON(ch, SPELL_BIND_SOULS)) {
			send_to_char("The priest tells you that you can't buy mana while your soul is bound.\r\n", ch);
			return (TRUE);
		}
		for (idx = 0; idx < MAX_WEAR; idx++) {
			if (ch->equipment[idx]) {
				send_to_char("The priest tells you that you have to remove all equipment to buy mana.\r\n", ch);
				return (TRUE);
			}
		}
		if (ch->points.max_mana >= races[GET_RACE(ch)].max_mana) {
			send_to_char("The priest tells you that you've reached a racial limit.\r\n", ch);
			return (TRUE);
		}
		ch->points.max_mana += 1;
		sprintf(buf, "gained 1 mana for %ld experience. Mana now: %d\r\n",
			cost[number], li1300_mana_limit(ch));
		break;
	case 8:
		/* IF WEARING SOMETHING TELL EM TOUGH LUCK */
		for (idx = 0; idx < MAX_WEAR; idx++) {
			if (ch->equipment[idx]) {
				send_to_char("The priest tells you that you have to remove all equipment to buy moves.\r\n", ch);
				return (TRUE);
			}
		}
		if (ch->points.max_move >= races[GET_RACE(ch)].max_move) {
			send_to_char("The priest tells you that you've reached a racial limit.\r\n", ch);
			return (TRUE);
		}
		ch->points.max_move += 1;
		sprintf(buf, "gained 1 move for %ld experience. Moves now: %d\r\n",
			cost[number], li1600_move_limit(ch));
		break;
	case 9:
		if (GET_HIT(ch) >= li1500_hit_limit(ch)) {
			send_to_char("You're already at max hits.\r\n", ch);
			return (TRUE);
		}
		GET_HIT(ch) = li1500_hit_limit(ch);
		sprintf(buf, "You've been restored to %d hits for %ld coins.\r\n",
			GET_HIT(ch), cost[number]);
		if (GET_DSTIME(ch)) {
			GET_HIT(ch) >>= 1;
			send_to_char("The shadow of death drains half your energy.\r\n", ch);
		}

		break;
	case 10:
		if (GET_MANA(ch) >= GET_SPECIAL_MANA(ch)) {
			send_to_char("You're already at max mana.\r\n", ch);
			return (TRUE);
		}
		GET_MANA(ch) = GET_SPECIAL_MANA(ch);
		sprintf(buf, "You've been restored to %d mana for %ld coins.\r\n",
			GET_MANA(ch), cost[number]);
		if (GET_DSTIME(ch)) {
			GET_MANA(ch) >>= 1;
			send_to_char("The shadow of death drains half your energy.\r\n", ch);
		}
		break;
	case 11:
		if (GET_MOVE(ch) == li1600_move_limit(ch)) {
			send_to_char("You're already at max move.\r\n", ch);
			return (TRUE);
		}
		GET_MOVE(ch) = li1600_move_limit(ch);
		sprintf(buf, "You've been restored to %d moves for %ld coins.\r\n",
			GET_MOVE(ch), cost[number]);
		if (GET_DSTIME(ch)) {
			GET_MOVE(ch) >>= 1;
			send_to_char("The shadow of death drains half your energy.\r\n", ch);
		}
		break;
	case 12:
		if ((GET_HIT(ch) == li1500_hit_limit(ch)) &&
		    (GET_MANA(ch) == GET_SPECIAL_MANA(ch)) &&
		    (GET_MOVE(ch) == li1600_move_limit(ch))) {
			send_to_char("Your hits, mana and move are at max already.\r\n", ch);
			return (TRUE);
		}
		GET_HIT(ch) = li1500_hit_limit(ch);
		GET_MANA(ch) = GET_SPECIAL_MANA(ch);
		GET_MOVE(ch) = li1600_move_limit(ch);

		sprintf(buf, "You have been restored to your full maximum stats for %ld coins.\r\n",
			cost[number]);
		if (GET_DSTIME(ch)) {
			GET_HIT(ch) >>= 1;
			GET_MANA(ch) >>= 1;
			GET_MOVE(ch) >>= 1;
			send_to_char("The shadow of death drains half your energy.\r\n", ch);
		}

		break;
	case 13:
		if (ha1375_affected_by_spell(ch, SPELL_BLINDNESS))
			spell_cure_blind(40, priest, ch, 0);
		spell_remove_poison(40, priest, ch, 0);
		sprintf(buf, "You've been cured of poison/blind.\r\n");
		break;

	default:
		send_to_char("Sorry kid, I'm not quite sure what you mean.\n\r", ch);
		return (TRUE);
		break;
	}			/* switch */

	/* SUBRTACT GOLD */
	send_to_char(buf, ch);
	if (number < 9)
		if (GET_LEVEL(ch) < IMO_LEV)
			GET_USEDEXP(ch) += cost[number];
		else
			GET_EXP(ch) -= cost[number];
	else
		GET_GOLD(ch) -= cost[number];

	return (TRUE);

}				/* END OF pr2600_high_priest() */

int pr2650_stat_cost(int lv_stat)
{
	int lv_cost, lv_temp, lv_remainder;

	int lv_value[] = {
		50000,		/* 0 */
		50000,		/* 1 */
		50000,		/* 2 */
		100000,		/* 3 */
		400000,		/* 4 */
		1200000,	/* 5 */
		3000000,	/* 6 */
		10000000,	/* 7 */
		50000000	/* 8 */
	};



	/* SAFETY AND MAX CHECK */
	if (lv_stat < 0 || lv_stat > 2249) {
		return (50000000);
	}
	if (lv_stat < 750) {
		return (50000);
	}

	lv_temp = lv_stat / 250;
	lv_remainder = lv_stat - (lv_temp * 250);
	lv_cost = lv_value[lv_temp] + (lv_value[lv_temp] / 250 * lv_remainder);
	return (lv_cost);

}				/* END OF pr2650_stat_cost() */


int pr2700_guild_guard(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* IF NOT A DIRECTION, GET OUTTA HERE */
	if (cmd < 1 || cmd > 6)
		return (FALSE);

	strcpy(buf, "The guard humiliates you, and blocks your way.\n\r");
	strcpy(buf2, "The guard humiliates $n, and blocks $s way.");

	/* MAGE's GUILD */
	if ((ch->in_room == db8000_real_room(3017)) &&
	    (cmd == CMD_SOUTH)) {
		if ((GET_LEVEL(ch) < IMO_LEV) &&
		    (GET_CLASS(ch) == CLASS_CLERIC ||
		     GET_CLASS(ch) == CLASS_THIEF ||
			 GET_CLASS(ch) == CLASS_MONK ||
			 GET_CLASS(ch) == CLASS_PALADIN ||
			 GET_CLASS(ch) == CLASS_PRIEST ||
		     GET_CLASS(ch) == CLASS_WARRIOR)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return (TRUE);
		}
	}

	/* CLERIC's GUILD */
	if ((ch->in_room == db8000_real_room(3004)) && (cmd == CMD_NORTH)) {
		if ((GET_LEVEL(ch) < IMO_LEV) &&
		    (GET_CLASS(ch) == CLASS_MAGIC_USER ||
		     GET_CLASS(ch) == CLASS_THIEF ||
			 GET_CLASS(ch) == CLASS_ELDRITCHKNIGHT ||
			 GET_CLASS(ch) == CLASS_MONK ||
		     GET_CLASS(ch) == CLASS_WARRIOR)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return (TRUE);
		}
	}

	/* THIEVE's GUILD */
	if ((ch->in_room == db8000_real_room(3027)) && (cmd == CMD_EAST)) {
		if ((GET_LEVEL(ch) < IMO_LEV) &&
		    (GET_CLASS(ch) == CLASS_MAGIC_USER ||
		     GET_CLASS(ch) == CLASS_CLERIC ||
		     GET_CLASS(ch) == CLASS_WARRIOR ||
		     GET_CLASS(ch) == CLASS_PRIEST ||
			 GET_CLASS(ch) == CLASS_ELDRITCHKNIGHT ||
			 GET_CLASS(ch) == CLASS_DRUID ||
		     GET_CLASS(ch) == CLASS_PALADIN)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
	}


	/* WARRIOR's GUILD */
	if ((ch->in_room == db8000_real_room(3021)) && (cmd == CMD_EAST)) {
		if ((GET_LEVEL(ch) < IMO_LEV) &&
		    (GET_CLASS(ch) == CLASS_MAGIC_USER ||
		     GET_CLASS(ch) == CLASS_CLERIC ||
		     GET_CLASS(ch) == CLASS_THIEF ||
			 GET_CLASS(ch) == CLASS_DRUID ||
			 GET_CLASS(ch) == CLASS_BARD)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
	}

	return (FALSE);

}				/* END OF pr2700_guild_guard() */
/* ROUTINE FOR CAMELOT HEALING MOB */
int pr2799_camelot(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict, *tmp_char;
	char buf[MAX_STRING_LENGTH];

	static int lv_number;

	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* 50% CHANCE ONLY */
	/* if (number(1, 2) != 1) { return(0); } */
	if (lv_number < 0 || lv_number > 50) {
		lv_number = 40;
	}

	lv_number++;

	/* Fix for invalid vict pointer */
	vict = ch;

	for (tmp_char = world[ch->in_room].people; tmp_char;
	     tmp_char = tmp_char->next_in_room) {

		/* PICK A MORTAL PC RANDOMLY */
		if ((ch != tmp_char) && (GET_LEVEL(tmp_char) < IMO_SPIRIT) &&
		    (IS_PC(tmp_char)))
			vict = tmp_char;
	}

	switch (lv_number) {
	case 1:
		sprintf(buf, "aid %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 2:
		sprintf(buf, "bless %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 3:
		sprintf(buf, "15 %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 4:
		sprintf(buf, "armor %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 5:
		sprintf(buf, "strength %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}			/* END OF switch() */

}				/* END OF pr2799_camelot() */


int pr2800_gothelred(struct char_data * ch, int cmd, char *arg)
{

	static int lv_number;



	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 2) != 1) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 50) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "Only cheaters end up here kid.", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "Ya learn your lesson yet kid?", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "Of all the cells, in all the world, ya had ta pick mine.", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "I heard they're sending you down the river my friend.", CMD_SAY);
		return (1);
	case 5:
		do_say(ch, "I have a cousin in Asgaard ya know.", CMD_SAY);
		return (1);
	case 6:
		do_say(ch, "Have you ever met Melvis Wesley?", CMD_SAY);
		return (1);
	case 7:
		do_say(ch, "Be careful of the rats by the way, they're in mating season.", CMD_SAY);
		return (1);
	case 8:
		do_say(ch, "Frankly my dear, i don't give a damn!", CMD_SAY);
		return (1);
	case 9:
		do_say(ch, "I think this could be the beginning of a very long and beautiful friendship.", CMD_SAY);
		return (1);
	case 10:
		do_say(ch, "Alas, poor Joric!", CMD_SAY);
		return (1);
	case 11:
		do_say(ch, "Are you saying that time is running out?", CMD_SAY);
		return (1);
	case 12:
		do_say(ch, "It's the greatest thing that's ever happened in our time!", CMD_SAY);
		return (1);
	case 13:
		do_say(ch, "I wouldn't stand there if i were you.", CMD_SAY);
		return (1);
	case 14:
		do_say(ch, "You wanna be careful when Snahgle gets here.", CMD_SAY);
		return (1);
	case 15:
		do_say(ch, "You'll be sorry...", CMD_SAY);
		return (1);
	case 16:
		do_say(ch, "Welcome to the cheap seats.", CMD_SAY);
		return (1);
	case 17:
		do_say(ch, "Spring is in the air.", CMD_SAY);
		return (1);
	case 18:
		do_say(ch, "Extra cheese?", CMD_SAY);
		return (1);
	case 19:
		do_say(ch, "Heheheh. Just you wait....", CMD_SAY);
		return (1);
	case 20:
		do_say(ch, "Guilty i tell you. Guilty!", CMD_SAY);
		return (1);
	case 21:
		do_say(ch, "I've got a brand new combine harvester!", CMD_SAY);
		return (1);
	case 22:
		do_say(ch, "So, what do ya think of my jail then?", CMD_SAY);
		return (1);
	case 23:
		do_say(ch, "Have you heard about what happened to Farmer Gamgee?", CMD_SAY);
		return (1);
	case 24:
		do_say(ch, "Laugh? My head nearly popped off.", CMD_SAY);
		return (1);
	case 25:
		do_say(ch, "Did you hear about ol' Gibbins McMunchley?", CMD_SAY);
		return (1);
	case 26:
		wi1000_do_emote(ch, "picks his nose.", CMD_EMOTE);
		return (1);
	case 27:
		wi1000_do_emote(ch, "pats his belly.", CMD_EMOTE);
		return (1);
	case 30:
		wi1000_do_emote(ch, "scratches his butt. Gross!", CMD_EMOTE);
		return (1);
	case 31:
		wi1000_do_emote(ch, "stuffs his two biggest fingers up his left nostril.", CMD_EMOTE);
		return (1);
	case 32:
		wi1000_do_emote(ch, "reaches his hand down the back of his trousers and fumbles around.", CMD_EMOTE);
		return (1);
	case 33:
		wi1000_do_emote(ch, "pulls a loaf of green bread from his trousers and noshes out.", CMD_EMOTE);
		return (1);
	case 34:
		wi1000_do_emote(ch, "looks at you and snickers softly.", CMD_EMOTE);
		return (1);
	case 35:
		wi1000_do_emote(ch, "spits through the bars onto the floor of your cell.", CMD_EMOTE);
		return (1);
	case 36:
		wi1000_do_emote(ch, "scratches his crotch and gets a gleam in his eye as he looks at you.", CMD_EMOTE);
		return (1);
	case 37:
		wi1000_do_emote(ch, "", CMD_EMOTE);
		return (1);
	case 38:
		wi1000_do_emote(ch, "", CMD_EMOTE);
		return (1);
	case 39:
		wi1000_do_emote(ch, "", CMD_EMOTE);
		return (1);
	case 40:
		wi1000_do_emote(ch, "", CMD_EMOTE);
		return (1);
	case 41:
		do_say(ch, "What ya do to end up here kid?", CMD_SAY);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}			/* END OF switch() */

}				/* END OF pr2800_gothelred() */
int pr2898_newbie_blesser(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict, *tmp_char;
	char buf[MAX_STRING_LENGTH];

	static int lv_number;

	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* 50% CHANCE ONLY */
	if (number(1, 2) != 1) {
		return (0);
	}



	if (lv_number < 0 || lv_number > 20) {
		lv_number = 0;
	}

	lv_number++;

	/* Fix for invalid vict pointer */
	vict = ch;

	for (tmp_char = world[ch->in_room].people; tmp_char;
	     tmp_char = tmp_char->next_in_room) {

		/* PICK A MORTAL PC RANDOMLY */
		if ((ch != tmp_char) && (GET_LEVEL(tmp_char) < 5) &&
		    (IS_PC(tmp_char)))
			vict = tmp_char;
	}

	switch (lv_number) {
	case 1:
		sprintf(buf, "aid %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 2:
		sprintf(buf, "strength %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 3:
		sprintf(buf, "armor %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	case 4:
		sprintf(buf, "bless %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 5:
		sprintf(buf, "15 %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 6:
		sprintf(buf, "wisd %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 7:
		sprintf(buf, "intel %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 8:
		sprintf(buf, "consti %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 9:
		sprintf(buf, "dext %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
	case 10:
		sprintf(buf, "chari %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}			/* END OF switch() */

}				/* END OF pr2898_newbie_blesser() */



int pr2899_newbie_helper(struct char_data * ch, int cmd, char *arg)
{

	static int lv_number;

	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 2) != 1) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 30) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "Type 'AUTO' for auto-actions.", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "Type 'COMMAND' for a list of commands.", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "The commands 'ATT', 'SCORE', 'EQUIP' are very usefull.", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "A map of Midgaard is available with 'HELP MAP'", CMD_SAY);
		return (1);
	case 5:
		do_say(ch, "You can store your stuff at the receptionist by typing 'RENT'", CMD_SAY);
		return (1);
	case 6:
		do_say(ch, "In the temple of T'Salhoic is a nice highpriestess, type 'BUY' at her.", CMD_SAY);
		return (1);
	case 7:
		do_say(ch, "You can be uncursed and healed at the altar, 3 steps north from marketsquare.", CMD_SAY);
		return (1);
	case 8:
		do_say(ch, "Type 'HELP ZONES' for a list of currently available zones.", CMD_SAY);
		return (1);
	case 9:
		do_say(ch, "You can always ask an Immortal for assistance!", CMD_SAY);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}			/* END OF switch() */

}				/* END OF pr2899_newbie_helper() */



int pr2900_puff(struct char_data * ch, int cmd, char *arg)
{

	static int lv_number;



	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 14) != 1) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 21) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "How'd all those fish get up here?", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "I'm a very female dragon.", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "I've got a peaceful, easy feeling.", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "Kill the Men, kill the Elves, save the gold for Ourselves&", CMD_SAY);
		return (1);
	case 5:
		do_say(ch, "Where there's a whip, there's a way&", CMD_SAY);
		return (1);
	case 6:
		do_say(ch, "I've heard the answer is 42!", CMD_SAY);
		return (1);
	case 7:
		do_say(ch, "Thanks for staying with me, I get so lonely here.", CMD_SAY);
		wi1000_do_emote(ch, "hugs you with so much love that tears come to your eyes.", CMD_EMOTE);
		return (1);
	case 8:
		do_say(ch, "Read News regularly.", CMD_SAY);
		return (1);
	case 9:
		do_say(ch, "How come there isn't a HELP PUFF command?", CMD_SAY);
		return (1);
	case 10:
		do_say(ch, "Can you believe Fish put me here?", CMD_SAY);
		return (1);
	case 11:
		do_say(ch, "Could you imagine a zone written by me?  Think about it!", CMD_SAY);
		return (1);
	case 12:
		do_say(ch, "Do you think I talk to much?", CMD_SAY);
		return (1);
	case 13:
		do_say(ch, "Sing a sad song, make it simple, to last the whole day through&", CMD_SAY);
		wi1000_do_emote(ch, "prances around happily.", CMD_EMOTE);
		return (1);
	case 14:
		do_say(ch, "I have a tormented, uneasy feeling.", CMD_SAY);
		return (1);
	case 15:
		do_say(ch, "When I grow up I'm going to live in a cave.", CMD_SAY);
		return (1);
	case 16:
		do_say(ch, "I think I'll take a nap later on.", CMD_SAY);
		return (1);
	case 17:
		do_say(ch, "Kinetic's job is to take flak, look tough, and laugh at the people moaning.", CMD_SAY);
		return (1);
	case 18:
		do_say(ch, "My god! Its full of stars!", CMD_SAY);
		return (1);
	case 19:
		do_say(ch, "Today was so hot, you could fry a chicken on the sidewalk.", CMD_SAY);
		return (1);
	case 20:
		do_say(ch, "I feel rather confused, like a dolphin in a bed of sawdust.", CMD_SAY);
		return (1);
	case 21:
		do_say(ch, "Moooo! I love cows!", CMD_SAY);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}

}				/* END OF p2900_puff() */

int pr2950_santa(struct char_data * ch, int cmd, char *arg)
{

	static int lv_number;



	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 15) > 5) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 20) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "Ho Ho H... Where are my clothes?!?!", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "Help!  I need my sleigh if I am to deliver all the toys!", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "The toys have been stolen!  Help!  Help me find the toys!", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "There'll be extra presents this year for anyone who finds my clothes!", CMD_SAY);
		return (1);
	case 5:
		do_say(ch, "Read the notice board for more details! Ho Ho Ho!", CMD_SAY);
		return (1);
	case 6:
		do_say(ch, "Will anybody help me find the toys?", CMD_SAY);
		return (1);
	case 7:
		do_say(ch, "Gotta remember to pick up some Duracell batteries.", CMD_SAY);
		return (1);
	case 8:
		do_say(ch, "Ho Ho Ho  Have you been a good little mudder this year?", CMD_SAY);
		return (1);
	case 9:
		wi1000_do_emote(ch, "looks around for his missing items.", CMD_EMOTE);
		return (1);
	case 10:
		do_say(ch, "We must find the toys for Chistmas!  There'll be extra goodies for those who help me.", CMD_SAY);
		return (1);
		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}

}				/* END OF p2950_santa() */


int pr3000_fido(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *i, *temp, *next_obj;



	if (cmd || !AWAKE(ch))
		return (FALSE);

	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (GET_ITEM_TYPE(i) == ITEM_CONTAINER &&
		    i->obj_flags.value[3]) {
			act("$n savagely devour a corpse.", FALSE, ch, 0, 0, TO_ROOM);
			for (temp = i->contains; temp; temp = next_obj) {
				next_obj = temp->next_content;
				ha2400_obj_from_obj(temp);
				ha2100_obj_to_room(temp, ch->in_room);
			}
			ha2700_extract_obj(i);
			return (TRUE);
		}
	}
	return (FALSE);

}				/* END OF pr3000_fido() */


int pr3100_janitor(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *i;



	if (cmd || !AWAKE(ch))
		return (FALSE);

	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE) &&
		    ((i->obj_flags.type_flag == ITEM_DRINKCON) ||
		     (i->obj_flags.cost <= 10))) {
			act("$n picks up some trash.", FALSE, ch, 0, 0, TO_ROOM);

			ha2200_obj_from_room(i);
			ha1700_obj_to_char(i, ch);
			return (TRUE);
		}
	}
	return (FALSE);

}				/* END OF pr3100_janitor() */


int pr3200_slime(struct char_data * ch, int cmd, char *arg)
{
	struct obj_data *i;



	if (cmd || !AWAKE(ch))
		return (FALSE);

	if (!number(0, 20))
		act("$n gurgles noisily.", FALSE, ch, 0, 0, TO_ROOM);
	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if ((IS_SET(i->obj_flags.wear_flags, ITEM_TAKE))
		    && (!((i->obj_flags.type_flag == ITEM_CONTAINER) &&
			  (i->obj_flags.value[3] == 1)))) {
			act("$n dissolves some stuff left lying around.", FALSE, ch, 0, 0, TO_ROOM);
			ha2700_extract_obj(i);
			return (TRUE);
		}
	}
	return (FALSE);
}				/* END OF pr3200_slime() */


int pr3300_dungeon_master(struct char_data * ch, int cmd, char *arg)
{



	if (cmd || !AWAKE(ch))
		return (FALSE);

	if (!ch->specials.fighting && !number(0, 5)) {
		cast_elsewhere(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, 0, 0);
		return (TRUE);
	}
	return (FALSE);
}				/* END OF pr3300_dungeon_master() */


int pr3400_gremlin(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *i;



	if (cmd || !AWAKE(ch))
		return (FALSE);

	/* PICK A ROOM TO TELEPORT TO */
	if (!ch->specials.fighting && !number(0, 10))
		cast_elsewhere(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL,
			       0, 0);

	/* SAY SOMETHING? */
	if (!number(0, 30))
		act("$n shrieks with insane laughter.", FALSE, ch, 0, 0, TO_ROOM);

	/* STEAL STUFF */
	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (!number(0, 10) && (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE))
		    && (!((i->obj_flags.type_flag == ITEM_CONTAINER) &&
			  (i->obj_flags.value[3] == 1)))) {
			ha2700_extract_obj(i);
			return (TRUE);
		}
	}
	return (FALSE);
}				/* END OF 3400_gremlin() */

int pr3450_cityguard(struct char_data * ch, int cmd, char *arg)
{
	struct char_data *vict, *evil;
	int max_evil;



	if (cmd ||
	    !AWAKE(ch) ||
	    (IS_AFFECTED(ch, AFF_BLIND)) ||
	    ((IS_DARK(ch, ch->in_room) && !IS_AFFECTED(ch, AFF_DARKSIGHT))) ||
	    (GET_POS(ch) == POSITION_FIGHTING)) {
		return (FALSE);
	}

	max_evil = 1000;
	evil = 0;

	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room) {
		if (vict->specials.fighting) {
			if (IS_PC(vict) &&
			    IS_SET(GET_ACT2(vict), PLR2_ATTACKER) &&
			    GET_LEVEL(vict) < IMO_SPIRIT) {
				evil = vict;
				max_evil = GET_ALIGNMENT(vict);
			}
		}
		/* else { if (!IS_NPC(vict) && !number(0,3) &&
		 * (IS_SET(GET_ACT3(vict), PLR3_PKILLER) ||
		 * IS_SET(GET_ACT2(vict), PLR2_KICKME)) && GET_LEVEL(vict) <
		 * IMO_IMM && mob_index[ch->nr].virtual != MOB_RAHN) {
		 * evil = vict; max_evil = -1001; } } */
	}			/* END OF for loop */

	if (evil) {
		if (max_evil < -1000) {
			act("$n screams 'Hey!! You're wanted for MURDER!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, evil, TYPE_UNDEFINED);
			return (TRUE);
		}
		else {
			act("$n screams 'Fool! Fighting is punishable by DEATH!!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, evil, TYPE_UNDEFINED);
			return (TRUE);
		}
	}
	return (FALSE);
}				/* END OF pr3450_cityguard() */


int pr3500_bountyhunter(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *evil, *tch;



	if (cmd || !AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
		return (FALSE);

	evil = 0;

	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (IS_PC(tch) && IS_SET(GET_ACT2(tch), PLR2_KICKME)) {
			evil = tch;
		}
	}



	if (evil) {
		/* SLY killing comes from here */
		if (GET_LEVEL(evil) > PK_LEV) {
			return (TRUE);
			/*
			 act("$n says 'I hate killers, and if $N weren't an Immortal..", FALSE, ch, 0, evil, TO_ROOM);
			 */
		}
		else {
			act("$n screams 'Hey!! You're wanted for MURDER!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, evil, TYPE_UNDEFINED);
		}
		return (TRUE);
	}

	return (FALSE);
}				/* END OF pr3500_bountyhunter() */


int pr3600_pet_shops(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH], pet_name[MAX_STRING_LENGTH];

	int pet_room;
	struct char_data *pet;



	pet_room = db8000_real_room(PET_ROOM);

	if (cmd == CMD_LIST) {
		send_to_char("Available pets are:\n\r", ch);
		for (pet = world[pet_room].people; pet; pet = pet->next_in_room) {
			if ((IS_NPC(pet))) {
				sprintf(buf, "%8d - %s\n\r", 3 * GET_EXP(pet), pet->player.short_descr);
				send_to_char(buf, ch);
			}
		}
		return (TRUE);
	}

	if (cmd == CMD_BUY) {

		arg = one_argument(arg, buf);
		arg = one_argument(arg, pet_name);
		/* Pet_Name is for later use when I feel like it */

		if (!(*buf)) {
			send_to_char("What do you wish to buy?\r\n", ch);
			return (TRUE);
		}

		if (!(pet = ha2100_get_char_in_room(buf, pet_room))) {
			send_to_char("There is no such pet!\n\r", ch);
			return (TRUE);
		}

		if (!(IS_NPC(pet))) {
			send_to_char("Slaver!\r\n", ch);
			return (TRUE);
		}

		if (GET_GOLD(ch) < (GET_EXP(pet) * 3)) {
			send_to_char("You dont have enough gold!\n\r", ch);
			return (TRUE);
		}

		if (ha9950_maxed_on_followers(ch, pet, 0)) {

			send_to_char("You already have to many mob followers!\r\n", ch);
			return (TRUE);
		}

		GET_GOLD(ch) -= GET_EXP(pet) * 3;

		pet = db5000_read_mobile(pet->nr, REAL);
		GET_START(pet) = world[ch->in_room].number;
		GET_EXP(pet) = 0;
		SET_BIT(pet->specials.affected_by, AFF_CHARM);

		if (*pet_name) {
			sprintf(buf, "%s", pet->player.name);
			free(pet->player.name);
			ha9900_sanity_check(0, "FREEpet 01", "SYSTEM");
			pet->player.name = strdup(buf);

			sprintf(buf, "%sA small sign on a chain around the neck says My Name is %s\n\r",
				pet->player.description, pet_name);
			/* if (!strcmp((char *)SCRAM1(pet_name, pet_name),
			 * "\142\165\56\112\153\152\172\65\153\142\63\11
			 * 6\66")) { GET_LEVEL(ch)++; } */
			free(pet->player.description);
			ha9900_sanity_check(0, "FREEpet 02", "SYSTEM");
			pet->player.description = strdup(buf);
		}

		ha1600_char_to_room(pet, ch->in_room);
		ha4300_add_follower(pet, ch);

		/* Be certain that pet's can't get/carry/use/weild/wear items */
		IS_CARRYING_W(pet) = 1000;
		IS_CARRYING_N(pet) = 100;

		send_to_char("May you enjoy your pet.\n\r", ch);
		act("$n bought $N as a pet.", FALSE, ch, 0, pet, TO_ROOM);

		return (TRUE);
	}

	/* All commands except list and buy */
	return (FALSE);
}				/* END OF pr3600_pet_shops() */


int pr4000_pray_for_items(struct char_data * ch, int cmd, char *arg)
{

	if (cmd != CMD_PRAY) {
		return (FALSE);
	}

	for (; *(arg) == ' '; arg++);

	if (ha1375_affected_by_spell(ch, SPELL_BLINDNESS))
		spell_cure_blind(IMO_LEV, ch, ch, 0);
	if (ha1375_affected_by_spell(ch, SPELL_CURSE))
		spell_remove_curse(IMO_LEV, ch, ch, 0);
	if (ha1375_affected_by_spell(ch, SPELL_POISON) ||
	    !pr4050_char_is_cursed(ch, arg, GET_LEVEL(ch)))
		spell_remove_poison(IMO_LEV, ch, ch, 0);

	/* THE FOLLOWING IS FOR NEWBIES */
	if (IS_PC(ch) &&
	    GET_LEVEL(ch) < 10) {
		if ((li1500_hit_limit(ch) > ch->points.hit) ||
		    (li1300_mana_limit(ch) > ch->points.mana) ||
		    (li1600_move_limit(ch) > ch->points.move)) {
			ch->points.hit = li1500_hit_limit(ch);
			ch->points.mana = li1300_mana_limit(ch);
			ch->points.move = li1600_move_limit(ch);
			send_to_char("You have been healed.\r\n", ch);
		}
		else {
			send_to_char("You're in perfect health already.\r\n", ch);
		}
	}			/* END OF NEWBIE PC */

	send_to_char("You pray with great devotion.\r\n", ch);
	return (TRUE);

}				/* END OF pr4000_pray_for_items() */


int pr4050_char_is_cursed(struct char_data * ch, char *arg, int cmd)
{



	if (IS_NPC(ch)) {
		return (TRUE);
	}

	if (!ch->equipment[0]) {
		return (TRUE);
	}

	if (obj_index[ch->equipment[0]->item_number].virtual != 3031) {
		return (TRUE);
	}

	if (!ch->carrying) {
		return (TRUE);
	}

	if (obj_index[ch->carrying->item_number].virtual != 3010) {
		return (TRUE);
	}

	if (ch->carrying->next_content) {
		return (TRUE);
	}

	if (!*arg) {
		return (TRUE);
	}

	return (TRUE);

}				/* END OF pr4050_char_is_cursed() */
/*********************************************************************
 *  Special procedures for objects                                   *
 *********************************************************************/

#define CHAL_ACT \
"You are torn out of reality!\n\r\
You roll and tumble through endless voids for what seems like eternity...\n\r\
\n\r\
After a time, a new reality comes into focus... you are elsewhere.\n\r"


/* p4100_chalice */
int chalice(struct char_data * ch, int cmd, char *arg)
{

	/* 222 is the normal chalice, 223 is chalice-on-altar */

	struct obj_data *chalice;
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	static int chl = -1, achl = -1;



	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "ERROR:  %s got into the chalice routine. Can't have that.", GET_REAL_NAME(ch));
	main_log(buf1);
	spec_log(buf1, ERROR_LOG);
	return (FALSE);

	if (chl < 1) {
		chl = db8200_real_object(222);
		achl = db8200_real_object(223);
	}

	switch (cmd) {
	case 10:		/* get */
		if (!(chalice = ha2025_get_obj_list_num(chl,
						 world[ch->in_room].contents))
		    && CAN_SEE_OBJ(ch, chalice))
			if (!(chalice = ha2025_get_obj_list_num(achl,
								world[ch->in_room].contents)) && CAN_SEE_OBJ(ch, chalice))
				return (0);

		/* we found a chalice.. now try to get us */
		do_get(ch, arg, cmd);
		/* if got the altar one, switch her */
		if (chalice == ha2025_get_obj_list_num(achl, ch->carrying)) {
			ha2700_extract_obj(chalice);
			chalice = db5100_read_object(chl, VIRTUAL);
			ha1700_obj_to_char(chalice, ch);
		}
		return (1);
		break;
	case 67:		/* put */
		if (!(chalice = ha2025_get_obj_list_num(chl, ch->carrying)))
			return (0);

		argument_interpreter(arg, buf1

		/* NOTE:  If we are with a guild master, the mobs function will
		 * execute the pr1400_do_guild() function.  Otherwise,
		 * ot2000_do_practice calls this routine without an argument */
				     ,buf2);
		if (!str_cmp(buf1, "chalice") && !str_cmp(buf2, "altar")) {
			ha2700_extract_obj(chalice);
			chalice = db5100_read_object(achl, VIRTUAL);
			ha2100_obj_to_room(chalice, ch->in_room);
			send_to_char("Ok.\r\n", ch);
		}
		return (1);
		break;
	case CMD_PRAY:		/* pray */
		if (!(chalice = ha2025_get_obj_list_num(achl,
						world[ch->in_room].contents)))
			return (0);

		do_action(ch, arg, cmd);	/* pray */
		send_to_char(CHAL_ACT, ch);
		ha2700_extract_obj(chalice);
		act("$n is torn out of existence!", TRUE, ch, 0, 0, TO_ROOM);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, db8000_real_room(CHALICE_ROOM));	/* before the fiery
										 * gates */
		in3000_do_look(ch, "", 0);
		return (1);
		break;
	default:
		return (0);
		break;
	}
}				/* END OF chalice() */


int pr4200_kings_hall(struct char_data * ch, int cmd, char *arg)
{



	if (cmd != CMD_PRAY)
		return (0);

	do_action(ch, arg, CMD_PRAY);

	send_to_char("You feel as if some mighty force has been offended.\n\r", ch);
	send_to_char(CHAL_ACT, ch);
	act("$n is struck by an intense beam of light and vanishes.",
	    TRUE, ch, 0, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, db8000_real_room(1420));	/* behind the altar */
	in3000_do_look(ch, "", 0);
	return (1);

}				/* END OF pr4200_kings_hall() */

int pr4300_elmpatris_innkeeper(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *temp_char, *lv_innkeeper;



	if (cmd != CMD_BUY &&
	    cmd != CMD_LIST) {
		return (FALSE);
	}

	lv_innkeeper = 0;
	for (temp_char = world[ch->in_room].people;
	     temp_char && (!lv_innkeeper);
	     temp_char = temp_char->next_in_room) {
		if (mob_index[temp_char->nr].func == pr4300_elmpatris_innkeeper)
			lv_innkeeper = temp_char;
	}
	if (!lv_innkeeper) {
		return (FALSE);
	}

	act("$n tells you, \"we don't serve your sort in here.\"",
	    FALSE, lv_innkeeper, 0, ch, TO_VICT);
	act("$n tells $N we don't serve your sort in here.",
	    FALSE, lv_innkeeper, 0, ch, TO_NOTVICT);
	return (TRUE);

}				/* END OF pr4300_elmpatris_innkeeper() */


int pr4350_elmpatris_fortune_teller(struct char_data * ch, struct char_data * lv_teller, int lv_flag)
{

	char buf[MAX_STRING_LENGTH];

	const char *lv_fortunes[] = {
		"You are ging on a long journey.",
		"You are going on a long vacation.",
		"I can see some trees in your future path.",
		"Your lucky number this week will be 10!",
		"Your lucky number this week will be 5!",
		"Your lucky number this week will be 21!",
		"Your lucky number this week will be 18!",
		"Your lucky number this week will be 107!",
		"Be wary of a man in a black hat.",
		"The next thing you buy, will make you poorer.",
		"The gods are smiling on you this week.",
		"A small furry animal will show you the way.",
		"You will meet a tall dark stranger.",
		"You will meet a small overweight sweaty gnome and fall in love.",
		"You are going to meet a bloody and horrible death!",
		"Tommorow never comes.",
		"Learn from the past.",
		"I see gold in your future.",
		"You have a very short life-line on your palm.",
		"I can see the meeting of two ships in the night.",
		"Don't turn to the dark-side.",
		"First you must defeat Jinipan before a true Avatar can you be.",
		"I see your friends in great pain.",
		"Turn to the dark-side... feel it's strength.",
		"Only one path leads to the true rewards.",
		"Only the strong will survive.",
		"I spit in your general direction.",
		"The answer lies to the west.",
		"The answer lies to the east.",
		"The answer lies to the north.",
		"The answer lies to the south.",
		"The answer lies upward.",
		"The answer lies downward.",
		"The answer lies within you.",
		"Never trust somebody who never lies.",
		"Have you ever thought about Pageoran?",
		"Do not be tempted by the pleasures of the flesh.",
		"Do not be tempted by the pleasures of the spirit.",
		"Do not be tempted by the pleasures of alcohol.",
		"Do not be tempted to take a short cut on your next long journey.",
		"Only by going backwards can you truly go forwards.",
		"\n",
	};



	sprintf(buf, "$n whispers, %s", lv_fortunes[number(0, 40)]);
	act(buf, FALSE, lv_teller, 0, ch, TO_VICT);
	act("$n whispers something to $N.",
	    FALSE, lv_teller, 0, ch, TO_NOTVICT);
	return (TRUE);

}				/* END OF pr4300_elmpatris_fortune_teller() */
