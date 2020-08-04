/* ma */
/* *******************************************************************
*  file: mail.c , mail module.           Part of Crimson MUD         *
*  Usage: Procedures handling Crimson II Mud Mail                    *
*                     Written by Hercules                            *
*                 Modified by Relic (May 2003)                       *
******************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

#define MAIL_MSG_NOT_FOUND      0
#define MAIL_NO_CFG     	1
#define MAIL_NO_MSG		2
#define MAIL_OK			3

#define MAIL_STRING_LENGTH	80

#define GROUP_MORTALS -2
#define GROUP_AVATS   -3
#define GROUP_SPIRITS -4
#define GROUP_IMMS    -5
#define GROUP_BOSS   -6
#define GROUP_CLAN    -7

struct msg_data {
	int real_nr;
	bool newMessage;
	long post_time;
	char name[20];
	char topic[160];
};

void ma1300_write_msg(struct char_data * ch, int target, int clanno, char *title);

void ma1350_handle_mail_input(struct descriptor_data * point, char *comm);

void ma1000_do_mail(struct char_data * ch, char *arg, int cmd)
{

	//char testbuf[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	int lv_choice, lv_target, lv_clan = 0, lv_msg_nr;
	const char *lv_mail_cmds[] = {
		"read",		/* 1 */
		"write",	/* 2 */
		"remove",	/* 3 */
		"list",		/* 4 */
		"clear",	/* 5 */
		"\n",
	};

