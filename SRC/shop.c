/* sh */
/* gv_location: 14501-15000 */
/* ********************************************************************
*  file: shop.c , Shop module.                       Part of DIKUMUD *
*  Usage: Procedures handling shops and shopkeepers.                 *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "parser.h"
#include "utility.h"
#include "constants.h"
#include "func.h"
#include "globals.h"

#define MAX_TRADE 5
#define MAX_PROD 10

extern char *db7000_fread_string(FILE * fl);

/* local vars to this module */
struct shop_data *shop_index;
int number_of_shops = 0;

struct shop_data {
	int producing[MAX_PROD];/* Which item to produce (virtual)      */
	float profit_buy;	/* Factor to multiply cost with.        */
	float profit_sell;	/* Factor to multiply cost with.        */
	byte type[MAX_TRADE];	/* Which item to trade.                 */
	char *no_such_item1;	/* Message if keeper hasn't got an item */
	char *no_such_item2;	/* Message if player hasn't got an item */
	char *missing_casha1;	/* Message if keeper hasn't got cash    */
	char *missing_casha2;	/* Message if player hasn't got cash    */
	char *do_not_buy;	/* If keeper dosn't buy such things. 	 */
	char *message_buy;	/* Message when player buys item        */
	char *message_sell;	/* Message when player sells item       */
	int temper1;		/* How does keeper react if no money    */
	int temper2;		/* How does keeper react when attacked  */
	int keeper;		/* The mobil who owns the shop (virtual) */
	int with_who;		/* Who does the shop trade with?	 */
	int in_room;		/* Where is the shop?			 */
	int open1, open2;	/* When does the shop open?		 */
	int close1, close2;	/* When does the shop close?		 */

	char stocked;		/* initially FALSE - dont want to repetitively
				 * stock a shopkeeper */
};



int is_ok(struct char_data * keeper, struct char_data * ch, int shop_nr)
{



	if (shop_index[shop_nr].open1 > time_info.hours) {
		do_say(keeper,
		       "Come back later!", 17);
		return (FALSE);
	}
	else if (shop_index[shop_nr].close1 < time_info.hours) {
		if (shop_index[shop_nr].open2 > time_info.hours) {
			do_say(keeper,
			   "Sorry, we have closed, but come back later.", 17);
			return (FALSE);
		}
		else if (shop_index[shop_nr].close2 < time_info.hours) {
			do_say(keeper,
			       "Sorry, come back tomorrow.", 17);
			return (FALSE);
		}
	};

	if (!(CAN_SEE(keeper, ch)) && GET_LEVEL(ch) <= PK_LEV) {
		do_say(keeper,
		       "I don't trade with someone I can't see!", 17);
		return (FALSE);
	};

	switch (shop_index[shop_nr].with_who) {
	case 0:
		return (TRUE);
	case 1:
		return (TRUE);
	default:
		return (TRUE);
	};
}

int trade_with(struct obj_data * item, int shop_nr)
{
	int counter;



	if (item->obj_flags.cost < 1)
		return (FALSE);

	for (counter = 0; counter < MAX_TRADE; counter++)
		if (shop_index[shop_nr].type[counter] == item->obj_flags.type_flag)
			return (TRUE);
	return (FALSE);
}

