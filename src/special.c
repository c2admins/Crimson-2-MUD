/* sf */
/* gv_location: 16501-17000 */
/* *******************************************************************
*  file: special.c , Special module.                 Part of DIKUMUD *
*  Usage: Procedures handling special procedures                     *
*         for several zones.                                         *
******************************************************************** */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "constants.h"
#include "globals.h"
#include "func.h"

int sp1000_citym_rainbow_box(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj, *tmp_obj;
	struct char_data *victim;
	int lv_found_keys;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];



	if (cmd != CMD_CLOSE) {
		return (FALSE);
	}

	/* IF THEY CLOSE THE BOX AND IT CONTAINS THE CORRECT KEYS */
	/* CONVERT THEM TO THE RAINBOW KEY                        */
	argument_interpreter(arg, type, dir);

	if (!*type) {
		return (FALSE);
	}

	victim = 0;
	obj = 0;
	if (!(ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj))) {
		return (FALSE);
	}

	/* this is an object */
	if (obj->obj_flags.type_flag != ITEM_CONTAINER)
		return (FALSE);

	if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
		return (FALSE);

	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
		return (FALSE);

	if (obj_index[obj->item_number].virtual != 7646)
		return (FALSE);

	/* WE HAVE OUR BOX, DOES IT CONTAIN THE 6 KEYS? */
	lv_found_keys = 0;
	for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if (obj_index[tmp_obj->item_number].virtual == 7609)
			SET_BIT(lv_found_keys, BIT0);
		if (obj_index[tmp_obj->item_number].virtual == 7610)
			SET_BIT(lv_found_keys, BIT1);
		if (obj_index[tmp_obj->item_number].virtual == 7611)
			SET_BIT(lv_found_keys, BIT2);
		if (obj_index[tmp_obj->item_number].virtual == 7612)
			SET_BIT(lv_found_keys, BIT3);
		if (obj_index[tmp_obj->item_number].virtual == 7613)
			SET_BIT(lv_found_keys, BIT4);
		if (obj_index[tmp_obj->item_number].virtual == 7614)
			SET_BIT(lv_found_keys, BIT5);
	}			/* END OF for loop */

	/* IF WE DON'T HAVE THE RIGHT KEYS, EXIT */
	if (lv_found_keys != BIT6 - 1)
		return (FALSE);

	/* SWAP THE KEYS FOR THE RAINBOW KEY */
	lv_found_keys = db8200_real_object(7645);
	if (lv_found_keys < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Tried to load object: 7645 for %s and it doesn't exist.",
			GET_REAL_NAME(ch));
		do_sys(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* IF MAXED, DON'T DO ANYTHING */
	if (obj_index[lv_found_keys].number >=
	    obj_index[lv_found_keys].maximum) {
		return (FALSE);
	}

	/* REMOVE ORIGINAL KEYS */
	while (obj->contains) {
		GET_OBJ_WEIGHT(obj) -= GET_OBJ_WEIGHT(obj->contains);
		ha2700_extract_obj(obj->contains);
	}			/* END OF while loop */

	/* LOAD THE OBJECT */
	tmp_obj = db5100_read_object(lv_found_keys, REAL);

	/* PUT THE KEY IN THE BOX */
	ha2300_obj_to_obj(tmp_obj, obj);

	return (FALSE);

}				/* END OF rainbow_box() */

