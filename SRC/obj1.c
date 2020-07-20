/* ob */
/* gv_location: 11001-11500 */
/* *********************************************************************
*  file: obj1.c , Implementation of commands.          Part of DIKUMUD *
*  Usage : Commands mainly moving around objects.                      *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "constants.h"
#include "spells.h"
#include "globals.h"
#include "xanth.h"
#include "func.h"


/* extern functions */
extern void ot3700_do_simple_split(struct char_data * ch, long total);
struct obj_data *ha3400_create_money(int amount);

/*
 * Code to keep gold from rolling over:
 * Essentially, we just subtract both the players gold
 * and what the player is trying to get from the Max gold
 * negative, tell them they can't do that and bail.
 * MAX_GOLD is defined in globals.h
 * Relic
 */

/*
 * CHECKS FOR ALL.<item> in argument.
 *     IF FOUND, IT RETURNS A 1 AND argument HAS THE "ALL." REMOVED.
 *     IF NOT FOUND, A ZERO IS RETURNED
 */
int remove_ALL_dot(char *argument)
{

	char buffer[MAX_STRING_LENGTH];
	int i;

	/* IF STRING ISN'T LONG ENOUGHT TO CONTAIN "ALL." THEN EXIT */
	if (strlen(argument) < 4)
		return (FALSE);

	/* CONVERT TO LOWER CASE */
	bzero(buffer, sizeof(buffer));
	for (i = 0; *(argument + i); i++)
		*(buffer + i) = LOWER(*(argument + i));

	/* IF IT DOESN'T HAVE ALL. RETURN FALSE */
	if (strncmp(buffer, "all.", 4) != 0)
		return (FALSE);

	/* WE FOUND IT SO RETURN TRUE */
	return (TRUE);

}				/* END OF remove_ALL_dot( */
/* procedures related to get                             */
/* WHERE:                                                */
/*        display_text BIT0 - display messages to getter */
/*        display_text BIT1 - display messages to room   */
int local_get(struct char_data * ch, struct obj_data * obj_object,
	        struct obj_data * sub_object, int display_text)
{
	char buffer[MAX_STRING_LENGTH];



	if (sub_object) {
		sprintf(buffer, "(obj) do_get %s - %s (%d) from %s", GET_REAL_NAME(ch), OBJS(obj_object, ch), GET_OBJ_VNUM(obj_object), OBJS(sub_object, ch));
		main_log(buffer);
		spec_log(buffer, EQUIPMENT_LOG);

		ha2400_obj_from_obj(obj_object);
		ha1700_obj_to_char(obj_object, ch);
		if (sub_object->carried_by == ch) {
			if (IS_SET(display_text, BIT0)) {
				act("You get $p from $P.", 0, ch, obj_object, sub_object,
				    TO_CHAR);
			}
			if (IS_SET(display_text, BIT1)) {
				act("$n gets $p from $P.", 1, ch, obj_object, sub_object, TO_ROOM);
			}
		}
		else {
			if (IS_SET(display_text, BIT0)) {
				act("You get $p from $P.", 0, ch, obj_object, sub_object,
				    TO_CHAR);
			}
			if (IS_SET(display_text, BIT1)) {
				act("$n gets $p from $P.", 1, ch, obj_object, sub_object, TO_ROOM);
			}
		}		/* END OF object is carried by */
	}
	else {
		sprintf(buffer, "(obj) do_get %s - %s from floor ROOM: %d", GET_REAL_NAME(ch), OBJS(obj_object, ch), world[ch->in_room].number);
		main_log(buffer);
		spec_log(buffer, EQUIPMENT_LOG);
		ha2200_obj_from_room(obj_object);
		ha1700_obj_to_char(obj_object, ch);
		if (IS_SET(display_text, BIT0)) {
			act("You get $p.", 0, ch, obj_object, 0, TO_CHAR);
		}
		if (IS_SET(display_text, BIT1)) {
			act("$n gets $p.", 1, ch, obj_object, 0, TO_ROOM);
		}
	}
	if ((obj_object->obj_flags.type_flag == ITEM_MONEY) &&
	    (obj_object->obj_flags.value[0] >= 1)) {
		ha1800_obj_from_char(obj_object);
		/* check for autosplit */
		if (IS_PC(ch) && GET_LEVEL(ch) < IMO_SPIRIT && IS_AFFECTED(ch, AFF_GROUP) &&
		    IS_SET(GET_ACT3(ch), PLR3_AUTOSPLIT)) {

			/* Can't get more gold than you can carry -- do not
			 * remove without changing do_simple_split */
			if ((MAX_GOLD - GET_GOLD(ch) - obj_object->obj_flags.value[0]) < 0) {
				send_to_char("Too bad you can't hold it all, then you'd be REALLY rich....\r\n", ch);
				ha2100_obj_to_room(obj_object, ch->in_room);
				return 0;
			}
			GET_GOLD(ch) += obj_object->obj_flags.value[0];
			ot3700_do_simple_split(ch, obj_object->obj_flags.value[0]);
		}
		else {
			sprintf(buffer, "There were %d coins.\r\n",
				obj_object->obj_flags.value[0]);
			send_to_char(buffer, ch);
			if ((MAX_GOLD - GET_GOLD(ch) - obj_object->obj_flags.value[0]) < 0) {
				send_to_char("Too bad you can't hold it all, then you'd be REALLY rich....\r\n", ch);
				ha2100_obj_to_room(obj_object, ch->in_room);
				return 0;
			}
			GET_GOLD(ch) += obj_object->obj_flags.value[0];
		}

		/* CHECK FOR IMM GETTING CASH */
		if (obj_object->obj_flags.type_flag == ITEM_MONEY) {
			if (GET_LEVEL(ch) > PK_LEV ||
			    obj_object->obj_flags.value[0] > 5000000) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s level %d picked up %d coins.",
					GET_NAME(ch),
					GET_LEVEL(ch),
					obj_object->obj_flags.value[0]);
				do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
				bzero(buffer, sizeof(buffer));
			}
		}		/* ITEM TYPE MONEY */

		/* REMOVE ITEM FROM GAME */
		ha2700_extract_obj(obj_object);
	}
	return 1;
}

