/* m2 */
/* gv_location: 7501-8000 */
/* ********************************************************************
*  file: magic2.c, Implementation of spells.          Part of DIKUMUD *
*  Usage : The actual effect of magic.                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "constants.h"
#include "utility.h"
#include "func.h"
#include "globals.h"
#include "spells.h"
#include "parser.h"

void spell_word_of_recall(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{

	extern int top_of_world;
	int loc_nr, location, was_in;
	bool found = FALSE;
	char buf[MAX_STRING_LENGTH];



	assert(victim);
	if (magic_fails(ch, victim))
		return;

	if (victim->in_room < 0) {
		return;
	}

	if (IS_NPC(victim)) {
		if (GET_START(victim) < 1 || ch != victim) {
			send_to_char("Sorry a mob can't recall.\r\n", ch);
			return;
		}
	}

	if ((ch != victim) &&
	    IS_SET(GET_ACT3(victim), PLR3_NOSUMMON) &&
	    (saves_spell(ch, victim, SAVING_SPELL) || gv_auto_level > 1)) {
		send_to_char("You failed.\n\r", ch);
		act("$n just attempted to recall you!", FALSE, ch, 0, victim, TO_VICT);
		return;
	}

	if (victim->in_room == JAIL_ROOM) {
		if (ch == victim)
			send_to_char("You can't leave here!", ch);
		else
			send_to_char("You keep that up, and bad things will happen to you!", ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s just tried to recall %s from jail.",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(victim));
		do_wizinfo(buf, IMO_IMM, ch);
		return;
	}

	if (!IS_UNCHARMED_NPC(ch)) {
		if (IS_SET(world[victim->in_room].room_flags, RM1_NO_SUMMON_OUT)) {
			send_to_char("You feel as though you crashed into a magical barrier.\r\n", ch);
			if (ch != victim)
				send_to_char("You temporarily feel out of touch with reality.\r\n", victim);
			return;
		}
	}

	if (!IS_UNCHARMED_NPC(ch)) {
		if (IS_SET(world[victim->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
			send_to_char("You feel a kernal of energy form, but nothing happens!\r\n", ch);
			send_to_char("You temporarily feel out of touch with reality.\r\n", victim);
			return;
		}
	}

	if (IS_PC(victim))
		loc_nr = 3001;
	else
		loc_nr = GET_START(victim);

	for (location = 0; location <= top_of_world; location++)
		if (world[location].number == loc_nr) {
			found = TRUE;
			break;
		}

	if ((location == top_of_world) || !found) {
		send_to_char("You are completely lost.\n\r", victim);
		return;
	}

	/* nothing is going to stop our leaving.  so if fighting... */
	if (victim->specials.fighting) {
		at2000_do_mob_hunt_check(victim->specials.fighting, victim, 1);
	}

	/* a location has been found. */
	was_in = ch->in_room;
	act("$n disappears.", TRUE, victim, 0, 0, TO_ROOM);
	ha1500_char_from_room(victim);
	ha1600_char_to_room(victim, location);
	act("$n appears in the middle of the room.", TRUE, victim, 0, 0, TO_ROOM);
	in3000_do_look(victim, "", 0);
	if (IS_SET(zone_table[world[was_in].zone].flags, ZONE_TESTING))
		ha3700_char_del_zone_objs(ch);

}				/* END OF spell_word_of_recall() */


