/* ma */
/* gv_location: 7501-8000 */
/* ********************************************************************
*  file: magic1.c, Implementation of spells.          Part of DIKUMUD *
*  Usage : The actual effect of magic.                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

/* WHERE lv_flag:                                           */
/*                 BIT0 not set means random room from zone */
/*                 BIT1 set, get room from known world      */
int ma1000_do_get_random_room(struct char_data * ch, int lv_flag)
{

	int idx, lv_location, lv_room, lv_zone, lv_bottom, lv_top;
	char buf[MAX_STRING_LENGTH];



	lv_room = ch->in_room;
	lv_zone = world[ch->in_room].zone;
	if (IS_SET(lv_flag, BIT0)) {
		lv_bottom = 1;
		lv_top = top_of_world - 1;
	}
	else {
		lv_bottom = zone_table[lv_zone].real_bottom;
		lv_top = zone_table[lv_zone].real_top;
	}

	/*
	 bzero(buf, sizeof(buf));
	 sprintf(buf, "room: %d zone: %d  bottom: %d top: %d.",
					 lv_room, lv_zone, lv_bottom, lv_top);
	 main_log(buf);
	 */

	/* TRY 100 times to find a valid room */
	for (idx = 0; idx < 100; idx++) {
		lv_location = number(lv_bottom, lv_top);
		if (!li3000_is_blocked_from_room(ch, lv_location, BIT0)) {
			if (lv_location < 0) {
				sprintf(buf, "ERROR: Returning negative room for %s.",
					GET_REAL_NAME(ch));
				main_log(buf);
			}
			return (lv_location);
		}
	}			/* END OF for loop */

	/* For want of anything better, return current room */
	return (lv_room);

}				/* END OF ma1000_do_get_random_room() */

int magic_fails(struct char_data * ch, struct char_data * tar_ch)
{



	if (GET_LEVEL(ch) >= IMO_IMP)
		return FALSE;

	/* check for magic resistance/wards/immunity */
	if ((tar_ch) &&
	    (IS_AFFECTED(tar_ch, AFF_MAGIC_IMMUNE))) {
		if (ch) {
			send_to_char("The magic is nullified somehow.\r\n", ch);
		}
		if (tar_ch) {
			send_to_char("Reality flickers briefly.\r\n", tar_ch);
		}
		return TRUE;
	}

	if ((tar_ch) &&
	    (IS_AFFECTED(tar_ch, AFF_MAGIC_RESIST)) &&
	    (number(0, 101) < GET_LEVEL(tar_ch) + 25)) {
		if (ch) {
			send_to_char("The magic fizzles unspectacularly.\n\r", ch);
		}
		if (tar_ch) {
			send_to_char("Reality flickers.\r\n", tar_ch);
		}
		return TRUE;
	}

	return FALSE;

}				/* END OF magic_fails() */