#define MAIL_READ	1
#define MAIL_WRITE	2
#define MAIL_REMOVE	3
#define MAIL_LIST	4
#define MAIL_CLEAR	5

	/* Strip first argument */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);
	lv_choice = 0;
	if (*buf)
		lv_choice = old_search_block(buf, 0, strlen(buf), lv_mail_cmds, 0);

	if (lv_choice < 1) {
		send_to_char("None or invalid argument passed to set.\r\n", ch);
		send_to_char("Valid options are:\r\n\r\n", ch);
		send_to_char(" read    <msg>\r\n", ch);
		send_to_char(" write   <name> <topic>\r\n", ch);
		send_to_char(" write   <group> <topic>\r\n", ch);
		if (GET_LEVEL(ch) > IMO_SPIRIT)
			send_to_char(" where group can be: mortals, avatars, imms, boss,\r\n or clan, plus a space and a clan number \r\n (mail write clan 5 <topic> for example).\r\n", ch);
		else
			send_to_char(" where group can be clan (if your a member of a clan) or imms.\r\n", ch);
		send_to_char(" remove  <msg>\r\n", ch);
		send_to_char(" list\r\n", ch);
		send_to_char(" clear\r\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	strcpy(buf, arg);

	for (; isspace(*arg); arg++);
	bzero(buf2, sizeof(buf2));
	arg = one_argument(arg, buf2);

	switch (lv_choice) {
	case MAIL_READ:
	case MAIL_REMOVE:{
			if (!*buf2) {
				send_to_char("Need a <msg> number for this command!\r\n", ch);
				return;
			}
			if (!is_number(buf2)) {
				send_to_char("<msg> number must be numeric.\r\n", ch);
				return;
			}
			lv_msg_nr = MAXV(1, atoi(buf2));
			break;
		}
	case MAIL_WRITE:{
			if (GET_LEVEL(ch) < 10) {
				send_to_char("You need to be level 10 to write mail.\r\n", ch);
				return;
			}
			if (!*buf2) {
				send_to_char("Need a <name> or <group> for this command.\r\n", ch);
				return;
			}
			if (!*arg) {
				send_to_char("Need a <topic> for this command.\r\n", ch);
				return;
			}

			if (is_abbrev(buf2, "imms")) {
				lv_target = GROUP_IMMS;
			}

			else if (is_abbrev(buf2, "mortals") && GET_LEVEL(ch) > IMO_SPIRIT) {
				lv_target = GROUP_MORTALS;
			}

			else if (is_abbrev(buf2, "avatars") && GET_LEVEL(ch) > IMO_SPIRIT) {
				lv_target = GROUP_AVATS;
			}

			else if (is_abbrev(buf2, "spirits") && GET_LEVEL(ch) >= IMO_SPIRIT) {
				lv_target = GROUP_SPIRITS;
			}

			else if (is_abbrev(buf2, "boss") && GET_LEVEL(ch) > IMO_SPIRIT) {
				lv_target = GROUP_BOSS;
			}

			else if (is_abbrev(buf2, "clan")) {
				lv_target = GROUP_CLAN;
				if (GET_LEVEL(ch) > IMO_SPIRIT) {
					arg = one_argument(arg, buf3);
					if (!is_number(buf3)) {
						send_to_char("Sorry, need a clan number.\r\n", ch);
						return;
					}

					lv_clan = atoi(buf3);

					if ((!CLAN_EXISTS(lv_clan))) {
						sprintf(buf, "Clan Number %d doesn't exist!\r\n", lv_clan);
						return;
					}
				}
				else {
					if (CLAN_RANK(ch) == CLAN_APPLY_RANK) {
						send_to_char("They have to ACCEPT you first!\r\n", ch);
						return;
					}
					if (!CLAN_EXISTS(CLAN_NUMBER(ch))) {
						send_to_char("But you aren't a memeber of any clan...\r\n", ch);
						return;
					}
					lv_clan = CLAN_NUMBER(ch);
				}
			}
			else {
				lv_target = pa2050_find_name_approx(buf2);
				if (lv_target < 0) {
					send_to_char("Unkown player.\r\n", ch);
					return;
				}
				if (player_table[lv_target].pidx_level < 10) {
					send_to_char("You can only mail players that are lvl 10+.\r\n", ch);
					return;
				}
			}
			break;
		}
	}

	switch (lv_choice) {
	case MAIL_LIST:
		ma1200_show_msg_list(ch);
		break;
	case MAIL_WRITE:
		ma1300_write_msg(ch, lv_target, lv_clan, arg);
		break;
	case MAIL_READ:
		ma1400_read_msg(ch, lv_msg_nr);
		break;
	case MAIL_REMOVE:
		ma1500_remove_msg(ch, lv_msg_nr);
		break;
	case MAIL_CLEAR:
		ma1600_clear_msg(ch);
		break;

	default:{
			send_to_char("Unkown command.\r\n", ch);
			return;
		}
	}
	return;
}				/* end of ma1000_do_mail() */

bool ma1100_check_for_msg(struct char_data * ch, int msg_nr)
{

	return (FALSE);
}				/* end of ma1100_check_for_msg() */


/* Odin - This code checks if there are any new messages */

int new_message(struct char_data * ch)
{

	FILE *file_in;
	char buf[MAX_STRING_LENGTH];
	char mail_config[MAX_STRING_LENGTH];
	char p_name[80];
	int lv_msg_count;
	struct msg_data msg_info;
	int new_msg_count = 0;

	strcpy(p_name, player_table[ch->nr].pidx_name);
	sprintf(buf, "rentfiles/%c/%s.mail", LOWER(p_name[0]), p_name);
	sprintf(mail_config, "%s.cfg", buf);
	file_in = fopen(mail_config, "rb");

	if (!file_in) {
		return (0);
	}

	lv_msg_count = 0;

	fread(&msg_info, sizeof(msg_info), 1, file_in);

	while (!feof(file_in)) {
		if (msg_info.newMessage == TRUE)
			new_msg_count++;
		fread(&msg_info, sizeof(msg_info), 1, file_in);
	}

	fclose(file_in);

	return (new_msg_count);
}


void make_old_message(struct char_data * ch, int msg_number)
{

	FILE *file_in;
	char buf[MAX_STRING_LENGTH];
	char mail_config[MAX_STRING_LENGTH];
	char p_name[20];
	struct msg_data msg_info;

	strcpy(p_name, player_table[ch->nr].pidx_name);
	sprintf(buf, "rentfiles/%c/%s.mail", LOWER(p_name[0]), p_name);
	sprintf(mail_config, "%s.cfg", buf);
	file_in = fopen(mail_config, "r+");

	if (!file_in) {
		return;
	}

	fseek(file_in, (msg_number - 1) * sizeof(msg_info), SEEK_SET);
	fread(&msg_info, sizeof(msg_info), 1, file_in);

	msg_info.newMessage = FALSE;

	fseek(file_in, (msg_number - 1) * sizeof(msg_info), SEEK_SET);
	fwrite(&msg_info, sizeof(msg_info), 1, file_in);

	fclose(file_in);
}

void ma1200_show_msg_list(struct char_data * ch)
{

	FILE *file_in;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char mail_config[MAX_STRING_LENGTH];
	char p_name[20];
	int lv_msg_count;
	struct msg_data msg_info;
	char *tmstr;

	strcpy(p_name, player_table[ch->nr].pidx_name);
	sprintf(buf, "rentfiles/%c/%s.mail", LOWER(p_name[0]), p_name);
	sprintf(mail_config, "%s.cfg", buf);
	file_in = fopen(mail_config, "rb");

	if (!file_in) {
		send_to_char("Awww, You have never had any messages yet.\r\n", ch);
		return;
	}

	lv_msg_count = 0;
	co2900_send_to_char(ch, "%s", "Nr    Date       Sender                Topic\r\n-----------------------------------------------------------------\r\n");
	fread(&msg_info, sizeof(msg_info), 1, file_in);
	while (!feof(file_in)) {
		lv_msg_count++;
		tmstr = (char *) asctime(localtime(&msg_info.post_time));
		*(tmstr + strlen(tmstr) - 1) = '\0';

		if (msg_info.newMessage == TRUE)
			send_to_char("&GN", ch);
		else
			send_to_char(" ", ch);

		sprintf(buf2, "&w(&r%s&w)", msg_info.name);
		co2900_send_to_char(ch, "&W%3d&w) &g%6.10s %-26s &c%s&n\r\n",
				    lv_msg_count,
				    tmstr,
				    buf2,
				    msg_info.topic);
		fread(&msg_info, sizeof(msg_info), 1, file_in);
	}
	co2900_send_to_char(ch, "\r\n");
	fclose(file_in);
	return;
}				/* end of ma1200_show_msg_list */

void ma1300_write_msg(struct char_data * ch, int target, int clanno, char *title)
{

	FILE *file_in;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], mail_config[MAX_STRING_LENGTH], msg_name[MAX_STRING_LENGTH],
	  msg_header[MAX_STRING_LENGTH];
	//char debug[MAX_STRING_LENGTH];
	char p_name[20];
	struct msg_data msg_info, msg_info_holder;
	int real_msg_nr;
	int i;

	if (target < 0) {
		title[150] = '\0';
		ch->desc->mailedit = 1;
		ch->desc->mailtoclan = clanno;
		send_to_char("Enter your message, and terminate with a @:\r\n\r\n", ch);

		msg_info.post_time = time(0);
		msg_info.newMessage = TRUE;
		strcpy(msg_info.topic, title);
		strcpy(msg_info.name, GET_REAL_NAME(ch));

		if (target == GROUP_MORTALS) {
			for (i = 0; i <= top_of_p_table; i++) {
				//sprintf(debug, "loop %d", i);
				//send_to_char(debug, ch);
				if (player_table[i].pidx_level > 9 &&
				    player_table[i].pidx_level < IMO_LEV &&
				    player_table[i].pidx_name[0] != '1' &&
				    player_table[i].pidx_name[0] != '2') {
					bzero(mail_config, sizeof(mail_config));
					sprintf(buf, "rentfiles/%c/%s.mail",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);
					sprintf(mail_config, "%s.cfg", buf);
					//sprintf(debug, "Character: %s, ", player_table[i].pidx_name);
					//send_to_char(debug, ch);
					real_msg_nr = 1;
					file_in = fopen(mail_config, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(msg_info_holder), SEEK_END)) {
							fread(&msg_info_holder, sizeof(msg_info_holder), 1, file_in);
							real_msg_nr = msg_info_holder.real_nr + 1;
							fclose(file_in);
						}
					}
					msg_info.real_nr = real_msg_nr;
					file_in = fopen(mail_config, "a");
					fwrite(&msg_info, sizeof(msg_info), 1, file_in);
					fclose(file_in);

					sprintf(msg_name, "%s.%d", buf, msg_info.real_nr);
					char temp[25];
					sprintf(temp, "(%s)", msg_info.name);
					sprintf(msg_header, "%3d %20s %s\r\n\r\n", msg_info.real_nr,
						temp, msg_info.topic);

					file_in = fopen(msg_name, "w");
					fputs(msg_header, file_in);
					fclose(file_in);
				}
			}
			ch->desc->mailedit = 1;
			ch->desc->mailto = target;
			ch->desc->mailtoclan = clanno;
			return;
		}

		if (target < -2 && target > -7) {
			int intended_level = 0;
			switch (target) {
			case GROUP_AVATS:
				intended_level = IMO_LEV;
				break;
			case GROUP_SPIRITS:
				intended_level = IMO_SPIRIT;
				break;
			case GROUP_IMMS:
				intended_level = IMO_IMM;
				break;
			case GROUP_ADMIN:
				intended_level = IMO_IMP;
				break;
			default:
				break;
			}

			for (i = 0; i <= top_of_p_table; i++) {
				if (player_table[i].pidx_level > 9 &&
				player_table[i].pidx_level == intended_level &&
				    player_table[i].pidx_name[0] != '1' &&
				    player_table[i].pidx_name[0] != '2') {
					sprintf(buf, "rentfiles/%c/%s.mail",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);
					sprintf(mail_config, "%s.cfg", buf);
					//sprintf(debug, "Character: %s, ", player_table[i].pidx_name);
					//send_to_char(debug, ch);
					real_msg_nr = 1;
					file_in = fopen(mail_config, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(msg_info_holder), SEEK_END)) {
							fread(&msg_info_holder, sizeof(msg_info_holder), 1, file_in);
							real_msg_nr = msg_info_holder.real_nr + 1;
							fclose(file_in);
						}
					}
					msg_info.real_nr = real_msg_nr;
					file_in = fopen(mail_config, "a");
					fwrite(&msg_info, sizeof(msg_info), 1, file_in);
					fclose(file_in);

					sprintf(msg_name, "%s.%d", buf, msg_info.real_nr);
					char temp[25];
					sprintf(temp, "(%s)", msg_info.name);
					sprintf(msg_header, "%3d %20s %s\r\n\r\n", msg_info.real_nr,
						temp, msg_info.topic);

					file_in = fopen(msg_name, "w");
					fputs(msg_header, file_in);
					fclose(file_in);
				}
			}
			ch->desc->mailedit = 1;
			ch->desc->mailto = target;
			ch->desc->mailtoclan = clanno;
			return;
		}

		if (target == GROUP_CLAN) {
			for (i = 0; i <= top_of_p_table; i++) {
				if (player_table[i].pidx_level > 9 &&
				 player_table[i].pidx_clan_number == clanno &&
				    player_table[i].pidx_clan_rank != CLAN_APPLY_RANK &&
				    player_table[i].pidx_name[0] != '1' &&
				    player_table[i].pidx_name[0] != '2') {
					sprintf(buf, "rentfiles/%c/%s.mail",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);
					sprintf(mail_config, "%s.cfg", buf);
					//sprintf(debug, "Character: %s\r\n", player_table[i].pidx_name);
					//send_to_char(debug, ch);
					real_msg_nr = 1;
					file_in = fopen(mail_config, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(msg_info_holder), SEEK_END)) {
							fread(&msg_info_holder, sizeof(msg_info_holder), 1, file_in);
							real_msg_nr = msg_info_holder.real_nr + 1;
							fclose(file_in);
						}
					}
					msg_info.real_nr = real_msg_nr;
					file_in = fopen(mail_config, "a");
					if (file_in) {
						fwrite(&msg_info, sizeof(msg_info), 1, file_in);
						fclose(file_in);
					}

					sprintf(msg_name, "%s.%d", buf, msg_info.real_nr);
					char temp[25];
					sprintf(temp, "(%s)", msg_info.name);
					sprintf(msg_header, "%3d %20s %s\r\n\r\n", msg_info.real_nr,
						temp, msg_info.topic);

					file_in = fopen(msg_name, "w");
					if (file_in) {
						fputs(msg_header, file_in);
						fclose(file_in);
					}
				}
			}
			ch->desc->mailedit = 1;
			ch->desc->mailto = target;
			ch->desc->mailtoclan = clanno;
			return;
		}
	}
	strcpy(p_name, player_table[target].pidx_name);
	sprintf(buf, "rentfiles/%c/%s.mail", LOWER(p_name[0]), p_name);
	sprintf(mail_config, "%s.cfg", buf);

	real_msg_nr = 1;
	file_in = fopen(mail_config, "r");
	if (file_in) {
		if (!fseek(file_in, -sizeof(msg_info), SEEK_END)) {
			fread(&msg_info, sizeof(msg_info), 1, file_in);
			real_msg_nr = msg_info.real_nr + 1;
			fclose(file_in);
		}
	}

	title[150] = '\0';
	strcpy(msg_info.topic, title);
	strcpy(msg_info.name, GET_REAL_NAME(ch));
	msg_info.real_nr = real_msg_nr;
	msg_info.post_time = time(0);
	msg_info.newMessage = TRUE;

	file_in = fopen(mail_config, "a");
	fwrite(&msg_info, sizeof(msg_info), 1, file_in);
	fclose(file_in);

	sprintf(msg_name, "%s.%d", buf, msg_info.real_nr);
	sprintf(buf2, "(%s)", msg_info.name);
	sprintf(buf, "%3d %20s %s\r\n\r\n",
		msg_info.real_nr,
		buf2,
		msg_info.topic);
	file_in = fopen(msg_name, "w");
	fputs(buf, file_in);
	fclose(file_in);
	ch->desc->mailedit = msg_info.real_nr;
	ch->desc->mailto = target;
	send_to_char("Write your message.  Terminate with a @.\r\n\r\n", ch);

	return;
}				/* end of ma1300_write_msg */

