/* mo */
/* gv_location: 10501-11000 */
/* ****************************************************************** *
*  file: move.c , Implementation of commands         Part of DIKUMUD *
*  Usage : Movement commands, close/open & lock/unlock doors.        *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform. *
* ****************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "constants.h"
#include "spells.h"
#include "ansi.h"
#include "xanth.h"
#include "globals.h"
#include "func.h"

int special(struct char_data * ch, int cmd, char *arg);

int mo1000_do_simple_move(struct char_data * ch, int cmd, int following, int lv_flag)
{
	/* Assumes, 1. That there is no master and no followers. 2. That the
	 * direction exists.
	 * 
	 * Returns : 1 : If succes. 0 : If fail -1 : If dead. */
	char tmp[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	int was_in, to_room, rc, need_movement, found;
	struct affected_type *hjp, *next_hjp;



	found = FALSE;

	if (IS_AFFECTED(ch, AFF_HOLD_PERSON)) {
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("Rock doesn't do much of anything!\r\n", ch);
			return (FALSE);
		}
	}

	if (special(ch, cmd + 1, ""))	/* Check for special routines (North is
					 * 1) */
		return (FALSE);

	need_movement = (movement_loss[world[ch->in_room].sector_type] +
			 movement_loss[world[world[ch->in_room].dir_option[cmd]->to_room].sector_type]) / 2;

	to_room = world[ch->in_room].dir_option[cmd]->to_room;
	rc = li3000_is_blocked_from_room(ch, to_room, 0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return (FALSE);
	}

	if (IS_PC(ch)) {
		if ((HUNTING(ch) && GET_MOVE(ch) < need_movement + 2) ||
		    (GET_MOVE(ch) < need_movement)) {
			if (!following)
				send_to_char("You are too exhausted.\n\r", ch);
			else
				send_to_char("You are too exhausted to follow.\n\r", ch);
			return (FALSE);
		}
	}

	if (IS_PC(ch) && GET_LEVEL(ch) < IMO_SPIRIT && !IS_AFFECTED(ch, AFF_FLY)) {
		if HUNTING
			(ch) {
			GET_MOVE(ch) -= need_movement + 2;
			}
		else {
			if (RIDING(ch))
				GET_MOVE(RIDING(ch)) -= need_movement;
			else
				GET_MOVE(ch) -= need_movement;
		}
	}

	if (!(IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		if ((!IS_SET(lv_flag, BIT1)) &&
		    (!IS_AFFECTED(ch, AFF_SNEAK) || IS_SET(lv_flag, BIT0))) {
			if (world[ch->in_room].number == XANTH_NORTH_VILLAGE &&
			    cmd == CMD_UP - 1) {
				sprintf(tmp, "$n leaves southwest.");
				act(tmp, TRUE, ch, 0, 0, TO_ROOM);
			}
			else {

				if (!RIDING(ch) && !RIDDEN_BY(ch)) {
					sprintf(tmp, "$n leaves %s.", dirs[cmd]);
					act(tmp, TRUE, ch, 0, 0, TO_ROOM);
				}
				if (RIDING(ch) && RIDING(ch)->in_room == ch->in_room) {

					sprintf(tmp, "You ride $N %s.", dirs[cmd]);
					act(tmp, TRUE, ch, 0, RIDING(ch), TO_CHAR);

					/* One Problem: Sends to the mount too */
					sprintf(tmp, "$n rides $N %s.", dirs[cmd]);
					act(tmp, TRUE, ch, 0, RIDING(ch), TO_ROOM);

					sprintf(tmp, "%s rides you %s.\r\n", GET_REAL_NAME(ch), dirs[cmd]);
					send_to_char(tmp, RIDING(ch));

				}


			}
		}
	}




	was_in = ch->in_room;
	to_room = world[was_in].dir_option[cmd]->to_room;




	/* are trying to go to a NO_ENTER zone */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    world[ch->in_room].zone != world[to_room].zone) {
		if (IS_SET(zone_table[world[to_room].zone].flags, ZONE_NO_ENTER)) {
			send_to_char("You cant go that way (Zone is NO_ENTER).\n\r", ch);
			if (IS_PC(ch) && HUNTING(ch)) {
				HUNTING(ch) = 0;
			}
			return (FALSE);
		}
	}

	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, to_room);

	//When mount moves involuntarily

		if (RIDDEN_BY(ch) && RIDDEN_BY(ch)->in_room == was_in) {
		ha1500_char_from_room(RIDDEN_BY(ch));
		ha1600_char_to_room(RIDDEN_BY(ch), to_room);
		sprintf(tmp, "%s takes you %s.\r\n", GET_REAL_NAME(ch), dirs[cmd]);
		send_to_char(tmp, RIDDEN_BY(ch));

		in3000_do_look(RIDDEN_BY(ch), "\0", 0);

	}




	if (!(IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		if (!IS_AFFECTED(ch, AFF_SNEAK) || lv_flag == 1) {
			if (HUNTING(ch)) {
				act("$n has arrived, $e appears to be hunting someone.", TRUE, ch, 0, 0, TO_ROOM);
			}
			else {
				if (RIDDEN_BY(ch) && RIDDEN_BY(ch)->in_room == ch->in_room) {
					;
				}
				else if (RIDING(ch) && RIDING(ch)->in_room == was_in) {
					sprintf(tmp, "$n the %s has arrived, riding $N.",
						races[GET_RACE(ch)].name);
					act(tmp, TRUE, ch, 0, RIDING(ch), TO_ROOM);

				}
				else {
					if (IS_PC(ch)) {
						sprintf(buf, "$n the %s has arrived.",
						    races[GET_RACE(ch)].name);
					}
					else {
						sprintf(buf, "$n has arrived.");
					}
					act(buf, TRUE, ch, 0, 0, TO_ROOM);
				}
			}

		}
	}

	in3000_do_look(ch, "\0", 0);
	/* are we leaving a testing zone */
	if (world[ch->in_room].zone != world[was_in].zone) {
		if (IS_SET(zone_table[world[was_in].zone].flags, ZONE_TESTING))
			ha3700_char_del_zone_objs(ch);
	}

	if ((world[ch->in_room].sector_type == SECT_UNDERWATER)
	    && GET_LEVEL(ch) < IMO_LEV &&
	    !IS_AFFECTED(ch, AFF_BREATHWATER)) {
		ansi_act("You're starting to drown!", FALSE, ch, 0, 0, TO_CHAR, CLR_DAM, 0);
		GET_HIT(ch) -= MAXV(10, (GET_HIT(ch) / 10));
		ft1200_update_pos(ch);

		/* if (GET_HIT(ch) < 0) { bzero(buf, sizeof(buf)); sprintf(buf,
		 * "%s L[%d] has been killed by drowning.\r\n",
		 * GET_REAL_NAME(ch), GET_LEVEL(ch)); do_info(buf, 1,
		 * IMO_IMM, ch); bzero(buf, sizeof(buf)); sprintf(buf, "%s
		 * L[%d] has been killed by drowning in room %d.\r\n",
		 * GET_REAL_NAME(ch), GET_LEVEL(ch),
		 * world[ch->in_room].number); do_info(buf, IMO_IMM,
		 * MAX_LEV, ch); GET_HOW_LEFT(ch) = LEFT_BY_DEATH_ROOM;
		 * GET_DEATHS(ch)++; if (GET_LEVEL(ch) > 20) { die(ch, ch); } } */
	}
	if (IS_SET(world[ch->in_room].room_flags, RM1_DRAIN_MAGIC) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		GET_MANA(ch) >>= 1;
		for (hjp = ch->affected; hjp; hjp = next_hjp) {
			next_hjp = hjp->next;
			ha1325_affect_remove(ch, hjp, REMOVE_FOLLOWER);
		}
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_HARMFULL1) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		ansi_act("Ouch! that hurt.", FALSE, ch, 0, 0, TO_CHAR, CLR_DAM, 0);
		GET_HIT(ch) -= number(1, 8);
		ft1200_update_pos(ch);
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_HARMFULL2) &&
	    GET_LEVEL(ch) < IMO_LEV) {
		ansi_act("Ouch! that really hurt.", FALSE, ch, 0, 0, TO_CHAR, CLR_DAM, 0);
		GET_HIT(ch) -= number(1, 20);
		ft1200_update_pos(ch);
	}

	switch (GET_POS(ch)) {
	case POSITION_MORTALLYW:
		act("$n is mortally wounded, and will die soon, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
		act("You are mortally wounded, and will die soon, if not aided.", FALSE, ch, 0, 0, TO_CHAR);
		break;
	case POSITION_INCAP:
		act("$n is incapacitated and will slowly die, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
		act("You are incapacitated an will slowly die, if not aided.", FALSE, ch, 0, 0, TO_CHAR);
		break;
	case POSITION_STUNNED:
		act("$n is stunned, but will probably regain consciousness again.", TRUE, ch, 0, 0, TO_ROOM);
		act("You're stunned, but will probably regain consciousness again.", FALSE, ch, 0, 0, TO_CHAR);
		break;
	case POSITION_DEAD:
		act("$n is dead! R.I.P.", TRUE, ch, 0, 0, TO_ROOM);
		act("You are dead!  Sorry...", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
		break;
	default:
		if (GET_HIT(ch) < (li1500_hit_limit(ch) / 5))
			ansi_act("You wish that your wounds would stop BLEEDING that much!", FALSE, ch, 0, 0, TO_CHAR, CLR_DAM, 0);
		break;
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_DEATH) &&
	    GET_LEVEL(ch) < IMO_SPIRIT &&
	    !(ha1175_isexactname(GET_NAME(ch),
				 zone_table[world[ch->in_room].zone].lord))) {
		ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_ROOM);
		return (FALSE);
	}

	/* The move worked, so let speedwalk know */
	gv_move_succeeded = TRUE;
	return (1);

}				/* END OF mo1000_do_simple_move */


