/* at */
/* *********************************************************************
*  file: attack.c, Offensive commands/procedures.      Part of DIKUMUD *
*  Usage : Offensive commands.                                         *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.    *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "limits.h"
#include "constants.h"
#include "spells.h"
#include "globals.h"
#include "bounty.h"
#include "func.h"

/* Prototype external functions */
int ft2800_spec_damage(struct char_data * ch, struct char_data * victim, int dam, struct obj_data * wielded);

void at1000_do_hit(struct char_data * ch, char *argument, int cmd)
{

	char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	/* TELL ON IMMORTS */
	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s hit%s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, arg);

	if (!(*arg)) {
		send_to_char("Hit who?\n\r", ch);
		return;
	}

	victim = ha2125_get_char_in_room_vis(ch, arg);
	if (!(victim)) {
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	if (victim == ch) {
		send_to_char("You hit yourself..OUCH!.\n\r", ch);
		act("$n hits $mself, and says OUCH!", FALSE, ch, 0, victim, TO_ROOM);
		return;
	}
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
		act("$N is just such a good friend, you simply can't hit $M.",
		    FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if ((GET_POS(ch) == POSITION_STANDING) &&
	    (victim != ch->specials.fighting)) {
		sprintf(buf, "FIRSTHIT: %s (%d/%d) hits %s (%d/%d).",
			GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		 GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
		main_log(buf);
		if (IS_PC(victim)) {
			spec_log(buf, PKILL_LOG);
		}
		else {
			spec_log(buf, FIRSTHIT_LOG);
		}
		hit(ch, victim, TYPE_UNDEFINED);
		WAIT_STATE(ch, PULSE_VIOLENCE);	/* HVORFOR DET?? */

	}
	else {
		send_to_char("You do the best you can!\n\r", ch);
	}

	return;

}				/* END OF at1000_do_hit() */


void at1100_do_kill(struct char_data * ch, char *argument, int cmd)
{

	char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];

	struct char_data *vict;

	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s kill%s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, arg);

	if (!*arg) {
		send_to_char("Kill who?\n\r", ch);
		return;
	}

	if (!(vict = ha2125_get_char_in_room_vis(ch, arg))) {
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if (ch == vict) {
		send_to_char("Your mother would be so sad.. :(\n\r", ch);
		return;
	}


	/* GOD KILL */

	if (IS_PC(ch) && GET_LEVEL(ch) > IMO_IMM) {
		act("You chop $M to pieces! Ah! The blood!", FALSE, ch, 0, vict,
		    TO_CHAR);
		act("$N chops you to pieces!", FALSE, vict, 0, ch, TO_CHAR);
		act("$n brutally slays $N.", FALSE, ch, 0, vict, TO_NOTVICT);
		ft1800_raw_kill(vict, ch);
	}
	else {
		at1000_do_hit(ch, argument, 0);
	}

	return;

}				/* END OF at1100_do_kill() */


void at1200_do_backstab(struct char_data * ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	signed char percent;
	int chance;

	if (GET_LEVEL(ch) >= IMO_IMP) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s backstab%s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, name);

	if (!(victim = ha2125_get_char_in_room_vis(ch, name))) {
		send_to_char("Backstab who?\n\r", ch);
		return;
	}

	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();

	if (victim == ch) {
		send_to_char("How can you sneak up on yourself?\n\r", ch);
		return;
	}


	if (!ch->equipment[WIELD]) {
		send_to_char("You need to wield a weapon, to make it a succes.\n\r", ch);
		return;
	}

	if (ch->equipment[WIELD]->obj_flags.value[3] != 11) {
		send_to_char("Only piercing weapons can be used for backstabbing.\n\r", ch);
		return;
	}

	if (victim->specials.fighting) {
		send_to_char("You can't backstab a fighting person, too alert!\n\r", ch);
		return;
	}
	if (IS_SET(GET_ROOM2(victim->in_room), RM2_ARENA)) {
		if (IS_AFFECTED(ch, AFF_SNEAK)) {
			chance = number(0, 242);
		}
		else {
			chance = number(0, 121);
		}
		if (chance <= victim->skills[SKILL_DODGE].learned) {
			sprintf(buf, "%s is aware of your presence, and swiftly avoids your backstab.\r\n",
				GET_REAL_NAME(victim));
			send_to_char(buf, ch);
			sprintf(buf, "You notice %s trying to backstab you and swiftly move aside!\r\n",
				GET_REAL_NAME(ch));
			send_to_char(buf, victim);
			sprintf(buf, "%s foils %s's attempt to backstab.\r\n",
				GET_REAL_NAME(victim),
				GET_REAL_NAME(ch));
			act(buf, TRUE, ch, 0, victim, TO_ROOM);
			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			return;
		}
	}

	/* IS THE MOB PROTECTED BY BACKSTAB FLAG? */
	if (gv_port != HELLNIGHT_PORT &&
	    IS_SET(GET_ACT2(victim), PLR2_BACKSTAB) &&
	    GET_POS(victim) > POSITION_RESTING &&
	    !IS_SET(GET_ROOM2(victim->in_room), RM2_ARENA)) {
		sprintf(buf, "%s is too fast for you, and skillfully dodges your lunge.\r\n",
			GET_REAL_NAME(victim));
		send_to_char(buf, ch);
		sprintf(buf, "%s tried to backstab you!\r\n",
			GET_REAL_NAME(ch));
		send_to_char(buf, victim);
		sprintf(buf, "%s foils %s's attempt to backstab.\r\n",
			GET_REAL_NAME(victim),
			GET_REAL_NAME(ch));
		act(buf, TRUE, ch, 0, victim, TO_ROOM);

		if (IS_NPC(victim)) {
			hit(victim, ch, TYPE_UNDEFINED);
		}

		WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
		WAIT_STATE(ch, PULSE_VIOLENCE);
		return;
	}

	if (AWAKE(victim) && !(ch->skills[SKILL_BACKSTAB].learned)) {
		send_to_char("You don't have the skills necessary for that.\r\n",
			     ch);
		return;
	}
	sprintf(buf, "FIRSTHIT: %s (%d/%d) backstabs %s (%d/%d).",
		GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
	main_log(buf);
	if (IS_PC(victim)) {
		spec_log(buf, PKILL_LOG);
	}
	else {
		spec_log(buf, FIRSTHIT_LOG);
	}
	percent = number(1, 101);	/* 101% is a complete failure */
	if (AWAKE(victim) && (percent > ch->skills[SKILL_BACKSTAB].learned))
		DAMAGE(ch, victim, 0, SKILL_BACKSTAB);
	else {
		percent = number(1, 100);
		if ((percent < 40) || (IS_PC(victim))) {
			SET_BIT(GET_ACT2(victim), PLR2_BACKSTAB);
		}
		li9900_gain_proficiency(ch, SKILL_BACKSTAB);
		hit(ch, victim, SKILL_BACKSTAB);
	}

	WAIT_STATE(ch, PULSE_VIOLENCE);

	return;

}				/* END OF at1200_do_backstab() */


void at1300_do_order(struct char_data * ch, char *argument, int cmd)
{

	char name[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	bool found = FALSE;
	int org_room;
	struct char_data *victim;
	struct follow_type *k, *nextk;

	MUZZLECHECK();
	half_chop(argument, name, message);

	if (!*name || !*message) {
		send_to_char("Order who to do what?\n\r", ch);
		return;
	}

	if (!(victim = ha2125_get_char_in_room_vis(ch, name)) &&
	    str_cmp("follower", name) && str_cmp("followers", name)) {
		send_to_char("That person isn't here.\n\r", ch);
		return;
	}

	if (ch == victim) {
		send_to_char("You obviously suffer from schizophrenia.\n\r", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM)) {
		send_to_char("Your superior would not aprove of you giving orders.\n\r", ch);
		return;
	}

	if (victim) {
		sprintf(buf, "$N orders you to '%s'", message);
		act(buf, FALSE, victim, 0, ch, TO_CHAR);
		act("$n gives $N an order.", FALSE, ch, 0, victim, TO_ROOM);

		if ((IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) &&
		  (victim->master != ch || !IS_AFFECTED(victim, AFF_CHARM))) {
			act("$n has an indifferent look.",
			    FALSE, victim, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("Ok.\n\r", ch);
			command_interpreter(victim, message);

			if (GET_LEVEL(ch) >= IMO_SPIRIT) {
				sprintf(buf, "%s orders %s to %s", GET_REAL_NAME(ch), GET_REAL_NAME(victim), message);
				spec_log(buf, GOD_COMMAND_LOG);
				do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
			}
		}
	}
	else {			/* This is order "followers" */
		/*
		 send_to_char("Sorry, you'll have to order them one at a time.", ch);
		 */
		sprintf(buf, "$n issues the order '%s'.", message);
		act(buf, FALSE, ch, 0, victim, TO_ROOM);

		org_room = ch->in_room;

		for (k = ch->followers; k; k = nextk) {
			nextk = k->next;
			if (org_room == k->follower->in_room)
				if (IS_AFFECTED(k->follower, AFF_CHARM)) {
					found = TRUE;
					command_interpreter(k->follower, message);
					if (GET_LEVEL(ch) >= IMO_SPIRIT) {
						sprintf(buf, "%s orders %s to %s", GET_REAL_NAME(ch), GET_REAL_NAME(k->follower), message);
						spec_log(buf, GOD_COMMAND_LOG);
						do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
					}
					/*
					 if (!k || !k->follower)
					 break;
					 */
				}
		}
		if (found)
			send_to_char("Ok.\n\r", ch);
		else
			send_to_char("Nobody here are loyal subjects of yours!\n\r",
				     ch);
	}			/* END OF order followers */

}				/* END OF at1300_do_order() */


void at1400_do_flee(struct char_data * ch, char *argument, int cmd)
{

	int i, attempt, lose, lv_die, lv_random;
	char buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	bzero(buf, sizeof(buf));

	victim = ch->specials.fighting;

	/* WE AREN'T FIGHTING? */
	if (!(victim)) {
		send_to_char("Run away!  Run Away!\r\n", ch);
		act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);
		return;
	}

	/* WE ARE FIGHTING */
	if (IS_AFFECTED(ch, AFF_RAGE)) {
		send_to_char("You are caught up in blood lust and wouldn't dream of leaving the carnage!\r\n", ch);
		return;
	}

	for (i = 0; i < 6; i++) {
		attempt = number(0, 5);	/* Select a random direction */
		if (CAN_GO(ch, attempt) &&
		    !IS_SET(world[EXIT(ch, attempt)->to_room].room_flags, RM1_DEATH) &&
		    !(IS_NPC(ch) && IS_SET(world[EXIT(ch, attempt)->to_room].room_flags, RM1_NO_MOB))) {
			act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);
			lv_die = mo1000_do_simple_move(ch, attempt, FALSE, BIT0);
			if (lv_die == 1) {
				/* The escape has succeded, if fighting a mob
				 * lose exp. */
				if (IS_NPC(victim)) {
					lv_random = number(-5, 5);
					lose = (GET_LEVEL(ch) + lv_random) * GET_LEVEL(ch) *
						GET_LEVEL(victim);
					if (lose < 0) {
						lose = 0;
					}
				}
				else {
					lose = 0;
				}
				if (cmd == CMD_ESCAPE)
					lose = 0;

				if (lose > 0 &&
				    IS_PC(ch) &&
				    GET_LEVEL(ch) > 5) {
					if (lose > GET_EXP(ch)) {
						lose = GET_EXP(ch);
						GET_EXP(ch) = 0;
					}
					else {
						li2200_gain_exp(ch, -1 * lose);
					}
					sprintf(buf, "You lose %d experience points!", lose);
					act(buf, TRUE, ch, 0, 0, TO_CHAR);
				}

				send_to_char("You flee head over heels.\r\n", ch);

				/* DO MOBS HUNT YOU BACK? */
				at2000_do_mob_hunt_check(victim, ch, 1);

				/* STOP FIGHTING FOR THE PERSON WE ARE FIGHTING */
				if (ch->specials.fighting) {
					if (ch->specials.fighting->specials.fighting == ch)
						ft1400_stop_fighting(ch->specials.fighting, 0);
				}

				if (cl1900_at_war(ch, ch->specials.fighting))
					cl2000_player_escape(ch, ch->specials.fighting);
				ft1400_stop_fighting(ch, 0);
				GET_POS(ch) = POSITION_STANDING;

				return;
			}
			else {
				if (!lv_die) {
					act("$n tries to flee, but is too exhausted!",
					    TRUE, ch, 0, 0, TO_ROOM);
				}
				return;
			}
		}
	}			/* END OF for loop */

	/* No exits was found */
	send_to_char("PANIC! You couldn't escape!\n\r", ch);
	return;
}				/* END OF at1400_do_flee() */


void at1500_do_bash(struct char_data * ch, char *argument, int cmd)
{

	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	signed char percent;
	int block, lv_wait = 2;

	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s bash%s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, name);

	if (ch->skills[SKILL_BASH].learned == 0) {
		send_to_char("You better leave all the martial arts to someone else.\n\r", ch);
		return;
	}
	if (name[0] == 0) {
		/* WE DIDN'T GET AN ARGUMENT.  SEE IF WE ARE FIGHTING */
		if (ch->specials.fighting) {
			victim = ch->specials.fighting;
		}
		else {
			/* WE AREN'T FIGHTING AND NO ARG GIVEN */
			send_to_char("Who do you want to bash?\n\r", ch);
			return;
		}
	}
	else {
		/* SEE IF THE THE SPECIFIED VICTIM IS AVAILABLE */
		victim = ha2125_get_char_in_room_vis(ch, name);
		if (!(victim)) {
			/* CAN'T LOCATE THAT VICTIM */
			send_to_char("Your good! that person didn't wait around to get bashed.\n\r", ch);
			return;
		}
	}

	if (victim == ch) {
		send_to_char("Aren't we funny today...\n\r", ch);
		return;
	}


	if (!ch->equipment[WIELD]) {
		send_to_char("You need to wield a weapon, to make it a success.\n\r", ch);
		return;
	}
	/*
   if (ch->equipment[WIELD]->obj_flags.value[3] != 6 &&
       ch->equipment[WIELD]->obj_flags.value[3] != 7) {
		 send_to_char("How are you suppose to bash with that?  Try something that bludgeons.\n\r",ch);
		 return;
   }
	 */
	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();
	if ((GET_POS(ch) == POSITION_STANDING) &&
	    (victim != ch->specials.fighting)) {
		sprintf(buf, "FIRSTHIT: %s (%d/%d) bashes %s (%d/%d).",
			GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		 GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
		main_log(buf);
		if (IS_PC(victim)) {
			spec_log(buf, PKILL_LOG);
		}
		else {
			spec_log(buf, FIRSTHIT_LOG);
		}
	}

	percent = number(1, 101);	/* 101% is a complete failure */

	if (GET_DEX(victim) > GET_DEX(ch)) {
		block = number(0, 2);
	}
	else {
		block = number(0, 3);
	}

	if (ch->in_room != victim->in_room) {
		send_to_char("Doesn't seem that you can stretch that far to bash them...\n\r", ch);
		lv_wait = 0;
		return;
	}

	if (percent > ch->skills[SKILL_BASH].learned) {
		DAMAGE(ch, victim, 0, SKILL_BASH);
		GET_POS(ch) = POSITION_SITTING;
		WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
	}
	else {
		if (victim->equipment[WEAR_SHIELD] && block == 1) {
			act("$N uses $S $p to block your bash.", TRUE, ch, victim->equipment[WEAR_SHIELD], victim, TO_CHAR);
			act("$N uses $S $p to block $n's bash.", TRUE, ch, victim->equipment[WEAR_SHIELD], victim, TO_NOTVICT);
			act("You raise your $p and block $n's bash.", TRUE, ch, victim->equipment[WEAR_SHIELD], victim, TO_VICT);
			lv_wait = 1;
		}
		else {
			li9900_gain_proficiency(ch, SKILL_BASH);
			DAMAGE(ch, victim, 15, SKILL_BASH);
			GET_POS(victim) = POSITION_SITTING;
			WAIT_STATE(victim, PULSE_VIOLENCE * lv_wait);
		}
	}
	WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
}				/* END OF at1500_do_bash() */

void at1600_do_rescue(struct char_data * ch, char *argument, int cmd)
{
	struct char_data *victim, *fight_ch, *fight_vict;
	int percent, lv_was_in_rage;
	char victim_name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s rescue%s\r\n", GET_NAME(ch), argument);
	}

	one_argument(argument, victim_name);

	if (!(victim = ha2125_get_char_in_room_vis(ch, victim_name))) {
		send_to_char("Who do you want to rescue?\n\r", ch);
		return;
	}

	if (victim == ch) {
		send_to_char("What about fleeing instead?\n\r", ch);
		return;
	}

	if (ch->specials.fighting == victim) {
		send_to_char("How can you rescue someone you are trying to kill?\n\r", ch);
		return;
	}

	fight_vict = victim->specials.fighting;


	if (fight_vict && IS_PC(fight_vict)) {
		send_to_char("You can't rescue a mob that's fighting a player.\r\n", ch);
		return;
	}

	for (fight_ch = world[ch->in_room].people;
	     fight_ch && (fight_ch->specials.fighting != victim);
	     fight_ch = fight_ch->next_in_room);

	if (!fight_ch) {
		act("But nobody is fighting $M?", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if (ch->skills[SKILL_RESCUE].learned == 0)
		send_to_char("But you don't know how do this!", ch);
	else {
		percent = number(1, 101);	/* 101% is a complete failure */
		if (percent > ch->skills[SKILL_RESCUE].learned) {
			send_to_char("You fail the rescue.\n\r", ch);
			return;
		}

		if (IS_AFFECTED(victim, AFF_RAGE))
			lv_was_in_rage = TRUE;
		else
			lv_was_in_rage = FALSE;

		if (victim->specials.fighting == fight_ch)
			ft1400_stop_fighting(victim, 0);
		if (fight_ch->specials.fighting)
			ft1400_stop_fighting(fight_ch, 0);
		if (ch->specials.fighting)
			ft1400_stop_fighting(ch, 0);
		ft1300_set_fighting(ch, fight_ch, 0);
		ft1300_set_fighting(fight_ch, ch, 0);

		li9900_gain_proficiency(ch, SKILL_RESCUE);
	}

	if (lv_was_in_rage) {
		send_to_char("UH OH! That person doesn't seem to recognize you as a friend!\r\n", ch);
		act("You are rescued by $N, It makes you mad!",
		    FALSE, victim, 0, ch, TO_CHAR);
		act("$n heroically rescues $N, but its not appreciated!",
		    FALSE, ch, 0, victim, TO_NOTVICT);
		/* if ((IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)) &&
		 * (IS_SET(GET_ACT2(victim), PLR2_PKILLABLE)))
		 * ft1300_set_fighting(victim, ch, 0); */
	}
	else {
		send_to_char("Banzai! To the rescue...\n\r", ch);
		act("You are rescued by $N, you are confused!",
		    FALSE, victim, 0, ch, TO_CHAR);
		act("$n heroically rescues $N.",
		    FALSE, ch, 0, victim, TO_NOTVICT);
	}

	WAIT_STATE(victim, 2 * PULSE_VIOLENCE);

}				/* END OF at1600_do_rescue() */

void at1650_do_stab(struct char_data * ch, char *argument, int cmd)
{
	//rlum

	struct obj_data *wield;
	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int lv_wait = 3;
	int lv_cost = 0;
	//int lv_totdmg = 0.75;
	signed char percent;

	wield = ch->equipment[WIELD];

	if (GET_LEVEL(ch) < 20) {
		lv_wait = 3;
		lv_cost = 20;
	}
	else if (GET_LEVEL(ch) < 30) {
		lv_wait = 2;
		lv_cost = 40;
	}
	else {
		lv_wait = 1;
		lv_cost = 80;
	}

	if (ch->skills[SKILL_STAB].learned == 0) {
		send_to_char("You better leave that skill to a REAL thief.\n\r", ch);
		return;
	}

	if (GET_MOVE(ch) < lv_cost) {
		send_to_char("You're just too tired for that kind of thing.\n\r", ch);
		return;
	}

	if (!ch->equipment[WIELD]) {
		send_to_char("You need to wield a weapon, to make it a succes.\n\r", ch);
		return;
	}

	if (ch->equipment[WIELD]->obj_flags.value[3] != 11) {
		send_to_char("Only piercing weapons can be used for stabbing.\n\r", ch);
		return;
	}

	one_argument(argument, name);

	if (name[0] == 0) {
		/* WE DIDN'T GET AN ARGUMENT.  SEE IF WE ARE FIGHTING */
		if (ch->specials.fighting) {
			victim = ch->specials.fighting;
		}
		else {
			/* WE AREN'T FIGHTING AND NO ARG GIVEN */
			send_to_char("Who do you want to stab?\n\r", ch);
			return;
		}
	}
	else {
		/* SEE IF THE THE SPECIFIED VICTIM IS AVAILABLE */
		victim = ha2125_get_char_in_room_vis(ch, name);
		if (!(victim)) {
			/* CAN'T LOCATE THAT VICTIM */
			send_to_char("You're good. That person didn't wait around to get stabbed!\n\r", ch);
			return;
		}
	}

	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	if (victim == ch) {
		send_to_char("Aren't we funny today...\n\r", ch);
		return;
	}
	CHECK_FOR_CHARM();
	if ((GET_POS(ch) == POSITION_STANDING) &&
	    (victim != ch->specials.fighting)) {
		sprintf(buf, "FIRSTHIT: %s (%d/%d) stabs %s (%d/%d).",
			GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		 GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
		main_log(buf);
		if (IS_PC(victim)) {
			spec_log(buf, PKILL_LOG);
		}
		else {
			spec_log(buf, FIRSTHIT_LOG);
		}
	}
	if (ch->in_room != victim->in_room) {
		send_to_char("I am afraid your blade isn't that long...\n\r", ch);
		lv_wait = 0;
		return;
	}

	percent = number(1, 101);

	if (percent > ch->skills[SKILL_STAB].learned) {
		DAMAGE(ch, victim, 0, SKILL_STAB);

	}
	else {
		li9900_gain_proficiency(ch, SKILL_STAB);

		int d1, d2, dam, ln;

		d1 = wield->obj_flags.value[1];
		d2 = wield->obj_flags.value[2];

		dam = dice(d1, d2);

		ln = ch->skills[SKILL_KICK].learned;


		if (ln < 20) {
			ln = 2;
		}
		else {
			ln = 4;
		}


		dam = (GET_STR(ch) + GET_BONUS_STR(ch) + dam);
		dam = ft2800_spec_damage(ch, victim, dam, wield);
		DAMAGE(ch, victim, (dam * ln), SKILL_STAB);
		sprintf(buf, "%d", (int) GET_LEVEL(ch) >> 3);

		main_log(buf);
	}

	GET_MOVE(ch) -= lv_cost;

	WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);

}				/* END OF at1650_do_stab() */

void at1700_do_kick(struct char_data * ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int lv_real_ac;
	int lv_wait = 3;
	int lv_cost = 0;
	float lv_totdmg = 0.6;
	int is_ninja = 0;
	signed char percent;

	if ((IS_PC(ch)) && IS_AFFECTED(ch, AFF_TRANSFORMED));
	is_ninja = IS_CASTED_ON(ch, SPELL_TRANSFORM_NINJA);

	if (is_ninja) {
		lv_wait = 1;
		lv_totdmg = 1;
	}
	else if (GET_LEVEL(ch) < 20) {
		lv_wait = 3;
	}
	else if (GET_LEVEL(ch) < 30) {
		lv_wait = 2;
		lv_cost = 10;
	}
	else {
		lv_wait = 1;
		lv_cost = 25;
	}

	if (ch->skills[SKILL_KICK].learned == 0 && !is_ninja) {
		send_to_char("You better leave all the martial arts to fighters.\n\r", ch);
		return;
	}

	if (GET_MOVE(ch) < lv_cost) {
		send_to_char("You're just too tired for that kind of thing.\n\r", ch);
		return;
	}


	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s kick%s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, name);

	if (name[0] == 0) {
		/* WE DIDN'T GET AN ARGUMENT.  SEE IF WE ARE FIGHTING */
		if (ch->specials.fighting) {
			victim = ch->specials.fighting;
		}
		else {
			/* WE AREN'T FIGHTING AND NO ARG GIVEN */
			send_to_char("Who do you want to kick?\n\r", ch);
			return;
		}
	}
	else {
		/* SEE IF THE THE SPECIFIED VICTIM IS AVAILABLE */
		victim = ha2125_get_char_in_room_vis(ch, name);
		if (!(victim)) {
			/* CAN'T LOCATE THAT VICTIM */
			send_to_char("Your good, that person didn't wait around to get kicked!\n\r", ch);
			return;
		}
	}

	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();


	if (victim == ch) {
		if (GET_LEVEL(ch) >= IMO_SPIRIT) {
			sprintf(buf, "&rYou try to kick yourself and fail.&n\n\r"
				"&RYou reach down and rip off your leg and proceed to beat yourself to death.&n\r\n"
				"&rThat's gonna hurt tomorrow....&n\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "&r$n tries to kick $mself and gets frustrated after $e fails.&n\n\r"
				"&R$n reaches down and rips off $s leg and proceeds to beat $mself to death.&n\r\n"
				"&rOuch that looked painful!&n\n\r");
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "%s L[%d] rips off one of %s legs and beats %sself to death.\r\n",
				GET_REAL_NAME(ch),
				GET_LEVEL(ch),
				HSHR(ch),
				HMHR(ch));
			do_info(buf, 1, MAX_LEV, ch);
			ft1800_raw_kill(ch, ch);
			return;
		}
		send_to_char("Aren't we funny today...\n\r", ch);
		return;
	}
	CHECK_FOR_CHARM();
	if ((GET_POS(ch) == POSITION_STANDING) &&
	    (victim != ch->specials.fighting)) {
		sprintf(buf, "FIRSTHIT: %s (%d/%d) kicks %s (%d/%d).",
			GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		 GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
		main_log(buf);
		if (IS_PC(victim)) {
			spec_log(buf, PKILL_LOG);
		}
		else {
			spec_log(buf, FIRSTHIT_LOG);
		}
	}
	/* 101% is a complete failure */
	lv_real_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim)) + GET_BONUS_DEX(victim) +
		races[GET_RACE(victim)].adj_ac;
	percent = ((10 - (lv_real_ac / 10)) << 1) + number(1, 101);

	if (ch->in_room != victim->in_room) {
		send_to_char("I am afraid your leg isn't that long...\n\r", ch);
		lv_wait = 0;
		return;
	}

	if (percent > ch->skills[SKILL_KICK].learned) {
		DAMAGE(ch, victim, 0, SKILL_KICK);
	}
	else {
		li9900_gain_proficiency(ch, SKILL_KICK);
		DAMAGE(ch, victim, (int) ((GET_LEVEL(ch) >> 3) * ch->skills[SKILL_KICK].learned * lv_totdmg), SKILL_KICK);
	}

	GET_MOVE(ch) -= lv_cost;

	WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
}				/* END OF at1700_do_kick() */