int sp1050_mithril_fountain(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj, *tmp_obj;
	struct char_data *victim;
	int lv_found_keys;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];



	if (cmd != CMD_CLOSE) {
		return (FALSE);
	}

	/* IF THEY CLOSE THE FOUNTAIN AND IT CONTAINS THE CORRECT KEYS */
	/* CONVERT THEM TO THE MITHRIL KEY                        */
	argument_interpreter(arg, type, dir);

	if (!*type) {
		return (FALSE);
	}

	victim = 0;
	obj = 0;
	if (!(ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch,
				  &victim, &obj))) {
		return (FALSE);
	}

	/* this is an object */
	if (obj->obj_flags.type_flag != ITEM_CONTAINER)
		return (FALSE);

	if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
		return (FALSE);

	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
		return (FALSE);

	if (obj_index[obj->item_number].virtual != 8661)
		return (FALSE);

	/* WE HAVE OUR FOUNTAIN, DOES IT CONTAIN THE KEYS? */
	lv_found_keys = 0;
	for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if (obj_index[tmp_obj->item_number].virtual == 8653)
			SET_BIT(lv_found_keys, BIT0);
		if (obj_index[tmp_obj->item_number].virtual == 8654)
			SET_BIT(lv_found_keys, BIT1);
		if (obj_index[tmp_obj->item_number].virtual == 8655)
			SET_BIT(lv_found_keys, BIT2);
		if (obj_index[tmp_obj->item_number].virtual == 8656)
			SET_BIT(lv_found_keys, BIT3);
		if (obj_index[tmp_obj->item_number].virtual == 8657)
			SET_BIT(lv_found_keys, BIT4);
		if (obj_index[tmp_obj->item_number].virtual == 8658)
			SET_BIT(lv_found_keys, BIT5);
		if (obj_index[tmp_obj->item_number].virtual == 8659)
			SET_BIT(lv_found_keys, BIT6);
	}			/* END OF for loop */


	/* IF WE DON'T HAVE THE RIGHT KEYS, EXIT */
	if (lv_found_keys != BIT7 - 1)
		return (FALSE);

	/* SWAP THE KEYS FOR THE RAINBOW KEY */
	lv_found_keys = db8200_real_object(8660);
	if (lv_found_keys < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Tried to load object: 8660 for %s and it doesn't exist.",
			GET_REAL_NAME(ch));
		do_sys(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* IF MAXED, DON'T DO ANYTHING */
	if (obj_index[lv_found_keys].number >=
	    obj_index[lv_found_keys].maximum) {
		return (FALSE);
	}

	/* REMOVE ORIGINAL KEYS */
	while (obj->contains) {
		GET_OBJ_WEIGHT(obj) -= GET_OBJ_WEIGHT(obj->contains);
		ha2700_extract_obj(obj->contains);
	}			/* END OF while loop */

	/* LOAD THE OBJECT */
	tmp_obj = db5100_read_object(lv_found_keys, REAL);

	/* PUT THE KEY IN THE FOUNTAIN */
	ha2300_obj_to_obj(tmp_obj, obj);

	return (FALSE);

}				/* END OF mithril_fountain() */

int sp1250_qelves_dias(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj, *tmp_obj;
	struct char_data *victim;
	int lv_found_runes;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];



	if (cmd != CMD_CLOSE) {
		return (FALSE);
	}

	/* IF THEY CLOSE THE DIAS AND IT CONTAINS THE CORRECT RUNESTONES CONVERT THEM TO THE DRAGON SHIELD     */
	argument_interpreter(arg, type, dir);

	if (!*type) {
		return (FALSE);
	}

	victim = 0;
	obj = 0;
	if (!(ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj))) {
		return (FALSE);
	}

	/* this is an object */
	if (obj->obj_flags.type_flag != ITEM_CONTAINER){
		return (FALSE);
	}

	if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED)){
		return (FALSE);
	}
	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE)){
		return (FALSE);
	}
	if (obj_index[obj->item_number].virtual != 20766){
		return (FALSE);
	}
	/* WE HAVE OUR DIAS, DOES IT CONTAIN THE RUNESTONES? */
	lv_found_runes = 0;
	for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if (obj_index[tmp_obj->item_number].virtual == 20759)
			SET_BIT(lv_found_runes, BIT0);
		if (obj_index[tmp_obj->item_number].virtual == 20760)
			SET_BIT(lv_found_runes, BIT1);
		if (obj_index[tmp_obj->item_number].virtual == 20761)
			SET_BIT(lv_found_runes, BIT2);
		if (obj_index[tmp_obj->item_number].virtual == 20762)
			SET_BIT(lv_found_runes, BIT3);
	}			/* END OF for loop */

	/* IF WE DON'T HAVE THE RIGHT RUNESTONES, EXIT */
	if (lv_found_runes != BIT4 - 1){
		return (FALSE);
	}
	/* SWAP THE KEYS FOR THE DRAGON SHIELD */
	lv_found_runes = db8200_real_object(20758);
	if (lv_found_runes < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Tried to load object: 20758 for %s and it doesn't exist.",
			GET_REAL_NAME(ch));
		do_sys(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* REMOVE RUNESTONES */
	while (obj->contains) {
		GET_OBJ_WEIGHT(obj) -= GET_OBJ_WEIGHT(obj->contains);
		ha2700_extract_obj(obj->contains);
	}			/* END OF while loop */

	/* LOAD THE OBJECT */
	tmp_obj = db5100_read_object(lv_found_runes, REAL);

	/* PUT THE SHIELD IN THE FOUNTAIN */
	ha2300_obj_to_obj(tmp_obj, obj);
	return (FALSE);

}				/* END OF sp1250_qelves_dias() */

