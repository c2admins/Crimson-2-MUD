/* m3 */
/* gv_location: 8501-9000 */
/* ********************************************************************
*  file: magic3.c , Implementation of spells.          Part of DIKUMUD *
*  Usage : The actual effect of magic.                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#define MAX_LIGHT_CHARGE 100

void spell_mirror_self(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){


	char buf[MAX_STRING_LENGTH];
	struct char_data *mob;
	int r_num;


	send_to_char("You see a copy of yourself appearing in front of you!\r\n", ch);
	act("You here a strange sound as $n duplicates himself.", FALSE, ch, 0, 0, TO_ROOM);

	r_num = db8100_real_mobile(908);
	mob = db5000_read_mobile(r_num, REAL);
	GET_START(mob) = world[ch->in_room].number;

	if (GET_LEVEL(ch) >= IMO_SPIRIT) {
		SET_BIT(GET_ACT1(mob), PLR1_NOCHARM);
	}

	/* Set keywords */
	sprintf(buf, "mob %s", GET_REAL_NAME(ch));
	free(mob->player.name);
	mob->player.name = strdup(buf);

	/* Set short_desc */
	sprintf(buf, "%s", GET_REAL_NAME(ch));
	free(mob->player.short_descr);
	mob->player.short_descr = strdup(buf);

	/* Set long_desc */
	sprintf(buf, "%s is standing here.  ", GET_TITLE(ch));
	free(mob->player.long_descr);
	mob->player.long_descr = strdup(buf);

	/* Set description */
	sprintf(buf, "%s\n\r", ch->player.description);
	free(mob->player.description);
	mob->player.description = strdup(buf);

	/* Set Hitpoints */
	mob->points.max_hit = (100 + number(1, 10) - 20) * GET_HIT_LIMIT(ch) / 400;
	mob->player.weight = 1;
	mob->points.hit = (100 + number(1, 10) - 20) * GET_HIT_LIMIT(ch) / 400;

	/* Set Damageroll */
	mob->specials.damnodice = 2;
	mob->specials.damsizedice = 2 * GET_LEVEL(ch);
	mob->points.damroll = 2 * GET_LEVEL(ch);

	/* Set general stuff */
	GET_GOLD(mob) = 0;
	GET_EXP(mob) = 0;
	GET_SEX(mob) = GET_SEX(ch);
	GET_ALIGNMENT(mob) = GET_ALIGNMENT(ch);
	GET_AC(mob) = GET_AC(ch);
	GET_LEVEL(mob) = GET_LEVEL(ch) / 2;
	GET_HITROLL(mob) = GET_HITROLL(ch) / 2;
	GET_STR(mob) = GET_STR(ch) / 2;
	GET_REAL_STR(mob) = GET_REAL_STR(ch) / 2;
	GET_DEX(mob) = GET_DEX(ch) / 2;
	GET_REAL_DEX(mob) = GET_REAL_DEX(ch) / 2;
	GET_INT(mob) = GET_INT(ch) / 2;
	GET_REAL_INT(mob) = GET_REAL_INT(ch) / 2;
	GET_WIS(mob) = GET_WIS(ch) / 2;
	GET_REAL_WIS(mob) = GET_REAL_WIS(ch) / 2;
	GET_CON(mob) = GET_CON(ch) / 2;
	GET_REAL_CON(mob) = GET_REAL_CON(ch) / 2;
	GET_CHA(mob) = GET_CHA(ch) / 2;
	GET_REAL_CHA(mob) = GET_REAL_CHA(ch) / 2;

	/* Move mob to character */
	ha1600_char_to_room(mob, ch->in_room);


	return;

}				/* END OF spell_mirror_self() */

void spell_recharge_light(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){

	int lv_charge;



	if (!ch) {
		main_log("ERROR: spell_recharge_light called without ch.");
		return;
	}

	if (!obj) {
		main_log("ERROR: spell_recharge_light called without obj.");
		return;
	}

	/* IF ITS NOT A LIGHT */
	if (obj->obj_flags.type_flag != ITEM_LIGHT &&
	    obj->obj_flags.type_flag != ITEM_QSTLIGHT) {
		act("You can't recharge a $p!",
		    FALSE, ch, obj, 0, TO_CHAR);
		return;
	}

	lv_charge = dice(3, GET_LEVEL(ch));
	obj->obj_flags.value[VALUE_LIGHT_CHARGES] += lv_charge;

	if (obj->obj_flags.value[VALUE_LIGHT_CHARGES] > MAX_LIGHT_CHARGE ||
	    number(1, 20) == 20) {
		act("$p takes on an angry red glow and explodes!",
		    TRUE, ch, obj, 0, TO_CHAR);
		act("$n's $p takes on an angry red glow and explodes!",
		    TRUE, ch, obj, 0, TO_ROOM);
		ha2700_extract_obj(obj);
		DAMAGE(ch, ch, lv_charge, SPELL_RECHARGE_LIGHT);
		return;
	}

	act("$p glows.",
	    TRUE, ch, obj, 0, TO_CHAR);
	act("$n's $p glows.",
	    TRUE, ch, obj, 0, TO_ROOM);
	return;

}				/* END OF spell_recharge_light() */