/* Offensive Spells */
/* missel - for bad spellers */
void spell_magic_missile(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj){
	int dam;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();


	if (magic_fails(ch, victim))
		return;

	dam = dice(MINV(level, 5), 6) + 2 * level;

	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	/* DOES PERSON HAVE AN ITEM OF SPELL ABSORPTION */
	lv_obj = ot4000_check_spell_store_item(victim, SPELL_MAGIC_MISSILE);

	if (lv_obj &&
	    !number(0, 3)) {
		sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
		send_to_char(buf, victim);
		lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
	}
	if (GET_CLASS(ch) == CLASS_BARD && GET_LEVEL(ch) >= 30) {
		if (CHANCE100(25)) {
			send_to_char("&P*&pMISS&PILE MAD&pNESS&P*\n\r", ch);
			DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
			if (CHANCE100(100)) {
				DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
				if (CHANCE100(50)) {
					DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
					if (CHANCE100(50)) {
						DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
						if (CHANCE100(50)) {
							DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
							if (CHANCE100(25)) {
								DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
								if (CHANCE100(25)) {
									DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
									if (CHANCE100(20)) {
										DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
										if (CHANCE100(15)) {
											DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
											if (CHANCE100(10)) {
												DAMAGE(ch, victim, dam * (GET_INT(ch) / number(5, 10)), SPELL_MAGIC_MISSILE);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
	}
	else {
		DAMAGE(ch, victim, dam, SPELL_MAGIC_MISSILE);
	}

	return;

}				/* END OF spell_magic_missile() */

void spell_chill_touch(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
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

	dam = dice(MINV(level >> 1, 10), 8) + MINV((level + 1) >> 1, 8);

	if (!saves_spell(ch, victim, SAVING_SPELL)) {
		af.type = SPELL_CHILL_TOUCH;
		af.duration = 6;
		af.modifier = -2;
		af.location = APPLY_STR;
		af.bitvector = 0;
		ha1400_affect_join(victim, &af, TRUE, FALSE);
	}
	else {
		dam >>= 1;
	}
	DAMAGE(ch, victim, dam, SPELL_CHILL_TOUCH);
}

void spell_burning_hands(sbyte level, struct char_data * ch,
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

	dam = dice(MINV(level >> 1, 12), 12) + 20;

	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	DAMAGE(ch, victim, dam, SPELL_BURNING_HANDS);
}				/* END OF spell_burning_hands() */

void spell_shocking_grasp(sbyte level, struct char_data * ch,
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

	dam = dice(MINV(level >> 1, 15), 8) + level;

	DAMAGE(ch, victim, dam, SPELL_SHOCKING_GRASP);
}				/* END OF spell_shocking_grasp() */

void spell_lightning_bolt(sbyte level, struct char_data * ch,
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

	dam = dice(MINV(level, 20), 6) + 20;

	if (saves_spell(ch, victim, SAVING_SPELL))
		dam >>= 1;

	DAMAGE(ch, victim, dam, SPELL_LIGHTNING_BOLT);
}

void spell_colour_spray(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
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

	dam = dice(MINV(level, 22), 8) + level;

	if (saves_spell(ch, victim, SAVING_SPELL) &&
	    !(af.bitvector = AFF_BLIND) &&
	    (GET_LEVEL(victim) < IMO_LEV)) {
		dam >>= 1;
		act("$n seems to be blinded!", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You have been blinded!\n\r", victim);
		af.type = SPELL_BLINDNESS;
		af.location = APPLY_HITROLL;
		af.modifier = -4;	/* Make hitroll worse */
		af.duration = 1;
		af.bitvector = AFF_BLIND;
		ha1300_affect_to_char(victim, &af);
		af.location = APPLY_AC;
		af.modifier = +40;	/* Make AC Worse! */
		ha1300_affect_to_char(victim, &af);
	}
	DAMAGE(ch, victim, dam, SPELL_COLOUR_SPRAY);
}				/* end of spell_colour_spary() */

/* Drain XP, MANA, HP - caster gains HP and MANA */
void spell_energy_drain(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	int dam, xp, mana;
	//int cmd = CMD_CAST;



	assert(victim && ch);
	if (magic_fails(ch, victim))
		return;


	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (!saves_spell(ch, victim, SAVING_SPELL)) {
		GET_ALIGNMENT(ch) = MINV(-1000, GET_ALIGNMENT(ch) - 200);
		if (GET_LEVEL(victim) <= 5) {
			DAMAGE(ch, victim, 150, SPELL_ENERGY_DRAIN);	/* Kill the sucker */
		}
		else {
			xp = number(level >> 1, level) * 1000;
			li2200_gain_exp(victim, -xp);

			dam = dice(1, MINV(15, GET_LEVEL(ch)));

			mana = GET_MANA(victim) >> 1;
			GET_MOVE(victim) >>= 1;
			GET_MANA(victim) = mana;

			GET_MANA(ch) += mana >> 1;
			GET_HIT(ch) += dam;

			send_to_char("Your life energy is drained!\n\r", victim);
			DAMAGE(ch, victim, dam, SPELL_ENERGY_DRAIN);
		}
	}
	else {
		DAMAGE(ch, victim, 0, SPELL_ENERGY_DRAIN);	/* Miss */
	}
}

void spell_fireball(sbyte level, struct char_data * ch,
		      struct char_data * victim, struct obj_data * obj){

	int dam, adj_dam;
	int do_attack;
	//char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_victim, *temp, *leader;
	//struct obj_data *lv_obj;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */

	dam = dice(MINV(level, 25), 20) + 4 * level;

	if (number(1, 101) <= 5 &&
	    GET_CLASS(ch) == CLASS_MAGIC_USER &&
	    GET_LEVEL(ch) >= 30) {
		/* Random critical fire damage for mages,Bingo 9 - 19 - 01 */
				dam *= 2;
		send_to_char("&R*&rF&YL&rA&RM&YE &rR&RU&YS&rH&R*&n ", ch);
	}
	
	if (number(1, 101) <= 5 &&
	    GET_CLASS(ch) == CLASS_ELDRITCH &&
	    GET_LEVEL(ch) >= 30) {
		/* Random critical fire damage for eldritch knight,Pythias 8 - 13 - 2020 */
				dam *= 1.5;
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
			/* DOES PERSON HAVE AN ITEM OF SPELL ABSORPTION */

			DAMAGE(ch, tmp_victim, adj_dam, SPELL_FIREBALL);

			/* WE'VE HIT ONE PERSON, IF NOT INDOORS GET OUT */
			if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS)) {
				return;
			}

		}		/* END OF do_attack */

	}			/* END OF for loop */

}				/* END OF spell_fireball() */

void spell_turn_undead(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	int dam;
	//int cmd = CMD_CAST;



	assert(victim && ch);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (IS_NPC(victim) &&
	    IS_SET(victim->specials.act, PLR1_CLASS_UNDEAD)) {
		dam = dice(level, 4) + level;
		DAMAGE(ch, victim, dam, SPELL_TURN_UNDEAD);
	}
	else {
		DAMAGE(ch, victim, 0, SPELL_TURN_UNDEAD);
	}
}

void spell_tremor(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj){

	int dam;
	int do_attack;
	struct char_data *tmp_victim, *temp, *leader;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */
	dam = dice(1, 8) + level;

	send_to_char("The earth trembles beneath your feet!\n\r", ch);
	act("$n makes the earth tremble and shiver!", TRUE, ch, 0, 0, TO_ROOM);

	if (ch->master)
		leader = ch->master;
	else
		leader = ch;

	/* SIMILAR TO EARTHQUAKE, BUT JUST PEOPLE IN THE ROOM */
	for (tmp_victim = world[ch->in_room].people; tmp_victim;
	     tmp_victim = temp) {
		temp = tmp_victim->next_in_room;
		if ((ch->in_room == tmp_victim->in_room)) {
			do_attack = TRUE;
			if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK FREE MOBS */
			    IS_NPC(tmp_victim) &&
			    !IS_AFFECTED(tmp_victim, AFF_CHARM))
				do_attack = FALSE;
			if (ch == tmp_victim)	/* don't attack ourselves */
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
			if (do_attack == TRUE)
				DAMAGE(ch, tmp_victim, dam, SPELL_EARTHQUAKE);
		}
		else if (world[ch->in_room].zone == world[tmp_victim->in_room].zone)
			send_to_char("The earth trembles and shivers.", tmp_victim);
	}

}				/* END OF spell_tremor() */

void spell_earthquake(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){

	int dam;
	int do_attack;
	struct char_data *tmp_victim, *temp, *leader;



	assert(ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	/* CHECK_FOR_CHARM(); CHECK_FOR_PK(); */
	dam = dice(7, level);

	send_to_char("The earth shakes beneath your feet!\n\r", ch);
	act("$n makes the earth shake and tremble!", TRUE, ch, 0, 0, TO_ROOM);

	if (ch->master)
		leader = ch->master;
	else
		leader = ch;

	/* WE HAVE TO DO THE WHOLE WORLD SO WE CAN AGGRAVATE PLAYERS IN THE
	 * ZONE. */
	for (tmp_victim = character_list; tmp_victim; tmp_victim = temp) {
		temp = tmp_victim->next;
		if ((ch->in_room == tmp_victim->in_room)) {
			do_attack = TRUE;
			if (IS_NPC(ch) &&	/* MOBS DON'T ATTACK FREE MOBS */
			    IS_NPC(tmp_victim) &&
			    !IS_AFFECTED(tmp_victim, AFF_CHARM))
				do_attack = FALSE;
			if (ch == tmp_victim)	/* don't attack ourselves */
				do_attack = FALSE;
			if (ch->master == tmp_victim)	/* don't attack our
							 * leader  */
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
			if (IS_AFFECTED(tmp_victim, AFF_FLY)) {
				send_to_char("You are flying.  The earthquake has no effect on you.\n\r", tmp_victim);
				return;
			}
			if (magic_fails(ch, tmp_victim))
				do_attack = FALSE;
			if (do_attack == TRUE)
				DAMAGE(ch, tmp_victim, dam, SPELL_EARTHQUAKE);
		}
		else if (world[ch->in_room].zone == world[tmp_victim->in_room].zone)
			send_to_char("The earth shakes and trembles.", tmp_victim);
	}

}				/* END OF spell_earthquake() */

void spell_dispel_evil(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	int dam;
	//int cmd = CMD_CAST;



	assert(ch && victim);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	if (IS_EVIL(ch))
		victim = ch;
	else if (IS_GOOD(victim)) {
		act("God protects $N.", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if ((GET_LEVEL(victim) < level) || (victim == ch))
		dam = 100;
	else {
		dam = dice(level, 4);
		if (saves_spell(ch, victim, SAVING_SPELL))
			dam >>= 1;
	}

	DAMAGE(ch, victim, dam, SPELL_DISPEL_EVIL);
}

void spell_hail_storm(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){
	int dam;
	//int cmd = CMD_CAST;

	extern struct weather_data weather_info;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	dam = dice(MINV(level, 15), 8);

	if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {

		if (saves_spell(ch, victim, SAVING_SPELL))
			dam >>= 1;

		DAMAGE(ch, victim, dam, SPELL_HAIL_STORM);
	}
}

void spell_harm(sbyte level, struct char_data * ch,
		  struct char_data * victim, struct obj_data * obj){

	char buf[MAX_STRING_LENGTH];
	int lv_hits, dam;
	//int cmd = CMD_CAST;



	assert(victim && ch);
	assert((level >= 1) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	lv_hits = GET_HIT(victim);
	dam = GET_HIT(victim) - dice(1, 4);
	dam = MINV(100, dam);

	if (dam < 0) {
		dam = 0;
	}
	else {
		if (saves_spell(ch, victim, SAVING_SPELL))
			dam = MINV(20, dam / 2);
	}

	if (GET_HIT(victim) < 1) {
		send_to_char("Nothing happens.\r\n", ch);
		send_to_char("You feel a deathly tingle.\r\n", victim);
		return;
	}

	DAMAGE(ch, victim, dam, SPELL_HARM);

	sprintf(buf, "You happily rain destruction on %s\r\n",
		GET_REAL_NAME(victim));
	send_to_char(buf, ch);

}				/* END OF spell_harm() */

void spell_conflagration(sbyte level, struct char_data * ch, struct char_data * victim, struct obj_data * obj){
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

	dam = dice(level, 12);
	dam = MINV(290, dam);

	DAMAGE(ch, victim, dam, SPELL_CONFLAGRATION);
	act("$n heavily burns you, ouch!", TRUE, ch, 0, victim, TO_VICT);
	act("You heavily burn $N!", TRUE, ch, 0, victim, TO_CHAR);
	return;

}				/* END OF spell_conflagration() */

/* spells2.c - Not directly offensive spells */
void spell_armor(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (!ha1375_affected_by_spell(victim, SPELL_ARMOR)) {
		af.type = SPELL_ARMOR;
		af.duration = 24;
		af.modifier = -1 * (MINV(20, level * 2));
		af.location = APPLY_AC;
		af.bitvector = 0;

		ha1300_affect_to_char(victim, &af);
		if (ch == victim)
			send_to_char("You feel protected.\n\r", victim);
		else
			send_to_char("You feel someone protecting you.\n\r", victim);

		act("$n glows white for a moment.", FALSE, victim, 0, 0, TO_ROOM);
	}
}

void spell_elsewhere(sbyte level, struct char_data * chr,
		       struct char_data * victim, struct obj_data * obj){
	int to_room;
	struct char_data *ch;



	assert(chr);

	if (!victim)
		ch = chr;
	else
		ch = victim;

	if (ch->in_room == JAIL_ROOM) {
		send_to_char("Sorry, you can't do that while in jail.\r\n", ch);
		return;
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT) && !IS_NPC(ch)) {
		send_to_char("You temporarily feel like you're everywhere at once!\r\n", victim);
		return;
	}

	to_room = number(0, top_of_world);
	while (IS_SET(world[to_room].room_flags, RM1_SOUNDPROOF) ||
	       li3000_is_blocked_from_room(ch, to_room, BIT0)) {
		to_room = number(0, top_of_world);
	}

	if (to_room == ch->in_room ||
	    ha9800_protect_char_from_dt(ch, to_room, 0)) {
		act("$n shimmers.", FALSE, ch, 0, 0, TO_ROOM);
		return;
	}

	/* nothing is going to stop our leaving.  so if fighting... */
	if (ch->specials.fighting) {
		if (cl1900_at_war(ch, ch->specials.fighting))
			cl2000_player_escape(ch, ch->specials.fighting);
		at2000_do_mob_hunt_check(ch->specials.fighting, ch, 1);
	}


	if (gv_port == ZONING_PORT) {
		if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[to_room].zone].lord) &&
		    GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to go into that zone!\n\r", ch);
			return;
		}
	}

	act("$n suddenly winks out of existence.", FALSE, ch, 0, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, to_room);
	act("$n suddenly winks into existence.", FALSE, ch, 0, 0, TO_ROOM);

	in3000_do_look(ch, "", 0);

	if (IS_PC(ch) &&
	    IS_SET(world[ch->in_room].room_flags, RM1_DEATH)) {
		ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_TELEPORT);
		return;
	}

	if (IS_NPC(ch) &&
	    IS_SET(world[ch->in_room].room_flags, RM1_NO_MOB)) {
		send_to_char("&RYou aren't supposed to be here!&n\n\rCasting elsewhere again to leave No_MOB room.\n\r", ch);
		spell_elsewhere(48, ch, ch, 0);
		return;
	}

	if (IS_NPC(ch) &&
	    IS_SET(world[ch->in_room].room_flags, RM1_DEATH)) {
		send_to_char("&RNo no no stay out of here!&n\n\rCasting elsewhere again to leave dt fast.\n\r", ch);
		spell_elsewhere(48, ch, ch, 0);
		return;
	}

	return;

}				/* END OF spell_elsewhere() */

void spell_bless(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	assert(ch && (victim || obj));
	assert((level >= 0) && (level <= NPC_LEV));

	if (obj) {
		if ((5 * GET_LEVEL(ch) > GET_OBJ_WEIGHT(obj)) &&
		    (GET_POS(ch) != POSITION_FIGHTING) &&
		    !IS_OBJ_STAT(obj, OBJ1_MAGIC) &&
		    (GET_ITEM_TYPE(obj) == ITEM_QUEST ||
		     GET_ITEM_TYPE(obj) == ITEM_ARMOR)) {
			SET_BIT(obj->obj_flags.flags1, OBJ1_BLESS);
			SET_BIT(obj->obj_flags.flags1, OBJ1_MAGIC);
			act("$p briefly glows.", FALSE, ch, obj, 0, TO_CHAR);
			if (IS_GOOD(ch)) {
				SET_BIT(obj->obj_flags.flags1, OBJ1_ANTI_EVIL);
				SET_BIT(obj->obj_flags.flags1, OBJ1_ANTI_NEUTRAL);
				OVAL0(obj) += 2;
			}
			else {
				OVAL0(obj) += 1;
			}
		}
	}
	else {
		if ((GET_POS(victim) != POSITION_FIGHTING) &&
		    (!ha1375_affected_by_spell(victim, SPELL_BLESS) &&
		     !ha1375_affected_by_spell(victim, SPELL_VEX))) {

			if (magic_fails(ch, victim))
				return;
			send_to_char("You feel righteous.\n\r", victim);
			af.type = SPELL_BLESS;
			af.duration = 6;
			if (GET_ALIGNMENT(ch) >= 400) {
				af.modifier = GET_ALIGNMENT(ch) / 200;
			}
			else {
				af.modifier = 3;
			}
			af.location = APPLY_HITROLL;
			af.bitvector = 0;
			ha1300_affect_to_char(victim, &af);

			af.location = APPLY_SAVING_SPELL;
			af.modifier = -1;	/* Make better */
			ha1300_affect_to_char(victim, &af);
		}
	}

}				/* END OF spell_bless */

void spell_blindness(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj){

	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;


	if (GET_LEVEL(ch) < IMO_IMP) {
		if (GET_LEVEL(victim) >= IMO_LEV) {
			act("$N shrugs off your spell.", FALSE, ch, 0, victim, TO_CHAR);
			act("$n tried to blind you. *laugh*", FALSE, ch, 0, victim, TO_VICT);
			return;
		}
	}

	if (saves_spell(ch, victim, SAVING_SPELL)) {
		act("$N shrugs off your spell with an effort of will.", FALSE, ch, 0, victim, TO_CHAR);
		send_to_char("The rooms dims for a second.\r\n", victim);
		return;
	}

	if (ha1375_affected_by_spell(victim, SPELL_BLINDNESS)) {
		send_to_char("Nothing appears to happen.\r\n", ch);
		send_to_char("You just felt something, but your condition is the same.\r\n", victim);
		return;
	}

	act("$n seems to be blinded!", TRUE, victim, 0, 0, TO_ROOM);
	send_to_char("You have been blinded!\n\r", victim);

	af.type = SPELL_BLINDNESS;
	af.location = APPLY_HITROLL;
	af.modifier = -4;	/* Make hitroll worse */
	af.duration = 3;
	af.bitvector = AFF_BLIND;
	ha1300_affect_to_char(victim, &af);

	af.location = APPLY_AC;
	af.modifier = +40;	/* Make AC Worse! */
	ha1300_affect_to_char(victim, &af);
	act("You successfully blind $N.", FALSE, ch, 0, victim, TO_CHAR);

}				/* END OF spell_blindness */

void spell_control_weather(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){
	/* Control Weather is not possible here!!! */
	/* Better/Worse can not be transferred     */
}

void spell_create_food(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	struct obj_data *tmp_obj;



	assert(ch);
	assert((level >= 0) && (level <= NPC_LEV));

	CREATE(tmp_obj, struct obj_data, 1);
	db7600_clear_object(tmp_obj);

	tmp_obj->name = (char *) strdup("mushroom");
	tmp_obj->short_description = strdup("A Magic Mushroom");
	tmp_obj->description = strdup("A really delicious looking magic mushroom lies here.");

	tmp_obj->obj_flags.type_flag = ITEM_FOOD;
	tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
	tmp_obj->obj_flags.value[0] = 5 + level;
	tmp_obj->obj_flags.weight = 1;
	tmp_obj->obj_flags.cost = 10;
	tmp_obj->obj_flags.cost_per_day = 1;

	tmp_obj->next = object_list;
	object_list = tmp_obj;

	ha2100_obj_to_room(tmp_obj, ch->in_room);

	tmp_obj->item_number = -1;

	act("$p suddenly appears.", TRUE, ch, tmp_obj, 0, TO_ROOM);
	act("$p suddenly appears.", TRUE, ch, tmp_obj, 0, TO_CHAR);
}				/* END OF spell_create_food() */

void spell_sustenance(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (races[GET_RACE(victim)].adj_food == 0) {
		victim->specials.conditions[FOOD] = -1;
	}
	else {
		victim->specials.conditions[FOOD] =
			MAXV(victim->specials.conditions[FOOD],
			     races[GET_RACE(victim)].max_food);
	}

	if (races[GET_RACE(victim)].adj_thirst == 0) {
		victim->specials.conditions[THIRST] = -1;
	}
	else {
		victim->specials.conditions[THIRST] =
			MAXV(victim->specials.conditions[THIRST],
			     races[GET_RACE(victim)].max_thirst);
	}

	/* THIS IS THE CURE WE'VE BEEN WAITING FOR!!! */
	victim->specials.conditions[DRUNK] = 0;

	if (GET_LEVEL(victim) >= IMO_LEV) {
		victim->specials.conditions[FOOD] = -1;
		victim->specials.conditions[THIRST] = -1;
		victim->specials.conditions[DRUNK] = 0;
	}
	act("You feel your hunger and thirst decrease.", TRUE, victim, 0, 0, TO_CHAR);
	act("$n glows yellow for a moment.", FALSE, victim, 0, 0, TO_ROOM);

	return;

}				/* END OF spell_sustenance() */

void spell_create_water(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){

	int water;

	extern struct weather_data weather_info;
	void name_to_drinkcon(struct obj_data * obj, int type);
	void name_from_drinkcon(struct obj_data * obj);



	assert(ch && obj);

	if (GET_ITEM_TYPE(obj) == ITEM_DRINKCON) {
		if ((obj->obj_flags.value[2] != LIQ_WATER)
		    && (obj->obj_flags.value[1] != 0)) {

			name_from_drinkcon(obj);
			obj->obj_flags.value[2] = LIQ_SLIME;
			name_to_drinkcon(obj, LIQ_SLIME);

		}
		else {

			water = 2 * level * ((weather_info.sky >= SKY_RAINING) ? 2 : 1);

			/* Calculate water it can contain, or water created */
			water = MINV(obj->obj_flags.value[0] - obj->obj_flags.value[1], water);

			if (water > 0) {
				obj->obj_flags.value[2] = LIQ_WATER;
				obj->obj_flags.value[1] += water;

				/*
				 weight_change_object(obj, water);
				 */

				name_from_drinkcon(obj);
				name_to_drinkcon(obj, LIQ_WATER);
				act("$p is filled.", FALSE, ch, obj, 0, TO_CHAR);
			}
		}
	}
}				/* END OF spell_create_water() */

void spell_cure_blind(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (!ha1375_affected_by_spell(victim, SPELL_BLINDNESS)) {
		send_to_char("You're not impressed with the results.\r\n", ch);
		if (ch != victim)
			send_to_char("You feel a tingle.\r\n", victim);
		return;
	}

	ha1350_affect_from_char(victim, SPELL_BLINDNESS);

	send_to_char("Your vision returns!\n\r", victim);
	if (victim != ch) {
		act("$N's vision returns.", FALSE, ch, 0, victim, TO_CHAR);
	}

}				/* end of spell_cure_blind() */

void spell_cure_critic(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	int healpoints;
	int max_points;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (number(0, 101) <= 12 &&
	    GET_CLASS(ch) == CLASS_PRIEST &&
	    GET_LEVEL(ch) >= 30) {
		/* Improved cure critic for Priests, Bingo 9 - 19 - 01 */
				healpoints = dice(number(3, 18), number(8, 48)) + number(GET_WIS(ch), GET_INT(ch) * 2);
		send_to_char("&B*&CC&cHARGE&CD&B*&n ", ch);
	}
	else {
		healpoints = dice(3, 8) + 3;
	}

	max_points = li1500_hit_limit(victim);
	if ((healpoints + GET_HIT(victim)) > li1500_hit_limit(victim))
		GET_HIT(victim) = li1500_hit_limit(victim);
	else
		GET_HIT(victim) += healpoints;

	send_to_char("You feel better!\n\r", victim);
	act("$n starts to look a little better.", FALSE, victim, 0, 0, TO_ROOM);

	ft1200_update_pos(victim);
}

void spell_donate_mana(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));



	if (magic_fails(ch, victim))
		return;

	if ((15 + GET_MANA(victim)) > GET_SPECIAL_MANA(victim))
		GET_MANA(victim) = GET_SPECIAL_MANA(victim);
	else
		GET_MANA(victim) += 15;

	send_to_char("You feel mana flow into you!\n\r", victim);
	if (victim != ch)
		act("You feel some of your mana flow to $N.", FALSE, ch, 0, victim, TO_CHAR);
}
/*
void spell_mana_link(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));



	if (magic_fails(ch, victim))
		return;

	if ((15 + GET_MANA(victim)) > GET_SPECIAL_MANA(victim))
		GET_MANA(victim) = GET_SPECIAL_MANA(victim);
	else
		GET_MANA(victim) += 15;

	send_to_char("You feel mana flow into you!\n\r", victim);
	if (victim != ch)
		act("You feel some of your mana flow to $N.", FALSE, ch, 0, victim, TO_CHAR);
}
*/
void spell_cause_critic(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	int healpoints;
	//int cmd = CMD_CAST;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	healpoints = dice(3, 8) + 3;
	DAMAGE(ch, victim, healpoints, SPELL_CAUSE_CRITIC);

	ft1200_update_pos(victim);
}

void spell_cure_light(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){
	int healpoints;



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	healpoints = dice(1, 8);

	if ((healpoints + GET_HIT(victim)) > li1500_hit_limit(victim))
		GET_HIT(victim) = li1500_hit_limit(victim);
	else
		GET_HIT(victim) += healpoints;

	ft1200_update_pos(victim);

	send_to_char("You feel slightly better!\n\r", victim);
	act("$n looks slightly better.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_cause_light(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){
	int healpoints;
	//int cmd = CMD_CAST;



	assert(ch && victim);
	assert((level >= 0) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (magic_fails(ch, victim))
		return;

	healpoints = dice(1, 8) + 1;
	DAMAGE(ch, victim, healpoints, SPELL_CAUSE_LIGHT);

	ft1200_update_pos(victim);
}

void spell_curse(sbyte level, struct char_data * ch,
		   struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(victim || obj);
	assert((level >= 0) && (level <= NPC_LEV));

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();


	if (obj) {
		SET_BIT(obj->obj_flags.flags1, OBJ1_CURSED);

		/* LOWER ATTACK DICE BY -1 */
		if (obj->obj_flags.type_flag == ITEM_WEAPON ||
		    obj->obj_flags.type_flag == ITEM_QSTWEAPON) {
			obj->obj_flags.value[2]--;
			act("$p glows red.", FALSE, ch, obj, 0, TO_CHAR);
		}
	}
	else {
		if (magic_fails(ch, victim))
			return;
		CHECK_FOR_CHARM();
		CHECK_FOR_PK();
		if (saves_spell(ch, victim, SAVING_SPELL) ||
		    ha1375_affected_by_spell(victim, SPELL_CURSE)) {
			return;
		}

		af.type = SPELL_CURSE;
		af.duration = 24 * 7;	/* 7 Days */
		af.modifier = -1;
		af.location = APPLY_HITROLL;
		af.bitvector = AFF_CURSE;
		ha1300_affect_to_char(victim, &af);

		af.location = APPLY_SAVING_PARA;
		af.modifier = 1;/* Make worse */
		ha1300_affect_to_char(victim, &af);

		act("$n is surrounded by a red aura!", FALSE, victim, 0, 0, TO_ROOM);
		act("You feel very uncomfortable.", FALSE, victim, 0, 0, TO_CHAR);
	}
}

void spell_detect_evil(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_DETECT_EVIL))
		return;

	af.type = SPELL_DETECT_EVIL;
	af.duration = level * 5;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_DETECT_EVIL;

	ha1300_affect_to_char(victim, &af);

	send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_detect_good(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_DETECT_GOOD))
		return;

	af.type = SPELL_DETECT_GOOD;
	af.duration = level * 5;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = 0;

	ha1300_affect_to_char(victim, &af);

	send_to_char("Your eyes tingle.\n\r", victim);

}				/* end of spell_detect_good */

void spell_detect_invisibility(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_DETECT_INVISIBLE))
		return;

	af.type = SPELL_DETECT_INVISIBLE;
	af.duration = level * 5;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_DETECT_INVISIBLE;

	ha1300_affect_to_char(victim, &af);

	send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_detect_magic(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_DETECT_MAGIC))
		return;

	af.type = SPELL_DETECT_MAGIC;
	af.duration = level * 5;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_DETECT_MAGIC;

	ha1300_affect_to_char(victim, &af);
	send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_breathwater(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_BREATHWATER))
		return;

	af.type = SPELL_BREATHWATER;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_BREATHWATER;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You breath easier.\n\r", victim);
	if (victim != ch)
		act("$N starts to breath a little easier.", FALSE, ch, 0, victim, TO_CHAR);
}

