/* li */
/* gv_location: 7001-7500 */
/* ******************************************************************
*  file: limits.c , Limit and gain control module.   Part of DIKUMUD *
*  Usage: Procedures controling gain and limit.                      *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "spells.h"
#include "constants.h"
#include "globals.h"
#include "limits.h"
#include "rent.h"
#include "func.h"


#define CANT_IDLE_IN_FIGHT		1

#define AVAT_IDLE_TIMEOUT					30
#define MORT_IDLE_TIMEOUT					15	/* 12 in ticks */
#define DROPLINK_TIMEOUT					60	/* 24 in ticks */

bool li1000_can_see_char(struct char_data * ch, struct char_data * victim, int lv_flag)
{
	int lv_stat;

	lv_stat = TRUE;		/* DEFAULT TO YOU CAN SEE IT */

	if (GET_LEVEL(ch) < GET_VISIBLE(victim))
		lv_stat = FALSE;
	if (IS_NPC(ch) && GET_VISIBLE(victim) > PK_LEV)
		lv_stat = FALSE;

	if (GET_LEVEL(ch) < GET_GHOST(victim))
		//Incognito & ghost 22.07 .01
			lv_stat = FALSE;
	if (IS_NPC(ch) && GET_GHOST(victim) > PK_LEV)
		lv_stat = FALSE;


	if (GET_LEVEL(ch) < IMO_LEV) {
		/* IS THE VICTIM INVISIBLE? */
		if (IS_AFFECTED((victim), AFF_INVISIBLE)) {
			if (!(IS_AFFECTED((ch), AFF_DETECT_INVISIBLE))) {
				lv_stat = FALSE;
			}
		}

		/* IS THE PERSON BLIND? */
		if (IS_AFFECTED(ch, AFF_BLIND))
			lv_stat = FALSE;

		/* IS THE ROOM DARK? */
		if (!(IS_AFFECTED(ch, AFF_DARKSIGHT))) {
			if (IS_DARK(ch, ch->in_room)) {
				lv_stat = FALSE;
			}
		}
	}			/* END OF level less than IMO_LEV */

	return (lv_stat);

}				/* END OF li1000_can_see_char() */
/* IF PASSED BIT0 AS FLAG, ROUTINE ALSO CHECKS NOLOCATE FLAG */
bool li1100_can_see_obj(struct char_data * ch, struct obj_data * obj, int lv_flag)
{

	int lv_stat;
	struct char_data *tmp_ch;
	struct obj_data *tmp_obj;



	lv_stat = TRUE;		/* DEFAULT TO YOU CAN SEE IT */

	if (IS_SET(lv_flag, BIT0)) {
		/* IF ITEM IS NO_LOCATE, THEY CAN'T SEE IT */
		if (IS_SET(GET_OBJ1(obj), OBJ1_NO_LOCATE)) {
			if (GET_LEVEL(ch) < IMO_IMM) {
				lv_stat = FALSE;
			}
		}
	}

	if (GET_LEVEL(ch) < IMO_IMM) {
		/* IF ITEM IS NO_SEE, THEY CAN'T SEE IT */
		if (IS_OBJ_STAT(obj, OBJ1_NO_SEE)) {
			if (GET_LEVEL(ch) < IMO_IMM) {
				lv_stat = FALSE;
			}
		}
	}

	/* SAFTY CHECK */
	if (!obj) {
		do_sys("ERROR: li1100 obj is zero.", IMO_IMP, ch);
		spec_log("ERROR: li1100 obj is zero.", ERROR_LOG);
		return (FALSE);
	}

	/* FIND MAIN CONTAINER THIS OBJECT IS IN */
	tmp_obj = obj;
	while (tmp_obj->in_obj) {
		tmp_obj = tmp_obj->in_obj;

	}			/* END OF while() */

	/* FIND THE PERSON WHO HAS THIS OBJECT */
	tmp_ch = 0;
	if (tmp_obj->carried_by) {
		tmp_ch = tmp_obj->carried_by;
	}

	/* IF A PERSON HAS THE OBJECT, CAN WE SEE THE PERSON? */
	if (tmp_ch) {
		if (!CAN_SEE(ch, tmp_ch)) {
			lv_stat = FALSE;
		}
	}

	if (GET_LEVEL(ch) < IMO_LEV) {
		/* IS THE ITEM INVISIBLE? */
		if (IS_SET(obj->obj_flags.flags1, OBJ1_INVISIBLE)) {
			if (!(IS_AFFECTED((ch), AFF_DETECT_INVISIBLE))) {
				lv_stat = FALSE;
			}
		}

		/* IS THE PERSON BLIND? */
		if (IS_AFFECTED(ch, AFF_BLIND))
			lv_stat = FALSE;

		/* IS THE ROOM DARK? */
		if (!(IS_AFFECTED(ch, AFF_DARKSIGHT))) {
			if (IS_DARK(ch, ch->in_room)) {
				lv_stat = FALSE;
			}
		}
	}			/* END OF level less than IMO_LEV */

	return (lv_stat);

}				/* END OF r1000_can_see_char() */

bool li1200_is_it_dark(struct char_data * ch, int lv_room, int lv_flag)
{



	if (lv_room < 0 || lv_room > top_of_world) {
		return (FALSE);
	}

	if (world[lv_room].number == 4) {
		/* for debugging */
		world[lv_room].number = 4;
	}

	if (world[lv_room].light) {
		return (FALSE);
	}

	/* IS ROOM FLAGGED DARK? */
	if (IS_SET(world[lv_room].room_flags, RM1_DARK))
		return (TRUE);

	/* IS IT DARK OUTSIDE? */
	if (world[lv_room].sector_type == SECT_FIELD ||
	    world[lv_room].sector_type == SECT_FOREST ||
	    world[lv_room].sector_type == SECT_HILLS ||
	    world[lv_room].sector_type == SECT_MOUNTAIN ||
	    world[lv_room].sector_type == SECT_WATER_SWIM ||
	    world[lv_room].sector_type == SECT_WATER_NOSWIM ||
	    world[lv_room].sector_type == SECT_UNDERWATER) {
		/* MAKE SURE CHAR IS NOT A NEWBIE */
		/* IE:  THEY ARE COMMITED TO THE GAME BEFORE WE NAIL EM */
		if (GET_LEVEL(ch) > 14 &&
		    (time_info.hours > 22 ||
		     time_info.hours < 4)) {
			return (TRUE);
		}
	}

	/* THE ROOM MUST NOT BE DARK */
	return (FALSE);

}				/* END OF li1200_is_it_dark() */


