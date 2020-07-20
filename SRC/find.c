/* fi */
/* *******************************************************************
*  File: find.c                                  Part of CircleMUD   *
*  Usage: various graph algorithms / 			             *
*  All rights reserved.  See license.doc for complete information.   *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins Univ. *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.          *
******************************************************************** */


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

/* Externals */
extern int top_of_world;
extern const char *dirs[];
extern struct room_data *world;

#define FI_BFS_ERROR               -1
#define FI_BFS_ALREADY_THERE       -2
#define FI_BFS_NO_PATH             -3

struct fi_bfs_queue_struct {
	sh_int room;
	char dir;
	struct fi_bfs_queue_struct *next;
};

static struct fi_bfs_queue_struct *queue_head = 0, *queue_tail = 0;

/* Utility macros */
#define FI_VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
														 (TOROOM(x, y) != NOWHERE) &&	\
														 (!IS_MARKED(TOROOM(x, y))))

int current_room;

void fi_bfs_enqueue(sh_int room, char dir)
{
	struct fi_bfs_queue_struct *curr;

	CREATE(curr, struct fi_bfs_queue_struct, 1);
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
}				/* END OF fi_bfs_enqueue() */


void fi_bfs_dequeue(void)
{
	struct fi_bfs_queue_struct *curr;

	curr = queue_head;

	if (!(queue_head = queue_head->next)) {
		queue_tail = 0;
	}
	free(curr);

}				/* END OF fi_bfs_dequeue() */


void fi_bfs_clear_queue(void)
{


	while (queue_head)
		fi_bfs_dequeue();
}


/* find_first_step: given a source room and a target room, find the
* first step on the shortest path from the source to the target.
* Intended usage: in mb1000_mobile_activity, give a mob a dir to go if
* they're tracking another mob or a PC.  Or, a 'track' skill for PCs.
*/

int fi_find_first_step(unsigned long src, unsigned long target)
{
	int curr_dir;
	unsigned long curr_room;


	if (src < 0 || src > top_of_world || target < 0 || target > top_of_world) {
		main_log("Illegal value passed to fi_find_first_step (graph.c)");
		return FI_BFS_ERROR;
	}
	if (src == target)
		return FI_BFS_ALREADY_THERE;

	/* clear marks first */
	for (curr_room = 0; curr_room <= top_of_world; curr_room++)
		UNMARK(curr_room);

	MARK(src);

	/* first, enqueue the first steps, saving which direction we're going. */
	for (curr_dir = 0; curr_dir < MAX_DIRS; curr_dir++)
		if (FI_VALID_EDGE(src, curr_dir)) {
			MARK(TOROOM(src, curr_dir));
			fi_bfs_enqueue(TOROOM(src, curr_dir), curr_dir);
		}
	/* now, do the classic fi_bfs. */
	while (queue_head) {
		if (queue_head->room == target) {
			curr_dir = queue_head->dir;
			fi_bfs_clear_queue();
			return curr_dir;
		}
		else {
			for (curr_dir = 0; curr_dir < MAX_DIRS; curr_dir++)
				if (FI_VALID_EDGE(queue_head->room, curr_dir)) {
					MARK(TOROOM(queue_head->room, curr_dir));
					fi_bfs_enqueue(TOROOM(queue_head->room, curr_dir), queue_head->dir);
				}
			fi_bfs_dequeue();
		}
	}

	return FI_BFS_NO_PATH;
}


void do_find(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *vict;



	/* do *NOT* enable this command */

	/* send_to_char("This command has been disabled!\r\n",ch); return; */



	if (GET_LEVEL(ch) < IMO_IMP)
		return;

	one_argument(arg, arg);
	if (!*arg) {
		send_to_char("What path do you want to find?\r\n", ch);
		return;
	}

	if (!(vict = ha3100_get_char_vis(ch, arg))) {
		send_to_char("No-one around by that name.\r\n", ch);
		return;
	}

	if (IS_NPC(ch)) {
		send_to_char("Sorry mobs aren't allowed to use this command.\r\n", ch);
		return;
	}

	if (vict->in_room < 1) {
		send_to_char("You can't find a path to that person...\r\n", ch);
		return;
	}

	HUNTING(ch) = vict;
	current_room = ch->in_room;
	send_to_char("Speedwalk to target is: ", ch);

	find_victim(ch, "", CMD_HUNT, 0, 0);

	return;

}				/* END OF do_find() */


