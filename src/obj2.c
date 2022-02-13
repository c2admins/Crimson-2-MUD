/* oj */
/* gv_location: 11501-12000 */
/* ***************************************************************** *
*  file: obj2.c , Implementation of commands.        Part of DIKUMUD *
*  Usage : Commands mainly using objects.                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "utility.h"
#include "globals.h"
#include "func.h"

void weight_change_object(struct obj_data * obj, int weight)
{

	struct obj_data *tmp_obj;
	struct char_data *tmp_ch;

	if (obj->in_room != NOWHERE) {
		GET_OBJ_WEIGHT(obj) += weight;
	}
	else if ((tmp_ch = obj->carried_by)) {
		ha1800_obj_from_char(obj);
		GET_OBJ_WEIGHT(obj) += weight;
		ha1700_obj_to_char(obj, tmp_ch);
	}
	else if ((tmp_obj = obj->in_obj)) {
		ha2400_obj_from_obj(obj);
		GET_OBJ_WEIGHT(obj) += weight;
		ha2300_obj_to_obj(obj, tmp_obj);
	}
	else {
		main_log("Unknown attempt to subtract weight from an object.");
	}
}


void name_from_drinkcon(struct obj_data * obj)
{
	int i;
	char *new_name;

	/* this code is annoying */
	return;

	for (i = 0; (*((obj->name) + i) != ' ') && (*((obj->name) + i) != '\0'); i++);

	if (*((obj->name) + i) == ' ') {
		new_name = strdup((obj->name) + i + 1);
		free(obj->name);
		ha9900_sanity_check(0, "FREE30", "SYSTEM");
		obj->name = new_name;
	}
}				/* END OF name_from_drinkcon() */

void name_to_drinkcon(struct obj_data * obj, int type)
{

	char *new_name;

	/* so is this this */
	return;

	CREATE(new_name, char, strlen(obj->name) + strlen(drinknames[type]) + 2);
	sprintf(new_name, "%s %s", drinknames[type], obj->name);
	free(obj->name);
	ha9900_sanity_check(0, "FREE31", "SYSTEM");
	obj->name = new_name;
}

