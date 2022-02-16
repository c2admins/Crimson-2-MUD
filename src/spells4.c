/* s4 */
/* gv_location: 19001-19500 */
/* *******************************************************************
*  file: spells4.c , handling of magic.              Part of DIKUMUD *
*  Usage : Procedures handling all offensive magic.                  *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "parser.h"
#include "spells.h"
#include "handler.h"
#include "globals.h"
#include "func.h"
#include "constants.h"

void cast_pestilence(sbyte level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_pestilence(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_pestilence(level, ch, victim, 0);
		else if (!tar_obj)
			spell_pestilence(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_pestilence(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in pestilence!");
		spec_log("ERROR: Invalid spell type in pestilence!", ERROR_LOG);
		break;
	}
}				/* END OF cast_pestilence() */


void cast_sense_danger(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



}				/* END OF cast_sense_danger() */


void cast_spiritual_transfer(sbyte level, struct char_data * ch, char
			      *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_spiritual_transfer(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		main_log("ERROR: Potion used for spiritual transfer, please remove item!");
		spec_log("ERROR: Potion used for spiritual transfer, please remove item!", ERROR_LOG);
		break;
	case SPELL_TYPE_SCROLL:
		spell_spiritual_transfer(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		main_log("ERROR: Staff used for spiritual transfer, please remove item!");
		spec_log("ERROR: Staff used for spiritual transfer, please remove item!", ERROR_LOG);
		break;
	case SPELL_TYPE_WAND:
		main_log("ERROR: Wand used for spiritual transfer, please remove item!");
		spec_log("ERROR: Wand used for spiritual transfer, please remove item!", ERROR_LOG);
		break;
	default:
		main_log("ERROR: Invalid spell type in cast_spiritual_transfer!");
		spec_log("ERROR: Invalid spell type in cast_spiritual_transfer!", ERROR_LOG);
		break;
	}

}				/* END OF cast_spiritual_transfer() */


void cast_bind_souls(sbyte level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_bind_souls(level, ch, ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_bind_souls(level, ch, ch, 0);
		return;
		spell_bind_souls(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (ha1375_affected_by_spell(victim, SPELL_BIND_SOULS))
			return;
		spell_bind_souls(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				if (!(IS_AFFECTED(victim, SPELL_BIND_SOULS)))
					spell_bind_souls(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		break;
	default:
		main_log("ERROR: Invalid spell type in bind souls!");
		spec_log("ERROR: Invalid spell type in bind souls!", ERROR_LOG);
		break;
	}
}				/* END OF cast_blind_souls() */

void cast_mordenkainens_sword(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_mordenkainens_sword(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		spell_mordenkainens_sword(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in mordenkainens sword!");
		spec_log("ERROR: Invalid spell type in mordenkainens sword!", ERROR_LOG);
		break;
	}

}				/* END OF cast_mordenkainens_sword() */

void cast_berserk(sbyte level, struct char_data * ch, char *arg, int type,
		    struct char_data * victim, struct obj_data * tar_obj)
{



}				/* END OF cast_berserk() */


void cast_call_lightning(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	extern struct weather_data weather_info;



	switch (type) {
	case SPELL_TYPE_SPELL:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_LIGHTNING)) {
			spell_call_lightning(level, ch, victim, 0);
		}
		else {
			send_to_char("You fail to call upon a lightning strike!\n\r", ch);
		}
		break;
	case SPELL_TYPE_POTION:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_LIGHTNING)) {
			spell_call_lightning(level, ch, ch, 0);
		}
		break;
	case SPELL_TYPE_SCROLL:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_LIGHTNING)) {
			if (victim)
				spell_call_lightning(level, ch, victim, 0);
			else if (!tar_obj)
				spell_call_lightning(level, ch, ch, 0);
		}
		break;
	case SPELL_TYPE_STAFF:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_LIGHTNING)) {
			for (victim = world[ch->in_room].people;
			     victim; victim = victim->next_in_room)
				if (victim != ch)
					spell_call_lightning(level, ch, victim, 0);
		}
		break;
	default:
		main_log("ERROR: Invalid spell type in call_lightning!");
		spec_log("ERROR: Invalid spell type in call_lightning!", ERROR_LOG);
		break;
	}

}				/* END OF cast_call_lightning() */


