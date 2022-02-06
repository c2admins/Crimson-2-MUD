/* ch */
/* gv_location: 2001-2500 */
/**************************************************************
* channel.c                                                  *
*   Implementation of communications commands                *
**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "constants.h"
#include "spells.h"
#include "ansi.h"
#include "func.h"
#include "globals.h"
#include "history.h"

/* Arbitrary define */
#define MAX_NOTE_LENGTH	1000

/* External Variables */
/* Used to check last gossip/shout/auction - keeps down spamming */

static char lv_last_gos_shout[80];

void do_disconnect(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct char_data *vict;

	/* STRIP OUT PLAYER TO MODIFY */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	CHECK_LEVEL(IMO_IMM)
		if (!(*buf)) {
		send_to_char("Everybody?\r\n", ch);
		return;
	}

	vict = 0;
	vict = ha3100_get_char_vis(ch, buf);
	if (!vict) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "Unable to locate %s.\r\n", buf);
		send_to_char(buf2, ch);
		return;
	}

	if ((IS_NPC(vict))) {
		send_to_char("I don't think we'll disconnect mobs today.\r\n",
			     ch);
		return;
	}

	/* VICTIM HIGHER IN LEVEL? */
	if (GET_LEVEL(ch) <= GET_LEVEL(vict) &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    ch != vict) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is to awesome for you to mess with!\r\n",
			GET_NAME(vict));
		send_to_char(buf, ch);
		return;
	}

	if (!(vict->desc)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s isn't connected.\r\n", GET_NAME(vict));
		send_to_char(buf, ch);
		return;
	}

	if (*(arg)) {
		send_to_char(arg, vict);
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s is outta here!\r\n", GET_NAME(vict));
	send_to_char(buf, ch);


	bzero(buf, sizeof(buf));
	sprintf(buf, "%s has disconnected %s.",
		GET_REAL_NAME(ch), GET_REAL_NAME(vict));
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

	co2500_close_single_socket(vict->desc);

	return;

}				/* END OF do_disconnect() */

void do_port(struct char_data * ch, char *argument, int cmd)
{
	char buf[MAX_INPUT_LENGTH];

	bzero(buf, sizeof(buf));
	sprintf(buf, "You are currently on port: %d", gv_port);
	send_to_char(buf, ch);

}				/* END OF do_port */