void do_drink(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *temp;
	struct affected_type af;
	int amount, idx, jdx;

	one_argument(argument, buf);

	if (!(temp = ha2075_get_obj_list_vis(ch, buf, ch->carrying))) {
		if (!(temp = ha2075_get_obj_list_vis(ch, buf, world[ch->in_room].contents))) {
			act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}

	if (temp->obj_flags.type_flag != ITEM_DRINKCON) {
		act("You can't drink from that!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	/* IS THE CHAR DRUNK? */
	if ((GET_COND(ch, DRUNK) >
	     (races[GET_RACE(ch)].max_drunk - 10)) &&
	    (races[GET_RACE(ch)].max_drunk > 0) &&
	    (GET_COND(ch, THIRST) > 0)) {
		act("You simply fail to reach your mouth!", FALSE, ch, 0, 0, TO_CHAR);
		act("$n tried to drink but missed $s mouth!", TRUE, ch, 0, 0, TO_ROOM);
		return;
	}

	if ((GET_COND(ch, FOOD) > (races[GET_RACE(ch)].max_food - 5)) &&
	    ((GET_COND(ch, THIRST) > 0) ||
	     (GET_COND(ch, THIRST) >
	      (races[GET_RACE(ch)].max_thirst - 5)))) {
		act("Your stomach can't contain anymore!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (temp->obj_flags.type_flag == ITEM_DRINKCON) {
		if (temp->obj_flags.value[1] > 0) {	/* Not empty */
			sprintf(buf, "$n drinks %s from $p", drinks[temp->obj_flags.value[2]]);
			if (world[ch->in_room].number != FOUNTAIN_ROOM) {
				act(buf, TRUE, ch, temp, 0, TO_ROOM);
			}

			sprintf(buf, "You drink %s.\n\r", drinks[temp->obj_flags.value[2]]);
			send_to_char(buf, ch);

			if (drink_aff[temp->obj_flags.value[2]][DRUNK] > 0) {
				idx = races[GET_RACE(ch)].max_thirst -
					GET_COND(ch, THIRST) + 1;
				jdx = temp->obj_flags.value[2];
				amount = idx / drink_aff[jdx][DRUNK];
				amount = MAXV(1, amount);
			}
			else {
				amount = number(3, 10);
			}

			amount = MINV(amount, temp->obj_flags.value[1]);

			/* weight_change_object(temp, -amount); */
			/* Subtract amount */

			idx = temp->obj_flags.value[2];
			jdx = drink_aff[idx][DRUNK];
			li2300_gain_condition(ch, DRUNK, (int) ((int) drink_aff
			     [temp->obj_flags.value[2]][DRUNK] * amount) / 4);

			/*
			 idx = temp->obj_flags.value[2];
			 jdx = drink_aff[idx][FOOD];
			 li2300_gain_condition(ch,FOOD,(int)((int)drink_aff
																					 [temp->obj_flags.value[2]][FOOD]*amount)/4);
			 */

			idx = temp->obj_flags.value[2];
			jdx = drink_aff[idx][THIRST];
			li2300_gain_condition(ch, THIRST, (int) ((int) drink_aff
			    [temp->obj_flags.value[2]][THIRST] * amount) / 4);

			if (GET_COND(ch, DRUNK) > 10)
				act("You feel drunk.", FALSE, ch, 0, 0, TO_CHAR);

			if (GET_COND(ch, THIRST) >
			    (races[GET_RACE(ch)].max_thirst - 5))
				act("You do not feel thirsty.", FALSE, ch, 0, 0, TO_CHAR);

			if (GET_COND(ch, FOOD) >
			    (races[GET_RACE(ch)].max_food - 5))
				act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

			if (temp->obj_flags.value[3]) {	/* The shit was poisoned
							 * ! */
				act("Ooups, it tasted rather strange ?!!?", FALSE, ch, 0, 0, TO_CHAR);
				act("$n chokes and utters some strange sounds.",
				    TRUE, ch, 0, 0, TO_ROOM);
				af.type = SPELL_POISON;
				af.duration = amount * 3;
				af.modifier = 0;
				af.location = APPLY_NONE;
				af.bitvector = AFF_POISON;
				ha1400_affect_join(ch, &af, FALSE, FALSE);
			}

			/* empty the container, and no longer poison. */
			temp->obj_flags.value[1] -= amount;
			/* The last bit */
			if (!temp->obj_flags.value[1]) {
				temp->obj_flags.value[2] = 0;
				temp->obj_flags.value[3] = 0;
				name_from_drinkcon(temp);
			}
			return;

		}
	}

	act("It's empty already.", FALSE, ch, 0, 0, TO_CHAR);

	return;
}



void do_eat(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *temp;
	struct affected_type af;



	one_argument(argument, buf);

	if (!(temp = ha2075_get_obj_list_vis(ch, buf, ch->carrying))) {
		act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (temp->obj_flags.type_flag != ITEM_FOOD &&
	    GET_RACE(ch) != RACE_TROLL &&
	    GET_LEVEL(ch) < IMO_SPIRIT) {
		act("Your stomach refuses to eat that!?!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	/* Stomach full */
	if (GET_COND(ch, FOOD) >
	    (races[GET_RACE(ch)].max_food - 5)) {
		act("You are to full to eat more!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (GET_COND(ch, THIRST) >
	    (races[GET_RACE(ch)].max_thirst - 5)) {
		act("You are to full of liquids to eat more!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (world[ch->in_room].number != FOUNTAIN_ROOM) {
		act("$n eats $p.", TRUE, ch, temp, 0, TO_ROOM);
	}
	act("You eat $o.", FALSE, ch, temp, 0, TO_CHAR);

	/* IF ITS A TROLL OR IMMORTAL EATING A NON FOOD  */
	/* ITEM, DEFAULT TO FOOD VALUE OF 1 */
	if (temp->obj_flags.type_flag != ITEM_FOOD) {
		li2300_gain_condition(ch, FOOD, 1);
	}
	else {
		li2300_gain_condition(ch, FOOD, temp->obj_flags.value[0]);
	}

	if (GET_COND(ch, FOOD) >
	    (races[GET_RACE(ch)].max_food - 5))
		act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

	/* POISONED? */
	if (temp->obj_flags.value[3] && (GET_LEVEL(ch) < IMO_LEV)) {
		act("Ooups, it tasted rather strange ?!!?", FALSE, ch, 0, 0, TO_CHAR);
		act("$n coughs and utters some strange sounds.", FALSE, ch, 0, 0, TO_ROOM);

		af.type = SPELL_POISON;
		af.duration = temp->obj_flags.value[0] * 2;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_POISON;
		ha1400_affect_join(ch, &af, FALSE, FALSE);
	}

	sprintf(buf, "(obj) do_eat %s - %s", GET_REAL_NAME(ch), OBJS(temp, ch));
	main_log(buf);
	spec_log(buf, EQUIPMENT_LOG);
	ha2700_extract_obj(temp);
}


void do_fill(struct char_data * ch, char *argument, int cmd)
{
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];



	bzero(buf1, sizeof(buf1));
	bzero(buf2, sizeof(buf2));
	argument = one_argument(argument, buf1);
	argument = one_argument(argument, buf2);

	bzero(buf3, sizeof(buf3));
	sprintf(buf3, "%s %s", buf2, buf1);
	do_pour(ch, buf3, CMD_POUR);

}				/* END OF do_fill() */

void do_pour(struct char_data * ch, char *argument, int cmd)
{
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct obj_data *from_obj;
	struct obj_data *to_obj;
	int i;
	int amount;
	int fountain_check;



	argument_interpreter(argument, arg1, arg2);

	if (!*arg1) {		/* No arguments */
		act("What do you want to pour from?", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (!(from_obj = ha2075_get_obj_list_vis(ch, arg1, ch->carrying))) {
		if (!(from_obj = ha2075_get_obj_list_vis(ch, arg1, world[ch->in_room].contents))) {
			act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}

	if (from_obj->obj_flags.type_flag != ITEM_DRINKCON) {
		act("You can't pour from that!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (from_obj->obj_flags.value[1] == 0) {
		act("The $p is empty.", FALSE, ch, from_obj, 0, TO_CHAR);
		return;
	}

	if (!*arg2) {
		act("Where do you want it? Out or in what?", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (!str_cmp(arg2, "out")) {
		/* IF ITS A FOUNTAIN THEN STOP THEM */
		fountain_check = FALSE;
		for (i = 0; i < strlen(from_obj->name); i++) {
			if (strcmp(from_obj->name + i, "fountain") == 0)
				fountain_check = TRUE;
		}
		if (fountain_check == TRUE) {
			act("$n breaks into a sweat trying to empty $p.", TRUE, ch, from_obj, 0, TO_ROOM);
			act("You break into a sweat trying to empty $p.", FALSE, ch, from_obj, 0, TO_CHAR);
		}
		else {
			act("You empty $p.", FALSE, ch, from_obj, 0, TO_CHAR);
			/* Empty */
			from_obj->obj_flags.value[1] = 0;
			from_obj->obj_flags.value[2] = 0;
			from_obj->obj_flags.value[3] = 0;
			name_from_drinkcon(from_obj);
		}
		return;

	}

	if (!(to_obj = ha2075_get_obj_list_vis(ch, arg2, ch->carrying))) {
		if (!(to_obj = ha2075_get_obj_list_vis(ch, arg2, world[ch->in_room].contents))) {
			act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}

	if (to_obj->obj_flags.type_flag != ITEM_DRINKCON) {
		act("You can't pour anything into that.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if ((to_obj->obj_flags.value[1] != 0) &&
	    (to_obj->obj_flags.value[2] != from_obj->obj_flags.value[2])) {
		act("There is already another liquid in it!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (!(to_obj->obj_flags.value[1] < to_obj->obj_flags.value[0])) {
		act("There is no room for more.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (to_obj == from_obj) {
		act("What kind of an idiot are you?", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	sprintf(buf, "You pour %s into %s.",
		drinks[from_obj->obj_flags.value[2]], arg2);
	send_to_char(buf, ch);

	/* New alias */
	if (to_obj->obj_flags.value[1] == 0)
		name_to_drinkcon(to_obj, from_obj->obj_flags.value[2]);

	/* First same type liq. */
	to_obj->obj_flags.value[2] = from_obj->obj_flags.value[2];

	/* Then how much to pour */
	from_obj->obj_flags.value[1] -= (amount =
		   (to_obj->obj_flags.value[0] - to_obj->obj_flags.value[1]));

	to_obj->obj_flags.value[1] = to_obj->obj_flags.value[0];

	if (from_obj->obj_flags.value[1] < 0) {	/* There was to little */
		to_obj->obj_flags.value[1] += from_obj->obj_flags.value[1];
		amount += from_obj->obj_flags.value[1];
		from_obj->obj_flags.value[1] = 0;
		from_obj->obj_flags.value[2] = 0;
		from_obj->obj_flags.value[3] = 0;
		name_from_drinkcon(from_obj);
	}

	/* Then the poison boogie */
	to_obj->obj_flags.value[3] =
		(to_obj->obj_flags.value[3] || from_obj->obj_flags.value[3]);

	/* And the weight boogie */

	/* weight_change_object(from_obj, -amount);
	 * weight_change_object(to_obj, amount); *//* Add weight */

	return;
}

void do_sip(struct char_data * ch, char *argument, int cmd)
{

	struct affected_type af;
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct obj_data *temp;



	one_argument(argument, arg);

	if (!(temp = ha2075_get_obj_list_vis(ch, arg, ch->carrying))) {
		act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (temp->obj_flags.type_flag != ITEM_DRINKCON) {
		act("You can't sip from that!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (GET_COND(ch, DRUNK) > 10) {	/* The pig is drunk ! */
		act("You simply fail to reach your mouth!", FALSE, ch, 0, 0, TO_CHAR);
		act("$n tries to sip, but fails!", TRUE, ch, 0, 0, TO_ROOM);
		return;
	}

	if (!temp->obj_flags.value[1]) {	/* Empty */
		act("But there is nothing in it?", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (world[ch->in_room].number != FOUNTAIN_ROOM) {
		act("$n sips from $o.", TRUE, ch, temp, 0, TO_ROOM);
	}

	sprintf(buf, "It tastes like %s.\n\r", drinks[temp->obj_flags.value[2]]);
	send_to_char(buf, ch);

	li2300_gain_condition(ch, DRUNK, (int) (drink_aff[temp->obj_flags.value[2]][DRUNK] / 4));

	li2300_gain_condition(ch, FOOD, (int) (drink_aff[temp->obj_flags.value[2]][FOOD] / 4));

	li2300_gain_condition(ch, THIRST, (int) (drink_aff[temp->obj_flags.value[2]][THIRST] / 4));

	/* weight_change_object(temp, -1); *//* Subtract one unit */

	if (GET_COND(ch, DRUNK) >
	    (races[GET_RACE(ch)].max_drunk - 5))
		act("You feel drunk.", FALSE, ch, 0, 0, TO_CHAR);

	if (GET_COND(ch, THIRST) >
	    (races[GET_RACE(ch)].max_thirst - 5))
		act("You do not feel thirsty.", FALSE, ch, 0, 0, TO_CHAR);

	if (GET_COND(ch, FOOD) >
	    (races[GET_RACE(ch)].max_food - 5))
		act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

	if (temp->obj_flags.value[3] && !IS_AFFECTED(ch, AFF_POISON)) {
		act("But it also had a strange taste!", FALSE, ch, 0, 0, TO_CHAR);

		af.type = SPELL_POISON;
		af.duration = 3;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_POISON;
		ha1300_affect_to_char(ch, &af);
	}

	temp->obj_flags.value[1]--;

/* The last bit */
	if (!temp->obj_flags.value[1]) {
		temp->obj_flags.value[2] = 0;
		temp->obj_flags.value[3] = 0;
		name_from_drinkcon(temp);
	}

	return;

}				/* END OF do_sip() */


void do_taste(struct char_data * ch, char *argument, int cmd)
{

	struct affected_type af;
	char arg[MAX_STRING_LENGTH];
	struct obj_data *temp;



	one_argument(argument, arg);

	if (!(temp = ha2075_get_obj_list_vis(ch, arg, ch->carrying))) {
		act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (temp->obj_flags.type_flag == ITEM_DRINKCON) {
		do_sip(ch, argument, 0);
		return;
	}

	if (!(temp->obj_flags.type_flag == ITEM_FOOD)) {
		act("Taste that?!? Your stomach refuses!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (world[ch->in_room].number != FOUNTAIN_ROOM) {
		act("$n tastes $o", FALSE, ch, temp, 0, TO_ROOM);
	}
	act("You taste $o", FALSE, ch, temp, 0, TO_CHAR);

	li2300_gain_condition(ch, FOOD, 1);

	if (GET_COND(ch, FOOD) >
	    (races[GET_RACE(ch)].max_food - 5))
		act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

	if (temp->obj_flags.value[3] && !IS_AFFECTED(ch, AFF_POISON)) {
		act("Ooups, it did not taste good at all!", FALSE, ch, 0, 0, TO_CHAR);

		af.type = SPELL_POISON;
		af.duration = 2;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector = AFF_POISON;
		ha1300_affect_to_char(ch, &af);
	}

	temp->obj_flags.value[0]--;

	/* Nothing left */
	if (!temp->obj_flags.value[0]) {
		act("There is nothing left now.", FALSE, ch, 0, 0, TO_CHAR);
		ha2700_extract_obj(temp);
	}

	return;
}


/* functions related to wear */
void perform_wear_msg(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose)
{



	if (lv_verbose == FALSE)
		return;

	switch (keyword) {
	case WEAR_LIGHT:
		if (obj_object->obj_flags.value[2]) {
			act("$n lights $p and holds it.", FALSE, ch, obj_object, 0, TO_ROOM);
			act("You light $p and hold it.", FALSE, ch, obj_object, 0, TO_CHAR);
		}
		else {
			act("$n holds $p.", FALSE, ch, obj_object, 0, TO_ROOM);
			act("$p won't light but you hold it anyway.", FALSE, ch, obj_object, 0, TO_CHAR);
		}
		break;
	case WEAR_FINGER_R:
		act("$n wears $p on $s right finger.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your right finger.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_FINGER_L:
		act("$n wears $p on $s left finger.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your left finger.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_NECK_1:
	case WEAR_NECK_2:
		act("$n wears $p around $s neck.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p around your neck.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_BODY:
		act("$n wears $p on $s body.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your body.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_HEAD:
		act("$n wears $p on $s head.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your head.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_LEGS:
		act("$n wears $p on $s legs.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your legs.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_FEET:
		act("$n wears $p on $s feet.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your feet.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_HANDS:
		act("$n wears $p on $s hands.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your hands.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_ARMS:
		act("$n wears $p on $s arms.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your arms.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_SHIELD:
		act("$n starts using $p as a shield.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You start using $p as a shield.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_ABOUT:
		act("$n wears $p about $s body.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p about your body.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_WAISTE:
		act("$n wears $p about $s waist.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p about your waist.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_WRIST_R:
		act("$n wears $p around $s right wrist.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p around your right wrist.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_WRIST_L:
		act("$n wears $p around $s left wrist.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p around your left wrist.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WIELD:
		act("$n wields $p.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wield $p.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case HOLD:
		act("$n grabs $p.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You grab $p.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_TAIL:
		act("$n wears $p on $s tail.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your tail.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_4LEGS_1:
		act("$n wears $p on $s front legs.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your front legs.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	case WEAR_4LEGS_2:
		act("$n wears $p on $s hind legs.", TRUE, ch, obj_object, 0, TO_ROOM);
		act("You wear $p on your hind legs.", TRUE, ch, obj_object, 0, TO_CHAR);
		break;
	}
	return;

}

bool remove_carried_object(struct char_data * ch, struct obj_data * obj_object, int wear_position)
{

	char buf[MAX_STRING_LENGTH];

	if (IS_SET(GET_OBJ1(obj_object), OBJ1_CURSED)) {
		if (GET_LEVEL(ch) < IMO_SPIRIT) {
			sprintf(buf, "%s tries desperately to remove %s.",
				GET_REAL_NAME(ch),
				OBJS(obj_object, ch));
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "ACK! you can't remove %s.\r\n",
				OBJS(obj_object, ch));
			co2900_send_to_char(ch, "%s", buf);
			return FALSE;
		}
		else {
			sprintf(buf, "%s laughs at the feeble curse spell on %s.",
				GET_REAL_NAME(ch),
				OBJS(obj_object, ch));
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "You laugh at the feeble curse spell on %s.\r\n",
				OBJS(obj_object, ch));
			co2900_send_to_char(ch, "%s", buf);
		}
	}			/* END OF ITS CURSED */

	if (CAN_CARRY_N(ch) == IS_CARRYING_N(ch)) {
		send_to_char("You can't carry that many items.\r\n", ch);
		return FALSE;
	}

	ha1700_obj_to_char(ha1930_unequip_char(ch, wear_position), ch);

	if (obj_object->obj_flags.type_flag == ITEM_LIGHT ||
	    obj_object->obj_flags.type_flag == ITEM_QSTLIGHT) {
		if (obj_object->obj_flags.value[2])
			world[ch->in_room].light--;
	}

	act("You stop using $p.", FALSE, ch, obj_object, 0, TO_CHAR);
	act("$n stops using $p.", TRUE, ch, obj_object, 0, TO_ROOM);
	return TRUE;

}				/* END OF remove_carried_object */


/* Odin: 1-14-01 */

int obj_has_apply(struct obj_data * object, int apply)
{

	if ((object->affected[0].location == apply) ||
	    (object->affected[1].location == apply) ||
	    (object->affected[2].location == apply) ||
	    (object->affected[3].location == apply)) {

		return TRUE;
	}

	else
		return FALSE;
}


/* Odin: 1-14-01
Usage: char_wearing_apply_limit(ch, 1, APPLY_EXTRA_ATTACKS); */


int char_wearing_apply_limit(struct char_data * ch, int limit, int apply)
{
	int count, num_applies = 0;
	struct obj_data *tmp_object;

	for (count = 0; (count < MAX_WEAR); count++) {
		if (ch->equipment[count]) {
			tmp_object = ch->equipment[count];
			if ((tmp_object->affected[0].location == apply) ||
			    (tmp_object->affected[1].location == apply) ||
			    (tmp_object->affected[2].location == apply) ||
			    (tmp_object->affected[3].location == apply)) {

				num_applies++;
			}
		}
	}

	if (num_applies >= limit)	/* already wearing too many applies */
		return TRUE;
	else
		return FALSE;
}


void oj5000_wear_obj(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose)
{

	char buffer[MAX_STRING_LENGTH];
	bool lv_got_specific_keyword = FALSE;



	if (keyword > 900) {	/* flag to mask output ie for wear all */
		keyword = keyword - 1000;
		lv_got_specific_keyword = TRUE;
	}

	if (obj_has_apply(obj_object, APPLY_EXTRA_ATTACKS)) {
		if (char_wearing_apply_limit(ch, 1, APPLY_EXTRA_ATTACKS)) {
			send_to_char("You can only wear one extra attack item at a time!\r\n", ch);
			return;
		}
	}

	if (!IS_PC(ch) && (GET_TIMER(obj_object) > -1) &&
	    IS_AFFECTED(ch, AFF_CHARM)) {
		send_to_char("Charmed mob's can't wear timed eq!", ch);
		return;
	}
/* Bingo 02-11-01, stops people from using mag imm items to cancel magic immune
if(obj_has_apply(obj_object, APPLY_MAGIC_IMMUNE)) {
	if(IS_AFFECTED(ch, AFF_MAGIC_IMMUNE)) {
		send_to_char("A strange magical force prevents you from wearing this item.\r\n", ch);
		return;
	}
}*/

/* Bingo 02-11-01, chance of failing to wear a magic resist item if resisted
if(obj_has_apply(obj_object, APPLY_MAGIC_RESIST)) {
	if(IS_AFFECTED(ch, AFF_MAGIC_RESIST)) {
		if(number(0,5) > 1) {
			send_to_char("A magical force causes you to fail wearing it, try again.\r\n", ch);
			return;
		}
	}
}
*/
	switch (keyword) {
	case WEAR_LIGHT:{	/* LIGHT SOURCE */
			if (ch->equipment[WEAR_LIGHT]) {
				if (!lv_got_specific_keyword) {
					if (remove_carried_object(ch, ch->equipment[WEAR_LIGHT], WEAR_LIGHT)) {
						ha1925_equip_char(ch, obj_object, WEAR_LIGHT, lv_verbose, BIT0);
						if (obj_object->obj_flags.value[2] && (ch->desc) && STATE(ch->desc) == CON_PLAYING)
							world[ch->in_room].light++;
						oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
					}
				}
			}
			else {
				ha1925_equip_char(ch, obj_object, WEAR_LIGHT, lv_verbose, BIT0);
				if (obj_object->obj_flags.value[2] && (ch->desc) && STATE(ch->desc) == CON_PLAYING)
					world[ch->in_room].light++;
				oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
			}
		}		/* END OF case 0 */
		break;

	case WEAR_FINGER_R:
	case WEAR_FINGER_L:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_FINGER)) {
				if ((ch->equipment[WEAR_FINGER_L]) && (ch->equipment[WEAR_FINGER_R])) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_FINGER_L], WEAR_FINGER_L)) {
							ha1925_equip_char(ch, obj_object, WEAR_FINGER_L, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					if (ch->equipment[WEAR_FINGER_L]) {
						ha1925_equip_char(ch, obj_object, WEAR_FINGER_R, lv_verbose, BIT0);
						oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
					}
					else {
						ha1925_equip_char(ch, obj_object, WEAR_FINGER_L, lv_verbose, BIT0);
						oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
					}
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your finger.\n\r", ch);
			}
		}		/* END OF CASE 2 */
		break;

	case WEAR_NECK_1:
	case WEAR_NECK_2:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_NECK)) {
				if ((ch->equipment[WEAR_NECK_1]) && (ch->equipment[WEAR_NECK_2])) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_NECK_1], WEAR_NECK_1)) {
							ha1925_equip_char(ch, obj_object, WEAR_NECK_1, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					if (ch->equipment[WEAR_NECK_1]) {
						ha1925_equip_char(ch, obj_object, WEAR_NECK_2, lv_verbose, BIT0);
					}
					else {
						ha1925_equip_char(ch, obj_object, WEAR_NECK_1, lv_verbose, BIT0);
					}
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that around your neck.\n\r", ch);
			}
		}		/* END OF CASE 2 */
		break;

	case WEAR_BODY:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_BODY)) {
				if (ch->equipment[WEAR_BODY]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_BODY], WEAR_BODY)) {
							ha1925_equip_char(ch, obj_object, WEAR_BODY, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_BODY, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your body.\n\r", ch);
			}
		}		/* END OF CASE 3 */
		break;

	case WEAR_HEAD:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_HEAD) && (GET_RACE(ch) != RACE_THRI_KREEN)) {
				if (ch->equipment[WEAR_HEAD]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_HEAD], WEAR_HEAD)) {
							ha1925_equip_char(ch, obj_object, WEAR_HEAD, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_HEAD, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your head.\n\r", ch);
			}
		}		/* END OF CASE HEAD */
		break;

	case WEAR_LEGS:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_LEGS) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN)) {
				if (ch->equipment[WEAR_LEGS]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_LEGS], WEAR_LEGS)) {
							ha1925_equip_char(ch, obj_object, WEAR_LEGS, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_LEGS, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your legs.\n\r", ch);
			}
		}		/* END OF CASE LEGS */
		break;

	case WEAR_FEET:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_FEET) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN)) {
				if (ch->equipment[WEAR_FEET]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_FEET], WEAR_FEET)) {
							ha1925_equip_char(ch, obj_object, WEAR_FEET, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_FEET, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your feet.\n\r", ch);
			}
		}		/* END OF CASE FEET */
		break;

	case WEAR_HANDS:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_HANDS)) {
				if (ch->equipment[WEAR_HANDS]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_HANDS], WEAR_HANDS)) {
							ha1925_equip_char(ch, obj_object, WEAR_HANDS, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_HANDS, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your hands.\n\r", ch);
			}
		}		/* END OF CASE HANDS */
		break;

	case WEAR_ARMS:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_ARMS)) {
				if (ch->equipment[WEAR_ARMS]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_ARMS], WEAR_ARMS)) {
							ha1925_equip_char(ch, obj_object, WEAR_ARMS, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_ARMS, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your arms.\n\r", ch);
			}
		}		/* END OF CASE ARM */
		break;

	case WEAR_SHIELD:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_SHIELD)) {
				if ((ch->equipment[WEAR_SHIELD])) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_SHIELD], WEAR_SHIELD)) {
							ha1925_equip_char(ch, obj_object, WEAR_SHIELD, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_SHIELD, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't use that as a shield.\n\r", ch);
			}
		}		/* END OF CASE SHIELD */
		break;

	case WEAR_ABOUT:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_ABOUT)) {
				if (ch->equipment[WEAR_ABOUT]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_ABOUT], WEAR_ABOUT)) {
							ha1925_equip_char(ch, obj_object, WEAR_ABOUT, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_ABOUT, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that about your body.\n\r", ch);
			}
		}		/* END OF CASE ABOUT */
		break;

	case WEAR_WAISTE:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_WAISTE) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN)) {
				if (ch->equipment[WEAR_WAISTE]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_WAISTE], WEAR_WAISTE)) {
							ha1925_equip_char(ch, obj_object, WEAR_WAISTE, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_WAISTE, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that about your waist.\n\r", ch);
			}
		}		/* END OF CASE WAISTE */
		break;

	case WEAR_WRIST_R:
	case WEAR_WRIST_L:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_WRIST)) {
				if ((ch->equipment[WEAR_WRIST_L]) && (ch->equipment[WEAR_WRIST_R])) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_WRIST_L], WEAR_WRIST_L)) {
							ha1925_equip_char(ch, obj_object, WEAR_WRIST_L, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					if (ch->equipment[WEAR_WRIST_L]) {
						ha1925_equip_char(ch, obj_object, WEAR_WRIST_R, lv_verbose, BIT0);
					}
					else {
						ha1925_equip_char(ch, obj_object, WEAR_WRIST_L, lv_verbose, BIT0);
					}
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that around your wrist.\n\r", ch);
			}
		}		/* END OF CASE WRIST */
		break;

	case WIELD:{
			if (CAN_WEAR(obj_object, ITEM_WIELD)) {
				if (ch->equipment[WIELD]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WIELD], WIELD))
							ha1925_equip_char(ch, obj_object, WIELD, lv_verbose, BIT0);
					}
				}
				else {
					/* Cleric exception has been removed,
					 * and is temporarily placed */
					/* at the end of this file                                      */
					if (GET_OBJ_WEIGHT(obj_object) >
					    ((GET_STR(ch) + GET_BONUS_STR(ch)) + (MAXV(0, (GET_STR(ch) + GET_BONUS_STR(ch)) - 16) * 2))) {
						send_to_char("It is too heavy for you to use.\n\r", ch);
					}
					else {
						ha1925_equip_char(ch, obj_object, WIELD, lv_verbose, BIT0);
					}
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wield that.\n\r", ch);
			}
		}		/* END OF CASE WIELD */
		break;

	case HOLD:{
			if (CAN_WEAR(obj_object, ITEM_HOLD)) {
				if (ch->equipment[HOLD]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[HOLD], HOLD)) {
							ha1925_equip_char(ch, obj_object, HOLD, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}

				}
				else {
					/* Cleric execption has been removed,
					 * and is temporarily placed */
					/* at the end of this file                                      */
					ha1925_equip_char(ch, obj_object, HOLD, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't hold this.\n\r", ch);
			}
		}
		break;		/* END OF CASE HOLD */

	case WEAR_TAIL:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_TAIL)) {
				if (ch->equipment[WEAR_TAIL]) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_TAIL], WEAR_TAIL)) {
							ha1925_equip_char(ch, obj_object, WEAR_TAIL, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					ha1925_equip_char(ch, obj_object, WEAR_TAIL, lv_verbose, BIT0);
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your tail.\n\r", ch);
			}
		}		/* END OF CASE TAIL */
		break;

	case WEAR_4LEGS_1:	/* Added 12/03/02 Nightwynde for Thri-Kreen and
				 * Centaur */
	case WEAR_4LEGS_2:{
			if (CAN_WEAR(obj_object, ITEM_WEAR_4LEGS)) {
				if ((ch->equipment[WEAR_4LEGS_1]) && (ch->equipment[WEAR_4LEGS_2])) {
					if (!lv_got_specific_keyword) {
						if (remove_carried_object(ch, ch->equipment[WEAR_4LEGS_1], WEAR_4LEGS_1)) {
							ha1925_equip_char(ch, obj_object, WEAR_4LEGS_1, lv_verbose, BIT0);
							oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
						}
					}
				}
				else {
					if (ch->equipment[WEAR_4LEGS_1]) {
						ha1925_equip_char(ch, obj_object, WEAR_4LEGS_2, lv_verbose, BIT0);
					}
					else {
						ha1925_equip_char(ch, obj_object, WEAR_4LEGS_1, lv_verbose, BIT0);
					}
					oj5100_wear_check_special(ch, obj_object, keyword, TRUE);
				}
			}
			else {
				if (!lv_got_specific_keyword)
					send_to_char("You can't wear that on your legs.\n\r", ch);
			}
		}		/* END OF CASE 2 */
		break;

	case -1:{
			sprintf(buffer, "Wear %s where?.\n\r", ha1100_fname(obj_object->name));
			send_to_char(buffer, ch);
		}		/* END OF CASE -1 */
		break;

	case -2:{
			if (!lv_got_specific_keyword) {
				sprintf(buffer, "You can't wear %s.\r\n", ha1100_fname(obj_object->name));
				send_to_char(buffer, ch);
			}
		}		/* END OF CASE -1 */
		break;

	default:{
			main_log("Unknown type called in wear.");
		}		/* END OF DEFAULT */
		break;

	}			/* END OF switch */

	return;

}				/* END OF oj5000_wear_obj() */