void spell_recharge_wand(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj){

	int lv_charge;



	if (!ch) {
		main_log("ERROR: spell_recharge_wand called without ch.");
		return;
	}

	if (!obj) {
		main_log("ERROR: spell_recharge_wand called without obj.");
		return;
	}

	/* IF ITS NOT A WAND */
	if (obj->obj_flags.type_flag != ITEM_WAND) {
		act("You can't recharge a $p!",
		    FALSE, ch, obj, 0, TO_CHAR);
		return;
	}

	lv_charge = dice(1, 3);
	obj->obj_flags.value[VALUE_CURR_CHARGE] += lv_charge;

	if (obj->obj_flags.value[VALUE_CURR_CHARGE] > obj->obj_flags.value[VALUE_MAX_CHARGES]) {
		int boom, goboom;
		boom = number(1, 10);
		goboom = 0;

		if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
			if (boom == 10)
				goboom = 1;
		}
		else {
			if (boom > 7)
				goboom = 1;
		}
		if (goboom == 1) {
			act("$p takes on an angry red glow and explodes!",
			    TRUE, ch, obj, 0, TO_CHAR);
			act("$n's $p takes on an angry red glow and explodes!",
			    TRUE, ch, obj, 0, TO_ROOM);
			if (ch->equipment[HOLD] == obj)
				ha1930_unequip_char(ch, HOLD);
			else
				ha2700_extract_obj(obj);
			DAMAGE(ch, ch, lv_charge, SPELL_RECHARGE_WAND);
			return;
		} //end if
	} //end
		else


		act("$p glows.", TRUE, ch, obj, 0, TO_CHAR);
	act("$n's $p glows.", TRUE, ch, obj, 0, TO_ROOM);
	return;


}				/* END OF spell_recharge_wand() */

void spell_venom_blade(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	int i;



	if (!ch) {
		main_log("ERROR: spell_venom_blade called without ch.");
		return;
	}

	if (!obj) {
		main_log("ERROR: spell_venom_blade called without obj.");
		return;
	}

	if (GET_ITEM_TYPE(obj) != ITEM_WEAPON) {
		send_to_char("You can only poison weapons.\r\n", ch);
		return;
	}

	if (IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC)) {
		send_to_char("You can't poison magical items.\r\n", ch);
		return;
	}

	/* IF WEAPON HAS ANY EXISTING APPLYs, GET OUT */
	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		if (obj->affected[i].location != APPLY_NONE) {
			send_to_char("The weapon resist your enchantment!\r\n", ch);
			return;
		}
	}

	if (IS_SET(obj->obj_flags.value[0], WEAPON_POISONOUS)) {
		send_to_char("You fail to make the item more poisonous.\r\n", ch);
		return;
	}

	SET_BIT(obj->obj_flags.flags1, OBJ1_MAGIC);
	SET_BIT(obj->obj_flags.value[0], WEAPON_POISONOUS);

	act("$p briefly glows a sickly green.",
	    TRUE, ch, obj, 0, TO_CHAR);
	act("$n's $p briefly glows a sickly green.",
	    TRUE, ch, obj, 0, TO_ROOM);
	return;

}				/* END OF spell_venom_blade() */

void spell_irresistable_dance(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){



	send_to_char("Nothing happens\r\n", ch);
	return;

}				/* END OF spell_irresistable_dance() */

void spell_wizard_lock(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){



	/* this skill is located somewhere else */
	/* i'll remove it later */

	send_to_char("Nothing happens\r\n", ch);
	return;

}				/* END OF spell_wizard_lock() */

