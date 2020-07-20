/* co */
/* gv_location: 3001-3500 */
/* *********************************************************************
*  file: comm.c , Communication module.                Part of DIKUMUD *
*  Usage: Communication, central game loop.                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inforn.   *
*  All Rights Reserved                                                 *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include <fcntl.h>

#include "structs.h"
#include "utility.h"
#include "db.h"
#include "parser.h"
#include "handler.h"
#include "limits.h"
#include "ban.h"
#include "ansi.h"
#include "weather.h"
#include "rent.h"
#include "signals.h"
#include "comm.h"
#include "constants.h"
#include "globals.h"
#include "xanth.h"
#include "func.h"
#include "alias.h"
#include "history.h"

#define ALARM_WAIT     120	/* Infinate loop check */
#define MAX_HOSTNAME   256
#define OPT_USEC     62500	/* time delay corresponding to 16 passes/sec */

/* going to be part of config.sys */
#define LOG_SOCKET_EOF     0	/* why would you wish to log this? */
#define LOG_NEW_CONNECTION 0	/* why would you wish to log this? */
#define USE_NAMESERVER	   1	/* slow server getting you down  */

/* For IPv6 support */
#define MAXSOCK 8
#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

/* abc ABC */
struct char_data *temp_ch;

extern int errno;		/* Why isn't this done in errno.h on alfa??? */
struct descriptor_data *descriptor_list, *next_to_process;

struct txt_block *bufpool = 0;

int slow_death = 0;		/* Shut her down, Martha, she's sucking mud */
int no_specials = 0;		/* Suppress ass. of special routines */

int maxdesc = 0, avail_descs = 0;
int tics = 0;			/* for extern checkpointing */
int pulse = 0;			/* for synching wait_states of characters */

void ma1350_handle_mail_input(struct descriptor_data * point, char *comm);

struct timeval co1600_timediff(struct timeval * a, struct timeval * b);
struct char_data *make_char(char *name, struct descriptor_data * desc);
int co2300_process_input_new(struct descriptor_data * t);
int co3400_process_input_old(struct descriptor_data * t);
int touch(const char *path);
void co1800_init_socket(ush_int port, int mother_descs[]);
int set_sendbuf(int s);
int co1200_game_loop(int mother_descs[]);
void co2400_close_all_sockets(int mother_descs[]);

/* *********************************************************************
*  main game loop and related stuff                 *
********************************************************************* */

int main(int argc, char **argv)
{
	int port, pos, idx;
	char buf[512], *dir;

	port = DEFAULT_PORT;
	dir = DEFAULT_DIR;
	gv_deathroom = DEATH_ROOM;

	thebits[0] = 1;
	for (idx = 1; idx < 31; idx++) {
		thebits[idx] = thebits[idx - 1] * 2;
	}

	ha9000_open_tracefile(1);
	if (!trace_file) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Error %d opening trace file\r\n", errno);
		perror(buf);
		exit(1);
	}
	trace_log("\r\nSTARTING GAME LOOP: Loop\r\n");
	main_log("\r\nSTARTING GAME LOOP: Loop\r\n");

	gv_ch_ptr = 0;
	gv_total_objects = 0;
	gv_total_mobiles = 0;
	gv_use_nameserver = 1;
	gv_rush_hour = 1;
	gv_rush_time = time(0) + RUSH_INTERVAL * 60 + number(0, RUSH_INTERVAL2) * 60;
	gv_verbose_messages = FALSE;
	gv_mega_verbose_messages = FALSE;
	gv_terminate_count = 0;
	gv_num = 0;
	gv_questreact = 0;
	bzero(gv_str, sizeof(gv_str));
	main_log("Starting up the MUD!");
	gv_trace_commands = 0;
	gv_sanity_return = 0;
	gv_rebuild = 0;
	gv_auto_level = 0;
	gv_pkill_level = 42;
	gv_force_infinate_loop = FALSE;
	gv_infinate_loop_count = time(0);
	gv_run_flag = 0;
	killer_mob = 0;
	buf_switches = 0;
	buf_largecount = 0;
	buf_overflows = 0;
	bufpool = 0;



	bzero(gv_login_message, sizeof(gv_login_message));

	/* DISPLAY ARGUMENTS */
	printf("There are %d arguments\r\n", argc);
	for (pos = 0; pos < argc; pos++) {
		printf("arg %d *%s*\r\n", pos, argv[pos]);
	}
	pos = 1;

	while ((pos < argc) && (*(argv[pos]) == '-')) {
		switch (*(argv[pos] + 1)) {
		case '?':
		case 'h':{
				printf("\r\n");
				printf(" FORMAT: -? This help\r\n");
				printf("         -d directory to use\r\n");
				printf("         -e create max eqp file.\r\n");
				printf("         -E resets everyone's eq to database stats.\r\n");
				printf("         -r rebuild routines.\r\n");
				printf("         -s no specials.\r\n");
				printf("         -v verbose.\r\n");
				printf("         -V MAXIMUM verbose.\r\n");
				printf("         -w supress weather messages.\r\n");
				printf("         -z don't reset zones.\r\n");
				printf("\r\n");
				exit(0);
				break;
			}
		case 'd':{
				if (*(argv[pos] + 2))
					dir = argv[pos] + 2;
				else if (++pos < argc)
					dir = argv[pos];
				else {
					bzero(buf, sizeof(buf));
					sprintf(buf, "ERROR: Directory arg expected after option -d.\r\n");
					perror(buf);
					ABORT_PROGRAM();
				}
				break;
			}
		case 'e':{
				SET_BIT(gv_run_flag, RUN_FLAG_CREATE_MAX_FILE);
				main_log("<<<Creating EQUIPMENT file>>>");
				spec_log("<<<Creating EQUIPMENT file>>>", SYSTEM_LOG);
				break;
			}
		case 'E':{
				SET_BIT(gv_run_flag, RUN_FLAG_REBUILD_EQUIPMENT);
				main_log("<<<EQUIPMENT REBUILDING ENABLED>>>");
				spec_log("<<<EQUIPMENT REBUILDING ENABLED>>>", SYSTEM_LOG);
				break;
			}
		case 'r':{
				gv_rebuild = 1;
				main_log("<<<running rebuild routines>>>");
				spec_log("<<<running rebuild routines>>>", SYSTEM_LOG);
				break;
			}
		case 's':{
				SET_BIT(gv_run_flag, RUN_FLAG_NOSPECIAL);
				main_log("Suppressing assignment of special routines.");
				spec_log("Suppressing assignment of special routines.", SYSTEM_LOG);
				break;
			}
		case 'v':{
				gv_verbose_messages = TRUE;
				main_log("Will print Verbose messages");
				spec_log("Will print Verbose messages", SYSTEM_LOG);
				break;
			}
		case 'V':{
				gv_verbose_messages = TRUE;
				gv_mega_verbose_messages = TRUE;
				main_log("Will print MEGA Verbose messages.");
				spec_log("Will print MEGA Verbose messages.", SYSTEM_LOG);
				break;
			}
		case 'w':{
				SET_BIT(gv_run_flag, RUN_FLAG_NOWEATHER);
				main_log("Won't print weather messages.");
				spec_log("Won't print weather messages.", SYSTEM_LOG);
				break;
			}
		case 'z':{
				SET_BIT(gv_run_flag, RUN_FLAG_NOZONE_RESET);
				main_log("Suppressing zone resets.");
				spec_log("Suppressing zone resets.", SYSTEM_LOG);
				break;
			}
		default:{
				sprintf(buf, "Unknown option -% in argument string.", *(argv[pos] + 1));
				main_log(buf);
				spec_log(buf, SYSTEM_LOG);
				break;
			}
		}
		pos++;
	}

	if (pos < argc) {
		if (!isdigit(*argv[pos])) {
			bzero(buf, sizeof(buf));
			printf("Argument *%s* is invalid\r\n", argv[pos]);
			bzero(buf, sizeof(buf));
			sprintf(buf, "Usage: %s [-s] [-d pathname] [ port # ]\n", argv[0]);
			perror(buf);
			ABORT_PROGRAM();
		}
		else if ((port = atoi(argv[pos])) <= 1024) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: illegal port\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}
	if (chdir(dir) < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: chdir\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	gv_port = port;
	fprintf(stderr, "Port %d, data from %s.", port, dir);

	if (gv_port == 4500) {	/* don't allow new chars on port 4500 */
		gv_highest_login_level = 2;
	}
	else if (gv_port == 9999) {
		gv_highest_login_level = 45;
	}
	else {
		gv_highest_login_level = 0;
	}

	srandom(time(0));
	au2000_remind_mob_to_auction_later(0, NULL, NULL, 0);
  	co1000_run_the_game(port);
	return (0);
}


/* Init sockets, run game, and cleanup sockets */
int co1000_run_the_game(int port)
{
	int mother_descs[MAXSOCK];

	/* Make sure if we just keep crashing, to kill the script. */
	touch(KILL_SCRIPT_FILE);

	descriptor_list = NULL;
	main_log("Signal trapping.");
	signal_setup();
	/* INCREASE MAX USERS */
	/* co1250_increase_max_users(); */
	main_log("Opening mother connection.");
	co1800_init_socket(port, mother_descs);
	db1000_boot_db();

	//Remove it before we enter the main game loop, because we should be all good here, now
	remove(KILL_SCRIPT_FILE);

	qu0100_questFileOpen();
	//loads QM variables
	isQuestAutoOn = FALSE;

	main_log("Entering game loop.");
	co1200_game_loop(mother_descs);
	co2400_close_all_sockets(mother_descs);
	destroy_histories();
	main_log("Normal termination of game.");
	return 0;
}
#define COLOR_SIZE 10