void oj5100_wear_check_special(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose)
{

	int lv_num;



	if (!obj_object || obj_object->item_number < 0) {
		return;
	}

	lv_num = obj_index[obj_object->item_number].virtual;

}				/* END OF oj5100_wear_check_special() */







void do_wear_all(struct char_data * ch)
{

	struct obj_data *tmp_object;
	struct obj_data *next_obj;
	bool lv_found_it = FALSE;



	for (tmp_object = ch->carrying; tmp_object; tmp_object = next_obj) {

		next_obj = tmp_object->next_content;

		if (CAN_SEE_OBJ(ch, tmp_object)) {

			if (obj_has_apply(tmp_object, APPLY_EXTRA_ATTACKS)) {

				if (char_wearing_apply_limit(ch, 1, APPLY_EXTRA_ATTACKS)) {
					send_to_char("You can only wear one extra attack item at a time!\r\n", ch);
				}
				else {
					do_find_wear_location(ch, tmp_object, BIT0);
					lv_found_it = TRUE;
				}
			}
			else {
				do_find_wear_location(ch, tmp_object, BIT0);
				lv_found_it = TRUE;
			}

		}
	}			/* END OF for LOOP */

	if (!lv_found_it) {
		send_to_char("You do not seem to have anything to wear.\n\r", ch);
	}
}				/* END OF wear_all() */


