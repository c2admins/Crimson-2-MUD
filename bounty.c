/* bt####
***************************************************************************
File: bounty.c

Part of Crimson MUD 2

Commands/functions pertaining to the bounty system.

Written by Relic
***************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "bounty.h"
#include "db.h"
#include "parser.h"
#include "globals.h"
#include "func.h"

struct bounty_info *bounty_list = NULL;

/* Options: */
#define MIN_BOUNTY			100000000	/* 0 to disable */
#define MAX_BOUNTY			1000000000	/* must be less than 2
							 * billion */
#define BOUNTY_MARKUP		.25	/* Percentage (25 means the total price
					 * will be 125%) */
#define PAYOFF_MARKUP   .25	/* Percentage amount for paying off your own
				 * bounty */

/* Prototyped functions */
struct char_data *get_char(char *name);

/* function implementations */

/*
 *********************************************
 * Functions to save the bounty list to disk *
 *********************************************
 */
void r_write_bounty_to_disk(FILE * fp, struct bounty_info * bt)
{
	if (bt) {
		r_write_bounty_to_disk(fp, bt->next);
		fprintf(fp, "%s %s %d %d %d\r\n", bt->name, bt->placed_by, bt->amount, bt->killed, bt->killable);
	}
}

void bt1000_write_bounty_list(void)
{
	FILE *fl;

	if (!(fl = fopen(BOUNTY_FILE, "w"))) {
		perror("Unable to open bounty file!");
		return;
	}

	r_write_bounty_to_disk(fl, bounty_list);
	fclose(fl);
	return;
}
/* Functions to read the bounty list in from the disk */
void bt1100_load_bounty(void)
{
	FILE *fl;
	char name[MAX_STRING_LENGTH], placed_by[MAX_STRING_LENGTH];
	int amount, killed, killable;
	struct bounty_info *bt;

	while (bounty_list) {
		/* Get rid of the current list, if there is one. */
		bt = bounty_list;
		bounty_list = bounty_list->next;
		free(bt->name);
		free(bt->placed_by);
		free(bt);
	}
	bounty_list = 0;

	if (!(fl = fopen(BOUNTY_FILE, "r"))) {
		perror("Unable to open bounty file!");
		return;
	}

	while (fscanf(fl, "%s %s %d %d %d\r\n", name, placed_by, &amount, &killed, &killable) == 5) {
		CREATE(bt, struct bounty_info, 1);
		bt->name = strdup(name);
		bt->placed_by = strdup(placed_by);
		bt->amount = amount;
		bt->next = bounty_list;
		bounty_list = bt;
	}
	fclose(fl);
}
/*
 ******************************
 * Finding/deleting functions *
 ******************************
 */
struct bounty_info *find_bounty_for_char(struct char_data * ch)
{
	struct bounty_info *bt;

	for (bt = bounty_list; bt; bt = bt->next) {
		if (!strcmp(bt->name, GET_NAME(ch))) {
			return bt;
		}
	}
	return (NULL);
}

void remove_bounty_listing(struct bounty_info * bt)
{
	struct bounty_info *pointer, *holder;

	if (bounty_list == bt) {
		bounty_list = bounty_list->next;
		free(bt->name);
		free(bt->placed_by);
		free(bt);
		bt1000_write_bounty_list();
		return;
		/* Don't wanna try to access bt anymore. */
	}

	holder = bounty_list;
	for (pointer = bounty_list->next; pointer; pointer = pointer->next) {
		if (pointer == bt) {
			holder->next = pointer->next;
			free(bt->name);
			free(bt);
			bt1000_write_bounty_list();
			return;
		}
		holder = holder->next;
	}
}

int bt1200_bounty_check(struct char_data * ch)
{
	struct bounty_info *bt;

	for (bt = bounty_list; bt; bt = bt->next) {
		if (!strcmp(GET_NAME(ch), bt->name))
			return 1;
	}

	return 0;
}

