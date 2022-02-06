/* hu */
/* gv_location: 6001-6500 */
/* *******************************************************************
*  File: hunt.c                                  Part of CircleMUD   *
*  Usage: various graph algorithms                                   *
*  All rights reserved.  See license.doc for complete information.   *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins Univ. *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.          *
******************************************************************** */

#define TRACK_THROUGH_DOORS

/* You can define or not define TRACK_THOUGH_DOORS, above, depending on
whether or not you want track to find paths which lead through closed
or hidden doors.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "func.h"
#include "constants.h"
#include "globals.h"

/* breadth-first searching */
#define BFS_ERROR		-1
#define BFS_ALREADY_THERE	-2
#define BFS_NO_PATH		-3

/* Externals */
extern int top_of_world;
extern const char *dirs[];
extern struct room_data *world;

struct bfs_queue_struct {
	sh_int room;
	char dir;
	struct bfs_queue_struct *next;
};

static struct bfs_queue_struct *queue_head = 0, *queue_tail = 0;

/* Utility macros */
#define MARK(room) (SET_BIT(ROOM_FLAGS(room), RM1_BFS_MARK))
#define UNMARK(room) (REMOVE_BIT(ROOM_FLAGS(room), RM1_BFS_MARK))
#define IS_MARKED(room) (IS_SET(ROOM_FLAGS(room), RM1_BFS_MARK))
#define TOROOM(x, y) (world[(x)].dir_option[(y)]->to_room)
#define IS_CLOSED(x, y) (IS_SET(world[(x)].dir_option[(y)]->exit_info, EX_CLOSED))

#ifdef TRACK_THROUGH_DOORS
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
													(TOROOM(x, y) != NOWHERE) &&	\
													(!IS_MARKED(TOROOM(x, y))))
#else
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
													(TOROOM(x, y) != NOWHERE) &&	\
													(!IS_CLOSED(x, y)) &&		\
													(!IS_MARKED(TOROOM(x, y))))
#endif

/* control the hunts going on */
void perform_hunt(void)
{

	struct char_data *ch;



	for (ch = character_list; ch; ch = ch->next) {
		if HUNTING
			(ch) {
			if (IS_PC(ch)) {
				hunt_victim(ch, "", CMD_HUNT, 1);
			}
			else {
				if (number(0, 2)) {
					hunt_victim(ch, "", CMD_HUNT, 1);
				}
			}
			}
	}			/* END OF for loop */

}				/* END OF perform_hunt() */


void bfs_enqueue(sh_int room, char dir)
{
	struct bfs_queue_struct *curr;



	CREATE(curr, struct bfs_queue_struct, 1);
	curr->room = room;
	curr->dir = dir;
	curr->next = 0;

	if (queue_tail) {
		queue_tail->next = curr;
		queue_tail = curr;
	}
	else {
		queue_head = queue_tail = curr;
	}
}				/* END OF bfs_enqueue() */


void bfs_dequeue(void)
{
	struct bfs_queue_struct *curr;



	curr = queue_head;

	if (!(queue_head = queue_head->next)) {
		queue_tail = 0;
	}
	free(curr);

}				/* END OF bfs_dequeue() */


void bfs_clear_queue(void)
{



	while (queue_head)
		bfs_dequeue();
}


/* find_first_step: given a source room and a target room, find the
* first step on the shortest path from the source to the target.
* Intended usage: in mb1000_mobile_activity, give a mob a dir to go if
* they're tracking another mob or a PC.  Or, a 'track' skill for PCs.
*/

