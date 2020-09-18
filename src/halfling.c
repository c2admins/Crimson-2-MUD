
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

#define NARSIL_ACT \
"&KYou wish to reforge the Legendary sword, Narsil?&n\
\n\r&KVery well, let's hope you are noble enough to wield its' power.&n\
\n\r\
\n\rThe blacksmith begins working his craft on the broken shards of the sword.\
\n\rOnce the blade has been mended this smith hands you a beautifully crafted blade,\
\n\rpulsating with power.\
\n\r\
\n\r&WCongratulations! You have completed the Narsil Quest!&n\
\n\r"

int halfling_smith(struct char_data * ch, int cmd, char *arg)
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
	static int shard1, shard2, shard3, shard4, hilt;

	if (sword == -1) {
		shard1 = 17622;
		shard2 = 17623;
		shard3 = 17624;
		shard4 = 17625;
		halt = 17626;
		
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
}				/* END OF citadel_alter() */
