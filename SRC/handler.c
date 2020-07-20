/* ha */
/* gv_location: 5501-6000 */
/* *********************************************************************
*  file: handler.c , Handler module.                   Part of DIKUMUD *
*  Usage: Various routines for moving about objects/players            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infol.    *
********************************************************************* */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "parser.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "spells.h"
#include "constants.h"
#include "xanth.h"
#include "ansi.h"
#include "globals.h"
#include "func.h"

/* Checks string *arg for reserverd words *lv_reserved.  If lv_flag  */
/* BIT0 ON tells routine to convert *arg to lower case.              */
/*      OFF tells routine to take it as it is                        */
/* BIT1 ON  tells routine to check only the first part of the string */
/*      OFF tells routine to check the whole string                  */
char *ha1000_is_there_a_reserved_word(char *arg, const char *lv_reserved[], int lv_flag)
{
	int my_len, loc, idx, jdx;
	char buf[MAX_STRING_LENGTH], *ptr1, *ptr2;




	if (!(*arg)) {
		return (0);
	}

	bzero(buf, sizeof(buf));
	strcpy(buf, arg);
	if (IS_SET(lv_flag, BIT0)) {
		for (idx = 0; idx < strlen(buf); idx++)
			buf[idx] = LOWER(buf[idx]);
	}

	my_len = strlen(arg);
	loc = 0;

	/* CHECK PREFIX */
	if (IS_SET(lv_flag, BIT1)) {
		for (jdx = 0; lv_reserved[jdx][0] != '\n'; jdx++) {
			ptr2 = (char *) lv_reserved[jdx];
			if (is_abbrev(ptr2, arg)) {
				return (ptr2);
			}
		}
		return (FALSE);
	}

	/* SEARCH THROUGH STRING */
	for (idx = 0; idx < my_len; idx++) {
		for (jdx = 0; lv_reserved[jdx][0] != '\n'; jdx++) {
			ptr1 = (char *) (buf + idx);
			ptr2 = (char *) lv_reserved[jdx];
			if (strncmp(ptr1, ptr2, strlen(ptr2)) == 0)
				return (ptr2);
		}		/* END OF for jdx loop */
	}			/* END OF for idx loop */
	return (FALSE);

}				/* END OF ha1000_is_there_a_reserved_word() */


int ha1050_is_there_a_curse_word(char *arg)
{

	int my_len, loc, idx;
	char buf[MAX_STRING_LENGTH];



	if (!(*arg))
		return (0);

	bzero(buf, sizeof(buf));
	strcpy(buf, arg);
	for (idx = 0; idx < strlen(buf); idx++)
		buf[idx] = LOWER(buf[idx]);

	my_len = strlen(arg);
	loc = 0;
	for (idx = 0; idx < my_len; idx++) {
		if (strncmp((char *) (buf + idx), "muzzle_me_please", 16) == 0)
			return (1);
	}
	return (0);


}				/* END OF ha1050_is_there_a_curse_word() */


char *ha1100_fname(char *namelist)
{

	static char holder[80];
	register char *point;



	if (!strncmp(namelist, "obj ", 4) ||
	    !strncmp(namelist, "mob ", 4))
		namelist += 4;	/* skip leading "obj " or "mob " */
	for (point = holder; isalpha(*namelist); namelist++, point++)
		*point = *namelist;

	*point = '\0';

	return (holder);

}				/* END OF ha1100_fname() */


int ha1150_isname(char *str, char *namelist)
{

	register char *curname, *curstr;


	if (!str || !namelist)
		return 0;
	curname = namelist;
	for (;;) {
		for (curstr = str;; curstr++, curname++) {
			if (!*curstr)
				return (1);

			if (!*curname)
				return (0);

			if (!*curstr || *curname == ' ')
				break;

			if (LOWER(*curstr) != LOWER(*curname))
				break;
		}

		/* skip to next name */
		for (; isalpha(*curname); curname++);
		if (!*curname)
			return (0);
		curname++;	/* first char of new name */
	}

}				/* END OF ha1150_isname() */


int ha1175_isexactname(char *str, char *namelist)
{

	register char *curname, *curstr;



	if (!str || !namelist)
		return 0;
	curname = namelist;
	for (;;) {
		for (curstr = str;; curstr++, curname++) {
			if (!*curstr && !isalpha(*curname))
				return (1);

			if (!*curname)
				return (0);

			if (!*curstr || *curname == ' ')
				break;

			if (LOWER(*curstr) != LOWER(*curname))
				break;
		}

		/* skip to next name */
		for (; isalpha(*curname); curname++);
		if (!*curname)
			return (0);
		curname++;	/* first char of new name */
	}
}				/* END OF ha1175_isexactname() */


void ha1200_affect_modify(struct char_data * ch, signed char loc, signed char mod, long bitv, bool add)
{

	struct affected_type af;
	char buf[MAX_STRING_LENGTH];
	int ac = 0;



	af.duration = -1;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	af.bitvector2 = 0;
	af.bitvector3 = 0;
	af.bitvector4 = 0;
	if (add) {
		if (bitv)
			SET_BIT(ch->specials.affected_by, bitv);
	}
	else {
		if (bitv)
			REMOVE_BIT(ch->specials.affected_by, bitv);
		mod = -mod;
	}

	switch (loc) {
	case APPLY_NONE:
		break;

	case APPLY_STR:{
			if (GET_STR(ch) + mod > races[GET_RACE(ch)].max_str)
				GET_STR(ch) = races[GET_RACE(ch)].max_str;
			else if (GET_STR(ch) + mod < 3)
				GET_STR(ch) = 3;
			else
				GET_STR(ch) += mod;
			break;
		}
	case APPLY_BONUS_STR:{
			GET_BONUS_STR(ch) += mod;
			break;
		}

	case APPLY_DEX:{
			/* REMOVE DEX ADJUSTMENT TO AC */
			if (GET_DEX(ch) + mod > races[GET_RACE(ch)].max_dex)
				GET_DEX(ch) = races[GET_RACE(ch)].max_dex;
			else if (GET_DEX(ch) + mod < 3)
				GET_DEX(ch) = 3;
			else
				GET_DEX(ch) += mod;

			break;
		}
	case APPLY_BONUS_DEX:{
			GET_BONUS_DEX(ch) += mod;
			break;
		}

	case APPLY_INT:{
			if (GET_INT(ch) + mod > races[GET_RACE(ch)].max_int)
				GET_INT(ch) = races[GET_RACE(ch)].max_int;
			else if (GET_INT(ch) + mod < 3)
				GET_INT(ch) = 3;
			else
				GET_INT(ch) += mod;
			break;
		}

	case APPLY_WIS:{
			if (GET_WIS(ch) + mod > races[GET_RACE(ch)].max_wis)
				GET_WIS(ch) = races[GET_RACE(ch)].max_wis;
			else if (GET_WIS(ch) + mod < 3)
				GET_WIS(ch) = 3;
			else
				GET_WIS(ch) += mod;
			break;
		}

	case APPLY_CON:{
			if (GET_CON(ch) + mod > races[GET_RACE(ch)].max_con)
				GET_CON(ch) = races[GET_RACE(ch)].max_con;
			else if (GET_CON(ch) + mod < 3)
				GET_CON(ch) = 3;
			else
				GET_CON(ch) += mod;
			break;
		}

	case APPLY_CHA:{
			if (GET_CHA(ch) + mod > races[GET_RACE(ch)].max_cha)
				GET_CHA(ch) = races[GET_RACE(ch)].max_cha;
			else if (GET_CHA(ch) + mod < 3)
				GET_CHA(ch) = 3;
			else
				GET_CHA(ch) += mod;
			break;
		}
	case APPLY_BONUS_CHA:{
			GET_BONUS_CHA(ch) += mod;
			break;
		}

	case APPLY_CLASS:
	case APPLY_LEVEL:
		break;

	case APPLY_AGE:
		/* bugged, temporarily disabled */
		break;
		ch->player.time.birth -= mod * SECS_PER_MUD_YEAR;
		if (z_age(ch).year < 1) {
			send_to_char("Oh Oh that item just de-aged you prior to birth... (you're now dead)\n\r", ch);
			ft1700_death_cry(ch);
			GET_HOW_LEFT(ch) = LEFT_BY_DEATH_AGE;
			if (gv_mega_verbose_messages == TRUE) {
				main_log("Leaving by death age");

			}
			ha3000_extract_char(ch, END_EXTRACT_BY_AGE);
		}
		break;

	case APPLY_CHAR_WEIGHT:
		GET_WEIGHT(ch) += mod;
		break;

	case APPLY_CHAR_HEIGHT:
		GET_HEIGHT(ch) += mod;
		break;

	case APPLY_MANA:
		ch->points.max_mana += mod;
		break;

	case APPLY_HIT:
		ch->points.max_hit += mod;
		break;

	case APPLY_MOVE:
		ch->points.max_move += mod;
		break;

	case APPLY_GOLD:
	case APPLY_EXP:
		break;

		/* Odin: sets  limits to ac applies (3/3/01) */
	case APPLY_AC:
		ac = GET_AC(ch);

		if ((GET_AC(ch) + mod) > 400 || (GET_AC(ch) + mod) < -400)
			GET_AC(ch) = ac;
			else
			GET_AC(ch) += mod;
		break;

	case APPLY_HITROLL:
		GET_HITROLL(ch) += mod;
		break;

	case APPLY_DAMROLL:
		GET_DAMROLL(ch) += mod;
		break;

	case APPLY_SAVING_PARA:
		ch->specials.apply_saving_throw[0] += mod;
		break;

	case APPLY_SAVING_ROD:
		ch->specials.apply_saving_throw[1] += mod;
		break;

	case APPLY_SAVING_PETRI:
		ch->specials.apply_saving_throw[2] += mod;
		break;

	case APPLY_SAVING_BREATH:
		ch->specials.apply_saving_throw[3] += mod;
		break;

	case APPLY_SAVING_SPELL:
		ch->specials.apply_saving_throw[4] += mod;
		break;

	case APPLY_EXTRA_ATTACKS:
		ch->points.extra_hits += mod;
		break;

	case APPLY_DETECT_INVIS:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_DETECT_INVISIBLE)) {
				af.type = SPELL_DETECT_INVISIBLE;
				af.bitvector = AFF_DETECT_INVISIBLE;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_DETECT_INVISIBLE))
			ha1350_affect_from_char(ch, SPELL_DETECT_INVISIBLE);
		break;

	case APPLY_DARKSIGHT:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_DARKSIGHT)) {
				af.type = SPELL_DARKSIGHT;
				af.bitvector = AFF_DARKSIGHT;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_DARKSIGHT))
			ha1350_affect_from_char(ch, SPELL_DARKSIGHT);
		break;

	case APPLY_MAGIC_RESIST:
		if (add) {
			if (!ha1375_affected_by_spell(ch,
						      SPELL_MAGIC_RESIST)) {
				af.type = SPELL_MAGIC_RESIST;
				af.bitvector = AFF_MAGIC_RESIST;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch,
						  SPELL_MAGIC_RESIST))
			ha1350_affect_from_char(ch,
						SPELL_MAGIC_RESIST);
		break;

	case APPLY_MAGIC_IMMUNE:
		if (add) {
			if (!ha1375_affected_by_spell(ch,
						      SPELL_MAGIC_IMMUNE)) {
				af.type = SPELL_MAGIC_IMMUNE;
				af.bitvector = AFF_MAGIC_IMMUNE;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch,
						  SPELL_MAGIC_IMMUNE))
			ha1350_affect_from_char(ch,
						SPELL_MAGIC_IMMUNE);
		break;

	case APPLY_BREATHWATER:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_BREATHWATER)) {
				af.type = SPELL_BREATHWATER;
				af.bitvector = AFF_BREATHWATER;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_BREATHWATER))
			ha1350_affect_from_char(ch, SPELL_BREATHWATER);
		break;

	case APPLY_DETECT_MAGIC:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_DETECT_MAGIC)) {
				af.type = SPELL_DETECT_MAGIC;
				af.bitvector = AFF_DETECT_MAGIC;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_DETECT_MAGIC))
			ha1350_affect_from_char(ch, SPELL_DETECT_MAGIC);
		break;

	case APPLY_SENSE_LIFE:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_SENSE_LIFE)) {
				af.type = SPELL_SENSE_LIFE;
				af.bitvector = AFF_SENSE_LIFE;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_SENSE_LIFE))
			ha1350_affect_from_char(ch, SPELL_SENSE_LIFE);
		break;

	case APPLY_DETECT_EVIL:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_DETECT_EVIL)) {
				af.type = SPELL_DETECT_EVIL;
				af.bitvector = AFF_DETECT_EVIL;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_DETECT_EVIL))
			ha1350_affect_from_char(ch, SPELL_DETECT_EVIL);
		break;

	case APPLY_SNEAK:
		break;

	case APPLY_INVIS:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_INVISIBLE)
			&& !ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS)
				) {
				af.type = SPELL_INVISIBLE;
				af.bitvector = AFF_INVISIBLE;
				af.location = APPLY_AC;
				af.modifier = -40;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_INVISIBLE)) {
			ha1350_affect_from_char(ch, SPELL_INVISIBLE);
		}
		break;

	case APPLY_IMPROVED_INVIS:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_INVISIBLE)
			&& !ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS)
				) {
				af.type = SPELL_IMPROVED_INVIS;
				af.bitvector = AFF_INVISIBLE;
				af.location = APPLY_AC;
				af.modifier = -40;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS)) {
			ha1350_affect_from_char(ch, SPELL_IMPROVED_INVIS);
		}
		break;

	case APPLY_REGENERATION:
		if (add) {
			if (!ha1375_affected_by_spell(ch,
						      SPELL_REGENERATION)) {
				af.type = SPELL_REGENERATION;
				af.bitvector = AFF_REGENERATION;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch,
						  SPELL_REGENERATION))
			ha1350_affect_from_char(ch,
						SPELL_REGENERATION);
		break;

	case APPLY_HOLD_PERSON:
		if (add) {
			if (!ha1375_affected_by_spell(ch,
						      SPELL_HOLD_PERSON)) {
				af.type = SPELL_HOLD_PERSON;
				af.bitvector = AFF_HOLD_PERSON;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch,
						  SPELL_HOLD_PERSON))
			ha1350_affect_from_char(ch,
						SPELL_HOLD_PERSON);
		break;

	case APPLY_HASTE:
		if (add) {
			if (!ha1375_affected_by_spell(ch,
						      SPELL_HASTE)) {
				af.type = SPELL_HASTE;
				af.bitvector = AFF_HASTE;
				ha1300_affect_to_char(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch,
						  SPELL_HASTE))
			ha1350_affect_from_char(ch,
						SPELL_HASTE);
		break;

	default:
		sprintf(buf, "unknown apply (handler.c) %d", loc);
		main_log(buf);
		break;
	}			/* switch */
}