void mo1100_do_scan(struct char_data * ch, char *arg, int cmd)
{

	int idx, lv_single_direction, lv_rooms[MAX_DIRS], lv_moves[MAX_DIRS], lv_pass_count[MAX_DIRS], lv_max_rooms;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	struct char_data *vict;



	/* IF BLIND, FORGET IT! */
	if (GET_LEVEL(ch) < IMO_SPIRIT && IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("You're blind and can't see a thing.\r\n", ch);
		return;
	}

	/* DID WE GET AN ARG? */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	lv_max_rooms = 0;
	for (idx = 0; idx < MAX_DIRS; idx++) {
		lv_rooms[idx] = ch->in_room;
		lv_moves[idx] = 0;
		lv_pass_count[idx] = 0;
	}

	lv_single_direction = 0;

	/* IF IMM, SEE IF WE SPECIFIED MAX ROOMS */
	if (*buf) {
		if (GET_LEVEL(ch) > PK_LEV) {
			if (is_number(buf)) {
				lv_max_rooms = atoi(buf);
				/* ADJUST ARGUMENTS */
				for (; isspace(*arg); arg++);
				bzero(buf, sizeof(buf));
				arg = one_argument(arg, buf);
			}
		}
	}

	/* IF USER SPECIFIED ROOM NUMBERS, MAKE SURE IN RANGE */
	if (lv_max_rooms != 0) {
		if (lv_max_rooms < 1) {
			send_to_char("Number must be greater than 0\r\n", ch);
			return;
		}
		if (GET_LEVEL(ch) < IMO_IMM) {
			if (lv_max_rooms > 5) {
				send_to_char("You can only view up to 5 rooms away\r\n",
					     ch);
				return;
			}
		}
		if (lv_max_rooms > 50) {
			send_to_char("50 rooms is max\r\n", ch);
			return;
		}
	}

	if (*buf) {
		for (idx = 0; idx < MAX_DIRS; idx++) {
			if (is_abbrev(buf, (char *) dirs[idx])) {
				if (lv_max_rooms > 0)
					lv_moves[idx] = 1000000;	/* really high so we */
				else	/* don't run out.    */
					lv_moves[idx] = 12;	/* extra for focus */
				lv_single_direction = idx + 1;
			}
		}
		if (!lv_single_direction) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "%s is invalid.  Directions are: north, south, east, west, up, down.\r\n", buf);
			send_to_char(buf2, ch);
			return;
		}
	}
	else {
		/* IF WE DON'T HAVE A DIRECTION, SET DEFAULTS */
		for (idx = 0; idx < MAX_DIRS; idx++) {
			if (lv_max_rooms > 0)
				lv_moves[idx] = 1000000;	/* really high so we
								 * don't */
			else	/* run out of moves        */
				lv_moves[idx] = 10;
		}
	}			/* END OF WE GOT AN ARG */

	/* LET OTHERS KNOW WHAT YOUR DOING */
	bzero(buf2, sizeof(buf2));
	if (lv_single_direction)
		sprintf(buf2, "$n peers %s", dirs[lv_single_direction - 1]);
	else
		sprintf(buf2, "$n peers all around.");
	act(buf2, TRUE, ch, 0, 0, TO_ROOM);

	/* IMMEDIATE ROOM */
	for (vict = world[ch->in_room].people; vict;
	     vict = vict->next_in_room) {
		if ((ch != vict) &&
		    (GET_LEVEL(ch) >= GET_VISIBLE(vict)) &&
		    (GET_LEVEL(ch) >= GET_GHOST(vict)) && //Incognito & ghost Shalira 22.07 .011
		    (GET_LEVEL(ch) >= GET_INCOGNITO(vict)) && //Incognito & ghost Shalira 22.07 .011
		    ((CAN_SEE(ch, vict)) ||
		     (IS_AFFECTED(ch, AFF_SENSE_LIFE) &&
		      !IS_AFFECTED(vict, AFF_HIDE)))) {
			bzero(buf, sizeof(buf));
			if ((IS_AFFECTED(vict, AFF_HIDE)) && GET_LEVEL(ch) < IMO_SPIRIT)
				strcpy(buf, "Hidden life form           ");
			else {
				if (IS_NPC(vict))
					sprintf(buf, "%-27s", vict->player.short_descr);
				else
					sprintf(buf, "%-27s", vict->player.name);
			}
			strcat(buf, " : Immediate area\r\n");
			send_to_char(buf, ch);
		}
	}

	while ((lv_moves[0] + lv_moves[1] + lv_moves[2] +
		lv_moves[3] + lv_moves[4] + lv_moves[5]) > 0) {
		for (idx = 0; idx < MAX_DIRS; idx++) {
			if (lv_moves[idx] > 0) {
				mo1200_scan_room_and_print(ch, arg, (int *) &lv_rooms[idx], lv_pass_count[idx], (int *) &lv_moves[idx], idx);
				lv_pass_count[idx]++;
				if (lv_max_rooms && lv_pass_count[idx] > lv_max_rooms)
					lv_moves[idx] = 0;	/* SO WE'LL EXIT */
				/* INFINATE LOOP SAFETY CHECK */
				if (lv_pass_count[idx] > 100)
					lv_moves[idx] = 0;	/* SO WE'LL EXIT */
			}
		}
	}
	return;
}				/* END OF mo1100_do_scan() */


