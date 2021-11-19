/* blah */
/* ot */
/* gv_location: 12001-12500 */
/* ********************************************************************
*  file: other.c , Implementation of commands.      Part of DIKUMUD *
*  Usage : Other commands.                                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "globals.h"
#include "func.h"

void ot1100_do_visible(struct char_data * ch, char *argument, int cmd)
{



	if (ha1375_affected_by_spell(ch, SPELL_INVISIBLE))
		ha1350_affect_from_char(ch, SPELL_INVISIBLE);
	if (ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS))
		ha1350_affect_from_char(ch, SPELL_IMPROVED_INVIS);

	REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
	send_to_char("You remove all invisibility spells from yourself", ch);
	act("$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);
}				/* END OF ot1100_do_visible() */


/* should guard against cursed items */
void ot1200_do_junk(struct char_data * ch, char *argument, int cmd)
{
	struct obj_data *tmp_obj;
	struct obj_data *j;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int did_we_find_all_dot;
	int number_items;
	int coin_award;
	size_t size;



	/* DON'T ALLOW ON HELL NIGHT */
	if (gv_auto_level > 1) {
		send_to_char("You can't junk while auto leveling is on!\r\n", ch);
		return;
	}

	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;

	argument = one_argument(argument, arg);

	if (!(*arg)) {
		send_to_char("Junk! Fine! But junk what?\n\r", ch);
		return;
	}

	coin_award = 0;
	number_items = 0;
	while (1) {

		tmp_obj = ha2075_get_obj_list_vis(ch, arg, ch->carrying);

		if (!tmp_obj) {
			if (number_items == 0) {
				act("You don't have anything like that.", FALSE, ch, 0, 0, TO_CHAR);
				return;
			}
			break;
		}

		coin_award += (tmp_obj->obj_flags.cost) / 100;

		/* LOG CORPSE JUNKING */
		/* if (tmp_obj->obj_flags.value[3] == 1) { bzero(buf,
		 * sizeof(buf)); sprintf(buf, "INFO: %s junked: %s",
		 * GET_REAL_NAME(ch), (tmp_obj)->short_description ?
		 * (tmp_obj)->short_description  : "a corpse"); main_log(buf);
		 * } */
		if (tmp_obj->obj_flags.value[3] != 1) {
			sprintf(buf, "(obj) do_junk %s - %s", GET_REAL_NAME(ch), OBJS(tmp_obj, ch));
			main_log(buf);
			spec_log(buf, EQUIPMENT_LOG);
			if ((GET_ITEM_TYPE(tmp_obj) == ITEM_CONTAINER) ||
			    (GET_ITEM_TYPE(tmp_obj) == ITEM_QSTCONT))
				do_wizinfo(buf, IMO_IMM, ch);
		}
		else {
			sprintf(buf, "(obj) (junkcorpse) do_junk %s - %s", GET_REAL_NAME(ch), OBJS(tmp_obj, ch));
			/****************************************************************************************
			*This needs to stay commented out until some possible buffer overflows are fixed.
			*One is here (and in the next commented block), others in do_stat in either wiz1.c or
			*wiz2.c, and the other (and this is the big one) is the color parsing code in comm.c
			*
			*Good luck!    Relic
			****************************************************************************************/
			strcpy(buf, "Contained :\n                            ");
			size = strlen(buf);
			for (j = tmp_obj->contains; j; j = j->next_content) {
				size = size + strlen(j->short_description) + strlen("\r\n                            ");
				strcat(buf, j->short_description);
				strcat(buf, "\r\n                            ");
				if (size >= sizeof(buf)) {
					strcpy(buf + (sizeof(buf) - 10) - strlen(gv_overflow_buf), gv_overflow_buf);
					/* 10 for stuff we strcat later.(Hack) */
					break;
				}
			}
		}
		main_log(buf);
		spec_log(buf, EQUIPMENT_LOG);
		do_wizinfo(buf, IMO_IMM, ch);

		if ((GET_ITEM_TYPE(tmp_obj) == ITEM_CONTAINER) ||
		    (GET_ITEM_TYPE(tmp_obj) == ITEM_QSTCONT)) {
			strcpy(buf, "Contained :\n                            ");
			size = strlen(buf);
			for (j = tmp_obj->contains; j; j = j->next_content) {
				size = size + strlen(j->short_description) + strlen("\r\n                            ");
				strcat(buf, j->short_description);
				strcat(buf, "\r\n                            ");
				if (size >= sizeof(buf)) {
					strcpy(buf + (sizeof(buf) - 20) - strlen(gv_overflow_buf), gv_overflow_buf);
					/* 20 for stuff we strcat later.(Hack) */
					break;
				}
			}
			main_log(buf);
			spec_log(buf, EQUIPMENT_LOG);
			do_wizinfo(buf, IMO_IMM, ch);
		} //end is container
		/* REMOVE OBJECT FROM INVENTORY */

			ha2700_extract_obj(tmp_obj);
		number_items++;

		/* IF WE DIDN'T HAVE "ALL." LETS GET OUT AFTER DOING ONE ITEM */
		if (did_we_find_all_dot != TRUE)
			break;

	}
	if (number_items > 1) {
		sprintf(arg, "You junk %d items and are rewarded %d coins!.\n\r",
			number_items, coin_award);
		send_to_char(arg, ch);
		act("$n junks several items.", TRUE, ch, 0, 0, TO_ROOM);
	}
	else {
		sprintf(arg, "You are rewarded %d coins!.\n\r", coin_award);
		send_to_char(arg, ch);
		act("$n junks an item.", TRUE, ch, 0, 0, TO_ROOM);
	}
	if ((MAX_GOLD - GET_GOLD(ch) - coin_award) < 0) {
		send_to_char("ACK, you can't hold all the reward, and drop some!\n\r", ch);
		GET_GOLD(ch) = MAX_GOLD;
	}
	else
		GET_GOLD(ch) += coin_award;

}				/* END OF ot1200_do_junk() */

void ot1300_do_quit(struct char_data * ch, char *argument, int cmd)
{

	int rc;
	int idx, lv_player_has_equip;
	char temp_str[MAX_STRING_LENGTH];




	if (auction_data.obj && auction_data.bidder && ((ch == auction_data.bidder->current) || (ch == auction_data.auctioner))) {
		send_to_char("Can't quit while your bid is standing.\n\rPlease wait (max. 60 seconds).\n\r", ch);
		return;
	}

	if (IS_NPC(ch)) {
		send_to_char("Sorry, mobs are committed to this mud.\r\n",
			     ch);
		return;
	}

	if (!ch->desc) {
		send_to_char("You can't quit, you don't have a desc!\r\n",
			     ch);
		return;
	}

	if (GET_POS(ch) == POSITION_FIGHTING) {
		send_to_char("No way! You are fighting.\n\r", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_HOLD_PERSON)) {
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't quit, you're imitating stone!\r\n", ch);
			return;
		}
	}

	if (GET_POS(ch) < POSITION_STUNNED) {
		send_to_char("You die before your time!\n\r", ch);
		ft2000_die(ch, ch);
		return;
	}

	/* if (GET_LEVEL(ch) < IMO_IMM && (IS_SET(GET_ROOM2(ch->in_room),
	 * RM2_NO_QUIT) || IS_SET(GET_ROOM1(ch->in_room), RM1_NO_TELEPORT_IN)
	 * || IS_SET(GET_ROOM1(ch->in_room), RM1_NO_TELEPORT_OUT))) {
	 * send_to_char("You can't quit from this room!\n\r", ch); return; } */
	rc = li3000_is_blocked_from_room(ch, ch->in_room, BIT0);
	if (rc && ch->in_room != JAIL_ROOM) {
		send_to_char("You can't quit from this room!\n\r", ch);
		return;
	}
	if ((GET_LEVEL(ch) <= IMO_LEV) && (GET_LEVEL(ch) >= gv_pkill_level) &&
	    (world[ch->in_room].number != 3008)) {
		send_to_char("Because of the current PK Level you can only quit in the reception.\r\n", ch);
		return;
	}

	if ((ch->desc->last_target) && (time(0) - ch->desc->last_hit_time < 300) && (GET_LEVEL(ch) <= PK_LEV) &&
	    (world[ch->in_room].number != 3008)) {
		send_to_char("No way! You can't quit unless you finish what you started.\r\n", ch);
		return;
	}

	/* cancel auction if player was auctioning */
	if (auction_data.auctioner == ch)
		au1400_do_cancel(ch, argument, 0);
	if (auction_data.bidder)
		au1800_remove_bidder(ch, 0);

	/* cancel quest if there is one */
	if (ch->countdown) {
		qu1400_clear_quest(ch);
		ch->nextquest = time(0) + 5 * 60;
	}

	r7300_do_quit(ch, argument, cmd);
	return;

	/* ignore the rest, not needed anymore */

	/* IF THIS IS AN IMM WITHOUT EQUIPMENT, DON'T CONFIRM */
	lv_player_has_equip = FALSE;
	for (idx = 0; idx < MAX_WEAR; idx++) {
		if (ch->equipment[idx]) {
			lv_player_has_equip = TRUE;
		}
	}

	if (lv_player_has_equip == TRUE ||
	    (ch->carrying) ||
	    GET_LEVEL(ch) < IMO_LEV) {
		send_to_char("Warning!: By quitting the game you forfeit your equipment.\r\n", ch);
		send_to_char("          To keep your equipment, RENT out at the Inn.\r\n", ch);
		send_to_char("Do you wish to quit? ", ch);
		ch->desc->connected = CON_QUIT;
		return;
	}


	if (RIDING(ch)) {
		send_to_char("Your rider has left you :(.\r\n", RIDING(ch));
		RIDDEN_BY(RIDING(ch)) = NULL;
		RIDING(ch) = NULL;
	}

	if (RIDDEN_BY(ch)) {
		send_to_char("Your mount has left you.\r\n", RIDDEN_BY(ch));
		RIDING(RIDDEN_BY(ch)) = NULL;
		RIDDEN_BY(ch) = NULL;
	}


	act("Goodbye, friend.. Come back soon!", FALSE, ch, 0, 0, TO_CHAR);
	act("$n has left the game.", TRUE, ch, 0, 0, TO_ROOM);

	bzero(temp_str, sizeof(temp_str));
	sprintf(temp_str, "Player: %s has quit.", GET_NAME(ch));
	do_connect(temp_str, GET_LEVEL(ch), ch);

	if (IS_SET(GET_ACT2(ch), PLR2_QUEST))
		REMOVE_BIT(GET_ACT2(ch), PLR2_QUEST);

	GET_HOW_LEFT(ch) = LEFT_BY_COMMAND_QUIT;
	if (gv_mega_verbose_messages == TRUE) {
		main_log("Leaving by quit immortal");
	}
	/* Char is saved in extract char */
	ha3000_extract_char(ch, END_EXTRACT_BY_COMMAND_QUIT);
	r2300_del_char_objs(ch);

}				/* END OF ot1300_do_quit() */


void do_qui(struct char_data * ch, char *argument, int cmd)
{



	send_to_char("You have to write quit - no less, to quit!\n\r", ch);
	return;
}				/* END OF do_qui() */


void ot1400_do_save(struct char_data * ch, char *argument, int cmd)
{
	struct obj_cost cost;
	char buf[MAX_STRING_LENGTH];



	if (IS_NPC(ch) || !ch->desc)
		return;

	send_to_char("Saving player data.\n\r", ch);
	db6400_save_char(ch, NOWHERE);
	send_to_char("Saving equipment.\n\r", ch);
	cost.no_carried = 0;
	cost.total_cost = 0;
	cost.ok = TRUE;
	r4000_save_obj(ch, &cost);
	sprintf(buf, "%s saved equipment.", GET_REAL_NAME(ch));
	main_log(buf);

}				/* END OF ot1400_do_save() */


void ot1500_do_not_here(struct char_data * ch, char *argument, int cmd)
{



	send_to_char("You can not do that here.\n\r", ch);
}				/* END OF ot1500_do_not_here() */


void ot1600_do_sneak(struct char_data * ch, char *argument, int cmd)
{
	struct affected_type af;
	signed char percent;



	if (IS_AFFECTED(ch, AFF_SNEAK)) {
		send_to_char("You are already being a big sneak.\n\r", ch);
		return;
	}
	else
		send_to_char("Ok, you'll try to move silently for a while.\n\r", ch);

	percent = number(1, 101);	/* 101% is a complete failure */

	if (percent > (ch->skills[SKILL_SNEAK].learned +
		       li9650_dex_adjustment(ch, 1)))
		return;

	af.type = SKILL_SNEAK;
	af.duration = GET_LEVEL(ch);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_SNEAK;
	ha1300_affect_to_char(ch, &af);

	li9900_gain_proficiency(ch, SKILL_SNEAK);
	return;
}				/* END OF ot1600_do_sneak() */


