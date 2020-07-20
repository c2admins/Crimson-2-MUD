/* so */
/* gv_location: 16001-16500 */
/* *********************************************************************
*  file: social.c , Implementation of commands.        Part of DIKUMUD *
*  Usage : Social commands.                                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform.   *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "func.h"
#include "utility.h"
#include "constants.h"
#include "globals.h"


struct social_messg {

	int act_nr;
	int hide;
	int min_victim_position;/* Position of victim */

	/* No argument was supplied */
	char *char_no_arg;
	char *others_no_arg;

	/* An argument was there, and a victim was found */
	char *char_found;	/* if NULL, read no further, ignore args */
	char *others_found;
	char *vict_found;

	/* An argument was there, but no victim was found */
	char *not_found;

	/* The victim turned out to be the character */
	char *char_auto;
	char *others_auto;
} *soc_mess_list = 0;



struct pose_type {
	int level;		/* minimum level for poser */
	char *poser_msg[4];	/* message to poser        */
	char *room_msg[4];	/* message to room         */
} pose_messages[MAX_MESSAGES];

static int list_top = -1;


char *fread_action(FILE * fl)
{
	char buf[MAX_STRING_LENGTH], *rslt;

	for (;;) {
		fgets(buf, MAX_STRING_LENGTH, fl);
		if (feof(fl)) {
			perror("ERROR: FREAD EOF\r\n");
			sprintf(buf, "ERROR: FREAD action unexpeced EOF\r\n");
			ABORT_PROGRAM();
		}

		if (*buf == '#')
			return (0);
		*(buf + strlen(buf) - 1) = '\0';
		CREATE(rslt, char, strlen(buf) + 1);
		strcpy(rslt, buf);
		return (rslt);
	}/* END OF for(;;) */
}/* END OF fread_action() */


void boot_social_messages(void)
{

	FILE *fl;
	char temp_str[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	int tmp, hide, min_pos;

	if (!(fl = fopen(SOCMESS_FILE, "r"))) {
		perror("boot_social_messages");
		sprintf(buf, "ERROR: boot_social_message\r\n");
		ABORT_PROGRAM();
	}

	for (;;) {
		fscanf(fl, " %d ", &tmp);
		if (tmp < 0)
			break;
		fscanf(fl, " %d ", &hide);
		fscanf(fl, " %d \n", &min_pos);

		/* alloc a new cell */
		if (!soc_mess_list) {
			CREATE(soc_mess_list, struct social_messg, 1);
			list_top = 0;
		}
		else if (!(soc_mess_list = (struct social_messg *) realloc(soc_mess_list, sizeof(struct social_messg) * (++list_top + 1)))) {
			perror("boot_social_messages. realloc");
			sprintf(buf, "ERROR: boot social realloc\r\n");
			ABORT_PROGRAM();
		}

		soc_mess_list[list_top].char_no_arg = 0;
		soc_mess_list[list_top].others_no_arg = 0;
		soc_mess_list[list_top].char_found = 0;
		soc_mess_list[list_top].others_found = 0;
		soc_mess_list[list_top].vict_found = 0;
		soc_mess_list[list_top].not_found = 0;
		soc_mess_list[list_top].char_auto = 0;
		soc_mess_list[list_top].others_auto = 0;

		/* read the stuff */
		if (gv_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			sprintf(temp_str, "     Loading social: %d", tmp);
			main_log(temp_str);
		}

		soc_mess_list[list_top].act_nr = tmp;
		soc_mess_list[list_top].hide = hide;
		soc_mess_list[list_top].min_victim_position = min_pos;
		soc_mess_list[list_top].char_no_arg = fread_action(fl);

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].char_no_arg) {
				sprintf(temp_str, "        char no arg: %s\r\n", soc_mess_list[list_top].char_no_arg);
				main_log(temp_str);
			}
		}

		soc_mess_list[list_top].others_no_arg = fread_action(fl);

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].others_no_arg) {
				sprintf(temp_str, "        others no arg: %s\r\n", soc_mess_list[list_top].others_no_arg);
				main_log(temp_str);
			}
		}


		soc_mess_list[list_top].char_found = fread_action(fl);

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].char_found) {
				sprintf(temp_str, "        char found: %s\r\n", soc_mess_list[list_top].char_found);
				main_log(temp_str);
			}
		}


		/* if no char_found, the rest is to be ignored */
		if (!soc_mess_list[list_top].char_found)
			continue;

		soc_mess_list[list_top].others_found = fread_action(fl);
		if (!soc_mess_list[list_top].others_found)
			continue;

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].others_found) {
				sprintf(temp_str, "        others_found: %s\r\n", soc_mess_list[list_top].others_found);
				main_log(temp_str);
			}
		}

		soc_mess_list[list_top].vict_found = fread_action(fl);
		if (!soc_mess_list[list_top].vict_found)
			continue;
		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].vict_found) {
				sprintf(temp_str, "        vict found: %s\r\n", soc_mess_list[list_top].vict_found);
				main_log(temp_str);
			}
		}


		soc_mess_list[list_top].not_found = fread_action(fl);
		if (!soc_mess_list[list_top].not_found)
			continue;

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].not_found) {
				sprintf(temp_str, "        not found: %s\r\n", soc_mess_list[list_top].not_found);
				main_log(temp_str);
			}
		}


		soc_mess_list[list_top].char_auto = fread_action(fl);
		if (!soc_mess_list[list_top].char_auto)
			continue;

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].char_auto) {
				sprintf(temp_str, "        char auto: %s\r\n", soc_mess_list[list_top].char_auto);
				main_log(temp_str);
			}
		}


		soc_mess_list[list_top].others_auto = fread_action(fl);
		if (!soc_mess_list[list_top].others_auto)
			continue;

		if (gv_mega_verbose_messages == TRUE) {
			bzero(temp_str, sizeof(temp_str));
			if (soc_mess_list[list_top].others_auto) {
				sprintf(temp_str, "        others auto: %s\r\n", soc_mess_list[list_top].others_auto);
				main_log(temp_str);
			}
		}

	}
	fclose(fl);
}


