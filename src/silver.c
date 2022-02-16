/* si */
/* gv_location: 15501-16000 */
/* *******************************************************************
*  file: silver.c , Special module.                  Part of DIKUMUD *
*  Usage: Procedures handling special procedures                     *
*         for object/room/mobile.                                    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

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
#include "globals.h"
#include "func.h"

int si1000_nosferatu(struct char_data * ch, int cmd, char *arg)
{



	if (cmd != -1)
		return 0;
	else {
		main_log("Nosferatu killed");
	}
	return 0;
}


int forge(struct char_data * ch, int cmd, char *arg)
{
	/*
	 forge : 4510
	
	 Special items:
	 4509   ingot
	 4511   silver dagger
	 4512   silver ssword
	 4513   silver lsword
	 */

#define ingot			4509
#define forge			4510
#define s_dagger		4511
#define s_ssword		4512
#define s_lsword     4513


	struct obj_data *obj, *obj2;
	struct obj_data *theForge;



	if (!ch)
		return 0;
	theForge = ha2000_get_obj_list_virtual(forge, world[ch->in_room].contents);
	if (!theForge)
		return 0;
	switch (cmd) {
	case CMD_PUT:		/* put */
		do_put(ch, arg, cmd);	/* perform the put */
		if ((obj = ha2000_get_obj_list_virtual(ingot, theForge->contains))) {
			if ((obj2 = ha2000_get_obj_list_virtual(s_lsword, theForge->contains))) {
				send_to_char("There is no room in the mold for more silver.\n\r", ch);
				ha2700_extract_obj(obj);
				obj = db5100_read_object(ingot, VIRTUAL);
				ha1700_obj_to_char(obj, ch);
				return (1);
			}
			if ((obj2 = ha2000_get_obj_list_virtual(s_ssword, theForge->contains))) {
				send_to_char("The silver melts into the mold, forming a large sword.\n\r", ch);
				ha2700_extract_obj(obj2);
				obj2 = db5100_read_object(s_lsword, VIRTUAL);
				ha2300_obj_to_obj(obj2, theForge);
				return (1);
			}
			if ((obj2 = ha2000_get_obj_list_virtual(s_dagger, theForge->contains))) {
				send_to_char("The silver melts into the mold, forming a small sword.\n\r", ch);
				ha2700_extract_obj(obj2);
				obj2 = db5100_read_object(s_ssword, VIRTUAL);
				ha2300_obj_to_obj(obj2, theForge);
				return (1);
			}
			send_to_char("The silver melts into the mold, forming a silver dagger.\n\r", ch);
			ha2700_extract_obj(obj);
			obj = db5100_read_object(s_dagger, VIRTUAL);
			ha2300_obj_to_obj(obj, theForge);
			return 1;
		}
		/* wasnt any of the ok items melt it! */
		for (obj = theForge->contains;
		     obj;
		     obj = theForge->contains) {
			act("$o melts in the heat of the forge.", 0, ch, obj, 0, TO_CHAR);
			ha2700_extract_obj(obj);
		}
		return (1);
		break;

	default:
		return (0);
		break;
	}
}


int cliff(struct char_data * ch, int cmd, char *arg)
{
	/*
	 edge of cliff room : 4513
	
	 Special items:
	 4516  rope
	 4517  grapple
	 4518  grapple on ground with rope
	 */

#define grapple 		4517
#define rope 			4516
#define rope_grapple 4518

#define GET			CMD_GET
#define DROP			CMD_DROP
#define TAKE			CMD_TAKE

	struct obj_data *obj, *obj2;



	switch (cmd) {
	case GET:		/* get */
	case TAKE:		/* get */
		do_get(ch, arg, cmd);	/* perform the get */
		/* if got any of the altar ones, switch her */
		if ((obj = ha2000_get_obj_list_virtual(rope_grapple, ch->carrying))) {
			ha2700_extract_obj(obj);
			obj = db5100_read_object(grapple, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
			obj = db5100_read_object(rope, VIRTUAL);
			ha1700_obj_to_char(obj, ch);
		}
		return (1);
		break;

	case DROP:		/* drop */
		do_drop(ch, arg, cmd);	/* perform the drop */
		/* if got any of the altar ones, switch her */
		if ((obj = ha2000_get_obj_list_virtual(rope, world[ch->in_room].contents))
		    && (obj2 = ha2000_get_obj_list_virtual(grapple, world[ch->in_room].contents))) {
			send_to_char("You attach the rope to the grapple, and hang it over the cliff.\n\r", ch);
			ha2700_extract_obj(obj);
			ha2700_extract_obj(obj2);
			obj = db5100_read_object(rope_grapple, VIRTUAL);
			ha2100_obj_to_room(obj, ch->in_room);
		}
		return (1);
		break;

	case CMD_WEST:
		if (!(obj = ha2000_get_obj_list_virtual(rope_grapple, world[ch->in_room].contents))) {
			send_to_char("Whoa... that's far to steep a cliff to climb down unaided.\n\r", ch);
			return (1);
		}
		return (0);
		break;

	default:
		return (0);
		break;
	}
}