void spell_summon(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj)
{
	char buf[MAX_STRING_LENGTH];
	int idx, zone;
	sh_int target;



	assert(ch && victim);
	if (magic_fails(ch, victim))
		return;

	if (ch->in_room == JAIL_ROOM) {
		send_to_char("Nobody in their right mind would want to join you.\r\n", ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s just tried to summon %s to jail.",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(victim));
		do_wizinfo(buf, IMO_IMM, ch);
		return;
	}

	if (victim->in_room == JAIL_ROOM) {
		send_to_char("You keep that up, and you might share a cell with them!\r\n", ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s just tried to summon %s from jail.",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(victim));
		do_wizinfo(buf, IMO_IMM, ch);
		return;
	}

	if (GET_LEVEL(victim) > MINV(41, level + 1)) {
		send_to_char("They are too high in level for YOU to summon.\n\r", ch);
		return;
	}

	if (IS_SET(GET_ACT3(victim), PLR3_NOSUMMON)) {
		if (saves_spell(ch, victim, SAVING_SPELL)) {
			send_to_char("You failed.\n\r", ch);
			act("$n just attempted to summon you!", FALSE, ch, 0, victim, TO_VICT);
			return;
		}
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT &&
	    IS_AFFECTED(victim, AFF_BLOCK_SUMMON)) {
		send_to_char("You really failed.\n\r", ch);
		return;
	}

	if (IS_NPC(victim) && (victim->questmob)) {
		send_to_char("Something is blocking you.\r\n", ch);
		return;
	}

	if (IS_SET(world[victim->in_room].room_flags, RM1_NO_SUMMON_OUT)) {
		send_to_char("You feel as though you crashed into a magical barrier.\r\n", ch);
		if (ch != victim)
			send_to_char("You temporarily feel out of touch with reality.\r\n", victim);
		return;
	}

	if (IS_SET(GET_ROOM2(ch->in_room), RM2_NO_SUMMON_IN)) {
		send_to_char("You feel as though you crashed into a magical barrier.\r\n", ch);
		if (ch != victim)
			send_to_char("You temporarily feel out of touch with reality.\r\n", victim);
		return;
	}

	/* ARE THEY IN A NEW ZONE? */
	zone = world[victim->in_room].zone;
	if (GET_LEVEL(ch) < IMO_IMP &&
	    (IS_SET(zone_table[zone].flags, ZONE_TESTING) ||
	     IS_SET(zone_table[zone].flags, ZONE_NO_ENTER))) {
		send_to_char("You can't summon chars from that zone.\r\n", ch);
		if (ch != victim)
			send_to_char("You temporarily feel out of touch with reality.\r\n", victim);
		return;
	}

	/* DO THEY HAVE A NOSUMMON ITEM? */
	for (idx = 0; idx < MAX_WEAR; idx++) {
		if (victim->equipment[idx]) {
			if (IS_OBJ_STAT(victim->equipment[idx], OBJ1_NO_SUMMON)) {
				send_to_char("You feel as though you've encountered a magical barrier.\r\n", ch);
				if (ch != victim &&
				  victim->equipment[idx]->short_description) {
					sprintf(buf, "Your %s vibrates for a second.\r\n",
						victim->equipment[idx]->short_description);
					send_to_char(buf, victim);
				}
				return;
			}
		}
	}			/* END OF for loop */

	if (IS_NPC(victim) &&
	    saves_spell(ch, victim, SAVING_SPELL)) {
		send_to_char("You failed.\n\r", ch);
		return;
	}

	/* nothing is going to stop our leaving.  so if fighting... */
	if (victim->specials.fighting) {
		at2000_do_mob_hunt_check(victim->specials.fighting, victim, 1);
	}

	target = ch->in_room;
	if (target < 0) {
		send_to_char("ACK: Summoners room is negative..\r\n", ch);
		return;
	}

	sprintf(buf, "%s has summoned %s.", GET_NAME(ch), GET_NAME(victim));
	do_wizinfo(buf, IMO_IMM, ch);
	spec_log(buf, SPELL_LOG);

	act("$n disappears suddenly.", TRUE, victim, 0, 0, TO_ROOM);

	ha1500_char_from_room(victim);
	ha1600_char_to_room(victim, target);
	act("$n arrives suddenly.", TRUE, victim, 0, 0, TO_ROOM);
	act("$n has summoned you!", FALSE, ch, 0, victim, TO_VICT);
	in3000_do_look(victim, "", 0);
	return;

}				/* END OF SPELL SUMMON */


void spell_succor(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj)
{

	char buf[MAX_STRING_LENGTH];
	sh_int target;



	assert(ch && victim);
	if (magic_fails(ch, victim))
		return;

	if (IS_NPC(victim)) {
		send_to_char("Nothing happened.\r\n", ch);
		return;
	}

	if (ch == victim) {
		send_to_char("You could hurt yourself doing that.\r\n", ch);
		return;
	}

	if (IS_SET(GET_ACT2(victim), PLR2_JAILED)) {
		send_to_char("Forget it! They are jailed till they die.\r\n", ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s just tried to succor %s from jail.",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(victim));
		do_wizinfo(buf, IMO_IMM, ch);
		return;
	}

	if (GET_LEVEL(victim) > PK_LEV &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You can't do that to Immortals.\n\r", ch);
		return;
	}

	if (IS_SET(GET_ACT3(victim), PLR3_NOSUMMON)) {
		send_to_char("That person doesn't want to be summoned/succored.\n\r", ch);
		send_to_char("You feel a slight tingle.\r\n", victim);
		return;
	}

	if (IS_SET(world[victim->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
		send_to_char("You create only a brief glow in the room.\r\n", ch);
		send_to_char("You temporarily feel out of touch with reality.\r\n",
			     victim);
		return;
	}

	if (victim->specials.fighting) {
		send_to_char("That person can't seem to focus their thoughts.\r\n", ch);
		send_to_char("You felt a brief tingle.\r\n", victim);
		return;
	}

	if (saves_spell(ch, victim, SAVING_SPELL)) {
		send_to_char("You meet up with a resistance which aborts the magic.\r\n", ch);
		send_to_char("You felt a brief tingle.\r\n", victim);
		return;
	}

	act("$n disappears suddenly.", TRUE, victim, 0, 0, TO_ROOM);
	target = db8000_real_room(MAGES_GUILD_ROOM);
	ha1500_char_from_room(victim);
	ha1600_char_to_room(victim, target);
	act("$n arrives suddenly.", TRUE, victim, 0, 0, TO_ROOM);
	act("$n has succored you!", TRUE, ch, 0, victim, TO_VICT);
	in3000_do_look(victim, "", 0);
	return;

}				/* END OF spell_succor() */


void spell_charm_person(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj)
{
	struct affected_type af;
	//int cmd = CMD_CAST;


	assert(ch && victim);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	if (victim == ch) {
		send_to_char("You like yourself even better!\n\r", ch);
		return;
	}

	if (IS_PC(victim) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You fail!\r\n",
			     ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMP) {
		/* CHARMING A PERSON OVER OUR LEVEL? */
		if (level < GET_LEVEL(victim)) {
			if (!number(0, 5))
				hit(ch, victim, TYPE_UNDEFINED);
			return;
		}
		/* IF ITS A NPC AND NOCHARM */
		if (IS_NPC(victim) &&
		    IS_SET(victim->specials.act, PLR1_NOCHARM)) {
			if (!number(0, 5))
				hit(ch, victim, TYPE_UNDEFINED);
			return;
		}
	}

	if (IS_AFFECTED(victim, AFF_CHARM)) {
		send_to_char("They are already infatuated with someone else.\r\n", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM)) {
		send_to_char("You're already infatuated with someone else.\r\n", ch);
		return;
	}

	if (ha4000_circle_follow(victim, ch)) {
		send_to_char("Sorry, following in circles can not be allowed.\n\r", ch);
		return;
	}

	if (saves_spell(ch, victim, SAVING_PARA)) {
		send_to_char("You felt a brief tingle\r\n", victim);
		if (!number(0, 5))
			hit(ch, victim, TYPE_UNDEFINED);
		return;
	}

	if (ha9950_maxed_on_followers(ch, victim, 0)) {
		send_to_char("You have too many followers.\r\n", ch);
		return;
	}

	if (victim->master)
		ha4100_stop_follower(victim, END_FOLLOW_BY_BEG_CHARM_PERSON);

	ha4300_add_follower(victim, ch);

	af.type = SPELL_CHARM_PERSON;

	if (GET_INT(victim))
		af.duration = 24 * 18 / GET_INT(victim);
	else
		af.duration = 24 * 18;

	af.modifier = 0;
	af.location = 0;
	af.bitvector = AFF_CHARM;
	ha1300_affect_to_char(victim, &af);

	act("Isn't $n just such a nice fellow?", FALSE, ch, 0, victim, TO_VICT);
	act("$N thinks your such a nice fellow!", TRUE, ch, 0, victim, TO_CHAR);

	return;

}				/* END OF spell_charm_person() */


void spell_sense_life(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj)
{
	struct affected_type af;



	assert(victim);
	if (magic_fails(ch, victim))
		return;

	if (!ha1375_affected_by_spell(victim, SPELL_SENSE_LIFE)) {
		send_to_char("Your feel your awareness improve.\n\r", ch);

		af.type = SPELL_SENSE_LIFE;
		af.duration = 5 * level;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_SENSE_LIFE;
		ha1300_affect_to_char(victim, &af);
	}
}


void spell_identify(sbyte level, struct char_data * ch,
		      struct char_data * victim, struct obj_data * obj)
{
	char buf[256], buf2[256];
	int i;
	bool found;



	assert(ch && (obj || victim));

	if (obj) {
		if (IS_SET(GET_OBJ2(obj), OBJ2_NO_IDENTIFY)) {
			send_to_char("You don't feel any wiser.\r\n", ch);
			return;
		}

		if (gv_port == ZONING_PORT) {
			if (GET_LEVEL(ch) < IMO_IMM) {
				send_to_char("You are not authorized to use identify on this port!\n\r", ch);
				return;
			}
		}

		send_to_char("You feel informed:\n\r", ch);

		sprintf(buf, "Object '%s', Item type: ", obj->name);
		sprinttype(GET_ITEM_TYPE(obj), item_types, buf2);
		strcat(buf, buf2);
		strcat(buf, "\n\r");
		send_to_char(buf, ch);

		if (obj->obj_flags.bitvector) {
			send_to_char("Item will give you following abilities:  ", ch);
			sprintbit(obj->obj_flags.bitvector, affected_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
		}

		send_to_char("Item is: ", ch);
		sprintbit(obj->obj_flags.flags1, oflag1_bits, buf);
		send_to_char(buf, ch);
		sprintbit(obj->obj_flags.flags2, oflag2_bits, buf);
		if ((OVAL1(obj) > 0) &&
		    (OVAL1(obj) < 36) &&
		    (GET_ITEM_TYPE(obj) == ITEM_ARMOR)) {
			sprintf(buf2, "ANTI_%s ", race_list[OVAL1(obj)]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if ((OVAL1(obj) > 50) &&
		    (OVAL1(obj) < 101) &&
		    (GET_ITEM_TYPE(obj) == ITEM_ARMOR)) {
			sprintf(buf2, "%s_ONLY ", race_list[OVAL1(obj) - 50]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if (GET_ITEM_TYPE(obj) == ITEM_ARMOR &&
		    OVAL2(obj) > 0) {
			sprintbit(OVAL2(obj), class_anti, buf);
			strcat(buf, "\0");
		}
		strcat(buf, "\n\r");
		send_to_char(buf, ch);

		/* Let them see the timer with identify. */
		if (GET_TIMER(obj) == -1)
			sprintf(buf2, "No Timer");
		else
			sprintf(buf2, "%d", GET_TIMER(obj));

		if ((GET_ITEM_TYPE(obj) == ITEM_CONTAINER)) {	/* shows container held
								 * weight */
			sprintf(buf, "Weight: %d/%d, Value: %d, Timer: %s\n\r",
				obj->obj_flags.weight, obj->obj_flags.value[0], obj->obj_flags.cost, buf2);
		}
		else {
			sprintf(buf, "Weight: %d, Value: %d, Timer: %s\n\r",
			    obj->obj_flags.weight, obj->obj_flags.cost, buf2);
		}

		send_to_char(buf, ch);

		switch (GET_ITEM_TYPE(obj)) {

		case ITEM_SCROLL:
		case ITEM_POTION:
			sprintf(buf, "Level %d spells of:\n\r", obj->obj_flags.value[0]);
			send_to_char(buf, ch);
			if (obj->obj_flags.value[1] >= 1) {
				sprinttype(obj->obj_flags.value[1] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			if (obj->obj_flags.value[2] >= 1) {
				sprinttype(obj->obj_flags.value[2] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			if (obj->obj_flags.value[3] >= 1) {
				sprinttype(obj->obj_flags.value[3] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
		case ITEM_SPELL:
			sprintf(buf, "Has %d charges, with %d charges left.\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			send_to_char(buf, ch);

			sprintf(buf, "Level %d spell of:\n\r", obj->obj_flags.value[0]);
			send_to_char(buf, ch);

			if (obj->obj_flags.value[3] >= 1) {
				sprinttype(obj->obj_flags.value[3] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			break;

		case ITEM_WEAPON:
			send_to_char("Weapons bits: ", ch);
			sprintbit(obj->obj_flags.value[0], weapon_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "Damage Dice is '%dD%d'\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			send_to_char(buf, ch);
			break;

		case ITEM_ARMOR:
			sprintf(buf, "AC-apply is %d\n\r",
				obj->obj_flags.value[0]);
			send_to_char(buf, ch);
			break;

		case ITEM_QUEST:
			sprintf(buf, "AC-apply is %d\n\rTime left: %d\n\r",
				obj->obj_flags.value[0], obj->obj_flags.timer);
			send_to_char(buf, ch);
			break;

		case ITEM_QSTWEAPON:
			send_to_char("Weapons bits: ", ch);
			sprintbit(obj->obj_flags.value[0], weapon_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "Damage Dice is '%dD%d'\n\rTime left: %d\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.timer);
			send_to_char(buf, ch);
			break;
		}

		found = FALSE;

		for (i = 0; i < MAX_OBJ_AFFECT; i++) {
			if ((obj->affected[i].location != APPLY_NONE) &&
			    (obj->affected[i].modifier != 0)) {
				if (!found) {
					send_to_char("Can affect you as :\n\r", ch);
					found = TRUE;
				}

				sprinttype(obj->affected[i].location, apply_types, buf2);
				sprintf(buf, "    Affects : %s By %d\n\r", buf2, obj->affected[i].modifier);
				send_to_char(buf, ch);
			}
		}

	}
	else {			/* victim */

		if (IS_PC(victim)) {
			sprintf(buf, "%d Years,  %d Months,  %d Days,  %d Hours old.\n\r",
				z_age(victim).year,
				z_age(victim).month,
				z_age(victim).day,
				z_age(victim).hours);
			send_to_char(buf, ch);
			/*
			 sprintf(buf,"Height %dcm  Weight %dpounds \n\r",
							 GET_HEIGHT(victim), GET_WEIGHT(victim));
			 send_to_char(buf,ch);
			 */
			sprintf(buf, "Str %d,  Int %d,  Wis %d,  Dex %d,  Con %d\n\r",
				GET_STR(victim),
				GET_INT(victim),
				GET_WIS(victim),
				GET_DEX(victim),
				GET_CON(victim));
			send_to_char(buf, ch);
		}
		else {		/* is a mob */
			send_to_char("You learn nothing new.\n\r", ch);
		}
	}

}

void spell_fire_breath(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj)
{
	int dam;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *burn;
	struct obj_data *j;
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();


	if (IS_NPC(ch)) {
		/* dam = GET_LEVEL(ch) * 30; */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 2;
	}
	else
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 2.5;

	if (saves_spell(ch, victim, SAVING_BREATH))
		dam >>= 1;

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
		dam = MINV(1000, dam);
	else
		dam = MINV(500, dam);

	/* And now for the damage on inventory */
	if (number(0, 50) > GET_LEVEL(ch)) {
		if (!saves_spell(ch, victim, SAVING_BREATH)) {
			for (burn = victim->carrying;
			     burn &&
			     (burn->obj_flags.type_flag != ITEM_SCROLL) &&
			     (burn->obj_flags.type_flag != ITEM_WAND) &&
			     (burn->obj_flags.type_flag != ITEM_STAFF) &&
			     (burn->obj_flags.type_flag != ITEM_NOTE) &&
			     (number(0, 2) == 0);
			     burn = burn->next_content);
			if (burn) {	/* if we found one thing to burn its
					 * gone */
				act("$o burns.", 0, victim, burn, 0, TO_CHAR);
				sprintf(buf, "(obj) fire_breath %s - %s burned by %s", GET_REAL_NAME(victim), OBJS(burn, victim), GET_REAL_NAME(ch));
				main_log(buf);
				spec_log(buf, EQUIPMENT_LOG);
				if ((GET_ITEM_TYPE(burn) == ITEM_CONTAINER) ||
				    (GET_ITEM_TYPE(burn) == ITEM_QSTCONT)) {
					strcpy(buf, "Contained :\n                            ");
					for (j = burn->contains; j; j = j->next_content) {
						strcat(buf, j->short_description);
						strcat(buf, "\r\n                            ");
					}
					main_log(buf);
					spec_log(buf, EQUIPMENT_LOG);
				}
				ha2700_extract_obj(burn);
			}
		}
	}

	DAMAGE(ch, victim, dam, SPELL_FIRE_BREATH);

}				/* END OF spell_fire_breath() */


void spell_frost_breath(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj)
{
	int dam;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *frozen;
	struct obj_data *j;
	struct affected_type af;

	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (!saves_spell(ch, victim, SAVING_BREATH)) {
		af.type = SPELL_CHILL_TOUCH;
		af.duration = 1;
		af.modifier = -1;
		af.location = APPLY_STR;
		af.bitvector = 0;
		ha1400_affect_join(victim, &af, TRUE, FALSE);
		send_to_char("The intense cold saps your strength.\n\r", victim);
	}
	if (IS_NPC(ch)) {
		/* dam = GET_LEVEL(ch) * 25;  */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 2.5;
	}
	else {
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 3.5;
	}
	if (saves_spell(ch, victim, SAVING_BREATH))
		dam >>= 1;

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
		dam = MINV(1000, dam);
	else
		dam = MINV(500, dam);

	/* And now for the damage on inventory */
	if (number(0, 50) < GET_LEVEL(ch)) {
		if (!saves_spell(ch, victim, SAVING_BREATH)) {
			for (frozen = victim->carrying;
			     frozen && (frozen->obj_flags.type_flag != ITEM_DRINKCON) &&
			     (frozen->obj_flags.type_flag != ITEM_FOOD) &&
			     (frozen->obj_flags.type_flag != ITEM_POTION) && (number(0, 2) == 0);
			     frozen = frozen->next_content);
			if (frozen) {
				act("$o breaks.", 0, victim, frozen, 0, TO_CHAR);
				sprintf(buf, "%s broke %s's %s",
					GET_REAL_NAME(ch),
					GET_REAL_NAME(victim),
					GET_OBJ_NAME(frozen));
				sprintf(buf, "(obj) frost_breath %s - %s iced by %s", GET_REAL_NAME(victim), OBJS(frozen, victim), GET_REAL_NAME(ch));
				main_log(buf);
				spec_log(buf, EQUIPMENT_LOG);
				if ((GET_ITEM_TYPE(frozen) == ITEM_CONTAINER) ||
				    (GET_ITEM_TYPE(frozen) == ITEM_QSTCONT)) {
					strcpy(buf, "Contained :\n                            ");
					for (j = frozen->contains; j; j = j->next_content) {
						strcat(buf, j->short_description);
						strcat(buf, "\r\n                            ");
					}
					main_log(buf);
					spec_log(buf, EQUIPMENT_LOG);
				}
				ha2700_extract_obj(frozen);
			}
		}
	}			/* End fo number(0,50) */

	DAMAGE(ch, victim, dam, SPELL_FROST_BREATH);

}				/* END OF spell_frost_breath()  */


void spell_acid_breath(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj)
{
	int dam;
	int damaged;
	int ha1900_apply_ac(struct char_data * ch, int eq_pos);
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));


	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (IS_NPC(ch)) {
		/* dam = GET_LEVEL(ch) * 20; */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 3.5;
	}
	else {
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 4.5;
	}

	if (saves_spell(ch, victim, SAVING_BREATH))
		dam >>= 1;

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
		dam = MINV(1000, dam);
	else
		dam = MINV(500, dam);

	/* And now for the damage on equipment */
	if (number(0, 50) < GET_LEVEL(ch)) {
		if (!saves_spell(ch, victim, SAVING_BREATH)) {
			for (damaged = 0; damaged < MAX_WEAR &&
			     (victim->equipment[damaged]) &&
			     (victim->equipment[damaged]->obj_flags.type_flag == ITEM_ARMOR) &&
			(victim->equipment[damaged]->obj_flags.value[0] > 0) &&
			     (number(0, 2) == 0); damaged++) {
				act("$o is damaged.", 0, victim, victim->equipment[damaged], 0, TO_CHAR);
				GET_AC(victim) -= ha1900_apply_ac(victim, damaged);
				ch->equipment[damaged]->obj_flags.value[0] -= number(1, 7);
				GET_AC(victim) += ha1900_apply_ac(victim, damaged);
				ch->equipment[damaged]->obj_flags.cost = 0;
			}
		}
	}
	DAMAGE(ch, victim, dam, SPELL_ACID_BREATH);

}				/* end of spell_acid_breath() */


void spell_gas_breath(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj)
{
	struct affected_type af;
	int dam;
	char buf[MAX_STRING_LENGTH];
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));


	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	if ((ch->master == victim)	/* don't attack our leader */
	    ||(victim->master == ch))	/* don't attack followers */
		return;

	/* IF ITS ONE MOB AGAINST ANOTHER, FORGET IT! */
	if (IS_NPC(ch) &&
	    IS_NPC(victim) &&
	    !IS_AFFECTED(victim, AFF_CHARM)) {
		send_to_char("You can't gas other mobs.\r\n", ch);
		send_to_char("You can't be gassed by other mobs.\r\n", victim);
		return;
	}

	/* IF WE ARE A MORTAL, AND THIS IS AN IMM, GET OUT OF HERE */
	if (GET_LEVEL(victim) >= IMO_SPIRIT && IS_PC(victim)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You are immune to %s's breath.\r\n",
			GET_REAL_NAME(ch));
		send_to_char(buf, victim);
		return;
	}

	if (IS_NPC(ch)) {
		/* dam = GET_LEVEL(ch) * 10; */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 4;
	}
	else {
		/* dam = dice(MINV(level>>1,4),3)+MINV(level>>1,4) */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 5.5;
	}

	if (saves_spell(ch, victim, SAVING_BREATH))
		dam >>= 1;

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
		dam = MINV(1000, dam);
	else
		dam = MINV(500, dam);

	if (!saves_spell(ch, victim, SAVING_PARA)) {
		af.type = SPELL_POISON;
		af.duration = level * 2;
		af.modifier = -2;
		af.location = APPLY_STR;
		af.bitvector = AFF_POISON;

		ha1400_affect_join(victim, &af, FALSE, FALSE);

		act("$n starts choking on the toxic fumes!!", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You feel very sick.\n\r", victim);
	}

	/* DOES IT BLIND? */
	if (!saves_spell(ch, victim, SAVING_BREATH) &&
	    (!ha1375_affected_by_spell(victim, SPELL_BLINDNESS)) &&
	    (GET_LEVEL(victim) < IMO_LEV)) {
		af.type = SPELL_BLINDNESS;
		af.location = APPLY_HITROLL;
		af.modifier = -4;	/* Make hitroll worse */
		af.duration = 3;
		af.bitvector = AFF_BLIND;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_AC;
		af.modifier = +40;	/* Make AC Worse! */
		ha1300_affect_to_char(victim, &af);

		act("$n seems to have been blinded by the gas!", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You have been blinded!\n\r", victim);
	}

	DAMAGE(ch, victim, dam, SPELL_GAS_BREATH);
}


void spell_lightning_breath(sbyte level, struct char_data * ch,
			      struct char_data * victim, struct obj_data * obj)
{
	int dam;
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));


	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (IS_NPC(ch)) {
		/* dam = GET_LEVEL(ch) * 15; */
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 3.5;
	}
	else {
		dam = ((level + dice(1, 4)) * (level + dice(1, 4))) / 5;
	}

	if (saves_spell(ch, victim, SAVING_BREATH))
		dam >>= 1;

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
		dam = MINV(1000, dam);
	else
		dam = MINV(500, dam);

	DAMAGE(ch, victim, dam, SPELL_LIGHTNING_BREATH);
}				/* END OF spell_lightning_breath() */


void spell_hold_person(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj)
{
	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(ch && victim);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	/* IF MOB HAS A LOT OF EXP, MAKE IT HARDER */
	if (IS_NPC(victim) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		if (GET_EXP(victim) > 1500000) {
			if (number(1, 5) < 4) {
				return;
			}
		}
		else if (GET_EXP(victim) > 1000000) {
			if (number(1, 5) < 3) {
				return;
			}
		}
		else if (GET_EXP(victim) > 500000) {
			if (number(1, 5) < 2) {
				return;
			}
		}
	}			/* END OF ITS A PC */

	/* DON'T ALLOW IF FIGHTING - Adrenaline (SP?) is flowing */
	if (victim->specials.fighting &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You shrug of a tingling feeling!\r\n", victim);
		return;
	}

	if (((IS_NPC(victim)) &&
	     (IS_SET(victim->specials.act, PLR1_NOHOLD))) ||
	    (level < GET_LEVEL(victim))) {
		if (!number(0, 5))
			hit(ch, victim, TYPE_UNDEFINED);
		return;
	}

	/* IF VICTIM IS PLAYER, DONT LET EM CAST IT */

	if ((IS_PC(victim) &&
	     (!IS_NPC(ch)) &&
	     GET_LEVEL(ch) < IMO_IMP)) {
		send_to_char("Sorry, but you can't cast this spell on players anymore.\r\n", ch);
		act("$n is trying to hold you but finds out it's not working.", FALSE, ch, 0, victim, TO_VICT);
		return;
	}



	if (saves_spell(ch, victim, SAVING_PARA)) {
		send_to_char("You felt a brief tingle.\r\n", victim);
		if (!number(0, 5))
			hit(ch, victim, TYPE_UNDEFINED);
		return;
	}


	af.type = SPELL_HOLD_PERSON;
	if (GET_INT(victim))
		af.duration = MAXV(2, 3 * 18 / GET_INT(victim));
	else
		af.duration = 2 * 18;

	af.modifier = 0;
	af.location = 0;
	af.bitvector = AFF_HOLD_PERSON;
	ha1300_affect_to_char(victim, &af);

	SET_BIT(GET_ACT1(victim), PLR1_CHAR_WAS_HELD);

	act("You feel like a stone block!", FALSE, ch, 0, victim, TO_VICT);
	act("$N begins imitating a stone block!", FALSE, ch, 0, victim, TO_CHAR);
	act("$N begins imitating a stone block!", FALSE, ch, 0, victim, TO_NOTVICT);

}				/* END OF hold_person() */

void spell_knock(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj)
{



	send_to_char("Nothing happens.\r\n", ch);
	return;
}				/* END OF spell_() */

void spell_waterwalk(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_WATERWALK))
		return;

	af.type = SPELL_WATERWALK;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_WATERWALK;

	ha1300_affect_to_char(victim, &af);

	send_to_char("You feel lighter on your feet.\n\r", victim);
	if (victim != ch)
		act("$N takes on a brief but holy glow.",
		    FALSE, ch, 0, victim, TO_CHAR);
	return;

}				/* END OF spell_() */

void spell_teleport_self(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj)
{

	int lv_dest_room, rc;
	char buf[MAX_STRING_LENGTH];



	assert(victim && ch);

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("Sorry, you can't do that while in jail.\r\n", ch);
		return;
	}

	if (!IS_UNCHARMED_NPC(ch) &&
	    IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
		send_to_char("You feel a kernal of energy form then it fades away.\r\n", ch);
		return;
	}

	/* SKILL CHECK FOR CORRECT ROOM */
	lv_dest_room = victim->in_room;
	rc = wz1500_did_we_make_teleport(ch,
					 ch->in_room,
					 lv_dest_room,
				       GET_SKILL(ch, SPELL_TELEPORT_SELF), 0);

	/* If it's their quest mob they can't teleport to it */
	if (victim)
		if (IS_NPC(victim))
			if (victim->questmob) {
				send_to_char("A strange but powerful force is stopping you from going there.\r\n", ch);
				return;
			}
	if (IS_PC(ch)) {
		if (ch->questmob)
			if (ch->questmob->in_room == lv_dest_room) {
				send_to_char("A strange put powerful force is stopping you from going to this room.\r\n", ch);
				return;
			}
		if (ch->questobj)
			if (ch->questobj->in_room == lv_dest_room) {
				send_to_char("There is a strange item at that location stopping you from going there.\r\n", ch);
				return;
			}
	}			/* end of quest part */



	if (rc != TELEPORT_WORKED) {
		if (rc == TELEPORT_FAILED_ZCORD) {
			return;
		}
		send_to_char("That didn't feel right.\r\n", ch);
		if (rc == TELEPORT_FAILED_MAJOR) {
			spell_elsewhere(GET_LEVEL(ch), ch, ch, 0);
			return;
		}
		else {
			lv_dest_room = ma1000_do_get_random_room(ch, 0);
		}
	}

	rc = li3000_is_blocked_from_room(ch, lv_dest_room, BIT0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	rc = ha9800_protect_char_from_dt(ch, lv_dest_room, 0);
	if (rc) {
		return;
	}

	if (ch->specials.fighting) {
		if (cl1900_at_war(ch, ch->specials.fighting))
			cl2000_player_escape(ch, ch->specials.fighting);
		at2000_do_mob_hunt_check(ch->specials.fighting, ch, 1);
		ft1400_stop_fighting(ch, 0);
	}

	if (gv_port == ZONING_PORT) {
		if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[lv_dest_room].zone].lord) &&
		    GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to go into that zone!\n\r", ch);
			return;
		}
	}

	act("There is a bright flash of light and $n winks out of existence!", TRUE, ch, 0, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_dest_room);
	if (ch->in_room < 0) {
		sprintf(buf, "ERROR 07 %s is in teleport_self with negative room. Dest=%d .",
			GET_REAL_NAME(ch), lv_dest_room);
		main_log(buf);
	}
	in3000_do_look(ch, "", 0);
	act("There is a bright flash of light and $n winks into existence!", TRUE, ch, 0, 0, TO_ROOM);

	if (IS_SET(world[ch->in_room].room_flags, RM1_DEATH)) {
		ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_TELEPORT);
		return;
	}

	return;

}				/* END OF spell_teleport_self() */


void spell_teleport_group(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{

	int lv_orig_room, lv_dest_room, rc, lv_follower_count;
	struct follow_type *fdx;
	struct char_data *leader;



	assert(victim && ch);

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("Sorry, you can't do that while in jail.\r\n", ch);
		return;
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
		send_to_char(" You feel the proper kernal of energy form but it fades away.\r\n", ch);
		return;
	}

	lv_orig_room = ch->in_room;

	if (IS_PC(victim) &&
	    GET_LEVEL(victim) > PK_LEV &&
	    GET_LEVEL(ch) < IMO_SPIRIT) {
		send_to_char("The magic fails!\r\n", ch);
		return;
	}

	/* If it's their quest mob they can't teleport to it */
	if (victim)
		if (IS_NPC(victim))
			if (victim->questmob) {
				send_to_char("A strange but powerful force is stopping you from going there.\r\n", ch);
				return;
			}

	lv_dest_room = victim->in_room;
	if (IS_PC(ch)) {
		if (ch->questmob)
			if (ch->questmob->in_room == lv_dest_room) {
				send_to_char("A strange put powerful force is stopping you from going to this room.\r\n", ch);
				return;
			}
		if (ch->questobj)
			if (ch->questobj->in_room == lv_dest_room) {
				send_to_char("There is a strange item at that location stopping you from going there.\r\n", ch);
				return;
			}
	}			/* end of quest part */


	/* FOLLOWING SHOULD BE BLOCKED IN SPELL_PARSER */
	if (!IS_AFFECTED(ch, AFF_GROUP)) {
		send_to_char("Your not grouped so nothing happens.\r\n", ch);
		return;
	}

	if (ch->master)
		leader = ch->master;
	else
		leader = ch;

	/* COUNT GROUP MEMBERS */
	lv_follower_count = 1;
	for (fdx = leader->followers; fdx; fdx = fdx->next) {
		if (IS_AFFECTED(fdx->follower, AFF_GROUP) &&
		    fdx->follower->in_room == lv_orig_room) {
			lv_follower_count++;
		}
	}			/* END OF FOR LOOP */

	if (leader->in_room == lv_orig_room &&
	    IS_AFFECTED(leader, AFF_GROUP)) {

		/* SKILL CHECK FOR CORRECT ROOM */
		lv_dest_room = victim->in_room;
		rc = wz1500_did_we_make_teleport(ch,
						 leader->in_room,
						 lv_dest_room,
					  GET_SKILL(ch, SPELL_TELEPORT_GROUP),
						 lv_follower_count);
		if (rc != TELEPORT_WORKED) {
			if (rc == TELEPORT_FAILED_ZCORD) {
				return;
			}
			send_to_char("That didn't feel right!\r\n", leader);
			if (rc == TELEPORT_FAILED_MAJOR) {
				spell_elsewhere(GET_LEVEL(ch), ch, ch, 0);
				return;
			}
			else {
				lv_dest_room = ma1000_do_get_random_room(leader, 0);
			}
		}

		if (lv_dest_room < 1) {
			send_to_char("A powerful force halts astral travel.\r\n", ch);
			return;
		}

		rc = li3000_is_blocked_from_room(leader, lv_dest_room, BIT0);
		if (rc) {
			li3100_send_blocked_text(ch, rc, 0);
			return;
		}

		rc = ha9800_protect_char_from_dt(ch, lv_dest_room, 0);
		if (rc) {
			return;
		}

		if (leader->specials.fighting) {
			if (cl1900_at_war(leader, leader->specials.fighting))
				cl2000_player_escape(leader, leader->specials.fighting);
			at2000_do_mob_hunt_check(leader->specials.fighting, leader, 1);
			ft1400_stop_fighting(leader, 0);
		}


		if (gv_port == ZONING_PORT) {
			if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[lv_dest_room].zone].lord) &&
			    GET_LEVEL(ch) < IMO_IMM) {
				send_to_char("You are not authorized to go into that zone!\n\r", ch);
				return;
			}
		}

		act("There is a sharp crack of sound and $n winks out of existence.",
		    FALSE, leader, 0, 0, TO_ROOM);
		ha1500_char_from_room(leader);
		ha1600_char_to_room(leader, lv_dest_room);
		in3000_do_look(leader, "", 0);
		act("There is a sharp crack of sound and $n winks into existence.",
		    FALSE, leader, 0, 0, TO_ROOM);
	}			/* END OF correct room */

	if (IS_SET(world[leader->in_room].room_flags, RM1_DEATH)) {
		ha1750_remove_char_via_death_room(leader, LEFT_BY_DEATH_TELEPORT);
		return;
	}

	/* NOW LOOP THROUGH FOR FOLLOWERS */
	for (fdx = leader->followers; fdx; fdx = fdx->next) {
		if (IS_AFFECTED(fdx->follower, AFF_GROUP) &&
		    fdx->follower->in_room == lv_orig_room) {
			/* GOT THE MANA FOR THIS PLAYER? */
			if (GET_LEVEL(ch) < IMO_SPIRIT) {
				if (GET_MANA(ch) < 11) {
					send_to_char("You don't feel up to the strain.\r\n", ch);
					return;
				}
				GET_MANA(ch) -= 10;
			}
			/* DOES TELEPORT FAIL? */
			lv_dest_room = victim->in_room;
			rc = wz1500_did_we_make_teleport(ch,
						       fdx->follower->in_room,
							 lv_dest_room,
					  GET_SKILL(ch, SPELL_TELEPORT_GROUP),
							 lv_follower_count);
			if (rc != TELEPORT_WORKED) {
				if (rc == TELEPORT_FAILED_ZCORD) {
					return;
				}
				send_to_char("That felt wrong somehow.\r\n", fdx->follower);
				if (rc == TELEPORT_FAILED_MAJOR) {
					spell_elsewhere(GET_LEVEL(ch), fdx->follower, fdx->follower, 0);
					return;
				}
				else {
					lv_dest_room =
						ma1000_do_get_random_room(fdx->follower, 0);
				}
			}

			rc = li3000_is_blocked_from_room(fdx->follower,
							 lv_dest_room, BIT0);
			if (rc) {
				/* JUST LEAVE THEM IN THE SAME ROOM */
				li3100_send_blocked_text(fdx->follower, rc, 0);
				lv_dest_room = fdx->follower->in_room;
			}

			rc = ha9800_protect_char_from_dt(fdx->follower,
							 lv_dest_room, 0);
			if (rc) {
				/* JUST LEAVE THEM IN THE SAME ROOM */
				lv_dest_room = fdx->follower->in_room;
			}

			if (fdx->follower->specials.fighting) {
				if (cl1900_at_war(fdx->follower, fdx->follower->specials.fighting))
					cl2000_player_escape(fdx->follower, fdx->follower->specials.fighting);
				at2000_do_mob_hunt_check(fdx->follower->specials.fighting, fdx->follower, 1);
				ft1400_stop_fighting(fdx->follower, 0);
			}

			act("There is a sharp crack of sound and $n winks out of existence.", FALSE, fdx->follower, 0, 0, TO_ROOM);
			ha1500_char_from_room(fdx->follower);
			ha1600_char_to_room(fdx->follower, lv_dest_room);
			in3000_do_look(fdx->follower, "", 0);
			act("There is a sharp crack of sound and $n winks into existence.", FALSE, fdx->follower, 0, 0, TO_ROOM);

			if (IS_SET(world[fdx->follower->in_room].room_flags, RM1_DEATH)) {
				ha1750_remove_char_via_death_room(fdx->follower, LEFT_BY_DEATH_TELEPORT);
				return;
			}
		}		/* end of group member */
	}			/* END OF for */

	return;

}				/* END OF spell_teleport_group() */

void spell_teleview_minor(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{



	spell_teleview_major(level, ch, victim, obj);
	return;

}				/* END OF spell_teleview_minor() */

void spell_teleview_major(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{

	int lv_orig_room, lv_was_riding, rc;



	assert(victim && ch);

	/* IF THE SAME ROOM... */
	if (ch->in_room == victim->in_room ||
	    ch->in_room == NOWHERE ||
	    victim->in_room == NOWHERE) {
		if (GET_LEVEL(ch) > IMO_LEV) {
			send_to_char("Nothing happens.\r\n", ch);
			return;
		}
		send_to_char("The shock of looking at yourself creates a paradox and HURTS you!\r\n", victim);
		GET_HIT(ch) = -1;
		GET_MANA(ch) = 0;
		GET_MOVE(ch) = 0;
		if (GET_LEVEL(ch) < IMO_LEV) {
			GET_COND(ch, FOOD) = 0;
			GET_COND(ch, THIRST) = 0;
			GET_COND(ch, DRUNK) = 0;
		}
		ft1200_update_pos(ch);
		return;
	}

	if (ch->in_room < 1 ||
	    victim->in_room < 1) {
		send_to_char("Hrmm a bug in the code keeps anything from happening.\r\n", ch);
		return;
	}

	rc = li3000_is_blocked_from_room(ch, victim->in_room, BIT0 | BIT1 | BIT2);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		/* send_to_char("The magic fails.\r\n", ch); */
		return;
	}

	lv_orig_room = ch->in_room;
	lv_was_riding = ch->riding;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, victim->in_room);
	send_to_char("Via your magic you see: ", ch);
	in3000_do_look(ch, "", 0);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_orig_room);
	ch->riding = lv_was_riding;
	return;

}				/* END OF spell_teleview_major() */

void spell_haste(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;



	assert(victim);

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_HASTE))
		return;

	af.type = SPELL_HASTE;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_HASTE;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You feel faster.\n\r", victim);
	act("$n flickers for a moment.", FALSE, victim, 0, 0, TO_ROOM);
	return;

}				/* END OF spell_haste() */

void spell_fly(sbyte level, struct char_data * ch,
	         struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;



	assert(victim);

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_FLY))
		return;

	af.type = SPELL_FLY;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_FLY;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You start floating.\n\r", victim);
	act("$n starts floating off the ground.", FALSE, victim, 0, 0, TO_ROOM);
	return;

}				/* END OF spell_fly() */

void spell_dreamsight(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj)
{

	//struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	send_to_char("Spell has been removed.\r\n", ch);
	return;
	/*
	 if (magic_fails(ch,victim))
	 return;
	
	 if ( ha1375_affected_by_spell(victim, SPELL_DREAMSIGHT))
	 return;
	
	 af.type      = SPELL_DREAMSIGHT;
	 af.duration  = MINV(5, level/2);
	 af.modifier  = 0;
	 af.location  = APPLY_NONE;
	 af.bitvector = AFF_DREAMSIGHT;
	
	 ha1300_affect_to_char(victim, &af);
	
	 send_to_char("Your eyes tingle.\n\r", victim);
	 if (victim != ch)
	 act("$N's eyes develope a reddish tint.",
			 FALSE,ch,0,victim,TO_CHAR);
	 return;
	 */
}				/* END OF spell_dreamsight() */

void kill_off_gate_mob(struct char_data * ch, struct char_data * mob)
{



	hit(mob, ch, TYPE_UNDEFINED);
	if (ch->specials.fighting)
		ft1400_stop_fighting(ch, 0);
	if (mob->specials.fighting)
		ft1400_stop_fighting(mob, 0);

	act("$N disappears!", FALSE, ch, 0, mob, TO_ROOM);
	act("$N disappears!", FALSE, ch, 0, mob, TO_CHAR);

	ha3000_extract_char(mob, END_EXTRACT_BY_GATE_MOB);
	return;

}				/* END OF kill_off_gate_mob() */


void spell_gate(sbyte level, struct char_data * ch,
		  struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;
	struct char_data *mob;
	int lv_mob, real_num;
	char buf[MAX_STRING_LENGTH];



	/* CAN'T CAST THIS IN JAIL */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    ch->in_room == JAIL_ROOM) {
		send_to_char("I don't think so!\r\n", ch);
		return;
	}

	lv_mob = number(0, 4) + 2690;

	real_num = db8100_real_mobile(lv_mob);
	if (real_num < 0) {
		send_to_char("You fail to make mental contact.\r\n", ch);
		return;
	}

	mob = db5000_read_mobile(real_num, REAL);
	GET_START(mob) = world[ch->in_room].number;
	ha1600_char_to_room(mob, ch->in_room);

	if (mob->in_room != ch->in_room ||
	    mob->in_room < 0) {
		sprintf(buf, "ERROR: Created mob %s with invalid room %d/%d.",
			GET_REAL_NAME(mob), mob->in_room, ch->in_room);
		ABORT_PROGRAM();
	}
	act("$n builds a hot fire and utters a horrible incantation.",
	    TRUE, ch, 0, 0, TO_ROOM);
	act("$n has summoned $N!", FALSE, ch, 0, mob, TO_ROOM);
	act("You have summoned $N!", FALSE, ch, 0, mob, TO_CHAR);

	if (ha4000_circle_follow(mob, ch)) {
		send_to_char("Sorry, following in circles not allowed.\n\r",
			     ch);
		kill_off_gate_mob(ch, mob);
		return;
	}

	if (mob->master)
		ha4100_stop_follower(mob, END_FOLLOW_BY_GATE_SPELL);

	if (ha9950_maxed_on_followers(ch, mob, 0)) {
		send_to_char("You have too many followers.\r\n", ch);
		kill_off_gate_mob(ch, mob);
		return;
	}

	if (number(1, 4) == 1) {
		send_to_char("The charm fails.\r\n", ch);
		kill_off_gate_mob(ch, mob);
		return;
	}

	ha4300_add_follower(mob, ch);
	af.type = SPELL_CHARM_PERSON;

	if (GET_INT(mob))
		af.duration = 24 * 18 / GET_INT(mob);
	else
		af.duration = 24 * 18;

	af.modifier = 0;
	af.location = 0;
	af.bitvector = AFF_CHARM;
	ha1300_affect_to_char(mob, &af);

	if (IS_NPC(ch) && !ch->master) {
		SET_BIT(ch->specials.affected_by, AFF_GROUP);
		SET_BIT(mob->specials.affected_by, AFF_GROUP);
	}

	act("Isn't $n just such a nice fellow?", FALSE, ch, 0, mob, TO_VICT);
	send_to_char("You have a new friend!\r\n", ch);
	return;

}				/* END OF spell_() */

void spell_vitalize_mana(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj)
{



	assert(ch);

	if (IS_NPC(victim))
		return;

	if (magic_fails(ch, victim))
		return;

	if (GET_MANA(victim) + GET_MOVE(victim) > GET_SPECIAL_MANA(victim)) {
		GET_MANA(victim) = GET_SPECIAL_MANA(victim);
	}
	else {
		GET_MANA(victim) += GET_MOVE(victim);
	}

	GET_MOVE(victim) = 0;
	return;

}				/* END OF spell_vitalize_mana() */

void spell_vitalize_hit(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj)
{



	assert(ch);

	if (IS_NPC(victim))
		return;

	if (magic_fails(ch, victim))
		return;

	if (GET_HIT(victim) + GET_MOVE(victim) > li1500_hit_limit(victim)) {
		GET_HIT(victim) = li1500_hit_limit(victim);
	}
	else {
		GET_HIT(victim) += GET_MOVE(victim);
	}

	GET_MOVE(victim) = 0;
	return;

}				/* END OF spell_vitalize_hit() */

void spell_vigorize_minor(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{

	int lv_gain;



	assert(ch);

	if (magic_fails(ch, victim))
		return;

	lv_gain = dice(1, 8) + 1;

	if (GET_MOVE(victim) + lv_gain > li1600_move_limit(victim)) {
		GET_MOVE(victim) = li1600_move_limit(victim);
	}
	else {
		GET_MOVE(victim) += lv_gain;
	}

	return;

}				/* END OF spell_vigorize_minor() */

void spell_vigorize_medium(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj)
{

	int lv_gain;



	assert(ch);

	if (magic_fails(ch, victim))
		return;

	lv_gain = dice(2, 8) + 1;

	if (GET_MOVE(victim) + lv_gain > li1600_move_limit(victim)) {
		GET_MOVE(victim) = li1600_move_limit(victim);
	}
	else {
		GET_MOVE(victim) += lv_gain;
	}

	return;

}				/* END OF spell_vigorize_medium() */

void spell_vigorize_major(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{

	int lv_gain;



	assert(ch);

	if (magic_fails(ch, victim))
		return;

	lv_gain = dice(3, 8) + 1;

	if (GET_MOVE(victim) + lv_gain > li1600_move_limit(victim)) {
		GET_MOVE(victim) = li1600_move_limit(victim);
	}
	else {
		GET_MOVE(victim) += lv_gain;
	}

	return;

}				/* END OF spell_vigorize_major() */

void spell_aid(sbyte level, struct char_data * ch,
	         struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;



	assert(ch &&
	       (victim || obj));
	assert((level >= 0) &&
	       (level <= NPC_LEV));

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_AID)) {
		send_to_char("nothing happens.\r\n", ch);
		return;
	}

	send_to_char("You feel better.\n\r", victim);
	af.type = SPELL_AID;
	af.duration = GET_LEVEL(ch);
	af.modifier = GET_LEVEL(ch);
	af.location = APPLY_HIT;
	af.bitvector = 0;
	ha1300_affect_to_char(victim, &af);

	return;

}				/* END OF spell_() */

void spell_silence(sbyte level, struct char_data * ch,
		     struct char_data * victim, struct obj_data * obj)
{
	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(ch && victim);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	if (saves_spell(ch, victim, SAVING_SPELL)) {
		send_to_char("You felt a brief tingle\r\n", victim);
		return;
	}

	af.type = SPELL_SILENCE;
	if (GET_INT(victim))
		af.duration = MAXV(2, 2 * 18 / GET_INT(victim));
	else
		af.duration = 2 * 18;

	af.modifier = 0;
	af.location = 0;
	af.bitvector = AFF_SILENCE;
	ha1300_affect_to_char(victim, &af);

	send_to_char("You've lost the ability to make a sound!\r\n", victim);
	act("$N has been silenced!", FALSE, ch, 0, victim, TO_CHAR);
	act("$N has been silenced!", FALSE, ch, 0, victim, TO_NOTVICT);
	return;

}				/* END OF spell_silence() */

void spell_dispel_silence(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj)
{



	if (magic_fails(ch, victim))
		return;

	/* if (IS_AFFECTED(victim, AFF_SILENCE)) { send_to_char("Nothing
	 * happens.\r\n", ch); return; } */
	/* bingo (working on improving spell) if (ch == victim) { if
	 * (IS_AFFECTED(victim, AFF_SILENCE)) { send_to_char("You wave your
	 * hands and dispel the silence upon you!\r\n", ch);
	 * ha1350_affect_from_char(victim, SPELL_SILENCE); return; } return; } */

	if (IS_AFFECTED(victim, AFF_SILENCE)) {
		act("You don't feel as quiet.", TRUE, victim, 0, 0, TO_VICT);
		act("$n glows white.", TRUE, victim, 0, 0, TO_ROOM);
		act("$n can speak again.", TRUE, victim, 0, 0, TO_ROOM);
		ha1350_affect_from_char(victim, SPELL_SILENCE);
	}
	else {
		send_to_char("How come you glowed but still feel quiet?\r\n", victim);
		act("$n still can't speak.", TRUE, victim, 0, 0, TO_NOTVICT);
	}

	return;

}				/* END OF spell_dispel_silence() */

void spell_dispel_hold(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj)
{



	if (magic_fails(ch, victim))
		return;

	if (!ha1375_affected_by_spell(victim, SPELL_HOLD_PERSON)) {
		send_to_char("Nothing happens.\r\n", ch);
		return;
	}

	act("$n glows green.", TRUE, victim, 0, 0, TO_ROOM);
	ha1350_affect_from_char(victim, SPELL_HOLD_PERSON);
	if (ha1375_affected_by_spell(victim, SPELL_HOLD_PERSON)) {
		send_to_char("You can move again.\r\n", ch);
		act("$n can move again.", TRUE, victim, 0, 0, TO_ROOM);
	}
	else {
		send_to_char("How come you glowed but still feel held?\r\n", victim);
		act("$n still can't move.", TRUE, victim, 0, 0, TO_NOTVICT);
	}

	return;

}				/* END OF spell_dispel_hold_person() */

void spell_block_summon(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj)
{

	struct affected_type af;



	assert(victim);
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_BLOCK_SUMMON))
		return;

	af.type = SPELL_BLOCK_SUMMON;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_BLOCK_SUMMON;

	ha1300_affect_to_char(victim, &af);
	send_to_char("The world seems more solid.\n\r", victim);
	act("$n glows indigo for a moment.", FALSE, victim, 0, 0, TO_ROOM);
	return;

}				/* END OF spell_block_summon() */

