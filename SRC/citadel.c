
/* ci */
/* gv_location: 2501-3000 */
/* ****************************************************************** *
*  file: citadel.c , Special module.                  Part of DIKUMUD *
*  Usage: Procedures handling special procs for object/room/mobile    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform.  *
********************************************************************* */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "constants.h"
#include "func.h"
#include "globals.h"

#define SWORD_ACT \
"&KSo, you wish to reassemble the &WSto&wr&Kmb&wl&Wade&K do you?&n\
\n\r&KVery well, your boldness shall not go unrewarded...&n\
\n\r\
\n\rA brilliant light emanates from the broken pieces of the blade, growing until it\
\n\rbecomes unbearable to look. Then, suddenly, your world returns to normal, and in\
\n\rplace of the once broken pieces lies a beautifully crafted blade, pulsating with\
\n\rpower.\
\n\r\
\n\r&WCongratulations! You have completed the Stormblade Quest!&n\
\n\r"

int citadel_altar(struct char_data * ch, int cmd, char *arg)
{
	/*
	 Altar room : 14670
	
	 Special items:
	 14611  first book
	 14612  second book
	 14613  third book
	 14614  hilt of sword
	 14615  middle of sword
	 14616  tip of sword
	
	 Items when dropped in altar room:
	 14617  first book
	 14618  second book
	 14619  third book
	 14620  hilt of sword
	 14621  middle of sword
	 14622  tip of sword
	 */

	struct obj_data *obj;
	static int sword = -1;
	static int book1, book2, book3, hilt, middle, tip;
	static int abook1, abook2, abook3, ahilt, amiddle, atip;


	if (sword == -1) {
		book1 = 14611;
		book2 = 14612;
		book3 = 14613;
		hilt = 14614;
		middle = 14615;
		tip = 14616;

		abook1 = 14617;
		abook2 = 14618;
		abook3 = 14619;
		ahilt = 14620;
		amiddle = 14621;
		atip = 14622;

		sword = 14610;
	}



	switch (cmd) {
	case CMD_GET:		/* get */
	case CMD_TAKE:		/* get */
		do_get(ch, arg, cmd);	/* perform the get */
		/* if got any of the altar ones, switch her */
		if ((obj = ha2000_get_obj_list_virtual(abook1, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(book1, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		if ((obj = ha2000_get_obj_list_virtual(abook2, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(book2, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		if ((obj = ha2000_get_obj_list_virtual(abook3, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(book3, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		if ((obj = ha2000_get_obj_list_virtual(ahilt, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(hilt, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		if ((obj = ha2000_get_obj_list_virtual(amiddle, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(middle, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		if ((obj = ha2000_get_obj_list_virtual(atip, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(tip, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		return (1);
		break;

		/* case 67:  put */
	case CMD_DROP:		/* drop */
		do_drop(ch, arg, cmd);	/* perform the drop */
		/* if got any of the altar ones, switch her */
		if ((obj = ha2000_get_obj_list_virtual(book1, world[ch->in_room].contents))) {
			send_to_char("You place the book upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(abook1, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		if ((obj = ha2000_get_obj_list_virtual(book2, world[ch->in_room].contents))) {
			send_to_char("You place the book upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(abook2, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		if ((obj = ha2000_get_obj_list_virtual(book3, world[ch->in_room].contents))) {
			send_to_char("You place the book upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(abook3, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		if ((obj = ha2000_get_obj_list_virtual(hilt, world[ch->in_room].contents))) {
			send_to_char("You place the hilt upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(ahilt, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		if ((obj = ha2000_get_obj_list_virtual(middle, world[ch->in_room].contents))) {
			send_to_char("You place the sword fragment upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(amiddle, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		if ((obj = ha2000_get_obj_list_virtual(tip, world[ch->in_room].contents))) {
			send_to_char("You place the sword tip upon the altar\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(atip, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		return (1);
		break;

	case CMD_PRAY:		/* pray */
		if ((obj = ha2000_get_obj_list_virtual(abook1, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(abook2, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(abook3, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(ahilt, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(amiddle, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(atip, world[ch->in_room].contents))) {
			do_action(ch, arg, CMD_PRAY);
			act(SWORD_ACT, TRUE, ch, 0, 0, TO_ROOM);
			act(SWORD_ACT, TRUE, ch, 0, 0, TO_CHAR);

			for (obj = world[ch->in_room].contents; obj; obj = world[ch->in_room].contents)
				ha2700_extract_obj(obj);

			obj = db5100_read_object(sword, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
			return (1);
		}
		else {
			return (0);
		}
		break;

	default:
		return (0);
		break;
	}
	return (0);
}				/* END OF citadel_alter() */