void do_get(struct char_data * ch, char *argument, int cmd)
{
	char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH], lv_name[MAX_STRING_LENGTH];
	struct obj_data *sub_object, *obj_object, *sav_object, *next_obj;
	bool found = FALSE;
	bool fail = FALSE;
	int local_get_success;
	int type = 3;
	int did_we_find_all_dot;
	int number_items;
	int lv_flag;
	int affected_weight;	/* Number to hold the weight subtracted from an
				 * obj you get from a container in your
				 * inventory. */



	bzero(lv_name, sizeof(lv_name));
	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;

	argument_interpreter(argument, arg1, arg2);

	/* get type */
	if (!*arg1) {
		send_to_char("Get what?\r\n", ch);
		return;
	}

	if (*arg1 && !*arg2) {
		if (!str_cmp(arg1, "all")) {
			type = 1;	/* GET ALL */
		}
		else {
			type = 2;	/* GET <item> */
		}
	}

	if (*arg1 && *arg2) {
		if (!str_cmp(arg1, "all")) {
			if (!str_cmp(arg2, "all")) {
				type = 3;	/* GET ALL ALL */
			}
			else {
				type = 4;	/* GET ALL <CONTAINER> */
			}
		}
		else {
			if (!str_cmp(arg2, "all")) {
				type = 5;	/* GET <ITEM> ALL */
			}
			else {
				type = 6;	/* GET <ITEM> <CONTAINER */
			}
		}
	}

	switch (type) {

		/* *** CASE ONE *** */
		/* get all */
	case 1:{
			sub_object = 0;
			found = FALSE;
			fail = FALSE;
			for (obj_object = world[ch->in_room].contents; obj_object; obj_object = next_obj) {
				/* Loop through all the obj's in the room */
				next_obj = obj_object->next_content;
				if (CAN_SEE_OBJ(ch, obj_object)) {
					if (((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)) || (GET_LEVEL(ch) >= IMO_IMM)) {
						if (((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) <= CAN_CARRY_W(ch)) || (GET_LEVEL(ch) >= IMO_IMM)) {
							if (check_lore(ch, obj_object)) {
								sprintf(buffer, "You already have %s.\r\n", OBJS(obj_object, ch));
								send_to_char(buffer, ch);
								fail = TRUE;
							}
							else if (check_corpse(ch, obj_object, 0)) {
								send_to_char("That isn't your corpse, you can't touch it!\r\n", ch);
								fail = TRUE;
							}
							else if ((CAN_WEAR(obj_object, ITEM_TAKE)) || (GET_LEVEL(ch) >= IMO_IMP)) {
								local_get_success = local_get(ch, obj_object, sub_object, BIT0 | BIT1);
								found = TRUE;
							}
							else {
								bzero(buffer, sizeof(buffer));
								sprintf(buffer,
									"You can't take %s\r\n",
									GET_OBJ_NAME(obj_object));
								send_to_char(buffer, ch);
								fail = TRUE;
							}
						}	/* END OF IS CARRYING_W */
						else {
							sprintf(buffer, "%s : You can't carry that much weight.\r\n",
								GET_OBJ_NAME(obj_object));
							send_to_char(buffer, ch);
							fail = TRUE;
						}
					}	/* END OF IS CARRYING IF */
					else {
						sprintf(buffer, "%s : You can't carry that many items.\r\n",
						    GET_OBJ_NAME(obj_object));
						send_to_char(buffer, ch);
						fail = TRUE;
					}
				}	/* END OF CAN SEE IF */

			}	/* END OF FOR LOOP */

			if (found)
				send_to_char("Ok.\r\n", ch);
			else if (!fail)
				send_to_char("You see nothing here.\r\n", ch);

		}		/* END OF CASE 1 */
		break;

		/* *** CASE TWO *** */
		/* get <ITEM> */
	case 2:{
			sub_object = 0;
			found = FALSE;

			/* LETS DO IT */
			number_items = 0;
			strcpy(lv_name, arg1);
			while (1) {

				obj_object = ha2075_get_obj_list_vis(ch, arg1,
						 world[ch->in_room].contents);

				/* IS THE ITEM AVAILABLE */
				if (!(obj_object)) {
					/* DID WE FIND AT LEAST ONE ITEM? */
					if (number_items == 0) {
						sprintf(buffer, "You do not see %s here.\r\n", arg1);
						send_to_char(buffer, ch);
						fail = TRUE;
					}
					break;
				}

				/* WE HAVE AN OBJECT, SET lv_name */
				if (obj_object->short_description)
					strcpy(lv_name, obj_object->short_description);
				else if (obj_object->description)
					strcpy(lv_name, obj_object->description);
				else if (obj_object->name)
					strcpy(lv_name, obj_object->name);

				/* DO YOU HAVE A SLOT FOR IT? */
				if ((IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch))) {
					/* DID WE FIND AT LEAST ONE ITEM? */
					if (number_items == 0) {
						sprintf(buffer, "Sorry, but getting %s would exceed the number of items you can carry\r\n",
							ha1100_fname(obj_object->name));
						send_to_char(buffer, ch);
						fail = TRUE;
					}
					else {
						send_to_char("You can't carry any more items.\r\n", ch);
						fail = TRUE;
						break;
					}
					break;
				}

				/* CAN YOU CARRY THE WEIGHT */
				if (((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) >= CAN_CARRY_W(ch)) && (GET_LEVEL(ch) < IMO_IMP)) {
					if (number_items == 0) {
						sprintf(buffer, "Sorry, but getting %s would exceed the weight you can carry\r\n",
							ha1100_fname(obj_object->name));
						send_to_char(buffer, ch);
						fail = TRUE;
						break;
					}
					else {
						send_to_char("You can't carry anymore weight\r\n", ch);
						fail = TRUE;
						break;
					}
				}

				if (check_lore(ch, obj_object)) {
					if (number_items == 0) {
						sprintf(buffer, "You already have %s.\r\n", OBJS(obj_object, ch));
						send_to_char(buffer, ch);
						fail = TRUE;
						break;
					}
					else {
						send_to_char("You can't carry anymore of these.\r\n", ch);
						fail = TRUE;
						break;
					}
				}

				if (check_corpse(ch, obj_object, 0)) {
					send_to_char("That isn't your corpse, you can't touch it!\r\n", ch);
					fail = TRUE;
					break;
				}

				/* CAN YOU TAKE THE ITEM? */
				if ((!(CAN_WEAR(obj_object, ITEM_TAKE))) && (GET_LEVEL(ch) < IMO_IMP)) {
					if (number_items == 0) {
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,
						     "You can't take %s.\r\n",
							ha1100_fname(obj_object->name));
						send_to_char(buffer, ch);
						fail = TRUE;
						break;
					}
					else {
						/* WE SHOULD NEVER GET HERE,
						 * BUT... */
						/* unless someone types get
						 * all.obj */
						send_to_char("Your unable to take the next one\r\n", ch);
						fail = TRUE;
						break;
					}
				}

				/* TAKE ITEM */
				local_get_success = local_get(ch, obj_object, sub_object, 0);
				found = TRUE;
				number_items++;

				/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER
				 * DOING ONE ITEM */
				if (did_we_find_all_dot != TRUE)
					break;

			}	/* END OF WHILE(1) */

			/* IF THERE WAS MORE THAN ONE, LET THE PERSON KNOW */
			if (local_get_success) {
				if (number_items == 1) {
					send_to_char("You get it.\r\n", ch);
					bzero(buffer, sizeof(buffer));
					sprintf(buffer, "%s picks up %s.\r\n",
						GET_NAME(ch), lv_name);
					act(buffer, 1, ch, 0, 0, TO_ROOM);
				}
				if (number_items > 1) {
					bzero(buffer, sizeof(buffer));
					sprintf(buffer, "You got %d of them.\r\n",
						number_items);
					send_to_char(buffer, ch);
					bzero(buffer, sizeof(buffer));
					sprintf(buffer, "%s gets several %s.\r\n",
						GET_NAME(ch), lv_name);
					act(buffer, 1, ch, 0, 0, TO_ROOM);
				}
			}

		}		/* END OF CASE 2 */
		break;


		/* get all all */
	case 3:{
			send_to_char("You must be joking?!\r\n", ch);
		} break;
		/* get all <CONTAINER> */
	case 4:{
			number_items = 0;
			found = FALSE;
			fail = FALSE;
			sub_object = ha2075_get_obj_list_vis(ch, arg2, world[ch->in_room].contents);
			if (!sub_object)
				sub_object = ha2075_get_obj_list_vis(ch, arg2, ch->carrying);
			if (sub_object) {
				if ((GET_ITEM_TYPE(sub_object) == ITEM_CONTAINER) ||
				(GET_ITEM_TYPE(sub_object) == ITEM_QSTCONT)) {
					if ((IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED)) &&
					    (sub_object->obj_flags.value[3] != 1)) {
						send_to_char("It seems to be closed.\r\n", ch);
						return;
					}
					for (obj_object = sub_object->contains; obj_object; obj_object = next_obj) {
						/* Loop that goes through all
						 * the objs in the container */
						next_obj = obj_object->next_content;
						if (CAN_SEE_OBJ(ch, obj_object)) {	/* Can they see it? */
							if (((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) || (GET_LEVEL(ch) >= IMO_IMM)) {
								/* ^ Inventory
								 * slots ^ */
								if (((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) < CAN_CARRY_W(ch)) ||
								    (GET_LEVEL(ch) >= IMO_IMM)) {	/* Weight, but become
													 * immune at a certain
													 * level */
									if (check_lore(ch, obj_object)) {	/* LORE */
										sprintf(buffer, "You already have %s.\r\n", OBJS(obj_object, ch));
										send_to_char(buffer, ch);
										fail = TRUE;
									}
									else {	/* not LORE */
										if ((GET_ITEM_TYPE(obj_object) == ITEM_CONTAINER) ||
										    (GET_ITEM_TYPE(obj_object) == ITEM_QSTCONT))
											lv_flag = 0;
										else
											lv_flag = 1;

										if (check_corpse(ch, sub_object, lv_flag)) {	/* Someone else's corpse */
											send_to_char("That isn't your corpse, you can't touch it!\r\n", ch);
											fail = TRUE;
											break;
										}

										if ((CAN_WEAR(obj_object, ITEM_TAKE)) || (GET_LEVEL(ch) >= IMO_IMP)) {
											/* G
											 * E
											 * T
											 * 
											 * TH
											 * E
											 * 
											 * OB
											 * J
											 * E
											 * C
											 * T
											 * 
											 * (i
											 * f
											 * 
											 * yo
											 * u
											 * 
											 * ca
											 * n
											 * )
											 *  */
											number_items++;
											if (GET_ITEM_TYPE(obj_object) == ITEM_MONEY)
												sav_object = NULL;
											else
												sav_object = obj_object;
											local_get_success = local_get(ch, obj_object, sub_object, BIT0);
											found = TRUE;
										}
										else {
											send_to_char("You can't take that\r\n", ch);
											fail = TRUE;
										}
									}
								}
								else {
									sprintf(buffer, "%s : You can't carry that much weight.\r\n",
										ha1100_fname(obj_object->name));
									send_to_char(buffer, ch);
									fail = TRUE;
								}
							}
							else {
								sprintf(buffer, "%s : You can't carry that many items.\r\n",
									ha1100_fname(obj_object->name));
								send_to_char(buffer, ch);
								fail = TRUE;
							}
						}
					}	/* End of all in container loop */

/* Now, tell everyone what happened */

					if (number_items == 1 && local_get_success) {
						if (sav_object)
							act("$n gets $p from $P.", 1, ch, sav_object, sub_object, TO_ROOM);
						else
							act("$n gets coins from $P.", 1, ch, sub_object, sub_object, TO_ROOM);
					}

					if (number_items > 1) {
						act("$n gets several items from $P", 1, ch, sub_object, sub_object, TO_ROOM);
					}

					if (!found && !fail) {
						sprintf(buffer, "You do not see anything in %s.\r\n",
							ha1100_fname(sub_object->name));
						send_to_char(buffer, ch);
						fail = TRUE;
					}
				}
				else {	/* end of if it's a container */
					sprintf(buffer, "The %s is not a container.\r\n",
					      ha1100_fname(sub_object->name));
					send_to_char(buffer, ch);
					fail = TRUE;
				}
			}
			else {	/* end of can_see */
				sprintf(buffer, "You do not see or have %s.\r\n", arg2);
				send_to_char(buffer, ch);
				fail = TRUE;
			}
		}		/* end of if (sub_object) */
		break;