void spell_darksight(sbyte level, struct char_data * ch,
		       struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_DARKSIGHT)) {
		send_to_char("Nothing new happens.\n\r", ch);
		return;
	}

	af.type = SPELL_DARKSIGHT;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_DARKSIGHT;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You see easier.\n\r", victim);
	act("$n's eyes begin to glow with an eerie light.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_regeneration(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_REGENERATION))
		return;

	af.type = SPELL_REGENERATION;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_REGENERATION;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You begin to regenerate.\n\r", victim);
	act("$n glows white for a moment.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_dispel_magic(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){



	if (!victim) {
		main_log("ERROR: spell_dispel_magic called without victim.");
		return;
	}

	if (magic_fails(ch, victim)) {
		return;
	}

	if (ch == victim) {
		ha1370_remove_all_spells(ch);
		send_to_char("You chant some arcane words and dispel all magic from your body.\r\n", ch);
		act("$n makes several strange gestures around $s body.", TRUE, ch, 0, 0, TO_ROOM);
		act("A bright rainbow colored aura rises around $n.", TRUE, ch, 0, 0, TO_ROOM);
		act("The aura goes back down and dispels all $s magic.", TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		send_to_char("You cannot dispel the magic of others!\r\n", ch);
	}
	return;

}				/* END OF spell_dispel_magic() */

void spell_magic_resist(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();

	if (GET_LEVEL(ch) < IMO_IMM) {
		if ((IS_PC(ch)) && (IS_PC(victim))) {
			if ((GET_LEVEL(ch) < gv_pkill_level) || (GET_LEVEL(victim) < gv_pkill_level)) {
				if (!cl1900_at_war(ch, victim)) {
					if (!IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA)) {
						if ((!IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)) ||
						    (!IS_SET(GET_ACT2(victim), PLR2_PKILLABLE)) ||
						    (IS_AFFECTED(ch, AFF_TRANSFORMED)) ||
						    (IS_AFFECTED(victim, AFF_TRANSFORMED))) {
							send_to_char("You can't do that, the gods wont let you! (help PKILLABLE for more info).\n\r", ch);
							return;
						}
					}
				}
			}
		}
	}

	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_MAGIC_RESIST))
		return;

	af.type = SPELL_MAGIC_RESIST;
	af.duration = level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_MAGIC_RESIST;

	ha1300_affect_to_char(victim, &af);
	send_to_char("You begin to resist the effects of magic.\n\r", victim);
	act("$n glows black for a moment.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_magic_immune(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));

	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();

	if (GET_LEVEL(ch) < IMO_IMM) {
		if ((IS_PC(ch)) && (IS_PC(victim))) {
			if ((GET_LEVEL(ch) < gv_pkill_level) || (GET_LEVEL(victim) < gv_pkill_level)) {
				if (!cl1900_at_war(ch, victim)) {
					if (!IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA)) {
						if ((!IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)) ||
						    (!IS_SET(GET_ACT2(victim), PLR2_PKILLABLE)) ||
						    (IS_AFFECTED(ch, AFF_TRANSFORMED)) ||
						    (IS_AFFECTED(victim, AFF_TRANSFORMED))) {
							send_to_char("You can't do that, the gods wont let you! (help PKILLABLE for more info).\n\r", ch);
							return;
						}
					}
				}
			}
		}
	}

	if (magic_fails(ch, victim))
		return;

	if (ch != victim) {
		if (saves_spell(ch, victim, SAVING_SPELL)) {
			send_to_char("You failed!\r\n", ch);
			return;
		}
	}

	if (ha1375_affected_by_spell(victim, SPELL_MAGIC_IMMUNE))
		return;

	af.type = SPELL_MAGIC_IMMUNE;
	af.duration = level >> 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_MAGIC_IMMUNE;

	ha1300_affect_to_char(victim, &af);
	send_to_char("All magic near you now fails.\n\r", victim);
	act("$n glows black for a moment.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_restoration(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){



	/*
	 send_to_char("This spell doesn't work any more *cackle*\r\n", ch);
	 return;
	 */
	assert(victim);
	assert((level >= 0) && (level <= NPC_LEV));
	if (magic_fails(ch, victim))
		return;
	if (IS_NPC(victim)) {
		send_to_char("Sorry, mobs can't restore themselves.\r\n", ch);
		return;
	}

	if (GET_LEVEL(victim) > 2 && GET_LEVEL(victim) < IMO_LEV)
		GET_EXP(victim) =
			MAXV(GET_EXP(victim), LEVEL_EXP(GET_LEVEL(victim) - 1) + 1);

	send_to_char("You have been restored.\n\r", victim);
	act("$n is surrounded by white light for a moment.", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_detect_poison(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj){
	assert(ch && (victim || obj));



	if (victim) {
		if (victim == ch)
			if (IS_AFFECTED(victim, AFF_POISON))
				send_to_char("You can sense poison in your blood.\n\r", ch);
			else
				send_to_char("You feel healthy.\n\r", ch);
		else if (IS_AFFECTED(victim, AFF_POISON)) {
			act("You sense that $E is poisoned.", FALSE, ch, 0, victim, TO_CHAR);
		}
		else {
			act("You sense that $E is healthy.", FALSE, ch, 0, victim, TO_CHAR);
		}
	}
	else {			/* It's an object */
		if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
		    (obj->obj_flags.type_flag == ITEM_FOOD)) {
			if (obj->obj_flags.value[3])
				act("Poisonous fumes are revealed.", FALSE, ch, 0, 0, TO_CHAR);
			else
				send_to_char("It looks very delicious.\n\r", ch);
		}
	}
}

void spell_enchant_weapon(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){
	int i;



	assert(ch && obj);
	assert(MAX_OBJ_AFFECT >= 2);

	if ((GET_ITEM_TYPE(obj) != ITEM_WEAPON) &&
	    (GET_ITEM_TYPE(obj) != ITEM_QSTWEAPON)) {
		send_to_char("You can only enchant weapons.\r\n", ch);
		return;
	}

	if (IS_SET(obj->obj_flags.flags1, OBJ1_MAGIC)) {
		send_to_char("You can't enchant magical items.\r\n", ch);
		return;
	}

	/* IF WEAPON HAS ANY EXISTING APPLYs, GET OUT */
	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		if (obj->affected[i].location != APPLY_NONE) {
			send_to_char("The weapon resists your enchantment!\r\n", ch);
			return;
		}
	}

	SET_BIT(obj->obj_flags.flags1, OBJ1_MAGIC);

	obj->affected[0].location = APPLY_HITROLL;
	if (GET_LEVEL(ch) < 18)
		obj->affected[0].modifier = 1;
	else if (GET_LEVEL(ch) < IMO_LEV)
		obj->affected[0].modifier = 2;
	else if (GET_LEVEL(ch) < IMO_IMP)
		obj->affected[0].modifier = 3;
	else {
		send_to_char("WOW!  You really put some energy into that one.\r\n", ch);
		obj->affected[0].modifier = 5;
	}

	obj->affected[1].location = APPLY_DAMROLL;
	if (GET_LEVEL(ch) < 28)
		obj->affected[1].modifier = 1;
	else if (GET_LEVEL(ch) < IMO_LEV)
		obj->affected[1].modifier = 2;
	else if (GET_LEVEL(ch) < IMO_IMP)
		obj->affected[1].modifier = 3;
	else {
		send_to_char("WOW!  You really put some energy into that one.\r\n", ch);
		obj->affected[1].modifier = 5;
	}

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

}				/* END OF spell_enchant() */