void ot1700_do_title(struct char_data * ch, char *argument, int cmd)
{

	int idx, my_len, loc;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH],
	 *lv_ptr;

	const char *lv_words[] = {
		"<arena>",
		"<quest>",
		"<killer>",
		"<pkiller>",
		"<bully_pkiller>",
		"<avatar_pkiller>",
		"<enforcer>",
		"<freeze>",
		"<jailed>",
		"<linkdead>",
		"<linkless>",
		"(linkdead)",
		"<pk>",
		"<afk>",
		"*arch*",
		"*meth*",
		"*ante*",
		"*high*",
		"*over*",
		"*imp*",
		"*god*",
		"<afw>",
		"\n",
	};



	if (IS_NPC(ch)) {
		send_to_char("Mobs don't have titles\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT)
		search_replace(argument, "&k", "&K");
	for (; *argument == ' '; argument++);
	if (!*argument) {
		send_to_char("Okay now what was that you wanted for a title?\r\nCurrent: ", ch);
		strcpy(buf, GET_TITLE(ch));
		send_to_char(buf, ch);
		return;
	}

	my_len = strlen(argument);
	loc = 0;
	for (idx = 0; idx < my_len; idx++)
		if (strncmp((char *) (argument + idx), "$n", 2) == 0 ||
		    strncmp((char *) (argument + idx), "$N", 2) == 0) {
			loc = idx;
			idx = my_len;
		}

	bzero(buf, sizeof(buf));
	if (loc == 0) {
		sprintf(buf, "%s %s&n", GET_NAME(ch), argument);
	}
	else {
		strncpy(buf, argument, loc - 1);
		strcat(buf, GET_NAME(ch));
		strcat(buf, &argument[loc + 2]);
		strcat(buf, "&n");
	}			/* END OF else we have a $n in argument */

	strcpy(buf2, buf);
	strcpy(buf3, buf);

	if ((strlen(buf2) > 55) || (strlen(buf) > 70)) {
		send_to_char("I'm afraid thats a bit lengthy.\n\r", ch);
		return;
	}

	lv_ptr = ha1000_is_there_a_reserved_word(buf2, lv_words, BIT0);
	if (lv_ptr) {
		sprintf(buf, "Sorry, but you can't have %s in your title.\r\n",
			lv_ptr);
		send_to_char(buf, ch);
		return;
	}

	lv_ptr = ha1000_is_there_a_reserved_word(buf2, curse_words, BIT0);
	if (lv_ptr) {
		sprintf(buf, "Sorry, but you can't have %s in your title.\r\n",
			lv_ptr);
		send_to_char(buf, ch);
		return;
	}

	if (GET_TITLE(ch))
		RECREATE(GET_TITLE(ch), char, strlen(buf3) + 1);
	else
		CREATE(GET_TITLE(ch), char, strlen(buf3) + 1);

	strcpy(GET_TITLE(ch), buf3);

	bzero(buf, sizeof(buf));
	co2900_send_to_char(ch, "Ok, you are now: %s\r\n", GET_TITLE(ch));


}				/* END OF ot1700_do_title */

void ot1800_do_hide(struct char_data * ch, char *argument, int cmd)
{

	signed char percent;



	send_to_char("You attempt to hide yourself.\n\r", ch);

	if (IS_AFFECTED(ch, AFF_HIDE))
		REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);

	percent = number(1, 101);	/* 101% is a complete failure */

	if (percent > (ch->skills[SKILL_HIDE].learned +
		       li9650_dex_adjustment(ch, 2)))
		return;

	SET_BIT(ch->specials.affected_by, AFF_HIDE);

	li9900_gain_proficiency(ch, SKILL_HIDE);
	return;
}				/* END OF ot1800_do_hide() */

void ot1850_do_envenom(struct char_data * ch, char *argument, int cmd)
{

	struct obj_data *obj;
	signed char percent;
	int i, did_we_find_all_dot;
	char arg[MAX_INPUT_LENGTH];

	/* REMOVE "ALL." IF ATTACHED */
	for (; isspace(*argument); argument++);
	did_we_find_all_dot = remove_ALL_dot(argument);
	if (did_we_find_all_dot == TRUE)
		argument += 4;

	argument = one_argument(argument, arg);

	if (!arg) {
		send_to_char("We need something to envenom.\n\r", ch);
		return;
	}

	percent = number(1, 101);	/* 101% is a complete failure */

	if (percent > (ch->skills[SKILL_ENVENOM_WEAPON].learned +
		       li9650_dex_adjustment(ch, 1)))
		return;

	while (1) {

		obj = ha2075_get_obj_list_vis(ch, arg, ch->carrying);

		if (!obj) {
			act("You don't have anything like that.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		if (GET_ITEM_TYPE(obj) != ITEM_WEAPON) {
			send_to_char("You can only envenom weapons.\r\n", ch);
			return;
		}
		/* IF WEAPON HAS ANY EXISTING APPLYs, GET OUT */
/*		for (i = 0; i < MAX_OBJ_AFFECT; i++) {
			if (obj->affected[i].location != APPLY_NONE) {
				send_to_char("The weapon cannot be poisoned!\r\n", ch);
				return;
			}
		}*/
		if (IS_SET(obj->obj_flags.value[0], WEAPON_POISONOUS) || (IS_SET(obj->obj_flags.value[0], WEAPON_ACID_ATTACK))) {
			send_to_char("You fail to make the item more poisonous.\r\n", ch);
			return;
		}
		if (percent <= (ch->skills[SKILL_ENVENOM_WEAPON].learned +
				li9650_dex_adjustment(ch, 1))) {
			SET_BIT(obj->obj_flags.value[0], WEAPON_ACID_ATTACK);
			act("$p is coated in a sickly green coating.", TRUE, ch, obj, 0, TO_CHAR);
			act("$n's $p is coated in a sickly green coating.", TRUE, ch, obj, 0, TO_ROOM);
			li9900_gain_proficiency(ch, SKILL_ENVENOM_WEAPON);
			return;
		}
		else
			send_to_char("You fail to envenom the weapon.\r\n", ch);
		return;

	}
}				/* END OF skill_envenom_weapon() */

void ot1900_do_steal(struct char_data * ch, char *argument, int cmd)
{

	struct char_data *victim;
	struct obj_data *tmp_object;
	struct obj_data *obj;
	char victim_name[240], obj_name[240], buf[MAX_STRING_LENGTH];

	int percent, gold, eq_pos, lv_save_invis;
	bool ohoh = FALSE;



	/* IS ROOM NO_STEAL? */
	if (IS_SET(world[ch->in_room].room_flags, RM1_NOSTEAL) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("A strange force stops you from doing that here.\r\n", ch);
		return;
	}

	if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM)) {
		send_to_char("Sorry, charmies can't do that.\r\n", ch);
		return;
	}

	bzero(obj_name, sizeof(obj_name));
	bzero(victim_name, sizeof(victim_name));
	argument = one_argument(argument, obj_name);
	one_argument(argument, victim_name);

	if (!(*victim_name)) {
		send_to_char("Who do you want to rip off?\n\r", ch);
		return;
	}

	if (!(victim = ha2125_get_char_in_room_vis(ch, victim_name))) {
		send_to_char("Steal what from who?\n\r", ch);
		return;
	}
	else if (victim == ch) {
		send_to_char("You stealthily slip a coin from your pocket.\n\r", ch);
		return;
	}

	/* OK TO PERFORM AGGRESSIVE ACT? */
	if (GET_LEVEL(ch) < IMO_IMM &&
	    IS_NPC(victim)) {
		CHECK_FOR_NO_FIGHTING();
		IS_ROOM_NO_KILL();
	}

	/* IF THIS ISN'T AN IMP, PRINT A MESSAGE TO THE ROOM */
	bzero(buf, sizeof(buf));
	sprintf(buf, "%s entered: steal %s %s\r\n",
		GET_REAL_NAME(ch), obj_name, victim_name);
	lv_save_invis = GET_VISIBLE(ch);
	spec_log(buf, STEAL_LOG);
	if (GET_LEVEL(ch) < IMO_IMP) {
		GET_VISIBLE(ch) = IMO_IMM;
		act(buf, TRUE, ch, 0, victim, TO_ROOM);
		GET_VISIBLE(ch) = lv_save_invis;
	}
	else if (GET_LEVEL(victim) == IMO_IMP) {
		send_to_char(buf, victim);
	}

	/* IF VICTIM IS LINKDEAD - FORGET IT */
	if (IS_PC(victim) &&
	    !victim->desc &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("You keep that up and you'll pay for it!\r\n", ch);
		return;
	}



	percent = number(1, 101) -
		(li9650_dex_adjustment(ch, 3) - GET_LEVEL(victim));

	if (GET_LEVEL(ch) < IMO_IMP && IS_PC(victim)) {
		if (!victim->desc) {
			percent = 101;
		}
		else {
			if (victim->desc->connected == CON_LINK_DEAD) {
				percent = 101;
			}
		}
	}
	if ((IS_PC(ch)) && (IS_PC(victim))) {
		send_to_char("You can't steal from other players.\r\n", ch);
		return;
	}

	if (IS_SET(victim->specials.act, PLR1_NOSTEAL) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("Your stealing attempt has been deftly blocked.\r\n", ch);
		return;
	}

	if (GET_POS(victim) < POSITION_SLEEPING) {
		percent = -1;	/* ALWAYS SUCCESS */
	}

	/* A MORTAL WILL NEVER STEAL FROM AN IMMORTAL */
	if (GET_LEVEL(ch) < IMO_SPIRIT) {
		if (GET_LEVEL(victim) > PK_LEV) {
			percent = 101;
		}
	}

	if (GET_LEVEL(ch) > IMO_IMM)
		percent = -1;

	/* 101% is a complete failure, -1 is allways a success */
	if (str_cmp(obj_name, "coins") && str_cmp(obj_name, "gold")) {

		if (!(obj = ha2075_get_obj_list_vis(victim, obj_name, victim->carrying))) {

			for (eq_pos = 0; (eq_pos < MAX_WEAR); eq_pos++)
				if (victim->equipment[eq_pos] &&
				    (ha1150_isname(obj_name, victim->equipment[eq_pos]->name)) &&
				 CAN_SEE_OBJ(ch, victim->equipment[eq_pos])) {
					obj = victim->equipment[eq_pos];
					break;
				}

			if (!obj) {
				act("$E has not got that item.", FALSE, ch, 0, victim, TO_CHAR);
				return;
			}
			else {	/* It is equipment */
				if ((GET_POS(victim) > POSITION_STUNNED) &&
				    GET_LEVEL(ch) < IMO_IMP) {
					send_to_char("Steal the equipment now? Impossible!\n\r", ch);
					return;
				}
				else {
					if (IS_PERSONAL(obj) && GET_LEVEL(ch) < IMO_IMP) {
						send_to_char("No way you'll be able to steal that!\r\n", ch);
						return;
					}
					if (check_nodrop(obj)) {
						send_to_char("You can't steal that item.\r\n", ch);
						return;
					}

					if (check_lore(ch, obj)) {
						send_to_char("You don't feel like stealing that, you already have one.\r\n", ch);
						return;
					}
					act("You unequip $p and take possession of it.", FALSE, ch, obj, 0, TO_CHAR);
					if (GET_LEVEL(ch) < IMO_IMP)
						act("$n steals $p from $N.", FALSE, ch, obj, victim, TO_NOTVICT);
					ha1700_obj_to_char(ha1930_unequip_char(victim, eq_pos), ch);
				}
			}
		}
		else {		/* obj found in inventory */

			percent += GET_OBJ_WEIGHT(obj);	/* Make heavy harder */
			if (IS_PERSONAL(obj) && GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("No way you'll be able to steal that!\r\n", ch);
				return;
			}

			if (GET_LEVEL(ch) < IMO_IMP && AWAKE(victim) && (percent > ch->skills[SKILL_STEAL].learned)) {
				ohoh = TRUE;
				act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
				act("$n tried to steal something from you!", FALSE, ch, 0, victim, TO_VICT);
				act("$n tries to steal something from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
			}
			else {	/* Steal the item */
				if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
					if (check_nodrop(obj)) {
						send_to_char("You can't steal that item.\r\n", ch);
						return;
					}

					if (check_lore(ch, obj)) {
						send_to_char("You don't feel like stealing that, you already have one.\r\n", ch);
						return;
					}

					if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) < CAN_CARRY_W(ch)) {
						ha1800_obj_from_char(obj);
						ha1700_obj_to_char(obj, ch);
						send_to_char("Got it!\n\r", ch);
						li9900_gain_proficiency(ch, SKILL_STEAL);
					}
				}
				else
					send_to_char("You cannot carry that much.\n\r", ch);
			}
		}
	}
	else {			/* Steal some coins */
		if (GET_LEVEL(ch) < IMO_IMP && AWAKE(victim) && (percent > ch->skills[SKILL_STEAL].learned)) {
			ohoh = TRUE;
			act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
			act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, victim, TO_VICT);
			act("$n tries to steal gold from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
		}
		else {
			/* Steal some gold coins */
			gold = (int) ((GET_GOLD(victim) * number(1, 10)) / 100);
			gold = MINV(2000, gold);
			if ((MAX_GOLD - GET_GOLD(ch) - gold) < 0) {
				tmp_object = ha3400_create_money(gold);
				ha2100_obj_to_room(tmp_object, ch->in_room);
				act("Oops...You couldn't hold that many coins and dropped your loot.", FALSE, ch, 0, 0, TO_CHAR);
				act("You hear the tink of coins hitting the ground, while $n looks really nervous.\r\nHey!  Your wallet feels a little lighter!",
				    FALSE, ch, 0, victim, TO_VICT);
				act("Coins hit the ground.\r\n$n looks kinda nervous as $N looks for his wallet",
				    TRUE, ch, 0, victim, TO_NOTVICT);
				return;
			}
			if (gold > 0) {
				GET_GOLD(ch) += gold;
				GET_GOLD(victim) -= gold;
				sprintf(buf, "Bingo! You got %d gold coins.\n\r", gold);
				send_to_char(buf, ch);
				li9900_gain_proficiency(ch, SKILL_STEAL);
			}
			else {
				send_to_char("No luck...\n\r", ch);
			}
		}
	}

	if (GET_LEVEL(ch) < IMO_SPIRIT &&
	    IS_PC(victim)) {
		WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
		WAIT_STATE(ch, PULSE_VIOLENCE);
	}
	return;

}				/* END OF ot1900_do_steal() */


 /* NOTE:  If we are with a guild master, the mobs function will execute the
  * pr1400_do_guild() function */