/* get <ITEM> all */
	case 5:{
			send_to_char("You can't take a thing from more than one container.\r\n", ch);
		}
		break;
/* get <ITEM> <CONTAINER> */
	case 6:{
			found = FALSE;
			fail = FALSE;
			sub_object = ha2075_get_obj_list_vis(ch, arg2, ch->carrying);
			if (!sub_object) {
				sub_object = ha2075_get_obj_list_vis(ch, arg2, world[ch->in_room].contents);
			}

			if (!sub_object) {
				sprintf(buffer, "You do not see or have %s.\r\n", arg2);
				send_to_char(buffer, ch);
				fail = TRUE;
				break;
			}

			/* IS THE ITEM A CONTAINER? */
			if (GET_ITEM_TYPE(sub_object) != ITEM_CONTAINER &&
			    GET_ITEM_TYPE(sub_object) != ITEM_QSTCONT) {
				sprintf(buffer, "The %s is not a container.\r\n", ha1100_fname(sub_object->name));
				send_to_char(buffer, ch);
				fail = TRUE;
				break;
			}

			if (IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED) &&
			    (sub_object->obj_flags.value[3] != 1) &&
			    GET_ITEM_TYPE(sub_object) != ITEM_QSTCONT) {
				send_to_char("It seems to be closed.\r\n", ch);
				return;
			}

			/* LETS DO IT */
			number_items = 0;
			while (1) {

				obj_object = ha2075_get_obj_list_vis(ch, arg1, sub_object->contains);

				/* IS THE ITEM AVAILABLE */
				if (!obj_object) {
					/* DID WE FIND AT LEAST ONE ITEM? */
					if (number_items == 0) {
						sprintf(buffer, "The %s does not contain %s.\r\n",
							ha1100_fname(sub_object->name), arg1);
						send_to_char(buffer, ch);
						fail = TRUE;
					}
					break;
				}

				/* DO YOU HAVE A SLOT FOR IT? */
				if (((IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch))) && (GET_LEVEL(ch) < IMO_IMM)) {
					if (number_items == 0) {
						sprintf(buffer, "%s : You can't carry that many items.\r\n",
							ha1100_fname(obj_object->name));
						send_to_char(buffer, ch);
						fail = TRUE;
					}
					else {
						send_to_char("You can't carry any more items.\r\n", ch);
					}
					break;
				}


				/* CAN YOU CARRY THE WEIGHT? */

				/* If it we find it in the sub_object, and
				 * we're moving it to the inventory, don't
				 * count its weight in the container it was in */
				affected_weight = (obj_object) ? obj_object->obj_flags.weight : 0;

				if ((((IS_CARRYING_W(ch) - affected_weight) + obj_object->obj_flags.weight >= CAN_CARRY_W(ch))
				     && (GET_LEVEL(ch) < IMO_IMM))) {
					if (number_items == 0) {
						sprintf(buffer, "%s : You can't carry that much weight. (%d)\r\n",
							ha1100_fname(obj_object->name), affected_weight);
						send_to_char(buffer, ch);
						fail = TRUE;
					}
					else {
						send_to_char("You can't carry anymore weight\r\n", ch);
					}
					break;
				}

				if (check_lore(ch, obj_object)) {
					if (number_items == 0) {
						sprintf(buffer, "You already have %s.\r\n", OBJS(obj_object, ch));
						send_to_char(buffer, ch);
						fail = TRUE;
						break;
					}
					else {
						send_to_char("You can't carry anymore of these.\r\n", ch);
					}
				}

				if ((!(CAN_WEAR(obj_object, ITEM_TAKE))) && (GET_LEVEL(ch) < IMO_IMP)) {
					send_to_char("You can't take that\r\n", ch);
					fail = TRUE;
					/* abc - I think we need a break here */
					break;
				}

				if (GET_ITEM_TYPE(obj_object) == ITEM_CONTAINER ||
				    GET_ITEM_TYPE(obj_object) == ITEM_QSTCONT)
					lv_flag = 0;
				else
					lv_flag = 1;

				if (IS_PERSONAL(obj_object) && sub_object->obj_flags.value[1] == ch->nr) {
					send_to_char("You can't loot personal items.\r\n", ch);
					sub_object->obj_flags.value[1] = ch->nr;
					fail = TRUE;
					break;
				}

				if (check_corpse(ch, sub_object, lv_flag)) {
					if (lv_flag == 0 && sub_object->obj_flags.value[1] == ch->nr) {
						send_to_char("You can't loot containers\r\n", ch);

					}
					else {
						send_to_char("That isn't your corpse, you can't touch it!\r\n", ch);
					}
					fail = TRUE;
					break;
				}

				/* TAKE ITEM FROM CONTAINER */
				local_get(ch, obj_object, sub_object, BIT0 | BIT1);
				found = TRUE;
				number_items++;

				/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER
				 * DOING ONE ITEM */
				if (did_we_find_all_dot != TRUE)
					break;

			}	/* END OF WHILE (1) */

			/* IF THERE WAS MORE THAN ONE, LET THE PERSON KNOW */
			if (number_items > 1) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "You got %d of them\r\n", number_items);
				send_to_char(buffer, ch);
			}
			break;


		}		/* END OF CASE 6 */

		break;

	}			/* END OF SWITCH */
	return;

}				/* END OF DO_GET() */


