/* wz */
/* gv_location: 21501-22000 */
/* *********************************************************************
*  file: wiz2.c , Implementation of commands.       Part of DIKUMUD   *
*  Usage : Additional commands.                                       *
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

int get_jail_time(long lv_end_jail, int lv_type)
{

	int lv_months, lv_days, lv_hours, lv_minutes;
	long lv_seconds_togo;

	lv_months = 0;
	lv_days = 0;
	lv_hours = 0;
	lv_minutes = 0;
	lv_seconds_togo = lv_end_jail - time(0);

	if (lv_seconds_togo < 0)
		return 0;

	/* MONTHS */
	if (lv_seconds_togo > 2592000)
		lv_months = (int) (lv_seconds_togo / 2592000);
	lv_seconds_togo = lv_seconds_togo - (lv_months * 2592000);

	/* DAYS */
	if (lv_seconds_togo > 86400)
		lv_days = (int) (lv_seconds_togo / 86400);
	lv_seconds_togo = lv_seconds_togo - (lv_days * 86400);

	/* HOURS */
	if (lv_seconds_togo > 3600)
		lv_hours = (int) (lv_seconds_togo / 3600);
	lv_seconds_togo = lv_seconds_togo - (lv_hours * 3600);

	/* MINUTES */
	if (lv_seconds_togo > 60)
		lv_minutes = (int) (lv_seconds_togo / 60);
	lv_seconds_togo = lv_seconds_togo - (lv_minutes * 60);

	switch (lv_type) {
	case 0:
		return lv_months;
	case 1:
		return lv_days;
	case 2:
		return lv_hours;
	case 3:
		return lv_minutes;
	case 4:
		return lv_seconds_togo;
	default:
		return 0;
	}
	return 0;
}				/* end of get_jail_time(); */



int get_hit_time(long lv_end_jail, int lv_type)
{

	int lv_months, lv_days, lv_hours, lv_minutes;
	long lv_seconds_togo;

	lv_months = 0;
	lv_days = 0;
	lv_hours = 0;
	lv_minutes = 0;
	lv_seconds_togo = time(0) - lv_end_jail;

	if (lv_seconds_togo < 0)
		return 0;

	/* MONTHS */
	if (lv_seconds_togo > 2592000)
		lv_months = (int) (lv_seconds_togo / 2592000);
	lv_seconds_togo = lv_seconds_togo - (lv_months * 2592000);

	/* DAYS */
	if (lv_seconds_togo > 86400)
		lv_days = (int) (lv_seconds_togo / 86400);
	lv_seconds_togo = lv_seconds_togo - (lv_days * 86400);

	/* HOURS */
	if (lv_seconds_togo > 3600)
		lv_hours = (int) (lv_seconds_togo / 3600);
	lv_seconds_togo = lv_seconds_togo - (lv_hours * 3600);

	/* MINUTES */
	if (lv_seconds_togo > 60)
		lv_minutes = (int) (lv_seconds_togo / 60);
	lv_seconds_togo = lv_seconds_togo - (lv_minutes * 60);

	switch (lv_type) {
	case 0:
		return lv_months;
	case 1:
		return lv_days;
	case 2:
		return lv_hours;
	case 3:
		return lv_minutes;
	case 4:
		return lv_seconds_togo;
	default:
		return 0;
	}
	return 0;
}				/* end of get_hit_time(); */

void perform_jail_penalty(struct char_data * ch, struct char_data * mob)
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "Yup, you're in deep shit, %s!", GET_NAME(ch));

	send_to_char(buf, ch);
}