void spell_dancing_sword(sbyte level, struct char_data * ch, struct char_data * victim, struct obj_data * obj){

	char buf[MAX_STRING_LENGTH];	/* Bingo 03-01-01 */
	struct affected_type af;
	struct char_data *mob;
	int i, r_num;

	if (obj->obj_flags.type_flag != ITEM_WEAPON ||
	    obj->obj_flags.type_flag != ITEM_QSTWEAPON) {

	sprintf(buf, "You transform %s into a mob!\n\r", GET_OSDESC(obj));
	send_to_char(buf, ch);

	r_num = db8100_real_mobile(909);
	mob = db5000_read_mobile(r_num, REAL);
	GET_START(mob) = world[ch->in_room].number;

	sprintf(buf, "mob %s", obj->name);
	free(mob->player.name);
	mob->player.name = strdup(buf);

	sprintf(buf, "%s", GET_OSDESC(obj));
	free(mob->player.short_descr);
	mob->player.short_descr = strdup(buf);

	sprintf(buf, "%s is here suspended in mid-air swaying back and forth.  ", GET_OSDESC(obj));
	free(mob->player.long_descr);
	mob->player.long_descr = strdup(buf);

	sprintf(buf, "%s seems to be alive.  It floats around on its own.\n\r", GET_OSDESC(obj));
	free(mob->player.description);
	mob->player.description = strdup(buf);

	SET_BIT(GET_ACT2(mob), PLR2_MOB_AI);

	SET_BIT(mob->specials.affected_by, AFF_DARKSIGHT);
	SET_BIT(mob->specials.affected_by, AFF_DETECT_INVISIBLE);

	if (IS_SET(obj->obj_flags.value[0], WEAPON_POISONOUS)) {
		SET_BIT(GET_ACT1(mob), PLR1_POISONOUS);
	}

	mob->specials.damnodice = obj->obj_flags.value[1] * 5;
	mob->specials.damsizedice = obj->obj_flags.value[2] * 5;

	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		if (obj->affected[i].location == APPLY_DAMROLL) {
			mob->points.damroll = obj->affected[i].modifier * 15;
		}
		if (obj->affected[i].location == APPLY_HITROLL) {
			GET_HITROLL(mob) = obj->affected[i].modifier * 20;
		}
	}

	if (GET_HITROLL(mob) < 20) {
		GET_HITROLL(mob) = 20;
	}
	if (mob->points.damroll < 15) {
		mob->points.damroll = 15;
	}

	GET_GOLD(mob) = obj->obj_flags.cost;
	GET_EXP(mob) = obj->obj_flags.cost / 100;
	GET_SEX(mob) = 0;
	GET_ALIGNMENT(mob) = GET_ALIGNMENT(ch);

	if (IS_OBJ_STAT(obj, OBJ1_MINLVL10)) {
		GET_LEVEL(mob) = 10;
	}
	else if (IS_OBJ_STAT(obj, OBJ2_MINLVL15)) {
		GET_LEVEL(mob) = 15;
	}
	else if (IS_OBJ_STAT(obj, OBJ1_MINLVL20)) {
		GET_LEVEL(mob) = 20;
	}
	else if (IS_OBJ_STAT(obj, OBJ2_MINLVL25)) {
		GET_LEVEL(mob) = 25;
	}
	else if (IS_OBJ_STAT(obj, OBJ1_MINLVL30)) {
		GET_LEVEL(mob) = 30;
	}
	else if (IS_OBJ_STAT(obj, OBJ2_MINLVL35)) {
		GET_LEVEL(mob) = 35;
	}
	else if (IS_OBJ_STAT(obj, OBJ1_MINLVL41)) {
		GET_LEVEL(mob) = 41;
	}
	else if (IS_OBJ_STAT2(obj, OBJ2_MINLVL42)) {
		GET_LEVEL(mob) = 42;
	}
	else {
		GET_LEVEL(mob) = number(1, 9);
	}

	mob->player.weight = 1;
	mob->points.max_hit = GET_HIT_LIMIT(ch) / 2 + (GET_LEVEL(mob) * number(1, GET_LEVEL(ch)));
	mob->points.hit = GET_HIT_LIMIT(mob);
	mob->points.max_mana = GET_MANA_LIMIT(ch) / 2 + (GET_LEVEL(mob) * number(1, GET_LEVEL(ch)));
	mob->points.mana = GET_MANA_LIMIT(mob);

	GET_REAL_STR(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_STR(mob) = GET_REAL_STR(mob);
	GET_REAL_DEX(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_DEX(mob) = GET_REAL_DEX(mob);
	GET_REAL_INT(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_INT(mob) = GET_REAL_INT(mob);
	GET_REAL_WIS(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_WIS(mob) = GET_REAL_WIS(mob);
	GET_REAL_CON(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_CON(mob) = GET_REAL_CON(mob);
	GET_REAL_CHA(mob) = GET_LEVEL(ch) / 3 + number(-5, 10);
	GET_CHA(mob) = GET_REAL_CHA(mob);

	ha1600_char_to_room(mob, ch->in_room);

	ha4300_add_follower(mob, ch);
	af.type = SPELL_CHARM_PERSON;

	if (GET_INT(mob))
	 af.duration = 24 * 18 / GET_INT(mob);
	else
	 af.duration = 20;

	af.modifier = 0;
	af.location = 0;
	af.bitvector = AFF_CHARM;
	ha1300_affect_to_char(mob, &af);

	if (IS_SET(obj->obj_flags.value[0], WEAPON_FLAME_ATTACK)) {
		mob->skills[SPELL_FIRE_BREATH].learned = 25 + number(25, 74);
	}
	if (IS_SET(obj->obj_flags.value[0], WEAPON_ICE_ATTACK)) {
		mob->skills[SPELL_FROST_BREATH].learned = 25 + number(25, 74);
	}
	if (IS_SET(obj->obj_flags.value[0], WEAPON_ELEC_ATTACK)) {
		mob->skills[SPELL_LIGHTNING_BREATH].learned = 25 + number(25, 74);
	}
	if (IS_SET(obj->obj_flags.value[0], WEAPON_DRAIN_MANA)) {
		mob->skills[SPELL_ENERGY_DRAIN].learned = 25 + number(25, 74);
	}
	if (IS_SET(obj->obj_flags.value[0], WEAPON_BLIND)) {
		mob->skills[SPELL_BLINDNESS].learned = 25 + number(25, 74);
	}

	//recycling mob move adjustment to record objs virtual number
		races[GET_RACE(mob)].adj_move = obj_index[obj->item_number].virtual;

	ha2700_extract_obj(obj);

	return;

	} /* END of IS WEAPON */
}				/* END OF spell_dancing_sword() */

void spell_demonic_aid(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	if (IS_CASTED_ON(victim, SPELL_DEMONIC_AID))
		return;

	if (magic_fails(ch, victim))
		return;

	act("$n traces out a rune pattern on $s forehead and stiffens suddenly.\r\n", TRUE, victim, 0, 0, TO_NOTVICT);
	act("You trace out the rune on your forehead and feel the power surge into you.\r\n", TRUE, victim, 0, 0, TO_CHAR);

	af.type = SPELL_DEMONIC_AID;
	af.duration = level;
	af.modifier = 1;
	af.location = APPLY_STR;
	af.bitvector = 0;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_INT;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_WIS;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_DEX;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_CON;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_CHA;
	af.modifier = -1;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MANA;
	af.modifier = (level / 2);
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_HIT;
	af.modifier = (level / 2);
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MOVE;
	af.modifier = (level / 2);
	ha1300_affect_to_char(victim, &af);

	return;
}				/* END OF spell_demonic_aid() */

void spell_clawed_hands(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){



	send_to_char("You feel your hands turn into gigantic claws, so big\r\n", ch);
	send_to_char("they could probably kill the all mighty vatang in a\r\n", ch);
	send_to_char("single lash.\r\n\r\nYou scratch your nose.\r\n\r\nYour hands return to normal.\r\n", ch);
	return;

}				/* END OF spell_clawed_hands() */

void spell_flying_disk(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){



	/* this skill has been removed (fly) */
	/* i'll remove it later */

	send_to_char("Nothing happens\r\n", ch);
	return;

}				/* END OF spell_flying_disk() */

void spell_strength(sbyte level, struct char_data * ch,
		      struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_strength called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_str - GET_STR(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you any stronger.\r\n");
		else
			sprintf(buf, "You fail to make %s any stronger.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_STRENGTH;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_STR;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel stronger.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically strengthen $N.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_strength() */

void spell_intelligence(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_intelligence called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_int - GET_INT(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you any smarter.\r\n");
		else
			sprintf(buf, "You fail to make %s any smarter.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_INTELLIGENCE;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_INT;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel smarter.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically enhance $N's intelligence.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_intelligence() */

void spell_wisdom(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_wisdom called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_wis - GET_WIS(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you any wiser.\r\n");
		else
			sprintf(buf, "You fail to make %s any wiser.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_WISDOM;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_WIS;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel wiser.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically enhance $N's wisdom.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_wisdom() */

void spell_dexterity(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_dexterity called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_dex - GET_DEX(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you more dexterous.\r\n");
		else
			sprintf(buf, "You fail to make %s more nimble.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_DEXTERITY;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_DEX;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel more nimble.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically enhance $N's dexterity.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_dexterity() */

void spell_constitution(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_constitution called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_con - GET_CON(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you more constitutiony.\r\n");
		else
			sprintf(buf, "You fail to make %s more constitutiony.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_CONSTITUTION;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_CON;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel more constitutiony.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically enhance $N's constitution.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_constitution() */

void spell_charisma(sbyte level, struct char_data * ch,
		      struct char_data * victim, struct obj_data * obj){

	int lv_diff_to_max;
	char buf[MAX_STRING_LENGTH];
	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_charisma called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	lv_diff_to_max = races[GET_RACE(victim)].max_cha - GET_CHA(victim);

	if (lv_diff_to_max < 1) {
		if (ch == victim)
			sprintf(buf, "Hrmmm. It doesn't appear to make you more attractive.\r\n");
		else
			sprintf(buf, "You fail to make %s more attractive.\r\n",
				GET_REAL_NAME(victim));

		send_to_char(buf, ch);
		return;
	}

	af.type = SPELL_CHARISMA;
	af.duration = level;
	if (GET_LEVEL(ch) > PK_LEV) {
		af.modifier = lv_diff_to_max;
	}
	else {
		if (GET_LEVEL(ch) > 17) {
			af.modifier = 2;
		}
		else {
			af.modifier = 1;
		}
	}
	af.modifier = MINV(af.modifier, lv_diff_to_max);
	af.location = APPLY_CHA;
	af.bitvector = 0;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You feel more attractive.", FALSE, victim, 0, 0, TO_CHAR);
	if (victim != ch) {
		act("You magically enhance $N's attractiveness.",
		    FALSE, ch, 0, victim, TO_CHAR);
	}

	return;

}				/* END OF spell_charisma() */

void spell_portal(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj){

	int lv_orig_room, lv_dest_room, lv_portal_num, lv_spell_time, rc;
	struct obj_data *lv_obj;



	if (!ch) {
		main_log("ERROR: spell_portal called without ch.");
		return;
	}

	if (!victim) {
		main_log("ERROR: spell_portal called without victim.");
		return;
	}

	if (level < 99) {
		if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
			send_to_char("Sorry, you can't create a portal in jail.\r\n", ch);
			act("$n draws archaic runes in the air.",
			    FALSE, ch, 0, 0, TO_ROOM);
			return;
		}

		if (GET_LEVEL(ch) < IMO_IMP &&
		 IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
			send_to_char("You draw the proper glowing runes, but nothing happens.\r\n", ch);
			act("$n draws several archaic runes in the air.",
			    FALSE, ch, 0, 0, TO_ROOM);
			return;
		}
	}

	lv_orig_room = ch->in_room;

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

	if (IS_PC(victim) &&
	    GET_LEVEL(victim) > PK_LEV &&
	    GET_LEVEL(ch) < IMO_IMM &&
	    level < 99) {
		send_to_char("The magic fails!\r\n", ch);
		return;
	}

	/* SKILL CHECK FOR CORRECT ROOM */
	lv_dest_room = victim->in_room;
	rc = wz1500_did_we_make_teleport(ch, ch->in_room,
					 lv_dest_room,
					 GET_SKILL(ch, SPELL_PORTAL),
					 0);
	if (rc != TELEPORT_WORKED) {
		if (rc == TELEPORT_FAILED_ZCORD) {
			return;
		}
		if (rc == TELEPORT_FAILED_MAJOR) {
			lv_dest_room = ma1000_do_get_random_room(ch, BIT0);
		}
		else {
			lv_dest_room = ma1000_do_get_random_room(ch, 0);
		}
	}			/* END OF TELEPORT DIDN'T WORK */

	if (lv_dest_room < 1 ||
	    lv_dest_room > top_of_world) {
		send_to_char("A flickering force halts astral travel.\r\n", ch);
		return;
	}

	rc = li3000_is_blocked_from_room(ch, lv_dest_room, BIT0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	/* ARE WE GOING TO THE SAME ROOM? */
	if (ch->in_room == lv_dest_room) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			act("You draw some archaic runes and the resulting magic implodes sending you somewhere!",
			    FALSE, ch, 0, 0, TO_CHAR);
			spell_elsewhere(GET_LEVEL(ch), ch, ch, 0);
		}
		else {
			act("You're Immortal and resist the backlash.",
			    FALSE, ch, 0, 0, TO_CHAR);
		}
		act("$n draws archaic runes and the resulting magic implodes!",
		    FALSE, ch, 0, 0, TO_ROOM);
		return;
	}


	if (gv_port == ZONING_PORT) {
		if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[lv_dest_room].zone].lord) &&
		    GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to go into that zone!", ch);
			return;
		}
	}

	act("You draw some archaic runes and a portal appears.",
	    FALSE, ch, 0, 0, TO_CHAR);
	act("$n draws archaic runes and a portal appears.",
	    FALSE, ch, 0, 0, TO_ROOM);

	lv_portal_num = db8200_real_object(OBJ_PORTAL);
	if (lv_portal_num < 1) {
		main_log("ERROR: unable to load obj portal in magic3.");
		return;
	}

	if (GET_LEVEL(ch) > IMO_IMM)
		lv_spell_time = 100;
	else
		lv_spell_time = MAXV((GET_LEVEL(ch) / 10 + 1), 1);


	if (level == 99)
		lv_spell_time = 1;

	/* LOAD A SEND PORTAL */
	lv_obj = db5100_read_object(lv_portal_num, REAL);
	lv_obj->obj_flags.value[VALUE_PORTAL_ROOM] = lv_dest_room;
	lv_obj->obj_flags.timer = lv_spell_time;
	ha2100_obj_to_room(lv_obj, ch->in_room);

	/* LOAD A RETURN PORTAL */
	co3300_send_to_room("A glowing portal suddenly appears.\r\n",
			    lv_dest_room);
	lv_obj = db5100_read_object(lv_portal_num, REAL);
	lv_obj->obj_flags.value[VALUE_PORTAL_ROOM] = ch->in_room;
	lv_obj->obj_flags.timer = lv_spell_time;
	ha2100_obj_to_room(lv_obj, lv_dest_room);

	return;

}				/* END OF spell_portal() */

void spell_hemmorage(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj){

	struct affected_type af;
	//int cmd = CMD_CAST;


	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	if (IS_AFFECTED2(victim, AFF_HEMMORAGE))
		return;

	if (saves_spell(ch, victim, SAVING_PARA)) {
		send_to_char("You felt a brief tingle.\r\n", victim);
		return;
	}

	af.type = SPELL_HEMMORAGE;
	af.duration = (level >> 2);
	af.modifier = (level / 5);
	af.location = APPLY_NONE;
	af.bitvector = AFF_HEMMORAGE;

	ha1300_affect_to_char(victim, &af);

	send_to_char("You feel a sharp pain.\n\r", victim);
	act("You see the pain in $N's eyes as you cast hemmorage.", FALSE, ch, 0, victim, TO_CHAR);


}				/* END OF spell_hemmorage() */

void spell_fear(sbyte level, struct char_data * ch,
		  struct char_data * victim, struct obj_data * obj){



	/* Under construction ;) */

	send_to_char("Nothing happens\r\n", ch);
	return;

}				/* END OF spell_fear() */

void spell_demon_flesh(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	if (!victim) {
		main_log("ERROR: spell_demon_flesh called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	if (IS_AFFECTED(victim, AFF_DEMON_FLESH)) {
		send_to_char("Nothing happens.", ch);
		if (ch != victim) {
			send_to_char("You feel a brief tingle.\r\n", victim);
		}
		return;
	}

	af.type = SPELL_DEMON_FLESH;
	af.duration = MAXV(1, level / 5);
	af.modifier = -5;
	af.location = APPLY_CHA;
	af.bitvector = AFF_DEMON_FLESH;
	ha1400_affect_join(victim, &af, TRUE, FALSE);

	act("You trace out a rune in the air and are engulfed in a",
	    FALSE, victim, 0, 0, TO_CHAR);
	act("feeling of euphoria as your body swells disgustingly.",
	    FALSE, victim, 0, 0, TO_CHAR);
	act("$n traces out a rune pattern in the air and $s body swells disgustingly.",
	    FALSE, ch, 0, 0, TO_ROOM);

	return;

}				/* END OF spell_demon_flesh() */

void spell_xxx3(sbyte level, struct char_data * ch,
		  struct char_data * victim, struct obj_data * obj){



	send_to_char("Nothing happens\r\n", ch);
	return;

}				/* END OF spell_xxx() */