char *co1100_give_prompt(struct descriptor_data * point)
{

	struct char_data *ch, *vict;
	struct room_direction_data *lv_dir_ptr;
	int i, kdx, lv_row, lv_color;
	static char p_buf[MAX_PROMPT_LENGTH];
	char buf[MAX_STRING_LENGTH], temp_str[MAX_STRING_LENGTH], lv_white[10], lv_gray[10],
	  lv_purple[10], lv_lgreen[10], lv_lred[10], lv_cyan[10], lv_yellow[10],
	  lv_red[10], lv_end_color[10], lvc[20];

	bzero(p_buf, sizeof(p_buf));
	bzero(temp_str, sizeof(temp_str));

	/* give the people some prompts - promt */

	ch = point->character;
	if (ch && IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
		sprintf(lv_white, WHITE);
		sprintf(lv_gray, GRAY);
		sprintf(lv_purple, PURPLE);
		sprintf(lv_lgreen, GREEN);
		sprintf(lv_lred, LRED);
		sprintf(lv_cyan, CYAN);
		sprintf(lv_red, RED);
		sprintf(lv_yellow, YELLOW);
		sprintf(lv_end_color, END);
	}
	else {
		bzero(lv_white, COLOR_SIZE);
		bzero(lv_gray, COLOR_SIZE);
		bzero(lv_purple, COLOR_SIZE);
		bzero(lv_lgreen, COLOR_SIZE);
		bzero(lv_lred, COLOR_SIZE);
		bzero(lv_cyan, COLOR_SIZE);
		bzero(lv_red, COLOR_SIZE);
		bzero(lv_yellow, COLOR_SIZE);
		bzero(lv_end_color, COLOR_SIZE);
	}

	if (point->showstr_count)
		snprintf(p_buf, sizeof(p_buf),
			 "\r\n[ Return to continue, (q)uit, (r)efresh, (b)ack, or page number (%d/%d) ]",
			 point->showstr_page, point->showstr_count);
	else if (point->str || point->mailedit)
		strncpy(p_buf, "] ", sizeof(p_buf));
	else if ((!point->connected) && ch) {
		/* heres the prompt to change */
		/* New block */
		int count;
		size_t len = 0;
		bzero(p_buf, sizeof(p_buf));
		bzero(buf, sizeof(buf));

		if (GET_NEWGRAPHICS(ch) && IS_SET(GET_ACT1(ch), PLR1_SHOW_DIAG) && (len < sizeof(p_buf))) {
			//********************************
				//New Graphics Prompt
				// ********************************
				//add fighting line
				if (ch->specials.fighting) {
				vict = ch;
				strcat(buf, "&W[");
				lv_row = (100 * GET_HIT(vict)) / GET_HIT_LIMIT(vict);
				for (kdx = 0; kdx < 21; kdx++) {
					if ((kdx * 5) < lv_row) {
						if (kdx % 7 == 0) {
							if ((kdx / 7) == 0)
								strcat(buf, "&R");
							if ((kdx / 7) == 1)
								strcat(buf, "&Y");
							if ((kdx / 7) == 2)
								strcat(buf, "&G");
						}
						strcat(buf, "*");
						lv_color = 1;
					}
					else {
						if (lv_color) {
							lv_color = 0;
							strcat(buf, "&w");
						};
						strcat(buf, "-");
					}

				}
				sprintf(buf + strlen(buf), "&W] &c%s\r\n", GET_REAL_NAME(vict));
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s", buf);
				if (count >= 0)
					len += count;

				bzero(buf, sizeof(buf));
				vict = ch->specials.fighting;
				strcat(buf, "&W[");
				lv_row = (100 * GET_HIT(vict)) / GET_HIT_LIMIT(vict);
				for (kdx = 0; kdx < 21; kdx++) {
					if ((kdx * 5) < lv_row) {
						if (kdx % 7 == 0) {
							if ((kdx / 7) == 0)
								strcat(buf, "&R");
							if ((kdx / 7) == 1)
								strcat(buf, "&Y");
							if ((kdx / 7) == 2)
								strcat(buf, "&G");
						}
						strcat(buf, "*");
						lv_color = 1;
					}
					else {
						if (lv_color) {
							lv_color = 0;
							strcat(buf, "&w");
						};
						strcat(buf, "-");
					}

				}
				sprintf(buf + strlen(buf), "&W] &c%s\r\n", GET_REAL_NAME(vict));
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s", buf);
				if (count >= 0)
					len += count;

				bzero(buf, sizeof(buf));
				if (vict->specials.fighting && (vict->specials.fighting != ch)) {
					vict = vict->specials.fighting;
					strcat(buf, "&W[");
					lv_row = (100 * GET_HIT(vict)) / GET_HIT_LIMIT(vict);
					for (kdx = 0; kdx < 21; kdx++) {
						if ((kdx * 5) < lv_row) {
							if (kdx % 7 == 0) {
								if ((kdx / 7) == 0)
									strcat(buf, "&R");
								if ((kdx / 7) == 1)
									strcat(buf, "&Y");
								if ((kdx / 7) == 2)
									strcat(buf, "&G");
							}
							strcat(buf, "*");
							lv_color = 1;
						}
						else {
							if (lv_color) {
								lv_color = 0;
								strcat(buf, "&w");
							};
							strcat(buf, "-");
						}

					}
					sprintf(buf + strlen(buf), "&W] &c%s\r\n", GET_REAL_NAME(vict));
					count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s", buf);
					if (count >= 0)
						len += count;
				}
			}
		}
		//********************************
			//Old Graphics Prompt
			// ********************************
			if (!IS_SET(GET_ACT2(ch), PLR2_SHOWNOEXP) && (len < sizeof(p_buf))) {
			if (GET_LEVEL(ch) < IMO_LEV) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len,
						 "&w<&W%d&w>xp ", LEVEL_EXP(GET_LEVEL(ch)) - GET_EXP(ch));
				if (count >= 0)
					len += count;
			}
			else if (GET_LEVEL(ch) == IMO_LEV || GET_LEVEL(ch) == PK_LEV) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&w<&W%d&w>xp ", GET_EXP(ch));
				if (count >= 0)
					len += count;
			}
		}


		switch (ch->points.prompt) {
		case 2:
			//Dark Rainbow
				sprintf(lvc, "CcrRWgGcCyYpPc");
			break;
		case 3:
			//Light Rainbow
				sprintf(lvc, "WcWRWgGcCyYpWc");
			break;
		case 4:
			//cool
				sprintf(lvc, "WCWCCWCWCWCcWC");
			break;
		case 5:
			//hot
				sprintf(lvc, "RrRrrRrRrRrrRr");
			break;
		default:
			//standard
				sprintf(lvc, "ccRRgggggggppc");
			break;

		}


		/* IDLE TICKER */
		if (ch && ch->specials.timer > 4 && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s{IDLE: %d}%s ",
				  lv_white, ch->specials.timer, lv_end_color);
			if (count >= 0)
				len += count;
		}

		/* WIZINVIS? */
		if (GET_VISIBLE(ch) > 0 && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s<%d>%s ",
				      lv_lred, GET_VISIBLE(ch), lv_end_color);
			if (count >= 0)
				len += count;
		}
		/* INCOGNITO? by Shalira 22.07.01 */
		if (GET_INCOGNITO(ch) > 0 && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s<INCOGNITO %d>%s ",
				    lv_gray, GET_INCOGNITO(ch), lv_end_color);
			if (count >= 0)
				len += count;
		}
		/* GHOST? by Shalira 22.07.01 */
		if (GET_GHOST(ch) > 0 && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s<GHOST %d>%s ",
					 lv_gray, GET_GHOST(ch), lv_end_color);
			if (count >= 0)
				len += count;
		}


		/* REBOOT */
		if (gv_terminate_count > 0 && (len < sizeof(p_buf))) {
			if (gv_terminate_count < 31 || GET_LEVEL(ch) >= IMO_IMP) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "%sREBOOT%s_%s%ld%s_%sMIN%s ",
						 lv_red, lv_yellow, lv_red, gv_terminate_count, lv_yellow, lv_red, lv_end_color);
				if (count >= 0)
					len += count;
			}
		}

		/* RIDING */
		if (ch->riding && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "RIDE ");
			if (count >= 0)
				len += count;
		}

		/* HIDING */
		if (IS_AFFECTED(ch, AFF_HIDE) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&b<HIDDEN>%s ", lv_end_color);
			if (count >= 0)
				len += count;
		}

		/* RUSH HOUR */
		if (gv_rush_hour > 1 && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%sRUSH%s ",
					 lv_white, lv_end_color);
			if (count >= 0)
				len += count;
		}

		/* AFK */
		if (IS_SET(GET_ACT2(ch), PLR2_AFK) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s<AFK>%s ",
					 lv_lred, lv_end_color);
			if (count >= 0)
				len += count;
		}

		/* AFW */
		if (IS_SET(GET_ACT2(ch), PLR2_AFW) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "%s<AFW>%s ",
					 lv_lred, lv_end_color);
			if (count >= 0)
				len += count;
		}
		/* SHOW NAME */
		if (IS_SET(GET_ACT2(ch), PLR2_SHOW_NAME) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c[&%c%s&%c] %s",
					 lvc[0], lvc[1], GET_REAL_NAME(ch), lvc[0], lv_end_color);
			if (count >= 0)
				len += count;
		}

		/* SHOW OPPONENT STATUS */
		if (IS_SET(GET_ACT1(ch), PLR1_SHOW_DIAG) && !GET_NEWGRAPHICS(ch) && (len < sizeof(p_buf))) {
			if (GET_FIGHTING(ch)) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c[&%c%s&%c] %s",
						 lvc[2], lvc[3], in1650_do_short_diag(ch, " ", 0), lvc[2], lv_end_color);
				strcpy(gv_str, "BLANK");
				if (count >= 0)
					len += count;
			}
		}

		/* SYSLOG */
		if (gv_syslog_loaded == TRUE && GET_LEVEL(ch) >= IMO_IMP && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "SYSLOG ");
			if (count >= 0)
				len += count;
		}

		/* LOGIN */
		if (gv_highest_login_level != 0 && GET_LEVEL(ch) >= IMO_IMM && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "LOGIN%d ", gv_highest_login_level);
			if (count >= 0)
				len += count;
		}

		/* AUTOLEVEL */
		if (gv_auto_level && GET_LEVEL(ch) >= IMO_IMP && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "AUTLV%d ", gv_auto_level);
			if (count >= 0)
				len += count;
		}

		/* HUNTING */
		if (HUNTING(ch) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "HUNTING ");
			if (count >= 0)
				len += count;
		}


		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_ROOM) && (GET_LEVEL(ch) > 40) && (len < sizeof(p_buf))) {
			bzero(temp_str, sizeof(temp_str));
			if (GET_LEVEL(ch) < IMO_IMP && ch->in_room >= 0 &&
			    world[ch->in_room].number >= VEHICLE_BEG_ROOM &&
			    world[ch->in_room].number <= VEHICLE_END_ROOM) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cr&%c[&%cVEHICLE&%c]%s ",
				lvc[3], lvc[2], lvc[3], lvc[2], lv_end_color);
				if (count >= 0)
					len += count;
			}
			else {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cr&%c[&%c%d&%c]%s ",
						 lvc[3], lvc[2], lvc[3], world[ch->in_room].number, lvc[2], lv_end_color);
				if (count >= 0)
					len += count;
			}

			if (GET_LEVEL(ch) >= IMO_IMP && ch->in_room >= 0 &&
			    world[ch->in_room].number == XANTH_PUZZLE_ROOM) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "p[%d/%d] ",
						 pz_puzzle_row, pz_puzzle_col);
				if (count >= 0)
					len += count;
			}
		}

		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_VEHICLE) && ch->in_room >= 0 && (len < sizeof(p_buf))) {
			strcat(temp_str, "veh[");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_NORTH))
				strcat(temp_str, "N");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_EAST))
				strcat(temp_str, "E");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_SOUTH))
				strcat(temp_str, "S");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_WEST))
				strcat(temp_str, "W");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_UP))
				strcat(temp_str, "U");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_DOWN))
				strcat(temp_str, "D");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_STATION))
				strcat(temp_str, "s");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_REVERSE))
				strcat(temp_str, "r");
			if (IS_SET(GET_ROOM2(ch->in_room), RM2_ROUTE_DEATH))
				strcat(temp_str, "d");
			strcat(temp_str, "] ");

			count = snprintf(p_buf + len, sizeof(p_buf) - len, temp_str);
			if (count >= 0)
				len += count;
		}		/* END OF SHOW_VEHICLE */

		if ((IS_SET(GET_ACT3(ch), PLR3_SHOW_HP) ||
		     IS_SET(GET_ACT3(ch), PLR3_SHOW_MANA) ||
		     IS_SET(GET_ACT3(ch), PLR3_SHOW_MOVE)) && (len < sizeof(p_buf))) {

			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c[", lvc[4]);
			if (count >= 0)
				len += count;
			if (IS_SET(GET_ACT3(ch), PLR3_SHOW_HP)) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cH&%c%d ",
						 lvc[5], lvc[6], GET_HIT(ch));
				if (count >= 0)
					len += count;
			}

			if (IS_SET(GET_ACT3(ch), PLR3_SHOW_MANA)) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cM&%c%d ",
						 lvc[7], lvc[8], GET_MANA(ch));
				if (count >= 0)
					len += count;
			}

			if (IS_SET(GET_ACT3(ch), PLR3_SHOW_MOVE)) {
				count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cV&%c%d ",
					       lvc[9], lvc[10], GET_MOVE(ch));
				if (count >= 0)
					len += count;
			}
			/* CLOSE OFF WITH A ] AND REMOVE TRAILING SPACE */
			len--;
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c]%s ", lvc[4], lv_end_color);
			if (count >= 0)
				len += count;
		}

		if (IS_SET(GET_ACT3(ch), PLR3_SHOW_EXITS) && (len < sizeof(p_buf))) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%cex&%c[&%c", lvc[11], lvc[12], lvc[11]);
			if (count >= 0)
				len += count;
			for (i = 0; i < MAX_DIRS; i++) {
				lv_dir_ptr = world[ch->in_room].dir_option[i];
				if (lv_dir_ptr) {
					if (lv_dir_ptr->to_room != NOWHERE) {
						if (!IS_SET(lv_dir_ptr->exit_info, EX_HIDDEN)) {
							if (IS_SET(lv_dir_ptr->exit_info, EX_CLOSED))
								count = snprintf(p_buf + len, sizeof(p_buf) - len, "%c ", *dirs[i]);
							else
								count = snprintf(p_buf + len, sizeof(p_buf) - len, "%c ", UPPER(*dirs[i]));
							if (count >= 0)
								len += count;
						}
					}	/* END OF not going no where */
				}	/* END OF direction exist */
			}	/* END OF for loop */
			/* CLOSE OFF WITH A ] AND REMOVE TRAILING SPACE */
			if (*(p_buf + len - 1) == 32) {
				*(p_buf + len - 1) = 0;
				len--;
			}
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c]%s ", lvc[12], lv_end_color);
			if (count >= 0)
				len += count;
		}		/* END OF SHOW EXITS */

		/* THROW AN END SYMBOL ON PROMPT LINE */
		if (len < sizeof(p_buf)) {
			count = snprintf(p_buf + len, sizeof(p_buf) - len, "&%c>%s", lvc[13], lv_end_color);
			if (count >= 0)
				len += count;
		}

		count = snprintf(p_buf + len, sizeof(p_buf) - len, "%c", '\0');

		parse_colors(p_buf, sizeof(p_buf), ch, END);
	}
	else
		*p_buf = '\0';

	return (p_buf);

}/* END OF co1100_give_prompts() */