void wz1000_do_jail(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct char_data *vict, *mob;
	int lv_jail, jail_days, jail_hours;
	const char *lv_jail_def[] = {
		"lock",
		"free",
		"self",
		"\n",
	};

#define JAIL_LOCK  1
#define JAIL_FREE  2
#define JAIL_SELF  3



	/* STRIP OUT PLAYER */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("Usage:\r\n\r\n", ch);
		send_to_char("Lock                  - JAIL <name> lock <days> <hours>\r\n", ch);
		send_to_char("Release               - JAIL <name> free\r\n", ch);
		send_to_char("See Jail Time         - JAIL <name>\r\n", ch);
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

	/* STRIP OUT TYPE */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*buf) {
		if (IS_SET(GET_ACT2(vict), PLR2_JAILED)) {
			act("$N's remaining jailtime is:", FALSE, ch, 0, vict, TO_CHAR);
			sprintf(buf2, "%d months %d days %d hours %d minutes %d seconds.\r\n",
				get_jail_time(GET_JAILTIME(vict), 0),
				get_jail_time(GET_JAILTIME(vict), 1),
				get_jail_time(GET_JAILTIME(vict), 2),
				get_jail_time(GET_JAILTIME(vict), 3),
				get_jail_time(GET_JAILTIME(vict), 4));
			send_to_char(buf2, ch);
			return;
		}
		else {
			send_to_char("This person isn't Jailed!\r\n", ch);
			return;
		}
	}

	lv_jail = old_search_block(buf, 0, strlen(buf), lv_jail_def, 0);
	if (lv_jail < 1) {
		sprintf(buf2, "Sorry, but jail type: %s is invalid.\r\n", buf);
		send_to_char(buf2, ch);
		send_to_char("type 'JAIL' for to see available types.\r\n", ch);
		return;
	}

	/* if ((lv_jail != JAIL_SELF) && (GET_LEVEL(ch) < IMO_IMM)) {
	 * send_to_char("Sorry, but you can only jail yourself.\r\n", ch);
	 * return; } */

	switch (lv_jail) {
	case JAIL_LOCK:{
			if (IS_SET(GET_ACT2(vict), PLR2_JAILED)) {
				send_to_char("Player has been jailed already!\r\n", ch);
				return;
			}
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
			if (!*buf) {
				send_to_char("Please use: JAIL <name> lock <days> <hours>\r\n", ch);
				return;
			}
			if (!is_number(buf)) {
				send_to_char("<days> must be numeric.\r\n", ch);
				return;
			}
			jail_days = MAXV(0, atoi(buf));
			for (; isspace(*arg); arg++);
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
			if (!*buf) {
				send_to_char("Please use: JAIL <name> lock <days> <hours>\r\n", ch);
				return;
			}
			if (!is_number(buf)) {
				send_to_char("<hours> must be numeric.\r\n", ch);
				return;
			}
			jail_hours = MAXV(0, atoi(buf));


			if (!jail_days && !jail_hours) {
				send_to_char("Sorry but a jail time of 0 is not allowed!\r\n", ch);
				return;
			}
			if (GET_LEVEL(ch) <= GET_LEVEL(vict) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				act("You create a faint outline of bars around $N, but they fade.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n tries to jail $N but only manages to show their limits.", TRUE, ch, 0, vict, TO_NOTVICT);
				act("$n tries to jail you but ends up embarassing themselves.", TRUE, ch, 0, vict, TO_VICT);
				send_to_char("That person is to awesome to jail!\r\n", ch);
				return;
			}
			if (IS_NPC(vict) &&
			    GET_LEVEL(ch) < IMO_IMP) {
				send_to_char("Afraid not!\r\n", ch);
				return;
			}

			GET_JAILTIME(vict) = time(0) + (jail_days * 86400) + (jail_hours * 3600);
			SET_BIT(GET_ACT2(vict), PLR2_JAILED);
			vict->jailed++;

			sprintf(buf, "You jailed %s for %d days %d hours!\r\n",
				GET_NAME(vict),
				jail_days,
				jail_hours);
			send_to_char(buf, ch);

			sprintf(buf2, "%s has jailed %s for %d days %d hours.",
				GET_NAME(ch),
			 (IS_NPC(vict) ? GET_MOB_NAME(vict) : GET_NAME(vict)),
				jail_days,
				jail_hours);
			do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
			spec_log(buf2, GOD_COMMAND_LOG);

			/* PUT THEM IN JAIL */
			ha1500_char_from_room(vict);
			ha1600_char_to_room(vict, db8000_real_room(JAIL_ROOM));
			in3000_do_look(vict, "", 0);
			sprintf(buf, "mob");
			if (!(mob = ha2125_get_char_in_room_vis(vict, buf))) {
				sprintf(buf, "You've been jailed for %d days %d hours!\r\n",
					jail_days,
					jail_hours);
				send_to_char(buf, vict);
			}
			else {
				sprintf(buf, "Welcome %s, You'll be staying here for %d days and %d hours!",
					(IS_NPC(vict) ? GET_MOB_NAME(vict) : GET_NAME(vict)),
					jail_days,
					jail_hours);
				send_to_char("\r\n", vict);
				do_say(mob, buf, CMD_SAY);
			}
			perform_jail_penalty(vict, mob);
			break;
		}
	case JAIL_FREE:{
			if (IS_SET(GET_ACT2(vict), PLR2_JAILED)) {
				REMOVE_BIT(GET_ACT2(vict), PLR2_JAILED);
				sprintf(buf, "Player %s has been unjailed.\r\n",
					GET_NAME(vict));
				send_to_char(buf, ch);
				send_to_char("You've been sprung.\r\n", vict);
				sprintf(buf2, "%s has unjailed %s.\r\n",
					GET_NAME(ch),
					(IS_NPC(vict) ? GET_MOB_NAME(vict) : GET_NAME(vict)));
				do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
				spec_log(buf2, GOD_COMMAND_LOG);
				/* MOVE THEM TO INN */
				GET_JAILTIME(vict) = 0;
				if (vict->jailed > 0)
					vict->jailed--;
				ha1500_char_from_room(vict);
				ha1600_char_to_room(vict, db8000_real_room(REHAB_ROOM));
				in3000_do_look(vict, "", 0);
				return;
			}
			else {
				act("Player $N is not jailed!", FALSE, ch, 0, vict, TO_CHAR);
				return;
			}
			break;
		}
	case JAIL_SELF:{
			send_to_char("That would be rather stupid. don't you think?\r\n", ch);
			return;
		}
	default:{
			send_to_char("Hmmm, you aren't supposed to get this message.\r\n", ch);
			return;
		}
	}
	return;
}