void do_say(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_INPUT_LENGTH + 128], arg[MAX_INPUT_LENGTH + 128], buf2[MAX_INPUT_LENGTH + 128],
	  last_two_chars[3];

	MUZZLECHECK();
	bzero(arg, sizeof(arg));
	strcpy(arg, argument);
	argument = arg;

	for (; *argument == ' '; argument++);

	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));

	if (cmd == CMD_QUEST)
		cmd = CMD_SAY;

	if (cmd == CMD_REPORT) {

		if (GET_LEVEL(ch) < IMO_LEV) {	/* For Mortals */

			sprintf(buf, "You report 'H[%d/%d], M[%d/%d], V[%d/%d], and I have %d experience points to level.'",
				GET_HIT(ch), GET_HIT_LIMIT(ch),
				GET_MANA(ch), GET_MANA_LIMIT(ch),
				GET_MOVE(ch), GET_MOVE_LIMIT(ch), LEVEL_EXP(GET_LEVEL(ch)) - GET_EXP(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_SAY, 0);

			sprintf(buf, "%s reports 'H[%d/%d], M[%d/%d], V[%d/%d], and I have %d experience points to level.'",
				GET_REAL_NAME(ch),
				GET_HIT(ch), GET_HIT_LIMIT(ch),
				GET_MANA(ch), GET_MANA_LIMIT(ch),
				GET_MOVE(ch), GET_MOVE_LIMIT(ch), LEVEL_EXP(GET_LEVEL(ch)) - GET_EXP(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_ROOM, CLR_SAY, 0);
		}

		else if (GET_LEVEL(ch) == IMO_LEV) {	/* For Avatars */

			sprintf(buf, "You report 'H[%d/%d], M[%d/%d], V[%d/%d] and my kills/death score is %d.'",
				GET_HIT(ch), GET_HIT_LIMIT(ch),
				GET_MANA(ch), GET_MANA_LIMIT(ch),
			     GET_MOVE(ch), GET_MOVE_LIMIT(ch), GET_SCORE(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_SAY, 0);

			sprintf(buf, "%s reports 'H[%d/%d], M[%d/%d], V[%d/%d] and my kills/death score is %d.'",
				GET_REAL_NAME(ch),
				GET_HIT(ch), GET_HIT_LIMIT(ch),
				GET_MANA(ch), GET_MANA_LIMIT(ch),
			     GET_MOVE(ch), GET_MOVE_LIMIT(ch), GET_SCORE(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_ROOM, CLR_SAY, 0);
		}

		else {
			//Higher than avats.

				sprintf(buf, "You report 'H[%d/%d], M[%d/%d], V[%d/%d]'",
					GET_HIT(ch), GET_HIT_LIMIT(ch),
					GET_MANA(ch), GET_MANA_LIMIT(ch),
					GET_MOVE(ch), GET_MOVE_LIMIT(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_SAY, 0);

			sprintf(buf, "%s reports 'H[%d/%d], M[%d/%d], V[%d/%d]'",
				GET_REAL_NAME(ch),
				GET_HIT(ch), GET_HIT_LIMIT(ch),
				GET_MANA(ch), GET_MANA_LIMIT(ch),
				GET_MOVE(ch), GET_MOVE_LIMIT(ch));
			ansi_act(buf, FALSE, ch, 0, 0, TO_ROOM, CLR_SAY, 0);
		}


		return;
	}


	if (!*(argument)) {
		send_to_char("Yes, but WHAT do you want to say?\r\n", ch);
		return;
	}

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {

		if (GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("The Librarian tells you this is a soundproof room.\r\n", ch);
			return;
		}
		else
			send_to_char("Its soundproof in here, but I'm not stopping you.\r\n", ch);
	}
	//Restructred and modified by Relic.

	/* GET LAST TWO CHARS */

		last_two_chars[0] = ' ';
	last_two_chars[1] = ' ';
	if (strlen(argument) > 1) {
		last_two_chars[0] = argument[strlen(argument) - 2];
		last_two_chars[1] = argument[strlen(argument) - 1];
	}

	last_two_chars[2] = 0;

	int length = strlen(argument);
	if (cmd == CMD_QUEST) {
		sprintf(buf, "&W$n says,\r\n&Y%s&n", argument);
		sprintf(buf2, "&WYou say,\r\n&Y%s&n", argument);
	}

	/* If we end in a string of periods, just mutter it */
	else if (!strcmp(last_two_chars, "..")) {
		sprintf(buf, "$n mutters '%s&n'", argument);
		sprintf(buf2, "You mutter '%s&n'", argument);
	}

	/* STATE */
	else if (argument[length - 1] == '.') {
		sprintf(buf, "$n states '%s&n'", argument);
		sprintf(buf2, "You state '%s&n'", argument);
	}

	/* ASK */
	else if (argument[length - 1] == '?') {
		sprintf(buf, "$n asks '%s&n'", argument);
		sprintf(buf2, "You ask '%s&n'", argument);
	}

	/* EXCLAIM */
	else if (argument[length - 1] == '!') {
		sprintf(buf, "$n exclaims '%s&n'", argument);
		sprintf(buf2, "You exclaim '%s&n'", argument);
	}

	/* SING */
	else if (argument[length - 1] == '&') {
		/* we don't need the & anymore */
		argument[length - 1] = 0;
		sprintf(buf, "$n sings '%s!&n'", argument);
		sprintf(buf2, "You sing '%s!&n'", argument);
	}

	/* HAPPILY */
	else if (!strcmp(last_two_chars, ":)") ||
		 !strcmp(last_two_chars, "=)") ||
		 !strcmp(last_two_chars, "(:") ||
		 !strcmp(last_two_chars, "(=")) {
		sprintf(buf, "$n says happily '%s&n'", argument);
		sprintf(buf2, "You say happily '%s&n'", argument);
	}

	else if (!strcmp(last_two_chars, ";)") ||
		 !strcmp(last_two_chars, "(;")) {
		sprintf(buf, "$n says jestingly '%s&n'", argument);
		sprintf(buf2, "You say jestingly '%s&n'", argument);
	}

	/* SADLY */
	else if (!strcmp(last_two_chars, ":(") ||
		 !strcmp(last_two_chars, "):")) {
		sprintf(buf, "$n says sadly '%s&n'", argument);
		sprintf(buf2, "You say sadly '%s&n'", argument);
	}

	/* DEFAULT TO SAY */
	else {
		sprintf(buf, "$n says '%s&n'", argument);
		sprintf(buf2, "You say '%s&n'", argument);
	}

	ansi_act(buf, FALSE, ch, 0, 0, TO_ROOM, CLR_SAY, 0);
	ansi_act(buf2, FALSE, ch, 0, 0, TO_CHAR, CLR_SAY, 0);

	qu2000_check_quest(ch, argument);

	return;

}				/* END OF do_say() */

void do_mob_master_muzzle(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	/* IF NOT MASTER, WE CAN'T DO ANYTHING */
	if (!ch->master) {
		send_to_char("If you had a master we would do more now!\r\n", ch);
		sprintf(buf, "Mob %s has been forced to use inappropriate language - LOGGED for disiplinary action.\r\n",
			GET_REAL_NAME(ch));
		do_info(buf, 1, MAX_LEV, ch);
		return;
	}

	/* IS THE MASTER AN NPC? */
	if (IS_NPC((ch->master))) {
		send_to_char("If you didn't have an NPC master, we would do more now!\r\n", ch);
		sprintf(buf, "Mob %s has been forced to use inappropriate language - LOGGED for disiplinary action.\r\n",
			GET_REAL_NAME(ch));
		do_info(buf, 1, MAX_LEV, ch);
		return;
	}

	/* MUZZLE MASTER */
	sprintf(buf, "%s forced a mob to use inappropriate language and has been muzzled.\r\n",
		GET_REAL_NAME(ch->master));
	SET_BIT(GET_ACT2(ch->master), PLR2_MUZZLE_SHOUT);
	ch->master->specials.time_of_muzzle = time(0);
	send_to_char("You need to think about others!  You are muzzled.\r\n", ch->master);

	/* THEY DON'T DESERVE THIS MOB ANYMORE */
	ha4100_stop_follower(ch, END_FOLLOW_BAD_LANGUAGE);
	return;

}				/* END OF do_mob_master_muzzle() */


void do_shout(struct char_data * ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char lv_this_shout[80];
	char arg[MAX_STRING_LENGTH];
	struct descriptor_data *i;
	int lv_send_message, idx;

	if (IS_NPC(ch) && !HUNTING(ch))
		return;

	bzero(arg, sizeof(arg));
	strcpy(arg, argument);
	argument = arg;
	REMOVE_COLOR(argument);
	for (; *argument == ' '; argument++);

	if (!(*argument)) {
		send_to_char("Shout? Yes! Fine! Shout we must, but WHAT??\r\n", ch);
		return;
	}

	if (ha1000_is_there_a_reserved_word(argument, curse_gossip, BIT0)) {
		for (idx = 0; idx < strlen(argument); idx++)
			argument[idx] = LOWER(argument[idx]);
	}

	sprintf(buf, "You shout, '%s'", argument);

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		ansi_act(buf, 0, ch, 0, 0, TO_CHAR, CLR_SHOUT, 0);
		return;
	}

	MUZZLECHECK();

	/* IS GOSSIP/SHOUT BLOCKED? */
	if (IS_PC(ch) &&
	    (GET_LEVEL(ch) < IMO_IMP &&
	     IS_SET(GET_ACT2(ch), PLR2_MUZZLE_SHOUT))) {
		send_to_char("You are MUZZLED and prevented from using shout/gossip/auction!!!\r\n", ch);
		send_to_char("Use SCORE command to see when it expires.\r\n", ch);
		return;
	}

	/* IF WE ARE A NPC, AND OUR MASTER IS BLOCKED, WE ARE BLOCKED */
	if (IS_NPC(ch) &&
	    IS_AFFECTED(ch, AFF_CHARM) &&
	    ch->master) {
		if (IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_SHOUT) ||
		    IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_ALL)) {
			act("$n tells you that you are muzzled.",
			    FALSE, ch, 0, ch->master, TO_VICT);
			act("$n tells $N that $E is muzzled.",
			    FALSE, ch, 0, ch->master, TO_NOTVICT);
			return;
		}		/* END OF a muzzled master */
	}			/* END OF a charmed mob */

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOSHOUT))) {
			send_to_char("And you have SHOUT turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOSHOUT))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
			send_to_char("Turning on SHOUT channel.\r\n", ch);
		}
	}

	/* IS THIS A DUPLICATE OF THE LAST SHOUT? */
	bzero(lv_this_shout, sizeof(lv_this_shout));
	sprintf(lv_this_shout, "%s", GET_REAL_NAME(ch));
	strncat(lv_this_shout, argument,
		sizeof(lv_this_shout) - strlen(lv_this_shout) - 5);
	if (!(strcmp(lv_last_gos_shout, lv_this_shout))) {
		send_to_char("They've heard that one before.\r\n", ch);
		return;
	}

	ansi_act(buf, 0, ch, 0, 0, TO_CHAR, CLR_SHOUT, 0);
	sprintf(buf, "$n shouts '%s'", argument);

	for (i = descriptor_list; i; i = i->next) {

		if (i->character != ch && !i->connected) {
			if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost shalira 22.07, 01
					sprintf(buf, "%s shouts '%s'", GET_REAL_NAME(ch), argument);
			}
			if (GET_INCOGNITO(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost Shalira 22.07 .01
					sprintf(buf, "Someone shouts '%s'", argument);
			}
			lv_send_message = TRUE;
			if (IS_SET(world[i->character->in_room].room_flags,
				   RM1_SOUNDPROOF))
				lv_send_message = FALSE;
			if (IS_SET(GET_ACT3(i->character), PLR3_NOSHOUT))
				lv_send_message = FALSE;
			if (GET_LEVEL(ch) > IMO_IMM)
				lv_send_message = TRUE;

			if (lv_send_message == TRUE) {
				ansi_act(buf, 0, ch, 0, i->character, TO_VICT, CLR_SHOUT, 0);
			}
		}		/* user connected */
	}			/* END OF for loop */

	/* DID USER CURSE? */
	if ((IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) &&
	    ha1050_is_there_a_curse_word(argument)) {
		send_to_char("You shouldn't have said that!!!\r\n", ch);
		bzero(buf, sizeof(buf));
		if (IS_NPC(ch)) {
			do_mob_master_muzzle(ch, argument, cmd);
		}
		else {
			sprintf(buf, "%s shouted with inappropriate language and has been muzzled.\r\n",
				GET_REAL_NAME(ch));
			SET_BIT(GET_ACT2(ch), PLR2_MUZZLE_SHOUT);
			ch->specials.time_of_muzzle = time(0);
			send_to_char("You need to think about others!  You are muzzled.\r\n", ch);
			do_info(buf, 1, MAX_LEV, ch);
		}
	}

	bzero(lv_last_gos_shout, sizeof(lv_last_gos_shout));
	sprintf(lv_last_gos_shout, "%s", GET_REAL_NAME(ch));
	strncat(lv_last_gos_shout, argument,
		sizeof(lv_last_gos_shout) - strlen(lv_last_gos_shout) - 5);

	/* MAKE EM READ HELP SHOUT */
	if (GET_LEVEL(ch) < 10) {
		co1500_write_to_head_q("HELP SHOUT", &ch->desc->input);
		return;
	}
}				/* END OF do_shout() */

