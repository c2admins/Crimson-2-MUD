/* m4 */
/* gv_location: 9001-9500 */
/* ********************************************************************
*  file: magic4.c , Implementation of spells.          Part of DIKUMUD *
*  Usage : The actual effect of magic.                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

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


// declaration from magic2.c
void kill_off_gate_mob(struct char_data * ch, struct char_data * mob);


void spell_grangorns_curse(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){
	int xp;
	struct affected_type af;

	assert(victim && ch);
	assert((level >= 0) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	if (obj) {
		act("Sorry, you can't cast grangorns curse on items.\r\n", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if (magic_fails(ch, victim))
		return;

	if (saves_spell(ch, victim, SAVING_SPELL) ||
	    ha1375_affected_by_spell(victim, SPELL_GRANGORNS_CURSE))
		return;

	xp = number(level >> 1, level) * 1000;
	li2200_gain_exp(victim, -xp);

	af.type = SPELL_GRANGORNS_CURSE;
	af.duration = level * 5;
	af.modifier = -3;
	af.location = APPLY_HITROLL;
	af.bitvector = AFF_CURSE;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_AC;
	af.modifier = 40;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_PARA;
	af.modifier = 2;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_ROD;
	af.modifier = 2;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_PETRI;
	af.modifier = 2;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_BREATH;
	af.modifier = 2;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_SPELL;
	af.modifier = 2;	/* make it worse */
	ha1300_affect_to_char(victim, &af);

	act("$n is surrounded by an orange aura!", FALSE, victim, 0, 0, TO_ROOM);
	act("You feel very uncomfortable.", FALSE, victim, 0, 0, TO_CHAR);

}				/* End of spell_grangorns_curse() */


void spell_pestilence(sbyte level, struct char_data * ch, struct char_data * victim, struct obj_data * obj){

	int dam, xp;

	struct affected_type af;



	assert(victim && ch);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	if (magic_fails(ch, victim))
		return;

	if (obj) {
		act("Sorry, you can't use pestilence to poison items.\r\n", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	dam = dice(level, 9);
	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	xp = number(level >> 1, level) * 1000;
	li2200_gain_exp(victim, -xp);

	/* POISON PART */
	if (!saves_spell(ch, victim, SAVING_PARA)) {

		af.type = SPELL_POISON;
		af.duration = level;
		af.modifier = -2;
		af.location = APPLY_STR;
		af.bitvector = AFF_POISON;
		ha1400_affect_join(victim, &af, FALSE, FALSE);

		act("$n seems pestilenced!!", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You feel very sick!\n\r", victim);
	}

	DAMAGE(ch, victim, dam, SPELL_PESTILENCE);
	act("You choke as $n pestilences you.", FALSE, victim, 0, 0, TO_CHAR);
	act("$n starts choking in a strange way.", FALSE, victim, 0, 0, TO_ROOM);

}				/* END OF spell_pestilence() */


void spell_spiritual_transfer(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){

	char buf[MAX_STRING_LENGTH];




	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();

	if (magic_fails(ch, victim))
		return;

	if (IS_NPC(ch)) {
		send_to_char("If you can read this you are already switched.. Really smart..", ch);
		return;
	}

	if (ch == victim) {
		send_to_char("Feeling smart today huh?\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) >= IMO_IMM) {
		send_to_char("Sorry, You can't use this spell, please use switch instead\r\n", ch);
		return;
	}

	if (ch->desc->snoop.snoop_by) {
		ch->desc->snoop.snoop_by->desc->snoop.snooping = 0;
		send_to_char("Your snoop victim switched. Terminating snoop\r\n",
			     ch->desc->snoop.snoop_by);
		ch->desc->snoop.snoop_by = 0;
	}


	if (victim->desc || (!IS_NPC(victim))) {
		send_to_char("You can't do that, the body is already in use!\r\n", ch);
		return;
	}

	if (!IS_SET(victim->specials.act, PLR1_CLASS_ANIMAL)) {
		send_to_char("You can only transfer your spirit into animals.\r\n", ch);
		return;
	}

	if ((IS_SET(victim->specials.act, PLR1_CLASS_ANIMAL)) &&
	    (GET_LEVEL(ch) < GET_LEVEL(victim) - 2)) {
		send_to_char("This animal's spirit is too strong.\r\n", ch);
		return;
	}

	sprintf(buf, "You transfer your spirit to %s.\r\n",
		GET_NAME(victim));
	send_to_char(buf, ch);
	act("$n has transfered $s spirit into $N.\r\n", FALSE, ch, 0, victim, TO_ROOM);

	ch->desc->character = victim;
	ch->desc->original = ch;

	victim->desc = ch->desc;
	ch->desc = 0;

}				/* END OF spell_spiritual_transfer (); */

void spell_bind_souls(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));

	if (ha1375_affected_by_spell(victim, SPELL_BIND_SOULS)) {
		send_to_char("Nothing seems to happen.\n\r", ch);
		return;
	}

	if (magic_fails(ch, victim))
		return;

	af.type = SPELL_BIND_SOULS;
	af.duration = (level >> 1) + number(1, 10);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;
	ha1300_affect_to_char(ch, &af);

	ch->extra_mana = 0;

	act("$n starts being attached to other souls.", FALSE, victim, 0, 0, TO_ROOM);
	act("You feel other souls being attached to you.", FALSE, victim, 0, 0, TO_CHAR);
	return;

}				/* END OF spell_bind_souls() */

void spell_mordenkainens_sword(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){

	char buf[MAX_STRING_LENGTH];
	struct obj_data *tmp_obj;
	int r_num;



	if (magic_fails(ch, victim)) {
		send_to_char("A sword materializes in your hands!\r\n", ch);
		send_to_char("The swords structure is unstable, it falls apart.\r\n", ch);
		return;
	}

	send_to_char("A sword suddenly appears in your hands!\r\n", ch);
	act("$n makes some magical gestures and suddenly a sword appears", FALSE, ch, 0, 0, TO_ROOM);
	r_num = db8200_real_object(960);
	tmp_obj = db5100_read_object(r_num, REAL);

	sprintf(buf, "The sword of %s", GET_REAL_NAME(ch));
	str_free(tmp_obj->short_description);
	tmp_obj->short_description = strdup(buf);
	sprintf(buf, "obj sword %s", GET_REAL_NAME(ch));
	str_free(tmp_obj->name);
	tmp_obj->name = strdup(buf);
	tmp_obj->obj_flags.value[1] = 6;
	tmp_obj->obj_flags.value[2] = (level / 4);
	tmp_obj->obj_flags.value[3] = 3;
	tmp_obj->affected[0].location = APPLY_HITROLL;
	tmp_obj->affected[0].modifier = (level / 8 + 1);
	tmp_obj->affected[1].location = APPLY_DAMROLL;
	tmp_obj->affected[1].modifier = (level / 16 + 1);
	ha1700_obj_to_char(tmp_obj, ch);

	return;

}				/* END OF spell_mordenkainens_sword() */

void spell_call_lightning(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){

	int dam;
	//int cmd = CMD_CAST;

	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = dice(16, 9) + 75;

	if (OUTSIDE(ch) && (weather_info.sky >= SKY_LIGHTNING)) {

		if (saves_spell(ch, victim, SAVING_SPELL))
			dam >>= 1;

		if (MAXV((level - 28), 0) * 4 + 20 > number(1, 100))
			DAMAGE(ch, victim, dam, SPELL_CALL_LIGHTNING);

	}
}				/* END OF spell_call_lightning() */

void spell_transform_womble(sbyte level, struct char_data * ch,
			      struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();
	/*
   if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
		 bzero(buf, sizeof(buf));
		 sprintf(buf, "You must be level %d to cast womble form.\r\n",
						 IMO_IMP);
		 send_to_char(buf, ch);
		 return;
   }
	 */
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast womble form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */
	send_to_char("You become a womble!\r\n", victim);
	act("$n turns into a womble!", FALSE, victim, 0, 0, TO_ROOM);

	lv_time = 25;

	af.type = SPELL_TRANSFORM_WOMBLE;
	af.duration = lv_time;
	af.modifier = 5;
	af.location = APPLY_BONUS_DEX;
	af.bitvector = AFF_TRANSFORMED;
	ha1300_affect_to_char(victim, &af);

	af.bitvector = 0;

	af.location = APPLY_BONUS_CHA;
	af.modifier = -5;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MOVE;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_HIT;
	af.modifier = 50;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_DAMROLL;
	af.modifier = -5;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_HITROLL;
	af.modifier = 5;
	ha1300_affect_to_char(victim, &af);

	return;

}				/* END OF spell_transform_womble() */

void spell_transform_wolf(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time, lv_ac;

	CHECK_TRANSFORMS();
	/*
   if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
		 bzero(buf, sizeof(buf));
		 sprintf(buf, "You must be level %d to cast wolf form.\r\n",
						 IMO_IMP);
		 send_to_char(buf, ch);
		 return;
   }
	 */
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast wolf form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */
	send_to_char("You become a wolf!\r\n", victim);
	act("$n turns into a wolf!", FALSE, victim, 0, 0, TO_ROOM);

	lv_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim) + GET_BONUS_DEX(victim)) + races[GET_RACE(victim)].adj_ac;
	if (lv_ac < 0) {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_WOLF;
		af.duration = lv_time;
		af.modifier = 25;
		af.location = APPLY_BONUS_DEX;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.location = APPLY_BONUS_CHA;
		af.modifier = 20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_STR;
		af.modifier = 10;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_DAMROLL;
		af.modifier = 10;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HITROLL;
		af.modifier = 30;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 1;
		ha1300_affect_to_char(victim, &af);
	}
	else if (lv_ac > 0) {

		lv_time = 15;

		af.type = SPELL_TRANSFORM_WOLF;
		af.duration = lv_time;
		af.modifier = 10;
		af.location = APPLY_BONUS_DEX;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.location = APPLY_BONUS_CHA;
		af.modifier = 20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_STR;
		af.modifier = 25;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_DAMROLL;
		af.modifier = 10;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HITROLL;
		af.modifier = 30;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 1;
		ha1300_affect_to_char(victim, &af);
	}
	return;

}				/* END OF spell_transform_wolf() */