void ha1210_affect2_modify(struct char_data * ch, signed char loc, signed char mod, long bitv, bool add)
{

	struct affected_type af;
	char buf[MAX_STRING_LENGTH];

	af.duration = -1;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	af.bitvector2 = 0;
	af.bitvector3 = 0;
	af.bitvector4 = 0;
	if (add) {
		if (bitv)
			SET_BIT(ch->specials.affected_02, bitv);
	}
	else {
		if (bitv)
			REMOVE_BIT(ch->specials.affected_02, bitv);
		mod = -mod;
	}

	switch (loc) {
	case APPLY_NONE:
		break;

	case AFF2_SANCTUARY_MEDIUM:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_SANCTUARY_MEDIUM)) {
				af.type = SPELL_SANCTUARY_MEDIUM;
				af.bitvector2 = AFF2_SANCTUARY_MEDIUM;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_SANCTUARY_MEDIUM))
			ha1350_affect_from_char(ch, SPELL_SANCTUARY_MEDIUM);

		break;

	case AFF2_SANCTUARY_MINOR:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_SANCTUARY_MINOR)) {
				af.type = SPELL_SANCTUARY_MINOR;
				af.bitvector2 = AFF2_SANCTUARY_MINOR;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_SANCTUARY_MINOR))
			ha1350_affect_from_char(ch, SPELL_SANCTUARY_MINOR);

		break;

	case APPLY_SLEEP_IMMUNITY:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_SLEEP_IMMUNITY)) {
				af.type = SPELL_SLEEP_IMMUNITY;
				af.bitvector2 = AFF2_SLEEP_IMMUNITY;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_SLEEP_IMMUNITY))
			ha1350_affect_from_char(ch, SPELL_SLEEP_IMMUNITY);
		break;

	case AFF2_BARKSKIN:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_BARKSKIN)) {
				af.type = SPELL_BARKSKIN;
				af.bitvector2 = AFF2_BARKSKIN;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_BARKSKIN))
			ha1350_affect_from_char(ch, SPELL_BARKSKIN);

		break;

	case AFF2_STONESKIN:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_STONESKIN)) {
				af.type = SPELL_STONESKIN;
				af.bitvector2 = AFF2_STONESKIN;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_STONESKIN))
			ha1350_affect_from_char(ch, SPELL_STONESKIN);

		break;

	case APPLY_DETECT_UNDEAD:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_DETECT_UNDEAD)) {
				af.type = SPELL_DETECT_UNDEAD;
				af.bitvector2 = AFF2_DETECT_UNDEAD;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_DETECT_UNDEAD))
			ha1350_affect_from_char(ch, SPELL_DETECT_UNDEAD);
		break;

	case AFF2_FAERIE_FIRE:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_FAERIE_FIRE)) {
				af.type = SPELL_FAERIE_FIRE;
				af.bitvector2 = AFF2_FAERIE_FIRE;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_FAERIE_FIRE))
			ha1350_affect_from_char(ch, SPELL_FAERIE_FIRE);

		break;

	case AFF2_FAERIE_FOG:
		if (add) {
			if (!ha1375_affected_by_spell(ch, SPELL_FAERIE_FOG)) {
				af.type = SPELL_FAERIE_FOG;
				af.bitvector2 = AFF2_FAERIE_FOG;
				ha1300_affect_to_char2(ch, &af);
			}
		}
		else if (ha1375_affected_by_spell(ch, SPELL_FAERIE_FOG))
			ha1350_affect_from_char(ch, SPELL_FAERIE_FOG);

		break;

	default:
		sprintf(buf, "unknown apply (handler.c) %d", loc);
		main_log(buf);
		break;
	}			/* switch */
}
/* This updates a character by subtracting everything he is affected by */
/* restoring original abilities, and then affecting all again           */
/* this is also a really stupid way to do things, especially when you consider
wear all and the fact that it makes spells on items impossible, hence its
mostly not used */
void ha1225_affect_total(struct char_data * ch)
{
	struct affected_type *af;
	int i, j;



	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i])
			for (j = 0; j < MAX_OBJ_AFFECT; j++)
				ha1200_affect_modify(ch, ch->equipment[i]->affected[j].location,
				       ch->equipment[i]->affected[j].modifier,
				ch->equipment[i]->obj_flags.bitvector, FALSE);
	}
	for (af = ch->affected; af; af = af->next)
		ha1200_affect_modify(ch, af->location, af->modifier, af->bitvector, FALSE);

	ch->tmpabilities = ch->abilities;
	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i])
			for (j = 0; j < MAX_OBJ_AFFECT; j++)
				ha1200_affect_modify(ch, ch->equipment[i]->affected[j].location,
				       ch->equipment[i]->affected[j].modifier,
				 ch->equipment[i]->obj_flags.bitvector, TRUE);
	}
	for (af = ch->affected; af; af = af->next)
		ha1200_affect_modify(ch, af->location, af->modifier, af->bitvector, TRUE);

	/* Make certain values are between 0..max, not < 0 and not > max! */
	GET_STR(ch) = MAXV(3, MINV(GET_STR(ch), races[GET_RACE(ch)].max_str));
	GET_INT(ch) = MAXV(3, MINV(GET_INT(ch), races[GET_RACE(ch)].max_int));
	GET_DEX(ch) = MAXV(3, MINV(GET_DEX(ch), races[GET_RACE(ch)].max_dex));
	GET_WIS(ch) = MAXV(3, MINV(GET_WIS(ch), races[GET_RACE(ch)].max_wis));
	GET_CON(ch) = MAXV(3, MINV(GET_CON(ch), races[GET_RACE(ch)].max_con));
	GET_CHA(ch) = MAXV(3, MINV(GET_CHA(ch), races[GET_RACE(ch)].max_cha));


}
/* This affects all of a particular location type to a char
*/
void one_ha1225_affect_total(struct char_data * ch, sbyte location)
{
	struct affected_type *af;
	int i, j;



	/* since we are only going to change the the one ability dont bother to
	 * remove anything */
	/* depends on the ability in question being reset prior to this being
	 * called */
	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i])
			for (j = 0; j < MAX_OBJ_AFFECT; j++)
				if (ch->equipment[i]->affected[j].location == location)
					ha1200_affect_modify(ch,
					ch->equipment[i]->affected[j].location,
					ch->equipment[i]->affected[j].modifier,
							     ch->equipment[i]->obj_flags.bitvector, TRUE);
	}
	for (af = ch->affected; af; af = af->next)
		if (af->location == location)
			ha1200_affect_modify(ch, af->location, af->modifier, af->bitvector, TRUE);
}


/* This updates a character by subtracting everything he is affected */
/* by.  Then restoring original abilities, and then affecting all    */
/* again except it will only affect the !ONE! ability is absolutely  */
/* has to called by ha1300_affect_to_char, ha1350_affect_from_char,    */
/* ha1925_equip_char and ha1930_unequip_char make sure for equip_char  */
/* unequip char it gets inserted into the for loop after each call   */
/* to ha1200_affect_modify       */

void smart_ha1225_affect_total(struct char_data * ch, sbyte location)
{



	if (location == APPLY_STR) {
		GET_STR(ch) = ch->abilities.str;	/* reset str to original */
		one_ha1225_affect_total(ch, APPLY_STR);
		GET_STR(ch) = MAXV(3, MINV(GET_STR(ch),
					   races[GET_RACE(ch)].max_str));
		return;
	}


	if (location == APPLY_DEX) {
		GET_DEX(ch) = ch->abilities.dex;	/* reset dex to original */
		one_ha1225_affect_total(ch, APPLY_DEX);
		GET_DEX(ch) = MAXV(3, MINV(GET_DEX(ch),
					   races[GET_RACE(ch)].max_dex));
		return;

	}
	if (location == APPLY_INT) {
		GET_INT(ch) = ch->abilities.intel;	/* reset int to original */
		one_ha1225_affect_total(ch, APPLY_INT);
		GET_INT(ch) = MAXV(3, MINV(GET_INT(ch),
					   races[GET_RACE(ch)].max_int));
		return;
	}

	if (location == APPLY_WIS) {
		GET_WIS(ch) = ch->abilities.wis;	/* reset wis to original */
		one_ha1225_affect_total(ch, APPLY_WIS);
		GET_WIS(ch) = MAXV(3, MINV(GET_WIS(ch),
					   races[GET_RACE(ch)].max_wis));
		return;
	}

	if (location == APPLY_CON) {
		GET_CON(ch) = ch->abilities.con;	/* reset con to original */
		one_ha1225_affect_total(ch, APPLY_CON);
		GET_CON(ch) = MAXV(3, MINV(GET_CON(ch),
					   races[GET_RACE(ch)].max_con));
		return;
	}

	if (location == APPLY_CHA) {
		GET_CHA(ch) = ch->abilities.cha;	/* reset cha to original */
		one_ha1225_affect_total(ch, APPLY_CHA);
		GET_CHA(ch) = MAXV(3, MINV(GET_CHA(ch),
					   races[GET_RACE(ch)].max_cha));
		return;
	}

	/*
	 bzero(buf, sizeof(buf));
	 sprintf(buf, "ERROR: in smart_ha1225_affect_total with unknown handler: %d",
					 location);
	 main_log(buf);
	 */
	return;

}				/* END OF smart_ha1225_affect_total() */


/* Insert an affect_type in a char_data structure
   Automatically sets apropriate bits and apply's */
void ha1300_affect_to_char(struct char_data * ch, struct affected_type * af)
{
	struct affected_type *affected_alloc;



	CREATE(affected_alloc, struct affected_type, 1);
	*affected_alloc = *af;
	affected_alloc->next = ch->affected;
	ch->affected = affected_alloc;

	ha1200_affect_modify(ch, af->location, af->modifier,
			     af->bitvector, TRUE);
	smart_ha1225_affect_total(ch, af->location);
}

void ha1300_affect_to_char2(struct char_data * ch, struct affected_type * af)
{
	struct affected_type *affected_alloc;



	CREATE(affected_alloc, struct affected_type, 1);
	*affected_alloc = *af;
	affected_alloc->next = ch->affected;
	ch->affected = affected_alloc;

	ha1210_affect2_modify(ch, af->location, af->modifier, af->bitvector2, TRUE);
	smart_ha1225_affect_total(ch, af->location);
}


/* Remove an affected_type structure from a char (called when duration
reaches zero). Pointer *af must never be NIL! Frees mem and calls
affect_location_apply                                                */
void ha1325_affect_remove(struct char_data * ch, struct affected_type * af, int lv_flag)
{
	struct affected_type *hjp;
	char buf[MAX_STRING_LENGTH];



	assert(ch->affected);
	ha1200_affect_modify(ch, af->location, af->modifier,
			     af->bitvector, FALSE);

	/* remove structure *af from linked list */
	if (ch->affected == af) {
		/* remove head of list */
		ch->affected = af->next;
	}
	else {
		for (hjp = ch->affected; (hjp->next) && (hjp->next != af); hjp = hjp->next);

		if (hjp->next != af) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: Could not locate affected_type in ch->affected. (handler.c, ha1325_affect_remove)\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
		hjp->next = af->next;	/* skip the af element */
	}

	if (af->type == SPELL_CHARM_PERSON) {
		/* ONLY REMOVE THE FOLLOWERS FOR CERTAIN CONDITIONS */
		if (lv_flag == 1) {
			if (ch->master) {
				ha4100_stop_follower(ch, END_FOLLOW_BY_END_CHARM_PERSON);
			}
		}
	}

	smart_ha1225_affect_total(ch, af->location);
	free(af);
	ha9900_sanity_check(0, "FREE80", "SYSTEM");

}				/* END OF ha1325_affect_remove() */