void do_spell_heal(sbyte level, struct char_data * ch,
	        struct char_data * victim, struct obj_data * obj, int lv_flag){

	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;
	int healpts;



	assert(victim);
	if (magic_fails(ch, victim))
		return;

	if (ha1375_affected_by_spell(victim, SPELL_BLINDNESS)) {
		spell_cure_blind(level, ch, victim, obj);
	}
	spell_remove_poison(level, ch, victim, obj);

	if (lv_flag == 1) {
		healpts = GET_LEVEL(ch) == 48 ? number(0, 300) : 100;
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_CLERIC && GET_LEVEL(ch) >= 30) {
			//Chance of double heals for clerics, Bingo 9 - 19 - 01
			  healpts *= 2;
			send_to_char
				("&B*&CC&cHARGE&CD&B*&n ", ch);
		}
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_PALADIN && GET_LEVEL(ch) >= 30) {
			/* Chance of 1.5 x heals for paladins, Bingo 9 - 19 - 01 */
					healpts *= 1.25;
			send_to_char("&B*&CC&cHARGE&CD&B*&n ", ch);
		}
		GET_HIT(victim) += healpts;
		lv_obj = ot4000_check_spell_store_item(victim, SPELL_HEAL_MINOR);
	}
	else if (lv_flag == 2) {
		healpts = GET_LEVEL(ch) == 48 ? number(100, 500) : 300;
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_CLERIC && GET_LEVEL(ch) >= 30) {
			/* Chance of double heals for clerics, Bingo 9 - 19 - 01 */
			  healpts *= 2;
			send_to_char
				("&B*&CC&cHARGE&CD&B*&n ", ch);
		}
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_PALADIN && GET_LEVEL(ch) >= 30) {
			/* Chance of 1.5 x heals for paladins, Bingo 9 - 19 - 01 */
					healpts *= 1.25;
			send_to_char("&B*&CC&cHARGE&CD&B*&n ", ch);
		}
		GET_HIT(victim) += healpts;
		lv_obj = ot4000_check_spell_store_item(victim, SPELL_HEAL_MEDIUM);
	}
	else if (lv_flag == 3) {
		healpts = GET_LEVEL(ch) == 48 ? number(300, 700) : 500;
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_CLERIC && GET_LEVEL(ch) >= 30) {
			//Chance of double heals for clerics, Bingo 9 - 19 - 01
			  healpts *= 2;
			send_to_char
				("&B*&CC&cHARGE&CD&B*&n ", ch);
		}
		GET_HIT(victim) += healpts;
		lv_obj = ot4000_check_spell_store_item(victim, SPELL_HEAL_MAJOR);
	}

	else {
		healpts = GET_LEVEL(ch) == 48 ? number(600, 1000) : 700;
		if (number(0, 101) <= 8 && GET_CLASS(ch) == CLASS_CLERIC && GET_LEVEL(ch) >= 30) {
			healpts *= 3;
			//rlum
				send_to_char("&r*&wC&WHARGE&wD&r*&n ", ch);
			GET_MANA(victim) += healpts;
			GET_MOVE(victim) += healpts;
		}
		GET_HIT(victim) += healpts;


		lv_obj = ot4000_check_spell_store_item(victim, SPELL_CONVALESCE);
	}



	/* DOES PERSON HAVE AN ITEM OF SPELL ABSORPTION */
	if (lv_obj &&
	    !number(0, 5)) {
		lv_obj->obj_flags.value[VALUE_CURR_CHARGE]++;
		sprintf(buf, "Your %s hums.", GET_OBJ_NAME(lv_obj));
		send_to_char(buf, victim);
		sprintf(buf, "$n's %s hums.", GET_OBJ_NAME(lv_obj));
		act(buf, FALSE, victim, 0, 0, TO_ROOM);
		return;
	}

	if (GET_HIT(victim) > li1500_hit_limit(victim))
		GET_HIT(victim) = li1500_hit_limit(victim);

	/* SAFTY CHECK */
	if (GET_HIT(victim) < 10) {
		send_to_char("Ack, we can't allow less than 10 hitpoints\r\n", ch);
		GET_HIT(victim) = 10;
	}

	ft1200_update_pos(victim);


	if (lv_flag != 4) {
		send_to_char("A warm feeling fills your body.\n\r", victim);
		act("$n suddenly looks a lot better.", FALSE, victim, 0, 0, TO_ROOM);
	}
	else {
		send_to_char("A surge of life flows through you.\n\r", victim);
		act("&K$n glows with a &Bbl&b&ruish-re&Bd &Kh&Wu&Ke &Kand seems to be invigorated.&e", FALSE, victim, 0, 0, TO_ROOM);

	}
}

