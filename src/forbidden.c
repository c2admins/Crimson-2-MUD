/* fi */
/* *******************************************************************
*  file: forbidden.c , Special module.       Part of Crimson MUD     *
*  Usage: Special code for Forbidden Inn zone                        *
*                  Written by Bingo                                  *
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
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "modify.h"
#include "ansi.h"
#include "globals.h"
#include "func.h"
#include "forbidden.h"

int fi1000_door_knocker(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	int knocker, lv_entrance, lv_inside;

	if (cmd != CMD_GRAB) {
		return (FALSE);
	}

	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);

	if (!*buf) {
		return (FALSE);
	}

	if (!is_abbrev(buf, "knocker"))
		return (FALSE);

	lv_entrance = db8000_real_room(FORBIDDEN_ENTRANCE);
	lv_inside = db8000_real_room(FORBIDDEN_INSIDE);

	if (cmd == CMD_GRAB) {
		if (is_abbrev(buf, "knocker")) {
			for (; *arg == ' '; arg++);
			send_to_char("You grab the eagle headed knocker and pound it on the door.\n\r", ch);
			act("$n grabs the eagle headed knocker and pounds it on the door.", TRUE, ch, 0, 0, TO_ROOM);
			WAIT_STATE(ch, PULSE_VIOLENCE * .5);
		}
		else {
			do_grab(ch, buf, CMD_GRAB);
		}
		knocker = 1;
		if (knocker == 1) {
			switch (number(0, 3)) {
			case 0:{
					if (time_info.hours >= 00 && time_info.hours <= 06) {
						knocker = 2;
					}
					else {
						co3300_send_to_room("Someone from behind the door yells 'We are not open, come back another time!'\n\r", ch->in_room);
						knocker = 0;
					}
					break;
				}
			case 1:{
					send_to_char("*knock* *knock* Nothing at all, maybe you should try again.\n\r", ch);
					knocker = 0;
					break;
				}
			case 2:{
					send_to_char("Try knocking again, maybe they didn't hear you.\n\r", ch);
					knocker = 0;
					break;
				}
			case 3:{
					send_to_char("Maybe no one is here, might keep trying anyways.\n\r", ch);
					knocker = 0;
					break;
				}
			}

			if (knocker == 2) {
				send_to_char("Code is incomplete.\n\r", ch);
				knocker = 0;
			}
		}
	}
	return (FALSE);
}