int shop_producing(struct obj_data * item, int shop_nr)
{
	int counter;



	if (item->item_number < 0)
		return (FALSE);
	/* irrelevant with new % repop  if (obj_index[item->item_number].number
	 * >= obj_index[item->item_number].maximum) { return(FALSE); } */
	for (counter = 0; counter < MAX_PROD; counter++)
		if (shop_index[shop_nr].producing[counter] == item->item_number)
			return (TRUE);
	return (FALSE);
}
/* Crimson addition: if the shopkeeper doesn't have an infinite */
/* item, give it to 'em */
void shop_stock(struct char_data * keeper, int shop_nr)
{
	int counter, lv_obj_num;
	struct obj_data *tmp_obj;



	if (shop_index[shop_nr].stocked)
		return;

	shop_index[shop_nr].stocked = TRUE;	/* will soon be stocked */

	for (counter = 0; counter < MAX_PROD; counter++)
		lv_obj_num = shop_index[shop_nr].producing[counter];
	if (lv_obj_num > -1) {
		tmp_obj = ha2025_get_obj_list_num(lv_obj_num, keeper->carrying);
		if (!tmp_obj) {
			if (shop_index[shop_nr].producing[counter] >= 0)
				/* DON'T GO OVER MAX */
				if (obj_index[lv_obj_num].number < obj_index[lv_obj_num].maximum) {
					tmp_obj = db5100_read_object(lv_obj_num, REAL);
					if (tmp_obj) {
						ha1700_obj_to_char(tmp_obj, keeper);
					}
				}
		}
	}
}				/* END OF shop_stock */

void shopping_buy(char *arg, struct char_data * ch,
		    struct char_data * keeper, int shop_nr)
{

	int lv_amount, lv_number_bought, lv_cost_of_item;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	struct obj_data *temp1;



	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	if (!(*buf)) {
		sprintf(buf, "%s What do you want to buy?", GET_REAL_NAME(ch));
		do_ask(keeper, buf, CMD_SAY);
		return;
	}

	/* DOES PLAYER WANT TO BUY MORE THAN ONE? */
	lv_amount = 0;
	if (is_number(buf)) {
		lv_amount = atoi(buf);
		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
	}

	lv_number_bought = 0;
	while (1) {
		/* MAKE SURE SHOPKEEPER HAS PLENTY OF ITEMS */
		shop_stock(keeper, shop_nr);

		/* DOES SHOPKEEPER HAVE THE REQUESTED ITEM? */
		temp1 = ha2075_get_obj_list_vis(ch, buf, keeper->carrying);
		if (!(temp1)) {
			sprintf(buf, "%s We don't have that item in stock.",
				GET_REAL_NAME(ch));
			if (GET_VISIBLE(ch) >= IMO_LEV)
				do_whisper(keeper, buf, CMD_TELL);
			else
				do_tell(keeper, buf, CMD_SAY);
			break;
		}

		/* SAVE ITEM NAME */
		if (lv_number_bought == 0) {
			bzero(buf3, sizeof(buf3));
			strcpy(buf3, temp1->short_description);
		}

		/* IF THIS IS A LESS THAN ZERO COST, SAY WE DON'T HAVE IT */
		if (temp1->obj_flags.cost <= 0) {
			sprintf(buf, shop_index[shop_nr].no_such_item1,
				GET_REAL_NAME(ch));
			do_whisper(keeper, buf, 19);
			ha2700_extract_obj(temp1);
			break;
		}

		if (check_lore(ch, temp1)) {
			send_to_char("You can't buy anymore of these.\r\n", ch);
			break;
		};

		/* CALCULATE COST */
		lv_cost_of_item =
			(int) (temp1->obj_flags.cost * shop_index[shop_nr].profit_buy);

		/* CAN PLAYER AFFORD IT? */
		if (GET_GOLD(ch) < lv_cost_of_item &&
		    GET_LEVEL(ch) < IMO_SPIRIT) {
			sprintf(buf,
				shop_index[shop_nr].missing_casha2,
				GET_REAL_NAME(ch));
			do_say(keeper, buf, CMD_SAY);
			if (lv_number_bought > 0) {
				sprintf(buf, "You only bought %d of them.\r\n",
					lv_number_bought);
				send_to_char(buf, ch);
			}
			switch (shop_index[shop_nr].temper1) {
			case 0:
				do_action(keeper, GET_REAL_NAME(ch), 30);
				break;
			case 1:
				wi1000_do_emote(keeper, "smokes on his joint", 36);
				break;
			default:
				break;
			}	/* END OF switch */
			break;
		}		/* END OF player can't afford */


		/* CAN WE CARRY IT? */
		if ((IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch))) {
			sprintf(buf, "%s : You can't carry that many items.\n\r",
				ha1100_fname(temp1->name));
			send_to_char(buf, ch);
			if (lv_number_bought > 0) {
				sprintf(buf, "You only bought %d of them.\r\n",
					lv_number_bought);
				send_to_char(buf, ch);
			}
			break;
		}