void do_tell(struct char_data * ch, char *argument, int cmd)
{

	struct char_data *vict, *k;
	struct follow_type *f;
	char name[MAX_INPUT_LENGTH];
	char message[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH + 128];
	char arg[MAX_INPUT_LENGTH + 128];
	char save_position;
	int i, vict_invis;

	MUZZLECHECK();

	bzero(arg, sizeof(arg));
	strcpy(arg, argument);
	argument = arg;
	if (cmd == CMD_GROUP_TELL) {
		strcpy(name, "GROUP");
		strcpy(message, argument + 1);	/* remove leading space */
	}

	else if ((IS_PC(ch)) && (cmd == CMD_REPLY)) {
		if (ch->desc->reply_to) {
			strcpy(name, ch->desc->reply_to);
			strcpy(message, argument + 1);
		}
		else {
			send_to_char("No one has talked to you recently.\r\n", ch);
			return;
		}
	}
	else
		half_chop(argument, name, message);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOTELL))) {
			send_to_char("And you have TELL turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOTELL))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOTELL);
			send_to_char("Turning on TELL channel.\r\n", ch);
		}
	}

	/* convert name to lower case */
	for (i = 0; *(name + i); i++)
		*(name + i) = LOWER(*(name + i));

	if (!*name || !*message) {
		send_to_char("Who do you wish to tell what??\r\n", ch);
		return;
	}

	if (!(strcmp(name, "group"))) {

		if (!IS_AFFECTED(ch, AFF_GROUP)) {
			send_to_char("You are not grouped!\r\n", ch);
			return;
		}

		sprintf(buf, "%s tells the group '%s&n'",
			(IS_NPC(ch) ? ch->player.short_descr : GET_REAL_NAME(ch)), message);
		k = (ch->master) ? ch->master : ch;
		if (IS_AFFECTED(k, AFF_GROUP) && k != ch) {
			/* MAKE SURE THEY ARE AWAKE */
			save_position = GET_POS(k);
			GET_POS(k) = POSITION_STANDING;
			ansi_act(buf, FALSE, k, 0, 0, TO_CHAR, CLR_COMM, 0);
			GET_POS(k) = save_position;
		}
		for (f = k->followers; f; f = f->next) {
			if (IS_AFFECTED(f->follower, AFF_GROUP) &&
			    f->follower != ch &&
			    GET_POS(f->follower) > POSITION_DEAD) {
				/* MAKE SURE THEY ARE AWAKE */
				save_position = GET_POS(f->follower);
				GET_POS(f->follower) = POSITION_STANDING;
				ansi_act(buf, FALSE, f->follower, 0, 0, TO_CHAR, CLR_TELL, 0);
				GET_POS(f->follower) = save_position;
			}
		}

		/* TEMPORARILY WAKE THEM AND SEND MESSAGE */
		save_position = GET_POS(ch);
		GET_POS(ch) = POSITION_STANDING;
		sprintf(buf, "You tell the group, '%s&n'", message);
		ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_TELL, 0);
		GET_POS(ch) = save_position;

		return;

	}			/* END OF TELL GROUP */
	if (!(vict = ha3100_get_char_vis(ch, name)) &&
	    (cmd != CMD_REPLY)) {
		send_to_char("You're unable to locate that person.\r\n", ch);
		return;
	}

	vict_invis = 0;
	if (!vict) {
		if (!(vict = get_char(name))) {
			send_to_char("You're unable to locate that person.\r\n", ch);
			return;
		}
		else {
			vict_invis = 1;
		}
	}

	if (IS_SET(GET_ACT3(vict), PLR3_NOTELL) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("That person is busy and doesn't want anyone telling to them.\r\n", ch);
		return;
	}

	if (!vict->desc &&
	    IS_PC(vict)) {
		send_to_char("That person is link dead and can't hear you.\r\n", ch);
		return;
	}

	if (ch == vict) {
		send_to_char("You try to tell yourself something.\r\n", ch);
		return;
	}

/* We don't mind if he's asleep :p

	if ((GET_POS(vict) == POSITION_SLEEPING) &&
			(GET_LEVEL(ch) < IMO_IMM)) {
		act("$E is asleep and can't hear you.",
				FALSE,ch,0,vict,TO_CHAR);
		return;
	}
*/

	if ((IS_SET(GET_ACT2(ch), PLR2_JAILED)) &&
	    (ch->in_room != vict->in_room) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		sprintf(buf, "You tell %s, 'Hey, you know, I wouldn't blame you for sitebanning me for life.  In fact, I would encourage it, I seem to have a problem....  It all started when I picked up the habbit of molesting little puppies...it only got worse.&n'", GET_REAL_NAME(vict));
		ansi_act(buf, 0, ch, 0, 0, TO_CHAR, CLR_TELL, 0);
		return;
	}

	if ((IS_SET(world[vict->in_room].room_flags, RM1_SOUNDPROOF)) &&
	    (ch->in_room != vict->in_room) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("That person can't hear you.\r\n", ch);
		return;
	}

	if (is_ignored(vict, GET_REAL_NAME(ch)) && (GET_LEVEL(ch) <= PK_LEV)) {
		send_to_char("That person has you on ignore.\r\n", ch);
		return;
	}

	if (IS_SET(GET_ACT2(vict), PLR2_AFK)) {
		send_to_char("That person is AFK and might not hear you.\r\n", ch);
	}

	if (vict->specials.timer > 4) {
		char tempbuf[MAX_STRING_LENGTH];
		sprintf(tempbuf, "That person has been idle for %d ticks, so be patient.\r\n", vict->specials.timer);
		send_to_char(tempbuf, ch);
	}

	if (IS_SET(GET_ACT2(vict), PLR2_AFW)) {
		send_to_char("That person is AFW and might take a few minutes to respond.\r\n", ch);
	}
	if (CAN_SEE(vict, ch)) {
		sprintf(buf, "%s tells you '%s&n'",
			GET_REAL_NAME(ch), message);
	}
	else if (GET_GHOST(ch) > GET_LEVEL(vict)) {
		//Incognito & ghost Shalira 22.07 .01
			sprintf(buf, "%s tells you '%s&n'",
				GET_REAL_NAME(ch), message);
	}
	else {
		sprintf(buf, "Someone tells you '%s&n'",
			message);
	}

/*
 if (GET_LEVEL(ch) < IMO_MEDITATOR) {
	 ansi_act(buf,FALSE,vict,0,0,TO_CHAR,CLR_TELL, 0);
 }
 else {
	 */
	ansi_act(buf, FALSE, vict, 0, 0, TO_CHAR, CLR_TELL, AA_IGNORE_SLEEP);

	if (vict_invis) {
		sprintf(buf, "You tell someone, '%s&n'", message);
	}
	else {
		sprintf(buf, "You tell %s, '%s&n'", GET_REAL_NAME(vict), message);
	}
/*        if (GET_LEVEL(ch) < IMO_IMM) {
           ansi_act(buf,FALSE,ch,0,0,TO_CHAR,CLR_TELL, 0);
}
else {
	*/
	ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_TELL, AA_IGNORE_SLEEP);

	if (IS_PC(vict) && IS_PC(ch)) {
		vict->desc->reply_to = ch->player.name;
	}
	return;

}				/* END OF do_tell() */