/* This routine takes your current max   */
/* mana and adds to it based on your INT */
int li1300_mana_limit(struct char_data * ch)
{

	int max, add_mana;



	max = (ch->points.max_mana);

	if (IS_NPC(ch) || GET_LEVEL(ch) > IMO_SPIRIT) {
		/* NO CHANGE */
		return (max);
	}

	/*
         add_mana = (GET_INT(ch) + (GET_WIS(ch) / 4) +
							       (GET_LEVEL(ch) / 5)) / 4;
	       */
	add_mana = ((races[GET_RACE(ch)].base_mana + 130) *
		    ((GET_INT(ch) * 10) + GET_WIS(ch)) *
		    (GET_LEVEL(ch) + 16)) / 100000;

//TODO:For class gains
	max +=  (add_mana * classes[GET_CLASS(ch)].adj_mana / 100);

	//max += add_mana;

	/* DON'T LET THEM EXCEED RACE BOUNDARIES */
	if (max >= races[GET_RACE(ch)].max_mana)
		max = races[GET_RACE(ch)].max_mana;

	return (max);

}
/* This routine takes your current max   */
/* hits and adds to it based on your CON */
int li1500_hit_limit(struct char_data * ch)
{
	int lv_gain, lv_result;



	if (IS_NPC(ch) || GET_LEVEL(ch) > IMO_SPIRIT) {
		return (ch->points.max_hit);
	}

	lv_gain = GET_CON(ch) + (GET_LEVEL(ch) / 5) - 5;

	/*
	 if (GET_LEVEL(ch) < 2)
	 lv_adj_level = 0;
	 else
	 lv_adj_level = MAXV(1, (GET_LEVEL(ch) / 10));
	 lv_adj_con   = MAXV(1, GET_CON(ch)  - 10);
	 lv_gain = lv_adj_con + ((lv_adj_con/2) * lv_adj_level);
	 lv_gain = lv_gain + lv_adj_level * 7;
	 lv_gain = MAXV(2, lv_gain);
	 */

//TODO:For class gains
	lv_result = ch->points.max_hit + (lv_gain * classes[GET_CLASS(ch)].adj_hit / 100);

	//lv_result = ch->points.max_hit + lv_gain;
	/* DON'T LET THEM EXCEED RACE BOUNDARIES */
	if (lv_result >= races[GET_RACE(ch)].max_hit)
		lv_result = races[GET_RACE(ch)].max_hit;

	return (lv_result);

}				/* END OF li1500_hit_limit() */


/* This routine takes your current max   */
/* move and adds to it based on your DEX */
int li1600_move_limit(struct char_data * ch)
{

	int max, add_move;



	max = ch->points.max_move;

	if (IS_NPC(ch) || GET_LEVEL(ch) > IMO_SPIRIT) {
		/* NO CHANGE */
		return (max);
	}

	add_move = (GET_DEX(ch) + (GET_CON(ch) / 5) + (GET_LEVEL(ch) / 5)) / 3.5;
	
	//TODO:For class gains
	max += (add_move * classes[GET_CLASS(ch)].adj_move / 100);
	//max += add_move;


	/* DON'T LET THEM EXCEED RACE BOUNDARIES */
	if (max >= races[GET_RACE(ch)].max_move)
		max = races[GET_RACE(ch)].max_move;

	return (max);

}				/* END OF li1600_move_limit */
#define EGYPTIANHEAL 5992

int li1700_mana_gain(struct char_data * ch)
{

	int gain;



	if (races[GET_RACE(ch)].regen_mana == 0) {
		return (0);
	}

	if (IS_NPC(ch)) {
		/* Neat and fast */
		gain = GET_LEVEL(ch);

	}
	else {
		gain = 8;
		gain += z_age(ch).year / 20;

		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_GOOD_ALIGNMENT) ||
		  IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_GOOD_ALIGNMENT)) {
			/* MUST MAINTAIN GOOD ALIGNMENT */
			gain = MINV(10, (GET_ALIGNMENT(ch) - 200) / 50);
		}
		else {
			if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_EVIL_ALIGNMENT) ||
			    IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_EVIL_ALIGNMENT)) {
				gain = MINV(10, (-1 * GET_ALIGNMENT(ch) - 200) / 50);
			}
			else {
				gain += gain;
			}
		}

		/* Position calculations    */
		switch (GET_POS(ch)) {
		case POSITION_SLEEPING:
			gain += gain;
			break;
		case POSITION_RESTING:
			gain += (gain >> 1);	/* Divide by 2 */
			break;
		case POSITION_SITTING:
			gain += (gain >> 2);	/* Divide by 4 */
			break;
		}
	}

	/* ADJUSTMENTS FOR RACE */
	if (gain > 0) {
		gain = gain * races[GET_RACE(ch)].regen_mana;
		gain = gain / 100;
	}

	if (IS_AFFECTED(ch, AFF_REGENERATION))
		gain += (gain >> 1);

	if (IS_AFFECTED(ch, AFF_POISON))
		gain >>= 2;

	if ((GET_COND(ch, FOOD) == 0) || (GET_COND(ch, THIRST) == 0))
		gain >>= 2;

	if (IS_AFFECTED(ch, AFF_HASTE))
		gain += gain;
	
	if (GET_LEVEL(ch) < IMO_LEV)
		gain += (gain >> 1);

	return (gain);

}				/* END OF li1700_mana_gain() */