void mo1200_scan_room_and_print(struct char_data * ch, char *arg, int *lv_current_room, int lv_pass_count, int *lv_current_move, int lv_dir)
{

	int lv_room;
	char buf[MAX_STRING_LENGTH], distance_string[MAX_INPUT_LENGTH];
	struct char_data *vict;



	lv_room = *lv_current_room;
	if (lv_room < 0) {
		sprintf(buf, "ERROR: Got a negative room while %s scanned in %d.\r\n",
			GET_REAL_NAME(ch),
			world[ch->in_room].number);
		do_sys(buf, MAXV(IMO_IMM, GET_LEVEL(ch) + 1), ch);
		spec_log(buf, ERROR_LOG);
		*lv_current_move = 0;
		return;
	}			/* END OF room < 0 */

	/* LOAD DISTANCE STRING */
	bzero(distance_string, sizeof(distance_string));
	if (lv_pass_count == 0)
		strcpy(distance_string, " : Immediately ");
	if (lv_pass_count == 1)
		strcpy(distance_string, " : Far ");
	if (lv_pass_count == 2)
		strcpy(distance_string, " : Way Far ");
	if (lv_pass_count == 3)
		strcpy(distance_string, " : Way Way Far ");
	if (lv_pass_count == 4)
		strcpy(distance_string, " : Way Way Way Far ");
	if (lv_pass_count == 5)
		strcpy(distance_string, " : Way Way Way Way Far ");
	if (lv_pass_count == 6)
		strcpy(distance_string, " : Way Way Way Way Way Far ");
	if (lv_pass_count == 7)
		strcpy(distance_string, " : Way Way Way Way Way Way Far ");
	if (lv_pass_count == 8)
		strcpy(distance_string, " : Way Way Way Way Way Way Way Far ");
	if (lv_pass_count > 8)
		strcpy(distance_string, " : Somewhere to the ");

	strcat(distance_string, dirs[lv_dir]);

	/* IS THERE SOMETHING IN THAT DIRECTION */
	if (!world[lv_room].dir_option[lv_dir]) {
		*lv_current_move = 0;
		return;
	}

	/* IF ITS HIDDEN, SKIP */
	if (IS_SET(world[lv_room].dir_option[lv_dir]->exit_info, EX_HIDDEN)) {
		*lv_current_move = 0;
		return;
	}

	/* IS IT OPEN? */
	if (IS_SET(world[lv_room].dir_option[lv_dir]->exit_info, EX_CLOSED)) {
		bzero(buf, sizeof(buf));
		if (world[lv_room].dir_option[lv_dir]->keyword) {
			sprintf(buf, "%-9s BLOCKS your view %s\r\n",
				ha1100_fname(world[lv_room].dir_option[lv_dir]->keyword),
				distance_string);
		}
		else {
			sprintf(buf, "Something BLOCKS your view %s\r\n",
				distance_string);

		}
		send_to_char(buf, ch);
		*lv_current_move = 0;
		return;
	}

	/* DOES IT GO SOMEWHERE? */
	if (EXIT(ch, lv_dir)->to_room == NOWHERE) {
		*lv_current_move = 0;
		return;
	}

	/* SUBTRACT COST OF TERRAIN */
	if (world[lv_room].sector_type == SECT_INSIDE ||
	    world[lv_room].sector_type == SECT_CITY ||
	    world[lv_room].sector_type == SECT_FIELD)
		*lv_current_move = *lv_current_move - 2;
	else {
		if (world[lv_room].sector_type == SECT_FOREST ||
		    world[lv_room].sector_type == SECT_HILLS)
			*lv_current_move = *lv_current_move - 3;
		else
			*lv_current_move = *lv_current_move - 9;
	}
	*lv_current_room = world[lv_room].dir_option[lv_dir]->to_room;

/*
 bzero(buf, sizeof(buf));
 sprintf(buf, "went %s to room %d\r\n", dirs[lv_dir], *lv_current_room);
 send_to_char(buf, ch);
 */


	for (vict = world[*lv_current_room].people; vict;
	     vict = vict->next_in_room) {
		buf[0] = 0;
		if ((ch != vict) &&
		    (GET_LEVEL(ch) >= GET_VISIBLE(vict)) &&
		    (GET_LEVEL(ch) >= GET_GHOST(vict)) && //Incognito & ghost Shalira 22.07 .011
		    (GET_LEVEL(ch) >= GET_INCOGNITO(vict)) && //Incognito & ghost Shalira 22.07 .011
		    ((CAN_SEE(ch, vict)) ||
		     (IS_AFFECTED(ch, AFF_SENSE_LIFE) &&
		      !IS_AFFECTED(vict, AFF_HIDE)))) {
			bzero(buf, sizeof(buf));
			if ((IS_AFFECTED(vict, AFF_HIDE)) && GET_LEVEL(ch) < IMO_SPIRIT)
				strcpy(buf, "Hidden life form           ");
			else {
				if (IS_NPC(vict))
					sprintf(buf, "%-27s", vict->player.short_descr);
				else
					sprintf(buf, "%-27s", vict->player.name);
			}
		}
		if (*buf) {
			strcat(buf, distance_string);
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}
	}

	return;

}				/* END OF mo1200_scan_room_and_print() */


void mo1500_do_move(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_INPUT_LENGTH];
	int rc, lv_move_dir, was_in, lv_mob_num;
	struct follow_type *k, *next_dude;
	struct char_data *holder;



	/* assume doesnt work as default */
	gv_move_succeeded = FALSE;

	if (cmd < 0 || cmd > MAX_DIRS) {
		send_to_char("Alas, you cannot go that way...\r\n", ch);
		return;
	}

	/* PUT CMD ON ZERO ARRAY BOUNDARY */
	lv_move_dir = cmd - 1;

	if (IS_AFFECTED(ch, AFF_HOLD_PERSON)) {
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't move, you are a rock right now.\r\n", ch);
			return;
		}
	}

	if (!world[ch->in_room].dir_option[lv_move_dir]) {
		send_to_char("Alas, you cannot go that way...\r\n", ch);
		if (IS_PC(ch) && HUNTING(ch)) {
			HUNTING(ch) = 0;
		}
		return;
	}

	/* IF THERE IS A CLOSED DOOR, TRY TO OPEN IT */
	if (IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_CLOSED)) {
		if (!IS_AFFECTED(ch, AFF_CHARM)) {
			if (IS_SET(GET_ACT2(ch), PLR2_OPEN_DOORS)) {
				sprintf(buf, "%s", dirs[lv_move_dir]);
				mo1700_do_open(ch, buf, CMD_OPEN);
			}	/* END OF MOB OPENS DOORS */
		}		/* END OF NOT CHARMED */
	}			/* END OF CLOSED EXIT */

	/* IS IT OPEN? */
	if (IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_CLOSED)) {
		bzero(buf, sizeof(buf));
		if (EXIT(ch, lv_move_dir)->keyword) {
			sprintf(buf, "The %s seems to be closed.\r\n",
				ha1100_fname(EXIT(ch, lv_move_dir)->keyword));
		}
		else {
			sprintf(buf, "It seems to be closed.\r\n");
		}
		send_to_char(buf, ch);
		if (IS_PC(ch) && HUNTING(ch)) {
			HUNTING(ch) = 0;
		}
		return;
	}

	/* DOES IT GO SOMEWHERE? */
	if (EXIT(ch, lv_move_dir)->to_room == NOWHERE) {
		send_to_char("Alas, you can't go that way.\r\n", ch);
		if (IS_PC(ch) && HUNTING(ch)) {
			HUNTING(ch) = 0;
		}
		return;
	}

	if (IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_CLIMB_ONLY)) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You can't get there without something to climb on.\r\n", ch);
			return;
		}
	}

	if (IS_PC(ch) &&
	    GET_LEVEL(ch) < IMO_SPIRIT &&
	    IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_NOCHAR)) {
		send_to_char("Very strange....  A power won't let you get through there!\r\n", ch);
		return;
	}
	if (IS_NPC(ch) &&
	    !IS_AFFECTED(ch, AFF_CHARM) &&
	    IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_NOMOB)) {
		send_to_char("Very strange....  A power won't let you get through there!\r\n", ch);
		return;
	}			/* END OF else */
	if (IS_PC(ch) && GET_LEVEL(ch) < IMO_SPIRIT &&
	    IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_NO_MOBS_IN_ROOM)) {
		holder = world[ch->in_room].people;
		while (holder) {
			if (!IS_PC(holder)) {
				act("$N humiliates you and blocks your way!", FALSE, ch, 0, holder, TO_CHAR);
				act("$N humiliates $n and blocks $s way!", FALSE, ch, 0, holder, TO_ROOM);
				return;
			}
			holder = holder->next_in_room;
		}
	}

	/* DO WE NEED TO PROTECT AN AVATAR FROM A DT? */
	rc = ha9800_protect_char_from_dt(ch,
					 EXIT(ch, lv_move_dir)->to_room, 0);
	if (rc) {
		return;
	}

	if (!ch->followers && !ch->master) {
		mo1000_do_simple_move(ch, lv_move_dir, FALSE, 0);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM) &&
	    (ch->master) &&
	    (ch->in_room == ch->master->in_room)) {
		if (IS_NPC(ch)) {
			lv_mob_num = mob_index[ch->nr].virtual;
		}
		else {
			lv_mob_num = 0;
		}
		if (lv_mob_num > 2689 && lv_mob_num < 2700)
			act("$n snarls with hatred at $N for binding it to this plane.", FALSE, ch, 0, ch->master, TO_ROOM);
		/* WE AREN'T returning() FROM HERE SO THE MOB CAN MOVE */
		else {
			send_to_char("The thought of leaving your master makes you weep.\n\r", ch);
			act("$n bursts into tears.", FALSE, ch, 0, 0, TO_ROOM);
			return;
		}
	}

	was_in = ch->in_room;

	/* Move the character */
	rc = mo1000_do_simple_move(ch, lv_move_dir, TRUE, 0);



	if (rc == 1) {




		if (ch->followers) {	/* If succes move followers */

			for (k = ch->followers; k; k = next_dude) {
				next_dude = k->next;

				if ((was_in == k->follower->in_room)) {
					if (IS_NPC(k->follower) && IS_SET(world[ch->in_room].room_flags, RM1_NO_MOB)) {
						send_to_char("You are not allowed in there.\r\n", k->follower);
					}
					else {
						if (GET_POS(k->follower) >= POSITION_STANDING) {
							act("You follow $N.", FALSE, k->follower, 0, ch, TO_CHAR);
							lv_move_dir++;
							send_to_char("\r\n", k->follower);
							mo1500_do_move(k->follower, arg, lv_move_dir);
							lv_move_dir--;
						}
						else {
							act("You would follow $N, but you aren't standing.",
							    FALSE, k->follower, 0, ch, TO_CHAR);
						}
					}
				}
			}
		}
	}

	return;

}				/* END OF mo1500_do_move() */


