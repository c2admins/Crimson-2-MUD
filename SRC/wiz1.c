/* wi */
/* gv_location: 21001-22500 */
/* *********************************************************************
*  file: wiz1.c , Implementation of commands.       Part of DIKUMUD *
*  Usage : Wizard Commands.                                           *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inforon. *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "constants.h"
#include "rent.h"
#include "spells.h"
#include "globals.h"
#include "func.h"

extern FILE *help_fl;
extern int top_of_helpt;

int touch(const char *path);

#define GOTO_PENALTY .04

#define CAN_PERSON_STAT(zone) {                                       \
	if (!zone_table[zone].lord) {                                   \
		zone_table[zone].lord = strdup(" ");                      \
	}                                                               \
	if (GET_LEVEL(ch) < IMO_IMM &&                                 \
			IS_SET(zone_table[zone].flags, ZONE_NEW_ZONE) &&             \
			!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[zone].lord)) {      \
		send_to_char("You can't stat items in a new zone.", ch);    \
			return;                                                    \
	}                                                               \
}

void wi1000_do_emote(struct char_data * ch, char *arg, int cmd)
{

	int j, my_len, loc;
	char buf[MAX_STRING_LENGTH];
	char temp_str[MAX_STRING_LENGTH];
	char temp2[MAX_STRING_LENGTH];
	char argument[MAX_STRING_LENGTH];



	bzero(argument, sizeof(argument));
	strcpy(argument, arg);
	arg = argument;

	MUZZLECHECK();
	/* EMOTE REMOVED? */
	if (IS_SET(GET_ACT2(ch), PLR2_NO_EMOTE)) {
		send_to_char("Sorry, but your not allowed to do emotes!\r\n", ch);
		return;
	}

	if (IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF) &&
	    (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP)) {
		send_to_char("Shhhhh, this is a quite room!\r\n", ch);
		return;
	}

	/* FIND FIRST CHAR THATS NOT A SPACE */
	for (; *(arg) == ' '; arg++);

	if (!*(arg)) {
		send_to_char("Yes.. But what?\r\n", ch);
		return;
	}

	my_len = strlen(arg);
	loc = 0;
	for (j = 0; j < my_len; j++)
		if (strncmp((char *) (arg + j), "$n", 2) == 0) {
			loc = j;
			j = my_len;
		}
	if (loc == 0) {
		sprintf(buf, "$n %s", arg);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "You emote '%s %s&n'\r\n",
			GET_REAL_NAME(ch),
			arg);
		send_to_char(buf, ch);
	}
	else {
		bzero(temp_str, sizeof(temp_str));
		strcpy(temp_str, arg);
		/* ZERO OUT STRING */
		bzero(temp2, sizeof(temp2));
		strncpy(temp2, temp_str, loc - 1);
		strcat(temp2, "&n");
		strcat(temp2, GET_REAL_NAME(ch));
		strcat(temp2, &temp_str[loc + 2]);
		/* SHOW OTHERS YOUR EMOTE */
		act(temp2, FALSE, ch, 0, 0, TO_ROOM);
		/* LET ORIGINATOR KNOW WHAT THEY DID */
		sprintf(buf, "You emote:  '%s&n'\r\n", temp2);
		send_to_char(buf, ch);
	}			/* END OF else we have a $n in arguments */

}				/* END OF wi1000_do_emotes() */


void wi1100_do_echo(struct char_data * ch, char *arg, int cmd)
{
	int j;
	char buf[MAX_STRING_LENGTH];	/* room for you echo: too */
	//struct descriptor_data *i;



	if (IS_NPC(ch))
		return;

	for (j = 0; *(arg + j) == ' '; j++);

	if (!*(arg + j)) {
		send_to_char("What do you want to echo?\r\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s", arg + j);
	act(buf, 0, ch, 0, 0, TO_ROOM);
	act(buf, 0, ch, 0, 0, TO_CHAR);

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s echos %s.", GET_NAME(ch), arg + j);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);

}				/* END OF wi1100_do_echo() */


void wi1200_do_gecho(struct char_data * ch, char *arg, int cmd)
{

	int j;
	char buf[MAX_STRING_LENGTH];
	struct descriptor_data *i;



	if (IS_NPC(ch))
		return;

	for (j = 0; *(arg + j) == ' '; j++);

	if (!*(arg + j)) {
		send_to_char("What do you want to gecho?\r\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s", arg + j);

	for (i = descriptor_list; i; i = i->next)
		if (!i->connected)
			act(buf, 0, ch, 0, i->character, TO_VICT);

	act(buf, 0, ch, 0, ch, TO_CHAR);
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s gechos %s.", GET_NAME(ch), arg + j);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);

}				/* END OF do_global_echo() */


void wi1300_do_trans(struct char_data * ch, char *arg, int cmd)
{

	struct descriptor_data *i;
	struct char_data *victim;
	char buf[MAX_STRING_LENGTH];
	char buffer[MAX_STRING_LENGTH];
	sh_int target;
	sh_int ok_to_transfer;
	int zone, number_transferred;



	if (IS_NPC(ch))
		return;

	one_argument(arg, buf);

	if (gv_port == ZONING_PORT) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to use transfer on this port!\n\r", ch);
			return;
		}
	}

	if (!*buf)
		send_to_char("Who do you wish to transfer?\r\n", ch);
	else if (str_cmp("all", buf)) {
		if (!(victim = ha3100_get_char_vis(ch, buf)))
			send_to_char("No-one by that name around.\r\n", ch);
		else {
			/* DEFAULT IS THEY CAN DO IT */
			ok_to_transfer = 1;
			/* DON'T ALLOW TRANFER OF HIGHER LEVEL CHARS UNLESS IMP */
			if (IS_PC(victim) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				if (GET_LEVEL(victim) > GET_LEVEL(ch)) {
					ok_to_transfer = 0;
				}
				if (IS_SET(GET_ACT2(victim), PLR2_JAILED)) {
					ok_to_transfer = 0;
				}
			}
			if (IS_NPC(victim) && GET_LEVEL(ch) < IMO_IMP) {
				if (victim->in_room >= 0) {
					zone = world[victim->in_room].zone;
					if (world[victim->in_room].number == JAIL_ROOM ||
					    world[victim->in_room].number == PET_ROOM ||
					    IS_SET(zone_table[zone].flags, ZONE_TESTING) ||
					    IS_SET(zone_table[zone].flags, ZONE_NO_ENTER)) {
						ok_to_transfer = 0;
					}
				}
			}

			/* LEVEL RESTRICTION? */
			if (!ok_to_transfer) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "Sorry, you can't transfer %s.\r\n",
					GET_NAME(victim));
				send_to_char(buffer, ch);

				bzero(buffer, sizeof(buf));
				sprintf(buffer, "%s tries unsuccessfully to transfer you!\r\n",
					GET_NAME(ch));
				send_to_char(buffer, victim);
			}
			else {
				act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
				target = ch->in_room;

				ha1500_char_from_room(victim);
				ha1600_char_to_room(victim, target);
				act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
				act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
				in3000_do_look(victim, "", 0);
				send_to_char("Ok.\r\n", ch);
				sprintf(buf, "TRANSFER: %s has transfered %s to room[%d].", GET_REAL_NAME(ch), GET_REAL_NAME(victim), target);
				spec_log(buf, GOD_COMMAND_LOG);
			}	/* END OF ELSE */
		}
	}
	else {			/* Trans All */

		number_transferred = 0;

		/* ONLY Ante or higher */
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("One at a time please!\r\n", ch);
			return;
		}

		for (i = descriptor_list; i; i = i->next) {

			/* DEFAULT IS THEY CAN DO IT */
			ok_to_transfer = 1;

			/* DON'T TRANFER IMMORTALS */
			if (i->character) {

				if (GET_LEVEL(i->character) > PK_LEV)
					ok_to_transfer = 0;

				if (IS_SET(GET_ACT2(i->character), PLR2_JAILED))
					ok_to_transfer = 0;
			}

			if (!ok_to_transfer &&
			    ch != i->character &&
			    GET_LEVEL(ch) >= IMO_IMP) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "   Skipping: %s (LEVEL: %d).\r\n",
					GET_NAME(i->character), GET_LEVEL(i->character));
				send_to_char(buffer, ch);
			}

			if (i->character != ch &&
			    !i->connected &&
			    ok_to_transfer) {
				number_transferred++;
				victim = i->character;
				act("$n disappears in a mushroom cloud.",
				    TRUE, victim, 0, 0, TO_ROOM);
				target = ch->in_room;
				ha1500_char_from_room(victim);
				ha1600_char_to_room(victim, target);
				in3000_do_look(victim, "", 0);
				act("$n arrives from a puff of smoke.",
				    TRUE, victim, 0, 0, TO_ROOM);
				act("$n has transferred you!", TRUE, ch, 0, victim, TO_VICT);
			}

		}		/* END OF for */
		bzero(buffer, sizeof(buffer));
		if (number_transferred == 1) {
			send_to_char("You transferred 1 player.\r\n", ch);
		}
		if (number_transferred > 1) {
			sprintf(buffer, "You transferred %d players.\r\n",
				number_transferred);
			send_to_char(buffer, ch);
		}
		sprintf(buf, "TRANSFER: %s as tranfered all to r[%d].", GET_REAL_NAME(ch), target);
		spec_log(buf, GOD_COMMAND_LOG);
	}
}


void wi1400_do_at(struct char_data * ch, char *arg, int cmd)
{

	char command[MAX_STRING_LENGTH], loc_str[MAX_STRING_LENGTH], logbuf[MAX_STRING_LENGTH], atlook[MAX_STRING_LENGTH];
	int loc_nr, rc, location, original_loc, lv_was_riding;
	struct char_data *target_mob;
	struct obj_data *target_obj;



	if (IS_NPC(ch))
		return;

	half_chop(arg, loc_str, command);
	if (!*loc_str) {
		send_to_char("You must supply a room number or a name.\r\n", ch);
		return;
	}

	if (is_number(loc_str)) {
		loc_nr = atoi(loc_str);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\r\n", ch);
				return;
			}
	}
	else {
		if ((target_mob = ha3100_get_char_vis(ch, loc_str)))
			location = target_mob->in_room;
		else {
			if ((target_obj = ha3200_get_obj_vis(ch, loc_str)))
				if (target_obj->in_room != NOWHERE)
					location = target_obj->in_room;
				else {
					send_to_char("The object is not available.\r\n", ch);
					return;
				}
			else {
				send_to_char("No such creature or object around.\r\n", ch);
				return;
			}
		}
	}

	/* a location has been found. */

	if (gv_port == ZONING_PORT) {
		if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[location].zone].lord) &&
		    GET_LEVEL(ch) < IMO_IMM && world[location].number != 8312) {
			send_to_char("You are not authorized to go into that zone!\n\r", ch);
			return;
		}
	}

	/* CAN THE CHAR ACCESS THE ROOM? */
	rc = li3000_is_blocked_from_room(ch, location, 0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	/* SAME ROOM? */
	if ((ch->in_room == location) && (cmd != CMD_DOFOR)) {
		send_to_char("Your already there!\r\n", ch);
	}

	original_loc = ch->in_room;
	lv_was_riding = ch->riding;
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, location);

	/* Messy code to find the command of what they want to do, but not
	 * destroy the command */

	int fdx = 0, fdxcount = 0;
	for (;;) {
		if (isspace(command[fdx]) && fdxcount < 1) {
			fdx++;
			continue;
		}
		else if (isspace(command[fdx])) {
			break;
		}
		else {
			atlook[fdx] = command[fdx];
			fdx++;
			fdxcount++;
		}
	}


	if ((is_abbrev(atlook, "look") || is_abbrev(atlook, "peek")) && GET_LEVEL(ch) < 50) {
		sprintf(logbuf, "%s (ATLOOK) at %s (r%d) %s", GET_NAME(ch), loc_str,
			world[location].number, command);
		do_wizinfo(logbuf, IMO_IMP, ch);
		spec_log(logbuf, GOD_COMMAND_LOG);
	}
	command_interpreter(ch, command);

	/* check if the guy's still there */
	for (target_mob = world[location].people; target_mob; target_mob =
	     target_mob->next_in_room)
		if (ch == target_mob) {
			ha1500_char_from_room(ch);
			ha1600_char_to_room(ch, original_loc);
			ch->riding = lv_was_riding;
		}
}				/* END OF wi1400_do_at() */