void spell_smite(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(victim && ch);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = 360;

	/* HURT YOURSELF? */
	if (GET_ALIGNMENT(ch) < -100) {
		if (saves_spell(ch, ch, SAVING_SPELL))
			dam >>= 1;
		act("$n envelops $eself in a towering column of flames!",
		    TRUE, ch, 0, 0, TO_NOTVICT);
		DAMAGE(ch, ch, dam, TYPE_UNDEFINED);

		return;
	}

	if (GET_ALIGNMENT(victim) < 0) {
		if (saves_spell(ch, victim, SAVING_SPELL))
			dam >>= 1;
		DAMAGE(ch, victim, dam, SPELL_SMITE);
		return;
	}

	act("$n is protectecd by $s holy ways.",
	    TRUE, victim, 0, 0, TO_NOTVICT);

	return;

}				/* END OF spell_smite() */


void kill_off_animate_dead_mob(struct char_data * ch, struct char_data * mob)
{



	hit(mob, ch, TYPE_UNDEFINED);
	if (ch->specials.fighting)
		ft1400_stop_fighting(ch, 0);
	if (mob->specials.fighting)
		ft1400_stop_fighting(mob, 0);

	act("$N fades into dust!", FALSE, ch, 0, mob, TO_ROOM);
	act("$N fades into dust!", FALSE, ch, 0, mob, TO_CHAR);

	ha3000_extract_char(mob, END_EXTRACT_BY_DEAD_MOB);
	return;

}				/* END OF kill_off_animate_dead_mob() */


