/* ft */
/* gv_location: 5001-5500 */
/* *******************************************************************
*  File: fight.c , Combat module.                    Part of DIKUMUD *
*  Usage: Combat system and messages.                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "constants.h"
#include "spells.h"
#include "ansi.h"
#include "fight.h"
#include "globals.h"
#include "mobact.h"
#include "bounty.h"
#include "func.h"

/* Function prototypes */
int pc3000_pulse_attack(struct char_data * ch, struct char_data * victim);

/* Structures */
/* head of l-list of fighting chars   */
struct char_data *combat_list = 0;

/* Next dude global trick           */
struct char_data *combat_next_dude = 0;


/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] = {
	{"hit", "hits"},	/* TYPE_HIT      */
	{"pound", "pounds"},	/* TYPE_BLUDGEON */
	{"pierce", "pierces"},	/* TYPE_PIERCE   */
	{"slash", "slashes"},	/* TYPE_SLASH    */
	{"whip", "whips"},	/* TYPE_WHIP     */
	{"claw", "claws"},	/* TYPE_CLAW     */
	{"bite", "bites"},	/* TYPE_BITE     */
	{"sting", "stings"},	/* TYPE_STING    */
	{"crush", "crushes"}	/* TYPE_CRUSH    */
};

void ft_get_damage_string(int dmg, char *buf)
{
	char *colors = "gyrGYRRR";
	int color;

	if (!buf)
		return;

	*buf = '\0';

	if (dmg <= 0)
		return;

	color = dmg / 200;
	if (color > 7)
		color = 7;

	sprintf(buf, " &W[&%c%d&W]&n", colors[color], dmg);
}


/* The Fight related routines */

