/* sm */
/* *******************************************************************
*  file: smobs.c , Special module.           Part of Crimson MUD     *
*  Usage: Procedures handling special mobs                           *
*                     Written by Hercules                            *
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


int does_contain(char *arg, char *arg2)
{
	char buf[MAX_STRING_LENGTH];
	int idx;

	strcpy(buf, arg);
	if (!*arg2)
		return (FALSE);
	for (idx = 0; idx < strlen(buf); idx++)
		buf[idx] = LOWER(buf[idx]);
	for (idx = 0; idx < strlen(buf); idx++) {

		if (strncmp((char *) (buf + idx), arg2, strlen(arg2)) == 0)
			return (TRUE);
	}
	return (FALSE);
}				/* end of does_contain */

int sm1000_midgaard_cityguard(struct char_data * ch, int cmd, char *arg)
{
	struct char_data *temp_char;
	struct char_data *mob = 0;
	char buf[MAX_STRING_LENGTH];

	if (!cmd)
		return (FALSE);

	if (GET_LEVEL(ch) > PK_LEV)
		return (FALSE);

	if (!ch->desc)
		return (FALSE);	/* You've forgot FALSE - NPC couldn't leave */

	for (temp_char = world[ch->in_room].people; (temp_char) && (!mob);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char))
			if (mob_index[temp_char->nr].func == sm1000_midgaard_cityguard)
				mob = temp_char;

	if (mob->master)
		return (FALSE);

	if (cmd == CMD_SAY) {
		if (ha1000_is_there_a_reserved_word(arg, curse_gossip, BIT0)) {
			do_say(ch, arg, CMD_SAY);
			if (ch->language) {
				do_say(mob, "Don't use that kind of language around here!", CMD_SAY);
				hit(mob, ch, TYPE_UNDEFINED);
			}
			else {
				sprintf(buf, "If you use language like that again %s i'll have to kill you!", GET_REAL_NAME(ch));
				do_say(mob, buf, CMD_SAY);
				ch->language = 1;
			}
			return (TRUE);
		}
	}

	return (FALSE);

}				/* end of sm1000_midgaard_cutyguard() */

int sm1100_midgaard_jinipan(struct char_data * ch, int cmd, char *arg)
{
	struct char_data *temp_char;
	struct char_data *mob = 0;
	char buf[MAX_STRING_LENGTH];


	/* disable this code for awhile */
	return (FALSE);

	if (!ch->desc)
		return (FALSE);

	for (temp_char = world[ch->in_room].people; (temp_char) && (!mob);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char))
			if (mob_index[temp_char->nr].func == sm1100_midgaard_jinipan)
				mob = temp_char;


	if ((cmd == CMD_GOSSIP) ||
	    (cmd == CMD_EMOTE) ||
	    (cmd == CMD_SAY) ||
	    (cmd == CMD_SHOUT) ||
	    (cmd == CMD_AVATAR)) {
		if (ha1000_is_there_a_reserved_word(arg, curse_gossip, BIT0)) {
			switch (ch->language) {
			case 0:{
					sprintf(buf, "%s Please watch your language %s, or I will be forced to do something about it!",
						GET_REAL_NAME(ch),
						GET_REAL_NAME(ch));
					do_tell(mob, buf, CMD_TELL);
				} break;
			case 1:{
					sprintf(buf, "I warned you %s, time to teach you some manors!", GET_REAL_NAME(ch));
					do_gossip(mob, buf, CMD_GOSSIP);
					hit(mob, ch, TYPE_UNDEFINED);
					ch->language = 0;
				} break;
			default:
				ch->language = 0;
				break;
			}
			ch->language++;
			return (TRUE);
		}
	}

	return (FALSE);

}				/* end of sm1100_midgaard_jinipan() */