/* Call ha1325_affect_remove with every spell of spelltype "skill" */
void ha1350_affect_from_char(struct char_data * ch, int lv_skill)
{
	struct affected_type *hjp, *next;



	for (hjp = ch->affected; hjp; hjp = next) {
		next = hjp->next;	/* Have to do this here cuz hjp might
					 * be free'd */
		if (hjp->type == lv_skill)
			ha1325_affect_remove(ch, hjp, 0);
	}
}				/* END OF ha1350_affect_from_char() */



/* Remove all spells from a player */
void ha1370_remove_all_spells(struct char_data * ch)
{
	int idx;



	for (idx = 0; idx < MAX_AFFECT && ch->affected; idx++) {
		if (ch->affected) {
			ha1325_affect_remove(ch, ch->affected, REMOVE_FOLLOWER);
		}
	}
}				/* END OF ha1370_remove_all_spells() */


/* Return if a char is affected by a spell (SPELL_XXX), NULL indicates
   not affected                                                        */
bool ha1375_affected_by_spell(struct char_data * ch, int lv_skill)
{
	struct affected_type *hjp;



	for (hjp = ch->affected; hjp; hjp = hjp->next)
		if (hjp->type == lv_skill)
			return (TRUE);

	return (FALSE);

}				/* END OF ha1375_affected_by_spell() */

void ha1400_affect_join(struct char_data * ch, struct affected_type * af,
			  bool avg_dur, bool avg_mod)
{
	struct affected_type *hjp, *next;
	bool found = FALSE;



	for (hjp = ch->affected; !found && hjp; hjp = next) {
		next = hjp->next;
		if (hjp->type == af->type) {

			af->duration += hjp->duration;
			if (avg_dur)
				af->duration /= 2;

			af->modifier += hjp->modifier;
			if (avg_mod)
				af->modifier /= 2;

			ha1325_affect_remove(ch, hjp, 0);
			ha1300_affect_to_char(ch, af);
			found = TRUE;
		}
	}
	if (!found)
		ha1300_affect_to_char(ch, af);
}
/* move a player out of a room */
void ha1500_char_from_room(struct char_data * ch)
{
	struct char_data *i;
	char buf[MAX_STRING_LENGTH];



	if (ch->in_room == NOWHERE) {
		main_log("ERROR: extracting char from room (handler.c, ha1500_char_from_room)");
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Char %s is NOWWHERE in routine",
			GET_REAL_NAME(ch));
		main_log(buf);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
		printf("Aborting\r\n");
		ABORT_PROGRAM();
	}

	/* IF BEING REMOVED FROM A VEHICLE ROOM, REMOVE RIDING */
	if (world[ch->in_room].number >= VEHICLE_BEG_ROOM &&
	    world[ch->in_room].number <= VEHICLE_END_ROOM) {
		ch->riding = 0;
	}

	if (ch->equipment[WEAR_LIGHT])
		if (ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_LIGHT ||
		    ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_QSTLIGHT)
			if (ch->equipment[WEAR_LIGHT]->obj_flags.value[2])	/* Light is ON */
				world[ch->in_room].light--;

	if (ch == world[ch->in_room].people)	/* head of list */
		world[ch->in_room].people = ch->next_in_room;

	else {
		for (i = world[ch->in_room].people;
		     i && i->next_in_room != ch; i = i->next_in_room);
		if (i) {
			i->next_in_room = ch->next_in_room;
		}
	}

	ch->in_room = NOWHERE;
	ch->next_in_room = 0;
	return;
}


/* place a character in a room */
void ha1600_char_to_room(struct char_data * ch, int room)
{



	ch->next_in_room = world[room].people;
	world[room].people = ch;
	ch->in_room = room;

	if (ch->equipment[WEAR_LIGHT])
		if (ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_LIGHT ||
		    ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_QSTLIGHT)
			if (ch->equipment[WEAR_LIGHT]->obj_flags.value[2])	/* Light is ON */
				world[room].light++;
}


/* WHERE lv_flag =                                           */
/*                  BIT0 ON - do look                        */
int ha1650_all_from_room_to_room(int lv_from_room, int lv_to_room,
			      char *lv_to_msg, char *lv_from_msg, int lv_flag)
{

	char buf[MAX_STRING_LENGTH];
	int lv_mob_num;
	struct char_data *tmp_ch, *next_ch;
	struct obj_data *tmp_obj, *next_obj;



	if (lv_from_room < 0 || lv_to_room < 0) {
		return (FALSE);
	}


	/* MOVE OBJECTS */
	for (tmp_obj = world[lv_from_room].contents; tmp_obj; tmp_obj = next_obj) {
		next_obj = tmp_obj->next_content;
		if (GET_ITEM_TYPE(tmp_obj) != ITEM_ZCMD) {
			ha2200_obj_from_room(tmp_obj);
			ha2100_obj_to_room(tmp_obj, lv_to_room);
		}
	}			/* END OF for loop */

	/* CHARS */
	for (tmp_ch = world[lv_from_room].people; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;

		lv_mob_num = 0;
		if (IS_NPC(tmp_ch)) {
			lv_mob_num = mob_index[tmp_ch->nr].virtual;
		}

		ha1500_char_from_room(tmp_ch);
		ha1600_char_to_room(tmp_ch, lv_to_room);
		if (*lv_to_msg) {
			bzero(buf, sizeof(buf));
			strcpy(buf, lv_to_msg);
			act(buf, TRUE, tmp_ch, 0, 0, TO_CHAR);
		}
		/* DON'T LET EM KNOW IRIS MOBS ARE ENTERING ROOM */
		if (lv_mob_num != MOB_XANTH_QUEEN_IRIS &&
		    lv_mob_num != MOB_XANTH_IRIS_DRAGON &&
		    lv_mob_num != MOB_XANTH_IRIS_GARDNER) {
			if (*lv_from_msg) {
				bzero(buf, sizeof(buf));
				strcpy(buf, lv_from_msg);
				act(buf, TRUE, tmp_ch, 0, 0, TO_ROOM);
			}
		}		/* END OF not IRIS */
		if (IS_SET(lv_flag, BIT0)) {
			in3000_do_look(tmp_ch, "\0", 0);
		}
	}			/* END OF for loop */

	return (TRUE);

}				/* END OF ha1650_all_from_room_to_room() */


/* give an object to a char   */
void ha1700_obj_to_char(struct obj_data * object, struct char_data * ch)
{



	object->next_content = ch->carrying;
	ch->carrying = object;
	object->carried_by = ch;
	object->in_room = NOWHERE;
	IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(object);
	IS_CARRYING_N(ch)++;

	ha2200_sort_obj_list(ch->carrying);
}				/* END OF  ha1700_obj_to_char() */


int ha1750_remove_char_via_death_room(struct char_data * ch, int lv_how_left)
{

	int location, i, old_location;
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];



	in3000_do_look(ch, "\0", 0);

	if (ch->in_room < 0) {
		send_to_char("You can't DT in a negative room.\r\n", ch);
		return (FALSE);
	}

	/* Immortals can't DT */
	if (GET_LEVEL(ch) > IMO_SPIRIT) {
		return (FALSE);
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s entered DeathTrap %s\r\n",
		GET_REAL_NAME(ch),
		world[ch->in_room].name);
	if (IS_PC(ch) || lv_how_left != LEFT_BY_DEATH_TELEPORT) {
		do_info(buf, 1, IMO_IMM, ch);
	}

	sprintf(buf, "%s entered r[%d] DeathTrap %s\r\n",
		GET_REAL_NAME(ch),
		world[ch->in_room].number,
		world[ch->in_room].name);
	sprintf(buf1, "%s entered r[%d] DeathTrap %s. ",
		GET_REAL_NAME(ch),
		world[ch->in_room].number,
		world[ch->in_room].name);
	if (IS_PC(ch) || lv_how_left != LEFT_BY_DEATH_TELEPORT) {
		do_info(buf, IMO_IMM, MAX_LEV, ch);
	}



	ft1700_death_cry(ch);
	GET_HOW_LEFT(ch) = lv_how_left;
	if (gv_mega_verbose_messages == TRUE) {
		main_log("Leaving by ha1750 DEATH ROOM");
	}

	/* IF this is an AVATAR and PK, there is a 100% chance of kit loss */
	if (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV) {
		wi2900_purge_items(ch, "\0", 0);
		sprintf(buf, "%s lost kit.", GET_REAL_NAME(ch));
		strcat(buf1, buf);

	}
	if (IS_PC(ch))
		spec_log(buf1, DT_LOG);

	for (location = 0; location <= top_of_world; location++) {
		if (world[location].number == 3000) {
			break;
		}
		else if (location == top_of_world) {
			send_to_char("Error! room 3000 removed, please SCREAM for help!!\r\n", ch);
			sprintf(buf, "Can't remove char from deathtrap!\r\n");
			do_wizinfo(buf, IMO_IMM, ch);
			return (FALSE);
		}
	}
	GET_HIT(ch) = MINV(1, GET_MAX_HIT(ch));
	GET_MANA(ch) = MINV(1, GET_MAX_MANA(ch));
	GET_MOVE(ch) = MINV(100, GET_MAX_MOVE(ch));
	GET_SCORE(ch) -= 200;
	if (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV) {
		GET_EXP(ch) -= MINV(GET_EXP(ch) / 4, 100000000);
		GET_GOLD(ch) -= MINV(GET_GOLD(ch) / 4, 200000000);
		GET_QPS(ch) -= MINV(GET_QPS(ch) / 4, 1000);
	}
	else
		GET_EXP(ch) -= 2 * MINV(GET_EXP(ch) >> 4, 25000 * GET_LEVEL(ch));
	player_table[ch->nr].pidx_score = GET_SCORE(ch);
	GET_DEATHS(ch)++;
	in5300_update_top_list(ch, top_deaths);
	old_location = ch->in_room;
	ch->in_room = location;
	ft1500_make_corpse(ch, ch);
	ch->in_room = old_location;
	for (i = 0; i < MAX_AFFECT; i++) {
		if (ch->affected)
			ha1325_affect_remove(ch, ch->affected, 0);
	}
	li9800_set_alignment(ch);
	ha3000_extract_char(ch, END_EXTRACT_BY_DEATH_ROOM);
	return (TRUE);

}				/* END OF ha1750_remove_char_via_death_room() */


/* take an object from a char */
void ha1800_obj_from_char(struct obj_data * object)
{
	struct obj_data *tmp;



	if (object->carried_by->carrying == object)	/* head of list */
		object->carried_by->carrying = object->next_content;

	else {
		for (tmp = object->carried_by->carrying;
		     tmp && (tmp->next_content != object);
		     tmp = tmp->next_content);	/* locate previous */

		tmp->next_content = object->next_content;
	}

	IS_CARRYING_W(object->carried_by) -= GET_OBJ_WEIGHT(object);
	IS_CARRYING_N(object->carried_by)--;
	object->carried_by = 0;
	object->next_content = 0;
}