void do_whisper(struct char_data * ch, char *argument, int cmd)
{

	struct char_data *vict;
	char name[MAX_INPUT_LENGTH];
	char message[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH + 128];
	char arg[MAX_INPUT_LENGTH + 128];

	MUZZLECHECK();

	bzero(arg, sizeof(arg));
	strcpy(arg, argument);
	argument = arg;
	REMOVE_COLOR(argument);
	half_chop(argument, name, message);

	if (!*name || !*message) {
		send_to_char("Who do you want to whisper to.. and what??\r\n", ch);
		return;
	}

	if (!(vict = ha2125_get_char_in_room_vis(ch, name))) {
		send_to_char("No-one by that name here..\r\n", ch);
		return;
	}

	if (vict == ch) {
		ansi_act("$n whispers quietly to $mself.", FALSE, ch, 0, 0, TO_ROOM, CLR_COMM, 0);
		ansi_act("You can't seem to get your mouth close enough to your ear...", FALSE, ch, 0, 0, TO_CHAR, CLR_COMM, 0);
		return;
	}

	if ((GET_POS(vict) == POSITION_SLEEPING) &&
	    (GET_LEVEL(ch) < IMO_SPIRIT)) {
		act("$E is asleep and can't hear you.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}

	if ((IS_SET(GET_ACT2(ch), PLR2_JAILED)) &&
	    (ch->in_room != vict->in_room) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You can't seem to get that person to hear you.\r\n", ch);
		return;
	}

	if ((IS_SET(world[vict->in_room].room_flags, RM1_SOUNDPROOF)) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("That person can't hear you.\r\n", ch);
		return;
	}

	sprintf(buf, "$n whispers to you, '%s'", message);
	ansi_act(buf, FALSE, ch, 0, vict, TO_VICT, CLR_WHISPER, 0);
	ansi_act("$n whispers something to $N.",
		 FALSE, ch, 0, vict, TO_NOTVICT, CLR_WHISPER, 0);
	sprintf(buf, "You whisper to %s, '%s'",
		GET_REAL_NAME(vict), message);
	ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_WHISPER, 0);
}

void do_ask(struct char_data * ch, char *argument, int cmd)
{
	struct char_data *vict;
	char name[MAX_INPUT_LENGTH];
	char message[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH + 128];
	char arg[MAX_INPUT_LENGTH + 128];

	MUZZLECHECK();

	bzero(arg, sizeof(arg));
	strcpy(arg, argument);
	argument = arg;
	REMOVE_COLOR(argument);
	half_chop(argument, name, message);

	if (!*name || !*message) {
		send_to_char("Who do you want to ask something.. and what??\r\n", ch);
		return;
	}
	else if (!(vict = ha2125_get_char_in_room_vis(ch, name))) {
		send_to_char("No-one by that name here..\r\n", ch);
		return;
	}
	else if (vict == ch) {
		act("$n quietly asks $mself a question.", FALSE, ch, 0, 0, TO_ROOM);
		send_to_char("You think about it for a while...\r\n", ch);
		return;
	}
	sprintf(buf, "$n asks you '%s'", message);
	ansi_act(buf, FALSE, ch, 0, vict, TO_VICT, CLR_COMM, 0);
	ansi_act("$n asks $N a question.", FALSE, ch, 0, vict, TO_NOTVICT, CLR_COMM, 0);
	sprintf(buf, "You ask %s, '%s'", GET_REAL_NAME(vict), message);
	ansi_act(buf, FALSE, ch, 0, 0, TO_CHAR, CLR_ASK, 0);
}

void do_write(struct char_data * ch, char *argument, int cmd)
{
	struct obj_data *paper = 0, *pen = 0;
	char papername[MAX_INPUT_LENGTH];
	char penname[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument_interpreter(argument, papername, penname);

	if (!ch->desc)
		return;

	if (!*papername) {	/* nothing was delivered */
		send_to_char(
			     "Write? On what? With what? what are you trying to do??\r\n", ch);
		return;
	}
	if (*penname) {		/* there were two arguments */
		if (!(paper = ha2075_get_obj_list_vis(ch, papername, ch->carrying)))
			if (!(paper = ha2075_get_obj_list_vis(ch, papername, world[ch->in_room].contents))) {
				sprintf(buf, "You have no %s.\r\n", papername);
				send_to_char(buf, ch);
				return;
			}
		if (!(pen = ha2075_get_obj_list_vis(ch, penname, ch->carrying))) {
			sprintf(buf, "You have no %s.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
	}
	else {			/* there was one arg.let's see what we can find */
		if (!(paper = ha2075_get_obj_list_vis(ch, papername, ch->carrying))) {
			sprintf(buf, "There is no %s in your inventory.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (paper->obj_flags.type_flag == ITEM_PEN) {	/* oops, a pen.. */
			pen = paper;
			paper = 0;
		}
		else if (paper->obj_flags.type_flag != ITEM_NOTE) {
			send_to_char("That thing has nothing to do with writing.\r\n", ch);
			return;
		}

		/* one object was found. Now for the other one. */
		if (!ch->equipment[HOLD]) {
			sprintf(buf, "You can't write with a %s alone.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (!CAN_SEE_OBJ(ch, ch->equipment[HOLD])) {
			send_to_char("The stuff in your hand is invisible! Yeech!!\r\n", ch);
			return;
		}

		if (pen)
			paper = ch->equipment[HOLD];
		else
			pen = ch->equipment[HOLD];
	}

	/* ok.. now let's see what kind of stuff we've found */
	if (pen->obj_flags.type_flag != ITEM_PEN) {
		act("$p is no good for writing with.", FALSE, ch, pen, 0, TO_CHAR);
	}
	else if (paper->obj_flags.type_flag != ITEM_NOTE) {
		act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);
	}
	else if (paper->action_description)
		send_to_char("There's something written on it already.\r\n", ch);
	else {
		/* we can write - hooray! */

		send_to_char("Ok.. go ahead and write.. end the note with a @.\r\n",
			     ch);
		act("$n begins to jot down a note.", TRUE, ch, 0, 0, TO_ROOM);
		ch->desc->str = &paper->action_description;
		ch->desc->max_str = MAX_NOTE_LENGTH;
	}

}				/* END OF do_write() */


void do_gossip(struct char_data * ch, char *argument, int cmd)
{

	char buf2[MAX_STRING_LENGTH];
	char buf1[MAX_INPUT_LENGTH + 128];
	char lv_this_gossip[80];
	struct descriptor_data *i;

	for (; *argument == ' '; argument++);
	if (!(*argument)) {
		show_history_to_char(CHANNEL_GOSSIP, ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "You gossip: Jail feels like home.  Bad food, anal penetration... Jail me longer!");
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_GOSSIP, AA_IGNORE_SLEEP);
		return;
	}

	MUZZLECHECK();

	/* IS GOSSIP/SHOUT BLOCKED? */
	if (IS_PC(ch) &&
	    (GET_LEVEL(ch) < IMO_IMP &&
	     IS_SET(GET_ACT2(ch), PLR2_MUZZLE_SHOUT))) {
		send_to_char("You are MUZZLED and prevented from using shout/gossip/auction!!!\r\n", ch);
		send_to_char("Use SCORE command to see when it expires.\r\n", ch);
		return;
	}

	/* IF WE ARE A NPC, AND OUR MASTER IS BLOCKED, WE ARE BLOCKED */
	if (IS_NPC(ch) &&
	    IS_AFFECTED(ch, AFF_CHARM) &&
	    ch->master) {
		if (IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_SHOUT) ||
		    IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_ALL)) {
			act("$n tells you that you are muzzled.",
			    FALSE, ch, 0, ch->master, TO_VICT);
			act("$n tells $N that $E is muzzled.",
			    FALSE, ch, 0, ch->master, TO_NOTVICT);
			return;
		}		/* END OF a muzzled master */
	}			/* END OF a charmed mob */

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOGOSSIP))) {
			send_to_char("And you have GOSSIP turned off!\r\n", ch);
		}
	}
	else {
		if ((IS_PC(ch) && IS_SET(GET_ACT3(ch), PLR3_NOGOSSIP))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOGOSSIP);
			send_to_char("Turning on GOSSIP channel.\r\n", ch);
		}
	}

	/* IS THIS A DUPLICATE OF THE LAST GOSSIP? */
	bzero(lv_this_gossip, sizeof(lv_this_gossip));
	sprintf(lv_this_gossip, "%s", GET_REAL_NAME(ch));
	strncat(lv_this_gossip, argument,
		sizeof(lv_this_gossip) - strlen(lv_this_gossip) - 5);
	if (!(strcmp(lv_last_gos_shout, lv_this_gossip))) {
		send_to_char("They've heard that one before.\r\n", ch);
		return;
	}

	if (GET_INCOGNITO(ch) > 1 || GET_VISIBLE(ch) > 1) {
		GET_INCOGNITO(ch) = 0;
		GET_VISIBLE(ch) = 0;
		co2900_send_to_char(ch, "You are now visible!\r\n");
	}


	sprintf(buf1, "Gossip: [$n] %s", argument);
	add_to_history(CHANNEL_GOSSIP, "Gossip: [%s] %s", GET_NAME(ch), argument);

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOGOSSIP)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				if (!is_ignored(i->character, GET_REAL_NAME(ch)) || (GET_LEVEL(ch) > PK_LEV)) {
					sprintf(buf1, "Gossip: [%s] %s", GET_REAL_NAME(ch), argument);
					if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "Gossip: [%s] %s", GET_REAL_NAME(ch), argument);
					}
					if ((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) &&
					    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "Gossip: [Someone] %s", argument);
					}
					ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_GOSSIP, AA_IGNORE_SLEEP);
				}

			}
		}

	sprintf(buf1, "You gossip: %s", argument);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_GOSSIP, AA_IGNORE_SLEEP);

	strcpy(buf2, argument);
	REMOVE_COLOR(buf2);
	if ((IS_NPC(ch) || GET_LEVEL(ch) < 10) &&
	    ha1000_is_there_a_reserved_word(buf2, curse_gossip, BIT0)) {
		send_to_char("You shouldn't have said that!!!\r\n", ch);
		bzero(buf1, sizeof(buf1));
		if (IS_NPC(ch)) {
			do_mob_master_muzzle(ch, buf2, cmd);
		}
		else {
			sprintf(buf1, "%s gossiped with inappropriate language and has been muzzled.\r\n",
				GET_REAL_NAME(ch));
			SET_BIT(GET_ACT2(ch), PLR2_MUZZLE_SHOUT);
			ch->specials.time_of_muzzle = time(0);
			send_to_char("You need to think about others!  You are muzzled.\r\n", ch);
			do_info(buf1, 1, MAX_LEV, ch);
		}
	}

	bzero(lv_last_gos_shout, sizeof(lv_last_gos_shout));
	sprintf(lv_last_gos_shout, "%s", GET_REAL_NAME(ch));
	strncat(lv_last_gos_shout, argument,
		sizeof(lv_last_gos_shout) - strlen(lv_last_gos_shout) - 5);

	return;

}				/* END OF do_gossip() */