int find_action(int cmd)
{
	int bot, top, mid;



	bot = 0;
	top = list_top;

	if (top < 0)
		return (-1);

	for (;;) {
		mid = (bot + top) / 2;

		if (soc_mess_list[mid].act_nr == cmd)
			return (mid);
		if (bot == top)
			return (-1);

		if (soc_mess_list[mid].act_nr > cmd)
			top = --mid;
		else
			bot = ++mid;
	}
}				/* END OF find_action() */


void do_action(struct char_data * ch, char *argument, int cmd)
{

	int act_nr;
	char buf[MAX_INPUT_LENGTH];
	struct social_messg *action;
	struct char_data *vict;



	act_nr = find_action(cmd);


	if (act_nr < 0) {
		send_to_char("That action is not supported.\n\r", ch);
		return;
	}

	action = &soc_mess_list[act_nr];

	if (action->char_found)
		one_argument(argument, buf);
	else
		*buf = '\0';


	/* IF MUZZLED AND ITS A SOUND TYPE SOCIAL, BLOCK IT */
	if (action->hide != TRUE) {
		MUZZLECHECK();
		SILENCE_CHECK();
	}

	if (!*buf) {
		send_to_char(action->char_no_arg, ch);
		send_to_char("\n\r", ch);
		act(action->others_no_arg, action->hide, ch, 0, 0, TO_ROOM);
		return;
	}

	vict = ha2125_get_char_in_room_vis(ch, buf);
	if (!vict) {
		act(action->not_found, action->hide, ch, 0, ch, TO_CHAR);
	}
	else if (vict == ch) {
		if (action->char_auto)
			act(action->char_auto, action->hide, ch, 0, vict, TO_CHAR);
		else
			act(action->char_no_arg, action->hide, ch, 0, vict, TO_CHAR);

		if (action->others_auto)
			act(action->others_auto, action->hide, ch, 0, vict, TO_ROOM);
		else
			act(action->others_no_arg, action->hide, ch, 0, vict, TO_ROOM);

	}
	else {
		if (GET_POS(vict) < action->min_victim_position) {
			act("$N is not in a proper position for that.", FALSE, ch, 0, vict, TO_CHAR);
		}
		else {
			act(action->char_found, 0, ch, 0, vict, TO_CHAR);

			act(action->others_found, action->hide, ch, 0, vict, TO_NOTVICT);

			if (!(strncmp(action->vict_found, "**FLOWERS**", 10))) {
				act("How romantic\r\n     (*)     \r\n *    |    * \r\n\\|/  \\|/  \\|/\r\n--------------\r\n$n sends you flowers", action->hide, ch, 0, vict, TO_VICT);
			}
			else {
				if (!(strncmp(action->vict_found, "**BEEP**", 8)))
					if (GET_LEVEL(ch) > 4) {
						act("$n beeps you \007", action->hide, ch, 0, vict, TO_VICT);
					}
					else {
						act("$n beeps you.", action->hide, ch, 0, vict, TO_VICT);
					}
				else {
					act(action->vict_found, action->hide, ch, 0, vict, TO_VICT);
				}

			}
		}
	}
}