int find_first_step(unsigned long src, unsigned long target)
{
	int curr_dir;
	unsigned long curr_room;



	if (src < 0 || src > top_of_world || target < 0 || target > top_of_world) {
		main_log("Illegal value passed to find_first_step (graph.c)");
		return BFS_ERROR;
	}
	if (src == target)
		return BFS_ALREADY_THERE;

	/* clear marks first */
	for (curr_room = 0; curr_room <= top_of_world; curr_room++)
		UNMARK(curr_room);

	MARK(src);

	/* first, enqueue the first steps, saving which direction we're going. */
	for (curr_dir = 0; curr_dir < MAX_DIRS; curr_dir++)
		if (VALID_EDGE(src, curr_dir)) {
			MARK(TOROOM(src, curr_dir));
			bfs_enqueue(TOROOM(src, curr_dir), curr_dir);
		}
	/* now, do the classic BFS. */
	while (queue_head) {
		if (queue_head->room == target) {
			curr_dir = queue_head->dir;
			bfs_clear_queue();
			return curr_dir;
		}
		else {
			for (curr_dir = 0; curr_dir < MAX_DIRS; curr_dir++)
				if (VALID_EDGE(queue_head->room, curr_dir)) {
					MARK(TOROOM(queue_head->room, curr_dir));
					bfs_enqueue(TOROOM(queue_head->room, curr_dir), queue_head->dir);
				}
			bfs_dequeue();
		}
	}

	return BFS_NO_PATH;
}


/****************************************************************** *
*  Functions and Commands which use the above fns		    *
******************************************************************* */

void do_hunt(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *vict;



	if (IS_PC(ch) && !(GET_SKILL(ch, SKILL_HUNT))) {
		send_to_char("You don't have the skill for this.\r\n", ch);
		return;
	}

	one_argument(arg, arg);
	if (!*arg) {
		send_to_char("Whom do you wish to hunt?\r\n", ch);
		return;
	}

	if (!(vict = ha3100_get_char_vis(ch, arg))) {
		send_to_char("No-one around by that name.\r\n", ch);
		return;
	}

	/* YOU CAN'T TRACK IMMORTS */
	if (IS_PC(vict) && GET_LEVEL(vict) > PK_LEV) {
		if (GET_LEVEL(ch) < PK_LEV ||
		    (GET_LEVEL(ch) < IMO_IMP && GET_LEVEL(vict) >= IMO_IMP)) {
			send_to_char("That person is to holy to be tracked.\r\n", ch);
			return;
		}
	}

	/* IS MOB CANT HUNT? */
	if (IS_SET(GET_ACT1(vict), PLR1_CANT_HUNT) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("A powerful force prevents you from doing that!\r\n", ch);
		return;
	}

	if (vict->in_room < 1) {
		send_to_char("You can't find a path to that person...\r\n", ch);
		return;
	}

	/* IS ROOM CANT HUNT? */
	if (IS_SET(world[vict->in_room].room_flags, RM1_CANT_HUNT) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("A powerful force inhibits your doing that!\r\n", ch);
		return;
	}

	HUNTING(ch) = vict;

	/* LETS LOCATE/HUNT THAT PUPPY! */
	hunt_victim(ch, "", CMD_HUNT, 0);

	return;

}				/* END OF do_hunt() */