/* WHERE lv_flag = BIT0 means return if item is non wearable */
void do_find_wear_location(struct char_data * ch, struct obj_data * lv_object, int lv_flag)
{

	int keyword;



	keyword = -2;
	if (lv_object->obj_flags.type_flag == ITEM_LIGHT ||
	    lv_object->obj_flags.type_flag == ITEM_QSTLIGHT)
		keyword = WEAR_LIGHT;
	if (CAN_WEAR(lv_object, ITEM_WEAR_FINGER))
		keyword = WEAR_FINGER_R;
	if (CAN_WEAR(lv_object, ITEM_WEAR_NECK))
		keyword = WEAR_NECK_1;
	if (CAN_WEAR(lv_object, ITEM_WEAR_BODY))
		keyword = WEAR_BODY;
	if (CAN_WEAR(lv_object, ITEM_WEAR_HEAD) && (GET_RACE(ch) != RACE_THRI_KREEN))
		keyword = WEAR_HEAD;
	if (CAN_WEAR(lv_object, ITEM_WEAR_LEGS) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_LEGS;
	if (CAN_WEAR(lv_object, ITEM_WEAR_FEET) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_FEET;
	if (CAN_WEAR(lv_object, ITEM_WEAR_HANDS))
		keyword = WEAR_HANDS;
	if (CAN_WEAR(lv_object, ITEM_WEAR_ARMS))
		keyword = WEAR_ARMS;
	if (CAN_WEAR(lv_object, ITEM_WEAR_SHIELD))
		keyword = WEAR_SHIELD;
	if (CAN_WEAR(lv_object, ITEM_WEAR_ABOUT))
		keyword = WEAR_ABOUT;
	if (CAN_WEAR(lv_object, ITEM_WEAR_WAISTE) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_WAISTE;
	if (CAN_WEAR(lv_object, ITEM_WEAR_WRIST))
		keyword = WEAR_WRIST_R;
	if (CAN_WEAR(lv_object, ITEM_HOLD))
		keyword = HOLD;
	if (CAN_WEAR(lv_object, ITEM_WIELD))
		keyword = WIELD;
	if (CAN_WEAR(lv_object, ITEM_WEAR_TAIL) && (GET_RACE(ch) == RACE_YUANTI || GET_RACE(ch) == RACE_SAURIAN))
		keyword = WEAR_TAIL;
	if (CAN_WEAR(lv_object, ITEM_WEAR_4LEGS) && (GET_RACE(ch) == RACE_THRI_KREEN || GET_RACE(ch) == RACE_CENTAUR))
		keyword = WEAR_4LEGS_1;

	/* IF BIT0 IS SET AND WE DIDN'T FIND A POSITION, RETURN */
	if (IS_SET(lv_flag, BIT0) && keyword == -2) {
		return;
	}

	keyword += 1000;
	oj5000_wear_obj(ch, lv_object, keyword, TRUE);

}				/* END OF do_find_wear_location() */


void do_wear(struct char_data * ch, char *argument, int cmd)
{

	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[256];
	char buffer[MAX_STRING_LENGTH];
	struct obj_data *obj_object;
	int keyword;
	const char *keywords[] = {
		"fingerl",
		"fingerr",
		"neck1",
		"neck2",
		"body",
		"head",
		"legs",
		"feet",
		"hands",
		"arms",
		"shield",
		"about",
		"waist",
		"wristl",
		"wristr",
		"tail",
		"4legs",
		"\n"
	};



	argument_interpreter(argument, arg1, arg2);

	if (!*arg1) {
		send_to_char("Wear what?\r\n", ch);
		return;
	}

	if (!str_cmp(arg1, "all")) {
		do_wear_all(ch);
		return;
	}
	obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);

	if (!obj_object) {
		sprintf(buffer, "You do not seem to have '%s'.\r\n", arg1);
		send_to_char(buffer, ch);
		return;
	}
	/* DID USER SPECIFY THE LOCATION? */
	if (*arg2) {
		keyword = search_block(arg2, keywords, FALSE);	/* Partial Match */
		if (keyword == -1) {
			sprintf(buf, "%s is an unknown body location.\r\n", arg2);
			co2900_send_to_char(ch, "%s", buf);
		}
		else {
			oj5000_wear_obj(ch, obj_object, keyword + 1, TRUE);
		}
		return;
	}

	/* DEFAULT TO LIGHT */
	keyword = -2;
	if (CAN_WEAR(obj_object, ITEM_WEAR_FINGER))
		keyword = WEAR_FINGER_R;
	if (CAN_WEAR(obj_object, ITEM_WEAR_NECK))
		keyword = WEAR_NECK_1;
	if (CAN_WEAR(obj_object, ITEM_WEAR_BODY))
		keyword = WEAR_BODY;
	if (CAN_WEAR(obj_object, ITEM_WEAR_HEAD) && (GET_RACE(ch) != RACE_THRI_KREEN))
		keyword = WEAR_HEAD;
	if (CAN_WEAR(obj_object, ITEM_WEAR_LEGS) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_LEGS;
	if (CAN_WEAR(obj_object, ITEM_WEAR_FEET) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_THRI_KREEN) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_FEET;
	if (CAN_WEAR(obj_object, ITEM_WEAR_HANDS))
		keyword = WEAR_HANDS;
	if (CAN_WEAR(obj_object, ITEM_WEAR_ARMS))
		keyword = WEAR_ARMS;
	if (CAN_WEAR(obj_object, ITEM_WEAR_SHIELD))
		keyword = WEAR_SHIELD;
	if (CAN_WEAR(obj_object, ITEM_WEAR_ABOUT))
		keyword = WEAR_ABOUT;
	if (CAN_WEAR(obj_object, ITEM_WEAR_WAISTE) && (GET_RACE(ch) != RACE_YUANTI) && (GET_RACE(ch) != RACE_CENTAUR) && (GET_RACE(ch) != RACE_SAURIAN))
		keyword = WEAR_WAISTE;
	if (CAN_WEAR(obj_object, ITEM_WEAR_WRIST))
		keyword = WEAR_WRIST_R;
	if (CAN_WEAR(obj_object, ITEM_HOLD))
		keyword = HOLD;
	if (CAN_WEAR(obj_object, ITEM_WIELD))
		keyword = WIELD;
	if (CAN_WEAR(obj_object, ITEM_WEAR_TAIL) && (GET_RACE(ch) == RACE_YUANTI || GET_RACE(ch) == RACE_SAURIAN))
		keyword = WEAR_TAIL;
	if (CAN_WEAR(obj_object, ITEM_WEAR_4LEGS) && (GET_RACE(ch) == RACE_THRI_KREEN || GET_RACE(ch) == RACE_CENTAUR))
		keyword = WEAR_4LEGS_1;

	oj5000_wear_obj(ch, obj_object, keyword, TRUE);

	return;

}				/* END OF do_wear() */


void do_wield(struct char_data * ch, char *argument, int cmd)
{
	char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];
	struct obj_data *obj_object;



	argument_interpreter(argument, arg1, arg2);
	if (!*arg1) {
		send_to_char("Wield what?\n\r", ch);
		return;
	}

	obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
	if (!obj_object) {
		sprintf(buffer, "You do not seem to have '%s'.\n\r", arg1);
		send_to_char(buffer, ch);
		return;
	}

	oj5000_wear_obj(ch, obj_object, WIELD, TRUE);

	return;

}				/* END OF do_wield() */


void do_grab(struct char_data * ch, char *argument, int cmd)
{

	char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];
	struct obj_data *obj_object;



	argument_interpreter(argument, arg1, arg2);

	if (!*arg1) {
		send_to_char("Hold what?\n\r", ch);
		return;
	}

	obj_object = ha2000_get_obj_list(arg1, ch->carrying);
	if (!obj_object) {
		sprintf(buffer, "You do not seem to have '%s'.\n\r", arg1);
		send_to_char(buffer, ch);
		return;
	}

	if (obj_object->obj_flags.type_flag == ITEM_LIGHT ||
	    obj_object->obj_flags.type_flag == ITEM_QSTLIGHT)
		oj5000_wear_obj(ch, obj_object, WEAR_LIGHT, TRUE);
	else
		oj5000_wear_obj(ch, obj_object, HOLD, TRUE);

	return;

}				/* END OF do_grab() */


void do_remove(struct char_data * ch, char *argument, int cmd)
{

	char arg1[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct obj_data *obj_object;
	int number_items_removed, number_items_left, idx, jdx;



	one_argument(argument, arg1);

	if (!(*arg1)) {
		send_to_char("Remove what?\r\n", ch);
		return;
	}

	number_items_removed = 0;
	number_items_left = 0;
	if (is_abbrev(arg1, "all")) {
		for (idx = 0; idx < MAX_WEAR; idx++) {
			if (ch->equipment[idx]) {
				if (IS_SET(GET_OBJ1(ch->equipment[idx]), OBJ1_CURSED)) {
					if (GET_LEVEL(ch) < IMO_SPIRIT) {
						number_items_left++;
						sprintf(buf, "%s tries desperately to remove %s.",
							GET_REAL_NAME(ch),
						OBJS(ch->equipment[idx], ch));
						act(buf, TRUE, ch, 0, 0, TO_ROOM);
						sprintf(buf, "ACK! you can't remove %s.\r\n",
						OBJS(ch->equipment[idx], ch));
						co2900_send_to_char(ch, "%s", buf);
						continue;
					}
					else {
						sprintf(buf, "%s laughs at the feeble curse spell on %s.",
							GET_REAL_NAME(ch),
						OBJS(ch->equipment[idx], ch));
						act(buf, TRUE, ch, 0, 0, TO_ROOM);
						sprintf(buf, "You laugh at the feeble curse spell on %s.\r\n",
						OBJS(ch->equipment[idx], ch));
						co2900_send_to_char(ch, "%s", buf);
					}
				}
				if (CAN_CARRY_N(ch) == IS_CARRYING_N(ch)) {
					number_items_left++;
					send_to_char("You can't carry any more items.\r\n", ch);
					break;
				}
				else {
					obj_object = ch->equipment[idx];
					ha1700_obj_to_char(ha1930_unequip_char(ch, idx), ch);

					if (obj_object->obj_flags.type_flag == ITEM_LIGHT ||
					    obj_object->obj_flags.type_flag == ITEM_QSTLIGHT)
						if (obj_object->obj_flags.value[2])
							world[ch->in_room].light--;
					number_items_removed++;
				}
			}
		}		/* END OF for loop */
		if (number_items_left) {
			if (number_items_removed) {
				act("you removed some of your equipment.", FALSE, ch, 0, 0, TO_CHAR);
				act("$n removes some of $s equipment.", TRUE, ch, 0, 0, TO_ROOM);
			}
			else {
				send_to_char("You didn't remove any equipment\r\n", ch);
			}
			return;
		}

		if (number_items_removed) {
			act("You remove all your equipment.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n removes all $s equipment.", TRUE, ch, 0, 0, TO_ROOM);
		}
		else {
			send_to_char("You don't have any equipment to remove\r\n",
				     ch);
		}
		return;

	}			/* END OF remove all */

	obj_object = in2300_get_object_in_equip_vis(ch, arg1, ch->equipment, &jdx);

	if (!(obj_object)) {
		send_to_char("You are not using it.\r\n", ch);
		return;
	}

	if (IS_SET(GET_OBJ1(obj_object), OBJ1_CURSED)) {
		if (GET_LEVEL(ch) < IMO_SPIRIT) {
			sprintf(buf, "%s tries desperately to remove %s.",
				GET_REAL_NAME(ch),
				OBJS(obj_object, ch));
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "ACK! you can't remove %s.\r\n",
				OBJS(obj_object, ch));
			co2900_send_to_char(ch, "%s", buf);
			return;
		}
		else {
			sprintf(buf, "%s laughs at the feeble curse spell on %s.",
				GET_REAL_NAME(ch),
				OBJS(obj_object, ch));
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			sprintf(buf, "You laugh at the feeble curse spell on %s.\r\n",
				OBJS(obj_object, ch));
			co2900_send_to_char(ch, "%s", buf);
		}
	}			/* END OF ITS CURSED */

	if (CAN_CARRY_N(ch) == IS_CARRYING_N(ch)) {
		send_to_char("You can't carry that many items.\r\n", ch);
		return;
	}

	ha1700_obj_to_char(ha1930_unequip_char(ch, jdx), ch);

	if (obj_object->obj_flags.type_flag == ITEM_LIGHT ||
	    obj_object->obj_flags.type_flag == ITEM_QSTLIGHT) {
		if (obj_object->obj_flags.value[2])
			world[ch->in_room].light--;
	}

	act("You stop using $p.", FALSE, ch, obj_object, 0, TO_CHAR);
	act("$n stops using $p.", TRUE, ch, obj_object, 0, TO_ROOM);

}				/* END OF do_remove */


/* Checks if the item is NODROP or when it 's a containter,
		   if there is a NODROP item in the container. */
int check_nodrop(struct obj_data * container)
{
	struct obj_data *tmp_obj;
	struct char_data *ch;

	if (!container)
		return FALSE;
	ch = container->carried_by;

	if (ch)
		if (IS_PC(ch) && (GET_LEVEL(ch) >= IMO_IMM))
			return FALSE;

	if (IS_SET(GET_OBJ2(container), OBJ2_NODROP))
		return TRUE;

	/* Check container */
	for (tmp_obj = container; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if (IS_SET(GET_OBJ2(tmp_obj), OBJ2_NODROP))
			return TRUE;
		if (GET_ITEM_TYPE(tmp_obj) == ITEM_CONTAINER ||
		    GET_ITEM_TYPE(tmp_obj) == ITEM_QSTCONT) {
			if (check_nodrop(tmp_obj->contains))
				return TRUE;
		}
	}

	/* No NODROP item found in the container */
	return FALSE;
}
/*Checks if character
	already has the LORE item. */
int check_lore(struct char_data * ch, struct obj_data * obj)
{

	int idx;
	struct obj_data *tmp_obj;

	if (!obj)
		return FALSE;

	/* No lore restrictions for Ante + */
	if (IS_PC(ch) && (GET_LEVEL(ch) >= IMO_IMM))
		return FALSE;

	/* Check if obj is a bag, if so check all contents. */
	if (GET_ITEM_TYPE(obj) == ITEM_CONTAINER ||
	    GET_ITEM_TYPE(obj) == ITEM_QSTCONT) {
		for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
			if (check_lore(ch, tmp_obj))
				return TRUE;
		}
	}

	/* Check if item has lore flag */
	if (!IS_SET(GET_OBJ2(obj), OBJ2_LORE))
		return FALSE;

	/* Check worn equipment first */
	for (idx = 0; (idx < MAX_WEAR); idx++) {
		if (ch->equipment[idx]) {
			tmp_obj = ch->equipment[idx];
			if ((tmp_obj->item_number == obj->item_number) && (tmp_obj != obj))
				return TRUE;
			if (GET_ITEM_TYPE(tmp_obj) == ITEM_CONTAINER ||
			    GET_ITEM_TYPE(tmp_obj) == ITEM_QSTCONT) {
				if (check_lore_container(tmp_obj->contains, obj))
					return TRUE;
			}
		}
	}

	//Check inventory
		if (check_lore_container(ch->carrying, obj))
		return TRUE;

	//Check auction system
		if (auction_data.obj) {
		if (auction_data.obj->item_number == obj->item_number) {
			if (auction_data.auctioner == ch)
				return TRUE;
			if (auction_data.bidder)
				if (auction_data.bidder->current == ch)
					return TRUE;
		}
	}

	//Lore item not found
		return FALSE;
}
 /* Checks if the container already contains the LORE item. */