/* Return the effect of a piece of armor in position eq_pos */
int ha1900_apply_ac(struct char_data * ch, int eq_pos)
{



	assert(ch->equipment[eq_pos]);

	if (!(GET_ITEM_TYPE(ch->equipment[eq_pos]) == ITEM_ARMOR) &&
	    !(GET_ITEM_TYPE(ch->equipment[eq_pos]) == ITEM_QUEST))
		return 0;

	switch (eq_pos) {

	case WEAR_BODY:
		return (2 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 20% */
	case WEAR_HEAD:
		return (1.5 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 15% */
	case WEAR_LEGS:
		return (1.5 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 15% */
	case WEAR_4LEGS_1:
		return (1.5 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 15% */
	case WEAR_4LEGS_2:
		return (1.5 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 15% */
	case WEAR_SHIELD:
		return (2 * ch->equipment[eq_pos]->obj_flags.value[0]);	/* 20% */
		/* case WEAR_FEET: return
		 * (ch->equipment[eq_pos]->obj_flags.value[0]);    case
		 * WEAR_HANDS: return
		 * (ch->equipment[eq_pos]->obj_flags.value[0]);    case
		 * WEAR_ARMS: return
	    (ch->equipment[eq_pos]->obj_flags.value[0]);    */ default:	/* used to be shield */
		return (ch->equipment[eq_pos]->obj_flags.value[0]);	/* 10% */
	}
	return 0;
}
/* WHERE lv_flag =                                               */
/*                  BIT0 - remove object before wearing          */
/*                  BIT1 - put object in inventory if wear fails */
void ha1925_equip_char(struct char_data * ch, struct obj_data * obj, int pos, int lv_verbose, int lv_flag)
{
	int j, small_can_use, normal_can_use, large_can_use;
	char buf[MAX_STRING_LENGTH];


	/* DETERMINE IF PLAYER CAN USE OBJECT */
	small_can_use = normal_can_use = large_can_use = 0;
	if (!IS_SET(GET_OBJ1(obj), OBJ1_SMALL_ONLY) &&
	    !IS_SET(GET_OBJ1(obj), OBJ1_NORMAL_ONLY) &&
	    !IS_SET(GET_OBJ1(obj), OBJ1_LARGE_ONLY)) {
		small_can_use = normal_can_use = large_can_use = 1;
	}

	/* UNCHARMED NPC CHARS CAN  USE ANYTHING */
	if (IS_UNCHARMED_NPC(ch)) {
		small_can_use = normal_can_use = large_can_use = 1;
	}

	if (IS_SET(GET_OBJ1(obj), OBJ1_SMALL_ONLY))
		small_can_use = 1;
	if (IS_SET(GET_OBJ1(obj), OBJ1_NORMAL_ONLY))
		normal_can_use = 1;
	if (IS_SET(GET_OBJ1(obj), OBJ1_LARGE_ONLY))
		large_can_use = 1;


	if (!(pos >= 0 && pos < MAX_WEAR)) {
		main_log("ERROR: EQUIP: loc out of range");
		return;
	}

	if (ch->equipment[pos]) {
		main_log("ERROR: EQUIP: Obj is equipped_by when equip.");
		return;
	}

	if (!IS_SET(lv_flag, BIT0) &&
	    obj->carried_by) {
		main_log("ERROR: EQUIP: Obj is carried_by when equip.");
		return;
	}

	if (obj->in_room != NOWHERE) {
		main_log("ERROR: EQUIP: Obj is in_room when equip.");
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMP) {
		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_SIZE_SMALL) &&
		    !small_can_use) {
			act("$p is to large for you.",
			    FALSE, ch, obj, 0, TO_CHAR);
			act("$n is unable to wear $p.",
			    FALSE, ch, obj, 0, TO_ROOM);
			if (IS_SET(lv_flag, BIT1)) {
				ha1700_obj_to_char(obj, ch);
			}
			return;
		}
		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_SIZE_NORMAL) &&
		    !normal_can_use) {
			act("$p doesn't seem to fit you.",
			    FALSE, ch, obj, 0, TO_CHAR);
			act("$n is unable to wear $p.",
			    FALSE, ch, obj, 0, TO_ROOM);
			if (IS_SET(lv_flag, BIT1)) {
				ha1700_obj_to_char(obj, ch);
			}
			return;
		}
		if (IS_SET(races[GET_RACE(ch)].flag, RFLAG_SIZE_LARGE) &&
		    !large_can_use) {
			act("$p is to small for you.",
			    FALSE, ch, obj, 0, TO_CHAR);
			act("$n is unable to wear $p.",
			    FALSE, ch, obj, 0, TO_ROOM);
			if (IS_SET(lv_flag, BIT1)) {
				ha1700_obj_to_char(obj, ch);
			}
			return;
		}
	}			/* END OF LESS THAN IMO_IMP */

	if (IS_PC(ch) &&
	    gv_port != HELLNIGHT_PORT) {
		if ((IS_OBJ_STAT(obj, OBJ1_ANTI_EVIL) && IS_EVIL(ch)) ||
		    (IS_OBJ_STAT(obj, OBJ1_ANTI_GOOD) && IS_GOOD(ch)) ||
		    (IS_OBJ_STAT(obj, OBJ1_ANTI_NEUTRAL) && IS_NEUTRAL(ch))) {
			if (ch->in_room != NOWHERE) {
				act("You are zapped by $p.",
				    FALSE, ch, obj, 0, TO_CHAR);
				act("$n is zapped by $p.",
				    FALSE, ch, obj, 0, TO_ROOM);
				if (IS_SET(lv_flag, BIT1)) {
					ha1700_obj_to_char(obj, ch);
				}
				return;
			}
			else {
				main_log("ERROR: ch->in_room = NOWHERE when equipping char.");
			}
		}
		if ((GET_ITEM_TYPE(obj) == ITEM_ARMOR) &&
		    GET_RACE(ch) == obj->obj_flags.value[1]) {
			send_to_char("You are not the correct race to wear this.\n\r", ch);
			return;
		} //Bingo - For individual race antis on objects
			if ((GET_ITEM_TYPE(obj) == ITEM_ARMOR) &&
			    obj->obj_flags.value[1] > 50 &&
			    obj->obj_flags.value[1] != GET_RACE(ch) + 50) {
			sprintf(buf, "Only %s can wear this!", race_list[obj->obj_flags.value[1] - 50]);
			send_to_char(buf, ch);
			return;
		} //Bingo - For individual races to only be able to wear the item
			if ((IS_OBJ_STAT(obj, OBJ1_MINLVL10) && GET_LEVEL(ch) < 10) ||
		   (IS_OBJ_STAT2(obj, OBJ2_MINLVL15) && GET_LEVEL(ch) < 15) ||
		    (IS_OBJ_STAT(obj, OBJ1_MINLVL20) && GET_LEVEL(ch) < 20) ||
		   (IS_OBJ_STAT2(obj, OBJ2_MINLVL25) && GET_LEVEL(ch) < 25) ||
		    (IS_OBJ_STAT(obj, OBJ1_MINLVL30) && GET_LEVEL(ch) < 30) ||
		   (IS_OBJ_STAT2(obj, OBJ2_MINLVL35) && GET_LEVEL(ch) < 35) ||
		    (IS_OBJ_STAT(obj, OBJ1_MINLVL41) && GET_LEVEL(ch) < 41) ||
		    (IS_OBJ_STAT2(obj, OBJ2_MINLVL42) && GET_LEVEL(ch) < 42) ||
			    (IS_OBJ_STAT2(obj, OBJ2_IMMONLY) && GET_LEVEL(ch) < IMO_SPIRIT)) {
			if (ch->in_room != NOWHERE) {
				act("You are too inexperienced to use $p.", FALSE, ch, obj, 0, TO_CHAR);
				act("$n realizes that $e is too inexperienced to use $p.", FALSE, ch,
				    obj, 0, TO_ROOM);
				if (IS_SET(lv_flag, BIT1))
					ha1700_obj_to_char(obj, ch);
				return;
			}
			else {
				main_log("ch->in_room = NOWHERE when equipping char.");
			}
		}

		if ((GET_LEVEL(ch) <= PK_LEV) &&
		    (((IS_OBJ_STAT(obj, OBJ1_ANTI_WARRIOR) && (GET_CLASS(ch) == CLASS_WARRIOR)) ||
		      (IS_OBJ_STAT(obj, OBJ1_ANTI_MAGE) && (GET_CLASS(ch) == CLASS_MAGIC_USER)) ||
		      (IS_OBJ_STAT(obj, OBJ1_ANTI_CLERIC) && (GET_CLASS(ch) == CLASS_CLERIC)) ||
		      (IS_OBJ_STAT(obj, OBJ1_ANTI_THIEF) && (GET_CLASS(ch) == CLASS_THIEF)) ||
		      (IS_SET(obj->obj_flags.flags2, OBJ2_ANTI_PALADIN) && (GET_CLASS(ch) == CLASS_PALADIN)) ||
		      (IS_SET(obj->obj_flags.flags2, OBJ2_ANTI_BARD) && (GET_CLASS(ch) == CLASS_BARD)) ||
		      (IS_SET(obj->obj_flags.flags2, OBJ2_ANTI_PRIEST) && (GET_CLASS(ch) == CLASS_PRIEST)) ||
		      (IS_SET(obj->obj_flags.flags2, OBJ2_ANTI_RANGER) && (GET_CLASS(ch) == CLASS_RANGER)) ||
		      (IS_SET(obj->obj_flags.flags2, OBJ2_ANTI_DRUID) && (GET_CLASS(ch) == CLASS_DRUID))) ||
		     ((GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_RANGER) && (GET_CLASS(ch) == CLASS_RANGER)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_DRUID) && (GET_CLASS(ch) == CLASS_DRUID)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_MAGE) && (GET_CLASS(ch) == CLASS_MAGIC_USER)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_CLERIC) && (GET_CLASS(ch) == CLASS_CLERIC)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_THIEF) && (GET_CLASS(ch) == CLASS_THIEF)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_WARRIOR) && (GET_CLASS(ch) == CLASS_WARRIOR)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_BARD) && (GET_CLASS(ch) == CLASS_BARD)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_PRIEST) && (GET_CLASS(ch) == CLASS_PRIEST)) ||
		      (GET_ITEM_TYPE(obj) == ITEM_ARMOR && IS_SET(OVAL2(obj), OVAL_ANTI_PALADIN) && (GET_CLASS(ch) == CLASS_PALADIN))))) {

			if (ch->in_room != NOWHERE) {
				act("You are the wrong class to use that.", FALSE, ch, obj, 0, TO_CHAR);
				act("$n realizes that $p isnt the sort of thing $e can use.", FALSE,
				    ch, obj, 0, TO_ROOM);
				if (IS_SET(lv_flag, BIT1))
					ha1700_obj_to_char(obj, ch);
				return;
			}
			else {
				main_log("ERROR: ch->in_room = NOWHERE when equipping char.");
			}
		}
	}

	/* DO WE NEED TO REMOVE IT FROM INVENTORY? */
	if (IS_SET(lv_flag, BIT0)) {
		ha1800_obj_from_char(obj);
	}

	ch->equipment[pos] = obj;
	obj->carried_by = ch;

	/* PRINT A MESSAGE ABOUT WEARING THE ITEM */
	if (lv_verbose) {
		perform_wear_msg(ch, obj, pos, lv_verbose);
	}

	if (GET_ITEM_TYPE(obj) == ITEM_ARMOR)
		GET_AC(ch) -= ha1900_apply_ac(ch, pos);

	if (GET_ITEM_TYPE(obj) == ITEM_QUEST)
		GET_AC(ch) -= ha1900_apply_ac(ch, pos);

	for (j = 0; j < MAX_OBJ_AFFECT; j++) {
		ha1200_affect_modify(ch, obj->affected[j].location,
				     obj->affected[j].modifier,
				     obj->obj_flags.bitvector, TRUE);
		if (obj->affected[j].location != APPLY_NONE)
			smart_ha1225_affect_total(ch, obj->affected[j].location);
	}
}				/* END OF ha1925_equip_char() */


struct obj_data *ha1930_unequip_char(struct char_data * ch, int pos)
{

	int j;
	struct obj_data *obj;



	assert(pos >= 0 && pos < MAX_WEAR);
	assert(ch->equipment[pos]);

	obj = ch->equipment[pos];
	obj->carried_by = NULL;
	if (GET_ITEM_TYPE(obj) == ITEM_ARMOR)
		GET_AC(ch) += ha1900_apply_ac(ch, pos);
	if (GET_ITEM_TYPE(obj) == ITEM_QUEST)
		GET_AC(ch) += ha1900_apply_ac(ch, pos);

	ch->equipment[pos] = 0;

	for (j = 0; j < MAX_OBJ_AFFECT; j++) {
		ha1200_affect_modify(ch, obj->affected[j].location,
				     obj->affected[j].modifier,
				     obj->obj_flags.bitvector, FALSE);
		smart_ha1225_affect_total(ch, obj->affected[j].location);
	}

	return (obj);
}


int ha1975_get_number(char **name)
{

	int i;
	char *ppos;
	char number[MAX_INPUT_LENGTH] = "";



	if ((ppos = (char *) index(*name, '.'))) {
		*ppos++ = '\0';
		strcpy(number, *name);
		strcpy(*name, ppos);

		for (i = 0; *(number + i); i++)
			if (!isdigit(*(number + i)))
				return (0);

		return (atoi(number));
	}

	return (1);
}


/* Search a given list for an object, and return a pointer to that object */
struct obj_data *ha2000_get_obj_list(char *name, struct obj_data * list)
{
	struct obj_data *i;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	for (i = list, j = 1; i && (j <= number); i = i->next_content)
		if (ha1150_isname(tmp, i->name)) {
			if (j == number)
				return (i);
			j++;
		}

	return (0);
}


/* Search a given list for an object number, and return a ptr to that obj */
/* changed from stock code to search for virtual numbers not real */
struct obj_data *ha2000_get_obj_list_virtual(int num, struct obj_data * list)
{
	struct obj_data *i;



	for (i = list; i; i = i->next_content)
		if (i->item_number > 0 && obj_index[i->item_number].virtual == num)
			return (i);

	return (0);
}


struct obj_data *ha2025_get_obj_list_num(int num, struct obj_data * list)
{
	struct obj_data *obj;



	for (obj = list; obj; obj = obj->next_content)
		if (obj->item_number == num)
			return (obj);

	return (0);

}				/* END OF *ha2025_get_obj_list_num() */


struct char_data *ha2030_get_mob_list_num(int num, struct char_data * list)
{
	struct char_data *mob;



	for (mob = list; mob; mob = mob->next_in_room)
		if (IS_NPC(mob) && mob->nr == num)
			return (mob);

	return (0);

}				/* END OF *ha2030_get_mob_list_num() */


int ha2050_count_objs_in_list(int num, struct obj_data * list)
{
	int lv_count;
	struct obj_data *lv_obj;



	lv_count = 0;
	for (lv_obj = list; lv_obj; lv_obj = lv_obj->next_content) {
		if (lv_obj->item_number == num) {
			lv_count++;
		} /* END OF match */
	} /* END OF for loop */

	return (lv_count);

} /* END OF *ha2050_count_objs_in_list() */


