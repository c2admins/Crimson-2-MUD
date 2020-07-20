/* vh */
/* gv_location: 20001-20500 */
/* ****************************************************************** *
*  file: vehicle.c , Special module.                  Part of DIKUMUD *
*  Usage: Procedures handling special procs for object/room/mobile    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "limits.h"
#include "db.h"
#include "constants.h"
#include "spells.h"
#include "func.h"
#include "globals.h"

/* ********************************************************************
*  Special procedures for vehicles                                    *
******************************************************************** */

int vh500_get_vehicle_room(struct char_data * ch, char *arg, int cmd, struct obj_data * lv_obj)
{

	struct vehicle_data *lv_veh, *lv_next_veh;
	char buf[MAX_STRING_LENGTH];



	lv_next_veh = 0;
	for (lv_veh = vehicle_list; lv_veh; lv_veh = lv_next_veh) {
		lv_next_veh = lv_veh->next;
		if (lv_veh->obj == lv_obj) {
			if (lv_veh->room < 1) {
				if (ch) {
					lv_veh->room = do_rcopy_for_vehicle(ch, arg, cmd);
					/* DOES THIS VEHICLE HAVE A ROOM DESC? */
					vh550_set_vehicle_description(lv_veh->obj, lv_veh->room);
				}
				else {
					sprintf(buf, "ERROR: Got negative room: %d for vehicle %s.\r\n",
						lv_veh->room, lv_obj->name);
					main_log(buf);
					spec_log(buf, ERROR_LOG);
					lv_veh->room = 0;
				}
			}
			return (lv_veh->room);
		}
	}

	if (ch) {
		send_to_char("I didn't find this vehicle in the link list. sorry\r\n", ch);
	}
	return (0);		/* OBJECT ISN'T IN LIST - SOMETHINGS WRONG... */

}				/* END OF  vh500_get_vehicle_room() */


void vh550_set_vehicle_description(struct obj_data * lv_obj, int lv_veh_room)
{

	char *ptr;



	if (lv_veh_room < 0)
		return;

	if (lv_obj->short_description) {
		if (world[lv_veh_room].name) {
			free(world[lv_veh_room].name);
		}
		world[lv_veh_room].name = strdup(lv_obj->short_description);
	}

	ptr = in2400_find_ex_description("INTERIOR", lv_obj->ex_description);
	if (ptr) {
		if (strlen(ptr) > 0) {
			if (world[lv_veh_room].description) {
				free(world[lv_veh_room].description);
			}
			world[lv_veh_room].description = strdup(ptr);
		}
	}

	return;

}				/* END OF vh550_set_vehicle_description() */

struct obj_data *vh700_get_vehicle_object(struct char_data * ch, char *arg, int cmd, int lv_veh_room)
{

	struct vehicle_data *lv_veh;



	for (lv_veh = vehicle_list; lv_veh; lv_veh = lv_veh->next) {
		if (lv_veh->room == lv_veh_room) {
			return (lv_veh->obj);
		}
	}
	send_to_char("I didn't find this vehicle in the link list. sorry\r\n", ch);
	return (0);		/* OBJECT ISN'T IN LIST - SOMETHINGS WRONG... */

}				/* END OF  vh700_get_vehicle_object() */