//Zone doesn't exist removing function
//int sp1500_arcain_chest(struct char_data * ch, int cmd, char *arg)
//{
//
//	struct obj_data *obj, *tmp_obj;
//	struct char_data *victim;
//	int lv_found_keys;
//	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
//
//
//	if (cmd != CMD_CLOSE) {
//		return (FALSE);
//	}
//
//	/* IF THEY CLOSE THE CHEST AND IT CONTAINS THE CORRECT KEYS */
//	/* CONVERT THEM TO THE SPECIAL KEY                          */
//	argument_interpreter(arg, type, dir);
//
//	if (!*type) {
//		return (FALSE);
//	}
//
//	victim = 0;
//	obj = 0;
//	if (!(ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch,
//				  &victim, &obj))) {
//		return (FALSE);
//	}
//
//	/* this is an object */
//	if (obj->obj_flags.type_flag != ITEM_CONTAINER)
//		return (FALSE);
//
//	if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
//		return (FALSE);
//
//	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
//		return (FALSE);
//
//	if (obj_index[obj->item_number].virtual != 2704)
//		return (FALSE);
//
//	/* WE HAVE OUR CHEST, DOES IT CONTAIN THE 2 KEYS? */
//	lv_found_keys = 0;
//	for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
//		if (obj_index[tmp_obj->item_number].virtual == 2701)
//			SET_BIT(lv_found_keys, BIT0);
//		if (obj_index[tmp_obj->item_number].virtual == 2702)
//			SET_BIT(lv_found_keys, BIT1);
//	}			/* END OF for loop */
//
//	/* IF WE DON'T HAVE THE RIGHT KEYS, EXIT */
//	if (lv_found_keys != BIT2 - 1)
//		return (FALSE);
//
//	/* SWAP THE KEYS FOR THE SPECIAL KEY */
//	lv_found_keys = db8200_real_object(2703);
//	if (lv_found_keys < 0) {
//		bzero(buf, sizeof(buf));
//		sprintf(buf, "ERROR: Tried to load object: 2703 for %s and it doesn't exist.",
//			GET_REAL_NAME(ch));
//
//		do_sys(buf, GET_LEVEL(ch) + 1, ch);
//		spec_log(buf, ERROR_LOG);
//		return (FALSE);
//	}
//
//	/* IF MAXED, DON'T DO ANYTHING */
//	if (obj_index[lv_found_keys].number >=
//	    obj_index[lv_found_keys].maximum) {
//		return (FALSE);
//	}
//
//	/* REMOVE ORIGINAL KEYS */
//	while (obj->contains) {
//		GET_OBJ_WEIGHT(obj) -= GET_OBJ_WEIGHT(obj->contains);
//		ha2700_extract_obj(obj->contains);
//	}			/* END OF while loop */
//
//	/* LOAD THE OBJECT */
//	tmp_obj = db5100_read_object(lv_found_keys, REAL);
//
//	/* PUT THE KEY IN THE BOX */
//	ha2300_obj_to_obj(tmp_obj, obj);
//
//	send_to_char("If you open the chest you'll find your key!\r\n", ch);
//	return (FALSE);
//
//}				/* END OF arcain_chest() */