/*search the entire world for an object, and return a pointer  */
struct obj_data *get_obj(char *name)
{
	struct obj_data *i;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	for (i = object_list, j = 1; i && (j <= number); i = i->next) {
		if (ha1150_isname(tmp, i->name)) {
			if (j == number)
				return (i);
			j++;
		}
	}

	return (0);
}


/*search the entire world for an object number, and return a pointer  */
struct obj_data *ha2090_get_obj_using_num(int nr)
{
	struct obj_data *i;



	for (i = object_list; i; i = i->next)
		if (i->item_number == nr)
			return (i);

	return (0);
}				/* END OF ha2090_get_obj_num() */



/* search a room for a char, and return a pointer if found..  */
struct char_data *ha2100_get_char_in_room(char *name, int room)
{
	struct char_data *i;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	for (i = world[room].people, j = 1; i && (j <= number); i = i->next_in_room)
		if (ha1150_isname(tmp, GET_NAME(i))) {
			if (j == number)
				return (i);
			j++;
		}

	return (0);
}






/* search the world for a char, and return a pointer if found */
struct char_data *get_char(char *name)
{

	struct char_data *vict;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH], *tmp;



	bzero(tmpname, sizeof(tmpname));
	strncpy(tmpname, name, MAX_INPUT_LENGTH - 5);
	tmp = tmpname;

	/* DID USER SPECIFY SOMETHING LIKE 2.<name> */
	number = ha1975_get_number(&tmp);
	if (!number)
		return (0);

	j = 1;
	for (vict = character_list; vict && (j <= number); vict = vict->next) {
		if (ha1150_isname(tmp, GET_NAME(vict))) {
			if (j == number)
				return (vict);
			j++;
		}
	}			/* END OF for loop */

	/* IF WE DIDN'T FIND THEM */
	return (0);

}				/* END OF get_char() */


struct obj_data *ha2075_get_obj_list_vis(struct char_data * ch, char *name,
					   struct obj_data * list)
{

	struct obj_data *i;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	for (i = list, j = 1; i && (j <= number); i = i->next_content)
		if (ha1150_isname(tmp, i->name))
			if (CAN_SEE_OBJ(ch, i)) {
				if (j == number)
					return (i);
				j++;
			}
	return (0);
}


/* search the world for a char num, return a pointer if found */
struct char_data *ha2175_get_char_num(int nr)
{

	struct char_data *vict;



	for (vict = character_list; vict; vict = vict->next)
		if (vict->nr == nr)
			return (vict);

	return (0);
}				/* END OF ha2175_get_char_num() */


/* finds objects the same as the one at the front of the list and moves them
 to the front too */
void ha2200_sort_obj_list(struct obj_data * object)
{
	struct obj_data *list_obj, *middle, *old_next;



	if (!object || GET_ITEM_TYPE(object) == ITEM_ZCMD || !object->next_content) {
		return;
	}
	else {			/* search for chain of like objects to group
				 * with start */
		list_obj = object->next_content;	/* start at next entry
							 * in list */
		while ((list_obj->next_content)
		       && strcmp(list_obj->next_content->description, object->description))
			list_obj = list_obj->next_content;
		if (!list_obj->next_content)
			return;	/* return if nothing found */

		/* found something I guess move it after top of list */
		middle = list_obj;	/* this one is going to be in the
					 * middle */
		old_next = object->next_content;
		object->next_content = list_obj->next_content;

		while ((list_obj->next_content)
		       && !strcmp(list_obj->next_content->description, object->description))
			list_obj = list_obj->next_content;
		middle->next_content = list_obj->next_content;
		list_obj->next_content = old_next;
	}
}
/* put an object in a room, zone commmands should be first  */
/* l8r will sort like items together somehow */
void ha2100_obj_to_room(struct obj_data * object, int room)
{
	struct obj_data *room_obj;



	room_obj = world[room].contents;
	if ((!room_obj) || (room_obj->obj_flags.type_flag != ITEM_ZCMD)) {
		object->next_content = world[room].contents;
		world[room].contents = object;
	}
	else {
		while ((room_obj->next_content)
		&& (room_obj->next_content->obj_flags.type_flag == ITEM_ZCMD))
			room_obj = room_obj->next_content;
		object->next_content = room_obj->next_content;
		room_obj->next_content = object;
	}

	/* internal obj data update stuff */
	object->in_room = room;
	object->carried_by = 0;

	/* group like objects together */
	ha2200_sort_obj_list(world[room].contents);
}
/* Take an object from a room */
void ha2200_obj_from_room(struct obj_data * object)
{
	struct obj_data *i;



	/* remove object from room */

	if (object == world[object->in_room].contents)	/* head of list */
		world[object->in_room].contents = object->next_content;

	else {			/* locate previous element in list */
		for (i = world[object->in_room].contents; i &&
		     (i->next_content != object); i = i->next_content);

		if (i)
			i->next_content = object->next_content;
	}

	object->in_room = NOWHERE;
	object->next_content = 0;
}


/* put an object in an object (quaint)  */
void ha2300_obj_to_obj(struct obj_data * obj, struct obj_data * obj_to)
{
	struct obj_data *tmp_obj;



	if (!obj || !obj_to) {
		main_log("ha2300_obj_to_obj: NULL object pointer");
		return;
	}
	obj->next_content = obj_to->contains;
	obj_to->contains = obj;
	obj->in_obj = obj_to;

	for (tmp_obj = obj->in_obj; tmp_obj;
	     GET_OBJ_WEIGHT(tmp_obj) += GET_OBJ_WEIGHT(obj), tmp_obj = tmp_obj->in_obj);

	ha2200_sort_obj_list(obj_to->contains);
}


/* remove an object from an object */
void ha2400_obj_from_obj(struct obj_data * obj)
{

	struct obj_data *tmp, *obj_from;
	char buf[MAX_STRING_LENGTH];



	if (obj->in_obj) {
		obj_from = obj->in_obj;
		if (obj == obj_from->contains)	/* head of list */
			obj_from->contains = obj->next_content;
		else {
			for (tmp = obj_from->contains;
			     tmp && (tmp->next_content != obj);
			     tmp = tmp->next_content);	/* locate previous */

			if (!tmp) {
				perror("ERROR: Fatal error in object structures.");
				sprintf(buf, "ERROR: in object structors\r\n");
				ABORT_PROGRAM();
			}

			tmp->next_content = obj->next_content;
		}


/* Subtract weight from containers container */
		for (tmp = obj->in_obj; tmp->in_obj; tmp = tmp->in_obj)
			GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);

		GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);

/* Subtract weight from char that carries the object */
		if (tmp->carried_by)
			IS_CARRYING_W(tmp->carried_by) -= GET_OBJ_WEIGHT(obj);

		obj->in_obj = 0;
		obj->next_content = 0;
	}
	else {
		perror("ERROR: Trying to object from object when in no object.");
		sprintf(buf, "ERROR: object from object\r\n");
		ABORT_PROGRAM();
	}
}


/* Set all carried_by to point to new owner */
void ha2500_object_list_new_owner(struct obj_data * list, struct char_data * ch)
{


	if (list) {
		ha2500_object_list_new_owner(list->contains, ch);
		ha2500_object_list_new_owner(list->next_content, ch);
		list->carried_by = ch;
	}
}

int ha2600_move_all_chars_objs_to_another_room(struct char_data * ch, char *arg, int lv_real_to_room)
{



	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (lv_real_to_room < 0) {
		return (FALSE);
	}

	return (TRUE);

}				/* END OF
				 * ha2600_move_all_chars_objs_to_another_room() */


/* Extract an object from the world */
void ha2700_extract_obj(struct obj_data * obj)
{
	struct obj_data *temp1, *temp2;
	char buf1[MAX_STRING_LENGTH];
	int lv_veh_room;



	if (GET_ITEM_TYPE(obj) == ITEM_VEHICLE) {
		lv_veh_room = vh500_get_vehicle_room(0, 0, 0, obj);
		if (lv_veh_room && world[lv_veh_room].people) {
			vh1450_remove_all_chars_from_vehicle(obj, lv_veh_room);
		}
		db5175_remove_obj_from_vehicle_list(obj);
	}

	/* Remove it from the linked lists */

	if (obj->in_room != NOWHERE)
		ha2200_obj_from_room(obj);
	else if (obj->carried_by)
		ha1800_obj_from_char(obj);
	else if (obj->in_obj) {
		temp1 = obj->in_obj;
		if (temp1->contains == obj)	/* head of list */
			temp1->contains = obj->next_content;
		else {
			for (temp2 = temp1->contains; temp2 && (temp2->next_content != obj); temp2 = temp2->next_content);
			if (temp2) {
				temp2->next_content = obj->next_content;
			}
		}
	}

	for (; obj->contains; ha2700_extract_obj(obj->contains));
	/* leaves nothing ! */

	if (object_list == obj)	/* head of list */
		object_list = obj->next;
	else {
		/* Remove it from the obj list. */
		for (temp1 = object_list; temp1 && (temp1->next != obj); temp1 = temp1->next);

		if (temp1)
			temp1->next = obj->next;
	}

	if (obj->item_number >= 0) {
		/* if it has a vnum / rnum(not corpse / head) */
		if (gv_total_objects > -1)
			gv_total_objects--;
		if (obj_index[obj->item_number].number > 0) {
			(obj_index[obj->item_number].number)--;
		}
		else {
			bzero(buf1, sizeof(buf1));
			sprintf(buf1, "ERROR: Attempt to set object qty negative. obj: %d",
				obj_index[obj->item_number].virtual);
			main_log(buf1);
		}
	}

	db7200_free_obj(obj);

}				/* END OF ha2700_extract_obj() */

void ha2750_decay_object(struct obj_data * obj, int verbose)
{
	struct char_data *carrier;
	struct obj_data *contained, *next_contained;
	int eq_pos;
	char buf[MAX_STRING_LENGTH];
	
	if (!obj) {
		snprintf(buf, sizeof(buf), "ERROR: ha2750_decay_object() called with null obj.  %s:%d", __FILE__, __LINE__);
		return;
	}

	/* SEND MESSAGES */
	if (verbose) {
		if (obj->carried_by) {
			carrier = obj->carried_by;
			/* Because it get's set to null by ha1930 below */
			act("$p decays into a fine dust and blows away.", FALSE, carrier, obj, 0, TO_CHAR);
			for (eq_pos = 0; eq_pos < MAX_WEAR; eq_pos++) {
				if (obj == carrier->equipment[eq_pos]) {
					ha1700_obj_to_char(ha1930_unequip_char(carrier, eq_pos), carrier);
				}
			}
		}
		else if ((obj->in_room != NOWHERE) && (world[obj->in_room].people)) {
			act("$p decays into a fine dust and blows away.", TRUE, world[obj->in_room].people, obj, 0, TO_ROOM);
			act("$p decays into a fine dust and blows away.", TRUE, world[obj->in_room].people, obj, 0, TO_CHAR);
		}
	}
	/* If it's a container, preserve the contents. */
	for (contained = obj->contains; contained; contained = next_contained) {
		next_contained = contained->next_content;	/* Next in contents */
		ha2400_obj_from_obj(contained);
		if (obj->in_obj)
			ha2300_obj_to_obj(contained, obj->in_obj);
		else if (obj->carried_by)
			ha1700_obj_to_char(contained, obj->carried_by);
		else if (obj->in_room != NOWHERE)
			ha2100_obj_to_room(contained, obj->in_room);
		else
			ha2700_extract_obj(contained);
	}

	if (obj->carried_by)
		sprintf(buf, "Object %s decayed, carried by %s.\r\n", GET_OSDESC(obj), GET_NAME(obj->carried_by));
	if (obj->in_obj) {
		if (obj->in_obj->carried_by)
			sprintf(buf, "Object %s decayed, in %s carried by %s.\r\n",
				GET_OSDESC(obj), GET_OSDESC(obj->in_obj), GET_NAME(obj->in_obj->carried_by));
		if (obj->in_obj->in_room != NOWHERE)
			sprintf(buf, "Object %s decayed, in %s in room %d.\r\n",
				GET_OSDESC(obj), GET_OSDESC(obj->in_obj), world[obj->in_obj->in_room].number);
	}
	if (obj->in_room != NOWHERE)
		sprintf(buf, "Object %s decayed in room %d.\r\n", GET_OSDESC(obj), world[obj->in_room].number);

	/* Log it, Vern! */
	main_log(buf);
	spec_log(buf, EQUIPMENT_LOG);

	ha2700_extract_obj(obj);/* Extract it. */
}