void spell_transform_bear(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time, lv_ac;

	CHECK_TRANSFORMS();
	/* if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
	 * bzero(buf, sizeof(buf)); sprintf(buf, "You must be level %d to cast
	 * bear form.\r\n", IMO_IMM); send_to_char(buf, ch); return; } */
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast bear form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */

	send_to_char("You become a bear!\r\n", victim);
	act("$n turns into a bear!", FALSE, victim, 0, 0, TO_ROOM);

	lv_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim) + GET_BONUS_DEX(victim)) + races[GET_RACE(victim)].adj_ac;
	if (lv_ac > 0) {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_BEAR;
		af.duration = lv_time;
		af.modifier = 30;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.location = APPLY_BONUS_CHA;
		af.modifier = -15;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_DEX;
		af.modifier = 10;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 2;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HITROLL;
		af.modifier = -10;
		ha1300_affect_to_char(victim, &af);
	}
	else if (lv_ac < 0) {

		lv_time = 20;

		af.type = SPELL_TRANSFORM_BEAR;
		af.duration = lv_time;
		af.modifier = 20;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.location = APPLY_BONUS_CHA;
		af.modifier = -15;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_DEX;
		af.modifier = 15;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 2;
		ha1300_affect_to_char(victim, &af);
	}
}				/* END OF spell_transform_bear() */

void spell_transform_manticore(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();
	/*
   if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
		 bzero(buf, sizeof(buf));
		 sprintf(buf, "You must be level %d to cast manticore form.\r\n",
						 IMO_IMP);
		 send_to_char(buf, ch);
		 return;
   }*/
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast manticore form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */

	if (IS_AFFECTED(victim, AFF_SENSE_LIFE)) {
		ha1350_affect_from_char(victim, SPELL_SENSE_LIFE);
	}

	if (IS_AFFECTED(victim, AFF_REGENERATION)) {
		ha1350_affect_from_char(victim, SPELL_REGENERATION);
	}

	if (IS_AFFECTED(victim, AFF_FLY)) {
		ha1350_affect_from_char(victim, SPELL_FLY);
	}

	send_to_char("You become a manticore!\r\n", victim);
	act("$n turns into a manticore!", FALSE, victim, 0, 0, TO_ROOM);

	lv_time = 25;

	af.type = SPELL_TRANSFORM_MANTICORE;
	af.duration = lv_time;
	af.modifier = 30;
	af.location = APPLY_BONUS_STR;
	af.bitvector = AFF_TRANSFORMED;
	ha1300_affect_to_char(victim, &af);

	af.bitvector = 0;

	af.modifier = -1;
	af.location = APPLY_NONE;
	af.bitvector = AFF_REGENERATION;
	ha1300_affect_to_char(victim, &af);

	af.duration = lv_time;
	af.location = APPLY_NONE;
	af.bitvector = AFF_FLY;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_NONE;
	af.bitvector = AFF_SENSE_LIFE;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_HIT;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MANA;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MOVE;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_EXTRA_ATTACKS;
	af.modifier = 1;
	ha1300_affect_to_char(victim, &af);


}				/* END OF spell_transform_manticore() */

void spell_transform_dragon(sbyte level, struct char_data * ch,
			      struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time, lv_ac;

	CHECK_TRANSFORMS();

	if (IS_AFFECTED(victim, AFF_SANCTUARY)) {
		ha1350_affect_from_char(victim, SPELL_SANCTUARY);
	}
	if (IS_AFFECTED(victim, AFF_FLY)) {
		ha1350_affect_from_char(victim, SPELL_FLY);
	}

	send_to_char("You become a &Rdragon&n!\r\n", victim);
	act("$n turns into a &Rdragon&n!", FALSE, victim, 0, 0, TO_ROOM);

	lv_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim) + GET_BONUS_DEX(victim)) + races[GET_RACE(victim)].adj_ac;
	if ((lv_ac <= 0) && (GET_RACE(ch) != RACE_PIXIE)) {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_DRAGON;
		af.duration = lv_time;
		af.modifier = 50;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.modifier = -1;
		af.location = APPLY_NONE;
		af.bitvector = AFF_SANCTUARY;
		ha1300_affect_to_char(victim, &af);

		af.duration = lv_time;
		af.location = APPLY_NONE;
		af.bitvector = AFF_FLY;
		ha1300_affect_to_char(victim, &af);

		//add 300 HP
			af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		//subtract 300 MANA
			af.location = APPLY_MANA;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		//add 300 MOVE
			af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 3;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_SAVING_BREATH;
		af.modifier = -20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_DEX;
		af.modifier = 50;
		ha1300_affect_to_char(victim, &af);
		return;
	}
	else if ((lv_ac > 0) && (GET_RACE(ch) != RACE_PIXIE)) {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_DRAGON;
		af.duration = lv_time;
		af.modifier = 50;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.modifier = -1;
		af.location = APPLY_NONE;
		af.bitvector = AFF_SANCTUARY;
		ha1300_affect_to_char(victim, &af);

		af.duration = lv_time;
		af.location = APPLY_NONE;
		af.bitvector = AFF_FLY;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MANA;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 3;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_SAVING_BREATH;
		af.modifier = -20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_DAMROLL;
		af.modifier = 50;
		ha1300_affect_to_char(victim, &af);
		return;
	}
	else if ((lv_ac <= 0) && (GET_RACE(ch) == RACE_PIXIE)) {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_DRAGON;
		af.duration = lv_time;
		af.modifier = 50;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		//add 300 HP
			af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		//subtract 300 MANA
			af.location = APPLY_MANA;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		//add 300 MOVE
			af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 3;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_SAVING_BREATH;
		af.modifier = -20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_BONUS_DEX;
		af.modifier = 50;
		ha1300_affect_to_char(victim, &af);
		return;
	}
	else {

		lv_time = 25;

		af.type = SPELL_TRANSFORM_DRAGON;
		af.duration = lv_time;
		af.modifier = 50;
		af.location = APPLY_BONUS_STR;
		af.bitvector = AFF_TRANSFORMED;
		ha1300_affect_to_char(victim, &af);

		af.bitvector = 0;

		af.modifier = -1;
		af.location = APPLY_NONE;
		af.bitvector = AFF_SANCTUARY;
		ha1300_affect_to_char(victim, &af);

        af.duration = lv_time;
		af.location = APPLY_NONE;
		af.bitvector = AFF_FLY;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_HIT;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MANA;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_MOVE;
		af.modifier = 100;
		ha1300_affect_to_char(victim, &af);
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_EXTRA_ATTACKS;
		af.modifier = 3;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_SAVING_BREATH;
		af.modifier = -20;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_DAMROLL;
		af.modifier = 50;
		ha1300_affect_to_char(victim, &af);
		return;
	}
}				/* END OF spell_transform_dragon() */