void do_insult(struct char_data * ch, char *argument, int cmd)
{
	static char buf[100];
	static char arg[MAX_STRING_LENGTH];
	struct char_data *victim;



	one_argument(argument, arg);

	if (*arg) {
		if (!(victim = ha2125_get_char_in_room_vis(ch, arg))) {
			send_to_char("Can't hear you!\n\r", ch);
		}
		else {
			if (victim != ch) {
				sprintf(buf, "You insult %s.\n\r", GET_NAME(victim));
				send_to_char(buf, ch);

				switch (random() % 3) {
				case 0:{
						if (GET_SEX(ch) == SEX_MALE) {
							if (GET_SEX(victim) == SEX_MALE)
								act(
								    "$n accuses you of fighting like a woman!", FALSE,
								    ch, 0, victim, TO_VICT);
							else
								act("$n says that women can't fight.",
								    FALSE, ch, 0, victim, TO_VICT);
						}
						else {	/* Ch == Woman */
							if (GET_SEX(victim) == SEX_MALE)
								act("$n accuses you of having the smallest.... (brain?)",
								    FALSE, ch, 0, victim, TO_VICT);
							else
								act("$n tells you that you'd loose a beautycontest against a troll.",
								    FALSE, ch, 0, victim, TO_VICT);
						}
					} break;
				case 1:{
						act("$n calls your mother a bitch!",
						FALSE, ch, 0, victim, TO_VICT);
					} break;
				default:{
						act("$n tells you to get lost!", FALSE, ch, 0, victim, TO_VICT);
					} break;
				}	/* end switch */

				act("$n insults $N.", TRUE, ch, 0, victim, TO_NOTVICT);
			}
			else {	/* ch == victim */
				send_to_char("You feel insulted.\n\r", ch);
			}
		}
	}
	else
		send_to_char("Sure you don't want to insult everybody.\n\r", ch);
}				/* END OF do_insult() */


void boot_pose_messages(void)
{

	FILE *fl;
	signed char counter, class;
	char buf[MAX_STRING_LENGTH];



	if (!(fl = fopen(POSEMESS_FILE, "r"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: boot_pose_message\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	for (counter = 0; counter < MAX_MESSAGES; counter++) {
		pose_messages[counter].level = 0;
		pose_messages[counter].level = 0;
		for (class = 0; class < 4; class++) {
			pose_messages[counter].poser_msg[class] = 0;
			pose_messages[counter].room_msg[class] = 0;
		}		/* END OF class loop */
	}			/* END OF counter loop */

	for (counter = 0;; counter++) {
		fscanf(fl, " %d ", &pose_messages[counter].level);
		if (pose_messages[counter].level < 0)
			break;
		for (class = 0; class < 4; class++) {
			pose_messages[counter].poser_msg[class] = fread_action(fl);
			pose_messages[counter].room_msg[class] = fread_action(fl);
		}
	}

	fclose(fl);
}				/* END OF do_boot_poses() */

void do_pose(struct char_data * ch, char *argument, int cmd)
{

	signed char to_pose, to_class, counter;
	char buf[MAX_STRING_LENGTH];



	/* WE CAN HANDLE 4 CLASSES */
	/* TYPE 0 = MAGIC USER */
	/* TYPE 1 = CLERIC */
	/* TYPE 2 = THIEF/ROGUE */
	/* TYPE 3 = FIGHTER */
	switch (GET_CLASS(ch)) {
	case CLASS_MAGIC_USER:{
			to_class = 0;
			break;
		}
	case CLASS_CLERIC:{
			to_class = 1;
			break;
		}
	case CLASS_THIEF:{
			to_class = 2;
			break;
		}
	case CLASS_WARRIOR:{
			to_class = 3;
			break;
		}
	case CLASS_BARD:{
			to_class = number(2, 3);
			break;
		}
	case CLASS_PRIEST:{
			to_class = number(0, 2);
			if (to_class == 2)
				to_class++;	/* SKIP THIEF CLASS */
			break;
		}
	default:{
			to_class = 3;	/* DEFAULT TO WARRIOR */
			break;
		}
	}			/* END OF switch() */

	/* VALIDATE */
	if (to_class < 0 || to_class > 3) {
		bzero(buf, sizeof(buf));
		send_to_char("Hrmmm somehow to_class is bad.\r\n", ch);
		return;
	}

	if ((GET_LEVEL(ch) <= pose_messages[0].level)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You must be level %d to perform poses.\r\n",
			pose_messages[0].level + 1);
		send_to_char(buf, ch);
		return;
	}

	/* SET COUNTER FROM 0 TO HIGHEST POSE LEVEL ALLOWED */
	for (counter = 0;
	     (pose_messages[counter].level < GET_LEVEL(ch)) &&
	     (pose_messages[counter].level > 0); counter++);
	counter--;

	if (counter < 0) {
		send_to_char("Hrmmmm, I just can't find a pose for you.\r\n", ch);
		return;
	}

	to_pose = number(0, counter);

	act(pose_messages[to_pose].poser_msg[to_class], 0, ch, 0, 0, TO_CHAR);
	act(pose_messages[to_pose].room_msg[to_class], 0, ch, 0, 0, TO_ROOM);
}				/* END OF do_pose */