void ot2000_do_practice(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *victim;
	int tmp;



	for (; isspace(*arg); arg++);

	/* IF USER WANTS TO PRACTICE SOMETHING, AND WE ARE HERE, */
	/* IT MEANS A GUILD MASTER ISN'T...                      */
	if (*(arg)) {
		if (GET_LEVEL(ch) < IMO_SPIRIT) {
			send_to_char("You need to find a guildmaster to practice that.\r\n", ch);
			return;
		}
		victim = 0;
		if (cmd == CMD_ADMIN)
			victim = ch->desc->admin;
		else
			victim = ha3100_get_char_vis(ch, arg);
		if (!victim) {
			send_to_char("Unable to find that person.\r\n", ch);
			return;
		}
		pr1200_do_the_guild_no_arg(ch, victim, "", CMD_PRACTICE);
		return;
	}
	tmp = pr1400_do_guild(ch, cmd, "");

	return;

}				/* END OF ot2000_do_practice() */

void ot2100_do_idea(struct char_data * ch, char *argument, int cmd)
{

	FILE *fl;
	char str[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];



	if (IS_NPC(ch)) {
		send_to_char("Monsters can't have ideas - Go away.\n\r", ch);
		return;
	}

	for (; isspace(*argument); argument++);

	if (!*argument) {
		send_to_char("That doesn't sound like a good idea to me.. Sorry.\n\r",
			     ch);
		return;
	}

	if (!(fl = fopen(IDEA_FILE, "a"))) {
		perror("ot2100_do_idea");
		send_to_char("Could not open the idea-file.\n\r", ch);
		return;
	}

	sprintf(str, "**%s: %s\n", GET_NAME(ch), argument);
	fputs(str, fl);
	fclose(fl);

	sprintf(buf, " write scribe [IDEA]: %s", argument);
	ma1000_do_mail(ch, buf, CMD_BUG);
	send_to_char("\r\nPlease enter more about your idea:\n\r", ch);

}				/* END OF ot2100_do_idea() */


void ot2200_do_typo(struct char_data * ch, char *argument, int cmd)
{

	FILE *fl;
	char str[MAX_STRING_LENGTH];



	if (IS_NPC(ch)) {
		send_to_char("Monsters can't spell - leave me alone.\n\r", ch);
		return;
	}

	/* skip whites */
	for (; isspace(*argument); argument++);

	if (!*argument) {
		send_to_char("I beg your pardon?\n\r", ch);
		return;
	}

	if (!(fl = fopen(TYPO_FILE, "a"))) {
		perror("ot2200_do_typo");
		send_to_char("Could not open the typo-file.\n\r", ch);
		return;
	}

	sprintf(str, "**%s[%d]: %s\n",
		GET_NAME(ch), world[ch->in_room].number, argument);
	fputs(str, fl);
	fclose(fl);
	send_to_char("Ok. thanks.\n\r", ch);
}				/* END OF ot2200_do_typo() */


void ot2300_do_bug(struct char_data * ch, char *argument, int cmd)
{

	FILE *fl;
	char str[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];



	if (IS_NPC(ch)) {
		send_to_char("You are a monster! Bug off!\n\r", ch);
		return;
	}

	/* skip whites */
	for (; isspace(*argument); argument++);

	if (!*argument) {
		send_to_char("Pardon?\n\r",
			     ch);
		return;
	}

	if (!(fl = fopen(BUG_FILE, "a"))) {
		perror("ot2300_do_bug");
		send_to_char("Could not open the bug-file.\n\r", ch);
		return;
	}

	sprintf(str, "**%s[%d]: %s\n",
		GET_NAME(ch), world[ch->in_room].number, argument);
	fputs(str, fl);
	fclose(fl);

	sprintf(buf, " write Relic [BUG]: %s", argument);
	ma1000_do_mail(ch, buf, CMD_BUG);
	send_to_char("\r\nPlease enter more information about the bug:\n\r", ch);

}				/* END OF ot2300_do_bug() */


void ot2400_do_brief(struct char_data * ch, char *argument, int cmd)
{



	if (IS_SET(GET_ACT3(ch), PLR3_BRIEF)) {
		send_to_char("Brief mode off.\n\r", ch);
		REMOVE_BIT(GET_ACT3(ch), PLR3_BRIEF);
	}
	else {
		send_to_char("Brief mode on.\n\r", ch);
		SET_BIT(GET_ACT3(ch), PLR3_BRIEF);
	}
}				/* END OF ot2400_do_brief() */


void ot2500_do_compact(struct char_data * ch, char *argument, int cmd)
{



	if (IS_SET(GET_ACT3(ch), PLR3_COMPACT)) {
		send_to_char("You are now in the uncompacted mode.\n\r", ch);
		REMOVE_BIT(GET_ACT3(ch), PLR3_COMPACT);
	}
	else {
		send_to_char("You are now in compact mode.\n\r", ch);
		SET_BIT(GET_ACT3(ch), PLR3_COMPACT);
	}
}				/* END OF ot2500_do_compact() */


void ot2600_do_assist(struct char_data * ch, char *arg, int cmd)
{



	if (IS_SET(GET_ACT3(ch), PLR3_ASSIST)) {
		send_to_char("You will no longer auto assist group members.\n\r", ch);
		REMOVE_BIT(GET_ACT3(ch), PLR3_ASSIST);
	}
	else {
		send_to_char("You will now auto assist group members that are attacked.\n\r", ch);
		SET_BIT(GET_ACT3(ch), PLR3_ASSIST);
	}
}				/* END OF ot2600_do_assist() */


void ot2700_do_wimpy(struct char_data * ch, char *arg, int cmd)
{



	if (IS_SET(GET_ACT3(ch), PLR3_WIMPY)) {
		send_to_char("You will no longer auto flee.\n\r", ch);
		REMOVE_BIT(GET_ACT3(ch), PLR3_WIMPY);
	}
	else {
		send_to_char("You will now auto flee when you are badly hurt (WIMP!).\n\r", ch);
		SET_BIT(GET_ACT3(ch), PLR3_WIMPY);
	}
}				/* END OF ot2700_do_wimpy() */