void wz1100_do_freeze(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct char_data *vict;



	/* STRIP OUT PLAYER TO MODIFY */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("Who do you wish to freeze?\r\n", ch);
		return;
	}

	/* GET VICTIM */
	vict = 0;
	vict = ha3100_get_char_vis(ch, buf);
	if (!vict) {
		bzero(buf2, sizeof(buf2));
		sprintf(buf2, "Unable to locate %s.\r\n", buf);
		send_to_char(buf2, ch);
		return;
	}

	/* freeze yourself? */
	if (ch == vict) {
		if (GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("That wasn't the best move you ever made!\r\n",
				     ch);
		}
	}
	else {
		/* freeze higher ups? */
		if (GET_LEVEL(ch) <= GET_LEVEL(vict) &&
		    GET_LEVEL(ch) < IMO_IMP) {
			act("You create a scintillating shower of ice particles as you fail to freeze $N.", FALSE, ch, 0, vict, TO_CHAR);
			act("$n tries to freeze $N but only manages to produce a couple snow flakes.", TRUE, ch, 0, vict, TO_NOTVICT);
			act("$n tries to freeze you but ends up creating a couple snow flakes instead.", TRUE, ch, 0, vict, TO_VICT);

			send_to_char("That person is to awesome to freeze!\r\n",
				     ch);
			return;
		}
	}

	if (IS_NPC(vict) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		send_to_char("Afraid not!\r\n", ch);
		return;
	}

	bzero(buf2, sizeof(buf2));

	if (IS_SET(GET_ACT2(vict), PLR2_FREEZE)) {
		REMOVE_BIT(GET_ACT2(vict), PLR2_FREEZE);
		sprintf(buf, "Player %s can move again.\r\n",
			GET_NAME(vict));
		send_to_char(buf, ch);
		send_to_char("You can move again.\r\n", vict);
		sprintf(buf2, "%s has unfrozen %s.\r\n",
			GET_NAME(ch),
			(IS_NPC(vict) ? GET_MOB_NAME(vict) : GET_NAME(vict)));

	}
	else {
		SET_BIT(GET_ACT2(vict), PLR2_FREEZE);
		sprintf(buf, "Player %s is now frozen!\r\n",
			GET_NAME(vict));
		send_to_char(buf, ch);
		send_to_char("You've been frozen!\r\n", vict);
		sprintf(buf2, "%s has frozen %s.",
			GET_NAME(ch),
			(IS_NPC(vict) ? GET_MOB_NAME(vict) : GET_NAME(vict)));
	}
	do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
	spec_log(buf2, GOD_COMMAND_LOG);

}				/* END OF wz1100_do_freeze() */


void wz1200_do_nohassle(struct char_data * ch, char *arg, int cmd)
{



	if ((GET_LEVEL(ch) < IMO_SPIRIT) && (!ha1175_isexactname(GET_NAME(ch),
				 zone_table[world[ch->in_room].zone].lord))) {
		send_to_char("Sorry you're too lowly for that command.\r\n", ch);
		return;
	}
	if (IS_SET(GET_ACT3(ch), PLR3_NOHASSLE)) {
		REMOVE_BIT(GET_ACT3(ch), PLR3_NOHASSLE);
		send_to_char("You can now be hassled by aggr. mobs.\r\n", ch);
	}
	else {
		SET_BIT(GET_ACT3(ch), PLR3_NOHASSLE);
		send_to_char("You can no longer be hassled by aggr. mobs.\r\n", ch);
	}

}				/* END OF wz1200_do_nohassle */
/* Not used -- marked for deletion
void wz1300_do_go30(struct char_data *ch, char *arg, int cmd) {
	
	char buf[MAX_STRING_LENGTH];
	
	
	
	sprintf(buf, "CHAR %s LEVEL 30", GET_REAL_NAME(ch));
	do_set(ch, buf, CMD_SET);
	return;
	
} END OF wz1300_do_go30() */


/* ROUTINE IS USED TO FIND HYPOTINUSE OF TWO NUMBERS */
double wz1400_hypot(double lv_x, double lv_y)
{

	double lv_max, lv_rtn, idx;



	lv_x = lv_x * lv_x;
	lv_y = lv_y * lv_y;
	lv_max = lv_x + lv_y;

	lv_rtn = 0;
	for (idx = 1; (idx * idx) < lv_max; idx++) {
		lv_rtn = idx;
	}

	return (lv_rtn);

}				/* END OF wz1400_hypot() */