void spell_transform_chimera(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();
	/*
   if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
		 bzero(buf, sizeof(buf));
		 sprintf(buf, "You must be level %d to cast chimera form.\r\n",
						 IMO_IMP);
		 send_to_char(buf, ch);
		 return;
   }*/
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast chimera form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */

	if (IS_AFFECTED(victim, AFF_FLY) && (GET_RACE(ch) != RACE_PIXIE)) {
		ha1350_affect_from_char(victim, SPELL_FLY);
	}

	send_to_char("You become a chimera!\r\n", victim);
	act("$n turns into a chimera!", FALSE, victim, 0, 0, TO_ROOM);

	lv_time = 25;

	af.type = SPELL_TRANSFORM_CHIMERA;
	af.duration = lv_time;
	af.modifier = 30;
	af.location = APPLY_BONUS_STR;
	af.bitvector = AFF_TRANSFORMED;
	ha1300_affect_to_char(victim, &af);

	af.bitvector = 0;

	af.duration = lv_time;
	af.location = APPLY_NONE;
	af.bitvector = AFF_FLY;
	ha1300_affect_to_char(victim, &af);

	//add 100 HP
		af.location = APPLY_HIT;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	//add 150 MANA
		af.location = APPLY_MANA;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MANA;
	af.modifier = 50;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MOVE;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_EXTRA_ATTACKS;
	af.modifier = 2;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_BREATH;
	af.modifier = -10;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_SAVING_SPELL;
	af.modifier = -10;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_BONUS_DEX;
	af.modifier = 30;
	ha1300_affect_to_char(victim, &af);

}				/* END OF spell_transform_chimera() */

void spell_transform_monkey(sbyte level, struct char_data * ch,
			      struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();
	/* if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
	 * bzero(buf, sizeof(buf)); sprintf(buf, "You must be level %d to cast
	 * monkey form.\r\n", IMO_IMP); send_to_char(buf, ch); return; } */
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast monkey form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */

	send_to_char("You become a monkey!\r\n", victim);
	act("$n turns into a monkey!", FALSE, victim, 0, 0, TO_ROOM);

	lv_time = 25;

	af.type = SPELL_TRANSFORM_MONKEY;
	af.duration = lv_time;
	af.modifier = 10;
	af.location = APPLY_BONUS_DEX;
	af.bitvector = AFF_TRANSFORMED;
	ha1300_affect_to_char(victim, &af);

	af.bitvector = 0;

	af.location = APPLY_MOVE;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_EXTRA_ATTACKS;
	af.modifier = 1;
	ha1300_affect_to_char(victim, &af);

	return;

}				/* END OF spell_transform_monkey() */

void spell_transform_ninja(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();
	/* if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM) {
	 * bzero(buf, sizeof(buf)); sprintf(buf, "You must be level %d to cast
	 * ninja form.\r\n", IMO_IMP); send_to_char(buf, ch); return; } */
	/*
   if (GET_LEVEL(ch) < IMO_IMM) {
	
	   bzero(buf, sizeof(buf));
	   sprintf(buf, "You must be level %d to cast ninja form.\r\n",IMO_IMM);
	   send_to_char(buf, ch);
	   return;
   }
	 */

	send_to_char("You become a ninja!\r\n", victim);
	act("$n turns into a ninja!", FALSE, victim, 0, 0, TO_ROOM);

	lv_time = 25;

	af.type = SPELL_TRANSFORM_NINJA;
	af.duration = lv_time;
	af.modifier = 15;
	af.location = APPLY_BONUS_DEX;
	af.bitvector = AFF_TRANSFORMED;
	ha1300_affect_to_char(victim, &af);

	af.bitvector = 0;

	af.location = APPLY_HIT;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_MOVE;
	af.modifier = 100;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_DAMROLL;
	af.modifier = 10;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_HITROLL;
	af.modifier = 10;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_EXTRA_ATTACKS;
	af.modifier = 1;
	ha1300_affect_to_char(victim, &af);

	return;

}				/* END OF spell_transform_ninja() */

void spell_transform_cow(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj){

	//char buf[MAX_INPUT_LENGTH];
	struct affected_type af;
	int lv_time;

	CHECK_TRANSFORMS();

	//if (GET_LEVEL(ch) >= IMO_SPIRIT && GET_LEVEL(ch) <= IMO_IMM && !ha1175_isexactname(GET_NAME(ch), "rlum")) {
		//if (GET_LEVEL(ch) < IMO_IMM && !ha1175_isexactname(GET_NAME(ch), "rlum")) {
			/* if (!ha1175_isexactname(GET_NAME(ch), "rlum")) {
			 * 
			 * bzero(buf, sizeof(buf)); sprintf(buf, "You must be
			 * level %d to cast cow form.\r\n",IMO_IMM);
			 * send_to_char(buf, ch); return; } */

			if (IS_AFFECTED(victim, AFF_FLY) && (GET_RACE(ch) != RACE_PIXIE)) {
				ha1350_affect_from_char(victim, SPELL_FLY);
			}

			send_to_char("&WMoooooooooo! You are a mighty bovine&n!\r\n", victim);
			act("$n turns into a &Wcow&n!", FALSE, victim, 0, 0, TO_ROOM);

			lv_time = 25;


			af.type = SPELL_TRANSFORM_COW;
			af.duration = lv_time;
			af.modifier = 30;
			af.location = APPLY_BONUS_STR;
			af.bitvector = AFF_TRANSFORMED;
			ha1300_affect_to_char(victim, &af);

			af.bitvector = 0;

			af.duration = lv_time;
			af.location = APPLY_NONE;
			af.bitvector = AFF_FLY;
			ha1300_affect_to_char(victim, &af);

			//add 100 HP
				af.location = APPLY_HIT;
			af.modifier = 100;
			ha1300_affect_to_char(victim, &af);

			//add 100 MANA
				af.location = APPLY_MANA;
			af.modifier = 100;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_MOVE;
			af.modifier = 100;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_EXTRA_ATTACKS;
			af.modifier = 4;	/* one for each udder */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_BREATH;
			af.modifier = -10;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_SPELL;
			af.modifier = -10;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_BONUS_DEX;
			af.modifier = 10;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_DAMROLL;
			af.modifier = 5;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_HITROLL;
			af.modifier = 5;
			ha1300_affect_to_char(victim, &af);

		}		/* END OF spell_transform_cow() */

void spell_faerie_fire(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim && ch);
			  assert((level >= 0) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();

			if (obj) {
				act("Sorry, you can't cast faerie fire on items.\r\n", FALSE, ch, 0, victim, TO_CHAR);
				return;
			}

			if (magic_fails(ch, victim))
				  return;

			if (saves_spell(ch, victim, SAVING_SPELL) ||
			  ha1375_affected_by_spell(victim, SPELL_FAERIE_FIRE))
				return;

			af.type = SPELL_FAERIE_FIRE;
			af.duration = level;
			af.modifier = 20;
			af.location = APPLY_AC;
			af.bitvector2 = AFF2_FAERIE_FIRE;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_BONUS_STR;
			af.modifier = ((level / 10) * -1);
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_PARA;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_ROD;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_PETRI;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_BREATH;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_SPELL;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			act("$n is surrounded by a &Pbright pink&n aura!", FALSE, victim, 0, 0, TO_ROOM);
			act("You feel very uncomfortable.", FALSE, victim, 0, 0, TO_CHAR);

		}		/* End of spell_faerie_fire() */

void spell_faerie_fog(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int do_attack;
			struct char_data *tmp_victim, *temp, *leader;
			struct affected_type af;
			//int cmd = CMD_CAST;

			  assert(victim && ch);
			  assert((level >= 0) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();

			if (ch->master)
				  leader = ch->master;
			else
				  leader = ch;


			for (tmp_victim = world[ch->in_room].people;
			       tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
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
				}
			}
			if (saves_spell(ch, victim, SAVING_SPELL) ||
			  ha1375_affected_by_spell(victim, SPELL_FAERIE_FIRE))
				  return;

			if (ha1375_affected_by_spell(ch, SPELL_INVISIBLE))
				ha1350_affect_from_char(ch, SPELL_INVISIBLE);

			if (ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS)) {
				REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
				act("$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);
			}

			af.type = SPELL_FAERIE_FOG;
			af.duration = level;
			af.modifier = 20;
			af.location = APPLY_AC;
			af.bitvector2 = AFF2_FAERIE_FOG;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_BONUS_STR;
			af.modifier = ((level / 10) * -1);
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_PARA;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_ROD;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_PETRI;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_BREATH;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_SPELL;
			af.modifier = 1;	/* make it worse */
			ha1300_affect_to_char(victim, &af);

			act("A &ppurple haze&n fills the room!", FALSE, victim, 0, 0, TO_ROOM);
			act("You feel very uncomfortable.", FALSE, victim, 0, 0, TO_CHAR);

		}		/* END OF spell_faerie_fog() */