/* Accept new connects, relay commands, and call 'heartbeat-functs' */
int co1200_game_loop(int mother_descs[])
{
	long last_autosave;
	fd_set input_set, output_set, exc_set;
	struct timeval last_time, now, timespent, timeout, null_time;
	static struct timeval opt_time;
	char comm[MAX_INPUT_LENGTH];
	struct descriptor_data *point, *next_point;
	int lv_four_mud_hours, newmail, aliased, socki;

	lv_four_mud_hours = SECS_PER_MUD_HOUR * 16;
	null_time.tv_sec = 0;
	null_time.tv_usec = 0;

	opt_time.tv_usec = OPT_USEC;	/* Init time values */
	opt_time.tv_sec = 0;
	gettimeofday(&last_time, (struct timezone *) 0);
	
	maxdesc = mother_descs[0];

	/* MAXUSERS MAX_USERS */
	avail_descs = MAX_USERS;

	/* Main loop */
	last_autosave = time(0);
	while (!IS_SET(gv_run_flag, RUN_FLAG_TERMINATE)) {

		ha9900_sanity_check(0, "WHILE1", "SYSTEM");

		/* Check what's happening out there */
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		for (socki = 0; socki < mother_descs[MAXSOCK-1]; socki++) {
			FD_SET(mother_descs[socki], &input_set);
			if (mother_descs[socki] > maxdesc)
				maxdesc = mother_descs[socki];
		}
		
		for (point = descriptor_list; point; point = point->next) {
			FD_SET(point->descriptor, &input_set);
			FD_SET(point->descriptor, &exc_set);
			FD_SET(point->descriptor, &output_set);
			if (mother_descs[socki] > maxdesc)
				maxdesc = mother_descs[socki];
		}

		/* check out the time */
		gettimeofday(&now, (struct timezone *) 0);
		timespent = co1600_timediff(&now, &last_time);
		timeout = co1600_timediff(&opt_time, &timespent);
		last_time.tv_sec = now.tv_sec + timeout.tv_sec;
		last_time.tv_usec = now.tv_usec + timeout.tv_usec;
		if (last_time.tv_usec >= 1000000) {
			last_time.tv_usec -= 1000000;
			last_time.tv_sec++;
		}


		if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
			perror("Select poll");
			return (-1);
		}

		if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0) {
			if (errno != EINTR) {
				perror("Select sleep");
				exit(1);
			}
		}
		/* Respond to whatever might be happening */


		/* New connection? */
		for (socki = 0; socki < mother_descs[MAXSOCK-1]; socki++) {
			if (FD_ISSET(mother_descs[socki], &input_set))
				if (co2000_new_descriptor(mother_descs[socki]) < 0)
					perror("New connection");
		}

		/* kick out the freaky folks */
		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &exc_set)) {
				FD_CLR(point->descriptor, &input_set);
				FD_CLR(point->descriptor, &output_set);
				co2500_close_single_socket(point);
			}
		}

		ha9900_sanity_check(0, "WHILE2", "SYSTEM");


		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &input_set)) {
				if (process_input(point) < 0)
					co2500_close_single_socket(point);
			}
		}

		ha9900_sanity_check(0, "WHILE3", "SYSTEM");

		/* handle heartbeat stuff */
		/* Note: pulse now changes every 1/16 sec  */

		/* RESET ALARM JUST BEFORE IT GOES OFF */
		if (gv_infinate_loop_count + ALARM_WAIT < time(0)) {
			if (gv_force_infinate_loop == TRUE ||
			    gv_port != DEVELOPMENT_PORT) {
				gv_infinate_loop_count = time(0);
			}
		}

		pulse++;

		if (!(pulse % PULSE_ZONE)) {
			db5500_zone_update();
		}

		ha9900_sanity_check(0, "WHILE 3.25", "SYSTEM");
		if (!(pulse % PULSE_MOBILE)) {
			mb1000_mobile_activity();
		}
		ha9900_sanity_check(0, "WHILE 3.75", "SYSTEM");

		if (!(pulse % PULSE_OBJECTS)) {
			oa1000_object_activity();
		}

		if (!(pulse % lv_four_mud_hours)) {

			if (!IS_SET(gv_run_flag, RUN_FLAG_NOWEATHER))
				weather_and_time(1);
			affect_update();
			li2500_point_update();
			if (weather_info.sky == SKY_LIGHTNING) {
				pr1000_lightning_strikes(0);
			}
		}

		if (gv_terminate_count) {
			if (!(pulse % 960)) {
				/* CHECK FOR REBOOTS - MINUTE INCREMENTS */
				check_reboot();
			}
		}


		/* CHECK QUEST TIMERS (45 seconds) */
		if (!(pulse % 720)) {
			qu1300_quest_update();
			ot5000_transform_update();
		}


		/* CHECK FOR RUSH HOUR & AUCTION SYSTEM */
		if (!(pulse % 16)) {

			/* auction */

			au1500_counter();

			/* rush */

			if (gv_rush_time < time(0)) {
				if (gv_rush_hour == 1) {
					gv_rush_hour = 2;
					gv_rush_time = time(0) + number(RUSH_TIME, RUSH_TIME2) * 60 - 1;
					do_info_noch("Rush hour has started!", 0, 99);
					main_log("Rush Started");
					//char buf[MAX_STRING_LENGTH];

					//Enables Questmaster transformation sales
						if (isQuestTransformOn) {
						int roll = dice(1, 101);

						if (roll >= QMSALES_MIN_CHANCE && roll <= QMSALES_MAX_CHANCE) {
							isQuestAutoOn = 1;
							main_log("Questmaster transformation sales is Enabled");
						}	/* end if */
					}	/* end if */
				}
				else {
					gv_rush_hour
						= 1;
					gv_rush_time = time(0) + RUSH_INTERVAL * 60 +
						number(1, RUSH_INTERVAL2) * 60;
					do_info_noch("Rush hour has ended.", 0, 99);

					//disables QM transformation sales
						if (isQuestAutoOn == 1) {
						main_log("Questmaster transformation sales is Disabled");
						main_log("Rush Finished");
						isQuestAutoOn = 0;
					} //end if
				}
			}

		}
		/* WE COME THROUGH THIS LOOP EVERY Minute */
		if (!(pulse % 960)) {
			co3500_clear_user_list();
		}

		if (!(pulse % 9600)) {
			/* Check for new mail. */
			for (point = descriptor_list; point; point = next_to_process) {
				next_to_process = point->next;

				if (point->character && !point->original && point->connected == CON_PLAYING) {
					if ((newmail = new_message(point->character)) > 0)
						co2900_send_to_char(point->character, "&YYou have %d new message%s!\r\n",
								    newmail, (newmail > 1) ? "s" : "");
				}
			}
		}

		tics++;		/* tics since last checkpoint  */

		/* process_commands: NEED TO DO THIS BETTER, CLEANER! */
		for (point = descriptor_list; point; point = next_to_process) {
			next_to_process = point->next;

			if ((--(point->wait) <= 0) && co1300_get_from_q(&point->input, comm, &aliased)) {
				if (point->character && point->connected == CON_PLAYING
				    && point->character->specials.was_in_room != NOWHERE) {
					if (point->character->in_room != NOWHERE)
						ha1500_char_from_room(point->character);
					ha1600_char_to_room(point->character, point->character->specials.was_in_room);
					point->character->specials.was_in_room = NOWHERE;
					act("$n has returned.", TRUE, point->character, 0, 0, TO_ROOM);
				}

				point->wait = 1;
				if (point->character) {
					point->character->specials.timer = 0;
					point->has_prompt = FALSE;
				}

				if (point->mailedit && (point->mailto != -1)) {
					ma1350_handle_mail_input(point, comm);
				}
				else if (point->str)
					string_add(point, comm);
				else if (STATE(point) != CON_PLAYING)
					nanny(point, comm);
				else if (!point->connected) {
					if (point->showstr_count)
						show_string(point, comm);
					else {

						/* STORE LAST COMMAND FOR DEBUG
						 * PURPOSES * I suppose this'll
						 * stay in, for now -- relic */
						bzero(gv_str, sizeof(gv_str));
						gv_ch_ptr = point->character;
						sprintf(gv_str, "*r%5d %10s ",
							world[gv_ch_ptr->in_room].number,
						      gv_ch_ptr->player.name);
						strncat(gv_str, comm, sizeof(gv_str) - strlen(gv_str) - 5);


						if (aliased)
							point->has_prompt = TRUE;
						else if (perform_alias(point, comm, sizeof(comm)))
							co1300_get_from_q(&point->input, comm, &aliased);
						command_interpreter(point->character, comm);
					}
				}
			}
		}

		/* moved fighting so that spells commands come b4 it */
		if (!(pulse % PULSE_VIOLENCE)) {
			ft3100_perform_violence();
		}

		/* Move vehicles around the mud */
		if (!(pulse % PULSE_VEHICLE)) {
			vh2000_move_vehicles();
		}

		/* HUNT */
		if (!(pulse % PULSE_HUNT)) {
			perform_hunt();
		}

		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &output_set) && *(point->output)) {
				co2100_process_output(point);
				if (point->bufptr == 0)
					point->has_prompt = TRUE;
			}
		}

		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (!point->has_prompt && point->bufptr == 0) {
				co2200_write_to_descriptor(point->descriptor, co1100_give_prompt(point));
				point->has_prompt = TRUE;
			}
		}


		if (IS_SET(gv_run_flag, RUN_FLAG_TERMINATE)) {
			wi4100_do_crashsave(0, 0, 0);
		}

		/* Autosave routine */
		if (time(0) - last_autosave > AUTOSAVE_DELAY) {
			struct obj_cost cost;

			cost.total_cost = 0;
			cost.no_carried = 0;
			cost.ok = TRUE;
			last_autosave = time(0);

			/* HAVE THE USERS TIMED OUT? */
			for (point = descriptor_list; point; point = point->next) {
				if (point->connected == CON_LINK_DEAD ||
				    point->connected == CON_PLAYING) {
					if (point->connected == CON_LINK_DEAD) {
						GET_HOW_LEFT(point->character) = LEFT_BY_LINK_DEATH;
						if (gv_mega_verbose_messages == TRUE) {
							main_log("Leaving by LINK DEATH.");
							spec_log("Leaving by LINK DEATH.", CONNECT_LOG);
						}
					}
					else {
						GET_HOW_LEFT(point->character) = LEFT_BY_CRASH;
						if (gv_mega_verbose_messages == TRUE) {
							main_log("Leaving by CRASH.");
							spec_log("Leaving by CRASH.", CONNECT_LOG);
						}
					}
					/* AUTO SAVE CHARS - EVEN THOSE WHO ARE
					 * LINK DEAD */
					r4000_save_obj(point->character, &cost);
					db6400_save_char(point->character, NOWHERE);
					if (IS_SET(gv_run_flag, RUN_FLAG_TERMINATE)) {
						co2200_write_to_descriptor(point->descriptor, "Mud is rebooting!\n\r");
					}
				}	/* END OF link_dead OR playing */
			}	/* END OF descriptor FOR loop */
		}		/* end autosave */
		//co1100_give_prompts();

		ha9900_sanity_check(0, "WHILE5", "SYSTEM");


	}			/* END OF while(!terminate) loop */

	return 0;		/* exit normally */

}				/* END OF co1200_game_loop() */