void cast_transform_wolf(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_wolf(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast wolf form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_wolf(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_wolf(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_wolf(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_wolf(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform_wolf!");
		spec_log("ERROR: Invalid spell type in transform_wolf!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_wolf() */

void cast_transform_bear(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{
	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_bear(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast bear form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_bear(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_bear(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_bear(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_bear(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transfor_bear!");
		spec_log("ERROR: Invalid spell type in transfor_bear!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_bear() */

void cast_transform_dragon(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{
	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_dragon(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast dragon form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}

		else {

			spell_transform_dragon(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:

		if (victim)
			spell_transform_dragon(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_dragon(level, ch, ch, 0);
		break;

	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_dragon(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform_dragon!");
		spec_log("ERROR: Invalid spell type in transform_dragon!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_dragon() */


void cast_transform_monkey(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_monkey(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast monkey form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {

			spell_transform_monkey(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_monkey(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_monkey(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_monkey(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform_monkey!");
		spec_log("ERROR: Invalid spell type in transform_monkey!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_monkey() */

void cast_transform_ninja(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_ninja(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast ninja form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {

			spell_transform_ninja(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_ninja(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_ninja(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_ninja(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transfor_ninja!");
		spec_log("ERROR: Invalid spell type in transfor_ninja!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_ninja() */

void cast_detect_giants(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_giants(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_giants(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_giants(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_giants(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_giants(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_giants(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect giants!");
		spec_log("ERROR: Invalid spell type in detect giants!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_giants() */

void cast_faerie_fog(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_faerie_fog(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_faerie_fog(level, ch, victim, 0);
		else if (!tar_obj)
			spell_faerie_fog(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_faerie_fog(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in faerie fog!");
		spec_log("ERROR: Invalid spell type in faerie fog!", ERROR_LOG);
		break;
	}
}				/* END OF cast_faerie_fog() */

void cast_detect_shadows(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_shadows(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_shadows(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_shadows(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_shadows(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_shadows(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_shadows(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect shadows!");
		spec_log("ERROR: Invalid spell type in detect shadows!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_shadows() */


void cast_barkskin(signed char level, struct char_data * ch, char *arg, int type,
		     struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_barkskin(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_barkskin(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_barkskin(level, ch, victim, 0);
		else if (!tar_obj)
			spell_barkskin(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_barkskin(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in barkskin!");
		spec_log("ERROR: Invalid spell type in barkskin!", ERROR_LOG);
		break;
	}
}				/* END OF cast_barkskin() */


void cast_stoneskin(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_stoneskin(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_stoneskin(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_stoneskin(level, ch, victim, 0);
		else if (!tar_obj)
			spell_stoneskin(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in stoneskin!");
		spec_log("ERROR: Invalid spell type in stoneskin!", ERROR_LOG);
		break;
	}
}				/* END OF cast_stoneskin() */



void cast_thornwrack(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_thornwrack(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_thornwrack(level, ch, victim, 0);
		else if (!tar_obj)
			spell_thornwrack(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_thornwrack(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in thornwrack!");
		spec_log("ERROR: Invalid spell type in thornwrack!", ERROR_LOG);
		break;
	}
}				/* END OF cast_thornwrack() */



void cast_detect_demons(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_demons(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_demons(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_demons(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_demons(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_demons(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_demons(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect demons!");
		spec_log("ERROR: Invalid spell type in detect demons!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_demons() */



void cast_detect_dragons(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_dragons(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_dragons(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_dragons(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_dragons(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_dragons(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_dragons(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect dragons!");
		spec_log("ERROR: Invalid spell type in detect dragons!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_dragons() */



void cast_detect_undead(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_undead(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_undead(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_undead(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_undead(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_undead(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_undead(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect undead!");
		spec_log("ERROR: Invalid spell type in detect undead!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_undead() */



void cast_animal_summoning(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_animal_summoning(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in animal summoning!");
		spec_log("ERROR: Invalid spell type in animal summoning!", ERROR_LOG);
		break;
	}
}				/* END OF cast_animal_summoning() */


void cast_detect_vampires(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj){

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_vampires(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_vampires(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_vampires(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_vampires(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_vampires(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_vampires(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect vampires!");
		spec_log("ERROR: Invalid spell type in detect vampires!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_vampires() */

void cast_earthmaw(signed char level, struct char_data * ch, char *arg, int type,
		     struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_earthmaw(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_earthmaw(level, ch, victim, 0);
		else if (!tar_obj)
			spell_earthmaw(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_earthmaw(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in earthmaw!");
		spec_log("ERROR: Invalid spell type in earthmaw!", ERROR_LOG);
		break;
	}
}				/* END OF cast_earthmaw() */


void cast_needlestorm(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_needlestorm(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_needlestorm(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_needlestorm(level, ch, victim, 0);
		else if (!tar_obj)
			spell_needlestorm(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_needlestorm(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in needlestorm!");
		spec_log("ERROR: Invalid spell type in needlestorm!", ERROR_LOG);
		break;
	}
}				/* END OF cast_needlestorm() */



void cast_treewalk(signed char level, struct char_data * ch, char *arg, int type,
		     struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_treewalk(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in treewalk!");
		spec_log("ERROR: Invalid spell type in treewalk!", ERROR_LOG);
		break;
	}
}				/* END OF cast_treewalk() */



void cast_summon_treant(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_summon_treant(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_summon_treant(level, ch, victim, 0);
		else if (!tar_obj)
			spell_summon_treant(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in summon treant!");
		spec_log("ERROR: Invalid spell type in summon treant!", ERROR_LOG);
		break;
	}
}				/* END OF cast_summon_treant() */



void cast_sporecloud(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sporecloud(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_sporecloud(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_sporecloud(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sporecloud!");
		spec_log("ERROR: Invalid spell type in sporecloud!", ERROR_LOG);
		break;
	}
}				/* END OF cast_sporecloud() */



void cast_wrath_of_nature(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_wrath_of_nature(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_wrath_of_nature(level, ch, victim, 0);
		else if (!tar_obj)
			spell_wrath_of_nature(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in wrath of nature!");
		spec_log("ERROR: Invalid spell type in wrath of nature!", ERROR_LOG);
		break;
	}
}				/* END OF cast_wrath of nature() */

void cast_faerie_fire(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_faerie_fire(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_faerie_fire(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_faerie_fire(level, ch, victim, 0);
		else if (!tar_obj)
			spell_faerie_fire(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_faerie_fire(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in faerie fire!");
		spec_log("ERROR: Invalid spell type in faerie fire!", ERROR_LOG);
		break;
	}
}				/* END OF cast_faerie_fire() */

void cast_beast_transform(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	level = 1;
	switch (type) {
	case SPELL_TYPE_POTION:
		spell_beast_transform(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_beast_transform(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_beast_transform(level, ch, victim, 0);
		else if (!tar_obj)
			spell_beast_transform(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_beast_transform(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in beast transform!");
		spec_log("ERROR: Invalid spell type in beast transform!", ERROR_LOG);
		break;
	}
}				/* END OF cast_beast_transform() */



void cast_create_spring(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_create_spring(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_create_spring(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_create_spring(level, ch, victim, 0);
		else if (!tar_obj)
			spell_create_spring(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in create spring!");
		spec_log("ERROR: Invalid spell type in create spring!", ERROR_LOG);
		break;
	}
}				/* END OF cast_create_spring() */



void cast_leviathan(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_leviathan(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in leviathan!");
		spec_log("ERROR: Invalid spell type in leviathan!", ERROR_LOG);
		break;
	}
}				/* END OF cast_leviathan() */



void cast_meteor_swarm(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_meteor_swarm(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_meteor_swarm(level, ch, victim, 0);
		else if (!tar_obj)
			spell_meteor_swarm(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_meteor_swarm(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in meteor swarm!");
		spec_log("ERROR: Invalid spell type in meteor swarm!", ERROR_LOG);
		break;
	}
}				/* END OF cast_meteor_swarm() */



void cast_tsunami(signed char level, struct char_data * ch, char *arg, int type,
		    struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_tsunami(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_tsunami(level, ch, victim, 0);
		else if (!tar_obj)
			spell_tsunami(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in tsunami!");
		spec_log("ERROR: Invalid spell type in tsunami!", ERROR_LOG);
		break;
	}
}				/* END OF cast_tsunami() */



void cast_tornado(signed char level, struct char_data * ch, char *arg, int type,
		    struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_tornado(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_tornado(level, ch, victim, 0);
		else if (!tar_obj)
			spell_tornado(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in tornado!");
		spec_log("ERROR: Invalid spell type in tornado!", ERROR_LOG);
		break;
	}
}				/* END OF cast_tornado() */



void cast_landslide(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_landslide(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_landslide(level, ch, victim, 0);
		else if (!tar_obj)
			spell_landslide(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in landslide!");
		spec_log("ERROR: Invalid spell type in landslide!", ERROR_LOG);
		break;
	}
}				/* END OF cast_landslide() */



void cast_acid_blast(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_acid_blast(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_acid_blast(level, ch, victim, 0);
		else if (!tar_obj)
			spell_acid_blast(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_acid_blast(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in acid blast!");
		spec_log("ERROR: Invalid spell type in acid blast!", ERROR_LOG);
		break;
	}
}				/* END OF cast_acid_blast() */



void cast_displacement(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_displacement(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_displacement(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_displacement(level, ch, victim, 0);
		else if (!tar_obj)
			spell_displacement(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_displacement(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in displacement!");
		spec_log("ERROR: Invalid spell type in displacement!", ERROR_LOG);
		break;
	}
}				/* END OF cast_displacement() */



void cast_summon_elemental(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_summon_elemental(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in summon elemental!");
		spec_log("ERROR: Invalid spell type in summon elemental!", ERROR_LOG);
		break;
	}
}				/* END OF cast_summon_elemental() */


void cast_charm_animal(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_charm_animal(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_charm_animal(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_charm_animal(level, ch, victim, 0);
		else if (!tar_obj)
			spell_charm_animal(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_charm_animal(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in charm_animal!");
		spec_log("ERROR: Invalid spell type in charm_animal!", ERROR_LOG);
		break;
	}
}				/* END OF cast_charm_animal() */


void cast_shockwave(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_shockwave(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_shockwave(level, ch, victim, 0);
		else if (!tar_obj)
			spell_shockwave(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_shockwave(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in shockwave!");
		spec_log("ERROR: Invalid spell type in shockwave!", ERROR_LOG);
		break;
	}
}				/* END OF cast_shockwave() */


void cast_gust(signed char level, struct char_data * ch, char *arg, int type,
	         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_gust(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_gust(level, ch, victim, 0);
		else if (!tar_obj)
			spell_gust(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_gust(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in gust!");
		spec_log("ERROR: Invalid spell type in gust!", ERROR_LOG);
		break;
	}
}				/* END OF cast_gust() */



void cast_granite_fist(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_granite_fist(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_granite_fist(level, ch, victim, 0);
		else if (!tar_obj)
			spell_granite_fist(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_granite_fist(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in granite fist!");
		spec_log("ERROR: Invalid spell type in granite fist!", ERROR_LOG);
		break;
	}
}				/* END OF cast_granite_fist() */




void cast_prayer(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_prayer(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_prayer(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_prayer(level, ch, victim, 0);
		else if (!tar_obj)
			spell_prayer(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_prayer(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in prayer!");
		spec_log("ERROR: Invalid spell type in prayer!", ERROR_LOG);
		break;
	}
}				/* END OF cast_prayer() */

void cast_elemental_shield(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_elemental_shield(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_elemental_shield(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_elemental_shield(level, ch, victim, 0);
		else if (!tar_obj)
			spell_elemental_shield(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in elemental shield!");
		spec_log("ERROR: Invalid spell type in elemental shield!", ERROR_LOG);
		break;
	}
}				/* END OF cast_elemental_shield() */

void cast_holy_healing(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_holy_healing(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_holy_healing(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_holy_healing(level, ch, victim, 0);
		else if (!tar_obj)
			spell_holy_healing(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_holy_healing(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in holy healing!");
		spec_log("ERROR: Invalid spell type in holy healing!", ERROR_LOG);
		break;
	}
}				/* END OF cast_holy_healing() */

void cast_lifetap(sbyte level, struct char_data * ch, char *arg, int type,
		    struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_lifetap(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_lifetap(level, ch, victim, 0);
		else if (!tar_obj)
			spell_lifetap(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_lifetap(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in lifetap!");
		spec_log("ERROR: Invalid spell type in lifetap!", ERROR_LOG);
		break;

	}
}				/* END OF cast_lifetap() */

void cast_enchant_armor(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_enchant_armor(level, ch, 0, tar_obj);
		break;

	case SPELL_TYPE_SCROLL:
		if (!tar_obj)
			return;
		spell_enchant_armor(level, ch, 0, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in enchant armor!");
		spec_log("ERROR: Invalid spell type in enchant armor!", ERROR_LOG);
		break;
	}
}

void cast_vex(sbyte level, struct char_data * ch, char *arg, int type,
	        struct char_data * victim, struct obj_data * tar_obj)
{
	switch (type) {
		case SPELL_TYPE_POTION:
		spell_vex(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_vex(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_vex(level, ch, victim, 0);
		else if (!tar_obj)
			spell_vex(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_vex(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vex!");
		spec_log("ERROR: Invalid spell type in vex!", ERROR_LOG);
		break;
	}
}

void cast_transform_womble(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_womble(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast womble form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_womble(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_womble(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_womble(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_womble(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform_womble!");
		spec_log("ERROR: Invalid spell type in transform_womble!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_womble() */

void cast_gusher(sbyte level, struct char_data * ch, char *arg, int type,
		   struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_gusher(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in gusher!");
		spec_log("ERROR: Invalid spell type in gusher!", ERROR_LOG);
		break;
	}
}

void cast_transform_manticore(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_manticore(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast manticore form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_manticore(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_manticore(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_manticore(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_manticore(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform manticore!");
		spec_log("ERROR: Invalid spell type in transform manticore!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_manticore() */

void cast_sleep_immunity(sbyte level, struct char_data * ch, char *arg, int type, struct char_data * victim, struct obj_data *
			   tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sleep_immunity(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sleep_immunity(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_sleep_immunity(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sleep immunity!");
		spec_log("ERROR: Invalid spell type in sleep immunity!", ERROR_LOG);
		break;
	}
}

void cast_transform_chimera(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_chimera(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		if (GET_LEVEL(ch) < IMO_IMM) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast chimera form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_chimera(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_chimera(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_chimera(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_chimera(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform chimera!");
		spec_log("ERROR: Invalid spell type in transform chimera!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_chimera() */

void cast_transform_cow(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_transform_cow(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:

		if (GET_LEVEL(ch) < IMO_IMM && !ha1175_isexactname(GET_NAME(ch), "rlum")) {
			char buf[MAX_INPUT_LENGTH];
			//bzero(buf, sizeof(buf));
			sprintf(buf, "You must be level %d to cast cow form.\r\n", IMO_IMM);
			send_to_char(buf, ch);
			break;
		}
		else {
			spell_transform_cow(level, ch, victim, 0);
			break;
		}
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_transform_cow(level, ch, victim, 0);
		else if (!tar_obj)
			spell_transform_cow(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_transform_cow(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in transform cow!");
		spec_log("ERROR: Invalid spell type in transform cow!", ERROR_LOG);
		break;
	}
}				/* END OF cast_transform_cow() */


void cast_purge_magic(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_SCROLL:
		if (!tar_obj)
			return;
	case SPELL_TYPE_POTION:
	case SPELL_TYPE_SPELL:
	case SPELL_TYPE_WAND:
	default:
		spell_purge_magic(level, ch, victim, tar_obj);
	}
}				/* END OF cast_purge_magic() */