void spell_detect_shadows(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_SHADOWS))
				  return;

			  af.type = SPELL_DETECT_SHADOWS;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector = AFF2_DETECT_SHADOWS;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_shadows() */

void spell_barkskin(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;


			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_SANCTUARY)) {
				send_to_char("You cannot combine Sanctuary and Barkskin.\r\n", ch);
				return;
			}

			if (ha1375_affected_by_spell(ch, SPELL_STONESKIN)) {
				send_to_char("You cannot combine Stoneskin and Barkskin.\r\n", ch);
				return;
			}

			act("$n's skin becomes hard like bark.", TRUE, victim, 0, 0, TO_ROOM);
			act("Your skin becomes hard like bark.", TRUE, victim, 0, 0, TO_CHAR);

			af.type = SPELL_BARKSKIN;
			af.duration = (level <= IMO_LEV) ? 2 + level / 8 : level;
			af.modifier = 0;
			af.location = APPLY_NONE;
			af.bitvector2 = AFF2_BARKSKIN;

			ha1300_affect_to_char2(victim, &af);

		}		/* END OF spell_barkskin() */

void spell_stoneskin(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;


			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_SANCTUARY)) {
				send_to_char("You cannot combine Sanctuary and Stoneskin.\r\n", ch);
				return;
			}

			if (ha1375_affected_by_spell(victim, SPELL_BARKSKIN)) {
				send_to_char("You cannot combine Stoneskin and Barkskin.\r\n", ch);
				return;
			}

			act("$n's skin becomes as hard as stone.", TRUE, victim, 0, 0, TO_ROOM);
			act("Your skin becomes as hard as stone.", TRUE, victim, 0, 0, TO_CHAR);

			af.type = SPELL_STONESKIN;
			af.duration = (level <= IMO_LEV) ? 2 + level / 10 : level;
			af.modifier = 0;
			af.location = APPLY_NONE;
			af.bitvector2 = AFF2_STONESKIN;
			ha1300_affect_to_char2(victim, &af);

		}		/* END OF spell_stoneskin() */

void spell_thornwrack(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam;
			////int cmd = CMD_CAST;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			  CHECK_FOR_CHARM();
			  CHECK_FOR_PK();

			if (magic_fails(ch, victim))
				  return;

			  dam = dice(level, 10) + (6 * level);

			if (number(1, 26) == 1 &&
			      GET_CLASS(ch) == CLASS_DRUID &&
			      GET_LEVEL(ch) >= IMO_LEV) {
				//Random critical damage for druid
				dam *= 2;
				send_to_char("&R*&yCRITICAL&R*&n ", ch);
			}

			if (saves_spell(ch, victim, SAVING_SPELL))
				  dam >>= 1;

			if (ch->in_room != victim->in_room) {
				send_to_char("Your spell is unable to reach such a far distance...\n\r", ch);
				return;
			}

			DAMAGE(ch, victim, dam, SPELL_THORNWRACK);

			return;


		}		/* END OF spell_thornwrack() */

void spell_detect_demons(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_DEMONS))
				  return;

			  af.type = SPELL_DETECT_DEMONS;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector = AFF2_DETECT_DEMONS;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_demons() */

void spell_detect_dragons(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_DRAGONS))
				  return;

			  af.type = SPELL_DETECT_DRAGONS;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector2 = AFF2_DETECT_DRAGONS;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_dragons() */

void spell_detect_undead(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_UNDEAD))
				  return;

			  af.type = SPELL_DETECT_UNDEAD;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector2 = AFF2_DETECT_UNDEAD;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_undead() */

void spell_animal_summoning(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

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

			  lv_mob = number(0, 4) + 2610;

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
				sprintf(buf, "ERROR: Created mob %s with invalid room %d/%d.", GET_REAL_NAME(mob), mob->in_room, ch->in_room);
				ABORT_PROGRAM();
			}
			act("$n prays to $M god for assistance.", TRUE, ch, 0, 0, TO_ROOM);
			act("$n has summoned $N!", FALSE, ch, 0, mob, TO_ROOM);
			act("You have summoned $N!", FALSE, ch, 0, mob, TO_CHAR);

			if (ha4000_circle_follow(mob, ch)) {
				send_to_char("Sorry, following in circles not allowed.\n\r", ch);
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

		}		/* END OF spell_animal_summoning() */

void spell_detect_vampires(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_VAMPIRES))
				  return;

			  af.type = SPELL_DETECT_VAMPIRES;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector = AFF2_DETECT_VAMPIRES;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_vampires() */

void spell_detect_giants(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_DETECT_GIANTS))
				  return;

			  af.type = SPELL_DETECT_GIANTS;
			  af.duration = level * 5;
			  af.modifier = 0;
			  af.location = APPLY_NONE;
			  af.bitvector = AFF2_DETECT_GIANTS;

			  ha1300_affect_to_char(victim, &af);

			  send_to_char("Your eyes tingle.\n\r", victim);

		}		/* END OF spell_detect_giants() */

void spell_earthmaw(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

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

			  dam = dice(MINV(level >> 1, 20), 8) + level;

			if (number(1, 26) == 1 &&
			      GET_CLASS(ch) == CLASS_MAGIC_USER &&
			      GET_LEVEL(ch) >= IMO_LEV) {
				//Random critical fire damage for mages, Bingo 9 - 19 - 01
				dam *= 2;
				send_to_char("&R*&yCRITICAL&R*&n ", ch);
			}

			if (saves_spell(ch, victim, SAVING_SPELL))
				  dam >>= 1;

			if (ch->in_room != victim->in_room) {
				send_to_char("Your spell is unable to reach such a far distance...\n\r", ch);
				return;
			}

			DAMAGE(ch, victim, dam, SPELL_EARTHMAW);

			return;


		}		/* END OF spell_earthmaw() */

void spell_needlestorm(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam, adj_dam;
			int do_attack;
			char buf[MAX_STRING_LENGTH];
			struct char_data *tmp_victim, *temp, *leader;
			struct obj_data *lv_obj;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

			  dam = dice(MINV(level, 30), 14) + 4 * level;
			  if (number(1, 26) == 1 &&
			      GET_CLASS(ch) == CLASS_DRUID &&
			      GET_LEVEL(ch) >= IMO_LEV) {
				//Random critical damage for druid
				dam *= 2;
				send_to_char("&R*&yCRITICAL&R*&n ", ch);
			}

			  send_to_char("Thousands of tiny, sharp needles rain from the sky!\n\r", ch);
			  act("$n calls forth thousands of tiny, sharp needles to rain from the sky!", TRUE, ch, 0, 0, TO_ROOM);

			if (ch->master)
				  leader = ch->master;
			else
				  leader = ch;

			for (tmp_victim = world[ch->in_room].people;
			       tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
					do_attack = FALSE;
				if (IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (tmp_victim->master == leader)	/* don't attack
									 * followers */
					do_attack = FALSE;
				if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
					do_attack = FALSE;
				if ((IS_PC(ch)) && (IS_PC(tmp_victim)))
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
					if (saves_spell(ch, tmp_victim, SAVING_SPELL))
						adj_dam = dam >> 1;
					else
						adj_dam = dam;
					/* DOES PERSON HAVE AN ITEM OF SPELL
					 * ABSORPTION */
					lv_obj = ot4000_check_spell_store_item(victim, SPELL_NEEDLESTORM);
					if (lv_obj &&
					    !number(0, 3)) {
						sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
						send_to_char(buf, victim);
						lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
					}
					else {
						DAMAGE(ch, tmp_victim, adj_dam, SPELL_NEEDLESTORM);
					}

					/* WE'VE HIT ONE PERSON, IF NOT INDOORS
					 * GET OUT */
					if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
						return;
					}

				}	/* END OF do_attack */

			}	/* END OF for loop */

		}		/* END OF spell_needlstorm() */

