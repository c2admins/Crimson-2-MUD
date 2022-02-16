/* sn */
/* gv_location: 17501-18000 */
/* *******************************************************************
*  file: spells1.c , handling of magic.              Part of DIKUMUD *
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

void cast_burning_hands(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_burning_hands(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		spell_burning_hands(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in burning hands!");
		spec_log("ERROR: Invalid spell type in burning hands!", ERROR_LOG);
		break;
	}
}


void cast_hail_storm(sbyte level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{
	extern struct weather_data weather_info;



	switch (type) {
	case SPELL_TYPE_SPELL:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
			spell_hail_storm(level, ch, victim, 0);
		}
		else {
			send_to_char("You fail to call upon a hail storm!\n\r", ch);
		}
		break;
	case SPELL_TYPE_POTION:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
			spell_hail_storm(level, ch, ch, 0);
		}
		break;
	case SPELL_TYPE_SCROLL:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
			if (victim)
				spell_hail_storm(level, ch, victim, 0);
			else if (!tar_obj)
				spell_hail_storm(level, ch, ch, 0);
		}
		break;
	case SPELL_TYPE_STAFF:
		if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
			for (victim = world[ch->in_room].people;
			     victim; victim = victim->next_in_room)
				if (victim != ch)
					spell_hail_storm(level, ch, victim, 0);
		}
		break;
	default:
		main_log("ERROR: Invalid spell type in hail_storm!");
		spec_log("ERROR: Invalid spell type in hail storm!", ERROR_LOG);
		break;
	}
}


void cast_chill_touch(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_chill_touch(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		spell_chill_touch(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in chill touch!");
		spec_log("ERROR: Invalid spell type in chill touch!", ERROR_LOG);

		break;
	}
}


void cast_shocking_grasp(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_shocking_grasp(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		spell_shocking_grasp(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in shocking grasp!");
		spec_log("ERROR: Invalid spell type in shocking grasp!", ERROR_LOG);
		break;
	}
}


void cast_colour_spray(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_colour_spray(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_colour_spray(level, ch, victim, 0);
		else if (!tar_obj)
			spell_colour_spray(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_colour_spray(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in colour spray!");
		spec_log("ERROR: Invalid spell type in colour spray!", ERROR_LOG);
		break;
	}
}


void cast_tremor(sbyte level, struct char_data * ch, char *arg, int type,
		   struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		case SPELL_TYPE_SCROLL:
		case SPELL_TYPE_STAFF:
		case SPELL_TYPE_WAND:
		spell_tremor(level, ch, 0, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in tremor!");
		spec_log("ERROR: Invalid spell type in tremor!", ERROR_LOG);
		break;
	}
}


void cast_energy_drain(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_energy_drain(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_energy_drain(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_energy_drain(level, ch, victim, 0);
		else if (!tar_obj)
			spell_energy_drain(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_energy_drain(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_energy_drain(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in energy drain!");
		spec_log("ERROR: Invalid spell type in energy drain!", ERROR_LOG);
		break;
	}
}


void cast_fireball(sbyte level, struct char_data * ch, char *arg, int type,
		     struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_fireball(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_fireball(level, ch, victim, 0);
		else if (!tar_obj)
			spell_fireball(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_fireball(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in fireball!");
		spec_log("ERROR: Invalid spell type in fireball!", ERROR_LOG);
		break;

	}
}

void cast_turn_undead(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_turn_undead(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_turn_undead(level, ch, victim, 0);
		else if (!tar_obj)
			spell_turn_undead(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_turn_undead(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in turn_undead!");
		spec_log("ERROR: Invalid spell type in turn undead!", ERROR_LOG);
		break;

	}
}


void cast_harm(sbyte level, struct char_data * ch, char *arg, int type,
	         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_harm(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_harm(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_harm(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		spell_harm(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in harm!");
		spec_log("ERROR: Invalid spell type in harm!", ERROR_LOG);
		break;

	}
}

void cast_conflagration(sbyte level, struct char_data * ch, char *arg, int type, struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_conflagration(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_conflagration(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_conflagration(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in conflagration!");
		spec_log("ERROR: Invalid spell type in conflagration!", ERROR_LOG);
		break;
	}
}


void cast_lightning_bolt(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_lightning_bolt(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_lightning_bolt(level, ch, victim, 0);
		else if (!tar_obj)
			spell_lightning_bolt(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_lightning_bolt(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in lightning bolt!");
		spec_log("ERROR: Invalid spell type in lightning bolt!", ERROR_LOG);
		break;

	}
}


void cast_magic_missile(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_magic_missile(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_magic_missile(level, ch, victim, 0);
		else if (!tar_obj)
			spell_magic_missile(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_magic_missile(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in magic missile!");
		spec_log("ERROR: Invalid spell type in magic missile!", ERROR_LOG);
		break;

	}
}				/* END OF cast_() */

