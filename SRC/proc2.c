/* pc */
/* gv_location: 13501-14000 */
/* ****************************************************************** *
*  file: proc2.c , Special module.                    Part of DIKUMUD *
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
#include "ansi.h"


int pc1000_gerinshill_room_1414(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int lv_to_room, lv_amount;



	if (cmd != CMD_DROP) {
		return (FALSE);
	}

	/* FIGURE OUT IF WE ARE DROPPING COINS AND HOW MUCH */

	if (IS_NPC(ch) &&
	    !(IS_AFFECTED(ch, AFF_CHARM)) &&
	    gv_switched_orig_level < IMO_IMP)
		return (FALSE);

	arg = one_argument(arg, buf);

	if (!(is_number(buf))) {
		return (FALSE);
	}

	lv_amount = atoi(buf);
	arg = one_argument(arg, buf);

	/* IF WE AREN'T DROPPING COINS, GET OUT */
	if (str_cmp("coins", buf) && str_cmp("coin", buf)) {
		return (FALSE);
	}

	/* WE ONLY DO THE SPECIAL IF ITS GREATER THAN 50K */
	if (lv_amount < 50000) {
		return (FALSE);
	}

	/* IF THE PERSON DOESN'T HAVE ENOUGH GOLD... */
	if (GET_GOLD(ch) < lv_amount) {
		return (FALSE);
	}

	/* MAKE SURE THEY PAY! */
	GET_GOLD(ch) -= lv_amount;

	/* REMOVE PERSON FROM THE ROOM */
	lv_to_room = db8000_real_room(1415);
	if (lv_to_room < 1) {
		send_to_char("ERROR: Destination room 1415 doesn't exist.\r\n", ch);
		return (FALSE);
	}

	act("Your offering is deemed adequate.\r\nYou find yourself somewhere else.",
	    FALSE, ch, 0, 0, TO_CHAR);

	act("$n drops some gold.",
	    FALSE, ch, 0, 0, TO_ROOM);
	act("$n disappears in a sparkle of bright light.",
	    FALSE, ch, 0, 0, TO_ROOM);

	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_to_room);
	act("$n suddenly winks into existence.",
	    TRUE, ch, 0, 0, TO_ROOM);

	return (TRUE);
}				/* END OF pc1000_gerinshill_room_1414() */


int pc1100_gerinshill_dentures_key(struct char_data * ch, int cmd, char *argument)
{

	char obj_name[MAX_INPUT_LENGTH], vict_name[MAX_INPUT_LENGTH];
	struct char_data *vict;
	struct obj_data *obj, *next_obj;



	if (IS_NPC(ch) &&
	    !IS_AFFECTED(ch, AFF_CHARM)) {
		return (FALSE);
	}

	argument = one_argument(argument, obj_name);
	if (is_number(obj_name)) {
		return (FALSE);
	}			/* END OF GIVE *NUMBER* COINS */

	argument = one_argument(argument, vict_name);
	if (!*obj_name || !*vict_name) {
		return (FALSE);
	}
	obj = ha2075_get_obj_list_vis(ch, obj_name, ch->carrying);
	if (!obj) {
		return (FALSE);
	}

	/* IS IT THE PROPER OBJECT? */
	if (obj_index[obj->item_number].virtual != OBJ_GERINSHILL_TEETH) {
		return (FALSE);
	}

	vict = ha2125_get_char_in_room_vis(ch, vict_name);
	if (!vict) {
		return (FALSE);
	}

	/* IF ITS NOT THE PROPER MOB, GET OUT */
	if (mob_index[vict->nr].virtual != MOB_GERINSHILL_THOTH) {
		return (FALSE);
	}

	send_to_char("Ok\r\n", ch);
	act("$n gives $p to $N.", TRUE, ch, obj, vict, TO_NOTVICT);
	act("$n gives you $p.", FALSE, ch, obj, vict, TO_VICT);

	act("$n puts $p in $s mouth and smiles.",
	    TRUE, vict, obj, vict, TO_NOTVICT);

	/* PURGE OBJECT */
	ha2700_extract_obj(obj);

	/* IF MOB has return object, give it to player */
	for (obj = vict->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj_index[obj->item_number].virtual == 1441) {
			ha1800_obj_from_char(obj);
			ha1700_obj_to_char(obj, ch);
			do_say(vict, "Thank you! Thank you! And in return, I give you this.", CMD_SAY);
			act("$n gives $p to $N.",
			    TRUE, vict, obj, ch, TO_NOTVICT);
			act("$n gives you $p.",
			    FALSE, vict, obj, ch, TO_VICT);
			return (TRUE);
		}
	}			/* END OF for loop */

	return (TRUE);

}				/* END OF pc1100_gerinshill_dentures_key() */


