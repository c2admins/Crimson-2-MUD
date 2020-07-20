/* xn */
/* gv_location: 22001-22500 */
/* ****************************************************************** *
*  file: xanth.c , Special module.                Part of DIKUMUD     *
*  Usage: Procedures handling special procs for object/room/mobile    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.    *
********************************************************************* */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include "utility.h"
#include "structs.h"
#include "parser.h"
#include "db.h"
#include "comm.h"
#include "constants.h"
#include "spells.h"
#include "ansi.h"
#include "xanth.h"
#include "math.h"
#include "handler.h"
#include "globals.h"
#include "func.h"

const char *button_color[] = {
	"black",
	"red",
	"green",
	"brown",
	"blue",
	"purple",
	"yellow",
	"gray"
};
#define BLACK_BUTTON   BIT0
#define RED_BUTTON     BIT1
#define GREEN_BUTTON   BIT2
#define BROWN_BUTTON   BIT3
#define BLUE_BUTTON    BIT4
#define PURPLE_BUTTON  BIT5
#define YELLOW_BUTTON  BIT6
#define GRAY_BUTTON    BIT7
#define ALL_BUTTONS    BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7

#define MAX_WRAITHS    20

void xn1000_xanth_set_puzzle(struct char_data * ch, int cmd, char *arg)
{

	int idx, jdx;



	/* RESET STARTING ROW/COL */
	pz_puzzle_row = XANTH_ENTRY_ROW;
	pz_puzzle_col = XANTH_ENTRY_COL;

	/* ZERO PUZZLE ARRAY */
	for (idx = 0; idx < NUMBER_PUZZLE_ROOMS; idx++) {
		for (jdx = 0; jdx < NUMBER_PUZZLE_ROOMS; jdx++) {
			pz_ar[idx][jdx] = 0;
		}		/* END OF for LOOP */
	}			/* END OF for LOOP */

	/* SURROUND THE PUZZLE ROOMS WITH MARBLE */
	for (idx = 0; idx < NUMBER_PUZZLE_ROOMS; idx++) {
		pz_ar[idx][0] = PZ_MARBLE;
		pz_ar[idx][NUMBER_PUZZLE_ROOMS - 1] = PZ_MARBLE;
		pz_ar[0][idx] = PZ_MARBLE;
		pz_ar[NUMBER_PUZZLE_ROOMS - 1][idx] = PZ_MARBLE;
	}			/* END OF for LOOP */

	/* OK ALL OF THE ABOVE IS STANDARD FOR ALL PUZZLES */
	/* TO ADD A NEW PUZZLE, CHANGE THE FOLLOWING, AND  */
	/* SET THE SOLVE VARIABLES DEFINED IN puzzle.h     */

	/* ADD WALLS/ROOMS FOR THE SPECIFIC PUZZLE */
	for (idx = 0; idx < NUMBER_PUZZLE_ROOMS; idx++) {
		pz_ar[idx][7] = PZ_MARBLE;
		pz_ar[7][idx] = PZ_MARBLE;
		pz_ar[8][idx] = PZ_MARBLE;
	}			/* END OF for LOOP */
	pz_ar[1][2] = PZ_MOVE_STONE;
	pz_ar[1][5] = PZ_MOVE_STONE;
	pz_ar[2][1] = PZ_MOVE_STONE;
	pz_ar[2][3] = PZ_MARBLE;
	pz_ar[2][5] = PZ_LADDER_NORTH;
	pz_ar[3][5] = PZ_MARBLE;
	pz_ar[4][1] = PZ_LADDER_EAST;
	pz_ar[4][4] = PZ_MOVE_STONE;
	pz_ar[4][5] = PZ_MOVE_STONE;
	pz_ar[5][3] = PZ_MOVE_STONE;
	pz_ar[6][1] = PZ_MARBLE;
	pz_ar[6][2] = PZ_MARBLE;
	pz_ar[6][6] = PZ_MARBLE;
	pz_ar[7][3] = 0;

	/* SET UP ROOM DESCRIPTIONS */
	xn1100_initialize_xanth_room();
	return;

}				/* END OF xn1000_xanth_set_puzzle() */


void xn1100_initialize_xanth_room(void)
{

	char buf[MAX_STRING_LENGTH];
	int idx, lv_room;



	lv_room = db8000_real_room(XANTH_PUZZLE_ROOM);
	if (lv_room < 0) {
		sprintf(buf, "ERROR: Room %d passed to xn1100 is invalid.",
			XANTH_PUZZLE_ROOM);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return;
	}

	/* REMOVE DESCRIPTION IF ROOM HAS ONE */
	if (world[lv_room].description) {
		free(world[lv_room].description);
		world[lv_room].description = 0;
	}

	/* ARE WE IN THE RIGHT PLACE TO SEE THE EXIT ROOM? */
	if (pz_puzzle_row == XANTH_EXIT_ROW &&
	    pz_puzzle_row == XANTH_EXIT_ROW) {
		world[lv_room].description =
			strdup("There is a small hole in the floor.\r\n");
	}
	else {
		world[lv_room].description =
			strdup("A low ceiling prevents an upward exit.\r\n");
	}

	for (idx = 0; idx < MAX_DIRS; idx++) {
		/* REMOVE OLD LINKS */
		wi2925_remove_room_links(XANTH_PUZZLE_ROOM, idx);
		/* INSERT NEW LINKS */
		xn1200_xanth_create_link(lv_room, idx);
	}

}				/* END OF xn1100_initialize_xanth_room() */


