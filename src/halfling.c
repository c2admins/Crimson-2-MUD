
/* ci */
/* gv_location: 2501-3000 */
/* ****************************************************************** *
*  file: halfling.c , Special module.                  Part of DIKUMUD *
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

#define NARSIL_ACT \
"&WThank you, let me work on these for a moment...&n\
\n\r \
\n\r&WThe blacksmith begins heating the metal pieces in his forge.&n\
\n\r\
\n\r&WTaking the pieces from his forge, the blacksmith begins pounding them \
\n\rtogether with his hammer.\
\n\r\
\n\r&WAfter several hours the blacksmith wipes the sweat from his brow and smiles.\
\n\r\
\n\r&WOut of the five pieces he constructs a finely crafted blade. \
\n\r\
\n\r&WThere you are. Please use it wisely.&n\
\n\r"

int halfling_smith(struct char_data * ch, int cmd, char *arg)
{
	/*
	 Altar room : 17607
	
	 Special items:
	 17622  shard of sword 1
	 17623  shard of sword 2
	 17624  shard of sword 3
	 17625  shard of sword 4
	 17626  hilt of sword
	
	 */

	struct obj_data *obj;
	static int sword = -1;
	static int shard1, shard2, shard3, shard4, hilt;

	if (sword == -1) {
		shard1 = 17622;
		shard2 = 17623;
		shard3 = 17624;
		shard4 = 17625;
		hilt = 17626;
		
		sword = 17627;
	}



	switch (cmd) {
	case CMD_PRAY:		/* pray */
		if ((obj = ha2000_get_obj_list_virtual(shard1, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(shard2, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(shard3, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(shard4, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(hilt, world[ch->in_room].contents))) {
			do_action(ch, arg, CMD_PRAY);
			act(NARSIL_ACT, TRUE, ch, 0, 0, TO_ROOM);
			act(NARSIL_ACT, TRUE, ch, 0, 0, TO_CHAR);

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
}				/* END OF halfling_smith() */