void spell_heal_minor(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){



	do_spell_heal(level, ch, victim, obj, 1);
	return;

}				/* END OF Spell_heal_minor() */

void spell_heal_medium(sbyte level, struct char_data * ch,
		         struct char_data * victim, struct obj_data * obj){

	do_spell_heal(level, ch, victim, obj, 2);
	return;

}				/* END OF Spell_heal_medium() */

void spell_heal_major(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){



	do_spell_heal(level, ch, victim, obj, 3);
	return;

}				/* END OF Spell_heal_major() */

void spell_convalesce(sbyte level, struct char_data * ch,
		        struct char_data * victim, struct obj_data * obj){

	//

	do_spell_heal(level, ch, victim, obj, 4);
	return;

}				/* END OF Spell_convalesce() */

void spell_invisibility(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert((ch && obj) || victim);
	if (victim) {
		if (magic_fails(ch, victim))
			return;
	}

	if (obj) {
		if (!IS_SET(obj->obj_flags.flags1, OBJ1_INVISIBLE)) {
			act("$p turns invisible.", FALSE, ch, obj, 0, TO_CHAR);
			act("$p turns invisible.", TRUE, ch, obj, 0, TO_ROOM);
			SET_BIT(obj->obj_flags.flags1, OBJ1_INVISIBLE);
		}
	}
	else {			/* Then it is a PC | NPC */
		if (magic_fails(ch, victim))
			return;
		if (!(IS_SET(victim->specials.affected_by, AFF_INVISIBLE))) {

			act("$n slowly fades out of existence.", TRUE, victim, 0, 0, TO_ROOM);
			send_to_char("You vanish.\n\r", victim);

			af.type = SPELL_INVISIBLE;
			af.duration = 24;
			af.modifier = -40;
			af.location = APPLY_AC;
			af.bitvector = AFF_INVISIBLE;
			ha1300_affect_to_char(victim, &af);
		}
	}
}