void spell_treewalk(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_treewalk() */

void spell_summon_treant(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

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

			  lv_mob = number(0, 3) + 2615;;

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
				sprintf(buf, "ERROR: Created mob %s with invalid room %d/%d.", GET_REAL_NAME(mob), mob->in_room, ch->in_room);
				ABORT_PROGRAM();
			}
			act("$n prays to $M forest gods for assistance.", TRUE, ch, 0, 0, TO_ROOM);
			act("$n has summoned $N!", FALSE, ch, 0, mob, TO_ROOM);
			act("You have summoned $N!", FALSE, ch, 0, mob, TO_CHAR);

			if (ha4000_circle_follow(mob, ch)) {
				send_to_char("Sorry, following in circles not allowed.\n\r", ch);
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
				af.duration = 24 * 15 / GET_INT(mob);
			else
				af.duration = 24 * 15;

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

		}		/* END OF spell_summon_treant() */

void spell_sporecloud(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam, adj_dam;
			int do_attack;
			char buf[MAX_STRING_LENGTH];
			struct char_data *tmp_victim, *temp, *leader;
			struct obj_data *lv_obj;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

			  dam = dice(MINV(level, 24), 6) + 2 * level;
			  
			  if (number(1, 26) == 1 &&
			      GET_CLASS(ch) == CLASS_DRUID &&
			      GET_LEVEL(ch) >= IMO_LEV) {
				//Random critical damage for druid
				dam *= 2;
				send_to_char("&R*&yCRITICAL&R*&n ", ch);
			  }

			  send_to_char("A cloud of poisonous spores fills the room!\n\r", ch);
			  act("$n calls forth a cloud of poisonous spores!", TRUE, ch, 0, 0, TO_ROOM);

			if (ch->master)
				  leader = ch->master;
			else
				  leader = ch;

			for (tmp_victim = world[ch->in_room].people;
			       tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
					do_attack = FALSE;
				if (IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (tmp_victim->master == leader)	/* don't attack
									 * followers */
					do_attack = FALSE;
				if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
					do_attack = FALSE;
				if ((IS_PC(ch)) && (IS_PC(tmp_victim)))
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
					if (saves_spell(ch, tmp_victim, SAVING_SPELL))
						adj_dam = dam >> 1;
					else
						adj_dam = dam;
					/* DOES PERSON HAVE AN ITEM OF SPELL
					 * ABSORPTION */
					lv_obj = ot4000_check_spell_store_item(victim, SPELL_SPORECLOUD);
					if (lv_obj &&
					    !number(0, 3)) {
						sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
						send_to_char(buf, victim);
						lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
					}
					else {
						DAMAGE(ch, tmp_victim, adj_dam, SPELL_SPORECLOUD);
					}

					/* WE'VE HIT ONE PERSON, IF NOT INDOORS
					 * GET OUT */
					if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
						return;
					}

				}	/* END OF do_attack */

			}	/* END OF for loop */

		}		/* END OF spell_sporecloud() */

void spell_wrath_of_nature(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam, adj_dam;
			int do_attack;
			char buf[MAX_STRING_LENGTH];
			struct char_data *tmp_victim, *temp, *leader;
			struct obj_data *lv_obj;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

			  dam = dice(level, 18) + 6 * level;
			  
			  if (number(1, 26) == 1 &&
			      GET_CLASS(ch) == CLASS_DRUID &&
			      GET_LEVEL(ch) >= IMO_LEV) {
				//Random critical damage for druid
				dam *= 2;
				send_to_char("&R*&yCRITICAL&R*&n ", ch);
			}

			  send_to_char("The entire area is engulfed in storm as nature's wrath is unleashed!\n\r", ch);
			  act("$n engulfs the room in storm as $E unleashes nature's wrath!", TRUE, ch, 0, 0, TO_ROOM);

			if (ch->master)
				  leader = ch->master;
			else
				  leader = ch;

			for (tmp_victim = world[ch->in_room].people;
			       tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
					do_attack = FALSE;
				if (IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (tmp_victim->master == leader)	/* don't attack
									 * followers */
					do_attack = FALSE;
				if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
					do_attack = FALSE;
				if ((IS_PC(ch)) && (IS_PC(tmp_victim)))
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
					if (saves_spell(ch, tmp_victim, SAVING_SPELL))
						adj_dam = dam >> 1;
					else
						adj_dam = dam;
					/* DOES PERSON HAVE AN ITEM OF SPELL
					 * ABSORPTION */
					lv_obj = ot4000_check_spell_store_item(victim, SPELL_WRATH_OF_NATURE);
					if (lv_obj &&
					    !number(0, 3)) {
						sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
						send_to_char(buf, victim);
						lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
					}
					else {
						DAMAGE(ch, tmp_victim, adj_dam, SPELL_WRATH_OF_NATURE);
					}

					/* WE'VE HIT ONE PERSON, IF NOT INDOORS
					 * GET OUT */
					if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
						return;
					}

				}	/* END OF do_attack */

			}	/* END OF for loop */

		}		/* END OF spell_wrath_of_nature() */