void find_victim(struct char_data * ch, char *arg, int cmd, int status, int loop)
{

	struct char_data *vict;
	char buf[MAX_STRING_LENGTH];

	int dir, lv_room;
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
		send_to_char("Lost target, Please use another target.\r\n", ch);
		HUNTING(ch) = 0;
		return;
	}

	/* HAD A CRASH WHERE THE MOB's in_room variable was out of bounds */
	if (current_room < 0 || current_room > top_of_world) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Finder %s has a room of %d",
			GET_REAL_NAME(ch), current_room);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
		HUNTING(ch) = 0;
		return;
	}
	if (vict->in_room < 0 || vict->in_room > top_of_world) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Find victim %s has a room of %d",
			GET_REAL_NAME(vict), vict->in_room);
		do_sys(buf, IMO_IMP, vict);
		spec_log(buf, ERROR_LOG);
		HUNTING(ch) = 0;
		return;
	}

	/* HAVE WE FOUND OUR VICTIM? */
	if (current_room == vict->in_room) {
		HUNTING(ch) = 0;
		send_to_char("You are there already.\r\n", ch);

	}

	dir = fi_find_first_step(current_room, vict->in_room);

	/* IF VICTIM IS IN THE ROOM ALREADY, WE'LL HAVE A NULL */
	if (!HUNTING(ch)) {
		return;
	}

	if (dir < 0 || dir > MAX_DIRS - 1) {
		bzero(buf, sizeof(buf));
		if (current_room == vict->in_room) {
			sprintf(buf, "Path completed (2).\r\n");
		}
		else {
			if (status == 0) {
				sprintf(buf, "Unable to find a path to %s from your current location!",
					GET_REAL_NAME(vict));
			}
			else {
				sprintf(buf, "You've lost %s! (please post about this)",
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
	/* sprintf(buf, "Finding: %s , room: %d , Heading: %s.\r\n",
	 * GET_REAL_NAME(vict), current_room, dirs[dir]); send_to_char(buf,
	 * ch); */

	if (world[current_room].dir_option[dir]) {
		lv_room = world[current_room].dir_option[dir]->to_room;

		/* IS ROOM NEGATIVE? */
		if (lv_room < 0 || lv_room > top_of_world) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: Room number is invalid: %d - ending find.\r\n",
				lv_room);
			send_to_char(buf, ch);
			do_sys(buf, IMO_IMP, ch);
			spec_log(buf, ERROR_LOG);
			HUNTING(ch) = 0;
			return;
		}

		/* WILL IT TAKE US TO A DT? */
		if (IS_SET(world[lv_room].room_flags, RM1_DEATH)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Going %s would lead us into a DT, Find halted.\r\n",
				dirs[dir]);
			send_to_char(buf, ch);
			if (IS_PC(ch)) {
				HUNTING(ch) = 0;
			}
			return;
		}

	}

	/* DON'T HUNT IF THIS IS OUR FIRST SEARCH */
	if (status != 0) {
		current_room = lv_room;
		sprintf(buf, "%s", dirs[dir]);
		buf[1] = 0;
		send_to_char(buf, ch);
	}

	/* NOTE if we hit a closed door in do_move, it'll return null */
	/* pointer in HUNTING(ch) so make sure its got a value   */
	if (HUNTING(ch)) {
		/* HAVE WE FOUND OUR VICTIM? */
		if (current_room == HUNTING(ch)->in_room) {
			HUNTING(ch) = 0;
			send_to_char("\r\n", ch);
		}
	}
	loop++;
	if (loop < 100) {
		find_victim(ch, "", CMD_HUNT, 1, loop);
	}
	else {
		send_to_char("Haven't found a path after 100 loops, Aborting find.\r\n", ch);
	}
	return;

}				/* END OF hunt_victim() */