void ha2800_update_object_timer(struct obj_data * obj, int use)
{
	int lv_decay = 0, lv_verbose = 1;

	/*
	 Ok, here's the scoop.  Use is one of a few flags I defined in handler.c, they
	 look like this
	 TIMER_USE_DROP 0
	 TIMER_USE_INV  1
	 TIMER_USE_EQ   2
	 For the inv and the eq, the int is the same as the amount I want to decrement the timer,
	 so I took advantage of that.
	
	 I made it a switch statement so it should be easier to add more uses, should we want to.
	 */
	switch (use) {
	case TIMER_USE_DROP:
		if (GET_TIMER(obj) == 0) {
			if (world[obj->in_room].people) {
				act("$p breaks into a thousand pieces as it hits the ground.",
				    FALSE, world[obj->in_room].people, obj, 0, TO_ROOM);
				act("$p breaks into a thousand pieces as it hits the ground.",
				    FALSE, world[obj->in_room].people, obj, 0, TO_CHAR);
			}
			lv_verbose = 0;
			lv_decay = 1;
		}
		else if (GET_TIMER(obj) > 1)
			TIMER(obj)--;
		else if (GET_TIMER(obj) == 1) {
			if (world[obj->in_room].people) {
				act("$p nearly breaks as it hits the ground.",
				    FALSE, world[obj->in_room].people, obj, 0, TO_CHAR);
				act("$p nearly breaks as it hits the ground.",
				    FALSE, world[obj->in_room].people, obj, 0, TO_ROOM);
			}
			TIMER(obj)--;
		}
		break;
	case TIMER_USE_INV:
	case TIMER_USE_EQ:
		if (GET_TIMER(obj) == 0)
			lv_decay = 1;
		else if (GET_TIMER(obj) > use)
			TIMER(obj) = TIMER(obj) - use;
		else if ((GET_TIMER(obj) == use) || GET_TIMER(obj) == 1) {
			if (obj->carried_by && world[obj->carried_by->in_room].people) {
				/* In case it's in a container. */
				act("$p is feeling more and more fragile all the time!",
				    FALSE, obj->carried_by, obj, 0, TO_CHAR);
				act("$n's $p looks very fragile!",
				    TRUE, world[obj->carried_by->in_room].people, obj, obj->carried_by, TO_ROOM);
			}
			TIMER(obj) = 1;	/* This is actually setting the Timer
					 * to "0." */
		}
		break;
	default:
		break;
	}

	/* If it's a container, go ahead and update the timer of the contents. */
	if (obj->contains)
		ha2800_update_object_timer(obj->contains, use);
	if (obj->next_content && use != TIMER_USE_DROP)
		ha2800_update_object_timer(obj->next_content, use);

	/* Actuall call the decay function -- really get rid of the item */
	if (lv_decay == 1)
		ha2750_decay_object(obj, lv_verbose);

}				/* END OF ha2800_update_object() */


void ha2900_update_char_objects(struct char_data * ch)
{

	int i;



	if (ch->equipment[WEAR_LIGHT])
		if (ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_LIGHT ||
		    ch->equipment[WEAR_LIGHT]->obj_flags.type_flag == ITEM_QSTLIGHT)
			if (ch->equipment[WEAR_LIGHT]->obj_flags.value[2] > 0)
				(ch->equipment[WEAR_LIGHT]->obj_flags.value[2])--;

	for (i = 0; i < MAX_WEAR; i++)
		if (ch->equipment[i])
			ha2800_update_object_timer(ch->equipment[i], TIMER_USE_EQ);

	if (ch->carrying)
		ha2800_update_object_timer(ch->carrying, TIMER_USE_INV);

	return;

}				/* END OF ha2900_update_char_objects() */


/* Extract a ch completely from the world, and leave his stuff behind */
void ha3000_extract_char(struct char_data * ch, int lv_extract_flag)
{

	char colorbuf[MAX_STRING_LENGTH];
	struct descriptor_data *t_desc, *next_desc;
	extern struct char_data *combat_list;

	struct char_data *tmp_ch, *k, *next_char, *lv_temp;

	struct obj_data *i;
	int l, was_in;
	char buf[MAX_STRING_LENGTH];



	/* IF THE CHAR DOING COMMAND DIED, GET OUT */
	if (ch == gv_ch_doing_command) {
		SET_BIT(gv_run_flag, RUN_FLAG_CHAR_DIED);
	}

	/* IF SWITCHED TO A MOB, RETURN */
	if (IS_PC(ch) && !ch->desc) {
		for (t_desc = descriptor_list; t_desc; t_desc = next_desc) {
			next_desc = t_desc->next;
			if (t_desc->original == ch) {
				wi2400_do_return(t_desc->character, "", 0);
			}
		}		/* END OF FOR LOOP */
	}

	if (ch->in_room == NOWHERE) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: extracting char. (handler.c, ha3000_extract_char)\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}
	ch->riding = 0;

	/* ADJUST FOLLOWERS */
	if (ch->followers || ch->master)
		ha4200_die_follower(ch, lv_extract_flag);

	if (ch->desc) {
		/* Forget snooping */
		if (ch->desc->snoop.snooping) {
			ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
			ch->desc->snoop.snooping = 0;
		}

		if (ch->desc->snoop.snoop_by) {
			send_to_char("Your victim is no longer among us.\n\r",
				     ch->desc->snoop.snoop_by);
			ch->desc->snoop.snoop_by->desc->snoop.snooping = 0;
			ch->desc->snoop.snoop_by = 0;
		}

	}			/* END OF ch->desc */

	/* GET RID OF FOLKS HUNTING US */
	for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
		if (HUNTING(tmp_ch) == ch) {
			HUNTING(tmp_ch) = 0;
			send_to_char("Your quarry escaped.\r\n", tmp_ch);
		}
	}

	if (ch->carrying) {

		/* transfer ch's objects to room */
		if (world[ch->in_room].contents) {
			/* room nonempty */
			/* locate tail of room-contents */
			for (i = world[ch->in_room].contents; i->next_content;
			     i = i->next_content);
			/* append ch's stuff to room-contents */
			i->next_content = ch->carrying;
		}		/* end of room non empty */
		else {
			world[ch->in_room].contents = ch->carrying;
		}

		/* connect the stuff to the room */
		for (i = ch->carrying; i; i = i->next_content) {
			i->carried_by = 0;
			i->in_room = ch->in_room;
		}		/* END OF for loop */
	}			/* END OF ch->carrying */

	if (ch->specials.fighting)
		ft1400_stop_fighting(ch, 0);

	for (k = combat_list; k; k = next_char) {
		next_char = k->next_fighting;
		if (k->specials.fighting == ch) {
			ft1400_stop_fighting(k, 0);
		}
	}

	/* Must remove from room before removing the equipment! */
	was_in = ch->in_room;
	ha1500_char_from_room(ch);

	/* clear equipment_list */
	for (l = 0; l < MAX_WEAR; l++)
		if (ch->equipment[l])
			ha2100_obj_to_room(ha1930_unequip_char(ch, l), was_in);

	GET_AC(ch) = MAX_ARMOUR;

	/* END SWITCHED */
	if (ch->desc) {
		if (ch->desc->original)
			wi2400_do_return(ch, "", 0);
	}

	if (IS_PC(ch)) {
		db6400_save_char(ch, NOWHERE);
		t_desc = ch->desc;
	}
	else
		t_desc = 0;


	//Place PC in Death room
		if (IS_PC(ch) && gv_deathroom > 0) {
		//Only certain deaths start you back in the death room
			if (lv_extract_flag == END_EXTRACT_BY_RAW_KILL ||
			    lv_extract_flag == END_EXTRACT_BY_AGE ||
			    lv_extract_flag == END_EXTRACT_BY_DEATH_ROOM) {
			//ch->desc->autologin = 1;
		}
	}

	/* pull the char from the list */
	REMOVE_FROM_LIST(ch, character_list, 1);

	if (ch->desc) {
		if (GET_LEVEL(ch) <= gv_highest_login_level ||
		    GET_HOW_LEFT(ch) == LEFT_BY_PURGE) {
			co2500_close_single_socket(ch->desc);
			ch->desc = 0;
		}
		else {
			ch->desc->connected = CON_GET_MENU_SELECTION;
			sprintf(colorbuf, CHOOSE_MENU(ch));
			send_to_char(colorbuf, ch);
		}
	}

	if (IS_NPC(ch)) {
		if (gv_total_mobiles > -1)
			gv_total_mobiles--;
		if (ch->nr > -1)/* if mobile */
			mob_index[ch->nr].number--;
		db7100_free_char(ch, 2);
	}

	ch = 0;

	if (t_desc != 0) {
		if (t_desc->autologin == 1) {
			pa8000_enter_game(t_desc);
			t_desc->autologin = 0;
		}
	}

	return;

}				/* END OF ha3000_extract_char() */



/* ***********************************************************************
   Here follows high-level versions of some earlier routines, ie functions
   which incorporate the actual player-data.
   *********************************************************************** */


struct char_data *ha2125_get_char_in_room_vis(struct char_data * ch, char *name)
{
	struct char_data *i;
	int j, number;
	int name_length;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);


	/* CHECK FOR NAMES THAT ARE THE EXACT LENGTH */
	for (i = world[ch->in_room].people, j = 1;
	     i && (j <= number); i = i->next_in_room) {
		name_length = strlen(GET_NAME(i));
		if (strlen(tmp) == name_length) {
			if (ha1150_isname(tmp, GET_NAME(i))) {
				if (CAN_SEE(ch, i)) {
					if (j == number)
						return (i);
					j++;
				}
			}
		}
	}			/* END OF FOR LOOP */


	/* CHECK FOR ANY NAMES THAT COME CLOSE TO WHAT WE TYPED */
	for (i = world[ch->in_room].people, j = 1;
	     i && (j <= number); i = i->next_in_room) {
		if (ha1150_isname(tmp, GET_NAME(i)))
			if (CAN_SEE(ch, i)) {
				if (j == number)
					return (i);
				j++;
			}
	}			/* END OF FOR LOOP */

	return (0);
}


struct char_data *ha2150_get_char_in_room_vis_exact(struct char_data * ch, char *name)
{
	struct char_data *i;
	int j, number;
	int name_length;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	/* CHECK FOR NAMES THAT ARE THE EXACT LENGTH */
	for (i = world[ch->in_room].people, j = 1;
	     i && (j <= number); i = i->next_in_room) {
		name_length = strlen(GET_NAME(i));
		if (strlen(tmp) == name_length) {
			if (ha1150_isname(tmp, GET_NAME(i))) {
				if (CAN_SEE(ch, i)) {
					if (j == number)
						return (i);
					j++;
				}
			}
		}
	}			/* END OF FOR LOOP */

	return (0);
}

struct char_data *ha3100_get_char_vis(struct char_data * ch, char *name)
{
	return ha3100_get_char_vis_ex(ch, name, FALSE);
}

struct char_data *ha3100_get_char_vis_ex(struct char_data * ch, char *name, int skipPets)
{
	struct char_data *i;
	int j, number;
	int name_length;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	/* ***** */
	/* EXACT */
	/* ***** */

	/* check location */
	if ((i = ha2150_get_char_in_room_vis_exact(ch, name)))
		return (i);

	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	j = 1;
	for (i = character_list; i && (j <= number); i = i->next) {
		name_length = strlen(GET_NAME(i));

		/* if (skipPets) if (name_length == 3) if (ha1150_isname("pet",
		 * GET_NAME(i))) continue; */
		if (strlen(tmp) == name_length) {
			if (ha1150_isname(tmp, GET_NAME(i))) {
				if (CAN_SEE(ch, i)) {
					if (j == number)
						return (i);
					j++;
				}
			}
		}
	}			/* END OF FOR LOOP */

	/* ************* */
	/* CLOSEST MATCH */
	/* ************* */
	/* check location */
	if ((i = ha2125_get_char_in_room_vis(ch, name)))
		return (i);

	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	j = 1;
	for (i = character_list; i && (j <= number); i = i->next) {
		if (ha1150_isname(tmp, GET_NAME(i))) {
			if (CAN_SEE(ch, i)) {
				if (j == number) {
					return (i);
				}
				j++;
			}
		}
	}
	return (0);
}				/* END OF ha3000_extract() */


struct char_data *ha3100_get_char_vis_exact(struct char_data * ch, char *name)
{

	struct char_data *i;
	int j, number;
	int name_length;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	/* check location */
	if ((i = ha2125_get_char_in_room_vis(ch, name)))
		return (i);

	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	j = 1;
	for (i = character_list; i && (j <= number); i = i->next) {
		name_length = strlen(GET_NAME(i));
		if (strlen(tmp) == name_length)
			if (ha1150_isname(tmp, GET_NAME(i)))
				if (CAN_SEE(ch, i)) {
					if (j == number)
						return (i);
					j++;
				}
	}			/* END OF FOR LOOP */

	return (0);

}				/* END OF ha3100_get_char_vis_exact() */


/*search the entire world for an object, and return a pointer  */
struct obj_data *ha3200_get_obj_vis(struct char_data * ch, char *name)
{
	struct obj_data *i;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;



	/* scan items carried */
	if ((i = ha2075_get_obj_list_vis(ch, name, ch->carrying)))
		return (i);

	/* scan room */
	if ((i = ha2075_get_obj_list_vis(ch, name, world[ch->in_room].contents)))
		return (i);

	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = ha1975_get_number(&tmp)))
		return (0);

	/* ok.. no luck yet. scan the entire obj list   */
	for (i = object_list, j = 1; i && (j <= number); i = i->next)
		if (ha1150_isname(tmp, i->name))
			if (CAN_SEE_OBJ(ch, i)) {
				if (j == number)
					return (i);
				j++;
			}
	return (0);
}