/* ******************************************************************
*  general utility stuff (for local use)                           *
****************************************************************** */
void co1250_increase_max_users()
{

#if defined (DIKU_WINDOWS)
	return;
#else
	int idx;
	char buf[MAX_STRING_LENGTH];
	struct rlimit lv_rlp;

	/* PUSH DEFAULT NUMBER OF USERS UP */
	idx = getrlimit(RLIMIT_NOFILE, &lv_rlp);
	if (idx != 0) {
		sprintf(buf, "ERROR in getrlimit: %d errno: %d", idx, errno);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
	}
	lv_rlp.rlim_cur = MAX_USERS;
	idx = setrlimit(RLIMIT_NOFILE, &lv_rlp);
	if (idx != 0) {
		sprintf(buf, "ERROR in setrlimit: %d errno: %d", idx, errno);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
	}
	idx = getrlimit(RLIMIT_NOFILE, &lv_rlp);
	sprintf(buf, "Max users set to: %ld", (long int) lv_rlp.rlim_cur);
	main_log(buf);
	spec_log(buf, SYSTEM_LOG);

	return;
#endif

}				/* END OF co1250_increase_max_users() */


int co1300_get_from_q(struct txt_q * queue, char *dest, int *aliased)
{
	struct txt_block *tmp;

	/* Q empty? */
	if (!queue->head)
		return (0);

	/* ADDED THIS FIX  95/08/14 */
	if (queue->head == queue->tail) {
		queue->tail = 0;
	}

	tmp = queue->head;
	strcpy(dest, queue->head->text);
	*aliased = queue->head->aliased;
	queue->head = queue->head->next;

	free(tmp->text);
	free(tmp);

	return (1);
}

size_t vwrite_to_output(struct descriptor_data * t, const char *color, const char *format, va_list args)
{
	static char txt[MAX_STRING_LENGTH];
	struct txt_block *point;
	size_t wantsize;
	int size;

	/* if we're in the overflow state already, ignore new messages */
	if (t->bufspace == 0)
		return 0;

	bzero(txt, sizeof(txt));
	wantsize = size = vsnprintf(txt, sizeof(txt), format, args);
	if (t->character && strchr(txt, '&'))
		wantsize = size = parse_colors(txt, sizeof(txt), t->character, color);
	if (size < 0 || wantsize > sizeof(txt)) {
		size = sizeof(txt) - 1;
		strcpy(txt + size - strlen("Sorry, overflow occured!\r\n"), "Sorry, overflow occured!\r\n");
	}

	/*
   * If the text is too big to fit into even a large buffer, truncate
   * the new text to make it fit.  (This will switch to the overflow
   * state automatically because t->bufspace will end up 0.)
   */
	if (size + t->bufptr + 1 > LARGE_BUFSIZE) {
		size = LARGE_BUFSIZE - t->bufptr - 1;
		txt[size] = '\0';
		buf_overflows++;
	}

	/*
         * If we have enough space, just write to buffer and that's it! If the
         * text just barely fits, then it's switched to a large buffer instead.
         */
	if (t->bufspace > size) {
		strcpy(t->output + t->bufptr, txt);	/* strcpy: OK (size
							 * checked above) */
		t->bufspace -= size;
		t->bufptr += size;
		return (t->bufspace);
	}

	buf_switches++;

	if (t->large_outbuf) {
		point = t->large_outbuf;
		if (bufpool != NULL) {	/* if the pool has a buffer in it, grab
					 * it */
			t->large_outbuf = bufpool;
			bufpool = bufpool->next;
		}
		else {		/* else create a new one */
			CREATE(t->large_outbuf, struct txt_block, 1);
			CREATE(t->large_outbuf->text, char, LARGE_BUFSIZE);
			buf_largecount++;
		}
		point->next = bufpool;
		bufpool = point;
	}
	else {
		if (bufpool != NULL) {	/* if the pool has a buffer in it, grab
					 * it */
			t->large_outbuf = bufpool;
			bufpool = bufpool->next;
		}
		else {		/* else create a new one */
			CREATE(t->large_outbuf, struct txt_block, 1);
			CREATE(t->large_outbuf->text, char, LARGE_BUFSIZE);
			buf_largecount++;
		}
	}

	strcpy(t->large_outbuf->text, t->output);	/* strcpy: OK (size
							 * checked previously) */
	t->output = t->large_outbuf->text;	/* make big buffer primary */
	strcat(t->output, txt);	/* strcat: OK (size checked) */

/* set the pointer for the next write */
	t->bufptr = strlen(t->output);

/* calculate how much space is left in the buffer */
	t->bufspace = LARGE_BUFSIZE - 1 - t->bufptr;

	return (t->bufspace);
}


/* Add a new string to a player's queue, for outside use */
size_t write_to_output(struct descriptor_data * d, const char *color, const char *txt,...)
{
	va_list args;
	size_t left;
	va_start(args, txt);
	left = vwrite_to_output(d, color, txt, args);
	va_end(args);

	return left;
}