int sp2000_cloud_stone(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH], arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	int lv_key, lv_lock, lv_unlock, lv_beg_dir, lv_end_room, lv_end_dir, did_we_find_all_dot, lv_dest_room, r_num;
	struct char_data *tmp_ch;
	struct obj_data *obj_object, *sub_object;



	lv_key = 2127;
	lv_lock = 2126;
	lv_unlock = 2141;
	lv_beg_dir = 4;
	lv_end_room = 2107;
	lv_end_dir = 5;

	if (cmd != CMD_PUT) {
		return (FALSE);
	}

	/* REMOVE "ALL." IF ATTACHED */
	for (; *arg == ' '; arg++);
	did_we_find_all_dot = remove_ALL_dot(arg);
	if (did_we_find_all_dot == TRUE)
		arg += 4;

	/* DID WE SPECIFY WHAT TO PUT? */
	argument_interpreter(arg, arg1, arg2);
	if (!(*arg1)) {
		return (FALSE);
	}

	/* DID WE SPECIFY A CONTAINER? */
	if (!(*arg2)) {
		return (FALSE);
	}

	/* DO WE HAVE WHAT WE ARE PUTTING? */
	obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
	if (!(obj_object)) {
		return (FALSE);
	}

	/* DO WE HAVE CONTAINER? */
	tmp_ch = 0;
	sub_object = 0;
	r_num = ha3500_generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_ch, &sub_object);
	if (!(sub_object)) {
		return (FALSE);
	}

	/* IF THIS ISN'T *THE* KEY GET OUT */
	if (obj_index[obj_object->item_number].virtual != lv_key) {
		return (FALSE);
	}

	/* IF THIS ISN'T *THE* LOCK GET OUT */
	if (obj_index[sub_object->item_number].virtual != lv_lock) {
		return (FALSE);
	}

	if (ch->in_room < 0) {
		send_to_char("You're NOWHERE dude!\r\n", ch);
		return (FALSE);
	}

	/*
	       bzero(buf, sizeof(buf));
	       sprintf(buf, "Cloud stone: Cmd=%d, arg=%s.\r\n",
					       cmd, arg);
	       send_to_char(buf, ch);
	       */

	/* REMOVE THE KEY FROM INVENTORY */
	ha2700_extract_obj(obj_object);

	/* REMOVE LOCK FROM ROOM */
	ha2700_extract_obj(sub_object);

	/* LOAD THE UNLOCK OBJECT */
	r_num = db8200_real_object(lv_unlock);
	if (r_num < 0) {
		send_to_char("There is a flash and the sword/stone disappear.\r\n", ch);
		return (TRUE);
	}
	obj_object = db5100_read_object(r_num, REAL);
	ha2100_obj_to_room(obj_object, ch->in_room);

	/* GET DESTINATION ROOM */
	lv_dest_room = db8000_real_room(lv_end_room);
	if (lv_dest_room < 0) {
		sprintf(buf, "Hrmmm, can't find destination room %d!\r\n",
			lv_end_room);
		send_to_char(buf, ch);
		return (TRUE);
	}

	/* CREATE LINK TO DESTINATION ROOM */
	if (world[ch->in_room].dir_option[lv_beg_dir] > 0) {
		send_to_char("Hrmmm, the portal is already open!\r\n", ch);
		return (TRUE);
	}

	CREATE(world[ch->in_room].dir_option[lv_beg_dir],
	       struct room_direction_data, 1);
	world[ch->in_room].dir_option[lv_beg_dir]->general_description =
		strdup("the stair.\r\n");
	world[ch->in_room].dir_option[lv_beg_dir]->keyword = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->exit_info = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->key = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->to_room = lv_dest_room;

	/* CREATE LINK BACKWARD FROM ROOM */
	if (world[lv_dest_room].dir_option[lv_end_dir] > 0) {
		send_to_char("Hrmmm, the down portal is already open!\r\n", ch);
		return (TRUE);
	}

	CREATE(world[lv_dest_room].dir_option[lv_end_dir],
	       struct room_direction_data, 1);
	world[lv_dest_room].dir_option[lv_end_dir]->general_description =
		strdup("the stair.\r\n");
	world[lv_dest_room].dir_option[lv_end_dir]->keyword = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->exit_info = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->key = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->to_room = ch->in_room;

	send_to_char("You insert the sword into the stone and a stairway leading upward appears.\r\n", ch);
	act("$n inserts a sword into the stone and a stairway leading upward appears.\r\n",
	    TRUE, ch, 0, ch, TO_ROOM);
	return (TRUE);

}				/* end of sp2000_cloud_stone() */