int mo1600_find_door(struct char_data * ch, char *type, char *dir)
{
	char buf[MAX_STRING_LENGTH];
	int door, lv_dir;



	if (*dir) {		/* a direction was specified */
		/* Partial Match */
		if ((door = search_block(dir, dirs, FALSE)) == -1) {
			send_to_char("That's not a direction.\n\r", ch);
			return (-1);
		}

		if (EXIT(ch, door))
			if (EXIT(ch, door)->keyword)
				if (ha1150_isname(type, EXIT(ch, door)->keyword))
					return (door);
				else {
					sprintf(buf, "I see no %s there.\n\r", type);
					send_to_char(buf, ch);
					return (-1);
				}
			else
				return (door);
		else {
			send_to_char("I really don't think thats a door.\n\r", ch);
			return (-1);
		}
	}

	else {			/* try to locate the keyword */
		for (door = 0; door <= 5; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->keyword)
					if (ha1150_isname(type, EXIT(ch, door)->keyword))
						return (door);

		/* OK, WE DIDN'T FIND IT, SEE IF type IS A DIRECTION */
		lv_dir = old_search_block(type, 0, strlen(type), dirs, 0);
		if (lv_dir > 0) {
			/* AH HA, ITS A DIRECTION, IS THERE AN EXIT THERE? */
			lv_dir--;
			if (EXIT(ch, lv_dir)) {
				return (lv_dir);
			}
		}

		sprintf(buf, "I see no %s here.\n\r", type);
		send_to_char(buf, ch);
		return (-1);
	}

}				/* END OF mo1600_find_door() */


void mo1700_do_open(struct char_data * ch, char *arg, int cmd)
{
	int door, other_room;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	struct room_direction_data *back;
	struct obj_data *obj;
	struct char_data *victim;



	argument_interpreter(arg, type, dir);

	if (!*type) {
		send_to_char("Open what?\n\r", ch);
		return;
	}
	if (ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj)) {

		/* this is an object */

		if (obj->obj_flags.type_flag != ITEM_CONTAINER &&
		    obj->obj_flags.type_flag != ITEM_QSTCONT)
			send_to_char("That's not a container.\n\r", ch);
		else if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
			send_to_char("But it's already open!\n\r", ch);
		else if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
			send_to_char("You can't do that.\n\r", ch);
		else if (IS_SET(obj->obj_flags.value[1], CONT_LOCKED))
			send_to_char("It seems to be locked.\n\r", ch);
		else if (obj->obj_flags.value[1] == ch->nr) {
			send_to_char("No need to open the corpse to get your booty\n\r", ch);
		}
		else {
			REMOVE_BIT(obj->obj_flags.value[1], CONT_CLOSED);
			act("You open $p.", FALSE, ch, obj, 0, TO_CHAR);
			act("$n opens $p.", FALSE, ch, obj, 0, TO_ROOM);
		}
		return;
	}

	/* perhaps it is a door */
	door = mo1600_find_door(ch, type, dir);
	if (door >= 0) {
		if (!IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR))
			send_to_char("That isn't a door.\n\r", ch);
		else if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
			send_to_char("It's already open!\n\r", ch);
		else if (IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED))
			send_to_char("It seems to be locked.\n\r", ch);
		else {
			REMOVE_BIT(EXIT(ch, door)->exit_info, EX_CLOSED);
			if (EXIT(ch, door)->keyword)
				act("$n opens the $F.", FALSE, ch, 0, EXIT(ch, door)->keyword,
				    TO_ROOM);
			else
				act("$n opens the door.", FALSE, ch, 0, 0, TO_ROOM);
			send_to_char("Ok.\n\r", ch);
			/* now for opening the OTHER side of the door! */
			if ((other_room = EXIT(ch, door)->to_room) != NOWHERE)
				if ((back = world[other_room].dir_option[reverse_dir[door]]))
					if (back->to_room == ch->in_room) {
						REMOVE_BIT(back->exit_info, EX_CLOSED);
						if (back->keyword) {
							sprintf(buf, "The %s is opened from the other side.\n\r",
								ha1100_fname(back->keyword));
							co3300_send_to_room(buf, EXIT(ch, door)->to_room);
						}
						else
							co3300_send_to_room("The door is opened from the other side.\n\r", EXIT(ch, door)->to_room);
					}
		}
	}
}				/* END OF mo1700_do_open() */