void ma1350_handle_mail_input(struct descriptor_data * point, char *comm)
{
	char p_name[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	FILE *file_in;

	if (*comm == '@') {
		point->mailedit = 0;
		point->mailto = -1;
		point->mailtoclan = 0;
	}
	else if (point->mailto > -1) {
		strcpy(p_name, player_table[point->mailto].pidx_name);
		sprintf(buf, "rentfiles/%c/%s.mail.%d", LOWER(p_name[0]), p_name, point->mailedit);
		file_in = fopen(buf, "a+");
		if (file_in) {
			sprintf(buf, "%s\r\n", comm);
			fputs(buf, file_in);
			fclose(file_in);
		}
	}
	else {			/* point->mailto < -1 (writting to a group) */
		/* First, set up some local variable that will make things
		 * easier. */
		char mail_file_name[MAX_STRING_LENGTH], mail_cfg_file_name[MAX_STRING_LENGTH];
		int i, message_nr;
		struct msg_data {
			int real_nr;
			bool newMessage;
			long post_time;
			char name[20];
			char topic[160];
		};
		struct msg_data multi_mail_info;

		/* Let's get some of the loop independant stuff ready to go,
		 * first, then loop through all player_table.... YUCK...Oh well */

		sprintf(buf, "%s\r\n", comm);


		if (point->mailto == GROUP_MORTALS) {
			for (i = 0; i < top_of_p_table; i++) {
				if (player_table[i].pidx_level > 9 &&
				    player_table[i].pidx_level < IMO_LEV &&
				    player_table[i].pidx_name[0] != '1' &&
				    player_table[i].pidx_name[0] != '2') {

					message_nr = 1;
					sprintf(mail_cfg_file_name, "rentfiles/%c/%s.mail.cfg",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);

					file_in = fopen(mail_cfg_file_name, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(multi_mail_info), SEEK_END)) {
							fread(&multi_mail_info, sizeof(multi_mail_info), 1, file_in);
							message_nr = multi_mail_info.real_nr;
							fclose(file_in);
						}
					}

					sprintf(mail_file_name, "rentfiles/%c/%s.mail.%d",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name,
						message_nr);

					file_in = fopen(mail_file_name, "a");
					if (file_in) {
						fputs(buf, file_in);
						fclose(file_in);
					}
				}
			}
		}
		if (point->mailto < -2 && point->mailto > -7) {
			int intended_level = 0;
			switch (point->mailto) {
			case GROUP_AVATS:
				intended_level = IMO_LEV;
				break;
			case GROUP_SPIRITS:
				intended_level = IMO_SPIRIT;
				break;
			case GROUP_IMMS:
				intended_level = IMO_IMM;
				break;
			case GROUP_ADMIN:
				intended_level = IMO_IMP;
				break;
			default:
				intended_level = 0;
				break;
			}

			for (i = 0; i < top_of_p_table; i++) {
				if (player_table[i].pidx_level == intended_level &&
				    player_table[i].pidx_name[0] != '1' && player_table[i].pidx_name[0] != '2') {
					message_nr = 1;

					sprintf(mail_cfg_file_name, "rentfiles/%c/%s.mail.cfg",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);

					file_in = fopen(mail_cfg_file_name, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(multi_mail_info), SEEK_END)) {
							fread(&multi_mail_info, sizeof(multi_mail_info), 1, file_in);
							message_nr = multi_mail_info.real_nr;
							fclose(file_in);
						}
					}

					sprintf(mail_file_name, "rentfiles/%c/%s.mail.%d",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name,
						message_nr);

					file_in = fopen(mail_file_name, "a");
					if (file_in) {
						fputs(buf, file_in);
						fclose(file_in);
					}
				}
			}
		}
		if (point->mailto == GROUP_CLAN) {
			for (i = 0; i < top_of_p_table; i++) {
				if (player_table[i].pidx_level > 9 &&
				    player_table[i].pidx_clan_number == point->mailtoclan &&
				    player_table[i].pidx_clan_rank != CLAN_APPLY_RANK &&
				    player_table[i].pidx_name[0] != '1' &&
				    player_table[i].pidx_name[0] != '2') {
					message_nr = 1;
					sprintf(mail_cfg_file_name, "rentfiles/%c/%s.mail.cfg",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name);

					file_in = fopen(mail_cfg_file_name, "r");
					if (file_in) {
						if (!fseek(file_in, -sizeof(multi_mail_info), SEEK_END)) {
							fread(&multi_mail_info, sizeof(multi_mail_info), 1, file_in);
							message_nr = multi_mail_info.real_nr;
							fclose(file_in);
						}
					}

					sprintf(mail_file_name, "rentfiles/%c/%s.mail.%d",
					  LOWER(player_table[i].pidx_name[0]),
						player_table[i].pidx_name,
						message_nr);

					file_in = fopen(mail_file_name, "a");
					if (file_in) {
						fputs(buf, file_in);
						fclose(file_in);
					}
				}
			}
		}
	}
}