/* THIS PROCEDURE LOOKS THOUGH THE ROOM, AND IF ALL REQUIRED */
/* OBJECTS ARE IN THE ROOM, TRANSPORTS THE PLAYERS           */
int pc1200_gerinshill_room_1496(struct char_data * ch, int cmd, char *argument)
{

	int lv_bit_flag, lv_from_room, lv_to_room;
	struct char_data *lv_char, *lv_next_char;
	struct obj_data *lv_obj, *lv_next_obj;



	/* IF THE ROOM CONTAINS NO OBJECTS, GET OUT */
	if (!world[ch->in_room].contents) {
		return (FALSE);
	}

	lv_bit_flag = 0;
	for (lv_obj = world[ch->in_room].contents; lv_obj; lv_obj = lv_next_obj) {
		lv_next_obj = lv_obj->next_content;
		if (obj_index[lv_obj->item_number].virtual == 1414)
			SET_BIT(lv_bit_flag, BIT0);
		if (obj_index[lv_obj->item_number].virtual == 1415)
			SET_BIT(lv_bit_flag, BIT1);
		if (obj_index[lv_obj->item_number].virtual == 1416)
			SET_BIT(lv_bit_flag, BIT2);
		if (obj_index[lv_obj->item_number].virtual == 1422)
			SET_BIT(lv_bit_flag, BIT3);
		if (obj_index[lv_obj->item_number].virtual == 1423)
			SET_BIT(lv_bit_flag, BIT4);
		if (obj_index[lv_obj->item_number].virtual == 1424)
			SET_BIT(lv_bit_flag, BIT5);

	}			/* END OF for loop */

	/* DO WE HAVE THE REAL CROSSES IN THE ROOM? */
	if (lv_bit_flag == (BIT0 + BIT1 + BIT2)) {
		/* REMOVE THE GOOD PIECES */
		for (lv_obj = world[ch->in_room].contents; lv_obj; lv_obj = lv_next_obj) {
			lv_next_obj = lv_obj->next_content;
			if (obj_index[lv_obj->item_number].virtual == 1414 ||
			    obj_index[lv_obj->item_number].virtual == 1415 ||
			    obj_index[lv_obj->item_number].virtual == 1416) {
				ha2700_extract_obj(lv_obj);
			}
		}
		/* MOVE ALL CHARS TO NEW ROOM */
		lv_from_room = ch->in_room;
		lv_to_room = db8000_real_room(1500);
		if (lv_to_room < 1) {
			send_to_char("Hrmm, the destination room is invalid.\r\n", ch);
			return (TRUE);
		}
		for (lv_char = world[lv_from_room].people;
		     lv_char; lv_char = lv_next_char) {
			lv_next_char = lv_char->next_in_room;
			send_to_char("You feel your world shift.\r\n", lv_char);
			ha1500_char_from_room(lv_char);
			ha1600_char_to_room(lv_char, lv_to_room);
		}
		return (TRUE);
	}			/* END OF its real */

	/* DO WE HAVE FAKE CROSSES IN THE ROOM? */
	if (lv_bit_flag == (BIT3 + BIT4 + BIT5)) {
		/* REMOVE THE FAKE PIECES */
		for (lv_obj = world[ch->in_room].contents; lv_obj; lv_obj = lv_next_obj) {
			lv_next_obj = lv_obj->next_content;
			if (obj_index[lv_obj->item_number].virtual == 1422 ||
			    obj_index[lv_obj->item_number].virtual == 1423 ||
			    obj_index[lv_obj->item_number].virtual == 1424) {
				ha2700_extract_obj(lv_obj);
			}
		}
		/* MOVE ALL CHARS TO NEW ROOM */
		lv_from_room = ch->in_room;
		lv_to_room = db8000_real_room(1590);
		if (lv_to_room < 1) {
			send_to_char("Hrmm, the destination room is invalid.\r\n", ch);
			return (TRUE);
		}
		for (lv_char = world[lv_from_room].people;
		     lv_char; lv_char = lv_next_char) {
			lv_next_char = lv_char->next_in_room;
			send_to_char("You feel your world shift.\r\n", lv_char);
			ha1500_char_from_room(lv_char);
			ha1600_char_to_room(lv_char, lv_to_room);
			ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_GERINS);
		}
		return (TRUE);

	}			/* END OF its false */

	/* DO WE HAVE A COMBINATION OF CROSSES IN THE ROOM? */
	if (((lv_bit_flag & BIT0) || (lv_bit_flag & BIT3)) &&
	    ((lv_bit_flag & BIT1) || (lv_bit_flag & BIT4)) &&
	    ((lv_bit_flag & BIT2) || (lv_bit_flag & BIT5))) {
		/* REMOVE ALL OBJECTS FROM THE ROOM */
		for (lv_obj = world[ch->in_room].contents; lv_obj; lv_obj = lv_next_obj) {
			lv_next_obj = lv_obj->next_content;
			ha2700_extract_obj(lv_obj);
		}
		/* MOVE ALL CHARS TO NEW ROOM */
		lv_from_room = ch->in_room;
		lv_to_room = db8000_real_room(1400);
		if (lv_to_room < 1) {
			send_to_char("Hrmm, the destination room is invalid.\r\n", ch);
			return (TRUE);
		}
		for (lv_char = world[lv_from_room].people;
		     lv_char; lv_char = lv_next_char) {
			lv_next_char = lv_char->next_in_room;
			send_to_char("You feel your world shift.\r\n", lv_char);
			ha1500_char_from_room(lv_char);
			ha1600_char_to_room(lv_char, lv_to_room);
		}
		return (TRUE);
	}			/* END OF its a combination */

	return (FALSE);

}				/* END OF pc1200_gerinshill_room_1496() */

int pc1300_newbie_book(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];



	if (cmd != CMD_READ &&
	    cmd != CMD_LOOK) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);

	if (!*buf) {
		return (FALSE);
	}

	if ((!is_abbrev(buf, "page")) &&
	    (!is_abbrev(buf, "book")) &&
	    (!is_abbrev(buf, "newbie")))
		return (FALSE);

	/* READ A PAGE? */
	if (cmd == CMD_READ) {
		if (is_abbrev(buf, "page")) {
			/* WHAT DO THEY WANT HELP ON? */
			for (; *arg == ' '; arg++);
			if (*arg) {
				sprintf(buf, "NEWBIE_%s", arg);
				in3500_do_help(ch, buf, CMD_HELP);
			}
			else {
				send_to_char("Which page do you want to read?\r\n", ch);
			}
			return (TRUE);
		}

		/* USER WANTS TO READ THE BOOK */
		strcpy(buf, "NEWBIE_BOOK");
		in3500_do_help(ch, buf, CMD_HELP);
		return (TRUE);

	}			/* END OF READ PAGE */

	/* **** */
	/* LOOK */
	/* **** */
	/* WE ARE GOING TO LOOK AT THE BOOK */

	strcpy(buf, "NEWBIE_BOOK");
	in3500_do_help(ch, buf, CMD_HELP);
	return (TRUE);

}				/* END OF pc1300_newbie_book() */