int sp2100_cloud_bookshelf(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH], arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	int lv_key, lv_lock, lv_unlock, lv_beg_dir, lv_end_room, lv_end_dir, did_we_find_all_dot, lv_dest_room, r_num;
	struct char_data *tmp_ch;
	struct obj_data *obj_object, *sub_object;



	lv_key = 2137;
	lv_lock = 2136;
	lv_unlock = 2142;
	lv_beg_dir = 0;
	lv_end_room = 2188;
	lv_end_dir = 2;

	if (cmd != CMD_PUT) {
		return (FALSE);
	}

	/* REMOVE "ALL." IF ATTACHED */
	for (; *arg == ' '; arg++);
	did_we_find_all_dot = remove_ALL_dot(arg);
	if (did_we_find_all_dot == TRUE)
		arg += 4;

	/* DID WE SPECIFY WHAT TO PUT? */
	argument_interpreter(arg, arg1, arg2);
	if (!(*arg1)) {
		return (FALSE);
	}

	/* DID WE SPECIFY A CONTAINER? */
	if (!(*arg2)) {
		return (FALSE);
	}

	/* DO WE HAVE WHAT WE ARE PUTTING? */
	obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
	if (!(obj_object)) {
		return (FALSE);
	}

	/* DO WE HAVE CONTAINER? */
	tmp_ch = 0;
	sub_object = 0;
	r_num = ha3500_generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_ch, &sub_object);
	if (!(sub_object)) {
		return (FALSE);
	}

	/* IF THIS ISN'T *THE* KEY GET OUT */
	if (obj_index[obj_object->item_number].virtual != lv_key) {
		return (FALSE);
	}

	/* IF THIS ISN'T *THE* LOCK GET OUT */
	if (obj_index[sub_object->item_number].virtual != lv_lock) {
		return (FALSE);
	}

	if (ch->in_room < 0) {
		send_to_char("You're NOWHERE dude!\r\n", ch);
		return (FALSE);
	}

	/*
	 bzero(buf, sizeof(buf));
	 sprintf(buf, "Cloud bookshelf: Cmd=%d, arg=%s.\r\n",
					 cmd, arg);
	 send_to_char(buf, ch);
	 */

	/* REMOVE THE KEY FROM INVENTORY */
	ha2700_extract_obj(obj_object);

	/* REMOVE LOCK FROM ROOM */
	ha2700_extract_obj(sub_object);

	/* LOAD THE UNLOCK OBJECT */
	r_num = db8200_real_object(lv_unlock);
	if (r_num < 0) {
		send_to_char("There is a flash and the book/shelf disappear.\r\n", ch);
		return (TRUE);
	}
	obj_object = db5100_read_object(r_num, REAL);
	ha2100_obj_to_room(obj_object, ch->in_room);

	/* GET DESTINATION ROOM */
	lv_dest_room = db8000_real_room(lv_end_room);
	if (lv_dest_room < 0) {
		sprintf(buf, "Hrmmm, can't find destination room %d!\r\n",
			lv_end_room);
		send_to_char(buf, ch);
		return (TRUE);
	}

	/* CREATE LINK TO DESTINATION ROOM */
	if (world[ch->in_room].dir_option[lv_beg_dir] > 0) {
		send_to_char("Hrmmm, the portal is already open!\r\n", ch);
		return (TRUE);
	}

	CREATE(world[ch->in_room].dir_option[lv_beg_dir],
	       struct room_direction_data, 1);
	world[ch->in_room].dir_option[lv_beg_dir]->general_description =
		strdup("the stair.\r\n");
	world[ch->in_room].dir_option[lv_beg_dir]->keyword = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->exit_info = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->key = 0;
	world[ch->in_room].dir_option[lv_beg_dir]->to_room = lv_dest_room;

	/* CREATE LINK BACKWARD FROM ROOM */
	if (world[lv_dest_room].dir_option[lv_end_dir] > 0) {
		send_to_char("Hrmmm, the down portal is already open!\r\n", ch);
		return (TRUE);
	}

	CREATE(world[lv_dest_room].dir_option[lv_end_dir],
	       struct room_direction_data, 1);
	world[lv_dest_room].dir_option[lv_end_dir]->general_description =
		strdup("the stair.\r\n");
	world[lv_dest_room].dir_option[lv_end_dir]->keyword = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->exit_info = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->key = 0;
	world[lv_dest_room].dir_option[lv_end_dir]->to_room = ch->in_room;

	send_to_char("You insert the book into the bookcase and a passage leading north appears.\r\n", ch);
	act("$n inserts a book into the bookcase and a passage leading northward appears.\r\n",
	    TRUE, ch, 0, ch, TO_ROOM);
	return (TRUE);

}				/* end of sp2100_cloud_bookshelf() */