int wz1500_did_we_make_teleport(struct char_data * ch, int lv_from_room, int lv_to_room, int av_tele_skill, int av_party_count)
{

	char buf[MAX_STRING_LENGTH];
	int lv_from_zone, lv_to_zone, lv_skill, rtn;

	double lv_x_distance, lv_y_distance;



	/* IF IMMORTAL, YOU CAN MAKE IT */
	if (GET_LEVEL(ch) > PK_LEV) {
		return (TELEPORT_WORKED);
	}

	lv_from_zone = world[lv_from_room].zone;
	lv_to_zone = world[lv_to_room].zone;

	/* IF DIFFERENT PLANES, RETURN FALSE */
	if (zone_table[lv_from_zone].z_coordinate !=
	    zone_table[lv_to_zone].z_coordinate) {
		send_to_char("You cannot travel to other dimensions this way.\r\n", ch);
		return (TELEPORT_FAILED_ZCORD);
	}

	lv_x_distance = zone_table[lv_from_zone].x_coordinate -
		zone_table[lv_to_zone].x_coordinate;
	lv_y_distance = zone_table[lv_from_zone].y_coordinate -
		zone_table[lv_to_zone].y_coordinate;

	rtn = (int) wz1400_hypot(lv_x_distance, lv_y_distance);
	rtn = MAXV(1, rtn);

	lv_skill = av_tele_skill + GET_INT(ch) + (GET_LEVEL(ch) / 2);
	lv_skill = lv_skill - rtn - (av_party_count * 5);
	lv_skill = MINV(95, lv_skill);

	/*
	 sprintf(buf2, "av_tele=%d  INT=%d  Level:%d rtn=%d  av_part=%d  skill=%d",
					 av_tele_skill,
					 GET_INT(ch),
					 GET_LEVEL(ch),
					 rtn,
					 av_party_count,
					 lv_skill);
	 trace_log(buf2);
	 */

	if (gv_port == DEVELOPMENT_PORT) {
		sprintf(buf, "You travel %d clicks, computed skill %d.\r\n",
			rtn, lv_skill);
	}
	else {
		sprintf(buf, "You travel %d clicks.\r\n", rtn);
	}
	send_to_char(buf, ch);

	if (number(1, 101) > lv_skill) {
		if (lv_skill < 36) {
			return (TELEPORT_FAILED_MAJOR);
			trace_log("Failed majorly.");
		}
		else
			return (TELEPORT_FAILED_NORMAL);
	}
	else
		return (TELEPORT_WORKED);

}				/* END OF wz1500_did_we_make_teleport() */