void co1400_write_to_q(char *txt, struct txt_q * queue, int aliased)
{

	struct txt_block *new;

	CREATE(new, struct txt_block, 1);
	new->text = strdup(txt);
	new->aliased = aliased;

	new->beg_rec_id = QUEUE_SANITY_CHECK;
	strcpy(new->text, txt);
	new->end_rec_id = QUEUE_SANITY_CHECK;

	/* Q empty? */
	if (!queue->head) {
		new->next = NULL;
		queue->head = queue->tail = new;
	}
	else {
		queue->tail->next = new;
		queue->tail = new;
		new->next = NULL;
	}
}				/* END OF co1400_write_to_q() */


void co1500_write_to_head_q(char *txt, struct txt_q * queue)
{

	struct txt_block *new;

	ha9925_queue_check(queue, "wri_que_head 01", "SYSTEM");

	CREATE(new, struct txt_block, 1);
	CREATE(new->text, char, strlen(txt) + 1);

	new->beg_rec_id = QUEUE_SANITY_CHECK;
	strcpy(new->text, txt);
	new->end_rec_id = QUEUE_SANITY_CHECK;

	/* Q empty? */
	if (!queue->head) {
		new->next = NULL;
		queue->head = queue->tail = new;
	}
	else {
		new->next = queue->head;
		queue->head = new;
	}

	ha9925_queue_check(queue, "wri_que_head 02", "SYSTEM");

	return;

}				/* END OF co1500_write_to_head_q() */


struct timeval co1600_timediff(struct timeval * a, struct timeval * b)
{

	struct timeval rslt, tmp;

	tmp = *a;

	if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0) {
		rslt.tv_usec += 1000000;
		--(tmp.tv_sec);
	}
	if ((rslt.tv_sec = tmp.tv_sec - b->tv_sec) < 0) {
		rslt.tv_usec = 0;
		rslt.tv_sec = 0;
	}
	return (rslt);
}
/* Empty the queues before closing connection */
void co1700_flush_queues(struct descriptor_data * d)
{
	char dummy[MAX_STRING_LENGTH];
	int aliased;

	if (d->large_outbuf) {
		d->large_outbuf->next = bufpool;
		bufpool = d->large_outbuf;
	}
	while (co1300_get_from_q(&d->input, dummy, &aliased));
}				/* co1700_flush_queues */


/* ******************************************************************
*  socket handling                      *
******************************************************************* */

void co1800_init_socket(ush_int port, int s[])
{
	struct addrinfo hints, *res, *res0;
	int error, nsock;
  	char pbuf[5];
  
  	snprintf(pbuf, 5, "%d", port);
	
  	memset(&hints, 0, sizeof(hints));
	
  	hints.ai_family = PF_UNSPEC;
  	hints.ai_socktype = SOCK_STREAM;
  	hints.ai_flags = AI_PASSIVE;
  	error = getaddrinfo(NULL, pbuf, &hints, &res0);
  	if (error) {
    	perror("SYSERR: getaddrinfo()");
    	exit(1);
  	}
  
  	for (nsock = 0; nsock < MAXSOCK; nsock++) {
    	s[nsock] = INVALID_SOCKET;
  	}
	
  	for (res = res0, nsock = 0; res && nsock < MAXSOCK - 1; res = res->ai_next) {
    	s[nsock] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    	if (s[nsock] == INVALID_SOCKET) {
			continue;
   		}
    
		set_sendbuf(s[nsock]);

		if (bind(s[nsock], res->ai_addr, res->ai_addrlen) < 0) {
			close(s[nsock]);
			s[nsock] = INVALID_SOCKET;
			continue;
		}
    
		co2600_nonblock(s[nsock]);
    
		if (listen(s[nsock], 5) < 0) {
			close(s[nsock]);
			s[nsock] = INVALID_SOCKET;
			continue;
		}
		nsock++;
	}
  	
	freeaddrinfo(res0);
	
	if (nsock == 0 && s[nsock] < 0) {
		perror("Error creating server socket(s)!");
		exit(1);
	}
	else if (s[nsock] < 0) {
		nsock--;
	}
  
	s[MAXSOCK - 1] = nsock + 1;
}

int set_sendbuf(int s)
{
  int opt = MAX_SOCK_BUF;

  if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
    perror("SYSERR: setsockopt SNDBUF");
    return (-1);
  }

  return (0);
}


int co2000_new_descriptor(int s)
{
	int desc, rc, flags = 0;
	socklen_t i;
	char *greeting;
	char greeting_buf[MAX_STRING_LENGTH];
	char hostname[NI_MAXHOST];

	struct descriptor_data *newd;
  	struct sockaddr_storage peer;
	
	i = sizeof(peer);
  	if ((desc = accept(s, (struct sockaddr *) &peer, &i)) == INVALID_SOCKET) {
    	perror("SYSERR: accept");
		return (-1);
  	}

	co2600_nonblock(desc);

	if ((maxdesc + 1) >= avail_descs) {
		co2200_write_to_descriptor(desc, "Sorry.. The game is full...\n\r");
		close(desc);
		return (0);
	}
	else {
		if (desc > maxdesc) {
			maxdesc = desc;
		}
	}

	CREATE(newd, struct descriptor_data, 1);
	*newd->host = '\0';

	if (gv_use_nameserver)
	/*	flags = NI_NUMERICHOST; */
		
	
	if ((rc = getnameinfo((struct sockaddr *) &peer, i, hostname, NI_MAXHOST, 
	                           NULL, 0, flags)) != 0) {
		perror("getnameinfo");
		*newd->host = '\0';
	}

	strncpy(newd->host, hostname, 50); // From structs.h....
	*(newd->host + 50) = '\0';
	
	/* init desc data */
	newd->descriptor = desc;
	newd->connected = 1;
	newd->wait = 1;
	newd->prompt_mode = 0;
	newd->prompt_cr = 1;
	newd->str = 0;
	newd->showstr_head = 0;
	*newd->curr_input = '\0';
	*newd->last_input = '\0';
	newd->output = newd->small_outbuf;
	newd->bufspace = SMALL_BUFSIZE - 1;
	*newd->output = '\0';
	newd->input.head = NULL;
	newd->next = descriptor_list;
	CREATE(newd->history, char *, HISTORY_SIZE);
	newd->character = 0;
	newd->original = 0;
	newd->snoop.snooping = 0;
	newd->snoop.snoop_by = 0;

	/* prepend to list */
	descriptor_list = newd;
	
	/* Send greetings and login messages. */
	sprintf(greeting_buf, "greetings/greeting.%d", number(1, GREETING_SCREENS));
	greeting = db7300_file_to_string(greeting_buf);
	write_to_output(newd, 0, "%s", greeting);
	free(greeting);
	/* PRINT LOGIN MESSAGE */
	if (*gv_login_message) {
		SEND_TO_Q("\r\n", newd);
		SEND_TO_Q("LOGIN MSG: ", newd);
		write_to_output(newd, 0, "%s", gv_login_message);
		SEND_TO_Q("\r\n\r\n", newd);
	}

	SEND_TO_Q("By what name do you wish to be known? ", newd);

	newd->connected_at = time(0);

	return (0);
}

int co2100_process_output(struct descriptor_data * t)
{
	char i[MAX_SOCK_BUF], *osb = i + 2;
	int result;

	/* we may need this \r\n for later -- see below */
	strcpy(i, "\r\n");	/* strcpy: OK (for 'MAX_SOCK_BUF >= 3') */

	/* now, append the 'real' output */
	strcpy(osb, t->output);	/* strcpy: OK (t->output:LARGE_BUFSIZE <
				 * osb:MAX_SOCK_BUF-2) */

	/* if we're in the overflow state, notify the user */
	if (t->bufspace == 0)
		strcat(osb, END "**OVERFLOW**\r\n");	/* strcpy: OK
							 * (osb:MAX_SOCK_BUF-2
							 * reserves space) */

	/* add the extra CRLF if the person isn't in compact mode */
	if (!t->connected && !(t->character && !IS_NPC(t->character) &&
			       IS_SET(GET_ACT3(t->character), PLR3_COMPACT)))
		strcat(osb, "\r\n");	/* strcpy: OK (osb:MAX_SOCK_BUF-2
					 * reserves space) */

	/* add a prompt */
	strcat(i, co1100_give_prompt(t));	/* strcpy: OK (i:MAX_SOCK_BUF
						 * reserves space) */

	/*
	       * now, send the output.  If this is an 'interruption', use the prepended
	       * CRLF, otherwise send the straight output sans CRLF.
	       */
	if (t->has_prompt) {
		t->has_prompt = FALSE;
		result = co2200_write_to_descriptor(t->descriptor, i);
		if (result >= 2)
			result -= 2;
	}
	else
		result = co2200_write_to_descriptor(t->descriptor, osb);

	if (result < 0) {	/* Oops, fatal error. Bye! */
		co2500_close_single_socket(t);
		return (-1);
	}
	else if (result == 0)	/* Socket buffer full. Try later. */
		return (0);

	/* Handle snooping: prepend "% " and send to snooper. */
	/* ok t->snoop.snoopby exist */
	if (t->snoop.snoop_by) {
		if (t->snoop.snoop_by->desc) {
			write_to_output(t->snoop.snoop_by->desc, 0, "%% %*s%%%%", result, t->output);
		}
		else {
			main_log("MEMORY TRACE:  line 1997 info.c -- desc DNE");
		}
	}
	/* The common case: all saved output was handed off to the kernel
	 * buffer. */
	if (result >= t->bufptr) {
		/*
		       * if we were using a large buffer, put the large buffer on the buffer pool
		       * and switch back to the small one
		       */
		if (t->large_outbuf) {
			t->large_outbuf->next = bufpool;
			bufpool = t->large_outbuf;
			t->large_outbuf = NULL;
			t->output = t->small_outbuf;
		}
		/* reset total bufspace back to that of a small buffer */
		t->bufspace = SMALL_BUFSIZE - 1;
		t->bufptr = 0;
		*(t->output) = '\0';

		/*
		       * If the overflow message or prompt were partially written, try to save
		       * them. There will be enough space for them if this is true.  'result'
		       * is effectively unsigned here anyway.
		       */
		if ((unsigned int) result < strlen(osb)) {
			size_t savetextlen = strlen(osb + result);

			strcat(t->output, osb + result);
			t->bufptr -= savetextlen;
			t->bufspace += savetextlen;
		}

	}
	else {
		/* Not all data in buffer sent.  result < output buffersize. */

		strcpy(t->output, t->output + result);	/* strcpy: OK (overlap) */
		t->bufptr -= result;
		t->bufspace += result;
	}

	return (result);
}