void cast_silence(sbyte level, struct char_data * ch, char *arg, int type,
		    struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_silence(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_silence(level, ch, victim, 0);
		else if (!tar_obj)
			spell_silence(level, ch, ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_silence(level, ch, victim, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_silence(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in silence!");
		spec_log("ERROR: Invalid spell type in silence!", ERROR_LOG);
		break;
	}
}				/* END OF cast_silence() */

void cast_smite(sbyte level, struct char_data * ch, char *arg, int type,
		  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_smite(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_smite(level, ch, victim, 0);
		else if (!tar_obj)
			spell_smite(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_smite(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in smite!");
		spec_log("ERROR: Invalid spell type in smite!", ERROR_LOG);
		break;
	}
}				/* END OF cast_smite() */


void cast_black_burst(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_black_burst(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_black_burst(level, ch, victim, 0);
		else if (!tar_obj)
			spell_black_burst(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_black_burst(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in black_burst!");
		spec_log("ERROR: Invalid spell type in black burst!", ERROR_LOG);

		break;

	}
}				/* END OF cast_black_burst() */



void cast_cremation(sbyte level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_cremation(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_cremation(level, ch, victim, 0);
		else if (!tar_obj)
			spell_cremation(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_cremation(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in cremation!");
		spec_log("ERROR: Invalid spell type in cremation!", ERROR_LOG);
		break;

	}
}				/* END OF cast_cremation() */


void cast_mortice_flame(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_mortice_flame(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_mortice_flame(level, ch, victim, 0);
		else if (!tar_obj)
			spell_mortice_flame(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_mortice_flame(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in mortice_flame!");
		spec_log("ERROR: Invalid spell type in mortice flame!", ERROR_LOG);
		break;

	}
}				/* END OF cast_mortice_flame() */


void cast_firelance(sbyte level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_firelance(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_firelance(level, ch, victim, 0);
		else if (!tar_obj)
			spell_firelance(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_firelance(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in firelance!");
		spec_log("ERROR: Invalid spell type in firelance!", ERROR_LOG);
		break;

	}
}				/* END OF cast_firelance() */


void cast_firestorm(sbyte level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_firestorm(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_firestorm(level, ch, victim, 0);
		else if (!tar_obj)
			spell_firestorm(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_firestorm(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in firestorm!");
		spec_log("ERROR: Invalid spell type in firestorm!", ERROR_LOG);
		break;

	}
}				/* END OF cast_firestorm() */


void cast_earthquake(sbyte level, struct char_data * ch, char *arg, int type,
		       struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_earthquake(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_earthquake(level, ch, victim, 0);
		else if (!tar_obj)
			spell_earthquake(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_earthquake(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in earthquake!");
		spec_log("ERROR: Invalid spell type in earthquake!", ERROR_LOG);
		break;

	}
}				/* END OF cast_earthquake() */


void cast_divine_retribution(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_divine_retribution(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_divine_retribution(level, ch, victim, 0);
		else if (!tar_obj)
			spell_divine_retribution(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_divine_retribution(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in divine_retribution!");
		spec_log("ERROR: Invalid spell type in divine retribution!", ERROR_LOG);
		break;

	}
}				/* END OF cast_divine_retribution() */