void spell_beast_transform(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;
			int lv_time, aa, idx, rdlev, spell_array[4] = {0, 0, 0, 0},
			  btnum;

			  CHECK_FOR_NO_FIGHTING();
			  CHECK_FOR_CHARM();
			  CHECK_TRANSFORMS();


			if (ha1375_affected_by_spell(victim, SPELL_BEAST_TRANSFORM)) {
				send_to_char("Nothing seems to happen.\n\r", ch);
				return;
			}
			if (ch->nextcast > 0) {
				send_to_char("Your body cannot yet handle another transformation.\n\r", ch);
				return;
			}
			if (magic_fails(ch, victim))
				return;

			ch->nextcast = time(0) + (number(30, 45)) * 60;
			if (GET_LEVEL(ch) >= IMO_IMP)
				ch->nextcast = 0;

			aa = 0;
			rdlev = -1;
			if (GET_LEVEL(ch) <= 20) {
				rdlev++;
				spell_array[rdlev] = 1;
			}
			if (GET_LEVEL(ch) <= 30 || GET_LEVEL(ch) > 20) {
				rdlev++;
				spell_array[rdlev] = 2;
			}
			if (GET_LEVEL(ch) <= 40 || GET_LEVEL(ch) > 30) {
				rdlev++;
				spell_array[rdlev] = 3;
			}
			if (GET_LEVEL(ch) >= 41) {
				rdlev++;
				spell_array[rdlev] = 4;
			}
			if (rdlev < 0) {
				return;
			}
			idx = number(0, rdlev);
			aa = 0;

			switch (spell_array[idx]) {
			case 1:{
					btnum = (number(1, 3));
					if (btnum == 1 || btnum == 3) {
						send_to_char("You become a serpent!\r\n", victim);
						act("$n turns into a serpent!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_CHA;
						af.modifier = ((GET_LEVEL(ch) / 5) - 1);
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 2) {
						send_to_char("You become a badger!\r\n", victim);
						act("$n turns into a badger!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 10;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_HIT;
						af.modifier = ((GET_LEVEL(ch) / 5) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MOVE;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 10));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
				}
			case 2:{
					btnum = (number(1, 3));
					if (btnum == 1) {
						send_to_char("You become a serpent!\r\n", victim);
						act("$n turns into a serpent!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_CHA;
						af.modifier = ((GET_LEVEL(ch) / 5) - 1);
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 2) {
						send_to_char("You become a badger!\r\n", victim);
						act("$n turns into a badger!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 10;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_HIT;
						af.modifier = ((GET_LEVEL(ch) / 5) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MOVE;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 10));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 3) {
						if (IS_AFFECTED(victim, AFF_FLY)) {
						ha1350_affect_from_char(victim, SPELL_FLY);
						}
						send_to_char("You become an eagle!\r\n", victim);
						act("$n turns into an eagle!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_MOVE;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.duration = lv_time;
						af.location = APPLY_NONE;
						af.bitvector = AFF_FLY;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 8));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
				}
			case 3:{
					btnum = (number(1, 4));

					if (btnum == 1) {
						if (IS_AFFECTED(victim, AFF_FLY)) {
						ha1350_affect_from_char(victim, SPELL_FLY);
						}
						send_to_char("You transform into a drake!\r\n", victim);
						act("$n transforms into a drake!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 20;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_DEX;
						af.modifier = (GET_LEVEL(ch) / 15);
						ha1300_affect_to_char(victim, &af);

						af.duration = lv_time;
						af.location = APPLY_NONE;
						af.bitvector = AFF_FLY;
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HIT;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = (GET_LEVEL(ch) / 10);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 2) {
						send_to_char("You become a serpent!\r\n", victim);
						act("$n turns into a serpent!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_CHA;
						af.modifier = ((GET_LEVEL(ch) / 5) - 1);
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 3) {
						if (IS_AFFECTED(victim, AFF_FLY)) {
						ha1350_affect_from_char(victim, SPELL_FLY);
						}						
						send_to_char("You become an eagle!\r\n", victim);
						act("$n turns into an eagle!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_MOVE;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.duration = lv_time;
						af.location = APPLY_NONE;
						af.bitvector = AFF_FLY;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 8));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 4) {
						send_to_char("You become a badger!\r\n", victim);
						act("$n turns into a badger!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 10;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_HIT;
						af.modifier = ((GET_LEVEL(ch) / 5) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MOVE;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 10));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
				}
			case 4:{
					btnum = (number(1, 5));

					if (btnum == 1 || btnum == 5) {
						send_to_char("You transform into a drake!\r\n", victim);
						act("$n transforms into a drake!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 20;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_DEX;
						af.modifier = (GET_LEVEL(ch) / 15);
						ha1300_affect_to_char(victim, &af);

						af.duration = lv_time;
						af.location = APPLY_NONE;
						af.bitvector = AFF_FLY;
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HIT;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = (GET_LEVEL(ch) / 10);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 2) {
						send_to_char("You become a serpent!\r\n", victim);
						act("$n turns into a serpent!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 10);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_BONUS_CHA;
						af.modifier = ((GET_LEVEL(ch) / 5) - 1);
						ha1300_affect_to_char(victim, &af);

						af.modifier = -1;
						af.location = APPLY_NONE;
						af.bitvector = AFF_BREATHWATER;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MANA;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 3) {
						send_to_char("You become an eagle!\r\n", victim);
						act("$n turns into an eagle!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 15;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_DEX;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_MOVE;
						af.modifier = (GET_LEVEL(ch) + 10);
						ha1300_affect_to_char(victim, &af);

						af.duration = lv_time;
						af.location = APPLY_NONE;
						af.bitvector = AFF_FLY;
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 8));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 8) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
					else if (btnum == 4) {
						send_to_char("You become a badger!\r\n", victim);
						act("$n turns into a badger!", FALSE, victim, 0, 0, TO_ROOM);
						send_to_char("Your body is drained from the transformation and will take some time to rejuvenate.\r\n", ch);

						lv_time = 10;

						af.type = SPELL_BEAST_TRANSFORM;
						af.duration = lv_time;
						af.modifier = (GET_LEVEL(ch) / 15);
						af.location = APPLY_BONUS_STR;
						af.bitvector = AFF_TRANSFORMED;
						ha1300_affect_to_char(victim, &af);

						af.bitvector = 0;

						af.location = APPLY_HIT;
						af.modifier = ((GET_LEVEL(ch) / 5) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_MOVE;
						af.modifier = ((GET_LEVEL(ch) / 2) + 10);
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_DAMROLL;
						af.modifier = ((GET_LEVEL(ch) / 10));
						ha1300_affect_to_char(victim, &af);

						af.location = APPLY_HITROLL;
						af.modifier = ((GET_LEVEL(ch) / 10) - 1);
						ha1300_affect_to_char(victim, &af);

						return;
					}
				}
			}
		}

void spell_create_spring(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_create_spring() */

void spell_leviathan(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_leviathan() */

void spell_meteor_swarm(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam, adj_dam;
			int do_attack;
			char buf[MAX_STRING_LENGTH];
			struct char_data *tmp_victim, *temp, *leader;
			struct obj_data *lv_obj;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

			  dam = dice(level, 20) + 3 * level;

			  send_to_char("Giant boulders rain down from the heavens as you unleash a mighty meteor storm!\n\r", ch);
			  act("Giant boulders rain down from the heavens as $n unleashes a mighty meteor storm!", TRUE, ch, 0, 0, TO_ROOM);

			if (ch->master)
				  leader = ch->master;
			else
				  leader = ch;

			for (tmp_victim = world[ch->in_room].people;
			       tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
					do_attack = FALSE;
				if (tmp_victim->master == leader)	/* don't attack
									 * followers */
					do_attack = FALSE;
				if (IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
					do_attack = FALSE;
				if ((IS_PC(ch)) && (IS_PC(tmp_victim)))
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
					if (saves_spell(ch, tmp_victim, SAVING_SPELL))
						adj_dam = dam >> 1;
					else
						adj_dam = dam;
					/* DOES PERSON HAVE AN ITEM OF SPELL
					 * ABSORPTION */
					lv_obj = ot4000_check_spell_store_item(victim, SPELL_METEOR_SWARM);
					if (lv_obj &&
					    !number(0, 3)) {
						sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
						send_to_char(buf, victim);
						lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
					}
					else {
						DAMAGE(ch, tmp_victim, adj_dam, SPELL_METEOR_SWARM);
					}

					/* WE'VE HIT ONE PERSON, IF NOT INDOORS
					 * GET OUT */
					if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
						return;
					}

				}	/* END OF do_attack */

			}	/* END OF for loop */

		}		/* END OF spell_meteor_swarm() */

void spell_charm_animal(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_charm_animal() */

void spell_tsunami(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int dam, adj_dam;
			int do_attack;
			char buf[MAX_STRING_LENGTH];
			struct char_data *tmp_victim, *temp, *leader;
			struct obj_data *lv_obj;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

			  send_to_char("The entire area is engulfed in a huge wave as a tsunami is unleashed!\n\r", ch);
			  act("$n engulfs the room in a huge wave as $E unleashes a tsunami!", TRUE, ch, 0, 0, TO_ROOM);

			if (GET_LEVEL(ch) < IMO_LEV && !IS_AFFECTED(ch, AFF_BREATHWATER)) {
				send_to_char("You are drowning!\r\n", ch);

				dam = -1 * (GET_HIT(ch) * number(10, 15) / 100);
			}
			if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_BREATHWATER)) {
				send_to_char("You are drowning!\r\n", ch);

				dam = dice(level, 22) + 2 * level;
			}
			if (ch->master)
				leader = ch->master;
			else
				leader = ch;

			for (tmp_victim = world[ch->in_room].people;
			     tmp_victim; tmp_victim = temp) {
				temp = tmp_victim->next_in_room;
				do_attack = TRUE;
				if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK
							 * FREE MOBS */
				    IS_NPC(tmp_victim) &&
				    !IS_AFFECTED(tmp_victim, AFF_CHARM))
					do_attack = FALSE;
				if (ch == tmp_victim)	/* don't attack
							 * ourselves */
					do_attack = FALSE;
				if (ch->master == tmp_victim)	/* don't attack our
								 * leader */
					do_attack = FALSE;
				if (tmp_victim->master == leader)	/* don't attack
									 * followers */
					do_attack = FALSE;
				if (IS_PC(tmp_victim) && GET_LEVEL(tmp_victim) > PK_LEV)
/* don't attack immorts */
					do_attack = FALSE;
				if ((IS_PC(ch)) && (IS_PC(tmp_victim)))
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
					if (saves_spell(ch, tmp_victim, SAVING_SPELL))
						adj_dam = dam >> 1;
					else
						adj_dam = dam;
					/* DOES PERSON HAVE AN ITEM OF SPELL
					 * ABSORPTION */
					lv_obj = ot4000_check_spell_store_item(victim, SPELL_TSUNAMI);
					if (lv_obj &&
					    !number(0, 3)) {
						sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
						send_to_char(buf, victim);
						lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
					}
					else {
						DAMAGE(ch, tmp_victim, adj_dam, SPELL_TSUNAMI);
					}

					/* WE'VE HIT ONE PERSON, IF NOT INDOORS
					 * GET OUT */
					if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
						return;
					}

				}	/* END OF do_attack */

			}	/* END OF for loop */

		}		/* END OF spell_tsunami() */

void spell_tornado(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_tornado() */

void spell_landslide(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_landslide() */

void spell_acid_blast(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_acid_blast() */

void spell_displacement(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(ch && victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(victim, SPELL_INVISIBLE)) {
				send_to_char("You cannot combine displacement and invisibility.\r\n", ch);
				return;
			}

			if (ha1375_affected_by_spell(victim, SPELL_IMPROVED_INVIS)) {
				send_to_char("You cannot combine displacement and improved invisibility.\r\n", ch);
				return;
			}

			if (!ha1375_affected_by_spell(victim, SPELL_DISPLACEMENT)) {
				af.type = SPELL_DISPLACEMENT;
				af.duration = 50;
				af.modifier = -1 * (MINV(50, level * 2));
				af.location = APPLY_AC;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				af.modifier = -5;
				af.location = APPLY_SAVING_SPELL;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				if (ch == victim)
					send_to_char("You feel a bit displaced.\n\r", victim);
				else
					send_to_char("You feel your body shift slightly.\n\r", victim);

				act("$n glows white and flickers for a moment.", FALSE, victim, 0, 0, TO_ROOM);
			}
		}		/* END OF spell_displacement() */

void spell_summon_elemental(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_summon_elemental() */

void spell_shockwave(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_shockwave() */

void spell_gust(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_gust() */

void spell_granite_fist(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_granite_fist() */

void spell_prayer(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(ch && victim);
			  assert((level >= 0) && (level <= NPC_LEV));
			  
			CHECK_FOR_NO_FIGHTING();

			if ((!ha1375_affected_by_spell(victim, SPELL_BLESS) &&
			      !ha1375_affected_by_spell(victim, SPELL_VEX) &&
			      !ha1375_affected_by_spell(victim, SPELL_ARMOR) &&
			      !ha1375_affected_by_spell(victim, SPELL_AID))) {

				if (magic_fails(ch, victim))
					return;

				send_to_char("You feel closer to your god.\n\r", victim);
				af.type = SPELL_PRAYER;
				af.duration = level * 2;
				if (GET_ALIGNMENT(ch) >= 400) {
					af.modifier = GET_ALIGNMENT(ch) / 200;
				}
				else {
					af.modifier = 3;
				}
				af.location = APPLY_HITROLL;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				if (GET_ALIGNMENT(ch) <= -400) {
					af.modifier = GET_ALIGNMENT(ch) / 200;
				}
				else {
					af.modifier = 3;
				}
				af.location = APPLY_DAMROLL;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_MANA;
				af.modifier = level;	/* Make better */
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_PARA;
				af.modifier = -2;	/* Make better */
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_ROD;
				af.modifier = -2;	/* Make better */
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_PETRI;
				af.modifier = -2;	/* Make better */
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_BREATH;
				af.modifier = -2;	/* Make better */
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_SPELL;
				af.modifier = -2;	/* Make better */
				ha1300_affect_to_char(victim, &af);
			}
		}		/* END OF spell_prayer() */

void spell_elemental_shield(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_elemental_shield() */

void spell_holy_healing(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int healpoints;

			  assert(victim && ch);
			if (magic_fails(ch, victim))
				  return;

			  healpoints = dice(GET_INT(ch), GET_WIS(ch)) + number(1, GET_LEVEL(victim));

			if (victim == ch) {
				send_to_char("You cannot become good by healing yourself!\r\n", ch);
				return;
			}

			if (!saves_spell(ch, victim, SAVING_SPELL)) {
				if (GET_ALIGNMENT(ch) >= 1000)
					GET_ALIGNMENT(ch) = 1000;
				else
					GET_ALIGNMENT(ch) += 1000;
			}

			if ((healpoints + GET_HIT(victim)) > li1500_hit_limit(victim))
				GET_HIT(victim) = li1500_hit_limit(victim);
			else
				GET_HIT(victim) += healpoints;

			act("You muster all of the holy energy from deep inside and heal $N.", TRUE, ch, 0, victim, TO_CHAR);
			act("$n musters all of $s holy energy from deep inside and heals you.", TRUE, ch, 0, victim, TO_VICT);
			act("$n musters all of $s holy energy from deep inside and heals $N.", TRUE, ch, 0, victim, TO_NOTVICT);

			ft1200_update_pos(victim);

			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
			WAIT_STATE(ch, PULSE_VIOLENCE);

		}		/* END OF spell_holy_healing() */

void spell_lifetap(sbyte level, struct char_data * ch, struct char_data * victim, struct obj_data * obj) {

			int drainpts;
			//int cmd = CMD_CAST;

			  assert(ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			  CHECK_FOR_CHARM();
			  CHECK_FOR_PK();

			if (magic_fails(ch, victim))
				  return;

			if ((GET_LEVEL(ch) <= 41) && (
				       (GET_CLASS(ch) != CLASS_NECROMANCER) ||
				      (GET_CLASS(ch) != CLASS_CHAOSKNIGHT))) {
				send_to_char("Sorry, only Necromancers and Chaos Knights can use this spell.\r\n", ch);
				return;
			}

			if (ch == victim) {
				send_to_char("That would be pointless...\r\n", ch);
				return;
			}

			drainpts = dice(3, GET_LEVEL(ch) / 4) + ((GET_STR(ch) * GET_INT(ch)) / dice(2, 6));

			act("&rYour lifetap drains hps from $N.&E", TRUE, ch, 0, victim, TO_CHAR);
			act("&r$n taps some life out of you!&E", TRUE, ch, 0, victim, TO_VICT);
			act("&r$n taps some life out of $N.&E", TRUE, ch, 0, victim, TO_NOTVICT);

			if ((drainpts + GET_HIT(ch)) > li1500_hit_limit(ch))
				GET_HIT(ch) = li1500_hit_limit(ch);
			else
				GET_HIT(ch) += drainpts;
			DAMAGE(ch, victim, drainpts, SPELL_LIFETAP);

		}		/* END OF spell_lifetap() */

void spell_enchant_armor(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			/* Bingo 01-18-01 */



			assert(ch && obj);

			if (GET_LEVEL(ch) < IMO_IMM) {
				send_to_char("No sir, I didn't like it - Mr. Horse\r\n", ch);
				return;
			}
			if ((GET_ITEM_TYPE(obj) != ITEM_ARMOR)) {
				send_to_char("You can only enchant armor.\r\n", ch);
				return;
			}

			if (IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC)) {
				send_to_char("You can't enchant magical items.\r\n", ch);
				return;
			}

			/* IF WEAPON HAS ANY EXISTING APPLYs, GET OUT */
			if (obj->affected[3].location != APPLY_NONE) {
				send_to_char("The armor resist your enchantment!\r\n", ch);
				return;
			}

			SET_BIT(obj->obj_flags.flags1, OBJ1_MAGIC);

			switch (number(1, 6)) {
			case 1:
				switch (number(1, 3)) {
				case 1:
					obj->affected[3].location = APPLY_HIT;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-10, 10);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-20, 20);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-50, 50);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 100);
					}
					break;
				case 2:
					obj->affected[3].location = APPLY_MANA;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-10, 10);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-20, 20);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-50, 50);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 100);
					}
					break;
				case 3:
					obj->affected[3].location = APPLY_MOVE;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-10, 10);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-20, 20);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-50, 50);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 100);
					}
					break;
				}
				break;
			case 2:
				switch (number(1, 2)) {
				case 1:
					obj->affected[3].location = APPLY_DAMROLL;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-1, 1);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-2, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 2:
					obj->affected[3].location = APPLY_HITROLL;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-1, 1);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-2, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				}
				break;
			case 3:
				switch (number(1, 6)) {
				case 1:
					obj->affected[3].location = APPLY_STR;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 2:
					obj->affected[3].location = APPLY_INT;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 3:
					obj->affected[3].location = APPLY_WIS;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 4:
					obj->affected[3].location = APPLY_DEX;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 5:
					obj->affected[3].location = APPLY_CON;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				case 6:
					obj->affected[3].location = APPLY_CHA;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = number(-2, 1);
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = number(-2, 2);
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 2);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = number(0, 5);
					}
					break;
				}
				break;
			case 4:
				switch (number(1, 13)) {
				case 1:
					obj->affected[3].location = APPLY_DETECT_INVIS;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 2:
					obj->affected[3].location = APPLY_DARKSIGHT;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 3:
					obj->affected[3].location = APPLY_MAGIC_RESIST;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 4:
					obj->affected[3].location = APPLY_MAGIC_IMMUNE;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 5:
					obj->affected[3].location = APPLY_BREATHWATER;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 6:
					obj->affected[3].location = APPLY_DETECT_MAGIC;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 7:
					obj->affected[3].location = APPLY_SENSE_LIFE;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 8:
					obj->affected[3].location = APPLY_DETECT_EVIL;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 9:
					obj->affected[3].location = APPLY_INVIS;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 10:
					obj->affected[3].location = APPLY_IMPROVED_INVIS;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 11:
					obj->affected[3].location = APPLY_REGENERATION;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 12:
					obj->affected[3].location = APPLY_HOLD_PERSON;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				case 13:
					obj->affected[3].location = APPLY_HASTE;
					if (GET_LEVEL(ch) < 20)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_LEV)
						obj->affected[3].modifier = 0;
					else if (GET_LEVEL(ch) < IMO_IMP)
						obj->affected[3].modifier = number(-1, 0);
					else {
						send_to_char("Power surge!  Wonder what that did?\r\n", ch);
						obj->affected[3].modifier = -1;
					}
					break;
				}
				break;
			case 5:
				switch (number(1, 4)) {
				case 1:
					SET_BIT(obj->obj_flags.flags1, OBJ1_NORENT);
					send_to_char("That's not good.... Better take a look.\r\n", ch);
					break;
				case 2:
					SET_BIT(obj->obj_flags.flags2, OBJ2_NODROP);
					send_to_char("That's not good.... Better take a look.\r\n", ch);
					break;
				case 3:
					SET_BIT(obj->obj_flags.flags2, OBJ2_LORE);
					send_to_char("That's not good.... Better take a look.\r\n", ch);
					break;
				case 4:
					SET_BIT(obj->obj_flags.flags1, OBJ1_NO_SUMMON);
					send_to_char("That's not good.... Better take a look.\r\n", ch);
					break;
				}
				break;
			case 6:
				if (GET_LEVEL(ch) < 20)
					obj->obj_flags.value[0] += number(-5, 5);
				else if (GET_LEVEL(ch) < IMO_LEV)
					obj->obj_flags.value[0] += number(-10, 10);
				else if (GET_LEVEL(ch) < IMO_IMP)
					obj->obj_flags.value[0] += number(-15, 15);
				else {
					send_to_char("Power surge!  Wonder what that did?\r\n", ch);
					obj->obj_flags.value[0] += number(0, 25);
				}
				break;
			}

			SET_BIT(obj->obj_flags.flags1, OBJ1_NORENT);

			if (IS_GOOD(ch)) {
				SET_BIT(obj->obj_flags.flags1, OBJ1_ANTI_EVIL);
				act("$p glows blue.", FALSE, ch, obj, 0, TO_CHAR);
			}
			else if (IS_EVIL(ch)) {
				SET_BIT(obj->obj_flags.flags1, OBJ1_ANTI_GOOD);
				act("$p glows red.", FALSE, ch, obj, 0, TO_CHAR);
			}
			else {
				act("$p glows.", FALSE, ch, obj, 0, TO_CHAR);
			}
			return;

		}		/* END OF spell_enchant_armor() */