struct obj_data *ha3400_create_money(int amount)
{
	struct obj_data *obj;
	struct extra_descr_data *new_descr;
	char buf[80];



	if (amount <= 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Try to create negative money.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	CREATE(obj, struct obj_data, 1);
	CREATE(new_descr, struct extra_descr_data, 1);
	db7600_clear_object(obj);

	if (amount == 1) {
		obj->name = strdup("coin gold");
		obj->short_description = strdup("a gold coin");
		obj->description = strdup("One miserable gold coin.");

		new_descr->keyword = strdup("coin gold");
		new_descr->description = strdup("One miserable gold coin.");
	}
	else {
		obj->name = strdup("coins gold");
		obj->short_description = strdup("gold coins");
		obj->description = strdup("A pile of gold coins.");

		new_descr->keyword = strdup("coins gold");
		if (amount < 10) {
			sprintf(buf, "There is %d coins.", amount);
			new_descr->description = strdup(buf);
		}
		else if (amount < 100) {
			sprintf(buf, "There is about %d coins", 10 * (amount / 10));
			new_descr->description = strdup(buf);
		}
		else if (amount < 1000) {
			sprintf(buf, "It looks like something round %d coins", 100 * (amount / 100));
			new_descr->description = strdup(buf);
		}
		else if (amount < 100000) {
			sprintf(buf, "You guess there is %d coins", 1000 * ((amount / 1000) + number(0, (amount / 1000))));
			new_descr->description = strdup(buf);
		}
		else
			new_descr->description = strdup("There is A LOT of coins");
	}

	new_descr->next = 0;
	obj->ex_description = new_descr;

	obj->obj_flags.type_flag = ITEM_MONEY;
	obj->obj_flags.wear_flags = ITEM_TAKE;
	obj->obj_flags.value[0] = amount;
	obj->obj_flags.cost = amount;
	obj->item_number = -1;

	obj->next = object_list;
	object_list = obj;

	return (obj);
}



/* Generic Find, designed to find any object/character                    */
/* Calling :                                                              */
/*  *arg     is the sting containing the string to be searched for.       */
/*           This string doesn't have to be a single word, the routine    */
/*           extracts the next word itself.                               */
/*  bitv..   All those bits that you want to "search through".            */
/*           Bit found will be result of the function                     */
/*  *ch      This is the person that is trying to "find"                  */
/*  **tar_ch Will be NULL if no character was found, otherwise points     */
/* **tar_obj Will be NULL if no object was found, otherwise points        */
/*                                                                        */
/* The routine returns a pointer to the next word in *arg (just like the  */
/* one_argument routine).                                                 */

int ha3500_generic_find(char *arg, int bitvector, struct char_data * ch,
		       struct char_data ** tar_ch, struct obj_data ** tar_obj)
{
	static const char *ignore[] = {
		"the",
		"in",
		"on",
		"at",
	"\n"};

	int i;
	char name[256];
	bool found;



	found = FALSE;


	/* Eliminate spaces and "ignore" words */
	while (*arg && !found) {

		for (; *arg == ' '; arg++);

		for (i = 0; (name[i] = *(arg + i)) && (name[i] != ' '); i++);
		name[i] = 0;
		arg += i;
		if (search_block(name, ignore, TRUE) > -1)
			found = TRUE;

	}

	if (!name[0])
		return (0);

	*tar_ch = 0;
	*tar_obj = 0;

	if (IS_SET(bitvector, FIND_CHAR_ROOM)) {	/* Find person in room */
		if ((*tar_ch = ha2125_get_char_in_room_vis(ch, name))) {
			return (FIND_CHAR_ROOM);
		}
	}

	if (IS_SET(bitvector, FIND_CHAR_WORLD)) {
		if ((*tar_ch = ha3100_get_char_vis(ch, name))) {
			return (FIND_CHAR_WORLD);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_INV)) {
		if ((*tar_obj = ha2075_get_obj_list_vis(ch, name, ch->carrying))) {
			return (FIND_OBJ_INV);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_EQUIP)) {
		for (found = FALSE, i = 0; i < MAX_WEAR && !found; i++)	/* used to be str_cmp */
			if (ch->equipment[i] && ha1150_isname(name, ch->equipment[i]->name)) {
				*tar_obj = ch->equipment[i];
				found = TRUE;
			}
		if (found) {
			return (FIND_OBJ_EQUIP);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_ROOM)) {
		if ((*tar_obj = ha2075_get_obj_list_vis(ch, name, world[ch->in_room].contents))) {
			return (FIND_OBJ_ROOM);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_WORLD)) {
		if ((*tar_obj = ha3200_get_obj_vis(ch, name))) {
			return (FIND_OBJ_WORLD);
		}
	}

	return (0);
}

void ha3600_extract_zone_objs(struct obj_data * obj)
{



	if (obj) {
		ha3600_extract_zone_objs(obj->contains);
		ha3600_extract_zone_objs(obj->next_content);
		if (obj->item_number >= 0 && obj->item_number <= top_of_objt)
			if (IS_SET(zone_table[obj_index[obj->item_number].zone].flags, ZONE_TESTING)) {
				ha1800_obj_from_char(obj);
				ha2700_extract_obj(obj);
			}
	}
}

void ha3700_char_del_zone_objs(struct char_data * ch)
{
	int i;
	struct obj_data *obj;
	char buf[MAX_STRING_LENGTH];



	send_to_char("Removing objects still being tested.\r\n", ch);
	sprintf(buf, "ERROR: Not removing objects for %s in room %d\r\n",
		GET_REAL_NAME(ch), world[ch->in_room].number);
	do_sys(buf, 1, ch);
	spec_log(buf, ERROR_LOG);
	return;

	for (i = 0; i < MAX_WEAR; i++)
		if ((obj = ch->equipment[i])) {
			if (obj->item_number >= 0 && obj->item_number <= top_of_objt)
				if (IS_SET(zone_table[obj_index[obj->item_number].zone].flags, ZONE_TESTING))
					ha1700_obj_to_char(ha1930_unequip_char(ch, i), ch);
		}
	if (ch->carrying)
		ha3600_extract_zone_objs(ch->carrying);

}


/* Check if making CH follow VICTIM will create an illegal */
/* Follow "Loop/circle"                                    */
bool ha4000_circle_follow(struct char_data * ch, struct char_data * victim)
{
	struct char_data *k;



	for (k = victim; k; k = k->master) {
		if (k == ch)
			return (TRUE);
	}

	return (FALSE);
}



/* Called when stop following persons, or stopping charm */
/* This will NOT do if a character quits/dies!!          */
void ha4100_stop_follower(struct char_data * ch, int lv_stop_flag)
{

	struct follow_type *j, *k;



	/* MAKE SURE WE GOT HERE ONLY WHEN FOLLOWING SOMEONE */
	assert(ch->master);

	/* WERE WE FOLLOWING BECAUSE OF CHARM? */
	if (IS_AFFECTED(ch, AFF_CHARM)) {
		if (lv_stop_flag == END_EXTRACT_BY_RENT ||
		    lv_stop_flag == END_EXTRACT_BY_RAW_KILL) {
			act("You realize that you'll miss $N!", FALSE, ch, 0, ch->master, TO_CHAR);
			act("$n realizes that $e'll miss $N!", TRUE, ch, 0, ch->master, TO_NOTVICT);
			act("$n will miss you!", TRUE, ch, 0, ch->master, TO_VICT);
		}
		else {
			act("You realize that $N is a jerk!", FALSE, ch, 0, ch->master, TO_CHAR);
			act("$n realizes that $N is a jerk!", TRUE, ch, 0, ch->master, TO_NOTVICT);
			act("$n hates your guts!", TRUE, ch, 0, ch->master, TO_VICT);
		}

		/* IF IT WAS A SPELL, REMOVE IT */
		if (ha1375_affected_by_spell(ch, SPELL_CHARM_PERSON))
			ha1350_affect_from_char(ch, SPELL_CHARM_PERSON);
	}
	else {
		act("You stop following $N.", FALSE, ch, 0, ch->master, TO_CHAR);
		act("$n stops following $N.", TRUE, ch, 0, ch->master, TO_NOTVICT);
		act("$n stops following you.", TRUE, ch, 0, ch->master, TO_VICT);
	}

	if (ch->master->followers->follower == ch) {
		/* Head of follower-list? */
		k = ch->master->followers;
		ch->master->followers = k->next;
		free(k);
	}
	else {
		/* locate follower who is not head of list */
		for (k = ch->master->followers; k->next->follower != ch; k = k->next);
		j = k->next;
		k->next = j->next;
		free(j);
	}

	ha9900_sanity_check(0, "FREE81", "SYSTEM");
	ch->master = 0;
	REMOVE_BIT(ch->specials.affected_by, AFF_CHARM | AFF_GROUP);

}				/* END OF ha4100_stop_follower() */



/* Called when a character that follows/is followed dies */
void ha4200_die_follower(struct char_data * ch, int lv_die_flag)
{

	struct follow_type *save_next, *k, *j, *new, *next_follower;
	struct char_data *leader;



	if (ch->master) {	/* leader didn't die */
		ha4100_stop_follower(ch, lv_die_flag);
	}

	/* remove all NPC chars who are following */
	new = ch->followers;
	if (!(new))
		return;		/* no leader, no followers */

	/* IF ITS AN NPC, REMOVE IT FROM THE LIST */
	/*
   if (IS_NPC(new->follower)) {
		 ha4100_stop_follower(new->follower, lv_die_flag);
   }
	 */
	for (k = new; k; k = next_follower) {
		next_follower = k->next;
		if (IS_NPC(k->follower)) {
			ha4100_stop_follower(k->follower, lv_die_flag);
		}
	}

	/* next person steps up to bat */
	new = ch->followers;
	if (!(new))
		return;		/* no leader, no followers */

	if (new->next) {	/* make new the leader */
		leader = new->follower;
		leader->master = 0;	/* we're the leader ! */
		send_to_char("Gratz! you just became the new leader of the group!\n\r", leader);
		for (k = new->next; k; k = save_next) {
			save_next = k->next;
			send_to_char(GET_NAME(leader), k->follower);
			send_to_char(" is now the new leader of the group!\n\r", k->follower);
			k->follower->master = leader;
			CREATE(j, struct follow_type, 1);
			j->follower = k->follower;
			j->next = leader->followers;
			leader->followers = j;
			free(k);
		}
		free(new);
	}
	else {
		/* down to last person in group */
		ha4100_stop_follower(new->follower, lv_die_flag);
	}

	ha9900_sanity_check(0, "FREE82", "SYSTEM");
}				/* END OF ha4200_die_follower() */


/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void ha4300_add_follower(struct char_data * ch, struct char_data * leader)
{

	struct follow_type *lv_k;



	assert(!ch->master);

	ch->master = leader;

	CREATE(lv_k, struct follow_type, 1);
	lv_k->follower = ch;
	lv_k->next = leader->followers;
	leader->followers = lv_k;

	act("You now follow $N.", FALSE, ch, 0, leader, TO_CHAR);
	act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);
	act("$n now follows $N.", TRUE, ch, 0, leader, TO_NOTVICT);

}				/* END OF ha4300_add_follower() */


/* IF CALLED WITH BIT0 set, DOOR LOCKES ARE IGNORED */
/*                BIT1 set, MESSAGES AREN'T PRINTED */
int ha5000_manually_open_door(int lv_room, int lv_dir, int lv_flag)
{

	struct room_direction_data *back, *lv_exit;
	int other_room;
	char buf[MAX_STRING_LENGTH];



	if (lv_room < 0) {
		return (FALSE);
	}

	if (lv_dir < 0 || lv_dir >= MAX_DIRS) {
		return (FALSE);
	}

	lv_exit = world[lv_room].dir_option[lv_dir];

	/* MAKE SURE THE EXIT EXIST */
	if (!lv_exit) {
		return (FALSE);
	}

	/* IS THE DOOR ALREADY OPEN? */
	if (!IS_SET(lv_exit->exit_info, EX_CLOSED)) {
		return (FALSE);
	}

	/* IS THE DOOR LOCKED? */
	if (IS_SET(lv_exit->exit_info, EX_LOCKED)) {
		if (!IS_SET(lv_flag, BIT0)) {
			return (FALSE);
		}

	}

	/* LETS OPEN THAT PUPPY */
	REMOVE_BIT(lv_exit->exit_info, EX_LOCKED);
	REMOVE_BIT(lv_exit->exit_info, EX_CLOSED);

	/* now for opening the OTHER side of the door! */
	if ((other_room = lv_exit->to_room) != NOWHERE) {
		if ((back = world[other_room].dir_option[reverse_dir[lv_dir]])) {
			if (back->to_room == lv_room) {
				REMOVE_BIT(back->exit_info, EX_CLOSED);
				if (!IS_SET(lv_flag, BIT1)) {
					if (back->keyword) {
						sprintf(buf, "The %s is opened from the other side.\r\n",
						 ha1100_fname(back->keyword));
						co3300_send_to_room(buf, lv_exit->to_room);
					}
					else {
						co3300_send_to_room("The door is opened from the other side.\r\n",
							    lv_exit->to_room);
					}
				}
			}
		}
	}			/* END OF other room not NOWHERE */

	return (TRUE);

}				/* END OF ha5000_manually_open_door() */