void do_invisible(struct char_data * ch, char *arg, int cmd)
{

	sbyte new_invis;
	char buf[MAX_STRING_LENGTH];

	/* REMOVE LEADING SPACES */
	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		bzero(buf, sizeof(buf));
		if (GET_VISIBLE(ch) >= IMO_IMP)
			sprintf(buf, "Your are IMP invis. (lvl %d)\r\n", GET_VISIBLE(ch));
		else if (GET_VISIBLE(ch) == IMO_IMP)
			sprintf(buf, "Your are invisible to immortals. (lvl %d)\r\n", GET_VISIBLE(ch));
		else if (GET_VISIBLE(ch) > PK_LEV)
			sprintf(buf, "Your are invisible to mortals. (lvl %d)\r\n", GET_VISIBLE(ch));
		else
			sprintf(buf, "You are visibile to all players.(lvl %d)\r\n", GET_VISIBLE(ch));

		send_to_char(buf, ch);
		return;
	}


	if (!is_number(arg)) {
		send_to_char("Only numeric arguments are allowed.\r\n", ch);
		return;
	}
	new_invis = atoi(arg);
	if (new_invis == 0) {
		GET_VISIBLE(ch) = 0;
		return;
	}
	if (GET_LEVEL(ch) < IMO_IMM) {
		sprintf(buf, "Sorry, but you have to be at least level %d to perform this command.\r\n", IMO_IMM);
		send_to_char(buf, ch);
		return;
	}
	else if (GET_LEVEL(ch) == IMO_IMM) {
		if (new_invis == IMO_IMM) {
			GET_VISIBLE(ch) = new_invis;
			GET_INCOGNITO(ch) = 0;
			GET_GHOST(ch) = 0;
			return;
		}
		else {
			sprintf(buf, "Sorry, you only have these invisiblity options:\r\n\n0     %d\r\n", IMO_IMM);
			send_to_char(buf, ch);
			return;
		}
	}
	else if (GET_LEVEL(ch) == IMO_IMP) {
		if ((new_invis >= 0) && (new_invis <= IMO_IMP)) {
			GET_VISIBLE(ch) = new_invis;
			GET_INCOGNITO(ch) = 0;
			GET_GHOST(ch) = 0;
			return;
		}
		else {
			sprintf(buf, "Must be a number between 0 and %d.\r\n", IMO_IMP);
			send_to_char(buf, ch);
			return;
		}
	}
	else if (GET_LEVEL(ch) == IMO_IMP) {
		if ((new_invis >= 0) && (new_invis <= IMO_IMP)) {
			GET_VISIBLE(ch) = new_invis;
			GET_INCOGNITO(ch) = 0;
			GET_GHOST(ch) = 0;
			return;
		}
		else {
			sprintf(buf, "Must be a number between 0 and %d.\r\n", IMO_IMP);
			send_to_char(buf, ch);
			return;
		}
	}
}
//Incognito & ghost Shalira 22.07 .01
void do_incognito(struct char_data * ch, char *arg, int cmd)
{

	sbyte new_incognito;
	char buf[MAX_STRING_LENGTH];

	/* REMOVE LEADING SPACES */
	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		bzero(buf, sizeof(buf));
		if (GET_INCOGNITO(ch))
			sprintf(buf, "Your incognito level is set to %d.\r\n",
				GET_INCOGNITO(ch));
		else
			sprintf(buf, "You are not incognito.\r\n");

		send_to_char(buf, ch);
		return;
	}

	if (!is_number(arg)) {
		send_to_char("Only numeric arguments are allowed.\r\n", ch);
		return;
	}

	new_incognito = atoi(arg);

	if (new_incognito < 0) {
		send_to_char("I think zero will be sufficient.\r\n", ch);
		return;
	}

	if (new_incognito > 0 && GET_LEVEL(ch) < IMO_IMM) {
		bzero(buf, sizeof(buf));
		send_to_char("You can't set incognito level higher than 0.\r\n",
			     ch);
		return;
	}

	if (new_incognito > GET_LEVEL(ch) && GET_LEVEL(ch) < IMO_IMP) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You can't set incognito level higher than %d.\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}


	if (new_incognito > 50) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You can't set incognito level higher than 50.\r\n");
		return;
	}


	GET_INCOGNITO(ch) = new_incognito;
	bzero(buf, sizeof(buf));
	if (GET_INCOGNITO(ch) == 0) {
		sprintf(buf, "You are no longer incognito.\r\n");
	}
	else {
		sprintf(buf, "You are incognito to players under level %d.\r\n",
			GET_INCOGNITO(ch));
		GET_GHOST(ch) = 0;
		GET_VISIBLE(ch) = 0;
	}
	send_to_char(buf, ch);
	return;

}
//Incognito & ghost Shalira 22.07 .01
void do_ghost(struct char_data * ch, char *arg, int cmd)
{

	sbyte new_ghost;
	char buf[MAX_STRING_LENGTH];

	/* REMOVE LEADING SPACES */
	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		bzero(buf, sizeof(buf));
		if (GET_GHOST(ch))
			sprintf(buf, "Your ghost level is set to %d.\r\n",
				GET_GHOST(ch));
		else
			sprintf(buf, "You are not a ghost.\r\n");

		send_to_char(buf, ch);
		return;
	}

	if (!is_number(arg)) {
		send_to_char("Only numeric arguments are allowed.\r\n", ch);
		return;
	}

	new_ghost = atoi(arg);

	if (new_ghost < 0) {
		send_to_char("I think zero will be sufficient.\r\n", ch);
		return;
	}

	if (new_ghost > 0 && GET_LEVEL(ch) < IMO_IMM) {
		bzero(buf, sizeof(buf));
		send_to_char("You can't set ghost level higher than 0.\r\n",
			     ch);
		return;
	}

	if (new_ghost > GET_LEVEL(ch) && GET_LEVEL(ch) < IMO_IMP) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You can't set ghost level higher than %d.\r\n",
			GET_LEVEL(ch));
		send_to_char(buf, ch);
		return;
	}


	if (new_ghost > 50) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You can't set ghost level higher than 50.\r\n");
		return;
	}


	GET_GHOST(ch) = new_ghost;
	bzero(buf, sizeof(buf));
	if (GET_GHOST(ch) == 0) {
		sprintf(buf, "You are no longer a ghost.\r\n");
	}
	else {
		sprintf(buf, "You are a ghost to players under level %d.\r\n",
			GET_GHOST(ch));
		GET_VISIBLE(ch) = 0;
		GET_INCOGNITO(ch) = 0;
	}
	send_to_char(buf, ch);
	return;

}

