/* s2 */
/* gv_location: 18001-18500 */
/* *******************************************************************
*  file: spells2.c , Implementation of magic.        Part of DIKUMUD *
*  Usage : All the non-offensive magic handling routines.            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "parser.h"
#include "spells.h"
#include "handler.h"
#include "constants.h"
#include "weather.h"
#include "ansi.h"
#include "globals.h"
#include "func.h"

void cast_armor(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_ARMOR)) {
			send_to_char("Nothing seems to happen.\n\r", ch);
			return;
		}
		if (ch != tar_ch)
			act("$N is protected by your deity.", FALSE, ch, 0, tar_ch, TO_CHAR);

		spell_armor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_ARMOR))
			return;
		spell_armor(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		if (ha1375_affected_by_spell(tar_ch, SPELL_ARMOR))
			return;
		spell_armor(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		if (ha1375_affected_by_spell(tar_ch, SPELL_ARMOR))
			return;
		spell_armor(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in armor!");
		spec_log("ERROR: Invalid spell type in armor!", ERROR_LOG);
		break;
	}
}

void cast_elsewhere(signed char level, struct char_data * ch, char *arg, int type, struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SCROLL:
		case SPELL_TYPE_POTION:
		case SPELL_TYPE_SPELL:
		if (!tar_ch)
			tar_ch = ch;
		spell_elsewhere(level, ch, tar_ch, 0);
		break;

	case SPELL_TYPE_WAND:
		if (!tar_ch)
			return;
		spell_elsewhere(level, ch, tar_ch, 0);
		break;

	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_elsewhere(level, ch, tar_ch, 0);
		break;

	default:
		main_log("ERROR: Invalid spell type in spell elsewhere!");
		spec_log("ERROR: Invalid spell type in elsewhere!", ERROR_LOG);
		break;
	}
}				/* END OF cast_elsewhere() */


void cast_bless(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (tar_obj) {	/* It's an object */
			if (IS_SET(tar_obj->obj_flags.flags1, OBJ1_BLESS)) {
				send_to_char("Nothing seems to happen.\n\r", ch);
				return;
			}
			spell_bless(level, ch, 0, tar_obj);

		}
		else {		/* Then it is a PC | NPC */

			if (ha1375_affected_by_spell(tar_ch, SPELL_BLESS) ||
			    (GET_POS(tar_ch) == POSITION_FIGHTING)) {
				send_to_char("Nothing seems to happen.\n\r", ch);
				return;
			}
			spell_bless(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_BLESS) ||
		    (GET_POS(ch) == POSITION_FIGHTING))
			return;
		spell_bless(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj) {	/* It's an object */
			if (IS_SET(tar_obj->obj_flags.flags1, OBJ1_BLESS))
				return;
			spell_bless(level, ch, 0, tar_obj);

		}
		else {		/* Then it is a PC | NPC */

			if (!tar_ch)
				tar_ch = ch;

			if (ha1375_affected_by_spell(tar_ch, SPELL_BLESS) ||
			    (GET_POS(tar_ch) == POSITION_FIGHTING))
				return;
			spell_bless(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj) {	/* It's an object */
			if (IS_SET(tar_obj->obj_flags.flags1, OBJ1_BLESS))
				return;
			spell_bless(level, ch, 0, tar_obj);

		}
		else {		/* Then it is a PC | NPC */

			if (ha1375_affected_by_spell(tar_ch, SPELL_BLESS) ||
			    (GET_POS(tar_ch) == POSITION_FIGHTING))
				return;
			spell_bless(level, ch, tar_ch, 0);
		}
		break;
	default:
		main_log("ERROR: Invalid spell type in bless!");
		spec_log("ERROR: Invalid spell type in bless!", ERROR_LOG);
		break;
	}
}