void vh1000_do_board(struct char_data * ch, char *arg, int cmd)
{

	struct follow_type *lv_f, *next_dude;
	struct obj_data *lv_obj;
	int lv_room, ch_room;



	for (; *arg == ' '; arg++);
	if (!*arg) {
		send_to_char("What do you want to board?\r\n", ch);
		return;
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg, world[ch->in_room].contents);
	if (!lv_obj) {
		send_to_char("Sorry, but I can't seem to find that.\r\n", ch);
		return;
	}

	/* IS IT A VEHICLE? */
	if (!(GET_ITEM_TYPE(lv_obj) == ITEM_VEHICLE)) {
		send_to_char("Sorry, but I don't think you can ride in that.\r\n", ch);
		return;
	}

	/* SAFETY CHECKS */

	if (lv_obj->carried_by ||
	    lv_obj->in_obj ||
	    CAN_WEAR(lv_obj, ITEM_TAKE)) {
		send_to_char("Sorry, but I don't think you can ride in that!\r\n", ch);
		return;
	}

	/* IF ITS NOT PARKED AT A STATION, FORGET IT */
	if (GET_LEVEL(ch) < IMO_SPIRIT &&
	    !lv_obj->obj_flags.timer) {
		send_to_char("You can only board when its parked at a station.\r\n", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM) &&
	    (ch->master) &&
	    (ch->in_room == ch->master->in_room)) {
		send_to_char("The thought of leaving your master makes you weep.\r\n", ch);
		return;
	}

	ch_room = ch->in_room;
	lv_room = vh500_get_vehicle_room(ch, arg, cmd, lv_obj);
	if (lv_room < 1) {
		return;
	}

	if (!vh1200_put_char_on_vehicle(ch, lv_obj, lv_room)) {
		return;
	}

	for (lv_f = ch->followers; lv_f; lv_f = next_dude) {
		next_dude = lv_f->next;
		if ((GET_POS(lv_f->follower) >= POSITION_STANDING) &&
		    (lv_f->follower->in_room == ch_room)) {
			if (vh1200_put_char_on_vehicle(lv_f->follower, lv_obj, lv_room)) {
				act("You follow $N aboard $p.",
				  FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
			}
			else {
				act("You don't follow $N.",
				  FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
			}
		}
		else {
			if (lv_f->follower->in_room == ch_room)
				act("You would follow $N, but you aren't standing.", FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
		}
	}
	return;

}				/* END OF vh1000_do_board() */


int vh1200_put_char_on_vehicle(struct char_data * ch, struct obj_data * lv_obj, int lv_veh_room)
{
	char buf[MAX_STRING_LENGTH];



	if (ch->riding) {
		send_to_char("Hrmm, your already riding something...\r\n", ch);
		return (FALSE);
	}

	if (ch->in_room == VEHICLE_BEG_ROOM) {
		send_to_char("Your already in the riding room.\r\n", ch);
		return (FALSE);
	}

	if (GET_LEVEL(ch) > 5) {
		if (GET_LEVEL(ch) < IMO_SPIRIT &&
		    GET_GOLD(ch) < lv_obj->obj_flags.cost) {
			sprintf(buf, "Sorry, but it cost %d gold to ride.",
				lv_obj->obj_flags.cost);
			act(buf, TRUE, ch, lv_obj, 0, TO_CHAR);
			act("$n is informed that $e has to pay or walk!",
			    TRUE, ch, lv_obj, 0, TO_ROOM);
			return (FALSE);
		}
		if ((GET_LEVEL(ch) < IMO_SPIRIT &&
		     lv_obj->obj_flags.cost) &&
		    (IS_PC(ch))) {
			GET_GOLD(ch) -= lv_obj->obj_flags.cost;
			if (lv_obj->obj_flags.cost == 1) {
				sprintf(buf, "You drop a gold coin into the payment slot.\r\n");
			}
			else {
				sprintf(buf, "You drop %d gold coins into a payment slot.\r\n",
					lv_obj->obj_flags.cost);
			}
			send_to_char(buf, ch);
		}
	}			/* END OF level > 5 */

	act("$n boards $p.", TRUE, ch, lv_obj, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_veh_room);
	ch->riding = lv_obj->item_number;
	act("You board $p.", TRUE, ch, lv_obj, 0, TO_CHAR);
	act("$n has entered the vehicle.", TRUE, ch, lv_obj, 0, TO_ROOM);
	return (TRUE);

}				/* END OF vh1200_put_char_on_vehicle() */


void vh1300_exit_from_vehicle(struct char_data * ch, char *arg, int cmd)
{
	struct obj_data *lv_obj;
	struct follow_type *lv_f, *next_dude;



	if (!ch->riding) {
		send_to_char("Your in a vehicle, but I can't find it!\r\n", ch);
		return;
	}

	/* FIND VEHICLE PERSON IS RIDING */
	lv_obj = vh700_get_vehicle_object(ch, arg, cmd, ch->in_room);

	if (!lv_obj) {
		send_to_char("ACK!  I can't find your vehicle.\r\n", ch);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, db8000_real_room(3000));
		return;
	}

	/* IF ITS NOT PARKED AT A STATION, FORGET IT */
	if (!lv_obj->obj_flags.timer) {
		send_to_char("You can only exit when parked at a station.\r\n", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM) &&
	    (ch->master) &&
	    (ch->in_room == ch->master->in_room)) {
		send_to_char("The thought of leaving your master makes you weep.\r\n", ch);
		return;
	}

	if (!vh1400_remove_char_from_vehicle(ch, lv_obj)) {
		return;
	}

	for (lv_f = ch->followers; lv_f; lv_f = next_dude) {
		next_dude = lv_f->next;
		if (GET_POS(lv_f->follower) >= POSITION_STANDING) {
			if (!vh1400_remove_char_from_vehicle(lv_f->follower, lv_obj)) {
				act("You follow $N exiting $p.",
				  FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
			}
			else {
				act("You don't follow $N.",
				  FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
			}
		}
		else {
			act("You would follow $N, but you aren't standing.",
			    FALSE, lv_f->follower, lv_obj, ch, TO_CHAR);
		}
	}
	return;

}				/* END OF vh1300_exit_from_vehicle() */


int vh1400_remove_char_from_vehicle(struct char_data * ch, struct obj_data * lv_obj)
{
	char buf[MAX_STRING_LENGTH];



	if (!lv_obj) {
		send_to_char("Entered vh1400 with lv_obj equal to zero\r\n", ch);
		return (FALSE);
	}

	if (!ch->riding) {
		send_to_char("Hrmm, you aren't riding anything...\r\n", ch);
		return (FALSE);
	}

	if (lv_obj->in_room < 0) {
		send_to_char("Hrmm the destination room is negative.\r\n", ch);
		return (FALSE);
	}

	act("$n exits $p.", TRUE, ch, lv_obj, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_obj->in_room);
	bzero(buf, sizeof(buf));
	act("$n exits $p.", TRUE, ch, lv_obj, 0, TO_ROOM);
	act("You exit $p.", FALSE, ch, lv_obj, 0, TO_CHAR);
	return (TRUE);

}				/* END OF vh1400_remove_char_from_vehicle() */


/* TAKE ALL PLAYERS FROM ROOM lv_veh_room and put in objs room */
void vh1450_remove_all_chars_from_vehicle(struct obj_data * lv_obj, int lv_veh_room)
{

	struct char_data *tmp_ch, *next_ch;
	int lv_dest_room;



	if (lv_veh_room < 1)
		return;

	if (lv_obj->in_room < 1)
		lv_dest_room = db8000_real_room(3000);
	else
		lv_dest_room = lv_obj->in_room;

	for (tmp_ch = world[lv_veh_room].people; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;
		act("Your vehicle runs into problems and you exit.",
		    TRUE, tmp_ch, 0, 0, TO_CHAR);
		act("$n exits $p.", TRUE, tmp_ch, lv_obj, 0, TO_ROOM);
		ha1500_char_from_room(tmp_ch);
		ha1600_char_to_room(tmp_ch, lv_dest_room);
		act("$n has exited $p.", TRUE, tmp_ch, lv_obj, 0, TO_ROOM);
	}

	return;
}				/* END OF
				 * vh1450_remove_all_chars_from_vehicle() */


void vh2000_move_vehicles()
{

	struct vehicle_data *lv_veh;
	struct obj_data *lv_obj;



	for (lv_veh = vehicle_list; lv_veh; lv_veh = lv_veh->next) {
		lv_obj = lv_veh->obj;
		/* MAKE SURE THEY DIDN'T LOAD THIS GUY IN SOMETHING */
		if (!lv_obj->in_obj) {
			vh2100_move_a_single_vehicle(lv_obj, lv_veh->room);
		}
	}

	return;

}				/* END OF vh2000_move_vehicles() */


void vh2100_move_a_single_vehicle(struct obj_data * lv_obj, int lv_veh_room)
{

	int lv_dir, lv_was_in, mylen, rc;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], *ptr;
	struct char_data *ch_obj_room, *ch_veh_room;



	rc = 0;
	if (lv_obj->in_room < 0) {
		return;
	}

	/* WE NEED A ch FOR ROUTINES */
	ch_obj_room = world[lv_obj->in_room].people;
	if (lv_veh_room > 0)
		ch_veh_room = world[lv_veh_room].people;
	else
		ch_veh_room = 0;

	/* IF WE HIT A ROUTE SLOW, PAUSE TWICE AS LONG */
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_SLOW)) {
		if (pulse % (PULSE_VEHICLE * 3)) {
			return;
		}
	}

	/* IF WE HIT A STATION, WE ARE PAUSED FOR AWHILE */
	if (lv_obj->obj_flags.timer > 0) {
		lv_obj->obj_flags.timer--;
		return;
	}

	/* LOOK THROUGH THE ROOM FOR A ROUTE TO TAKE */
	lv_dir = vh2200_get_vehicle_direction(lv_obj, lv_veh_room);

	/* DID WE FIND A ROUTE? */
	if (lv_dir == -1) {
		sprintf(buf, "ERROR: Unable to move vehicle in room: %d.  Add route.\r\n",
			world[lv_obj->in_room].number);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		vh1450_remove_all_chars_from_vehicle(lv_obj, lv_veh_room);
		db5175_remove_obj_from_vehicle_list(lv_obj);
		return;
	}

	/* DOES THE ROUTE GO SOMEWHERE? */
	if (!world[lv_obj->in_room].dir_option[lv_dir]) {
		sprintf(buf, "ERROR: Unable to move vehicle in room: %d  dir: %d. Fix route.\r\n",
			world[lv_obj->in_room].number, lv_dir);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		vh1450_remove_all_chars_from_vehicle(lv_obj, lv_veh_room);
		db5175_remove_obj_from_vehicle_list(lv_obj);
		return;
	}

	/* IS IT OPEN? */
	if (IS_SET(world[lv_obj->in_room].dir_option[lv_dir]->exit_info, EX_CLOSED)) {
		bzero(buf2, sizeof(buf2));
		if (world[lv_obj->in_room].dir_option[lv_dir]->keyword) {
			strcpy(buf2,
			       ha1100_fname(world[lv_obj->in_room].dir_option[lv_dir]->keyword));
		}
		else {
			strcpy(buf2, "SOMETHING");
		}
		bzero(buf, sizeof(buf));
		sprintf(buf, "The %s bumps against %s.\r\n",
			lv_obj->name, buf2);
		lv_obj->obj_flags.timer = 5;
		act(buf, FALSE, ch_obj_room, lv_obj, 0, TO_ROOM);
		act(buf, FALSE, ch_obj_room, lv_obj, 0, TO_CHAR);
		if (ch_veh_room) {
			act(buf, FALSE, ch_veh_room, lv_obj, 0, TO_ROOM);
			act(buf, FALSE, ch_veh_room, lv_obj, 0, TO_CHAR);
		}
		return;
	}

	/* SAFTEY CHECK */
	if (world[lv_obj->in_room].dir_option[lv_dir]->to_room == NOWHERE) {
		sprintf(buf, "ERROR: Unable to move vehicle to negative room: %d  dir: %d. Fix route.\r\n",
			world[lv_obj->in_room].number, lv_dir);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		vh1450_remove_all_chars_from_vehicle(lv_obj, lv_veh_room);
		db5175_remove_obj_from_vehicle_list(lv_obj);
		return;
	}

	/* DOES THE VEHICLE HAVE A CHANCE TO RUN OVER PLAYERS? */
	if (lv_obj->obj_flags.value[0] > 0) {
		rc = vh2500_damage_chars_in_room(lv_obj);
	}

	/* MOVE THE vehicle */
	sprintf(buf, "$p heads %s.", dirs[lv_dir]);
	act(buf, TRUE, ch_obj_room, lv_obj, 0, TO_ROOM);
	act(buf, TRUE, ch_obj_room, lv_obj, 0, TO_CHAR);
	lv_was_in = lv_obj->in_room;
	ha2200_obj_from_room(lv_obj);
	ha2100_obj_to_room(lv_obj,
			   world[lv_was_in].dir_option[lv_dir]->to_room);

	/* UPDATE ch FOR OBJ ROOM */
	ch_obj_room = world[lv_obj->in_room].people;

	/* DID WE HIT A REVERSAL FLAG? */
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_REVERSE)) {
		if (IS_SET(GET_OBJ2(lv_obj), OBJ2_VEHICLE_REV))
			REMOVE_BIT(GET_OBJ2(lv_obj), OBJ2_VEHICLE_REV);
		else
			SET_BIT(GET_OBJ2(lv_obj), OBJ2_VEHICLE_REV);
	}

	/* NOTIFY EVERYONE */
	sprintf(buf, "$p arrives from the %s.", dirs[reverse_dir[lv_dir]]);
	act(buf, FALSE, ch_obj_room, lv_obj, 0, TO_CHAR);
	act(buf, FALSE, ch_obj_room, lv_obj, 0, TO_ROOM);
	if (ch_veh_room) {
		sprintf(buf, "%s", world[lv_obj->in_room].name);
		act(buf, FALSE, ch_veh_room, lv_obj, 0, TO_CHAR);
		act(buf, FALSE, ch_veh_room, lv_obj, 0, TO_ROOM);
	}

	/* DID WE ARRIVE AT A STATION? IF SO, PAUSE AWHILE */
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_STATION)) {
		if (ch_veh_room) {
			sprintf(buf, "Entering station. Use EXIT to leave vehicle.");
			act(buf, TRUE, ch_veh_room, lv_obj, 0, TO_CHAR);
			act(buf, TRUE, ch_veh_room, lv_obj, 0, TO_ROOM);
		}
		/* DOES THIS OBJECT HAVE AN ARRIVAL MESSAGE? */
		ptr = in2400_find_ex_description("ARRIVAL", lv_obj->ex_description);
		if (ptr) {
			bzero(buf, sizeof(buf));
			strncpy(buf, ptr, sizeof(buf) - 5);
			/* REMOVE TRAILING NL/CR */
			mylen = strlen(buf) - 1;
			if (buf[mylen] == '\r' || buf[mylen] == '\n') {
				buf[mylen] = 0;
			}
			mylen = strlen(buf) - 1;
			if (mylen > 0 &&
			    (buf[mylen] == '\r' || buf[mylen] == '\n')) {
				buf[mylen] = 0;
			}
		}
		else {
			sprintf(buf, "A recording blares out that passengers are now loading for a %d fare.",
				lv_obj->obj_flags.cost);
		}
		act(buf, TRUE, ch_obj_room, lv_obj, 0, TO_CHAR);
		act(buf, TRUE, ch_obj_room, lv_obj, 0, TO_ROOM);
		lv_obj->obj_flags.timer = 10;
	}
	else {
		lv_obj->obj_flags.timer = 0;
	}

	return;

}				/* END OF  vh2100_move_a_single_vehicle() */