void spell_vex(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;

			  assert(ch && victim);
			  assert((level >= 0) && (level <= NPC_LEV));

			if ((GET_POS(victim) != POSITION_FIGHTING) &&
			      (!ha1375_affected_by_spell(victim, SPELL_VEX) &&
			    !ha1375_affected_by_spell(victim, SPELL_BLESS))) {

				if (GET_ALIGNMENT(ch) < -1000)
					GET_ALIGNMENT(ch) = -1000;

				if (magic_fails(ch, victim))
					return;
				send_to_char("You feel a deep agonizing irritation course through your veins.\n\r", victim);
				af.type = SPELL_VEX;
				af.duration = 6;
				if (GET_ALIGNMENT(ch) <= -400) {
					af.modifier = GET_ALIGNMENT(ch) / -200;
				}
				else {
					af.modifier = 3;
				}
				af.location = APPLY_DAMROLL;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				if (GET_ALIGNMENT(ch) <= -400) {
					af.modifier = GET_ALIGNMENT(ch) / 200;
				}
				else {
					af.modifier = -1;
				}
				af.location = APPLY_HITROLL;
				af.bitvector = 0;
				ha1300_affect_to_char(victim, &af);

				af.location = APPLY_SAVING_SPELL;
				af.modifier = +1;	/* Make worse */
				ha1300_affect_to_char(victim, &af);
			}

		}		/* END OF spell_vex */

