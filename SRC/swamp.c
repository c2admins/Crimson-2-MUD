
/* ci */
/* gv_location: 2501-3000 */
/* ****************************************************************** *
*  file: swamp.c , Special module.                  Part of DIKUMUD *
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

#define SMITER_ACT \
"&PThank you, let me work on these for a moment...&n\
\n\r \
\n\r&BThe old man begins working his magic to mend the broken pieces.&n\
\n\r\
\n\rThe old man pauses a moment to collect his thoughts... \
\n\r\
\n\rThe priest murmurs a strange chant and slowly the pieces begin to come together.\
\n\r\
\n\rOut of the 3 pieces he constructs a holy Smiter. \
\n\r\
\n\r&PThere you are. Please use it wisely.&n\
\n\r"

int swamp_priest(struct char_data * ch, int cmd, char *arg)
{
	/*
	 Altar room : 16956
	
	 Special items:
	 16925  handle smiter
	 16926  shaft smiter
	 16927  head smiter
	 
	 Reward:
	 16934  Holy Smiter
	 
	 */

	struct obj_data *obj;
	static int mace = -1;
	static int handle, shaft, head;

	if (mace == -1) {
		handle = 16925;
		shaft = 16926;
		head = 16927;
			
		mace = 16934;
	}

	switch (cmd) {
	case CMD_PRAY:		/* pray */
		if ((obj = ha2000_get_obj_list_virtual(handle, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(shaft, world[ch->in_room].contents)) &&
			(obj = ha2000_get_obj_list_virtual(head, world[ch->in_room].contents))) {
			do_action(ch, arg, CMD_PRAY);
			act(SMITER_ACT, TRUE, ch, 0, 0, TO_ROOM);
			act(SMITER_ACT, TRUE, ch, 0, 0, TO_CHAR);

			for (obj = world[ch->in_room].contents; obj; obj = world[ch->in_room].contents)
				ha2700_extract_obj(obj);

			obj = db5100_read_object(mace, VIRTUAL);
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
}				/* END OF swamp_priest() */
