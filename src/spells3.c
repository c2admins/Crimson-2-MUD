/* s3 */
/* gv_location: 18501-19000 */
/* *******************************************************************
*  file: spells3.c , handling of magic.              Part of DIKUMUD *
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


void cast_mirror_self(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_mirror_self(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_mirror_self(level, ch, victim, 0);
		else
			spell_mirror_self(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_mirror_self(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in mirror_self!");
		spec_log("ERROR: Invalid spell type in mirror_self!", ERROR_LOG);
		break;

	}
}				/* END OF cast_mirror_self() */


void cast_recharge_light(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_recharge_light(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_recharge_light(level, ch, victim, tar_obj);
		else
			spell_recharge_light(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_recharge_light(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in recharge_light!");
		spec_log("ERROR: Invalid spell type in recharge_light!", ERROR_LOG);
		break;

	}
}				/* END OF cast_recharge_light() */


void cast_recharge_wand(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_recharge_wand(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_recharge_wand(level, ch, victim, tar_obj);
		else
			spell_recharge_wand(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_recharge_wand(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in recharge_wand!");
		spec_log("ERROR: Invalid spell type in recharge_wand!", ERROR_LOG);
		break;

	}
}				/* END OF cast_recharge_wand() */


void cast_venom_blade(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_venom_blade(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_venom_blade(level, ch, victim, tar_obj);
		else
			spell_venom_blade(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_venom_blade(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in venom_blade!");
		spec_log("ERROR: Invalid spell type in venom_blade!", ERROR_LOG);
		break;

	}
}				/* END OF cast_venom_blade() */


void cast_irresistable_dance(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_irresistable_dance(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_irresistable_dance(level, ch, victim, tar_obj);
		else
			spell_irresistable_dance(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_irresistable_dance(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in irresistable_dance!");
		spec_log("ERROR: Invalid spell type in irresistable_dance!", ERROR_LOG);
		break;

	}
}				/* END OF cast_irresistable_dance() */


void cast_wizard_lock(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_wizard_lock(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_wizard_lock(level, ch, victim, tar_obj);
		else
			spell_wizard_lock(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_wizard_lock(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in wizard_lock!");
		spec_log("ERROR: Invalid spell type in wizard_lock!", ERROR_LOG);
		break;

	}
}				/* END OF cast_wizard_lock() */


void cast_dancing_sword(sbyte level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dancing_sword(level, ch, victim, tar_obj);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_dancing_sword(level, ch, victim, tar_obj);
		else
			spell_dancing_sword(level, ch, ch, tar_obj);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_dancing_sword(level, ch, victim, tar_obj);
		break;
	default:
		main_log("ERROR: Invalid spell type in dancing_sword!");
		spec_log("ERROR: Invalid spell type in dancing_sword!", ERROR_LOG);
		break;

	}
}				/* END OF cast_dancing_sword() */


void cast_demonic_aid(sbyte level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_demonic_aid(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_demonic_aid(level, ch, victim, 0);
		else
			spell_demonic_aid(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_demonic_aid(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in demonic_aid!");
		spec_log("ERROR: Invalid spell type in demonic_aid!", ERROR_LOG);
		break;

	}
}				/* END OF cast_demonic_aid() */


void cast_grangorns_curse(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_grangorns_curse(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_grangorns_curse(level, ch, victim, 0);
		else
			spell_grangorns_curse(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_grangorns_curse(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in grangorns_curse!");
		spec_log("ERROR: Invalid spell type in grangorns_curse!", ERROR_LOG);
		break;

	}
}				/* END OF cast_grangorns_curse() */


void cast_clawed_hands(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_clawed_hands(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_clawed_hands(level, ch, victim, 0);
		else
			spell_clawed_hands(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_clawed_hands(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in clawed_hands!");
		spec_log("ERROR: Invalid spell type in clawed_hands!", ERROR_LOG);
		break;

	}
}				/* END OF cast_clawed_hands() */


void cast_detect_animals(signed char level, struct char_data * ch, char *arg, int type,
			  struct char_data * victim, struct obj_data * tar_obj)
{

	switch (type) {
		case SPELL_TYPE_POTION:
		spell_detect_animals(level, ch, ch, 0);
		break;
	case SPELL_TYPE_SPELL:
		spell_detect_animals(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_detect_animals(level, ch, victim, 0);
		else if (!tar_obj)
			spell_detect_animals(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_detect_animals(level, ch, victim, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (victim = world[ch->in_room].people;
		     victim; victim = victim->next_in_room)
			if (victim != ch)
				spell_detect_animals(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in detect animals!");
		spec_log("ERROR: Invalid spell type in detect animals!", ERROR_LOG);
		break;
	}
}				/* END OF cast_detect_animals() */


void cast_intelligence(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_intelligence(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_intelligence(level, ch, victim, 0);
		else
			spell_intelligence(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_intelligence(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in intelligence!");
		spec_log("ERROR: Invalid spell type in intelligence!", ERROR_LOG);
		break;

	}
}				/* END OF cast_intelligence() */


void cast_wisdom(sbyte level, struct char_data * ch, char *arg, int type,
		   struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_wisdom(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_wisdom(level, ch, victim, 0);
		else
			spell_wisdom(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_wisdom(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in wisdom!");
		spec_log("ERROR: Invalid spell type in wisdom!", ERROR_LOG);
		break;

	}
}				/* END OF cast_wisdom() */


void cast_dexterity(sbyte level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_dexterity(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_dexterity(level, ch, victim, 0);
		else
			spell_dexterity(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_dexterity(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in dexterity!");
		spec_log("ERROR: Invalid spell type in dexterity!", ERROR_LOG);
		break;

	}
}				/* END OF cast_dexterity() */


void cast_constitution(sbyte level, struct char_data * ch, char *arg, int type,
		         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_constitution(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_constitution(level, ch, victim, 0);
		else
			spell_constitution(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_constitution(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in constitution!");
		spec_log("ERROR: Invalid spell type in constitution!", ERROR_LOG);
		break;

	}
}				/* END OF cast_constitution() */


void cast_charisma(sbyte level, struct char_data * ch, char *arg, int type,
		     struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_charisma(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_charisma(level, ch, victim, 0);
		else
			spell_charisma(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_charisma(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in charisma!");
		spec_log("ERROR: Invalid spell type in charisma!", ERROR_LOG);
		break;

	}
}				/* END OF cast_charisma() */

void cast_portal(signed char level, struct char_data * ch, char *arg, int type,
		   struct char_data * tar_ch, struct obj_data * tar_obj)
{

	//struct obj_data *lv_obj;



	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_portal(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (!tar_ch)
			return;
		spell_portal(level, ch, tar_ch, 0);
		break;
	case SPELL_TYPE_STAFF:
		for (tar_ch = world[ch->in_room].people;
		     tar_ch; tar_ch = tar_ch->next_in_room)
			if (tar_ch != ch)
				spell_portal(level, ch, tar_ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in portal!");
		spec_log("ERROR: Invalid spell type in portal!", ERROR_LOG);
		break;
	}
}				/* END OF cast_portal() */


void cast_hemmorage(signed char level, struct char_data * ch, char *arg, int type,
		      struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_hemmorage(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_hemmorage(level, ch, victim, 0);
		else
			spell_hemmorage(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_hemmorage(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in hemmorage!");
		spec_log("ERROR: Invalid spell type in hemmorage!", ERROR_LOG);
		break;

	}
}				/* END OF cast_hemmorage() */


void cast_fear(signed char level, struct char_data * ch, char *arg, int type,
	         struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_fear(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_fear(level, ch, victim, 0);
		else
			spell_fear(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_fear(level, ch, victim, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in fear!");
		spec_log("ERROR: Invalid spell type in fear!", ERROR_LOG);
		break;

	}
}				/* END OF cast_fear() */


void cast_demon_flesh(signed char level, struct char_data * ch, char *arg, int type,
		        struct char_data * victim, struct obj_data * tar_obj)
{



	switch (type) {
		case SPELL_TYPE_SPELL:
		spell_demon_flesh(level, ch, victim, 0);
		break;
	case SPELL_TYPE_SCROLL:
		if (victim)
			spell_demon_flesh(level, ch, victim, 0);
		else
			spell_demon_flesh(level, ch, ch, 0);
		break;
	case SPELL_TYPE_WAND:
		if (victim)
			spell_demon_flesh(level, ch, victim, 0);
		break;
	case SPELL_TYPE_POTION:
		spell_demon_flesh(level, ch, ch, 0);
		break;
	default:
		main_log("ERROR: Invalid spell type in demon_flesh!");
		spec_log("ERROR: Invalid spell type in demon_flesh!", ERROR_LOG);
		break;

	}
}				/* END OF cast_demon_flesh() */