/* IF CALLED WITH BIT0 set, DOOR IS LOCKED */
/*                BIT1 set, MESSAGES AREN'T PRINTED */
int ha5100_manually_close_door(int lv_room, int lv_dir, int lv_flag)
{

	struct room_direction_data *back, *lv_exit;
	int other_room;
	char buf[MAX_STRING_LENGTH];



	if (lv_room < 0) {
		return (FALSE);
	}

	if (lv_dir < 0 || lv_dir >= MAX_DIRS) {
		return (FALSE);
	}

	lv_exit = world[lv_room].dir_option[lv_dir];

	/* MAKE SURE THE EXIT EXIST */
	if (!lv_exit) {
		return (FALSE);
	}

	/* IS THE DOOR ALREADY CLOSED? */
	if (IS_SET(lv_exit->exit_info, EX_CLOSED)) {
		return (FALSE);
	}

	/* LETS CLOSE THAT PUPPY */
	SET_BIT(lv_exit->exit_info, EX_CLOSED);
	if (IS_SET(lv_flag, BIT0)) {
		SET_BIT(lv_exit->exit_info, EX_LOCKED);
	}

	/* now for opening the OTHER side of the door! */
	if ((other_room = lv_exit->to_room) != NOWHERE) {
		if ((back = world[other_room].dir_option[reverse_dir[lv_dir]])) {
			if (back->to_room == lv_room) {
				SET_BIT(back->exit_info, EX_CLOSED);
				if (IS_SET(lv_flag, BIT0)) {
					SET_BIT(back->exit_info, EX_LOCKED);
				}
				if (!IS_SET(lv_flag, BIT1)) {
					if (back->keyword) {
						sprintf(buf, "The %s closes.\r\n",
						 ha1100_fname(back->keyword));
						co3300_send_to_room(buf, lv_exit->to_room);
					}
					else {
						co3300_send_to_room("The door closes.\r\n",
							    lv_exit->to_room);
					}
				}
			}
		}
	}			/* END OF other room not NOWHERE */

	return (TRUE);

}				/* END OF ha5100_manually_close_door() */


void ha9000_open_tracefile(int lv_direction)
{

	char lv_file_name[MAX_STRING_LENGTH];



	bzero(lv_file_name, sizeof(lv_file_name));
	if (gv_next_trace_file > 8) {
		gv_next_trace_file = 0;
	}
	gv_next_trace_file++;

	/* STAY WITH FILE 1 */
	gv_next_trace_file = 1;

	if (lv_direction == 1) {
		sprintf(lv_file_name, "logs/tracefile.%d",
			gv_next_trace_file);
	}
	else {
		sprintf(lv_file_name, "../logs/tracefile.%d",
			gv_next_trace_file);
	}

	printf("Opening %s\r\n", lv_file_name);
	if (gv_next_trace_file == 1) {
		trace_file = fopen(lv_file_name, "ab");
	}
	else {
		trace_file = fopen(lv_file_name, "wb");
	}

	return;

}				/* END OF ha9000_open_tracefile() */


void ha9700_set_saving_throws(struct char_data * ch, int lv_flag)
{


#define ASSIGN_SAVING(para, rod, petri, breath, spell) {           \
	ch->specials.apply_saving_throw[SAVING_PARA]   = (para);   \
		ch->specials.apply_saving_throw[SAVING_ROD]    = (rod);    \
			ch->specials.apply_saving_throw[SAVING_PETRI]  = (petri);  \
        ch->specials.apply_saving_throw[SAVING_BREATH] = (breath); \
					ch->specials.apply_saving_throw[SAVING_SPELL]  = (spell);  \
						return;                                                    \
}



	if (GET_LEVEL(ch) < 4) {
		ASSIGN_SAVING(14, 16, 16, 17, 17);
	}
	else if (GET_LEVEL(ch) < 8) {
		ASSIGN_SAVING(13, 15, 14, 16, 16);
	}
	else if (GET_LEVEL(ch) < 12) {
		ASSIGN_SAVING(11, 13, 12, 13, 14);
	}
	else if (GET_LEVEL(ch) < 16) {
		ASSIGN_SAVING(10, 12, 11, 12, 13);
	}
	else if (GET_LEVEL(ch) < 20) {
		ASSIGN_SAVING(8, 10, 8, 10, 9);
	}
	else if (GET_LEVEL(ch) < 24) {
		ASSIGN_SAVING(7, 9, 8, 8, 10);
	}
	else if (GET_LEVEL(ch) < 28) {
		ASSIGN_SAVING(5, 7, 6, 5, 8);
	}
	else if (GET_LEVEL(ch) < 32) {
		ASSIGN_SAVING(4, 6, 5, 4, 7);
	}
	else if (GET_LEVEL(ch) < 36) {
		ASSIGN_SAVING(3, 5, 4, 4, 6);
	}
	else if (GET_LEVEL(ch) < 40) {
		ASSIGN_SAVING(2, 4, 3, 3, 5);
	}
	else if (GET_LEVEL(ch) < IMO_LEV) {
		ASSIGN_SAVING(2, 3, 3, 2, 4);
	}
	else if (GET_LEVEL(ch) < IMO_SPIRIT || IS_NPC(ch)) {
		ASSIGN_SAVING(2, 2, 2, 2, 2);	/* AVATARS AND REMAINING MOBS */
	}
	else if (GET_LEVEL(ch) < IMO_IMP) {
		ASSIGN_SAVING(1, 1, 1, 1, 1);	/* IMMORTS */
	}
	else {
		ASSIGN_SAVING(0, 0, 0, 0, 0);
	}

	return;

}				/* END OF ha9700_set_saving_throws() */


int ha9800_protect_char_from_dt(struct char_data * ch, int to_room, int lv_flag)
{



	if (to_room < 0)
		return (1);

	if (GET_LEVEL(ch) > 5) {
		return (FALSE);
	}

	if (IS_SET(world[to_room].room_flags, RM1_DEATH)) {
		if (number(0, 1)) {
			send_to_char("The Gods recommend you stay out of there!\r\n", ch);
			return (TRUE);
		}
	}

	return (FALSE);

}				/* END OF */


int ha9900_sanity_check(struct char_data * in_ch, char *lv_location, char *lv_name)
{

	char buf[MAX_STRING_LENGTH];
	struct char_data *ch, *hunt_ch, *last_dude;
	char *ptr;

	/* WE THINK ITS FIXED!!!! */
	return (0);

	/* printf("%s\r\n", lv_location); */

	/* WE NEED TO ZERO OUT ANY FREED MEMORY SO WE CAN CHECK */
	/* FOR THE BUG */
	ptr = malloc(5000);
	bzero(ptr, 5000);
	strcpy(ptr, gv_str);
	free(ptr);

	/* IF WE GOT A PREVIOUS character_list ERROR, DON'T BOTHER */
	if (gv_sanity_return == 1)
		return (0);

	if (gv_port != 4000 &&
	    gv_port != 5000) {
		return (gv_sanity_return);
	}

	last_dude = 0;
	/* SANITY CHECK */
	for (ch = character_list; ch; ch = ch->next) {
		if (FAILS_SANITY(ch)) {
			gv_sanity_return = 1;
			bzero(buf, sizeof(buf));
			sprintf(buf, "gv_str=%s", gv_str);
			main_log(buf);
			if (last_dude) {
				sprintf(buf, "ERROR1: Failed sanity check. LOC=%s Name=%s.",
					lv_location, lv_name);
				do_sys(buf, 1, last_dude);
				sprintf(buf, "last char in list: %s",
					GET_REAL_NAME(last_dude));
				do_sys(buf, IMO_IMP, last_dude);
				spec_log(buf, ERROR_LOG);
				sprintf(buf, "beg %d  end %d",
					ch->beg_rec_id, ch->end_rec_id);
				do_sys(buf, IMO_IMP, last_dude);
				spec_log(buf, ERROR_LOG);
				if (ch->in_room < 0)
					sprintf(buf, "Invalid char is in room: %d",
						ch->in_room);
				else
					sprintf(buf, "Invalid char is in room: %d real room: %d",
						ch->in_room, world[ch->in_room].number);
				do_sys(buf, IMO_IMP, last_dude);
				spec_log(buf, ERROR_LOG);

				/* THIS ISN'T A FIX - ITS TO STOP SPAMMING */
				ch->beg_rec_id = CH_SANITY_CHECK;
				ch->end_rec_id = CH_SANITY_CHECK;
			}
			else {
				/* UH OH, THE FIRST GUY IN THE CHARACTER LIST
				 * IS BAD */
				sprintf(buf, "******ERROR***** SANITY ABORT\r\n");
				ABORT_PROGRAM();
			}
		}		/* END OF faild sanity check */
		else {
			if (ch->in_room < 0) {
				sprintf(buf, "ERROR: %s has negative room.\r\n",
					GET_REAL_NAME(ch));
				main_log(buf);
				spec_log(buf, ERROR_LOG);
			}
		}		/* END OF else */
		/* NEGATIVE ROOM? */

		if (ch->in_room < 0) {
			sprintf(buf, "ERROR1: Char in negative room LOC=%s Name=%s.",
				lv_location, lv_name);
			do_sys(buf, 1, ch);
			spec_log(buf, ERROR_LOG);
		}		/* END OF A negative room */

		last_dude = ch;
		/* ARE WE HUNTING? */
		if (HUNTING(ch)) {
			hunt_ch = HUNTING(ch);
			if (FAILS_SANITY(hunt_ch)) {
				gv_sanity_return = 2;
				bzero(buf, sizeof(buf));
				sprintf(buf, "gv_str=%s", gv_str);
				main_log(buf);
				sprintf(buf, "ERROR2: Failed perform_hunt sanity check for mob being hunted by %s",
					GET_REAL_NAME(ch));
				do_sys(buf, 1, ch);
				spec_log(buf, ERROR_LOG);
				/* FIX POINTER */
				HUNTING(ch) = 0;
			}
		}
	}

	return (gv_sanity_return);

}				/* END OF ha9900_sanity_check() */

int ha9925_queue_check(struct txt_q * lv_queue, char *lv_location, char *lv_name)
{
	int lv_return;
	char buf[MAX_STRING_LENGTH];
	struct txt_block *q;



	lv_return = 0;

	if (gv_port != 4000 &&
	    gv_port != 5000) {
		return (lv_return);
	}

	for (q = lv_queue->head; q; q = q->next) {
		if (q->beg_rec_id != QUEUE_SANITY_CHECK ||
		    q->end_rec_id != QUEUE_SANITY_CHECK) {
			lv_return = 1;
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERRORh: Failed queue sanity check. LOC=%s Name=%s.",
				lv_location, lv_name);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			bzero(buf, sizeof(buf));
			sprintf(buf, "text = %s\r\n", q->text);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			return (lv_return);	/* SO WE DON'T GET IN A LOOP */
		}
	}

	for (q = lv_queue->tail; q; q = q->next) {
		if (q->beg_rec_id != QUEUE_SANITY_CHECK ||
		    q->end_rec_id != QUEUE_SANITY_CHECK) {
			lv_return = 1;
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERRORt: Failed queue sanity check. LOC=%s Name=%s.",
				lv_location, lv_name);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			return (lv_return);	/* SO WE DON'T GET IN A LOOP */
		}

	}

	return (lv_return);

}				/* END OF ha9925_queue_check() */


/* IF WE ENTER WITH FLAG EQUAL TO 1, ALLOW 10 FOLLOWERS */
int ha9950_maxed_on_followers(struct char_data * ch, struct char_data * victim, int lv_flag)
{

	struct follow_type *lv_k;

	int num_mobs, num_chars, lv_max;



	num_mobs = 0;
	num_chars = 0;
	for (lv_k = ch->followers; lv_k; lv_k = lv_k->next) {
		if ((lv_k->follower) && IS_NPC(lv_k->follower))
			num_mobs++;
		else
			num_chars++;
	}			/* END OF for loop */

	/* MAX FOLLOWERS ? */
	lv_max = GET_LEVEL(ch) / 10;

	/* ADJUST FOR CHARISMA */
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) < 6)
		lv_max--;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) < 11)	/* ACCUMALATIVE */
		lv_max--;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 14)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 19)	/* ACCUMULATIVE */
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 24)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 29)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 34)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 39)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 44)
		lv_max++;
	if (GET_CHA(ch) + GET_BONUS_CHA(ch) > 49)
		lv_max++;

	if (lv_max < 1)
		lv_max = 1;

	/* MAX PC FOLLOWER CHECK */
	if (num_mobs + num_chars > 11)
		return (TRUE);

	/* MAX MOB FOLLOWER CHECK */
	if (num_mobs >= lv_max)
		return (TRUE);

	return (FALSE);

}				/* END OF ha9950_maxed_on_followers() */