int bt1250_can_attack(struct char_data * killer, struct char_data * vict)
{
	struct bounty_info *bt;

	if (bt1200_bounty_check(vict)) {
		bt = find_bounty_for_char(vict);
		if (bt->killed == 0) {
			if ((IS_SET(GET_ACT2(killer), PLR2_PKILLABLE) && IS_SET(GET_ACT2(vict), PLR2_PKILLABLE)) ||
			    (bt->killable == 1) || (killer == bounty_hunter))
				return 1;
		}
	}
	return 0;
}
/* Removes the bounty_info entry for ch */
void bt1300_remove_bounty(struct char_data * ch)
{

	if (bt1200_bounty_check(ch)) {
		//If there 's no bounty, we don' t need to try to remove it.
		remove_bounty_listing(find_bounty_for_char(ch));
	}
}
/* Make a head for ch, very much like making a corpse. */
struct obj_data *bt1400_make_head(struct char_data * ch, int loot)
{
	struct obj_data *head;
	struct bounty_info *bt;
	char buf[MAX_STRING_LENGTH];

	/* If he's been killed once, set the killed flag on his bounty */
	bt = find_bounty_for_char(ch);
	bt->killed = 1;
	bt1000_write_bounty_list();
	/* Create and initialize the head. */
	CREATE(head, struct obj_data, 1);
	db7600_clear_object(head);
	head->item_number = NOWHERE;
	head->in_room = NOWHERE;
	/* String the names and such. */
	sprintf(buf, "obj head %s", GET_REAL_NAME(ch));
	head->name = strdup(buf);
	sprintf(buf, "Head of %s is lying here.", GET_REAL_NAME(ch));
	head->description = strdup(buf);
	sprintf(buf, "Head of %s", GET_REAL_NAME(ch));
	head->short_description = strdup(buf);

	/* Set a few flags */
	head->obj_flags.type_flag = ITEM_CONTAINER;
	head->obj_flags.wear_flags = ITEM_TAKE;
	head->obj_flags.value[0] = 0;

	head->action_description = strdup(GET_REAL_NAME(ch));

	/* Perhaps put the head_eq in there? */
	if (loot && (number(0, 9) < 5)) {
		if (ch->equipment[WEAR_HEAD] &&
		    (ch->equipment[WEAR_HEAD])->obj_flags.type_flag != ITEM_CONTAINER)
			ha2300_obj_to_obj(ha1930_unequip_char(ch, WEAR_HEAD), head);
	}

	/* Put it in the world */
	head->next = object_list;
	object_list = head;

	return head;
}