int pc1400_island3_parts(struct char_data * ch, int cmd, char *argument)
{

	int lv_item_num, lv_obj_check;
	char obj_name[MAX_INPUT_LENGTH], vict_name[MAX_INPUT_LENGTH];
	struct char_data *vict;
	struct obj_data *obj, *lv_obj, *next_obj;



	if (IS_NPC(ch) &&
	    !IS_AFFECTED(ch, AFF_CHARM)) {
		return (FALSE);
	}

	argument = one_argument(argument, obj_name);
	if (is_number(obj_name)) {
		return (FALSE);
	}			/* END OF GIVE *NUMBER* COINS */

	argument = one_argument(argument, vict_name);
	if (!*obj_name || !*vict_name) {
		return (FALSE);
	}

	obj = ha2075_get_obj_list_vis(ch, obj_name, ch->carrying);
	if (!obj) {
		return (FALSE);
	}

	/* IS IT THE PROPER OBJECT? */
	if (obj_index[obj->item_number].virtual != OBJ_PART_ONE &&
	    obj_index[obj->item_number].virtual != OBJ_PART_TWO &&
	    obj_index[obj->item_number].virtual != OBJ_PART_THREE &&
	    obj_index[obj->item_number].virtual != OBJ_PART_FOUR) {
		return (FALSE);
	}

	vict = ha2125_get_char_in_room_vis(ch, vict_name);
	if (!vict) {
		return (FALSE);
	}

	/* IF ITS NOT THE PROPER MOB, GET OUT */
	if (mob_index[vict->nr].virtual != MOB_ISLAND3_MOB) {
		return (FALSE);
	}

	send_to_char("Ok.\r\n", ch);
	act("$n gives $p to $N.", TRUE, ch, obj, vict, TO_NOTVICT);
	act("$n gives you $p.", FALSE, ch, obj, vict, TO_VICT);

	/* GIVE OBJECT TO MOB */
	ha1800_obj_from_char(obj);
	ha1700_obj_to_char(obj, vict);

	/* SEE IF MOB HAS ALL THE OBJECTS */
	lv_obj_check = 0;
	for (obj = vict->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj_index[obj->item_number].virtual == OBJ_PART_ONE)
			SET_BIT(lv_obj_check, BIT0);
		if (obj_index[obj->item_number].virtual == OBJ_PART_TWO)
			SET_BIT(lv_obj_check, BIT1);
		if (obj_index[obj->item_number].virtual == OBJ_PART_THREE)
			SET_BIT(lv_obj_check, BIT2);
		if (obj_index[obj->item_number].virtual == OBJ_PART_FOUR)
			SET_BIT(lv_obj_check, BIT3);
	}			/* END OF for loop */

	/* IF THE MOB DOESN'T HAVE ALL FOUR OBJECTS, RETURN */
	if (lv_obj_check != (BIT0 + BIT1 + BIT2 + BIT3)) {
		return (TRUE);
	}

	/* LOAD REPLACEMENT OBJECT */
	lv_item_num = db8200_real_object(OBJ_PART_FIVE);
	if (lv_item_num < 1) {
		return (TRUE);
	}

	obj = db5100_read_object(lv_item_num, REAL);

	/* DON'T PASS LIMITS */
	if (obj_index[obj->item_number].number >=
	    obj_index[obj->item_number].maximum) {
		return (TRUE);
	}

	/* EVERYTHING IS SET, SO GIVE OBJECT TO PLAYER */
	ha1700_obj_to_char(obj, ch);

	/* REMOVE PARTS FROM MOB */
	for (lv_obj = vict->carrying; lv_obj; lv_obj = next_obj) {
		next_obj = lv_obj->next_content;
		if (obj_index[lv_obj->item_number].virtual == OBJ_PART_ONE ||
		    obj_index[lv_obj->item_number].virtual == OBJ_PART_TWO ||
		    obj_index[lv_obj->item_number].virtual == OBJ_PART_THREE ||
		    obj_index[lv_obj->item_number].virtual == OBJ_PART_FOUR) {
			ha1800_obj_from_char(lv_obj);
			ha2700_extract_obj(lv_obj);
		}
	}			/* END OF for loop */

	do_say(vict, "Thank you! Thank you! And in return, I give you this.", CMD_SAY);
	act("$n gives $p to $N.",
	    TRUE, vict, obj, ch, TO_NOTVICT);
	act("$n gives you $p.",
	    FALSE, vict, obj, ch, TO_VICT);
	return (TRUE);

}				/* END OF pc1400_island3_parts() */


int pc1500_glass_door(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj;
	struct char_data *victim;

	char lv_type[MAX_STRING_LENGTH], lv_dir[MAX_STRING_LENGTH];

	int lv_to_room, rc, lv_door;




	if (cmd != CMD_OPEN) {
		return (FALSE);
	}

	argument_interpreter(arg, lv_type, lv_dir);
	if (!*lv_type) {
		return (FALSE);
	}
	rc = ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM,
				 ch, &victim, &obj);
	if (rc) {
		return (FALSE);
	}

	lv_door = mo1600_find_door(ch, lv_type, lv_dir);
	if (lv_door < 0) {
		return (FALSE);
	}

	/* OK, WE ARE OPENING THE DOOR IN THE SPECIAL ROOM */
	send_to_char("You foolishly open the glass door...", ch);
	act("$n foolishly opens the glass door.",
	    TRUE, ch, obj, 0, TO_ROOM);

	lv_to_room = db8000_real_room(17932);
	if (lv_to_room < 1) {
		send_to_char("ERROR: Destination room 17932 doesn't exist.\r\n", ch);
		return (FALSE);
	}

	ha1650_all_from_room_to_room(ch->in_room,
				     lv_to_room,
				     "\0",
				     "$n is swept into the abyss.",
				     BIT0);

	return (TRUE);

}				/* END OF pc1500_glass_door() */

int pc1550_darkcastle_portal(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj;
	struct char_data *victim;

	char lv_type[MAX_STRING_LENGTH], lv_dir[MAX_STRING_LENGTH];

	int lv_to_room, rc, lv_door;

	if (cmd != CMD_OPEN) {
		return (FALSE);
	}

	argument_interpreter(arg, lv_type, lv_dir);
	if (!*lv_type) {
		return (FALSE);
	}
	rc = ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM,
				 ch, &victim, &obj);
	if (rc) {
		return (FALSE);
	}

	lv_door = mo1600_find_door(ch, lv_type, lv_dir);
	if (lv_door < 0) {
		return (FALSE);
	}

	/* OK, WE ARE OPENING THE DOOR IN THE SPECIAL ROOM */
	send_to_char("You foolishly enter the portal...", ch);
	act("$n foolishly enters the portal.",
	    TRUE, ch, obj, 0, TO_ROOM);

	lv_to_room = db8000_real_room(19262);
	if (lv_to_room < 1) {
		send_to_char("ERROR: Destination room 19262 doesn't exist.\r\n", ch);
		return (FALSE);
	}

	ha1650_all_from_room_to_room(ch->in_room,
				     lv_to_room,
				     "\0",
				     "$n is swept into the darkness.",
				     BIT0);

	return (TRUE);

}				/* END OF pc1500_glass_door() */