void do_donate(struct char_data * ch, char *argument, int cmd)
{
	char arg[MAX_STRING_LENGTH];
	int amount;
	char buffer[MAX_STRING_LENGTH];
	struct obj_data *tmp_object;
	struct obj_data *next_obj;
	bool test = FALSE;
	int did_we_find_all_dot;
	int number_items;
	int lv_donate_room;
	int location;
	int clan_dona_room;



	clan_dona_room = GET_CLAN_DON_ROOM(CLAN_NUMBER(ch));
	if (cmd == CMD_CLAN_DONATE) {
		for (location = 0; location <= top_of_world; location++) {
			if (world[location].number == clan_dona_room)
				break;
			if (location == top_of_world) {
				send_to_char("Error! Clan donationroom not found!\r\n", ch);
				return;
			}
		}
		lv_donate_room = location;
	}
	else
		lv_donate_room = gv_donate_room;

	if (ch->in_room < 0) {
		send_to_char("Hrmmm, your room number is negative!\r\n", ch);
		return;
	}

	/* IS ROOM NO DONATE? */
	if (IS_SET(GET_ROOM2(ch->in_room), RM2_NO_DONATE)) {
		send_to_char("You can't donate here!\r\n", ch);
		return;
	}

	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;


	if (IS_NPC(ch) &&
	    !IS_AFFECTED(ch, AFF_CHARM))
		return;

	argument = one_argument(argument, arg);

	if (is_number(arg)) {
		amount = atoi(arg);
		argument = one_argument(argument, arg);
		if (str_cmp("coins", arg) && str_cmp("coin", arg)) {
			send_to_char("Sorry, you can't do that...\r\n", ch);
			return;
		}
		send_to_char("Sorry, Donating coins is not allowed.\r\n", ch);
		return;
		/*
		 if(amount<0)
		 {
			 send_to_char("Sorry, you can't do that!\r\n",ch);
			 return;
		 }
		 if(GET_GOLD(ch) < amount)
		 {
			 send_to_char("You haven't got that many coins!\r\n",ch);
			 return;
		 }
		 send_to_char("OK.\r\n",ch);
		 if(amount==0)
		 return;
		
		 if (GET_LEVEL(ch) >= IMO_LEV) {
			 bzero(buffer, sizeof(buffer));
			 sprintf(buffer,"%s level %d donated %d coins.",
							 GET_NAME(ch),
							 GET_LEVEL(ch),
							 amount);
			 do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
			 bzero(buffer, sizeof(buffer));
		 }
		
		 act("$n donates some gold.", FALSE, ch, 0, 0, TO_ROOM);
		 tmp_object = ha3400_create_money(amount);
		 ha2100_obj_to_room(tmp_object,lv_donate_room);
		 GET_GOLD(ch)-=amount;
		 return;
		 */
	}

	if (!(*arg)) {
		send_to_char("Donate what?\r\n", ch);
		return;
	}

	if (!str_cmp(arg, "all")) {
		for (tmp_object = ch->carrying; tmp_object; tmp_object = next_obj) {
			next_obj = tmp_object->next_content;

			if (GET_LEVEL(ch) > IMO_IMM ||
			 !IS_SET(tmp_object->obj_flags.flags1, OBJ1_CURSED) ||
			!IS_SET(tmp_object->obj_flags.flags1, OBJ1_PERSONAL)) {
				if (check_nodrop(tmp_object)) {
					sprintf(buffer, "You can't donate %s.\r\n", OBJS(tmp_object, ch));
					send_to_char(buffer, ch);
				}
				else {
					if (CAN_SEE_OBJ(ch, tmp_object)) {
						sprintf(buffer, "You donate %s.\r\n", OBJS(tmp_object, ch));
						send_to_char(buffer, ch);
					}
					else {
						send_to_char("You donate something.\r\n", ch);
					}
					act("$n donated $p.", 1, ch, tmp_object, 0, TO_ROOM);
					sprintf(buffer, "(obj) do_donate %s - %s", GET_REAL_NAME(ch), OBJS(tmp_object, ch));
					main_log(buffer);
					spec_log(buffer, EQUIPMENT_LOG);
					ha1800_obj_from_char(tmp_object);
					ha2100_obj_to_room(tmp_object, lv_donate_room);
					ha2800_update_object_timer(tmp_object, TIMER_USE_DROP);
					test = TRUE;
				}
			}
			else {
				if (CAN_SEE_OBJ(ch, tmp_object)) {
					if (IS_SET(tmp_object->obj_flags.flags1, OBJ1_CURSED))
						sprintf(buffer, "You can't donate %s, it must be CURSED!\r\n",
							ha1100_fname(tmp_object->name));
					if (IS_SET(tmp_object->obj_flags.flags1, OBJ1_PERSONAL))
						sprintf(buffer, "You can't donate %s, it is a personal item.\r\n",
							ha1100_fname(tmp_object->name));

					send_to_char(buffer, ch);
					test = TRUE;
				}
			}
		}

		if (!test) {
			send_to_char("You do not seem to have anything.\r\n", ch);
		}
		return;
	}			/* END OF DROP ALL */

	number_items = 0;
	while (1) {

		tmp_object = ha2075_get_obj_list_vis(ch, arg, ch->carrying);

		if (!tmp_object) {
			if (number_items == 0) {
				send_to_char("You do not have that item.\r\n", ch);
			}
			break;
		}

		if (check_nodrop(tmp_object)) {
			send_to_char("You can't donate it.\r\n", ch);
			break;
		}

		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(tmp_object->obj_flags.flags1, OBJ1_CURSED)) {
			send_to_char("You can't donate it, it must be CURSED!\r\n", ch);
			break;
		}

		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(tmp_object->obj_flags.flags1, OBJ1_PERSONAL)) {
			send_to_char("You can't donate personal items!\r\n", ch);
			break;
		}

		/* DROP THE ITEM */
		sprintf(buffer, "(obj) do_donate %s - %s", GET_REAL_NAME(ch), OBJS(tmp_object, ch));
		main_log(buffer);
		spec_log(buffer, EQUIPMENT_LOG);

		ha1800_obj_from_char(tmp_object);
		ha2100_obj_to_room(tmp_object, lv_donate_room);
		ha2800_update_object_timer(tmp_object, TIMER_USE_DROP);
		number_items++;

		/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER DOING ONE ITEM */
		if (did_we_find_all_dot != TRUE)
			break;


	}			/* END OF while(1) */

	/* IF THERE WAS MORE THAN ONE, LET THE PERSON KNOW */
	if (number_items == 1) {
		sprintf(buffer, "You donated it.\r\n");
		send_to_char(buffer, ch);
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n donated %s.\r\n", arg);
		act(buffer, 1, ch, 0, 0, TO_ROOM);
	}
	else {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "You donated %d of them\r\n", number_items);
		send_to_char(buffer, ch);
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n donates several %s.\r\n", arg);
		act(buffer, 1, ch, 0, 0, TO_ROOM);
	}
	return;

}				/* END OF do_donate() */