int li1800_hit_gain(struct char_data * ch)
{

	struct affected_type *af, *next_af;

	char buf[MAX_STRING_LENGTH];
	int gain;
	int dam;
	int poison;



	if (races[GET_RACE(ch)].regen_hit == 0) {
		return (0);
	}

	/* DAMAGE DONE BY HEMMORAGE */
	if (IS_AFFECTED(ch, AFF_HEMMORAGE)) {
		for (af = ch->affected; af; af = next_af) {
			next_af = af->next;
			if (af->type == SPELL_HEMMORAGE) {
				dam = dice(10, af->modifier);
				send_to_char("You feel the hemmorage burning through your blood.\r\n", ch);
				DAMAGE(ch, ch, dam, SPELL_HEMMORAGE);
			}
		}
	}			/* END OF HEMMORAGE */

	if (IS_NPC(ch)) {
		gain = GET_LEVEL(ch);
		/* Neat and fast */
		return (gain);
	}

	/* DEFAULT GAIN FOR PLAYERS */
	gain = 16;

	/* DARKLINGS */
	if (GET_RACE(ch) == RACE_DARKLING &&
	    GET_LEVEL(ch) < IMO_SPIRIT) {
		if ((weather_info.sky == SKY_CLOUDLESS) &&
		    (time_info.hours > 5) &&
		    (time_info.hours < 22) &&
		    OUTSIDE(ch)) {
			send_to_char("You suffer in the sunlight..\r\n", ch);
			gain -= MAXV(10, GET_HIT(ch) / 40);
		}
	}			/* END OF RACE_DARKLING */

	if ((world[ch->in_room].sector_type == SECT_UNDERWATER) &&
	    GET_LEVEL(ch) < IMO_LEV && !IS_AFFECTED(ch, AFF_BREATHWATER)) {
		send_to_char("You are drowning!\r\n", ch);
		/* gain = -1*number(1,4); */
		/* gain = -1 * MAXV(10, GET_HIT(ch) / 10); */

		gain = -1 * (GET_HIT(ch) * number(50, 80) / 100);

		if (GET_HIT(ch) < 0) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s L[%d] has been killed by drowning.\r\n",
				GET_REAL_NAME(ch), GET_LEVEL(ch));
			do_info(buf, 1, IMO_SPIRIT, ch);
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s L[%d] has been killed by drowning in room %d.",
				GET_REAL_NAME(ch),
				GET_LEVEL(ch),
				world[ch->in_room].number);
			spec_log(buf, DEATH_LOG);
			strcat(buf, "\r\n");
			do_info(buf, IMO_IMM, MAX_LEV, ch);

			GET_HOW_LEFT(ch) = LEFT_BY_DEATH_ROOM;
			if (GET_LEVEL(ch) > 20) {
				send_to_char("You choke of the water in your lungs and die!\r\n", ch);
				ft2000_die(ch, ch);
			}
		}
	}
	/* Adjust gain for age of character. Should we go negative here for
	 * really old characters? */

	if (gain < z_age(ch).year / 10) {
		gain = 0;
	}
	else {
		gain -= z_age(ch).year / 10;
	}

	/* IF THEY WERE STUPID ENOUGH TO CHOOSE A GOOD/EVIL */
	/* COMBINATION, LET THEM SUFFER                     */
	if (GET_LEVEL(ch) < IMO_SPIRIT) {
		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_GOOD_ALIGNMENT) ||
		  IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_GOOD_ALIGNMENT)) {
			if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_EVIL_ALIGNMENT) ||
			    IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_EVIL_ALIGNMENT)) {

				send_to_char("Selecting opposing alignments wasn't such a good idea\r\n", ch);
				gain = GET_HIT(ch) + 1;
				if (gain < 10)
					gain = 10;
				gain = gain * -1;
				return (gain);
			}	/* END OF evil alignment race/class */
		}		/* END OF good alignment race/class */
	}			/* END OF less than IMO_SPIRIT */

	if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_GOOD_ALIGNMENT) ||
	    IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_GOOD_ALIGNMENT)) {
		gain = MINV(18, (GET_ALIGNMENT(ch) - 200) / 50);
		gain = MAXV(gain, -5);
		if (gain < 0) {
			send_to_char("You suffer because of alignment.\r\n",
				     ch);
		}
	}
	else {
		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_EVIL_ALIGNMENT) ||
		  IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_EVIL_ALIGNMENT)) {
			gain = MINV(18, (-1 * GET_ALIGNMENT(ch) - 200) / 50);
			gain = MAXV(gain, -5);
			if (gain < 0) {
				send_to_char("You suffer because of alignment.\r\n",
					     ch);
			}
		}
		else {
			if ((GET_CLASS(ch) == CLASS_PALADIN) && (GET_LEVEL(ch) > 5)) {
				if ((GET_ALIGNMENT(ch) >= -500) && (GET_ALIGNMENT(ch) <= 500)) {
				gain = -5; 
				}
				if (gain < 0) {
				send_to_char("You suffer because due to alignment.\r\n", ch);
				}
			}
			else {
			gain += gain >> 1;
		}
		}
	}

	/* Position calculations    */
	switch (GET_POS(ch)) {
	case POSITION_SLEEPING:
		gain += (gain >> 1);	/* Divide by 2 */
		break;
	case POSITION_RESTING:
		gain += (gain >> 2);	/* Divide by 4 */
		break;
	case POSITION_SITTING:
		gain += (gain >> 3);	/* Divide by 8 */
		break;
	}

	/* ADJUSTMENTS FOR RACE */
	if (gain > 0) {
		gain = gain * races[GET_RACE(ch)].regen_hit;
		gain = gain / 100;
	}

	if (IS_AFFECTED(ch, AFF_REGENERATION))
		gain += gain >> 1;

	if (IS_AFFECTED(ch, AFF_POISON)) {
		gain >>= 2;
		poison = IS_NPC(ch) ? ((GET_HIT(ch) * .05) + GET_LEVEL(ch)) / 50 : (GET_HIT(ch) * .05) + GET_LEVEL(ch);
		if (GET_HIT(ch) - poison <= 0) {
			ansi_act("&rYou crumple to the ground and die violently from poisoning.&n", FALSE, ch, 0, 0, TO_VICT, 0, 0);
			ansi_act("&r$n crumples to the ground and dies violently from poisoning.&n", FALSE, ch, 0, 0, TO_NOTVICT, 0, 0);
			sprintf(buf, "%s L[%d] crumples over and dies from severe poisoning.\r\n",
				GET_REAL_NAME(ch),
				GET_LEVEL(ch));
			do_info(buf, 1, MAX_LEV, ch);
			ha1370_remove_all_spells(ch);
			ft1800_raw_kill(ch, ch);
		}
		else {
			DAMAGE(ch, ch, poison, SPELL_POISON);
		}
	}

	if (IS_AFFECTED(ch, AFF_PROTECT_EVIL)) {
		if (GET_ALIGNMENT(ch) < -99) {
			gain >>= 2;
			send_to_char("You feel a repulsion from evil.\r\n", ch);
			DAMAGE(ch, ch, 2, SPELL_PROTECT_FROM_EVIL);
		}
	}

	if ((GET_COND(ch, FOOD) == 0) || (GET_COND(ch, THIRST) == 0))
		gain >>= 2;

	if (IS_AFFECTED(ch, AFF_HASTE))
		gain += gain;
	
		if (GET_LEVEL(ch) < IMO_LEV)
		gain += (gain >> 1);

	return (gain);

}				/* END OF li1800_hit_gain() */


int li1900_move_gain(struct char_data * ch)
{
	int gain;

	if (races[GET_RACE(ch)].regen_move == 0) {
		return (0);
	}

	if (IS_NPC(ch)) {
		/* short and fast */
		return (GET_LEVEL(ch));
	}

	 /* gain = graf(z_age(ch).year, 12,18,22,21,14,10,6) */ ;
	gain = 21;

	/* Adjust for age */

	if (gain < z_age(ch).year / 10) {
		gain = 0;
	}
	else {
		gain -= z_age(ch).year / 10;
	}

	/* Position calculations    */
	switch (GET_POS(ch)) {
	case POSITION_SLEEPING:
		gain += (gain >> 1);	/* Divide by 2 */
		break;
	case POSITION_RESTING:
		gain += (gain >> 2);	/* Divide by 4 */
		break;
	case POSITION_SITTING:
		gain += (gain >> 3);	/* Divide by 8 */
		break;
	}

	/* ADJUSTMENTS FOR RACE */
	if (gain > 0) {
		gain = gain * races[GET_RACE(ch)].regen_move;
		gain = gain / 100;
	}

	if (IS_AFFECTED(ch, AFF_REGENERATION))
		gain += gain >> 1;

	if (IS_AFFECTED(ch, AFF_POISON))
		gain >>= 2;

	if ((GET_COND(ch, FOOD) == 0) || (GET_COND(ch, THIRST) == 0))
		gain >>= 2;

	if (IS_AFFECTED(ch, AFF_HASTE))
		gain += gain;

	if (GET_LEVEL(ch) < IMO_LEV)
		gain += (gain >> 1);
	
	return (gain);

}				/* END OF li1900_move_gain() */


/* Gain in various points */
void li2000_advance_level(struct char_data * ch)
{
	int idx;

	ch->points.max_hit = MAXV(1, li1500_hit_limit(ch));
	ch->points.max_mana = MAXV(1, li1300_mana_limit(ch));
	ch->points.max_move = MAXV(1, li1600_move_limit(ch));

	/* ADJUST FOR SPECIAL RACIAL MODIFIERS */
	ch->points.max_hit += races[GET_RACE(ch)].adj_hit;
	ch->points.max_mana += races[GET_RACE(ch)].adj_mana;
	ch->points.max_move += races[GET_RACE(ch)].adj_move;

	/* PRACTICES */
	ch->specials.spells_to_learn += GET_WIS(ch) / 3 + 1;

	if (GET_LEVEL(ch) >= IMO_LEV)
		for (idx = 0; idx < 3; idx++)
			ch->specials.conditions[idx] = -1;

	/* Reward them with a restore for leveling */
	ch->points.hit = li1500_hit_limit(ch);
	ch->points.mana = li1300_mana_limit(ch);
	ch->points.move = li1600_move_limit(ch);
	send_to_char("Your health has been replenished as a reward for your level!\r\n", ch);

	ha9700_set_saving_throws(ch, 0);

	/* UPDATE descriptor level */
	if (IS_PC(ch)) {
		GET_DSC_LEVEL(ch) = GET_LEVEL(ch);
	}			/* END OF is pc */

	return;

}				/* END OF li2000_advance_level() */