void cast_blindness(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (IS_AFFECTED(tar_ch, AFF_BLIND)) {
			send_to_char("Nothing seems to happen.\n\r", ch);
			return;
		}
		spell_blindness(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (IS_AFFECTED(ch, AFF_BLIND))
			return;
		spell_blindness(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		if (IS_AFFECTED(ch, AFF_BLIND))
			return;
		spell_blindness(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		if (IS_AFFECTED(ch, AFF_BLIND))
			return;
		spell_blindness(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, AFF_BLIND)))
					spell_blindness(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in blindness!");
		spec_log("ERROR: Invalid spell type in blindness!", ERROR_LOG);
		break;
	}
}

void cast_control_weather(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{
	char buffer[MAX_STRING_LENGTH];
	extern struct weather_data weather_info;



	switch (type) {
	case SPELL_TYPE_SPELL:

		one_argument(arg, buffer);

		if (str_cmp("better", buffer) && str_cmp("worse", buffer)) {
			send_to_char("Do you want it to get better or worse?\n\r", ch);
			return;
		}

		if (!str_cmp("better", buffer)) {
			weather_info.change += (dice(((level) / 3), 4));
			send_to_char("The weather takes a turn for the better.\r\n",
				     ch);
			weather_change();
		}
		else {
			weather_info.change -= (dice(((level) / 3), 4));
			send_to_char("The weather takes a turn for the worse.\r\n",
				     ch);
			weather_change();
		}
		break;
	default:
		main_log("ERROR: Invalid spell type in control weather!");
		spec_log("ERROR: Invalid spell type in control weather!", ERROR_LOG);
		break;
	}
}



void cast_create_food(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		act("$n magically creates a mushroom.", FALSE, ch, 0, 0, TO_ROOM);
		spell_create_food(level, ch, 0, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (tar_ch)
			return;
		spell_create_food(level, ch, 0, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in create food!");
		spec_log("ERROR: Invalid spell type in create food!", ERROR_LOG);
		break;
	}
}



void cast_create_water(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (tar_obj->obj_flags.type_flag != ITEM_DRINKCON) {
			send_to_char("It is unable to hold water.\n\r", ch);
			return;
		}
		spell_create_water(level, ch, 0, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in create water!");
		spec_log("ERROR: Invalid spell type in create water!", ERROR_LOG);
		break;
	}
}



void cast_cure_blind(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_cure_blind(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_cure_blind(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (ha1375_affected_by_spell(tar_ch, SPELL_BLINDNESS))
				spell_cure_blind(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cure blind!");
		spec_log("ERROR: Invalid spell type in cure blind!", ERROR_LOG);
		break;
	}
}


void cast_cure_critic(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_SCROLL:
		case SPELL_TYPE_POTION:
		if (!tar_ch)
			tar_ch = ch;
		spell_cure_critic(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_cure_critic(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cure critic!");
		spec_log("ERROR: Invalid spell type in cure critic!", ERROR_LOG);
		break;

	}
}				/* END OF cast_cure_critic() */


void cast_cause_critic(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_SCROLL:
		case SPELL_TYPE_POTION:
		if (!tar_ch)
			tar_ch = ch;
		spell_cause_critic(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_cause_critic(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cause critic!");
		spec_log("ERROR: Invalid spell type in cause critic!", ERROR_LOG);
		break;

	}
}

void cast_donate_mana(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_donate_mana(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_donate_mana(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_donate_mana(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in donate mana!");
		spec_log("ERROR: Invalid spell type in donate mana!", ERROR_LOG);
		break;

	}
}


void cast_mana_link(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_mana_link(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_mana_link(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_mana_link(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in mana link!");
		spec_log("ERROR: Invalid spell type in mana link!", ERROR_LOG);
		break;

	}
}

void cast_sustenance(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sustenance(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sustenance(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sustenance(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in donate mana!");
		spec_log("ERROR: Invalid spell type in sustenance!", ERROR_LOG);
		break;

	}
}

void cast_cure_light(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_cure_light(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_cure_light(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_cure_light(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cure light!");
		spec_log("ERROR: Invalid spell type in cure light!", ERROR_LOG);
		break;
	}
}

void cast_cause_light(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_cause_light(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_cause_light(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_cause_light(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cause light!");
		spec_log("ERROR: Invalid spell type in cause light!", ERROR_LOG);
		break;
	}
}
void cast_curse(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (tar_obj)	/* It is an object */
			spell_curse(level, ch, 0, tar_obj);
		else {		/* Then it is a PC | NPC */
			spell_curse(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_POTION:
		spell_curse(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)	/* It is an object */
			spell_curse(level, ch, 0, tar_obj);
		else {		/* Then it is a PC | NPC */
			if (!tar_ch)
				tar_ch = ch;
			spell_curse(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_curse(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in curse!");
		spec_log("ERROR: Invalid spell type in curse!", ERROR_LOG);
		break;
	}
}


void cast_detect_evil(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_DETECT_EVIL)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_detect_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_DETECT_EVIL))
			return;
		spell_detect_evil(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_DETECT_EVIL)))
					spell_detect_evil(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect evil!");
		spec_log("ERROR: Invalid spell type in detect evil!", ERROR_LOG);
		break;
	}
}


void cast_detect_good(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_DETECT_GOOD)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_detect_good(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_DETECT_GOOD))
			return;
		spell_detect_good(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_DETECT_GOOD)))
					spell_detect_good(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect good!");
		spec_log("ERROR: Invalid spell type in detect good!", ERROR_LOG);
		break;
	}
}				/* end of cast_detect_good */



void cast_detect_invisibility(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_DETECT_INVISIBLE)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_detect_invisibility(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_DETECT_INVISIBLE))
			return;
		spell_detect_invisibility(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_DETECT_INVISIBLE)))
					spell_detect_invisibility(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect invisibility!");
		spec_log("ERROR: Invalid spell type in detect invisibility!", ERROR_LOG);
		break;
	}
}



void cast_detect_magic(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_DETECT_MAGIC)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_detect_magic(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_DETECT_MAGIC))
			return;
		spell_detect_magic(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_DETECT_MAGIC)))
					spell_detect_magic(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect magic!");
		spec_log("ERROR: Invalid spell type in detect magic!", ERROR_LOG);
		break;
	}
}

void cast_breathwater(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_BREATHWATER)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_breathwater(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_BREATHWATER))
			return;
		spell_breathwater(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_BREATHWATER)))
					spell_breathwater(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in breathwater!");
		spec_log("ERROR: Invalid spell type in breathwater!", ERROR_LOG);
		break;
	}
}