void mo1800_do_close(struct char_data * ch, char *arg, int cmd)
{
	int door, other_room;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	struct room_direction_data *back;
	struct obj_data *obj;
	struct char_data *victim;




	argument_interpreter(arg, type, dir);

	if (!*type) {
		send_to_char("Close what?\n\r", ch);
	}
	else if (ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj)) {

		/* this is an object */

		if (obj->obj_flags.type_flag != ITEM_CONTAINER &&
		    obj->obj_flags.type_flag != ITEM_QSTCONT)
			send_to_char("That's not a container.\n\r", ch);
		else if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
			send_to_char("But it's already closed!\n\r", ch);
		else if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
			send_to_char("This item isn't closable.\n\r", ch);
		else {
			SET_BIT(obj->obj_flags.value[1], CONT_CLOSED);
			act("You close $p.", FALSE, ch, obj, 0, TO_CHAR);
			act("$n closes $p.", FALSE, ch, obj, 0, TO_ROOM);
		}
		return;
	}

	if ((door = mo1600_find_door(ch, type, dir)) >= 0) {

		/* Or a door */

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR))
			send_to_char("That's absurd.\n\r", ch);
		else if (IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
			send_to_char("It's already closed!\n\r", ch);
		else {
			SET_BIT(EXIT(ch, door)->exit_info, EX_CLOSED);
			if (EXIT(ch, door)->keyword)
				act("$n closes the $F.", 0, ch, 0, EXIT(ch, door)->keyword, TO_ROOM);
			else
				act("$n closes the door.", FALSE, ch, 0, 0, TO_ROOM);
			send_to_char("Ok.\n\r", ch);

			/* now for closing the other side, too */
			if ((other_room = EXIT(ch, door)->to_room) != NOWHERE)
				if ((back = world[other_room].dir_option[reverse_dir[door]]))
					if (back->to_room == ch->in_room) {
						SET_BIT(back->exit_info, EX_CLOSED);
						if (back->keyword) {
							sprintf(buf, "The %s closes quietly.\n\r", back->keyword);
							co3300_send_to_room(buf, EXIT(ch, door)->to_room);
						}
						else
							co3300_send_to_room("The door closes quietly.\n\r", EXIT(ch, door)->to_room);
					}
		}

	}
}				/* END OF mo1800_do_close() */


void mo1900_check_key_usage(struct char_data * ch, struct obj_data * obj)
{



	/* UNLIMITED USES? */

	if ((obj->obj_flags.value[1] < 1) && (!IS_SET(GET_OBJ1(obj), OBJ1_OMAX))) {
		return;
	}

	/* IF WE HAVE MORE THAN ONE USE, SUBTRACT ONE OF THEM */
	if (obj->obj_flags.value[1] > 1) {
		obj->obj_flags.value[1]--;
		return;
	}

	/* We've used it for the last time */
	act("$p crumbles to dust.", FALSE, ch, obj, 0, TO_CHAR);
	int idx;
	for (idx = 0; idx < MAX_WEAR_SLOTS; idx++) {
		if (obj == ch->equipment[idx]) {
			ha1700_obj_to_char(ha1930_unequip_char(ch, idx), ch);
		}
	}
	ha2700_extract_obj(obj);

	return;

}				/* END OF mo1900_check_key_usage() */


/* IF lv_flag == BIT0, CHECK IS MADE FOR NUMBER OF USES */
/*                                                      */
int mo2000_has_key(struct char_data * ch, int key, int lv_flag)
{
	struct obj_data *lv_obj;



	for (lv_obj = ch->carrying; lv_obj; lv_obj = lv_obj->next_content) {
		if (lv_obj->item_number > 0 && obj_index[lv_obj->item_number].virtual == key) {
			if (IS_SET(lv_flag, BIT0)) {
				mo1900_check_key_usage(ch, lv_obj);
			}
			return (TRUE);
		}
	}

	lv_obj = ch->equipment[HOLD];
	if (lv_obj) {
		if (obj_index[lv_obj->item_number].virtual == key) {
			if (IS_SET(lv_flag, BIT0)) {
				mo1900_check_key_usage(ch, lv_obj);
			}
			return (TRUE);
		}
	}

	if (GET_LEVEL(ch) >= IMO_IMP) {
		send_to_char("You call upon your Godly powers\r\n", ch);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF mo2000_has_key() */


void mo2100_do_lock(struct char_data * ch, char *arg, int cmd)
{

	int door, other_room;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	struct room_direction_data *back;
	struct obj_data *obj;
	struct char_data *victim;



	argument_interpreter(arg, type, dir);

	if (!*type) {
		send_to_char("Lock what?\n\r", ch);
		return;
	}

	if (ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM,
				ch, &victim, &obj)) {

		/* this is an object */

		if (obj->obj_flags.type_flag != ITEM_CONTAINER &&
		    obj->obj_flags.type_flag != ITEM_QSTCONT) {
			send_to_char("That's not a container.\n\r", ch);
			return;
		}

		if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSED)) {
			send_to_char("Maybe you should close it first...\n\r", ch);
			return;
		}

		if (obj->obj_flags.value[2] < 0) {
			send_to_char("That thing can't be locked.\n\r", ch);
			return;
		}

		if (IS_SET(obj->obj_flags.value[1], CONT_LOCKED)) {
			send_to_char("It is locked already.\n\r", ch);
			return;
		}

		if (!mo2000_has_key(ch, obj->obj_flags.value[2], BIT0)) {
			send_to_char("You don't seem to have the proper key.\n\r", ch);
			return;
		}

		SET_BIT(obj->obj_flags.value[1], CONT_LOCKED);
		send_to_char("*Clunk*\n\r", ch);
		act("$n locks $p - 'cluck', it says.", FALSE, ch, obj, 0, TO_ROOM);
	}			/* END OF its an object */

	if ((door = mo1600_find_door(ch, type, dir)) >= 0) {

		/* a door, perhaps */

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR)) {
			send_to_char("That's absurd.\n\r", ch);
			return;
		}

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
			send_to_char("You have to close it first, I'm afraid.\n\r", ch);
			return;
		}

		if (EXIT(ch, door)->key < 0) {
			send_to_char("There does not seem to be any keyholes.\n\r", ch);
			return;
		}

		if (IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED)) {
			send_to_char("It's already locked!\n\r", ch);
			return;
		}

		if (!mo2000_has_key(ch, EXIT(ch, door)->key, BIT0)) {
			send_to_char("You don't have the proper key.\n\r", ch);
			return;
		}

		SET_BIT(EXIT(ch, door)->exit_info, EX_LOCKED);
		if (EXIT(ch, door)->keyword) {
			act("$n locks the $F.", 0, ch, 0, EXIT(ch, door)->keyword,
			    TO_ROOM);
		}
		else {
			act("$n locks the door.", FALSE, ch, 0, 0, TO_ROOM);
		}
		send_to_char("*Click*\n\r", ch);

		/* now for locking the other side, too */
		if ((other_room = EXIT(ch, door)->to_room) != NOWHERE)
			if ((back = world[other_room].dir_option[reverse_dir[door]]))
				if (back->to_room == ch->in_room)
					SET_BIT(back->exit_info, EX_LOCKED);

	}			/* END OF its a door */

}				/* END OF mo2100_do_lock() */


void mo2200_do_unlock(struct char_data * ch, char *arg, int cmd)
{

	int door, other_room;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	struct room_direction_data *back;
	struct obj_data *obj;
	struct char_data *victim;



	argument_interpreter(arg, type, dir);

	if (!*type) {
		send_to_char("Unlock what?\n\r", ch);
		return;
	}

	if (ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM,
				ch, &victim, &obj)) {

		/* this is an object */
		if (obj->obj_flags.type_flag != ITEM_CONTAINER &&
		    obj->obj_flags.type_flag != ITEM_QSTCONT) {
			send_to_char("That's not a container.\n\r", ch);
			return;
		}

		if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSED)) {
			send_to_char("Silly - it ain't even closed!\n\r", ch);
			return;
		}

		if (obj->obj_flags.value[2] < 0) {
			send_to_char("Odd - you can't seem to find a keyhole.\n\r",
				     ch);
			return;
		}
		if (!IS_SET(obj->obj_flags.value[1], CONT_LOCKED)) {
			send_to_char("Oh.. it wasn't locked, after all.\n\r", ch);
			return;
		}

		if (!mo2000_has_key(ch, obj->obj_flags.value[2], BIT0)) {
			send_to_char("You don't seem to have the proper key.\n\r",
				     ch);
			return;
		}

		REMOVE_BIT(obj->obj_flags.value[1], CONT_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n unlocks $p.", FALSE, ch, obj, 0, TO_ROOM);
		return;
	}

	if ((door = mo1600_find_door(ch, type, dir)) >= 0) {

		/* it is a door */

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR)) {
			send_to_char("That's absurd.\n\r", ch);
			return;
		}

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
			send_to_char("Heck.. it ain't even closed!\n\r", ch);
			return;
		}

		if (EXIT(ch, door)->key < 0) {
			send_to_char("You can't seem to spot any keyholes.\n\r", ch);
			return;
		}

		if (!IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED)) {
			send_to_char("It's already unlocked, it seems.\n\r", ch);
			return;
		}

		if (!mo2000_has_key(ch, EXIT(ch, door)->key, BIT0)) {
			send_to_char("You do not have the proper key for that.\n\r",
				     ch);
			return;
		}

		REMOVE_BIT(EXIT(ch, door)->exit_info, EX_LOCKED);
		if (EXIT(ch, door)->keyword) {
			act("$n unlocks the $F.", 0, ch, 0, EXIT(ch, door)->keyword,
			    TO_ROOM);
		}
		else {
			act("$n unlocks the door.", FALSE, ch, 0, 0, TO_ROOM);
		}

		send_to_char("*click*\n\r", ch);
		/* now for unlocking the other side, too */
		if ((other_room = EXIT(ch, door)->to_room) != NOWHERE)
			if ((back = world[other_room].dir_option[reverse_dir[door]]))
				if (back->to_room == ch->in_room)
					REMOVE_BIT(back->exit_info, EX_LOCKED);

	}			/* END OF its a door */

	return;

}				/* END OF mo2200_do_unlock() */