void li2100_set_title(struct char_data * ch)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int lv_check;

	if (IS_NPC(ch))
		return;

	if (!*GET_NAME(ch)) {
		main_log("ERROR: Trying to set title on a blank char");
		return;
	}

	/* WHAT WOULD THE OLD STANDARD TITLE BE? */
	bzero(buf, sizeof(buf));
	if (GET_LEVEL(ch) > 1) {
		sprintf(buf, "%s the level %d %s",
		GET_NAME(ch), GET_LEVEL(ch) - 1, classes[GET_CLASS(ch)].name);
	}
	else {
		if (GET_TITLE(ch)) {
			strcpy(buf, GET_TITLE(ch));
		}
		else {
			sprintf(buf, "%s", GET_NAME(ch));
		}
	}

	/* MAKE SURE ITS NOT TOO LONG */
	buf[70] = 0;
	buf[71] = 0;

	/* COMPUTE NEW TITLE */
	bzero(buf2, sizeof(buf));
	sprintf(buf2, "%s the level %d %s",
		GET_NAME(ch), GET_LEVEL(ch), classes[GET_CLASS(ch)].name);

	/* MAKE SURE ITS NOT TOO LONG */
	buf2[70] = 0;
	buf2[71] = 0;

	if (GET_TITLE(ch)) {
		/* IF THEY HAVE THE STANDARD TITLE CHANGE IT? */
		lv_check = strcmp(buf, GET_TITLE(ch));
		if (!lv_check) {
			RECREATE(GET_TITLE(ch), char, strlen(buf2) + 1);
			strcpy(GET_TITLE(ch), buf2);
		}
	}
	else {
		CREATE(GET_TITLE(ch), char, strlen(buf2) + 1);
		strcpy(GET_TITLE(ch), buf2);
	}

}				/* END OF li2100_set_title() */


void li2200_gain_exp(struct char_data * ch, int gain)
{
	int i, lv_exp, orig_lev;
	bool is_altered = FALSE;
	char buf[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) > PK_LEV)
		return;

	if (ch->in_room < 0) {
		send_to_char("You're in an invalid room.\r\n", ch);
		return;
	}

	if (IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA))
		return;

	if (IS_SET(zone_table[world[ch->in_room].zone].flags, ZONE_TESTING)) {
		send_to_char("(except this zone is still being tested\n\r", ch);
		return;
	}

	if ((gain > 0) && (GET_LEVEL(ch) <= PK_LEV) && (GET_LEVEL(ch) > 0)) {
		orig_lev = GET_LEVEL(ch);
		if (GET_LEVEL(ch) < 6 &&
		    gain > 0) {
			GET_EXP(ch) += (gain * 2);
		}
		else {
			GET_EXP(ch) += gain;
		}

		if ((GET_LEVEL(ch) < IMO_LEV) && (IS_SET(GET_ACT4(ch), PLR4_NOADVANCE)) && (GET_EXP(ch) > LEVEL_EXP(orig_lev))) {
			GET_EXP(ch) = LEVEL_EXP(orig_lev);
			//send_to_char("&RYou have reached max experience for this level.&n\n\r", ch);
		}


		if ((GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV) &&
		    GET_EXP(ch) > 1000000000) {
			GET_EXP(ch) = 1000000000;
			send_to_char("&RYou have already reached max experience.&n\n\r", ch);
		}

		/* FIND LEVEL FOR CHAR AFTER ADJUSTING EXP */
		for (i = orig_lev; (lv_exp = LEVEL_EXP(i)) < GET_EXP(ch) && i < MAX_LEV; i++);
		if (GET_EXP(ch) > lv_exp)
			i++;
		else {
			/* IF WE ARE OVER LEVEL 5 DON'T ALLOW MORE THAN ONE
			 * LEVEL */
			if (i > orig_lev) {
				if (GET_LEVEL(ch) == IMO_LEV ||
				    GET_LEVEL(ch) == PK_LEV || IS_SET(GET_ACT4(ch), PLR4_NOADVANCE)) {
					//Bingo - 11 - 24 - 01, so avats and pk can gain more exp w / o leveling
						i = orig_lev;
				}
				else {
					i = orig_lev + 1;
				}
			}
		}

		/* WE ADVANCED */
		/*
		If 
		*/
		if (i > orig_lev) {
			GET_LEVEL(ch) = i;
			GET_EXP(ch) = LEVEL_EXP(i - 1) + 1;
			for (i = orig_lev + 1; i <= GET_LEVEL(ch); i++) {
				send_to_char("You raise a level\n\r", ch);
				li2000_advance_level(ch);
				sprintf(buf, "[%s] has gained level %d!", GET_REAL_NAME(ch), i);
				do_info(buf, 1, MAX_LEV, ch);
				is_altered = TRUE;
			}
			if (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV)
				GET_EXP(ch) -= GET_USEDEXP(ch);
			li2100_set_title(ch);
		}
	}

	/* LIMIT EXP LOSS */
	if (gain < 0) {
		gain = MAXV(-500000, gain);
		GET_EXP(ch) += gain;
		if (GET_EXP(ch) < 0)
			GET_EXP(ch) = 0;
	}
}				/* END OF li2200_gain_exp() */


void li2300_gain_nocheck_exp(struct char_data * ch, int gain)
{

	int lv_exp, idx, i;
	bool is_altered = FALSE;



	/*
	 if (!IS_NPC(ch))
	 return;
   */

	if (ch->in_room < 0) {
		send_to_char("You are in an invalid room.\r\n", ch);
		return;
	}

	if (gain > 0) {
		GET_EXP(ch) += gain;
		for (i = 1; (i < MAX_LEV) &&
		     (GET_EXP(ch) > (lv_exp = LEVEL_EXP(i))); i++) {
			idx = i;
			if (GET_EXP(ch) > lv_exp)
				idx++;
			if (idx > GET_LEVEL(ch)) {
				send_to_char("You raise a level.\n\r", ch);
				GET_LEVEL(ch) = idx;
				li2000_advance_level(ch);
				is_altered = TRUE;
			}
		}
	}

	if (gain < 0)
		GET_EXP(ch) += gain;
	if (GET_EXP(ch) < 0)
		GET_EXP(ch) = 0;

	if (is_altered)
		li2100_set_title(ch);
}

void li2300_gain_condition(struct char_data * ch, int condition, int value)
{

	bool lv_intoxicated;



	/* IF NO CHANGE - GET OUT */
	if (!value)
		return;

	/* IF OUT OF BOUNDS GET OUT */
	if (condition < 0 || condition > 2)
		return;

	/* IF A NPC, GET OUT */
	if (IS_NPC(ch))
		return;

	if (GET_COND(ch, condition) == -1)	/* No change */
		return;

	/* KEEP TRACK OF WHETHER WE ARE DRUNK OR NOT */
	lv_intoxicated = (GET_COND(ch, DRUNK) > 0);

	GET_COND(ch, condition) += value;
	GET_COND(ch, condition) = MAXV(0, GET_COND(ch, condition));

	/* MAKE SURE WE DON'T GO OVER MAX VALUE */
	switch (condition) {
	case FOOD:{
			if (GET_COND(ch, condition) >= races[GET_RACE(ch)].max_food)
				GET_COND(ch, condition) = races[GET_RACE(ch)].max_food;
			break;
		}
	case THIRST:{
			if (GET_COND(ch, condition) >= races[GET_RACE(ch)].max_thirst)
				GET_COND(ch, condition) = races[GET_RACE(ch)].max_thirst;
			break;
		}
	case DRUNK:{
			if (GET_COND(ch, condition) >= races[GET_RACE(ch)].max_drunk)
				GET_COND(ch, condition) = races[GET_RACE(ch)].max_drunk;
			break;
		}
	}

	/* IF WE STILL HAVE VALUE LEFT, skip OUT */
	if (GET_COND(ch, condition))
		return;

	switch (condition) {
	case FOOD:{
			send_to_char("You are hungry.\n\r", ch);
			return;
		}
	case THIRST:{
			send_to_char("You are thirsty.\n\r", ch);
			return;
		}
	case DRUNK:{
			if (lv_intoxicated) {
				send_to_char("You are now sober.\n\r", ch);
			}
			return;
		}
	default:
		break;
	}

}				/* END OF li2300_gain_condition() */

