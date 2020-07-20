/* ******************************************************************* *
*  file: skills.c, Special module.                 Part of DIKUMUD     *
*                                                                      *
*  Special skills for different classes (not grouped per class!)       *
*								       *
*		Created By Hercules                                    *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "constants.h"
#include "func.h"
#include "globals.h"
#include "spells.h"


/* DEATHSHADOW, SPECIAL SPELL CASTED WHEN PLAYER DIES */
void do_deathshadow(struct char_data * ch)
{

	int lv_mp;
	struct affected_type af;

	lv_mp = 1;
	if (IS_CASTED_ON(ch, SPELL_DEATHSHADOW)) {
		ha1350_affect_from_char(ch, SPELL_DEATHSHADOW);
		lv_mp = 3;
	}

	send_to_char("A shadow of death falls upon you.\r\n", ch);
	act("A shadow of death falls upon $n.", FALSE, ch, 0, 0, TO_ROOM);

	af.type = SPELL_DEATHSHADOW;
	af.duration = 8;
	af.modifier = -lv_mp * (GET_MANA_LIMIT(ch) / 5);
	af.location = APPLY_MANA;
	af.bitvector = 0;
	ha1300_affect_to_char(ch, &af);

	af.modifier = -lv_mp * (GET_HIT_LIMIT(ch) / 5);
	af.location = APPLY_HIT;
	ha1300_affect_to_char(ch, &af);

	af.modifier = -lv_mp * (GET_MOVE_LIMIT(ch) / 5);
	af.location = APPLY_MOVE;
	ha1300_affect_to_char(ch, &af);

	return;
}				/* end of do_deathshadow() */


void skill_meditate(struct char_data * ch, char *arg, int cmd)
{

	struct affected_type af;

	if (ch->skills[SKILL_MEDITATE].learned < number(1, 101)) {

		send_to_char("You fail to meditate.\r\n", ch);
		return;
	}


	send_to_char("You clear your mind and begin to meditate.\r\n", ch);
	act("$n mummers strange words as he enters a trance.", TRUE, ch, 0, 0, TO_ROOM);

	af.type = SKILL_MEDITATE;
	af.duration = -1;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	ha1300_affect_to_char(ch, &af);
	li9900_gain_proficiency(ch, SKILL_MEDITATE);

	return;

}				/* end of skill_meditate() */


void do_special(struct char_data * ch, char *arg, int cmd)
{

	/* I'll do this part later ;) */

	return;
}				/* end of do_special() */

void special_restore(struct char_data * ch, struct char_data * vict)
{

	if (ch == vict)
		send_to_char("You fully heal yourself.\r\n", ch);
	else {
		act("You have been fully healed by $n.", FALSE, ch, 0, vict, TO_VICT);
		act("You fully heal $N.", FALSE, ch, 0, vict, TO_CHAR);
	}

	GET_MANA(vict) = GET_SPECIAL_MANA(vict);
	GET_HIT(vict) = GET_HIT_LIMIT(vict);
	GET_MOVE(vict) = GET_MOVE_LIMIT(vict);

	return;

}				/* end of special_restore() */


void special_transport(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	extern int top_of_world;
	int loc_nr, location;
	bool found = FALSE;

	if (ch == vict)
		send_to_char("You call upon your godly powers for transportation.\r\n", ch);
	else {
		act("You call upon your godly powers to transport $N.", FALSE, ch, 0, vict, TO_CHAR);
		act("$n transports you back to town.", FALSE, ch, 0, vict, TO_VICT);
	}

	if (vict->in_room == JAIL_ROOM) {
		send_to_char("I don't think so, resetting experience.\r\n", ch);
		GET_EXP(ch) = 0;
		return;
	}

	if (vict->in_room < 3) {
		send_to_char("From all the places, you found the one place you can't use this!\r\n", ch);
		return;
	}

	if (IS_NPC(vict)) {
		send_to_char("I think that that mob is just happy where he is now...\r\n", ch);
		return;
	}

	loc_nr = 3001;

	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == loc_nr) {
			found = TRUE;
			break;
		}

	if ((location == top_of_world) || (!found)) {
		send_to_char("Houston, we have a problem!\r\n", ch);
		main_log("ERROR: top_of_world reached or not found (Room 3001 lost).\r\n");
		spec_log("ERROR: top_of_world reached or not found (Room 3001 lost).\r\n", ERROR_LOG);
		return;
	}

	if (vict->specials.fighting)
		at2000_do_mob_hunt_check(vict->specials.fighting, vict, 1);

	ha1500_char_from_room(vict);
	ha1600_char_to_room(vict, location);
	in3000_do_look(vict, "", 0);
	return;
}

void special_tornado(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_tornado */


void special_excalibur(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special _excalibur */


void special_fireshroud(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_fireshroud */


void special_enchant(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_enchant */


void special_group_tornado(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_group_tornado */


void special_group_fireblast(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_group_firestorm */


void special_group_transform(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_group_transform */


void special_group_rotation(struct char_data * ch, struct char_data * vict, struct obj_data * obj)
{

	return;
}				/* end of special_group_rotation */