void mo2300_do_pick(struct char_data * ch, char *arg, int cmd)
{

	signed char percent;
	int door, other_room;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	struct room_direction_data *back;
	struct obj_data *obj;
	struct char_data *victim;



	argument_interpreter(arg, type, dir);

	percent = number(1, 101);	/* 101% is a complete failure */

	if (percent > (ch->skills[SKILL_PICK_LOCK].learned)) {
		send_to_char("You failed to pick the lock.\n\r", ch);
		return;
	}

	li9900_gain_proficiency(ch, SKILL_PICK_LOCK);

	if (!*type)
		send_to_char("Pick what?\n\r", ch);
	else if (ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM,
				     ch, &victim, &obj))
		/* this is an object */

		if (obj->obj_flags.type_flag != ITEM_CONTAINER &&
		    obj->obj_flags.type_flag != ITEM_QSTCONT)
			send_to_char("That's not a container.\n\r", ch);
		else if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
			send_to_char("Silly - it ain't even closed!\n\r", ch);
		else if (obj->obj_flags.value[2] < 0)
			send_to_char("Odd - you can't seem to find a keyhole.\n\r", ch);
		else if (!IS_SET(obj->obj_flags.value[1], CONT_LOCKED))
			send_to_char("Oho! This thing is NOT locked!\n\r", ch);
		else if (IS_SET(obj->obj_flags.value[1], CONT_PICKPROOF))
			send_to_char("It resists your attempts at picking it.\n\r", ch);
		else {
			REMOVE_BIT(obj->obj_flags.value[1], CONT_LOCKED);
			send_to_char("*Click*\n\r", ch);
			act("$n fiddles with $p.", FALSE, ch, obj, 0, TO_ROOM);
		}
	else if ((door = mo1600_find_door(ch, type, dir)) >= 0) {
		if (!IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR))
			send_to_char("That's absurd.\n\r", ch);
		else if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
			send_to_char("You realize that the door is already open.\n\r", ch);
		else if (EXIT(ch, door)->key < 0)
			send_to_char("You can't seem to spot any lock to pick.\n\r", ch);
		else if (!IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED))
			send_to_char("Oh.. it wasn't locked at all.\n\r", ch);
		else if (IS_SET(EXIT(ch, door)->exit_info, EX_PICKPROOF))
			send_to_char("You seem to be unable to pick this lock.\n\r", ch);
		else {
			REMOVE_BIT(EXIT(ch, door)->exit_info, EX_LOCKED);
			if (EXIT(ch, door)->keyword)
				act("$n skillfully picks the lock of the $F.", 0, ch, 0,
				    EXIT(ch, door)->keyword, TO_ROOM);
			else
				act("$n picks the lock of the.", TRUE, ch, 0, 0, TO_ROOM);
			send_to_char("The lock quickly yields to your skills.\n\r", ch);
			/* now for unlocking the other side, too */
			if ((other_room = EXIT(ch, door)->to_room) != NOWHERE)
				if ((back = world[other_room].dir_option[reverse_dir[door]]))
					if (back->to_room == ch->in_room)
						REMOVE_BIT(back->exit_info, EX_LOCKED);
		}
	}

	return;

}				/* END OF mo2300_do_pick() */



void mo2400_do_enter(struct char_data * ch, char *arg, int cmd)
{

	int door;
	char buf[MAX_INPUT_LENGTH], tmp[MAX_STRING_LENGTH];



	one_argument(arg, buf);

	if (*buf) {		/* an argument was supplied, search for door
				 * keyword */
		for (door = 0; door <= 5; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->keyword)
					if (!str_cmp(EXIT(ch, door)->keyword, buf)) {
						mo1500_do_move(ch, "", ++door);
						return;
					}
		sprintf(tmp, "There is no %s here.\n\r", buf);
		send_to_char(tmp, ch);
	}
	else if (IS_SET(world[ch->in_room].room_flags, RM1_INDOORS))
		send_to_char("You are already indoors.\n\r", ch);
	else {
		/* try to locate an entrance */
		for (door = 0; door <= 5; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->to_room != NOWHERE)
					if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED) &&
					    IS_SET(world[EXIT(ch, door)->to_room].room_flags,
						   RM1_INDOORS)) {
						mo1500_do_move(ch, "", ++door);
						return;
					}
		send_to_char("You can't seem to find anything to enter.\n\r", ch);
	}

	return;

}				/* END OF mo2400_do_enter() */