void spell_improved_invis(sbyte level, struct char_data * ch,
			    struct char_data * victim, struct obj_data * obj){
	struct affected_type af;



	assert((ch) || victim);
	if (magic_fails(ch, victim))
		return;

	/* Then it is a PC | NPC */
	if ((!ha1375_affected_by_spell(victim, SPELL_INVISIBLE)) &&
	    (!ha1375_affected_by_spell(victim, SPELL_IMPROVED_INVIS))) {

		act("$n slowly fades out of existence.", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You vanish.\n\r", victim);

		af.type = SPELL_IMPROVED_INVIS;
		af.duration = level / 3 + 3;
		af.modifier = -40;
		af.location = APPLY_AC;
		af.bitvector = AFF_INVISIBLE;
		ha1300_affect_to_char(victim, &af);

	}
}

void spell_locate(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj){
	char lv_name[MAX_STRING_LENGTH];



	bzero(lv_name, sizeof(lv_name));
	strcpy(lv_name, ha1100_fname(obj->name));
	spell_locate_object(level, ch, victim, obj, lv_name);
	return;

}				/* END OF spell_locate() */

void spell_locate_object(sbyte level, struct char_data * ch,
	      struct char_data * victim, struct obj_data * obj, char *lv_name){
	struct obj_data *i, *next;
	char buf[MAX_STRING_LENGTH];
	int j, lv_number_found;



	lv_number_found = 0;
	assert(ch);

	/* SHOULDN'T HAPPEN, BUT LETS PREVENT A CRASH */
	if (!*lv_name) {
		send_to_char("You are unable to focus on a particular object.\r\n", ch);
		return;
	}

	if (gv_port == ZONING_PORT) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to use locate object on this port!\n\r", ch);
			return;
		}
	}

	j = level >> 1;
	if (GET_LEVEL(ch) < IMO_IMM)
		if ((!strcmp(lv_name, "obj")) || (!strcmp(lv_name, "ob")) || (!strcmp(lv_name, "o"))) {
			send_to_char("a Booger carried by the Dread Booger Beast.\n\r", ch);
			return;
		}

	for (i = object_list; i && (j > 0); i = next) {
		next = i->next;
		if ((ha1150_isname(lv_name, i->name))) {
			if (!IS_SET(GET_OBJ1(i), OBJ1_NO_LOCATE) ||
			    CAN_SEE_OBJ_NOLOCATE(ch, i)) {
				if (i->carried_by) {
					if (CAN_SEE(ch, i->carried_by)) {
						bzero(buf, sizeof(buf));
						lv_number_found++;
						sprintf(buf, "%s carried by %s.\n\r",
							i->short_description, PERS(i->carried_by, ch));
						send_to_char(buf, ch);
					}	/* end of can_see char */
				}	/* end of carried */


				else if (i->in_obj) {
					bzero(buf, sizeof(buf));
					lv_number_found++;
					sprintf(buf, "%s in %s.\n\r", i->short_description,
						i->in_obj->short_description);
					send_to_char(buf, ch);
				}	/* end of in obj */
				else {
					bzero(buf, sizeof(buf));
					lv_number_found++;
					sprintf(buf, "%s in %s.\n\r", i->short_description,
						(i->in_room == NOWHERE ? "NOWHERE!?!(bug)" : world[i->in_room].name));
					send_to_char(buf, ch);
				}	/* end of not anywhere */
			}	/* end of is nolocate */
			j--;
		}		/* END OF name is a match */
	}			/* END OF for loop */

	bzero(buf, sizeof(buf));
	if (j == 0) {
		send_to_char("You have reached the limits of your concentration.\n\r", ch);
	}
	if (j == level >> 1) {
		return;
	}

}				/* END OF spell_locate() */