void cast_darksight(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_DARKSIGHT)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_darksight(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_DARKSIGHT))
			return;
		spell_darksight(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_DARKSIGHT)))
					spell_darksight(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in darksight!");
		spec_log("ERROR: Invalid spell type in darksight!", ERROR_LOG);
		break;
	}
}


void cast_regeneration(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_REGENERATION)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_regeneration(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_REGENERATION))
			return;
		spell_regeneration(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_REGENERATION)))
					spell_regeneration(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in regeneration!");
		spec_log("ERROR: Invalid spell type in regeneration!", ERROR_LOG);
		break;
	}
}


void cast_dispel_magic(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dispel_magic(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			tar_ch = ch;
		spell_dispel_magic(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_dispel_magic(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dispel_magic!");
		spec_log("ERROR: Invalid spell type in dispel magic!", ERROR_LOG);
		break;
	}
}				/* END OF cast_dispel_magic() */

void cast_magic_resist(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_MAGIC_RESIST)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_magic_resist(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_MAGIC_RESIST))
			return;
		spell_magic_resist(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_MAGIC_RESIST)))
					spell_magic_resist(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in magic resist!");
		spec_log("ERROR: Invalid spell type in magic resist!", ERROR_LOG);
		break;
	}
}

void cast_magic_immune(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_MAGIC_IMMUNE)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_magic_immune(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_MAGIC_IMMUNE))
			return;
		spell_magic_immune(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_MAGIC_IMMUNE)))
					spell_magic_immune(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in magic immune!");
		spec_log("ERROR: Invalid spell type in magic immune!", ERROR_LOG);
		break;
	}
}


void cast_restoration(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (ha1375_affected_by_spell(tar_ch, SPELL_RESTORATION)) {
			send_to_char("Nothing seems to happen.\n\r", tar_ch);
			return;
		}
		spell_restoration(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ha1375_affected_by_spell(ch, SPELL_RESTORATION))
			return;
		spell_restoration(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, SPELL_RESTORATION)))
					spell_restoration(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in restoration!");
		spec_log("ERROR: Invalid spell type in restoration!", ERROR_LOG);
		break;
	}
}


void cast_detect_poison(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_detect_poison(level, ch, tar_ch, tar_obj);
		break;
	case SPELL_TYPE_POTION:
		spell_detect_poison(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj) {
			spell_detect_poison(level, ch, 0, tar_obj);
			return;
		}
		if (!tar_ch)
			tar_ch = ch;
		spell_detect_poison(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect poison!");
		spec_log("ERROR: Invalid spell type in detect poison!", ERROR_LOG);
		break;
	}
}