void li2400_check_idling(struct char_data * ch)
{
	int lv_rent_check = 0;
	struct obj_cost cost;
	char buf[MAX_STRING_LENGTH];



	if ((++(ch->specials.timer) > MORT_IDLE_TIMEOUT && GET_LEVEL(ch) < IMO_LEV) ||
		(ch->specials.timer > AVAT_IDLE_TIMEOUT && GET_LEVEL(ch) == IMO_LEV)) {
		if (ch->specials.was_in_room == NOWHERE && ch->in_room != NOWHERE) {
			if (ch->specials.fighting) {
				if (CANT_IDLE_IN_FIGHT) {
					return;	/* keep fighting till death */
				}
				else {
					ft1400_stop_fighting(ch->specials.fighting, 0);
					ft1400_stop_fighting(ch, 0);
				}
			}
			if (!IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
				ch->specials.was_in_room = ch->in_room;
				act("$n disappears into the void.", TRUE, ch, 0, 0, TO_ROOM);
				send_to_char("You have been idle, and are pulled into a void.\n\r", ch);
				ha1500_char_from_room(ch);
				ha1600_char_to_room(ch, 1);	/* Into room number 0 */
			}
		}
	}
	if ((ch->specials.timer > DROPLINK_TIMEOUT) && (!(ch->desc) || ch->in_room == 1)) {
		int save_room;

		bzero(buf, sizeof(buf));
		sprintf(buf, "Dropping player %s because of linkdeath.", GET_REAL_NAME(ch));
		do_connect(buf, GET_LEVEL(ch), ch);
		main_log(buf);
		spec_log(buf, CONNECT_LOG);

		if (ch->in_room != NOWHERE)
			ha1500_char_from_room(ch);
		if (ch->specials.was_in_room != NOWHERE)
			ha1600_char_to_room(ch, ch->specials.was_in_room);
		else
			ha1600_char_to_room(ch, 3008);
		//The dressing room.

		/* if we got the money rent us out */
		lv_rent_check = r2100_recep_offer(ch, ch, &cost, RECEPTION_IGNORE);
		cost.total_cost = cost.total_cost * 1.25;
		if (lv_rent_check) {
			save_room = NOWHERE;
			if (cost.total_cost < GET_GOLD(ch) + GET_INN_GOLD(ch)) {
				r4000_save_obj(ch, &cost);
				r3400_del_objs(ch);
				GET_HOW_LEFT(ch) = LEFT_BY_LINK_RENT;
				main_log("Leaving by link death - saving char");
				spec_log("Leaving by link death - saving char", CONNECT_LOG);
			}
			else {
				GET_HOW_LEFT(ch) = LEFT_BY_LINK_NO_$4_RENT;
				sprintf(buf, "%s is leaving by link death - No $4 rent", GET_REAL_NAME(ch));
				main_log(buf);
				spec_log(buf, RENT_GONE_LOG);
				wi2900_purge_items(ch, "\0", 0);
				if (r1100_rent_delete_file(GET_REAL_NAME(ch))) {
					main_log("Purged rent file. Player can't afford rent.\r\n");
					spec_log("Purged rent file. Player can't afford rent.\r\n", RENT_GONE_LOG);
				}
				else {
					main_log("Player can't afford rent, didn't have a rent file.\r\n");
					spec_log("Player can't afford rent, didn't have a rent file.\r\n", RENT_GONE_LOG);
				}
			}
			ha3000_extract_char(ch, END_EXTRACT_BY_EQ_TIMEOUT);
			ch->in_room = save_room;

			db6400_save_char(ch, ch->in_room == NOWHERE ? NOWHERE : world[ch->in_room].number);
		}
		else {
			GET_HOW_LEFT(ch) = LEFT_BY_LINK_NORENT;
			sprintf(buf, "%s is leaving by link death - NOrent", GET_REAL_NAME(ch));
			main_log(buf);
			spec_log(buf, RENT_GONE_LOG);
			ha3000_extract_char(ch, END_EXTRACT_BY_NOEQ_TIMEOUT);
			r2300_del_char_objs(ch);
		}

		/* close out the link the player */
		if (ch->desc)
			co2500_close_single_socket(ch->desc);
		ch->desc = 0;
	}

}
/* Update both PC's & NPC's and objects*/
void li2500_point_update(void)
{

	struct char_data *i, *next_dude;
	struct obj_data *j, *next_thing, *jj, *next_thing2;
	char buf2[MAX_STRING_LENGTH];
	static bool lv_flip_flop;
	int lv_gain, lv_max;



	/* WE NEED A WAY TO HANDLE PERCENTAGES less than 100%  */
	/* SO ON EVERY OTHER PASS, WE'LL TREAT IT AS A WHOLE    */
	/* FOR EXAMPLE, A USAGE OF 150% WOULD USE 2 ON THE TRUE */
	/* PASS AND 1 O THE FALSE PASS                          */
	if (lv_flip_flop == TRUE)
		lv_flip_flop = FALSE;
	else
		lv_flip_flop = TRUE;

	/* characters */
	for (i = character_list; i; i = next_dude) {
		next_dude = i->next;

		/* Tick message */
		if (IS_SET(GET_ACT2(i), PLR2_SHOW_TICK)) {
			send_to_char("Tick...\r\n", i);
		}

		/* Check Jail & muzzle timers */
		if (IS_SET(GET_ACT2(i), PLR2_JAILED)) {
			if ((GET_JAILTIME(i) - time(0)) <= 0) {
				send_to_char("You've been sprung.\r\n", i);
				sprintf(buf2, "%s has done his time in jail and has been released.\r\n",
				 (IS_NPC(i) ? GET_MOB_NAME(i) : GET_NAME(i)));
				do_wizinfo(buf2, IMO_IMM, i);
				spec_log(buf2, MAIN_LOG);
				/* MOVE THEM TO INN */
				REMOVE_BIT(GET_ACT2(i), PLR2_JAILED);
				GET_JAILTIME(i) = 0;
				ha1500_char_from_room(i);
				ha1600_char_to_room(i, db8000_real_room(REHAB_ROOM));
				in3000_do_look(i, "", 0);
			}
		}		/* end of timers */

		if ((GET_DSTIME(i)) && (GET_DSTIME(i) <= time(0))) {
			GET_DSTIME(i) = 0;	/* Set DeathShadow to 0 if time
						 * passed */
			send_to_char("The shadow of death has left you.\r\n", i);
		}

		lv_gain = li1800_hit_gain(i);
		if (i->in_room > 0) {
			//Array bounds checking.
				if (world[i->in_room].number == 3073)
				lv_gain *= (1 + (GET_LEVEL(i) >> 3));
			if (world[i->in_room].number == 5992)
				lv_gain *= (1 + (GET_LEVEL(i) >> 3));
		}
		lv_max = li1500_hit_limit(i);
		if (GET_DSTIME(i))
			lv_max >>= 1;

		GET_HIT(i) = MINV(GET_HIT(i) + lv_gain, lv_max);

		if (IS_NPC(i)) {
			if (GET_HIT(i) == lv_max) {
				/* MOB IS FULLY HEALED, SO IF IT WAS HELD WITH  */
				/* HOLD_PERSON PREVOUSLY, REMOVE THAT FLAG      */
				REMOVE_BIT(GET_ACT1(i), PLR1_CHAR_WAS_HELD);
			}
		}

		lv_gain = li1700_mana_gain(i);
		if (i->in_room > 0) {
			//Array bounds checking.
				if (world[i->in_room].number == 5992)
				lv_gain *= (1 + (GET_LEVEL(i) >> 3));
		}
		if (ha1375_affected_by_spell(i, SKILL_MEDITATE))
			lv_gain *= (1 + (GET_LEVEL(i) >> 2));
		lv_max = GET_SPECIAL_MANA(i);
		if (GET_DSTIME(i))
			lv_max >>= 1;
		GET_MANA(i) = MINV(GET_MANA(i) + lv_gain, lv_max);


		lv_gain = li1900_move_gain(i);
		if (i->in_room > 0) {
			//Array bounds checking.
				if (world[i->in_room].number == 5992)
				lv_gain *= (1 + (GET_LEVEL(i) >> 3));
			if (world[i->in_room].number == 3073)
				lv_gain *= (1 + (GET_LEVEL(i) >> 3));
		}
		lv_max = li1600_move_limit(i);
		if (GET_DSTIME(i))
			lv_max >>= 1;
		GET_MOVE(i) = MINV(GET_MOVE(i) + lv_gain, lv_max);

		if (GET_POS(i) == POSITION_INCAP)
			DAMAGE(i, i, 1, TYPE_SUFFERING);
		else if (!IS_NPC(i) && (GET_POS(i) == POSITION_MORTALLYW)) {
			DAMAGE(i, i, 2, TYPE_SUFFERING);
		}
		else if (GET_POS(i) == POSITION_STUNNED || GET_HIT(i) <= 0)
			ft1200_update_pos(i);
		if (!IS_NPC(i)) {
			ha2900_update_char_objects(i);
			li2400_check_idling(i);
		}

		if (IS_PC(i)) {

			/* HUNGER */
			if (lv_flip_flop) {
				/* IF THE CHAR GAINS A PERCENTAGE, GIVE IT NOW */
				if (races[GET_RACE(i)].adj_food -
				    (races[GET_RACE(i)].adj_food / 100) * 100)
					if (GET_COND(i, FOOD))
						li2300_gain_condition(i, FOOD, -1);
			}
			li2300_gain_condition(i, FOOD,
				      -1 * races[GET_RACE(i)].adj_food / 100);

			/* THIRST */
			if (lv_flip_flop) {
				/* IF THE CHAR GAINS A PERCENTAGE, GIVE IT NOW */
				if (races[GET_RACE(i)].adj_thirst -
				  (races[GET_RACE(i)].adj_thirst / 100) * 100)
					if (GET_COND(i, THIRST))
						li2300_gain_condition(i, THIRST, -1);
			}
			li2300_gain_condition(i, THIRST,
				    -1 * races[GET_RACE(i)].adj_thirst / 100);

			/* DRUNK */
			if (lv_flip_flop) {
				/* IF THE CHAR GAINS A PERCENTAGE, GIVE IT NOW */
				if (races[GET_RACE(i)].adj_drunk -
				    (races[GET_RACE(i)].adj_drunk / 100) * 100)
					if (GET_COND(i, DRUNK))
						li2300_gain_condition(i, DRUNK, -1);
			}
			li2300_gain_condition(i, DRUNK,
				     -1 * races[GET_RACE(i)].adj_drunk / 100);
		}

	}			/* END OF for loop */

	/* objects */
	for (j = object_list; j; j = next_thing) {
		next_thing = j->next;	/* Next in object list */

		/* If this is a corpse */
		if ((GET_ITEM_TYPE(j) == ITEM_CONTAINER)) {
			if (j->obj_flags.value[3]) {
				/* timer count down */
				if (GET_TIMER(j) > 0)
					TIMER(j)--;

				if (!GET_TIMER(j) || !j->contains) {

					if (j->carried_by)
						act("$p decay in your hands.", FALSE, j->carried_by, j, 0, TO_CHAR);
					else if ((j->in_room != NOWHERE) && (world[j->in_room].people)) {
						act("The $p decays into dust.", TRUE, world[j->in_room].people, j, 0, TO_ROOM);
						act("The $p decays into dust.", TRUE, world[j->in_room].people, j, 0, TO_CHAR);
					}

					for (jj = j->contains; jj; jj = next_thing2) {
						next_thing2 = jj->next_content;	/* Next in inventory */
						ha2400_obj_from_obj(jj);

						if (j->in_obj)
							ha2300_obj_to_obj(jj, j->in_obj);
						else if (j->carried_by)
							ha2100_obj_to_room(jj, j->carried_by->in_room);
						else if (j->in_room != NOWHERE)
							ha2100_obj_to_room(jj, j->in_room);
						else
							assert(FALSE);
					}	/* END OF for LOOP */
					ha2700_extract_obj(j);
					continue;
				}	/* END OF timer */
			}	/* END OF flags3.value */

			/* IF ITS THE PORTAL */
			if (j->item_number > -1 && obj_index[j->item_number].virtual == OBJ_PORTAL) {
				if (GET_TIMER(j) > 0)
					TIMER(j)--;

				if (GET_TIMER(j) == 1) {
					if (j->in_room != NOWHERE) {
						co3300_send_to_room("The portal begins flickering.\r\n",
								  j->in_room);
					}
				}

				if (!GET_TIMER(j)) {
					if (j->in_room != NOWHERE) {
						co3300_send_to_room("The portal flickers and fades from sight.\r\n",
								  j->in_room);
					}
					ha2700_extract_obj(j);
					continue;
				}

			}	/* END OF object PORTAL */

		}		/* END OF ITS A CONTAINER */
	}			/* END OF for object list */
}				/* END OF li2500_point_update() */