void do_drop(struct char_data * ch, char *argument, int cmd)
{
	char arg[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH], lv_name[MAX_STRING_LENGTH];
	int amount;
	struct obj_data *tmp_object;
	struct obj_data *next_obj;
	bool test = FALSE;
	int did_we_find_all_dot;
	int number_items;




	bzero(lv_name, sizeof(lv_name));
	strcpy(lv_name, argument);

	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;


	if (IS_NPC(ch) &&
	    !(IS_AFFECTED(ch, AFF_CHARM)) &&
	    gv_switched_orig_level < IMO_IMP)
		return;

	argument = one_argument(argument, arg);

	if (is_number(arg)) {
		amount = atoi(arg);
		argument = one_argument(argument, arg);
		if (str_cmp("coins", arg) && str_cmp("coin", arg)) {
			send_to_char("Sorry, you can't do that (yet)...\n\r", ch);
			return;
		}
		if (amount < 0) {
			send_to_char("Sorry, you can't do that!\n\r", ch);
			return;
		}
		if (GET_GOLD(ch) < amount) {
			send_to_char("You haven't got that many coins!\n\r", ch);
			return;
		}
		send_to_char("OK.\n\r", ch);
		if (amount == 0)
			return;

		/* CHECK FOR IMM DROPPING CASH */
		/* if (GET_LEVEL(ch) >= IMO_LEV) { */
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "%s level %d dropped %d coins.",
			GET_NAME(ch),
			GET_LEVEL(ch),
			amount);
		do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
		bzero(buffer, sizeof(buffer));
		/* } */

		act("$n drops some gold.", FALSE, ch, 0, 0, TO_ROOM);
		tmp_object = ha3400_create_money(amount);
		ha2100_obj_to_room(tmp_object, ch->in_room);
		GET_GOLD(ch) -= amount;
		return;
	}

	if (!(*arg)) {
		send_to_char("Drop what?\n\r", ch);
		return;
	}

	if (!str_cmp(arg, "all")) {
		for (tmp_object = ch->carrying; tmp_object; tmp_object = next_obj) {
			next_obj = tmp_object->next_content;
			if (GET_LEVEL(ch) > IMO_IMM ||
			    (!IS_SET(GET_OBJ1(tmp_object), OBJ1_CURSED) && !check_nodrop(tmp_object) &&
			     !IS_SET(tmp_object->obj_flags.flags1, OBJ1_PERSONAL))) {
				sprintf(buffer, "(obj) do_drop %s - %s ROOM: %d", GET_REAL_NAME(ch), OBJS(tmp_object, ch),
					world[ch->in_room].number);
				main_log(buffer);
				spec_log(buffer, EQUIPMENT_LOG);
				if ((CAN_SEE_OBJ(ch, tmp_object))) {
					sprintf(buffer, "You drop %s.\n\r", OBJS(tmp_object, ch));
					send_to_char(buffer, ch);
				}
				else {
					send_to_char("You drop something.\n\r", ch);
				}
				act("$n drops $p.", FALSE, ch, tmp_object, 0, TO_ROOM);
				ha1800_obj_from_char(tmp_object);
				ha2100_obj_to_room(tmp_object, ch->in_room);
				ha2800_update_object_timer(tmp_object, TIMER_USE_DROP);
				test = TRUE;
			}
			else {
				if (CAN_SEE_OBJ(ch, tmp_object)) {
					sprintf(buffer, "You can't drop %s, it must be CURSED!\n\r",
					      ha1100_fname(tmp_object->name));
					send_to_char(buffer, ch);
					test = TRUE;
				}
			}
		}
		if (!test) {
			send_to_char("You do not seem to have anything.\n\r", ch);
		}
		return;
	}			/* END OF DROP ALL */

	number_items = 0;
	strcpy(lv_name, arg);
	while (1) {
		tmp_object = ha2075_get_obj_list_vis(ch, arg, ch->carrying);

		if (!tmp_object) {
			if (number_items == 0) {
				send_to_char("You do not have that item.\n\r", ch);
			}
			break;
		}

		/* WE HAVE AN OBJECT, SET lv_name */
		if (tmp_object->short_description)
			strcpy(lv_name, tmp_object->short_description);
		else if (tmp_object->description)
			strcpy(lv_name, tmp_object->description);
		else if (tmp_object->name)
			strcpy(lv_name, tmp_object->name);

		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(tmp_object->obj_flags.flags1, OBJ1_CURSED)) {
			send_to_char("You can't drop it, it must be CURSED!\n\r", ch);
			return;
		}

		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(tmp_object->obj_flags.flags1, OBJ1_PERSONAL)) {
			send_to_char("You can't drop personal items.\r\n", ch);
			return;
		}

		if (check_nodrop(tmp_object)) {
			send_to_char("You can't drop this item.\r\n", ch);
			return;
		}

		/* DROP THE ITEM */
		sprintf(buffer, "(obj) do_drop %s - %s ROOM: %d", GET_REAL_NAME(ch),
			OBJS(tmp_object, ch), world[ch->in_room].number);
		main_log(buffer);
		spec_log(buffer, EQUIPMENT_LOG);

		ha1800_obj_from_char(tmp_object);
		ha2100_obj_to_room(tmp_object, ch->in_room);

		ha2800_update_object_timer(tmp_object, TIMER_USE_DROP);

		number_items++;

		/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER DOING ONE ITEM */
		if (did_we_find_all_dot != TRUE)
			break;

	}			/* END OF while(1) */

	/* IF THERE WAS MORE THAN ONE, LET THE PERSON KNOW */
	if (number_items == 1) {
		sprintf(buffer, "You dropped it.\n\r");
		send_to_char(buffer, ch);
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n drops %s.\r\n", lv_name);
		act(buffer, FALSE, ch, 0, 0, TO_ROOM);
	}
	else if (number_items > 1) {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "You dropped %d of them.\r\n", number_items);
		send_to_char(buffer, ch);
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n drops several %s.\r\n", lv_name);
		act(buffer, FALSE, ch, 0, 0, TO_ROOM);
	}

	return;

}				/* END OF do_drop () */