void ma1400_read_msg(struct char_data * ch, int msg_nr)
{

	char buf[MAX_STRING_LENGTH];
	char mail_line[MAIL_STRING_LENGTH];

	FILE *file_in;
	bzero(buf, sizeof(buf));
	switch (ma1700_get_msg_rname(ch, msg_nr, buf)) {

	case MAIL_NO_CFG:{
			send_to_char("How do you want to read something you never had?\r\n", ch);
			return;
		}
	case MAIL_MSG_NOT_FOUND:{
			send_to_char("This message does not exist.\r\n", ch);
			return;
		}
	case MAIL_OK:{
			file_in = fopen(buf, "rb");
			if (!file_in) {
				send_to_char("Error: Message is empty, removing message from list", ch);
				ma1500_remove_msg(ch, msg_nr);
				return;
			}
			while (!feof(file_in)) {
				//Odin
					make_old_message(ch, msg_nr);
				bzero(mail_line, sizeof(mail_line));
				fgets(mail_line, sizeof(mail_line), file_in);

				if (!(!(mail_line) || !(*mail_line)))
					co2900_send_to_char(ch, "%s", mail_line);
			}
			fclose(file_in);
			break;
		}
	default:{
			send_to_char("Unknown error reading message, please report.\r\n", ch);
			break;
		}
	}
	return;
}				/* end of ma1400_read_msg */