void do_auction(struct char_data * ch, char *argument, int cmd)
{
	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	/* ***************************************** */
	/* This auction is not being used anymore.  */
	/* ***************************************** */

	for (; *argument == ' '; argument++);
	if (!(*argument)) {
		send_to_char("Auction? Yes! Fine! Auction we must, but WHAT??\r\n", ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "You auction: %s", argument);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_AUCTION, 0);
		return;
	}

	MUZZLECHECK();

	if (IS_PC(ch) &&
	    (GET_LEVEL(ch) < IMO_IMP &&
	     IS_SET(GET_ACT2(ch), PLR2_MUZZLE_SHOUT))) {
		send_to_char("You are MUZZLED and prevented from using shout/gossip/auction!!!\r\n", ch);
		send_to_char("Use SCORE command to see when it expires.\r\n", ch);
		return;
	}

	/* IF WE ARE A NPC, AND OUR MASTER IS BLOCKED, WE ARE BLOCKED */
	if (IS_NPC(ch) &&
	    IS_AFFECTED(ch, AFF_CHARM) &&
	    ch->master) {
		if (IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_SHOUT) ||
		    IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_ALL)) {
			act("$n tells you that you are muzzled.",
			    FALSE, ch, 0, ch->master, TO_VICT);
			act("$n tells $N that $E is muzzled.",
			    FALSE, ch, 0, ch->master, TO_NOTVICT);
			return;
		}		/* END OF a muzzled master */
	}			/* END OF a charmed mob */

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOAUCTION))) {
			send_to_char("And you have AUCTION turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOAUCTION))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOAUCTION);
			send_to_char("Turning on AUCTION channel.\r\n", ch);
		}
	}

	sprintf(buf1, "Auction: [$n] %s", argument);

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOAUCTION)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character,
					 TO_VICT, CLR_AUCTION, 0);
			}
		}

	sprintf(buf1, "You auction: %s", argument);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_AUCTION, 0);

	/* DID USER CURSE? */
	if ((IS_NPC(ch) ||
	     GET_LEVEL(ch) < IMO_IMP) &&
	    ha1050_is_there_a_curse_word(argument)) {
		send_to_char("You shouldn't have said that!!!\r\n", ch);
		bzero(buf1, sizeof(buf1));
		if (IS_NPC(ch)) {
			do_mob_master_muzzle(ch, argument, cmd);
		}
		else {
			sprintf(buf1, "%s auctioned with inappropriate language and has been muzzled.\r\n",
				GET_REAL_NAME(ch));
			SET_BIT(GET_ACT2(ch), PLR2_MUZZLE_SHOUT);
			ch->specials.time_of_muzzle = time(0);
			send_to_char("You need to think about others!  You are muzzled.\r\n", ch);
			do_info(buf1, 1, MAX_LEV, ch);
		}
	}

}				/* END OF do_auction() */


void do_immtalk(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	MUZZLECHECK();
	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_IMMTALK, ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&cYou &Cimmtalk&c:&C %s&E", arg);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_IMMTALK, 0);
		return;
	}

	if (GET_INCOGNITO(ch) > IMO_IMM || GET_VISIBLE(ch) > IMO_IMM) {
		(GET_INCOGNITO(ch) == 0) ? (GET_VISIBLE(ch) = IMO_IMM) : (GET_INCOGNITO(ch) = IMO_IMM);
		co2900_send_to_char(ch, "You are now visible to imms!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&CIMM: &c[&C$n&c] &C%s&E", arg);
	add_to_history(CHANNEL_IMMTALK, "&CIMM: &c[&C%s&c] &C%s&E", GET_NAME(ch), arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOIMM))) {
			send_to_char("And you have IMM turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOIMM))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOIMM);
			send_to_char("Turning on IMM channel.\r\n", ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    GET_LEVEL(i->character) >= IMO_IMM &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOIMM)) {
			if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost shalira 22.07, 01
					sprintf(buf1, "&CIMM: &c[&C%s&C] &C%s&E", GET_REAL_NAME(ch), arg);
			}
			if ((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) &&
			    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost Shalira 22.07 .01
					sprintf(buf1, "&CIMM: &c[&CSomeone&c] &C%s&E", arg);
			}
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_IMMTALK, 0);
			}
		}

	sprintf(buf1, "&cYou &Cimmtalk&c:&C %s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_IMMTALK, 0);

}				/* END OF do_immtalk() */

void do_imp(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	MUZZLECHECK();

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_BOSS, ch);
		return;
	}
	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&wBOSS: &K*&w$n&K* &w%s&E", arg);
	add_to_history(CHANNEL_BOSS, "&wBOSS: &K*&w%s&K* &w%s&E", GET_NAME(ch), arg);


	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    IS_PC(i->character) &&
		    GET_LEVEL(i->character) >= IMO_IMP) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, GRAY, 0);
			}
		}

	sprintf(buf1, "&KYou &wBOSS&K:&w %s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, GRAY, 0);

	return;

}				/* END OF do_imp */


void do_over(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	MUZZLECHECK();

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_STAFF, ch);
		return;
	}

	if (GET_INCOGNITO(ch) > IMO_IMM || GET_VISIBLE(ch) > IMO_IMM) {
		(GET_INCOGNITO(ch) == 0) ? (GET_VISIBLE(ch) = IMO_IMM) : (GET_INCOGNITO(ch) == IMO_IMM);
		co2900_send_to_char(ch, "You are now visible to IMMS's!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&RSTAFF: &r[&R$n&r]&R %s&E", arg);
	add_to_history(CHANNEL_STAFF, "&RSTAFF: &r[&R%s&r]&R %s&E", GET_NAME(ch), arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR2_NOSTAFF))) {
			send_to_char("And you have STAFF turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR2_NOSTAFF))) {
			REMOVE_BIT(GET_ACT3(ch), PLR2_NOSTAFF);
			send_to_char("Turning on STAFF channel.\r\n", ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    IS_PC(i->character) &&
		    GET_LEVEL(i->character) >= IMO_IMM) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, RED, 0);
			}
		}

	sprintf(buf1, "&rYou &RSTAFF&r: &R%s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, RED, 0);

	return;

}				/* END OF do_over */







void do_high(struct char_data * ch, char *arg, int cmd)
{
	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	MUZZLECHECK();

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	/* IF THIS ISN 'T A GOD, DO THE SOCIAL */
	if (GET_LEVEL(ch) < IMO_IMM || IS_NPC(ch)) {
		do_action(ch, arg, CMD_HIGH5);
		return;
	}

	if (!(*arg)) {
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON 'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&GYou &gHIGH&G:&g %s&E", arg);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_HIGH, 0);
		return;
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&gHIGH: &G[&g$n&G]&g %s&E", arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch && !i->connected && IS_PC(i->character) &&
		    GET_LEVEL(i->character) >= IMO_IMM) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_HIGH, 0);
			}
		}

	sprintf(buf1, "You HIGH: %s", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_HIGH, 0);

	return;

}				/* END OF do_high */


void do_ante(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;

	MUZZLECHECK();

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		send_to_char("What do you want to say?\r\n", ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		sprintf(buf1, "&gYou &GANTE&g: &G%s&E", arg);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_ANTE, 0);
		return;
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&GANTE: &g[&G$n&g]&G %s&E", arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch && !i->connected && IS_PC(ch) &&
		    GET_LEVEL(i->character) >= IMO_IMM) {
			if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost shalira 22.07, 01
					sprintf(buf1, "ANTE: [%s] %s", GET_REAL_NAME(ch), arg);
			}
			if ((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) &&
			    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost Shalira 22.07 .01
					sprintf(buf1, "ANTE: [Someone] %s", arg);
			}
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_ANTE, 0);
			}

		}
	sprintf(buf1, "You ANTE: %s", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_ANTE, 0);

	return;


}				/* END OF do_ante() */

void do_spirittalk(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;


	MUZZLECHECK();

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_SPIRIT, ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&GYou SPIRIT: Hey, I'm a retard!  Please jail me for longer!&E");
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_IMMTALK, 0);
		return;
	}

	if (GET_INCOGNITO(ch) > IMO_SPIRIT || GET_VISIBLE(ch) > IMO_SPIRIT) {
		(GET_INCOGNITO(ch) == 0) ? (GET_VISIBLE(ch) = IMO_SPIRIT) : (GET_INCOGNITO(ch) = IMO_SPIRIT);
		co2900_send_to_char(ch, "You are now visible to spirits and higher!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&GSPIRIT: &g[$n] &G%s&E", arg);
	add_to_history(CHANNEL_SPIRIT, "&GSPIRIT: &g[%s] &G%s&E", GET_NAME(ch), arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT4(ch), PLR4_NOSPIRIT))) {
			send_to_char("And you have Spirit turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT4(ch), PLR4_NOSPIRIT))) {
			REMOVE_BIT(GET_ACT4(ch), PLR4_NOSPIRIT);
			send_to_char("Turning on Spirit channel.\r\n", ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    GET_LEVEL(i->character) >= IMO_SPIRIT &&
		    !IS_SET(GET_ACT4(i->character), PLR4_NOSPIRIT)) {
			if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost shalira 22.07, 01
					sprintf(buf1, "&GSPIRIT: &g[%s] &G%s&E", GET_REAL_NAME(ch), arg);
			}
			if (((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) ||
			     GET_VISIBLE(ch) > GET_LEVEL(i->character)) &&
			    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
				//Incognito & ghost Shalira 22.07 .01
					sprintf(buf1, "&GSPIRIT: &g[Someone] &G%s&E", arg);
			}
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, LGREEN, 0);
			}
		}

	sprintf(buf1, "&GYou SPIRIT: %s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, LGREEN, 0);

}				/* END OF do_spirittalk() */