ssize_t perform_socket_write(int desc, const char *txt, size_t length)
{
	ssize_t result;

	result = write(desc, txt, length);

	if (result > 0) {
		/* Write was successful. */
		return (result);
	}

	if (result == 0) {
		/* This should never happen! */
		return (-1);
	}

	/*
         * result < 0, so an error was encountered - is it transient?
         */
#ifdef EAGAIN
	if (errno == EAGAIN)
		return (0);
#endif

#ifdef EWOULDBLOCK
	if (errno == EWOULDBLOCK)
		return (0);
#endif

#ifdef EDEADLK			/* Macintosh */
	if (errno == EDEADLK)
		return (0);
#endif

	/* Looks like the error was fatal.  Too bad. */
	return (-1);
}


int co2200_write_to_descriptor(int desc, char *txt)
{
	ssize_t bytes_written;
	size_t total = strlen(txt), write_total = 0;

	while (total > 0) {
		bytes_written = perform_socket_write(desc, txt, total);

		if (bytes_written < 0) {
			/* Fatal error.  Disconnect the player. */
			perror("SYSERR: Write to socket");
			return (-1);
		}
		else if (bytes_written == 0) {
			/* Temporary failure -- socket buffer full. */
			return (write_total);
		}
		else {
			txt += bytes_written;
			total -= bytes_written;
			write_total += bytes_written;
		}
	}

	return (write_total);
}				/* END OF co2200_write_to_descriptor */


ssize_t perform_socket_read(int desc, char *read_point, size_t space_left)
{
	ssize_t ret;

#if defined(DIKU_WINDOWS)
	ret = recv(desc, read_point, space_left, 0);
#else
	ret = read(desc, read_point, space_left);
#endif

	/* Read was successful. */
	if (ret > 0)
		return (ret);

	/* read() returned 0, meaning we got an EOF. */
	if (ret == 0) {
		return (-1);
	}

	/*
         * read returned a value < 0: there was an error
         */

#if defined(DIKU_WINDOWS)	/* Windows */
	if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
		return (0);
#else

#ifdef EINTR			/* Interrupted system call - various platforms */
	if (errno == EINTR)
		return (0);
#endif

#ifdef EAGAIN			/* POSIX */
	if (errno == EAGAIN)
		return (0);
#endif

#ifdef EWOULDBLOCK		/* BSD */
	if (errno == EWOULDBLOCK)
		return (0);
#endif				/* EWOULDBLOCK */

#ifdef EDEADLK			/* Macintosh */
	if (errno == EDEADLK)
		return (0);
#endif

#ifdef ECONNRESET
	if (errno == ECONNRESET)
		return (-1);
#endif

#endif				/* CIRCLE_WINDOWS */

	/*
         * We don't know what happened, cut them off. This qualifies for
         * a SYSERR because we have no idea what happened at this point.
         */
	perror("SYSERR: perform_socket_read: about to lose connection");
	return (-1);
}

int process_input(struct descriptor_data * t)
{
	int buf_length, failed_subst;
	ssize_t bytes_read;
	size_t space_left;
	char *ptr, *read_point, *write_point, *nl_pos = NULL;
	char tmp[MAX_INPUT_LENGTH];

	/* first, find the point where we left off reading data */
	buf_length = strlen(t->curr_input);
	read_point = t->curr_input + buf_length;
	space_left = MAX_RAW_INPUT_LENGTH - buf_length - 1;

	do {
		if (space_left <= 0) {
			return (-1);
		}

		bytes_read = perform_socket_read(t->descriptor, read_point, space_left);

		if (bytes_read < 0)	/* Error, disconnect them. */
			return (-1);
		else if (bytes_read == 0)	/* Just blocking, no problems. */
			return (0);

		/* at this point, we know we got some data from the read */

		*(read_point + bytes_read) = '\0';	/* terminate the string */

		/* search for a newline in the data we just read */
		for (ptr = read_point; *ptr && !nl_pos; ptr++)
			if (ISNEWL(*ptr))
				nl_pos = ptr;

		read_point += bytes_read;
		space_left -= bytes_read;
	} while (nl_pos == NULL);
	/*
         * okay, at this point we have at least one newline in the string; now we
         * can copy the formatted data to a new array for further processing.
         */

	read_point = t->curr_input;

	while (nl_pos != NULL) {
		write_point = tmp;
		space_left = MAX_INPUT_LENGTH - 1;

		/* The '> 1' reserves room for a '$ => $$' expansion. */
		for (ptr = read_point; (space_left > 1) && (ptr < nl_pos); ptr++) {
			if (*ptr == '\b' || *ptr == 127) {	/* handle backspacing or
								 * delete key */
				if (write_point > tmp) {
					if (*(--write_point) == '$') {
						write_point--;
						space_left += 2;
					}
					else
						space_left++;
				}
			}
			else if (isascii(*ptr) && isprint(*ptr)) {
				if ((*(write_point++) = *ptr) == '$') {	/* copy one character */
					*(write_point++) = '$';	/* if it's a $, double
								 * it */
					space_left -= 2;
				}
				else
					space_left--;
			}
		}

		*write_point = '\0';

		if ((space_left <= 0) && (ptr < nl_pos)) {
			char buffer[MAX_INPUT_LENGTH + 64];

			snprintf(buffer, sizeof(buffer), "Line too long.  Truncated to:\r\n%s\r\n", tmp);
			if (co2200_write_to_descriptor(t->descriptor, buffer) < 0)
				return (-1);
		}

		if (t->snoop.snoop_by)
			write_to_output(t->snoop.snoop_by->desc, 0, "%% %s\r\n", tmp);

		failed_subst = 0;

		if (*tmp == '!' && !(*(tmp + 1)))	/* Redo last command. */
			strcpy(tmp, t->last_input);	/* strcpy: OK (by mutual
							 * MAX_INPUT_LENGTH) */
		else if (*tmp == '!' && *(tmp + 1)) {
			char *commandln = (tmp + 1);
			int starting_pos = t->history_pos, cnt = (t->history_pos == 0 ? HISTORY_SIZE - 1 : t->history_pos - 1);

			skip_spaces(commandln);
			for (; cnt != starting_pos; cnt--) {
				if (t->history[cnt] && is_abbrev(commandln, t->history[cnt])) {
					strcpy(tmp, t->history[cnt]);	/* strcpy: OK (by mutual
									 * MAX_INPUT_LENGTH) */
					strcpy(t->last_input, tmp);	/* strcpy: OK (by mutual
									 * MAX_INPUT_LENGTH) */
					write_to_output(t, 0, "%s\r\n", tmp);
					break;
				}
				if (cnt == 0)	/* At top, loop to bottom. */
					cnt = HISTORY_SIZE;
			}
		}
		else {
			strcpy(t->last_input, tmp);	/* strcpy: OK (by mutual
							 * MAX_INPUT_LENGTH) */
			if (t->history[t->history_pos])
				free(t->history[t->history_pos]);	/* Clear the old line. */
			t->history[t->history_pos] = strdup(tmp);	/* Save the new. */
			if (++t->history_pos >= HISTORY_SIZE)	/* Wrap to top. */
				t->history_pos = 0;
		}

		if (!failed_subst)
			co1400_write_to_q(tmp, &t->input, 0);

		/* find the end of this line */
		while (ISNEWL(*nl_pos))
			nl_pos++;

		/* see if there's another newline in the input buffer */
		read_point = ptr = nl_pos;
		for (nl_pos = NULL; *ptr && !nl_pos; ptr++)
			if (ISNEWL(*ptr))
				nl_pos = ptr;
	}

/* now move the rest of the buffer up to the beginning for the next pass */
	write_point = t->curr_input;
	while (*read_point)
		*(write_point++) = *(read_point++);
	*write_point = '\0';

	return (1);
}				/* END OF process_input() */


void co2400_close_all_sockets(int mother_descs[])
{
	int i;
	struct descriptor_data *d;

	main_log("Closing all sockets.");
	spec_log("Closing all sockets.", SYSTEM_LOG);

	while ((d = descriptor_list)) {
		if (d->connected == CON_PLAYING) {
			if (GET_HOW_LEFT(d->character) == LEFT_BY_CRASH) {
				GET_HOW_LEFT(d->character) =
					LEFT_BY_SHUTDOWN;
				if (gv_mega_verbose_messages == TRUE) {
					main_log("Leaving by shutdown");
					spec_log("Leaving by shutdown", CONNECT_LOG);
				}
			}
		}
		co2500_close_single_socket(descriptor_list);
	}

	for (i = 0; i < mother_descs[MAXSOCK-1]; i++) {
		close(mother_descs[i]);		
	}
}


void co2500_close_single_socket(struct descriptor_data * d)
{

	struct descriptor_data *lv_temp;
	char buf[MAX_STRING_LENGTH];

#if defined (DIKU_WINDOWS)
	closesocket(d->descriptor);
#else
	close(d->descriptor);
#endif
	co1700_flush_queues(d);
	if (d->descriptor == maxdesc)
		--maxdesc;

	/* Forget snooping */
	if (d->snoop.snooping)
		d->snoop.snooping->desc->snoop.snoop_by = 0;

	if (d->snoop.snoop_by) {
		send_to_char("Your victim is no longer among us.\n\r", d->snoop.snoop_by);
		d->snoop.snoop_by->desc->snoop.snooping = 0;
	}

	if (d->character) {
		if (d->connected == CON_PLAYING ||
		    d->connected == CON_QUIT) {
			if (GET_HOW_LEFT(d->character) == LEFT_BY_CRASH) {
				GET_HOW_LEFT(d->character) =
					LEFT_BY_LINK_DEATH;
			}
			db6400_save_char(d->character, NOWHERE);
			act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
			sprintf(buf, "Closing link to: %s.", GET_NAME(d->character));
			do_connect(buf, GET_LEVEL(d->character), d->character);
			d->character->desc = 0;
			/* give them a little while to reconnect del_char_objs
			 * moved to limits.c when they time out */
		}
		else {
			if (!(d->character->player.name) || !*(d->character->player.name)) {
				sprintf(buf, "Losing player: {NULL} CONNECT %d.",
					d->connected);
			}
			else {
				sprintf(buf, "Losing player: %s CONNECT %d.",
					GET_NAME(d->character),
					d->connected);
			}

			main_log(buf);
			spec_log(buf, CONNECT_LOG);
			db7100_free_char(d->character, 1);
		}
	}
	else {
		sprintf(buf, "Losing player: <NULL> CONNECTED: %d",
			d->connected);
		main_log(buf);
		spec_log(buf, CONNECT_LOG);
	}

	gv_way_player_left_game = 0;

	if (next_to_process == d)	/* to avoid crashing the process loop */
		next_to_process = next_to_process->next;

	/* REMOVE FROM DESCRIPTOR LIST */
	REMOVE_FROM_LIST(d, descriptor_list, 2);

	if (d->showstr_head) {
		free(d->showstr_head);
	}
	free(d);
}