int sp2200_clan_teleporter_button(struct char_data * ch, int cmd, char *arg)
{

	int location, loc_nr;


	if (cmd != CMD_PUSH)
		return (FALSE);

	for (; *arg == ' '; arg++);
	if (!(*arg))
		return (FALSE);
	if (!is_abbrev(arg, "button"))
		return (FALSE);


	if (!CLAN_RANK(ch)) {
		send_to_char("You press the button but nothing seems to happen.\r\n", ch);
		return (TRUE);
	}

	if (!GET_CLAN_ROOM(CLAN_NUMBER(ch))) {
		send_to_char("You press the button and see the world around you fading out of existence.\r\n", ch);
		send_to_char("As the world fades back into existence you notice you haven't moved.\r\n", ch);
		act("$n presses the button and starts flickering in and out of existence", TRUE, ch, 0, 0, TO_ROOM);
		return (TRUE);
	}

	loc_nr = GET_CLAN_ROOM(CLAN_NUMBER(ch));
	for (location = 0; location <= top_of_world; location++) {
		if (world[location].number == loc_nr) {
			break;
		}
		else if (location == top_of_world) {
			send_to_char("As you press the button the teleporter starts to make strange noises.\r\n", ch);
			act("As $n presses the button the teleporter makes some strange noises.", TRUE, ch, 0, 0, TO_ROOM);
			return (TRUE);
		}
	}

	send_to_char("As you press the button the world around you fades away.\r\n", ch);
	act("$n presses the button and suddenly winks out of existence", TRUE, ch, 0, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, location);
	in3000_do_look(ch, "", 0);
	act("$n suddenly winks into existence.", TRUE, ch, 0, 0, TO_ROOM);
	return (TRUE);

}				/* END OF sp2200_clan_teleporter_button() */