/* ROUTINE CHECKS TO SEE IF PLAYER CAN ENTER ROOM. */
/*       BIT0     SET MEANS CHECK MAGICAL ENTRY    */
/*       BIT1 NOT SET MEANS CHECK BOATS            */
/*       BIT2     SET MEANS IGNORE RM1_TUNNEL      */
int li3000_is_blocked_from_room(struct char_data * ch, int lv_real_room, int lv_flag)
{

	struct obj_data *obj;
	struct char_data *tmp_ch;
	int number_pc, number_npc, has_boat, zone;
	char buf[MAX_STRING_LENGTH];

	zone = world[lv_real_room].zone;

	/* VALIDATE ROOM */
	if (lv_real_room < 0 || lv_real_room > top_of_world) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s entered li3000 with invalid room %d.\r\n", GET_REAL_NAME(ch), lv_real_room);
		main_log(buf);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
		return (BLOCKED_BY_INVALID_ROOM);
	}

	if (IS_PC(ch) && GET_LEVEL(ch) >= IMO_IMP) {
		return (0);
	}

	if (IS_SET(zone_table[zone].flags, ZONE_NO_ENTER)) {
		if (!zone_table[zone].lord) {
			return (BLOCKED_BY_ZONE);
		}
		if (!ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord)) {
			return (BLOCKED_BY_ZONE);
		}
	}

	if (GET_LEVEL(ch) < world[lv_real_room].min_level) {
		return (BLOCKED_MIN_LEV);
	}

	if ((GET_LEVEL(ch) > world[lv_real_room].max_level) && (GET_LEVEL(ch) < IMO_SPIRIT)) {
		return (BLOCKED_MAX_LEV);
	}

	if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_SPIRIT) {
		if (IS_SET(lv_flag, BIT0)) {
			if (IS_SET(world[lv_real_room].room_flags,
				   RM1_NO_TELEPORT_IN)) {
				return (BLOCKED_BY_TELEPORT);
			}
		}
		if (IS_SET(GET_ROOM1(lv_real_room), RM1_TUNNEL)) {
			if (!IS_SET(lv_flag, BIT2)) {
				if (world[lv_real_room].people) {
					return (BLOCKED_BY_TUNNEL);
				}
			}
		}
		/* WOULD WE LEAVE ZONE? */
		if (IS_UNCHARMED_NPC(ch) &&
		    world[ch->in_room].zone != world[lv_real_room].zone) {
			if ((IS_SET(ch->specials.act, PLR1_STAY_ZONE))) {
				return (BLOCKED_BY_3);
			}
		}
	}			/* END OF level < IMO_SPIRIT */

	/* Morals Smorals */
	if ((IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMM) &&
	    IS_SET(GET_ROOM1(lv_real_room), RM1_PRIVATE)) {
		number_pc = 0;
		number_npc = 0;
		tmp_ch = world[lv_real_room].people;
		while (tmp_ch) {
			if (IS_PC(tmp_ch))
				number_pc++;
			else
				number_npc++;
			tmp_ch = tmp_ch->next_in_room;
		}
		if (number_pc > 1) {
			return (BLOCKED_BY_PRIVATE);
		}
	}			/* END OF morals smorals */

	/* GOING TO JAIL? */
	if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMM) {
		if (world[lv_real_room].number == JAIL_ROOM) {
			return (BLOCKED_BY_JAIL);
		}
	}

	if (world[lv_real_room].number == PET_ROOM) {
		return (BLOCKED_BY_PET);
	}
		if (world[lv_real_room].number == BRIMSTONE_PET_ROOM) {
		return (BLOCKED_BY_PET);
	}

	if (!IS_SET(lv_flag, BIT1) &&
	    (world[ch->in_room].sector_type == SECT_WATER_NOSWIM ||
	     world[lv_real_room].sector_type == SECT_UNDERWATER ||
	     world[lv_real_room].sector_type == SECT_WATER_NOSWIM)) {
		has_boat = FALSE;
		/* See if char is carrying a boat */
		for (obj = ch->carrying; obj; obj = obj->next_content) {
			if (obj->obj_flags.type_flag == ITEM_BOAT)
				has_boat = TRUE;
		}		/* END OF for loop */

		if ((GET_LEVEL(ch) < IMO_SPIRIT) &&
		    !(IS_AFFECTED(ch, AFF_WATERWALK)) &&
		    !has_boat &&
		    world[lv_real_room].sector_type == SECT_WATER_NOSWIM) {
			return (BLOCKED_BY_NOBOAT);
		}
		else {
			if (has_boat &&
			 world[lv_real_room].sector_type == SECT_UNDERWATER) {
				return (BLOCKED_BY_HASBOAT);
			}
			return (FALSE);
		}
	}			/* END OF ITS A WATER WORLD OUT THERE */

	if (GET_LEVEL(ch) < IMO_LEV &&
	    ((IS_SET(world[lv_real_room].room_flags, RM1_ANTI_GOOD) && IS_GOOD(ch)) ||
	     (IS_SET(world[lv_real_room].room_flags, RM1_ANTI_NEUTRAL) && IS_NEUTRAL(ch)) ||
	     (IS_SET(world[lv_real_room].room_flags, RM1_ANTI_EVIL) && IS_EVIL(ch)))) {
		return (BLOCKED_BY_ALIGNMENT);
	}			/* END OF BLOCKED BY ALIGNMENT */

	return (0);

}				/* END OF li3000_is_blocked_from_room() */