void wi1500_do_goto(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int loc_nr, location, lv_is_obj, rc;
	struct char_data *target_mob;
	struct obj_data *target_obj;



	if (IS_NPC(ch) && ch != bounty_hunter)
		return;

	/* if (GET_LEVEL(ch) <= IMO_LEV) { send_to_char ("You are not godly
	 * enough to perform this action.\r\n", ch); return; } */
	one_argument(arg, buf);
	if (!*buf) {
		send_to_char("You must supply a room number or a name.\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT) {
		if (GET_HIT(ch) < (GOTO_PENALTY * GET_MAX_HIT(ch)) - 8) {
			send_to_char("It would kill you to do that!!\r\n", ch);
			return;
		}
		if ((GET_MOVE(ch) < GOTO_PENALTY * GET_MAX_MOVE(ch)) ||
		    (GET_MANA(ch) < (GOTO_PENALTY * GET_MAX_MANA(ch)) + 2)) {
			send_to_char("Whew! you're too tired for that kind of thing.\r\n", ch);
			return;
		}

		/* WE ARE FIGHTING */
		if (IS_AFFECTED(ch, AFF_RAGE)) {
			send_to_char("You are caught up in blood lust and wouldn't dream of leaving the carnage!\r\n", ch);
			return;
		}
		if (GET_POS(ch) == POSITION_SLEEPING) {
			send_to_char("How can you go anywhere while you are asleep?\r\n", ch);
			return;
		}
	}

	target_obj = 0;
	target_mob = 0;
	lv_is_obj = 0;
	if (is_number(buf)) {
		loc_nr = atoi(buf);
		for (location = 0; location <= top_of_world; location++) {
			if (world[location].number == loc_nr) {
				break;
			}
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\r\n", ch);
				return;
			}
		}
	}
	else {
		if ((target_mob = ha3100_get_char_vis_ex(ch, buf, TRUE)))
			location = target_mob->in_room;
		else {
			if ((target_obj = ha3200_get_obj_vis(ch, buf)))
				if (target_obj->in_room != NOWHERE) {
					location = target_obj->in_room;
					lv_is_obj = 1;
				}
				else {
					send_to_char("The object is not available.\r\n", ch);
					return;
				}
			else {
				send_to_char("No such creature or object around.\r\n", ch);
				return;
			}
		}
	}

	/* CAN THE CHAR ACCESS THE ROOM? */
	rc = li3000_is_blocked_from_room(ch, location, BIT0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	/* If it's their quest mob/obj they can't goto it */
	if (target_mob)
		if (IS_NPC(target_mob))
			if (target_mob->questmob) {
				send_to_char("A strange but powerful force is stopping you from going there.\r\n", ch);
				return;
			}
	if (lv_is_obj)
		if (target_obj)
			if (target_obj->in_room != NOWHERE)
				if (IS_QUEST_ITEM(target_obj)) {
					send_to_char("A strange put powerful force is stopping you from going to this item.\r\n", ch);
					return;
				}
	if (IS_PC(ch)) {
		if (ch->questmob)
			if (ch->questmob->in_room == location) {
				send_to_char("A strange put powerful force is stopping you from going to this room.\r\n", ch);
				return;
			}
		if (ch->questobj)
			if (ch->questobj->in_room == location) {
				send_to_char("There is a strange item at that location stopping you from going there.\r\n", ch);
				return;
			}
	}			/* end of quest part */

	/* SAME ROOM? */
	if (ch->in_room == location) {
		send_to_char("Your already there!\r\n", ch);
		return;
	}

	/* SUBTRACT PENALTY */
	if (GET_LEVEL(ch) <= PK_LEV) {
		GET_HIT(ch) -= ((GOTO_PENALTY * GET_MAX_HIT(ch)) + 2);
		GET_MOVE(ch) -= (GOTO_PENALTY * GET_MAX_MOVE(ch));
		GET_MANA(ch) -= ((GOTO_PENALTY * GET_MAX_MANA(ch)) + 2);
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT) {
		if (IS_SET(world[ch->in_room].room_flags, RM1_NO_TELEPORT_OUT)) {
			send_to_char("You feel a kernal of energy form, but nothing happens.\r\n", ch);
			return;
		}
		if (IS_SET(zone_table[world[ch->in_room].zone].flags,
			   ZONE_NO_GOTO)) {
			send_to_char("You feel a burst of energy, but nothing happens.\r\n", ch);
		}
		/* nothing is going to stop our leaving.  so if fighting... */
		if (ch->specials.fighting) {
			if (cl1900_at_war(ch, ch->specials.fighting))
				cl2000_player_escape(ch, ch->specials.fighting);
			at2000_do_mob_hunt_check(ch->specials.fighting, ch, 1);
			ft1400_stop_fighting(ch, 0);
		}
	}

	if (gv_port == ZONING_PORT) {
		if (!ha1175_isexactname(GET_REAL_NAME(ch), zone_table[world[location].zone].lord) &&
		    GET_LEVEL(ch) < IMO_IMM && world[location].number != 8312) {
			send_to_char("You are not authorized to go into that zone!\n\r", ch);
			return;
		}
	}

	if (ch->player.immortal_exit)
		strcpy(buf, ch->player.immortal_exit);
	else
		strcpy(buf, "$n disappears in a puff of smoke.");

	if (!(IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		act(buf, TRUE, ch, 0, 0, TO_ROOM);
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT &&
	    IS_SET(world[location].room_flags, RM1_DEATH) &&
	    !(ha1175_isexactname(GET_NAME(ch),
				 zone_table[world[location].zone].lord))) {

		/* DO WE NEED TO PROTECT AN AVATAR FROM A DT? */
		rc = ha9800_protect_char_from_dt(ch, location, 0);
		if (rc) {
			return;
		}

		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, location);
		ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_ROOM);
		return;

	}			/* END OF DEATH TRAP */

	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, location);
	if (ch->player.immortal_enter)
		strcpy(buf, ch->player.immortal_enter);
	else
		strcpy(buf, "$n magically appears in the middle of the room.");
	if (!(IS_SET(world[ch->in_room].room_flags, RM1_SOUNDPROOF))) {
		act(buf, TRUE, ch, 0, 0, TO_ROOM);
	}

	in3000_do_look(ch, "", 0);
	return;

}				/* END OF wi1500_do_goto() */