int sp2300_load_ammo(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj_object;
	struct obj_data *obj_weapon;
	struct obj_data *obj_ammo;
	int lv_ammo;
	char buf[MAX_STRING_LENGTH];

	if (cmd != CMD_RELOAD)
		return (FALSE);
	for (; *arg == ' '; arg++);
	if (!(*arg)) {
		send_to_char("What do you wish to load?\r\n", ch);
		return (TRUE);
	}

	obj_object = ha2075_get_obj_list_vis(ch, arg, ch->carrying);
	if (!(obj_object)) {
		send_to_char("You don't have that!\r\n", ch);
		return (TRUE);
	}

	lv_ammo = obj_index[obj_object->item_number].virtual;

	switch (lv_ammo) {
	case OBJ_ARROW:
	case OBJ_P_ARROW:
	case OBJ_BOLT:
	case OBJ_P_BOLT:
	case OBJ_D_BOLT:
		break;
	default:
		send_to_char("This can't be used with any weapon!\r\n", ch);
		return (TRUE);
	}

	obj_weapon = ch->equipment[17];

	if (!obj_weapon) {
		send_to_char("You need to have a missile weapon equipped first.\r\n", ch);
		return (TRUE);
	}
	switch (obj_index[obj_weapon->item_number].virtual) {
	case OBJ_BOW:
		if ((lv_ammo != OBJ_ARROW) && (lv_ammo != OBJ_P_ARROW)) {
			send_to_char("You can't use this with your bow.\r\n", ch);
			return (TRUE);
		}
		break;
	case OBJ_CROSSBOW:
		if ((lv_ammo != OBJ_BOLT) && (lv_ammo != OBJ_P_BOLT)) {
			send_to_char("You can't use this with your crossbow.\r\n", ch);
			return (TRUE);
		}
		break;
	case OBJ_HEAVYCROSSBOW:
		if ((lv_ammo != OBJ_BOLT) &&
		    (lv_ammo != OBJ_P_BOLT) &&
		    (lv_ammo != OBJ_D_BOLT)) {
			send_to_char("You can't use this with your heavy crossbow.\r\n", ch);
			return (TRUE);
		}
		break;
	default:
		send_to_char("You need to have a missile weapon equipped first.\r\n", ch);
		return (TRUE);
	}

	obj_ammo = obj_weapon->contains;
	if (obj_ammo) {
		sprintf(buf, "%s is already loaded.\r\n", obj_weapon->short_description);
		send_to_char(buf, ch);
		return (TRUE);
	}

	ha1800_obj_from_char(obj_object);
	ha2300_obj_to_obj(obj_object, obj_weapon);

	sprintf(buf, "You load %s with %s.\r\n", obj_weapon->short_description, obj_object->short_description);
	send_to_char(buf, ch);
	sprintf(buf, "$n loads %s with %s.", obj_weapon->short_description, obj_object->short_description);
	act(buf, TRUE, ch, 0, 0, TO_NOTVICT);
	WAIT_STATE(ch, PULSE_VIOLENCE * 2);
	return (TRUE);

}				/* END OF sp2300_load_ammo() */