		/* CAN WE CARRY THE WEIGHT? */
		if ((IS_CARRYING_W(ch) + temp1->obj_flags.weight) >
		    CAN_CARRY_W(ch)) {
			sprintf(buf, "%s : You can't carry that much weight.\n\r",
				ha1100_fname(temp1->name));
			send_to_char(buf, ch);
			if (lv_number_bought > 0) {
				sprintf(buf, "You only bought %d of them.\r\n",
					lv_number_bought);
				send_to_char(buf, ch);
			}
			break;
		}

		/* TRANSFER MONEY */
		if (GET_LEVEL(ch) < IMO_SPIRIT)
			GET_GOLD(ch) -= lv_cost_of_item;
		GET_GOLD(keeper) += lv_cost_of_item;

		/* Test if producing shop ! */
		if (shop_producing(temp1, shop_nr))
			temp1 = db5100_read_object(temp1->item_number, REAL);
		else
			ha1800_obj_from_char(temp1);

		/* GIVE ITEM TO CHAR */
		ha1700_obj_to_char(temp1, ch);

		/* LEAVE THE LOOP IF WE PURCHASED ENOUGH ITEMS */
		lv_number_bought++;
		if (lv_number_bought >= lv_amount)
			break;

	}			/* END OF while */

	/* IF WE DIDN'T BUY ANYTHING, PLAYER GOT MESSAGE ABOVE */
	if (lv_number_bought < 1)
		return;

	/* SALE MESSAGES */
	if (lv_number_bought == 1) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "$n buys %s.", buf3);
	}
	else {
		sprintf(buf2, "$n buys several %s.", buf3);
	}
	act(buf2, FALSE, ch, temp1, 0, TO_ROOM);

	/* print message about cost of item */
	sprintf(buf,
		shop_index[shop_nr].message_buy,
		GET_REAL_NAME(ch),
		(lv_cost_of_item * lv_number_bought));

	/* REMOVE PERIOD SO USER DOESN'T SEE EXCLAIM FROM SAY */
	bzero(buf2, sizeof(buf2));
	sprintf(buf2, "%s ", buf);
	do_say(keeper, buf2, CMD_SAY);

	if (lv_number_bought == 1) {
		sprintf(buf, "You now have %s.\n\r", buf3);
	}
	else {
		sprintf(buf, "You now have %d %s.\n\r", lv_number_bought, buf3);
	}
	send_to_char(buf, ch);

	return;
}