int pc1600_healer(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict;
	char buf[MAX_STRING_LENGTH];
	int lv_players = 0, lv_random, lv_number;

	if (cmd)
		return (FALSE);

	for (vict = world[ch->in_room].people; vict;
	     vict = vict->next_in_room)
		lv_players++;

	if (!lv_players)
		return (FALSE);

	lv_random = number(0, lv_players - 1);
	lv_players = 0;

	for (vict = world[ch->in_room].people; lv_players < lv_random;
	     vict = vict->next_in_room)
		lv_players++;

	if (!vict)
		return (FALSE);
	if ((GET_LEVEL(vict) > 20) || IS_NPC(vict))
		return (FALSE);

	lv_number = number(1, 6);

	switch (lv_number) {
	case 1:
		sprintf(buf, "aid %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	case 2:
		sprintf(buf, "armor %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	case 3:
		sprintf(buf, "bless %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	case 4:
		sprintf(buf, "aid %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	case 5:
		sprintf(buf, "35 %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	case 6:
		sprintf(buf, "43 %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
		return (TRUE);
	default:
		return (FALSE);
	}			/* END OF switch() */

	return (FALSE);
}

int pc1700_special_sword(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict;
	struct obj_data *obj;
	long lv_dam, lv_choice, lv_level, lv_sex;
	char *pName, *oName;
	char buf[MAX_STRING_LENGTH];

	if (cmd || !sp_obj)
		return (FALSE);

	obj = sp_obj;

	/* check if weapon is being carried */
	if (!(ch = sp_obj->carried_by))
		return (FALSE);

	/* check if weapon is worn */
	if (sp_obj != ch->equipment[WIELD])
		return (FALSE);

	lv_sex = ch->player.sex;
	pName = GET_REAL_NAME(ch);
	oName = obj->short_description;
	if (IS_NPC(ch))
		ch->player.short_descr = oName;
	else
		ch->player.name = oName;
	ch->player.sex = 0;

	/* check if master is fighting */
	if (vict == ch->specials.fighting) {
		lv_choice = number(0, 4);
		lv_dam = ch->points.damroll << 2;
		lv_level = ch->player.level;

		switch (lv_choice) {
		case 0:
			damage(ch, vict, lv_dam, TYPE_SLASH, 99);
			ft2400_damage_message(lv_dam, ch, vict, TYPE_SLASH);
			break;
		case 1:
			GET_HIT(ch) = MAXV(GET_HIT_LIMIT(ch), GET_HIT(ch) + lv_dam);
			sprintf(buf, "%s heals you.\r\n", oName);
			send_to_char(buf, ch);
			sprintf(buf, "$n heals %s.", pName);
			act(buf, FALSE, ch, 0, 0, TO_NOTCHAR);
			break;
		default:
			break;
		}
	}

	ch->player.sex = lv_sex;
	if (IS_NPC(ch))
		ch->player.short_descr = pName;
	else
		ch->player.name = pName;
	return (TRUE);
}

int pc1800_killer_womble(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict;
	int lv_choice, lv_count, lv_found = 0, headbite;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct descriptor_data *d;


	if (!cmd) {
		/* Actions for when mob is fighting */
		vict = ch->specials.fighting;
		if (vict) {
			if (GET_MAX_HIT(vict) < 250)
				headbite = 50;
			else if (GET_MAX_HIT(vict) < 500)
				headbite = 100;
			else if (GET_MAX_HIT(vict) < 750)
				headbite = 200;
			else if (GET_MAX_HIT(vict) < 1000)
				headbite = 300;
			else if (GET_MAX_HIT(vict) < 1250)
				headbite = 500;
			else if (GET_MAX_HIT(vict) < 1500)
				headbite = 600;
			else if (GET_MAX_HIT(vict) < 1750)
				headbite = 700;
			else if (GET_MAX_HIT(vict) < 2000)
				headbite = 900;
			else
				headbite = 1100;
			if (!IS_SET(GET_ACT2(vict), PLR2_QUEST)) {
				do_set_flag(vict, "quest on", cmd, vict);
				send_to_char("&YNaughty, Naughty, you shouldn't fight these without your QUEST flag on.&n\n\r", vict);
				strcpy(buf, " %s");
				sprintf(buf2, buf, GET_NAME(vict));
				at2200_do_rage(ch, buf2, cmd);
				at1700_do_kick(ch, buf2, cmd);
				return (TRUE);
			}
			switch (number(0, 5)) {
			case 0:
				/* Kill victim if hps fall under headbite range */
				if ((GET_HIT(vict) < headbite)) {
					lv_choice = number(0, 6);
					switch (lv_choice) {
					case 0:
						strcpy(buf, "Time to die %s.");
						break;
					case 1:
						strcpy(buf, "Man did you pick the wrong day to come and fight me %s.");
						break;
					case 2:
						strcpy(buf, "Bye bye %s, time for you to meet your maker.");
						break;
					case 3:
						strcpy(buf, "Muahahahahah %s, I believe you are dead...");
						break;
					case 4:
						strcpy(buf, "You should've stayed at home today %s.");
						break;
					case 5:
						strcpy(buf, "%s..... I'm sorry but.... laters.");
						break;
					case 6:
						strcpy(buf, "I feel good, tralalalala.. But you don't %s!");
						break;
					}
					sprintf(buf2, buf, GET_REAL_NAME(vict));
					send_to_char("\r\n", vict);
					do_say(ch, buf2, 0);
					ansi_act("&w$n bites $N's head off.&n", FALSE, ch, 0, vict, TO_NOTVICT, 0, 0);
					ansi_act("&w$n bites your head off.&n", FALSE, ch, 0, vict, TO_VICT, 0, 0);
					ft1800_raw_kill(vict, ch);
					sprintf(buf, "%s L[%d] lost his head in a very painful way.\r\n",
						GET_REAL_NAME(vict),
						GET_LEVEL(vict));
					do_info(buf, 1, MAX_LEV, vict);
					return (TRUE);
				}
				break;
			case 1:
				/* Elsewhere victim if own hp is lower than
				 * 100000 */
				if (GET_HIT(ch) < 50000) {
					lv_choice = number(0, 4);
					switch (lv_choice) {
					case 0:
						strcpy(buf, "Time for you to take a hike %s.");
						break;
					case 1:
						strcpy(buf, "That's it, you're outa here %s.");
						break;
					case 2:
						strcpy(buf, "I can't take this any longer, Go play elsewhere %s.");
						break;
					case 3:
						strcpy(buf, "Argh, you're to strong %s.. Time for a different approach.");
						break;
					case 4:
						strcpy(buf, "This is not my style %s, but you leave me no choice...");
						break;
					}
					sprintf(buf2, buf, GET_REAL_NAME(vict));
					do_say(ch, buf2, 0);
					spell_elsewhere(48, ch, vict, 0);
					return (TRUE);
				}
				break;
			default:
				break;
			}
			return (FALSE);
		}

		/* when mob is hunting */
		vict = ch->specials.hunting;
		if (vict) {
			switch (number(0, 5)) {
			case 0:
				/* Try to summon the person */
				send_to_char("Trying summon.\r\n", ch);
				if (ch->in_room != vict->in_room)
					spell_summon(48, ch, vict, 0);
				if (ch->in_room == vict->in_room)
					hit(ch, vict, 0);
				return (TRUE);
			case 1:
				/* try to teleport to the person */
				send_to_char("Trying teleport.\r\n", ch);
				if (ch->in_room != vict->in_room)
					spell_teleport_self(48, ch, vict, 0);
				if (ch->in_room == vict->in_room)
					hit(ch, vict, 0);
				return (TRUE);
			case 2:
				/* try to portal to the person */
				send_to_char("Trying portal.\r\n", ch);
				if (ch->in_room != vict->in_room)
					spell_portal(48, ch, vict, 0);
				mo2500_enter_portal(ch, CMD_ENTER, " portal");
				if (ch->in_room == vict->in_room)
					hit(ch, vict, 0);
				return (TRUE);
			default:
				break;
			}
			return (FALSE);
		}

		/* when mob is idle, pick random player with quest_flag &&
		 * BULLY */
		lv_count = number(0, 50);
		do {
			lv_found = 0;
			for (d = descriptor_list; d && lv_count; d = d->next) {
				if (d->character && (d->connected == CON_PLAYING) && (d->character->in_room >= 0)) {

					vict = d->character;

					if (//(vict->player.level < IMO_SPIRIT) &&
					 IS_SET(GET_ACT2(vict), PLR2_QUEST) ||
					    IS_SET(GET_ACT2(vict), PLR2_KICKME)
						) {
						lv_count--;
						lv_found++;
					}	/* end if */
				}	/* end if */
			}
		} while (lv_count && lv_found);


		if (vict && lv_found) {
			switch (number(0, 5)) {
			case 1:
				switch (number(0, 4)) {
				case 0:
					strcpy(buf, " %s I'm coming to get you...");
					break;
				case 1:
					strcpy(buf, " %s I think i'll go kill you now.");
					break;
				case 2:
					strcpy(buf, " %s If I were you, i'd watch my back..");
					break;
				case 3:
					strcpy(buf, " %s Get ready to rumble!");
					break;
				case 4:
					strcpy(buf, " %s Get ready for some action");

				}
				sprintf(buf2, buf, GET_REAL_NAME(vict));
				do_tell(ch, buf2, CMD_TELL);
				ch->specials.hunting = vict;
				return (TRUE);
			default:
				break;
			}
		}
	}			/* end of: if (!cmd) */
	else {
		return (FALSE);
	}
	return (TRUE);
}

int pc1900_slot_machine(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int lv_one, lv_two, lv_three, lv_wins = 0, lv_wager = 0;

	if (cmd != CMD_PULL)
		return (FALSE);

	if (IS_NPC(ch)) {
		send_to_char("Mobs can't play slots....SO DON'T TRY!\n\r", ch);
		return (FALSE);
	}

	WAIT_STATE(ch, PULSE_VIOLENCE * 1.5);

	for (; isspace(*arg); arg++);

	if (cmd == CMD_PULL) {
		if (!*arg) {
			send_to_char("Try pull <amount>.\r\n", ch);
			return (TRUE);
		}
		if (!is_number(arg)) {
			send_to_char("Please enter pull <wager amount>.\r\n", ch);
			return (TRUE);
		}
		lv_wager = atoi(arg);
		/* if (lv_wager < 100) { send_to_char ("You cheapskate enter a
		 * value from 100 to 25 mil coins.\r\n", ch); return(TRUE); }
		 * if (lv_wager > 25000000) { send_to_char ("Whoa there big
		 * roller, you can only wager 100 to 25 mil coins.\r\n", ch);
		 * return(TRUE); } */
		if (GET_LEVEL(ch) <= 19) {
			if ((lv_wager <= 99) || (lv_wager >= 10001)) {
				send_to_char("Your wager must be from 100 to 10000 coins.\n\r", ch);
				return (TRUE);
			}
		}
		if (GET_LEVEL(ch) >= 20 && GET_LEVEL(ch) <= 29) {
			if ((lv_wager <= 999) || (lv_wager >= 100001)) {
				send_to_char("Your wager must be from 1000 to 100000 coins.\n\r", ch);
				return (TRUE);
			}
		}
		if (GET_LEVEL(ch) >= 30 && GET_LEVEL(ch) <= 40) {
			if ((lv_wager <= 9999) || (lv_wager >= 1000001)) {
				send_to_char("Your wager must be from 10000 to 1000000 coins.\n\r", ch);
				return (TRUE);
			}
		}
		if (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV) {
			if ((lv_wager <= 24999) || (lv_wager >= 25000001)) {
				send_to_char("Your wager must be from 25000 to 25000000 coins.\n\r", ch);
				return (TRUE);
			}
		}

		if (GET_GOLD(ch) < lv_wager) {
			send_to_char("You do not have that much money to wager.\r\n", ch);
			return (TRUE);
		}

		GET_GOLD(ch) -= lv_wager;
		act("You insert your money in the machine.\n\r&Y*&Rcrank&Y*&n You yank the slot lever.", TRUE, ch, 0, 0, TO_CHAR);
		act("&Y*&Rcrank&Y*&n a slot lever is pulled.", TRUE, ch, 0, 0, TO_NOTVICT);

		lv_one = number(number(1, 5), 9);
		lv_two = number(number(1, 5), 9);
		lv_three = number(number(1, 5), 9);

		if (lv_one == 1 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 20;
		}
		if (lv_one == 2 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 15;
		}
		if (lv_one == 1 && lv_two == 2 && lv_three == 3) {
			lv_wins = lv_wager * 10;
		}
		if (lv_one == 3 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 9;
		}
		if (lv_one == 2 && lv_two == 3 && lv_three == 4) {
			lv_wins = lv_wager * 8;
		}
		if (lv_one == 4 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 7;
		}
		if (lv_one == 3 && lv_two == 4 && lv_three == 5) {
			lv_wins = lv_wager * 6;
		}
		if (lv_one == 5 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 5;
		}
		if (lv_one > 5 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = lv_wager * 2;
		}
		if (lv_one == 6 && lv_two == lv_one && lv_three == lv_one) {
			lv_wins = 666;
			if (CHANCE100(50)) {
				act("A demon jump's from the slot machine into your body!", TRUE, ch, 0, 0, TO_CHAR);
				send_to_char("The demon possesses you and you are now &revil&n!\n\r", ch);
				act("A demon jump's from the slot machine into $n's body!", TRUE, ch, 0, 0, TO_NOTVICT);
				GET_ALIGNMENT(ch) = -1000;
			}
		}
		if (lv_one == 7 && lv_two == lv_one && lv_three == lv_one) {
			if (lv_wager < 777) {
				lv_wins = 777;
			}
			else if (lv_wager < 7777) {
				lv_wins = 7777;
			}
			else if (lv_wager < 77777) {
				lv_wins = 77777;
			}
			else if (lv_wager < 777777) {
				lv_wins = 777777;
			}
			else {
				lv_wins = 7777777;
			}
			if (CHANCE100(5)) {
				if (lv_wager < 7777) {
					send_to_char("&G&fLucky!&n You get a bonus of 7 quest points!\n\r", ch);
					GET_QPS(ch) += 7;
				}
				else if (lv_wager < 777777) {
					send_to_char("&G&fLucky!&n You get a bonus of 77 quest points!\n\r", ch);
					GET_QPS(ch) += 77;
				}
				else {
					send_to_char("&G&fLucky!&n You get a bonus of 777 quest points!\n\r", ch);
					GET_QPS(ch) += 777;
				}
			}
		}
		if ((lv_one < 6 && lv_two == lv_one && lv_three != lv_one) ||
		    (lv_one < 6 && lv_two != lv_one && lv_three == lv_one) ||
		    (lv_one != lv_two && lv_two < 6 && lv_three == lv_two)) {
			lv_wins = lv_wager * 2;
			if (CHANCE100(1)) {
				send_to_char("&G&fLucky!&n You get a bonus of 50 quest points!\n\r", ch);
				GET_QPS(ch) += 50;
			}
		}
		if ((lv_one > 5 && lv_two == lv_one && lv_three != lv_one) ||
		    (lv_one > 5 && lv_two != lv_one && lv_three == lv_one) ||
		    (lv_one != lv_two && lv_two > 5 && lv_three == lv_two)) {
			lv_wins = lv_wager / 2;
			if (CHANCE100(1)) {
				send_to_char("&G&fLucky!&n You get a bonus of 10 quest points!\n\r", ch);
				GET_QPS(ch) += 10;
			}
		}

		GET_GOLD(ch) += lv_wins;
		send_to_char("   &Y______________\n\r", ch);
		send_to_char("  &Y/   &RC&C2 &BSlots   &Y/}\n\r", ch);
		send_to_char(" &x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&x/&y(&Y*&y)\n\r", ch);
		send_to_char("  &Y[ &r--- &g--- &p--- &Y]&y//\n\r", ch);
		sprintf(buf, "  &Y[&b|&R&1 %d &b|&G&2 %d &b|&P&5 %d &b|&Y]&y|\\ \n\r", lv_one, lv_two, lv_three);
		send_to_char(buf, ch);
		send_to_char("  &Y[ &r--- &g--- &p--- &Y]&K*&Y}\n\r", ch);
		send_to_char("  &Y[             ] }\n\r", ch);
		send_to_char("  &Y[&y_____________&Y]/\n\r", ch);
		send_to_char("    &Y/&y_________&Y/}|\n\r", ch);
		send_to_char("    &Y[         ]}|\n\r", ch);
		send_to_char("    &Y[&y*********&Y]}/\n\r", ch);
		send_to_char("    &Y[&y_________&Y]/&n\n\r", ch);
		sprintf(buf, "  &cYou win&C %d &ccoins.&n\n\r", lv_wins);
		send_to_char(buf, ch);
		return (TRUE);
	}
	/* Bingo last updated 02-05-01 */
	return (TRUE);
}				/* END OF slot_machine() */

int pc2000_darken(struct char_data * ch, int cmd, char *arg)
{

	struct char_data *vict;
	int lv_choice;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];


	if (!cmd) {
		/* Actions for when mob is fighting */
		vict = ch->specials.fighting;
		if (vict) {
			switch (number(0, 1)) {
			case 0:
				/* Trash talking to the dead. */
				if ((GET_HIT(vict) < 250)) {
					lv_choice = number(0, 6);
					switch (lv_choice) {
					case 0:
						strcpy(buf, "Snap Your finger, I snap your neck %s.");
						break;
					case 1:
						strcpy(buf, "Wake up, its time to die %s.");
						break;
					case 2:
						strcpy(buf, "Grain of sand %s is going off to nevernever land.");
						break;
					case 3:
						strcpy(buf, "Prepare for Total Domination %s!");
						break;
					case 4:
						strcpy(buf, "Where is your saviour now %s?");
						break;
					case 5:
						strcpy(buf, "This menu moment has been brought to you by...ME!");
						break;
					case 6:
						strcpy(buf, "Do or do not, there is no try.");
						break;
					}
					sprintf(buf2, buf, GET_REAL_NAME(vict));
					send_to_char("\r\n", vict);
					do_say(ch, buf2, 0);
					ansi_act("&w$n sends $N back to the menu screen.&n", FALSE, ch, 0, vict, TO_NOTVICT, 0, 0);
					ansi_act("&w$n kicks your skull in.&n", FALSE, ch, 0, vict, TO_VICT, 0, 0);
					ft1800_raw_kill(vict, ch);
					sprintf(buf, "%s L[%d] is sent back to the menu screen.\r\n",
						GET_REAL_NAME(vict),
						GET_LEVEL(vict));
					do_info(buf, 1, MAX_LEV, vict);
					return (TRUE);
				}
				break;
			case 1:
				/* rages if mobs hp is lower than 2500 */
				if (GET_HIT(ch) < 2500) {
					lv_choice = number(0, 1);
					switch (lv_choice) {
					case 0:
						strcpy(buf, "Now you are starting to piss me off.");
						break;
					case 1:
						strcpy(buf, "This is going to hurt a little.");
						break;
					}
					sprintf(buf2, buf, GET_REAL_NAME(vict));
					do_say(ch, buf2, 0);
					strcpy(buf, " %s");
					sprintf(buf2, buf, GET_NAME(vict));
					at2200_do_rage(ch, buf2, cmd);
					return (TRUE);
					break;
				}
			default:
				break;
			}
			return (FALSE);
		}

		/* when mob is hunting */
		vict = ch->specials.hunting;
		if (vict) {
			switch (number(0, 3)) {
			case 0:
				/* Try to summon the person */
				send_to_char("Trying summon.\r\n", ch);
				if (ch->in_room != vict->in_room)
					spell_summon(48, ch, vict, 0);
				if (ch->in_room == vict->in_room)
					hit(ch, vict, 0);
				return (TRUE);
			default:
				break;
			}
			return (FALSE);
		}


	}			/* end of: if (!cmd) */
	else {
		return (FALSE);
	}
	return (TRUE);
}

int pc2200_sretaw(struct char_data * ch, int cmd, char *arg)
{
	struct char_data *vict;
	struct descriptor_data *d;
	int randomno = 0, count = 0, idx;
	char buf[MAX_STRING_LENGTH];

	if (!cmd) {

		if (GET_POS(ch) < POSITION_SLEEPING)
			return 0;

		if (number(0, 100) > 4)
			//Change this, could get annoing.
				return 0;

		for (d = descriptor_list; d; d = d->next) {
			if (d->character && IS_SET(GET_ACT2(d->character), PLR2_MUZZLE_ALL))
				count++;
		}

		if (count < 1)
			return 0;

		randomno = number(0, count);

		d = descriptor_list;
		idx = 0;
		while (idx < randomno) {
			if (!d)
				return 0;
			if (d->character && IS_SET(GET_ACT2(d->character), PLR2_MUZZLE_ALL)) {
				idx++;
				if (idx == randomno)
					continue;
			}
			d = d->next;
		}

		if (!d || !d->character)
			return 0;

		vict = d->character;

		if (!IS_SET(GET_ACT2(vict), PLR2_MUZZLE_ALL))
			return 0;

		randomno = number(0, 8);
		switch (randomno) {
		case 0:
			sprintf(buf, "Awwwwww, poor little %s. Does that muzzle hurt?", GET_REAL_NAME(vict));
			break;
		case 1:
			sprintf(buf, "Ha ha, no speakee the English, %s? Hum for me!", GET_REAL_NAME(vict));
			break;
		case 2:
			sprintf(buf, "Silence is golden, %s.", GET_REAL_NAME(vict));
			break;
		case 3:
			sprintf(buf, "Look what I've got %s! *dangles tongue* Mime for me, silent one!", GET_REAL_NAME(vict));
			break;
		case 4:
			sprintf(buf, "Hey, that %s %s looks like a little weakling, who agrees?", GET_REAL_NAME(vict),
				(GET_SEX(vict) == SEX_FEMALE) ? "gal" : "guy");
			break;
		case 5:
			sprintf(buf, "What's the matter, %s, just got nothing to say, or can't you formulate "
				"a proper English sentance to defend your honor?", GET_REAL_NAME(vict));
			break;
		default:
			sprintf(buf, "HAHAHAH!  %s is muzzled.  %se can't say a damn thing!", GET_REAL_NAME(vict),
				(GET_SEX(vict) == SEX_FEMALE) ? "Sh" : "H");
			break;
		}

		do_gossip(ch, buf, CMD_GOSSIP);
		return 1;
	}
	else
		return 0;
}


int pc2300_teleport_room(struct char_data * ch, int cmd, char *arg)
{
	/* Meant for zone 239 Enchanted Hollow(s) Starting in room #23917 (Cave
	 * of Decision) Saying word "fire" would take them to room #23918
	 * Saying word "water" would take them to room #23919 Saying word
	 * "earth" would take them to room #23920 Saying word "air" would take
	 * them to room #23921 */
	int location, loc_nr;
	char charbuf[MAX_STRING_LENGTH], roombuf[MAX_STRING_LENGTH];

	if (cmd != CMD_SAY)
		return (FALSE);

	for (; *arg == ' '; arg++);
	if (!(*arg))
		return (FALSE);

/* Note:Change this to a switch statement to avoid reduntant strcmps... */

	if (!(!strcmp(arg, "fire") || !strcmp(arg, "Fire") ||
	      !strcmp(arg, "water") || !strcmp(arg, "Water") ||
	      !strcmp(arg, "earth") || !strcmp(arg, "Earth") ||
	      !strcmp(arg, "air") || !strcmp(arg, "Air"))) {
		//not the right word.return
			return (FALSE);
	}

	if (is_abbrev(arg, "fire")) {
		loc_nr = 23918;
		strcpy(charbuf, "As you say the magic word, a sheet of fire appears and engulfs you.\r\nAfter a brief pain you finds yourself... elsewhere.\r\n");
		strcpy(roombuf, "$n mutters some words, and is engulfed in a sheet of fire. When it disappears, $n is gone.\r\n");
	}
	if (is_abbrev(arg, "water")) {
		loc_nr = 23919;
		strcpy(charbuf, "As you say the magic word, a pool of water appears and swallows you.\r\n After a short drowning sensation you find yourself...elsewhere.\r\n");
		strcpy(roombuf, "$n mutters some words, and a pool appears beneth his feet swallowing him. When the pool disappears all traces of $n is gone.\r\n");
	}
	if (is_abbrev(arg, "earth")) {
		loc_nr = 23920;
		strcpy(charbuf, "As you say the magic word, a cave-in hits you.\r\nAfter a short suffocating sensation you find yourself...elsewhere.\r\n");
		strcpy(roombuf, "After $n mutters some words, he is hit by a cave-in.\r\nAlmost as if by magic the dirt dissolves, and leaves... nothing.\r\n");
	}
	if (is_abbrev(arg, "air")) {
		loc_nr = 23921;
		strcpy(charbuf, "As you say the magic word, a tornado suddely appears and swallows you.\r\nAfter a short suffocating sensation you find yourself...elsewhere.\r\n");
		strcpy(roombuf, "$n mutters some words, and a tornado appears and swallows him. When the tornado disappears all traces of $n is gone.\r\n");
	}

	send_to_char(charbuf, ch);
	act(roombuf, TRUE, ch, 0, 0, TO_ROOM);

	/* Is this really the most effecient way to find where we 're going? */

	for (location = 0; location <= top_of_world; location++) {
		if (world[location].number == loc_nr) {
			break;
		}
	}
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, location);
	in3000_do_look(ch, "", 0);
	act("$n suddenly winks into existance.", TRUE, ch, 0, 0, TO_ROOM);
	return (TRUE);


}				/* END OF pc2300_teleport_room() */

int pc2400_cliff(struct char_data * ch, int cmd, char *arg)
{
	static int lv_number;


	if (cmd)
		return (0);

	if (GET_POS(ch) == POSITION_SLEEPING) {
		return (0);
	}

	/* don't do this all the time */
	if (number(1, 6) != 1) {
		return (0);
	}

	if (lv_number < 0 || lv_number > 5) {
		lv_number = 0;
	}

	lv_number++;

	switch (lv_number) {
	case 1:
		do_say(ch, "Well Norm, I heard the gnomes aren't really as smart as they say they are.", CMD_SAY);
		return (1);
	case 2:
		do_say(ch, "Well you know Woody, golf was invented by the Irish!", CMD_SAY);
		return (1);
	case 3:
		do_say(ch, "Hey Sam, can you hook me up with one of those twins I saw you with last week.", CMD_SAY);
		return (1);
	case 4:
		do_say(ch, "Hey Norm, do you think the mayor has been taking kick backs from the locals in Midgaard?", CMD_SAY);
		return (1);
	//case 7:
		//do_say(ch, "Thanks for staying with me, I get so lonely here.", CMD_SAY);
		//wi1000_do_emote(ch, "hugs you with so much love that tears come to your eyes.", CMD_EMOTE);
		//return (1);
	case 5:
		do_say(ch, "I feel rather confused, like a dolphin in a bed of sawdust.", CMD_SAY);
		return (1);

		/* IF ADDING TO THIS, BE SURE TO ADJUST lv_number CHECK ABOVE */
	default:
		return (0);
	}

}				/* END OF pc2400_cliff() */

int pc2500_maxlvl20(struct char_data * ch, int cmd, char *arg)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	struct obj_data *temp;
	int maxlvl = 20;
	//GET_ITEM_TYPE(obj)
		if (cmd != CMD_QUAFF &&
		    cmd != CMD_RECITE &&
		    cmd != CMD_WEAR &&
		    cmd != CMD_WIELD &&
		    cmd != CMD_HOLD) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);

	if (!*buf) {
		return (FALSE);
	}

	if (!(temp = ha2075_get_obj_list_vis(ch, buf, ch->carrying)))
		return (FALSE);

	if ((cmd == CMD_QUAFF) && (temp->obj_flags.type_flag != ITEM_POTION)) {
		act("You can only quaff potions.", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
	}
	if (cmd == CMD_QUAFF) {
		if (GET_LEVEL(ch) > maxlvl) {
			act("That didn't feel right!\n", FALSE, ch, 0, 0, TO_CHAR);
			cast_dispel_magic(50, ch, arg, SPELL_TYPE_SCROLL, ch, 0);
			ha2700_extract_obj(temp);
			return (TRUE);
		}
		else {
			ot3100_do_quaff(ch, arg, CMD_QUAFF);
			return (FALSE);
		}
	}

	if ((cmd == CMD_RECITE) && (temp->obj_flags.type_flag != ITEM_SCROLL)) {
		act("Recite is normally used for scroll's.", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
	}
	if (cmd == CMD_RECITE) {
		if (GET_LEVEL(ch) > maxlvl) {
			act("That didn't feel right!\n", FALSE, ch, 0, 0, TO_CHAR);
			spell_elsewhere(50, ch, ch, 0);
			ha2700_extract_obj(temp);
			return (TRUE);
		}
		else {
			ot3200_do_recite(ch, arg, CMD_RECITE);
			return (FALSE);
		}
	}

	if ((cmd == CMD_WEAR) && (!((CAN_WEAR(temp, ITEM_WEAR_FINGER)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_NECK)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_BODY)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_HEAD)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_LEGS)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_FEET)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_HANDS)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_ARMS)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_SHIELD)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_ABOUT)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_WAISTE)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_WRIST)) ||
				    (CAN_WEAR(temp, ITEM_HOLD)) ||
				    (CAN_WEAR(temp, ITEM_LIGHT_SOURCE)) ||
				    (CAN_WEAR(temp, ITEM_WEAR_TAIL))))) {
		act("You can't wear that.", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
	}
	if (cmd == CMD_WEAR) {
		if (GET_LEVEL(ch) > maxlvl) {
			sprintf(buf1, "The %s crumbles to dust when you try to wear it", temp->short_description);
			act(buf1, FALSE, ch, 0, 0, TO_CHAR);
			ha2700_extract_obj(temp);
			return (TRUE);
		}
		else {
			do_wear(ch, arg, CMD_WEAR);
			return (FALSE);
		}
	}

	if ((cmd == CMD_WIELD) && (!((CAN_WEAR(temp, ITEM_WIELD))))) {
		act("You can't wield.", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
	}
	if (cmd == CMD_WIELD) {
		if (GET_LEVEL(ch) > maxlvl) {
			sprintf(buf1, "The %s crumbles to dust when you try to wield it", temp->short_description);
			act(buf1, FALSE, ch, 0, 0, TO_CHAR);
			ha2700_extract_obj(temp);
			return (TRUE);
		}
		else {
			do_wield(ch, arg, CMD_WIELD);
			return (FALSE);
		}
	}

	if ((cmd == CMD_HOLD) && (!((CAN_WEAR(temp, ITEM_HOLD)) ||
				    (CAN_WEAR(temp, ITEM_LIGHT_SOURCE))))) {
		act("You can't hold that.", FALSE, ch, 0, 0, TO_CHAR);
		return (FALSE);
	}
	if (cmd == CMD_HOLD) {
		if (GET_LEVEL(ch) > maxlvl) {
			sprintf(buf1, "The %s crumbles to dust when you try to hold it", temp->short_description);
			act(buf1, FALSE, ch, 0, 0, TO_CHAR);
			ha2700_extract_obj(temp);
			return (TRUE);
		}
		else {
			do_grab(ch, arg, CMD_HOLD);
			return (FALSE);
		}
	}
	return (FALSE);
}
/* End of pc2400_maxlvl20 */

#define OBJ_SPAM		14516
#define OBJ_COUNCIL		15157
#define OBJ_WIENER		870
#define OBJ_HORN		845
#define MOB_COW			804
#define OBJ_CHEESE		14517
int cows_room(struct char_data * ch, int cmd, char *arg)
{

	if (ha1175_isexactname(GET_NAME(ch), "rlum")) {
		switch (cmd) {
			case CMD_PRAY:{	/* pray */

				struct obj_data *lv_obj, *lv_next_obj, *obj;
				struct char_data *mob;

				int r_num = db8200_real_object(OBJ_WIENER);
				obj = db5100_read_object(r_num, REAL);
				ha2100_obj_to_room(obj, ch->in_room);

				r_num = db8200_real_object(OBJ_HORN);
				obj = db5100_read_object(r_num, REAL);
				ha2100_obj_to_room(obj, ch->in_room);

				act("&wA &WHoly Light&w blinds You!&n\r\n", TRUE, ch, 0, 0, TO_ROOM);
				act("&wA &WHoly Light&w blinds You!&n\r\n", TRUE, ch, 0, 0, TO_CHAR);




				for (lv_obj = world[ch->in_room].contents; lv_obj; lv_obj = lv_next_obj) {
					lv_next_obj = lv_obj->next_content;

					if (obj_index[lv_obj->item_number].virtual == OBJ_SPAM ||
					    obj_index[lv_obj->item_number].virtual == OBJ_CHEESE) {

						int r_num = db8100_real_mobile(MOB_COW);
						mob = db5000_read_mobile(r_num, REAL);
						GET_START(mob) = world[ch->in_room].number;
						ha1600_char_to_room(mob, ch->in_room);
						act("&WMoooooo!&n\r\n", TRUE, ch, 0, 0, TO_ROOM);
						act("&WMoooooo!&n\r\n", TRUE, ch, 0, 0, TO_CHAR);


					}	/* end if */
				}	/* end for */


			}	/* end PRAY */
			break;

		default:
			return (0);
			break;
		}
	} //end if
		return
		(0);
}				/* END OF cow_room() */

int council_room(struct char_data * ch, int cmd, char *arg)
{

	/* 18 == COUNCIL */
	if (CLAN_NUMBER(ch) == 18) {
		switch (cmd) {
			case CMD_PRAY:{	/* pray */

				struct obj_data *obj;
				int r_num = db8200_real_object(OBJ_COUNCIL);
				obj = db5100_read_object(r_num, REAL);
				ha2100_obj_to_room(obj, ch->in_room);

				act("&wA &WHoly Light&w blinds You!&n\r\n", TRUE, ch, 0, 0, TO_ROOM);
				act("&wA &WHoly Light&w blinds You!&n\r\n", TRUE, ch, 0, 0, TO_CHAR);


			}
			break;

		default:
			return (0);
			break;
		}
	} //end if
		return
		(0);
}

int pc3000_pulse_attack(struct char_data * ch, struct char_data * victim)
{
	int fyredrain = 0;
	int dam = 0;
	struct obj_data *wielded = 0;

	if (!ch || !victim || !ch->equipment[WIELD])
		return 0;

	wielded = ch->equipment[WIELD];
	/* Hold on to the attacker's wielded. */

	/* Return no extra damage if we don't have a plusing weapon */
	if (!IS_SET(wielded->obj_flags.value[0], WEAPON_PULSE_ATTACK)
	    || number(0, 24) != 1)
		return 0;

	dam += number(10, 50);
	/* Damage. */

	/* Fix alignment */
	if (IS_EVIL(victim)) {
		if (GET_ALIGNMENT(ch) >= 0) {
			GET_ALIGNMENT(ch) = 1000;
		}
		else {
			GET_ALIGNMENT(ch) += 250;
		}
	}

	fyredrain = GET_MANA(victim) / 2;
	GET_MANA(victim) = fyredrain;
	GET_MANA(ch) += fyredrain / 2;


	act("&WYour $p &rpulses&W to life &yreplenishing&W your l&wifeforc&We.&n",
	    TRUE, ch, wielded, victim, TO_CHAR);
	act("&WYou feel the &Kevil &Wdrain from your body as $n's $p &rpulses &Wto life!&n",
	    TRUE, ch, wielded, victim, TO_VICT);
	act("$n&W's $p &rpulses &Wto life and drains the &Kevil &Wfrom $N&W's soul&w!&n",
	    TRUE, ch, wielded, victim, TO_NOTVICT);
	return dam;
}