int bt2000_bounty_master(struct char_data * ch, int cmd, char *arg)
{
	char arg1[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	struct obj_data *obj;
	struct bounty_info *bt;

	if (cmd != CMD_BOUNTY && cmd != CMD_DROP)
		return (FALSE);
	if (cmd == CMD_BOUNTY) {
		bt2100_do_bounty(ch, arg, 0);
		return (TRUE);
	}
	else {
		for (; isspace(*arg); arg++);
		arg = one_argument(arg, arg1);
		if (is_number(arg1)) {
			do_say(bounty_master, "I don't need your damn charity!", CMD_SAY);
			return (TRUE);
		}
		if (!(*arg1)) {
			send_to_char("Drop what?\n\r", ch);
			return (TRUE);
		}
		obj = ha2075_get_obj_list_vis(ch, arg1, ch->carrying);
		if (!obj) {
			send_to_char("You do not have that item.\n\r", ch);
			return (TRUE);
		}
		if (!obj->action_description) {
			do_say(bounty_master, "I fuckin' hate litterers!!!", CMD_SAY);
			at2200_do_rage(bounty_master, "", CMD_RAGE);
			return (TRUE);
		}
		for (bt = bounty_list; bt; bt = bt->next) {
			if (!(strcmp(obj->action_description, bt->name)))
				break;
		}
		if (!bt) {
			do_say(bounty_master, "Sorry, I don't know that bounty", CMD_SAY);
			return (TRUE);
		}
		do_say(bounty_master, "Ahh...glad that fucker is finally dead, here's your cash.", CMD_SAY);
		GET_GOLD(bounty_master) += bt->amount;
		sprintf(buf, " %d coins %s", bt->amount, GET_NAME(ch));
		do_give(bounty_master, buf, CMD_GIVE);
		sprintf(buf, "BOUNTY: %s collected the bounty on %s (%d).",
			GET_NAME(ch), bt->name, bt->amount);
		main_log(buf);
		spec_log(buf, BOUNTY_LOG);
		remove_bounty_listing(bt);
		bt = NULL;
		/* If they left something in the head... */
		if (obj->contains) {
			do_say(bounty_master, "Ahh, more stuff for me!", CMD_SAY);
			ha1800_obj_from_char(obj);
			ha2100_obj_to_room(obj, bounty_master->in_room);
			do_get(bounty_master, " head", CMD_GET);
			do_get(bounty_master, " obj head", CMD_GET);
			if (bounty_master->carrying)
				au2000_remind_mob_to_auction_later(1, bounty_master, bounty_master->carrying, 50000000);
		}
		ha2700_extract_obj(obj);
		bt1000_write_bounty_list();
		return (TRUE);
	}
}
/* If cmd is 0, then we know the bountymaster is present, if it's CMD_BOUNTY, the
bountymaster is *NOT* present. */
void bt2100_do_bounty(struct char_data * ch, char *arg, int cmd)
{
	struct bounty_info *bt;
	struct char_data *vict;
	char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int lv_amount, lv_cost;

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg1);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg2);

	if (!(*arg1)) {
		if (cmd)
			send_to_char("What do you want?\r\n", ch);
		else
			do_say(bounty_master, "What do you want?", CMD_SAY);
		return;
	}
	if (is_abbrev(arg1, "list")) {
		send_to_char(" Name           Amount     Placed by\r\n", ch);
		send_to_char("-------------------------------------------------\r\n", ch);
		for (bt = bounty_list; bt; bt = bt->next) {
			sprintf(buf, " %-13s %10d  %s\r\n", bt->name, bt->amount, bt->placed_by);
			send_to_char(buf, ch);
		}
	}
	else if (is_abbrev(arg1, "payoff")) {
		if (cmd) {
			send_to_char("I don't see the Bounty Master!\r\n", ch);
			return;
		}
		if (!bt1200_bounty_check(ch)) {
			do_say(bounty_master, "But you don't have a bounty on your head...", CMD_SAY);
			return;
		}
		bt = find_bounty_for_char(ch);

		if ((bt->amount + (bt->amount * PAYOFF_MARKUP)) > GET_GOLD(ch)) {
			sprintf(buf, "Sure, %s, I'd let you payoff your own bounty (for a mark up, of course) "
				"but it'd cost you %d, which is more than your poor ass has!",
				GET_NAME(ch), (int) (bt->amount + (bt->amount * PAYOFF_MARKUP)));
			do_say(bounty_master, buf, CMD_SAY);
			return;
		}
		sprintf(buf, "Ok, I'll remove the bounty of %d off your head for %d, %s.",
			bt->amount, (int) (bt->amount + (bt->amount * PAYOFF_MARKUP)), GET_NAME(ch));
		do_say(bounty_master, buf, CMD_SAY);
		bzero(buf, sizeof(buf));
		sprintf(buf, "BOUNTY: %s payed off the bounty of %d on their head for %d.",
			GET_NAME(ch), bt->amount, (int) (bt->amount + (bt->amount * PAYOFF_MARKUP)));
		main_log(buf);
		spec_log(buf, BOUNTY_LOG);
		GET_GOLD(ch) -= (bt->amount + (bt->amount * PAYOFF_MARKUP));
		remove_bounty_listing(bt);
		bt1000_write_bounty_list();
	}
	else {

		if (IS_PC(ch) && cmd) {
			send_to_char("The bounty master is nowhere to be seen!", ch);
			return;
		}

		if (IS_PC(ch) && GET_LEVEL(ch) > 41 && GET_LEVEL(ch) < 49) {
			send_to_char("Why? Why?\r\n", ch);
			return;
		}

		if (!(vict = get_char(arg1))) {
			do_say(bounty_master, "Well, I can't find that person!", CMD_SAY);
			return;
		}

		if (!IS_PC(vict)) {
			do_say(bounty_master, "Sorry, I don't place bounties on monsters.", CMD_SAY);
			return;
		}

		if (IS_PC(ch)) {
			if (!(IS_SET(GET_ACT2(vict), PLR2_PKILLABLE) &&
			      IS_SET(GET_ACT2(ch), PLR2_PKILLABLE))) {
				do_say(bounty_master, "I hate that guy, too, but it's against the rules for me to put a bounty on him.",
				       CMD_SAY);
				return;
			}
		}

		if (!isdigit(*arg2)) {
			do_say(bounty_master, "And how much gold do you want to pay to the killer?", CMD_SAY);
			return;
		}

		lv_amount = atoi(arg2);
		//Need to markup the price, too.
			if ((lv_amount > MAX_BOUNTY || lv_amount < MIN_BOUNTY) && IS_PC(ch)) {
			sprintf(buf, "Sorry, but the bounty must be in between %d and %d", MIN_BOUNTY, MAX_BOUNTY);
			do_say(bounty_master, buf, CMD_SAY);
			return;
		}

		/* Does the victim already have a bounty placed on them? */
		if (!bt1200_bounty_check(vict)) {
			lv_cost = lv_amount + ((lv_amount * BOUNTY_MARKUP) / 100);
			if (lv_cost > GET_GOLD(ch) && IS_PC(ch)) {
				sprintf(buf, "Sorry, that would cost, including my usual fee, %d, which you don't have!", lv_cost);
				do_say(bounty_master, buf, CMD_SAY);
				return;
			}
			CREATE(bt, struct bounty_info, 1);
			if (IS_PC(ch)) {
				GET_GOLD(ch) -= lv_cost;
				bt->placed_by = strdup(GET_NAME(ch));
			}
			else
				bt->placed_by = strdup((ch)->player.short_descr);
			bt->name = strdup(GET_NAME(vict));
			bt->amount = lv_amount;
			bt->killed = 0;
			if (!IS_PC(ch))
				bt->killable = 1;
			//Add it to the linked list
				bt->next = bounty_list;
			bounty_list = bt;
		}
		else {
			bt = find_bounty_for_char(vict);
			if (bt->amount + lv_amount > MAX_BOUNTY) {
				lv_amount = MAX_BOUNTY - bt->amount;
				sprintf(buf, "I can't place bounties that high, so I'll make it %d, with you contribution of %d",
					MAX_BOUNTY, lv_amount);
				if (IS_PC(ch))
					do_say(bounty_master, buf, CMD_SAY);
			}
			lv_cost = lv_amount + (lv_amount * BOUNTY_MARKUP);
			if (lv_cost > GET_GOLD(ch) && IS_PC(ch)) {
				sprintf(buf, "Sorry, that would cost, including my usual fee, %d, which you don't have!", lv_cost);
				do_say(bounty_master, buf, CMD_SAY);
				return;
			}
			if (IS_PC(ch)) {
				GET_GOLD(ch) -= lv_cost;
				bt->placed_by = strdup(GET_NAME(ch));
			}
			else {
				bt->killable = 1;
				bt->placed_by = strdup((ch)->player.short_descr);
			}
			bt->amount += lv_amount;
		}

		sprintf(buf, "%s placed a bounty of %d on the head of %s.",
			bt->placed_by, bt->amount, bt->name);
		main_log(buf);
		spec_log(buf, BOUNTY_LOG);
		bt1000_write_bounty_list();
		sprintf(buf, "%s now has a bounty of %d on their head!", bt->name, bt->amount);
		do_gossip(bounty_master, buf, CMD_GOSSIP);
		do_gossip(bounty_master, "Go kill 'em!", CMD_GOSSIP);
	}
}