void li3100_send_blocked_text(struct char_data * ch, int lv_error, int lv_flag)
{
	char buf[MAX_STRING_LENGTH];



	bzero(buf, sizeof(buf));
	switch (lv_error) {
	case BLOCKED_MIN_LEV:{
			sprintf(buf, "Your level isn't high enough.\r\n");
			break;
		}
	case BLOCKED_MAX_LEV:{
			sprintf(buf, "Your level is too high.\r\n");
			break;
		}
	case BLOCKED_BY_TELEPORT:{
			sprintf(buf, "You can't teleport to that room.\r\n");
			break;
		}
	case BLOCKED_BY_TUNNEL:{
			sprintf(buf, "Its too crowded in there!\r\n");
			break;
		}
	case BLOCKED_BY_PRIVATE:{
			sprintf(buf, "There is a private conversation going on in that room.\r\n");
			break;
		}
	case BLOCKED_BY_JAIL:{
			sprintf(buf, "Thats jail!  You wouldn't want to go there.\r\n");
			break;
		}
	case BLOCKED_BY_PET:{
			sprintf(buf, "The pet shop is reserved and off limits to you.\r\n");
			break;
		}
	case BLOCKED_BY_NOBOAT:{
			sprintf(buf, "You need a boat to go there.\r\n");
			break;
		}
	case BLOCKED_BY_HASBOAT:{
			sprintf(buf, "You can't go there with a boat.\r\n");
			break;
		}
	case BLOCKED_BY_ALIGNMENT:{
			sprintf(buf, "Your ways prevent you from going there.\r\n");
			break;
		}
	case BLOCKED_BY_ZONE:{
			sprintf(buf, "Sorry, that zone is currently under construction.\r\n");
			break;
		}
	case BLOCKED_BY_INVALID_ROOM:{
			sprintf(buf, "Sorry, room is invalid.\r\n");
			break;
		}
	case BLOCKED_BY_3:{
			sprintf(buf, "Sorry, You gotta stay in your zone.\r\n");
			break;
		}
	default:{
			sprintf(buf, "You can't go there because of value: %d.\r\n",
				lv_error);
		}
	}			/* END OF switch() */
	send_to_char(buf, ch);

	return;

}				/* END OF li3100_send_blocked_text() */


/* following routine takes four flag values  */
/*    1 = sneak adjustment                   */
/*    2 = hide adjustment                    */
/*    3 = pick pocket adjustment             */
/*    4 = defensive adjustment               */