void ft1000_appear(struct char_data * ch)
{

	if (ha1375_affected_by_spell(ch, SPELL_INVISIBLE))
		ha1350_affect_from_char(ch, SPELL_INVISIBLE);

	if (!(ha1375_affected_by_spell(ch, SPELL_IMPROVED_INVIS))) {
		REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
		act("$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);
	}
}


void ft1100_load_message(void)
{

	FILE *f1;
	int i, type;
	struct message_type *messages;
	char chk[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];



	if (!(f1 = fopen(MESS_FILE, "r"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: in routine ft1100_load_message\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	for (i = 0; i < MAX_MESSAGES; i++) {
		fight_messages[i].a_type = 0;
		fight_messages[i].number_of_attacks = 0;
		fight_messages[i].msg = 0;
	}

	fscanf(f1, " %s \n", chk);

	while (*chk == 'M') {
		fscanf(f1, " %d\n", &type);
		//printf("%s\n", chk);
		for (i = 0; (i < MAX_MESSAGES) && (fight_messages[i].a_type != type) && (fight_messages[i].a_type); i++) {
			//printf("i ** %d\n", i);
		}

		if (i >= MAX_MESSAGES) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR:Too many combat messages.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}

		CREATE(messages, struct message_type, 1);
		fight_messages[i].number_of_attacks++;
		fight_messages[i].a_type = type;
		messages->next = fight_messages[i].msg;
		fight_messages[i].msg = messages;

		//printf("%d -- %d\n", i, type);

		messages->die_msg.attacker_msg = db7000_fread_string(f1);
		messages->die_msg.victim_msg = db7000_fread_string(f1);
		messages->die_msg.room_msg = db7000_fread_string(f1);
		messages->miss_msg.attacker_msg = db7000_fread_string(f1);
		messages->miss_msg.victim_msg = db7000_fread_string(f1);
		messages->miss_msg.room_msg = db7000_fread_string(f1);
		messages->hit_msg.attacker_msg = db7000_fread_string(f1);
		messages->hit_msg.victim_msg = db7000_fread_string(f1);
		messages->hit_msg.room_msg = db7000_fread_string(f1);
		messages->god_msg.attacker_msg = db7000_fread_string(f1);
		messages->god_msg.victim_msg = db7000_fread_string(f1);
		messages->god_msg.room_msg = db7000_fread_string(f1);
		fscanf(f1, " %s \n", chk);
		//printf("Last -- %s\n", chk);
	}

	fclose(f1);
}				/* END OF ft1100_load_message() */


void ft1200_update_pos(struct char_data * victim)
{



	if ((GET_HIT(victim) > 0) && (GET_POS(victim) > POSITION_STUNNED))
		return;
	else if (GET_HIT(victim) > 0)
		GET_POS(victim) = POSITION_STANDING;
	else if (GET_HIT(victim) <= -11)
		GET_POS(victim) = POSITION_DEAD;
	else if (GET_HIT(victim) <= -6)
		GET_POS(victim) = POSITION_MORTALLYW;
	else if (GET_HIT(victim) <= -3)
		GET_POS(victim) = POSITION_INCAP;
	else
		GET_POS(victim) = POSITION_STUNNED;

}				/* END OF ft1200_update_pos() */


/* start one char fighting another (yes, it is horrible, I know... )  */
void ft1300_set_fighting(struct char_data * ch, struct char_data * vict, int lv_flag)
{

	/* lv_flag not used at this time.   Turned out I didn't need it */



	if (ch->specials.fighting)
		return;

	ch->next_fighting = combat_list;
	combat_list = ch;

	if (IS_AFFECTED(ch, AFF_SLEEP))
		ha1350_affect_from_char(ch, SPELL_SLEEP);

	/* mark if not self-defense */
	/* IF THE OPPONENT IS ALREADY FIGHTING, TAKE THE */
	/* OPPOSITE OF HIS/HER ATTACKER FLAG             */
	if (vict->specials.fighting) {
		if (IS_SET(GET_ACT2(vict), PLR2_ATTACKER)) {
			REMOVE_BIT(GET_ACT2(ch), PLR2_ATTACKER);
		}
		else {
			if (!IS_SET(GET_ACT2(ch), PLR2_ATTACKER))
				if (cl1900_at_war(ch, vict))
					cl1500_attack_player(ch, vict);
			SET_BIT(GET_ACT2(ch), PLR2_ATTACKER);
		}
	}
	else {
		if (!IS_SET(GET_ACT2(ch), PLR2_ATTACKER))
			if (cl1900_at_war(ch, vict))
				cl1500_attack_player(ch, vict);
		SET_BIT(GET_ACT2(ch), PLR2_ATTACKER);
		if (IS_SET(ch->specials.act, PLR1_WIMPY)) {
			hu9000_call_for_help(ch, vict, 2);
		}
	}

	if (IS_SET(GET_ACT1(ch), PLR1_SIGNAL_HUNT_HELP)) {
		hu9000_call_for_help(ch, vict, 1);
	}

	/*
	 if ((IS_SET(GET_ACT2(ch), PLR2_ATTACKER)))
   printf("%s is fighting %s and has ATTACKER flag\r\n",
					GET_REAL_NAME(ch), GET_REAL_NAME(vict));
	 else
   printf("%s is fighting %s and DOES NOT have ATTACKER flag\r\n",
					GET_REAL_NAME(ch), GET_REAL_NAME(vict));
	 */

	ch->specials.fighting = vict;
	GET_POS(ch) = POSITION_FIGHTING;

}				/* END OF ft1300_set_fighting() */



/* remove a char from the list of fighting chars */
void ft1400_stop_fighting(struct char_data * ch, int lv_flag)
{

	struct char_data *tmp;



	if (!ch) {
		main_log("ERROR: ft1400_stop_fighting: NULL ch");
		spec_log("ERROR: ft1400_stop_fighting: NULL ch", ERROR_LOG);
		return;
	}

	if (!ch->specials.fighting) {
		main_log("ERROR: ft1400_stop_fighting: (fight.c) ch->specials.fighting == NUL");
		spec_log("ERROR: ft1400_stop_fighting: (fight.c) ch->specials.fighting == NUL", ERROR_LOG);
		return;
	}

	/* WE AREN'T RAGING ANYMORE */
	REMOVE_BIT(ch->specials.affected_by, AFF_RAGE);

	if (IS_PC(ch)) {
		REMOVE_BIT(GET_ACT2(ch), PLR2_ATTACKER);
	}

	if (ch == combat_next_dude)
		combat_next_dude = ch->next_fighting;

	if (combat_list == ch)
		combat_list = ch->next_fighting;
	else {
		for (tmp = combat_list; tmp && (tmp->next_fighting != ch);
		     tmp = tmp->next_fighting);
		if (!tmp) {
			main_log("ERROR: Char fighting not found Error (fight.c, ft1400_stop_fighting)");
			main_log(GET_REAL_NAME(ch));
			send_to_char("Hrmm, You're not in fight list...\r\n", ch);
		}
		else {
			tmp->next_fighting = ch->next_fighting;
		}
	}

	ch->next_fighting = 0;
	ch->specials.fighting = 0;
	GET_POS(ch) = POSITION_STANDING;
	ft1200_update_pos(ch);

}				/* END OF ft1400_stop_fighting() */



#define MAX_NPC_CORPSE_TIME 5
#define MAX_PC_CORPSE_TIME 10
#define MAX_PC_CORPSE_TIME2 7200

void ft1500_make_corpse(struct char_data * ch, struct char_data * killer)
{
	struct obj_data *corpse, *o;
	struct obj_data *money;
	char buf[MAX_STRING_LENGTH];
	int i;



	CREATE(corpse, struct obj_data, 1);
	db7600_clear_object(corpse);

	corpse->item_number = NOWHERE;
	corpse->in_room = NOWHERE;
	sprintf(buf, "corpse %s", GET_REAL_NAME(ch));
	corpse->name = strdup(buf);

	sprintf(buf, "Corpse of %s is lying here.",
		GET_REAL_NAME(ch));
	corpse->description = strdup(buf);

	sprintf(buf, "Corpse of %s",
		GET_REAL_NAME(ch));
	corpse->short_description = strdup(buf);

	corpse->contains = ch->carrying;

	if (IS_NPC(ch) &&
	    GET_GOLD(ch) > 0) {
		money = ha3400_create_money(GET_GOLD(ch));
		GET_GOLD(ch) = 0;
		ha2300_obj_to_obj(money, corpse);
	}

	corpse->obj_flags.type_flag = ITEM_CONTAINER;
	corpse->obj_flags.wear_flags = ITEM_TAKE;
	corpse->obj_flags.value[0] = 0;	/* You can't store stuff in a corpse */
	if (IS_PC(ch)) {
		if (IS_PC(killer) &&
		    IS_SET(GET_ACT2(ch), PLR2_PKILLABLE) &&
		    IS_SET(GET_ACT2(killer), PLR2_PKILLABLE)) {
			corpse->obj_flags.value[1] = killer->nr;	/* Killers ID */
		}
		else {
			corpse->obj_flags.value[1] = 0;
		}
		corpse->obj_flags.value[2] = ch->nr;	/* Victims ID */
	}
	else {
		corpse->obj_flags.value[1] = 0;
		corpse->obj_flags.value[2] = 0;
	}


	corpse->obj_flags.value[3] = 1;	/* corpse identifyer */
	corpse->obj_flags.weight = GET_WEIGHT(ch) + IS_CARRYING_W(ch);
	corpse->obj_flags.cost_per_day = 100000;

	if (IS_NPC(ch)) {
		sprintf(buf, "%d", mob_index[ch->nr].virtual);
		corpse->action_description = strdup(buf);
	}
	else
		corpse->action_description = 0;


	if (IS_NPC(ch))
		corpse->obj_flags.timer = MAX_NPC_CORPSE_TIME;
	else if (GET_LEVEL(ch) < 10)
		corpse->obj_flags.timer = MAX_PC_CORPSE_TIME;
	else
		corpse->obj_flags.timer = MAX_PC_CORPSE_TIME2;

	for (i = 0; i < MAX_WEAR; i++)
		if (ch->equipment[i])
			ha2300_obj_to_obj(ha1930_unequip_char(ch, i), corpse);

	ch->carrying = 0;
	IS_CARRYING_N(ch) = 0;
	IS_CARRYING_W(ch) = 0;

	corpse->next = object_list;
	object_list = corpse;

	for (o = corpse->contains; o; o->in_obj = corpse, o = o->next_content);
	ha2500_object_list_new_owner(corpse, 0);

	ha2100_obj_to_room(corpse, ch->in_room);

	return;

}				/* END OF ft1500_make_corpse() */


/* When ch kills victim */
void ft1600_change_alignment(struct char_data * ch, struct char_data * victim)
{
	int increment;



	if (GET_ALIGNMENT(victim) > 0) {
		/* KILLED SOMEONE GOOD - WE ARE GETTING EVILER   10 points for
		 * each 100 */
		increment = (GET_ALIGNMENT(victim) / 100) * 10;
		GET_ALIGNMENT(ch) -= increment;
	}
	else {
		/* KILLED SOMEONE EVIL - WE ARE GETTING HOLIER    5 points for
		 * each 100 */
		increment = ((GET_ALIGNMENT(victim) * -1) / 100) * 5;
		GET_ALIGNMENT(ch) += increment;
	}

	/* MAKE SURE WE DIDN'T GO OUT OF BOUNDS */
	if (GET_ALIGNMENT(ch) > 1000)
		GET_ALIGNMENT(ch) = 1000;
	if (GET_ALIGNMENT(ch) < -1000)
		GET_ALIGNMENT(ch) = -1000;

	return;
}				/* END OF ft1600_change_alignment() */


void ft1700_death_cry(struct char_data * ch)
{
	int door, lv_room;
	struct char_data *tmp_ch;



	/* This is redundant they must drop link, quit or rent out */
	/* we should create file in case crash occurs between corpse
	 * retrieval^save */
	r2300_del_char_objs(ch);

	act("Your blood freezes as you hear $n's death cry.",
	    FALSE, ch, 0, 0, TO_ROOM);

	for (door = 0; door <= 5; door++) {
		if (CAN_GO(ch, door)) {
			lv_room = world[ch->in_room].dir_option[door]->to_room;
			if (ch->in_room != lv_room) {
				tmp_ch = world[lv_room].people;
				if (tmp_ch) {
					act("Your blood freezes as you hear someones death cry.",
					    FALSE, tmp_ch, 0, 0, TO_CHAR);
					act("Your blood freezes as you hear someones death cry.",
					    FALSE, tmp_ch, 0, 0, TO_ROOM);
				}	/* END OF tmp_ch */
			}	/* END OF DIFFERENT ROOM */
		}		/* END OF VALID DIRECTION */
	}			/* END OF FOR door LOOP */
	return;

}				/* END OF ft1700_death_cry() */


void ft1800_raw_kill(struct char_data * ch, struct char_data * killer)
{
	struct obj_data *head;



	if (ch->specials.fighting)
		ft1400_stop_fighting(ch, 0);

	ft1700_death_cry(ch);

	if (GET_HOW_LEFT(ch) != LEFT_BY_LIGHTNING_DEATH)
		GET_HOW_LEFT(ch) = LEFT_BY_DEATH_FIGHT;
	if (gv_mega_verbose_messages == TRUE) {
		main_log("Leaving by death fight.");
	}

	/* CHECK FOR SPECIAL MOB PROCEDURES */
	if (IS_SET(GET_ACT2(ch), PLR2_DEATH_PROCEDURE)) {
		if (ft1900_special_procedure(ch)) {
			return;
		}
	}			/* END OF SPECIAL DEATH PROCEDURE */

	/* If we just killed an lvl 25+, there is a 30 % chance we 'll take an
	 * item and auction it. */
	if (ch != killer && IS_PC(ch) && IS_NPC(killer) &&
	    (GET_LEVEL(ch) > 25 && GET_LEVEL(ch) <= PK_LEV) && /* lvl 25 and PK level */
	    (number(0, 99) < 15)) {
		ft1850_remove_item_from_dying_char_and_auction_it(ch, killer);
	}


/* Here's where we check to see if there's a bounty on their head.  If they're not killing themself
   or if their not killing their own clanny, then give them a head.  Also, penalties?  Also, what to do
	 if the Bounty Hunter get's them. */

	if (bt1250_can_attack(killer, ch) && killer != ch &&
	    (IS_PC(killer) || killer == bounty_hunter)) {
		if (CLAN_NUMBER(ch) != CLAN_NUMBER(killer) ||
		    (CLAN_NUMBER(ch) == NO_CLAN ||
		     !(CLAN_EXISTS(CLAN_NUMBER(ch))) ||
		     (CLAN_RANK(ch) == CLAN_APPLY_RANK ||
		      CLAN_RANK(killer) == CLAN_APPLY_RANK)) ||
		    killer == bounty_hunter) {
			if (IS_SET(GET_ACT2(ch), PLR2_PKILLABLE) && IS_SET(GET_ACT2(killer), PLR2_PKILLABLE))
				head = bt1400_make_head(ch, FALSE);
			else
				head = bt1400_make_head(ch, FALSE);
			ha1700_obj_to_char(head, killer);
		}
		//Put penalties here ?
	}

	if (IS_SET(GET_ACT2(ch), PLR2_QUEST))
		REMOVE_BIT(GET_ACT2(ch), PLR2_QUEST);

	ft1500_make_corpse(ch, killer);
	ha3000_extract_char(ch, END_EXTRACT_BY_RAW_KILL);

	return;

}				/* END OF ft1800_raw_kill() */

int ft1849_determine_min_bid_amount(int level, struct obj_data * obj)
{
	/* level is the level of the killed.  Make it harder for avats * and
	 * not for mortals.                                        */

#define LADONDRA 2447
#define HAMMERFANG 1516
#define SHATTERER 8900
#define FIREBALL 15707
#define FYREBRAND 7763
#define EARTH 15708
#define MADNESS 20582
#define DARKSOULS 19204
#define PENTAGRAM 15524
#define SCEPTRE 15699
#define NADALS 17915
#define BOOTSWAR 15705
#define SABBAT 20616
#define BLUEDRAGON 2222
#define DARKNESS 6604
#define CARAPACE 1755

	if (obj_index[obj->item_number].virtual == LADONDRA)
		return 500000000;
	if (obj_index[obj->item_number].virtual == HAMMERFANG ||
	    obj_index[obj->item_number].virtual == SHATTERER ||
	    obj_index[obj->item_number].virtual == FIREBALL ||
	    obj_index[obj->item_number].virtual == FYREBRAND ||
	    obj_index[obj->item_number].virtual == EARTH ||
	    obj_index[obj->item_number].virtual == MADNESS ||
	    obj_index[obj->item_number].virtual == SCEPTRE ||
	    obj_index[obj->item_number].virtual == PENTAGRAM ||
	    obj_index[obj->item_number].virtual == DARKSOULS ||
	    obj_index[obj->item_number].virtual == NADALS ||
	    obj_index[obj->item_number].virtual == BOOTSWAR ||
	    obj_index[obj->item_number].virtual == SABBAT ||
	    obj_index[obj->item_number].virtual == BLUEDRAGON ||
	    obj_index[obj->item_number].virtual == DARKNESS ||
	    obj_index[obj->item_number].virtual == CARAPACE)
		return 100000000;

	if (level < IMO_LEV)
		return 50000;
	int temp = number(0, 99);

	if (temp < 25)
		return 3000000;
	if (temp < 45)
		return 5000000;
	if (temp < 60)
		return 7500000;
	if (temp < 70)
		return 10000000;
	if (temp < 80)
		return 25000000;
	if (temp < 88)
		return 50000000;
	if (temp < 94)
		return 70000000;
	if (temp < 99)
		return 80000000;
	return 100000000;

}


void ft1850_remove_item_from_dying_char_and_auction_it(struct char_data * ch,
						    struct char_data * killer)
{
	struct obj_data *lv_obj;
	int idx;
	bool lv_FoundItem;
	char buf[MAX_STRING_LENGTH];

	if (!ch) {
		return;
	}

	if (!killer) {
		return;
	}

	if (IS_SET(GET_ACT2(killer), PLR2_JUNKS_EQ)) {
		return;
	}

	/* We 'll enter a loop and when we stop on the last item, we' ll take
	 * it Aww...that sucks ! Let 's just look for the good stuff, first,
	 * weapons/on body/slots with extra attack. */

	lv_FoundItem = TRUE;
	/* Stay in loop at least once */
	int i;
	/* look through the mob 's eq first, at most four times, for four
	 * specific slots. */
	for (i = 0; i < 5; i++) {
		int desired_type;
		int temp = number(0, 10);
		switch (temp) {
		case 0:
		case 1:
		case 2:
		case 3:
			desired_type = ITEM_WIELD;
			break;
		case 4:
		case 5:
		case 6:
			desired_type = ITEM_WEAR_BODY;
			break;
		case 7:
		case 8:
			desired_type = ITEM_WEAR_FEET;
			break;
		default:
			desired_type = ITEM_HOLD;
			break;
		}
		lv_FoundItem = FALSE;
		lv_obj = NULL;

		/* Assuming best items are equiped, lets check there first */
		for (idx = 0; idx < MAX_WEAR; idx++) {
			lv_obj = ch->equipment[idx];
			if (lv_obj) {
				if (CAN_SEE_OBJ(killer, lv_obj) &&
				    GET_ITEM_TYPE(lv_obj) != ITEM_CONTAINER &&
				    GET_ITEM_TYPE(lv_obj) != ITEM_QSTCONT &&
				    check_nodrop(lv_obj) == FALSE &&
				    check_lore(killer, lv_obj) == FALSE) {
					if (temp == 3) {
						if (GET_ITEM_TYPE(lv_obj) == ITEM_LIGHT) {
							lv_obj = ha1930_unequip_char(ch, idx);
							ha1700_obj_to_char(lv_obj, killer);
							act("You remove $p from $N's corpse.", TRUE, killer, lv_obj, ch, TO_CHAR);
							act("$n removes $p from $N's corpse.", TRUE, killer, lv_obj, ch, TO_ROOM);
							REMOVE_BIT(GET_ACT3(ch), PLR3_NOAUCTION);
							au2000_remind_mob_to_auction_later(1, killer, lv_obj,
											   ft1849_determine_min_bid_amount(GET_LEVEL(ch), lv_obj));
							sprintf(buf, "%s looted %s from %s", GET_REAL_NAME(killer), GET_OSDESC(lv_obj), GET_REAL_NAME(ch));
							do_wizinfo(buf, IMO_IMM, ch);
							spec_log(buf, FUN_LOG);
							return;
						}
					}
					else {
						if (CAN_WEAR(lv_obj, desired_type)) {
							/* TAKE THIS ITEM AND
							 * PASS TO KILLER MOB */
							lv_obj = ha1930_unequip_char(ch, idx);
							ha1700_obj_to_char(lv_obj, killer);
							act("You remove $p from $N's corpse.", TRUE, killer, lv_obj, ch, TO_CHAR);
							act("$n removes $p from $N's corpse.", TRUE, killer, lv_obj, ch, TO_ROOM);
							//Be kind and turn on auction for avatar
								REMOVE_BIT(GET_ACT3(ch), PLR3_NOAUCTION);
							au2000_remind_mob_to_auction_later(1, killer, lv_obj, ft1849_determine_min_bid_amount(GET_LEVEL(ch), lv_obj));
							sprintf(buf, "%s looted %s from %s", GET_REAL_NAME(killer), GET_OSDESC(lv_obj), GET_REAL_NAME(ch));
							do_wizinfo(buf, IMO_IMM, ch);
							spec_log(buf, FUN_LOG);
							return;
						}
					}
				}	/* Can see and not a container */
			}	/* found object */
		}		/* for () loop looking at equipment */
	}			/* for loop looking at eq 5 times for specific
				 * eq... */

	if (lv_FoundItem == FALSE) {
		sprintf(buf, "Curses!  I really wanted to get something from %s, but %she's got nothing I like!!",
			GET_NAME(ch), (GET_SEX(ch) == SEX_FEMALE) ? "s" : "");
		do_gossip(killer, buf, CMD_GOSSIP);
	}

	return;

} //END OF ft1850_remove_item_from_dying_char()

int ft1900_special_procedure(struct char_data *ch)
{

	int lv_obj_num;
	struct obj_data *lv_obj;

	if (IS_PC(ch)) {
		return (FALSE);
	}

	if (ch->in_room < 0) {
		return (FALSE);
	}

	if (mob_index[ch->nr].virtual == MOB_GERINSHILL_FLAME_ELEMENTAL) {
		wi2900_purge_items(ch, "\0", 0);
		/* LETS CHECK TO SEE IF REPLACEMENT SWORD EXIT */
		lv_obj_num = db8200_real_object(OBJ_GERINSHILL_FLAME_SWORD);
		/* IF OBJECT IS MAXED, LEAVE A CORPSE */
		if (obj_index[lv_obj_num].number < obj_index[lv_obj_num].maximum) {
			lv_obj = db5100_read_object(lv_obj_num, REAL);
			ha2100_obj_to_room(lv_obj, ch->in_room);
			return (TRUE);
		}
		return (FALSE);
	}			/* END OF flame elemental */

	return (FALSE);

}				/* END OF ft1900_special_procedure() */


/* ft2000_die */
void ft2000_die(struct char_data * ch, struct char_data * killer)
{
	int i, lv_lose_flags;
	long lose;
	char buf[128];

	if (auction_data.auctioner && auction_data.auctioner == ch)
		au1400_do_cancel(ch, "died", CMD_CANCEL_DEAD);
	if (auction_data.bidder)
		au1800_remove_bidder(ch, 0);

	if (ch == killer_mob) {
		killer_mob = 0;
	}

	lv_lose_flags = TRUE;
	if (killer) {
		/* DON'T LOSE FLAGS FOR UNCHARMED MOBS */
		if (IS_UNCHARMED_NPC(killer)) {
			lv_lose_flags = FALSE;
		}
	}


	if (IS_PC(ch)) {
		/* REMOVE SPELLS */
		for (i = 0; i < MAX_AFFECT; i++) {
			if (ch->affected)
				ha1325_affect_remove(ch, ch->affected, 0);
		}

		li9800_set_alignment(ch);

		pi1000_show_rip(ch);

		if (GET_LEVEL(ch) > 5) {
			if (IS_PC(ch) && (ch->in_room > 0)) {
				if (!IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA)) {
					GET_DEATHS(ch)++;
					GET_SCORE(ch) -= 100;
					if ((ch->nr >= 0) && (ch->nr <= top_of_p_file))
						player_table[ch->nr].pidx_score = GET_SCORE(ch);
					in5300_update_top_list(ch, top_deaths);
				}
			}	/* END OF IS_PC() */

			if (!IS_PC(killer)) {
				lose = MINV(GET_EXP(ch) >> 4, 25000 * GET_LEVEL(ch));
				GET_EXP(ch) -= lose;
				sprintf(buf, "You lose %ld experience points!\r\n", lose);
				send_to_char(buf, ch);
				GET_EXP(ch) = MAXV(0, GET_EXP(ch));
			}
		}
	}

	GET_MANA(ch) >>= 1;
	GET_MOVE(ch) >>= 1;

	ft1800_raw_kill(ch, killer);
}				/* END OF ft2000_die */


void ft2100_group_gain(struct char_data * ch, struct char_data * victim)
{
	char buf[256];
	int no_members, share, your_share, no_levels, max_lev = 0;
	struct char_data *k;
	struct follow_type *f;



	k = ch->master;
	if (!(k))
		k = ch;
	max_lev = 0;

	if (IS_AFFECTED(k, AFF_GROUP) &&
	    (k->in_room == ch->in_room)) {
		no_members = 1;
		no_levels = GET_LEVEL(k);
		max_lev = MAXV(max_lev, GET_LEVEL(k));
	}
	else {
		no_members = 0;
		no_levels = 0;
	}

	for (f = k->followers; f; f = f->next) {
		if (IS_AFFECTED(f->follower, AFF_GROUP) &&
		    (f->follower->in_room == ch->in_room)) {
			no_members++;
			no_levels += GET_LEVEL(f->follower);
			max_lev = MAXV(max_lev, GET_LEVEL(f->follower));
		}
	}

	share = (GET_EXP(victim)) / 3;
	if (IS_SET(GET_ACT1(victim), PLR1_CHAR_WAS_HELD)) {
		share = MAXV(1, share / 10);
	}

	/* IF WE KILLED A PC, COMPUTE EXP DIFFERENTLY */
	if (IS_PC(victim)) {
		share = 500 * GET_LEVEL(victim);
	}

	if (victim->in_room > 0)
		if (IS_SET(GET_ROOM2(victim->in_room), RM2_ARENA))
			share = 0;

	if (IS_AFFECTED(k, AFF_GROUP) &&
	    (k->in_room == ch->in_room)) {
		your_share = share * GET_LEVEL(k) / no_levels;
		/* scale xp according to rel. levels */
		your_share += (your_share * MINV(8, (GET_LEVEL(victim) - max_lev))) >> 3;
		your_share = MAXV(1, your_share) * gv_rush_hour;
		sprintf(buf, "You receive your share of %d experience points.", your_share);
		act(buf, FALSE, k, 0, 0, TO_CHAR);
		li2200_gain_exp(k, your_share);
		ft1600_change_alignment(k, victim);
	}

	for (f = k->followers; f; f = f->next) {
		if (IS_AFFECTED(f->follower, AFF_GROUP) &&
		    (f->follower->in_room == ch->in_room)) {
			your_share = share * GET_LEVEL(f->follower) / no_levels;
			your_share += (your_share * MINV(8, (GET_LEVEL(victim) - max_lev))) >> 3;
			your_share = MAXV(1, your_share) * gv_rush_hour;
			sprintf(buf, "You receive your share of %d experience points.", your_share);
			act(buf, FALSE, f->follower, 0, 0, TO_CHAR);
			li2200_gain_exp(f->follower, your_share);
			ft1600_change_alignment(f->follower, victim);
		}
	}
}				/* END OF ft2100_group_gain() */

char *ft2200_replace_string(char *str, char *weapon, char *weapon_p)
{
	static char buf[256];
	char *cp;

	cp = buf;

	for (; *str; str++) {
		if (*str == '#') {
			switch (*(++str)) {
			case 'W':
				for (; *weapon; *(cp++) = *(weapon++));
				break;
			case 'P':
				for (; *weapon_p; *(cp++) = *(weapon_p++));
				break;
			default:
				*(cp++) = '#';
				break;
			}
		}
		else {
			*(cp++) = *str;
		}

		*cp = 0;
	}			/* For */

	return (buf);
}				/* END OF ft2200_replace_string() */


int ft2300_get_fight_message_no(int dam)
{
	int message_number;

	message_number = 0;
	if (dam == 0)
		message_number = 0;
	else if (dam < 5)
		message_number = 1;
	else if (dam < 10)
		message_number = 2;
	else if (dam < 15)
		message_number = 3;
	else if (dam < 20)
		message_number = 4;
	else if (dam < 30)
		message_number = 5;
	else if (dam < 40)
		message_number = 6;
	else if (dam < 60)
		message_number = 7;
	else if (dam < 80)
		message_number = 8;
	else if (dam < 100)
		message_number = 9;
	else if (dam < 120)
		message_number = 10;
	else if (dam < 140)
		message_number = 11;
	else if (dam < 160)
		message_number = 12;
	else if (dam < 180)
		message_number = 13;
	else if (dam < 200)
		message_number = 14;
	else if (dam < 220)
		message_number = 15;
	else if (dam < 250)
		message_number = 16;
	else if (dam < 300)
		message_number = 17;
	else if (dam < 350)
		message_number = 18;
	else if (dam < 400)
		message_number = 19;
	else if (dam < 450)
		message_number = 20;
	else if (dam < 500)
		message_number = 21;
	else if (dam < 550)
		message_number = 22;
	else if (dam < 600)
		message_number = 23;
	else if (dam < 650)
		message_number = 24;
	else if (dam < 700)
		message_number = 25;
	else if (dam < 750)
		message_number = 26;
	else if (dam < 800)
		message_number = 27;
	else if (dam < 850)
		message_number = 28;
	else if (dam < 900)
		message_number = 29;
	else if (dam < 950)
		message_number = 30;
	else if (dam < 1000)
		message_number = 31;
	else if (dam < 1050)
		message_number = 32;
	else if (dam < 1100)
		message_number = 33;
	else
		message_number = 34;

	return (message_number);

}				/* END OF ft2300_get_fight_message_no() */


void ft2400_damage_message(int dam, struct char_data * ch, struct char_data * victim, int w_type)
{

	struct obj_data *wield;
	int message_number;
	char *buf;
	char buf2[MAX_STRING_LENGTH];

	w_type -= TYPE_HIT;	/* Change to base of table with text */
	wield = ch->equipment[WIELD];

	message_number = ft2300_get_fight_message_no(dam);
	ft_get_damage_string(dam, buf2);

	/* MESSAGES TO BYSTANDERS */
	buf = ft2200_replace_string(dam_weapons[message_number].to_room,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_NOTVICT, CLR_FIGHT, AA_FIGHT);

	/* MESSAGES TO ATTACKER */
	buf = ft2200_replace_string(dam_weapons[message_number].to_char,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_CHAR, CLR_HIT, AA_FIGHT);

	/* MESSAGES TO VICTIM */
	buf = ft2200_replace_string(dam_weapons[message_number].to_victim,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_VICT, CLR_DAM, AA_FIGHT);

}				/* END OF ft2400_damage_message() */


void ft2405_damage_message(int dam, struct char_data * ch, struct char_data * victim,
			     int w_type)
{

	struct obj_data *wield;
	int message_number;
	char *buf;
	char buf2[MAX_STRING_LENGTH];



	w_type -= TYPE_HIT;	/* Change to base of table with text */
	wield = ch->equipment[WIELD];

	message_number = ft2300_get_fight_message_no(dam);

	ft_get_damage_string(dam, buf2);

	/* MESSAGES TO BYSTANDERS */
	buf = ft2200_replace_string(dam_weapons[message_number].to_room,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_ROOM, CLR_FIGHT, AA_FIGHT);



	/* MESSAGES TO VICTIM */
	buf = ft2200_replace_string(dam_weapons[message_number].to_victim,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_VICT, CLR_DAM, AA_FIGHT);

}				/* END OF ft2405_damage_message() */



char *ft2200_ft2500_replace_string2(char *str, char *weapon, char *num_attacks)
{
	static char buf[256];
	char *cp;



	cp = buf;

	for (; *str; str++) {
		if (*str == '#') {
			switch (*(++str)) {
			case 'W':
				for (; *weapon; *(cp++) = *(weapon++));
				break;
			case 'H':
				for (; *num_attacks; *(cp++) = *(num_attacks++));
				break;
			default:
				*(cp++) = '#';
				break;
			}
		}
		else {
			*(cp++) = *str;
		}

		*cp = 0;
	}			/* For */

	return (buf);
}				/* END OF ft2200_ft2500_replace_string2() */


void mult_ft2400_damage_message(int dam, struct char_data * ch, struct char_data * victim,
				  int w_type, int hits)
{

	struct obj_data *wield;
	int message_number;
	char *buf;
	char num[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	w_type -= TYPE_HIT;	/* Change to base of table with text */
	wield = ch->equipment[WIELD];

	message_number = ft2300_get_fight_message_no(dam);
	sprintf(num, "%d", hits);
	ft_get_damage_string(dam, buf2);

	/* BYSTANDERS */
	buf = ft2200_ft2500_replace_string2(mult_dam_weapons[message_number].to_room, attack_hit_text[w_type].plural, num);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_NOTVICT, CLR_FIGHT, AA_FIGHT);

	/* ATTACKER */
	buf = ft2200_ft2500_replace_string2(mult_dam_weapons[message_number].to_char,
					    message_number == 2 ? attack_hit_text[w_type].singular : attack_hit_text[w_type].plural, num);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_CHAR, CLR_HIT, AA_FIGHT);

	/* VICTIM */
	buf = ft2200_ft2500_replace_string2(mult_dam_weapons[message_number].to_victim, attack_hit_text[w_type].plural, num);
	strcat(buf, buf2);
	ansi_act(buf, FALSE, ch, wield, victim, TO_VICT, CLR_DAM, AA_FIGHT);

}				/* END OF mult_ft2400_damage_message() */
/* ft3000_damage */

int perform_condition_damage_multiplier(int origdam, struct char_data * ch, struct char_data * victim)
{
	int victcondition, chcondition, victdamagechange, chdamagechange;

	int dam = origdam;

	victcondition = (GET_HIT(victim) * 100) / GET_MAX_HIT(victim);
	chcondition = (GET_HIT(ch) * 100) / GET_MAX_HIT(ch);

	if (chcondition < 5 && CHANCE100(13)) {
		act("$n flies into a blind fury, and attacks "
		    "without any regard to $s own safety.", TRUE, ch, 0, 0, TO_ROOM);
		act("Knowing you don't have much life energy left, "
		    "you attack with total disregard to your own safety.", FALSE, ch, 0, 0, TO_CHAR);
		chdamagechange = 0.7 * dam;
	}
	else if (chcondition < 10)
		chdamagechange = -(dam * 0.35);
	else if (chcondition < 20)
		chdamagechange = -(dam * 0.3);
	else if (chcondition < 30)
		chdamagechange = -(dam * 0.25);
	else if (chcondition < 40)
		chdamagechange = -(dam * 0.2);
	else if (chcondition < 50)
		chdamagechange = -(dam * 0.15);
	else if (chcondition < 60)
		chdamagechange = -(dam * 0.10);
	else if (chcondition < 70)
		chdamagechange = -(dam * 0.05);
	else if (chcondition < 80)
		chdamagechange = 0;
	else if (chcondition < 90)
		chdamagechange = dam * 0.05;
	else
		chdamagechange = dam * 0.15;


	if (victcondition < 5)
		victdamagechange = 0.7 * dam;
	else if (victcondition < 10)
		victdamagechange = dam * 0.35;
	else if (victcondition < 20)
		victdamagechange = dam * 0.3;
	else if (victcondition < 30)
		victdamagechange = dam * 0.25;
	else if (victcondition < 40)
		victdamagechange = dam * 0.2;
	else if (victcondition < 50)
		victdamagechange = dam * 0.15;
	else if (victcondition < 60)
		victdamagechange = dam * 0.1;
	else if (victcondition < 70)
		victdamagechange = dam * 0.05;
	else if (victcondition < 80)
		victdamagechange = 0;
	else if (victcondition < 90)
		victdamagechange = -(dam * 0.05);
	else
		victdamagechange = -(dam * 0.15);

	dam += chdamagechange;
	dam += victdamagechange;
	if (dam < 0)
		dam = 0;
	
	return dam;
}

void damage(struct char_data * ch, struct char_data * victim, int dam, int attacktype, int hits)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char buf4[MAX_STRING_LENGTH];
	struct message_type *messages;
	struct char_data *temp_char;
	int i, orgdam, j, nr, max_hit, exp, lv_ac, lv_extra_damage, lv_sub_damage, lv_exp_gain,
	  lv_display, lv_was_victim_a_pc;

	struct obj_data *wielded;

	lv_display = 1;
	orgdam = dam;

	if (hits == 99) {
		lv_display = 0;
		hits = 1;
	};



	if (IS_AFFECTED(ch, AFF_HOLD_PERSON)) {
		if (IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("You can't fight, you're imitating a rock right now.\r\n", ch);
			return;
		}
	}			/* END OF hold person */

	if (GET_POS(victim) <= POSITION_DEAD) {
		/* We get here for things like warp scroll doing multi attacks. */
		/* main_log("ERROR: Tried to damage a dead person... ugh!"); */
		return;
	}
	if (IS_PC(ch) && ch->desc && (dam > 0)) {
		ch->desc->last_target = victim;
		ch->desc->last_hit_time = time(0);
	}

	if (ch != victim) {
		if (GET_POS(ch) > POSITION_STUNNED) {
			if (!(ch->specials.fighting)) {
				/* check for assist & group here */
				struct char_data *k;
				struct follow_type *f;

				if (ch->master)
					k = ch->master;
				else
					k = ch;

				/* IF THE PERSON WE ARE FIGHTING IS THE LEADER */
				/* DON'T USE THE LEADER TO INCLUDE FOLLOWERS.  */
				if (ch->master == victim) {
					k = ch;
				}

				/* IF THE PERSON WE ARE FIGHTING IS FOLLOWING
				 * THE */
				/* SAME LEADER DON'T INCLUDE OUR GROUP MEMBERS    */
				/* IF BOTH ARE ZERO, k IS EQUAL TO ch ANYWAY      */
				if (ch->master == victim->master) {
					k = ch;
				}

				/* ADD OUR LEADER TO THE FIGHT */
				if ((k != ch)
				    && (!(k->specials.fighting))
				    && (!IS_NPC(k))
				    && (k->in_room == ch->in_room)
				    && (IS_SET(GET_ACT3(k), PLR3_ASSIST))
				    && (IS_AFFECTED(k, AFF_GROUP))) {
					ft1300_set_fighting(k, victim, 1);
					GET_POS(k) = POSITION_FIGHTING;
				}
				/* ADD FOLLOWERS TO THE FIGHT */
				for (f = k->followers; f; f = f->next)
					if ((((IS_AFFECTED(f->follower, AFF_GROUP))
					      && ((IS_NPC(f->follower) || IS_SET(GET_ACT3(f->follower), PLR3_ASSIST))
						  || (IS_AFFECTED(f->follower, AFF_CHARM)))))
					&& (!(f->follower->specials.fighting))
					    && (f->follower->in_room == ch->in_room)
					    && (f->follower != ch)) {
						ft1300_set_fighting(f->follower, victim, 1);
						GET_POS(f->follower) = POSITION_FIGHTING;
					}

				/* SET CHAR TO FIGHTING */
				ft1300_set_fighting(ch, victim, 1);
			}	/* END OF ch->specials.fighting */

			if (IS_NPC(ch) && IS_NPC(victim) && victim->master &&
			    !number(0, 10) && IS_AFFECTED(victim, AFF_CHARM) &&
			    (victim->master->in_room == ch->in_room)) {
				if (ch->specials.fighting)
					ft1400_stop_fighting(ch, 0);
				hit(ch, victim->master, TYPE_UNDEFINED);
				return;
			}
		}		/* END OF position ch > stunned */

		if (GET_POS(victim) > POSITION_STUNNED) {
			if (!(victim->specials.fighting)) {
				/* check for assist & group here */
				struct char_data *k;
				struct follow_type *f;

				if (!(k = victim->master))
					k = victim;
				if ((k != victim)
				    && (!(k->specials.fighting))
				    && (!IS_NPC(k))
				    && (k->in_room == victim->in_room)
				    && (IS_SET(GET_ACT3(k), PLR3_ASSIST))
				    && (IS_AFFECTED(k, AFF_GROUP))
					) {
					ft1300_set_fighting(k, ch, 0);
					GET_POS(k) = POSITION_FIGHTING;
					/* IF OUR LEADER DOESN'T HAVE THE
					 * plr_attacker FLAG */
					/* WE SHOULDN'T EITHER.                             */
				}
				for (f = k->followers; f; f = f->next)
					if ((((IS_AFFECTED(f->follower, AFF_GROUP)) &&
					      (IS_NPC(f->follower) || IS_SET(GET_ACT3(f->follower), PLR3_ASSIST)) &&
					      (IS_AFFECTED(f->follower, AFF_GROUP))) || (IS_AFFECTED(f->follower, AFF_CHARM))) &&
					    (!(f->follower->specials.fighting)) && (f->follower->in_room == victim->in_room) &&
					    (f->follower != victim)) {
						ft1300_set_fighting(f->follower, ch, 0);
						GET_POS(f->follower) = POSITION_FIGHTING;
					}

				/* IF THE FIRST PERSON IN THE FIGHT LEFT/DIED,
				 * WE'LL */
				/* DRAG THE OPPONENT BACK IN */
				ft1300_set_fighting(victim, ch, 0);
			}
			GET_POS(victim) = POSITION_FIGHTING;
		}		/* END OF position victim > stunned */
	}			/* END OF ch != victim */
	if (victim->master == ch) {
		ha4100_stop_follower(victim, END_FOLLOW_BY_TREASON);
		if (victim->specials.fighting == victim) {
			ft1400_stop_fighting(victim, 0);
			ft1300_set_fighting(victim, ch, 0);
		}
	}
	/* Old Location DOES THE WEAPON FLAME/ICE/LIGHTENING? */
	wielded = 0;

	/* EXTRA DAMAGE */
	if (ch != victim &&
	    (attacktype >= TYPE_HIT) &&
	    (attacktype <= TYPE_CRUSH) &&
	    number(0, 101) < ch->skills[SKILL_EXTRA_DAMAGE].learned) {
		li9900_gain_proficiency(ch, SKILL_EXTRA_DAMAGE);
		bzero(buf, sizeof(buf));
		lv_extra_damage = MINV(25, dam / 4);
		lv_extra_damage = MAXV(1, lv_extra_damage);
		dam += lv_extra_damage;
	}

	if (IS_AFFECTED(ch, AFF_RAGE) &&
	    (attacktype >= TYPE_HIT) &&
	    (attacktype <= TYPE_CRUSH)) {
		li9900_gain_proficiency(ch, SKILL_RAGE);
		lv_extra_damage = dam * .2;
		dam += lv_extra_damage;
	}
	/* DECREASE DAMAGE ACCORDING TO AC */
	if (ch != victim &&
	    (attacktype >= TYPE_HIT) &&
	    (attacktype <= TYPE_CRUSH) &&
	    !IS_PC(ch) &&
	    IS_PC(victim)) {
		lv_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim) + GET_BONUS_DEX(victim)) +
			races[GET_RACE(victim)].adj_ac;
		if (lv_ac <= -600)
			lv_ac = -50; //84% of normal
		else if (lv_ac <= -500)
			lv_ac = -48; //86% of normal
		else if (lv_ac <= -400)
			lv_ac = -46; //88% of normal
		else if (lv_ac <= -300)
			lv_ac = -44; //90% of normal
		else if (lv_ac <= -200)
			lv_ac = -42; //92% of normal
		else if (lv_ac <= -100)
			lv_ac = -40; //94% of normal
		else if (lv_ac <= 0)
			lv_ac = -38; //96% of normal
		else if (lv_ac > 0)
			lv_ac = 34; //100% or normal This is not truly 100% of damage but it is new 100% due to previous changes to zones to pump mobs. 
		lv_sub_damage = lv_ac >= 0 ?
			(100 - (lv_ac)) :
			100 + (lv_ac);
		dam = (lv_sub_damage * dam) / 100;
	}
	if (IS_AFFECTED(ch, AFF_INVISIBLE)) {
		ft1000_appear(ch);
	}

	/* Affect the damage by the condition of the two players  */

	if (ch != victim && attacktype >= TYPE_HIT && attacktype <= TYPE_CRUSH) {
		dam = perform_condition_damage_multiplier(dam, ch, victim);
	}

	if (IS_AFFECTED(victim, AFF_SANCTUARY)) {
		if (((attacktype >= TYPE_HIT) && (attacktype <= TYPE_CRUSH)) || IS_PC(victim))
			dam = MAXV(0, dam * 0.50);
		else
			dam = MAXV(0, dam * 0.80);
	}

	if (IS_CASTED_ON(victim, SPELL_BARKSKIN)) {
		if (((attacktype >= TYPE_HIT) && (attacktype <= TYPE_CRUSH)) || IS_PC(victim))
			dam = MAXV(0, dam * 0.80);
		else
			dam = MAXV(0, dam * 0.80);

	}

	if (IS_CASTED_ON(victim, SPELL_STONESKIN)) {
		if (((attacktype >= TYPE_HIT) && (attacktype <= TYPE_CRUSH)) || IS_PC(victim))
			dam = MAXV(0, dam * 0.60);	/* 40 % for melee */
		else
			dam = MAXV(0, dam * 0.50);	/* 50 % for spells */
	}

	if (IS_AFFECTED(victim, AFF_DEMON_FLESH)) {
		if (attacktype >= TYPE_HIT && attacktype <= TYPE_CRUSH) {
			if (!number(0, 9)) {
				ft2750_create_demon_flesh(victim);
				dam = 1;
			}
		}
	}

	dam = MAXV(0, dam);
	GET_HIT(victim) -= dam;


	if (ch != victim && GET_LEVEL(ch) < IMO_SPIRIT) {
		lv_exp_gain = gv_rush_hour * GET_LEVEL(victim) * dam / 3;
		if (ha1375_affected_by_spell(victim, SPELL_HOLD_PERSON)) {
			lv_exp_gain = 1 * gv_rush_hour;
		}
		if (ch->in_room > 0)
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA))
				lv_exp_gain = 0;
		li2200_gain_exp(ch, lv_exp_gain);
	}

	/* UPDATE VICTIM'S POSITION */
	ft1200_update_pos(victim);

	if ((GET_HIT(victim) > 0) && (attacktype >= TYPE_HIT) && (attacktype <= TYPE_CRUSH)) {
		if (hits > 1) {
			if (lv_display)
				mult_ft2400_damage_message(dam, ch, victim, attacktype, hits);
		}
		else {
			if (lv_display)
				ft2400_damage_message(dam, ch, victim, attacktype);
		}
	}
	else {
		for (i = 0; i < MAX_MESSAGES; i++) {
			if (fight_messages[i].a_type == attacktype && attacktype != TYPE_SHIELDSHOCK) {
				nr = dice(1, fight_messages[i].number_of_attacks);
				messages = fight_messages[i].msg;
				for (j = 1; (j < nr) && (messages); j++)
					messages = messages->next;


				ft_get_damage_string(dam, buf3);

				if (dam != 0 && (attacktype)) {
					if (GET_POS(victim) == POSITION_DEAD) {
						sprintf(buf4, "%s %s", messages->die_msg.attacker_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR, CLR_HIT, AA_FIGHT);
						sprintf(buf4, "%s %s", messages->die_msg.victim_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT, CLR_DAM, AA_FIGHT);
						sprintf(buf4, "%s %s", messages->die_msg.room_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT, CLR_FIGHT, AA_FIGHT);
					}
					else {
						sprintf(buf4, "%s %s", messages->hit_msg.attacker_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR, CLR_HIT, AA_FIGHT);
						sprintf(buf4, "%s %s", messages->hit_msg.victim_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT, CLR_DAM, AA_FIGHT);
						sprintf(buf4, "%s %s", messages->hit_msg.room_msg, buf3);
						ansi_act(buf4, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT, CLR_FIGHT, AA_FIGHT);
					}
				}
				else {	/* Dam == 0 */
					ansi_act(messages->miss_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR, CLR_FIGHT, AA_FIGHT);
					ansi_act(messages->miss_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT, CLR_FIGHT, AA_FIGHT);
					ansi_act(messages->miss_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT, CLR_FIGHT, AA_FIGHT);
				}
			}
		}
	}

	switch (GET_POS(victim)) {
	case POSITION_MORTALLYW:
		act("$n is mortally wounded, and will die soon, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
		act("You are mortally wounded, and will die soon, if not aided.", FALSE, victim, 0, 0, TO_CHAR);
		break;
	case POSITION_INCAP:
		act("$n is incapacitated and will slowly die, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
		act("You are incapacitated an will slowly die, if not aided.", FALSE, victim, 0, 0, TO_CHAR);
		break;
	case POSITION_STUNNED:
		act("$n is stunned, but will probably regain consciousness again.", TRUE, victim, 0, 0, TO_ROOM);
		act("You're stunned, but will probably regain consciousness again.", FALSE, victim, 0, 0, TO_CHAR);
		break;
	case POSITION_DEAD:
		act("$n is dead! R.I.P.", TRUE, victim, 0, 0, TO_ROOM);
		send_to_char("You are dead!  Sorry...\n\r", victim);
		break;

	default:		/* >= POSITION SLEEPING */

		max_hit = li1500_hit_limit(victim);
		if (dam > (max_hit / 5))
			ansi_act("That Really did &RHURT!&n", FALSE, victim, 0, 0, TO_CHAR, CLR_DAM, 0);

		if (GET_HIT(victim) < (max_hit / 5) &&
		    GET_HIT(victim) < 200) {
			ansi_act("You wish that your wounds would stop &RB&rLEEDIN&RG&n that much!", FALSE, victim, 0, 0, TO_CHAR, CLR_DAM, 0);
			if (IS_NPC(victim)) {
				if (IS_SET(victim->specials.act, PLR1_WIMPY) &&
				    ch != victim) {
					at1400_do_flee(victim, "", 0);
				}
			}
			else {
				if (IS_SET(GET_ACT3(victim), PLR3_WIMPY)) {
					at1400_do_flee(victim, "", 0);
				}
			}
		}
		break;
	}

	if (!IS_NPC(victim) && !(victim->desc)) {
		at1400_do_flee(victim, "", 0);
		if (!victim->specials.fighting) {
			act("$n is rescued by divine forces.", FALSE, victim, 0, 0, TO_ROOM);
			victim->specials.was_in_room = victim->in_room;
			ha1500_char_from_room(victim);
			ha1600_char_to_room(victim, 0);
		}
	}

	if (GET_POS(victim) == POSITION_DEAD) {
		if ((GET_LEVEL(victim) == PK_LEV && IS_PC(victim)) &&
		    (GET_LEVEL(ch) == PK_LEV && IS_PC(ch))) {

			if (CHANCE100(10) &&
			    !IS_SET(GET_ROOM2(victim->in_room), RM2_ARENA) &&
			    CLAN_NUMBER(ch) != CLAN_NUMBER(victim)) {
				sprintf(buf, "You absorb %d experience points from %s!", GET_EXP(victim) / 2,
					GET_REAL_NAME(victim));
				send_to_char(buf, ch);
				sprintf(buf, "%s absorbs %d experience points from you!", GET_REAL_NAME(victim),
					GET_EXP(victim) / 2);
				send_to_char(buf, ch);
				GET_EXP(ch) = GET_EXP(victim) / 2;
				GET_EXP(victim) = GET_EXP(victim) / 2;
			}
		}
		if (IS_NPC(victim)) {
			if (IS_AFFECTED(ch, AFF_GROUP)) {
				ft2100_group_gain(ch, victim);
			}
			else {
				exp = GET_EXP(victim) / 3;
				exp += (exp * MINV(8, (GET_LEVEL(victim) - GET_LEVEL(ch)))) >> 3;
				if (IS_SET(GET_ACT1(victim), PLR1_CHAR_WAS_HELD)) {
					exp = lv_exp_gain / 10;
				}
				exp = MAXV(exp, 1) * gv_rush_hour;
				if (victim->in_room > 0)
					if (IS_SET(GET_ROOM2(victim->in_room), RM2_ARENA))
						exp = 0;
				sprintf(buf, "You receive %d experience points.", exp);
				act(buf, FALSE, ch, 0, 0, TO_CHAR);

				li2200_gain_exp(ch, exp);
				ft1600_change_alignment(ch, victim);
			}
			if (IS_PC(ch)) {
				if (IS_NPC(victim)) {
					GET_KILLS(ch)++;
					if (IS_CASTED_ON(ch, SPELL_BIND_SOULS)) {
						ch->extra_mana += MAXV(0, (20 + GET_LEVEL(victim) - GET_LEVEL(ch)));
						send_to_char("Another soul is bound to yours.\r\n", ch);
					}
					GET_SCORE(ch) += GET_LEVEL(victim) < GET_LEVEL(ch) ?
						MAXV(GET_LEVEL(victim) - GET_LEVEL(ch) + 10, -10) :
						(GET_LEVEL(victim) - GET_LEVEL(ch)) * 5 + 10;
					if ((ch->nr >= 0) && (ch->nr <= top_of_p_table))
						player_table[ch->nr].pidx_score = GET_SCORE(ch);
					in5300_update_top_list(ch, top_kills);
				}


				/* Check if the mob was a quest
				 * 
				 * targe t */
				if (IS_NPC(victim) && ch->questmob && ch->questmob == victim) {
					send_to_char("&gYou have completed your quest!&n\r\n", ch);
					send_to_char("&gReturn to the questmaster quickly to get your reward.&n\r\n", ch);
					ch->questmob = 0;
				}
				if (IS_NPC(victim) && victim->questmob && victim->questmob != ch) {
					sprintf(buf2, "%s I'm afraid someone else got to %s before you...",
					      GET_REAL_NAME(victim->questmob),
						GET_REAL_NAME(victim));
					do_tell(questman, buf2, CMD_TELL);
					victim->questmob->questmob = 0;
					victim->questmob->questobj = 0;
					victim->questmob->countdown = 0;
					victim->questmob->nextquest = time(0);
				}


			}	/* end of IS_PC() */

			for (temp_char = character_list; temp_char; temp_char = temp_char->next) {
				if (temp_char->desc)
					if (temp_char->desc->last_target == ch)
						temp_char->desc->last_target = 0;
			}

			/* IF PROCEDURE CALL IT WHEN MOB DIES */
			if (mob_index[victim->nr].func) {
				(*mob_index[victim->nr].func) (ch, -1, "");
			}
		}
		if (!(IS_NPC(victim))) {

			if (cl1900_at_war(ch, victim))
				cl1600_kill_player(ch, victim);

			sprintf(buf, "%s L[%d] killed by %s L[%d] at %s\r\n",
				GET_REAL_NAME(victim),
				GET_LEVEL(victim),
				GET_REAL_NAME(ch),
				GET_LEVEL(ch),
				world[victim->in_room].name);

			if (GET_LEVEL(ch) == PK_LEV && GET_LEVEL(victim) == PK_LEV) {
				sprintf(buf, "&KDeath is in the air&w...&n           &Y\\&wO&Y/ &W%s\n\r"
					"                                        &R|\n\r"
					"                             &ckilled by &y/ \\\n\r"
					"      &y_\\&B_&Y|&wo &K%s\n\r"

					"&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&x-&n",
				    GET_REAL_NAME(ch), GET_REAL_NAME(victim));
			}
			do_info(buf, 1, IMO_IMM, victim);

			bzero(buf, sizeof(buf));
			sprintf(buf, "You were killed by %s.\r\n",
			(IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
			send_to_char(buf, victim);

			j = GET_DEATHS(victim);
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s L[%d] killed by %s L[%d] in room %d Deaths: %d",
				GET_REAL_NAME(victim),
				GET_LEVEL(victim),
				GET_REAL_NAME(ch),
				GET_LEVEL(ch),
				world[ch->in_room].number,
				j);

			if (IS_PC(ch)) {
				spec_log(buf, PKILL_LOG);
			}
			else {
				spec_log(buf, DEATH_LOG);
			}
			strcat(buf, "\r\n");
			do_info(buf, IMO_IMM, MAX_LEV, victim);

		}

		if (IS_PC(victim))
			lv_was_victim_a_pc = TRUE;
		else
			lv_was_victim_a_pc = FALSE;

		ft2000_die(victim, ch);
		victim = 0;

		if (lv_was_victim_a_pc == FALSE) {
			if (!IS_NPC(ch) && IS_SET(GET_ACT3(ch), PLR3_AUTOLOOT)) {
				do_get(ch, " all corpse", CMD_GET);
			}
			else if (!IS_NPC(ch) && IS_SET(GET_ACT3(ch), PLR3_AUTOGOLD)) {
				do_get(ch, " coins corpse", CMD_GET);
			}
		}
	}			/* end of Position is Dead */
	if (victim) {
		if (GET_POS(victim) < POSITION_STUNNED)
			if (ch->specials.fighting == victim) {
				if ((IS_SET(GET_ACT3(ch), PLR3_AUTOAGGR))) {
					send_to_char("Autoaggression applies and you keep fighting.\r\n", ch);
				}
				else {
					ft1400_stop_fighting(ch, 0);
				}
			}
		if (!AWAKE(victim)) {
			if (victim->specials.fighting)
				ft1400_stop_fighting(victim, 0);
		}
	}
	return;

}				/* END OF damage() */
/* MAGIC and ACID attacks added 11-28-02 */


int ft2600_check_for_special_weapon_attacks(struct char_data * ch, struct char_data * victim, struct obj_data * wielded)
{

	char buf[MAX_STRING_LENGTH];
	int rc, idx, element, attack_array[6] = {0, 0, 0, 0, 0, 0};



	rc = 0;
	element = -1;
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_FLAME_ATTACK)) {
		element++;
		attack_array[element] = 1;
	}
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_ICE_ATTACK)) {
		element++;
		attack_array[element] = 2;
	}
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_ELEC_ATTACK)) {
		element++;
		attack_array[element] = 3;
	}
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_MAGIC_ATTACK)) {
		element++;
		attack_array[element] = 4;
	}
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_ACID_ATTACK)) {
		element++;
		attack_array[element] = 5;
	}
	if (IS_SET(wielded->obj_flags.value[0], WEAPON_DARKMAGIC_ATTACK)) {
		element++;
		attack_array[element] = 6;
	}
	if (element < 0) {
		return (0);
	}
	idx = number(0, element);
	rc = 0;
	switch (attack_array[idx]) {
	case 1:{
			bzero(buf, sizeof(buf));
			act("&RF&rl&Ya&rm&Res &wshoot from your $p &Re&rngulfin&Rg $N&r.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&RF&rl&Ya&rm&Res &wshoot from $n's $p &Re&rngulfin&Rg $N&r.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&RF&rl&Ya&rm&Res &wshoot from $n's $p &Re&rngulfin&Rg &wYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(2, 5)) / 2);
			break;
		}
	case 2:{

			bzero(buf, sizeof(buf));
			act("&CI&Wc&Ce &Ws&whard&Ws &wfly from your $p &We&wngulfin&Wg $N&w.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&CI&Wc&Ce &Ws&whard&Ws &wfly from $n's $p &We&wngulfin&Wg $N&w.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&CI&Wc&Ce &Ws&whard&Ws &wfly from $n's $p &We&wngulfin&Wg &wYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(2, 3)) / 2);
			break;
		}
	case 3:{
			bzero(buf, sizeof(buf));
			act("&wA &Ylightning bolt&n &Wl&Ye&Wa&Yp&Ws &wfrom your $p &Yengulfin&Yg $N&w.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&wA &Ylightning bolt&n &Wl&Ye&Wa&Yp&Ws &wfrom $n's $p &Yengulfing $N&w.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&wA &Ylightning bolt&n &Wl&Ye&Wa&Yp&Ws &wfrom $n's $p &Yengulfing &wYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(1, 2)) / 2);
			break;
		}
	case 4:{
			bzero(buf, sizeof(buf));
			act("&WEldritch energy &cleaps from your $p &cand engulfs $N.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&WEldritch energy &cleaps from $n's $p &cand engulfs $N.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&WEldritch energy &cleaps from $n's $p &cand engulfs &rYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(2, 6)) / 2);
			break;
		}
	case 5:{
			bzero(buf, sizeof(buf));
			act("&wA stream of &Kacid &wshoots from your $p, &rsplattering $N.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&wA stream of &Kacid &wshoots from $n's $p &rsplatters $N.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&KAcid &wsprays from $n's $p, &rsplattering &KYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(2, 4)) / 2);
			break;
		}
	case 6:{
			bzero(buf, sizeof(buf));
			act("&KDark eldritch energy &cleaps from your $p &cand engulfs $N.&n",
			    0, ch, wielded, victim, TO_CHAR);
			act("&KDark eldritch energy &cleaps from $n's $p &cand engulfs $N.&n",
			    1, ch, wielded, victim, TO_NOTVICT);
			act("&Dark eldritch energy &cleaps from $n's $p &cand engulfs &rYOU.&n",
			    0, ch, wielded, victim, TO_VICT);
			rc = ((GET_LEVEL(ch) * number(2, 6)) / 2);
			break;
		}
	}			/* END OF switch() */

	return (rc);


}				/* END OF ft2600_check_for_special_weapon_attacks() */




void ft2750_create_demon_flesh(struct char_data * victim)
{

	int lv_obj_num;
	struct obj_data *lv_obj;



	lv_obj_num = db8200_real_object(OBJ_DEMON_FLESH);
	if (lv_obj_num > 1 &&
	    obj_index[lv_obj_num].number < obj_index[lv_obj_num].maximum) {
		lv_obj = db5100_read_object(lv_obj_num, REAL);
		ha2100_obj_to_room(lv_obj, victim->in_room);
	}
	act("The attack sends flesh flying from $n.",
	    TRUE, victim, 0, 0, TO_ROOM);
	act("The attack is absorbed into your body, leaving you unharmed.",
	    TRUE, victim, 0, 0, TO_CHAR);
	return;

}				/* END OF ft2750_create_demon_flesh() */
#define OBJ_HOLY_SMITER 16934
#define OBJ_FYREBRAND 7763
#define OBJ_WHISPERWIND 21235
#define OBJ_ARAGORN 23810
/*for x-mas quest*/
#define OBJ_SANTA_WHIP 24480


int ft2800_spec_damage(struct char_data * ch, struct char_data * victim, int dam, struct obj_data * wielded)
{
	int lv_slaying = 1;



	if ((wielded) &&
	    (wielded->obj_flags.type_flag == ITEM_WEAPON ||
	     wielded->obj_flags.type_flag == ITEM_QSTWEAPON)) {
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_POISONOUS) && (number(0, 5) == 0)) {
			cast_poison(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
		}
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_BLIND) && (number(0, 5) == 0)) {
			cast_blindness(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
		}
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_VAMPIRIC) && (number(0, 5) == 0)) {
			GET_HIT(ch) = MINV((GET_LEVEL(ch) < IMO_SPIRIT) ? 50000 : 100000000, GET_HIT(ch) + (dam >> 3));
			act("&wYou feel &Ksto&wlen &Wlife &wene&Krgy &wrush through your body!&n", FALSE, ch, 0, 0, TO_CHAR);
		}
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_DRAIN_MANA) && (number(0, 3) == 0)) {
			cast_energy_drain(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
		}
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_SILENCE) && (number(0, 10) == 0)) {
			cast_silence(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
		}

		/* WEAPON GOOD/NEUTRAL/EVIL FLAGS damage on these was adjusted
		 * 11-28-02 */
		if ((IS_SET(wielded->obj_flags.value[0], WEAPON_GOOD))) {
			if (IS_GOOD(victim)) {
				dam = (int) (dam * 0.5);
			}
			if (IS_GOOD(ch)) {
				dam = (int) (dam * 1.1);
			}
		}
		if ((IS_SET(wielded->obj_flags.value[0], WEAPON_NEUTRAL))) {
			if (IS_NEUTRAL(victim)) {
				dam = (int) (dam * 0.5);
			}
			if (IS_NEUTRAL(ch)) {
				dam = (int) (dam * 1.1);
			}
		}
		if ((IS_SET(wielded->obj_flags.value[0], WEAPON_EVIL))) {
			if (IS_EVIL(victim)) {
				dam = (int) (dam * 0.5);
			}
			if (IS_EVIL(ch)) {
				dam = (int) (dam * 1.1);
			}
		}

		if (IS_SET(wielded->obj_flags.value[0], WEAPON_GOOD_SLAYER) && IS_GOOD(victim))
			lv_slaying = lv_slaying * 2;
		if (IS_SET(wielded->obj_flags.value[0], WEAPON_EVIL_SLAYER) && IS_EVIL(victim))
			lv_slaying = lv_slaying * 2;


		/* Add the damage and do other stuff for pulsing weapons --ff
		 * 1/20/07 */
		dam += pc3000_pulse_attack(ch, victim);

		if (IS_NPC(victim)) {
			if (IS_SET(wielded->obj_flags.value[0], WEAPON_DRAGON_SLAYER)
			    && IS_SET(victim->specials.act, PLR1_CLASS_DRAGON))
				lv_slaying = lv_slaying * 2;
			if (IS_SET(wielded->obj_flags.value[0], WEAPON_UNDEAD_SLAYER)
			    && IS_SET(victim->specials.act, PLR1_CLASS_UNDEAD))
				lv_slaying = lv_slaying * 2;
			if (obj_index[ch->equipment[WIELD]->item_number].virtual == OBJ_HOLY_SMITER &&
			    IS_SET(victim->specials.act, PLR1_CLASS_UNDEAD) && number(0, 25) == 1) {
				lv_slaying = lv_slaying * number(2, 4);
				act("&wA strange &Wwhite light&w shines forth from $p &wand engulfs $N&w.&n", TRUE, ch, wielded, victim, TO_CHAR);
				act("&wA strange &Wwhite light &wshines forth from $n's $p &wand engulfs $N&w.&n", TRUE, ch, wielded, victim, TO_NOTVICT);
			}	/* This should work now. 11-28-02 */


			if (IS_SET(wielded->obj_flags.value[0], WEAPON_DEMON_SLAYER)	/* added by Nightwynde 		 * 11-28-02 */
			    &&IS_SET(victim->specials.act, PLR1_CLASS_DEMON))
				lv_slaying = lv_slaying * 2;
			if (IS_SET(wielded->obj_flags.value[0], WEAPON_ANIMAL_SLAYER)
			    && IS_SET(victim->specials.act, PLR1_CLASS_ANIMAL))
				lv_slaying = lv_slaying * 2;
			if (IS_SET(wielded->obj_flags.value[0], WEAPON_GIANT_SLAYER)
			    && IS_SET(victim->specials.act, PLR1_CLASS_GIANT))
				lv_slaying = lv_slaying * 2;
			if (!IS_SET(wielded->obj_flags.value[0], WEAPON_SILVER)
			    && IS_SET(victim->specials.act, PLR1_CLASS_SHADOW))
				dam = 0;
			if (!IS_SET(wielded->obj_flags.value[0], WEAPON_WOOD_STAKE)
			  && IS_SET(victim->specials.act, PLR1_CLASS_VAMPIRE))
				dam = 0;
		}
		dam = dam * lv_slaying;
	}
	else if (IS_NPC(victim) &&
		 (IS_SET(victim->specials.act, PLR1_CLASS_VAMPIRE)
		  || IS_SET(victim->specials.act, PLR1_CLASS_SHADOW)))
		dam = 0;

	if (IS_NPC(ch)) {
		if (IS_SET(ch->specials.act, PLR1_POISONOUS) && number(0, 2) == 0)
			cast_poison(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);

		if (IS_SET(ch->specials.act, PLR1_DRAIN_XP) && number(0, 2) == 0)
			cast_energy_drain(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, victim, 0);
	}

	return (dam);
}				/* END OF ft2800_spec_damage() */
#define OBJ_WIZARD_STAFF	23821
#define OBJ_PENTAGRAM		15524