int bt2200_bounty_hunter(struct char_data * ch, int cmd, char *arg)
{
	struct char_data *victim;
	struct obj_data *head;
	struct bounty_info *bt;
	char buf[MAX_STRING_LENGTH];

	if (!bounty_hunter)
		return (FALSE);

	head = ha2075_get_obj_list_vis(bounty_hunter, "head", bounty_hunter->carrying);

	if (cmd)
		return (FALSE);
	else {
		if (head) {
			//Dropping heads should take priority.
				wi1500_do_goto(bounty_hunter, " 21", CMD_GOTO);
			do_drop(bounty_hunter, " head", CMD_DROP);
		}
		else if (GET_FIGHTING(bounty_hunter)) {
			/* Make sure we don 't just hop from fight to fight.
			 * Think of fun / badass things to do here. */
			if (GET_FIGHTING(bounty_hunter)->in_room != bounty_hunter->in_room) {
				bzero(buf, sizeof(buf));
				strncpy(buf, GET_NAME(ch), sizeof(buf));
				wi1500_do_goto(bounty_hunter, buf, CMD_GOTO);
			}
			else {
				/* Here 's where the badass stuff goes. */
			}
			return (TRUE);
		}
		else if (bounty_list) {
			//If we have some bounties, try to find a target online.
				for (bt = bounty_list; bt; bt = bt->next) {
				victim = ha3100_get_char_vis(ch, bt->name);
				if (victim)
					break;
			}
			if (victim && bt1200_bounty_check(victim)) {
				sprintf(buf, " %s", GET_NAME(victim));
				if (victim->in_room == bounty_hunter->in_room) {
					at1000_do_hit(bounty_hunter, buf, CMD_HIT);
					return (TRUE);
				}
				else if (number(1, 100) < 30) {
					wi1500_do_goto(bounty_hunter, buf, CMD_GOTO);
					return (TRUE);
				}
				else if (number(1, 100) < 40) {
					bzero(buf, sizeof(buf));
					sprintf(buf, " %s I'm gonna tear you a new one!", GET_NAME(victim));
					do_tell(bounty_hunter, buf, CMD_TELL);
					return (TRUE);
				}
			}
		}
	}
	return (TRUE);
}

int bt2300_perform_mob_bounty(struct char_data * mob, struct char_data * victim)
{
	return 0;
}
/*int bt2300_perform_mob_bounty(struct char_data *mob, struct char_data *victim) {
char buf[MAX_STRING_LENGTH];
int lv_amount, tmp;


// Really low occurance rate
if (GET_LEVEL(victim) < 35 || GET_LEVEL(mob) < 41 || (number(0,999) > 2))
return 0;

tmp = number(0, 13);

switch (tmp) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4: lv_amount = 100000000; break;
		case 5:
		case 6: lv_amount = 125000000; break;
		case 7:
		case 8: lv_amount = 175000000; break;
		case 9:
		case 10: lv_amount = 250000000; break;
		case 12: lv_amount = 350000000; break;
		case 13: lv_amount = 500000000; break;
		default: return 0;
}

sprintf(buf, "BOUNTY: Mob: %s placed a bounty of %d upon %s.",
				GET_NAME(mob), lv_amount, GET_NAME(victim));
main_log(buf);
spec_log(buf, BOUNTY_LOG);

sprintf(buf, " %s %d", GET_NAME(victim), lv_amount);
bt2100_do_bounty(mob, buf, CMD_BOUNTY);
return 1;
}
*/