void at1800_do_breath(struct char_data * ch, char *arg, int cmd)
{

	char name[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];
	struct char_data *victim;
	int percent, is_dragon, is_chimera;
	int cost = 3;
	int lv_wait = 3;

	if (IS_NPC(ch)) {
		send_to_char("This used to crash the mud you dough-head!\n\r", ch);
		return;
	}

	is_dragon = IS_CASTED_ON(ch, SPELL_TRANSFORM_DRAGON);
	is_chimera = IS_CASTED_ON(ch, SPELL_TRANSFORM_CHIMERA);

	if (is_dragon)
		lv_wait = 1;
	if (is_chimera)
		lv_wait = 2;

	if ((ch->specials.fighting) &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    (pulse % PULSE_VIOLENCE) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		sprintf(buffer, "breath %s", arg);
		WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
		co1500_write_to_head_q(buffer, &ch->desc->input);
		return;		/* we'll be back at the beging of first combat
				 * round */
	}


	if ((GET_RACE(ch) != RACE_DRAGONKIN) &&
	    (GET_LEVEL(ch) < IMO_SPIRIT) && !is_dragon &&
	    !is_chimera) {
		act("You don't know how to imitate a dragon.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n exhales loudly, in a futile attempt to imitate a dragon.", FALSE, ch, 0, 0, TO_ROOM);
		return;
	}

	if ((GET_MOVE(ch) < cost) || (is_dragon && GET_MOVE(ch) < 50) || (is_chimera && GET_MOVE(ch) < 50)) {
		send_to_char("You're just too tired for that kind of thing.\n\r", ch);
		return;
	}

	arg = one_argument(arg, name);
	if (*name)		/* if the name exists get next arg */
		arg = one_argument(arg, message);

	if (!(*name && *message)) {
		if (!*name) {
			send_to_char("Breath what on who?\n\r", ch);
			return;
		}
		else {
			if (str_cmp("gas", name)) {	/* only gas is area
							 * affect */
				send_to_char("Breath what on who?\n\r", ch);
				return;
			}

			if (IS_AFFECTED(ch, AFF_CHARM) && ch->master && (ch->master->in_room == ch->in_room)) {
				act("$N is just such a good friend, you simply can't hurt $M.", FALSE, ch, 0, ch->master, TO_CHAR);
				return;
			}

			percent = number(0, 101);

			if ((!str_cmp("gas", name)) && (percent < ch->skills[SPELL_GAS_BREATH].learned)) {
				if (is_dragon)
					cost = 25;
				li9900_gain_proficiency(ch, SPELL_GAS_BREATH);
				send_to_char("You inhale mightily...!\n\r", ch);
				WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
				GET_MOVE(ch) -= cost;
				cast_gas_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, 0, 0);
				return;
			}
		}
	}
	else {
		victim = ha2125_get_char_in_room_vis(ch, message);
		if (victim) {
			CHECK_FOR_NO_FIGHTING();
			IS_ROOM_NO_KILL();

			CHECK_FOR_CHARM();
			CHECK_FOR_PK();

			if (victim == ch) {
				send_to_char("Even dragon-kin arent that dumb..\n\r", ch);
			}
			else {
				if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
					act("$N is just such a good friend, you simply can't hurt $M.", FALSE, ch, 0, victim, TO_CHAR);
					return;
				}
				if (GET_POS(ch) >= POSITION_FIGHTING) {
					percent = number(0, 101);
					if ((!str_cmp("gas", name)) &&
					    ((percent < ch->skills[SPELL_GAS_BREATH].learned) || is_dragon)) {
						if (!is_dragon)
							li9900_gain_proficiency(ch, SPELL_GAS_BREATH);
						else
							cost = 25;
						send_to_char("You inhale mightily...!\n\r", ch);
						if (victim != ch->specials.fighting) {
							sprintf(buf, "FIRSTHIT: %s (%d/%d) uses gas breath against: %s (%d/%d).",
								GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
								GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
							main_log(buf);
							if (IS_PC(victim)) {
								spec_log(buf, PKILL_LOG);
							}
							else {
								spec_log(buf, FIRSTHIT_LOG);
							}
						}
						cast_gas_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
						WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
						if (GET_LEVEL(ch) < IMO_SPIRIT)
							GET_MOVE(ch) -= cost;
						return;
					}
					if ((!str_cmp("frost", name)) && (is_dragon || (percent < ch->skills[SPELL_FROST_BREATH].learned))) {
						if (!is_dragon)
							li9900_gain_proficiency(ch, SPELL_FROST_BREATH);
						else
							cost = 10;
						send_to_char("You inhale mightily...!\n\r", ch);
						cast_frost_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
						WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
						if (GET_LEVEL(ch) < IMO_SPIRIT)
							GET_MOVE(ch) -= cost;
						return;
					}
					if ((!str_cmp("acid", name)) && (is_dragon || (percent < ch->skills[SPELL_ACID_BREATH].learned))) {
						if (!is_dragon)
							li9900_gain_proficiency(ch, SPELL_ACID_BREATH);
						else
							cost = 20;
						send_to_char("You inhale mightily...!\n\r", ch);
						if (victim != ch->specials.fighting) {
							sprintf(buf, "FIRSTHIT: %s (%d/%d) uses acid breath against: %s (%d/%d).",
								GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
								GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
							main_log(buf);
							if (IS_PC(victim)) {
								spec_log(buf, PKILL_LOG);
							}
							else {
								spec_log(buf, FIRSTHIT_LOG);
							}
						}
						cast_acid_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
						WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
						if (GET_LEVEL(ch) < IMO_SPIRIT)
							GET_MOVE(ch) -= cost;
						return;
					}
					if ((!str_cmp("lightning", name)) && (is_dragon || (percent < ch->skills[SPELL_LIGHTNING_BREATH].learned))) {
						if (!is_dragon)
							li9900_gain_proficiency(ch, SPELL_LIGHTNING_BREATH);
						else
							cost = 30;
						send_to_char("You inhale mightily...!\n\r", ch);
						if (victim != ch->specials.fighting) {
							sprintf(buf, "FIRSTHIT: %s (%d/%d) uses lightning breath against: %s (%d/%d).",
								GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
								GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
							main_log(buf);
							if (IS_PC(victim)) {
								spec_log(buf, PKILL_LOG);
							}
							else {
								spec_log(buf, FIRSTHIT_LOG);
							}
						}
						cast_lightning_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
						WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
						if (GET_LEVEL(ch) < IMO_SPIRIT)
							GET_MOVE(ch) -= cost;
						return;
					}
					if ((!str_cmp("fire", name)) && (is_dragon || is_chimera || (percent < ch->skills[SPELL_FIRE_BREATH].learned))) {
						if (!is_dragon || is_chimera)
							li9900_gain_proficiency(ch, SPELL_FIRE_BREATH);
						else
							cost = 50;
						send_to_char("You inhale mightily...!\n\r", ch);
						if (victim != ch->specials.fighting) {
							sprintf(buf, "FIRSTHIT: %s (%d/%d) uses fire breath against: %s (%d/%d).",
								GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
								GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
							main_log(buf);
							if (IS_PC(victim)) {
								spec_log(buf, PKILL_LOG);
							}
							else {
								spec_log(buf, FIRSTHIT_LOG);
							}
						}
						cast_fire_breath(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
						WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
						if (GET_LEVEL(ch) < IMO_SPIRIT)
							GET_MOVE(ch) -= cost;
						return;
					}
					send_to_char("Nothing much seems to happen...\n\r", ch);
				}
			}
		}
		else
			send_to_char("No such person around.\n\r", ch);
	}
}				/* END OF at1800_do_breath() */


void at2000_do_mob_hunt_check(struct char_data * ch, struct char_data * vict, int flag)
{

	char buf[MAX_STRING_LENGTH];

	/* IS THIS A HUNT MOB? */
	if (IS_NPC(ch) &&
	    GET_SKILL(ch, SKILL_HUNT) > 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has begun hunting attacker %s.\r\n",
			GET_REAL_NAME(ch), GET_REAL_NAME(vict));
		/* do_sys(buf, IMO_IMP, vict); */
		WAIT_STATE(ch, PULSE_VIOLENCE * 2);
		HUNTING(ch) = vict;
		if (!bt2300_perform_mob_bounty(ch, vict))
			//Let 's not get spammy
				at2100_do_mob_hunt_msgs(ch, vict, 1);
	}

}				/* END OF  at2000_do_mob_hunt_check() */

void at2100_do_mob_hunt_msgs(struct char_data * ch, struct char_data
			     * vict, int flag)
{

	char buf[MAX_STRING_LENGTH];
	static int lv_number;


	/* lv_number = number(0,1); */
	/* DON'T PRINT MESSAGES FOR LOW LEVEL MOBS */
	if (GET_LEVEL(ch) < 30)
		return;

	if (number(1, 10) != 1)
		return;		/* 20% chance only */

	if (flag == 1) {
		if (lv_number < 1 || lv_number > 50) {
			lv_number = 0;
		}
	}
	else {
		if (lv_number < 1 || lv_number > 30) {
			lv_number = 0;
		}
	}

	bzero(buf, sizeof(buf));
	lv_number++;

	/* STARTING */
	if (flag == 1) {

		switch (lv_number) {
		case 1:
			sprintf(buf, "You brought it to me, Now I'm bringing it your way %s",
				GET_REAL_NAME(vict));
			break;
		case 2:
			sprintf(buf, "I'm going to bleed you like a hog %s!",
				GET_REAL_NAME(vict));
			break;
		case 3:
			sprintf(buf, "I am going to tear %s to shreds!",
				GET_REAL_NAME(vict));
			break;
		case 4:
			sprintf(buf, "I can't wait to feast on %s's carcass, warm blood turns me on.",
				GET_REAL_NAME(vict));
			break;
		case 5:
			sprintf(buf, "I hope you burn all the way to HELL %s!",
				GET_REAL_NAME(vict));
			break;
		case 6:
			sprintf(buf, "I hope we have a body bag made up to %s's size.",
				GET_REAL_NAME(vict));
			break;
		case 7:
			sprintf(buf, "You should have stayed at home today %s, because when I kill you, your family is next!",
				GET_REAL_NAME(vict));
			break;
		case 8:
			sprintf(buf, "Today is %s's day to DIE!",
				GET_REAL_NAME(vict));
			break;
		case 9:
			sprintf(buf, "When %s wakes up, tell %s that it was me that knocked %s socks off :)",

				GET_REAL_NAME(vict), HMHR(vict), HSHR(vict));
			break;
		case 10:
			sprintf(buf, "%s you can run from me, but you can't hide!",
				GET_REAL_NAME(vict));
			break;
		case 11:
			sprintf(buf, "I'm going to tear %s a new bunghole!!",
				GET_REAL_NAME(vict));
			break;
		case 12:
			sprintf(buf, "%s is trying to tickle me to death!",
				GET_REAL_NAME(vict));
			break;
		case 13:
			sprintf(buf, "%s tickles you with %s hit *snicker*",
				GET_REAL_NAME(vict), HSHR(vict));
			break;
		case 14:
			sprintf(buf, "Let %s's gruesome death be a lesson to you all.",
				GET_REAL_NAME(vict));
			break;
		case 15:
			sprintf(buf, "I'm coming to hug, squeeze, and crush %s to pieces.",
				GET_REAL_NAME(vict));
			break;
		case 16:
			sprintf(buf, "Your soul is *MINE* now %s!",
				GET_REAL_NAME(vict));
			break;
		case 17:
			sprintf(buf, "Imagine the dastardly things I'll do to your corpse after you die, %s!",
				GET_REAL_NAME(vict));
			break;
		case 18:
			sprintf(buf, "Come on %s, you hit like a fairy!",
				GET_REAL_NAME(vict));
			break;
		case 19:
			sprintf(buf, "Rahnkara wont save your soul %s",
				GET_REAL_NAME(vict));
			break;
		case 20:
			sprintf(buf, "%s is a coward and soon to be a dead coward!",
				GET_REAL_NAME(vict));
			break;
		case 21:
			sprintf(buf, "I fear newbies more than %s!",
				GET_REAL_NAME(vict));
			break;
		case 25:
			sprintf(buf, "Thats right %s!  Run to Rahnkara...",
				GET_REAL_NAME(vict));
			break;
		case 26:
			sprintf(buf, "The Reception must be looking very inviting about now %s.",
				GET_REAL_NAME(vict));
			break;
		case 27:
			sprintf(buf, "There, there, no need to wet your pants %s.",
				GET_REAL_NAME(vict));
			break;
		case 28:
			sprintf(buf, "Ooh! You nearly hurt me there %s.",
				GET_REAL_NAME(vict));
			break;
		case 29:
			sprintf(buf, "Come back %s, I am not finished with you yet!",
				GET_REAL_NAME(vict));
			break;
		case 30:
			sprintf(buf, "Sure %s!  Pick a fight then flee like a womble!",
				GET_REAL_NAME(vict));
			break;
		case 31:
			sprintf(buf, "Your fighting style has two parts, %s: Crappy and really crappy.",
				GET_REAL_NAME(vict));
			break;

		default:
			break;
		}		/* END OF switch() */
	}			/* END OF flag == 1) */

	/* FOUND */
	if ((flag == 2) && (number(1, 5) == 1)) {
		switch (lv_number) {
		case 1:
			sprintf(buf, "I told you %s couldn't hide forever!",
				GET_REAL_NAME(vict));
			break;
		case 2:
			sprintf(buf, "I am going to tear %s to shreds!",
				GET_REAL_NAME(vict));
			break;
		case 3:
			sprintf(buf, "%s needs to find a new hiding place this one is too easy!!",
				GET_REAL_NAME(vict));
			break;
		case 4:
			sprintf(buf, "Today must be a good day to die, %s is about to.",
				GET_REAL_NAME(vict));
			break;
		case 5:
			sprintf(buf, "%s fled and couldn't get away. Now, I shall devour %s soul!",
				GET_REAL_NAME(vict), HSHR(vict));
			break;
		case 6:
			sprintf(buf, "I'm still trying to figure out if you are brave, or just plain foolish %s",
				GET_REAL_NAME(vict));
			break;
		case 7:
			sprintf(buf, "I shall feast on your bloated corpse for days %s",
				GET_REAL_NAME(vict));
			break;
		case 8:
			sprintf(buf, "It was foolish to try and kill me %s, remember that on your way to hell!",
				GET_REAL_NAME(vict));
			break;
		case 9:
			sprintf(buf, "%s why are you hiding under that table?",
				GET_REAL_NAME(vict));
			break;
		case 10:
			sprintf(buf, "I wish I didnt have to kill you, %s, but its just so much fun!",
				GET_REAL_NAME(vict));
			break;
		case 11:
			sprintf(buf, "I hope %s has a will, I hate seeing people fighting over a still-warm corpse",
				GET_REAL_NAME(vict));
			break;
		case 12:
			sprintf(buf, "I didn't think it was possible to miss as much as %s.",
				GET_REAL_NAME(vict));
			break;
		case 13:
			sprintf(buf, "Groveling won't save your miserable life %s.",
				GET_REAL_NAME(vict));
			break;
		case 14:
			sprintf(buf, "Oh, %s was confused... They thought I was the weak one.",
				GET_REAL_NAME(vict));
			break;
		case 15:
			sprintf(buf, "Muhahah %s is trying to kill me without any luck.",
				GET_REAL_NAME(vict));
			break;
		case 16:
			sprintf(buf, "At first I applauded your bravery, %s, but damn, you suck.",
				GET_REAL_NAME(vict));
			break;
		case 17:
			sprintf(buf, "%s could you come here a sec?  I have something for you.",
				GET_REAL_NAME(vict));
			break;
		case 18:
			sprintf(buf, "%s better go hide %s butt in the dressing room!",
				GET_REAL_NAME(vict), HSHR(vict));
			break;
		case 19:
			sprintf(buf, "Post suggestions for additional gossips, while I chase %s.",
				GET_REAL_NAME(vict));
			break;
		case 20:
			sprintf(buf, "Please tell %s that I want to hold %s corpse for %s.",
				GET_REAL_NAME(vict), HSHR(vict), HMHR(vict));
			break;
		case 21:
			sprintf(buf, "%s should be exploring Xanth instead of me killing %s.",
				GET_REAL_NAME(vict), HMHR(vict));
			break;
		case 22:
			sprintf(buf, "%s will need someone to retrieve %s corpse in about 2 seconds.",
				GET_REAL_NAME(vict), HSHR(vict));
			break;
		case 23:
			sprintf(buf, "%s, I'm going to show you just how mortal you are!",
				GET_REAL_NAME(vict));
			break;
		case 24:
			sprintf(buf, "You're going to have to do better than that, %s.",
				GET_REAL_NAME(vict));
			break;
		case 25:
			sprintf(buf, "Fear me puny mortals. I will crush %s as a sacrifice.",
				GET_REAL_NAME(vict));
			break;
		case 26:
			sprintf(buf, "Someone needs to call a paramedic, %s is gonna need it.",
				GET_REAL_NAME(vict));
			break;
		case 27:
			sprintf(buf, "I am gonna play kick the bucket with %s's head.",
				GET_REAL_NAME(vict));
			break;
		case 28:
			sprintf(buf, "I just can't get enough of this, %s!",
				GET_REAL_NAME(vict));
			break;
		case 29:
			sprintf(buf, "I didn't start it %s but i am sure gonna finish it!",
				GET_REAL_NAME(vict));
			break;
		case 30:
			sprintf(buf, "Geeze, if I make %s flee this much he's gonna be set back a level.",
				GET_REAL_NAME(vict));
			break;
		case 31:
			sprintf(buf, "%s parries the blow that would have been %s death *cackle*",
				GET_REAL_NAME(vict), HSHR(vict));
			break;
		case 32:
			sprintf(buf, "Run!, baby %s, Run!",
				GET_REAL_NAME(vict));
			break;
		case 33:
			sprintf(buf, "You really know how to suck, %s, but do you know how to blow?",
				GET_REAL_NAME(vict));
			break;

		default:
			break;
		}		/* END OF switch() */
	}			/* END OF flag == 1) */

	if (*(buf)) {
		do_gossip(ch, buf, CMD_GOSSIP);
	}
	return;

}				/* END OF at2100_do_mob_hunt_msgs() */


void at2200_do_rage(struct char_data * ch, char *arg, int cmd)
{


	if (!GET_FIGHTING(ch)) {
		act("You glare at everyone.",
		    TRUE, ch, 0, 0, TO_CHAR);
		act("$n glares at everyone in general.",
		    TRUE, ch, 0, 0, TO_ROOM);
		return;
	}

	if (IS_AFFECTED(ch, AFF_RAGE)) {
		send_to_char("You're already foaming at the mouth.\r\n", ch);
		return;
	}

	if (number(0, 101) > ch->skills[SKILL_RAGE].learned) {
		send_to_char("You taste blood, but fail to erupt into a rage.\r\n",
			     ch);
		return;
	}

	act("You erupt into a rage!",
	    TRUE, ch, 0, 0, TO_CHAR);
	act("$n starts to foam at the mouth with &rbloodlust&n!.",
	    TRUE, ch, 0, 0, TO_ROOM);
	SET_BIT(ch->specials.affected_by, AFF_RAGE);

	return;

}				/* END OF at2200_do_rage() */

void at2300_do_escape(struct char_data * ch, char *argument, int cmd)
{

	int i, moved, lv_dir = -1, percent;
	char buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	bzero(buf, sizeof(buf));

	percent = number(1, 101);
	if (percent > ch->skills[SKILL_ESCAPE].learned) {
		send_to_char("PANIC! You couldn't escape!\n\r", ch);
		return;
	}

	victim = ch->specials.fighting;

	/* WE AREN'T FIGHTING? */
	if (!(victim)) {
		send_to_char("You're pretty paranoid aren't you?\r\n", ch);
		act("$n looks frightened and tries to escape....from nothing.\r\n", TRUE, ch, 0, 0, TO_ROOM);
		return;
	}

	/* WE ARE FIGHTING */
	if (IS_AFFECTED(ch, AFF_RAGE)) {
		send_to_char("You are caught up in blood lust and wouldn't dream of leaving the carnage!\r\n", ch);
		return;
	}

	for (; isspace(*argument); argument++);
	argument = one_argument(argument, buf);

	if ((*buf) && (cmd == CMD_ESCAPE)) {
		lv_dir = old_search_block(buf, 0, strlen(buf), dirs, 0);
		lv_dir--;
		if ((lv_dir < 0) || (lv_dir > 5)) {
			send_to_char("Thats not a valid direction.\r\n", ch);
			return;
		}
		if (!CAN_GO(ch, lv_dir)) {
			send_to_char("You try to escape, but bang your head against a wall!\r\n", ch);
			act("$n panics, and attempts to flee but bumps against a wall!\r\n", TRUE, ch, 0, 0, TO_ROOM);
			return;
		}
	}
	else {
		for (i = 0; i < 6; i++) {
			if CAN_GO
				(ch, i) {
				lv_dir = i;
				break;
				}
		}
	}

	/* escape succeeded, moving to other room */

	if (lv_dir == -1)
		moved = FALSE;
	else
		moved = mo1000_do_simple_move(ch, lv_dir, FALSE, BIT0);
	li9900_gain_proficiency(ch, SKILL_ESCAPE);

	if (moved) {
		sprintf(buf, "You escape %s!\r\n", dirs[lv_dir]);
		send_to_char(buf, ch);
		sprintf(buf, "$n escapes %s!\r\n", dirs[lv_dir]);
		act(buf, TRUE, ch, 0, 0, TO_ROOM);
		if (ch->specials.fighting) {
			if (ch->specials.fighting->specials.fighting == ch)
				ft1400_stop_fighting(ch->specials.fighting, 0);
			WAIT_STATE(ch, PULSE_ESCAPE);
		}
		if (cl1900_at_war(ch, ch->specials.fighting))
			cl2000_player_escape(ch, ch->specials.fighting);
		ft1400_stop_fighting(ch, 0);
		at2000_do_mob_hunt_check(victim, ch, 1);
		WAIT_STATE(ch, PULSE_ESCAPE);
		GET_POS(ch) = POSITION_STANDING;
	}
	else {
		send_to_char("PANIC! You couldn't escape!\n\r", ch);
	}

	return;
}				/* END OF at2300_do_escape() */

void at2400_do_flurry(struct char_data * ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int lv_real_ac;
	int lv_wait = 3;
	int lv_cost = 0;
	float lv_totdmg = 0.5;
	int is_ninja = 0;
	signed char percent;

	if ((IS_PC(ch)) && IS_AFFECTED(ch, AFF_TRANSFORMED));
	is_ninja = IS_CASTED_ON(ch, SPELL_TRANSFORM_NINJA);

	if (is_ninja) {
		lv_wait = 1;
		lv_totdmg = 1;
	}
	else if (GET_LEVEL(ch) < 20) {
		lv_wait = 3;
	}
	else if (GET_LEVEL(ch) < 30) {
		lv_wait = 2;
		lv_cost = 10;
	}
	else {
		lv_wait = 1;
		lv_cost = 25;
	}

	if (GET_CLASS(ch) == CLASS_MONK) {
		lv_wait = 1;
		lv_cost = 10;
		lv_totdmg = lv_totdmg * 2;
	}
	
	if (ch->skills[SKILL_FLURRY].learned == 0 && !is_ninja) {
		send_to_char("You better leave all the martial arts to fighters.\n\r", ch);
		return;
	}

	if (GET_MOVE(ch) < lv_cost) {
		send_to_char("You're just too tired for that kind of thing.\n\r", ch);
		return;
	}


	if (GET_LEVEL(ch) > PK_LEV) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s flurry %s", GET_NAME(ch), argument);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	one_argument(argument, name);

	if (name[0] == 0) {
		/* WE DIDN'T GET AN ARGUMENT.  SEE IF WE ARE FIGHTING */
		if (ch->specials.fighting) {
			victim = ch->specials.fighting;
		}
		else {
			/* WE AREN'T FIGHTING AND NO ARG GIVEN */
			send_to_char("Who do you want to flurry attack?\n\r", ch);
			return;
		}
	}
	else {
		/* SEE IF THE THE SPECIFIED VICTIM IS AVAILABLE */
		victim = ha2125_get_char_in_room_vis(ch, name);
		if (!(victim)) {
			/* CAN'T LOCATE THAT VICTIM */
			send_to_char("Your good, that person didn't wait around to get attacked!\n\r", ch);
			return;
		}
	}

	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();


	if (victim == ch) {
		if (GET_LEVEL(ch) >= IMO_SPIRIT) {
			sprintf(buf, "&rYou try to attack yourself and fail.&n\n\r"
				"&RYou start punching yourself in a flurry.&n\r\n"
				"&rThat's gonna hurt tomorrow....&n\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "&r$n tries to flurry attack $mself and gets frustrated after $e fails.&n\n\r"
				"&R$n begins punching and kicking $mself to death.&n\r\n"
				"&rOuch that looked painful!&n\n\r");
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "%s L[%d] flurry attacks %sself to death.\r\n",
				GET_REAL_NAME(ch),
				GET_LEVEL(ch),
				HMHR(ch));
			do_info(buf, 1, MAX_LEV, ch);
			ft1800_raw_kill(ch, ch);
			return;
		}
		send_to_char("Aren't we funny today...\n\r", ch);
		return;
	}
	CHECK_FOR_CHARM();
	if ((GET_POS(ch) == POSITION_STANDING) &&
	    (victim != ch->specials.fighting)) {
		sprintf(buf, "FIRSTHIT: %s (%d/%d) flurrys %s (%d/%d).",
			GET_REAL_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		 GET_REAL_NAME(victim), GET_HIT(victim), GET_MAX_HIT(victim));
		main_log(buf);
		if (IS_PC(victim)) {
			spec_log(buf, PKILL_LOG);
		}
		else {
			spec_log(buf, FIRSTHIT_LOG);
		}
	}
	/* 101% is a complete failure */
	lv_real_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim)) + GET_BONUS_DEX(victim) +
		races[GET_RACE(victim)].adj_ac;
	percent = ((10 - (lv_real_ac / 10)) << 1) + number(1, 101);

	if (ch->in_room != victim->in_room) {
		send_to_char("I am afraid your opponent isn't around...\n\r", ch);
		lv_wait = 0;
		return;
	}

	if (percent > ch->skills[SKILL_FLURRY].learned) {
		DAMAGE(ch, victim, 0, SKILL_FLURRY);
	}
	else {
		li9900_gain_proficiency(ch, SKILL_FLURRY);
		DAMAGE(ch, victim, (int) ((GET_LEVEL(ch) >> 3) * ch->skills[SKILL_FLURRY].learned * lv_totdmg), SKILL_FLURRY);
	}

	GET_MOVE(ch) -= lv_cost;

	WAIT_STATE(ch, PULSE_VIOLENCE * lv_wait);
}				/* END OF at2400_do_flurry() */