void do_avatar(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;
	char argument[MAX_INPUT_LENGTH + 128];
\
	MUZZLECHECK();

	bzero(argument, sizeof(argument));
	strcpy(argument, arg);
	arg = argument;
	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_AVATAR, ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&YYou &yAVAT&Y:&y My idiocy knows no bounds!  Lay another week of jail on me!&E");
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_AVAT, 0);
		return;
	}

	if (GET_INCOGNITO(ch) > IMO_LEV || GET_VISIBLE(ch) > IMO_LEV) {
		(GET_INCOGNITO(ch) == 0) ? (GET_VISIBLE(ch) = IMO_LEV) : (GET_INCOGNITO(ch) = IMO_LEV);
		co2900_send_to_char(ch, "You are now visible to avatars!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&yAVAT: &Y[&y$n&Y]&y %s&E", arg);
	add_to_history(CHANNEL_AVATAR, "&yAVAT: &Y[&y%s&y]&y %s&E", GET_NAME(ch), arg);


	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOAVATAR))) {
			send_to_char("And you have AVATAR turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOAVATAR))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOAVATAR);
			send_to_char("Turning on AVATAR channel.\r\n", ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    GET_LEVEL(i->character) >= IMO_LEV &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOAVATAR)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				if (!is_ignored(i->character, GET_REAL_NAME(ch)) || (GET_LEVEL(ch) > IMO_LEV)) {
					sprintf(buf1, "&yAVAT: &Y[&y%s&Y]&y %s&E", GET_REAL_NAME(ch), arg);
					if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&yAVAT: &Y[&y%s&Y]&y %s&E", GET_REAL_NAME(ch), arg);
					}
					if (((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) ||
					     (GET_VISIBLE(ch) > GET_LEVEL(i->character))) &&
					    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&yAVAT: &Y[&ySomeone&Y]&y %s&E", arg);
					}
					ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_AVAT, 0);
				}
			}
		}

	sprintf(buf1, "&YYou &yAVAT&Y:&y %s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_AVAT, 0);

	return;

}				/* END OF do_avatar() */

void do_music(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	char buf2[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;
	char argument[MAX_INPUT_LENGTH + 128];
	int idx;

	MUZZLECHECK();

	bzero(argument, sizeof(argument));
	strcpy(argument, arg);
	arg = argument;
	for (; *arg == ' '; arg++);

	if (!(*arg)) {
		show_history_to_char(CHANNEL_MUSIC, ch);
		return;
	}

	if (ha1000_is_there_a_reserved_word(arg, curse_gossip, BIT0)) {
		for (idx = 0; idx < strlen(arg); idx++)
			arg[idx] = LOWER(arg[idx]);
	}

	if (GET_LEVEL(ch) < 10) {
		send_to_char("You have to be level 10 or higher to use the music channel.\n\r", ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&CYou &bmusic&C:&B %s&E", arg);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_MUSIC, 0);
		return;
	}

	/* IS GOSSIP/SHOUT BLOCKED? */
	if (IS_PC(ch) &&
	    (GET_LEVEL(ch) < IMO_IMP &&
	     IS_SET(GET_ACT2(ch), PLR2_MUZZLE_SHOUT))) {
		send_to_char("You are MUZZLED and prevented from using shout/gossip/auction/music!!!\r\n", ch);
		send_to_char("Use SCORE command to see when it expires.\r\n", ch);
		return;
	}

	/* IF WE ARE A NPC, AND OUR MASTER IS BLOCKED, WE ARE BLOCKED */
	if (IS_NPC(ch) &&
	    IS_AFFECTED(ch, AFF_CHARM) &&
	    ch->master) {
		if (IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_SHOUT) ||
		    IS_SET(GET_ACT2(ch->master), PLR2_MUZZLE_ALL)) {
			act("$n tells you that you are muzzled.", FALSE, ch, 0, ch->master, TO_VICT);
			act("$n tells $N that $E is muzzled.", FALSE, ch, 0, ch->master, TO_NOTVICT);
			return;
		}		/* END OF a muzzled master */
	}			/* END OF a charmed mob */

	if (GET_INCOGNITO(ch) > 0 || GET_VISIBLE(ch) > 0) {
		GET_INCOGNITO(ch) = 0;
		GET_VISIBLE(ch) = 0;
		co2900_send_to_char(ch, "You are now visible!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&B*&CMusic&B* &C[&B$n&C]&B %s&E", arg);
	add_to_history(CHANNEL_MUSIC, "&B*&CMusic&B* &C[&B%s&C]&B %s&E", GET_NAME(ch), arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOMUSIC))) {
			send_to_char("And you have MUSIC turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOMUSIC))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOMUSIC);
			send_to_char("Turning on MUSIC channel.\r\n", ch);
		}
	}


	if ((IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) &&
	    ha1000_is_there_a_reserved_word(buf2, curse_gossip, BIT0)) {
		send_to_char("You shouldn't have said that!!!\r\n", ch);
		bzero(buf1, sizeof(buf1));
		if (IS_NPC(ch)) {
			do_mob_master_muzzle(ch, buf2, cmd);
		}
		else {
			sprintf(buf1, "%s used music with inappropriate language and has been muzzled.\r\n", GET_REAL_NAME(ch));
			SET_BIT(GET_ACT2(ch), PLR2_MUZZLE_SHOUT);
			ch->specials.time_of_muzzle = time(0);
			send_to_char("You need to think about others!  You are muzzled.\r\n", ch);
			do_info(buf1, 1, MAX_LEV, ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOMUSIC)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				if (!is_ignored(i->character, GET_REAL_NAME(ch)) || (GET_LEVEL(ch) > PK_LEV)) {
					sprintf(buf1, "&B*&CMusic&B* &C[&B%s&C]&B %s&E", GET_REAL_NAME(ch), arg);
					if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&B*&CMusic&B* &C[&B%s&C]&B %s&E", GET_REAL_NAME(ch), arg);
					}
					if ((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) &&
					    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&B*&CMusic&B* &C[&BSomeone&C]&B %s&E", arg);
					}
					ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, CLR_MUSIC, 0);
				}
			}
		}

	sprintf(buf1, "&CYou &Bmusic&C:&B %s&E", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, CLR_MUSIC, 0);

	return;

}				/* END OF do_music() */