void xn1200_xanth_create_link(lv_room, lv_dir)
{

	int lv_adj_row, lv_adj_col;
	char buf[MAX_STRING_LENGTH];



	lv_adj_row = 0;
	lv_adj_col = 0;

	switch (lv_dir) {
	case NORTH:{
			lv_adj_row = -1;
			break;
		}
	case SOUTH:{
			lv_adj_row = +1;
			break;
		}
	case EAST:{
			lv_adj_col = +1;
			break;
		}
	case WEST:{
			lv_adj_col = -1;
			break;
		}
	case UP:{
			/* IF WE PUT THE SOLVE BLOCK IN PLACE, WE'LL CREATE
			 * EXIT */
			if (pz_ar[XANTH_SOLVE_ROW][XANTH_SOLVE_COL] ==
			    XANTH_SOLVE_BLOCK) {
				break;
			}
			return;
		}
	case DOWN:{
			/* IF AT ENTRY, WE NEED AN EXIT OUTTA HERE */
			if (pz_puzzle_row == XANTH_ENTRY_ROW &&
			    pz_puzzle_col == XANTH_ENTRY_COL) {
				break;
			}
			return;
		}
	default:{
			return;
		}
	}			/* END OF switch() */

	/* THE ROOM WE ARE IN WILL BE EMPTY, SO IF WE FALL THROUGHT TO */
	/* HERE, buf WILL STAY ZERO */
	bzero(buf, sizeof(buf));
	switch (pz_ar[pz_puzzle_row + lv_adj_row][pz_puzzle_col + lv_adj_col]) {
	case PZ_MARBLE:{
			sprintf(buf, "You see a solid marble wall to the %s.\r\n",
				dirs[lv_dir]);
			break;
		}
	case PZ_MOVE_STONE:{
			sprintf(buf, "You see a sandstone wall to the %s.\r\n",
				dirs[lv_dir]);
			break;
		}
	case PZ_LADDER_NORTH:{
			if (lv_dir == SOUTH) {
				sprintf(buf, "You see a ladder attached to the sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			}
			else
				sprintf(buf, "You see a sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			break;
		}
	case PZ_LADDER_SOUTH:{
			if (lv_dir == NORTH) {
				sprintf(buf, "You see a ladder attached to the sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			}
			else
				sprintf(buf, "You see a sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			break;
		}
	case PZ_LADDER_WEST:{
			if (lv_dir == EAST) {
				sprintf(buf, "You see a ladder attached to the sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			}
			else
				sprintf(buf, "You see a sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			break;
		}
	case PZ_LADDER_EAST:{
			if (lv_dir == WEST) {
				sprintf(buf, "You see a ladder attached to the sandstone wall to the %s.\r\n",
					dirs[lv_dir]);
			}
			else
				sprintf(buf, "You see a ladder attached to the sandstone to the %s.\r\n",
					dirs[lv_dir]);
			break;
		}
	}			/* END OF switch() */


	if (buf[0] != 0) {

		if (world[lv_room].description)
			world[lv_room].description =
				(char *) realloc(world[lv_room].description,
			strlen(world[lv_room].description) + strlen(buf) + 1);
		else {
			world[lv_room].description = (char *) malloc(strlen(buf) + 1);
			bzero(world[lv_room].description, strlen(buf) + 1);
		}
		strcat(world[lv_room].description, buf);
	}
	else {
		CREATE(world[lv_room].dir_option[lv_dir],
		       struct room_direction_data, 1);
		world[lv_room].dir_option[lv_dir]->keyword = 0;
		world[lv_room].dir_option[lv_dir]->exit_info = 0;
		world[lv_room].dir_option[lv_dir]->key = 0;
		world[lv_room].dir_option[lv_dir]->to_room = lv_room;
		if (lv_dir == UP) {
			if (db8000_real_room(XANTH_PUZZLE_EXIT) < 0) {
				main_log("Xanth exit room is invalid.");
				spec_log("Xanth exit room is invalid.", ERROR_LOG);
				return;
			}
			world[lv_room].dir_option[lv_dir]->to_room =
				db8000_real_room(XANTH_PUZZLE_EXIT);
		}
		if (lv_dir == DOWN) {
			if (db8000_real_room(XANTH_PUZZLE_ENTRY) < 0) {
				main_log("Xanth entry room is invalid.");
				spec_log("Xanth entry room is invalid.", ERROR_LOG);
				return;
			}
			world[lv_room].dir_option[lv_dir]->to_room =
				db8000_real_room(XANTH_PUZZLE_ENTRY);
		}
	}

	return;

}				/* END OF xn1100_initialize_xanth_room() */

int xn2000_xanth_entry_room(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_NORTH &&
	    cmd != CMD_EAST &&
	    cmd != CMD_SOUTH &&
	    cmd != CMD_WEST &&
	    cmd != CMD_UP &&
	    cmd != CMD_DOWN) {
		return (FALSE);
	}

	/* IS THERE AN EXIT THAT WAY? */
	if (world[ch->in_room].dir_option[cmd - 1] == 0)
		return (FALSE);

	/* MAKE SURE ITS NOT NOWHERE */
	if (world[ch->in_room].dir_option[cmd - 1]->to_room < 0)
		return (FALSE);

	/* IF ITS NOT OUR PUZZLE ROOM, GET OUT */
	if (world[world[ch->in_room].dir_option[cmd - 1]->to_room].number !=
	    XANTH_PUZZLE_ROOM)
		return (FALSE);

	/* IF THERE IS SOMEONE IN THE PUZZLE ROOM */
	if (world[world[ch->in_room].dir_option[cmd - 1]->to_room].people) {
		/* HAVE THEY LEFT THE ENTRANCE? */
		if (pz_puzzle_row != XANTH_ENTRY_ROW ||
		    pz_puzzle_col != XANTH_ENTRY_COL) {
			send_to_char("Something is blocking the way.\r\n", ch);
			return (TRUE);
		}
	}

	sprintf(buf, "You follow $n %s.", dirs[cmd - 1]);
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	xn1000_xanth_set_puzzle(ch, cmd, arg);
	return (FALSE);

}				/* END OF  xn2000_xanth_entry_room() */

int xn3000_xanth_puzzle_room(struct char_data * ch, int cmd, char *arg)
{

	int lv_dir, lv_adj_row, lv_adj_col;
	char buf[MAX_STRING_LENGTH];



	if (cmd == CMD_NORTH ||
	    cmd == CMD_EAST ||
	    cmd == CMD_SOUTH ||
	    cmd == CMD_WEST) {
		/* IF WE GOT HERE DIR IS VALID, UPDATE ROOM */
		switch (cmd) {
		case CMD_NORTH:
			pz_puzzle_row--;
			break;
		case CMD_SOUTH:
			pz_puzzle_row++;
			break;
		case CMD_EAST:
			pz_puzzle_col++;
			break;
		case CMD_WEST:
			pz_puzzle_col--;
			break;
		}		/* END OF switch() */

		/* SET UP NEW ROOM DESCRIPTIONS */
		xn1100_initialize_xanth_room();
		in3000_do_look(ch, "\0", 0);
		return (TRUE);
	}

	if (cmd != CMD_PUSH)
		return (FALSE);

	/* FIGURE OUT DIRECTION USER WANTS TO PUSH */
	for (; *arg == ' '; arg++);

	lv_adj_row = 0;
	lv_adj_col = 0;
	if (is_abbrev(arg, "north")) {
		lv_dir = NORTH;
		lv_adj_row = -1;
	}
	else if (is_abbrev(arg, "east")) {
		lv_dir = EAST;
		lv_adj_col = +1;
	}
	else if (is_abbrev(arg, "south")) {
		lv_dir = SOUTH;
		lv_adj_row = +1;
	}
	else if (is_abbrev(arg, "west")) {
		lv_dir = WEST;
		lv_adj_col = -1;
	}
	else {
		send_to_char("Which direction do you want to push?\r\n",
			     ch);
		return (TRUE);
	}

	/* CHECK FOR STONES */
	bzero(buf, sizeof(buf));
	switch (pz_ar[pz_puzzle_row + lv_adj_row][pz_puzzle_col + lv_adj_col]) {
	case PZ_MARBLE:{
			send_to_char("The marble wall won't budge.\r\n", ch);
			return (TRUE);
			break;
		}
	case PZ_MOVE_STONE:
	case PZ_LADDER_NORTH:
	case PZ_LADDER_SOUTH:
	case PZ_LADDER_EAST:
	case PZ_LADDER_WEST:{
			switch (lv_dir) {
			case NORTH:{
					if (pz_puzzle_row < 3) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					if (pz_ar[pz_puzzle_row - 2][pz_puzzle_col] != 0) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					break;
				}
			case SOUTH:{
					if (pz_puzzle_row > 7) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					if (pz_ar[pz_puzzle_row + 2][pz_puzzle_col] != 0) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					break;
				}
			case EAST:{
					if (pz_puzzle_col > 7) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					if (pz_ar[pz_puzzle_row][pz_puzzle_col + 2] != 0) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					break;
				}
			case WEST:{
					if (pz_puzzle_col < 3) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					if (pz_ar[pz_puzzle_row][pz_puzzle_col - 2] != 0) {
						send_to_char("Something appears to be blocking it.\r\n", ch);
						return (TRUE);
					}
					break;
				}
			}
			break;
		}
	default:{
			send_to_char("I can't find anything to push.\r\n", ch);
			return (TRUE);
		}
	}			/* END OF switch() */

	sprintf(buf, "You push the stone %s.\r\n", dirs[lv_dir]);
	send_to_char(buf, ch);
	sprintf(buf, "$n pushes against the wall and it slides %s.", dirs[lv_dir]);
	act(buf, TRUE, ch, 0, 0, TO_ROOM);

	/* LETS MOVE THAT PUPPY */
	pz_ar[pz_puzzle_row + (lv_adj_row * 2)][pz_puzzle_col + (lv_adj_col * 2)] =
		pz_ar[pz_puzzle_row + lv_adj_row][pz_puzzle_col + lv_adj_col];
	pz_ar[pz_puzzle_row + lv_adj_row][pz_puzzle_col + lv_adj_col] = 0;

	/* SET UP NEW ROOM DESCRIPTIONS */
	xn1100_initialize_xanth_room();
	return (TRUE);

}				/* END OF xn3000_xanth_work_puzzle() */


int xn3100_xanth_north_village(struct char_data * ch, int cmd, char *arg)
{

	int lv_rtn, lv_real_to, lv_real_from;



	if (cmd == CMD_SOUTHWEST) {
		/* CREATE TEMPORARY LINK */
		lv_real_to = db8000_real_room(XANTH_FROM_NORTH);
		lv_real_from = db8000_real_room(XANTH_NORTH_VILLAGE);
		lv_rtn = wi2920_create_room_links(lv_real_from, lv_real_to, CMD_UP - 1, "\0");
		if (!lv_rtn) {
			/* user has been notified */
			return (TRUE);
		}
		/* MOVE CHAR(s) */
		mo1500_do_move(ch, "\0", CMD_UP);
		/* REMOVE TEMP LINK */
		wi2925_remove_room_links(XANTH_NORTH_VILLAGE, CMD_UP - 1);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF xn3100_xanth_north_village() */


int xn3200_xanth_justin_tree(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;
	struct char_data *lv_mob, *temp_char;



	if (IS_NPC(ch))
		return (FALSE);

	if (!ch->desc)
		return (FALSE);

	/* LOOK FOR JAMA, ZINK, POTIPHER */
	lv_mob = 0;
	for (temp_char = world[ch->in_room].people; (temp_char) && (!lv_mob);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char)) {
			if (mob_index[temp_char->nr].virtual == MOB_XANTH_JAMA ||
			 mob_index[temp_char->nr].virtual == MOB_XANTH_ZINK ||
			mob_index[temp_char->nr].virtual == MOB_XANTH_POTIPHER)
				lv_mob = temp_char;
		}

	if (lv_mob) {
		/* THE BAD GUYS ARE ALIVE, SO SKIP OUT */
		return (FALSE);
	}

	/* LOOK FOR JUSTIN TREE */
	lv_mob = 0;
	for (temp_char = world[ch->in_room].people; (temp_char) && (!lv_mob);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char)) {
			if (mob_index[temp_char->nr].func == xn3200_xanth_justin_tree)
				lv_mob = temp_char;
		}

	if (!lv_mob) {
		send_to_char("No Justin tree in room!\r\n", ch);
		main_log("ERROR: NO JUSTIN TREE - MOB SPECIAL!!!");
		spec_log("ERROR: NO JUSTIN TREE - MOB SPECIAL!!!", ERROR_LOG);
		return (FALSE);
	}


	/* SEE IF SOMEONE IS USING THE SPONGE ON JUSTIN */
	if (cmd == CMD_USE) {
		arg = one_argument(arg, buf);

		/* CHECK INVENTORY */
		lv_obj = ha2075_get_obj_list_vis(ch, buf, ch->carrying);

		/* ARE WE HOLDING SOMETHING? */
		if (!lv_obj) {
			lv_obj = ch->equipment[HOLD];
		}

		/* IF WE STILL DIDN'T FIND IT, GET OUT */
		if (!lv_obj) {
			return (FALSE);
		}

		if (obj_index[lv_obj->item_number].virtual !=
		    OBJ_XANTH_SPONGE) {
			return (FALSE);
		}

		/* OK, THE PERSON IS USING A SPONGE AROUND JUSTIN, SO YIPPY */
		ansi_act("Justin psionically thanks you.   He also tells you your quest begins",
			 FALSE, ch, 0, 0, TO_CHAR, CLR_TELL, 0);
		ansi_act("by heading SOUTHWEST from the North Village.",
			 FALSE, ch, 0, 0, TO_CHAR, CLR_TELL, 0);

		/* GET RID OF SPONGE */
		if (lv_obj == ch->equipment[HOLD]) {
			ha1930_unequip_char(ch, HOLD);
		}
		ha2700_extract_obj(lv_obj);

		return (TRUE);
	}

	if (number(1, 5) < 4)
		return (FALSE);

	/* SEND A PLEA FOR HELP TO CHARS IN THE ROOM */
	for (temp_char = world[ch->in_room].people; temp_char;
	     temp_char = temp_char->next_in_room) {
		if (temp_char != lv_mob) {
			ansi_act("Justine psionically tells you that he is in pain and begs",
				 FALSE, temp_char, 0, 0, TO_CHAR, CLR_TELL, 0);
			ansi_act("you to get something to stop the bleeding.",
				 FALSE, temp_char, 0, 0, TO_CHAR, CLR_TELL, 0);
		}
	}

	return (FALSE);

}				/* END OF xn3200_xanth_justin_tree() */


int xn3300_xanth_chameleon(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int lv_new_form, lv_old_form, idx;
#define NUMBER_CHAMELEON_NAMES 3
	const char *chameleon_names[] = {
		"lizard",
		"stringray beetle",
		"fiery salamander",
		"/n"
	};



	/* IF THE ROOM ONLY HAS THE LIZARD, SKIP OUT */
	/* NOTE:  We are assured of one person because of the lizard */
	if (!(world[ch->in_room].people->next_in_room)) {
		return (FALSE);
	}

	/* SAFTY CHECK */
	if (!ch->player.long_descr) {
		ch->player.long_descr = strdup(" ");
	}

	/* FIND OLD FORM */
	lv_old_form = 0;
	for (idx = 0; idx < NUMBER_CHAMELEON_NAMES; idx++) {
		if (*(ch->player.long_descr + 2) == *(chameleon_names[idx])) {
			lv_old_form = idx;
		}
	}

	/* PICK NEW FORM */
	lv_new_form = number(1, NUMBER_CHAMELEON_NAMES) - 1;

	/* DO WE ALREADY HAVE THIS FORM? */
	if (lv_new_form == lv_old_form) {
		return (FALSE);
	}

	free(ch->player.long_descr);
	sprintf(buf, "A %s is sitting on the stone.\r\n",
		chameleon_names[lv_new_form]);
	ch->player.long_descr = strdup(buf);
	sprintf(buf, "The %s metamorphoses into a %s.",
		chameleon_names[lv_old_form], chameleon_names[lv_new_form]);
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	return (TRUE);

}				/* END OF xn3300_xanth_chameleon() */

int xn3400_xanth_auto_dump_in_ocean(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int lv_from_room, lv_to_room;



	if (GET_LEVEL(ch) >= IMO_IMP) {
		return (FALSE);
	}

	if (ch->in_room < 0) {
		return (FALSE);
	}

	/* FIGURE OUT ROOMS */
	lv_from_room = ch->in_room;
	if (world[lv_from_room].number == XANTH_OCEAN_AUTO_DUMP1)
		lv_to_room = db8000_real_room(XANTH_OCEAN_AUTO_LOAD1);
	else if (world[lv_from_room].number == XANTH_OCEAN_AUTO_DUMP2)
		lv_to_room = db8000_real_room(XANTH_OCEAN_AUTO_LOAD2);
	else if (world[lv_from_room].number == XANTH_OCEAN_AUTO_DUMP3)
		lv_to_room = db8000_real_room(XANTH_OCEAN_AUTO_LOAD3);
	else {
		sprintf(buf, "ERROR: Entered xn3400 with unexpected room: %d virtual %d",
			lv_from_room, world[lv_from_room].number);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	ha1650_all_from_room_to_room(lv_from_room, lv_to_room,
	  "You suddenly find yourself floundering in the middle of an ocean.",
				     "You notice $n is here.",
				     0);

	/* RETURN FALSE BECAUSE WE WANT USER COMMAND TO COMPLETE */
	return (FALSE);

}				/* END OF xn3400_xanth_auto_dump_in_ocean() */


int xn3500_xanth_room_say_friend(struct char_data * ch, int cmd, char *arg)
{

	int lv_dir;



	if (cmd != CMD_SAY) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);

	/* DID THEY SAY FRIEND/MELLON? */
	if (strlen(arg) != 6) {
		return (FALSE);
	}

	if (!(is_abbrev(arg, "friend")) && !(is_abbrev(arg, "mellon"))) {
		return (FALSE);
	}

	if (world[ch->in_room].number == XANTH_FRIEND_JUNCTION)
		lv_dir = CMD_WEST - 1;
	else
		lv_dir = CMD_SOUTH - 1;

	if (!ha5000_manually_open_door(ch->in_room, lv_dir, BIT0)) {
		return (FALSE);
	}

	/* LETS OPEN THAT PUPPY */
	do_say(ch, arg, CMD_SAY);
	co3300_send_to_room("Suddenly the door is outlined in a bright glow which fades.  Silently\r\n",
			    ch->in_room);
	co3300_send_to_room("the doors swing outward inch by inch until both doors lay back against\r\n",
			    ch->in_room);
	co3300_send_to_room("the wall.  Through the opening you can see the castle hallway.\r\n",
			    ch->in_room);

	return (TRUE);

}				/* END OF xn3500_xanth_room_say_friend() */

int xn3600_xanth_room_elsewhere(struct char_data * ch, int cmd, char *arg)
{

#define BUTTON_IN_LEVER_UP  1
#define BUTTON_OUT_LEVER_DOWN 2

	int lv_stat, lv_num, lv_tmp_num, lv_buttons_rc;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_ch, *next_ch;
	struct obj_data *lv_obj, *next_obj, *tmp_obj;



	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (cmd != CMD_PUSH &&
	    cmd != CMD_PULL) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	if (!*arg) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;
	if (lv_num != OBJ_XANTH_LEVER &&
	    (lv_num < OBJ_XANTH_BUTTON_BLACK ||
	     lv_num > OBJ_XANTH_BUTTON_GRAY)) {
		return (FALSE);
	}

	lv_stat = BUTTON_OUT_LEVER_DOWN;
	if (lv_obj->description) {
		if (strlen(lv_obj->description) < 3) {
			lv_stat = BUTTON_OUT_LEVER_DOWN;
		}
		else {
			sprintf(buf, lv_obj->description +
				strlen(lv_obj->description) - 3);
			/* COVER FOR button IN and LEVER up */
			if (strncmp(buf, "in", 2) &&
			    strncmp(buf, "up", 2))
				lv_stat = BUTTON_OUT_LEVER_DOWN;
			else
				lv_stat = BUTTON_IN_LEVER_UP;
		}
	}			/* END OF we have a description */

	/* USER IS PULLING */
	if (cmd == CMD_PULL) {
		if (lv_stat == BUTTON_IN_LEVER_UP) {
			if (lv_num == OBJ_XANTH_LEVER) {
				lv_buttons_rc =
					xn3625_xanth_find_status_of_buttons(ch->in_room, 0);
				if (!lv_buttons_rc || lv_buttons_rc == ALL_BUTTONS) {
					send_to_char("The lever won't budge.\r\n", ch);
					act("$n tries to pull the lever down, but it won't budge.",
					    TRUE, ch, 0, 0, TO_ROOM);
					return (TRUE);
				}
				sprintf(buf, "You pull the lever down.\r\n");
				send_to_char(buf, ch);
				act("$n pulls the lever down.", TRUE, ch, 0, 0, TO_ROOM);
				sprintf(buf, "a lever protrudes from the wall.  The lever is down.");
				if (lv_obj->description) {
					free(lv_obj->description);
				}
				lv_obj->description = strdup(buf);
				/* THE CORRECT BUTTON MUST BE PRESSED FOR
				 * ANYTHING TO HAPPEN */
				if (IS_SET(lv_buttons_rc, BROWN_BUTTON)) {
					if (lv_buttons_rc == BROWN_BUTTON + GREEN_BUTTON) {
						if (EXIT(ch, CMD_DOWN - 1)) {
							REMOVE_BIT(EXIT(ch, CMD_DOWN - 1)->exit_info,
								   EX_LOCKED);
							co3300_send_to_room("You hear something click.", ch->in_room);
							return (TRUE);
						}
					}
					co3300_send_to_room("The room flickers around you.", ch->in_room);
					for (tmp_ch = world[ch->in_room].people; tmp_ch; tmp_ch = next_ch) {
						next_ch = tmp_ch->next_in_room;
						if (GET_LEVEL(tmp_ch) < IMO_IMM) {
							spell_elsewhere(15, tmp_ch, tmp_ch, 0);
						}
					}	/* END OF for loop */
					/* RESET THE BUTTONS */
					for (tmp_obj = world[ch->in_room].contents; tmp_obj; tmp_obj = next_obj) {
						next_obj = tmp_obj->next_content;
						lv_tmp_num = obj_index[tmp_obj->item_number].virtual;
						if (lv_tmp_num >= OBJ_XANTH_BUTTON_BLACK &&
						    lv_tmp_num <= OBJ_XANTH_BUTTON_GRAY) {
							sprintf(buf, "the %s button is out.",
								button_color[lv_tmp_num - 510]);
							if (tmp_obj->description) {
								free(tmp_obj->description);
							}
							tmp_obj->description = strdup(buf);
						}
					}	/* END OF for loop */
					return (TRUE);
				}
				else {
					co3300_send_to_room("You hear a clanking sound.", ch->in_room);
				}
			}
			else {
				sprintf(buf, "You pull the %s button out.\r\n",
					button_color[lv_num - 510]);
				send_to_char(buf, ch);
				sprintf(buf, "$n pulls the %s button out.\r\n",
					button_color[lv_num - 510]);
				act(buf, FALSE, ch, 0, 0, TO_ROOM);
				sprintf(buf, "the %s button is out.",
					button_color[lv_num - 510]);
				if (lv_obj->description) {
					free(lv_obj->description);
				}
				lv_obj->description = strdup(buf);
			}
		}
		else {
			if (lv_num == OBJ_XANTH_LEVER) {
				send_to_char("Its already pulled down.\r\n", ch);
				act("$n tries to pull the lever lower, but fails.",
				    TRUE, ch, 0, 0, TO_ROOM);
			}
			else {
				send_to_char("Its already in the out position.\r\n", ch);
				act("$n tries to pull a button out, but fails.",
				    TRUE, ch, 0, 0, TO_ROOM);
			}
		}
		return (TRUE);
	}

	/* PUSH LEVER */
	if (lv_num == OBJ_XANTH_LEVER) {
		if (lv_stat == BUTTON_IN_LEVER_UP) {
			send_to_char("The lever is already pushed up.", ch);
			act("$n tries to push the lever higher, but fails.",
			    TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			sprintf(buf, "You push the lever up.\r\n");
			send_to_char(buf, ch);
			sprintf(buf, "$n pushes the lever up.\r\n");
			act(buf, FALSE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "a lever protrudes from the wall.  The lever is up.");
			if (lv_obj->description) {
				free(lv_obj->description);
			}
			lv_obj->description = strdup(buf);
		}
		return (TRUE);
	}

	/* OK, USER IS PUSHING A BUTTON */
	if (lv_stat == BUTTON_IN_LEVER_UP) {
		sprintf(buf, "You push the %s button and it pops out.\r\n",
			button_color[lv_num - 510]);
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the %s button and it pops out.\r\n",
			button_color[lv_num - 510]);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "the %s button is out.",
			button_color[lv_num - 510]);
	}
	else {
		sprintf(buf, "You push the %s button in.\r\n",
			button_color[lv_num - 510]);
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the %s button in.\r\n",
			button_color[lv_num - 510]);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "the %s button is in.",
			button_color[lv_num - 510]);
	}
	if (lv_obj->description) {
		free(lv_obj->description);
	}
	lv_obj->description = strdup(buf);

	return (TRUE);

}				/* END OF xn3600_xanth_room_elsewhere() */

int xn3625_xanth_find_status_of_buttons(int lv_room, int lv_flag)
{

	int lv_return, lv_num, lv_bit;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *tmp_obj, *next_obj;



	lv_return = 0;
	for (tmp_obj = world[lv_room].contents; tmp_obj; tmp_obj = next_obj) {
		next_obj = tmp_obj->next_content;
		lv_num = obj_index[tmp_obj->item_number].virtual;
		lv_bit = 0;
		if (lv_num == OBJ_XANTH_BUTTON_BLACK)
			lv_bit = BLACK_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_RED)
			lv_bit = RED_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_GREEN)
			lv_bit = GREEN_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_BROWN)
			lv_bit = BROWN_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_BLUE)
			lv_bit = BLUE_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_PURPLE)
			lv_bit = PURPLE_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_YELLOW)
			lv_bit = YELLOW_BUTTON;
		if (lv_num == OBJ_XANTH_BUTTON_GRAY)
			lv_bit = GRAY_BUTTON;

		if (tmp_obj->description) {
			if (strlen(tmp_obj->description) > 2) {
				sprintf(buf, tmp_obj->description +
					strlen(tmp_obj->description) - 3);
				if (!strncmp(buf, "in", 2)) {
					SET_BIT(lv_return, lv_bit);
				}
			}
		}
	}			/* END OF for loop */

	return (lv_return);

}				/* END OF xn3625_xanth_find_status_of_buttons() */