int mo2500_enter_portal(struct char_data * ch, int cmd, char *arg)
{

	struct follow_type *lv_f, *next_dude;
	struct obj_data *lv_obj;
	int lv_room, lv_old_room, lv_we_can_go, rc;

	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_ENTER &&
	    cmd != CMD_LOOK) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);
	if (!*buf) {
		return (FALSE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, buf,
					 world[ch->in_room].contents);
	if (!lv_obj) {
		return (FALSE);
	}
	
	/* Bounding check --Frank 1/8 */
	if (lv_obj->item_number < 0) {
		return (FALSE);
	}

	/* IS IT A PORTAL? */
	if (obj_index[lv_obj->item_number].virtual != OBJ_PORTAL) {
		return (FALSE);
	}

	lv_room = lv_obj->obj_flags.value[VALUE_PORTAL_ROOM];
	if (lv_room < 1 ||
	    lv_room > top_of_world) {
		send_to_char("The portal leads to an invalid room.\r\n", ch);
		return (TRUE);
	}

	/* CAN WE ACCESS THAT ROOM? */
	rc = li3000_is_blocked_from_room(ch, lv_room, BIT0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return (TRUE);
	}

	if (cmd == CMD_LOOK) {
		send_to_char("Looking through the portal, you see:\r\n", ch);
		act("$n looks into the portal.",
		    FALSE, ch, lv_obj, 0, TO_ROOM);
		lv_old_room = ch->in_room;
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, lv_room);
		in3000_do_look(ch, "\0", 0);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, lv_old_room);
		return (TRUE);
	}

	if (IS_AFFECTED(ch, AFF_HOLD_PERSON)) {
		send_to_char("You are imitating a stone right now.\r\n", ch);
		return (TRUE);
	}

	if (GET_POS(ch) == POSITION_FIGHTING) {
		send_to_char("You're to busy fighting.\r\n", ch);
		return (TRUE);
	}

	if (IS_AFFECTED(ch, AFF_CHARM) &&
	    (ch->master) &&
	    (ch->in_room == ch->master->in_room)) {
		send_to_char("The thought of leaving your master gives you an anxiety attack.\r\n", ch);
		return (TRUE);
	}

	if (IS_SET(world[lv_room].room_flags, RM1_DEATH) &&
	    GET_LEVEL(ch) < IMO_SPIRIT) {
		/* DO WE NEED TO PROTECT AN AVATAR FROM A DT? */
		rc = ha9800_protect_char_from_dt(ch, lv_room, 0);
		if (rc) {
			return (TRUE);
		}
		act("You hear $n scream in agony as $e steps into the portal.",
		    FALSE, ch, lv_obj, 0, TO_ROOM);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, lv_room);
		ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_ROOM);
		return (TRUE);
	}

	send_to_char("You step through the portal.\r\n", ch);
	act("$n steps into the portal.",
	    FALSE, ch, lv_obj, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_room);
	act("$n steps out of the portal.",
	    FALSE, ch, lv_obj, 0, TO_ROOM);

	for (lv_f = ch->followers; lv_f; lv_f = next_dude) {
		next_dude = lv_f->next;

		lv_we_can_go = TRUE;
		if (GET_POS(lv_f->follower) < POSITION_STANDING) {
			act("You would follow $N, but you aren't standing.",
			    FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
			lv_we_can_go = FALSE;
		}

		if (lv_we_can_go == TRUE) {
			if (GET_POS(lv_f->follower) == POSITION_FIGHTING) {
				send_to_char("You're to busy fighting to step through the portal.\r\n", ch);
				lv_we_can_go = FALSE;
			}
		}

		if (lv_we_can_go == TRUE) {
			if (IS_AFFECTED(lv_f->follower, AFF_HOLD_PERSON)) {
				send_to_char("You are imitating a stone right now.\r\n", ch);
				lv_we_can_go = FALSE;
			}
		}

		/* CAN WE ACCESS THAT ROOM? */
		if (lv_we_can_go == TRUE) {
			rc = li3000_is_blocked_from_room(lv_f->follower, lv_room, BIT0);
			if (rc) {
				li3100_send_blocked_text(lv_f->follower, rc, 0);
				lv_we_can_go = FALSE;
			}
		}

		if (lv_we_can_go == TRUE) {

			//Odin - Bug Fix
				if ((lv_f->follower)->in_room == lv_old_room) {
				act("$n steps into the portal.",
				    FALSE, lv_f->follower, lv_obj, 0, TO_ROOM);
				act("You follow $N through the portal.\r\n",
				  FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
				ha1500_char_from_room(lv_f->follower);
				ha1600_char_to_room(lv_f->follower, lv_room);
				act("$n steps out of the portal.",
				    FALSE, lv_f->follower, lv_obj, 0, TO_ROOM);
			}
		}
	}			/* END OF for LOOP */

	return (TRUE);

}				/* END OF mo2500_enter_portal() */


void mo3000_do_leave(struct char_data * ch, char *arg, int cmd)
{

	int door;



	if (!IS_SET(world[ch->in_room].room_flags, RM1_INDOORS))
		send_to_char("You are outside.. where do you want to go?\n\r", ch);
	else {
		for (door = 0; door <= 5; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->to_room != NOWHERE)
					if (!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED) &&
					    !IS_SET(world[EXIT(ch, door)->to_room].room_flags, RM1_INDOORS)) {
						mo1500_do_move(ch, "", ++door);
						return;
					}
		send_to_char("I see no obvious exits to the outside.\n\r", ch);
	}

	return;

}				/* END OF mo3000_do_leave() */