void shopping_sell(char *arg, struct char_data * ch, struct char_data * keeper, int shop_nr)
{
	char argm[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct obj_data *temp1;



	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	one_argument(arg, argm);

	if (!(*argm)) {
		sprintf(buf,
			"%s What do you want to sell??"
			,GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	if (!(temp1 = ha2075_get_obj_list_vis(ch, argm, ch->carrying))) {
		sprintf(buf,
			shop_index[shop_nr].no_such_item2,
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	/* IS THE ITEM OUT OF CHARGES? */
	if (temp1->obj_flags.type_flag == ITEM_STAFF ||
	    temp1->obj_flags.type_flag == ITEM_WAND) {
		if (temp1->obj_flags.value[2] == 0) {
			sprintf(buf, "sell uncharged magic items elsewhere.");
			do_whisper(keeper, buf, 19);
			return;
		}
	}

	if (!(trade_with(temp1, shop_nr)) || (temp1->obj_flags.cost < 1)) {
		sprintf(buf,
			shop_index[shop_nr].do_not_buy,
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	if (GET_GOLD(keeper) < (int) (temp1->obj_flags.cost *
				      shop_index[shop_nr].profit_sell)) {
		sprintf(buf, shop_index[shop_nr].missing_casha1,
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	if (check_nodrop(temp1)) {
		send_to_char("You can't sell this item.\r\n", ch);
		return;
	}

	if ((MAX_GOLD - GET_GOLD(ch) - (int) (temp1->obj_flags.cost * shop_index[shop_nr].profit_sell)) < 0) {
		sprintf(buf, "Sorry, you already seem to be rich enough, %s.", GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	act("$n sells $p.", FALSE, ch, temp1, 0, TO_ROOM);

	sprintf(buf, shop_index[shop_nr].message_sell,
		GET_REAL_NAME(ch), (int) (temp1->obj_flags.cost *
					  shop_index[shop_nr].profit_sell));
	do_whisper(keeper, buf, 19);
	sprintf(buf, "The shopkeeper now has %s.\n\r",
		temp1->short_description);
	send_to_char(buf, ch);
	GET_GOLD(ch) += (int) (temp1->obj_flags.cost *
			       shop_index[shop_nr].profit_sell);
	GET_GOLD(keeper) -= (int) (temp1->obj_flags.cost *
				   shop_index[shop_nr].profit_sell);

	if ((ha2000_get_obj_list(argm, keeper->carrying)) ||
	    (GET_ITEM_TYPE(temp1) == ITEM_TRASH))
		ha2700_extract_obj(temp1);
	else {
		ha1800_obj_from_char(temp1);
		ha1700_obj_to_char(temp1, keeper);
	}

	return;
}

void shopping_value(char *arg, struct char_data * ch,
		      struct char_data * keeper, int shop_nr)
{
	char argm[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct obj_data *temp1;



	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	one_argument(arg, argm);

	if (!(*argm)) {
		sprintf(buf, "%s What do you want me to valuate??",
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	if (!(temp1 = ha2075_get_obj_list_vis(ch, argm, ch->carrying))) {
		sprintf(buf, shop_index[shop_nr].no_such_item2,
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	if (!(trade_with(temp1, shop_nr))) {
		sprintf(buf,
			shop_index[shop_nr].do_not_buy,
			GET_REAL_NAME(ch));
		do_whisper(keeper, buf, 19);
		return;
	}

	sprintf(buf, "%s I'll give you %d gold coins for that!",
		GET_REAL_NAME(ch), (int) (temp1->obj_flags.cost *
					  shop_index[shop_nr].profit_sell));
	do_whisper(keeper, buf, 19);

	return;
}

void shopping_list(char *arg, struct char_data * ch,
		     struct char_data * keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	struct obj_data *temp1;
	int found_obj;



	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	shop_stock(keeper, shop_nr);
	strcpy(buf, "You can buy:\n\r");
	found_obj = FALSE;
	if (keeper->carrying)
		for (temp1 = keeper->carrying;
		     temp1;
		     temp1 = temp1->next_content)
			if ((CAN_SEE_OBJ(ch, temp1)) && (temp1->obj_flags.cost > 0)) {
				found_obj = TRUE;
				if (temp1->obj_flags.type_flag != ITEM_DRINKCON) {
					bzero(buf3, sizeof(buf3));
					if (IS_OBJ_STAT(temp1, OBJ1_MINLVL10))
						strcpy(buf3, " &W(&rlevel 10&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ2_MINLVL15))
						strcpy(buf3, " &W(&rlevel 15&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ1_MINLVL20))
						strcpy(buf3, " &W(&rlevel 20&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ2_MINLVL25))
						strcpy(buf3, " &W(&rlevel 25&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ1_MINLVL30))
						strcpy(buf3, " &W(&rlevel 30&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ2_MINLVL35))
						strcpy(buf3, " &W(&rlevel 35&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ1_MINLVL41))
						strcpy(buf3, " &W(&rlevel 41&W)&n");
					if (IS_OBJ_STAT(temp1, OBJ1_MINLVL41))
						strcpy(buf3, " &W(&rlevel 42&W)&n");
					sprintf(buf2, "&c%s&n for &G%d&n gold coins. %s\r\n",
						(temp1->short_description),
						(int) (temp1->obj_flags.cost *
					      shop_index[shop_nr].profit_buy),
						buf3);
				}
				else {
					if (temp1->obj_flags.value[1])
						sprintf(buf3, "%s of %s", (temp1->short_description)
							,drinks[temp1->obj_flags.value[2]]);
					else
						sprintf(buf3, "%s", (temp1->short_description));
					sprintf(buf2, "&c%s&n for &G%d&n gold coins.\n\r", buf3,
						(int) (temp1->obj_flags.cost * shop_index[shop_nr].profit_buy));
				}
				CAP(buf2);
				strcat(buf, buf2);
			};

	if (!found_obj)
		strcat(buf, "Nothing!\n\r");

	send_to_char(buf, ch);
	return;
}

void shopping_kill(char *arg, struct char_data * ch, struct char_data * keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH];



	switch (shop_index[shop_nr].temper2) {
	case 0:
		sprintf(buf, "%s Don't ever try that again!", GET_REAL_NAME(ch));
		if (CAN_SEE(keeper, ch)) {
			do_whisper(keeper, buf, 19);
		}
		else {
			send_to_char("It doesn't look like he even saw you.\r\n", ch);
		}
		return;

	case 1:
		sprintf(buf, "%s Scram - midget!", GET_REAL_NAME(ch));
		if (CAN_SEE(keeper, ch)) {
			do_whisper(keeper, buf, 19);
		}
		else {
			send_to_char("It doesn't look like he even saw you.\r\n", ch);
		}
		return;

	default:
		return;
	}
}


int shop_keeper(struct char_data * ch, int cmd, char *arg)
{
	char buf[MAX_STRING_LENGTH];
	struct char_data *temp_char;
	struct char_data *keeper;
	int shop_nr;



	keeper = 0;

	for (temp_char = world[ch->in_room].people; (!keeper) && (temp_char);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char) && mob_index[temp_char->nr].func == shop_keeper)
			keeper = temp_char;

	if (cmd == 0)
		return (FALSE);

	for (shop_nr = 0; shop_nr <= number_of_shops; shop_nr++)
		if (shop_index[shop_nr].keeper == keeper->nr)
			break;

	if (shop_nr > number_of_shops)
		return (FALSE);

	if ((cmd == CMD_BUY) && (ch->in_room == (shop_index[shop_nr].in_room))) {
		shopping_buy(arg, ch, keeper, shop_nr);
		return (TRUE);
	}

	if ((cmd == CMD_SELL) && (ch->in_room == (shop_index[shop_nr].in_room)))
		/* Sell */
	{
		shopping_sell(arg, ch, keeper, shop_nr);
		return (TRUE);
	}

	if ((cmd == CMD_VALUE) && (ch->in_room == (shop_index[shop_nr].in_room)))
		/* value */
	{
		shopping_value(arg, ch, keeper, shop_nr);
		return (TRUE);
	}

	if ((cmd == CMD_LIST) && (ch->in_room == (shop_index[shop_nr].in_room)))
		/* List */
	{
		shopping_list(arg, ch, keeper, shop_nr);
		return (TRUE);
	}

	if ((GET_LEVEL(ch) < IMO_IMM) &&
	    ((cmd == CMD_KILL) ||
	     (cmd == CMD_HIT) ||
	     (cmd == CMD_BACKSTAB) ||
	     (cmd == CMD_BASH) ||
	     (cmd == CMD_KICK) ||
		 /*(cmd == CMD_FLURRY) ||*/
	     (cmd == CMD_BREATH))) {
		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
		if (cmd == CMD_BREATH &&
		    is_abbrev(buf, "gas")) {
			shopping_kill(arg, ch, keeper, shop_nr);
			return (TRUE);
		}
		if (cmd == CMD_BREATH) {	/* STRIP OUT TYPE */
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
		}
		if (!(*buf)) {
			return (FALSE);
		}
		if (keeper == ha2100_get_char_in_room(buf, ch->in_room)) {
			shopping_kill(arg, ch, keeper, shop_nr);
			return (TRUE);
		}
		else {
			/* LET THE FIGHT CODE HANDLE THIS */
			return (FALSE);
		}
	}
	if ((GET_LEVEL(ch) < IMO_IMM) &&
	    ((cmd == CMD_CAST) ||
	     (cmd == CMD_USE) ||
	     (cmd == CMD_RECITE))) {
		act("$N tells you 'No magic here - kid!'.", FALSE, ch, 0, keeper, TO_CHAR);
		return TRUE;
	}

	return (FALSE);
}


void load_a_shopfile(char *filename)
{
	char *buf;
	int temp;
	int count;
	FILE *shop_f;



	if (!(shop_f = fopen(filename, "r"))) {
		return;		/* no shops, no shops */
	}


	for (;;) {
		buf = db7000_fread_string(shop_f);
		if (*buf == '#') {	/* a new shop */
			if (!number_of_shops)	/* first shop */
				CREATE(shop_index, struct shop_data, 1);
			else if (!(shop_index = (struct shop_data *) realloc(shop_index, (number_of_shops + 1) * sizeof(struct shop_data)))) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: in boot shop\r\n");
				perror(buf);
				ABORT_PROGRAM();
			}

			/* 2 to 11 */
			for (count = 0; count < MAX_PROD; count++) {
				fscanf(shop_f, "%d \n", &temp);
				if (temp >= 0)
					shop_index[number_of_shops].producing[count] = db8200_real_object(temp);
				else
					shop_index[number_of_shops].producing[count] = temp;
			}

			/* 12 */
			fscanf(shop_f, "%f \n", &shop_index[number_of_shops].profit_buy);

			/* 13 */
			fscanf(shop_f, "%f \n", &shop_index[number_of_shops].profit_sell);

			/* 14 to 18 */
			for (count = 0; count < MAX_TRADE; count++) {
				fscanf(shop_f, "%d \n", &temp);
				shop_index[number_of_shops].type[count] = (byte) temp;
			}

			/* 19 */
			shop_index[number_of_shops].no_such_item1 = db7000_fread_string(shop_f);

			/* 20 */
			shop_index[number_of_shops].no_such_item2 = db7000_fread_string(shop_f);

			/* 21 */
			shop_index[number_of_shops].do_not_buy = db7000_fread_string(shop_f);

			/* 22 */
			shop_index[number_of_shops].missing_casha1 = db7000_fread_string(shop_f);

			/* 23 */
			shop_index[number_of_shops].missing_casha2 = db7000_fread_string(shop_f);

			/* 24 */
			shop_index[number_of_shops].message_buy = db7000_fread_string(shop_f);

			/* 25 */
			shop_index[number_of_shops].message_sell = db7000_fread_string(shop_f);

			/* 26 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].temper1);

			/* 27 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].temper2);

			/* 28 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].keeper);
			shop_index[number_of_shops].keeper = db8100_real_mobile(shop_index[number_of_shops].keeper);

			/* 29 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].with_who);

			/* 30 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].in_room);
			shop_index[number_of_shops].in_room = db8000_real_room(shop_index[number_of_shops].in_room);

			/* 31 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].open1);

			/* 32 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].close1);

			/* 33 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].open2);

			/* 34 */
			fscanf(shop_f, "%d \n", &shop_index[number_of_shops].close2);
			shop_index[number_of_shops].stocked = FALSE;	/* not stocked initially */

			number_of_shops++;
		}
		else if (*buf == '$')	/* EOF */
			break;
	}

	fclose(shop_f);
}

void assign_the_shopkeepers()
{
	int temp1;



	for (temp1 = 0; temp1 < number_of_shops; temp1++)
		mob_index[shop_index[temp1].keeper].func = shop_keeper;

}

void boot_the_shops(void)
{
	int zone;
	char buf[128];



	/* now load a shopfile for every zone */
	for (zone = 0; zone <= top_of_zone_table; zone++) {
		sprintf(buf, "areas/%s.shp", zone_table[zone].filename);
		load_a_shopfile(buf);
	}
}