void ma1500_remove_msg(struct char_data * ch, int msg_nr)
{

	char buf[MAX_STRING_LENGTH], tfile[MAX_STRING_LENGTH];
	char p_name[20];
	FILE *file_in, *file_out;
	struct msg_data msg_info;
	int lv_msg_count;
	int lv_removed = 0;

	bzero(buf, sizeof(buf));
	switch (ma1700_get_msg_rname(ch, msg_nr, buf)) {
	case MAIL_NO_CFG:{
			send_to_char("How do you want to remove something you never had?\r\n", ch);
			return;
		}
	case MAIL_MSG_NOT_FOUND:{
			send_to_char("This message does not exist.\r\n", ch);
			return;
		}
	case MAIL_OK:{
			remove(buf);
			strcpy(p_name, player_table[ch->nr].pidx_name);
			sprintf(buf, "rentfiles/%c/%s.mail.cfg", LOWER(p_name[0]), p_name);
			sprintf(tfile, "%s.temp", buf);
			remove(tfile);
			rename(buf, tfile);
			file_in = fopen(tfile, "rb");
			file_out = fopen(buf, "wb");
			lv_msg_count = 0;
			fread(&msg_info, sizeof(msg_info), 1, file_in);
			while (!feof(file_in)) {
				lv_msg_count++;
				if (lv_msg_count != msg_nr) {
					fwrite(&msg_info, sizeof(msg_info), 1, file_out);
				}
				else {
					co2900_send_to_char(ch, "Removed message %d. [%s] %s\r\n", msg_nr, msg_info.name, msg_info.topic);
					lv_removed = 1;
				}
				fread(&msg_info, sizeof(msg_info), 1, file_in);
			}
			fclose(file_in);
			fclose(file_out);
			remove(tfile);
			if (!lv_removed) {
				sprintf(buf, "Message %d not found.\r\n", msg_nr);
				send_to_char(buf, ch);
			}
		}
	}
	return;
}				/* end of ma1500_remove_msg() */