void spell_animate_dead(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj)
{

	struct char_data *mob;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int lv_mob, real_num;



	if (!obj) {
		send_to_char("What was that you wanted to animate?\r\n", ch);
		return;
	}

	/* does it have a mob number? */
	if (!obj->action_description) {
		send_to_char("You can't animate that.\r\n", ch);
		return;
	}

	/* is it numeric */
	if (is_number(obj->action_description)) {
		lv_mob = atoi(obj->action_description);
	}
	else {
		send_to_char("You can't animate that!\r\n", ch);
		return;
	}

	real_num = db8100_real_mobile(lv_mob);
	if (real_num < 0) {
		send_to_char("You fail to make mental contact.\r\n", ch);
		return;
	}

	send_to_char("The dead wish to remain dead today,\r\n", ch);
	send_to_char("maybe they'll come and work for you tomorrow.\r\n", ch);
	send_to_char("Or perhaps if you got something to offer for them\r\n", ch);
	send_to_char("I mean you have to agree here that there's no way\r\n", ch);
	send_to_char("in hell that a great corpse like that is going to\r\n", ch);
	send_to_char("help a puny (no offense intended) mortal like yourself.\r\n", ch);
	send_to_char("So move along, let the corpse rest in peace.\r\n", ch);
	return;

	ha2700_extract_obj(obj);

	mob = db5000_read_mobile(real_num, REAL);
	GET_START(mob) = world[ch->in_room].number;
	GET_LEVEL(mob) = (GET_LEVEL(mob) + 1) / 2;
	GET_GOLD(mob) = 0;

	if (mob->player.short_descr) {
		strcpy(buf, GET_MOB_NAME(mob));
		free(mob->player.short_descr);
	}
	else {
		strcpy(buf, " blob");
	}

	if (mob->player.long_descr) {
		free(mob->player.long_descr);
	}
	sprintf(buf2, "The animated corpse of %s is decaying here.  ",
		buf);
	mob->player.long_descr = strdup(buf2);

	sprintf(buf2, "The animated corpse of %s", buf);
	mob->player.short_descr = strdup(buf2);

	ha1600_char_to_room(mob, ch->in_room);
	GET_GOLD(mob) = 0;
	GET_EXP(mob) = 0;

	act("$n calls on demonic powers as the corpse of $N climbs to its feet.",
	    TRUE, ch, 0, mob, TO_ROOM);
	act("You have revived $N!", FALSE, ch, 0, mob, TO_CHAR);

	if (ha4000_circle_follow(mob, ch)) {
		send_to_char("Sorry, following in circles is not allowed.\n\r",
			     ch);
		kill_off_animate_dead_mob(ch, mob);
		return;
	}

	if (mob->master)
		ha4100_stop_follower(mob, END_FOLLOW_BY_ANIMATE_DEAD_SPELL);

	if (ha9950_maxed_on_followers(ch, mob, 0)) {
		send_to_char("You have too many followers.\r\n", ch);
		kill_off_animate_dead_mob(ch, mob);
		return;
	}

	ha4300_add_follower(mob, ch);

	SET_BIT(mob->specials.affected_by, AFF_CHARM);

	act("$n brought you back from the dead?", TRUE, ch, 0, mob, TO_VICT);
	send_to_char("You have a new follower!\r\n", ch);
	return;

}				/* END OF spell_animate_dead() */