void ot2800_do_display(struct char_data * ch, char *arg, int cmd)
{

	const char *toggle[] = {
		"noprompt",
		"room",
		"hit",
		"mana",
		"move",
		"exits",
		"name",
		"vehicle",
		"allprompt",
		"\n"
	};
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int i;



	arg = one_argument(arg, buf);
	if (*buf)
		i = (old_search_block(buf, 0, strlen(buf), toggle, 0));
	else
		i = -1;

	switch (i) {
	case 1:		/* no prompt */
		REMOVE_BIT(GET_ACT2(ch), PLR2_SHOW_NAME);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_ROOM);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_VEHICLE);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_HP);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_MANA);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_MOVE);
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_EXITS);
		break;

	case 2:		/* room */
		if (GET_LEVEL(ch) < IMO_LEV) {
			send_to_char("Sorry mortals can't view room numbers\r\n",
				     ch);
			return;
		}
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_ROOM)) {
			send_to_char("Display room off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_ROOM);
		}
		else {
			send_to_char("Display room on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_ROOM);
		}
		break;
	case 3:		/* hit */
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_HP)) {
			send_to_char("Display hit points off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_HP);
		}
		else {
			send_to_char("Display hit points on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_HP);
		}
		break;
	case 4:		/* mana */
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_MANA)) {
			send_to_char("Display mana off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_MANA);
		}
		else {
			send_to_char("Display mana on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_MANA);
		}
		break;
	case 5:		/* move */
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_MOVE)) {
			send_to_char("Display move off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_MOVE);
		}
		else {
			send_to_char("Display move on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_MOVE);
		}
		break;
	case 6:		/* exits */
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_EXITS)) {
			send_to_char("Display exits off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_EXITS);
		}
		else {
			send_to_char("Display exits on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_EXITS);
		}
		break;
	case 7:		/* name */
		if (IS_SET(ch->specials.act2, PLR2_SHOW_NAME)) {
			send_to_char("Display name off.\n\r", ch);
			REMOVE_BIT(ch->specials.act2, PLR2_SHOW_NAME);
		}
		else {
			send_to_char("Display name on.\n\r", ch);
			SET_BIT(ch->specials.act2, PLR2_SHOW_NAME);
		}
		break;
	case 8:		/* vehicles */
		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_VEHICLE)) {
			send_to_char("Display vehicle off.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_VEHICLE);
		}
		else {
			send_to_char("Display vehicle on.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_VEHICLE);
		}
		break;
	case 9:		/* all prompt */
		SET_BIT(GET_ACT2(ch), PLR2_SHOW_NAME);
		if (GET_LEVEL(ch) > PK_LEV) {
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_ROOM);
			SET_BIT(GET_ACT3(ch), PLR3_SHOW_VEHICLE);
		}
		SET_BIT(GET_ACT3(ch), PLR3_SHOW_HP);
		SET_BIT(GET_ACT3(ch), PLR3_SHOW_MANA);
		SET_BIT(GET_ACT3(ch), PLR3_SHOW_MOVE);
		SET_BIT(GET_ACT3(ch), PLR3_SHOW_EXITS);
		break;
	default:
		sprintf(buf, "Display <TOGGLE>\n\rValid toggles are: ");
		for (i = 0; *toggle[i] != '\n'; i++) {
			strcat(buf, toggle[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\n\r\n\r");
		send_to_char(buf, ch);
		send_to_char("Prompt Displays:        Channels:\n\r", ch);

		/* Line 1 */
		sprintf(buf, "Hit Points:     ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_SHOW_HP) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Gossip:         ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOGOSSIP) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 2 */
		sprintf(buf, "Mana Points:    ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_SHOW_MANA) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Auction:        ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOAUCTION) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 3 */
		sprintf(buf, "Move Points:    ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_SHOW_MOVE) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Info:           ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOINFO) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 4 */
		sprintf(buf, "Visible Exits:  ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_SHOW_EXITS) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     System:         ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOSYSTEM) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 5 */
		sprintf(buf, "Room Numbers:   ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_SHOW_ROOM) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     IMM:            ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOIMM) && (GET_LEVEL(ch) >= IMO_LEV) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 6 */
		send_to_char("                   ", ch);
		sprintf(buf, "     Connect:        ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOCONNECT) && (GET_LEVEL(ch) >= IMO_LEV) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 7 */
		send_to_char("Misc.:             ", ch);
		sprintf(buf, "     Wizinfo:        ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOWIZINFO) && (GET_LEVEL(ch) >= IMO_LEV) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 8 */
		sprintf(buf, "Ansi:           ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_ANSI) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Tells:          ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOTELL) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 9 */
		sprintf(buf, "Compact:        ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_COMPACT) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Avatar:         ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOAVATAR) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 10 */
		sprintf(buf, "Brief:          ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_BRIEF) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Pkflame:        ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOPKFLAME) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);		
		//sprintf(buf, "     Antediluvian:   ");
		//strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOANTE) ? "Yes\n\r" : "No \n\r");
		//send_to_char(buf, ch);

		/* Line 11 */
		sprintf(buf, "                        Staff:          ");
		strcat(buf, !IS_SET(GET_ACT2(ch), PLR2_NOSTAFF) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 12 */
		sprintf(buf, "Nosummon:       ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_NOSUMMON) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Boss:           ");
		strcat(buf, !IS_SET(GET_ACT2(ch), PLR2_NOBOSS) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);
		
		/* Line 13 */
		sprintf(buf, "Nohassle:       ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_NOHASSLE) && (GET_LEVEL(ch) >= IMO_LEV) ? "Yes" : "No ");
		send_to_char(buf, ch);
		sprintf(buf, "     Music:          ");
		strcat(buf, !IS_SET(GET_ACT3(ch), PLR3_NOMUSIC) ? "Yes\n\r" : "No \n\r");
		send_to_char(buf, ch);

		/* Line 14 */
		bzero(buf, sizeof(buf2));
		sprintf(buf, "Visibility:     ");
		if (GET_VISIBLE(ch)) {
			sprintf(buf2, "LVL: %d\r\n", GET_VISIBLE(ch));
			strcat(buf, buf2);
		}
		else {
			strcat(buf, "Full\r\n");
		}
		send_to_char(buf, ch);

		break;
	}
}				/* END OF ot2800_do_display() */


void ot2900_do_auto(struct char_data * ch, char *arg, int cmd)
{

	const char *toggle[] = {
		"none",
		"loot",
		"gold",
		"split",
		"aggressive",
		"flee",
		"assist",
		"all",
		"\n"
	};
	char buf[MAX_STRING_LENGTH];
	int i;



	arg = one_argument(arg, buf);
	if (*buf)
		i = (old_search_block(buf, 0, strlen(buf), toggle, 0));
	else
		i = -1;

	switch (i) {
	case 1:		/* none */
		REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOGOLD);
		REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOLOOT);
		REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOSPLIT);
		REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOAGGR);
		REMOVE_BIT(GET_ACT3(ch), PLR3_WIMPY);
		REMOVE_BIT(GET_ACT3(ch), PLR3_ASSIST);
		break;

	case 2:		/* loot */
		if (IS_SET(GET_ACT3(ch), PLR3_AUTOLOOT)) {
			send_to_char("You will no longer auto loot.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOLOOT);
		}
		else {
			send_to_char("You will now auto loot your kills.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_AUTOLOOT);
		}
		break;
	case 3:		/* gold */
		if (IS_SET(GET_ACT3(ch), PLR3_AUTOGOLD)) {
			send_to_char("You will no longer auto gold.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOGOLD);
		}
		else {
			send_to_char("You will now auto gold your kills.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_AUTOGOLD);
		}
		break;
	case 4:		/* split */
		if (IS_SET(GET_ACT3(ch), PLR3_AUTOSPLIT)) {
			send_to_char("You will no longer auto split gold.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOSPLIT);
		}
		else {
			send_to_char("You will now auto split gold with your group.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_AUTOSPLIT);
		}
		break;
	case 5:		/* aggr */
		if (IS_SET(GET_ACT3(ch), PLR3_AUTOAGGR)) {
			send_to_char("You will try to spare unconsious mobs.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_AUTOAGGR);
		}
		else {
			send_to_char("You will finish killing unconsious mobs.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_AUTOAGGR);
		}
		break;
	case 6:		/* flee */
		if (IS_SET(GET_ACT3(ch), PLR3_WIMPY)) {
			send_to_char("You will no longer auto flee.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_WIMPY);
		}
		else {
			send_to_char("You will now auto flee when badly hurt (WIMP!).\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_WIMPY);
		}
		break;
	case 7:		/* assist */
		if (IS_SET(GET_ACT3(ch), PLR3_ASSIST)) {
			send_to_char("You will no longer auto assist.\n\r", ch);
			REMOVE_BIT(GET_ACT3(ch), PLR3_ASSIST);
		}
		else {
			send_to_char("You will now auto assist group members.\n\r", ch);
			SET_BIT(GET_ACT3(ch), PLR3_ASSIST);
		}
		break;
	case 8:		/* all */
		SET_BIT(GET_ACT3(ch), PLR3_AUTOGOLD);
		SET_BIT(GET_ACT3(ch), PLR3_AUTOLOOT);
		SET_BIT(GET_ACT3(ch), PLR3_AUTOSPLIT);
		SET_BIT(GET_ACT3(ch), PLR3_AUTOAGGR);
		SET_BIT(GET_ACT3(ch), PLR3_WIMPY);
		SET_BIT(GET_ACT3(ch), PLR3_ASSIST);
		send_to_char("all auto actions are now on.\n\r", ch);
		break;
	default:
		sprintf(buf, "Auto <TOGGLE>\n\rValid toggles are: ");
		for (i = 0; *toggle[i] != '\n'; i++) {
			strcat(buf, toggle[i]);
			strcat(buf, " ");
		}
		send_to_char(buf, ch);

		sprintf(buf, "\n\r\n\rAuto loot:       ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_AUTOLOOT) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);

		sprintf(buf, "Auto gold:       ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_AUTOGOLD) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);

		sprintf(buf, "Auto split:      ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_AUTOSPLIT) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);

		sprintf(buf, "Auto aggressive: ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_AUTOAGGR) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);

		sprintf(buf, "Auto flee:       ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_WIMPY) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);

		sprintf(buf, "Auto assist:     ");
		strcat(buf, IS_SET(GET_ACT3(ch), PLR3_ASSIST) ? "Yes\n\r" : "No\n\r");
		send_to_char(buf, ch);
		break;
	}
}				/* END OF ot2900_do_auto() */


void ot3000_do_group(struct char_data * ch, char *argument, int cmd)
{

	int number_in_group, number_grouped;
	char name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	struct char_data *victim, *k;
	struct follow_type *f;
	bool found;



	one_argument(argument, name);

	if (!*name) {
		if (!IS_AFFECTED(ch, AFF_GROUP)) {
			send_to_char("But you are a member of no group?!\n\r", ch);
			return;
		}

		send_to_char("Your group consists of:\n\r", ch);
		if (ch->master)
			k = ch->master;
		else
			k = ch;

		bzero(buf, sizeof(buf));
		if (IS_AFFECTED(k, AFF_GROUP)) {
			sprintf(buf, "[%c%2d] [%-15s] [%4d/%4d %4d/%4d %4d/%4d ] (Leader)\n\r",
				classes[GET_CLASS(k)].desc[0],
				GET_LEVEL(k),
				GET_REAL_NAME(k),
				GET_HIT(k),
				GET_HIT_LIMIT(k),
				GET_MANA(k),
				GET_MANA_LIMIT(k),
				GET_MOVE(k),
				GET_MOVE_LIMIT(k));
			send_to_char(buf, ch);
		}

		for (f = k->followers; f; f = f->next)
			if (IS_AFFECTED(f->follower, AFF_GROUP)) {
				sprintf(buf, "[%c%2d] [%-15s] [%4d/%4d %4d/%4d %4d/%4d ]\n\r",
				      classes[GET_CLASS(f->follower)].desc[0],
					GET_LEVEL(f->follower),
					GET_REAL_NAME(f->follower),
					GET_HIT(f->follower),
					GET_HIT_LIMIT(f->follower),
					GET_MANA(f->follower),
					GET_MANA_LIMIT(f->follower),
					GET_MOVE(f->follower),
					GET_MOVE_LIMIT(f->follower));
				send_to_char(buf, ch);
			}
		return;
	}			/* END OF no name specified */

	/* group where */
	if (!(strcmp(name, "where"))) {
		if (ch->master) {
			k = ch->master;
		}
		else {
			k = ch;
		}
		send_to_char("\r\nGroup members:\r\n--------------\r\n", ch);
		/* LEADER */
		if ((IS_AFFECTED(k, AFF_GROUP)) &&
		    (GET_VISIBLE(k) <= GET_LEVEL(ch))) {
			sprintf(buf, "%-20s - r[%d] %s",
				GET_REAL_NAME(k),
				world[k->in_room].number,
				world[k->in_room].name);
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}
		for (f = k->followers; f; f = f->next) {
			if (f->follower &&
			    (f->follower->in_room >= 0)) {
				victim = f->follower;
				if ((IS_AFFECTED(victim, AFF_GROUP)) &&
				    (GET_VISIBLE(victim) <= GET_LEVEL(ch))) {
					sprintf(buf, "%-20s - r[%d] %s",
						GET_REAL_NAME(victim),
						world[victim->in_room].number,
						world[victim->in_room].name);
					strcat(buf, "\r\n");
					send_to_char(buf, ch);
				}
			}
		}
		return;
	}			/* END of group where */


	/* GROUP ALL */
	if (strlen(name) == 3 && !(strcmp(name, "all"))) {
		/* WE HAVE TO BE A LEADER */
		if (ch->master) {
			act("You can't group others when your following someone!",
			    FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		number_in_group = 1;
		number_grouped = 0;	/* COUNT YOURSELF */
		/* GROUP LEADER */
		if (!(IS_AFFECTED(ch, AFF_GROUP))) {
			number_grouped++;
			send_to_char("You are now a group member\r\n", ch);
			SET_BIT(ch->specials.affected_by, AFF_GROUP);
		}
		/* GROUP FOLLOWERS */
		for (f = ch->followers; f; f = f->next) {
			victim = f->follower;
			if (CAN_SEE(ch, victim)) {
				number_in_group++;
				if (!(IS_AFFECTED(victim, AFF_GROUP)) &&
				    (ch->in_room == victim->in_room)) {
					number_grouped++;
					bzero(buf, sizeof(buf));
					sprintf(buf, "%s is now a member of %s's group!",
						GET_REAL_NAME(victim), GET_REAL_NAME(ch));
					act(buf, TRUE, victim, 0, 0, TO_ROOM);
					act("You are now a group member.", FALSE, victim, 0, 0, TO_CHAR);
					SET_BIT(victim->specials.affected_by, AFF_GROUP);
				}
			}
		}		/* END OF for loop */
		if (number_grouped == 0) {
			send_to_char("I couldn't find anyone who needed to be grouped.\r\n", ch);
		}
		if (number_grouped == 1) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "You grouped 1 player and now have %d in the group.\r\n",
				number_in_group);
			send_to_char(buf, ch);
		}
		if (number_grouped > 1) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "You grouped %d players and now have %d in the group.\r\n",
				number_grouped, number_in_group);
			send_to_char(buf, ch);
		}
		return;
	}

	/* GROUP A PARTICULAR PLAYER */
	victim = ha2125_get_char_in_room_vis(ch, name);
	if (!(victim)) {
		send_to_char("No one here by that name.\n\r", ch);
		return;
	}

	if (ch->master) {
		act("You can't group others when your following someone!",
		    FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	found = FALSE;

	if (victim == ch)
		found = TRUE;
	else {
		for (f = ch->followers; f; f = f->next) {
			if (f->follower == victim) {
				found = TRUE;
				break;
			}
		}
	}

	if (!found) {
		act("$N must follow you, to enter the group", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_AFFECTED(victim, AFF_GROUP)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s has been kicked out of %s's group!",
		   (IS_NPC(victim) ? GET_MOB_NAME(victim) : GET_NAME(victim)),
			(IS_NPC(ch) ? GET_MOB_NAME(ch) : GET_NAME(ch)));
		act(buf, TRUE, victim, 0, ch, TO_ROOM);
		act("You are no longer a member of the group!", FALSE, victim, 0, 0, TO_CHAR);
		REMOVE_BIT(victim->specials.affected_by, AFF_GROUP);
	}
	else {
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s is now a member of %s's group!",
		   (IS_NPC(victim) ? GET_MOB_NAME(victim) : GET_NAME(victim)),
			(IS_NPC(ch) ? GET_MOB_NAME(ch) : GET_NAME(ch)));
		act(buf, TRUE, victim, 0, 0, TO_ROOM);
		act("You are now a group member.", FALSE, victim, 0, 0, TO_CHAR);
		SET_BIT(victim->specials.affected_by, AFF_GROUP);
	}

}				/* END OF ot3000_do_group() */


void ot3100_do_quaff(struct char_data * ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	struct obj_data *temp;
	int i, lag;
	bool equipped;



	equipped = FALSE;

	one_argument(argument, buf);

	if (!(temp = ha2075_get_obj_list_vis(ch, buf, ch->carrying))) {
		temp = ch->equipment[HOLD];
		equipped = TRUE;
		if ((temp == 0) || !ha1150_isname(buf, temp->name)) {
			act("You do not have that item.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}

	if (temp->obj_flags.type_flag != ITEM_POTION) {
		act("You can only quaff potions.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	act("$n quaffs $p.", TRUE, ch, temp, 0, TO_ROOM);
	act("You quaff $p which dissolves.", FALSE, ch, temp, 0, TO_CHAR);


	/* IS ROOM NO MAGIC? */
	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Hrmmm, you chugged that right down but nothing happens.\r\n", ch);
		return;
	}
	else {
		for (i = 1; i < 4; i++)
			if ((temp->obj_flags.value[i] >= 0)
			    && (temp->obj_flags.value[i] < MAX_SKILLS)
			    && (spell_info[temp->obj_flags.value[i]].spell_pointer))
				((*spell_info[temp->obj_flags.value[i]].spell_pointer)
				 ((signed char) MINV(NPC_LEV, MAXV(1, temp->obj_flags.value[0])), ch, "", SPELL_TYPE_POTION, ch, 0));

	}
	if (GET_LEVEL(ch) == IMO_LEV && GET_LEVEL(ch) < IMO_IMM)
	{
    	if (obj_index[temp->item_number].virtual == OBJ_WHITE_POTION) {
    		lag = 3;
    		if (ch->specials.fighting) {
    			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
    			WAIT_STATE(ch, PULSE_VIOLENCE * lag);
    		}
    	}
    	if (obj_index[temp->item_number].virtual == OBJ_HEALING_POTION) {
    		lag = 2;
    		if (ch->specials.fighting) {
    			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
    			WAIT_STATE(ch, PULSE_VIOLENCE * lag);
    		}
    	}
    	if (obj_index[temp->item_number].virtual == OBJ_NEWBIE_HEALING_POTION) {
    		lag = 2;
    		if (ch->specials.fighting) {
    			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
    			WAIT_STATE(ch, PULSE_VIOLENCE * lag);
    		}
    	}
		// Default cooldown on potions for Avatars == 1 round.  --ff 01/17/2009
		else {
    		lag = 1;
    		if (ch->specials.fighting) {
    			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
    			WAIT_STATE(ch, PULSE_VIOLENCE * lag);
    		}
    	}
		/*
    	if (obj_index[temp->item_number].virtual == OBJ_BLUE_POTION) {
    		lag = 1;
    		if (ch->specials.fighting) {
    			WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
    			WAIT_STATE(ch, PULSE_VIOLENCE * lag);
    		}
    	}*/
    }
	if (equipped)
		ha1930_unequip_char(ch, HOLD);

	ha2700_extract_obj(temp);

}				/* END OF ot3100_do_quaff() */


void ot3200_do_recite(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *scroll, *obj;
	struct char_data *victim;
	int i, bits;
	bool equipped;



	equipped = FALSE;
	obj = 0;
	victim = 0;

	argument = one_argument(argument, buf);

	if (!(scroll = ha2075_get_obj_list_vis(ch, buf, ch->carrying))) {
		scroll = ch->equipment[HOLD];
		equipped = TRUE;
		if ((scroll == 0) || !ha1150_isname(buf, scroll->name)) {
			act("You do not have that item.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}

	if (scroll->obj_flags.type_flag != ITEM_SCROLL) {
		act("Recite is normally used for scroll's.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if (gv_port == ZONING_PORT) {
		if (GET_LEVEL(ch) < IMO_IMM) {
			send_to_char("You are not authorized to use recite on this port!\n\r", ch);
			return;
		}
	}
	if ((IS_OBJ_STAT(scroll, OBJ1_MINLVL10) && GET_LEVEL(ch) < 10) ||
	    (IS_OBJ_STAT2(scroll, OBJ2_MINLVL15) && GET_LEVEL(ch) < 15) ||
	    (IS_OBJ_STAT(scroll, OBJ1_MINLVL20) && GET_LEVEL(ch) < 20) ||
	    (IS_OBJ_STAT2(scroll, OBJ2_MINLVL25) && GET_LEVEL(ch) < 25) ||
	    (IS_OBJ_STAT(scroll, OBJ1_MINLVL30) && GET_LEVEL(ch) < 30) ||
	    (IS_OBJ_STAT2(scroll, OBJ2_MINLVL35) && GET_LEVEL(ch) < 35) ||
	    (IS_OBJ_STAT(scroll, OBJ1_MINLVL41) && GET_LEVEL(ch) < 41) ||
	    (IS_OBJ_STAT2(scroll, OBJ2_MINLVL42) && GET_LEVEL(ch) < 42) ||
	 (IS_OBJ_STAT2(scroll, OBJ2_IMMONLY) && GET_LEVEL(ch) < IMO_SPIRIT)) {
		act("You are too inexperienced to use $p.",
		    FALSE, ch, scroll, 0, TO_CHAR);
		act("$n realizes that $s is too inexperienced to use $p.",
		    FALSE, ch, scroll, 0, TO_ROOM);
		return;
	}

	if (*argument) {
		bits = ha3500_generic_find(argument, FIND_OBJ_INV | FIND_OBJ_ROOM |
			  FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &victim, &obj);
		if (bits == 0) {
			send_to_char("No such thing around to recite the scroll on.\n\r", ch);
			return;
		}
	}
	else {
		victim = ch;
	}

	act("$n recites $p.", TRUE, ch, scroll, 0, TO_ROOM);
	act("You recite $p which dissolves.", FALSE, ch, scroll, 0, TO_CHAR);

	/* IS ROOM NO MAGIC? */
	if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Hrmmm, you chanted and hopped around like the book said, but nothing happens.\r\n", ch);
		return;
	}
	/* PREVENT LOW LEVELS WARPSCROLLING AVATAR ETC */
	/* else if (IS_PC(victim) && (GET_LEVEL(ch) - GET_LEVEL(victim) <= 10))
	 * { send_to_char("Sorry, you can't do that anymore!\n\r", ch); return;
	 * } */
	else {
		for (i = 1; i < 4; i++)
			if ((scroll->obj_flags.value[i] >= 1)
			    && (scroll->obj_flags.value[i] < MAX_SKILLS)
			    && (spell_info[scroll->obj_flags.value[i]].spell_pointer))
				((*spell_info[scroll->obj_flags.value[i]].spell_pointer)
				 ((signed char) MAXV(1, MINV(NPC_LEV, scroll->obj_flags.value[0])), ch, "", SPELL_TYPE_SCROLL, victim, obj));
	}

	if (equipped)
		ha1930_unequip_char(ch, HOLD);

	ha2700_extract_obj(scroll);
}				/* END OF ot3200_do_recite() */


void ot3300_do_use(struct char_data * ch, char *argument, int cmd)
{

#define USE_FOUND_IN_EQUIPMENT 1
#define USE_FOUND_IN_INVENTORY 2

	struct obj_data
	 *tmp_object, *stick;
	char buf[MAX_STRING_LENGTH];
	struct char_data *tmp_char;
	int bits, idx, lv_rtn, lv_real_to, lv_where_found;



	argument = one_argument(argument, buf);

	/* CHECK WORN ITEMS */
	lv_where_found = USE_FOUND_IN_EQUIPMENT;
	stick = in2300_get_object_in_equip_vis(ch, buf, ch->equipment, &idx);
	/* CHECK INVENTORY */
	if (!stick) {
		lv_where_found = USE_FOUND_IN_INVENTORY;
		stick = ha2075_get_obj_list_vis(ch, buf, ch->carrying);
	}
	if (!stick) {
		send_to_char("You do not have that item.\r\n", ch);
		return;
	}

	if ((IS_OBJ_STAT(stick, OBJ1_MINLVL10) && GET_LEVEL(ch) < 10) ||
	    (IS_OBJ_STAT2(stick, OBJ2_MINLVL15) && GET_LEVEL(ch) < 15) ||
	    (IS_OBJ_STAT(stick, OBJ1_MINLVL20) && GET_LEVEL(ch) < 20) ||
	    (IS_OBJ_STAT2(stick, OBJ2_MINLVL25) && GET_LEVEL(ch) < 25) ||
	    (IS_OBJ_STAT(stick, OBJ1_MINLVL30) && GET_LEVEL(ch) < 30) ||
	    (IS_OBJ_STAT2(stick, OBJ2_MINLVL35) && GET_LEVEL(ch) < 35) ||
	    (IS_OBJ_STAT(stick, OBJ1_MINLVL41) && GET_LEVEL(ch) < 41) ||
	    (IS_OBJ_STAT2(stick, OBJ2_MINLVL42) && GET_LEVEL(ch) < 42) ||
	  (IS_OBJ_STAT2(stick, OBJ2_IMMONLY) && GET_LEVEL(ch) < IMO_SPIRIT)) {
		act("You are too inexperienced to use $p.",
		    FALSE, ch, stick, 0, TO_CHAR);
		act("$n realizes that $s is too inexperienced to use $p.",
		    FALSE, ch, stick, 0, TO_ROOM);
		return;
	}

	if (stick->obj_flags.type_flag == ITEM_STAFF) {
		if (stick != ch->equipment[HOLD]) {
			act("You must hold that item in your hand to use it.",
			    FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		act("$n taps $p three times on the ground.",
		    TRUE, ch, stick, 0, TO_ROOM);
		act("You tap $p three times on the ground.",
		    FALSE, ch, stick, 0, TO_CHAR);

		/* Are there any charges left? */
		if (stick->obj_flags.value[2] > 0) {
			stick->obj_flags.value[2]--;


			/* IS ROOM NO MAGIC? */
			if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("Hrmmm, those were good taps but nothing happens.\r\n", ch);
				return;
			}

			if (spell_info[MINV(MAX_SKILLS - 1,
			 MAXV(1, stick->obj_flags.value[3]))].spell_pointer) {
				((*spell_info[stick->obj_flags.value[3]].spell_pointer)
				 ((signed char) stick->obj_flags.value[0], ch, "",
				  SPELL_TYPE_STAFF, 0, 0));
			}
		}
		else {
			send_to_char("The staff seems powerless.\n\r", ch);
		}
		return;

	}			/* END OF ITS A STAFF */

	if (stick->obj_flags.type_flag == ITEM_WAND) {
		if (stick != ch->equipment[HOLD]) {
			act("You must hold that item in your hand to use it.",
			    FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		bits = ha3500_generic_find(argument, FIND_CHAR_ROOM | FIND_OBJ_INV |
					   FIND_OBJ_ROOM | FIND_OBJ_EQUIP,
					   ch, &tmp_char, &tmp_object);
		if (!bits) {
			send_to_char("What should the wand be pointed at?\n\r", ch);
			return;
		}

		if (bits == FIND_CHAR_ROOM) {
			act("$n point $p at $N.", TRUE, ch, stick, tmp_char, TO_ROOM);
			act("You point $p at $N.",
			    FALSE, ch, stick, tmp_char, TO_CHAR);
		}
		else {
			act("$n point $p at $P.",
			    TRUE, ch, stick, tmp_object, TO_ROOM);
			act("You point $p at $P.",
			    FALSE, ch, stick, tmp_object, TO_CHAR);
		}
		/* Are there any charges left? */
		if (stick->obj_flags.value[2] > 0) {
			stick->obj_flags.value[2]--;

			/* IS ROOM NO MAGIC? */
			if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("Hrmmm, you pointed it properly but nothing happens.\r\n", ch);
				return;
			}

			if (spell_info[MINV(MAX_SKILLS - 1,
			 MAXV(1, stick->obj_flags.value[3]))].spell_pointer) {
				((*spell_info[stick->obj_flags.value[3]].spell_pointer)
				 ((signed char) stick->obj_flags.value[0], ch, "",
				  SPELL_TYPE_WAND, tmp_char, tmp_object));
			}
		}
		else {
			send_to_char("The wand seems powerless.\n\r", ch);
			return;
		}

		return;

	}			/* END OF ITEM IS A WAND */

	if (stick->obj_flags.type_flag == ITEM_SPELL) {
		if (lv_where_found != USE_FOUND_IN_EQUIPMENT) {
			send_to_char("You need to equip the item to use it.\r\n", ch);
			return;
		}

		bits = ha3500_generic_find(argument, FIND_CHAR_ROOM | FIND_OBJ_INV |
					   FIND_OBJ_ROOM | FIND_OBJ_EQUIP,
					   ch, &tmp_char, &tmp_object);
		if (!bits) {
			send_to_char("What should the item be used on?\r\n", ch);
			return;
		}

		if (bits == FIND_CHAR_ROOM) {
			act("$n uses $p on $N.", TRUE, ch, stick, tmp_char, TO_ROOM);
			act("You use $p on $N.",
			    FALSE, ch, stick, tmp_char, TO_CHAR);
		}
		else {
			act("$n uses $p on $P.",
			    TRUE, ch, stick, tmp_object, TO_ROOM);
			act("You use $p on $P.",
			    FALSE, ch, stick, tmp_object, TO_CHAR);
		}
		/* Are there any charges left? */
		if (stick->obj_flags.value[2] > 0) {
			stick->obj_flags.value[2]--;

			/* IS ROOM NO MAGIC? */
			if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("Hrmmm, you used it properly but nothing happens.\r\n", ch);
				return;
			}

			if (spell_info[MINV(MAX_SKILLS - 1,
			 MAXV(1, stick->obj_flags.value[3]))].spell_pointer) {
				((*spell_info[stick->obj_flags.value[3]].spell_pointer)
				 ((signed char) stick->obj_flags.value[0], ch, "",
				  SPELL_TYPE_WAND, tmp_char, tmp_object));
			}
		}
		else {
			send_to_char("The item seems powerless.\r\n", ch);
			return;
		}

		return;

	}			/* END OF ITEM IS A SPELL */

	if (obj_index[stick->item_number].virtual == OBJ_ICE_PICK) {
		if (world[ch->in_room].number == 2472) {
			/* IS THERE A LINK UP ALREADY? */
			if (EXIT(ch, CMD_UP - 1)) {
				send_to_char("Nothing happens.\r\n", ch);
				act("$n fiddles with $p.",
				    FALSE, ch, stick, 0, TO_ROOM);
				return;
			}

			lv_real_to = db8000_real_room(2473);
			lv_rtn = wi2920_create_room_links(ch->in_room,
							  lv_real_to,
							  CMD_UP - 1,
							  "that you can step upwards using a spike stuck in the wall");


			lv_rtn = wi2920_create_room_links(lv_real_to,
							  ch->in_room,
							  CMD_DOWN - 1,
							  "that you can step downwards using a spike stuck in the wall");

			act("You ram $p into the wall creating a step upwards.",
			    FALSE, ch, stick, 0, TO_CHAR);
			act("$n rams $p into the wall.",
			    FALSE, ch, stick, 0, TO_ROOM);
			ha2700_extract_obj(stick);
			return;

		}		/* END OF world = 2472 */

		/* DON"T DO ANYTHING AND FALL THROUGH */

	}			/* END OF ice pick */

	act("Fiddling with $p appears to serve no useful purpose.",
	    FALSE, ch, stick, 0, TO_CHAR);
	act("$n fiddles with $p.",
	    FALSE, ch, stick, 0, TO_ROOM);
	return;

}				/* END OF ot3300_do_use() */


void ot3400_do_deposit(struct char_data * ch, char *argument, int cmd)
{



	send_to_char("Try the money-lenders... \n\r", ch);
}				/* END OF ot3400_do_deposit() */


void ot3500_do_withdraw(struct char_data * ch, char *argument, int cmd)
{



	send_to_char("Try the money-lenders... \n\r", ch);
}				/* END OF ot3500_do_withdraw() */

void ot3600_do_balance(struct char_data * ch, char *argument, int cmd)
{



	send_to_char("Try the money-lenders... \n\r", ch);
}				/* END OF ot3600_do_balance() */


void ot3700_do_simple_split(struct char_data * ch, long total)
{
	char buf[MAX_INPUT_LENGTH];
	char buf2[MAX_INPUT_LENGTH];
	int no_members, share;
	struct char_data *lead;
	struct follow_type *f;
	struct obj_data *tmp_object;




	/* will choke if the character isnt in a group */
	if (!IS_AFFECTED(ch, AFF_GROUP))
		return;

	sprintf(buf, "There were %ld coins.\r\n", total);
	send_to_char(buf, ch);
	/* if (total > GET_GOLD(ch)){ total = GET_GOLD(ch); sprintf(buf,"total:
	 * %d \r\n",total); send_to_char(buf,ch); } */
	if (!(lead = ch->master))
		lead = ch;	/* find leader */

	if (IS_AFFECTED(lead, AFF_GROUP) && (lead->in_room == ch->in_room))
		no_members = 1;
	else
		no_members = 0;
	for (f = lead->followers; f; f = f->next)
		if (IS_AFFECTED(f->follower, AFF_GROUP)
		    && (f->follower->in_room == ch->in_room)
		    && (IS_PC(f->follower)))
			no_members += 1;

	if (no_members <= 1) {	/* This got moved up a level */

		//sprintf(buf, "There were %d coins.\r\n", total);
		//send_to_char(buf, ch);
		act("You split the money with yourself.\r\n", FALSE, ch, 0, 0, TO_CHAR);
		/*
		 if ((MAX_GOLD - GET_GOLD(ch) - total) < 0) {
			 send_to_char("You can't hold all the money, and drop it on the ground\r\n", ch);
			 tmp_object = ha3400_create_money(total);
			 ha2100_obj_to_room(tmp_object, ch->in_room);
		 }
		 else {
			 GET_GOLD(ch) += total;
		 }*/

		return;
	}
	share = total / no_members;
	total = share * no_members;	/* round off */
	GET_GOLD(ch) -= total;
	GET_GOLD(ch) += total % no_members;	/* give back remainder and
						 * their share. */

	sprintf(buf, "You split the money and keep your share of %ld coins.", share + total % no_members);
	act(buf, FALSE, ch, 0, 0, TO_CHAR);

	sprintf(buf, "$n splits the money and you receive your share of %d coins.", share);

	if (IS_AFFECTED(lead, AFF_GROUP) &&
	    (lead->in_room == ch->in_room) &&
	    (IS_PC(lead))) {
		if ((MAX_GOLD - GET_GOLD(lead) - share) < 0) {
			send_to_char("ACK!  You drop your share because the sheer bulk of all your gold coins.\r\n", lead);
			act("You hear the distinctive sound of gold coins hitting the ground.", TRUE, ch, 0, 0, TO_ROOM);

			if (share > 0) {
				tmp_object = ha3400_create_money(share);
				ha2100_obj_to_room(tmp_object, lead->in_room);
				//GET_GOLD(lead) -= share;
				sprintf(buf2, "do_split: %s splits %ld with a share of %d", GET_REAL_NAME(ch), total, share);
				main_log(buf2);
			}
		}
		else
			GET_GOLD(lead) += share;
		if (lead != ch)
			act(buf, FALSE, ch, 0, lead, TO_VICT);
	}
	for (f = lead->followers; f; f = f->next)
		if (IS_AFFECTED(f->follower, AFF_GROUP)
		    && (f->follower->in_room == ch->in_room)
		    && (IS_PC(f->follower))) {
			if ((MAX_GOLD - GET_GOLD(f->follower) - share) < 0) {
				send_to_char("ACK!  You drop your share because the sheer bulk of all your gold coins.\r\n", f->follower);
				act("You hear the distinctive sound of gold coins hitting the ground.", TRUE, ch, 0, 0, TO_ROOM);


				if (share > 0) {
					tmp_object = ha3400_create_money(share);
					ha2100_obj_to_room(tmp_object, f->follower->in_room);
					//GET_GOLD(f->follower) = MAX_GOLD;
					sprintf(buf2, "do_split: %s splits %ld", GET_REAL_NAME(ch), total);
					main_log(buf2);

				}
			}
			else
				GET_GOLD(f->follower) += share;
			if (f->follower != ch)
				act(buf, FALSE, ch, 0, f->follower, TO_VICT);
		}
}				/* END OF ot3700_do_simple_split() */

void ot3800_do_split(struct char_data * ch, char *arg, int cmd)
{


	char buf[MAX_INPUT_LENGTH];
	int total;



	if (!IS_AFFECTED(ch, AFF_GROUP)) {
		act("You can't split up money when you're not part of a group.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	arg = one_argument(arg, buf);
	if (is_number(buf))
		total = atoi(buf);
	else {
		act("Split yes, fine but how much?", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if (total > GET_GOLD(ch)) {
		act("You dont have that much money.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	/* bugfix for immorts who are abusing this */

	if (GET_LEVEL(ch) > IMO_SPIRIT) {
		send_to_char("You try to split money with mortals, this will be reported!\n\r", ch);

		bzero(buf, sizeof(buf));
		sprintf(buf, "%s, level %d tried to split money with a mortal!",
			GET_REAL_NAME(ch), GET_LEVEL(ch));
		do_wizinfo(buf, IMO_IMM, ch);
		return;
	}
	else {
		/* This code checks to see if the person is a solo group */
		int no_members;
		struct char_data *lead;
		struct follow_type *f;
		//struct obj_data *tmp_object;


		/* will choke if the character isnt in a group */
		if (!IS_AFFECTED(ch, AFF_GROUP))
			return;

		sprintf(buf, "There were %d coins.\r\n", total);
		send_to_char(buf, ch);
		if (!(lead = ch->master))
			lead = ch;	/* find leader */

		if (IS_AFFECTED(lead, AFF_GROUP) && (lead->in_room == ch->in_room))
			no_members = 1;
		else
			no_members = 0;
		for (f = lead->followers; f; f = f->next)
			if (IS_AFFECTED(f->follower, AFF_GROUP)
			    && (f->follower->in_room == ch->in_room)
			    && (IS_PC(f->follower)))
				no_members += 1;

		if (no_members <= 1) {
			send_to_char("You can't split money with yourself!", ch);
		}

		else {
			ot3700_do_simple_split(ch, total);
		}
	}

}
 /* END OF ot3800_do_split() */

void ot3900_do_climb(struct char_data * ch, char *arg, int cmd)
{

	int lv_dir, lv_room, lv_flag, idx, rc;
	char buf[MAX_STRING_LENGTH];
	struct obj_data *lv_obj;
	struct room_direction_data *lv_exit;



	for (; *arg && *arg == ' '; arg++);

	arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("What do you want to climb?\r\n", ch);
		return;
	}

	lv_obj = ha2075_get_obj_list_vis(ch, buf,
					 world[ch->in_room].contents);

	if (!lv_obj) {
		send_to_char("I couldn't find that.\r\n", ch);
		return;
	}

	/* IS IT A CLIMBABLE TYPE? */
	if (!(GET_ITEM_TYPE(lv_obj) == ITEM_CLIMBABLE)) {
		send_to_char("Sorry, you can't climb that.\r\n", ch);
		return;
	}

	/* DID USER SPECIFY A DIRECTION? */
	arg = one_argument(arg, buf);
	if (!*buf) {
		/* WHERE DOES THIS ITEM GO? */
		lv_room = 0;
		if (lv_obj->obj_flags.value[0] > 0 &&
		    lv_obj->obj_flags.value[0] < 32767) {
			lv_room = db8000_real_room(lv_obj->obj_flags.value[0]);
		}

		if (lv_room < 1) {
			/* LOOK FOR A DEFAULT DIRECTION */
			for (idx = 0; lv_room < 1 && idx < MAX_DIRS; idx++) {
				if (EXIT(ch, idx) &&
				    EXIT(ch, idx)->to_room > 0 &&
				    IS_SET(EXIT(ch, idx)->exit_info, EX_CLIMB_ONLY)) {
					rc = ha9800_protect_char_from_dt(ch, EXIT(ch, idx)->to_room, 0);
					if (rc)
						return;
					sprintf(buf, "You begin climbing %s.\r\n",
						dirs[idx]);
					send_to_char(buf, ch);
					lv_room = EXIT(ch, idx)->to_room;
				}
			}
			/* IF WE STILL DIDN'T FIND A ROOM */
			if (lv_room < 1) {
				act("You try to climb $p. but don't get anywhere.",
				    FALSE, ch, lv_obj, 0, TO_CHAR);
				return;
			}
		}
		rc = li3000_is_blocked_from_room(ch, lv_room, 0);
		if (rc) {
			li3100_send_blocked_text(ch, rc, 0);
			return;
		}
		rc = ha9800_protect_char_from_dt(ch, lv_room, 0);
		if (rc)
			return;
		sprintf(buf, "You climb $p.");
		act(buf, TRUE, ch, lv_obj, 0, TO_CHAR);
		sprintf(buf, "$n climbs $p.");
		act(buf, TRUE, ch, lv_obj, 0, TO_ROOM);

		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, lv_room);

		act("$n climbs into the room.", TRUE, ch, lv_obj, 0, TO_ROOM);
		/* Is it a DT?  --ff */
		if (IS_SET(world[lv_room].room_flags, RM1_DEATH) &&
		    GET_LEVEL(ch) < IMO_SPIRIT &&
		    !(ha1175_isexactname(GET_NAME(ch), zone_table[world[ch->in_room].zone].lord))) {
			in3000_do_look(ch, "", 0);
			ha1750_remove_char_via_death_room(ch, LEFT_BY_DEATH_ROOM);
			return;
		}

		in3000_do_look(ch, "", 0);
		return;
	}
	lv_dir = old_search_block(buf, 0, strlen(buf), dirs, 0);
	if ((lv_dir < 0) ||
	    !(EXIT(ch, lv_dir - 1))) {
		send_to_char("Thats not a valid direction.\r\n", ch);
		return;
	}
	lv_dir--;
	/* WE ARE LEAVING THE ROOM VIA A CLIMB EXIT */

	sprintf(buf, "You climb %s on $p.", dirs[lv_dir]);
	act(buf, TRUE, ch, lv_obj, 0, TO_CHAR);

	sprintf(buf, "$n climbs %s using $p.", dirs[lv_dir]);
	act(buf, TRUE, ch, lv_obj, 0, TO_ROOM);

	strcpy(buf, dirs[lv_dir]);

	lv_flag = 0;
	lv_exit = EXIT(ch, lv_dir);
	if (lv_exit) {
		lv_flag = lv_exit->exit_info;
		REMOVE_BIT(lv_exit->exit_info, EX_CLIMB_ONLY);
	}

	mo1500_do_move(ch, buf, lv_dir + 1);

	/* PUT THE FLAG BACK */
	if (lv_flag) {
		lv_exit->exit_info = lv_flag;
	}

	return;

}				/* END OF ot3900_do_climb() */


/* this routine is called with a spell number.  It looks for a worn
   item of  with that type of spell */
struct obj_data *ot4000_check_spell_store_item(struct char_data * ch, int av_spell)
{

	int idx, lv_num;
	struct obj_data *lv_obj;



	for (idx = 0; idx < MAX_WEAR; idx++) {
		if ((lv_obj = ch->equipment[idx])) {
			if (lv_obj->obj_flags.value[VALUE_SPELL] == av_spell) {
				if (lv_obj->obj_flags.value[VALUE_CURR_CHARGE] <
				 lv_obj->obj_flags.value[VALUE_MAX_CHARGES]) {
					lv_num = obj_index[lv_obj->item_number].virtual;
				}	/* END OF IT CAN BE RECHARGED */
			}	/* END OF FOUND THE SPELL */
		}		/* END OF SOMETHING AT LOCATION */
	}			/* END OF FOR LOOP */
	return (0);

}				/* END OF ot4000_check_spell_store_item() */

void ot4100_do_sense(struct char_data * ch, char *arg, int cmd)
{

	int lv_dir, lv_room;
	char buf[MAX_STRING_LENGTH];
	struct char_data *vict;

	/* CHECK IF POISONED */
	if (GET_LEVEL(ch) < IMO_SPIRIT && IS_AFFECTED(ch, AFF_POISON)) {
		send_to_char("You're poisoned and can't sense a thing.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	/* IS THERE AN ARGUMENT GIVEN */
	if (!*buf) {
		send_to_char("Sense in what direction?\r\n", ch);
		return;
	}

	/* IS ARGUMENT A DIRECTION */
	lv_dir = old_search_block(buf, 0, strlen(buf), dirs, 0);
	lv_dir--;
	if ((lv_dir < 0) || (lv_dir > 5)) {
		send_to_char("Thats not a valid direction.\r\n", ch);
		return;
	}

	/* IS THERE A ROOM IN THAT DIRECTION */
	if (!world[ch->in_room].dir_option[lv_dir]) {
		send_to_char("That direction only exists in your imagination.\r\n", ch);
		return;
	}

	/* ARE THERE HIDDEN OR AGGRESIVE MOBS IN THAT DIRECTION */
	sprintf(buf, "You try to sense what is %s of you.\r\n", dirs[lv_dir]);
	send_to_char(buf, ch);
	lv_room = world[ch->in_room].dir_option[lv_dir]->to_room;
	for (vict = world[lv_room].people; vict; vict = vict->next_in_room) {
		if ((ch != vict) &&
		    ((IS_AFFECTED(vict, AFF_HIDE)) ||
		     (IS_SET(GET_ACT1(vict), PLR1_AGGRESSIVE_EVIL)) ||
		     (IS_SET(GET_ACT1(vict), PLR1_AGGRESSIVE_NEUT)) ||
		     (IS_SET(GET_ACT1(vict), PLR1_AGGRESSIVE_GOOD)))) {
			send_to_char("The hairs on the back of your neck stand up!\r\n", ch);
			return;
		}
	}
	send_to_char("You sense nothing at all.\r\n", ch);
	return;

}				/* END OF ot4100_do_sense () */

void ot4200_do_wizlist(struct char_data * ch, char *argument, int cmd)
{

	int max = 75, min = 48;
	int idx, jdx, i;
	int lv_seconds_gone, lv_months, lv_days, lv_hours, lv_minutes;

	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];

	const char *toggle[] = {
		"all",
		"spirit",
		"avatar",
		"gods",
		"42",
		"\n"
	};


	send_to_char(" &RLv &BName&E\r\n", ch);
	send_to_char(" &r-- &b------------&E\r\n", ch);


	for (; isspace(*argument); argument++);
	bzero(buf1, sizeof(buf1));
	argument = one_argument(argument, buf1);

	if (*buf1)
		i = (old_search_block(buf1, 0, strlen(buf1), toggle, 0));
	else
		i = -1;

	switch (i) {
	case 1:
		max = 75;
		min = 41;
		break;
	case 2:
		max = 47;
		min = 47;
		break;
	case 3:
		max = 41;
		min = 41;
		break;
	case 4:
		max = 75;
		min = 48;
		break;
	case 5:
		max = 42;
		min = 42;
		break;
	case -1:
		max = 75;
		min = 48;
		break;
	default:
		send_to_char("options are: 'spirit', 'avatar', '42', 'god' or 'all'.", ch);
		send_to_char("No argument will give the god option", ch);
		return;
	}


	for (jdx = max; jdx >= min; jdx--)
		for (idx = 0; idx <= top_of_p_table; idx++) {
			if (player_table[idx].pidx_level == jdx) {


				sprintf(buf, "%3d %-20s",
					player_table[idx].pidx_level,
					player_table[idx].pidx_name);




				bzero(buf1, sizeof(buf1));

				if (GET_LEVEL(ch) >= IMO_IMP || IMO_IMP) {

					if (player_table[idx].pidx_last_on > 0) {

						lv_seconds_gone = time(0) - player_table[idx].pidx_last_on;
						lv_months = 0;
						lv_days = 0;
						lv_hours = 0;
						lv_minutes = 0;


						if (lv_seconds_gone > 2592000)
							lv_months = (int) (lv_seconds_gone / 2592000);
						lv_seconds_gone = lv_seconds_gone - (lv_months * 2592000);


						if (lv_seconds_gone > 86400)
							lv_days = (int) (lv_seconds_gone / 86400);
						lv_seconds_gone = lv_seconds_gone - (lv_days * 86400);

						if (lv_seconds_gone > 3600)
							lv_hours = (int) (lv_seconds_gone / 3600);
						lv_seconds_gone = lv_seconds_gone - (lv_hours * 3600);


						if (lv_seconds_gone > 60)
							lv_minutes = (int) (lv_seconds_gone / 60);
						lv_seconds_gone = lv_seconds_gone - (lv_minutes * 60);

						if (GET_LEVEL(ch) >= IMO_IMM) {
							sprintf(buf1, "%-25s Gone %d months, %d days, %d hours, %d minutes, %d seconds.\r\n",
								buf, lv_months, lv_days, lv_hours, lv_minutes, lv_seconds_gone);
						}
						else {
							sprintf(buf1, "%-25s\r\n", buf);
						}

					}
				}
				else
					sprintf(buf1, "%s\r\n", buf);

				if (buf[4] != UPPER(buf[4])) {
					send_to_char(buf1, ch);
				}


			}
		}
}

void ot4300_do_rushtime(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	if (gv_rush_hour > 1) {
		sprintf(buf, "Rush time left: %d hours %d minutes and %d seconds.\r\n",
			get_jail_time(gv_rush_time, 2),
			get_jail_time(gv_rush_time, 3),
			get_jail_time(gv_rush_time, 4));
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "Next rush in %d hours %d minutes and %d seconds.\r\n",
			get_jail_time(gv_rush_time, 2),
			get_jail_time(gv_rush_time, 3),
			get_jail_time(gv_rush_time, 4));
		send_to_char(buf, ch);
	}
	return;
}

void ot4400_do_rushhour(struct char_data * ch, char *argument, int cmd)
{

	char *arg;
	char buf[MAX_STRING_LENGTH];
	long lv_time = 10;

	arg = argument;

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (*buf)
		lv_time = atoi(buf);

	if (gv_rush_hour == 1) {
		if (dice(1,100) >= 60){
		gv_rush_hour = 3;
		gv_rush_time = time(0) + lv_time * 60 - 1;
		sprintf(buf, "Mega Rush has started, Time left: %ld min!", lv_time);
		do_info_noch(buf, 0, 99);
		sprintf(buf, "RUSH: %s made a %ld minute rush.", GET_REAL_NAME(ch), lv_time);
		spec_log(buf, GOD_COMMAND_LOG);
		main_log(buf);
		}
		else {
		gv_rush_hour = 2;
		gv_rush_time = time(0) + lv_time * 60 - 1;
		sprintf(buf, "Rush has started, Time left: %ld min!", lv_time);
		do_info_noch(buf, 0, 99);
		sprintf(buf, "RUSH: %s made a %ld minute rush.", GET_REAL_NAME(ch), lv_time);
		spec_log(buf, GOD_COMMAND_LOG);
		main_log(buf);	
		}
	}
	else {
		gv_rush_hour = 1;
		gv_rush_time = time(0) + RUSH_INTERVAL * 60 +
			number(0, RUSH_INTERVAL2) * 60;
		do_info_noch("Rush has ended.", 0, 99);
	}
	return;
}

void ot4500_do_for(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char *pbuf, *pname;
	struct descriptor_data *d;
	int lv_min_lvl, lv_max_lvl, lv_type, lv_count, lv_min_interval, lv_max_interval,
	  idx, jdx;
	const char *lv_for_types[] = {
		"all",		/* 1 */
		"level",	/* 2 */
		"mortals",	/* 3 */
		"avatars",	/* 4 */
		"gods",		/* 5 */
		"quest",	/* 6 */
		"count",	/* 7 */
		"interval",	/* 8 */
		"\n",
	};

#define FOR_ALL		1
#define FOR_LEVEL	2
#define FOR_MORTAL	3
#define FOR_AVATAR 	4
#define FOR_GOD		5
#define FOR_QUEST       6
#define FOR_COUNT	7
#define FOR_INTERVAL	8

	strcpy(buf, arg);
	for (idx = 0; idx < strlen(buf); idx++)
		buf[idx] = LOWER(buf[idx]);
	for (idx = 0; idx < strlen(buf); idx++) {
		if (strncmp((char *) (buf + idx), "dofor", 5) == 0) {
			send_to_char("I don't think so....\r\n", ch);
			return;
		}
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	lv_type = 0;
	if (*buf)
		lv_type = old_search_block(buf, 0, strlen(buf), lv_for_types, 0);
	if (lv_type < 1) {
		send_to_char("Not a valid option. Please use all, level, mortals, avatars, gods, quest, count, interval\r\n", ch);
		return;
	}

	switch (lv_type) {
	case FOR_QUEST:
	case FOR_ALL:{
			lv_min_lvl = 1;
			lv_max_lvl = GET_LEVEL(ch);
			break;
		}
	case FOR_MORTAL:{
			lv_min_lvl = 1;
			lv_max_lvl = IMO_LEV - 1;
			break;
		}
	case FOR_AVATAR:{
			lv_min_lvl = IMO_LEV;
			lv_max_lvl = IMO_LEV;
			break;
		}
	case FOR_GOD:{
			lv_min_lvl = IMO_SPIRIT;
			lv_max_lvl = GET_LEVEL(ch);
			break;
		}
	case FOR_COUNT:{
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
			if ((!*buf) || (!is_number(buf))) {
				send_to_char("Need a <nr> for dofor count.\r\n", ch);
				return;
			}
			lv_count = atoi(buf);
			if (lv_count < 1) {
				send_to_char("Lol, you really can't count can you?\r\n", ch);
				return;
			}
			for (jdx = 0; jdx < lv_count; jdx++) {
				sprintf(buf2, "%d", jdx);
				bzero(buf, sizeof(buf));
				pbuf = buf;
				for (idx = 0; *(arg + idx); idx++) {
					if (*(arg + idx) == '#') {
						pname = buf2;
						for (; *pname; *(pbuf++) = *(pname++));
					}
					else
						*(pbuf++) = *(arg + idx);
				}
				*pbuf = 0;

				command_interpreter(ch, buf);
			}
			break;
		}
	case FOR_INTERVAL:{
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
			for (; isspace(*arg); arg++);
			bzero(buf2, sizeof(buf2));
			arg = one_argument(arg, buf2);

			if (!(*buf) || !(*buf2) || !is_number(buf) || !is_number(buf2)) {
				send_to_char("Please use 'for interval <min_interval> <max_interval>' with numeric values.\r\n", ch);
				return;
			}
			lv_min_interval = MAXV(1, atoi(buf));
			lv_max_interval = MINV(32767, atoi(buf2));
			if (lv_min_interval > lv_max_interval) {
				send_to_char("min_interval has to be lower than max_interval.\r\n", ch);
				return;
			}

			for (jdx = lv_min_interval; jdx < lv_max_interval; jdx++) {
				sprintf(buf2, "%d", jdx);
				bzero(buf, sizeof(buf));
				pbuf = buf;
				for (idx = 0; *(arg + idx); idx++) {
					if (*(arg + idx) == '#') {
						pname = buf2;
						for (; *pname; *(pbuf++) = *(pname++));
					}
					else
						*(pbuf++) = *(arg + idx);
				}
				*pbuf = 0;

				command_interpreter(ch, buf);
			}
			break;
		}
	case FOR_LEVEL:{
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
			for (; isspace(*arg); arg++);
			bzero(buf2, sizeof(buf2));
			arg = one_argument(arg, buf2);

			if (!(*buf) || !(*buf2) ||
			    !is_number(buf) || !is_number(buf2)) {
				send_to_char("Please use 'for level <min_level> <max_level>' with numeric levels.\r\n", ch);
				return;
			}
			lv_min_lvl = MAXV(1, atoi(buf));
			lv_max_lvl = MINV(99, atoi(buf2));
			if (lv_min_lvl > lv_max_lvl) {
				send_to_char("min_level has to be lower than max_level.\r\n", ch);
				return;
			}
			break;
		}
	default:{
			send_to_char("Error in for code.\r\n", ch);
			return;
		}
	}			/* end of switch */


	for (d = descriptor_list; d; d = d->next) {
		if (d->character &&
		    (d->connected == CON_PLAYING) &&
		    (d->character->in_room >= 0) &&
		    (d->character != ch) &&
		    (d->character->player.level >= lv_min_lvl) &&
		    (d->character->player.level <= lv_max_lvl)) {
			/* check for # in the command */
			bzero(buf, sizeof(buf));
			pbuf = buf;
			for (idx = 0; *(arg + idx); idx++) {
				if (*(arg + idx) == '#') {
					pname = d->character->player.name;
					for (; *pname; *(pbuf++) = *(pname++));
				}
				else
					*(pbuf++) = *(arg + idx);
			}
			*pbuf = 0;
			sprintf(buf2, "%s %s", d->character->player.name, buf);
			if ((lv_type != FOR_QUEST) ||
			    (IS_SET(GET_ACT2(d->character), PLR2_QUEST)))
				wi1400_do_at(ch, buf2, cmd);
		}
	}
	return;
}

void ot4600_do_forget(struct char_data * ch, char *arg, int cmd)
{

	int slevel;
	int lv_what_to_practice;
	char buf[MAX_STRING_LENGTH];

	for (; (arg && *arg == ' '); arg++);

	lv_what_to_practice = 0;
	if (is_number(arg)) {
		lv_what_to_practice = atoi(arg);
		if (lv_what_to_practice < 0 || lv_what_to_practice > NUM_SKILLS ||
		!(strncmp(spell_names[lv_what_to_practice], "undefined", 9))) {
			send_to_char("Sorry, but that is not a valid spell/skill.\r\n",
				     ch);
			return;
		}
	}
	else {
		/* SEE IF WE CAN FIND IT BY NAME */
		lv_what_to_practice =
			old_search_block(arg, 0, strlen(arg), skill_names, FALSE);

		if (lv_what_to_practice > 0) {
			/* WE FOUND IT IN THE SKILL LIST SO CONVERT TO REAL
			 * NUMBER */
			lv_what_to_practice = skill_numbers[lv_what_to_practice - 1];
		}
		else {
			/* IT WASN'T A SKILL SO SEARCH FOR A SPELL */
			lv_what_to_practice =
				old_search_block(arg, 0, strlen(arg), spell_names, FALSE);
		}
	}

	if (lv_what_to_practice < 1) {
		send_to_char("Sorry, but that is not a valid spell/skill.\r\n", ch);
		return;
	}

	/* ARE WE ALREADY LEARNED IN THIS AREA? */
	if (GET_SKILL(ch, lv_what_to_practice) < 1) {
		send_to_char("How can you forget something you do not know?\r\n", ch);
		return;
	}

	if (classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice] > races[GET_RACE(ch)].skill_min_level[lv_what_to_practice])
		slevel = races[GET_RACE(ch)].skill_min_level[lv_what_to_practice];
	else
		slevel = classes[GET_CLASS(ch)].skill_min_level[lv_what_to_practice];


	if (slevel != IMO_LEV && slevel != PK_LEV) {
		send_to_char("You can't seem to be able to forget this spell.\r\n", ch);
		return;
	}

	GET_SKILL(ch, lv_what_to_practice) = 0;
	ch->points.fspells = MAXV(0, ch->points.fspells - 1);
	sprintf(buf, "You forget %s.\r\n", spell_names[lv_what_to_practice - 1]);
	send_to_char(buf, ch);
}

int ot4700_count_foreign_spells(struct char_data * ch)
{

	int slevel;
	int idx;
	int count = 0;

	for (idx = 1; idx < MAX_SKILLS; idx++) {
		/* HAVE WE EXCEEDED VALID SKILLS? */
		if (spell_names[idx - 1][0] == '\n') {
			idx = MAX_SKILLS;
			continue;
		}
		if (classes[GET_CLASS(ch)].skill_min_level[idx] > races[GET_RACE(ch)].skill_min_level[idx])
			slevel = races[GET_RACE(ch)].skill_min_level[idx];
		else
			slevel = classes[GET_CLASS(ch)].skill_min_level[idx];

		if (slevel != IMO_LEV && slevel != PK_LEV)
			continue;
		if (GET_SKILL(ch, idx) < 1)
			continue;
		count++;
	}

	return (count);
}

void ot4800_do_airwalk(struct char_data * ch, char *argument, int cmd)
{
	struct affected_type af;
	signed char percent;



	if ((GET_RACE(ch) != RACE_PIXIE) &&
	    (GET_LEVEL(ch) < IMO_SPIRIT)) {
		act("You don't have tiny pixie wings to fly with.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n hops up and down and tries to imitate a pixie.", FALSE, ch, 0, 0, TO_ROOM);
		return;
	}

	if (IS_AFFECTED(ch, AFF_FLY)) {
		send_to_char("You are already walking on air!\r\n", ch);
		return;
	}
	else
		send_to_char("You spread your tiny wings and start to walk on air.\n\r", ch);

	WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
	WAIT_STATE(ch, PULSE_VIOLENCE);

	percent = number(1, 101);	/* 101% is a complete failure */

	if (percent > (ch->skills[SKILL_AIRWALK].learned +
		       li9650_dex_adjustment(ch, 1)))
		return;

	af.type = SKILL_AIRWALK;
	af.duration = GET_LEVEL(ch) / 4;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_FLY;
	ha1300_affect_to_char(ch, &af);

	af.location = APPLY_NONE;
	af.bitvector = AFF_WATERWALK;
	ha1300_affect_to_char(ch, &af);

	li9900_gain_proficiency(ch, SKILL_AIRWALK);
	return;
}				/* END OF ot4800_do_airwalk() */
#define PK_QUEST_FINAL_ROOM 3433
#define OBJ_PK_CERTIFICATE 30

//Advancing to pk level(testing only atm, final version will change) - Bingo
	void advance_pk_level(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	struct obj_data *obj_next;
	int hitp_bonus, mana_bonus, move_bonus;
	bool obj_found;

	if (cmd != CMD_ADHERE)
		return;

	if (gv_port != DEVELOPMENT_PORT) {
		send_to_char("This command is not available at this time.\n\r", ch);
		return;
	}

	arg = one_argument(arg, arg1);
	arg = one_argument(arg, arg2);

	obj = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);

	if (!*arg1 || !*arg2) {
		send_to_char("Usage: adhere <certificate> [YES | NO]\n\r"
		 "Note: YES or NO *MUST* be in all caps for comfirmation.\n\r"
		 "      If you say YES you will become a full on Pkiller.\n\r"
		  "      If you say NO you will be sent back to Midgaard.\n\r"
			     "Requirements:\n\r"
		   "             You must have a kill/death score of 10+.\n\r"
			     "             You must have 10+ quest points.\n\r"
		    "             You must have 100+ hp, mana, and moves.\n\r"
			     "             You must have YOUR own personal pk certificate.\n\r", ch);
		return;
	}

	if (GET_LEVEL(ch) == PK_LEV) {
		send_to_char("You are already a Pkiller!\n\r", ch);
		return;
	}
	if (GET_LEVEL(ch) < IMO_LEV || GET_LEVEL(ch) > PK_LEV) {
		send_to_char("You MUST be an Avatar to obtain Pkiller status!", ch);
		return;
	}

	if (!str_cmp(arg2, "YES")) {

		if (world[ch->in_room].number != PK_QUEST_FINAL_ROOM) {
			send_to_char("You can only adhere to the pkilling ways in the Room of Adherence!\n\r", ch);
			return;
		}

		if (GET_MAX_HIT(ch) < 100 || GET_MAX_MANA(ch) < 100 || GET_MAX_MOVE(ch) < 100) {
			send_to_char("You must have 100 hp, mana, and move to become a Pkiller!\n\r", ch);
			return;
		}

		if (GET_SCORE(ch) < 10) {
			send_to_char("You must have a kill/death score of 10 to become a Pkiller!\n\r", ch);
			return;
		} //change to 10000 after testing phase

			if (GET_QPS(ch) < 10) {
			send_to_char("You must have 10 quest points to become a Pkiller!\n\r", ch);
			return;
		} //change to 10000 after testing phase

			if (ch->carrying) {
			for (obj = ch->carrying; obj; obj = obj_next) {
				obj_found = FALSE;
				obj_next = obj->next_content;
				if (GET_OBJ_VNUM(obj) == OBJ_PK_CERTIFICATE &&
				    OVAL0(obj) == ch->nr) {
					if (GET_OBJ_VNUM(obj) == OBJ_PK_CERTIFICATE &&
					    OVAL0(obj) != ch->nr) {
						send_to_char("You must have YOUR pk certificate to adhere to the pkilling ways!\n\r", ch);
						return;
					}
					obj_found = TRUE;
					break;
				}
			}
			if (!obj_found) {
				send_to_char("You must have a pk certificate to adhere to the pkilling ways!\n\r", ch);
				return;
			}
			act("&BYour &y$p &Bvanishes into thin air.&n", TRUE, ch, obj, 0, TO_CHAR);
			ha2700_extract_obj(obj);
		}

		if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
			hitp_bonus = 20;
			mana_bonus = 50;
			move_bonus = 20;
		}
		if (GET_CLASS(ch) == CLASS_CLERIC) {
			hitp_bonus = 25;
			mana_bonus = 40;
			move_bonus = 25;
		}
		if (GET_CLASS(ch) == CLASS_THIEF) {
			hitp_bonus = 40;
			mana_bonus = 0;
			move_bonus = 50;
		}
		if (GET_CLASS(ch) == CLASS_WARRIOR) {
			hitp_bonus = 50;
			mana_bonus = 0;
			move_bonus = 40;
		}
		if (GET_CLASS(ch) == CLASS_BARD) {
			hitp_bonus = 30;
			mana_bonus = 30;
			move_bonus = 30;
		}
		if (GET_CLASS(ch) == CLASS_PRIEST) {
			hitp_bonus = 30;
			mana_bonus = 35;
			move_bonus = 25;
		}
		if (GET_CLASS(ch) == CLASS_PALADIN) {
			hitp_bonus = 40;
			mana_bonus = 20;
			move_bonus = 30;
		}

		if (GET_LEVEL(ch) == 41) {
			GET_LEVEL(ch) += 1;
		}

		GET_MAX_HIT(ch) += hitp_bonus;
		GET_MAX_MANA(ch) += mana_bonus;
		GET_MAX_MOVE(ch) += move_bonus;

		GET_SCORE(ch) = 0;
		//May or may not make score / deaths / kills hit 0 once they change
			GET_DEATHS(ch) = 0;
		GET_KILLS(ch) = 0;
		GET_QPS(ch) -= 10;
		//change to 10000 after testing phase

			send_to_char("\n\r&RCongratulations, you have completed the quest and joined the ranks of pkiller!&n\n\r", ch);
		sprintf(buf, "&WYou have gained &G%d &Whp, &C%d &Wmana, and &Y%d &Wmoves.&n\n\r", hitp_bonus, mana_bonus, move_bonus);
		send_to_char(buf, ch);
		send_to_char("&KYour kills, your deaths, and your kill/death score have been reset to &w0&K.&n\n\r", ch);
		send_to_char("&rEnjoy the world of pkilling, for it is a harsh one, but has many rewards.&n", ch);
		send_to_char("\n\r&CYou are now being transported to the Dressing Room of Midgaard.&n\n\r"
		     "\n\r&cYou are now in the Dressing Room.&n\n\r\n\r", ch);

		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, db8000_real_room(DRESSING_ROOM));

		sprintf(buf, "%s has adhered to the ranks of &RP&Kk&wi&Wl&wl&Ke&Rr&n and is now level %d!",
			GET_REAL_NAME(ch),
			GET_LEVEL(ch));
		do_info_noch(buf, 0, 99);

		sprintf(buf, "&K%s &rarrives in a dark red mist.&n", GET_REAL_NAME(ch));
		act(buf, TRUE, ch, 0, 0, TO_NOTCHAR);

	}
	else if (!str_cmp(arg2, "NO")) {

		if (world[ch->in_room].number != PK_QUEST_FINAL_ROOM) {
			send_to_char("You can only use adhere in the Room of Adherence!\n\r", ch);
			return;
		}

		send_to_char("&YChickening out huh?  &GOh well, have fun!&n", ch);
		send_to_char("\n\r&CYou are now being transported to the Dressing Room of Midgaard.&n\n\r"
		     "\n\r&cYou are now in the Dressing Room.&n\n\r\n\r", ch);

		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, db8000_real_room(DRESSING_ROOM));

		sprintf(buf, "&B%s &Carrives in a weak blue mist.&n", GET_REAL_NAME(ch));
		act(buf, TRUE, ch, 0, 0, TO_NOTCHAR);

		return;
	}
	else {
		send_to_char("You must use either YES or NO in all caps!\n\r", ch);
		return;
	}

}				/* end of advance_pk_level() */

void ot5000_transform_update(void)
{

	struct char_data *ch;
	struct descriptor_data *d;

	/* Check which players are doing transform and how much time is left */
	for (d = descriptor_list; d; d = d->next) {
		if (d->character &&
		    (d->connected == CON_PLAYING) &&
		    (d->character->in_room >= 0)) {
			ch = d->character;
			if (ch->nextcast && (ch->nextcast < time(0))) {
				send_to_char("Your body can once again handle the stress of a transformation.\r\n", ch);
				ch->nextcast = 0;
			}
		}
	}
}				/* END OF ot5000_transform_update() */
#define OBJ_WRAPS 13302
void ot5100_wraps_procedure(struct char_data * ch, struct obj_data * obj)
{

	if ((ch->equipment[WEAR_LIGHT]) && (obj_index[ch->equipment[WEAR_LIGHT]->item_number].virtual == OBJ_WRAPS)) {
		if (IS_AFFECTED(ch, AFF_HIDE))
			REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);
		SET_BIT(ch->specials.affected_by, AFF_HIDE);
		return;
	}
}				/* End of ot5100_wraps_procedure() */

int does_player_exist(char *name)
{

	int idx;

	for (idx = 0; idx <= top_of_p_table; idx++) {
		if (!strcasecmp(player_table[idx].pidx_name, name))
			return player_table[idx].pidx_level;
	}
	return FALSE;
}