void ma1600_clear_msg(struct char_data * ch)
{



	return;
}				/* end of ma1600_clear_msg() */



int ma1700_get_msg_rname(struct char_data * ch, int msg_nr, char *rname)
{

	FILE *file_in;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char mail_config[MAX_STRING_LENGTH];
	char p_name[20];
	struct msg_data msg_info;
	int real_msg_nr;

	strcpy(p_name, player_table[ch->nr].pidx_name);
	sprintf(buf, "rentfiles/%c/%s.mail", LOWER(p_name[0]), p_name);
	sprintf(mail_config, "%s.cfg", buf);
	file_in = fopen(mail_config, "rb+");

	/* strcpy (p_name,player_table[ch->nr].pidx_name);
	 * sprintf(buf,"rentfiles/%c/%s.mail.cfg",LOWER(p_name[0]),p_name);
	 * sprintf (tfile,"%s.temp", buf); remove (tfile); rename (buf, tfile);
	 * file_out = fopen ( buf, "wb" ); */
	if (!file_in)
		return MAIL_NO_CFG;

	if (!msg_nr) {
		fclose(file_in);
		return MAIL_MSG_NOT_FOUND;
	}

	real_msg_nr = 0;
	msg_info.real_nr = 0;
	if (!fseek(file_in, sizeof(msg_info) * (msg_nr - 1), SEEK_SET)) {
		fread(&msg_info, sizeof(msg_info), 1, file_in);
		real_msg_nr = msg_info.real_nr;
	}

	//msg_info.newMessage = FALSE;
	//fwrite(&msg_info, msg_nr * sizeof(msg_info), 1, file_in);

	fclose(file_in);
	//fclose(file_out);
	if (!real_msg_nr)
		return MAIL_MSG_NOT_FOUND;
	sprintf(buf2, "%s.%d", buf, real_msg_nr);
	strcpy(rname, buf2);

	return MAIL_OK;
}