void spell_black_burst(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	dam = dice(6, level) + level + 10;
	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	/* ALIGNMENT CHECK */
	if (GET_ALIGNMENT(victim) < GET_ALIGNMENT(ch)) {
		if (GET_ALIGNMENT(ch) < -850) {
			GET_ALIGNMENT(ch) = -1000;
		}
		else {
			GET_ALIGNMENT(ch) -= 250;
		}
	}			/* END OF alignment check */

	DAMAGE(ch, victim, dam, SPELL_BLACK_BURST);
	return;

}				/* END OF spell_black_burst() */


void spell_cremation(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = dice(MINV(level, 30), 10) + 30;
	if (saves_spell(ch, ch, SAVING_SPELL))
		dam >>= 1;

	DAMAGE(ch, victim, dam, SPELL_CREMATION);
	return;

	send_to_char("nothing happens.\r\n", ch);
	return;

}				/* END OF spell_cremation() */


void spell_mortice_flame(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = dice(11, level) + level;

	/* HURT YOURSELF? */
	if (GET_ALIGNMENT(ch) > 0) {
		if (saves_spell(ch, ch, SAVING_SPELL))
			dam >>= 1;
		act("You scream in agony as you are engulfed in black flames.",
		    TRUE, ch, 0, 0, TO_CHAR);
		act("$n screams in agony as black flames engulf $m.",
		    TRUE, ch, 0, 0, TO_NOTVICT);
		DAMAGE(ch, ch, dam, TYPE_UNDEFINED);
		return;
	}

	/* HURT VICTIM */
	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;
	DAMAGE(ch, victim, dam, SPELL_MORTICE_FLAME);

	return;

}				/* END OF spell_mortice_flame() */