void wz1600_do_bless(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct char_data *vict;



	/* STRIP OUT PLAYER TO MODIFY */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("Who do you wish to bless?\r\n", ch);
		return;
	}

	/* GET VICTIM */
	vict = ha2125_get_char_in_room_vis(ch, buf);
	if (!vict) {
		sprintf(buf2, "Unable to locate %s.\r\n", buf);
		send_to_char(buf2, ch);
		return;
	}

	/* Don't let IMMORTALS cast on MORTALS */
	if (GET_LEVEL(ch) > PK_LEV &&
	    GET_LEVEL(ch) < IMO_IMM &&
	    GET_LEVEL(vict) < IMO_SPIRIT) {
		send_to_char("Nothing happens.\r\n", ch);
		return;
	}
	GET_BLESS(ch) = 1;
	sprintf(buf, "aid %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	if (IS_EVIL(vict)) {
		sprintf(buf, "vex %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	}
	sprintf(buf, "bless %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "armor %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "improved %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect evil' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect good' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect invis' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect magic' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect undead' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'detect animals' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	if (IS_GOOD(vict)) {
		sprintf(buf, "protection %s", GET_REAL_NAME(vict));
		do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	}
	sprintf(buf, "sanctuary %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'strength' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'sense life' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'darksight' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'regeneration' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'breathwater' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'waterwalk' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'bind souls' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'demon flesh' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'demonic aid' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'intelligence' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'wisdom' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'dexterity' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'constitution' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'charisma' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	sprintf(buf, "'fly' %s", GET_REAL_NAME(vict));
	do_cast_proc(ch, vict, buf, CMD_CAST, 0);
	GET_BLESS(ch) = 0;

	if (GET_LEVEL(ch) > PK_LEV) {
		sprintf(buf, "BLESS: %s blessed %s", GET_REAL_NAME(ch), GET_REAL_NAME(vict));
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}
}				/* END OF wz1600_do_bless() */


void wz1700_do_phase(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];



	if (cmd == CMD_PN) {
		sprintf(buf, "phase north");
		do_cast(ch, buf, CMD_CAST);
		return;
	}
	if (cmd == CMD_PS) {
		sprintf(buf, "phase south");
		do_cast(ch, buf, CMD_CAST);
		return;
	}
	if (cmd == CMD_PE) {
		sprintf(buf, "phase east");
		do_cast(ch, buf, CMD_CAST);
		return;
	}
	if (cmd == CMD_PW) {
		sprintf(buf, "phase west");
		do_cast(ch, buf, CMD_CAST);
		return;
	}
	if (cmd == CMD_PU) {
		sprintf(buf, "phase up");
		do_cast(ch, buf, CMD_CAST);
		return;
	}
	if (cmd == CMD_PD) {
		sprintf(buf, "phase down");
		do_cast(ch, buf, CMD_CAST);
		return;
	}

	send_to_char("Nothing happens.\r\n", ch);
	return;

}				/* END OF wz1700_do_phase() */


void wz1800_do_finger(struct char_data * ch, char *arg, int cmd)
{

	int lv_char_nr, lv_months, lv_days, lv_hours, lv_minutes, lv_seconds;
	unsigned long lv_seconds_gone;
	char buf[MAX_STRING_LENGTH];



	/* ZERO OUT buf AREA */
	bzero(buf, sizeof(buf));

	for (; *arg == ' '; arg++);

	if (!*arg) {
		send_to_char("Who do you want to finger?\r\n", ch);
		return;
	}
	/* ON */
	if (!strcmp(arg, "on") || !strcmp(arg, "ON")) {
		if (IS_SET(GET_DSC1(ch), DFLG1_CAN_FINGER)) {
			send_to_char("You can already be fingered.\r\n", ch);
			return;
		}
		else {
			send_to_char("You can now be fingered.\r\n", ch);
			SET_BIT(GET_DSC1(ch), DFLG1_CAN_FINGER);
			return;
		}
	}
	/* OFF */
	if (!strcmp(arg, "off") || !strcmp(arg, "OFF")) {
		if (IS_SET(GET_DSC1(ch), DFLG1_CAN_FINGER)) {
			send_to_char("You can't be fingered.\r\n", ch);
			REMOVE_BIT(GET_DSC1(ch), DFLG1_CAN_FINGER);
			return;
		}
		else {
			send_to_char("You are already blocked.\r\n", ch);
			return;
		}
	}

	if (is_number(arg)) {
		lv_char_nr = atoi(arg);
	}
	else {
		lv_char_nr = pa2050_find_name_approx(arg);
		if (lv_char_nr < 0) {
			send_to_char("Sorry, unknown player.\r\n",
				     ch);
			return;
		}
	}

	if (lv_char_nr > top_of_p_file ||
	    lv_char_nr < 0) {
		sprintf(buf, "Numeric position must be between 0 and %d.\r\n",
			top_of_p_file);
		send_to_char(buf, ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMP) {
		if (GET_LEVEL(ch) < IMO_IMM ||
		/* ANTE CAN ONLY FINGER THOSE <= TO THEMSELVES */
		    GET_DSC_LEVEL_NUM(lv_char_nr) > GET_LEVEL(ch)) {
			if (!(IS_SET(GET_DSC1_NUM(lv_char_nr), DFLG1_CAN_FINGER))) {
				sprintf(buf, "Sorry, but you can't finger %s\r\n",
					player_table[lv_char_nr].pidx_name);
				send_to_char(buf, ch);
				return;
			}
		}
	}

	bzero(buf, sizeof(buf));

	/* PLAYER NUMBER */
	if (GET_LEVEL(ch) >= IMO_IMP) {
		sprintf(buf, "Player number: %d\r\n", lv_char_nr);
		send_to_char(buf, ch);
	}

	/* EMAIL */
	/* PROTECT EMAIL ADDRESS PRIVACY */
	if (GET_LEVEL(ch) >= IMO_IMP ||
	    (IS_SET(GET_DSC1_NUM(lv_char_nr), DFLG1_CAN_FINGER))) {
		if (strlen(player_table[lv_char_nr].pidx_email) > 0) {
			sprintf(buf, "Current email address: %s.\r\n",
				player_table[lv_char_nr].pidx_email);
		}
		else {
			sprintf(buf, "Current email address: undefined.\r\n");
		}
		send_to_char(buf, ch);
	}

	/* LAST LOGON */
	if (player_table[lv_char_nr].pidx_last_on > 0) {

		/* calculate time away */
		lv_seconds_gone = time(0) -
			player_table[lv_char_nr].pidx_last_on;

		lv_months = 0;
		lv_days = 0;
		lv_hours = 0;
		lv_minutes = 0;

		/* MONTHS */
		if (lv_seconds_gone > 2592000)
			lv_months = (int) (lv_seconds_gone / 2592000);
		lv_seconds_gone = lv_seconds_gone - (lv_months * 2592000);

		/* DAYS */
		if (lv_seconds_gone > 86400)
			lv_days = (int) (lv_seconds_gone / 86400);
		lv_seconds_gone = lv_seconds_gone - (lv_days * 86400);

		/* HOURS */
		if (lv_seconds_gone > 3600)
			lv_hours = (int) (lv_seconds_gone / 3600);
		lv_seconds_gone = lv_seconds_gone - (lv_hours * 3600);

		/* MINUTES */
		if (lv_seconds_gone > 60)
			lv_minutes = (int) (lv_seconds_gone / 60);
		lv_seconds_gone = lv_seconds_gone - (lv_minutes * 60);

		lv_seconds = lv_seconds_gone;

		bzero(buf, sizeof(buf));
		sprintf(buf, "Gone %d months, %d days, %d hours, %d minutes, %d seconds\r\n",
			lv_months, lv_days, lv_hours, lv_minutes, lv_seconds);
		send_to_char(buf, ch);

		sprintf(buf, "%s was on at: %s\r\n",
			player_table[lv_char_nr].pidx_name,
			(char *) asctime(
			  localtime(&player_table[lv_char_nr].pidx_last_on)));
		send_to_char(buf, ch);
		if (player_table[lv_char_nr].pidx_clan_number) {
			sprintf(buf, "Level: %d Clan: %s   Rank: %s.\r\n",
				player_table[lv_char_nr].pidx_level,
				GET_CLAN_NAME(player_table[lv_char_nr].pidx_clan_number),
				GET_CLAN_RANK_NAME(player_table[lv_char_nr].pidx_clan_number,
				    player_table[lv_char_nr].pidx_clan_rank));
		}
		else
			sprintf(buf, "Level: %d", player_table[lv_char_nr].pidx_level);
		send_to_char(buf, ch);


	}
	else {
		sprintf(buf, "%s doesn't have a valid last on time\r\n",
			player_table[lv_char_nr].pidx_name);
		send_to_char(buf, ch);
	}

	return;

}				/* END OF  wz1800_do_finger() */


void wz1900_do_email(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];



	if (IS_NPC(ch)) {
		send_to_char("Where do you expect to read mail?\r\n", ch);
		return;
	}

	for (; *arg == ' '; arg++);

	/* IF USER DIDN'T SPECIFY AN EMAIL, PRINT CURRENT VALUE */
	if (!*arg) {
		bzero(buf, sizeof(buf));
		if (strlen(GET_EMAIL(ch)) > 0) {
			sprintf(buf, "Your current email address: %s.\r\n",
				GET_EMAIL(ch));
		}
		else {
			sprintf(buf, "Your current email address: undefined.\r\n");
		}
		send_to_char(buf, ch);
		return;
	}

	/* SET EMAIL */
	bzero(GET_EMAIL(ch), sizeof(GET_EMAIL(ch)));
	strncpy(GET_EMAIL(ch), arg, EMAIL_SIZE - 1);

	bzero(buf, sizeof(buf));
	if (strlen(GET_EMAIL(ch)) > 0) {
		sprintf(buf, "Your new email address: %s.\r\n",
			GET_EMAIL(ch));
	}
	else {
		sprintf(buf, "Your new email address: undefined.\r\n");
	}
	send_to_char(buf, ch);
	return;

}				/* END OF wz1900_do_email() */

void wz2000_do_peace(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *vict;
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "%s used peace in room [%d].\r\n", GET_NAME(ch), world[ch->in_room].number);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);
	send_to_char("You have stopped all fighting in this room.\r\n", ch);
	act("$n has stopped all fighting in this room.", FALSE, ch, 0, 0, TO_ROOM);
	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room) {
		if (vict->specials.fighting) {
			ft1400_stop_fighting(vict, 0);
		}
	}
	return;

}				/* END OF wz2000_do_peace() */
//On the fly object editing by Bingo(adapted from GodWars code by KaVir)
	void do_oset(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	unsigned long flag;
	int loc, i, one;

	if (IS_NPC(ch)) {
		send_to_char("I don't think so.\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if (!*arg1 || !*arg2 || !*arg3) {
		send_to_char("Syntax: oset <object> <type> <value>    \n\r"
		" Types: &Rtype&n, &Rwear&n, &Rflag&n, &Rwflag&n,        \n\r"
		"        &Raff0&n, &Raff1&n, &Raff2&n, &Raff3&n,         \n\r"
		"        &Rcost&n, &Rrent&n, &Rweight&n, &Rvalues&n,     \n\r"
		"        &Ykeywords&n, &Yshort&n, &Ylong&n, &Yedesc&n,   \n\r"
		"        &Rtimer&n, &Ycopy&n                             \n\r"
			     " Commands in &RRED&n are implemented.       \n\r"
		      " Commands in &YYELLOW&n are not implemented.\n\r", ch);

		return;
	}

	if ((obj = ha2075_get_obj_list_vis(ch, arg1, ch->carrying)) == NULL) {

		send_to_char("You do not have that item.\n\r", ch);
		return;
	}

	flag = atoi(arg3);

	/* Setting object timers */
	if (!str_cmp(arg2, "timer")) {
		if (is_number(arg3)) {
			if (GET_LEVEL(ch) < IMO_IMP && atoi(arg3) > 500) {
				send_to_char("Value must be lower than 500.\n\r", ch);
				return;
			}
			TIMER(obj) = atoi(arg3) + 1;
			sprintf(buf, "You have set the timer to: [%d]\r\n", GET_TIMER(obj));
			send_to_char(buf, ch);
			return;
		}
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "copy")) {
		send_to_char("Command not implemented.\n\r", ch);
		return;
	}

	/* For setting object type */
	if (!str_cmp(arg2, "type")) {
		loc = (old_search_block(arg3, 0, strlen(arg3), item_types, -1));
		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;
		if (loc == 0) {
			send_to_char("You cant set an item type to UNDEFINED.\r\n", ch);
			return;
		}
		if (loc == ITEM_ZCMD) {
			send_to_char("Those are reserved for online zone editing.\r\n", ch);
			return;
		}
		sprintf(buf, "You have changed type to '%s'.\r\n", item_types[loc]);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
		send_to_char(buf, ch);
		GET_ITEM_TYPE(obj) = loc;
	}

	/* For setting object flags */
	if (!str_cmp(arg2, "flag")) {
		CHECK_LEVEL(IMO_IMP);
		loc = (old_search_block(arg3, 0, strlen(arg3), oflag1_bits, -1));
		if (loc == -1) {
			loc = (old_search_block(arg3, 0, strlen(arg3), oflag2_bits, -1));
			if (loc == -1) {
				send_to_char("That flag doesn't exist.\r\n", ch);
				return;
			}
			loc--;

			flag = 1;
			for (i = 0; i < loc; i++)
				flag *= 2;

			if (IS_SET(obj->obj_flags.flags2, flag)) {
				sprintf(buf, "You have removed the '%s' flag.\r\n", oflag2_bits[loc]);
				REMOVE_BIT(obj->obj_flags.flags2, flag);
				sprintf(buf1, "OSET: %s : %s", arg1, buf);
				spec_log(buf1, GOD_COMMAND_LOG);
				send_to_char(buf, ch);
			}
			else {
				sprintf(buf, "You have set the '%s' flag.\r\n", oflag2_bits[loc]);
				SET_BIT(obj->obj_flags.flags2, flag);
				sprintf(buf1, "OSET: %s : %s", arg1, buf);
				spec_log(buf1, GOD_COMMAND_LOG);
				send_to_char(buf, ch);
			}
			return;
		}
		loc--;

		flag = 1;
		for (i = 0; i < loc; i++)
			flag *= 2;

		if (IS_SET(obj->obj_flags.flags1, flag)) {
			sprintf(buf, "You have removed the '%s' flag.\r\n", oflag1_bits[loc]);
			/* We don't want them removing the omax flag, why you
			 * ask? no clue :P */
			if (flag == OBJ1_OMAX) {
				send_to_char("You cannot remove the OMAX flag.\n\r", ch);
				return;
			}
			REMOVE_BIT(obj->obj_flags.flags1, flag);
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
		else {
			sprintf(buf, "You have set the '%s' flag.\r\n", oflag1_bits[loc]);
			SET_BIT(obj->obj_flags.flags1, flag);
			/* We don't want them adding the omax flag, why you
			 * ask? no clue :P */
			if (flag == OBJ1_OMAX) {
				send_to_char("You cannot add the OMAX flag.\n\r", ch);
				return;
			}
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
	}
	/* For setting wear bits */
	if (!str_cmp(arg2, "wear")) {

		loc = (old_search_block(arg3, 0, strlen(arg3), wear_bits, -1));
		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		flag = 1;
		for (i = 0; i < loc; i++)
			flag *= 2;

		if (IS_SET(obj->obj_flags.wear_flags, flag)) {
			sprintf(buf, "You have removed the '%s' flag.\r\n", wear_bits[loc]);
			REMOVE_BIT(obj->obj_flags.wear_flags, flag);
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
		else {
			sprintf(buf, "You have set the '%s' flag.\r\n", wear_bits[loc]);
			SET_BIT(obj->obj_flags.wear_flags, flag);
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
	}

	if (!str_cmp(arg2, "aff0")) {

		CHECK_LEVEL(IMO_IMP);

		argument = one_argument(argument, arg4);

		if (isalpha(*arg4)) {
			send_to_char("Value must be numerical.\n\r", ch);
			return;
		}

		loc = (old_search_block(arg3, 0, strlen(arg3), apply_types, -1));
		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		i = atoi(arg3);

		obj->affected[0].location = loc;
		sscanf(arg4, " %d", &one);
		obj->affected[0].modifier = one;

		sprinttype(obj->affected[0].location, apply_types, arg3);
		sprintf(buf, "Ok, it now affects: %s By %d\r\n", arg3, obj->affected[0].modifier);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "aff1")) {

		CHECK_LEVEL(IMO_IMP);

		argument = one_argument(argument, arg4);

		if (isalpha(*arg4)) {
			send_to_char("Value must be numerical.\n\r", ch);
			return;
		}

		loc = (old_search_block(arg3, 0, strlen(arg3), apply_types, -1));
		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		i = atoi(arg3);

		obj->affected[1].location = loc;
		sscanf(arg4, " %d", &one);
		obj->affected[1].modifier = one;

		sprinttype(obj->affected[1].location, apply_types, arg3);
		sprintf(buf, "Ok, it now affects: %s By %d\r\n", arg3, obj->affected[1].modifier);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "aff2")) {

		CHECK_LEVEL(IMO_IMP);

		argument = one_argument(argument, arg4);
		loc = (old_search_block(arg3, 0, strlen(arg3), apply_types, -1));

		if (isalpha(*arg4)) {
			send_to_char("Value must be numerical.\n\r", ch);
			return;
		}

		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		i = atoi(arg3);

		obj->affected[2].location = loc;
		sscanf(arg4, " %d", &one);
		obj->affected[2].modifier = one;

		sprinttype(obj->affected[2].location, apply_types, arg3);
		sprintf(buf, "Ok, it now affects: %s By %d\r\n", arg3, obj->affected[2].modifier);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "aff3")) {

		CHECK_LEVEL(IMO_IMP);

		argument = one_argument(argument, arg4);
		loc = (old_search_block(arg3, 0, strlen(arg3), apply_types, -1));

		if (isalpha(*arg4)) {
			send_to_char("Value must be numerical.\n\r", ch);
			return;
		}

		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		i = atoi(arg3);

		obj->affected[3].location = loc;
		sscanf(arg4, " %d", &one);
		obj->affected[3].modifier = one;

		sprinttype(obj->affected[3].location, apply_types, arg3);
		sprintf(buf, "Ok, it now affects: %s By %d\r\n", arg3, obj->affected[3].modifier);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "wflag")) {
		CHECK_LEVEL(IMO_IMP);
		loc = (old_search_block(arg3, 0, strlen(arg3), weapon_bits, -1));
		if (loc == -1) {
			send_to_char("That flag doesn't exist.\r\n", ch);
			return;
		}
		loc--;

		flag = 1;
		for (i = 0; i < loc; i++)
			flag *= 2;

		if (IS_SET(obj->obj_flags.value[0], flag)) {
			sprintf(buf, "You have removed the '%s' flag.\r\n", weapon_bits[loc]);
			REMOVE_BIT(obj->obj_flags.value[0], flag);
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
		else {
			sprintf(buf, "You have set the '%s' flag.\r\n", weapon_bits[loc]);
			SET_BIT(obj->obj_flags.value[0], flag);
			send_to_char(buf, ch);
			sprintf(buf1, "OSET: %s : %s", arg1, buf);
			spec_log(buf1, GOD_COMMAND_LOG);
		}
	}

	if (!str_cmp(arg2, "short")) {
		send_to_char("Command not implemented.\n\r", ch);
	}

	if (!str_cmp(arg2, "long")) {
		send_to_char("Command not implemented.\n\r", ch);
	}

	if (!str_cmp(arg2, "keywords")) {
		send_to_char("Command not implemented.\n\r", ch);
	}

	if (!str_cmp(arg2, "cost")) {
		if (is_number(arg3))
			obj->obj_flags.cost = atoi(arg3);
		sprintf(buf, "You have made the cost: [%d]", obj->obj_flags.cost);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "rent")) {
		if (is_number(arg3))
			obj->obj_flags.cost_per_day = atoi(arg3);
		sprintf(buf, "You have made the rent cost: [%d]", obj->obj_flags.cost_per_day);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "weight")) {
		if (is_number(arg3))
			obj->obj_flags.weight = atoi(arg3);
		sprintf(buf, "You have made the weight: [%d]", obj->obj_flags.weight);
		send_to_char(buf, ch);
		sprintf(buf1, "OSET: %s : %s", arg1, buf);
		spec_log(buf1, GOD_COMMAND_LOG);
	}

	if (!str_cmp(arg2, "values")) {
		CHECK_LEVEL(IMO_IMP);
		obj->obj_flags.value[0] = 0;
		obj->obj_flags.value[1] = 0;
		obj->obj_flags.value[2] = 0;
		obj->obj_flags.value[3] = 0;
		if (isdigit(*arg3))
			obj->obj_flags.value[0] = atoi(arg3);
		argument = one_argument(argument, arg3);
		if (isdigit(*arg3))
			obj->obj_flags.value[1] = atoi(arg3);
		argument = one_argument(argument, arg3);
		if (isdigit(*arg3))
			obj->obj_flags.value[2] = atoi(arg3);
		argument = one_argument(argument, arg3);
		if (isdigit(*arg3))
			obj->obj_flags.value[3] = atoi(arg3);
		sprintf(buf, "Current values are: v0[%d] v1[%d] v2[%d] v3[%d]",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		send_to_char(buf, ch);
	}

	if (!str_cmp(arg2, "edesc")) {
		send_to_char("Command not implemented.\n\r", ch);
	}

	sprintf(buf, "[%s] oset %s %s %s", GET_REAL_NAME(ch), GET_OSDESC(obj), arg2, arg3);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);
}
//Code converted and modified from God Wars by Bingo.Original code created by KaVir
void do_otransfer(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	struct char_data *victim, *carrier;
	int chroom, objroom, eq_pos;

	arg = one_argument(arg, arg1);
	arg = one_argument(arg, arg2);

	if (arg1[0] == '\0') {
		send_to_char("Otransfer which object?\n\r", ch);
		return;
	}

	if (arg2[0] == '\0')
		victim = ch;
	else if ((victim = ha3100_get_char_vis(ch, arg2)) == NULL) {
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if ((obj = ha3200_get_obj_vis(ch, arg1)) == NULL) {
		send_to_char("Sorry, that object isnt out there.\n\r", ch);
		return;
	}
	if (obj->carried_by != NULL) {
		carrier = obj->carried_by;
		for (eq_pos = 0; (eq_pos < MAX_WEAR); eq_pos++)
			if (obj == obj->carried_by->equipment[eq_pos]) {
				ha1700_obj_to_char(ha1930_unequip_char(obj->carried_by, eq_pos), carrier);
			}
		sprintf(buf, "[%s] transfers %s vnum(%d) to/from %s", GET_REAL_NAME(ch), GET_OBJ_NAME(obj), GET_OBJ_VNUM(obj), GET_REAL_NAME(carrier));
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);

		act("$p vanishes from your hands in an explosion of energy.", TRUE, carrier, obj, NULL, TO_CHAR);
		act("$p vanishes from $n's hands in an explosion of energy.", TRUE, carrier, obj, NULL, TO_ROOM);
		ha1800_obj_from_char(obj);

	}
	else if (obj->in_obj != NULL)
		ha2400_obj_from_obj(obj);
	else if (obj->in_room >= 0) {

		objroom = obj->in_room;
		chroom = ch->in_room;
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, objroom);
		act("$p vanishes from the ground in an explosion of energy.", TRUE, ch, obj, NULL, TO_ROOM);
		if (chroom == objroom)
			act("$p vanishes from the ground in an explosion of energy.", TRUE, ch, obj, NULL, TO_CHAR);
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, chroom);
		ha2200_obj_from_room(obj);

		sprintf(buf, "[%s] transfers %s vnum(%d) from room %d", GET_REAL_NAME(ch), GET_OBJ_NAME(obj), GET_OBJ_VNUM(obj), objroom);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, GOD_COMMAND_LOG);
	}
	else {
		send_to_char("You were unable to get it.\n\r", ch);
		return;
	}
	ha1700_obj_to_char(obj, victim);
	act("$p appears in your hands in an explosion of energy.", TRUE, victim, obj, NULL, TO_CHAR);
	act("$p appears in $n's hands in an explosion of energy.", TRUE, victim, obj, NULL, TO_ROOM);
	return;
}