void do_pkflame(struct char_data * ch, char *arg, int cmd)
{

	char buf1[MAX_INPUT_LENGTH + 128];
	struct descriptor_data *i;
	char argument[MAX_INPUT_LENGTH + 128];

	bzero(argument, sizeof(argument));
	strcpy(argument, arg);
	arg = argument;
	for (; *arg == ' '; arg++);

	if (GET_LEVEL(ch) < PK_LEV &&
	    !IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)) {
		send_to_char("You must be a pkiller to use this channel!\n\r", ch);
		return;
	}

	if (!(*arg)) {
		show_history_to_char(CHANNEL_PKFLAME, ch);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		/* THEY DON'T NEED TO KNOW IT WAS LIMITED!!! */
		sprintf(buf1, "&RYou &KPK&YF&RL&rA&RM&YE&R: %s", arg);
		ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, 0, 0);
		return;
	}

	if (GET_INCOGNITO(ch) > 0 || GET_VISIBLE(ch) > 0) {
		GET_INCOGNITO(ch) = 0;
		GET_VISIBLE(ch) = 0;
		co2900_send_to_char(ch, "You are now visible!\r\n");
	}

	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "&KPK&YF&RL&rA&RM&YE&R: &K*&R$n&K* &R%s", arg);
	add_to_history(CHANNEL_PKFLAME, "&KPK&YF&RL&rA&RM&YE&R: &K*&R%s&K* &R%s", GET_NAME(ch), arg);

	if ((IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		send_to_char("This is a soundproof room so you won't hear replies.\r\n", ch);
		if ((IS_SET(GET_ACT3(ch), PLR3_NOPKFLAME))) {
			send_to_char("And you have &KPK&YF&RL&rA&RM&YE&n turned off!\r\n", ch);
		}
	}
	else {
		if (IS_PC(ch) && (IS_SET(GET_ACT3(ch), PLR3_NOPKFLAME))) {
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOPKFLAME);
			send_to_char("Turning on &KPK&YF&RL&rA&RM&YE&n channel.\r\n", ch);
		}
	}

	for (i = descriptor_list; i; i = i->next)
		if (i->character != ch &&
		    !i->connected &&
		    (GET_LEVEL(i->character) >= PK_LEV ||
		     IS_SET(GET_ACT2(i->character), PLR2_PKILLABLE)) &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOPKFLAME)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				if (!is_ignored(i->character, GET_REAL_NAME(ch)) || (GET_LEVEL(ch) > PK_LEV)) {
					sprintf(buf1, "&KPK&YF&RL&rA&RM&YE&R: &K*&R%s&K* &R%s", GET_REAL_NAME(ch), arg);
					if (GET_GHOST(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&KPK&YF&RL&rA&RM&YE&R: &K*&R%s&K* &R%s", GET_REAL_NAME(ch), arg);
					}
					if (((GET_INCOGNITO(ch) > GET_LEVEL(i->character)) ||
					     (GET_VISIBLE(ch) > GET_LEVEL(i->character))) &&
					    GET_LEVEL(ch) > GET_LEVEL(i->character)) {
						//Incognito & ghost Shalira 22.07 .01
							sprintf(buf1, "&KPK&YF&RL&rA&RM&YE&R: &K*&RSomeone&K* &R%s", arg);
					}
					ansi_act(buf1, 0, ch, 0, i->character, TO_VICT, LRED, 0);
				}
			}
		}

	sprintf(buf1, "&RYou &KPK&YF&RL&rA&RM&YE&R: %s", arg);
	ansi_act(buf1, 0, ch, 0, 0, TO_CHAR, LRED, 0);

	return;

}				/* END OF do_pkflame() */

void do_sys(char *arg, int min_level, struct char_data * ch)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];
	int adj_min_level, lv_ch_visible;

	if (min_level > IMO_IMP)
		adj_min_level = IMO_IMP;
	else
		adj_min_level = min_level;

	if (ch)
		lv_ch_visible = GET_VISIBLE(ch);
	else
		lv_ch_visible = IMO_IMM;

	bzero(buf, sizeof(buf));

	//Remove color from string
		REMOVE_COLOR(arg);

	strcpy(buf, "SYS: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));

	/* LOG MESSAGES TO LOG FILE */
	main_log(buf);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOSYSTEM)) {
				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (!IS_NPC(i->character)) {
						if (GET_LEVEL(i->character) >= lv_ch_visible) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_SYS, 0);
							}
						}
					}
				}
			}
		}
	}
}				/* END OF do_sys () */


void do_info(char *arg, int min_level, int max_level, struct char_data * ch)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];
	int adj_min_level, adj_max_level;

	if (max_level < 1)
		adj_max_level = 1;
	else
		adj_max_level = max_level;

	if (min_level > IMO_IMP)
		adj_min_level = IMO_IMP;
	else
		adj_min_level = min_level;

	bzero(buf, sizeof(buf));
	strcpy(buf, "INFO: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));

	/* LOG MESSAGES TO LOG FILE */
	main_log(buf);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOINFO)) {
				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (GET_LEVEL(i->character) <= adj_max_level) {
						if (!IS_NPC(i->character)) {
							if (GET_LEVEL(i->character) >= GET_VISIBLE(ch)) {
								if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
									ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_INFO, 0);
								}
							}
						}
					}
				}
			}
		}
	}
}				/* END OF do_info() */

void do_info_noch(char *arg, int min_level, int max_level)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];
	int adj_min_level, adj_max_level;

	if (max_level < 1)
		adj_max_level = 1;
	else
		adj_max_level = max_level;

	if (min_level > IMO_IMP)
		adj_min_level = IMO_IMP;
	else
		adj_min_level = min_level;

	bzero(buf, sizeof(buf));
	strcpy(buf, "INFO: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));

	/* LOG MESSAGES TO LOG FILE */
	main_log(buf);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOINFO)) {
				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (GET_LEVEL(i->character) <= adj_max_level) {
						if (!IS_NPC(i->character)) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_INFO, 0);
							}
						}
					}
				}
			}
		}
	}
}				/* END OF do_info_noch() */


void do_connect(char *arg, int min_level, struct char_data * ch)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];
	int adj_min_level;

	if (min_level > IMO_IMP)
		adj_min_level = IMO_IMP;
	else
		adj_min_level = MAXV(IMO_SPIRIT, min_level);

	bzero(buf, sizeof(buf));
	strcpy(buf, "CONNECT: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));


	/* LOG MESSAGES TO LOG FILE */
	main_log(buf);
	spec_log(buf, CONNECT_LOG);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOCONNECT)) {
				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (!IS_NPC(i->character)) {
						if (GET_LEVEL(i->character) >= GET_VISIBLE(ch)) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_CONNECT, 0);
							}
						}
					}
				}
			}
		}
	}
}				/* END OF do_connect () */

void do_connect_clan(char *arg, int min_level, struct char_data * ch)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int adj_min_level;

	if (min_level > IMO_IMP)
		adj_min_level = IMO_IMP;
	else
		adj_min_level = MAXV(IMO_SPIRIT, min_level);

	bzero(buf, sizeof(buf));
	strcpy(buf, "CONNECT: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));

	sprintf(buf2, "Clan: %s entering Crimson II", GET_REAL_NAME(ch));


	/* LOG MESSAGES TO LOG FILE */
	main_log(buf);
	spec_log(buf, CONNECT_LOG);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOCONNECT)) {

				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (!IS_NPC(i->character)) {
						if (GET_LEVEL(i->character) >= GET_VISIBLE(ch)) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_CONNECT, 0);
							}
						}
					}
				}
				if ((CLAN_NUMBER(i->character) == CLAN_NUMBER(ch)) &&
				(CLAN_RANK(i->character) > CLAN_APPLY_RANK)) {
					if (!IS_NPC(i->character)) {
						if (GET_LEVEL(i->character) >= GET_VISIBLE(ch)) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf2, 0, i->character, 0, 0, TO_CHAR, CLR_CONNECT, 0);
							}
						}
					}
				}
			}
		}
	}

}				/* END OF do_connect_clan () */


void do_wizinfo(char *arg, int min_level, struct char_data * ch)
{
	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];
	int adj_min_level;

	if (min_level < IMO_IMM)
		adj_min_level = IMO_IMM;
	else
		adj_min_level = min_level;


	bzero(buf, sizeof(buf));

	//Remove all the color from the string
		// REMOVE_COLOR(arg);

	strcpy(buf, "WIZINFO: ");
	strncat(buf, arg, (sizeof(buf) - strlen(buf) - 2));

	/* LOG MESSAGES TO LOG FILE */
	//if (GET_LEVEL(ch) <= MAX_LEV)
		main_log(buf);

	for (i = descriptor_list; i; i = i->next) {
		if (!i->connected) {
			if (!IS_SET(GET_ACT3(i->character), PLR3_NOWIZINFO)) {
				if (GET_LEVEL(i->character) >= adj_min_level) {
					if (!IS_NPC(i->character)) {
						if (GET_LEVEL(i->character) >= GET_VISIBLE(ch)) {
							if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
								ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_WIZINFO, 0);
							}
						}
					}
				}
			}
		}
	}

}				/* END OF do_wizinfo () */


void do_manual_info(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	for (; *arg == ' '; arg++);
	do_info_noch(arg, 0, 99);
	sprintf(buf, "%s did INFO: %s", GET_REAL_NAME(ch), arg);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
}
/* Allows a string to be replaced, great for stopping naughty words :P */
void search_replace(char *string, const char *find, const char *replace)
{
	char final[MAX_INPUT_LENGTH], temp[2];
	size_t start, end, i;

	while (strstr(string, find) != NULL) {

		final[0] = '\0';
		start = strstr(string, find) - string;
		end = start + strlen(find);

		temp[1] = '\0';

		strncat(final, string, start);

		strcat(final, replace);

		for (i = end; string[i] != '\0'; i++) {
			temp[0] = string[i];
			strcat(final, temp);
		}

		sprintf(string, final);

	}
	/* Added by Bingo 02-01-01, code by Andrew Ritchie */
}