void cast_dispel_evil(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dispel_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_dispel_evil(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_dispel_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		spell_dispel_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_dispel_evil(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dispel evil!");
		spec_log("ERROR: Invalid spell type in dispel evil!", ERROR_LOG);
		break;
	}
}


void cast_enchant_weapon(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_enchant_weapon(level, ch, 0, tar_obj);
		break;

	case SPELL_TYPE_SCROLL:
		if (!tar_obj)
			return;
		spell_enchant_weapon(level, ch, 0, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in enchant weapon!");
		spec_log("ERROR: Invalid spell type in enchant weapon!", ERROR_LOG);
		break;
	}
}


void cast_heal_minor(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		act("$n heals $N.", FALSE, ch, 0, tar_ch, TO_NOTVICT);
		if (ch != tar_ch)
			act("You heal $N.", FALSE, ch, 0, tar_ch, TO_CHAR);
		spell_heal_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_heal_minor(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			spell_heal_minor(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in heal!");
		spec_log("ERROR: Invalid spell type in heal minor!", ERROR_LOG);
		break;
	}
}				/* END OF cast_heal_minor() */

void cast_heal_medium(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		act("$n heals $N.", FALSE, ch, 0, tar_ch, TO_NOTVICT);
		act("You heal $N.", FALSE, ch, 0, tar_ch, TO_CHAR);
		spell_heal_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_heal_medium(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		spell_heal_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_heal_medium(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in heal!");
		spec_log("ERROR: Invalid spell type in heal medium!", ERROR_LOG);
		break;
	}
}				/* END OF cast_heal_medium() */

void cast_heal_major(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		act("$n heals $N.", FALSE, ch, 0, tar_ch, TO_NOTVICT);
		act("You heal $N.", FALSE, ch, 0, tar_ch, TO_CHAR);
		spell_heal_major(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_heal_major(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_heal_major(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in heal!");
		spec_log("ERROR: Invalid spell type in heal major!", ERROR_LOG);
		break;
	}
}				/* END OF cast_heal_major() */

void cast_convalesce(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{

	//

	switch (type) {
		case SPELL_TYPE_SPELL:
		act("$n heals $N.", FALSE, ch, 0, tar_ch, TO_NOTVICT);
		act("You heal $N.", FALSE, ch, 0, tar_ch, TO_CHAR);
		spell_convalesce(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_convalesce(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_convalesce(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in convalesce!");
		spec_log("ERROR: Invalid spell type in convalesce!", ERROR_LOG);
		break;
	}
}				/* END OF cast_heal_major() */



void cast_invisibility(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (tar_obj) {
			if (IS_SET(tar_obj->obj_flags.flags1, OBJ1_INVISIBLE))
				send_to_char("Nothing new seems to happen.\n\r", ch);
			else
				spell_invisibility(level, ch, 0, tar_obj);
		}
		else {		/* tar_ch */
			if (IS_AFFECTED(tar_ch, AFF_INVISIBLE))
				send_to_char("Nothing new seems to happen.\n\r", ch);
			else
				spell_invisibility(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_POTION:
		if (!IS_AFFECTED(ch, AFF_INVISIBLE))
			spell_invisibility(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj) {
			if (!(IS_SET(tar_obj->obj_flags.flags1, OBJ1_INVISIBLE)))
				spell_invisibility(level, ch, 0, tar_obj);
		}
		else {		/* tar_ch */
			if (!tar_ch)
				tar_ch = ch;

			if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
				spell_invisibility(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj) {
			if (!(IS_SET(tar_obj->obj_flags.flags1, OBJ1_INVISIBLE)))
				spell_invisibility(level, ch, 0, tar_obj);
		}
		else {		/* tar_ch */
			if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
				spell_invisibility(level, ch, tar_ch, 0);
		}
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
					spell_invisibility(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in invisibility!");
		spec_log("ERROR: Invalid spell type in invisibility!", ERROR_LOG);
		break;
	}
}


void cast_improved_invis(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		if (IS_AFFECTED(tar_ch, AFF_INVISIBLE))
			send_to_char("Nothing new seems to happen.\n\r", ch);
		else
			spell_improved_invis(level, ch, tar_ch, 0);
		break;

	case SPELL_TYPE_POTION:
		if (!IS_AFFECTED(ch, AFF_INVISIBLE))
			spell_improved_invis(level, ch, ch, 0);
		break;

	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			tar_ch = ch;
		if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
			spell_improved_invis(level, ch, tar_ch, 0);
		break;

	case SPELL_TYPE_WAND:
		if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
			spell_improved_invis(level, ch, tar_ch, 0);
		break;

	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				if (!(IS_AFFECTED(tar_ch, AFF_INVISIBLE)))
					spell_improved_invis(level, ch, tar_ch, 0);
		break;

	default:
		main_log("ERROR: Invalid spell type in improved invisibility!");
		spec_log("ERROR: Invalid spell type in improved invisibility!", ERROR_LOG);
		break;
	}
}


void cast_locate(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_locate(level, ch, 0, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in locate object!");
		spec_log("ERROR: Invalid spell type in locate object!", ERROR_LOG);
		break;
	}
}				/* END OF cast_locate() */


void cast_poison(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_poison(level, ch, tar_ch, tar_obj);
		break;
	case SPELL_TYPE_POTION:
		spell_poison(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_poison(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in poison!");
		spec_log("ERROR: Invalid spell type in poison!", ERROR_LOG);
		break;
	}
}


void cast_protection_from_evil(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_protection_from_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_protection_from_evil(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_protection_from_evil(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_protection_from_evil(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in protection from evil!");
		spec_log("ERROR: Invalid spell type in protection from evil!", ERROR_LOG);
		break;
	}
}


void cast_remove_curse(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_remove_curse(level, ch, tar_ch, tar_obj);
		break;
	case SPELL_TYPE_POTION:
		spell_remove_curse(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj) {
			spell_remove_curse(level, ch, 0, tar_obj);
			return;
		}
		if (!tar_ch)
			tar_ch = ch;
		spell_remove_curse(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_remove_curse(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in remove curse!");
		spec_log("ERROR: Invalid spell type in remove curse!", ERROR_LOG);
		break;
	}
}



void cast_remove_poison(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_remove_poison(level, ch, tar_ch, tar_obj);
		break;
	case SPELL_TYPE_POTION:
		spell_remove_poison(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_remove_poison(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in remove poison!");
		spec_log("ERROR: Invalid spell type in remove poison!", ERROR_LOG);
		break;
	}
}



void cast_sanctuary(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sanctuary(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sanctuary(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_sanctuary(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sanctuary(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sanctuary!");
		spec_log("ERROR: Invalid spell type in sanctuary!", ERROR_LOG);
		break;
	}
}

void cast_sanctuary_minor(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sanctuary_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sanctuary_minor(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_sanctuary_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sanctuary_minor(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sanctuary!");
		spec_log("ERROR: Invalid spell type in sanctuary minor!", ERROR_LOG);
		break;
	}
}

void cast_sanctuary_medium(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sanctuary_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sanctuary_medium(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_sanctuary_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sanctuary_medium(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sanctuary!");
		spec_log("ERROR: Invalid spell type in sanctuary medium!", ERROR_LOG);
		break;
	}
}

void cast_sleep(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sleep(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sleep(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_sleep(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		spell_sleep(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sleep(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sleep!");
		spec_log("ERROR: Invalid spell type in sleep!", ERROR_LOG);
		break;
	}
}


void cast_strength(signed char level, struct char_data * ch, char *arg, int type,
		     struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_strength(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_strength(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_strength(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_strength(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in strength!");
		spec_log("ERROR: Invalid spell type in strength!", ERROR_LOG);
		break;
	}
}


void cast_ventriloquate(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{

	struct char_data *tmp_ch;
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH], buf4[MAX_STRING_LENGTH];



	/* IS CHAR MUZZLED? */
	MUZZLECHECK();

	if (type != SPELL_TYPE_SPELL) {
		main_log("Attempt to ventriloquate by non-cast-spell.");
		spec_log("Attempt to ventriloquate by non-cast-spell.", ERROR_LOG);
		return;
	}
	for (; *arg && (*arg == ' '); arg++);
	if (tar_obj) {
		sprintf(buf1, "The %s says '%s'\n\r", ha1100_fname(tar_obj->name), arg);
		sprintf(buf2, "Someone makes it sound like the %s says '%s'.\n\r",
			ha1100_fname(tar_obj->name), arg);
		sprintf(buf3, "Something says, '%s'\n\r", arg);
		sprintf(buf4, "%s uses ventricate to make it appear the %s said %s.\r\n",
			GET_REAL_NAME(ch), ha1100_fname(tar_obj->name), arg);
	}
	else {
		sprintf(buf1, "%s says '%s'\n\r", GET_REAL_NAME(tar_ch), arg);
		sprintf(buf2, "Someone makes it sound like %s says '%s'\n\r",
			GET_NAME(tar_ch), arg);
		sprintf(buf3, "Someone says, '%s'\n\r", arg);
		sprintf(buf4, "%s uses ventriloquate to make it appear %s said %s.\r\n",
			GET_REAL_NAME(ch), GET_REAL_NAME(tar_ch), arg);
	}

	/* THIS COULD BE ABUSED, SO LOG IT */
	main_log(buf4);
	spec_log(buf4, MAIN_LOG);

	for (tmp_ch = world[ch->in_room].people; tmp_ch;
	     tmp_ch = tmp_ch->next_in_room) {
		if (GET_LEVEL(tmp_ch) > PK_LEV) {
			/* send_to_char(buf4, tmp_ch); */
			ansi_act(buf4, FALSE, tmp_ch, 0, 0, TO_CHAR, CLR_SAY, 0);
		}
		else {
			if ((tmp_ch != ch) && (tmp_ch != tar_ch)) {
				if (saves_spell(ch, tmp_ch, SAVING_SPELL))
					/* send_to_char(buf2, tmp_ch); */
					ansi_act(buf2, FALSE, tmp_ch, 0, 0, TO_CHAR, CLR_SAY, 0);
				else
					/* send_to_char(buf1, tmp_ch); */
					ansi_act(buf1, FALSE, tmp_ch, 0, 0, TO_CHAR, CLR_SAY, 0);
			}
			else {
				if (tmp_ch == tar_ch) {
					/* send_to_char(buf3, tar_ch); */
					ansi_act(buf3, FALSE, tmp_ch, 0, 0, TO_CHAR, CLR_SAY, 0);
				}
			}
		}		/* END OF else less than or equal to IMO_LEV */
	}			/* END OF for loop */
}				/* END OF cast ventriloquate() */



void cast_word_of_recall(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_word_of_recall(level, ch, ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_word_of_recall(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_word_of_recall(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (tar_obj)
			return;
		spell_word_of_recall(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_word_of_recall(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in word of recall!");
		spec_log("ERROR: Invalid spell type in word of recall!", ERROR_LOG);
		break;
	}
}



void cast_summon(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_summon(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in summon!");
		spec_log("ERROR: Invalid spell type in summon!", ERROR_LOG);
		break;
	}
}


void cast_succor(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_succor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (tar_obj)
			return;
		if (!tar_ch)
			tar_ch = ch;
		spell_succor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		if (ch->master)
			spell_succor(level, ch, ch->master, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if ((tar_ch != ch) && (tar_ch->master))
				spell_word_of_recall(level, ch, tar_ch->master, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in succor!");
		spec_log("ERROR: Invalid spell type in succor!", ERROR_LOG);
		break;
	}
}


void cast_charm_person(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_charm_person(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_charm_person(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_charm_person(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in charm person!");
		spec_log("ERROR: Invalid spell type in char person!", ERROR_LOG);
		break;
	}
}



void cast_sense_life(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_sense_life(level, ch, ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_sense_life(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_sense_life(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in sense life!");
		spec_log("ERROR: Invalid spell type in sense life!", ERROR_LOG);
		break;
	}
}


void cast_identify(signed char level, struct char_data * ch, char *arg, int type,
		     struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_identify(level, ch, tar_ch, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		spell_identify(level, ch, tar_ch, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in identify!");
		spec_log("ERROR: Invalid spell type in identify!", ERROR_LOG);
		break;
	}
}


void cast_fire_breath(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_WAND:
		if (ch->in_room == tar_ch->in_room)
			spell_fire_breath(level, ch, tar_ch, 0);
		break;		/* It's a spell.. But people can'c cast it! */
	default:
		main_log("ERROR: Invalid spell type in firebreath!");
		spec_log("ERROR: Invalid spell type in firebreath!", ERROR_LOG);
		break;
	}
}


void cast_frost_breath(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_WAND:
		if (ch->in_room == tar_ch->in_room)
			spell_frost_breath(level, ch, tar_ch, 0);
		break;		/* It's a spell.. But people can'c cast it! */
	default:
		main_log("ERROR: Invalid spell type in frostbreath!");
		spec_log("ERROR: Invalid spell type in frostbreath!", ERROR_LOG);
		break;
	}
}

void cast_acid_breath(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_WAND:
		if (ch->in_room == tar_ch->in_room)
			spell_acid_breath(level, ch, tar_ch, 0);
		break;		/* It's a spell.. But people can'c cast it! */
	default:
		main_log("ERROR: Invalid spell type in acidbreath!");
		spec_log("ERROR: Invalid spell type in acidbreath!", ERROR_LOG);
		break;
	}
}

void cast_gas_breath(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{

	struct char_data *leader, *next_guy;



	if (ch->master)
		leader = ch->master;
	else
		leader = ch;
	switch (type) {
	case SPELL_TYPE_SPELL:
	case SPELL_TYPE_WAND:
		if (tar_ch == 0) {
			for (tar_ch = world[ch->in_room].people; tar_ch; tar_ch = next_guy) {
				next_guy = tar_ch->next_in_room;
				if ((tar_ch != ch) && (leader != tar_ch->master))
					spell_gas_breath(level, ch, tar_ch, 0);
			}
		}
		else {
			if (ch->in_room == tar_ch->in_room)
				spell_gas_breath(level, ch, tar_ch, 0);
		}
		break;
		/* THIS ONE HURTS!! */
	default:
		main_log("ERROR: Invalid spell type in gasbreath!");
		spec_log("ERROR: Invalid spell type in gasbreath!", ERROR_LOG);
		break;
	}
}

void cast_lightning_breath(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_WAND:
		if (ch->in_room == tar_ch->in_room)
			spell_lightning_breath(level, ch, tar_ch, 0);
		break;		/* It's a spell.. But people can'c cast it! */
	default:
		main_log("ERROR: Invalid spell type in lightningbreath!");
		spec_log("ERROR: Invalid spell type in lightningbreath!", ERROR_LOG);
		break;
	}
}

void cast_hold_person(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_hold_person(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_hold_person(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_hold_person(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in hold person!");
		spec_log("ERROR: Invalid spell type in hold person!", ERROR_LOG);
		break;
	}
}				/* END OF cast_() */

void cast_knock(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_knock(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_knock(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_knock(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in knock!");
		spec_log("ERROR: Invalid spell type in knock!", ERROR_LOG);
		break;
	}
}				/* END OF cast_() */

void do_phase_door(signed char level, struct char_data * ch, char *arg, int type)
{
	int lv_dir, idx, lv_to_room, lv_die;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	for (; *arg == ' '; arg++);
	if (!*arg) {
		/* LOOK FOR A DIRECTION */
		for (idx = 0; idx < MAX_DIRS; idx++) {
			if (EXIT(ch, idx)) {
				if (IS_SET(EXIT(ch, idx)->exit_info, EX_ISDOOR)) {
					sprintf(buf, "door %s", dirs[idx]);
					do_phase_door(level, ch, buf, type);
					return;
				}
			}
		}		/* END OF for loop */
		send_to_char("phase through what?\r\n", ch);
		return;
	}

	arg = one_argument(arg, buf);
	for (; *arg == ' '; arg++);
	bzero(buf2, sizeof(buf2));

	/* IF WE DIDN'T GET AN ARGUMENT, SEE IF ITS A DIRECTION */
	if (!*arg) {
		lv_dir = search_block(arg, dirs, FALSE);
		if (lv_dir >= 0 && lv_dir < MAX_DIRS) {
			bzero(buf, sizeof(buf));
			strcpy(buf2, arg);
		}
	}
	/* IF WE GOT AN ARGUMENT, WE PROBABLY HAVE A DIRECTION */
	if (*arg) {
		bzero(buf, sizeof(buf));
		strcpy(buf2, arg);
	}

	lv_dir = mo1600_find_door(ch, buf, buf2);

	if (lv_dir < 0 || lv_dir > MAX_DIRS - 1) {
		send_to_char("You might think that is a door, but I don't\r\n", ch);
		return;
	}

	if (!IS_SET(EXIT(ch, lv_dir)->exit_info, EX_ISDOOR)) {
		send_to_char("That's impossible, I'm afraid.\n\r", ch);
		return;
	}

	if (EXIT(ch, lv_dir)) {
		lv_to_room = EXIT(ch, lv_dir)->to_room;
		if (lv_to_room > 0) {
			if (li3000_is_blocked_from_room(ch, lv_to_room, BIT0)) {
				send_to_char("A strange force slams you back into the room!\r\n", ch);
				return;
			}
		}
		if (ha9800_protect_char_from_dt(ch, lv_to_room, 0)) {
			return;
		}
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_IN) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Wisp of engery coalesce, but nothing happens!\r\n", ch);
		return;
	}
	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT) && GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("A massive kernal of energy forms, but nothing happens!\r\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "$n coalesces into a blazing globe of light and flashes %s", dirs[lv_dir]);
	act(buf, FALSE, ch, 0, ch, TO_ROOM);
	lv_die = mo1000_do_simple_move(ch, lv_dir, FALSE, BIT1);
	if (lv_die) {
		sprintf(buf, "Light flashes in from the %s and coalesces into $n.", dirs[reverse_dir[lv_dir]]);
		act(buf, FALSE, ch, 0, ch, TO_ROOM);
	}

	return;

}				/* END OF do_phase_door() */

void cast_phase_door(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:{
			do_phase_door(level, ch, arg, type);
			break;
		}
	case SPELL_TYPE_SCROLL:{
			if (!tar_ch)
				return;
			do_phase_door(level, ch, arg, type);
			break;
		}
	case SPELL_TYPE_STAFF:{
			for (tar_ch = world[ch->in_room].people;
			     tar_ch; tar_ch = tar_ch->next_in_room)
				if (tar_ch != ch)
					do_phase_door(level, ch, arg, type);
			break;
		}
	default:{
			main_log("ERROR: Invalid spell type in phase_door!");
			spec_log("ERROR: Invalid spell type in phase door!", ERROR_LOG);
			break;
		}
	}
}				/* END OF cast_phase_door() */

void cast_waterwalk(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_waterwalk(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_waterwalk(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_waterwalk(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in waterwalk!");
		spec_log("ERROR: Invalid spell type in waterwalk!", ERROR_LOG);
		break;
	}
}				/* END OF cast_waterwalk() */

void cast_(signed char level, struct char_data * ch, char *arg, int type,
	     struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_waterwalk(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_waterwalk(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_waterwalk(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in waterwalk!");
		spec_log("ERROR: Invalid spell type in waterwalk!", ERROR_LOG);
		break;
	}
}				/* END OF cast_waterwalk() */

void cast_teleport_self(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_teleport_self(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_teleport_self(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_teleport_self(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in teleport self!");
		spec_log("ERROR: Invalid spell type in teleport self!", ERROR_LOG);
		break;
	}
}				/* END OF cast_teleport_self() */

void cast_teleport_group(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_teleport_group(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_teleport_group(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_teleport_group(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in teleport group!");
		spec_log("ERROR: Invalid spell type in teleport group!", ERROR_LOG);
		break;
	}
}				/* END OF cast_teleport_group() */

void cast_teleview_minor(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_teleview_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_teleview_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_teleview_minor(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in teleview minor!");
		spec_log("ERROR: Invalid spell type in teleview minor!", ERROR_LOG);
		break;
	}
}				/* END OF cast_teleview_minor() */

void cast_teleview_major(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_teleview_major(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_teleview_major(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_teleview_major(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in teleview_major!");
		spec_log("ERROR: Invalid spell type in teleview major!", ERROR_LOG);
		break;
	}
}				/* END OF cast_teleview_major() */

void cast_haste(signed char level, struct char_data * ch, char *arg, int type,
		  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_haste(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_haste(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_haste(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_haste(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in haste!");
		spec_log("ERROR: Invalid spell type in haste!", ERROR_LOG);
		break;
	}
}				/* END OF cast_haste() */

void cast_fly(signed char level, struct char_data * ch, char *arg, int type,
	        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_fly(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_fly(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_fly(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_fly(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in fly!");
		spec_log("ERROR: Invalid spell type in fly!", ERROR_LOG);
		break;
	}
}				/* END OF cast_fly() */

void cast_dreamsight(signed char level, struct char_data * ch, char *arg, int type,
		       struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_POTION:
		case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			tar_ch = ch;
		spell_dreamsight(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_dreamsight(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dream sight!");
		spec_log("ERROR: Invalid spell type in dream sight!", ERROR_LOG);
		break;
	}
}				/* END OF cast_dreamsight() */

void cast_gate(signed char level, struct char_data * ch, char *arg, int type,
	         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_gate(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_gate(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_gate(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in gate!");
		spec_log("ERROR: Invalid spell type in gate!", ERROR_LOG);
		break;
	}
}				/* END OF cast_gate() */


void cast_vitalize_hit(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_vitalize_hit(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_vitalize_hit(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_vitalize_hit(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vitalize_hit!");
		spec_log("ERROR: Invalid spell type in vitalize hit!", ERROR_LOG);
		break;
	}
}				/* END OF cast_vitalize_hit() */

void cast_vitalize_mana(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_vitalize_mana(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_vitalize_mana(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_vitalize_mana(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vitalze mana!");
		spec_log("ERROR: Invalid spell type in vitalize mana!", ERROR_LOG);
		break;
	}
}				/* END OF cast_vitlize_mana() */

void cast_vigorize_minor(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_vigorize_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_vigorize_minor(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_vigorize_minor(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vigorize minor!");
		spec_log("ERROR: Invalid spell type in vigorize minor!", ERROR_LOG);
		break;
	}
}				/* END OF cast_vigorize_minor() */

void cast_vigorize_medium(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_vigorize_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_vigorize_medium(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_vigorize_medium(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vigorize medium!");
		spec_log("ERROR: Invalid spell type in vigorize medium!", ERROR_LOG);
		break;
	}
}				/* END OF cast_vigorize_medium() */

void cast_vigorize_major(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_vigorize_major(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_vigorize_major(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_vigorize_major(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in vigorize major!");
		spec_log("ERROR: Invalid spell type in vigorize major!", ERROR_LOG);
		break;
	}
}				/* END OF cast_vigorize_major() */

void cast_aid(signed char level, struct char_data * ch, char *arg, int type,
	        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_aid(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_aid(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_aid(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_aid(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in aid!");
		spec_log("ERROR: Invalid spell type in aid!", ERROR_LOG);
		break;
	}
}				/* END OF cast_aid() */

void cast_dispel_silence(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dispel_silence(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_dispel_silence(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_dispel_silence(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dispel_silence!");
		spec_log("ERROR: Invalid spell type in dispel silence!", ERROR_LOG);
		break;
	}
}				/* END OF cast_dispel_silence() */

void cast_dispel_hold(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dispel_hold(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_dispel_hold(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_dispel_hold(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dispel hold!");
		spec_log("ERROR: Invalid spell type in dispel hold!", ERROR_LOG);
		break;
	}
}				/* END OF cast_dispel_hold() */

void cast_block_summon(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_block_summon(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_block_summon(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_block_summon(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in block summon!");
		spec_log("ERROR: Invalid spell type in block summon!", ERROR_LOG);
		break;
	}
}				/* END OF cast_block_summon() */


void cast_animate_dead(signed char level, struct char_data * ch, char *arg, int type,
		         struct char_data * tar_ch, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_animate_dead(level, ch, 0, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_animate_dead(level, ch, 0, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in gate!");
		spec_log("ERROR: Invalid spell type in animate death!", ERROR_LOG);
		break;
	}
}				/* END OF cast_animate_dead() */