int li9650_dex_adjustment(struct char_data * ch, int lv_flag)
{

	int lv_value, lv_dex;

	struct dex_skill_type {
		sh_int p_pocket;
		sh_int p_locks;
		sh_int traps;
		sh_int sneak;
		sh_int hide;
	};

	/* [dex] skill apply (thieves only) */
	const struct dex_skill_type dex_app_skill[31] = {
		{-99, -99, -90, -99, -60},	/* 0 */
		{-90, -90, -60, -90, -50},	/* 1 */
		{-80, -80, -40, -80, -45},
		{-70, -70, -30, -70, -40},
		{-60, -60, -30, -60, -35},
		{-50, -50, -20, -50, -30},	/* 5 */
		{-40, -40, -20, -40, -25},
		{-30, -30, -15, -30, -20},
		{-20, -20, -15, -20, -15},
		{-15, -10, -10, -20, -10},
		{-10, -5, -10, -15, -5},	/* 10 */
		{-5, 0, -5, -10, 0},
		{0, 0, 0, -5, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},/* 15 */
		{0, 5, 0, 0, 0},
		{5, 10, 0, 5, 5},
		{10, 15, 5, 10, 10},
		{15, 20, 10, 15, 15},
		{15, 20, 10, 15, 15},	/* 20 */
		{20, 25, 10, 15, 20},
		{20, 25, 15, 20, 20},
		{25, 25, 15, 20, 20},
		{25, 30, 15, 25, 25},
		{25, 30, 15, 25, 25},	/* 25 */
		{30, 35, 20, 20, 30},
		{35, 40, 25, 25, 35},
		{40, 45, 30, 30, 40},
		{45, 50, 35, 35, 45},
		{99, 99, 99, 99, 99}	/* 30 */
	};

	struct dex_app_type {
		sh_int reaction;
		sh_int miss_att;
		sh_int defensive;
	};

	/* [dex] apply (all) */
	const struct dex_app_type dex_app[31] = {
		{-6, -6, 6},	/* 0 */
		{-5, -5, 5},	/* 1 */
		{-4, -4, 5},
		{-3, -3, 4},
		{-2, -2, 3},
		{-1, -1, 2},	/* 5 */
		{0, 0, 1},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},	/* 10 */
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{1, 0, -1},	/* 15 */
		{1, 1, -2},
		{2, 2, -3},
		{2, 2, -4},
		{3, 3, -4},
		{3, 3, -4},	/* 20 */
		{4, 4, -5},
		{4, 4, -5},
		{5, 4, -5},
		{5, 5, -6},
		{6, 5, -6},	/* 25 */
		{6, 6, -7},
		{7, 7, -8},
		{7, 8, -9},
		{8, 9, -10},
		{9, 10, -11}	/* 30 */
	};




	if (GET_DEX(ch) > 25)
		lv_dex = 25;
	else
		lv_dex = GET_DEX(ch);

	lv_value = 0;
	switch (lv_flag) {
	case 1:{
			lv_value = dex_app_skill[lv_dex].sneak;
			break;
		}
	case 2:{
			lv_value = dex_app_skill[lv_dex].hide;
			break;
		}
	case 3:{
			lv_value = dex_app_skill[lv_dex].p_pocket;
			break;
		}
	case 4:{
			lv_value = dex_app[lv_dex].defensive;
			break;
		}
	default:{
			/* WE'LL never get here */
			lv_value = 0;
			break;
		}
	}			/* END OF switch */

	return (lv_value);

}				/* END OF li9650_dex_adjustment() */

int li9675_thaco(struct char_data * ch)
{

	int lv_thaco;



	lv_thaco = 30.5 - (classes[GET_CLASS(ch)].thaco_numerator *
		    GET_LEVEL(ch) / classes[GET_CLASS(ch)].thaco_denominator);

	if (IS_PC(ch) && GET_LEVEL(ch) > IMO_IMM)
		lv_thaco = -30;

	if (IS_PC(ch) && GET_LEVEL(ch) == IMO_LEV)
		lv_thaco = -10;

	if (IS_PC(ch) && GET_LEVEL(ch) == PK_LEV)
		lv_thaco = -11;

	if (IS_NPC(ch) && GET_LEVEL(ch) < PK_LEV)
		lv_thaco = -10;

	if (IS_NPC(ch) && GET_LEVEL(ch) > PK_LEV + 2)
		lv_thaco = -15;

	return (lv_thaco);

}				/* END OF li9675_thaco() */

int li9700_adjust_damage_bonus(int lv_str)
{

	int lv_str_damage_bonus;



	if (lv_str > 12) {
		lv_str_damage_bonus = lv_str - 12;
	}
	else {
		if (lv_str > 0 && lv_str < 9) {
			lv_str_damage_bonus = lv_str - 9;
		}
		else {
			lv_str_damage_bonus = 0;
		}
	}

	return (lv_str_damage_bonus);

}				/* END OF li9700_adjust_damage_bonus() */

int li9725_adjust_tohit_bonus(int lv_str)
{

	int lv_str_tohit_bonus;



	if (lv_str > 15) {
		lv_str_tohit_bonus = (lv_str / 2) - 6;
	}
	else {
		if (lv_str > 0 && lv_str < 6) {
			lv_str_tohit_bonus = lv_str - 6;
		}
		else {
			lv_str_tohit_bonus = 0;
		}
	}

	return (lv_str_tohit_bonus);

}				/* END OF li9725_adjust_tohit_bonus() */

int li9750_ac_bonus(int lv_dex)
{

	int lv_dex_ac_bonus;



	if (lv_dex > 17) {
		lv_dex_ac_bonus = (lv_dex - 17) * -10;
	}
	else {
		if (lv_dex > 0 && lv_dex < 10) {
			lv_dex_ac_bonus = (10 - lv_dex) * 10;
		}
		else {
			lv_dex_ac_bonus = 0;
		}
	}

	return (lv_dex_ac_bonus);

}				/* END OF li9750_ac_bonus() */

void li9800_set_alignment(struct char_data * ch)
{



	/* DEFAULT ALIGNMENT */
	GET_ALIGNMENT(ch) = 0;

	/* SPECIAL ALIGNMENTS? */
	if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_EVIL_ALIGNMENT) ||
	    IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_EVIL_ALIGNMENT)) {
		GET_ALIGNMENT(ch) = -1000;
		return;
	}
	if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_GOOD_ALIGNMENT) ||
	    IS_SET(classes[GET_CLASS(ch)].flag, CFLAG_GOOD_ALIGNMENT)) {
		GET_ALIGNMENT(ch) = 1000;
		return;
	}

	return;

}				/* END OF void li9800_set_alignment() */


void li9850_set_conditions(struct char_data * ch)
{



	if (GET_LEVEL(ch) > 2 &&
	    GET_LEVEL(ch) < IMO_LEV) {
		GET_COND(ch, FOOD) = races[GET_RACE(ch)].max_food;
		GET_COND(ch, THIRST) = races[GET_RACE(ch)].max_thirst;
		GET_COND(ch, DRUNK) = 0;
	}
	else {
		GET_COND(ch, FOOD) = -1;
		GET_COND(ch, THIRST) = -1;
		GET_COND(ch, DRUNK) = -1;
	}

	/* IF THEY DON'T EAT/DRINK/DRUNK SET TO -1 */
	if (races[GET_RACE(ch)].adj_food == 0)
		GET_COND(ch, FOOD) = -1;
	if (races[GET_RACE(ch)].adj_thirst == 0)
		GET_COND(ch, THIRST) = -1;
	if (races[GET_RACE(ch)].adj_drunk == 0)
		GET_COND(ch, DRUNK) = 0;

	return;


}				/* END OF li9850_set_conditions() */


void li9900_gain_proficiency(struct char_data * ch, int lv_skill)
{

	char buf[MAX_STRING_LENGTH];



	if (IS_NPC(ch))
		return;

	/* CAN WE BEGIN LEARNING ON OUR OWN? */
	if (ch->skills[lv_skill].learned < 50)
		return;

	/* CAN PROFICIENCY GO UP? */
	if (ch->skills[lv_skill].learned <
	    classes[GET_CLASS(ch)].skill_max[lv_skill] &&
	    (ch->skills[lv_skill].learned <
	     races[GET_RACE(ch)].skill_max[lv_skill])) {
		/* FIRST HURDLE TO GAINING */
		if (number(0, 100) > GET_SKILL(ch, lv_skill)) {
			/* IT ONLY HAPPENS a % of the time */
			if (number(0, 50) < 3) {
				if (lv_skill == SKILL_PARRY &&
				    GET_LEVEL(ch) < 20) {
					return;
				}
				ch->skills[lv_skill].learned++;
				sprintf(buf, "You gain skill in %s.\r\n",
					spell_names[lv_skill - 1]);
				send_to_char(buf, ch);
			}	/* END OF % of the time */
		}		/* END OF % of first hurdle */
	}			/* END OF prof not at max */

	if (ch->skills[lv_skill].learned > 99)
		ch->skills[lv_skill].learned = 99;

	return;

}				/* END OF 19900_gain_proficiency() */