void spell_firelance(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = dice(level, 18) + (4 * level);

	if (number(1, 26) == 1 &&
	    GET_CLASS(ch) == CLASS_MAGIC_USER &&
	    GET_LEVEL(ch) >= IMO_LEV) {
		/* Random critical fire damage for mages, Bingo 9 - 19 - 01 */
		dam *= 2;
		send_to_char("&R*&rF&YL&rA&RM&YE &rR&RU&YS&rH&R*&n ", ch);
	}

	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	if (ch->in_room != victim->in_room) {
		send_to_char("Your spell is unable to reach such a far distance...\n\r", ch);
		return;
	}

	DAMAGE(ch, victim, dam, SPELL_FIRELANCE);

	return;

}				/* END OF spell_firelance() */


void spell_firestorm(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj)
{

	int dam, adj_dam;
	int do_attack;
	struct char_data *tmp_victim, *temp, *leader;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

	dam = dice(level, 20) + 2 * level;

	if (number(1, 26) == 1 &&
	    GET_CLASS(ch) == CLASS_MAGIC_USER &&
	    GET_LEVEL(ch) >= IMO_LEV) {
		/* Random critical fire damage for mages, Bingo 9 - 19 - 01 */
		dam *= 2;
		send_to_char("&R*&rF&YL&rA&RM&YE &rR&RU&YS&rH&R*&n ", ch);
	}

	send_to_char("The room is engulfed in flames!\n\r", ch);
	act("$n engulfs the room in flames!", TRUE, ch, 0, 0, TO_ROOM);

	if (ch->master)
		leader = ch->master;
	else
		leader = ch;

	for (tmp_victim = world[ch->in_room].people;
	     tmp_victim; tmp_victim = temp) {
		temp = tmp_victim->next_in_room;
		do_attack = TRUE;
		if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK FREE MOBS */
		    IS_NPC(tmp_victim) &&
		    !IS_AFFECTED(tmp_victim, AFF_CHARM))
			do_attack = FALSE;
		if (ch == tmp_victim)	/* don't attack ourselves */
			do_attack = FALSE;
		if (ch->master == tmp_victim)	/* don't attack our leader */
			do_attack = FALSE;
		if (tmp_victim->master == leader)	/* don't attack
							 * followers */
			do_attack = FALSE;
		if (IS_AFFECTED(tmp_victim, AFF_CHARM))
			do_attack = FALSE;
		if (IS_PC(ch) && IS_PC(tmp_victim))
			do_attack = FALSE;
		if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
			do_attack = FALSE;
		if (magic_fails(ch, tmp_victim))
			do_attack = FALSE;
		if (GET_LEVEL(tmp_victim) < 4)	/* don't attack newbies */
			do_attack = FALSE;
		if (IS_SET(world[tmp_victim->in_room].room_flags, RM1_NOPKILL) &&
		    IS_PC(tmp_victim))
			do_attack = FALSE;
		if (IS_SET(world[tmp_victim->in_room].room_flags, RM1_NOMKILL) &&
		    IS_NPC(tmp_victim))
			do_attack = FALSE;
		if (IS_SET(GET_ACT1(tmp_victim), PLR1_NOKILL))
			do_attack = FALSE;

		if (magic_fails(ch, tmp_victim))
			do_attack = FALSE;

		if (do_attack == TRUE) {
			victim = tmp_victim;
			CHECK_FOR_PK();
			if (saves_spell(ch, tmp_victim, SAVING_SPELL))
				adj_dam = dam >> 1;
			else
				adj_dam = dam;
			DAMAGE(ch, tmp_victim, adj_dam, SPELL_FIREBALL);

		}		/* END OF do_attack */

	}			/* END OF for loop */

}				/* END OF spell_firestorm() */


void spell_divine_retribution(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj)
{

	int dam;
	//int cmd = CMD_CAST;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = MINV(500, dice(5, 100) + level);

	/* HURT YOURSELF? */
	if (GET_ALIGNMENT(ch) < 500) {
		if (saves_spell(ch, ch, SAVING_SPELL))
			dam >>= 1;
		send_to_char("The gods punish you for your crimes!\r\n", ch);
		act("You feel a charge run though the air as $n doubles up in agony and is engulfed in an almost blinding light.",
		    TRUE, ch, 0, 0, TO_NOTVICT);
		DAMAGE(ch, ch, dam, TYPE_UNDEFINED);
		return;
	}

	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;
	DAMAGE(ch, victim, dam, SPELL_DIVINE_RETRIBUTION);

	return;

}				/* END OF spell_divine_retribution() */
