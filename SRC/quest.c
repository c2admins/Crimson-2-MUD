/* qu */
/* *******************************************************************
*  file: quest.c , Special module.           Part of Crimson MUD     *
*  Usage: Quest system for Crimson MUD II                            *
*                  Written by Hercules                               *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

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


/* Vnums of quest objects */
#define QUEST_OBJ1 900
#define QUEST_OBJ2 901
#define QUEST_OBJ3 902
#define QUEST_OBJ4 903
#define QUEST_OBJ5 904

struct qm {

	int form;		/* 0 or 1 */
	int chance;		/* [0, 100] */
	int override;
};




void qu0000_questsales(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	int lv_choice;

	const char *lv_mail_cmds[] = {
		"enable",	/* 1 */
		"disable",	/* 2 */
		"status",	/* 3 */
		"override",	/* 4 */
		"\n",
	};

#define QSALES_ON		1
#define QSALES_OFF		2
#define QSALES_DISPLAY  3
#define QSALES_OVER		4


	/* Strip first argument */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);


	for (; isspace(*arg); arg++);
	bzero(buf2, sizeof(buf2));
	arg = one_argument(arg, buf2);

	lv_choice = 0;
	if (*buf) {

		lv_choice = old_search_block(buf, 0, strlen(buf), lv_mail_cmds, 0);
	}
	if (lv_choice < 1) {
		send_to_char("usage qmsales {enable,disable,status,override} {1..100}\r\n", ch);
		return;
	}

	if (QSALES_ON == lv_choice) {

		int ret_var = qu0200_questWriteVar(1, atoi(buf2), isQuestOverrideOn);


		if (ret_var == 1) {
			send_to_char("Questmaster has yielded to your request.\r\n", ch);
			main_log("Questmaster transformation sales bit has been set");
			sprintf(buf, "QMSALES: %s has enabled Questmaster transformation sales at %d", GET_NAME(ch), atoi(buf2));
			spec_log(buf, GOD_COMMAND_LOG);
		}
		else {
			send_to_char("usage qmsales {enable,disable,status,override} {1..100}\r\n", ch);
		}

		return;


	}


	if (QSALES_OFF == lv_choice) {

		int ret_var = qu0200_questWriteVar(0, transformSalesChance, isQuestOverrideOn);

		if (ret_var == 1) {
			send_to_char("The Questmaster has yielded to your request", ch);
			main_log("Questmaster transformation sales have ceased");
			sprintf(buf, "QMSALES: %s has disabled Questmaster transformation sales.", GET_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
		}
		else {
			send_to_char("Questmaster has an error in setting the transformation sales bit", ch);
			main_log("Questmaster has an error in setting the transformation sales bit");
		}
		return;
	}

	else if (QSALES_DISPLAY == lv_choice) {

		char buffer[MAX_STRING_LENGTH];
		if (isQuestTransformOn == 1) {
			sprintf(buffer, "Transformation sales &Wenabled&n with a &W%d&n percent chance of activation.\r\n", transformSalesChance);
		}
		else {
			sprintf(buffer, "Transformation sales &Wdisabled&n with a &W%d&n percent chance of activation.\r\n", transformSalesChance);
		}
		send_to_char(buffer, ch);

		if (isQuestOverrideOn) {
			send_to_char("Override is &Won&n\r\n", ch);
		}
		else {
			send_to_char("Override is &Woff&n\r\n", ch);
		}
	}

	else if (QSALES_OVER == lv_choice) {

		int ret_var;

		if (!isQuestOverrideOn) {
			isQuestOverrideOn = TRUE;
			ret_var = qu0200_questWriteVar(isQuestTransformOn, transformSalesChance, isQuestOverrideOn);
			sprintf(buf, "QMSALES: %s has enabled overrided Questmaster transformation sales", GET_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
		} //end if
			else {
			isQuestOverrideOn
				= FALSE;
			ret_var = qu0200_questWriteVar(isQuestTransformOn, transformSalesChance, isQuestOverrideOn);
			sprintf(buf, "QMSALES: %s has disabled overrided Questmaster transformation sales", GET_NAME(ch));
			spec_log(buf, GOD_COMMAND_LOG);
		}


		if (ret_var) {
			send_to_char("The Questmaster has yielded to your request", ch);
			if (isQuestOverrideOn) {
				main_log("Questmaster override is activated");
			}
			else {
				main_log
					("Questmaster override is deactivated");
			}
		}

	}
}				/* end qu0000_questsales */
 /* qu0100_questFileOpen // opens files in / lib / quest // it expects quest to
  * hold an 0 or 1 in ascii format. // it will also look for a newline. */
void qu0100_questFileOpen()
{

	FILE *input;
	FILE *out;
	struct qm r;
	struct qm *ptr_p = &r;



	input = fopen("quest", "rb");

	if (input == NULL) {
		main_log("Error: cannot open quest file Creating new file");
		isQuestTransformOn = FALSE;
		transformSalesChance = 10;
		//default level
			isQuestOverrideOn = FALSE;

		ptr_p->form = isQuestTransformOn;
		ptr_p->chance = transformSalesChance;
		ptr_p->override = isQuestOverrideOn;
		out = fopen("quest", "wb");

		if (out == NULL) {
			//boo boo
		}
		else {
			fwrite(ptr_p, sizeof(struct qm), 1, out);
			main_log("Questmaster variables set");
			fclose(out);

		}


	} //end if

		else {

		fread
			(ptr_p, sizeof(struct qm), 1, input);
		isQuestTransformOn = ptr_p->form;
		transformSalesChance = ptr_p->chance;
		isQuestOverrideOn = ptr_p->override;
		main_log("Questmaster transform variables set");
		fflush(input);
		fclose(input);


	}

}				/* end void qu0100_questFileOpen() */
 /* qu0200_questWriteVar // opens files in lib / quest // it expects quest to
  * hold an 0 or 1 in ascii format. // it will also look for a newline.
  * 
  * // max should be around 10 */

int qu0200_questWriteVar(int v1, int v2, int v3)
{
	FILE *out;
	//FILE * outf;
	struct qm r;
	struct qm *ptr_p = &r;
	//in global.h
	if ((v1 == TRUE || v1 == FALSE) && (v2 >= QMSALES_MIN_CHANCE && v2 <= QMSALES_MAX_CHANCE)) {

		isQuestTransformOn = v1;
		transformSalesChance = v2;
		isQuestOverrideOn = v3;
		ptr_p->form = v1;
		ptr_p->chance = v2;
		ptr_p->override = v3;

		out = fopen("quest", "wb");

		if (out == NULL) {
			main_log("Error: cannot open quest");
		}
		else {
			main_log("Updating quest file");
			fwrite(ptr_p, sizeof(struct qm), 1, out);
			fclose(out);
		}
		return 1;
	} //end if

		else {
		return
			0;
	}



}				/* qu0101_questWriterVar() */
int qu1000_questmaster(struct char_data * ch, int cmd, char *arg)
{

	int lv_number;
	int cost = 10000;
	char buf[MAX_STRING_LENGTH];

	if ((cmd != CMD_QUEST) &&
	    (cmd != CMD_SELL) &&
	    (cmd != CMD_BUY))
		return (FALSE);


	if (cmd == CMD_QUEST) {
		qu1100_do_quest(ch, arg, 0);
		return (TRUE);
	}

	for (; isspace(*arg); arg++);

	if (cmd == CMD_SELL) {
		if (!*arg) {
			send_to_char("How many questpoints do you wish to sell?\r\n", ch);
			return (TRUE);
		}

		if (GET_QPS(ch) == 0) {
			send_to_char("You don't have that many questpoints.\r\n", ch);
			return (TRUE);
		}

		if (!is_number(arg)) {
			send_to_char("Please enter a numeric value.\r\n", ch);
			return (TRUE);
		}

		lv_number = atoi(arg);
		if (lv_number < 1) {
			send_to_char("Har har....\r\n", ch);
			return (TRUE);
		}

		if (lv_number > 100000) {
			send_to_char("You can't sell more than 100.000 at a time.\r\n", ch);
			return (TRUE);
		}

		if (GET_QPS(ch) < lv_number) {
			sprintf(buf, "You do not have %d questpoints.\r\n", lv_number);
			send_to_char(buf, ch);
			return (TRUE);
		}
		if ((MAX_GOLD - GET_GOLD(ch) - (lv_number * cost)) < 0) {
			send_to_char("The Questmaster mutters something deragotory about your wealth....", ch);
			return (TRUE);
		}
		GET_GOLD(ch) += lv_number * cost;
		GET_QPS(ch) -= lv_number;
		sprintf(buf, "You sell %d questpoints for %d gold.\r\n", lv_number, lv_number * cost);
		send_to_char(buf, ch);
		return (TRUE);
	}


	for (; isspace(*arg); arg++);

	if (cmd == CMD_BUY) {
		if (!*arg) {
			send_to_char("How many questpoints do you wish to buy?\r\n", ch);
			return (TRUE);
		}

		if (!is_number(arg)) {
			send_to_char("Please enter a numeric value.\r\n", ch);
			return (TRUE);
		}

		lv_number = atoi(arg);
		if (lv_number < 1) {
			send_to_char("Har har....\r\n", ch);
			return (TRUE);
		}

		if (lv_number > 10000) {
			send_to_char("You can only buy 10.000 at a time.\r\n", ch);
			return (TRUE);
		}

		if (GET_GOLD(ch) < (lv_number * cost)) {
			sprintf(buf, "You do not have enough money to buy %d questpoints.\r\n", lv_number);
			send_to_char(buf, ch);
			return (TRUE);
		}
		GET_GOLD(ch) -= lv_number * cost;
		GET_QPS(ch) += lv_number;
		sprintf(buf, "You buy %d questpoints for %d gold.\r\n", lv_number, lv_number * cost);
		send_to_char(buf, ch);
		return (TRUE);
	}

	return (FALSE);
}

void qu1100_do_quest(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH],
	  arg3[MAX_INPUT_LENGTH];
	struct obj_data *obj, *obj_next;
	int lv_choice, lv_reward, lv_qp, lv_buyitem, exp_modifier = 1;
	bool obj_found;
	const char *lv_quest_cmds[] = {
		"points",	/* 1 */
		"info",		/* 2 */
		"time",		/* 3 */
		"request",	/* 4 */
		"complete",	/* 5 */
		"list",		/* 6 */
		"buy",		/* 7 */
		"refuse",	/* 8 */
		"extend",	/* 9 */
		"transformations",	/* 10 */
		"\n",
	};

	int TransformPrices[] = {
		1000, //womble
		2000, //monkey
		7500, //ninja
		10000, //manticore
		15000, //wolf
		25000, //bear
		30000, //cow
		35000, //chimera
		40000, //dragon
	};


	int TransformSpells[] = {
		210, //womble
		161, //monkey
		162, //ninja
		212, //manticore
		158, //wolf
		159, //bear
		217, //cow
		215, //chimera
		160, //dragon
	};

	int QuestPrices[] = {
		50,
		200,
		100,
		100,
		25,
		40,
		55,
		75,
		100,
		1500,   //Bow 
		3000,   //Crossbow 
		4500,   //Heavy Crossbow
		1000,   //Large Backpack
		10000,	//Hasek's Wedding Ring
		15000,	//Sapphire Butterfly
		25000,	//Emerald Cloak
		30000,	//Essence of the Plague
		120000,	//Nameless Sword of Assassination
	};

	/* Command list */
#define QUEST_POINTS		1
#define QUEST_INFO			2
#define QUEST_TIME			3
#define QUEST_REQUEST		4
#define QUEST_COMPLETE		5
#define QUEST_LIST			6
#define QUEST_BUY			7
#define QUEST_REFUSE		8
#define QUEST_EXTEND		9
#define QUEST_TRANSFORM		10

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg1);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg2);
	for (; isspace(*arg); arg++);
	arg = one_argument(arg, arg3);
	//For how many more ticks of timed qeq you wanna buy.

		if (*arg1)
		lv_choice = old_search_block(arg1, 0, strlen(arg1), lv_quest_cmds, 0);

	/* Check for wrong option */
	if (lv_choice < 1) {
		send_to_char("Quest commands: Points, Info, Time, Request, Complete, List, Extend, Buy, Refuse.\r\n", ch);
		return;
	}

	/* Commands that need a quest master */
	switch (lv_choice) {
	case QUEST_REFUSE:
	case QUEST_POINTS:
	case QUEST_REQUEST:
	case QUEST_COMPLETE:
	case QUEST_BUY:
	case QUEST_TRANSFORM:
	case QUEST_LIST:
	case QUEST_EXTEND:
		if (cmd) {
			send_to_char("There is no questmaster here.\r\n", ch);
			return;
		}
	}

	switch (lv_choice) {
	case QUEST_POINTS:
		act("$n asks $N about $s questpoints.", TRUE, ch, 0, questman, TO_ROOM);
		act("You ask $N how many quest points you have.", TRUE, ch, 0, questman, TO_CHAR);
		sprintf(buf, "%s You have %d quest points.", GET_REAL_NAME(ch), ch->questpoints);
		do_whisper(questman, buf, CMD_WHISPER);
		return;

	case QUEST_INFO:
		if (!ch->countdown) {
			send_to_char("You aren't on any quest.\r\n", ch);
			return;
		}
		if (!ch->questmob && !ch->questobj) {
			send_to_char("You have completed your quest, return to the questmaster quickly.\r\n", ch);
			return;
		}
		if (ch->questobj) {
			sprintf(buf, "You are on a quest to return %s.\r\n", GET_OSDESC(ch->questobj));
			send_to_char(buf, ch);
			if (ch->questmob) {
				sprintf(buf, "Rumor has it was stolen by %s.\r\n", GET_REAL_NAME(ch->questmob));
				send_to_char(buf, ch);
			}
			return;
		}
		sprintf(buf, "You are on a quest to slay %s.\r\n", GET_REAL_NAME(ch->questmob));
		send_to_char(buf, ch);
		return;

	case QUEST_TIME:
		if (ch->nextquest) {
			sprintf(buf, "You can quest again in %d minutes.\r\n", get_jail_time(ch->nextquest, 3) + 1);
			send_to_char(buf, ch);
			return;
		}
		if (!ch->countdown) {
			send_to_char("You aren't on any quest.\r\n", ch);
			return;
		}
		sprintf(buf, "You have %d minutes left to complete your quest.\r\n", get_jail_time(ch->countdown, 3) + 1);
		send_to_char(buf, ch);
		return;

	case QUEST_REQUEST:
		act("$n asks $N for a quest.", TRUE, ch, 0, questman, TO_ROOM);
		act("You ask $N for a quest.", TRUE, ch, 0, questman, TO_CHAR);
		if (GET_LEVEL(ch) < 6) {
			send_to_char("You aren't strong enough to quest yet.\r\n", ch);
			return;
		}
		if (GET_LEVEL(ch) > PK_LEV) {
			send_to_char("Your quest is to be a good immortal!\r\n", ch);
			return;
		}
		if (IS_NPC(ch)) {
			send_to_char("Your quest is to kill players!\r\n", ch);
			return;
		}
		if (ch->countdown) {
			send_to_char("You already are on a quest.\r\n", ch);
			return;
		}
		if (ch->nextquest) {
			send_to_char("You can't quest yet, try again later.\r\n", ch);
			return;
		}
		qu1200_make_quest(ch);

		return;
	case QUEST_COMPLETE:
		act("$n informs $N $e has completed $s quest.", TRUE, ch, 0, questman, TO_ROOM);
		act("You inform $N you have completed your quest.", TRUE, ch, 0, questman, TO_CHAR);
		if (!ch->countdown) {
			sprintf(buf, "%s I didn't send you on a quest.", GET_REAL_NAME(ch));
			do_whisper(questman, buf, CMD_WHISPER);
			return;
		}
		/* Check if char has the obj in inventory */
		if (ch->questobj) {
			obj_found = FALSE;
			for (obj = ch->carrying; obj; obj = obj_next) {
				obj_next = obj->next_content;
				if (obj == ch->questobj) {
					obj_found = TRUE;
					break;
				}
			}
			if (!obj_found) {
				sprintf(buf, "%s You haven't completed your quest until you bring me %s. ",
					GET_REAL_NAME(ch),
					GET_OSDESC(ch->questobj));
				do_whisper(questman, buf, CMD_WHISPER);
				return;
			}
			act("You hand $p over to $N.", TRUE, ch, obj, questman, TO_CHAR);
			act("$n hands $p over to $N.", TRUE, ch, obj, questman, TO_ROOM);
		}

		if (ch->questmob && !ch->questobj) {
			sprintf(buf, "%s Nice try, but you'll have to slay %s first. ",
				GET_REAL_NAME(ch),
				GET_REAL_NAME(ch->questmob));
			do_whisper(questman, buf, CMD_WHISPER);
			return;
		}
		sprintf(buf, "Thank you very much %s! ", GET_REAL_NAME(ch));
		do_say(questman, buf, CMD_SAY);

		lv_reward = (ch->reward * ch->reward * 100) + number(10, (ch->reward * ch->reward * 10));
		lv_qp = (9 + ch->reward - GET_LEVEL(ch)) * 10 + number(0, 9);
		bzero(buf, sizeof(buf));
		sprintf(buf, "As a reward, I am giving you %d quest points", lv_qp);
		do_say(questman, buf, CMD_SAY);

		ch->questpoints += lv_qp;

		if (CHANCE100(25)) {
			exp_modifier += number(-1, 2);
			if (CHANCE100(15)) {
				exp_modifier += number(0, 2);
				if (CHANCE100(10)) {
					exp_modifier += number(1, 3);
					if (CHANCE100(5)) {
						exp_modifier += number(2, 4);
						if (CHANCE100(2)) {
							exp_modifier += number(3, 25);
						}
					}
				}
			}
			if (exp_modifier <= 1) {
				exp_modifier = 1;
			}
			else {
				sprintf(buf, "Your exp has been multiplied by %d", exp_modifier);
				do_say(questman, buf, CMD_SAY);
			}
		}

		if (GET_LEVEL(ch) <= PK_LEV) {
			GET_EXP(ch) += (lv_reward / 10) * exp_modifier;
			sprintf(buf, "You also gained %d experience points for your quest.",
				(lv_reward / 10) * exp_modifier);
			do_say(questman, buf, CMD_SAY);
		}

		/* 5% Chance he'll give you extra qps */
		if (CHANCE100(5)) {
			sprintf(buf, "I'm in such a good mood today i'm gonna give you an extra %d questpoints!", lv_qp);
			GET_QPS(ch) += lv_qp;
			do_say(questman, buf, CMD_SAY);
		}

		/* randon chance of getting a transform or spell, rare. Bingo
		 * 01-18-01 */
		if (CHANCE100(6)) {
			if (!IS_AFFECTED(ch, AFF_TRANSFORMED)) {
				char gosbuf[20];
				switch (number(0, 12)) {
				case 0:
				case 1:
				case 2:
				case 3:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "wolf %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "wolf");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;

				case 4:
				case 5:
				case 6:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "bear %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "bear");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;

				case 7:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "dragon %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "dragon");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;
				case 8:
				case 9:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "manticore %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "manticore");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;
				case 10:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "chimera %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "chimera");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;
				case 11:
				case 12:
					sprintf(buf, "Good job, %s!  Take this special transform for your trouble!", GET_REAL_NAME(ch));
					do_say(questman, buf, CMD_SAY);
					sprintf(buf, "cow %s", GET_REAL_NAME(ch));
					sprintf(gosbuf, "cow");
					do_cast_proc(questman, ch, buf, CMD_CAST, 0);
					break;

				}
				bzero(buf, sizeof(buf));
				sprintf(buf, "%s has helped me out so much, I'm rewarding %s with a %s form.  "
					"I do, however, have more stuff that needs taking care of, come see me!",
					GET_NAME(ch), (GET_SEX(ch) == SEX_FEMALE) ? "her" : "him", gosbuf);
				do_gossip(questman, buf, CMD_GOSSIP);
			}
		}
		/* To add qeq to what the qm will load, add the number to the
		 * qeq_array, and increment the two number(0, x) calls for each
		 * one you add.  */
		if (number(0, 99) > 95) {
			//94 is a 5 in 100 chance
				// changed to 4 % 27.08 .2004 because all equipment now are untimed
				struct obj_data *obj = 0;
			int r_num, lv_timer = -1;
			int qeq_array[] = {
				80,     //Hatred Belt
				20864,	//The Ghoul's Belt
				8985,	//Chains of an Angel
				20090,	//Boots of Blinding Speed
				20091,	//Gauntlets of Godly Might
				9111,   //green sleeves adamantium thread
				9114,   //black hood with silve runes
				9115,   //green pants adamantium thread
				9116,   //drow chainmail
				9117,   //gloves with silver runes
				9118,   //black boots with silver runes
				19251,  //The gauntlets of the agile
				880,	//Legacy Leg Plates
				19252,  //fire forged legplates
				20850,	//Holy Cross of Byzantium
				876,	//Ebony Blade of Irredeemable Despair
				6999,   //scythe of death
				8900,   //Shatterer of Dreams
				7522,   //Nameless Sword
				7445,	//Ladondra's Cross
				8976,	//A Tail Made of Flames
				9122,   //Pseudodragon "Singe"
				9106,   //bracer of god
				20863,  //The Mummy's Charm
				20103,  //Talisman of Death
				99112,	//Brimstone Ring
				15801   //Detroit NHL RING
			};
			
			int mort_qeq_array[] = {
				8986,   //Skin Laced Sandals
				9106,   //bracer of god
				880,    //Legacy Leg Plates
				881,    //Mantle of Dread
				882,    //Spear of Treachery
				7522,   //Nameless Sword
				5981,   //the belt of haste
				85      //little black cat
			};

			int temp_number;
			if (GET_LEVEL(ch) >= IMO_LEV) {
				temp_number = (number(0, 26));
			}
			else {
				temp_number = (number(0, 7));
			}


			if (GET_LEVEL(ch) == IMO_LEV) {
				r_num = (db8200_real_object(qeq_array[temp_number]));
			}
			else {
				r_num = (db8200_real_object(mort_qeq_array[temp_number]));
			}

			char buf[MAX_STRING_LENGTH];
			sprintf(buf, "Questmaster");
			main_log(buf);
			sprintf(buf, "Questmaster loaded: temp_number == %d", temp_number);
			main_log(buf);

			obj = db5100_read_object(db8200_real_object(r_num), REAL);

			obj = db5100_read_object(r_num, REAL);

			if (check_lore(ch, obj)) {

				ha2700_extract_obj(obj);
				obj = 0;
				if (GET_LEVEL(ch) >= IMO_LEV) {
					switch (number(0, 2)) {
					case 0:
						obj = db5100_read_object(db8200_real_object(80), REAL);
						sprintf(buf, "Questmaster lore 0 -- 80");
						main_log(buf);
						break;
					case 1:
						obj = db5100_read_object(db8200_real_object(7445), REAL);
						sprintf(buf, "Questmaster lore 1 -- 7445");
						main_log(buf);
						break;
					case 2:
						obj = db5100_read_object(db8200_real_object(876), REAL);
						sprintf(buf, "Questmaster lore 2 -- 876");
						main_log(buf);
						break;
					}

				}
				else {
					switch (number(0, 1)) {
					case 0:
						obj = db5100_read_object(db8200_real_object(881), REAL);
						sprintf(buf, "Questmaster non-lore 0 -- 881");
						main_log(buf);
						break;
					case 1:
						obj = db5100_read_object(db8200_real_object(882), REAL);
						sprintf(buf, "Questmaster non-lore 1 -- 882");
						main_log(buf);
						break;
					}
				}
			}

			/* CHECK FOR OMAX ITEMS */

			if (IS_SET(obj_index[r_num].flags, OBJI_OMAX)) {
				if (obj_index[r_num].number >= obj_index[r_num].maximum) {
					if (GET_LEVEL(ch) == IMO_LEV) {
						r_num = (db8200_real_object(891));
						sprintf(buf, "Questmaster IS_SET(obj_index[r_num].flags, OBJI_OMAX) -- 891");
						main_log(buf);
					}
					else {
						sprintf(buf, "Questmaster IS_SET(obj_index[r_num].flags, OBJI_OMAX) -- 892");
						main_log(buf);
						r_num = (db8200_real_object(892));
					}
					ha2700_extract_obj(obj);
					obj = 0;

				}
			}
			//Removed timer on questmaster handed questequipment
				// 27.08 .2004 Shalira
				// if (number(0, 19) < 19)
				//lv_timer = (30 + number(0, 30)) + 1;

			if (obj) {
				TIMER(obj) = lv_timer;
				sprintf(buf, "Nice going, %s!!  Maybe this will help you in your future quests.", GET_REAL_NAME(ch));
				ha1700_obj_to_char(obj, ch);
				act("$N gives you a $p!", TRUE, ch, obj, questman, TO_CHAR);
				act("$N gives $n a $p.", TRUE, ch, obj, questman, TO_ROOM);
				bzero(buf, sizeof(buf));
				do_say(questman, buf, CMD_SAY);
				bzero(buf, sizeof(buf));
				sprintf(buf, "Wow!  %s is one of the greatest questers I've ever known, "
					"so great, in fact, that I'm gonna give %s %s %s!",
					GET_NAME(ch), HMHR(ch), ANA(obj), GET_OSDESC(obj));
				do_gossip(questman, buf, CMD_GOSSIP);
			}
		}
		obj = ch->questobj;
		ch->questobj = 0;
		if (obj)
			ha2700_extract_obj(obj);
		qu1400_clear_quest(ch);
		return;

		/* QUEST TRANSFORM */
	case QUEST_TRANSFORM:

		if ((isQuestAutoOn && isQuestTransformOn) || isQuestOverrideOn) {

			if (GET_LEVEL(ch) == IMO_IMP)
				ch->nextquest = 0;
			if (!*arg2) {
				act("$n asks $N for a list of possible transformations.", TRUE, ch, 0, questman, TO_ROOM);
				act("you ask $N for a list of possible transformations.", TRUE, ch, 0, questman, TO_CHAR);
				send_to_char("Possible transformations:\r\n", ch);
				send_to_char(" &W1&n) &wWomble&n        &G1000&n Qps\r\n", ch);
				send_to_char(" &W2&n) &YMonkey&n        &G2000&n Qps\r\n", ch);
				send_to_char(" &W3&n) &bNinja&n         &G7500&n Qps\r\n", ch);
				send_to_char(" &W4&n) &gManticore&n    &G10000&n Qps\r\n", ch);
				send_to_char(" &W5&n) &CWolf&n         &G15000&n Qps\r\n", ch);
				send_to_char(" &W6&n) &yBear&n         &G25000&n Qps\r\n", ch);
				send_to_char(" &W7&n) &rCow&n          &G30000&n Qps\r\n", ch);
				send_to_char(" &W8&n) &WChimera&n      &G35000&n Qps\r\n", ch);
				send_to_char(" &W9&n) &RDragon&n       &G40000&n Qps\r\n", ch);
				send_to_char("\r\nTo transform into one of these type Quest Transform <nr> \r\n", ch);
				return;
			}
			if (!is_number(arg2)) {
				send_to_char("Please enter a numeric value (see quest list).\r\n", ch);
				return;
			}
			lv_buyitem = atoi(arg2);
			if ((lv_buyitem < 1) || (lv_buyitem > 9)) {
				send_to_char("Unknown form.\r\n", ch);
				return;
			}
			if (TransformPrices[lv_buyitem - 1] > ch->questpoints) {
				send_to_char("You do not have enough Quest Points!\r\n", ch);
				return;
			}

			if (IS_AFFECTED(ch, AFF_TRANSFORMED)) {
				sprintf(buf, "%s, you already have form.  Freakin\' moron!", GET_REAL_NAME(ch));
				do_say(questman, buf, CMD_SAY);
				return;
			}

			ch->questpoints -= TransformPrices[lv_buyitem - 1];

			sprintf(buf, "%d %s", TransformSpells[lv_buyitem - 1], GET_REAL_NAME(ch));
			do_cast_proc(questman, ch, buf, CMD_CAST, 0);
		} //end if
			break;

	case QUEST_LIST:
		act("$n asks $N for a list of quest items.", TRUE, ch, 0, questman, TO_ROOM);
		act("You ask $N for a list of quest items.", TRUE, ch, 0, questman, TO_CHAR);
		send_to_char("Quest Items for sale:\n\r", ch);
		send_to_char("    &cQuestpoints                             &C10000&n Gold\r\n", ch);
		send_to_char("&W 1&n) &cFull Restore                         &G    50&n Qps\n\r", ch);
		send_to_char("&W 2&n) &cSanctuary                            &G   200&n Qps\n\r", ch);
		send_to_char("&W 3&n) &cPotion of Fly                        &G   100&n Qps\n\r", ch);
		send_to_char("&W 4&n) &cScroll of Demonic Aid                &G   100&n Qps\n\r", ch);
		send_to_char("&W 5&n) &cArrow                                &G    25&n Qps\n\r", ch);
		send_to_char("&W 6&n) &cPoisoned arrow                       &G    40&n Qps\n\r", ch);
		send_to_char("&W 7&n) &cBolt                                 &G    55&n Qps\n\r", ch);
		send_to_char("&W 8&n) &cPoisoned bolt                        &G    75&n Qps\n\r", ch);
		send_to_char("&W 9&n) &cDeadly bolt                          &G   100&n Qps\n\r", ch);
		send_to_char("&W10&n) &cBow                         &R(lvl  1) &G  1500&n Qps\n\r", ch);
		send_to_char("&W11&n) &cCrossbow                    &R(lvl 20) &G  3000&n Qps\n\r", ch);
		send_to_char("&W12&n) &cHeavy Crossbow              &R(lvl 30) &G  4500&n Qps\n\r", ch);
		send_to_char("&W13&n) &cLarge Backpack              &R(lvl 10) &G  1000&n Qps\n\r", ch);
		send_to_char("&W14&n) &cHasek's Wedding Ring <Lore> &R(lvl 41) &G 10000&n Qps\n\r", ch);
		send_to_char("&W15&n) &cSapphire Butterfly          &R(lvl 41) &G 15000&n Qps\n\r", ch);
		send_to_char("&W16&n) &cEmerald Green Cloak         &R(lvl 41) &G 25000&n Qps\n\r", ch);
		send_to_char("&W17&n) &cEssence of the Plague       &R(lvl 41) &G 30000&n Qps\n\r", ch);
		send_to_char("&W18&n) &cNameless Sword <Lore>       &R(lvl 41)&G 120000&n Qps\n\r", ch);
		send_to_char("\r\nTo buy an item type Quest buy <nr>\r\n", ch);
		send_to_char("For transformations type Quest Transform\r\n", ch);
		send_to_char("To buy questpoints, type 'buy <amount>'\r\n", ch);
		return;

	case QUEST_BUY:
		if (GET_LEVEL(ch) == IMO_IMP)
			ch->nextquest = 0;
		if (!*arg2) {
			send_to_char("What do you wish to buy?\r\n", ch);
			return;
		}
		if (!is_number(arg2)) {
			send_to_char("Please enter a numeric value (see quest list).\r\n", ch);
			return;
		}
		lv_buyitem = atoi(arg2);
		if ((lv_buyitem < 1) || (lv_buyitem > 18)) {
			send_to_char("Unknown item.\r\n", ch);
			return;
		}
		if (QuestPrices[lv_buyitem - 1] > ch->questpoints) {
			send_to_char("You do not have enough Quest Points!\r\n", ch);
			return;
		}
		ch->questpoints -= QuestPrices[lv_buyitem - 1];

		switch (lv_buyitem) {
		case 1:
			special_restore(questman, ch);
			break;
		case 2:
			sprintf(buf, "sanctuary %s", GET_REAL_NAME(ch));
			do_cast_proc(questman, ch, buf, CMD_CAST, 0);
			break;
		case 13:
			// Large Backpack
                        if (GET_LEVEL(ch) < 10){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 3463;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		case 14:
			// This is for a Hasek's Wedding Ring
                        if (GET_LEVEL(ch) < IMO_LEV){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 15802;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		case 15:
			// This is for a Sapphire Butterfly
                        if (GET_LEVEL(ch) < IMO_LEV){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 1037;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		case 16:
			// This is for an Emerald Green Cloak
                        if (GET_LEVEL(ch) < IMO_LEV){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 20093;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		case 17:
			//This is for an Essence of the Plague
                        if (GET_LEVEL(ch) < IMO_LEV){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 79;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		case 18:
			// This is for Nameless Sword of Assassination
                        if (GET_LEVEL(ch) < IMO_LEV){
                        send_to_char("You do not have enough experience to buy this item!\r\n", ch);
						ch->questpoints += QuestPrices[lv_buyitem - 1];
                        break;
                        }
                        else {
                        lv_buyitem = 7522;
                        sprintf(buf, "obj %d", lv_buyitem);
                        wi2700_do_load(questman, buf, CMD_QUEST);
                        sprintf(buf, "obj %s", GET_REAL_NAME(ch));
                        do_give(questman, buf, CMD_QUEST);
                        break;
			}
		default:
			sprintf(buf, "obj %d", 547 + lv_buyitem);
      			wi2700_do_load(questman, buf, CMD_QUEST);
			sprintf(buf, "obj %s", GET_REAL_NAME(ch));
			do_give(questman, buf, CMD_QUEST);
			break;
		}
		break;
	case QUEST_REFUSE:
		act("$n informs $N that $e refuses $s quest.", TRUE, ch, 0, questman, TO_ROOM);
		act("You inform $N that you refuse the quest.", TRUE, ch, 0, questman, TO_CHAR);
		if (!ch->countdown) {
			sprintf(buf, "%s How can you refuse a quest when you have none?", GET_REAL_NAME(ch));
			do_whisper(questman, buf, CMD_WHISPER);
			return;
		}
		sprintf(buf, "%s You'll have to wait 5 min before you can start your next quest", GET_REAL_NAME(ch));
		do_whisper(questman, buf, CMD_WHISPER);
		qu1400_clear_quest(ch);
		ch->nextquest = time(0) + 5 * 60;
		break;

	case QUEST_EXTEND:

		if (!*arg2) {
			do_say(questman, "Which object?", CMD_SAY);
			return;
		}

		if (!*arg3) {
			do_say(questman, "How many ticks?", CMD_SAY);
			return;
		}

		if (!is_number(arg3)) {
			do_say(questman, "No, no, no.  Ticks in *numbers* ;)", CMD_SAY);
			return;
		}

		lv_buyitem = atoi(arg3);
		obj = ha2075_get_obj_list_vis(ch, arg2, ch->carrying);

		if (!obj) {
			send_to_char("Sorry, you don't have anything like that!\r\n", ch);
			return;
		}

		if (GET_TIMER(obj) == -1) {
			do_say(questman, "I don't think you want me to mess with that!", CMD_SAY);
			return;
		}

		if (IS_OBJ_STAT2(obj, OBJ2_NO_EXTEND)) {
			do_say(questman, "Hrmm...I can't seem to mess with this object, a special magic protects it!", CMD_SAY);
			return;
		}

		if (GET_TIMER(obj) + lv_buyitem > 100) {
			do_say(questman, "I can't make it last that long!", CMD_SAY);
			return;
		}

		if (lv_buyitem * 200 > ch->questpoints) {
			do_say(questman, "You don't have enough questpoints!", CMD_SAY);
			do_say(questman, "Each additional tick costs 200 questpoints.", CMD_SAY);
			return;
		}

		TIMER(obj) = TIMER(obj) + lv_buyitem;
		ch->questpoints = ch->questpoints - (lv_buyitem * 200);
		do_say(questman, "There you go, enjoy!", CMD_SAY);

		break;
	}			/* end of switch (lv_choice) */
}				/* end of qu1100_do_quest() */

void qu1200_make_quest(struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];
	struct char_data *vict;
	struct obj_data *obj;
	int lv_number, lv_dev, lv_div, lv_objvnum, lv_level, r_num, idx,
	  jdx;

	/* Pick a random number between bottom and top of mob table */
	lv_number = number(1, 10000);
	lv_dev = number(1, 25);
	if (GET_LEVEL(ch) == PK_LEV)
		lv_level = number(47, 58);	/* 50 is playrs level - pk -5
						 * level +6 else if
						 * (GET_LEVEL(ch) == IMO_LEV)
						 * lv_level = number (46, 57);
						 * 50 is players level - avats
						 * -4 level +7 */
	else
		lv_level = number(44, 55);	/* 50 is players level -
						 * mortals -6 level +5 */
	idx = 0;

	/* Find the mob with that number */
	while (idx < lv_number)
		for (vict = character_list; vict && (idx <= lv_number); vict = vict->next)
			idx++;

	if (!vict) {
		send_to_char("No quests available at this time, Try again later.\r\n", ch);
		main_log("ERROR: QUEST: Top_of_mobt reaches above mob_table.");
		spec_log("ERROR: QUEST: Top_of_mobt reaches above mob_table.", ERROR_LOG);
		return;
	}

	lv_div = 0;
	idx = 0;
	jdx = 0;

	/* Continue walking through mob list until a suitable mob is found */
	while ((lv_div != lv_level) ||
	       (IS_SET(GET_ACT1(vict), PLR1_NOKILL)) ||
	       (IS_PC(vict)) ||
	       (vict->specials.fighting) ||
	       (!vict->in_room) ||
	       (IS_SET(GET_ROOM1(vict->in_room), RM1_NOMKILL)) ||
	       (IS_AFFECTED(vict, AFF_CHARM)) ||
	       (IS_SET(GET_ACT2(vict), PLR2_JUNKS_EQ)) ||
	       (world[vict->in_room].min_level > GET_LEVEL(ch)) ||
	       (world[vict->in_room].max_level < GET_LEVEL(ch)) ||
	       (vict->questmob) ||
	       (jdx < lv_dev)) {

		vict = vict->next;
		if (!vict) {
			vict = character_list;
			if ((idx++) > 10) {
				/* If we went through the mob table more then
				 * 10 times then there are no available mobs */
				send_to_char("No quests available at this time, Try again later.\r\n", ch);
				sprintf(buf, "ERROR: QUEST: Can't find mob for %s level %d", GET_REAL_NAME(ch), GET_LEVEL(ch));
				main_log(buf);
				spec_log(buf, ERROR_LOG);
				return;
			}
		}
		lv_div = 50 + GET_LEVEL(vict) - GET_LEVEL(ch);
		if ((lv_div < 42) || (lv_div > 57))
			jdx++;
	}

	/* 25% chance you have to find an item */
	if ((CHANCE100(25)) && (!IS_SET(GET_ACT2(ch), PLR2_JUNKS_EQ))) {
		lv_objvnum = 0;
		switch (number(0, 4)) {
		case 0:
			lv_objvnum = QUEST_OBJ1;
			break;
		case 1:
			lv_objvnum = QUEST_OBJ2;
			break;
		case 2:
			lv_objvnum = QUEST_OBJ3;
			break;
		case 3:
			lv_objvnum = QUEST_OBJ4;
			break;
		case 4:
			lv_objvnum = QUEST_OBJ5;
			break;
		}

		r_num = db8200_real_object(lv_objvnum);
		obj = db5100_read_object(r_num, REAL);

		/* 50% chance the item is on a mob */
		if (CHANCE100(50)) {
			ha1700_obj_to_char(obj, vict);
			sprintf(buf, "%s %s is stolen from the royal treasury!", GET_NAME(ch), GET_OSDESC(obj));
			do_whisper(questman, buf, CMD_WHISPER);
			sprintf(buf, "%s My court wizard has located it on %s.", GET_NAME(ch), GET_REAL_NAME(vict));
			do_whisper(questman, buf, CMD_WHISPER);

			SET_BIT((obj)->obj_flags.flags1, OBJ1_QUEST_ITEM);
			ch->questobj = obj;
			ch->questmob = vict;
			vict->questmob = ch;
			ch->nextquest = 0;
			ch->countdown = time(0) + number(15, 35) * 60;	/* give them 15 - 35 min */
			ch->reward = GET_LEVEL(vict);
		}
		else {
			ha2100_obj_to_room(obj, vict->in_room);
			sprintf(buf, "%s %s is stolen from the royal treasury!", GET_NAME(ch), GET_OSDESC(obj));
			do_whisper(questman, buf, CMD_WHISPER);
			sprintf(buf, "%s It seems the thief lost it at %s in %s ",
				GET_REAL_NAME(ch),
				world[vict->in_room].name,
				zone_table[world[vict->in_room].zone].name);
			do_whisper(questman, buf, CMD_WHISPER);
			SET_BIT((obj)->obj_flags.flags1, OBJ1_QUEST_ITEM);
			ch->questobj = obj;
			ch->questmob = 0;
			ch->nextquest = 0;
			ch->countdown = time(0) + number(15, 35) * 60;	/* give them 15 - 35 min */
			ch->reward = GET_LEVEL(vict);
		}
	}
	else {
		switch (number(0, 1)) {
		case 0:{
				sprintf(buf, "%s %s is making threats against my life.", GET_NAME(ch), GET_REAL_NAME(vict));
				do_whisper(questman, buf, CMD_WHISPER);
				sprintf(buf, "%s You are to stop these threaths!", GET_NAME(ch));
				do_whisper(questman, buf, CMD_WHISPER);
			} break;
		case 1:{
				sprintf(buf, "%s The most heinous criminal, %s, has escaped from jail!", GET_NAME(ch), GET_REAL_NAME(vict));
				do_whisper(questman, buf, CMD_WHISPER);
				sprintf(buf, "%s Since the escape, %s has murdered %d civillians!", GET_NAME(ch), GET_REAL_NAME(vict), number(20, 200));
				do_whisper(questman, buf, CMD_WHISPER);
				sprintf(buf, "%s The penalty for this crime is death, and you are to deliver the sentence!", GET_REAL_NAME(ch));
				do_whisper(questman, buf, CMD_SAY);
			} break;
		}
		sprintf(buf, "%s %s was last seen at %s in %s",
			GET_REAL_NAME(ch),
			GET_REAL_NAME(vict),
			world[vict->in_room].name,
			zone_table[world[vict->in_room].zone].name);
		do_whisper(questman, buf, CMD_WHISPER);
		ch->questobj = 0;
		ch->questmob = vict;
		vict->questmob = ch;
		ch->nextquest = 0;
		if (lv_div <= 50)
			ch->countdown = time(0) + number(15, 30) * 60;	/* give them 15 - 30 min */
		else
			ch->countdown = time(0) + number(20, 40) * 60;	/* give them 20 - 40 min */
		ch->reward = GET_LEVEL(vict);
	}

	if (ch->questmob > 0 || ch->questobj > 0) {
		sprintf(buf, "%s You have %d minutes to complete this quest.", GET_REAL_NAME(ch), get_jail_time(ch->countdown, 3));
		do_whisper(questman, buf, CMD_WHISPER);
	}
}				/* END OF qu1200_make_quest() */



void qu1300_quest_update(void)
{

	struct char_data *ch;
	struct descriptor_data *d;

	/* Check which players are doing quest and how much time is left */
	for (d = descriptor_list; d; d = d->next) {
		if (d->character &&
		    (d->connected == CON_PLAYING) &&
		    (d->character->in_room >= 0)) {
			ch = d->character;
			if (ch->countdown) {
				/* Reset quest data when time is up */
				if (ch->countdown < time(0)) {
					send_to_char("&YYou have run out of time for your quest.&n\r\n", ch);
					send_to_char("&YYou can get another quest in 5 min.&n\r\n", ch);
					qu1400_clear_quest(ch);
					ch->reward = 0;
				}
				else if (get_jail_time(ch->countdown, 3) == 5)
					send_to_char("&YHurry up, you're almost out of time for your quest!&n\r\n", ch);
			}
			if (ch->nextquest && (ch->nextquest < time(0))) {
				send_to_char("You may now quest again.\r\n", ch);
				ch->nextquest = 0;
			}
		}
	}
}				/* END OF qu1300_quest_update() */



void qu1400_clear_quest(struct char_data * ch)
{

	if (ch->questmob)
		ch->questmob->questmob = 0;
	if (ch->questobj)
		REMOVE_BIT((ch->questobj)->obj_flags.flags1, OBJ1_QUEST_ITEM);
	/* if (ch->questobj) ha2750_decay_object(ch->questobj, FALSE); */
	ch->questobj = 0;
	ch->questmob = 0;
	ch->countdown = 0;
	ch->nextquest = time(0);
	//+5 * 60;
	ch->reward = 0;

}


void qu2000_check_quest(struct char_data * ch, char *arg)
{

	struct char_data *mob;
	struct quest_data *quest = 0;
	struct conversation_data *convo = 0;
	struct keyword_data *keys;
	int found = 0, count, idx;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char old_arg[MAX_STRING_LENGTH];
	char *pbuf, *pname;

	if (!IS_PC(ch))
		return;
	if (!arg || !*arg)
		return;

	mob = world[ch->in_room].people;
	if (!mob)
		return;
	strcpy(old_arg, arg);

	while (mob && !found) {
		if (IS_NPC(mob) && mob->questdata) {
			quest = mob->questdata;
			if (quest)
				convo = quest->convo;
			while (convo && !found) {
				keys = convo->keywords;
				count = 0;
				while (keys) {
					count = (count << 1) + 1;
					keys = keys->next;
				}
				arg = old_arg;
				while (arg && *arg) {
					bzero(buf, sizeof(buf));
					arg = one_argument(arg, buf);
					idx = 1;
					keys = convo->keywords;
					while (keys) {
						if (is_abbrev(keys->keyword, buf))
							SET_BIT(found, idx);
						keys = keys->next;
						idx <<= 1;
					}
				}
				if (found != count) {
					found = 0;
				}
				else {
					if (convo->reply) {
						strcpy(buf, convo->reply);
						while ((*(buf + strlen(buf) - 1) == '\r') || (*(buf + strlen(buf) - 1) == '\n'))
							*(buf + strlen(buf) - 1) = '\0';

						bzero(buf2, sizeof(buf2));
						pbuf = buf2;
						for (idx = 0; *(buf + idx); idx++) {
							if (*(buf + idx) == '#') {
								pname = GET_REAL_NAME(ch);
								for (; *pname; *(pbuf++) = *(pname++));
							}
							else
								*(pbuf++) = *(buf + idx);
						}
						*pbuf = 0;
						if (*buf) {
							act(" ", 0, ch, 0, 0, TO_CHAR);
							act(" ", 0, ch, 0, 0, TO_ROOM);
							do_say(mob, buf2, CMD_QUEST);
						}
						if (convo->command) {
							bzero(buf, sizeof(buf));
							strcpy(buf, convo->command);
							bzero(buf2, sizeof(buf2));
							pbuf = buf2;
							for (idx = 0; *(buf + idx); idx++) {
								if (*(buf + idx) == '#') {
									pname = GET_REAL_NAME(ch);
									for (; *pname; *(pbuf++) = *(pname++));
								}
								else
									*(pbuf++) = *(buf + idx);
							}
							*pbuf = 0;
							if (*buf) {
								act(" ", 0, ch, 0, 0, TO_CHAR);
								act(" ", 0, ch, 0, 0, TO_ROOM);

								gv_questreact = TRUE;
								command_interpreter(mob, buf2);
								gv_questreact = FALSE;
							}
						}
					}
					return;

				}

				convo = convo->next;
			}

		}
		mob = mob->next_in_room;
	}
}

void qu2100_do_mquestkey(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], keybuf[MAX_STRING_LENGTH];
	int vnum, lv_nr, idx, count;
	bool exinfo = FALSE, exexinfo = FALSE;
	int was_new = 0;
	struct conversation_data *convo, *prev_convo;
	struct quest_data *qd;
	struct keyword_data *key;
	struct char_data *mob;
	char *p, *pp;

	if ((GET_LEVEL(ch) < IMO_IMM)) {
		send_to_char("Huh?\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !(*buf)) {
		send_to_char("Missing arguments, Usage:\r\n", ch);
		send_to_char("\r\n", ch);
		send_to_char("mquestkey list\r\n", ch);
		send_to_char("    Gives a list of mobs with the amount of mquest replies\r\n", ch);
		send_to_char("\r\n", ch);
		send_to_char("mquestkey listx\r\n", ch);
		send_to_char("    Gives a list of mobs and a list of all their mquest conversations\r\n", ch);
		send_to_char("\r\n", ch);
		send_to_char("mquestkey <mob_vnum> <convo_nr> <keywords>.\r\n", ch);
		send_to_char("    Changes/adds the mquest keywords of a mob\r\n", ch);
		return;
	}

	if (!is_number(buf)) {
		if (is_abbrev(buf, "listxxx")) {
			exinfo = ((strcmp(buf, "listx") == 0) || (strcmp(buf, "listxxx") == 0));
			exexinfo = ((strcmp(buf, "listxx") == 0) || (strcmp(buf, "listxxx") == 0));

			if (GET_LEVEL(ch) < IMO_IMP)
				exexinfo = FALSE;

			if (exinfo) {
				send_to_char(" V-Num    #  Hi Lo  Keywords\r\n", ch);
				send_to_char("--------------------------------------------\r\n", ch);
			}
			else {
				send_to_char(" V-Num  # Mob name\r\n", ch);
				send_to_char("--------------------------------------\r\n", ch);
			}

			/* show list of all quest keys */
			for (idx = 0; idx <= top_of_questt; idx++) {
				qd = &quest_index[idx];

				/* get mob prototype */
				if (!(mob = db5000_read_mobile(qd->vmob, PROTOTYPE)))
					continue;

				/* Check if it's in the same zone */
				if (!exexinfo)
					if (mob_index[mob->nr].zone != world[ch->in_room].zone)
						continue;

				/* print vnum */
				if (exinfo) {
					sprintf(buf, "&C%6d&n %s\r\n", qd->vmob, GET_REAL_NAME(mob));
					send_to_char(buf, ch);
				}

				/* Loop through convos */
				count = 0;
				for (convo = qd->convo; convo; convo = convo->next) {
					/* only display info when extended info
					 * is requested */
					if (exinfo) {
						/* combine keywords to one
						 * string */
						bzero(keybuf, sizeof(keybuf));
						for (key = convo->keywords; key; key = key->next) {
							sprintf(buf, "%s %s", keybuf, key->keyword);
							strcpy(keybuf, buf);
						}

						/* Print convo info */
						sprintf(buf, "        &c%3d&n) %2d %2d &W%s\r\n", convo->number, convo->low, convo->high, keybuf);
						send_to_char(buf, ch);

						/* Print reply(add leading
						 * spaces for each line) */
						sprintf(buf2, convo->reply);

						for (p = pp = buf2; p != &buf2[MAX_STRING_LENGTH]; p++) {
							if (*p == '\r')
								*p = ' ';

							/* if end of string,
							 * add an < enter > */
							if ((!*p) && (p != pp)) {
								*p = '\n';
								*(p + 1) = '\0';
							}

							/* if <enter>, print
							 * the string and move
							 * pointer */
							if (*p == '\n') {
								*p = '\0';

								sprintf(buf, "              &Y%s&n\r\n", pp);
								send_to_char(buf, ch);

								p++;
								pp = p;
							}

							/* Break on end of
							 * string */
							if (!*p)
								break;
						}
					}
					count++;
				}

				if (exinfo)
					send_to_char("\r\n", ch);
				/* show single line info per mob */
				else {
					sprintf(buf, "&C%6d &W%2d&n %s\r\n", qd->vmob, count, GET_REAL_NAME(mob));
					send_to_char(buf, ch);
				}

			}
		}
		else {
			send_to_char("first argument (mob vnum) has to be numeric.\r\n", ch);
		}
		return;
	}

	vnum = atoi(buf);
	for (idx = 0; idx <= top_of_mobt; idx++) {
		if (mob_index[idx].virtual == vnum)
			break;
	}

	if (idx > top_of_mobt) {
		sprintf(buf, "Mob %d does not exist.\r\n", vnum);
		send_to_char(buf, ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !*buf) {
		qu2300_quest_info(ch, vnum);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("second argument (convo_nr) has to be numeric.\r\n", ch);
		return;
	}

	lv_nr = atoi(buf);
	if ((lv_nr < 1) || (lv_nr > 99)) {
		send_to_char("second argument (convo_nr) has to be between 1 and 99.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);

	if (!*arg) {
		qu2400_convo_info(ch, vnum, lv_nr);
		return;
	}

	/* First check if there's already quest info available */
	for (idx = 0; idx <= top_of_questt; idx++) {
		if (quest_index[idx].vmob == vnum)
			break;
	}

	/* alocate new structure if mob has no quest attached yet */
	if (idx > top_of_questt) {
		db2500_allocate_quest(idx);
		quest_index[idx].vmob = vnum;
		top_of_questt++;
	}

	/* check if convo exists, if not create a new one */
	convo = quest_index[idx].convo;
	if (!convo) {
		convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
		bzero((char *) convo, sizeof(struct conversation_data));
		quest_index[idx].convo = convo;
		was_new = TRUE;
	}
	else {
		while (convo) {
			if (convo->number == lv_nr)
				break;
			prev_convo = convo;
			convo = convo->next;
		}
		if (!convo) {
			convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
			bzero((char *) convo, sizeof(struct conversation_data));
			prev_convo->next = convo;
			was_new = TRUE;
		}
	};

	/* Delete old keywords if they exist and attach new ones */
	if (convo->keywords)
		free(convo->keywords);
	if (was_new) {
		convo->number = lv_nr;
		convo->low = 1;
		convo->high = 99;
	};
	convo->keywords = (struct keyword_data *) db4750_split_keywords(arg);

	sprintf(buf, "Keywords for mob<%d> convo<%d> changed to '%s'.\r\n", vnum, lv_nr, arg);
	send_to_char(buf, ch);
	sprintf(buf, "%d %d ADD", vnum, lv_nr);
	if (was_new)
		qu2200_do_mquestsay(ch, buf, CMD_MQUESTSAY);
};

void qu2200_do_mquestsay(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int vnum;
	int lv_nr;
	int idx;
	struct conversation_data *convo, *prev_convo;
	struct keyword_data *key, *prev_key;

	if ((GET_LEVEL(ch) < IMO_IMM) && !ha1175_isexactname(GET_NAME(ch), "dutch")) {
		send_to_char("Huh?\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !*buf) {
		send_to_char("Missing arguments for mquestsay <mob_vnum> <convo_nr> <command>.\r\n", ch);
		send_to_char("Command can be one of the following: EDIT, SHOW, DELETE\r\n", ch);
		send_to_char("Defaults to SHOW when not entered.\r\n", ch);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("first argument (mob vnum) has to be numeric.\r\n", ch);
		return;
	}

	vnum = atoi(buf);
	for (idx = 0; idx <= top_of_mobt; idx++) {
		if (mob_index[idx].virtual == vnum)
			break;
	}

	if (idx > top_of_mobt) {
		sprintf(buf, "Mob %d does not exist.\r\n", vnum);
		send_to_char(buf, ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !*buf) {
		qu2300_quest_info(ch, vnum);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("second argument (convo_nr) has to be numeric.\r\n", ch);
		return;
	}

	lv_nr = atoi(buf);
	if ((lv_nr < 1) || (lv_nr > 99)) {
		send_to_char("second argument (convo_nr) has to be between 1 and 99.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*buf || is_abbrev(buf, "show")) {
		qu2400_convo_info(ch, vnum, lv_nr);
		return;
	}

	/* First check if there 's already quest info available */
	for (idx = 0; idx <= top_of_questt; idx++) {
		if (quest_index[idx].vmob == vnum)
			break;
	}

	if (is_abbrev(buf, "delete")) {
		if (idx > top_of_questt) {
			sprintf(buf, "mob<%d> doesn't have any quest data.\r\n", vnum);
			send_to_char(buf, ch);
			return;
		}

		convo = quest_index[idx].convo;
		if (!convo) {
			sprintf(buf, "Quest data for mob<%d> has already been deleted.\r\n", vnum);
			send_to_char(buf, ch);
			return;
		};

		prev_convo = 0;
		while (convo) {
			if (convo->number == lv_nr) {
				if (!prev_convo)
					quest_index[idx].convo = convo->next;
				else
					prev_convo->next = convo->next;
				prev_key = 0;
				key = convo->keywords;
				while (key) {
					prev_key = key;
					key = key->next;
					prev_key->next = 0;
					if (prev_key->keyword)
						free(prev_key->keyword);
					free(prev_key);
				}
				if (convo->reply)
					free(convo->reply);
				if (convo->command)
					free(convo->command);
				free(convo);
				sprintf(buf, "mob<%d> convo<%d> has been deleted.\r\n", vnum, lv_nr);
				send_to_char(buf, ch);
				return;
			}
			prev_convo = convo;
			convo = convo->next;
		}
		sprintf(buf, "mob<%d> doesn't have a convo<%d>.\r\n", vnum, lv_nr);
		send_to_char(buf, ch);
		return;

	}

	if (is_abbrev(buf, "edit") || is_abbrev(buf, "add")) {

		/* alocate new structure if mob has no quest attached yet */
		if (idx > top_of_questt) {
			db2500_allocate_quest(idx);
			quest_index[idx].vmob = vnum;
			top_of_questt++;
		}

		/* check if convo exists, if not create a new one */
		convo = quest_index[idx].convo;
		if (!convo) {
			convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
			bzero((char *) convo, sizeof(struct conversation_data));
			quest_index[idx].convo = convo;
		}
		else {
			while (convo) {
				if (convo->number == lv_nr)
					break;
				prev_convo = convo;
				convo = convo->next;
			}
			if (!convo) {
				convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
				bzero((char *) convo, sizeof(struct conversation_data));
				prev_convo->next = convo;
			}
		}

		/* Delete old keywords if they exist and attach new ones */
		if (convo->reply)
			free(convo->reply);
		convo->reply = 0;
		send_to_char("\r\nEnter a new quest reply.  Terminate with a '@'\r\n", ch);
		ch->desc->str = &(convo->reply);
		ch->desc->max_str = 2048;
		return;
	}

	send_to_char("Unknown command for mquestsay <mob_vnum> <convo_nr> <command>.\r\n", ch);
};

void qu2300_quest_info(struct char_data * ch, int vnum)
{
	int idx;
	struct conversation_data *convo;
	struct keyword_data *keys;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];

	for (idx = 0; idx <= top_of_questt; idx++) {
		if (quest_index[idx].vmob == vnum)
			break;
	}

	if (idx > top_of_questt) {
		sprintf(buf, "mob<%d> does not have any quest data.\r\n", vnum);
		send_to_char(buf, ch);
		return;
	}

	convo = quest_index[idx].convo;
	if (!convo) {
		sprintf(buf, "Quest data for mob<%d> is empty.\r\n", vnum);
		send_to_char(buf, ch);
		return;
	}

	while (convo) {
		bzero(buf2, sizeof(buf2));
		keys = convo->keywords;
		while (keys) {
			sprintf(buf, " %s", keys->keyword);
			strcat(buf2, buf);
			keys = keys->next;
		}
		strcpy(buf, convo->reply ? convo->reply : "(empty)");
		while ((*(buf + strlen(buf) - 1) == '\r') || (*(buf + strlen(buf) - 1) == '\n'))
			*(buf + strlen(buf) - 1) = '\0';
		sprintf(buf3, "&c%d&w)&W%s&Y\r\n", convo->number, buf2);
		send_to_char(buf3, ch);
		if (*buf) {
			sprintf(buf3, "%s&n\r\n", buf);
			send_to_char(buf3, ch);
		}
		if (convo->command) {
			sprintf(buf3, "&G%s&n\r\n", convo->command);
			send_to_char(buf3, ch);
		}
		convo = convo->next;
		send_to_char("\r\n", ch);
	};
};

void qu2400_convo_info(struct char_data * ch, int vnum, int lv_nr)
{
};

void qu2500_do_qsave(struct char_data * ch, char *arg, int cmd)
{
	char sys[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	int index, idx;
	FILE *theNewFile;
	struct conversation_data *convo;
	struct keyword_data *keys;

	if ((GET_LEVEL(ch) < IMO_IMM) && !ha1175_isexactname(GET_NAME(ch), "dutch")) {
		send_to_char("Huh?\r\n", ch);
		return;
	}

	zone = world[ch->in_room].zone;
	//CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (zone_table[zone].bot_of_mobt < 0) {
		sprintf(sys, "WARNING: Zone %s.mob no mobs to save", zone_table[zone].filename);
		main_log(sys);
		spec_log(sys, SYSTEM_LOG);
		return;
	}

	sprintf(sys, "areas/%s.qst", zone_table[zone].filename);
	if ((theNewFile = fopen(sys, "r+"))) {
		fclose(theNewFile);
		sprintf(sys, "mv areas/%s.qst areas.bak/%s.qst",
			zone_table[zone].filename,
			zone_table[zone].filename);
		system(sys);
	}

	sprintf(sys, "areas/%s.qst", zone_table[zone].filename);
	if (!(theNewFile = fopen(sys, "w+"))) {
		main_log("qu2500_do_qsave: could not open file for writing.");
		spec_log("qu2500_do_qsave: could not open file for writing.", ERROR_LOG);
		perror("fopen");
		exit(1);
	}

	for (index = zone_table[zone].bot_of_mobt; index <= zone_table[zone].top_of_mobt; index = mob_index[index].next) {
		for (idx = 0; idx <= top_of_questt; idx++)
			if (quest_index[idx].vmob == mob_index[index].virtual) {
				convo = quest_index[idx].convo;
				if (convo)
					fprintf(theNewFile, "#%d\n", quest_index[idx].vmob);
				while (convo) {
					bzero(buf2, sizeof(buf2));
					keys = convo->keywords;
					while (keys) {
						sprintf(buf, " %s", keys->keyword);
						strcat(buf2, buf);
						keys = keys->next;
					}
					fprintf(theNewFile, "%d %d %d\n%s~\n",
						convo->number,
						convo->low,
						convo->high,
						(buf2 + 1));

					strcpy(buf, convo->reply ? convo->reply : "(empty)");
					while ((*(buf + strlen(buf) - 1) == '\r') || (*(buf + strlen(buf) - 1) == '\n'))
						*(buf + strlen(buf) - 1) = '\0';
					fprintf(theNewFile, "%s~\n", buf);

					convo = convo->next;
					if (convo)
						fprintf(theNewFile, "~\n");
				}
			};
	}
	fprintf(theNewFile, "$~\n");
	fclose(theNewFile);
	send_to_char("Quest data saved.\r\n", ch);
};

void qu2600_do_mquestcmd(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int vnum;
	int lv_nr;
	int idx;
	int was_new = 0;
	struct conversation_data *convo, *prev_convo;

	if ((GET_LEVEL(ch) < IMO_IMM) && !ha1175_isexactname(GET_NAME(ch), "dutch")) {
		send_to_char("Huh?\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !(*buf)) {
		send_to_char("Missing arguments for mquestcmd <mob_vnum> <convo_nr> <command>.\r\n", ch);
		send_to_char("Use 'delete' to remove a command.\r\n", ch);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("first argument (mob vnum) has to be numeric.\r\n", ch);
		return;
	}

	vnum = atoi(buf);
	for (idx = 0; idx <= top_of_mobt; idx++) {
		if (mob_index[idx].virtual == vnum)
			break;
	}

	if (idx > top_of_mobt) {
		sprintf(buf, "Mob %d does not exist.\r\n", vnum);
		send_to_char(buf, ch);
		return;
	}

	for (; isspace(*arg); arg++);
	arg = one_argument(arg, buf);

	if (!buf || !*buf) {
		qu2300_quest_info(ch, vnum);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("second argument (convo_nr) has to be numeric.\r\n", ch);
		return;
	}

	lv_nr = atoi(buf);
	if ((lv_nr < 1) || (lv_nr > 99)) {
		send_to_char("second argument (convo_nr) has to be between 1 and 99.\r\n", ch);
		return;
	}

	for (; isspace(*arg); arg++);

	if (!*arg) {
		qu2400_convo_info(ch, vnum, lv_nr);
		return;
	}

	/* First check if there 's already quest info available */
	for (idx = 0; idx <= top_of_questt; idx++) {
		if (quest_index[idx].vmob == vnum)
			break;
	}

	/* alocate new structure if mob has no quest attached yet */
	if (idx > top_of_questt) {
		db2500_allocate_quest(idx);
		quest_index[idx].vmob = vnum;
		top_of_questt++;
	}

	/* check if convo exists, if not create a new one */
	convo = quest_index[idx].convo;
	if (!convo) {
		convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
		bzero((char *) convo, sizeof(struct conversation_data));
		quest_index[idx].convo = convo;
		was_new = TRUE;
	}
	else {
		while (convo) {
			if (convo->number == lv_nr)
				break;
			prev_convo = convo;
			convo = convo->next;
		}
		if (!convo) {
			convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
			bzero((char *) convo, sizeof(struct conversation_data));
			prev_convo->next = convo;
			was_new = TRUE;
		}
	};

	/* Delete old command if it exist and attach new one */
	if (convo->command)
		free(convo->command);
	if (was_new) {
		convo->number = lv_nr;
		convo->low = 1;
		convo->high = 99;
	}

	convo->command = strdup(arg);

	sprintf(buf, "Command for mob<%d> convo<%d> changed to '%s'.\r\n", vnum, lv_nr, arg);
	send_to_char(buf, ch);
};