void do_track(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *vict;
	int dir, num, idx;
	char buf[MAX_STRING_LENGTH];



	if (!(GET_SKILL(ch, SKILL_TRACK))) {
		send_to_char("You don't have the skill for this.\r\n", ch);
		return;
	}

	one_argument(arg, arg);
	if (!*arg) {
		send_to_char("Who do you wish to track?\r\n", ch);
		return;
	}

	if (!(vict = ha3100_get_char_vis(ch, arg))) {
		send_to_char("No-one around by that name.\r\n", ch);
		return;
	}

	/* YOU CAN'T HUNT IMMORTS */
	if ((IS_PC(vict)) && GET_LEVEL(vict) > PK_LEV) {
		if (GET_LEVEL(ch) < PK_LEV ||
		    (GET_LEVEL(ch) < IMO_IMP && GET_LEVEL(vict) >= IMO_IMP)) {
			send_to_char("That person is to holy to be hunted.\r\n", ch);
			return;
		}
	}

	/* IS MOB CANT HUNT? */
	if (IS_SET(GET_ACT1(vict), PLR1_CANT_HUNT) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("Protective magic prevents you from doing that!\r\n", ch);
		return;
	}

	/* IS ROOM CANT HUNT? */
	if (IS_SET(world[vict->in_room].room_flags, RM1_CANT_HUNT) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("A powerful force inhibits your doing that!\r\n", ch);
		return;
	}

	/* ARE WE IN A MAZE? */
	if (IS_SET(world[ch->in_room].room_flags, RM1_MAZE)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "The signs are all mixed up and you can't find a trail.\r\n");
		send_to_char(buf, ch);
		return;
	}

	dir = find_first_step(ch->in_room, vict->in_room);

	if (dir == BFS_ERROR) {
		send_to_char("Hmm.. something seems to be wrong.\r\n", ch);
		return;
	}

	if (dir == BFS_ALREADY_THERE) {
		send_to_char("You're already in the same room!!\r\n", ch);
		return;
	}

	if (dir == BFS_NO_PATH) {
		sprintf(buf, "You can't sense a trail to %s from here.\r\n",
			HMHR(vict));
		send_to_char(buf, ch);
		return;
	}

	if (dir < 0 || dir > MAX_DIRS - 1) {
		send_to_char("hrmmmm direction is less than 1 - Sorry.\r\n", ch);
		return;
	}

	num = FALSE;
	/* TWO CHANCES TO SUCCESSFULLY HUNT */
	if (number(0, 101) < GET_SKILL(ch, SKILL_TRACK))
		num = TRUE;
	if (number(50, 101) < GET_SKILL(ch, SKILL_HUNT))
		num = TRUE;
	if (num == FALSE) {
		/* WE FAILED SKILL, SO PICK A RANDOM DIRECTION */
		idx = 0;
		dir = 0;
		while (!CAN_GO(ch, dir) && idx < 100) {
			dir = number(0, MAX_DIRS - 1);
			idx++;
		}
		if (idx > 99) {
			sprintf(buf, "You fail.\r\n");
		}
		else {
			sprintf(buf, "You sense a trail %s from here!\r\n", dirs[dir]);
		}
		send_to_char(buf, ch);
		return;
	}
	if (num == TRUE) {
		li9900_gain_proficiency(ch, SKILL_TRACK);
		dir = find_first_step(ch->in_room, vict->in_room);
		sprintf(buf, "You sense a trail %s from here!\r\n", dirs[dir]);
	}
	send_to_char(buf, ch);
	return;


}				/* END OF do_track() */


/* WE COME INTO THIS PARAGRAPH BY A PC HUNTING, OR AN NPC HUNTING A PLAYER */