int xn3700_xanth_room_heal_lever(struct char_data * ch, int cmd, char *arg)
{

	int lv_stat, lv_num;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;



	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (cmd != CMD_PUSH &&
	    cmd != CMD_PULL) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	if (!*arg) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;
	if (lv_num != OBJ_XANTH_LEVER) {
		return (FALSE);
	}

	lv_stat = BUTTON_OUT_LEVER_DOWN;
	if (lv_obj->description) {
		if (strlen(lv_obj->description) < 3) {
			lv_stat = BUTTON_OUT_LEVER_DOWN;
		}
		else {
			sprintf(buf, lv_obj->description +
				strlen(lv_obj->description) - 3);
			/* COVER FOR button IN and LEVER up */
			if (strncmp(buf, "up", 2))
				lv_stat = BUTTON_OUT_LEVER_DOWN;
			else
				lv_stat = BUTTON_IN_LEVER_UP;
		}
	}			/* END OF we have a description */

	/* USER IS PULLING */
	if (cmd == CMD_PULL) {
		if (lv_stat == BUTTON_IN_LEVER_UP) {
			sprintf(buf, "You pull the lever down.\r\n");
			send_to_char(buf, ch);
			act("$n pulls the lever down.", TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "a lever protrudes from the wall.  The lever is down.");
			if (lv_obj->description) {
				free(lv_obj->description);
			}
			lv_obj->description = strdup(buf);
			/* ARE WE MAXED ON HITS/MANA/MOVE? */
			if (GET_HIT(ch) == li1500_hit_limit(ch) &&
			    GET_MANA(ch) == li1300_mana_limit(ch) &&
			    GET_MOVE(ch) == li1600_move_limit(ch)) {
				return (TRUE);
			}
			/* PICK A RANDOME ATTRIBUTE */
			switch (number(1, 6)) {
			case 1:{
					if (GET_REAL_STR(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_STR(ch) -= 1;
					GET_STR(ch) -= 1;
					break;
				}
			case 2:{
					if (GET_REAL_INT(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_INT(ch) -= 1;
					GET_INT(ch) -= 1;
					break;
				}
			case 3:{
					if (GET_REAL_WIS(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_WIS(ch) -= 1;
					GET_WIS(ch) -= 1;
					break;
				}
			case 4:{
					if (GET_REAL_DEX(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_DEX(ch) -= 1;
					GET_DEX(ch) -= 1;
					break;
				}
			case 5:{
					if (GET_REAL_CON(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_CON(ch) -= 1;
					GET_CON(ch) -= 1;
					break;
				}
			case 6:{
					if (GET_REAL_CHA(ch) < 4) {
						return (TRUE);
					}
					GET_REAL_CHA(ch) -= 1;
					GET_CHA(ch) -= 1;
					break;
				}
			}
			co3300_send_to_room("You hear a humming sound.\r\n",
					    ch->in_room);
			send_to_char("You feel better - but different.\r\n", ch);
			act("$n looks fit, but different.", TRUE, ch, 0, 0, TO_ROOM);
			GET_HIT(ch) = li1500_hit_limit(ch);
			GET_MANA(ch) = li1300_mana_limit(ch);
			GET_MOVE(ch) = li1600_move_limit(ch);
		}
		else {
			send_to_char("Its already pulled down.\r\n", ch);
			act("$n tries to pull the lever lower, but fails.",
			    TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	/* PUSH LEVER */
	if (lv_stat == BUTTON_IN_LEVER_UP) {
		send_to_char("The lever is already pushed up.", ch);
		act("$n tries to push the lever higher, but fails.",
		    TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		sprintf(buf, "You push the lever up.\r\n");
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the lever up.\r\n");
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "a lever protrudes from the wall.  The lever is up.");
		if (lv_obj->description) {
			free(lv_obj->description);
		}
		lv_obj->description = strdup(buf);
	}

	return (TRUE);

}				/* END OF xn3700_xanth_room_heal_lever() */


int xn3800_xanth_room_portculis_lever(struct char_data * ch, int cmd, char *arg)
{

	int lv_stat, lv_num;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;



	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (cmd != CMD_PUSH &&
	    cmd != CMD_PULL) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	if (!*arg) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;
	if (lv_num == OBJ_XANTH_BUTTON_RED) {
		xn3850_xanth_room_portculis_to_jail(ch, cmd, arg);
		return (TRUE);
	}

	if (lv_num != OBJ_XANTH_LEVER) {
		return (FALSE);
	}

	lv_stat = BUTTON_OUT_LEVER_DOWN;
	if (lv_obj->description) {
		if (strlen(lv_obj->description) < 3) {
			lv_stat = BUTTON_OUT_LEVER_DOWN;
		}
		else {
			sprintf(buf, lv_obj->description +
				strlen(lv_obj->description) - 3);
			/* COVER FOR button IN and LEVER up */
			if (strncmp(buf, "up", 2))
				lv_stat = BUTTON_OUT_LEVER_DOWN;
			else
				lv_stat = BUTTON_IN_LEVER_UP;
		}
	}			/* END OF we have a description */

	/* USER IS PULLING */
	if (cmd == CMD_PULL) {
		if (lv_stat == BUTTON_IN_LEVER_UP) {
			sprintf(buf, "You pull the lever down.\r\n");
			send_to_char(buf, ch);
			act("$n pulls the lever down.", TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "a lever protrudes from the wall.  The lever is down.");
			if (lv_obj->description) {
				free(lv_obj->description);
			}
			lv_obj->description = strdup(buf);

			if (!ha5000_manually_open_door(ch->in_room, CMD_NORTH - 1, BIT0)) {
				return (TRUE);
			}

			co3300_send_to_room("You hear a scraping sound.  You turn towards it and see the portculis rising.\r\n",
					    ch->in_room);
		}
		else {
			send_to_char("Its already pulled down.\r\n", ch);
			act("$n tries to pull the lever lower, but fails.",
			    TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	/* PUSH LEVER */
	if (lv_stat == BUTTON_IN_LEVER_UP) {
		send_to_char("The lever is already pushed up.", ch);
		act("$n tries to push the lever higher, but fails.",
		    TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		sprintf(buf, "You push the lever up.\r\n");
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the lever up.\r\n");
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "a lever protrudes from the wall.  The lever is up.");
		if (lv_obj->description) {
			free(lv_obj->description);
		}
		lv_obj->description = strdup(buf);

		if (!ha5100_manually_close_door(ch->in_room, CMD_NORTH - 1, BIT0)) {
			return (TRUE);
		}

		co3300_send_to_room("You hear a scraping sound.  You turn towards it and see the portculis lowering.\r\n",
				    ch->in_room);
	}

	return (TRUE);

}				/* END OF xn3800_xanth_room_portculis_lever() */

void xn3850_xanth_room_portculis_to_jail(struct char_data * ch, int cmd, char *arg)
{

	int lv_real_room, lv_guard_count;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_ch, *next_ch;



	lv_real_room = db8000_real_room(XANTH_OUTER_GUARD_ROOM);
	if (lv_real_room < 0) {
		send_to_char("Nothing happens because the guard room is missing.\r\n", ch);
		sprintf(buf, "ERROR: Guard room %d in xn3850 is invalid.",
			XANTH_OUTER_GUARD_ROOM);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return;
	}

	/* COUNT GUARDS IN GUARD ROOM */
	lv_guard_count = 0;
	for (tmp_ch = world[lv_real_room].people; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;
		if (mob_index[tmp_ch->nr].virtual == MOB_XANTH_GATE_GUARD) {
			lv_guard_count++;
		}
	}

	/* AT THIS POINT, CLOSE THE GATES */

	if (lv_guard_count < 1) {
		sprintf(buf, "You push the red button.\r\n");
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the red button.\r\n");
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		if (!IS_SET(EXIT(ch, CMD_WEST - 1)->exit_info, EX_CLOSED)) {
			/* CLOSE THE GATE */
			if (!ha5100_manually_close_door(ch->in_room,
							CMD_WEST - 1, BIT0))
				return;
			co3300_send_to_room("The western gate closes.\r\n", ch->in_room);
		}
		return;
	}

	/* LET PEOPLE IN GUARD ROOM KNOW SOMETHING IS GOING ON */
	co3300_send_to_room("The guards leave via the eastern gate then return passing through\r\nthe room and toss someone into the jail cell.\r\n", lv_real_room);

	if (!IS_SET(EXIT(ch, CMD_WEST - 1)->exit_info, EX_CLOSED)) {
		/* CLOSE THE GATE */
		if (!ha5100_manually_close_door(ch->in_room,
						CMD_WEST - 1, BIT0));
	}

	lv_real_room = db8000_real_room(XANTH_OUTER_JAIL_CELL);
	if (lv_real_room < 0) {
		send_to_char("Nothing happens because the cell room is missing.\r\n", ch);
		sprintf(buf, "ERROR: Guard room %d in xn3850 is invalid.",
			XANTH_OUTER_JAIL_CELL);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return;
	}

	send_to_char("Almost before you have taken your finger off of the button, the gates\r\nswing open and three guards burst through yelling something about their\r\nportcullis.  Immediately the guards set upon you, grappling your arms\r\nand bundling you inside.  You are thrown into a solid stone cell, and\r\nas you hit the floor you can hear the cell door clanging shut behind you.\r\n", ch);
	act("$n pushes the red button.  Almost before $e takes $s finger off of\r\nthe button, the gates swing open and three guards burst through yelling\r\nsomething about their portcullis.  Immediately the guards set upon\r\n$n, grappling $s arms and bundling $m through the west gate.",
	    FALSE, ch, 0, 0, TO_ROOM);

	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_real_room);

	act("The cell door is opened and $n is thrown onto the cell floor.  Almost\r\nbefore $e hits the floor, the cell door clangs shut.\r\n",
	    FALSE, ch, 0, 0, TO_ROOM);

	return;

}				/* END OF xn3850_xanth_room_portculis_to_jail()  */


int xn3900_xanth_race_class_room(struct char_data * ch, int cmd, char *arg)
{

	int lv_num, lv_stat, lv_button;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_ch, *next_ch;
	struct obj_data *lv_obj, *race_button, *class_button, *level_button, *tmp_obj, *next_obj;



	if (ch->in_room < 1) {
		return (FALSE);
	}

	if (cmd != CMD_PUSH) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	if (!*buf) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, buf,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;
	if (lv_num != OBJ_XANTH_BUTTON_BLACK &&
	    lv_num != OBJ_XANTH_BUTTON_RED &&
	    lv_num != OBJ_XANTH_BUTTON_GENERIC_ONE &&
	    lv_num != OBJ_XANTH_BUTTON_GENERIC_TWO &&
	    lv_num != OBJ_XANTH_BUTTON_GENERIC_THREE) {
		return (FALSE);
	}

	if (cmd == CMD_PULL) {
		send_to_char("The button resist being pulled.\r\n", ch);
		return (TRUE);
	}

	lv_stat = BUTTON_OUT_LEVER_DOWN;
	if (lv_obj->description) {
		if (strlen(lv_obj->description) < 3) {
			lv_stat = BUTTON_OUT_LEVER_DOWN;
		}
		else {
			sprintf(buf, lv_obj->description +
				strlen(lv_obj->description) - 3);
			/* COVER FOR button IN and LEVER up */
			if (strncmp(buf, "in", 2) &&
			    strncmp(buf, "up", 2))
				lv_stat = BUTTON_OUT_LEVER_DOWN;
			else
				lv_stat = BUTTON_IN_LEVER_UP;
		}
	}			/* END OF we have a description */

	/* FIND RACE/CLASS BUTTON */
	race_button = 0;
	class_button = 0;
	level_button = 0;
	for (tmp_obj = world[ch->in_room].contents; tmp_obj;
	     tmp_obj = next_obj) {
		next_obj = tmp_obj->next_content;
		if (obj_index[tmp_obj->item_number].virtual ==
		    OBJ_XANTH_BUTTON_GENERIC_THREE) {
			race_button = tmp_obj;
		}
		if (obj_index[tmp_obj->item_number].virtual ==
		    OBJ_XANTH_BUTTON_GENERIC_ONE) {
			class_button = tmp_obj;
		}
		if (obj_index[tmp_obj->item_number].virtual ==
		    OBJ_XANTH_BUTTON_GENERIC_TWO) {
			level_button = tmp_obj;
		}
	}

	if (!race_button) {
		send_to_char("Can't find the race button so nothing happens.\r\n", ch);
		return (TRUE);
	}

	if (!class_button) {
		send_to_char("Can't find the class button so nothing happens.\r\n", ch);
		return (TRUE);
	}
	if (!level_button) {
		send_to_char("Can't find the level button so nothing happens.\r\n", ch);
		return (TRUE);
	}

	/* ACTIVATE BUTTON */
	if (lv_num == OBJ_XANTH_BUTTON_RED) {
		send_to_char("You push the red button.\r\n", ch);
		act("$n pushes the red button", TRUE, ch, 0, 0, TO_ROOM);
		if (race_button->obj_flags.value[1] == 0 &&
		    class_button->obj_flags.value[1] == 0 &&
		    level_button->obj_flags.value[1] == 0) {
			return (TRUE);
		}
		/* IF WE GOT THE RIGHT RACE/CLASS, OPEN PORTAL */
		if (race_button->obj_flags.value[1] == GET_RACE(ch) &&
		    class_button->obj_flags.value[1] == GET_CLASS(ch) &&
		    level_button->obj_flags.value[1] == GET_LEVEL(ch)) {
			if (ha5000_manually_open_door(ch->in_room, CMD_EAST - 1, BIT0)) {
				co3300_send_to_room("You hear a grating sound.  The eastern door opens.\r\n", ch->in_room);
				return (TRUE);
			}
		}
		else {
			/* WE DIDN'T GET IT RIGHT, SPREAD EM AROUND THE WORLD */
			co3300_send_to_room("The room flickers around you.", ch->in_room);
			for (tmp_ch = world[ch->in_room].people; tmp_ch; tmp_ch = next_ch) {
				next_ch = tmp_ch->next_in_room;
				if (GET_LEVEL(tmp_ch) < IMO_IMM) {
					spell_elsewhere(15, tmp_ch, tmp_ch, 0);
				}
			}	/* END OF for loop */
		}
		/* RESET THE BUTTONS */
		race_button->obj_flags.value[1] = 0;
		class_button->obj_flags.value[1] = 0;
		level_button->obj_flags.value[1] = 0;
	}

	/* RESET BUTTON */
	if (lv_num == OBJ_XANTH_BUTTON_BLACK) {
		send_to_char("You push the black button.\r\n", ch);
		act("$n pushes the black button.", TRUE, ch, 0, 0, TO_ROOM);
		if (race_button->obj_flags.value[1]) {
			co3300_send_to_room("The green button pops out.\r\n",
					    ch->in_room);
			race_button->obj_flags.value[1] = 0;
		}
		if (class_button->obj_flags.value[1]) {
			co3300_send_to_room("The blue button pops out.\r\n",
					    ch->in_room);
			class_button->obj_flags.value[1] = 0;
		}
		if (level_button->obj_flags.value[1]) {
			co3300_send_to_room("The purple button pops out.\r\n",
					    ch->in_room);
			level_button->obj_flags.value[1] = 0;
		}
		if (ha5100_manually_close_door(ch->in_room, CMD_EAST - 1, BIT0)) {
			send_to_char("There is a grating sound and the east door closes.\r\n", ch);
		}
	}			/* END OF RESET BUTTON */

	/* RACE BUTTON */
	if (lv_num == OBJ_XANTH_BUTTON_GENERIC_THREE) {
		for (; *arg && *arg == ' '; arg++);
		if (!*arg ||
		    !is_number(arg)) {
			send_to_char("Which button do you want to press?\r\n", ch);
			send_to_char("Try PUSH green <#>.\r\n", ch);
			return (TRUE);
		}

		lv_button = atoi(arg);
		if (lv_button < 1 ||
		    lv_button > race_button->obj_flags.value[0]) {
			send_to_char("That is an invalid button.\r\n", ch);
			return (TRUE);
		}

		if (lv_button == race_button->obj_flags.value[1]) {
			send_to_char("Its already pushed in.\r\n", ch);
			act("$n tries to push a green button but its already pushed.", TRUE, ch, 0, 0, TO_ROOM);
			return (TRUE);
		}

		sprintf(buf, "You push green button #%d in.\r\n",
			lv_button);
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes green button #%d in.\r\n",
			lv_button);
		act(buf, TRUE, ch, 0, 0, TO_ROOM);

		if (race_button->obj_flags.value[1]) {
			sprintf(buf, "Green button #%d pops out.\r\n",
				race_button->obj_flags.value[1]);

			co3300_send_to_room(buf, ch->in_room);
		}
		race_button->obj_flags.value[1] = lv_button;
	}			/* END OF PUSH RACE BUTTON */


	/* CLASS BUTTON */
	if (lv_num == OBJ_XANTH_BUTTON_GENERIC_ONE) {
		for (; *arg && *arg == ' '; arg++);
		if (!*arg ||
		    !is_number(arg)) {
			send_to_char("Which button do you want to press?\r\n", ch);
			send_to_char("Try PUSH blue <#>.\r\n", ch);
			return (TRUE);
		}

		lv_button = atoi(arg);
		if (lv_button < 1 ||
		    lv_button > class_button->obj_flags.value[0]) {
			send_to_char("That is an invalid button.\r\n", ch);
			return (TRUE);
		}

		if (lv_button == class_button->obj_flags.value[1]) {
			send_to_char("Its already pushed in.\r\n", ch);
			act("$n tries to push a blue button but its already pushed.", TRUE, ch, 0, 0, TO_ROOM);
			return (TRUE);
		}

		sprintf(buf, "You push blue button #%d in.\r\n",
			lv_button);
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes blue button #%d in.\r\n",
			lv_button);
		act(buf, TRUE, ch, 0, 0, TO_ROOM);

		if (class_button->obj_flags.value[1]) {
			sprintf(buf, "Blue button #%d pops out.\r\n",
				class_button->obj_flags.value[1]);

			co3300_send_to_room(buf, ch->in_room);
		}
		class_button->obj_flags.value[1] = lv_button;

	}			/* END OF PUSH CLASS BUTTON */

	/* LEVEL BUTTON */
	if (lv_num == OBJ_XANTH_BUTTON_GENERIC_TWO) {
		for (; *arg && *arg == ' '; arg++);
		if (!*arg ||
		    !is_number(arg)) {
			send_to_char("Which button do you want to press?\r\n", ch);
			send_to_char("Try PUSH purple <#>.\r\n", ch);
			return (TRUE);
		}

		lv_button = atoi(arg);
		if (lv_button < 1 ||
		    lv_button > level_button->obj_flags.value[0]) {
			send_to_char("That is an invalid button.\r\n", ch);
			return (TRUE);
		}

		if (lv_button == level_button->obj_flags.value[1]) {
			send_to_char("Its already pushed in.\r\n", ch);
			act("$n tries to push a purple button but its already pushed.", TRUE, ch, 0, 0, TO_ROOM);
			return (TRUE);
		}

		sprintf(buf, "You push purple button #%d in.\r\n",
			lv_button);
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes purple button #%d in.\r\n",
			lv_button);
		act(buf, TRUE, ch, 0, 0, TO_ROOM);

		if (level_button->obj_flags.value[1]) {
			sprintf(buf, "Purple button #%d pops out.\r\n",
				level_button->obj_flags.value[1]);

			co3300_send_to_room(buf, ch->in_room);
		}
		level_button->obj_flags.value[1] = lv_button;

	}			/* END OF PUSH CLASS BUTTON */


	/* UPDATE STATUS OF OLD RACE BUTTONS */
	if (race_button->obj_flags.value[1]) {
		sprintf(buf, "There are %d green buttons mounted in the wall.  Button %d is pushed in.",
			race_button->obj_flags.value[0],
			race_button->obj_flags.value[1]);
	}
	else {
		sprintf(buf, "There are %d green buttons mounted in the wall.  They are all out.",
			race_button->obj_flags.value[0]);
	}
	if (race_button->description)
		free(race_button->description);
	race_button->description = strdup(buf);


	/* UPDATE STATUS OF OLD CLASS BUTTONS */
	if (class_button->obj_flags.value[1]) {
		sprintf(buf, "There are %d blue buttons mounted in the wall.  Button %d is pushed in.",
			class_button->obj_flags.value[0],
			class_button->obj_flags.value[1]);
	}
	else {
		sprintf(buf, "There are %d blue buttons mounted in the wall.  They are all out.",
			class_button->obj_flags.value[0]);
	}

	if (class_button->description)
		free(class_button->description);
	class_button->description = strdup(buf);

	/* UPDATE STATUS OF OLD LEVEL BUTTONS */
	if (level_button->obj_flags.value[1]) {
		sprintf(buf, "There are %d purple buttons mounted in the wall.  Button %d is pushed in.",
			level_button->obj_flags.value[0],
			level_button->obj_flags.value[1]);
	}
	else {
		sprintf(buf, "There are %d purple buttons mounted in the wall.  They are all out.",
			level_button->obj_flags.value[0]);
	}

	if (level_button->description)
		free(level_button->description);
	level_button->description = strdup(buf);

	return (TRUE);

}				/* END OF xn3900_xanth_race_class_room() */


int xn4000_xanth_outside_mousevator(struct char_data * ch, int cmd, char *arg)
{

	int lv_stat, lv_num;;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;



	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (cmd != CMD_PUSH &&
	    cmd != CMD_PULL) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	if (!*arg) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;

	if (lv_num == OBJ_XANTH_BUTTON_BLACK) {
		send_to_char("You push the black button.\r\n", ch);
		act("$n pushes the black button.", TRUE, ch, 0, 0, TO_ROOM);
		if (ha5100_manually_close_door(ch->in_room, CMD_WEST - 1, BIT0)) {
			co3300_send_to_room("You hear a grating sound.  The western door closes.\r\n", ch->in_room);
		}
		return (TRUE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;
	if (lv_num == OBJ_XANTH_BUTTON_RED) {
		send_to_char("You push the red button.\r\n", ch);
		act("$n pushes the red button.", TRUE, ch, 0, 0, TO_ROOM);
		if (ha5000_manually_open_door(ch->in_room, CMD_WEST - 1, BIT0)) {
			co3300_send_to_room("You hear a grating sound.  The western door opens.\r\n", ch->in_room);
		}
		return (TRUE);
	}

	if (lv_num != OBJ_XANTH_LEVER) {
		return (FALSE);
	}
	lv_stat = BUTTON_OUT_LEVER_DOWN;
	if (lv_obj->description) {
		if (strlen(lv_obj->description) < 3) {
			lv_stat = BUTTON_OUT_LEVER_DOWN;
		}
		else {
			sprintf(buf, lv_obj->description +
				strlen(lv_obj->description) - 3);
			/* COVER FOR button IN and LEVER up */
			if (strncmp(buf, "up", 2))
				lv_stat = BUTTON_OUT_LEVER_DOWN;
			else
				lv_stat = BUTTON_IN_LEVER_UP;
		}
	}			/* END OF we have a description */

	/* USER IS PULLING */
	if (cmd == CMD_PULL) {
		if (lv_stat == BUTTON_IN_LEVER_UP) {
			sprintf(buf, "You pull the lever down.\r\n");
			send_to_char(buf, ch);
			act("$n pulls the lever down.", TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "a lever protrudes from the wall.  The lever is down.");
			if (lv_obj->description) {
				free(lv_obj->description);
			}
			lv_obj->description = strdup(buf);

			/* PULL LEVER DOWN IN ENTRY */
			if (world[ch->in_room].number == XANTH_MOUSEVATOR_ENTR) {
				xn4050_xanth_elevator_doors(XANTH_MOUSEVATOR_ENTR,
							XANTH_MOUSEVATOR_EXIT,
							XANTH_MOUSEVATOR_ROOM,
							    CMD_SOUTH - 1,
							    CMD_NORTH - 1,
							    "down",
							    BIT1);
			}	/* END OF XANTH_MOUSEVATOR_ENTR */
			else {
				xn4050_xanth_elevator_doors(XANTH_MOUSEVATOR_ENTR,
							XANTH_MOUSEVATOR_EXIT,
							XANTH_MOUSEVATOR_ROOM,
							    CMD_SOUTH - 1,
							    CMD_NORTH - 1,
							    "down",
							    BIT0);
			}	/* END OF else XANTH_MOUSEVATOR_EXIT */
		}		/* END OF PULL LEVER DOWN when ITS UP */
		else {
			send_to_char("Its already pulled down.\r\n", ch);
			act("$n tries to pull the lever lower, but fails.",
			    TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	/* PUSH LEVER */
	if (lv_stat == BUTTON_IN_LEVER_UP) {
		send_to_char("The lever is already pushed up.", ch);
		act("$n tries to push the lever higher, but fails.",
		    TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		sprintf(buf, "You push the lever up.\r\n");
		send_to_char(buf, ch);
		sprintf(buf, "$n pushes the lever up.\r\n");
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "a lever protrudes from the wall.  The lever is up.");
		if (lv_obj->description) {
			free(lv_obj->description);
		}
		lv_obj->description = strdup(buf);

		/* PUSH LEVER UP IN ENTRY */
		if (world[ch->in_room].number == XANTH_MOUSEVATOR_ENTR) {
			xn4050_xanth_elevator_doors(XANTH_MOUSEVATOR_EXIT,
						    XANTH_MOUSEVATOR_ENTR,
						    XANTH_MOUSEVATOR_ROOM,
						    CMD_NORTH - 1,
						    CMD_SOUTH - 1,
						    "up",
						    BIT0);
		}		/* END OF XANTH_MOUSEVATOR_ENTR */
		else {
			xn4050_xanth_elevator_doors(XANTH_MOUSEVATOR_EXIT,
						    XANTH_MOUSEVATOR_ENTR,
						    XANTH_MOUSEVATOR_ROOM,
						    CMD_NORTH - 1,
						    CMD_SOUTH - 1,
						    "up",
						    BIT1);
		}		/* END OF else XANTH_MOUSEVATOR_EXIT */
	}

	return (TRUE);

}				/* END OF xn4000_xanth_outside_mousevator() */


/* IF BIT0 IS SET, THE LEVER IS FLIPPED IN ENTRY ROOM */
/*                 OTHERWISE, ITS DONE IN EXIT ROOM.  */
/* IF BIT1 IS SET, THE LEVER MSG IS PRINTED IN OTHER ROOM */
int xn4050_xanth_elevator_doors(int lv_enter_room_in,
		   int lv_exit_room_in, int lv_elev_room_in, int lv_enter_dir,
			     int lv_exit_dir, char *lv_lever_dir, int lv_flag)
{

	int lv_enter_room, lv_exit_room, lv_elev_room;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *tmp_obj, *next_obj;



	/* GET ROOM FOR ENTRANCE */
	lv_enter_room = db8000_real_room(lv_enter_room_in);
	if (lv_enter_room < 1) {
		return (FALSE);
	}

	/* GET ROOM FOR ELEVATOR */
	lv_elev_room = db8000_real_room(lv_elev_room_in);
	if (lv_elev_room < 1) {
		return (FALSE);
	}

	/* GET ROOM FOR EXIT */
	lv_exit_room = db8000_real_room(lv_exit_room_in);
	if (lv_exit_room < 1) {
		return (FALSE);
	}

	/* THE LEVER STARTS THIS SO DO IT IN THE OTHER ROOM */
	if (is_abbrev(lv_lever_dir, "up"))
		sprintf(buf, "The lever shifts to an %sward position.\r\n", lv_lever_dir);
	else
		sprintf(buf, "The lever shifts to a %sward position.\r\n", lv_lever_dir);

	if (IS_SET(lv_flag, BIT0)) {
		co3300_send_to_room(buf, lv_enter_room);
	}
	if (IS_SET(lv_flag, BIT1)) {
		co3300_send_to_room(buf, lv_exit_room);
	}

	/* CLOSE DOOR IN ENTRY */
	if (ha5100_manually_close_door(lv_enter_room, lv_enter_dir, BIT0 | BIT1)) {
		co3300_send_to_room("The elevator door slides shut.\r\n", lv_enter_room);
		co3300_send_to_room("The elevator door slides shut.\r\n", lv_elev_room);
	}

	/* LET FOLKS IN ENTRY/EXIT HEAR VATOR */
	co3300_send_to_room("You hear a humming sound.\r\n", lv_enter_room);

	sprintf(buf, "You hear a humming sound and feel %sward motion.\r\n", lv_lever_dir);
	co3300_send_to_room(buf, lv_elev_room);
	co3300_send_to_room("You hear a humming sound.\r\n", lv_exit_room);

	/* OPEN EXIT DOOR */
	if (ha5000_manually_open_door(lv_exit_room, lv_exit_dir, BIT0 | BIT1)) {
		co3300_send_to_room("The elevator door slides open.\r\n", lv_exit_room);
		co3300_send_to_room("The elevator door slides open.\r\n", lv_elev_room);
	}

	/* CHANGE LEVER HANDLE IN OTHER ROOM */
	if (IS_SET(lv_flag, BIT0)) {
		for (tmp_obj = world[lv_enter_room].contents; tmp_obj;
		     tmp_obj = next_obj) {
			next_obj = tmp_obj->next_content;
			if (obj_index[tmp_obj->item_number].virtual ==
			    OBJ_XANTH_LEVER) {
				sprintf(buf, "a lever protrudes from the wall.  The lever is %s.",
					lv_lever_dir);
				if (tmp_obj->description)
					free(tmp_obj->description);
				tmp_obj->description = strdup(buf);
			}
		}		/* END OF for LOOP */
	}			/* END OF BIT0 */

	if (IS_SET(lv_flag, BIT1)) {
		for (tmp_obj = world[lv_exit_room].contents; tmp_obj;
		     tmp_obj = next_obj) {
			next_obj = tmp_obj->next_content;
			if (obj_index[tmp_obj->item_number].virtual ==
			    OBJ_XANTH_LEVER) {
				sprintf(buf, "a lever protrudes from the wall.  The lever is %s.",
					lv_lever_dir);
				if (tmp_obj->description)
					free(tmp_obj->description);
				tmp_obj->description = strdup(buf);
			}
		}		/* END OF for LOOP */
	}			/* END OF BIT1 */

	return (TRUE);
}				/* END OF xn4050_xanth_elevator_doors() */


int xn4100_xanth_inside_mousevator(struct char_data * ch, int cmd, char *arg)
{

	int lv_num;
	struct obj_data *lv_obj;



	if (cmd != CMD_PUSH) {
		return (FALSE);
	}

	for (; *arg && *arg == ' '; arg++);
	if (!*arg) {
		if (cmd == CMD_PUSH) {
			send_to_char("You push against the air and nothing happens.\r\n", ch);
			act("$n pushes air around.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You pull on your chin, deep in thought.\r\n", ch);
			act("$n pulls on $s chin in deap thought.", TRUE, ch, 0, 0, TO_ROOM);
		}
		return (TRUE);
	}

	lv_obj = ha2075_get_obj_list_vis(ch, arg,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		return (FALSE);
	}

	lv_num = obj_index[lv_obj->item_number].virtual;

	if (lv_num == OBJ_XANTH_BUTTON_GRAY) {
		send_to_char("You press the gray button.\r\n", ch);
		act("$n pushes the gray button.", TRUE, ch, 0, 0, TO_ROOM);

		/* ARE WE ON THE FIRST FLOOR? */
		if (EXIT(ch, CMD_NORTH - 1) &&
		    IS_SET(EXIT(ch, CMD_NORTH - 1)->exit_info, EX_CLOSED)) {
			xn4050_xanth_elevator_doors(XANTH_MOUSEVATOR_EXIT,
						    XANTH_MOUSEVATOR_ENTR,
						    XANTH_MOUSEVATOR_ROOM,
						    CMD_NORTH - 1,
						    CMD_SOUTH - 1,
						    "up",
						    BIT0 | BIT1);
		}
		return (TRUE);
	}

	if (lv_num == OBJ_XANTH_BUTTON_BLUE) {
		send_to_char("You press the blue button.\r\n", ch);
		act("$n pushes the blue button.", TRUE, ch, 0, 0, TO_ROOM);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF xn4100_xanth_inside_mousevator() */


int xn4200_xanth_sabrina(struct char_data * ch, int cmd, char *arg)
{

	static int lv_number;



	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 14) != 1) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 20) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "You must seek out Humphrey.", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "I hope they aren't attacking Justin Tree!", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "Only the Good Magician Humphrey can help!", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "You have to rescue Justin!", CMD_SAY);
		return (1);
	case 5:
		do_say(ch, "Humphrey knows a hundred spells.  Maybe one of them--", CMD_SAY);
		return (1);
	case 6:
		do_say(ch, "You must seek he source of magic.", CMD_SAY);
		return (1);
	case 7:
		do_say(ch, "Trent is gone now, but his evil works remain", CMD_SAY);
		return (1);
	default:
		return (0);
	}			/* END OF switch() */

}				/* END OF xn4200_xanth_sabrina() */


int xn4300_xanth_pine_grove(struct char_data * ch, int cmd, char *arg)
{

	int lv_number;



	/* IMMORTALS ARE IMMUNE */
	if (GET_LEVEL(ch) > PK_LEV) {
		return (FALSE);
	}

	/* ONLY CERTAIN POSITIONS ARE SUBJECT TO THIS ROOM */
	if (GET_POS(ch) < POSITION_RESTING) {
		return (FALSE);
	}
	if (GET_POS(ch) == POSITION_FIGHTING) {
		return (FALSE);
	}

	/* IF CHAR TRIES TO SLEEP HERE, CAST SLEEP SPELL ON THEM */
	if (cmd == CMD_SLEEP) {
		xn4350_xanth_make_char_sleep(ch, 0);
		return (TRUE);
	}			/* END OF cmd == SLEEP */

	/* THERE IS A RANDOM CHANCE THIS CHAR WILL SLEEP */
	/* IF SITTING OR RESTING, YOUR CHANCES GO UP     */
	lv_number = 2;
	if (GET_POS(ch) == POSITION_SITTING) {
		lv_number = 3;
	}
	if (GET_POS(ch) == POSITION_RESTING) {
		lv_number = 6;
	}

	if (number(0, 10) < lv_number) {
		xn4350_xanth_make_char_sleep(ch, 0);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF xn4300_xanth_pine_grove() */


void xn4350_xanth_make_char_sleep(struct char_data * ch, int lv_flag)
{

	struct affected_type af;



	if (IS_AFFECTED(ch, AFF_MAGIC_IMMUNE)) {
		return;
	}

	if (IS_AFFECTED(ch, AFF_MAGIC_RESIST) &&
	    (number(0, 101) < GET_LEVEL(ch) + 25)) {
		send_to_char("You feel sleepy.\r\n", ch);
		return;
	}

	act("You feel very sleepy ..... zzzzzz", FALSE, ch, 0, 0, TO_CHAR);
	act("$n silently falls asleep.", TRUE, ch, 0, 0, TO_ROOM);

	GET_POS(ch) = POSITION_SLEEPING;

	af.type = SPELL_SLEEP;
	if (world[ch->in_room].number > 8199 && world[ch->in_room].number < 8350) {
				af.duration = number(1, 6);
	}
	else {
		af.duration = number(20, 30);
	}
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_SLEEP;
	ha1400_affect_join(ch, &af, FALSE, FALSE);

	act("You feel very sleepy ..... zzzzzz", FALSE, ch, 0, 0, TO_CHAR);
	act("$n suddenly falls asleep.", TRUE, ch, 0, 0, TO_ROOM);

	return;

}				/* END OF xn4350_xanth_make_char_sleep() */


/* FOLLOWING ROUTINE MAKES A DEX CHECK AND IF YOU FAIL, DROPS YOU IN */
/* THE CREVICE */
int xn4400_xanth_crevice(struct char_data * ch, int cmd, char *arg)
{

	int lv_to_room, lv_plank_num, lv_is_plank_in_room;



	/* ONLY CHECK WHEN LEAVING THE ROOM */
	if (cmd != CMD_NORTH &&
	    cmd != CMD_SOUTH) {
		return (FALSE);
	}

	/* IMMORTALS ARE IMMUNE */
	if (GET_LEVEL(ch) > PK_LEV) {
		return (FALSE);
	}

	/* IS THE PLANK IN THE ROOM? */
	lv_plank_num = db8200_real_object(OBJ_XANTH_PLANK);
	if (lv_plank_num > 0 &&
	    ha2025_get_obj_list_num(lv_plank_num,
				    world[ch->in_room].contents))
		lv_is_plank_in_room = TRUE;
	else
		lv_is_plank_in_room = FALSE;

	/* DEX CHECK */
	if (number(1, 16) > GET_DEX(ch) &&
	    lv_is_plank_in_room == FALSE) {
		lv_to_room = db8000_real_room(XANTH_CREVICE_ROOM);
		if (lv_to_room < 1) {
			send_to_char("Hrmm good thing that crevice doesn't exist.\r\n", ch);
			return (FALSE);
		}
		send_to_char("You stumble and fall into the crevice.\r\n", ch);
		act("$n stumbles and falls into the crevice.\r\n",
		    TRUE, ch, 0, 0, TO_ROOM);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, lv_to_room);
		act("$n almost falls on top of you.\r\n",
		    TRUE, ch, 0, 0, TO_ROOM);
		return (TRUE);
	}

	return (FALSE);

}				/* END OF xn4400_xanth_crevice() */


int xn4500_xanth_pandoras_box(struct char_data * ch, int cmd, char *arg)
{

	int lv_wraith_count, lv_mob_nr, lv_new_wraith;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *obj;
	struct char_data *tmp_ch, *next_ch, *lv_mob;



	if (!cmd)
		return (FALSE);

	/* DON'T DO ANYTHING FOR IMMORTS */
	if (GET_LEVEL(ch) > PK_LEV) {
		return (FALSE);
	}

	arg = one_argument(arg, buf);

	if (!*buf) {
		return (FALSE);
	}

	if (!ha3500_generic_find(buf, FIND_OBJ_ROOM, ch, &lv_mob, &obj)) {
		return (FALSE);
	}

	/* IF NOT PANDORA'S BOX, GET OUT */
	if (obj->item_number < 0 ||
		obj_index[obj->item_number].virtual != OBJ_XANTH_PANDORAS_BOX) {
		return (FALSE);
	}

	/* IF LOCKED, DON'T DO ANYTHING */
	if (IS_SET(obj->obj_flags.value[1], CONT_LOCKED)) {
		return (FALSE);
	}

	/* IF ITS OPEN, DON'T DO ANYTHING */
	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSED)) {
		return (FALSE);
	}

	REMOVE_BIT(obj->obj_flags.value[1], CONT_CLOSED);
	act("you open $p!", FALSE, ch, obj, 0, TO_CHAR);
	act("$n opens $p.", FALSE, ch, obj, 0, TO_ROOM);

	/* MAKE SURE ROOM IS VALID */
	if (obj->in_room < 0 || obj->in_room > top_of_world) {
		return (FALSE);
	}

	/* COUNT WRAITHS IN ROOM */
	lv_wraith_count = 0;
	for (tmp_ch = world[obj->in_room].people; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;
		if (mob_index[tmp_ch->nr].virtual == MOB_XANTH_WRAITH) {
			lv_wraith_count++;
		}
	}

	lv_mob_nr = db8100_real_mobile(MOB_XANTH_WRAITH);
	if (lv_mob_nr < 0) {
		send_to_char("Hrmmm - missing the mobs that should be here.\r\n", ch);
		return (FALSE);
	}

	/* MAKE MOB COME OUT AN ATTACK OPENER */
	send_to_char("A ghostly form floats out of the box and attacks YOU!\r\n", ch);
	act("As $n opens $p, a ghostly form floats out and attacks!",
	    TRUE, ch, obj, 0, TO_ROOM);

	lv_mob = db5000_read_mobile(lv_mob_nr, REAL);
	GET_START(lv_mob) = world[obj->in_room].number;
	ha1600_char_to_room(lv_mob, obj->in_room);
	lv_wraith_count++;
	hit(lv_mob, ch, TYPE_UNDEFINED);

	/* SEARCH THROUGH ROOM LOOKING FOR PEOPLE */
	tmp_ch = world[obj->in_room].people;
	while (tmp_ch && lv_wraith_count < MAX_WRAITHS) {
		next_ch = tmp_ch->next_in_room;

		lv_new_wraith = TRUE;
		if (GET_LEVEL(tmp_ch) > PK_LEV) {
			lv_new_wraith = FALSE;
		}
		if (IS_NPC(tmp_ch) && !IS_AFFECTED(tmp_ch, AFF_CHARM)) {
			lv_new_wraith = FALSE;
		}

		if (lv_new_wraith == TRUE) {
			/* LOAD A MOB AND START IT FIGHTING */
			lv_mob = db5000_read_mobile(lv_mob_nr, REAL);
			GET_START(lv_mob) = world[obj->in_room].number;
			ha1600_char_to_room(lv_mob, obj->in_room);

			send_to_char("A ghostly form floats from the box and attacks YOU!\r\n",
				     tmp_ch);
			act("A ghostly forms floats out of the box and attacks $n!",
			    TRUE, tmp_ch, obj, 0, TO_ROOM);

			lv_wraith_count++;
			hit(lv_mob, tmp_ch, TYPE_UNDEFINED);
		}		/* END OF NEW MOB */
		tmp_ch = next_ch;
	}
	return (TRUE);

}				/* END OF xn4500_xanth_pandoras_box() */

void xn4550_xanth_load_more_wraiths(struct obj_data * lv_obj, int lv_flag)
{

	int lv_wraith_count, lv_mob_nr, lv_new_wraith;
	struct char_data *tmp_ch, *next_ch, *lv_mob;



	/* MAKE SURE ROOM IS VALID */
	if (lv_obj->in_room < 0 || lv_obj->in_room > top_of_world) {
		return;
	}

	/* COUNT WRAITHS IN ROOM */
	lv_wraith_count = 0;
	for (tmp_ch = world[lv_obj->in_room].people;
	     tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next_in_room;
		if (mob_index[tmp_ch->nr].virtual == MOB_XANTH_WRAITH) {
			lv_wraith_count++;
		}
	}

	/* IF THERE ARE NO WRAITHS, EXIT */
	if (lv_wraith_count < 1) {
		return;
	}

	lv_mob_nr = db8100_real_mobile(MOB_XANTH_WRAITH);
	if (lv_mob_nr < 0) {
		return;
	}

	/* SEARCH THROUGH ROOM LOOKING FOR PEOPLE */
	tmp_ch = world[lv_obj->in_room].people;
	while (tmp_ch && lv_wraith_count < MAX_WRAITHS) {
		next_ch = tmp_ch->next_in_room;

		lv_new_wraith = TRUE;
		if (GET_LEVEL(tmp_ch) > PK_LEV) {
			lv_new_wraith = FALSE;
		}
		if (IS_NPC(tmp_ch) && !IS_AFFECTED(tmp_ch, AFF_CHARM)) {
			lv_new_wraith = FALSE;
		}

		if (lv_new_wraith == TRUE) {
			/* LOAD A MOB AND START IT FIGHTING */
			lv_mob = db5000_read_mobile(lv_mob_nr, REAL);
			GET_START(lv_mob) = world[lv_obj->in_room].number;
			ha1600_char_to_room(lv_mob, lv_obj->in_room);
			lv_wraith_count++;
			send_to_char("A ghostly form floats from the box and attacks YOU!\r\n",
				     tmp_ch);
			act("A ghostly forms floats out of the box and attacks $n!",
			    TRUE, tmp_ch, lv_obj, 0, TO_ROOM);

			/* START MOB FIGHTING */
			hit(lv_mob, tmp_ch, TYPE_UNDEFINED);
		}		/* END OF NEW MOB */
		tmp_ch = next_ch;
	}

	return;

}				/* END OF xn4550_xanth_load_more_wraiths() */


/* IF PERSON IS LOOKING WEST, WE WANT TO SHOW THE JAIL */
int xn4600_xanth_outer_guard_room(struct char_data * ch, int cmd, char *arg)
{
	int lv_real_room, lv_was_in_room;
	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_LOOK) {
		return (FALSE);
	}

	if (ch->in_room < 1) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	if (!is_abbrev(arg, "west")) {
		return (FALSE);
	}

	lv_real_room = db8000_real_room(XANTH_OUTER_JAIL_CELL);
	if (lv_real_room < 0) {
		sprintf(buf, "ERROR: Guard room %d in xn4600 is invalid.",
			XANTH_OUTER_JAIL_CELL);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* TEMPORARILY PUT CHAR IN OTHER ROOM */
	lv_was_in_room = ch->in_room;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_real_room);

	/* SHOW JAIL CELL */
	send_to_char("Looking through the gate, you see:\r\n", ch);
	in3000_do_look(ch, "\0", 0);

	/* RETURN CHAR TO ORIGINAL ROOM */
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_was_in_room);

	return (TRUE);

}				/* END OF xn4600_xanth_outer_guard_room() */


/* IF PERSON IS LOOKING EAST, WE WANT TO SHOW GUARD ROOM */
int xn4700_xanth_outer_jail_cell(struct char_data * ch, int cmd, char *arg)
{
	int lv_real_room, lv_was_in_room;
	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_LOOK) {
		return (FALSE);
	}

	if (ch->in_room < 1) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	if (!is_abbrev(arg, "east")) {
		return (FALSE);
	}

	lv_real_room = db8000_real_room(XANTH_OUTER_GUARD_ROOM);
	if (lv_real_room < 0) {
		sprintf(buf, "ERROR: jail cell: %d in xn4700 is invalid.",
			XANTH_OUTER_GUARD_ROOM);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* TEMPORARILY PUT CHAR IN OTHER ROOM */
	lv_was_in_room = ch->in_room;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_real_room);

	/* SHOW ROOM */
	send_to_char("Looking through the gate, you see:\r\n", ch);
	in3000_do_look(ch, "\0", 0);

	/* RETURN CHAR TO ORIGINAL ROOM */
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_was_in_room);

	return (TRUE);

}				/* END OF xn4700_xanth_outer_jail_cell() */


/* THIS ROUTINE ALLOWS A PERSON TO LOOK IN A DIRECTION AND ITS AS */
/* THOUGH THEY WERE IN THE ROOM THATS IN THAT DIRECTION           */
int xn5000_xanth_shades_glasses(struct char_data * ch, int cmd, char *arg)
{

	int rc, lv_dir, lv_real_room, lv_was_in_room;
	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_LOOK) {
		return (FALSE);
	}

	/* IF WE AREN'T WEARING THE SHADES, GET OUT */
	if (!ch->equipment[WEAR_HEAD]) {
		return (FALSE);
	}
	if (obj_index[ch->equipment[WEAR_HEAD]->item_number].virtual !=
	    OBJ_XANTH_SHADES_GLASSES) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	lv_dir = old_search_block(arg, 0, strlen(arg), dirs, 0);
	if (lv_dir < 1 || lv_dir > MAX_DIRS + 1) {
		return (FALSE);
	}
	/* ADJUST FOR ARRAY */
	lv_dir--;

	/* IS THERE ISN'T AN EXIT GET OUT */
	if (!EXIT(ch, lv_dir)) {
		return (FALSE);
	}

	/* MAKE SURE ITS NOT NOWHERE */
	lv_real_room = world[ch->in_room].dir_option[lv_dir]->to_room;
	if (lv_real_room < 0) {
		return (FALSE);
	}

	if (IS_SET(EXIT(ch, lv_dir)->exit_info, EX_CLOSED)) {
		return (FALSE);
	}

	/* DOES PERSON HAVE ACCESS TO THE ROOM? */
	rc = li3000_is_blocked_from_room(ch, lv_real_room, 0);
	if (rc) {
		return (FALSE);
	}

	/* TEMPORARILY PUT CHAR IN JAIL CELL */
	lv_was_in_room = ch->in_room;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_real_room);

	/* SHOW JAIL CELL */
	sprintf(buf, "looking %s, you see:\r\n", dirs[lv_dir]);
	send_to_char(buf, ch);
	in3000_do_look(ch, "\0", 0);

	/* RETURN CHAR TO ORIGINAL ROOM */
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_was_in_room);

	return (TRUE);

}				/* END OF xn5000_xanth_shades_glasses() */


int xn5100_xanth_chars_illusion_to_isle(int lv_flag)
{

	char buf[MAX_STRING_LENGTH];
	int lv_real_to, lv_real_from, lv_rtn, lv_num;
	struct obj_data *lv_obj, *next_obj, *lv_grass_obj, *lv_rice_obj, *lv_cup_obj, *lv_panties_obj, *lv_shawl_obj;
	struct char_data *tmp_ch, *next_ch;



	/* CREATE NEW LINK TO ISLE  */
	lv_real_to = db8000_real_room(XANTH_ISLE_WEEDGROWN);
	lv_real_from = db8000_real_room(XANTH_ISLE_BEACH);
	lv_rtn = wi2920_create_room_links(lv_real_from, lv_real_to, CMD_EAST - 1, "a weedgrown field.");
	if (!lv_rtn) {
		sprintf(buf, "ERROR2 in xn5100 to: %d from: %d rtn %d",
			lv_real_to, lv_real_from, lv_rtn);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
	}			/* END OF lv_rtn error */

	xn5150_xanth_move_chars(XANTH_ISLE_MAINPATH_1, XANTH_ISLE_WEEDGROWN);
	xn5150_xanth_move_chars(XANTH_ISLE_MAINPATH_2, XANTH_ISLE_WEEDGROWN);
	xn5150_xanth_move_chars(XANTH_ISLE_DINING, XANTH_ISLE_PANTRY);
	xn5150_xanth_move_chars(XANTH_ISLE_PALACE, XANTH_ISLE_SHACK);
	xn5150_xanth_move_chars(XANTH_ISLE_BATHROOM, XANTH_ISLE_OUTHOUSE);

	lv_grass_obj = db5100_read_object(OBJ_XANTH_IRIS_GRASS, PROTOTYPE);
	if (!lv_grass_obj) {
		main_log("ERROR: unable to get xn5100 grass.\r\n");
		spec_log("ERROR: unable to get xn5100 grass.", ERROR_LOG);
		return (FALSE);
	}

	lv_cup_obj = db5100_read_object(OBJ_XANTH_IRIS_CUP_WATER, PROTOTYPE);
	if (!lv_cup_obj) {
		main_log("ERROR: unable to get xn5100 cup water.\r\n");
		spec_log("ERROR: unable to get xn5100 cup water.", ERROR_LOG);
		return (FALSE);
	}

	lv_rice_obj = db5100_read_object(OBJ_XANTH_IRIS_RICE, PROTOTYPE);
	if (!lv_rice_obj) {
		main_log("ERROR: unable to get xn5100 rice.\r\n");
		spec_log("ERROR: unable to get xn5100 rice.", ERROR_LOG);
		return (FALSE);
	}

	lv_panties_obj = db5100_read_object(OBJ_XANTH_IRIS_PANTIES, PROTOTYPE);
	if (!lv_panties_obj) {
		main_log("ERROR: unable to get xn5100 panties.\r\n");
		spec_log("ERROR: unable to get xn5100 panties.", ERROR_LOG);
		return (FALSE);
	}

	lv_shawl_obj = db5100_read_object(OBJ_XANTH_IRIS_SHAWL, PROTOTYPE);
	if (!lv_shawl_obj) {
		main_log("ERROR: unable to get xn5100 pebble.\r\n");
		spec_log("ERROR: unable to get xn5100 pebble.", ERROR_LOG);
		return (FALSE);
	}

	/* CONVERT ALL ILLUSION OBJECTS TO RICE AND WORTHLESS PEBBLE */
	for (lv_obj = object_list; lv_obj; lv_obj = next_obj) {
		next_obj = lv_obj->next;
		if (lv_obj->item_number < 0) {
			lv_num = 0;
		}
		else {
			lv_num = obj_index[lv_obj->item_number].virtual;
		}
		/* LETS WEED EM OUT WITH THE FEWEST IF STATEMENTS */
		if (lv_num > OBJ_XANTH_IRIS_END_ITEM) {
			continue;
		}
		if (lv_num < OBJ_XANTH_IRIS_BEG_ITEM) {
			continue;
		}
		/* ONLY CONVERSION OBJECTS ARE LEFT */
		if (lv_num == OBJ_XANTH_IRIS_FLOWER) {
			xn5175_xanth_convert_illusion_objects(lv_obj, lv_grass_obj);
			continue;
		}
		if (lv_num == OBJ_XANTH_IRIS_STEAK ||
		    lv_num == OBJ_XANTH_IRIS_COOKIE) {
			xn5175_xanth_convert_illusion_objects(lv_obj, lv_rice_obj);
			continue;
		}
		if (lv_num == OBJ_XANTH_IRIS_WINE) {
			xn5175_xanth_convert_illusion_objects(lv_obj, lv_cup_obj);
			continue;
		}
		if (lv_num == OBJ_XANTH_IRIS_KNICKERS) {
			xn5175_xanth_convert_illusion_objects(lv_obj, lv_panties_obj);
			continue;
		}
		if (lv_num == OBJ_XANTH_IRIS_UNIFORM) {
			xn5175_xanth_convert_illusion_objects(lv_obj, lv_shawl_obj);
			continue;
		}
	}			/* END OF for LOOP TO CONVERT ILLUSION OBJECTS */

	/* PURGE OLD ILLUSION CHARS */
	for (tmp_ch = character_list; tmp_ch; tmp_ch = next_ch) {
		next_ch = tmp_ch->next;
		if (IS_NPC(tmp_ch) &&
		    (mob_index[tmp_ch->nr].virtual == MOB_XANTH_IRIS_DRAGON ||
		   mob_index[tmp_ch->nr].virtual == MOB_XANTH_IRIS_GARDNER)) {
			wi2900_purge_items(tmp_ch, "\0", 0);
			ha3000_extract_char(tmp_ch, END_EXTRACT_BY_XANTH);
		}
	}

	/* printf("Xanth IRIS dies.\r\n"); */
	return (TRUE);

}				/* END OF xn5100_xanth_chars_illusion_to_isle() */


int xn5150_xanth_move_chars(int lv_real_from, int lv_real_to)
{

	char buf[MAX_STRING_LENGTH];
	int lv_from_room, lv_to_room;



	/* FIGURE FROM ROOM */
	lv_from_room = db8000_real_room(lv_real_from);
	if (lv_from_room < 0) {
		sprintf(buf, "ERROR: Entered xn5150 with unexpected from room: %d.",
			lv_real_from);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* FIGURE TO ROOM */
	lv_to_room = db8000_real_room(lv_real_to);
	if (lv_to_room < 0) {
		sprintf(buf, "ERROR: Entered xn5150 with unexpected from room: %d.",
			lv_real_to);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	ha1650_all_from_room_to_room(lv_from_room, lv_to_room,
		      "You suddenly find yourself in different surroundings.",
				     "You notice $n is here.",
				     0);

	return (FALSE);

}				/* END OF xn5150_xanth_move_chars() */


int xn5175_xanth_convert_illusion_objects(struct obj_data * from_obj, struct obj_data * to_obj)
{

	int idx;



	/* DECREMENT FROM COUNT */
	if (obj_index[from_obj->item_number].number > 0) {
		obj_index[from_obj->item_number].number--;
	}

	/* INCREMENT TO COUNT */
	obj_index[to_obj->item_number].number++;

	/* CHANGE NAME */
	if (from_obj->name) {
		free(from_obj->name);
	}
	from_obj->name = strdup(to_obj->name);

	/* CHANGE SHORT DESCRIPTION */
	if (from_obj->short_description) {
		free(from_obj->short_description);
	}
	from_obj->short_description = strdup(to_obj->short_description);

	/* CHANGE LONG DESCRIPTION */
	if (from_obj->description) {
		free(from_obj->description);
	}
	from_obj->description = strdup(to_obj->description);

	/* CHANGE OTHER VALUES */
	if (from_obj->action_description) {
		free(from_obj->action_description);
	}
	from_obj->action_description = 0;;
	from_obj->obj_flags = to_obj->obj_flags;
	for (idx = 0; idx < MAX_OBJ_AFFECT; idx++) {
		from_obj->affected[idx].location =
			to_obj->affected[idx].location;
		from_obj->affected[idx].modifier =
			to_obj->affected[idx].modifier;
	}			/* END OF for LOOP */

	/* CHANGE OBJECT NUMBER */
	from_obj->item_number = to_obj->item_number;

	return (TRUE);

}				/* END OF
				 * xn5175_xanth_convert_illusion_objects() */


int xn5200_xanth_chars_isle_to_illusion(int lv_flag)
{

	char buf[MAX_STRING_LENGTH];
	int lv_real_to, lv_real_from, lv_rtn;



	/* CREATE NEW LINK TO ISLE  */
	lv_real_to = db8000_real_room(XANTH_ISLE_MAINPATH_1);
	lv_real_from = db8000_real_room(XANTH_ISLE_BEACH);
	lv_rtn = wi2920_create_room_links(lv_real_from, lv_real_to, CMD_EAST - 1, "a beatiful garden.");
	if (!lv_rtn) {
		sprintf(buf, "ERROR2 in xn5200 to: %d from: %d rtn %d",
			lv_real_to, lv_real_from, lv_rtn);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
	}			/* END OF lv_rtn error */

	xn5150_xanth_move_chars(XANTH_ISLE_WEEDGROWN, XANTH_ISLE_MAINPATH_1);
	xn5150_xanth_move_chars(XANTH_ISLE_PANTRY, XANTH_ISLE_DINING);
	xn5150_xanth_move_chars(XANTH_ISLE_SHACK, XANTH_ISLE_PALACE);
	xn5150_xanth_move_chars(XANTH_ISLE_OUTHOUSE, XANTH_ISLE_BATHROOM);

	/* printf("Xanth IRIS reset.\r\n"); */
	return (FALSE);

}				/* END OF xn5100_xanth_chars_illusion_to_isle() */