int vh2200_get_vehicle_direction(struct obj_data * lv_obj, int lv_veh_room)
{
	int lv_dir;



	lv_dir = -1;

	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_NORTH)) {
		lv_dir = 0;
	}
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_EAST)) {
		lv_dir = 1;
	}
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_SOUTH)) {
		lv_dir = 2;
	}
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_WEST)) {
		lv_dir = 3;
	}
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_UP)) {
		lv_dir = 4;
	}
	if (IS_SET(GET_ROOM2(lv_obj->in_room), RM2_ROUTE_DOWN)) {
		lv_dir = 5;
	}

	if (lv_dir > -1 &&
	    IS_SET(GET_OBJ2(lv_obj), OBJ2_VEHICLE_REV)) {
		lv_dir = reverse_dir[lv_dir];
	}

	return (lv_dir);

}				/* END OF vh2200_get_vehicle_direction() */


/* following paragraph examines chars in the room and damages them */
/* as the vehicle leaves the room.  THEY CAN RUN!!!                */
int vh2500_damage_chars_in_room(struct obj_data * lv_obj)
{

	int lv_dice_roll, lv_hit_loss, rc;
	char buf1[MAX_STRING_LENGTH];
	struct char_data *tmp_ch, *next_ch;



	rc = 0;

	if (lv_obj->in_room < 1) {
		return (rc);
	}

	for (tmp_ch = world[lv_obj->in_room].people; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;

		if (IS_NPC(tmp_ch)) {
			continue;
		}
		if (IS_PC(tmp_ch)) {
			if (IS_SET(world[tmp_ch->in_room].room_flags,
				   RM1_NOPKILL)) {
				continue;
			}
		}

		/* PROTECT NEWBIES AND IGNORE IMMORTALS */
		if (GET_LEVEL(tmp_ch) < 5 ||
		    GET_LEVEL(tmp_ch) > IMO_SPIRIT) {
			continue;
		}

		/* DID CHAR GET HIT? */
		lv_dice_roll = number(0, 99);
		if (GET_POS(tmp_ch) > POSITION_SITTING) {
			lv_dice_roll += GET_DEX(tmp_ch);
		}

		if (lv_obj->obj_flags.value[0] > lv_dice_roll) {
			act("You've been hit by $p!",
			    TRUE, tmp_ch, lv_obj, 0, TO_CHAR);
			act("$n has been hit by $p!",
			    TRUE, tmp_ch, lv_obj, 0, TO_ROOM);
			lv_hit_loss = lv_obj->obj_flags.value[1] * GET_HIT(tmp_ch);
			lv_hit_loss = lv_hit_loss / 100;
			lv_hit_loss = MAXV(lv_obj->obj_flags.value[2], lv_hit_loss);
			lv_hit_loss = MINV(lv_obj->obj_flags.value[3], lv_hit_loss);
			GET_HIT(tmp_ch) -= lv_hit_loss;
			ft1200_update_pos(tmp_ch);
			if (GET_HIT(tmp_ch) < -9) {
				bzero(buf1, sizeof(buf1));
				sprintf(buf1, "%s L[%d] has been killed by a %s.",
					GET_REAL_NAME(tmp_ch), GET_LEVEL(tmp_ch), GET_OBJ_NAME(lv_obj));
				spec_log(buf1, DEATH_LOG);
				strcat(buf1, "\r\n");
				do_info(buf1, 1, IMO_IMP, tmp_ch);
				GET_HOW_LEFT(tmp_ch) = LEFT_BY_DEATH_VEHICLE;
				if (GET_LEVEL(tmp_ch) > 20) {
					ft2000_die(tmp_ch, tmp_ch);
				}	/* END OF level > 20 */
			}
		}		/* END OF value[0] > lv_dice_roll */
	}			/* END OF for LOOP */
	return (rc);

}				/* END OF vh2500_damage_chars_in_room() */


int vh3000_vehicle_proc(struct char_data * ch, int cmd, char *arg)
{

	int lv_was_in_room;
	struct obj_data *lv_obj;



	for (; *arg == ' '; arg++);
	if (!(*arg) && cmd == CMD_LOOK) {
		in3000_do_look(ch, arg, cmd);
		lv_obj = vh700_get_vehicle_object(ch, arg, cmd, ch->in_room);
		if (lv_obj > 0 && lv_obj->in_room > 0) {
			lv_was_in_room = ch->in_room;
			ha1500_char_from_room(ch);
			ha1600_char_to_room(ch, lv_obj->in_room);
			send_to_char("Looking outside you see:\r\n", ch);
			in3000_do_look(ch, arg, cmd);
			ha1500_char_from_room(ch);
			ha1600_char_to_room(ch, lv_was_in_room);
			ch->riding = lv_obj->item_number;
		}
		return (TRUE);
	}

	return (FALSE);

}				/* END OF vh3000_vehicle_proc() */