void hunt_victim(struct char_data * ch, char *arg, int cmd, int status)
{

	struct char_data *vict;
	char buf[MAX_STRING_LENGTH];

	int dir, num, lv_room;
	byte found;
	struct char_data *tmp;

	/* SET vict variable */
	vict = HUNTING(ch);
	if (!ch || !vict)
		return;

	/* make sure the char still exists.  Handle, quits, deaths, etc. */
	found = 0;
	for (tmp = character_list; tmp && !found; tmp = tmp->next) {
		if (HUNTING(ch) == tmp) {
			found = 1;
		}
	}

	/* IS THE CHAR NO LONG IN THE WORLD? */
	if (!found) {
		send_to_char("Hrmmmm, your quarry has made good with an escape.\r\n", ch);
		HUNTING(ch) = 0;
		return;
	}

	/* HAD A CRASH WHERE THE MOB's in_room variable was out of bounds */
	if (ch->in_room < 0 || ch->in_room > top_of_world) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Hunter %s has a room of %d",
			GET_REAL_NAME(ch), ch->in_room);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
		HUNTING(ch) = 0;
		return;
	}
	if (vict->in_room < 0 || vict->in_room > top_of_world) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Hunt victim %s has a room of %d",
			GET_REAL_NAME(vict), vict->in_room);
		do_sys(buf, IMO_IMP, vict);
		spec_log(buf, ERROR_LOG);
		HUNTING(ch) = 0;
		return;
	}

	/* WE DON'T WANT MOBS HUNTING/KILLING MOBS */
	if (IS_NPC(ch) && IS_NPC(vict)) {
		/* LETS SEE IF THIS GUY HAS A LEADER */
		if (vict->master) {
			HUNTING(ch) = vict->master;
		}
		else {
			HUNTING(ch) = 0;
		}
		return;
	}

	/* MAKE SKILL CHECK */
	if (IS_PC(ch)) {
		num = FALSE;
		/* TWO CHANCES TO SUCCESSFULLY HUNT */
		if (number(0, 101) < GET_SKILL(ch, SKILL_HUNT))
			num = TRUE;
		if (number(50, 101) < GET_SKILL(ch, SKILL_HUNT))
			num = TRUE;
		if (num == FALSE) {
			sprintf(buf, "Examining the surroundings, you're unable to find a path to %s.\r\n",
				GET_REAL_NAME(vict));
			send_to_char(buf, ch);
			HUNTING(ch) = 0;
			return;
		}
	}

	if (status == 0) {
		li9900_gain_proficiency(ch, SKILL_HUNT);
	}

	/* HAVE WE FOUND OUR VICTIM? */
	if (ch->in_room == vict->in_room) {
		HUNTING(ch) = 0;
		if (IS_PC(ch)) {
			send_to_char("You've found your quarry.\r\n", ch);
		}
		else {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s has found YOU!\r\n",
				GET_REAL_NAME(ch));
			send_to_char(buf, vict);
			at2100_do_mob_hunt_msgs(ch, vict, 2);
			hit(ch, vict, TYPE_UNDEFINED);
		}
		return;
	}

	/* ARE WE IN A MAZE? */
	if (IS_PC(ch) && IS_SET(world[ch->in_room].room_flags, RM1_MAZE)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "The signs are all mixed up and you can't find a trail.\r\n");
		send_to_char(buf, ch);
		HUNTING(ch) = 0;
		return;
	}

	dir = find_first_step(ch->in_room, vict->in_room);

	/* IF VICTIM IS IN THE ROOM ALREADY, WE'LL HAVE A NULL */
	if (!HUNTING(ch)) {
		return;
	}

	if (dir < 0 || dir > MAX_DIRS - 1) {
		bzero(buf, sizeof(buf));
		if (ch->in_room == vict->in_room) {
			sprintf(buf, "You've found your quarry.\r\n");
		}
		else {
			if (status == 0) {
				sprintf(buf, "You can't find a path to %s!",
					GET_REAL_NAME(vict));
			}
			else {
				sprintf(buf, "You've lost %s!",
					GET_REAL_NAME(vict));
			}
		}
		send_to_char(buf, ch);
		if (IS_PC(ch)) {
			HUNTING(ch) = 0;
		}
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "You hunt %s and head %s.\r\n",
		GET_REAL_NAME(vict),
		dirs[dir]);
	send_to_char(buf, ch);

	if (world[ch->in_room].dir_option[dir]) {
		lv_room = world[ch->in_room].dir_option[dir]->to_room;

		/* IS ROOM NEGATIVE? */
		if (lv_room < 0 || lv_room > top_of_world) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: Room number is invalid: %d - ending hunt.\r\n",
				lv_room);
			send_to_char(buf, ch);
			do_sys(buf, IMO_IMP, ch);
			HUNTING(ch) = 0;
			return;
		}

		/* WILL IT TAKE US TO A DT? */
		if (IS_SET(world[lv_room].room_flags, RM1_DEATH)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "A kind force prevents you from heading %s.\r\n",
				dirs[dir]);
			send_to_char(buf, ch);
			if (IS_PC(ch)) {
				HUNTING(ch) = 0;
			}
			return;
		}

		/* IF WE'RE A MOB WILL IT TAKE US TO A RM1_NO_MOB */
		if (IS_NPC(ch) && IS_SET(world[lv_room].room_flags, RM1_NO_MOB)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "A hateful force won't allow mobs in there!\r\n");
			send_to_char(buf, ch);
			return;
		}

		/* IF WE'RE A MOB WILL IT TAKE US OUT OF ZONE? */
		if (IS_NPC(ch)) {
			if (world[ch->in_room].zone != world[lv_room].zone) {
				if ((IS_SET(ch->specials.act, PLR1_STAY_ZONE))) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "A hateful force prevents you from leaving this realm!\r\n");
					send_to_char(buf, ch);
					return;
				}
			}
		}

	}

	/* DON'T HUNT IF THIS IS OUR FIRST SEARCH */
	if (status != 0) {
		bzero(buf, sizeof(buf));
		strcpy(buf, dirs[dir]);
		mo1500_do_move(ch, buf, dir + 1);
	}

	/* NOTE if we hit a closed door in do_move, it'll return null */
	/* pointer in HUNTING(ch) so make sure its got a value   */
	if (HUNTING(ch)) {
		/* HAVE WE FOUND OUR VICTIM? */
		if (ch->in_room == HUNTING(ch)->in_room) {
			HUNTING(ch) = 0;
			if (IS_PC(ch)) {
				send_to_char("You've found your quarry.\r\n", ch);
			}
			else {
				bzero(buf, sizeof(buf));
				sprintf(buf, "%s has found YOU!\r\n",
					GET_REAL_NAME(ch));
				send_to_char(buf, vict);
				at2100_do_mob_hunt_msgs(ch, vict, 2);
				hit(ch, vict, TYPE_UNDEFINED);
			}
			return;
		}
	}

	return;

}				/* END OF hunt_victim() */