void co2600_nonblock(int s)
{
#if defined (DIKU_WINDOWS)
	unsigned long nonblocking = 1;

	if (ioctlsocket(s, FIONBIO, &nonblocking) != 0) {
		perror("ioctlsocket() failed");
		exit(1);
	}

#else
	int flags;

	flags = fcntl(s, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if (fcntl(s, F_SETFL, flags) < 0) {
		perror("nonblock");
		exit(1);
	}
#endif
}


/* ****************************************************************
*  Public routines for system-to-player-communication          *
**************************************************************** */



void send_to_char(char *message, struct char_data * ch)
{

	char buf[MAX_STRING_LENGTH];

	/* IF THERE ISN'T A MESSAGE - ITS BAD!!! */
	if (!(message) || !(*message)) {
		sprintf(buf, "ERROR: Got a zerolength message string. gv_str=%s",
			gv_str);
		main_log(buf);
		main_log(gv_str);
		spec_log(buf, ERROR_LOG);
		spec_log(gv_str, ERROR_LOG);
		return;
	}

	if (ch == NULL) {
		sprintf(buf, "ERROR: Got a null ch. gv_str=%s",
			gv_str);
		main_log(buf);
		main_log(gv_str);
		spec_log(buf, ERROR_LOG);
		spec_log(gv_str, ERROR_LOG);
		return;
	}

	co2900_send_to_char(ch, "%s", message);

}/* END OF send_to_char() */

size_t co2900_send_to_char(struct char_data * ch, const char *messg,...)
{
	if (ch && ch->desc && messg && *messg) {
		size_t left;
		va_list args;

		va_start(args, messg);
		left = vwrite_to_output(ch->desc, 0, messg, args);
		va_end(args);
		return left;
	}
	return 0;
}

size_t co2901_ansi_send_to_char(struct char_data * ch, const char *ansi, const char *messg,...)
{
	if (ch && ch->desc && messg && *messg) {
		size_t left;
		va_list args;

		if (ansi && *ansi) {
			if (IS_SET(GET_ACT3(ch), PLR3_ANSI))
				write_to_output(ch->desc, 0, ansi);
		}

		va_start(args, messg);
		left = vwrite_to_output(ch->desc, ansi, messg, args);
		va_end(args);

		if (ansi && *ansi)
			if (IS_SET(GET_ACT3(ch), PLR3_ANSI))
				write_to_output(ch->desc, 0, END);

		return left;
	}
	return 0;
}


void co3000_send_to_all(char *messg)
{
	struct descriptor_data *i;

	if (messg)
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected)
				write_to_output(i, 0, messg);
}


void co3100_send_to_outdoor(char *messg)
{
	struct descriptor_data *i;

	if (messg)
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected)
				if (OUTSIDE(i->character))
					write_to_output(i, 0, messg);
}


void co3200_send_to_except(char *messg, struct char_data * ch)
{
	struct descriptor_data *i;

	if (messg && ch)
		for (i = descriptor_list; i; i = i->next)
			if (ch->desc != i && !i->connected)
				write_to_output(i, 0, messg);
}



void co3300_send_to_room(char *messg, int room)
{
	struct char_data *i;

	if (messg && room > -1)
		for (i = world[room].people; i; i = i->next_in_room)
			if (i->desc)
				write_to_output(i->desc, 0, messg);
}




void co3425_send_except_room(char *messg, int room, struct char_data * ch)
{
	struct char_data *i;

	if (messg && ch && room > -1)
		for (i = world[room].people; i; i = i->next_in_room)
			if (i != ch && i->desc)
				write_to_output(i->desc, 0, messg);
}

void co3450_send_to_except_room_two
  (char *messg, int room, struct char_data * cha1, struct char_data * cha2) {
	struct char_data *i;

	if (messg && cha1 && cha2 && room > -1)
		for (i = world[room].people; i; i = i->next_in_room)
			if (i != cha1 && i != cha2 && i->desc)
				write_to_output(i->desc, 0, messg);
}



/* higher-level communication */
void ansi(char *color, struct char_data * ch)
{

	if (ch->desc) {
		if (IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
			write_to_output(ch->desc, 0, color);
		}
	}
}				/* END OF ansi() */


void act(char *str, int hide_invisible, struct char_data * ch,
	   struct obj_data * obj, void *vict_obj, int type)
{

	/* no color */
	ansi_act(str, hide_invisible, ch, obj, vict_obj, type, NULL, 0);

}				/* END OF act */


void ansi_act(char *str,
	        int hide_invisible,
	        struct char_data * ch,
	        struct obj_data * obj,
	        void *vict_obj,
	        int type,
	        char *color,
	        int show_flags)
{

	register char *strp;
	register char *point;
	register char *idx;
	struct char_data *to;
	char buf[MAX_STRING_LENGTH];
	int let_em_see_message, where_im_at;

	if (!str)
		return;

	if (!*str)
		return;

	if (!ch)
		return;

	if (ch->in_room < 0) {
		main_log("ERROR - negative ch->in_room in ansi_act\r\n");
		spec_log("ERROR - negative ch->in_room in ansi_act", ERROR_LOG);
		return;
	}

	/* DECIDE WHO WE ARE DOING IT TO */
	where_im_at = 0;
	if (type == TO_VICT) {	/* type == 1 */
		where_im_at = 2;
		to = (struct char_data *) vict_obj;
	}
	else {
		if (type == TO_CHAR) {	/* type == 3 */
			where_im_at = 4;
			to = ch;
		}
		else {
			where_im_at = 8;
			to = world[ch->in_room].people;
		}
	}

	/* SAFTEY CHECK */
	if (to == 0) {
		where_im_at = where_im_at + 1;	/* FLIP FIRST BIT */
		to = ch;
	}

	for (; to; to = to->next_in_room) {
		let_em_see_message = TRUE;
		if (!(to->desc))
			let_em_see_message = FALSE;

		if ((to == ch) && (type != TO_CHAR))
			let_em_see_message = FALSE;

		if (!(CAN_SEE(to, ch)) && hide_invisible)
			let_em_see_message = FALSE;

		if (!IS_SET(show_flags, AA_IGNORE_SLEEP)) {
			if (!(AWAKE(to))	/* && !(IS_AFFECTED(to,AFF_D
			        REAMSIGHT)) */ )
				let_em_see_message = FALSE;
		}

		if (type == TO_NOTVICT &&
		    (to == (struct char_data *) vict_obj))
			let_em_see_message = FALSE;

		/* IF FIGHTING AND SUPPRESS FLAG IS TURNED ON */
		if (IS_SET(show_flags, AA_FIGHT)) {

			if ((IS_SET(to->specials.act2, PLR2_NO_OPPONENT_FMSG)) && type == TO_VICT)
				let_em_see_message = FALSE;
			if ((IS_SET(to->specials.act2, PLR2_NO_PERSONAL_FMSG)) && type == TO_CHAR)
				let_em_see_message = FALSE;
			if ((IS_SET(to->specials.act2, PLR2_NO_BYSTANDER_FMSG)) && type == TO_NOTVICT)
				let_em_see_message = FALSE;
		}

		if (let_em_see_message == TRUE) {
			for (strp = str, point = buf;;)
				if (*strp == '$') {
					switch (*(++strp)) {
					case 'n':{
							if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR1: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = PERS(ch, to);
							}
							break;
						}
					case 'N':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR2: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR3: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = PERS((struct char_data *) vict_obj, to);
							}
							break;
						}
					case 'm':{
							idx = HMHR(ch);
							break;
						}
					case 'M':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR4: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = HMHR((struct char_data *) vict_obj);
							}
							break;
						}
					case 's':{
							idx = HSHR(ch);
							break;
						}
					case 'S':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR5: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = HSHR((struct char_data *) vict_obj);
							}
							break;
						}
					case 'e':{
							idx = HSSH(ch);
							break;
						}
					case 'E':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR6: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = HSSH((struct char_data *) vict_obj);
							}
							break;
						}
					case 'o':{
							if (!obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR7: ansi_act has zero obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR8: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = OBJS(obj, to);
							}
							break;
						}
					case 'O':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR9: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR10: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = OBJS((struct obj_data *) vict_obj, to);
							}
							break;
						}
					case 'p':{
							if (!obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR11: ansi_act has zero obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR12: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = OBJS(obj, to);
							}
							break;
						}
					case 'P':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR13: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else if (!to) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR14: ansi_act has zero to pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = OBJS((struct obj_data *) vict_obj, to);
							}
							break;
						}
					case 'a':{
							if (!obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR15: ansi_act has zero obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = SANA(obj);
							}
							break;
						}
					case 'A':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR16: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = SANA((struct obj_data *) vict_obj);
							}
							break;
						}
					case 'T':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR17: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = (char *) vict_obj;
							}
							break;
						}
					case 'F':{
							if (!vict_obj) {
								bzero(buf, sizeof(buf));
								sprintf(buf, "ERROR18: ansi_act has zero vict_obj pointer for %s.\r\n", GET_REAL_NAME(ch));
								do_sys(buf, IMO_IMP, ch);
								return;
							}
							else {
								idx = ha1100_fname((char *) vict_obj);
							}
							break;
						}
					case '$':{
							idx = "$";
							break;
						}
					default:{
							sprintf(buf, "ERROR: Illegal $-code to act():%s", str);
							main_log(buf);
							spec_log(buf, ERROR_LOG);
							break;
						}
					}	/* END OF switch */

					while ((idx) && (*point = *(idx++)))
						++point;
					++strp;
				}	/* END OF "$" */

				else if ((!(*(point++) = *(strp++))))
					break;

			*(--point) = '\r';
			*(++point) = '\n';
			*(++point) = '\0';
			if (color)
				ansi(color, to);
			if (color && *color)
				if (IS_SET(GET_ACT3(to), PLR3_ANSI))
					strcat(buf, END);
			CAP(buf);
			write_to_output(to->desc, color, "%s", buf);
		}		/* END OF let_em_see_message == TRUE */

		if ((type == TO_VICT) || (type == TO_CHAR))
			return;

	}			/* END OF for loop */

	return;

}				/* END OF ansi_act() */