int check_lore_container(struct obj_data * container, struct obj_data * obj)
{

	struct obj_data *tmp_obj;

	//Check container

		if (!container)
		return FALSE;

	for (tmp_obj = container; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if ((tmp_obj->item_number == obj->item_number) && (tmp_obj != obj))
			return TRUE;
		if (GET_ITEM_TYPE(tmp_obj) == ITEM_CONTAINER ||
		    GET_ITEM_TYPE(tmp_obj) == ITEM_QSTCONT) {
			if (check_lore_container(tmp_obj->contains, obj))
				return TRUE;
		}
	}

	//Lore item not found
		return FALSE;
}
 /* Checks if object is a corpse and if so if player is allowed to loot it,
  * returns TRUE when not allowed. */

int check_corpse(struct char_data * ch, struct obj_data * obj, int looting)
{

	if (!ch || !obj)
		return FALSE;

	/* No restrictions for OVERSEER+ */
	if (IS_PC(ch) && (GET_LEVEL(ch) >= IMO_IMM))
		return FALSE;

	if ((GET_ITEM_TYPE(obj) == ITEM_CONTAINER)) {
		if (obj->obj_flags.value[3]) {

			if ((obj->obj_flags.value[1] == ch->nr) && looting) {
				obj->obj_flags.value[1] = -1;
				return FALSE;	/* enable/disable pkloot */
			}

			/* If it's this players corpse just let them loot */
			if (obj->obj_flags.value[2] == ch->nr)
				return FALSE;

			/* If it's a mob corpse (id 0) just let them loot */
			if (obj->obj_flags.value[2] == 0)
				return FALSE;

			/* If they are a pkiller below the level 42 PKILL level
			 * don 't let them loot. */
			return TRUE;
			/* If it 's the killer, only let them loot one item,
			 * not the corpse itself. */


			//Player not allowed to loot or take corpse
				return TRUE;
		}
	}

	return FALSE;
}