void do_put(struct char_data * ch, char *argument, int cmd)
{
	char buffer[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	struct obj_data *obj_object;
	struct obj_data *sub_object;
	struct char_data *tmp_char;
	int did_we_find_all_dot;
	int number_items;
	int bits;



	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;

	/* DID WE SPECIFY WHAT TO PUT? */
	argument_interpreter(argument, arg1, arg2);
	if (!(*arg1)) {
		send_to_char("Put what in what?\n\r", ch);
		return;
	}

	/* DID WE SPECIFY A CONTAINER? */
	if (!(*arg2)) {
		sprintf(buffer, "Put %s in what?\n\r", arg1);
		send_to_char(buffer, ch);
		return;
	}

	/* DO WE HAVE WHAT WE ARE PUTTING? */
	obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
	if (!(obj_object)) {
		sprintf(buffer, "You dont have %s.\n\r", arg1);
		send_to_char(buffer, ch);
		return;
	}


	/* DO WE HAVE CONTAINER? */
	bits = ha3500_generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &sub_object);
	if (!(sub_object)) {
		sprintf(buffer, "You dont have %s.\n\r", arg2);
		send_to_char(buffer, ch);
		return;
	}

	/* IS THE CONTAINER REALLY A CONTAINER? */
	if (GET_ITEM_TYPE(sub_object) != ITEM_CONTAINER &&
	    GET_ITEM_TYPE(sub_object) != ITEM_QSTCONT) {
		sprintf(buffer, "The %s is not a container.\n\r", ha1100_fname(sub_object->name));
		send_to_char(buffer, ch);
		return;
	}

	/* IS THE CONTAINER OPEN? */
	if (IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED) &&
	    (sub_object->obj_flags.value[3] != 1) &&
	    (GET_ITEM_TYPE(sub_object) != ITEM_QSTCONT)) {
		send_to_char("It seems to be closed.\n\r", ch);
		return;
	}

	/* ARE WE TRYING TO PUT A BAG IN A BAG? */
	if (obj_object == sub_object) {
		send_to_char("You attempt to fold it into itself, but fail.\n\r", ch);
		return;
	}

	if ((GET_ITEM_TYPE(obj_object) == ITEM_CONTAINER ||
	     GET_ITEM_TYPE(obj_object) == ITEM_QSTCONT) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You cannot put containers in containers!\n\r", ch);
		return;
	}

	/* LETS DO IT */
	number_items = 0;
	while (1) {

		/* HAVE ALL OF THE ITEMS BEEN PUT IN THE CONTAINER? */
		obj_object = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
		if (!(obj_object)) {
			break;
		}

		/* ARE WE TRYING TO PUT A BAG IN A BAG? */
		if (obj_object == sub_object) {
			send_to_char("You attempt to fold an item into itself, but fail.\n\r", ch);
			break;
		}

		/* WILL IT FIT? */
		if ((((sub_object->obj_flags.weight) + (obj_object->obj_flags.weight)) >=
		     (sub_object->obj_flags.value[0]))) {
			/* DID WE PUT AT LEAST ONE ITEM IN THE CONTAINER? */
			if (number_items == 0)
				send_to_char("It won't fit.\n\r", ch);
			else {
				sprintf(buffer, "Bummer!!! The %s is full and won't hold anymore %s.\r\n",
					arg2, arg1);
				send_to_char(buffer, ch);
			}
			break;
		}

		if (sub_object->carried_by != ch) {
			if (check_nodrop(obj_object)) {
				send_to_char("You can't drop this.\r\n", ch);
				break;
			}
			if (check_lore_container(sub_object, obj_object)) {
				sprintf(buffer, "You can't put anymore %s in %s.\r\n", OBJS(obj_object, ch), arg2);
				send_to_char(buffer, ch);
				break;
			}
		}

		if (bits == FIND_OBJ_INV) {
			/* if ((GET_ITEM_TYPE(obj_object) == ITEM_QUEST ||
			 * GET_ITEM_TYPE(obj_object) == ITEM_QSTWEAPON ||
			 * GET_ITEM_TYPE(obj_object) == ITEM_QSTLIGHT) &&
			 * GET_LEVEL(ch) < IMO_IMP) { send_to_char("You cannot
			 * put quest items in containers (for now).\n\r", ch);
			 * return; } */
			sprintf(buffer, "(obj) do_put %s - %s in %s", GET_REAL_NAME(ch), OBJS(obj_object, ch), OBJS(sub_object, ch));
			main_log(buffer);
			spec_log(buffer, EQUIPMENT_LOG);

			ha1800_obj_from_char(obj_object);
			/* make up for above line */
			IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(obj_object);
			ha2300_obj_to_obj(obj_object, sub_object);
		}
		else {
			sprintf(buffer, "(obj) do_put %s - %s in %s", GET_REAL_NAME(ch), OBJS(obj_object, ch), OBJS(sub_object, ch));
			main_log(buffer);
			spec_log(buffer, EQUIPMENT_LOG);
			ha1800_obj_from_char(obj_object);
			/* Do we need ha2200_obj_from_room???(sub_object,....); */
			ha2300_obj_to_obj(obj_object, sub_object);
			/* Do we need ha2100_obj_to_room???(sub_object,ch);    */
		}

		/* WE SUCCESSFULLY PUT AN ITEM SO INCREMENT COUNTER */
		number_items++;

		/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER DOING ONE ITEM */
		if (did_we_find_all_dot != TRUE)
			break;

	}			/* END OF FOR number_items LOOP */

	/* IF SOMETHING WENT WRONG, WE'VE ALREADY NOTIFIED THE USER */
	if (number_items < 1)
		return;

	/* DID WE ONLY PUT ONE ITEM IN THE CONTAINER? */
	if (number_items < 2) {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "You put %s in %s.\r\n",
			arg1, OBJS(sub_object, ch));	/* arg2 */
		send_to_char(buffer, ch);

		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n puts %s in %s.",
			arg1, OBJS(sub_object, ch));
		act(buffer, TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "You put %d %s in %s.\r\n", number_items,
			arg1, OBJS(sub_object, ch));
		send_to_char(buffer, ch);

		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "$n puts several %s in %s",
			arg1, OBJS(sub_object, ch));
		act(buffer, TRUE, ch, 0, 0, TO_ROOM);
	}

	return;

}				/* END OF DO PUT */