const char *ANSI[] = {"&", BLACK, RED, GREEN, BROWN, BLUE, PURPLE, CYAN, DGRAY, LRED, LGREEN, YELLOW,
	LBLUE, LPURPLE, LCYAN, WHITE, GRAY, BLINK, UNDERL, INVERSE, BACK_BLACK, BACK_RED,
BACK_GREEN, BACK_BROWN, BACK_BLUE, BACK_PURPLE, BACK_CYAN, BACK_GRAY, NEWLINE, END, "!"};
const char CCODE[] = "&krgybpcKRGYBPCWwfuv01234567^E!";

#define NEW_STRING_LENGTH (size_t)(dest_char-save_pos)
size_t parse_colors(char *txt, size_t maxlen, struct char_data * ch, const char *color)
{
	char *dest_char, *source_char, *color_char, *save_pos;
	char colors[20], bcolors[20], ccolors[20];
	int i;
	size_t wanted;
	if (!txt || !strchr(txt, '&'))
		return strlen(txt);

	source_char = txt;
	CREATE(dest_char, char, maxlen);
	save_pos = dest_char;

	/* loop that walks through the string. */
	for (; *source_char && (NEW_STRING_LENGTH < maxlen);) {
		/* no color code - just copy */
		if (*source_char != '&') {
			*dest_char++ = *source_char++;
			continue;
		}

		/* We have a color code! */

		source_char++;
		//Point to the code.

			if (*source_char == '\0') {
			*dest_char++ = '&';
			continue;
		}

		if (!ch || !IS_SET(GET_ACT3(ch), PLR3_ANSI)) {
			//Not parsing the code, skip it.
				if (*source_char == '&') {
				*dest_char++ = '&';
			}
			source_char++;
			continue;
		}

		/* take care of &n's (which could be END or *color) */
		if (*source_char == 'n') {
			if (color && NEW_STRING_LENGTH + strlen(color) < maxlen) {
				for (color_char = (char *) color; *color_char;)
					*dest_char++ = *color_char++;
			}
			else if (NEW_STRING_LENGTH + strlen(END) < maxlen) {
				for (color_char = (char *) END; *color_char;)
					*dest_char++ = *color_char++;
			}
		}

		strcpy(colors, "rgybpcwKRGYBPCW");
		strcpy(bcolors, "01234567");
		strcpy(ccolors, "cCbBW");

		if (*source_char == 'x' || *source_char == 's') {
			*source_char = *(colors + number(0, 14));
		}
		if (*source_char == 'S' || *source_char == 'i') {
			*source_char = *(ccolors + number(0, 4));
		}

		if (*source_char == 'X') {
			*source_char = *(bcolors + number(0, 7));
		}

		/* If we get this far, we have to figure out what the code
		 * means */
		for (i = 0; CCODE[i] != '!'; i++) {	/* do we find it ? */
			if ((*source_char) == CCODE[i]) {	/* if so : */
				if (NEW_STRING_LENGTH + strlen(ANSI[i]) < maxlen) {	/* only substitute if
											 * there's room for the
											 * whole code */
					/* color_char now points to the first
					 * char in color code */
					for (color_char = (char *) ANSI[i]; *color_char;)
						*dest_char++ = *color_char++;
				}
				break;
			}
		}
		source_char++;
	}			/* For loop that walks through the string(s) */

	*dest_char = '\0';

	wanted = strlen(source_char);
	strncpy(txt, save_pos, maxlen - 1);
	free(save_pos);

	return NEW_STRING_LENGTH + wanted;
}
#undef NEW_STRING_LENGTH

int co3400_process_input_old(struct descriptor_data * t)
{

	int sofar, thisround, begin, squelch, i, k, flag;
	char tmp[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], lv_name[256];

	bzero(lv_name, sizeof(lv_name));
	strcpy(lv_name, "SYSTEM");
	if (t->descriptor) {
		if (t->character) {
			if (t->character->player.name) {
				strcpy(lv_name, t->character->player.name);
			}
		}
	}

	sofar = 0;
	flag = 0;
	begin = strlen(t->curr_input);

	/* Read in some stuff */
	do {
		bzero(buf1, sizeof(buf1));
#if defined (DIKU_WINDOWS)
		thisround = recv(t->descriptor, buf1, MAX_INPUT_LENGTH - 1, 0);
#else
		thisround = read(t->descriptor, buf1, MAX_INPUT_LENGTH - 1);
#endif
		if (thisround > 0) {
			if (strlen(t->curr_input) + strlen(buf1) >
			    MAX_INPUT_LENGTH - 5) {
				/*
				 strcpy(buf1, "ERROR: Line to long, truncated.\r\n");
				 main_log(buf1);
				 */
				if (co2200_write_to_descriptor(t->descriptor, buf1) < 0)
					return (-1);
				if (t->descriptor)
					if (t->character)
						if (t->character->player.name) {
							sprintf(buf1, "Caused by %s.\r\n",
								t->character->player.name);
							main_log(buf1);
						}
				bzero(t->curr_input, sizeof(t->curr_input));
				return (-1);
			}
			strcat(t->curr_input, buf1);
			sofar += thisround;
		}
		else {
			if (thisround < 0) {
#if defined (DIKU_WINDOWS)
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
				if (errno != EWOULDBLOCK) {
#endif
					perror("ERROR: Readb1 - ERROR");
					return (-1);
				}
				else {
					break;
				}
			}	/* END OF thisround < 0 */
			else {
				/* thisround == 0 */
				if (LOG_SOCKET_EOF) {
					main_log("EOF encountered on socket read.");
				}
				return (-1);
			}
		}
	} while (!ISNEWL(*(t->curr_input + begin + sofar - 1)));

	*(t->curr_input + begin + sofar) = 0;

	/* if we couldn't find a newline, return without proc'ing */
	for (i = begin; !ISNEWL(*(t->curr_input + i)); i++) {
		if (!*(t->curr_input + i))
			return (0);
	}

	/* input contains 1 or more newlines; process the stuff */
	for (i = 0, k = 0; *(t->curr_input + i);) {
		if (!ISNEWL(*(t->curr_input + i)) &&
		    !(flag = (k >= (MAX_INPUT_LENGTH - 2)))) {
			/* backspace */
			if (*(t->curr_input + i) == '\b') {
				/* more than one char ? */
				if (k) {
					if (*(tmp + --k) == '$') {
						k--;
					}
					i++;
				}
				else {
					i++;	/* no or just one char.. Skip
						 * backsp */
				}
			}
			else {
				if (isascii(*(t->curr_input + i)) &&
				    isprint(*(t->curr_input + i))) {
					/* trans char, double for '$' (printf) */
					if ((*(tmp + k) = *(t->curr_input + i)) == '$')
						*(tmp + ++k) = '$';
					k++;
					i++;
				}
				else {
					i++;
				}
			}
		}		/* END OF if not a newline */
		else {

			*(tmp + k) = 0;
			if (*tmp == '!')
				strcpy(tmp, t->last_input);
			else
				strcpy(t->last_input, tmp);

			write_to_output(t, 0, tmp);
			/* return supplied by user this time */
			t->prompt_cr = 0;

			if (t->snoop.snoop_by) {
				write_to_output(t->snoop.snoop_by->desc, 0, "%% ");
				write_to_output(t->snoop.snoop_by->desc, 0, tmp);
				write_to_output(t->snoop.snoop_by->desc, 0, "\n\r");
			}

			if (flag) {
				sprintf(buf1,
				"Line too long. Truncated to:\n\r%s\n\r", tmp);
				if (co2200_write_to_descriptor(t->descriptor, buf1) < 0)
					return (-1);

				/* skip the rest of the line */
				for (; !ISNEWL(*(t->curr_input + i)); i++);
			}

			/* find end of entry */
			for (; ISNEWL(*(t->curr_input + i)); i++);

			/* squelch the entry from the buffer */
			for (squelch = 0;; squelch++) {
				if ((*(t->curr_input + squelch) =
				     *(t->curr_input + i + squelch)) == '\0') {
					break;
				}
			}	/* END OF for loop */
			k = 0;
			i = 0;
		}		/* END OF else */

	}			/* END OF for loop */

	return (1);

}				/* END OF co3400_process_input_old() */

void co3500_clear_user_list()
{
	struct descriptor_data *lv_user, *lv_next_user;
	struct char_data *ch, *tmp_ch;
	int lv_found_name;
	char buf[MAX_STRING_LENGTH];

	trace_log("Purging user list - START");

	lv_found_name = TRUE;	/* LEAVE THE FIRST GUY ALONE */
	for (lv_user = descriptor_list; lv_user; lv_user = lv_next_user) {
		lv_next_user = lv_user->next;
		if (lv_user->original)
			tmp_ch = lv_user->original;
		else
			tmp_ch = lv_user->character;
			
		/* Clear people sitting at certain states: */
		if (((STATE(lv_user) == CON_GET_NAME) || 
			(STATE(lv_user) == CON_GET_MENU_SELECTION)) &&
			((time(0) - lv_user->connected_at) > 600))
		{
			co2500_close_single_socket(lv_user);
		}

		/* FIND THIS PERSON IN THE USER LIST */
		for (ch = character_list; ch; ch = ch->next) {
			if (ch->player.name && tmp_ch && tmp_ch->player.name) {
				if (!(strncmp(GET_NAME(ch), GET_NAME(tmp_ch), 20))) {
					lv_found_name = TRUE;
					break;
				}
			}
		}		/* END OF character FOR loop */

		/* WE DIDN'T FIND THE NAME SO REMOVE IT */
		if (lv_found_name == FALSE) {
			bzero(buf, sizeof(buf));
			if (!tmp_ch) {
				sprintf(buf, "WARNING: co3500 UNDEFINED in DESC, not in CHAR list - Not clearing.");
			}
			else {
				sprintf(buf, "WARNING: co3500 %s in DESC, not in CHAR list",
					GET_NAME(tmp_ch));
				co2500_close_single_socket(lv_user);
			}
			main_log(buf);
			spec_log(buf, ERROR_LOG);
		}

		lv_found_name = FALSE;

	}			/* END OF descriptor FOR loop */

	trace_log("Purging user list - END");

}				/* END OF co3500_clear_user_list() */