void wi1600_do_stat(struct char_data * ch, char *arg, int cmd)
{
	struct affected_type *aff;
	char arg1[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct room_data *rm = 0;
	struct char_data *k = 0;
	struct obj_data *j = 0;
	struct obj_data *j2 = 0;
	struct extra_descr_data *desc;
	struct follow_type *fol;
	int i, virtual, i2, zone;
	bool found;
	size_t size;



	arg = one_argument(arg, arg1);

	/* no arg */
	if (!*arg1) {
		send_to_char("Stats on who or what?\r\n", ch);
		send_to_char("   Format STAT ROOM\r\n", ch);
		send_to_char("   Format STAT <mob> (if not found, will check objects)\r\n", ch);
		send_to_char("   Format STAT obj <object>\r\n", ch);
		return;
	}

	if (!str_cmp("obj", arg1)) {
		strcpy(buf, arg1);
		/* STRIP OUT THE obj FROM arg1 */
		arg = one_argument(arg, arg1);
		goto DO_OBJECT;
	}

	/* stats on room */
	if (!str_cmp("room", arg1)) {

		rm = &world[ch->in_room];

		zone = world[ch->in_room].zone;
		CAN_PERSON_STAT(zone);

		sprintf(buf, "Room name: %s, Of zone: %d. V-Number: %d, R-number: %d next: %d\r\n",
			rm->name, rm->zone, rm->number, ch->in_room, rm->next);
		send_to_char(buf, ch);

		sprintf(buf, "Minimum level %d maximum level %d.\r\n",
			world[ch->in_room].min_level,
			world[ch->in_room].max_level);
		send_to_char(buf, ch);

		sprinttype(rm->sector_type, sector_types, buf2);
		sprintf(buf, "Sector type : %s", buf2);
		send_to_char(buf, ch);

		strcpy(buf, "Special procedure : ");
		strcat(buf, (rm->funct) ? "Exists\r\n" : "No\r\n");
		send_to_char(buf, ch);

		send_to_char("Room flags: ", ch);
		sprintbit((long) rm->room_flags, room_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		send_to_char("Description:\r\n", ch);
		send_to_char(rm->description, ch);

		strcpy(buf, "Extra description keywords(s): ");
		if (rm->ex_description) {
			strcat(buf, "\r\n");
			for (desc = rm->ex_description; desc; desc = desc->next) {
				strcat(buf, desc->keyword);
				strcat(buf, "\r\n");
			}
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}
		else {
			strcat(buf, "None\r\n");
			send_to_char(buf, ch);
		}

		strcpy(buf, "------- Chars present -------\r\n");
		size = strlen(buf);
		for (k = rm->people; k; k = k->next_in_room) {
			if (GET_LEVEL(ch) >= GET_VISIBLE(k)) {
				size = size + strlen(GET_NAME(k)) + strlen("(MOB)\r\n");
				strcat(buf, GET_NAME(k));
				strcat(buf, (!IS_NPC(k) ? "(PC)\r\n" : (!IS_MOB(k) ? "(NPC)\r\n" : "(MOB)\r\n")));
				if (size >= sizeof(buf)) {
					strcpy(buf + (sizeof(buf) - 1) - strlen(gv_overflow_buf) - strlen("\r\n"),
					       gv_overflow_buf);
					/* Don't forget the next strcat (\r\n) */
					break;
				}
			}
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		strcpy(buf, "--------- Contents ---------\r\n");
		size = strlen(buf);
		for (j = rm->contents; j; j = j->next_content) {
			size = size + strlen(j->name) + strlen("\r\n");
			strcat(buf, j->name);
			strcat(buf, "\r\n");
			if (size >= sizeof(buf)) {
				strcpy(buf + (sizeof(buf) - 1) - strlen(gv_overflow_buf) - strlen("\r\n"),
				       gv_overflow_buf);
				/* Don't forget the next strcat (\r\n) */
				break;
			}
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		send_to_char("------- Exits defined -------\r\n", ch);
		for (i = 0; i <= 5; i++) {
			if (rm->dir_option[i]) {
				sprintf(buf, "Direction %s . ", dirs[i]);
				send_to_char(buf, ch);
				if (rm->dir_option[i]->keyword) {
					sprintf(buf, "Keyword : %s\r\n",
						rm->dir_option[i]->keyword);
					send_to_char(buf, ch);
				}
				strcpy(buf, "Description:\r\n  ");
				if (rm->dir_option[i]->general_description)
					strcat(buf, rm->dir_option[i]->general_description);
				else
					strcat(buf, "UNDEFINED\r\n");
				send_to_char(buf, ch);
				sprintbit(rm->dir_option[i]->exit_info, exit_bits, buf2);
				sprintf(buf, "Exit flag: %s \r\nKey no: %d\r\nTo room (R-Number): %d\r\n",
					buf2, rm->dir_option[i]->key,
					rm->dir_option[i]->to_room);
				send_to_char(buf, ch);
				sprintf(buf1, "STAT: %s did stat ROOM in r[%d].", GET_REAL_NAME(ch), rm->number);
				spec_log(buf1, GOD_COMMAND_LOG);
			}
		}
		return;
	}

	/* ************* */
	/* MOB       */
	/* ************* */
	k = ha3100_get_char_vis(ch, arg1);
	if (k) {
		if (IS_NPC(k)) {
			zone = mob_index[k->nr].zone;
			CAN_PERSON_STAT(zone);
		}


		switch (k->player.sex) {
		case SEX_NEUTRAL:
			strcpy(buf, "NEUTER");
			break;
		case SEX_MALE:
			strcpy(buf, "MALE");
			break;
		case SEX_FEMALE:
			strcpy(buf, "FEMALE");
			break;
		default:
			strcpy(buf, "ILLEGAL-SEX!!");
			break;
		}

		sprintf(buf2, " %s - Name : %s [R-Number: %d, %d], In room [%d]\r\n",
			(!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
			GET_NAME(k), k->nr, db8100_real_mobile(mob_index[k->nr].virtual),
			world[k->in_room].number);
		strcat(buf, buf2);
		send_to_char(buf, ch);
		if (IS_MOB(k)) {
			sprintf(buf, "V-Number [%d]\r\n", mob_index[k->nr].virtual);
			send_to_char(buf, ch);
		}

		strcpy(buf, "Short description: ");
		strcat(buf, (k->player.short_descr ? k->player.short_descr : "None"));
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		strcpy(buf, "Title: ");
		strcat(buf, (k->player.title ? k->player.title : "None"));
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		send_to_char("Long description: ", ch);
		if (k->player.long_descr)
			send_to_char(k->player.long_descr, ch);
		else
			send_to_char("None", ch);
		send_to_char("\r\n", ch);

		if (IS_NPC(k)) {
			strcpy(buf, "Monster Class: ");
			sprinttype(k->player.class, npc_class_types, buf2);
		}
		else {
			strcpy(buf, "Class:[");
			sprinttype(k->player.class, pc_class_types, buf2);
		}
		strcat(buf, buf2);

		sprintf(buf2, "] Level[%d] Alignment[%d] ", k->player.level,
			k->specials.alignment);
		strcat(buf, buf2);
		send_to_char(buf, ch);
		sprintf(buf, "Age:[%d]Y [%d]M [%d]D [%d]H\r\n",
		 z_age(k).year, z_age(k).month, z_age(k).day, z_age(k).hours);
		send_to_char(buf, ch);

		sprintf(buf, "Height [%d]cm  Weight [%d]pounds \r\n", GET_HEIGHT(k), GET_WEIGHT(k));
		send_to_char(buf, ch);

		sprintf(buf, "Spells to learn[%d]\r\n",
			k->specials.spells_to_learn);
		send_to_char(buf, ch);

		sprintf(buf, "Str:[%d/%d]  Int:[%d/%d]  Wis:[%d/%d]  Dex:[%d/%d]  Con:[%d/%d] Cha: [%d/%d]\r\n",
			GET_REAL_STR(k),
			GET_STR(k),
			GET_REAL_INT(k),
			GET_INT(k),
			GET_REAL_WIS(k),
			GET_WIS(k),
			GET_REAL_DEX(k),
			GET_DEX(k),
			GET_REAL_CON(k),
			GET_CON(k),
			GET_REAL_CHA(k),
			GET_CHA(k));
		send_to_char(buf, ch);

		sprintf(buf, "Mana p.:[%d/%d+%d]  Hit p.:[%d/%d+%d]  Move p.:[%d/%d+%d]\r\n",
			GET_MANA(k), li1300_mana_limit(k), li1700_mana_gain(k),
			GET_HIT(k), li1500_hit_limit(k), li1800_hit_gain(k),
		      GET_MOVE(k), li1600_move_limit(k), li1900_move_gain(k));
		send_to_char(buf, ch);

		sprintf(buf, "AC:[%d], Coins: [%d], Exp: [%d], Hitroll: [%d], Damroll: [%d]\r\n",
		 (GET_AC(k) + li9750_ac_bonus(GET_DEX(k)) + GET_BONUS_DEX(k) +
		  races[GET_RACE(k)].adj_ac),
			GET_GOLD(k),
			GET_EXP(k),
			k->points.hitroll,
			k->points.damroll);
		send_to_char(buf, ch);

		sprinttype(GET_POS(k), position_types, buf2);
		sprintf(buf, "Pos.: %s, Fighting: %s", buf2,
			((k->specials.fighting) ? GET_NAME(k->specials.fighting) : "Nobody"));
		if (k->desc) {
			sprinttype(k->desc->connected, connected_types, buf2);
			strcat(buf, ", Connect: ");
			strcat(buf, buf2);
		}
		strcat(buf, ", Def. pos.: ");
		sprinttype((k->specials.default_pos), position_types, buf2);
		strcat(buf, buf2);
		strcat(buf, ", PC flags:\r\n ");
		sprintbit(GET_ACT1(k), player_bits1, buf2);
		strcat(buf, buf2);
		sprintbit(GET_ACT2(k), player_bits2, buf2);
		strcat(buf, buf2);
		sprintbit(GET_ACT3(k), player_bits3, buf2);
		strcat(buf, buf2);
		if (IS_PC(k)) {
			sprintbit(GET_DSC1(k), descriptor_bits1, buf2);
			strcat(buf, buf2);
		}

		sprintf(buf2, "\r\nTimer [%d] ", k->specials.timer);
		strcat(buf, buf2);
		send_to_char(buf, ch);

		if (IS_MOB(k)) {
			strcpy(buf, "\r\nMobile Special procedure : ");
			strcat(buf, (mob_index[k->nr].func ? "Exists\r\n" : "None\r\n"));
			send_to_char(buf, ch);
		}

		if (IS_NPC(k)) {
			sprintf(buf, "NPC Bare Hand Damage %dd%d.\r\n",
			      k->specials.damnodice, k->specials.damsizedice);
			send_to_char(buf, ch);
		}

		sprintf(buf, "Carried weight: %d   Carried items: %d\r\n",
			IS_CARRYING_W(k),
			IS_CARRYING_N(k));
		send_to_char(buf, ch);

		for (i = 0, j = k->carrying; j; j = j->next_content, i++);
		sprintf(buf, "Items in inventory: %d, ", i);

		for (i = 0, i2 = 0; i < MAX_WEAR; i++)
			if (k->equipment[i])
				i2++;
		sprintf(buf2, "Items in equipment: %d\r\n", i2);
		strcat(buf, buf2);
		send_to_char(buf, ch);

		sprintf(buf, "Apply saving throws: [%d][%d][%d][%d][%d] ",
			k->specials.apply_saving_throw[0],
			k->specials.apply_saving_throw[1],
			k->specials.apply_saving_throw[2],
			k->specials.apply_saving_throw[3],
			k->specials.apply_saving_throw[4]);
		send_to_char(buf, ch);

		sprintf(buf, "Thirst:%d, Hunger:%d, Drunk:%d\r\n",
			k->specials.conditions[THIRST],
			k->specials.conditions[FOOD],
			k->specials.conditions[DRUNK]);
		send_to_char(buf, ch);

		sprintf(buf, "Bounty: %s, Jailed %d time%s.\r\n",
			(bt1200_bounty_check(k)) ? "Present" : "None",
			k->jailed,
			(k->jailed == 1) ? "" : "s");
		send_to_char(buf, ch);

		sprintf(buf, "Normal Maxes : Mana.:[%d]  Hit p.:[%d]  Move p.:[%d]\r\n",
			k->points.max_mana,
			k->points.max_hit,
			k->points.max_move);
		send_to_char(buf, ch);

		sprintf(buf, "Bonuses : Hit p:[%d]  Mana:[%d]  Move:[%d]\r\n",
			races[GET_RACE(k)].adj_hit,
			races[GET_RACE(k)].adj_mana,
			races[GET_RACE(k)].adj_move);
		send_to_char(buf, ch);

		bzero(buf, sizeof(buf));
		sprintf(buf, "Invis level: %d  Start room: %d, IMM room %d\r\n",
			GET_VISIBLE(k), GET_START(k), GET_PRIVATE(k));
		send_to_char(buf, ch);

		sprintf(buf, "Master:'%s' ",
			((k->master) ? GET_REAL_NAME(k->master) : "NOBODY"));
		send_to_char(buf, ch);

		sprintf(buf, " Hunting: '%s' ",
			((HUNTING(k)) ? GET_REAL_NAME(HUNTING(k)) : "NOBODY"));
		send_to_char(buf, ch);

		send_to_char("Followers:\r\n", ch);
		for (fol = k->followers; fol; fol = fol->next)
			act("    $N", FALSE, ch, 0, fol->follower, TO_CHAR);

		/* Showing the bitvector */
		sprintbit(k->specials.affected_by, affected_bits, buf);
		send_to_char("Affected 01: ", ch);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		send_to_char("Affected 02: ", ch);
		sprintbit(k->specials.affected_02, affected_02_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		/* Routine to show what spells a char is affected by */
		if (k->affected) {
			send_to_char("\r\nAffecting Spells:\r\n--------------\r\n", ch);
			for (aff = k->affected; aff; aff = aff->next) {
				sprintf(buf, "Spell : '%s'\r\n", spell_names[aff->type - 1]);

				send_to_char(buf, ch);
				sprintf(buf, "     Modifies %s by %d points\r\n",
				   apply_types[aff->location], aff->modifier);
				send_to_char(buf, ch);
				sprintf(buf, "     Expires in %3d hours, Bits set ",
					aff->duration);
				send_to_char(buf, ch);
				sprintbit(aff->bitvector, affected_bits, buf);
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
				sprintf(buf1, "STAT: %s did stat mob/char [%s] v[%d].", GET_REAL_NAME(ch),
					(k->player.short_descr ? k->player.short_descr : "None"),
					mob_index[k->nr].virtual);
				spec_log(buf1, GOD_COMMAND_LOG);
			}
		}
		return;


	}

	/* ************* */
	/* OBJ       */
	/* ************* */
DO_OBJECT:

	/* stat on object */
	j = ha3200_get_obj_vis(ch, arg1);
	if (j) {

		if (j->item_number == NOWHERE) {
			send_to_char("This obj. is not available!\r\n", ch);
			return;
		}
		zone = obj_index[j->item_number].zone;
		CAN_PERSON_STAT(zone);

		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(GET_OBJ2(j), OBJ2_NO_IDENTIFY)) {
			send_to_char("That object can not be identified with this command.\r\n", ch);
			return;
		}

		/* DONT LET EM STAT NO_LOCATE ITEMS! */
		if (GET_LEVEL(ch) < IMO_IMM &&
		    IS_SET(GET_OBJ1(j), OBJ1_NO_LOCATE)) {
			send_to_char("That mobile/object not found in the whole world.\r\n", ch);
			return;
		}


		virtual = (j->item_number >= 0) ? obj_index[j->item_number].virtual : 0;
		sprintf(buf, "Object name: [%s], R-number: [%d], V-number: [%d] Item type: ",
			j->name, j->item_number, virtual);
		sprinttype(GET_ITEM_TYPE(j), item_types, buf2);
		strcat(buf, buf2);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		if (GET_LEVEL(ch) > IMO_IMM) {
			sprintf(buf, "Currently in game: %d  Percent reset: %d%%\r\n",
				obj_index[j->item_number].number,
				obj_index[j->item_number].maximum);
			send_to_char(buf, ch);
		}

		sprintf(buf, "Short description: %s\r\nLong description:\r\n%s\r\n",
		     ((j->short_description) ? j->short_description : "None"),
			((j->description) ? j->description : "None"));
		send_to_char(buf, ch);

		/*
		 if(j->ex_description){
			 strcpy(buf, "Extra description keyword(s):\r\n----------\r\n");
			 for (desc = j->ex_description; desc; desc = desc->next) {
				 strcat(buf, desc->keyword);
				 strcat(buf, "\r\n");
			 }
			 strcat(buf, "----------\r\n");
			 send_to_char(buf, ch);
		 } else {
			 strcpy(buf,"Extra description keyword(s): None\r\n");
			 send_to_char(buf, ch);
		 }
		 */
		send_to_char("Can be worn on: ", ch);
		sprintbit(j->obj_flags.wear_flags, wear_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		send_to_char("Set char bits: ", ch);
		sprintbit(j->obj_flags.bitvector, affected_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		send_to_char("Extra flags: ", ch);
		sprintbit(j->obj_flags.flags1, oflag1_bits, buf);
		send_to_char(buf, ch);
		sprintbit(j->obj_flags.flags2, oflag2_bits, buf);
		if ((OVAL1(j) > 0) &&
		    (OVAL1(j) < 36) &&
		    (GET_ITEM_TYPE(j) == ITEM_ARMOR)) {
			sprintf(buf2, "ANTI_%s ", race_list[OVAL1(j)]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if ((OVAL1(j) > 50) &&
		    (OVAL1(j) < 86) &&
		    (GET_ITEM_TYPE(j) == ITEM_ARMOR)) {
			sprintf(buf2, "%s_ONLY ", race_list[OVAL1(j) - 50]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if (GET_ITEM_TYPE(j) == ITEM_ARMOR &&
		    OVAL2(j) > 0) {
			sprintbit(OVAL2(j), class_anti, buf);
			strcat(buf, "\0");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		if (GET_ITEM_TYPE(j) == ITEM_WEAPON ||
		    GET_ITEM_TYPE(j) == ITEM_QSTWEAPON) {
			send_to_char("Weapon flags:", ch);
			sprintbit(j->obj_flags.value[0], weapon_bits, buf);
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}

		sprintf(buf, "Weight: %d, Value: %d, Cost/day: %d, Timer: %d\r\n",
			j->obj_flags.weight, j->obj_flags.cost,
			j->obj_flags.cost_per_day, GET_TIMER(j));
		send_to_char(buf, ch);

		strcpy(buf, "In room: ");
		if (j->in_room == NOWHERE)
			strcat(buf, "Nowhere");
		else {
			sprintf(buf2, "%d", world[j->in_room].number);
			strcat(buf, buf2);
		}
		strcat(buf, ", In object: ");
		strcat(buf, (!j->in_obj ? "None" : ha1100_fname(j->in_obj->name)));
		strcat(buf, ", Carried by:");
		strcat(buf, (!j->carried_by) ? "Nobody" : GET_NAME(j->carried_by));
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		switch (j->obj_flags.type_flag) {
		case ITEM_LIGHT:
			sprintf(buf, "Colour : [%d]\r\nType : [%d]\r\nHours : [%d]",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2]);
			break;
		case ITEM_SCROLL:
			sprintf(buf, "Spells : %d, %d, %d, %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		case ITEM_WAND:
		case ITEM_STAFF:
		case ITEM_SPELL:
			sprintf(buf, "Spell Level: %d Max Charge: %d Left: %d Spell: %d\r\n",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		case ITEM_WEAPON:
			sprintf(buf, "Tohit : %d\r\nTodam : %dD%d\r\nType : %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		case ITEM_FURNITURE:
			sprintf(buf, "Hit regen:[%d] Mana regen:[%d] Move regen:[%d]\r\n",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2]);
			/* REMOVED: Undef:[] -> j->obj_flags.value[3] */
			break;
		case ITEM_MISSILE:
			sprintf(buf, "Tohit : %d\r\nTodam : %d\r\nType : %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[3]);
			break;

		case ITEM_ARMOR:
			sprintf(buf, "AC-apply:[%d] Race(Anti/Only):[%d] Anti_Class:[%d] Undef:[%d]\r\n",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;

		case ITEM_POTION:
			sprintf(buf, "Spells : %d, %d, %d, %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		case ITEM_TRAP:
			sprintf(buf, "Spell : %d\r\n- Hitpoints : %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1]);
			break;
		case ITEM_CONTAINER:
			sprintf(buf, "Max-contains : %d\r\nLocktype : %d\r\nCorpse : %s",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[3] ? "Yes" : "No");
			break;
		case ITEM_DRINKCON:
			sprinttype(j->obj_flags.value[2], drinks, buf2);
			sprintf(buf, "Max-contains : %d\r\nContains : %dn\rPoisoned : %d\r\nLiquid : %s",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[3],
				buf2);
			break;
		case ITEM_NOTE:
			sprintf(buf, "Tounge : %d",
				j->obj_flags.value[0]);
			break;
		case ITEM_KEY:
			sprintf(buf, "Opens obj : %d",
				j->obj_flags.value[0]);
			strcat(buf, " Number of uses : ");
			if (j->obj_flags.value[1]) {
				sprintf(buf2, "%d", j->obj_flags.value[1]);
				strcat(buf, buf2);
			}
			else {
				strcat(buf, " unlimited");
			}
			break;
		case ITEM_FOOD:
			sprintf(buf, "Makes full : %d\r\nPoisoned : %d",
				j->obj_flags.value[0],
				j->obj_flags.value[3]);
			break;
		case ITEM_QUEST:
			sprintf(buf, "AC-apply is : %d\r\nHitpoints : %d",
				j->obj_flags.value[0],
				//j->obj_flags.value[1],
				j->obj_flags.value[2]);
			//j->obj_flags.value[3]);
			break;
		case ITEM_QSTWEAPON:
			sprintf(buf, "Wflag : %d\r\nTodam : %dD%d\r\nType : %d",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		case ITEM_QSTLIGHT:
			sprintf(buf, "Undef : [%d]\r\nUndef : [??]\r\nHours : [??]",
				j->obj_flags.value[2]);
			break;
		case ITEM_QSTCONT:
			sprintf(buf, "Max-contains : %d",
				j->obj_flags.value[0]);
			break;
		case ITEM_CORPSE:
			sprintf(buf, "Max-contains : %d\r\nOwner NR : %d\r\nKiller NR : %d\r\nCan Loot? : %s",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				(j->obj_flags.value[3] ? "No" : "Yes"));
			break;
		default:
			sprintf(buf, "Values 0-3 : [%d] [%d] [%d] [%d]",
				j->obj_flags.value[0],
				j->obj_flags.value[1],
				j->obj_flags.value[2],
				j->obj_flags.value[3]);
			break;
		}
		send_to_char(buf, ch);

		strcpy(buf, "\r\nEquipment Status: ");
		if (!j->carried_by)
			strcat(buf, "NONE");
		else {
			found = FALSE;
			for (i = 0; i < MAX_WEAR; i++) {
				if (j->carried_by->equipment[i] == j) {
					sprinttype(i, equipment_types, buf2);
					strcat(buf, buf2);
					found = TRUE;
				}
			}
			if (!found)
				strcat(buf, "Inventory");
		}
		send_to_char(buf, ch);

		strcpy(buf, "\r\nSpecial procedure : ");
		if (j->item_number >= 0)
			strcat(buf, (obj_index[j->item_number].func ? "exists\r\n" : "No\r\n"));
		else
			strcat(buf, "No\r\n");
		send_to_char(buf, ch);

		strcpy(buf, "Contains :\r\n");
		size = strlen(buf);
		found = FALSE;
		for (j2 = j->contains; j2; j2 = j2->next_content) {
			size = size + strlen(j2->short_description) + strlen("\r\n");
			strcat(buf, j2->short_description);
			strcat(buf, "\r\n");
			if (size >= sizeof(buf)) {
				strcpy(buf + (sizeof(buf) - 1) - strlen(gv_overflow_buf) - strlen("\r\n"),
				       gv_overflow_buf);
				strcat(buf, "\r\n");
				//What the hell, eh ?
					break;
			}
			found = TRUE;
		}
		if (!found)
			strcpy(buf, "Contains : Nothing\r\n");
		send_to_char(buf, ch);

		send_to_char("Can affect char :\r\n", ch);
		for (i = 0; i < MAX_OBJ_AFFECT; i++) {
			sprinttype(j->affected[i].location, apply_types, buf2);
			sprintf(buf, "    Affects : %s By %d\r\n", buf2, j->affected[i].modifier);
			send_to_char(buf, ch);
			sprintf(buf1, "STAT: %s did stat obj [%s] v[%d].", GET_REAL_NAME(ch), j->name, virtual);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
		return;
	}


	send_to_char("That mobile/object not found in the whole world.\r\n", ch);
	return;

}				/* END OF wi1600_do_stat() */



/* wi2000_do_shutdow */
void do_shutdow(struct char_data * ch, char *arg, int cmd)
{



	send_to_char("If you want to shut something down - say so!\r\n", ch);

}				/* END OF do_shutdow() */


void wi2100_do_shutdown(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	int lv_value;



	if (IS_NPC(ch))
		return;

	/* IF SHUTDOWN AND WE ARE AREN'T ON PRODUCTON VERSION */
	if (cmd == CMD_SHUTDOWN && gv_port != 4000) {
		send_to_char("You can only shutdown port 4000. Use HALT!\r\n", ch);
		if (gv_terminate_count > 0)
			co2900_send_to_char(ch, "Auto rebooting in %d minutes.\r\n", (int) gv_terminate_count);
		return;
	}

	/* DON'T ALLOW HALT TO WORK ON PRODUCTION SYSTEMS */
	if (cmd == CMD_HALT && gv_port == 4000) {
		send_to_char("You can't HALT port 4000. Use SHUTDOWN!\r\n", ch);
		return;
	}

	bzero(arg1, sizeof(arg1));
	bzero(arg2, sizeof(arg2));

	half_chop(arg, arg1, arg2);

	if (!*arg) {
		send_to_char("You have to enter IMMEDIATE or a numeric argument.\r\n", ch);
		send_to_char("You can also specify stop, to kill the autorun script, reboot,"
			     " to bring the MUD up quicker, or pause, to pause the script.\r\n", ch);
		if (gv_terminate_count > 0)
			co2900_send_to_char(ch, "Rebooting in %d minutes.\r\n", (int) gv_terminate_count);
		return;
	}

	if (!str_cmp(arg1, "immediate")) {
		sprintf(buf, "SYS: Terminated by %s.", GET_NAME(ch));
		gv_highest_login_level = IMO_IMM;
		co3000_send_to_all(buf);
		main_log(buf);
		spec_log(buf, GOD_COMMAND_LOG);
		spec_log(buf, SYSTEM_LOG);
		SET_BIT(gv_run_flag, RUN_FLAG_TERMINATE);
	}
	else if (!is_number(arg1)) {
		send_to_char("You have to enter IMMEDIATE or a numeric argument.\r\n", ch);
		return;
	}
	else {
		lv_value = atoi(arg1);
		sprintf(buf, "SYS: SHUTDOWN %d by %s.", lv_value, GET_REAL_NAME(ch));
		main_log(buf);
		spec_log(buf, GOD_COMMAND_LOG);
		spec_log(buf, SYSTEM_LOG);

		if (lv_value == 0) {
			gv_terminate_count = 0;
			send_to_char("System will not reboot.\r\n", ch);
			return;
		}
		else if (lv_value < 1) {
			send_to_char("Sorry, but a value of 1 or higher is required.\r\n", ch);
			return;
		}
		else {
			gv_terminate_count = lv_value;
			co2900_send_to_char(ch, "Rebooting in %d minutes.\r\n", (int) gv_terminate_count);
		}
	}
	if (*arg2) {
		if (is_abbrev(arg2, "stop")) {
			touch(KILL_SCRIPT_FILE);
			co2900_send_to_char(ch, "The autorun script will be STOPPED!\r\n");
		}
		else if (is_abbrev(arg2, "reboot")) {
			touch(FASTBOOT_SCRIPT_FILE);
			co2900_send_to_char(ch, "The autorun script will be rebooted, bringing up the MUD quicker than normal.\r\n");
		}
		else if (is_abbrev(arg2, "pause")) {
			touch(PAUSE_SCRIPT_FILE);
			co2900_send_to_char(ch, "The autorun script will be paused.  Remove the file named pause in the root of the port.\r\n");
		}
		else
			co2900_send_to_char(ch, "The script option can be: stop, reboot, or pause.\r\n");
	}

}				/* END OF wi2100_do_shutdown() */


void wi2200_do_snoop(struct char_data * ch, char *arg, int cmd)
{

	char arg1[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct descriptor_data *ddx;
	struct char_data *snooper, *victim;
	int lv_number;

	if (!ch->desc) {
		return;
	}

	one_argument(arg, arg1);

	if (!*arg1) {
		if (GET_LEVEL(ch) >= IMO_IMP) {
			lv_number = 0;
			send_to_char("\r\nSnoopers :\r\n----------\r\n", ch);
			for (ddx = descriptor_list; ddx; ddx = ddx->next) {
				if ((ddx->connected == CON_PLAYING)) {
					if (ddx->original)	/* If switched */
						snooper = ddx->original;
					else
						snooper = ddx->character;

					if ((ddx->snoop.snooping)) {
						lv_number++;
						victim = ddx->snoop.snooping;
						bzero(buf, sizeof(buf));
						if ((victim->desc) &&
						    (victim->desc->original)) {
							sprintf(buf, "   %15s is snooping %s (%s)\r\n",
							    GET_NAME(snooper),
							     GET_NAME(victim),
								GET_NAME(victim->desc->original));
						}
						else {
							sprintf(buf, "   %15s is snooping %s.\r\n",
								GET_NAME(snooper), GET_NAME(victim));
						}
						send_to_char(buf, ch);
					}
				}
			}
			if (lv_number < 1) {
				send_to_char("     none.\r\n", ch);
			}
			return;
		}
		else {
			send_to_char("Snoop who ?\r\n", ch);
			return;
		}
	}

	if (!(victim = ha3100_get_char_vis(ch, arg1))) {
		send_to_char("No such person around.\r\n", ch);
		return;
	}

	if (!victim->desc) {
		send_to_char("There's no link.. nothing to snoop.\r\n", ch);
		return;
	}

	if (victim == ch) {
		send_to_char("Ok, you just snoop yourself.\r\n", ch);
		if (ch->desc->snoop.snooping) {
			ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
			ch->desc->snoop.snooping = 0;
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s ends snoop.", GET_NAME(ch));
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}
		return;
	}

	if (GET_LEVEL(victim) >= GET_LEVEL(ch) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You failed.\r\n", ch);
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s tries to snoop %s, but fails.", GET_NAME(ch), GET_NAME(victim));
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		return;
	}

	if (ch->desc->snoop.snoop_by) {
		if (victim == ch->desc->snoop.snoop_by) {
			send_to_char("I don't think so...\r\n", ch);
			return;
		}
	}

	if (victim->desc->snoop.snoop_by) {
		/* IF A LOWER IMM WANTS IN AND YOUR INVIS IS HIGHER THAN THAT
		 * IMMS, LET HIM IN SO HE WON'T KNOW YOUR AROUND */
		snooper = victim->desc->snoop.snoop_by;
		if (GET_LEVEL(ch) < GET_LEVEL(snooper)) {
			/* CAN WE SNOOP THE NEW PERSON? */
			if (GET_LEVEL(snooper) > GET_LEVEL(ch) &&
			    (!(ch->desc->snoop.snoop_by))) {

				bzero(buf, sizeof(buf));
				sprintf(buf, "%s snoops %s cancelling your link. Swapping to %s\r\n", GET_NAME(ch), GET_NAME(victim), GET_NAME(ch));
				send_to_char(buf, snooper);
				/* BREAK CONNECTION */
				snooper->desc->snoop.snooping = ch;
				ch->desc->snoop.snoop_by = snooper;
				victim->desc->snoop.snoop_by = 0;
			}
			else {
				/* NEW GUY IS BUSY SO BREAK CONNECTION */
				bzero(buf, sizeof(buf));
				sprintf(buf, "%s snoops %s cancelling your link.\r\n",
					GET_NAME(ch), GET_NAME(victim));
				send_to_char(buf, snooper);
				/* BREAK CONNECTION */
				snooper->desc->snoop.snooping = 0;
				victim->desc->snoop.snoop_by = 0;
			}
		}
		else {
			send_to_char("Busy already. \r\n", ch);
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s tries to snoop %s but that person is busy.",
				GET_NAME(ch), GET_NAME(victim));
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
			return;
		}
	}

	send_to_char("Ok. \r\n", ch);

	if (ch->desc->snoop.snooping)
		ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;

	ch->desc->snoop.snooping = victim;
	victim->desc->snoop.snoop_by = ch;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s snoops %s.", GET_NAME(ch), GET_NAME(victim));
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

}				/* END OF wi2200_do_snoop */


void wi2300_do_switch(struct char_data * ch, char *arg, int cmd)
{
	char arg1[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct char_data *victim;
	struct descriptor_data *ddx;
	int lv_number, zone, rc;

	if (IS_NPC(ch)) {
		return;
	}

	one_argument(arg, arg1);

	if (!*arg1) {
		if (GET_LEVEL(ch) >= IMO_IMP) {
			lv_number = 0;
			send_to_char("\r\nSwitched :\r\n----------\r\n", ch);
			for (ddx = descriptor_list; ddx; ddx = ddx->next) {
				if ((ddx->connected == CON_PLAYING)) {
					if (ddx->original) {	/* If switched */
						victim = ddx->original;
						lv_number++;
						bzero(buf, sizeof(buf));
						sprintf(buf, "   %15s switched to %s.\r\n",
							GET_NAME(victim), GET_NAME(ddx->character));
						send_to_char(buf, ch);
					}
				}
			}
			if (lv_number < 1) {
				send_to_char("     none.\r\n", ch);
			}
			return;
		}
		else {
			send_to_char("Switch to who ?\r\n", ch);
			return;
		}
	}

	victim = ha2125_get_char_in_room_vis(ch, arg1);
	if (!victim) {
		victim = ha3100_get_char_vis(ch, arg1);
	}

	if (!victim) {
		send_to_char("They aren't here.\r\n", ch);
		return;
	}

	if (ch == victim) {
		send_to_char("He he he... We are jolly funny today, eh?\r\n", ch);
		return;
	}

	/* SAFETY CHECK? */
	if (!ch->desc) {
		send_to_char("Mixing snoop & switch is bad for your health.\r\n", ch);
		return;
	}

	/* ARE WE SNOOPING SOMEONE ELSE */
	if (ch->desc->snoop.snooping) {
		send_to_char("Mixing snoop & switch is bad for your health!\r\n", ch);
		return;
	}

	/* SOMEONE SNOOPING US? */
	if (ch->desc->snoop.snoop_by) {
		ch->desc->snoop.snoop_by->desc->snoop.snooping = 0;
		send_to_char("Your snoop victim switched. Terminating snoop\r\n",
			     ch->desc->snoop.snoop_by);
		ch->desc->snoop.snoop_by = 0;
	}

	if (victim->desc || (!IS_NPC(victim))) {
		send_to_char("You can't do that, the body is already in use!\r\n", ch);
		return;
	}

	zone = mob_index[victim->nr].zone;
	CAN_PERSON_STAT(zone);

	/* CAN THE CHAR ACCESS THE ROOM? */
	rc = li3000_is_blocked_from_room(ch, victim->in_room, 0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	if (gv_port == ZONING_PORT) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to use switch on this port!\n\r", ch);
			return;
		}
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s has switched to %s in room: %d.",
		GET_NAME(ch),
		GET_NAME(victim),
		world[victim->in_room].number);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);
	sprintf(buf, "You switch to %s in room: %d",
		GET_NAME(victim),
		world[victim->in_room].number);
	send_to_char(buf, ch);

	ch->desc->character = victim;
	ch->desc->original = ch;

	victim->desc = ch->desc;
	ch->desc = 0;

}				/* END OF do_switch() */


void wi2400_do_return(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	struct char_data *snooper;

	if (!ch->desc)
		return;

	if (!ch->desc->original) {
		send_to_char("Arglebargle, glop-glyf!?!\r\n", ch);
		return;
	}
	else {
		if (GET_LEVEL(ch->desc->original) >= IMO_IMM) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s has returned from %s.",
				GET_NAME(ch->desc->original),
				GET_NAME(ch));
			do_wizinfo(buf, GET_LEVEL(ch->desc->original) + 1, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}


		send_to_char("You return to your original body.\r\n", ch);

		ch->desc->character = ch->desc->original;
		ch->desc->original = 0;

		/* BEFORE RETURNING desc BACK TO PLAYER, SWAP SNOOP LINKS */
		snooper = ch->desc->snoop.snoop_by;
		if (snooper) {
			snooper->desc->snoop.snooping = ch->desc->character;
		}

		/* RETURN desc to original user */
		ch->desc->character->desc = ch->desc;
		ch->desc = 0;
	}
}				/* END OF wi2400_do_return() */


void wi2500_do_force(struct char_data * ch, char *arg, int cmd)
{
	struct descriptor_data *i;
	struct char_data *vict;
	char name[MAX_STRING_LENGTH], to_force[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int number_forced;

	number_forced = 0;
	if (IS_NPC(ch))
		return;

	half_chop(arg, name, to_force);

	if (!*name || !*to_force) {
		send_to_char("Who do you wish to force to do what?\r\n", ch);
		return;
	}

	if (str_cmp("all", name)) {
		if (!(vict = ha3100_get_char_vis(ch, name))) {
			send_to_char("No-one by that name here..\r\n", ch);
			return;
		}
		if (IS_PC(vict) &&
		    GET_LEVEL(ch) <= GET_LEVEL(vict) &&
		    GET_LEVEL(ch) < IMO_IMP) {

			act("$N is so awesome, your attempt rebounds and you do it yourself.", FALSE, ch, 0, vict, TO_CHAR);
			act("$n tries to force you but does it to $mself instead.", TRUE, ch, 0, vict, TO_VICT);

			co1500_write_to_head_q(to_force, &ch->desc->input);
			return;
		}
		/* DO FORCE */

		bzero(buf, sizeof(buf));
		one_argument(to_force, buf);
		if (is_abbrev(buf, "force")) {
			bzero(buf, sizeof(buf));
			send_to_char("You are treading on dangerous ground!\r\n", ch);
			sprintf(buf, "%s is attempting to crash the system with force loops!", GET_NAME(ch));
			do_wizinfo(buf, IMO_LEV, ch);
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}

		if (GET_LEVEL(vict) >= GET_VISIBLE(ch)) {
			sprintf(buf, "$n has forced you to '%s'.", to_force);
			act(buf, FALSE, ch, 0, vict, TO_VICT);
			send_to_char("Ok, but the person saw you do it!\r\n", ch);
		}
		else {
			act(buf, FALSE, ch, 0, vict, TO_VICT);
			send_to_char("Ok. They didn't see you do it.\r\n", ch);
		}
		command_interpreter(vict, to_force);
		sprintf(buf, "[%s] force%s.", ch->player.name, arg);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}
	else {			/* force all */

		/* ONLY Ante or higher */
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("One at a time please!\r\n", ch);
			return;
		}

		bzero(buf, sizeof(buf));
		one_argument(to_force, buf);
		if (is_abbrev(buf, "force")) {
			bzero(buf, sizeof(buf));
			send_to_char("You are treading on dangerous ground!\r\n", ch);
			sprintf(buf, "%s is attempting to crash the system with force loops.", GET_NAME(ch));
			do_wizinfo(buf, IMO_LEV, ch);
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}

		for (i = descriptor_list; i; i = i->next) {
			if (i->character != ch && !i->connected) {
				vict = i->character;
				if (CAN_SEE(ch, i->character)) {
					if (GET_LEVEL(ch) <= GET_LEVEL(vict)) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "Sorry, %s is to awesome to force.\r\n",
							GET_NAME(vict));
						send_to_char(buf, ch);
					}
					else {
						number_forced++;
						if (GET_LEVEL(vict) >= GET_VISIBLE(ch)) {
							sprintf(buf, "$n has forced you to '%s'.", to_force);
							act(buf, FALSE, ch, 0, vict, TO_VICT);
						}
						command_interpreter(vict, to_force);
					}
				}	/* END OF CAN SEE */
			}
		}		/* END OF for */
		if (number_forced == 1) {
			send_to_char("You forced 1 player.\r\n", ch);
			sprintf(buf, "[%s] force%s.", ch->player.name, arg);
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}
		if (number_forced > 1) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "You forced %d players.\r\n", number_forced);
			send_to_char(buf, ch);
			sprintf(buf, "[%s] force%s.", ch->player.name, arg);
			do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
			spec_log(buf, GOD_COMMAND_LOG);
		}
	}			/* END OF else ALL */
}				/* END OF wi2500_do_force () */


void wi2600_do_vnum(struct char_data * ch, char *arg, int cmd)
{
	struct obj_data *tmp_obj;
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int i, lv_count;

	arg = one_argument(arg, buf);
	if (*buf)
		arg = one_argument(arg, buf1);

	if (!*buf || !*buf1) {
		send_to_char("Try vnum <obj || mob> <keyword>\r\n", ch);
		return;
	}

	arg = one_argument(arg, buf3);

	lv_count = 0;
	if (is_abbrev(buf, "obj")) {

		for (i = 0; i <= top_of_objt; i++) {

			if (ha1175_isexactname(buf1, obj_index[i].name) && ((!*buf3) || ha1175_isexactname(buf3, obj_index[i].name))) {
				if (lv_count == 0) {
					send_to_char("[Virt#] curr.  perc Object Name\r\n", ch);
				}
				lv_count++;
				bzero(buf2, sizeof(buf2));
				if (GET_LEVEL(ch) >= IMO_IMM) {
					tmp_obj = db5100_read_object(obj_index[i].virtual, PROTOTYPE);
					if (tmp_obj) {
						if (IS_OBJ_STAT(tmp_obj, OBJ1_NORENT)) {
							strcpy(buf2, " <NORENT>");
						}
						if (GET_ITEM_TYPE(tmp_obj) == ITEM_QUEST ||
						    GET_ITEM_TYPE(tmp_obj) == ITEM_QSTWEAPON ||
						    GET_ITEM_TYPE(tmp_obj) == ITEM_QSTLIGHT) {
							strcpy(buf2, " <QUEST>");
						}
					}
				}

				if (IS_SET(obj_index[i].flags, OBJI_OMAX)) {
					sprintf(buf, "[%5d] %5d %5d*%s %s\r\n",
						obj_index[i].virtual,
						obj_index[i].number,
						obj_index[i].maximum,
						obj_index[i].name,
						buf2);
					send_to_char(buf, ch);
				}
				else {
					sprintf(buf, "[%5d] %5d %5d %s %s\r\n",
						obj_index[i].virtual,
						obj_index[i].number,
						obj_index[i].maximum,
						obj_index[i].name,
						buf2);
					send_to_char(buf, ch);
				}
			}
		}
		if (lv_count) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Found %d objects.\r\n", lv_count);
			send_to_char(buf, ch);
		}
		else {
			send_to_char("I'm sorry, but I was unable to locate that object.\r\n", ch);
		}
		sprintf(buf2, "VNUM: %s entered vnum %s.", GET_REAL_NAME(ch), buf1);
		spec_log(buf2, GOD_COMMAND_LOG);
		return;
	}			/* END OF its an obj */

	if (is_abbrev(buf, "mob") ||
	    is_abbrev(buf, "char")) {
		/* its a mob */
		for (i = 0; i <= top_of_mobt; i++) {
			if (ha1175_isexactname(buf1, mob_index[i].name)) {
				lv_count++;
				sprintf(buf, "[%5d] (%3d) %s\r\n",
					mob_index[i].virtual,
					mob_index[i].number,
					mob_index[i].name);
				send_to_char(buf, ch);
			}
		}
		if (lv_count) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Found %d mobs.\r\n", lv_count);
			send_to_char(buf, ch);
		}
		else {
			send_to_char("I'm sorry, but I was unable to locate that mob.\r\n", ch);
		}
		sprintf(buf2, "VNUM: %s entered vnum %s.", GET_REAL_NAME(ch), buf1);
		spec_log(buf2, GOD_COMMAND_LOG);
		return;
	}			/* END OF mob/char */

	send_to_char("Try vnum <obj || mob> <keyword>.\r\n", ch);
	return;
}				/* END OF wi2600_do_vnum() */


void wi2700_do_load(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *mob;
	struct obj_data *obj;
	char type[MAX_STRING_LENGTH], num[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], tmp[MAX_STRING_LENGTH],
	  tmp2[MAX_STRING_LENGTH], timed1[MAX_STRING_LENGTH], timed2[MAX_STRING_LENGTH];
	int lv_number, r_num, c_num, zone, lv_timer[2];

	lv_timer[0] = -1;
	lv_timer[1] = -1;


	if (IS_NPC(ch) && cmd != CMD_QUEST && !gv_questreact)
		return;

	if (ch->in_room < 0) {
		send_to_char("Your NOWHERE right now.\r\n", ch);
		return;
	}
	for (; *arg == ' '; arg++);
	/* I know, yuck.  I should just do this myself. */
	half_chop(arg, type, tmp);
	half_chop(tmp, num, tmp2);
	half_chop(tmp2, timed1, timed2);

	if (GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("You don't have anything to load!\r\n", ch);
		return;
	}

	if (!*type) {
		send_to_char("Syntax:\r\n   load <'char/mob' | 'obj'> <number>.\r\n", ch);
		if (GET_LEVEL(ch) == IMO_IMP) {
			send_to_char("or load <news, commands, todo, classes, credits, wiztitles\r\n         motd, info, races, syslog0, changelog or wizlist>\r\n", ch);
		}
		return;
	}

	if (GET_LEVEL(ch) >= IMO_IMP) {
		if (is_abbrev(type, "news")) {
			free(news);
			ha9900_sanity_check(0, "FREE41", "SYSTEM");
			news = db7300_file_to_string(NEWS_FILE);
			send_to_char("News re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded NEWS.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "todo")) {
			free(todo);
			ha9900_sanity_check(0, "FREE41", "SYSTEM");
			todo = db7300_file_to_string(TODO_FILE);
			send_to_char("Todo re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded TODO.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "commands")) {
			free(commands);
			ha9900_sanity_check(0, "FREE41", "SYSTEM");
			commands = db7300_file_to_string(COMMANDS_FILE);
			send_to_char("Commands re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded commands.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "wipe")) {
			free(wipe);
			ha9900_sanity_check(0, "FREE4_1.5", "SYSTEM");
			wipe = db7300_file_to_string(WIPE_FILE);
			send_to_char("Wipe re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded WIPE.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "classes")) {
			db5300_load_classes();
			send_to_char("Classes loaded.\r\n", ch);
			sprintf(buf, " %s has loaded classes.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "clans")) {
			send_to_char("You can no longer reload clans, use admin to change clan info.\r\n", ch);
			return;
		}
		else if (is_abbrev(type, "credits")) {
			free(credits);
			ha9900_sanity_check(0, "FREE42", "SYSTEM");
			credits = db7300_file_to_string(CREDITS_FILE);
			send_to_char("Credits re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded credits.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "motd")) {
			free(motd);
			ha9900_sanity_check(0, "FREE43", "SYSTEM");
			motd = db7300_file_to_string(MOTD_FILE);
			send_to_char("MOTD re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded MOTD.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "info")) {
			free(info);
			ha9900_sanity_check(0, "FREE44", "SYSTEM");
			info = db7300_file_to_string(INFO_FILE);
			send_to_char("Info re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded INFO.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "help")) {
			free(help);
			ha9900_sanity_check(0, "FREE44", "SYSTEM");
			help = db7300_file_to_string(HELP_PAGE_FILE);
			send_to_char("Help page loaded.\r\n", ch);
			fclose(help_fl);
			help_fl = fopen(HELP_KWRD_FILE, "r");
			if (!help_fl)
				main_log("   Could not open help file.");
			else {
				free(help_index);
				ha9900_sanity_check(0, "FREE44.5", "SYSTEM");
				top_of_helpt = 0;
				help_index = build_help_index(help_fl, &top_of_helpt);
				help = db7300_file_to_string(HELP_PAGE_FILE);
				send_to_char("Help index loaded.\r\n", ch);
			}
			sprintf(buf, " %s has loaded HELP.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "races")) {
			db5200_load_races();
			send_to_char("Races loaded.\r\n", ch);
			sprintf(buf, " %s has loaded races.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "wiztitles")) {
			loadwiztitles();
			send_to_char("Wiztitles loaded.\r\n", ch);
			sprintf(buf, " %s has loaded wiztitles.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "syslog")) {
			free(gv_syslog);
			ha9900_sanity_check(0, "FREE45", "SYSTEM");
			gv_syslog = db7300_file_to_string(SYSLOG_FILE);
			send_to_char("Syslog re-loaded.\r\n", ch);
			gv_syslog_loaded = TRUE;
			sprintf(buf, " %s has loaded syslog.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "syslog0")) {
			free(gv_syslog);
			ha9900_sanity_check(0, "FREE46", "SYSTEM");
			CREATE(gv_syslog, char, 7);
			strcpy(gv_syslog, "syslog");
			send_to_char("Syslog zeroed!\r\n", ch);
			gv_syslog_loaded = FALSE;
			sprintf(buf, " %s has zeroed syslog.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "wizlist")) {
			free(wizlist);
			ha9900_sanity_check(0, "FREE47", "SYSTEM");
			wizlist = db7300_file_to_string(WIZLIST_FILE);
			send_to_char("Wizlist re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded wizlist.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
		else if (is_abbrev(type, "changelog")) {
			free(changelog);
			ha9900_sanity_check(0, "FREE47", "SYSTEM");
			wizlist = db7300_file_to_string(CHANGELOG_FILE);
			send_to_char("Changelog re-loaded.\r\n", ch);
			sprintf(buf, " %s has loaded the changelog.", GET_REAL_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}
	}			/* END OF priv loads */
	/* loads a deck of cards for quests Bingo 03-08-01 */
	if (is_abbrev(type, "deckofcards")) {
		wi2700_do_load(ch, "obj 17", cmd);
		for (c_num = 0; c_num < 53; c_num++) {
			sprintf(buf, "obj %d", 15200 + c_num);
			wi2700_do_load(ch, buf, cmd);
			do_put(ch, "card deck", cmd);
		}
	}


	if (!*type || !*num || !isdigit(*num)) {
		send_to_char("Syntax:\r\n   load <'char/mob' | 'obj'> <number>.\r\n", ch);
		return;
	}

	if ((lv_number = atoi(num)) < 0) {
		send_to_char("A *NEGATIVE* number??\r\n", ch);
		return;
	}

	if ((is_abbrev(type, "char")) || (is_abbrev(type, "mob"))) {
		if (ch->in_room == db8000_real_room(PET_ROOM)) {
			send_to_char("You can't load chars in this room.\r\n", ch);
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s tried to load a mob in pet shop room.\r\n",
				GET_NAME(ch));
			do_sys(buf, IMO_IMM, ch);
			spec_log(buf, GOD_COMMAND_LOG);
			return;
		}

		if ((r_num = db8100_real_mobile(lv_number)) < 0) {
			send_to_char("There is no mob with that number.\r\n", ch);
			return;
		}

		zone = mob_index[r_num].zone;
		if ((GET_LEVEL(ch) < IMO_IMM) &&
		    !(gv_port == 4500 && GET_LEVEL(ch) >= IMO_IMM) &&
		 (!ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord))) {
			send_to_char("You are not authorized to create in that zone.\r\n", ch);
			return;
		}

		mob = db5000_read_mobile(r_num, REAL);
		GET_START(mob) = world[ch->in_room].number;
		ha1600_char_to_room(mob, ch->in_room);

		act("$n makes a quaint, magical gesture with one hand.", TRUE, ch,
		    0, 0, TO_ROOM);
		act("$n has created $N!", TRUE, ch, 0, mob, TO_ROOM);
		act("You have made $N", FALSE, ch, 0, mob, TO_CHAR);
		sprintf(buf, "[%s] load%s %s.",
			ch->player.name, arg, mob->player.short_descr);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}
	else {
		if (is_abbrev(type, "obj")) {
			if ((r_num = db8200_real_object(lv_number)) < 0) {
				send_to_char("There is no object with that number.\r\n", ch);
				return;
			}
			zone = obj_index[r_num].zone;

			if (GET_LEVEL(ch) < IMO_IMM &&
			    (obj_index[r_num].virtual == OBJ_BOARD_MORTAL ||
			     obj_index[r_num].virtual == OBJ_BOARD_AUCTION ||
			     obj_index[r_num].virtual == OBJ_BOARD_COUNSIL ||
			     obj_index[r_num].virtual == OBJ_BOARD_CREGRP ||
			     obj_index[r_num].virtual == OBJ_BOARD_COUGRP ||
			     obj_index[r_num].virtual == OBJ_BOARD_DEVGRP ||
			     obj_index[r_num].virtual == OBJ_BOARD_RELGRP ||
			     obj_index[r_num].virtual == OBJ_BOARD_SOCIAL ||
			     obj_index[r_num].virtual == OBJ_BOARD_APPEALS ||
			     obj_index[r_num].virtual == OBJ_CLAN_BOARD6 ||
			     obj_index[r_num].virtual == OBJ_CLAN_BOARD7 ||
			     obj_index[r_num].virtual == OBJ_CLAN_BOARD10 ||
			     obj_index[r_num].virtual == OBJ_BOARD_IMMORTAL ||
			     obj_index[r_num].virtual == OBJ_BOARD_ZONEBUILD ||
			     obj_index[r_num].virtual == OBJ_BOARD_ANTE ||
			     obj_index[r_num].virtual == OBJ_BOARD_HIGHER ||
			     obj_index[r_num].virtual == OBJ_BOARD_OVERLORD ||
			 obj_index[r_num].virtual == OBJ_BOARD_IMPLEMENTOR)) {
				send_to_char("Sorry, but you can't load boards.\r\n", ch);
				return;
			}

			if (obj_index[r_num].virtual == 875 && (GET_LEVEL(ch) < IMO_IMP)) {
				send_to_char("Sorry, but you can't load the Logrus!", ch);
				return;
			}

			if (obj_index[r_num].virtual == 874 && (GET_LEVEL(ch) < IMO_IMP)) {
				send_to_char("Sorry, but you can't load Lord Relic's Sceptre!", ch);
				return;
			}

			if ((GET_LEVEL(ch) < IMO_IMM) &&
			!(gv_port == 4500 && GET_LEVEL(ch) >= IMO_IMM) &&
			    (!ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord))) {
				send_to_char("You are not authorized to create in that zone.\r\n", ch);
				return;
			}

			if (*timed1 && isdigit(*timed1))
				lv_timer[0] = atoi(timed1);

			if (*timed2 && isdigit(*timed2))
				lv_timer[1] = atoi(timed2);

			obj = db5100_read_object(r_num, REAL);
			if (CAN_WEAR(obj, ITEM_TAKE)) {
				ha1700_obj_to_char(obj, ch);
			}
			else {
				ha2100_obj_to_room(obj, ch->in_room);
			}
			act("$n makes a strange magical gesture.", TRUE, ch, 0, 0, TO_ROOM);
			act("$n has created $p!", TRUE, ch, obj, 0, TO_ROOM);
			act("You have created $p!", FALSE, ch, obj, 0, TO_CHAR);
			if (lv_timer[0] > -1) {
				if (lv_timer[1] != -1 && lv_timer[0] > lv_timer[1]) {
					/* Basically, if there is a lv_timer[1]
					 * and it's smaller than the
					 * lv_timer[0] */
					send_to_char("Minumum random number must not exceed maximum number!\r\n", ch);
				}
				else if (lv_timer[0] > 500 || lv_timer[1] > 500)
					send_to_char("Maximum random timer seed is 500!\r\n", ch);
				else {
					if (lv_timer[1] > -1)
						TIMER(obj) = number(lv_timer[0], lv_timer[1]) + 1;
					else if (lv_timer[0] < 1)
						send_to_char("Maximum timer must be at least 1.", ch);
					else
						TIMER(obj) = number(1, lv_timer[0]) + 1;
					sprintf(buf, "[%s] load%s %s [%d].",
						GET_NAME(ch), arg, obj->short_description, GET_TIMER(obj));
					do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
					spec_log(buf, GOD_COMMAND_LOG);
				}
			}
			else {
				sprintf(buf, "[%s] load%s %s.", ch->player.name, arg, obj->short_description);
				do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
				spec_log(buf, GOD_COMMAND_LOG);
			}
		}
		else {
			send_to_char("Syntax:\r\n   load <'char/mob' | 'obj'> <number>.\r\n", ch);
			send_to_char("or load <news, todo, commands, credits, motd, info, syslog0, or wizlist>\r\n", ch);
			return;
		}

		return;
	}
	return;

}				/* END OF wi2700_do_load() */
/* clean a room of all mobiles and objects */
void wi2800_do_purge(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *vict, *next_v;
	struct obj_data *obj, *next_o;
	int zone, location, idx, count = 0;
	int lv_purge_room = 30;

	char name[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];



	/* error check bypassed by negative cmd (and no output too!) */
	/* so that zreboot is faster mostly */
	if (cmd < 0) {
		*name = '\0';
	}
	else {
		if (IS_NPC(ch))
			return;
		one_argument(arg, name);
		zone = world[ch->in_room].zone;
		if ((GET_LEVEL(ch) < IMO_IMM) &&
		 (!ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord))) {
			send_to_char("You are not authorized to do that here.\r\n", ch);
			return;
		}
		if (!strncmp(name, "all", 4)) {
			*name = '\0';
		}
		else {
			if (!*name) {
				send_to_char("Please specify what you want to purge.  all for all.\r\n", ch);
				return;
			}
		}		/* END OF else */
	}			/* END OF else */

	if (*name) {		/* arg supplied. destroy single object or char */
		if ((vict = ha2125_get_char_in_room_vis(ch, name))) {
			if (!IS_NPC(vict) && (GET_LEVEL(ch) < IMO_IMM)) {
				send_to_char("I don't think so!\r\n", ch);
				return;
			}

			if (((IS_PC(vict)) &&
			     GET_LEVEL(ch) < GET_LEVEL(vict)) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("That person is to awesome to purge!\r\n", ch);
				if (GET_LEVEL(vict) >= IMO_IMP) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "%s tries unsuccessfully to purge you!\r\n", GET_NAME(ch));
					send_to_char(buf, vict);
				}
				return;
			}

			act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);
			act("You disintegrate $N.", FALSE, ch, 0, vict, TO_CHAR);

			if (IS_NPC(vict)) {
				ha3000_extract_char(vict, END_EXTRACT_BY_PURGE_NPC);
			}
			else {
				if (r1100_rent_delete_file(GET_REAL_NAME(vict))) {
					send_to_char("Players rentfile has been purged!\r\n",
						     ch);
				}
				else {
					send_to_char("Player did not have a rentfile.\r\n", ch);
				}
				GET_HOW_LEFT(vict) = LEFT_BY_PURGE;
				if (gv_mega_verbose_messages == TRUE) {
					main_log("Leaving by PURGE");

				}
				//locate purge room
					for (location = 0; location <= top_of_world; location++)
					if (world[location].number == lv_purge_room)
						break;

				if (location > top_of_world) {
					send_to_char("Can't locate purge room!\r\n", ch);
					location = 1;
					sprintf(buf, "Items are moved to room #%d\r\n", world[location].number);
					send_to_char(buf, ch);
				}


				//Move all players equiment to purge room
					for (idx = 0; idx < MAX_WEAR; idx++)
					if ((obj = vict->equipment[idx])) {
						ha2100_obj_to_room(ha1930_unequip_char(vict, idx), location);
						count++;
					}

				for (obj = vict->carrying; obj; obj = next_o) {
					next_o = obj->next_content;
					ha1800_obj_from_char(obj);
					ha2100_obj_to_room(obj, location);
					count++;
				}
				sprintf(buf, "%d item(s) moved to room #%d\r\n", count, world[location].number);
				send_to_char(buf, ch);

				ha3000_extract_char(vict, END_EXTRACT_BY_PURGE_PC);


			}	/* END OF ELSE */
		}
		else if ((obj = ha2075_get_obj_list_vis(ch, name, world[ch->in_room].contents))) {
			act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
			act("You destroy $p.", FALSE, ch, obj, 0, TO_CHAR);
			ha2700_extract_obj(obj);
		}
		else {
			send_to_char("I don't know anyone or anything by that name.\r\n", ch);
			return;
		}

	}
	else {			/* no arg. clean out room */
		if (cmd >= 0) {	/* neg command masks output(for zreboot mostly) */
			send_to_char("You purge the room of mobiles and objects.\r\n", ch);
			act("$n gestures... You are surrounded by scorching flames!",
			    FALSE, ch, 0, 0, TO_ROOM);
			co3300_send_to_room("The world seems a little cleaner.\r\n", ch->in_room);
		}
		for (vict = world[ch->in_room].people; vict; vict = next_v) {
			next_v = vict->next_in_room;
			if (IS_NPC(vict))
				ha3000_extract_char(vict, END_EXTRACT_BY_PURGE_ALL);
		}

		for (obj = world[ch->in_room].contents; obj; obj = next_o) {
			next_o = obj->next_content;
			ha2700_extract_obj(obj);
		}
	}
	if (cmd >= 0) {
		sprintf(buf, "%s purge %s", ch->player.name, name);
		do_sys(buf, IMO_IMM, ch);
		main_log(buf);
		spec_log(buf, GOD_COMMAND_LOG);
		spec_log(buf, EQUIPMENT_LOG);

	}
}				/* END OF wi2800_do_purge() */

void wi2900_purge_items(struct char_data * ch, char *arg, int cmd)
{

	int idx;
	struct obj_data *obj, *next_obj;
	//char buf1[MAX_STRING_LENGTH];



	for (idx = 0; idx < MAX_WEAR; idx++) {
		if (ch->equipment[idx]) {
			obj = ch->equipment[idx];
			ha1700_obj_to_char(ha1930_unequip_char(ch, idx), ch);
			/* REMOVE LIGHT SOURCE */
			if (obj->obj_flags.type_flag == ITEM_LIGHT ||
			    obj->obj_flags.type_flag == ITEM_QSTLIGHT)
				if (obj->obj_flags.value[2])
					world[ch->in_room].light--;
		}
	}			/* END OF for loop */

	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;

		/* REMOVE OBJECT FROM INVENTORY */
		if ((IS_OBJ_STAT(obj, OBJ1_NORENT)) ||
		    (GET_ITEM_TYPE(obj) == ITEM_QUEST) ||
		    (GET_ITEM_TYPE(obj) == ITEM_QSTWEAPON) ||
		    (GET_ITEM_TYPE(obj) == ITEM_QSTLIGHT)) {
			ha2700_extract_obj(obj);
		} //We only want quest and no_rent items purged - Bingo
	}			/* END OF for loop */

	return;

}				/* END OF wi2900_purge_items() */


/* This routine creates a link from one room to another.  It returns */
/* TRUE if the link is created and FALSE if its not                  */
int wi2920_create_room_links(int lv_from_real_room, int lv_to_real_room,
			       int lv_dir, char *gen_desc)
{
	char buf[MAX_STRING_LENGTH];



	if (lv_dir < 0 || lv_dir > MAX_DIRS) {
		sprintf(buf, "ERROR: wi2920_create_room_link called with invalid lv_dir %d.",
			lv_dir);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	if (lv_from_real_room < 0 || lv_from_real_room > top_of_world) {
		sprintf(buf, "ERROR: wi2920_create_room_link called with invalid from room %d.",
			lv_from_real_room);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	if (lv_to_real_room < 0 || lv_to_real_room > top_of_world) {
		sprintf(buf, "ERROR: wi2920_create_room_link called with invalid to room %d.",
			lv_to_real_room);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* GET RID OF OLD LINK */
	wi2925_remove_room_links(world[lv_from_real_room].number, lv_dir);

	CREATE(world[lv_from_real_room].dir_option[lv_dir], struct room_direction_data, 1);
	if (gen_desc)
		world[lv_from_real_room].dir_option[lv_dir]->general_description = strdup(gen_desc);
	else
		world[lv_from_real_room].dir_option[lv_dir]->general_description = 0;
	world[lv_from_real_room].dir_option[lv_dir]->keyword = 0;
	if (*gen_desc) {	/* if there are keywords add them */
		world[lv_from_real_room].dir_option[lv_dir]->keyword = str_alloc(gen_desc);
	}
	world[lv_from_real_room].dir_option[lv_dir]->exit_info = 0;
	world[lv_from_real_room].dir_option[lv_dir]->key = -1;
	world[lv_from_real_room].dir_option[lv_dir]->to_room = lv_to_real_room;

	return (TRUE);

}				/* END OF wi2920_create_room_links() */


void wi2925_remove_room_links(int lv_virtual_room, int lv_dir)
{

	int lv_real_num;



	lv_real_num = db8000_real_room(lv_virtual_room);
	if (lv_real_num < 0) {
		return;
	}

	/* REMOVE LINK ROOM ROOM */
	if (world[lv_real_num].dir_option[lv_dir] > 0) {
		if (world[lv_real_num].dir_option[lv_dir]->general_description) {
			free((char *) world[lv_real_num].dir_option[lv_dir]->general_description);
		}
		if (world[lv_real_num].dir_option[lv_dir]->keyword) {
			free((char *) world[lv_real_num].dir_option[lv_dir]->keyword);
		}
		free((char *) world[lv_real_num].dir_option[lv_dir]);
		world[lv_real_num].dir_option[lv_dir] = 0;
	}

}				/* END OF wi2925_remove_room_links() */

void wi2950_remove_virtual_obj_from_room(int lv_obj_num, int lv_room)
{

	char buf[MAX_STRING_LENGTH];
	int lv_real_room;
	struct obj_data *lv_obj, *lv_next_obj;



	lv_real_room = db8000_real_room(lv_room);
	if (lv_real_room < 0) {	/* NOT MUCH WE CAN DO HERE */
		sprintf(buf, "ERROR: Room invalid in wi2950 %d", lv_room);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return;
	}

	for (lv_obj = world[lv_real_room].contents; lv_obj; lv_obj = lv_next_obj) {
		lv_next_obj = lv_obj->next_content;
		if (obj_index[lv_obj->item_number].virtual == lv_obj_num) {
			ha2200_obj_from_room(lv_obj);
		}
	}			/* END OF for loop */

	return;

}				/* END OF wi2950_remove_virtual_obj_from_room() */
/* WHERE lv_flag                                                   */
/*                  BIT0 - Full reset                              */
void wi3000_create_level_one_char(struct char_data * ch, int lv_flag)
{

	int i, idx;
	/* int starter_kit[]={3010,3102,3403,3404, 3051, 3052, -1}; */
	/* sword wear  boot  pants yellow recall */
	int starter_kit[] = {3403, 3404, 3405, 3081, 3435, 3051, 3052, -1};
	struct obj_data *obj;
	char buf[MAX_INPUT_LENGTH];



	if (IS_SET(lv_flag, BIT0)) {
		send_to_char("******************************************************************************\r\n", ch);
		send_to_char("*    You are now in your new character.  You'll want to earn some experience,*\r\n", ch);
		send_to_char("* gain some levels, and make new friends.  We at Crimson welcome you here and*\r\n", ch);
		send_to_char("* hope you find it a friendly and rewarding experience.  If there is anything*\r\n", ch);
		send_to_char("* we've overlooked or some way we can be of further assistance, let us know  *\r\n", ch);
		send_to_char("* and we'll do our best.                                                     *\r\n", ch);
		send_to_char("*    To assist you in your new life, you've been given armor and a weapon,   *\r\n", ch);
		send_to_char("* and trained to use them.                                                   *\r\n", ch);
		send_to_char("*    There are many commands to help you get started, but INFO and HELP are  *\r\n", ch);
		send_to_char("* the most useful.  As mentioned on the create screen when you picked your   *\r\n", ch);
		send_to_char("* STR, INT, etc, you were told you could type HELP STATS later.  You can do  *\r\n", ch);
		send_to_char("* that now!  Refer to your newbie book for FREE HEALS.                       *\r\n", ch);
		send_to_char("******************************************************************************\r\n", ch);
	}			/* END OF BIT0 SET */

	GET_LEVEL(ch) = 1;
	GET_EXP(ch) = 1;
	GET_VISIBLE(ch) = 0;
	GET_GOLD(ch) = 5000;
	GET_INN_GOLD(ch) = 0;
	GET_BANK_GOLD(ch) = 0;
	GET_STAT_GOLD(ch) = 0;
	GET_DEATHS(ch) = 0;

	li2100_set_title(ch);

	/* SET BASE DEFAULTS */
	ch->points.max_hit = MAXV(5, races[GET_RACE(ch)].base_hit +
				  ((GET_CON(ch)) * 1.005));
	ch->points.max_mana = MAXV(5, races[GET_RACE(ch)].base_mana +
		       ((GET_INT(ch)) * .05 * races[GET_RACE(ch)].base_mana));
	ch->points.max_move = MAXV(5, races[GET_RACE(ch)].base_move +
		       ((GET_DEX(ch)) * .02 * races[GET_RACE(ch)].base_move));

	/* RESTORE PLAYER TO FULL HEALTH */
	ch->points.hit = li1500_hit_limit(ch);
	ch->points.mana = GET_SPECIAL_MANA(ch);
	ch->points.move = li1600_move_limit(ch);

	for (idx = 1; idx < MAX_SKILLS; idx++) {	/* Zero them out first */
		ch->skills[idx].learned = 0;
	}

	/* ADD POINTS FOR INITIAL LEVEL */
	ch->specials.spells_to_learn = 5;

	/* THIS WILL BE ZEROED NEXT TIME THEY LOG ONTO THE MUD */
	GET_HITROLL(ch) = 20;

	li9850_set_conditions(ch);

	ch->skills[SKILL_PIERCE].learned = 50;
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);
	SET_BIT(GET_ACT3(ch), PLR3_WIMPY);
	SET_BIT(GET_ACT3(ch), PLR3_SHOW_EXITS);
	SET_BIT(GET_ACT3(ch), PLR3_SHOW_HP);
	SET_BIT(GET_ACT3(ch), PLR3_SHOW_MANA);
	SET_BIT(GET_ACT3(ch), PLR3_SHOW_MOVE);
	SET_BIT(GET_ACT3(ch), PLR3_NOSYSTEM);
	SET_BIT(GET_ACT3(ch), PLR3_AUTOAGGR);

	SET_BIT(GET_DSC1(ch), DFLG1_CAN_FINGER);

	GET_COND(ch, FOOD) = -1;
	GET_COND(ch, THIRST) = -1;
	GET_COND(ch, DRUNK) = 0;

	if (IS_SET(lv_flag, BIT0)) {
		for (i = 0; starter_kit[i] != -1; i++) {
			if ((obj = db5100_read_object(starter_kit[i], VIRTUAL))) {
				ha1700_obj_to_char(obj, ch);
			}
		}
		send_to_char("\r\n", ch);
		do_wear(ch, "all\0", CMD_WEAR);
	}			/* END OF BIT0 SET */

	if (gv_auto_level > 1) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Auto adjusting new player %s to level %d.\r\n",
			GET_NAME(ch), (gv_auto_level));
		do_info(buf, IMO_IMM, MAX_LEV, ch);
		li2300_gain_nocheck_exp(ch,
					LEVEL_EXP(GET_LEVEL(ch) + gv_auto_level - 2) - GET_EXP(ch) + 1);
	}

}				/* END OF wi3000_create_level_one_char() */


void wi3100_back_to_level_one(struct char_data * victim, char *arg, int cmd)
{



	db7750_roll_stat_dice(victim, 0);
	wi3000_create_level_one_char(victim, 0);
	li2000_advance_level(victim);
	return;

}				/* END OF wi3100_back_to_level_one() */


int wi3150_return_avatar_level(struct char_data * ch)
{
	int lv_level;

	for (lv_level = 1;
	     lv_level < IMO_SPIRIT && LEVEL_EXP(lv_level) < GET_EXP(ch);
	     lv_level++);
	return (lv_level);

}				/* END OF wi3150_return_avatar_level() */


void wi3200_do_advance(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *victim;
	char name[MAX_STRING_LENGTH];
	char level[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char passwd[MAX_STRING_LENGTH];
	int adv, newlevel, old_exp, new_exp;

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s advances%s\r\n", GET_NAME(ch), arg);
	do_sys(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

	half_chop(arg, name, buf);
	argument_interpreter(buf, level, passwd);

	if (*name) {
		if (!(victim = ha3100_get_char_vis(ch, name))) {
			send_to_char("You can't find that char.\r\n", ch);
			return;
		}
	}
	else {
		send_to_char("Advance whom?\r\n", ch);
		return;
	}

	if (IS_NPC(victim)) {
		send_to_char("NO! Not on NPC's.\r\n", ch);
		return;
	}

	if (GET_LEVEL(victim) == 0)
		adv = 1;
	else if (!*level) {
		send_to_char("You must supply a level number.\r\n", ch);
		return;
	}
	else {
		if (!isdigit(*level)) {
			send_to_char("Second arg must be a positive integer.\r\n", ch);
			return;
		}
	}
	newlevel = atoi(level);

	if (newlevel < 1) {
		send_to_char("1 is the lowest possible level.\r\n", ch);
		return;
	}

	if (newlevel > MAX_LEV) {
		send_to_char("Too high...try again.\r\n", ch);
		return;
	}

	if (newlevel > GET_LEVEL(ch) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("That level's beyond you...\r\n", ch);
		return;
	}

	if (newlevel < GET_LEVEL(victim)) {
		send_to_char("Warning: Lowering a player's level!\r\n", ch);
		wi3100_back_to_level_one(victim, arg, cmd);
	}

	adv = newlevel - GET_LEVEL(victim);

	/* IF THIS IS A NEW CHAR, LEVEL IS 0 */
	if (GET_LEVEL(victim) == 0) {
		wi3000_create_level_one_char(victim, 0);
	}

	send_to_char("You feel generous.\r\n", ch);
	//act("$n makes a strange gesture and you feel slightly different.", FALSE, ch, 0, victim, TO_VICT);

	act("$n makes some strange gestures.\r\nA strange feeling comes upon you,"
	"\r\nLike a giant hand, light comes down from\r\nabove, grabbing your "
	"body, that begins\r\nto pulse with colored lights from inside.\r\nYo"
	"ur head seems to be filled with demons\r\nfrom another plane as your"
	" body dissolves\r\nto the elements of time and space itself.\r\nSudde"
	"nly a silent explosion of light snaps\r\nyou back to reality. You fee"
	    "l slightly\r\ndifferent.", FALSE, ch, 0, victim, TO_VICT);


	old_exp = GET_EXP(victim);
	GET_EXP(victim) = 0;
	new_exp = LEVEL_EXP(GET_LEVEL(victim) + adv - 1) + 1;

	li2300_gain_nocheck_exp(victim, new_exp);

	/* if practices fall below 5, give them some */
	if (ch->specials.spells_to_learn < 5)
		ch->specials.spells_to_learn = 5;

	/* Make new rev's on the zp have max stats for testing purposes */

	if (gv_port == ZONING_PORT && newlevel == 48) {
		GET_MAX_HIT(victim) = 2500;
		GET_MAX_MOVE(victim) = 2500;
		GET_MAX_MANA(victim) = 2500;
	}

	return;
}				/* END OF wi3200_do_advance() */

void wi3300_do_reroll(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *victim;
	char buf[MAX_STRING_LENGTH];
	//struct descriptor_data *q;

	if (IS_NPC(ch))
		return;

	one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Who do you wish to reroll?\r\n", ch);
		return;
	}

	victim = get_char(buf);
	if (!victim) {
		send_to_char("No-one by that name in the world.\r\n", ch);
		return;
	}

	db7750_roll_stat_dice(victim, 0);
	send_to_char("Rerolled... You must exit and re-enter the game for new values to apply.\r\n", victim);
	send_to_char("Rerolled... Char must exit and re-enter the game for new values to apply.\r\n", ch);

	sprintf(buf, "IMM: %s rerolls %s.", ch->player.name, arg);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

	send_to_char("Ok.\r\n", ch);
}				/* END OF wi3300_do_reroll() */


void wi3400_do_restore(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *victim, *next_ch;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	for (; *arg == ' '; arg++);
	one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Who do you wish to restore?\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) >= IMO_IMM &&
	    !strcmp(arg, "DEFAULT")) {
		send_to_char("Clearing spells.\n\rDefaulting to 500000H 5000M 5000V.\n\r", ch);
		ha1370_remove_all_spells(ch);
		GET_MAX_HIT(ch) = 500000;
		GET_HIT(ch) = 500000;
		GET_MAX_MANA(ch) = 5000;
		GET_MANA(ch) = 5000;
		GET_MAX_MOVE(ch) = 5000;
		GET_MOVE(ch) = 5000;
		return;
	}

	if (GET_LEVEL(ch) >= IMO_IMP &&
	    !strcmp(arg, "ALL")) {	/* NOTE: Use arg, not buf - caps */
		for (victim = character_list; victim; victim = next_ch) {
			next_ch = victim->next;
			if (IS_PC(victim) &&
			    GET_LEVEL(victim) < IMO_SPIRIT) {
				wi3450_actually_restore_char(ch, victim);
			}
		}
	}
	else {
		victim = get_char(buf);
		if (!victim) {
			send_to_char("No-one by that name in the world.\r\n", ch);
			return;
		}
		wi3450_actually_restore_char(ch, victim);
	}

	return;

}				/* END OF wi3400_do_restore() */


void wi3450_actually_restore_char(struct char_data * ch, struct char_data * victim)
{

	int i;
	char buf[MAX_STRING_LENGTH];



	if (GET_LEVEL(ch) < IMO_IMM) {
		if (IS_NPC(victim)) {
			send_to_char("You can't restore a mob!\r\n", ch);
			return;
		}
	}

	if (GET_LEVEL(ch) < IMO_IMP) {
		sprintf(buf, "%s level %d restored %s.",
		     GET_REAL_NAME(ch), GET_LEVEL(ch), GET_REAL_NAME(victim));
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}

	/* STUFF FOR EVERYBODY */
	GET_MANA(victim) = GET_SPECIAL_MANA(victim);
	GET_HIT(victim) = GET_HIT_LIMIT(victim);
	GET_MOVE(victim) = GET_MOVE_LIMIT(victim);

	li9850_set_conditions(victim);

	if (IS_PC(victim)) {
		if (GET_LEVEL(victim) > PK_LEV) {
			for (i = 0; i < MAX_SKILLS; i++) {
				if (GET_LEVEL(victim) >= IMO_IMP) {
					victim->skills[i].learned = 100;
				}
				else {
					victim->skills[i].learned = 99;
				}
			}
		}

		if (GET_LEVEL(victim) > PK_LEV) {
			/* victim->points.max_hit = 50000;
			 * victim->points.max_mana = 5000;
			 * victim->points.max_move = 5000; */
			victim->abilities.str = races[GET_RACE(victim)].max_str;
			victim->abilities.intel = races[GET_RACE(victim)].max_int;
			victim->abilities.wis = races[GET_RACE(victim)].max_wis;
			victim->abilities.dex = races[GET_RACE(victim)].max_dex;
			victim->abilities.con = races[GET_RACE(victim)].max_con;
			victim->abilities.cha = races[GET_RACE(victim)].max_cha;
			victim->tmpabilities = victim->abilities;
		}
	}			/* END OF is PC */

	ft1200_update_pos(victim);
	send_to_char("Done.\r\n", ch);
	if (CAN_SEE(victim, ch)) {
		act("You have been fully healed by $N!", TRUE, victim, 0, ch, TO_CHAR);
	}
	return;
}				/* END OF wi3450_actually_restore_char() */


void wi3500_do_noshout(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	one_argument(arg, buf);

	if (!*buf)
		if (IS_SET(GET_ACT3(ch), PLR3_NOSHOUT)) {
			send_to_char("You can now hear shouts again.\r\n", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
		}
		else {
			send_to_char("From now on, you won't hear shouts.\r\n", ch);
			SET_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
		}
	else if (!ha3500_generic_find(arg, FIND_CHAR_WORLD, ch, &vict, &dummy))
		send_to_char("Couldn't find any such creature.\r\n", ch);
	else if (IS_NPC(vict))
		send_to_char("Can't do that to a beast.\r\n", ch);
	else if (GET_LEVEL(vict) > GET_LEVEL(ch))
		act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
	else if (IS_SET(GET_ACT3(vict), PLR3_NOSHOUT)) {
		send_to_char("You can shout again.\r\n", vict);
		send_to_char("NOSHOUT removed.\r\n", ch);
		REMOVE_BIT(GET_ACT3(vict), PLR3_NOSHOUT);
	}
	else {
		send_to_char("The gods take away your ability to shout!\r\n", vict);
		send_to_char("NOSHOUT set.\r\n", ch);
		SET_BIT(GET_ACT3(vict), PLR3_NOSHOUT);
	}
}				/* END OF wi3500_do_noshout() */


int wi3600_do_cmd_blocked_by_freeze(struct char_data * ch, char *arg, int cmd)
{
	if (GET_LEVEL(ch) >= IMO_IMP)
		return (FALSE);

	/* WHAT COMMANDS ARE ALLOWED          */
	/* NOTE:  THIS IS OPPOSIT FROM JAIL   */
	/* IN THAT IT SAYS WHAT YOU    */
	/* CAN DO.                     */
	if (cmd == CMD_LOOK ||
	    cmd == CMD_RENT ||
	    cmd == CMD_SAY ||
	    cmd == CMD_SAVE ||
	    cmd == CMD_SHAKE ||
	    cmd == CMD_NEWS ||
	    cmd == CMD_NOD ||
	    cmd == CMD_WHO ||
	    cmd == CMD_HELP) {
		return (FALSE);
	}

	return (TRUE);

}				/* END OF wi3600_do_cmd_blocked_by_freeze() */


int wi3700_do_cmd_blocked_by_hold_person(struct char_data * ch, char *arg, int cmd)
{
	if (GET_LEVEL(ch) >= IMO_IMP)
		return (FALSE);
	/* WHAT COMMANDS ARE ALLOWED          */
	/* NOTE:  THIS IS OPPOSIT FROM JAIL   */
	/* IN THAT IT SAYS WHAT YOU    */
	/* CAN DO.                     */
	if (cmd == CMD_LOOK ||
	    cmd == CMD_NEWS ||
	    cmd == CMD_HELP ||
	    cmd == CMD_WHO ||
	    cmd == CMD_RETURN ||
	    cmd == CMD_QUIT) {
		return (FALSE);
	}

	return (TRUE);
}				/* END OF
				 * wi3700_do_cmd_blocked_by_hold_person() */


int wi3800_do_cmd_blocked_by_jail(struct char_data * ch, char *arg, int cmd)
{
	if (GET_LEVEL(ch) >= IMO_IMP)
		return (FALSE);

	/* WHAT COMMANDS ARE NOT ALLOWED      */
	/* NOTE:  THIS IS OPPOSIT FROM FREEZE */
	/* IN THAT IT SAYS WHAT YOU    */
	/* CAN DO                      */
	if (cmd == CMD_FORCE ||
	    cmd == CMD_TRANSFER ||
	    cmd == CMD_TITLE ||
	    cmd == CMD_GOTO ||
	    cmd == CMD_LOAD ||
	    cmd == CMD_PURGE ||
	    cmd == CMD_SHUTDOWN ||
	    cmd == CMD_IDEA ||
	    cmd == CMD_TYPO ||
	    cmd == CMD_BUG ||
	    cmd == CMD_AT ||
	    cmd == CMD_SLAP ||
	    cmd == CMD_WHAP ||
	    cmd == CMD_SNOOP ||
	    cmd == CMD_ADVANCE ||
	    cmd == CMD_REROLL ||
	    cmd == CMD_RESTORE ||
	    cmd == CMD_RETURN ||
	    cmd == CMD_SWITCH ||
	    cmd == CMD_RFLAG ||
	    cmd == CMD_INVISIBLE ||
	    cmd == CMD_MUZZLE ||
	    cmd == CMD_BAN ||
	    cmd == CMD_UNBAN ||
	    cmd == CMD_DONATE ||
	    cmd == CMD_GECHO) {
		return (TRUE);
	}

	return (FALSE);

}				/* END OF wi3600_do_cmd_blocked_by_freeze() */


void wi3900_do_muzzle(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	one_argument(arg, buf);

	if (!*buf) {
		send_to_char("Try muzzle <char>.\r\n", ch);
		return;
	}

	if (!ha3500_generic_find(arg, FIND_CHAR_WORLD, ch, &vict, &dummy)) {
		send_to_char("Couldn't find any such creature.\r\n", ch);
		return;
	}

	if (IS_NPC(vict)) {
		send_to_char("Can't do that to a beast.\r\n", ch);
		return;
	}

	if (GET_LEVEL(vict) >= GET_LEVEL(ch)) {
		act("$E might object to that.. better not.",
		    0, ch, 0, vict, TO_CHAR);
		return;
	}

	if (IS_SET(GET_ACT2(vict), PLR2_MUZZLE_ALL) ||
	    (IS_SET(GET_ACT2(vict), PLR2_MUZZLE_SHOUT) &&
	     GET_LEVEL(ch) > IMO_IMM) ||
	    (IS_SET(GET_ACT2(vict), PLR2_MUZZLE_BEEP))) {
		send_to_char("You can speak again.\r\n", vict);
		send_to_char("MUZZLE removed.\r\n", ch);
		sprintf(buf, "MUZZLE: %s has removed %s's muzzle.", GET_REAL_NAME(ch), GET_REAL_NAME(vict));
		spec_log(buf, GOD_COMMAND_LOG);
		REMOVE_BIT(GET_ACT2(vict), PLR2_MUZZLE_ALL);
		if (GET_LEVEL(ch) > IMO_IMM) {
			REMOVE_BIT(GET_ACT2(vict), PLR2_MUZZLE_SHOUT);
		}
		REMOVE_BIT(GET_ACT2(vict), PLR2_MUZZLE_BEEP);
		return;
	}

	send_to_char("The gods take away your ability to speak!\r\n", vict);
	send_to_char("MUZZLE set.\r\n", ch);
	sprintf(buf, "MUZZLE: %s has muzzled %s.", GET_REAL_NAME(ch), GET_REAL_NAME(vict));
	spec_log(buf, GOD_COMMAND_LOG);
	SET_BIT(GET_ACT2(vict), PLR2_MUZZLE_ALL);
	return;

}				/* END OF wi3900_do_muzzle() */


void wi4000_do_poofin(struct char_data * ch, char *arg, int cmd)
{
	int jdx, my_len, loc;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch) && cmd != CMD_ADMIN)
		return;

	for (; *(arg) == ' '; arg++);
	if ((!*arg) && (cmd != CMD_ADMIN)) {
		send_to_char("Usage: poofin <emote_message>\r\n", ch);
		send_to_char("Example: poofin appears in the middle of the room in a puff of smoke.\r\n", ch);
		if ((ch->player.immortal_enter)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Poofin: %s\r\n", ch->player.immortal_enter);
			send_to_char(buf, ch);
		}
		return;
	}

	my_len = strlen(arg);
	loc = 0;
	for (jdx = 0; jdx < my_len; jdx++) {
		if (strncmp((char *) (arg + jdx), "$n", 2) == 0 ||
		    strncmp((char *) (arg + jdx), "$N", 2) == 0) {
			loc = jdx;
			jdx = my_len;
		}
	}

	bzero(buf, sizeof(buf));
	if (loc == 0) {
		sprintf(buf, "%s %s&n", GET_REAL_NAME(ch), arg);
	}
	else {
		strncpy(buf, arg, loc - 1);
		strcat(buf, GET_REAL_NAME(ch));
		strcat(buf, (char *) (arg + loc + 2));
		strcat(buf, "&n");
	}

	/* DON'T LET THEM ENTER "$n" ONLY */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    loc > 0 &&
	    strlen(arg) < 4) {	/* will convert to $$n */
		send_to_char("That poofin message is too short.\r\n", ch);
		return;
	}

	if (strlen(buf) > (87)) {
		send_to_char("That poofin message is too long.\r\n", ch);
		return;
	}

	if (ch->player.immortal_enter) {	/* only free if it exists */
		str_free(ch->player.immortal_enter);
		ha9900_sanity_check(0, "FREE48", "SYSTEM");
	}
	search_replace(buf, "&k", "&K");

	ch->player.immortal_enter = str_alloc(buf);

	if (cmd != CMD_ADMIN) {
		send_to_char("Poofin message changed:\r\n", ch);
		send_to_char(buf, ch);
		send_to_char("\r\n", ch);
	}
}				/* END OF do_poofin() */


void do_poofout(struct char_data * ch, char *arg, int cmd)
{

	int jdx, my_len, loc;
	char buf[MAX_STRING_LENGTH];
	//char buf2[MAX_STRING_LENGTH];



	if (IS_NPC(ch) && cmd != CMD_ADMIN)
		return;

	for (; *(arg) == ' '; arg++);
	if ((!*arg) && (cmd != CMD_ADMIN)) {
		send_to_char("Usage: poofout <emote_message>\r\n", ch);
		send_to_char("Example: poofout disappears in a puff of smoke.\r\n", ch);
		if ((ch->player.immortal_exit)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Poofout: %s\r\n", ch->player.immortal_exit);
			send_to_char(buf, ch);
		}
		return;
	}

	my_len = strlen(arg);
	loc = 0;
	for (jdx = 0; jdx < my_len; jdx++) {
		if (strncmp((char *) (arg + jdx), "$n", 2) == 0 ||
		    strncmp((char *) (arg + jdx), "$N", 2) == 0) {
			loc = jdx;
			jdx = my_len;
		}
	}

	bzero(buf, sizeof(buf));
	if (loc == 0) {
		sprintf(buf, "%s %s&n", GET_REAL_NAME(ch), arg);
	}
	else {
		strncpy(buf, arg, loc - 1);
		strcat(buf, GET_REAL_NAME(ch));
		strcat(buf, (char *) (arg + loc + 2));
		strcat(buf, "&n");
	}

	/* DON'T LET THEM ENTER "$n" ONLY */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    loc > 0 &&
	    strlen(arg) < 4) {	/* will convert to $$n */
		send_to_char("That poofout message is too short.\r\n", ch);
		return;
	}

	if (strlen(buf) > 87) {
		send_to_char("That poofout message is too long.\r\n", ch);
		return;
	}

	if (ch->player.immortal_exit) {
		str_free(ch->player.immortal_exit);
		ha9900_sanity_check(0, "FREE49", "SYSTEM");
	}
	search_replace(buf, "&k", "&K");
	ch->player.immortal_exit = str_alloc(buf);
	if (cmd != CMD_ADMIN) {
		send_to_char("Poofout message changed:\r\n", ch);
		send_to_char(buf, ch);
		send_to_char("\r\n", ch);
	}
	return;

}				/* END OF wi4000_do_poofout() */


void wi4100_do_crashsave(struct char_data * ch, char *arg, int cmd)
{
	struct char_data *vict;
	struct obj_cost cost;
	char buf[MAX_STRING_LENGTH];



	cost.no_carried = 0;
	cost.total_cost = 0;
	cost.ok = TRUE;

	for (vict = character_list; vict; vict = vict->next) {
		if (vict &&
		    IS_PC(vict)) {
			r4000_save_obj(vict, &cost);
			db6400_save_char(vict, NOWHERE);
		}
	}			/* END OF for loop */

	if (ch) {
		send_to_char("All characters saved (even linkless).\r\n", ch);
		sprintf(buf, "CRASHSAVE: %s has Crashsaved.", GET_REAL_NAME(ch));
		spec_log(buf, GOD_COMMAND_LOG);
		spec_log(buf, SYSTEM_LOG);
	}

	return;

}				/* END OF wi4100_do_crashsave() */

struct wiztitle_data *wiztitles = NULL;

/** Find the next unallocated slot in a bit mask.  Returns -1 if no slots are
* available. */
int findunallocatedmask(int mask)
{
	int pos;
	if (mask == -1)
		return -1;
	for (pos = 0; pos < 32; pos++) {
		if (!(mask & 1))
			return pos;
		mask >>= 1;
	}
	return -1;
}
/** Count the number of elements in this mask. */
int countbitsinmask(int mask)
{
	int pos, total = 0;
	if (mask == -1)
		return 32;
	if (mask == 0)
		return 0;
	for (pos = 0; pos < 32; pos++)
		if (mask & (1 << pos))
			total++;
	return total;
}
/** Get the index of the bit'th bit in this bit mask. Returns -1 if the bit
* hasn't got an index. */
int getbitindex(int mask, int bit)
{
	int pos, total = 0;
	if (mask == -1)
		return bit;
	if (mask == 0)
		return -1;
	for (pos = 0; pos < 32; pos++)
		if (mask & (1 << pos)) {
			if (total == bit)
				return pos;
			total++;
		}
	return -1;
}
/** A function to add a wiztitle to an existing char.  Returns false (0) if
* there was not enough space to add the wiztitle. */
int addwiztitle(char *name, char *title2)
{
	int index = 0;
	struct wiztitle_data *title;
	title = getwiztitlestruct(name);
	if (title == NULL) {
		setwiztitle(name, title2);
		return 1;
	}
	index = findunallocatedmask(title->wtmask);
	if (index == -1) {
		/** Tell user they have too many wiztitles to add another. */
		return 0;
	}
	if (strlen(title2) >= 45) {
		memcpy(title->wiztitle[index], title2, 44);
		title->wiztitle[index][44] = 0;
	}
	else
		strcpy(title->wiztitle[index], title2);
	formatwiztitle(title->wiztitle[index]);
	title->wtmask |= (1 << index);
	return -1;
}

int removewiztitle(char *name, int index)
{
	struct wiztitle_data *title, *last;
	title = getwiztitlestruct(name);
	if (title == NULL)
		return 0;
	if (index >= 31)
		return 0;
	if (index < 0)
		return 0;
	index = getbitindex(title->wtmask, index);
	if (index == -1)
		return 0;
	title->wtmask &= (-1 - (1 << index));
	if (title->wtmask == 0) {
		for (last = wiztitles; (last != NULL) && (last->next != title);
		     last = last->next);
		if (last == NULL) {
			if (title == wiztitles) {
				wiztitles = title->next;
			}
		}
		else
			last->next = title->next;
		free(title);
	}
	return -1;
}

struct wiztitle_data *getwiztitlestruct(char *name)
{
	struct wiztitle_data *title;
	for (title = wiztitles; title != NULL; title = title->next)
		if (!strcasecmp(title->name, name))
			break;
	return title;
}

int wiztitlecount(char *name)
{
	int pos;
	struct wiztitle_data *title = getwiztitlestruct(name);
	if (!title)
		return 0;
	return countbitsinmask(title->wtmask);
	return pos;
}

char *getwiztitleindex(char *name, int index)
{
	int ind;
	struct wiztitle_data *title = getwiztitlestruct(name);
	if (!title)
		return NULL;
	ind = getbitindex(title->wtmask, index);
	if (ind < 0)
		return NULL;
	return title->wiztitle[ind];
}
/** A function to set a wiztitle on the linked list. */
int setwiztitle(char *name, char *title2)
{
	struct wiztitle_data *title;
	title = getwiztitlestruct(name);
	if (!title) {
		title = (struct wiztitle_data *) malloc(sizeof(struct wiztitle_data));
		title->next = wiztitles;
		wiztitles = title;
	}
	if (strlen(name) >= 20) {
		memcpy(title->name, name, 19);
		title->name[19] = 0;
	}
	else
		strcpy(title->name, name);
	if (strlen(title2) >= 45) {
		memcpy(title->wiztitle[0], title2, 44);
		title->wiztitle[0][44] = 0;
	}
	else
		strcpy(title->wiztitle[0], title2);
	title->wtmask = (1 << 0);
	formatwiztitle(title->wiztitle[0]);
	return -1;
}
 /** A function to clear all of the wiztitles. */
void deletewiztitles()
{
	struct wiztitle_data *next;
	struct wiztitle_data *current = wiztitles;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	wiztitles = NULL;
}
/** Find the wiztitle for the character name specified.  Returns NULL if the
* character specified doesnt have a wiztitle. Will return one of the random
* wiztitles, if this character has multiple. */
char *getwiztitle(char *name)
{
	int len, index;
	struct wiztitle_data *title = getwiztitlestruct(name);
	if (title == NULL)
		return NULL;
	len = wiztitlecount(name);
	index = (int) ((rand() & 0xffff) * len) >> 16;
	index = getbitindex(title->wtmask, index);
	return title->wiztitle[index];
}

void loadwiztitles()
{
	char *endline, *endname, *line, buffer[32768];
	char oldend;
	int len;
	FILE *wtfile;
	wtfile = fopen(wiztitle_file_name, "r");
	if (!wtfile)
		return;
	len = fread(buffer, 1, 32767, wtfile);
	buffer[len] = 0;
	fclose(wtfile);
	line = buffer;
	if (wiztitles)
		deletewiztitles();
	while (line && (*line)) {
		/** Find the first space, the end of the name part. */
		for (endname = line; !isspace(*endname) && (*endname != 0); endname++);
		*(endname++) = 0;
		for (;;) {
			/** Find a newline, tab or NULL character, that marks the end of
	    * a wiztitle section. */
			for (endline = endname; (*endline != 9) && (*endline != 13)
			   && (*endline != 10) && (*endline != 0); endline++);
			/** NULL terminate name and wiztitle strings. */
			oldend = *endline;
			*endline = 0;
			/** Check the name and wiztitle strings are not too long. */
			addwiztitle(line, endname);
			/** Set the line to the start of the newline. */
			*endline = oldend;
			for (; ((*endline == 9) || (*endline == 10) || (*endline == 13))
			     && (*endline != 0); endline++);
			endname = endline;
			if (oldend != 9)
				break;
		}
		line = endline;
	}
}

int savewiztitles()
{
	int index, first;
	FILE *wtfile;
	struct wiztitle_data *title;
	wtfile = fopen(wiztitle_file_name, "w");
	title = wiztitles;
	if (!wtfile) {
		/* Better tell people it didnt work. */
		return 0;
	}
	while (title) {
		first = -1;
		fprintf(wtfile, "%s ", title->name);
		for (index = 0; index < 32; index++)
			if (title->wtmask & (1 << index)) {
				if (!first)
					fprintf(wtfile, "\t");
				fprintf(wtfile, "%s", title->wiztitle[index]);
				first = 0;
			}
		fprintf(wtfile, "\n");
		title = title->next;
	}
	fclose(wtfile);
	return -1;
}

char wiztitleANSIcodes[256];

void setupwiztitles()
{
	memset(wiztitleANSIcodes, 2, 255);
	wiztitleANSIcodes['r'] = 0;
	wiztitleANSIcodes['g'] = 0;
	wiztitleANSIcodes['y'] = 0;
	wiztitleANSIcodes['b'] = 0;
	wiztitleANSIcodes['p'] = 0;
	wiztitleANSIcodes['c'] = 0;
	wiztitleANSIcodes['w'] = 0;
	wiztitleANSIcodes['K'] = 0;
	wiztitleANSIcodes['R'] = 0;
	wiztitleANSIcodes['G'] = 0;
	wiztitleANSIcodes['Y'] = 0;
	wiztitleANSIcodes['B'] = 0;
	wiztitleANSIcodes['P'] = 0;
	wiztitleANSIcodes['C'] = 0;
	wiztitleANSIcodes['W'] = 0;
	wiztitleANSIcodes['0'] = 0;
	wiztitleANSIcodes['1'] = 0;
	wiztitleANSIcodes['2'] = 0;
	wiztitleANSIcodes['3'] = 0;
	wiztitleANSIcodes['4'] = 0;
	wiztitleANSIcodes['5'] = 0;
	wiztitleANSIcodes['6'] = 0;
	wiztitleANSIcodes['7'] = 0;
	wiztitleANSIcodes['s'] = 0;
	wiztitleANSIcodes['i'] = 0;
	wiztitleANSIcodes['x'] = 0;
	wiztitleANSIcodes['X'] = 0;
	wiztitleANSIcodes['S'] = 0;
	wiztitleANSIcodes['f'] = 0;
	wiztitleANSIcodes['u'] = 0;
	wiztitleANSIcodes['v'] = 0;
	wiztitleANSIcodes['n'] = 0;
	wiztitleANSIcodes['E'] = 0;
	wiztitleANSIcodes['&'] = 1;
	wiztitleANSIcodes[0] = 1;
}


int clrstrlen(char *title)
{
	int len = 0;
	while (*title != 0) {
		if (*title != '&')
			len++;
		else {
			title++;
			len += wiztitleANSIcodes[(unsigned char) *title];
			if (*title == 0)
				return len;
		}
		title++;
	}
	return len;
}

void cropclrstrlen(char *title, int len)
{
	while (*title != 0) {
		if (*title != '&') {
			len--;
			if (len < 0)
				*title = 0;
		}
		else {
			title++;
			len -= wiztitleANSIcodes[(unsigned char) *title];
			if (len < 0)
				*(title - (len + 1)) = 0;
			if (*title == 0)
				return;
		}
		title++;
	}
	return;
}

void formatwiztitle(char *title)
{
	int alen, left, right;
	int length = clrstrlen(title);
	if (length > 14)
		cropclrstrlen(title, 14);
	else if (length < 14) {
		alen = strlen(title);
		left = ((14 - length) >> 1) + 1;
		right = (14 - length) - left;
		memmove(title + left, title, alen);
		memset(title, '-', left);
		memset(title + left + alen, '-', right);
		title[0] = '[';
		title[left + right + alen] = 0;
	}
	return;
}