void do_give(struct char_data * ch, char *argument, int cmd)
{

	char obj_name[MAX_INPUT_LENGTH], vict_name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH], org_arg[MAX_STRING_LENGTH], logOut[MAX_STRING_LENGTH], buffer[MAX_INPUT_LENGTH];
	int amount /* , gold_extra */ ;
	struct char_data *vict;
	struct obj_data *obj;

	/* if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM) && cmd != CMD_QUEST )
	 * { return; } */

	if (argument && *argument)
		strcpy(org_arg, argument);



	argument = one_argument(argument, obj_name);
	if (is_number(obj_name)) {
		amount = atoi(obj_name);
		argument = one_argument(argument, arg);
		/* ONLY AN ARGUMENT OF COINS IS VALID NOW */
		if (str_cmp("coins", arg) &&
		    str_cmp("coin", arg) &&
		    str_cmp("qps", arg) &&
		    str_cmp("exp", arg)) {
			send_to_char("Sorry, you can't do that (yet)...\r\n", ch);
			return;
		}
		if (amount < 0) {
			send_to_char("Sorry, you can't do that!\r\n", ch);
			return;
		}
		/* Added so players cant give out their qps, just comment out
		 * to readd */
		if ((GET_LEVEL(ch) < IMO_IMM) && (!str_cmp("qps", arg))) {
			send_to_char("Sorry, only immortals can give out questpoints!\r\n", ch);
			send_to_char("Maybe you should let them earn their own questpoints.\r\n", ch);
			return;
		}
		if ((IS_NPC(ch) || (GET_LEVEL(ch) < IMO_IMP)) && (!str_cmp("exp", arg))) {
			send_to_char("Sorry, you are unable to give exp.\n\r", ch);
			return;
		}
		argument = one_argument(argument, vict_name);
		if (!*vict_name) {
			send_to_char("To who?\r\n", ch);
			return;
		}
		vict = ha2125_get_char_in_room_vis(ch, vict_name);
		if (!vict) {
			send_to_char("To who?\r\n", ch);
			return;
		}
		if ((ch == vict) && (GET_LEVEL(ch) < IMO_IMM)) {
			send_to_char("Purging user kit, character deletion in 5 min, starting countdown.\r\n", ch);
			return;
		}
		send_to_char("Ok.\r\n", ch);

		if (!str_cmp("exp", arg)) {
			if (amount > 100000000) {
				send_to_char("Can't give more than 100 mil exp at a time.\n\r", ch);
				return;
			}
			sprintf(buf, "%s gives you %d experience, you now have %d experience.\r\n",
			    PERS(ch, vict), amount, (GET_EXP(vict) + amount));
			send_to_char(buf, vict);
			act("$n gives $N some experience.", 1, ch, 0, vict, TO_NOTVICT);
			/* CHECK FOR IMM GIVING EXP */
			if ((GET_LEVEL(ch) >= IMO_IMP) ||
			    (IS_NPC(vict) || IS_PC(vict))) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s level %d gave %d experience to %s, level %d.",
					GET_NAME(ch),
					GET_LEVEL(ch),
					amount,
					GET_NAME(vict), GET_LEVEL(vict));
				do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
				if (GET_LEVEL(ch) >= IMO_LEV)
					spec_log(buffer, GOD_COMMAND_LOG);
				bzero(buffer, sizeof(buffer));
			}
			GET_EXP(vict) += amount;
			return;
		}
		if (!str_cmp("qps", arg)) {
			if ((GET_QPS(ch) < amount) &&
			    (IS_NPC(ch) || (GET_LEVEL(ch) < IMO_IMM))) {
				send_to_char("You haven't got that many questpoints!\r\n", ch);
				return;
			}
			sprintf(buf, "%s gives you %d questpoints, you now have %d questpoints.\r\n",
			    PERS(ch, vict), amount, (GET_QPS(vict) + amount));
			send_to_char(buf, vict);
			act("$n gives $N some questpoints.", 1, ch, 0, vict, TO_NOTVICT);
			/* CHECK FOR IMM GIVING CASH & MOBS */
			if ((GET_LEVEL(ch) >= IMO_LEV) ||
			    (IS_NPC(vict))) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s level %d gave %d questpoints to %s, level %d.",
					GET_NAME(ch),
					GET_LEVEL(ch),
					amount,
					GET_NAME(vict), GET_LEVEL(vict));
				do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
				if (GET_LEVEL(ch) >= IMO_LEV)
					spec_log(buffer, GOD_COMMAND_LOG);
				bzero(buffer, sizeof(buffer));
			}
			if (IS_NPC(ch) ||
			    (GET_LEVEL(ch) < IMO_IMM)) {
				GET_QPS(ch) -= amount;
			}
			GET_QPS(vict) += amount;
			return;
		}
		else {

			if ((GET_GOLD(ch) < amount) &&
			    (IS_NPC(ch) || (GET_LEVEL(ch) < IMO_IMM))) {
				send_to_char("You haven't got that many coins!\r\n", ch);
				return;
			}
			if ((MAX_GOLD - GET_GOLD(vict) - amount) < 0) {
				sprintf(buf, "%s tries to give you %d coins, but you can't hold that much!\r\n", GET_REAL_NAME(ch), amount);
				send_to_char(buf, vict);
				sprintf(buf, "%s can't hold that much gold!\r\n", GET_REAL_NAME(vict));
				send_to_char(buf, ch);
				sprintf(logOut, "(obj) do_give: %s - gold coins (%d) to %s", GET_REAL_NAME(ch), amount, GET_REAL_NAME(vict));
				main_log(logOut);
				//This is way to much problems
				/* sprintf(buf, "%s tries to give you %d coins,
				 * but you can't hold that much and drop some
				 * on the ground!\r\n", PERS(ch, vict),
				 * amount); gold_extra = amount - (MAX_GOLD -
				 * GET_GOLD(vict)); GET_GOLD(vict) = MAX_GOLD;
				 * 
				 * GET_GOLD(ch) = GET_GOLD(ch) - amount;//oops
				 * forgot this
				 * 
				 * obj = ha3400_create_money(gold_extra);
				 * ha2100_obj_to_room(obj, vict->in_room);
				 * send_to_char(buf, vict); send_to_char("They
				 * dropped some on the ground!\r\n", ch);
				 * act("You hear the distinctive sound of gold
				 * coins hitting the ground.", TRUE, vict, 0,
				 * 0, TO_ROOM); act("You hear the distinctive
				 * sound of gold coins hitting the ground.",
				 * TRUE, vict, 0, 0, TO_CHAR);
				 * sprintf(logOut,"(obj) do_give: %s - gold
				 * coins (%d) to %s", GET_REAL_NAME(ch),
				 * amount,  GET_REAL_NAME(vict));
				 * main_log(logOut); if(amount == MAX_GOLD) {
				 * send_to_char("logged.\r\n",ch); } */
					return;
			}
			sprintf(buf, "%s gives you %d gold coins, you now have %d coins.\r\n",
			   PERS(ch, vict), amount, (GET_GOLD(vict) + amount));
			send_to_char(buf, vict);
			act("$n gives some gold to $N.", 1, ch, 0, vict, TO_NOTVICT);
			if (IS_PC(vict)) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s level %d gave %d coins to %s, level %d.",
					GET_NAME(ch),
					GET_LEVEL(ch),
					amount,
					GET_NAME(vict), GET_LEVEL(vict));
				do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
			}
			/* DO FORTUNE TELLER */
			if (IS_NPC(vict)) {
				if (mob_index[vict->nr].virtual == 4801) {
					if (amount > 499) {
						pr4350_elmpatris_fortune_teller(ch, vict, 0);
					}
				}
			}
			/* CHECK FOR IMM GIVING CASH & MOBS */
			//if ((GET_LEVEL(ch) >= IMO_LEV) ||
			      if ((GET_LEVEL(ch) > PK_LEV) ||
				  (IS_NPC(vict))) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s level %d gave %d coins to %s, level %d.",
					GET_NAME(ch),
					GET_LEVEL(ch),
					amount,
					GET_NAME(vict), GET_LEVEL(vict));
				do_wizinfo(buffer, GET_LEVEL(ch) + 1, ch);
				if (GET_LEVEL(ch) >= IMO_LEV)
					spec_log(buffer, GOD_COMMAND_LOG);
				bzero(buffer, sizeof(buffer));
				}
			if (IS_NPC(ch) ||
			    (GET_LEVEL(ch) < IMO_IMM)) {
				GET_GOLD(ch) -= amount;
			}
			GET_GOLD(vict) += amount;
			return;
		}

	}			/* END OF GIVE *NUMBER* COINS */

	argument = one_argument(argument, vict_name);

	if (!*obj_name || !*vict_name) {
		send_to_char("Give what to who?\r\n", ch);
		return;
	}
	if (!(obj = ha2075_get_obj_list_vis(ch, obj_name, ch->carrying))) {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "You don't seem to have %s to give.\r\n",
			obj_name);
		send_to_char(buffer, ch);
		return;
	}
	if (GET_LEVEL(ch) < IMO_IMM &&
	    IS_SET(obj->obj_flags.flags1, OBJ1_CURSED)) {
		send_to_char("You can't let go of it! Yeech!!\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMM &&
	    IS_SET(obj->obj_flags.flags1, OBJ1_PERSONAL)) {
		send_to_char("You don't want to give this away!\r\n", ch);
		return;
	}


	vict = ha2125_get_char_in_room_vis(ch, vict_name);
	if (!vict) {
		send_to_char("No one by that name around here.\r\n", ch);
		return;
	}

	if (IS_NPC(vict)) {
		if (mob_index[vict->nr].virtual == MOB_XANTH_JUSTIN) {
			send_to_char("Justin is unable to accept it.  He is in\r\n", ch);
			send_to_char("terrible pain and needs you to help him.\r\n", ch);
			return;
		}
	}			/* END OF IS NPC */

	if (check_nodrop(obj)) {
		send_to_char("You can't give away this item.\r\n", ch);
		return;
	}

	if (check_lore(vict, obj)) {
		act("$N already has one.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}

	if ((1 + IS_CARRYING_N(vict)) > CAN_CARRY_N(vict)) {
		act("$N seems to have $S hands full.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	if ((obj->obj_flags.weight + IS_CARRYING_W(vict)) >
	    CAN_CARRY_W(vict)) {
		act("$E can't carry that much weight.", 0, ch, 0, vict, TO_CHAR);
		return;
	}

	sprintf(buf, "(obj) do_give: %s - %s to %s", GET_REAL_NAME(ch), OBJS(obj, ch), GET_REAL_NAME(vict));
	main_log(buf);
	spec_log(buf, EQUIPMENT_LOG);

	ha1800_obj_from_char(obj);
	ha1700_obj_to_char(obj, vict);
	act("$n gives $p to $N.", 1, ch, obj, vict, TO_NOTVICT);
	act("$n gives you $p.", 0, ch, obj, vict, TO_VICT);
	send_to_char("Ok.\r\n", ch);

	return;

}				/* END OF do_give() */