/* ft3000_hit */
void hit(struct char_data * ch, struct char_data * victim, int type)
{

	struct obj_data *wielded;
	struct obj_data *held;
	int lv_temp_dam, w_type, lv_part1, victim_ac, calc_thaco, dam, num_attacks,
	  lv_temp_num, i;
	sbyte diceroll;
	char buf[MAX_STRING_LENGTH];



	HUNTING(ch) = 0;

	if (!victim)
		return;

	if (ch->in_room != victim->in_room) {
		sprintf(buf, "ERROR: %s r[%d] is not in the same room as opponent %s r[%d]!",
			GET_REAL_NAME(ch),
			ch->in_room,
			GET_REAL_NAME(victim),
			victim->in_room);
		main_log(buf);
		return;
	}
	calc_thaco = 0;

	if (ch->equipment[HOLD])
		held = ch->equipment[HOLD];
	else
		held = 0;
	
	if (type == TYPE_DUALWIELD && ch->equipment[WIELD]) {
		wielded = held;
	}
	else {
		if (ch->equipment[WIELD] &&
		  (ch->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON ||
		 ch->equipment[WIELD]->obj_flags.type_flag == ITEM_QSTWEAPON))
			wielded = ch->equipment[WIELD];
		else
			wielded = 0;
	}

	/* DEFAULT TO A BASIC HIT */
	w_type = TYPE_HIT;

	/* UNARMED DARKLINGS CLAW */
	if (!wielded &&
	    (GET_RACE(ch) == RACE_DARKLING ||
	     GET_RACE(ch) == RACE_RAVSHI ||
	     GET_RACE(ch) == RACE_PIXIE ||
	     GET_RACE(ch) == RACE_FELINE)) {
		w_type = TYPE_CLAW;
	}

	if (IS_NPC(ch) && (ch->specials.attack_type > TYPE_HIT))
		w_type = ch->specials.attack_type;

	else {
		if (wielded) {
			switch (wielded->obj_flags.value[3]) {
			case 0:
			case 1:
			case 2:{
					w_type = TYPE_WHIP;
					li9900_gain_proficiency(ch, SKILL_SLASH);
					calc_thaco = ch->skills[SKILL_SLASH].learned / 10;
					break;
				}
			case 3:{
					w_type = TYPE_SLASH;
					li9900_gain_proficiency(ch, SKILL_SLASH);
					calc_thaco = ch->skills[SKILL_SLASH].learned / 10;
					break;
				}
			case 4:
			case 5:
			case 6:{
					w_type = TYPE_CRUSH;
					li9900_gain_proficiency(ch, SKILL_BLUDGEON);
					calc_thaco = ch->skills[SKILL_BLUDGEON].learned / 10;
					break;
				}
			case 7:{
					w_type = TYPE_BLUDGEON;
					li9900_gain_proficiency(ch, SKILL_BLUDGEON);
					calc_thaco = ch->skills[SKILL_BLUDGEON].learned / 10;
					break;
				}
			case 8:
			case 9:
			case 10:
			case 11:{
					w_type = TYPE_PIERCE;
					li9900_gain_proficiency(ch, SKILL_PIERCE);
					calc_thaco = ch->skills[SKILL_PIERCE].learned / 10;
					break;
				}
			default:{
					w_type = TYPE_HIT;
					break;
				}	/* END OF default */
			}	/* END OF switch */
		}		/* END OF wielded */
	}			/* END OF else not a PC and type > TYPE_HIT */

	/* Calculate the raw armor including magic armor */
	/* The lower AC, the better                      */

	calc_thaco = li9675_thaco(ch) - calc_thaco;
	calc_thaco -= li9725_adjust_tohit_bonus(GET_STR(ch) + GET_BONUS_STR(ch));
	calc_thaco -= GET_HITROLL(ch);

	/* MOBS GET AN EXTRA +2 to hit */
	if (IS_NPC(ch))
		calc_thaco -= 2;

	victim_ac = GET_AC(victim) + li9750_ac_bonus(GET_DEX(victim) + GET_BONUS_DEX(victim)) +
		races[GET_RACE(victim)].adj_ac;

	/* OLD DODGE
	if (number(0, 101) < victim->skills[SKILL_DODGE].learned ||
	    number(0, 101) < victim->skills[SKILL_DODGE].learned) {
		li9900_gain_proficiency(victim, SKILL_DODGE);
		victim_ac -= victim->skills[SKILL_DODGE].learned;
	}
	 */
	victim_ac = victim_ac / 10;

	if (IS_AFFECTED(victim, AFF_HOLD_PERSON)) {
		victim_ac += 50;
	}

	diceroll = number(1, 30);
	if (AWAKE(victim))
		victim_ac += li9650_dex_adjustment(victim, 4);

	if ((diceroll < 30) && AWAKE(victim) &&
	    ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac))) {	/* miss */
		if (type == SKILL_BACKSTAB)
			damage(ch, victim, 0, SKILL_BACKSTAB, 0);
		else
			damage(ch, victim, 0, w_type, 0);
	}
	else {
		diceroll = number(1, 30);
		num_attacks = 1;
		dam = 0;

		if (number(1, 200) == 1) {
			num_attacks += 1;
			if IS_PC(ch)
			send_to_char("Good luck and fortune shine upon you! You get an extra attack in!\r\n", ch);
		}

		if ((number(0, 101) < ch->skills[SKILL_SECOND].learned) &&
		    (!((diceroll < 30) && AWAKE(victim) && ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac))))) {	/* hit */
			li9900_gain_proficiency(ch, SKILL_SECOND);
			num_attacks += 1;
			diceroll = number(1, 30);
			if ((number(0, 101) < ch->skills[SKILL_THIRD].learned) &&
			    (!((diceroll < 30) && AWAKE(victim) && ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac))))) {	/* hit */
				li9900_gain_proficiency(ch, SKILL_THIRD);
				num_attacks += 1;
				diceroll = number(1, 30);
				if ((number(0, 101) < ch->skills[SKILL_FOURTH].learned) &&
				    (!((diceroll < 30) && AWAKE(victim) && ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac))))) {	/* hit */
					li9900_gain_proficiency(ch, SKILL_FOURTH);
					num_attacks += 1;
					diceroll = number(1, 30);
					if ((number(0, 101) < ch->skills[SKILL_FIFTH].learned) &&
						(!((diceroll < 30) && AWAKE(victim) && ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac))))) {	/* hit */
						li9900_gain_proficiency(ch, SKILL_FIFTH);
						num_attacks += 1;
					}
				}
			}
		}
		if (IS_PC(ch)){
			num_attacks += ch->points.extra_hits;
		}
		
		/* Flurry Attack*/
		if (number(0, 101) < MINV(33, (ch->skills[SKILL_FLURRY].learned / 3))) 
		{
			li9900_gain_proficiency(ch, SKILL_FLURRY);
			num_attacks += dice(1,num_attacks);
			if IS_PC(ch)
				send_to_char("&WYou bombard your opponent with a flurry of attacks!&n\r\n", ch);
		}/* end of Flurry Attack*/
		
		/* GIVE DARKLINGS A POISON ATTACK IF USING CLAW */
		if ((GET_RACE(ch) == RACE_DARKLING ||
		     GET_RACE(ch) == RACE_RAVSHI ||
		     GET_RACE(ch) == RACE_PIXIE ||
		     GET_RACE(ch) == RACE_FELINE) &&
		    !wielded &&
		    !number(0, 3)) {
			spell_poison(GET_LEVEL(ch), ch, victim, 0);
		} /* End of poison bare hand attack */
        
		/* New Dodge */
		if (victim->skills[SKILL_DODGE].learned > 1 && (number(0,101) < MINV(25, (victim->skills[SKILL_DODGE].learned / 4)))){
			li9900_gain_proficiency(victim, SKILL_DODGE);
	    	if (num_attacks > 1 ){
			num_attacks >>= 1;  //Dodges half of the attacks if more than one
			}
			else {
			num_attacks -= 1;	//Dodges one attack if not more than one attack
			}
			if (IS_PC(victim)){
			send_to_char("&WYou deftly dodged some of your opponents attacks!&n\r\n", victim); //Display dodge text to player.
			}
		} /* End of new DODGE */

		if ((type == TYPE_DUALWIELD) && (num_attacks > 1))
			num_attacks >>= 1;
		
		for (i = 0; i < num_attacks; i++) {
			lv_temp_dam = 0;

			if (IS_NPC(ch)) {	/* This gets added to the weapon */
				lv_temp_dam += dice(ch->specials.damnodice, ch->specials.damsizedice);
			}
			else {
				if (GET_CLASS(ch) == CLASS_MONK && (!wielded)) {
				lv_temp_dam += dice((GET_LEVEL(ch)/2), 3) + (GET_LEVEL(ch)/2);	/* Monk Unarmed Strike */
				}
				lv_temp_dam+= number(0, 2);	/* Max. 2 dam with bare hands */	
			}

			if (wielded)	/* NPC barehand damage is added to this * -- ouch */
				lv_temp_dam += dice(wielded->obj_flags.value[1], wielded->obj_flags.value[2]);

			lv_temp_dam += li9700_adjust_damage_bonus(GET_STR(ch) + GET_BONUS_STR(ch));

			lv_temp_dam = ft2800_spec_damage(ch, victim, lv_temp_dam, wielded);	/* slaying etc */

			if (type == SKILL_BACKSTAB) {
				lv_part1 = lv_temp_dam + (2 * GET_LEVEL(ch));
				if (number(0, 101) <= 10 && GET_CLASS(ch) == CLASS_THIEF && GET_LEVEL(ch) >= 30) {
					//Critical backstab(2 x damage) Bingo 9 - 19 - 01
						lv_part1 *= 2;
					send_to_char("&y<&YCRITICAL&y>&n ", ch);
				}
				lv_temp_dam = (lv_part1 * backstab_mult[GET_LEVEL(ch)]);
				/* ONLY ONE ATTACK ON BACKSTAB */
				i = num_attacks;

			}	/* END OF BACKSTAB */

			struct obj_data *held;
			if (ch->equipment[HOLD])
				held = ch->equipment[HOLD];
			else
				held = 0;

			if (held) {
				int d;

				if (obj_index[ch->equipment[HOLD]->item_number].virtual == OBJ_WIZARD_STAFF &&
				    IS_EVIL(victim) && number(0, 20) == 1) {


					if (GET_LEVEL(ch) <= 41) {
						d = dice(41, 4) + 41;	/* from the turn undead
									 * formula -- leveld4+41 */
						if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
							d *= 2;
						}
					}
					else if (GET_LEVEL(ch) >= 42) {
						d = dice(42, 4) + 42;	/* from the turn undead
									 * formula -- leveld4+41 */
						if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
							d *= 2;
						}

					}
					lv_temp_dam += d;

					act("&wA &Wwhite light &wshines forth from $p &wand engulfs&n $N&w.&n", TRUE, ch, held, victim, TO_CHAR);
					act("&wA &Wwhite light &wshines forth from $n's $p &wand engulfs YOU&w.&n", TRUE, ch, held, victim, TO_VICT);
					act("&wA &Wwhite light &wshines forth from $n's $p &wand engulfs $N&w.&n", TRUE, ch, held, victim, TO_NOTVICT);

				}

				else if (obj_index[ch->equipment[HOLD]->item_number].virtual == OBJ_PENTAGRAM &&
					 IS_GOOD(victim) && number(0, 20) == 1 && IS_EVIL(ch)) {


					if (GET_LEVEL(ch) <= 41) {
						d = dice(41, 4) + 41;
						if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
							d *= 2;
						}
					}
					else if (GET_LEVEL(ch) >= 42) {
						d = dice(42, 4) + 42;	/* from the turn undead
									 * formula -- leveld4+41 */
						if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
							d *= 2;
						}
					}
					lv_temp_dam += d;

					act("&wThe&n $p &wflies from your hand and &Rb&rurn&Rs&w $N &win the forehead.&n", TRUE, ch, held, victim, TO_CHAR);
					act("&wYour eyes &Rb&rlee&Rd&w as $n's $p &Rb&rurn&Rs&w your forehead with it's &Ke&wv&Ki&wl.&n", TRUE, ch, held, victim, TO_VICT);
					act("&wYour eyes &Rb&rlee&Rd&w as $n's $p &Rb&rurn&Rs&w $N's forehead with it's &Ke&wv&Ki&wl.&n", TRUE, ch, held, victim, TO_NOTVICT);


				}
			}	/* end if (held) */
			if (GET_POS(victim) < POSITION_FIGHTING)
				lv_temp_dam *= 1 + (POSITION_FIGHTING - GET_POS(victim)) / 3;

			lv_temp_dam += GET_DAMROLL(ch);
			
			/* TOUGHER SCALES */
			if (number(0, 101) < victim->skills[SKILL_SCALES].learned ||
			    number(0, 101) < victim->skills[SKILL_SCALES].learned) {
				li9900_gain_proficiency(victim, SKILL_SCALES);
				lv_temp_dam -= (victim->skills[SKILL_SCALES].learned * (GET_DEX(ch) + GET_BONUS_DEX(ch))) / 100;
			}

			if (GET_POS(victim) == POSITION_DEAD)	/* got 'em with specials
								 * - a tough bug to spot
								 * btw */
				return;
			lv_temp_dam = MAXV(1, lv_temp_dam);	/* Not less than 1
								 * damage */
			dam += lv_temp_dam;
		} /* End of Number of Attacks For Loop */
		if (type == TYPE_DUALWIELD) {
			if (ch->equipment[HOLD] && (ch->equipment[HOLD]->obj_flags.type_flag == ITEM_WEAPON || ch->equipment[WIELD]->obj_flags.type_flag == ITEM_QSTWEAPON))
			if (number(1,5) == 3){
				dam += ft2600_check_for_special_weapon_attacks(ch, victim, ch->equipment[HOLD]);
			}
		} /* Off Hand Bits Proc*/
		else if (type == TYPE_UNDEFINED && ch->equipment[WIELD] && (ch->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON || ch->equipment[WIELD]->obj_flags.type_flag == ITEM_QSTWEAPON)) {
			if (number(1, 5) == 3) {
				dam += ft2600_check_for_special_weapon_attacks(ch, victim, ch->equipment[WIELD]);
			}
		} /* Normal Hand Bits Proc */
		
		if (type == SKILL_BACKSTAB)
			damage(ch, victim, dam, SKILL_BACKSTAB, num_attacks);
		else {
			lv_temp_num = dam;
			if (IS_AFFECTED(victim, AFF_SANCTUARY)) {
				lv_temp_num = lv_temp_num / 2;
			}
			if (IS_CASTED_ON(victim, SPELL_BARKSKIN)) {
				lv_temp_num = lv_temp_num * 0.8;
			}
			if (IS_CASTED_ON(victim, SPELL_STONESKIN)) {
				lv_temp_num = lv_temp_num * 0.6;
			}

			/* Critical attack for warriors ,paladins, priests, eldritch knight, and monk
			 * Bingo 9 - 19 - 01 */
			if (number(0, 101) <= 9 && IS_PC(ch) && GET_LEVEL(ch) >= 30) {
				if (GET_CLASS(ch) == CLASS_WARRIOR) { 
					if (GET_MOVE(ch) >= 25) {
						dam *= 2;
						GET_MOVE(ch) -= 25;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
				else if (GET_CLASS(ch) == CLASS_PALADIN) {
					if (GET_MOVE(ch) >= 12) {
						dam *= 1.5;
						GET_MOVE(ch) -= 12;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
				else if (GET_CLASS(ch) == CLASS_PRIEST) {
					if (GET_MOVE(ch) >= 10) {
						dam *= 1.25;
						GET_MOVE(ch) -= 10;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
				else if (GET_CLASS(ch) == CLASS_BARD) {
					if (GET_MOVE(ch) >= 10) {
						dam *= 1.35;
						GET_MOVE(ch) -= 10;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
				else if (GET_CLASS(ch) == CLASS_ELDRITCHKNIGHT) {
					if (GET_MOVE(ch) >= 12) {
						dam *= 1.35;
						GET_MOVE(ch) -= 12;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
				else if (GET_CLASS(ch) == CLASS_MONK) {
					if (GET_MOVE(ch) >= 25) {
						dam *= 2;
						GET_MOVE(ch) -= 25;
						send_to_char("&y<&YCRITICAL&y> &n", ch);
					}
				}
			} /* End of Melee Critical Attacks */
			else
					dam *= 1;
			 /* END of not a Backstab */


			if ((GET_POS(victim) = POSITION_FIGHTING) &&
			    (lv_temp_num > GET_HIT(victim)) &&
			    (!IS_AFFECTED(ch, AFF_HOLD_PERSON)) &&
			    (number(0, 101) < MINV(65, victim->skills[SKILL_PARRY].learned))) {
				li9900_gain_proficiency(victim, SKILL_PARRY);
				GET_MOVE(victim) = MAXV(0, GET_MOVE(victim) - 50);
				act("You desperately parry the blow that would have been your death.",
				    FALSE, victim, 0, 0, TO_CHAR);
				act("$n desperately parries the blow that would have been $s death.",
				    FALSE, victim, 0, 0, TO_ROOM);
			} /* END of Parry Death Blow */
			else
				damage(ch, victim, dam, w_type, num_attacks);
		} /* END of didn't miss */
	}
}	/* END OF hit() */


/* control the fights going on */
void ft3100_perform_violence(void)
{
	struct char_data *ch;
	struct obj_data *dual;

	int lv_save_type, lv_save_second, lv_save_third, lv_save_fourth, lv_we_did_something,
	  idx;



	for (ch = combat_list; ch; ch = combat_next_dude) {

		combat_next_dude = ch->next_fighting;


		assert(ch->specials.fighting);

		if (!AWAKE(ch) || (ch->in_room != ch->specials.fighting->in_room))
			ft1400_stop_fighting(ch, 0);
		else {
			if (IS_PC(ch)) {
				hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
				if ((dual = ch->equipment[HOLD]) && !ch->equipment[WEAR_SHIELD]) {
					if (GET_ITEM_TYPE(dual) == ITEM_WEAPON ||
					    GET_ITEM_TYPE(dual) == ITEM_QSTWEAPON) {
						if (IS_SET(GET_OBJ2(dual), OBJ2_DUALWIELD))
							hit(ch, ch->specials.fighting, TYPE_DUALWIELD);
					}
				}
			}
			else {
				/* IF THIS IS THE PRIES T ENFYR N, WE HAVE A  */
				/* 20% CHANC E OF BECOM ING THE ENFYR N DRAGO N */
				if (mob_index[ch->nr].virtual == MOB_ENFYRN_PRIEST &&
				    !(number(0, 4))) {
					mb1100_enfyrn_priest_changes_to_dragon(ch);
				}
				else {
					lv_we_did_something = FALSE;
					/* A
					 * 
					 * N Y S
					 * 
					 * 
					 * P E
					 * 
					 * C I A L
					 * 
					 * A L
					 * 
					 * 
					 * 
					 * 
					 * T T
					 * 
					 * A C K S ? */
					for (idx = 0; idx < MAX_SPECIAL_ATTACK &&
					     ch->specials.fighting; idx++) {
						if (mob_index[ch->nr].attack_type[idx] > 0) {
							lv_we_did_something = TRUE;
							lv_save_type = ch->specials.attack_type;
							lv_save_second = ch->skills[SKILL_SECOND].learned;
							lv_save_third = ch->skills[SKILL_THIRD].learned;
							lv_save_fourth = ch->skills[SKILL_FOURTH].learned;
							ch->specials.attack_type = mob_index[ch->nr].attack_type[idx];
							ch->skills[SKILL_SECOND].learned = mob_index[ch->nr].attack_skill[idx][0];
							ch->skills[SKILL_THIRD].learned = mob_index[ch->nr].attack_skill[idx][1];
							ch->skills[SKILL_FOURTH].learned = mob_index[ch->nr].attack_skill[idx][2];
							hit(ch, ch->specials.fighting, mob_index[ch->nr].attack_type[idx]);
							ch->specials.attack_type = lv_save_type;
							ch->skills[SKILL_SECOND].learned = lv_save_second;
							ch->skills[SKILL_THIRD].learned = lv_save_third;
							ch->skills[SKILL_FOURTH].learned = lv_save_fourth;
						}	/* END OF attack_type >
							 * 0 */
					}	/* END OF for loop */
					if (lv_we_did_something == FALSE) {
						hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
					}	/* END OF we didn't do
						 * something */
				}	/* END OF else not ENFYRN */
			}	/* END OF else IS_NPC */
		}		/* end of else */
	}			/* END OF combat list */
}				/* END OF ft3100_perform_violence */

int death_taunt(struct char_data * ch, struct char_data * killer)
{

	char buf[MAX_INPUT_LENGTH];

	if (IS_NPC(killer)) {
		if (CHANCE100(50)) {
			if (IS_EVIL(killer)) {
				switch (number(0, 10)) {
				case 0:
					sprintf(buf, "Oops, thats what what happens when you beat %s's head in!",
						GET_REAL_NAME(ch));
					do_gossip(killer, buf, CMD_AVATAR);
					break;
				case 1:
					sprintf(buf, "Oh man I just stepped in a pile of %s!  Damn, somebody get me a mop!",
						GET_REAL_NAME(ch));
					do_gossip(killer, buf, CMD_AVATAR);
					break;
				case 2:
					sprintf(buf, "Come and get it!  Fresh meat straight from the corpse of %s!",
						GET_REAL_NAME(ch));
					do_gossip(killer, buf, CMD_AVATAR);
					break;
				case 3:
					sprintf(buf, "*yawn* Wow %s was barely a challenge, I almost fell asleep.",
						GET_REAL_NAME(ch));
					do_gossip(killer, buf, CMD_AVATAR);
					break;
				case 4:
					sprintf(buf, "*puke* Hey someone come get %s's corpse, man it stinks!",
						GET_REAL_NAME(ch));
					do_gossip(killer, buf, CMD_AVATAR);
					break;
				}
			}
			if (IS_GOOD(killer)) {
				switch (number(0, 10)) {
				}
			}
			if (GET_ALIGNMENT(killer) < 350 && GET_ALIGNMENT(killer) > -350) {
				switch (number(0, 10)) {
				}
			}
		}
	}
	return (FALSE);
}