void spell_gusher(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {
			int dam;
			//int cmd = CMD_CAST;

			  assert(victim && ch);
			  assert((level >= 1) && (level <= NPC_LEV));

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			  CHECK_FOR_CHARM();
			  CHECK_FOR_PK();

			if (magic_fails(ch, victim))
				  return;

			  dam = dice(MINV(level >> 1, 15), 12) + 15;

			if (saves_spell(ch, victim, SAVING_SPELL))
				  dam >>= 1;

			  DAMAGE(ch, victim, dam, SPELL_GUSHER);
		}		/* END OF spell_gusher() */

void spell_sleep_immunity(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {
			struct affected_type af;

			  assert(victim);
			if (magic_fails(ch, victim))
				  return;

			if (ha1375_affected_by_spell(ch, AFF_SLEEP))
				  ha1350_affect_from_char(ch, AFF_SLEEP);

			if (ha1375_affected_by_spell(victim, SPELL_SLEEP_IMMUNITY)) {
				send_to_char("You feel the weight of sleep lift from your body.\n\r", ch);

				af.type = SPELL_SLEEP_IMMUNITY;
				af.duration = 3 * level;
				af.modifier = 0;
				af.location = APPLY_NONE;
				af.bitvector2 = AFF2_SLEEP_IMMUNITY;
				ha1300_affect_to_char(victim, &af);
			}
		}

void spell_xxx4(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			send_to_char("Nothing happens\r\n", ch);
			return;

		}		/* END OF spell_xxx() */

void spell_purge_magic(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			int vnum, i, empty_affects = TRUE;
			struct obj_data *prototype;

			if (!obj) {
				send_to_char("You can't find that object!?\r\n", ch);
				return;
			}

			/* So, we need to take away some bits, and possibly
			 * some hitroll, damroll, and AC, but we have no real
			 * idea if we need to take away hitroll/damroll/AC.
			 * So, let's just grab them from the prototype database
			 * and be done with it.
			 * 
			 * This could destroy some oldgen stats, but not all of
			 * them, and only if certain cases, but could still be
			 * use to accidentially downgrade oldgen and upgrade to
			 * newgen.  Unfortunately, there's no real way around
			 * this, because the changes by the spell are not
			 * recorded into the object, and because there are
			 * "oldgen" items.
			 * 
			 * -ff */

			  vnum = obj_index[obj->item_number].virtual;

			prototype = db5100_read_object(vnum, PROTOTYPE);

			/* If all the proper bits are set, then assume it's
			 * been blessed, and load the AC (ovalue 0) from the
			 * prototype. */
			if ((GET_ITEM_TYPE(obj) == ITEM_QUEST || GET_ITEM_TYPE(obj) == ITEM_ARMOR) &&
			    IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC) && IS_SET(obj->obj_flags.flags1, OBJ1_BLESS)) {
				OVAL0(obj) = OVAL0(prototype);
			}

			/* If it's a weapon (or quest weapon), and the magic
			 * bit is set, and it has hitroll, then damroll, then
			 * the rest of the affects are empty, we assume it's
			 * been enchanted, and load all the affects from the
			 * prototype.
			 * 
			 * This code is kinda messy, but it shouldn't need to be
			 * touched if more object affect slots are added later. */
			if ((GET_ITEM_TYPE(obj) == ITEM_WEAPON || GET_ITEM_TYPE(obj) == ITEM_QSTWEAPON) &&
			    IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC) &&
			    obj->affected[0].location == APPLY_HITROLL &&
			    obj->affected[1].location == APPLY_DAMROLL) {
				for (i = 2; i < MAX_OBJ_AFFECT; i++) {
					if (obj->affected[i].location != APPLY_NONE)
						empty_affects = FALSE;
				}
				if (empty_affects) {
					for (i = 0; i < MAX_OBJ_AFFECT; i++) {
						obj->affected[i].location = prototype->affected[i].location;
						obj->affected[i].modifier = prototype->affected[i].modifier;
					}
				}
			}

			/* If magic, bless, or invis bit is set, reset all the
			 * bits, and remove them.  (This way we get rid of any
			 * extra anti's, but not ones that are supposed to be
			 * there.) */

			if (IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC) ||
			    IS_SET(obj->obj_flags.flags1, OBJ1_BLESS) ||
			    IS_SET(obj->obj_flags.flags1, OBJ1_INVISIBLE)) {
				obj->obj_flags = prototype->obj_flags;
				if (IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC))
					REMOVE_BIT(obj->obj_flags.flags1, OBJ1_MAGIC);
				if (IS_SET(obj->obj_flags.flags1, OBJ1_BLESS))
					REMOVE_BIT(obj->obj_flags.flags1, OBJ1_BLESS);
				if (IS_SET(obj->obj_flags.flags1, OBJ1_INVISIBLE))
					REMOVE_BIT(obj->obj_flags.flags1, OBJ1_INVISIBLE);
				if (IS_SET(obj->obj_flags.value[0], WEAPON_POISONOUS))
					REMOVE_BIT(obj->obj_flags.value[0], WEAPON_POISONOUS);
			}



		}		/* End of spell_purge_magic */