void spell_poison(sbyte level, struct char_data * ch,
		    struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(victim || obj);

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	if (victim) {
		CHECK_FOR_CHARM();
		CHECK_FOR_PK();
		if (saves_spell(ch, victim, SAVING_PARA)) {
			send_to_char("You felt a brief tingle.\r\n", victim);
		}
		else {
			if (magic_fails(ch, victim))
				return;
			af.type = SPELL_POISON;
			af.duration = 5;
			af.modifier = -2;
			af.location = APPLY_STR;
			af.bitvector = AFF_POISON;
			ha1400_affect_join(victim, &af, FALSE, FALSE);

			send_to_char("You feel very sick.\n\r", victim);
			act("You have poisoned $N!", FALSE, ch, 0, victim, TO_CHAR);
		}
		return;
	}

	/* object poison */
	if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
	    (obj->obj_flags.type_flag == ITEM_FOOD)) {
		obj->obj_flags.value[3] = 1;
		act("You poison $o!", FALSE, ch, obj, victim, TO_CHAR);
	}
	else {
		act("You cant poison $o!", FALSE, ch, obj, victim, TO_CHAR);
	}
	return;

}				/* END OF spell_poison */

void spell_protection_from_evil(sbyte level, struct char_data * ch,
			     struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
	//int cmd = CMD_CAST;



	assert(victim);
	if (magic_fails(ch, victim))
		return;

	/* ARE WE EVIL? */
	if ((GET_ALIGNMENT(ch) < -99) && (GET_LEVEL(ch) <= 5)) {
		CHECK_FOR_NO_FIGHTING();
		IS_ROOM_NO_KILL();
		CHECK_FOR_CHARM();
		if (!(ch == victim))
			CHECK_FOR_PK();


		send_to_char("You feel a violent repulsion from evil.\r\n", ch);
		DAMAGE(victim, victim, GET_LEVEL(ch), SPELL_PROTECT_FROM_EVIL);
	}

	if (ha1375_affected_by_spell(victim, SPELL_PROTECT_FROM_EVIL)) {
		send_to_char("Nothing happens.\r\n", ch);
		send_to_char("You feel a brief tingle.\r\n", victim);
		return;
	}

	af.type = SPELL_PROTECT_FROM_EVIL;
	af.duration = 24;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_PROTECT_EVIL;
	ha1300_affect_to_char(victim, &af);
	send_to_char("You have a righteous feeling!\n\r", victim);
	act("$n briefly glows with a white light!", FALSE, victim, 0, 0, TO_ROOM);
}

void spell_remove_curse(sbyte level, struct char_data * ch,
			  struct char_data * victim, struct obj_data * obj){

	int idx, did_we_do_anything;
	assert(ch && (victim || obj));



	if (obj) {
		if (IS_SET(obj->obj_flags.flags1, OBJ1_CURSED)) {

			/* RESTORE ATTACK DICE - Both flags must be set from
			 * curse command */
			if (IS_SET(obj->obj_flags.flags1, OBJ1_CURSED)) {
				if (obj->obj_flags.type_flag == ITEM_WEAPON ||
				 obj->obj_flags.type_flag == ITEM_QSTWEAPON) {
					obj->obj_flags.value[2]++;
				}
			}
			act("$p briefly glows blue.", TRUE, ch, obj, 0, TO_CHAR);

			REMOVE_BIT(obj->obj_flags.flags1, OBJ1_CURSED);

		}
		else {
			send_to_char("Nothing happens.\r\n", ch);
		}
		return;
	}
	else {			/* Then it is a PC | NPC */
		did_we_do_anything = FALSE;
		if (magic_fails(ch, victim))
			return;
		if (ha1375_affected_by_spell(victim, SPELL_CURSE) || ha1375_affected_by_spell(victim, SPELL_GRANGORNS_CURSE)) {
			act("$n briefly glows red, then blue.", FALSE, victim, 0, 0, TO_ROOM);
			act("You feel better.", FALSE, victim, 0, 0, TO_CHAR);
			ha1350_affect_from_char(victim, SPELL_CURSE);
			ha1350_affect_from_char(victim, SPELL_GRANGORNS_CURSE);
			did_we_do_anything = TRUE;
		}
		/* LETS GO THROUGH THEIR INVENTORY */
		for (idx = 0; idx < MAX_WEAR; idx++) {
			if (victim->equipment[idx]) {
				if (IS_SET(GET_OBJ1(victim->equipment[idx]), OBJ1_CURSED)) {
					spell_remove_curse(level, ch, 0, victim->equipment[idx]);
					did_we_do_anything = TRUE;
				}
			}	/* END OF we have equipment */
		}		/* END OF for LOOP */
		if (ch != victim) {
			send_to_char("You feel a brief tingle.\r\n", victim);
		}
		if (did_we_do_anything == FALSE) {
			send_to_char("Nothing happens.\r\n", ch);
		}
	}			/* END OF else THIS IS A PC | NPC */
	return;

}				/* END OF spell_remove_curse() */