void hu9000_call_for_help(struct char_data * ch, struct char_data * victim, int lv_flag)
{

	struct char_data *temp_vict, *next_vict;
	int lv_number_assisting;



	/* IF CHARMED, GET OUT */
	if (IS_AFFECTED(ch, AFF_CHARM)) {
		return;
	}

	if (ch->in_room < 0) {
		return;
	}


	lv_number_assisting = 0;
	for (temp_vict = character_list; temp_vict; temp_vict = next_vict) {
		next_vict = temp_vict->next;
		if (ch == temp_vict)
			continue;

		if (IS_PC(temp_vict))
			continue;
		/*
		 if (HUNTING(temp_vict))
		 continue;
		 */

		if (!IS_SET(GET_ACT1(temp_vict), PLR1_HUNTASSIST))
			continue;

		/*
		 if (GET_SKILL(temp_vict, SKILL_HUNT) == 0)
		 continue;
		 */

		if (temp_vict->specials.fighting)
			continue;

		if (IS_AFFECTED(temp_vict, AFF_CHARM))
			continue;

		if (temp_vict->in_room < 0)
			continue;

		/* IF MOB HAS GUARD SKILL OR SAME VNUM IT WILL ASSIST */
		if (!IS_SET(GET_ACT1(temp_vict), PLR1_GUARD1) &&
		    ch->nr != temp_vict->nr) {
			continue;
		}

		if (IS_SET(GET_ACT1(temp_vict), PLR1_STAY_ZONE) &&
		  world[ch->in_room].zone != world[temp_vict->in_room].zone) {
			continue;

		}

		lv_number_assisting++;
		HUNTING(temp_vict) = victim;

	}			/* END OF for loop */


	/* IF WE MADE IT HERE, LETS DO IT */
	if (lv_number_assisting) {
		act("$n signals for help!\r\n", TRUE, ch, 0, 0, TO_ROOM);
	}

	return;

}				/* END OF hu9000_call_for_help() */