/* Move Later To Another File*/

void do_ride(struct char_data * ch, char *argument, int cmd)
{

	struct char_data *victim;
	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];

	one_argument(argument, name);

	victim = ha2125_get_char_in_room_vis(ch, name);

	if (!(victim)) {
		send_to_char("Who did you want to mount??\n\r", ch);
		return;
	}

	if (victim == ch) {
		send_to_char("You try to mount yourself.. boy you look funny!", ch);
		return;
	}

	if (RIDING(ch)) {
		send_to_char("You are already mounted.\r\n", ch);
		return;
	}
	if (RIDDEN_BY(ch)) {
		send_to_char("Dismount from your mount first!\r\n", ch);
		return;
	}
	if (RIDING(victim)) {
		send_to_char("They are already mounted themselves!\r\n", ch);
		return;
	}
	if (RIDDEN_BY(victim)) {
		send_to_char("They are being ridden by someone already!\r\n", ch);
		return;
	}


	if (victim->master && victim->master != ch) {
		send_to_char("That mount is already being used by someone.\r\n", ch);
		return;
	}

	if (victim->master == NULL)
		mo4000_do_follow(victim, GET_NAME(ch), CMD_FOLLOW);

	RIDING(ch) = victim;
	RIDDEN_BY(victim) = ch;
	sprintf(buf, "You mount %s.\r\n", GET_REAL_NAME(victim));
	send_to_char(buf, ch);
	sprintf(buf, "%s mounts you.\r\n", GET_REAL_NAME(ch));
	send_to_char(buf, victim);
	act("$n mounts $N.", TRUE, ch, 0, RIDING(ch), TO_NOTVICT);



}

void do_dismount(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	if (RIDING(ch) == NULL)
		send_to_char("You are not even mounted.\r\n", ch);
	else {

		RIDDEN_BY(RIDING(ch)) = NULL;

		act("$n dismounts $N.", TRUE, ch, 0, RIDING(ch), TO_NOTVICT);
		sprintf(buf, "%s dismounts you.\r\n", GET_REAL_NAME(ch));
		send_to_char(buf, RIDING(ch));
		sprintf(buf, "You dismount %s\r\n", GET_REAL_NAME(RIDING(ch)));
		send_to_char(buf, ch);

		RIDING(ch) = NULL;
	}

}