void spell_remove_poison(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj){

	assert(ch && (victim || obj));



	if (victim) {
		if (magic_fails(ch, victim))
			return;
		if (ha1375_affected_by_spell(victim, SPELL_POISON)) {
			ha1350_affect_from_char(victim, SPELL_POISON);
			act("A warm feeling runs through your body.", FALSE, victim, 0, 0, TO_CHAR);
			act("$N looks better.", FALSE, ch, 0, victim, TO_ROOM);
		}
	}
	else {
		if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
		    (obj->obj_flags.type_flag == ITEM_FOOD)) {
			obj->obj_flags.value[3] = 0;
			act("The $p steams briefly.", FALSE, ch, obj, 0, TO_CHAR);
		}
	}
}

void spell_sanctuary(sbyte level, struct char_data * ch, struct char_data * victim, struct obj_data * obj){

	struct affected_type af;
	int lv_loop;



	if (magic_fails(ch, victim))
		return;

	lv_loop = 0;
	while (IS_AFFECTED(victim, AFF_SANCTUARY) && lv_loop < 5) {
		lv_loop++;
		send_to_char("You cancel the previous sanctuary.\r\n", ch);
		ha1350_affect_from_char(victim, SPELL_SANCTUARY);
	}

	if (lv_loop > 4) {
		send_to_char("How strange, I was unable to remove sanc.\r\n",
			     ch);
		return;
	}

	if (ha1375_affected_by_spell(victim, SPELL_BARKSKIN)) {
		send_to_char("You cannot combine Sanctuary and Barkskin.\r\n", ch);
		return;
	}

	if (ha1375_affected_by_spell(victim, SPELL_STONESKIN)) {
		send_to_char("You cannot combine Stoneskin and Sanctuary.\r\n", ch);
		return;
	}

	act("$n is surrounded by a white aura.", TRUE, victim, 0, 0, TO_ROOM);
	act("You start glowing.", TRUE, victim, 0, 0, TO_CHAR);

	af.type = SPELL_SANCTUARY;
	af.duration = (level <= IMO_LEV) ? 2 + level / 10 : level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_SANCTUARY;
	ha1300_affect_to_char(victim, &af);

}				/* END OF spell_sanctuary() */

void spell_sanctuary_medium(sbyte level, struct char_data * ch,
			      struct char_data * victim, struct obj_data * obj){
	struct affected_type af;
	int lv_loop;



	if (magic_fails(ch, victim))
		return;

	lv_loop = 0;
	//while (IS_AFFECTED2(victim, AFF_SANCTUARY_MEDIUM) && lv_loop < 5) {
		while (IS_AFFECTED(victim, AFF_SANCTUARY) && lv_loop < 5) {
			lv_loop++;
			send_to_char("You cancel the previous sanctuary.\r\n", ch);
			ha1350_affect_from_char(victim, SPELL_SANCTUARY_MEDIUM);
		}

		if (lv_loop > 4) {
			send_to_char("How strange, I was unable to remove sanc.\r\n",
				     ch);
			return;
		}

		act("$n is surrounded by a white aura.", TRUE, victim, 0, 0, TO_ROOM);
		act("You start glowing.", TRUE, victim, 0, 0, TO_CHAR);

		af.type = SPELL_SANCTUARY_MEDIUM;
		af.duration = (level <= IMO_LEV) ? 2 + level / 10 : level;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_SANCTUARY;
		ha1300_affect_to_char(victim, &af);

	}			/* END OF spell_sanctuary() */

void spell_sanctuary_minor(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {
		struct affected_type af;
		int lv_loop;



		if (magic_fails(ch, victim))
			  return;

		  lv_loop = 0;
		//while (IS_AFFECTED(victim, AFF_SANCTUARY_MINOR) && lv_loop < 5) {
			while (IS_AFFECTED(victim, AFF_SANCTUARY) && lv_loop < 5) {
				lv_loop++;
				send_to_char("You cancel the previous sanctuary.\r\n", ch);
				ha1350_affect_from_char(victim, SPELL_SANCTUARY_MINOR);
			}

			if (lv_loop > 4) {
				send_to_char("How strange, I was unable to remove sanc.\r\n",
					     ch);
				return;
			}

			act("$n is surrounded by a white aura.", TRUE, victim, 0, 0, TO_ROOM);
			act("You start glowing.", TRUE, victim, 0, 0, TO_CHAR);

			af.type = SPELL_SANCTUARY_MINOR;
			af.duration = (level <= IMO_LEV) ? 2 + level / 10 : level;
			af.modifier = 0;
			af.location = APPLY_NONE;
			af.bitvector = AFF_SANCTUARY;
			ha1300_affect_to_char(victim, &af);

		}		/* END OF spell_sanctuary() */

void spell_sleep(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {

			struct affected_type af;
			//int cmd = CMD_CAST;



			  assert(victim);

			  CHECK_FOR_NO_FIGHTING();
			  IS_ROOM_NO_KILL();
			  CHECK_FOR_CHARM();
			  CHECK_FOR_PK();

			if (magic_fails(ch, victim))
				  return;

			if (saves_spell(ch, victim, SAVING_SPELL)) {
				if (!number(0, 5))
					hit(ch, victim, TYPE_UNDEFINED);
				act("$N resists the effect of your magic!",
				    TRUE, ch, 0, victim, TO_CHAR);
				send_to_char("You felt a brief tingle.\r\n", victim);
				return;
			}

			/* IF THE MOB IS INJURED, MAKE IT HARDER TO SLEEP IT */
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				if (GET_INT(victim) > 14) {
					if (number(0, 2)) {
						act("$N resists the effect of your magic.",
						TRUE, ch, 0, victim, TO_CHAR);
						send_to_char("You felt a brief tingle.\r\n", victim);
						return;
					}
				}
			}


			af.type = SPELL_SLEEP;
			af.duration = MAXV(24, 4 + (level >> 1));
			af.modifier = 0;
			af.location = APPLY_NONE;
			af.bitvector = AFF_SLEEP;
			ha1400_affect_join(victim, &af, FALSE, FALSE);

			if (GET_POS(victim) > POSITION_SLEEPING) {
				act("You feel very sleepy ..... zzzzzz", FALSE, victim, 0, 0, TO_CHAR);
				act("$n suddenly falls asleep.", TRUE, victim, 0, 0, TO_ROOM);
				GET_POS(victim) = POSITION_SLEEPING;
			}

		}		/* END OF spell_sleep() */

void spell_ventriloquate(sbyte level, struct char_data * ch,
			   struct char_data * victim, struct obj_data * obj) {



			/* Not possible!! No argument! */
			return;
		}
