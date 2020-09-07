/* au */
/* *******************************************************************
*  file: auction.c , Special module.         Part of Crimson MUD     *
*  Usage: Auction system for Crimson MUD II                          *
*                  Written by Hercules   asdfasdf                            *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

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

char *show_with_dots(int value)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int idx, jdx;

	bzero(buf, sizeof(buf));
	sprintf(buf2, "%d", value);

	for (jdx = idx = 0; idx < strlen(buf2); idx++) {
		buf[jdx++] = buf2[idx];
		if (idx % 3 == (strlen(buf2) - 1) % 3)
			buf[jdx++] = '.';
	}

	buf[strlen(buf) - 1] = 0;
	strcpy(gv_str, buf);
	return (gv_str);

}				/* end of show_with_dots() */



void remove_dots(char *lv_input)
{

	char buf[MAX_STRING_LENGTH];
	int idx, jdx;

	if (!*lv_input)
		return;

	bzero(buf, sizeof(buf));
	strcpy(buf, lv_input);

	for (idx = jdx = 0; idx < strlen(buf); idx++)
		if (buf[idx] != '.')
			lv_input[jdx++] = buf[idx];

	lv_input[jdx++] = 0;

}				/* end of remove_dots() */


void au999_do_auction_identify(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	struct obj_data *obj;
	int i;
	bool found;

	obj = auction_data.obj;
	if (!obj) {
		send_to_char("There's nothing being auctioned!\n\r", ch);
		return;
	}


	if (IS_NPC(ch) && (ch->master)) {
		send_to_char("Charmies don't need to know!\n\r", ch->master);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("You cannot auction while you are jailed!\n\r", ch);
		return;
	}

	if (IS_SET(GET_OBJ2(obj), OBJ2_NO_IDENTIFY)) {
		send_to_char("Sorry, no information available on this item.\r\n", ch);
		return;
	}

	if (GET_GOLD(ch) < 5780 && GET_LEVEL(ch) < IMO_IMM) {
		send_to_char("You don't seem to have enough money, sorry.\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMM)
		GET_GOLD(ch) -= 5780;

	if (obj) {
		send_to_char("Currently being auctioned:\n\r--------------------------\n\r", ch);
		sprintf(buf, "Item          : %s\n\r", GET_OSDESC(obj));
		send_to_char(buf, ch);
		sprintf(buf, "Current bid   : %s\n\r",
			show_with_dots(auction_data.bidder ? auction_data.bidder->bid : auction_data.price));
		send_to_char(buf, ch);
		sprintf(buf, "Minimum raise : %s\n\r", show_with_dots(auction_data.raise));
		send_to_char(buf, ch);
		sprintf(buf, "Seller        : %s\n\r", GET_REAL_NAME(auction_data.auctioner));
		send_to_char(buf, ch);
		if (auction_data.bidder) {
			sprintf(buf, "Bidder        : %s\n\r\n\r\n\r", GET_REAL_NAME(auction_data.bidder->current));
			send_to_char(buf, ch);
		}
		else
			send_to_char("Bidder        : None\n\r\n\r\n\r", ch);

		send_to_char("Item information:\n\r----------------\n\r", ch);
		sprintf(buf, "Name               : '%s'\n\rItem type          : ", obj->name);
		sprinttype(GET_ITEM_TYPE(obj), item_types, buf2);
		strcat(buf, buf2);
		strcat(buf, "\n\r");
		send_to_char(buf, ch);

		if (obj->obj_flags.bitvector) {
			send_to_char("Abilities          : ", ch);
			sprintbit(obj->obj_flags.bitvector, affected_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
		}

		send_to_char("Flags              : ", ch);
		sprintbit(obj->obj_flags.flags1, oflag1_bits, buf);
		send_to_char(buf, ch);
		sprintbit(obj->obj_flags.flags2, oflag2_bits, buf);
		if ((OVAL1(obj) > 0) &&
		    (OVAL1(obj) < 36) &&
		    (GET_ITEM_TYPE(obj) == ITEM_ARMOR)) {
			sprintf(buf2, "ANTI_%s ", race_list[OVAL1(obj)]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if ((OVAL1(obj) > 50) &&
		    (OVAL1(obj) < 101) &&
		    (GET_ITEM_TYPE(obj) == ITEM_ARMOR)) {
			sprintf(buf2, "%s_ONLY ", race_list[OVAL1(obj) - 50]);
			strcat(buf, buf2);
			send_to_char(buf, ch);
		}
		if (GET_ITEM_TYPE(obj) == ITEM_ARMOR &&
		    OVAL2(obj) > 0) {
			sprintbit(OVAL2(obj), class_anti, buf);
			strcat(buf, "\0");
		}
		strcat(buf, "\n\r");
		send_to_char(buf, ch);

		/* Show them worn flags. */
		send_to_char("Can be worn on     : ", ch);
		sprintbit(obj->obj_flags.wear_flags, wear_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);

		/* Timer: */
		if (GET_TIMER(obj) == -1)
			sprintf(buf, "Timer              : No Timer\r\n");
		else
			sprintf(buf, "Timer              : %d\r\n", GET_TIMER(obj));
		send_to_char(buf, ch);

		sprintf(buf, "Weight             : %d\n\rValue              : %d\n\r\n\r",
			obj->obj_flags.weight, obj->obj_flags.cost);
		send_to_char(buf, ch);

		switch (GET_ITEM_TYPE(obj)) {

		case ITEM_SCROLL:
		case ITEM_POTION:
			sprintf(buf, "Level %d spells of :\n\r", obj->obj_flags.value[0]);
			send_to_char(buf, ch);
			if (obj->obj_flags.value[1] >= 1) {
				sprinttype(obj->obj_flags.value[1] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			if (obj->obj_flags.value[2] >= 1) {
				sprinttype(obj->obj_flags.value[2] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			if (obj->obj_flags.value[3] >= 1) {
				sprinttype(obj->obj_flags.value[3] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
		case ITEM_SPELL:
			sprintf(buf, "Has %d charges, with %d charges left.\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			send_to_char(buf, ch);

			sprintf(buf, "Level %d spell of:\n\r", obj->obj_flags.value[0]);
			send_to_char(buf, ch);

			if (obj->obj_flags.value[3] >= 1) {
				sprinttype(obj->obj_flags.value[3] - 1, spell_names, buf);
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			}
			break;

		case ITEM_WEAPON:
			send_to_char("Weapons bits       : ", ch);
			sprintbit(obj->obj_flags.value[0], weapon_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "Damage Dice is '%dD%d'\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			send_to_char(buf, ch);
			break;

		case ITEM_ARMOR:
			sprintf(buf, "AC-apply is %d\n\r",
				obj->obj_flags.value[0]);
			send_to_char(buf, ch);
			break;

		case ITEM_QUEST:
			sprintf(buf, "AC-apply is %d\n\r",
				obj->obj_flags.value[0]);
			send_to_char(buf, ch);
			break;

		case ITEM_QSTWEAPON:
			send_to_char("Weapons bits: ", ch);
			sprintbit(obj->obj_flags.value[0], weapon_bits, buf);
			strcat(buf, "\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "Damage Dice is '%dD%d'\n\r",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			send_to_char(buf, ch);
			break;

		}		/* end of switch */

		found = FALSE;
		for (i = 0; i < MAX_OBJ_AFFECT; i++) {
			if ((obj->affected[i].location != APPLY_NONE) &&
			    (obj->affected[i].modifier != 0)) {
				if (!found) {
					send_to_char("Can affect you as  :\n\r", ch);
					found = TRUE;
				}

				sprinttype(obj->affected[i].location, apply_types, buf2);
				sprintf(buf, "    Affects : %s By %d\n\r", buf2, obj->affected[i].modifier);
				send_to_char(buf, ch);
			}
		}

		if (GET_LEVEL(ch) < IMO_IMM)
			send_to_char("\r\nThe Auctioneer thanks you for your donation of 5,780 coins.\r\n", ch);

		return;

	}
}


void au1000_do_auction(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	struct obj_data *obj;
	int lv_bid, lv_raise, location, min_lvl, max_lvl;

	obj = auction_data.obj;

	for (; isspace(*arg); arg++);

	if (IS_NPC(ch) && (ch->master)) {
		send_to_char("Charmies can't auction items!\n\r", ch->master);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("You cannot auction while you are jailed!\n\r", ch);
		return;
	}

	if (!*arg) {
		if (obj) {
			send_to_char("Currently being auctioned:\n\r--------------------------\n\r", ch);
			sprintf(buf, "Item          : %s\n\r", GET_OSDESC(obj));
			send_to_char(buf, ch);
			sprintf(buf, "Current bid   : %s\n\r",
				show_with_dots(auction_data.bidder ? auction_data.bidder->bid : auction_data.price));
			send_to_char(buf, ch);
			sprintf(buf, "Minimum raise : %s\n\r", show_with_dots(auction_data.raise));
			send_to_char(buf, ch);
			sprintf(buf, "Seller        : %s\n\r", GET_REAL_NAME(auction_data.auctioner));
			send_to_char(buf, ch);
			if (auction_data.bidder) {
				sprintf(buf, "Bidder        : %s\n\r\n\r\n\r", GET_REAL_NAME(auction_data.bidder->current));
				send_to_char(buf, ch);
			}
			else
				send_to_char("Bidder        : None\n\r\n\r\n\r", ch);

			/* Don't let the auctioner see the item information */
			return;
		}
		else {
			send_to_char("There is nothing being auctioned at the moment.\n\r\n\r", ch);
			send_to_char("Type 'help auction' for more information.\n\r", ch);
			return;
		}

	}			/* end of if (!*arg) */

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (is_abbrev(buf, "identify")) {
		au999_do_auction_identify(ch);
		return;
	}

	if (is_abbrev(buf, "close")) {
		au1900_close_auction(ch);
		return;
	}

	if (obj) {
		send_to_char("There already is an auction going, type 'auction' for more info.\n\r", ch);
		return;
	}

	obj = 0;


	if (!(obj = ha2075_get_obj_list_vis(ch, buf, ch->carrying))) {
		sprintf(buf2, "You don't seem to have %s to put on auction.\n\r", buf);
		send_to_char(buf2, ch);
		return;
	}

	/* If this is a corpse */
	if ((GET_ITEM_TYPE(obj) == ITEM_CONTAINER)) {
		if (obj->obj_flags.value[3]) {
			send_to_char("You can't auction corpses!\r\n", ch);
			return;
		}
	}

	if (IS_PERSONAL(obj)) {
		send_to_char("You can't auction personal items.\n\r", ch);
		return;
	}

	if (check_nodrop(obj)) {
		send_to_char("You can't auction this item.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	remove_dots(buf);

	if (!*buf)
		lv_bid = 100;
	else {
		if (!is_number(buf)) {
			send_to_char("Minimum bid must be a numeric value.\n\r", ch);
			return;
		}
		lv_bid = MAXV(100, atoi(buf));
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	remove_dots(buf);

	if (!*buf)
		lv_raise = MAXV(100, lv_bid / 10);
	else {
		if (!is_number(buf)) {
			send_to_char("Minimum raise must be a numeric value.\n\r", ch);
			return;
		}
		lv_raise = MAXV(100, atoi(buf));
	}

	/* Odin- this code implements a min and max level on bids */

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	remove_dots(buf);

	if (!*buf)
		min_lvl = 1;
	else {
		if (!is_number(buf)) {
			send_to_char("Minimum level must be a numeric value.\n\r", ch);
			return;
		}
		min_lvl = MINV(100, atoi(buf));
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	remove_dots(buf);

	if (!*buf)
		max_lvl = 50;
	else {
		if (!is_number(buf)) {
			send_to_char(" Maximum level must be a numeric value.\r\n", ch);
			return;
		}
		max_lvl = MINV(100, atoi(buf));
	}

	if (lv_raise > lv_bid) {
		send_to_char("Minbid must be higher than min raise.\n\r", ch);
		return;
	}
	sprintf(buf, "&B%s &n- &R%s &n(&W", GET_REAL_NAME(ch), GET_OSDESC(obj));
	sprinttype(GET_ITEM_TYPE(obj), item_types, buf2);
	strcat(buf, buf2);
	strcat(buf, "&n) ");
	sprintf(buf2, " Minbid: &W%s &ncoins,", show_with_dots(lv_bid));
	strcat(buf, buf2);
	sprintf(buf2, " Min. Raise: &W%s &ncoins.", show_with_dots(lv_raise));
	strcat(buf, buf2);
	au1200_show_info(buf);
	sprintf(buf, "You give %s to the auctioneer.\n\r", GET_OSDESC(obj));
	send_to_char(buf, ch);
	/* send_to_char ("Type 'cancel' to withdraw the item.\n\r",ch); */

	au1600_clear_bidders();
	auction_data.auctioner = ch;
	auction_data.price = lv_bid;
	auction_data.raise = lv_raise;
	auction_data.obj = obj;
	auction_data.counter = 0;
	auction_data.min_lvl = min_lvl;
	auction_data.max_lvl = max_lvl;

	for (location = 0; location <= top_of_world; location++) {
		if (world[location].number == 24)
			break;
		if (location == top_of_world) {
			send_to_char("Error! Auctionroom not found!\r\n", ch);
			au1300_reboot_auction();
			return;
		}
	}

	sprintf(buf, "(obj) do_auction %s - %s to Auction room", GET_REAL_NAME(ch), OBJS(obj, ch));
	main_log(buf);
	spec_log(buf, EQUIPMENT_LOG);
	ha1800_obj_from_char(obj);
	ha2100_obj_to_room(obj, location);

}				/* end of au1000_do_auction() */


void au1100_do_bid(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int lv_bid;
	struct char_data *vict = 0;
	struct obj_data *tmp_object;
	//incase we need to drop coins at their feet

		if (IS_NPC(ch) && (ch->master)) {
		send_to_char("Charmies can't bid on items!\n\r", ch->master);
		return;
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("You cannot bid while you are jailed!\n\r", ch);
		return;
	}

	if (!auction_data.obj) {
		send_to_char("There is nothing being auctioned at the moment.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (auction_data.bidder)
		vict = auction_data.bidder->current;
	/*
   if ((GET_LEVEL(ch) == 41) && (GET_LEVEL (auction_data.auctioner) < 41) && IS_PC(auction_data.auctioner)) {
		 send_to_char ("Can't bid on items from mortals.\r\n", ch);
		 return;
   }
	 */
	if (vict == ch) {
		send_to_char("You can't outbid yourself!\r\n", ch);
		return;
	}

	if (auction_data.auctioner == ch) {
		send_to_char("You can't bid on your own items!\r\n", ch);
		return;
	}

	if (!*buf) {
		lv_bid = (auction_data.bidder ? (auction_data.bidder->bid + auction_data.raise) : auction_data.price);
	}
	else {
		remove_dots(buf);
		if (!is_number(buf)) {
			send_to_char("Must supply a numeric value for the amount of coins!\r\n", ch);
			return;
		}
		lv_bid = MAXV(0, atoi(buf));
	}


	if ((GET_LEVEL(ch) > auction_data.max_lvl || GET_LEVEL(ch) < auction_data.min_lvl) &&
	    (GET_LEVEL(ch) < IMO_IMP)) {
		send_to_char("Sorry but your level is not allowed to bid!\r\n", ch);
		return;
	}

	if (lv_bid > GET_GOLD(ch)) {
		sprintf(buf, "You do not have %s coins.\r\n", show_with_dots(lv_bid));
		send_to_char(buf, ch);
		return;
	}

	if (lv_bid < (auction_data.bidder ? auction_data.bidder->bid : auction_data.price)) {
		send_to_char("Your bid must be higher than the current one!\r\n", ch);
		return;
	}

	if (vict) {

		if (lv_bid < ((auction_data.bidder ? auction_data.bidder->bid : auction_data.price) + auction_data.raise)) {
			sprintf(buf, "There is a minimum raise of %s coins!\r\n", show_with_dots(auction_data.raise));
			send_to_char(buf, ch);
			return;
		}
	}

	if (check_lore(ch, auction_data.obj)) {
		send_to_char("You already have one of those.\r\n", ch);
		return;
	}

	sprintf(buf, "New bid: &W%s&n coins by &B%s&n.",
		show_with_dots(lv_bid),
		GET_REAL_NAME(ch));
	au1200_show_info(buf);

	if (vict) {
		sprintf(buf, "Returning %s coins from your bid.\r\n",
			show_with_dots(auction_data.bidder->bid));
		send_to_char(buf, vict);
		if ((MAX_GOLD - GET_GOLD(vict) - auction_data.bidder->bid) < 0) {
			send_to_char("Blerg!  You can't hold all that money, and drop some of it on the ground.\r\n", vict);
			tmp_object = ha3400_create_money(auction_data.bidder->bid);
			ha2100_obj_to_room(tmp_object, vict->in_room);
		}
		else
			GET_GOLD(vict) += auction_data.bidder->bid;
	}

	sprintf(buf, "You give %s coins to the auctioneer.\r\n", show_with_dots(lv_bid));
	send_to_char(buf, ch);
	/* send_to_char ("Type 'cancel' to withdraw your bid\n\r",ch); */

	GET_GOLD(ch) -= lv_bid;
	au1700_new_bidder(ch, lv_bid);
	auction_data.counter = 0;

}				/* end of au1100_do_bid() */



void au1200_show_info(char *msg)
{

	struct descriptor_data *i;
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "AUCTION: %s", msg);
	for (i = descriptor_list; i; i = i->next)
		if (!i->connected &&
		    !IS_SET(GET_ACT3(i->character), PLR3_NOAUCTION)) {
			if (!(IS_SET(world[i->character->in_room].room_flags, RM1_SOUNDPROOF))) {
				ansi_act(buf, 0, i->character, 0, 0, TO_CHAR, CLR_AUCTION, 0);
			}
		}
}				/* end of au1200_show_info() */


void au1300_reboot_auction(void)
{

	au1600_clear_bidders();
	auction_data.auctioner = 0;
	auction_data.price = 0;
	auction_data.raise = 0;
	auction_data.obj = 0;
	auction_data.counter = 0;

}				/* end of au1300_boot_auction() */


void au1400_do_cancel(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	if (!auction_data.auctioner) {
		send_to_char("There is nothing being auctioned at the moment.\r\n", ch);
		return;
	}

	if ((cmd == CMD_CANCEL) && (GET_LEVEL(ch) <= IMO_SPIRIT)) {
		send_to_char("Only immortals can cancel an auction.\r\n", ch);
		return;
	}

	if ((((auction_data.auctioner == ch) || GET_LEVEL(ch) >= IMO_IMM)) && (auction_data.obj)) {

		switch (cmd) {
		case CMD_COUNTER:
			sprintf(buf, "Item &R%s&n has been withdrawn (&WNo bids&n).", GET_OSDESC(auction_data.obj));
			break;

		case CMD_CANCEL:
			sprintf(buf, "Item &R%s&n has been withdrawn (&WCanceled by player&n).", GET_OSDESC(auction_data.obj));
			break;

		case CMD_CANCEL_DEAD:
			sprintf(buf, "Item &R%s&n has been withdrawn (&WPlayer died&n).", GET_OSDESC(auction_data.obj));
			break;
		default:
			sprintf(buf, "Item &R%s&n has been withdrawn (&WPlayer left the game&n).", GET_OSDESC(auction_data.obj));
			break;
		}
		au1200_show_info(buf);
		sprintf(buf, "The auctioneer gives you %s back.\n\r", GET_OSDESC(auction_data.obj));
		send_to_char(buf, auction_data.auctioner);

		if (auction_data.bidder) {
			sprintf(buf, "The auctioneer gives you your %s coins back.\n\r", show_with_dots(auction_data.bidder->bid));
			send_to_char(buf, auction_data.bidder->current);
			GET_GOLD(auction_data.bidder->current) += auction_data.bidder->bid;
		}

		sprintf(buf, "(obj) do_auction %s - %s from Auction room", GET_REAL_NAME(auction_data.auctioner), OBJS(auction_data.obj, auction_data.auctioner));
		main_log(buf);
		spec_log(buf, EQUIPMENT_LOG);

		ha2200_obj_from_room(auction_data.obj);
		ha1700_obj_to_char(auction_data.obj, auction_data.auctioner);
		if (cmd == CMD_COUNTER && IS_NPC(ch) && (auction_data.price != 50000) && (number(0, 9) > 6)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Well, I guess no one wanted this %s, maybe some of my mob friends will be able to make use of it.",
				GET_OSDESC(auction_data.obj));
			do_gossip(ch, buf, CMD_GOSSIP);
			bzero(buf, sizeof(buf));
			sprintf(buf, "MOB DONATE: %s donated %s.", GET_NAME(ch), GET_OSDESC(auction_data.obj));
			spec_log(buf, FUN_LOG);
			ha1800_obj_from_char(auction_data.obj);
			ha2100_obj_to_room(auction_data.obj, gv_mob_donate_room);
		}
		au1300_reboot_auction();
		return;
	}


	if (auction_data.bidder)
		if (auction_data.bidder->current == ch) {
			/* au1800_remove_bidder(ch, 0); */
			send_to_char("Can't withdraw your bid!\r\n", auction_data.bidder->current);
			return;
		}

	if (!auction_data.obj) {
		sprintf(buf, "Item (&Wunknown&n) has been withdrawn due to &Ran internal error&n.");
		au1200_show_info(buf);
		send_to_char("Error with auction obj, Can't return object. Rebooting auction.\n\r", ch);
		au1300_reboot_auction();
		return;
	}

	send_to_char("You can only cancel your own auctions / bids!\n\r", ch);

}				/* end of au1400_do_cancel() */


void au1500_counter(void)
{

	char buf[MAX_STRING_LENGTH];
	struct obj_data *tmp_object;
	int leftover_gold;

	au2000_remind_mob_to_auction_later(2, NULL, NULL, 0);

	if (!auction_data.obj)
		return;
	auction_data.counter++;

	switch (auction_data.counter) {

	case 44:{
			if (!auction_data.bidder)
				au1400_do_cancel(auction_data.auctioner, buf, CMD_COUNTER);
			return;
		}
	case 45:{
			sprintf(buf, "&R%s&n going &Wonce", GET_OSDESC(auction_data.obj));
			au1200_show_info(buf);
			return;
		}
	case 50:{
			sprintf(buf, "&R%s&n going &Wtwice", GET_OSDESC(auction_data.obj));
			au1200_show_info(buf);
			return;
		}
	case 57:{
			sprintf(buf, "&R%s&n sold to &B%s&n for &W%s&n coins",
				GET_OSDESC(auction_data.obj),
				GET_REAL_NAME(auction_data.bidder->current),
				show_with_dots(auction_data.bidder->bid));
			au1200_show_info(buf);
			if ((MAX_GOLD - GET_GOLD(auction_data.auctioner) - auction_data.bidder->bid) < 0) {
				send_to_char("Woof, you can't hold that much gold and drop it all on the ground.\r\n", auction_data.auctioner);
				leftover_gold = auction_data.bidder->bid - (MAX_GOLD - GET_GOLD(auction_data.auctioner));
				GET_GOLD(auction_data.auctioner) = MAX_GOLD;
				tmp_object = ha3400_create_money(leftover_gold);
				ha2100_obj_to_room(tmp_object, auction_data.auctioner->in_room);
			}
			else if (IS_PC(auction_data.auctioner))
				GET_GOLD(auction_data.auctioner) += auction_data.bidder->bid;

			sprintf(buf, "AUCTION: SELL: %s - %s from Auction room", GET_REAL_NAME(auction_data.bidder->current), OBJS(auction_data.obj, auction_data.bidder->current));
			main_log(buf);
			spec_log(buf, EQUIPMENT_LOG);

			ha2200_obj_from_room(auction_data.obj);
			ha1700_obj_to_char(auction_data.obj, auction_data.bidder->current);
			sprintf(buf, "The auctioneer gives you %s coins.\n\r", show_with_dots(auction_data.bidder->bid));
			send_to_char(buf, auction_data.auctioner);
			sprintf(buf, "The auctioneer gives you %s.\n\r", GET_OSDESC(auction_data.obj));
			send_to_char(buf, auction_data.bidder->current);
			sprintf(buf, "AUCTION: WON: The auctioneer gives %s %s. sold for %s coins",
				GET_REAL_NAME(auction_data.bidder->current),
				GET_OSDESC(auction_data.obj),
				show_with_dots(auction_data.bidder->bid));
			spec_log(buf, EQUIPMENT_LOG);
			au1300_reboot_auction();
			return;
		}
	}
}				/* end of au1500_counter() */

void au1600_clear_bidders(void)
{

	struct auction_bidders *tmp_bidder;

	while (auction_data.bidder) {
		tmp_bidder = auction_data.bidder;
		auction_data.bidder = auction_data.bidder->next;
		free(tmp_bidder);
	}
	auction_data.bidder = 0;
}


void au1700_new_bidder(struct char_data * ch, int bid)
{

	struct auction_bidders *tmp_bidder;

	CREATE(tmp_bidder, struct auction_bidders, 1);

	tmp_bidder->current = ch;
	tmp_bidder->bid = bid;
	tmp_bidder->next = auction_data.bidder;
	auction_data.bidder = tmp_bidder;
}

void au1800_remove_bidder(struct char_data * ch, int lv_flag)
{

	struct auction_bidders *tmp_bidder2 = 0, *tmp_bidder = 0, *new_list = 0;
	struct char_data *old_char;
	struct obj_data *tmp_object;
	char buf[MAX_STRING_LENGTH];
	int old_bid;


	/* IF THERE ARE NO BIDDERS RETURN IMMEDIATLY */
	if (!auction_data.bidder)
		return;

	/* SAVE DATA INCASE CURRENT BIDDER IS REMOVED */
	old_char = auction_data.bidder->current;
	old_bid = auction_data.bidder->bid;

	/* REMOVE CH FROM THE BIDDERS LIST (TURNS AROUND LIST NOT VERY NEAT ;) */
	while (auction_data.bidder) {
		if (auction_data.bidder->current == ch) {
			tmp_bidder = auction_data.bidder;
			auction_data.bidder = auction_data.bidder->next;
			free(tmp_bidder);
			tmp_bidder = 0;
		}
		else {
			tmp_bidder = auction_data.bidder;
			auction_data.bidder = auction_data.bidder->next;
		}
		if (tmp_bidder) {
			tmp_bidder2 = tmp_bidder;
			tmp_bidder2->next = new_list;
			new_list = tmp_bidder2;
		}
	}

	/* TURN BACK LIST */
	while (new_list) {
		tmp_bidder2 = new_list->next;
		tmp_bidder = new_list;
		tmp_bidder->next = auction_data.bidder;
		auction_data.bidder = tmp_bidder;
		new_list = tmp_bidder2;
	}

	if (lv_flag)
		return;

	/* IF THERE ARE NO BIDDERS LEFT SET PRICE AT MIN BID */
	if (!auction_data.bidder) {
		sprintf(buf, "&W%s&n canceled bid. &WNo other bidders&n, new minbid &W%s&n coins.",
			GET_REAL_NAME(ch),
			show_with_dots(auction_data.price));
		au1200_show_info(buf);
		sprintf(buf, "The auctioneer gives you your %s coins back.\n\r", show_with_dots(old_bid));
		send_to_char(buf, old_char);
		if ((MAX_GOLD - GET_GOLD(old_char) - old_bid) < 0) {
			send_to_char("Woof, you can't hold that much gold and drop it all on the ground.\r\n", ch);
			tmp_object = ha3400_create_money(old_bid);
			ha2100_obj_to_room(tmp_object, old_char->in_room);
		}
		else
			GET_GOLD(old_char) += old_bid;
		auction_data.counter = 0;
		return;
	}

	/* IF THE REMOVED BIDDER WAS THE CURRENT BIDDER MAKE THE NEXT ON IN THE
	 * LIST CURRENT */
	if (auction_data.bidder->current != old_char) {
		if (GET_GOLD(auction_data.bidder->current) < auction_data.bidder->bid) {
			au1800_remove_bidder(auction_data.bidder->current, 1);
			if (!auction_data.bidder) {
				sprintf(buf, "&B%s&n canceled bid. &Wother bidders out of cash&n, new minbid &W%s&n coins.",
					GET_REAL_NAME(ch),
					show_with_dots(auction_data.price));
				au1200_show_info(buf);
				sprintf(buf, "The auctioneer gives you your %s coins back.\n\r", show_with_dots(old_bid));
				send_to_char(buf, old_char);
				if ((MAX_GOLD - GET_GOLD(old_char) - old_bid) < 0) {
					send_to_char("Woof, you can't hold that much gold and drop some of it on the ground.\r\n", ch);
					tmp_object = ha3400_create_money(old_bid - GET_GOLD(old_char));
					GET_GOLD(old_char) = MAX_GOLD;
					ha2100_obj_to_room(tmp_object, old_char->in_room);
				}
				else
					GET_GOLD(old_char) += old_bid;
				auction_data.counter = 0;
				return;
			}
		}
		auction_data.counter = 0;
		sprintf(buf, "&B%s&n canceled bid, Bid going back to &B%s&n at &W%s&n coins.",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(auction_data.bidder->current),
			show_with_dots(auction_data.bidder->bid));
		au1200_show_info(buf);
		sprintf(buf, "The auctioneer gives you your %s coins back.\n\r", show_with_dots(old_bid));
		send_to_char(buf, old_char);
		if ((MAX_GOLD - GET_GOLD(old_char) - old_bid) < 0) {
			send_to_char("Woof, you can't hold that much gold and drop it all on the ground.\r\n", ch);
			tmp_object = ha3400_create_money(old_bid);
			ha2100_obj_to_room(tmp_object, old_char->in_room);
		}
		else
			GET_GOLD(old_char) += old_bid;
		sprintf(buf, "You give %s coins to the auctioneer.\n\r", show_with_dots(auction_data.bidder->bid));
		send_to_char(buf, auction_data.bidder->current);
		GET_GOLD(auction_data.bidder->current) -= auction_data.bidder->bid;
	}

}				/* end of au1800_remove_bidder(); */


void au1900_close_auction(struct char_data * ch)
{

	if (!auction_data.auctioner) {
		send_to_char("There is nothing being auctioned at the moment.\r\n", ch);
		return;
	}

	if ((auction_data.auctioner != ch) && (GET_LEVEL(ch) < IMO_IMP)) {
		send_to_char("You can only close your own auctions!\r\n", ch);
		return;
	}

	if (!auction_data.bidder) {
		send_to_char("You can't close an auction if there are no bidders, Use cancel instead.\r\n", ch);
		return;
	}

	if (auction_data.counter > 42) {
		send_to_char("Too late to close the auction, Last calls already started.\r\n", ch);
		return;
	}

	auction_data.counter = 43;

}				/* end of au1900_close_auction */

void au1999_mob_auction_gossip(struct char_data * ch, int amount)
{
	char buf[MAX_STRING_LENGTH];
	int do_it = 0;

	if (amount == 1000000000) {
		sprintf(buf, "Ha ha, look what I looted!  Who needs a cross??");
		do_it = 1;
	}
	if (amount == 250000000) {
		sprintf(buf, "Hey!  If you don't bid, I know some mobs that could be draining your ass!");
		do_it = 1;
	}
	if (amount == 300000000) {
		sprintf(buf, "Damn!  These things hurt.  Anyone else wanna try it out?");
		do_it = 1;
	}
	if (amount == 375000000) {
		sprintf(buf, "Of all the flaming balls of gas I've encountered, this is my favorite.");
		do_it = 1;
	}
	if (amount == 275000000) {
		sprintf(buf, "Does Vorn know you had this?  Maybe I should give it back.");
		do_it = 1;
	}
	if (amount == 150000000) {
		sprintf(buf, "Man, this thing is dirty, but maybe someone still wants it.");
		do_it = 1;
	}
	if (amount == 325000000) {
		sprintf(buf, "We have to be insane to auction at these rock bottom prices!  Why, selling such an item is madness!  We're craaaaaaaazy.");
		do_it = 1;
	}
	if (amount == 350000000) {
		sprintf(buf, "I dominated you so bad, I made you look like War.");
		do_it = 1;
	}
	if (amount == 267000000) {
		sprintf(buf, "Wouldn't it be great if you could buy singing pentagrams?  Imagine sending the devil door to door!");
		do_it = 1;
	}
	if (amount == 200000000) {
		sprintf(buf, "How the hell can you make a robe out of souls... and what's the difference between a dark soul and a light soul?  Start sharing the crack, boys!");
		do_it = 1;
	}
	if (amount == 225000000) {
		sprintf(buf, "Look at my pretty boots!  Aren't my boots pretty?  Pretty boots!  Pretty pretty boots!");
		do_it = 1;
	}
	if (amount == 260000000) {
		sprintf(buf, "Boots of War are really neat, they help me cover up my feet!");
		do_it = 1;
	}
	if (amount < 251000000 && amount > 50000 && (number(0, 9) > 8)) {
		sprintf(buf, "Man, it looks like you need, this, it'd be a shame if I junked it....");
		do_it = 1;
	}

	if (do_it)
		do_gossip(ch, buf, CMD_GOSSIP);
}

#define MAX_AUCTION_REMINDS 50
#define MOB_AUCTION_DELAY   10
/* The following routine is controlled by iFlag.
   0:== Initializes memory
   1:== stores name and object to auction and sets timer
   2:== checks to see if its time to remind a mob to auction */
void au2000_remind_mob_to_auction_later(int iFlag, struct char_data * ch, struct obj_data * obj, int amount)
{

	static struct char_data *mob_ch[MAX_AUCTION_REMINDS];
	static char *item_name[MAX_AUCTION_REMINDS];
	static int min_bid_amount[MAX_AUCTION_REMINDS];
	static int iDelayCount[MAX_AUCTION_REMINDS];
	static int iCurrentCount;
	struct char_data *tmp_ch;
	char buf1[MAX_STRING_LENGTH];
	int idx;

	if (iFlag == 0) {
		for (idx = 0; idx < MAX_AUCTION_REMINDS; idx++) {
			mob_ch[iCurrentCount] = NULL;
			item_name[iCurrentCount] = NULL;
			min_bid_amount[iCurrentCount] = 0;
			iDelayCount[iCurrentCount] = 0;
		} //for ()
			iCurrentCount = 0;
		return;
	}

	if (iFlag == 1) {
		//Is there room to add this one ?
			if (iCurrentCount + 2 > MAX_AUCTION_REMINDS) {
			return;
		}

		mob_ch[iCurrentCount] = ch;
		item_name[iCurrentCount] = str_alloc(GET_OBJ_NAME(obj));
		min_bid_amount[iCurrentCount] = amount;
		iDelayCount[iCurrentCount] = MOB_AUCTION_DELAY;
		iCurrentCount++;

	}
	if (iFlag == 2) {
		for (idx = 0; idx < iCurrentCount; idx++) {
			if (iDelayCount[idx] > 0) {
				iDelayCount[idx]--;
			}
		} //for ()
			//If there is no auction and first mob is ready to auction...
				if (!auction_data.obj &&
				    iCurrentCount > 0) {
				if (iDelayCount[0] < 1) {
					//Does the mob still exist ?
						for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
						if (tmp_ch == mob_ch[0] &&
						    IS_NPC(tmp_ch)) {
							if (min_bid_amount[0] == 50000)
								sprintf(buf1, "%s %d 5000 1 40\r\n", item_name[0], min_bid_amount[0]);
							else {
								au1999_mob_auction_gossip(tmp_ch, min_bid_amount[0]);
								sprintf(buf1, "%s %d %d 41 41\r\n", item_name[0], min_bid_amount[0],
									min_bid_amount[0] / 10);
							}
							au1000_do_auction(tmp_ch, buf1, CMD_AUCTION);
							//Free string used to store object name
								free(item_name[0]);
							min_bid_amount[0] = 0;

						}
					}
					//Shift array up
						for (idx = 0; idx < iCurrentCount; idx++) {
						mob_ch[idx] = mob_ch[idx + 1];
						item_name[idx] = item_name[idx + 1];
						min_bid_amount[idx] = min_bid_amount[idx + 1];
						iDelayCount[idx] = iDelayCount[idx + 1];
					} //for ()
						iCurrentCount--;
				}
			}
	}

	return;
} //END OF au2000_remind_mob_to_auction_later()