void sp2400_do_shoot(struct char_data * ch, char *arg, int cmd)
{

	struct obj_data *obj_weapon;
	struct obj_data *obj_ammo;
	int lv_wp, lv_ammo, lv_dam;
	char lv_name[MAX_STRING_LENGTH], lv_name2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct char_data *vict;
	struct char_data *victim;

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	//arg = one_argument(arg, buf);

	obj_weapon = ch->equipment[17];
	if (!obj_weapon) {
		send_to_char("You don't have anything to shoot with.\r\n", ch);
		return;
	}

	lv_wp = obj_index[obj_weapon->item_number].virtual;

	if ((lv_wp != OBJ_BOW) &&
	    (lv_wp != OBJ_CROSSBOW) &&
	    (lv_wp != OBJ_HEAVYCROSSBOW)) {
		send_to_char("You don't have anything to shoot with.\r\n", ch);
		return;
	}

	obj_ammo = obj_weapon->contains;
	if (!obj_ammo) {
		sprintf(buf, "%s has nothing loaded.\r\n", obj_weapon->short_description);
		send_to_char(buf, ch);
		act("$n tries to shoot air.", TRUE, ch, 0, 0, TO_NOTVICT);
		return;
	}

	if ((!*arg) && (!ch->specials.fighting)) {
		send_to_char("Shoot at what?\r\n", ch);
		return;
	}

	vict = ch->specials.fighting;

	if (!vict) {
		vict = ha2125_get_char_in_room_vis(ch, arg);
		if (!vict) {
			send_to_char("Huh? You don't see that.\r\n", ch);
			return;
		}
	}

	victim = vict;

	CHECK_FOR_CHARM();
	CHECK_FOR_PK();
	CHECK_FOR_NO_FIGHTING();
	IS_ROOM_NO_KILL();

	lv_ammo = obj_index[obj_ammo->item_number].virtual;

	if (ch == vict) {
		sprintf(buf, "You put %s against your head and release %s.\r\n",
		  obj_weapon->short_description, obj_ammo->short_description);
		send_to_char(buf, ch);
		sprintf(buf, "$n puts %s against $s head and releases %s.\r\n",
		  obj_weapon->short_description, obj_ammo->short_description);
		act(buf, TRUE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "%s L[%d] committed suicide.\r\n",
			GET_REAL_NAME(vict),
			GET_LEVEL(vict));
		do_info(buf, 1, MAX_LEV, vict);
		spec_log(buf, DEATH_LOG);
		ha2700_extract_obj(obj_ammo);
		ft1800_raw_kill(vict, ch);
		return;
	}
	strcpy(lv_name, obj_ammo->short_description);
	strcpy(lv_name2, IS_PC(vict) ? GET_REAL_NAME(vict) : vict->player.short_descr);
	sprintf(buf, "You shoot %s at %s.\r\n", lv_name, lv_name2);
	send_to_char(buf, ch);
	sprintf(buf, "$n shoots %s at %s.", lv_name, lv_name2);
	act(buf, TRUE, ch, 0, 0, TO_NOTVICT);

	lv_dam = 0;
	switch (lv_ammo) {
	case OBJ_ARROW:
	case OBJ_P_ARROW:
		sprintf(buf, "&r%s eviscerates %s.&n", lv_name, lv_name2);
		act(buf, TRUE, ch, obj_ammo, 0, TO_ROOM);
		act(buf, TRUE, ch, obj_ammo, 0, TO_CHAR);
		lv_dam = 300;
		break;
	case OBJ_BOLT:
	case OBJ_P_BOLT:
		sprintf(buf, "&r%s mutilates %s.&n", lv_name, lv_name2);
		act(buf, TRUE, ch, obj_ammo, 0, TO_ROOM);
		act(buf, TRUE, ch, obj_ammo, 0, TO_CHAR);
		lv_dam = 600;
		break;
	case OBJ_D_BOLT:
		sprintf(buf, "&r%s sends %s to oblivion.&n", lv_name, lv_name2);
		act(buf, TRUE, ch, obj_ammo, 0, TO_ROOM);
		act(buf, TRUE, ch, obj_ammo, 0, TO_CHAR);
		lv_dam = 1500;
		break;
	default:
		sprintf(buf, "&r%s hits %s (that had to hurt!).&n", lv_name, lv_name2);
		act(buf, TRUE, ch, obj_ammo, 0, TO_ROOM);
		act(buf, TRUE, ch, obj_ammo, 0, TO_CHAR);
		lv_dam = (obj_ammo->obj_flags.weight);
		break;
	}

	if (lv_dam > 0) {
		if (IS_PC(vict))
			lv_dam = lv_dam >> 1;
		DAMAGE(ch, vict, lv_dam, TYPE_ARROW);
	}

	ha2700_extract_obj(obj_ammo);

	obj_ammo = obj_weapon->contains;

}				/* END OF sp2400_do_shoot() */


bool is_targeted(struct char_data * ch, struct char_data * victim)
{

	bool lv_found;
	struct char_data *k;
	struct descriptor_data *d;

	if (!ch || !victim)
		return (FALSE);


	lv_found = FALSE;
	for (d = descriptor_list; d; d = d->next) {
		if (d->character) {
			k = d->character;
			if ((d->connected == CON_PLAYING) &&
			    (k->in_room >= 0) &&
			    (k != ch) &&
			    (!are_grouped(k, ch)) &&
			    ((time(0) - d->last_hit_time) < 300) &&
			    (d->last_target == victim))
				lv_found = TRUE;
		}
	}

	return (lv_found);
}

bool are_grouped(struct char_data * ch, struct char_data * victim)
{

	struct char_data *k;
	struct follow_type *f;
	bool lv_found;

	if (!IS_AFFECTED(ch, AFF_GROUP) || !IS_AFFECTED(victim, AFF_GROUP))
		return (FALSE);

	if (ch->master) {
		k = ch->master;
	}
	else {
		k = ch;
	}

	lv_found = FALSE;
	for (f = k->followers; f; f = f->next)
		if (f->follower == victim)
			lv_found = TRUE;

	if (k == victim)
		lv_found = TRUE;

	return (lv_found);
}


void reset_target(struct char_data * victim)
{

	struct descriptor_data *d;

	for (d = descriptor_list; d; d = d->next) {
		if (d->character) {
			if (d->last_target == victim) {
				d->last_hit_time = 0;
				d->last_target = 0;
			}
		}
	}
}