void mo3100_do_stand(struct char_data * ch, char *arg, int cmd)
{



	switch (GET_POS(ch)) {
		case POSITION_STANDING:{
			act("You are already standing.", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	case POSITION_SITTING:{
			act("You stand up.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n clambers on $s feet.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_STANDING;
		} break;
	case POSITION_RESTING:{
			act("You stop resting, and stand up.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops resting, and clambers on $s feet.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_STANDING;
		} break;
	case POSITION_SLEEPING:{
			act("You have to wake up first!", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	case POSITION_FIGHTING:{
			act("Do you not consider fighting as standing?", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	default:{
			act("You stop floating around, and put your feet on the ground.",
			    FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops floating around, and puts $s feet on the ground.",
			    TRUE, ch, 0, 0, TO_ROOM);
		} break;
	}			/* END OF switch() */

	return;

}				/* END OF mo3100_do_stand() */


void mo3200_do_sit(struct char_data * ch, char *arg, int cmd)
{



	switch (GET_POS(ch)) {
		case POSITION_STANDING:{
			act("You sit down.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n sits down.", FALSE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SITTING;
		} break;

	case POSITION_SITTING:{
			send_to_char("You are sitting already.\n\r", ch);
		} break;

	case POSITION_RESTING:{
			act("You stop resting, and sit up.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops resting.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SITTING;
		} break;

	case POSITION_SLEEPING:{
			act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
		} break;

	case POSITION_FIGHTING:{
			act("Sit down while fighting? are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
		} break;

	default:{
			act("You stop floating around, and sit down.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops floating around, and sits down.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SITTING;
		} break;
	}			/* END OF switch() */

	return;
}				/* END OF mo3200_do_sit() */


void mo3500_do_rest(struct char_data * ch, char *arg, int cmd)
{



	switch (GET_POS(ch)) {
		case POSITION_STANDING:{
			act("You sit down and rest your tired bones.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n sits down and rests.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_RESTING;
		} break;
	case POSITION_SITTING:{
			act("You rest your tired bones.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n rests.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_RESTING;
		} break;
	case POSITION_RESTING:{
			act("You are already resting.", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	case POSITION_SLEEPING:{
			act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	case POSITION_FIGHTING:{
			act("Rest while fighting? are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
		} break;
	default:{
			act("You stop floating around, and stop to rest your tired bones.",
			    FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops floating around, and rests.", FALSE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SITTING;
		} break;
	}			/* END OF switch() */

	return;

}				/* END OF mo3500_do_rest() */


void mo3600_do_sleep(struct char_data * ch, char *arg, int cmd)
{



	switch (GET_POS(ch)) {
		case POSITION_STANDING:
		case POSITION_SITTING:
		case POSITION_RESTING:{
			send_to_char("You go to sleep.\n\r", ch);
			act("$n lies down and falls asleep.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SLEEPING;
		} break;
	case POSITION_SLEEPING:{
			send_to_char("You are already sound asleep.\n\r", ch);
		} break;
	case POSITION_FIGHTING:{
			send_to_char("Sleep while fighting? are you MAD?\n\r", ch);
		} break;
	default:{
			act("You stop floating around, and lie down to sleep.",
			    FALSE, ch, 0, 0, TO_CHAR);
			act("$n stops floating around, and lie down to sleep.",
			    TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POSITION_SLEEPING;
		} break;
	}			/* END OF switch() */

	return;

}				/* END OF mo3600_do_sleep() */


void mo3700_do_wake(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *tmp_char;
	char arg1[MAX_INPUT_LENGTH];



	one_argument(arg, arg1);

	if (*arg1) {
		/* ARE WE ASLEEP OURSELVES? */
		if (GET_POS(ch) == POSITION_SLEEPING) {
			act("You can't wake people up if you are asleep yourself!",
			    FALSE, ch, 0, 0, TO_CHAR);
			return;
		}

		tmp_char = ha2125_get_char_in_room_vis(ch, arg1);

		/* CAN WE SEE THE OTHER PERSON */
		if (!(tmp_char)) {
			send_to_char("You do not see that person here.\n\r", ch);
			return;
		}

		/* ARE WE TRYING TO WAKE OURSELVES UP? */
		if (tmp_char == ch) {
			act("If you want to wake yourself up, just type 'wake'",
			    FALSE, ch, 0, 0, TO_CHAR);
			return;
		}

		/* IS THE OTHER PERSON ASLEEP? */
		if (GET_POS(tmp_char) != POSITION_SLEEPING) {
			act("$N is already awake.", FALSE, ch, 0, tmp_char, TO_CHAR);
			return;
		}

		/* IS THERE A SLEEP SPELL IN AFFECT? */
		if (IS_AFFECTED(tmp_char, AFF_SLEEP)) {
			if (GET_LEVEL(ch) > PK_LEV) {
				act("You call upon your Immortal abilities and wake $M up!", FALSE, ch, 0, tmp_char, TO_CHAR);
				REMOVE_BIT(tmp_char->specials.affected_by, AFF_SLEEP);
				/* fall through and wake person up */
			}
			else {
				act("You can not wake $M up!", FALSE, ch, 0, tmp_char, TO_CHAR);
				return;
			}
		}

		/* LETS DO IT */
		act("You wake $M up.", FALSE, ch, 0, tmp_char, TO_CHAR);
		GET_POS(tmp_char) = POSITION_SITTING;
		act("You are awakened by $n.", FALSE, ch, 0, tmp_char, TO_VICT);

		return;

	}			/* END OF IF ARG PROVIDED */

	if (IS_AFFECTED(ch, AFF_SLEEP)) {
		if (GET_LEVEL(ch) > PK_LEV) {
			send_to_char("You call upon your Immortal abilities.\r\n",
				     ch);
			REMOVE_BIT(ch->specials.affected_by, AFF_SLEEP);
			/* FALL THROUGH */
		}
		else {
			send_to_char("A magical force prevents you from doing that!\n\r", ch);
			return;
		}
	}

	if (GET_POS(ch) > POSITION_SLEEPING) {
		send_to_char("You are already awake...\n\r", ch);
		return;
	}

	/* DO IT */
	send_to_char("You wake and stand up.\n\r", ch);
	act("$n clambers on $s feet.", TRUE, ch, 0, 0, TO_ROOM);
	GET_POS(ch) = POSITION_STANDING;

	return;

}				/* END OF mo3700_do_wake() */


void mo4000_do_follow(struct char_data * ch, char *arg, int cmd)
{

	char name[MAX_INPUT_LENGTH + 20];
	struct char_data *leader;



	one_argument(arg, name);

	/* DID THEY SAY WHO TO FOLLOW? */
	if (!(*name)) {
		send_to_char("Who do you wish to follow?\n\r", ch);
		return;
	}

	/* CAN WE SEE THE PERSON TO FOLLOW? */
	leader = ha2125_get_char_in_room_vis(ch, name);
	if (!(leader)) {
		send_to_char("I see no person by that name here!\n\r", ch);
		return;
	}

	/* ARE WE AFFECTED BY CHARM? */
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master)) {
		act("But you only feel like following $N!",
		    FALSE, ch, 0, ch->master, TO_CHAR);
		return;
	}

	/* ARE WE TRYING TO FOLLOW A MOB THAT IS FLAGGED NO FOLLOW? */
	if ((IS_SET(GET_ACT1(leader), PLR1_NOFOLLOW))) {
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) {
			act("For some reason, you can't keep track of $N!",
			    FALSE, ch, 0, leader, TO_CHAR);
			return;
		}
	}

	/* DID WE SAY FOLLOW OURSELF? */
	if (leader == ch) {
		/* IF ALREADY FOLLOWING OURSELF */
		if (!ch->master) {
			send_to_char("You are already following yourself.\n\r",
				     ch);
		}
		else {
			/* QUIT FOLLOWING SOMEONE ELSE */
			ha4100_stop_follower(ch, END_FOLLOW_BY_SELF);
		}
		return;
	}			/* END OF FOLLOW OURSELF */

	/* ARE WE TRYING TO FOLLOW SOMEONE FOLLOWING US? */
	if (ha4000_circle_follow(ch, leader)) {
		act("Sorry, but following in 'loops' is not allowed", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (ha9950_maxed_on_followers(leader, ch, 0)) {
		send_to_char("That person is already followed by the maximum number of followers.\r\n", ch);
		return;
	}

	/* WE ARE FOLLOWING SOMEONE ELSE, BREAK THAT CONNECTION */
	if (ch->master)
		ha4100_stop_follower(ch, END_FOLLOW_BY_ANOTHER);

	ha4300_add_follower(ch, leader);

	/* REMOVE GROUP FLAG IF ITS ON */
	if ((IS_AFFECTED(ch, AFF_GROUP))) {
		REMOVE_BIT(ch->specials.affected_by, AFF_GROUP);
	}

}				/* END OF mo4000_do_follow */


void mo4100_do_speedwalk(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_INPUT_LENGTH];



	/* move two spaces at a time */
	if (ch->specials.fighting)
		return;		/* stop if fight encountered */
	for (; (*arg)
	     && (*arg != 'n')
	     && (*arg != 'e')
	     && (*arg != 's')
	     && (*arg != 'w')
	     && (*arg != 'd')
	     && (*arg != 'u'); arg++);

	switch (*arg) {
	case '\0':{
			return;
		}
	case 'n':{
			mo1500_do_move(ch, "\0", CMD_NORTH);
			break;
		}
	case 'e':{
			mo1500_do_move(ch, "\0", CMD_EAST);
			break;
		}
	case 's':{
			mo1500_do_move(ch, "\0", CMD_SOUTH);
			break;
		}
	case 'w':{
			mo1500_do_move(ch, "\0", CMD_WEST);
			break;
		}
	case 'd':{
			mo1500_do_move(ch, "\0", CMD_DOWN);
			break;
		}
	case 'u':{
			mo1500_do_move(ch, "\0", CMD_UP);
			break;
		}
	}			/* END OF switch() */

	arg++;
	if (!*arg ||
	    gv_move_succeeded == FALSE) {
		return;		/* all done */
	}

	if (ch->specials.fighting) {
		return;		/* stop if fight encountered */
	}

	for (; (*arg)
	     && (*arg != 'n')
	     && (*arg != 'e')
	     && (*arg != 's')
	     && (*arg != 'w')
	     && (*arg != 'd')
	     && (*arg != 'u'); arg++);

	switch (*arg) {
	case '\0':
		return;
	case 'n':
		mo1500_do_move(ch, "\0", CMD_NORTH);
		break;
	case 'e':
		mo1500_do_move(ch, "\0", CMD_EAST);
		break;
	case 's':
		mo1500_do_move(ch, "\0", CMD_SOUTH);
		break;
	case 'w':
		mo1500_do_move(ch, "\0", CMD_WEST);
		break;
	case 'd':
		mo1500_do_move(ch, "\0", CMD_DOWN);
		break;
	case 'u':
		mo1500_do_move(ch, "\0", CMD_UP);
		break;
	}
	arg++;
	if (!*arg || gv_move_succeeded == FALSE)
		return;		/* all done */

	/* still some left to do */
	sprintf(buf, "speedwalk %s", arg);
	if (ch->desc) {
		WAIT_STATE(ch, 1);	/* wait an iteration for next move */
		co1500_write_to_head_q(buf, &ch->desc->input);
	}

	return;

}				/* END OF mo4100_do_speedwalk() */


void survivor_poison(struct char_data * ch)
{

	struct affected_type af;

	if (IS_AFFECTED(ch, AFF_MAGIC_IMMUNE)) {
		return;
	}


	af.type = SPELL_POISON;
	af.duration = number(2, 12);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_POISON;
	ha1300_affect_to_char(ch, &af);
	act("You feel kind of sick.", FALSE, ch, 0, 0, TO_CHAR);
	act("$n looks kind of sick.", TRUE, ch, 0, 0, TO_ROOM);

	return;

}				/* END OF survivor_poison() */


void survivor_blindness(struct char_data * ch)
{

	struct affected_type af;

	if (IS_AFFECTED(ch, AFF_MAGIC_IMMUNE)) {
		return;
	}


	af.type = SPELL_BLINDNESS;
	af.duration = number(1, 4);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_BLIND;
	ha1300_affect_to_char(ch, &af);

	act("You lose your sight!", FALSE, ch, 0, 0, TO_CHAR);
	act("$n has lost $s sight!", TRUE, ch, 0, 0, TO_ROOM);

	return;

}				/* END OF survivor_blindness() */
