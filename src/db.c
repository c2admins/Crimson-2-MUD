 /* db *//* gv_location: 4501-5000 *//*
*******************************************************************
*  file: db.c , Database module.                    Part of DIKUMUD *
*  Usage: Loading/Saving chars, booting world, resetting etc.       *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.  *
******************************************************************* */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <unistd.h>

#include "structs.h"
#include "utility.h"
#include "db.h"
#include "comm.h"
#include "handler.h"
#include "limits.h"
#include "constants.h"
#include "parser.h"
#include "ban.h"
#include "weather.h"
#include "spells.h"
#include "globals.h"
#include "mobact.h"
#include "bounty.h"
#include "func.h"
#include "alias.h"

/* see also in comm.c for a few others */
#define INIT_CARVED_WORLD    4000	/* put in zones.tbl file l8r */
#define INIT_CARVED_MOB      2000	/* have mud remember what it was last */
#define INIT_CARVED_OBJ      2000	/* get zcmds for each zone too */
#define INIT_CARVED_QUEST    100
#define INIT_CARVED_SCRIPT 10
#define DIE_ON_ERROR         0	/* change to zero to disable this */
#define BACKUP_ZONES         1

#define SAVE_ALIAS 1
#define LOAD_ALIAS 2
#define MAX_PURGE_SIZE 3000
#define PURGE_NOW 1
#define PURGE_LATER 2

/***************************************************************** *
*  declarations of most of the 'global' variables                  *
****************************************************************** */
long up_time;			/* this ought to be interesting */
struct room_data *world;	/* dyn array of rooms     */
int top_of_world = 0;		/* ref to the top element of world */
struct zone_data *zone_table;	/* table of reset data             */
int top_of_zone_table = 0;
int gv_booting_mud = 1;

/* global linked list */
struct char_data *character_list = 0;
struct obj_data *object_list = 0;
struct vehicle_data *vehicle_list = 0;

struct message_list fight_messages[MAX_MESSAGES];	/* fighting messages   */
int top_of_p_table = 0;		/* ref to top of table      */
int top_of_p_file = 0;

char *credits;			/* the Credits List                */
char *news;			/* the news                        */
char *todo;			/* the todo list                   */
char *wipe;			/* future purges                   */
char *motd;			/* the messages of today           */
char *help;			/* the main help page              */
char *info;			/* the info text                   */
char *wizlist;			/* the wizlist                     */
char *gv_syslog;		/* the syslog                      */
char *commands;			/* the command list                */
char *changelog;		/* the changelog file				*/

FILE *mob_f;			/* file containing mob prototypes  */
FILE *obj_f;			/* obj prototypes                  */
struct mob_index_data *mob_index;	/* index table for mobiles    */
struct obj_index_data *obj_index;	/* index table for objects    */
struct quest_data *quest_index;
struct questsys_script *script_index;	/* index table for quest system */

int mob_f_zone = -1;		/* zone of the open file */
int obj_f_zone = -1;		/* needed for new file system */

int carved_mob = 0;		/* number of blocks currently chunked */
int carved_obj = 0;		/* number of blocks currently chunked */
int carved_quest = 0;
int carved_script = 0;

int top_of_mobt = 0;		/* top of mobile index table       */
int top_of_objt = 0;		/* top of object index table       */
int top_of_questt = 0;		/* top of quest index table	  */
int top_of_script = 0;		/* top of quest system table	  */

FILE *help_fl;			/* file for help texts (HELP <kwd>) */
int top_of_helpt;		/* top of help index table         */
struct help_index_element *help_index = 0;


/***************************************************************** *
*  routines for booting the system                                 *
****************************************************************** */

//in utility.c
int touch(const char *path);
void boot_histories(void);

/* body of the booting system */
void db1000_boot_db(void)
{
	int i;



	/* SET BITS WE WANT TO SET/REMOVE FOR CHARS/MOBILES  */
	/* THE FOLLOWING VARIABLES ARE SET SO WE CAN QUICKLY */
	/* ADJUST CHARS/MOBS                                 */

	/* PLAYERS CAN'T HAVE THESE BITS TURNED ON */
	gv_anti1_PC_bits = PLR1_SENTINEL | PLR1_SCAVENGER |
		PLR1_ISNPC | PLR1_CLASS_DEMON |
		PLR1_STAY_ZONE | PLR1_UNFILTER |
		PLR1_WIMPY | PLR1_SIGNAL_HUNT_HELP |
		PLR1_GUARD1 | PLR1_GUARD2 |
		PLR1_NOSTEAL | PLR1_INJURED_ATTACK |
		PLR1_AGGRESSIVE_EVIL | PLR1_AGGRESSIVE_NEUT |
		PLR1_AGGRESSIVE_GOOD | PLR1_POISONOUS |
		PLR1_NOCHARM |
		PLR1_DRAIN_XP | PLR1_CLASS_SHADOW |
		PLR1_CLASS_VAMPIRE | PLR1_CLASS_DRAGON |
		PLR1_CLASS_GIANT | PLR1_CLASS_ANIMAL |
		PLR1_CLASS_UNDEAD | PLR1_CANT_HUNT |
		PLR1_HUNTASSIST | PLR1_NOFOLLOW |
		PLR1_NOKILL | PLR1_NOHOLD;

	gv_anti2_PC_bits = PLR2_MOUNT | PLR2_AFK |
		PLR2_MOB_AI | PLR2_CLOSE_DOORS |
		PLR2_BACKSTAB | PLR2_EQTHIEF |
		PLR2_OPEN_DOORS | PLR2_USES_EQ |
		PLR2_JUNKS_EQ | PLR2_NO_LOCATE |
		PLR2_ATTACKER | PLR2_AFW;

	gv_anti3_PC_bits = PLR3_NOSHOUT;

	/* NPC CHARS CAN'T HAVE THESE BITS TURNED ON */
	gv_anti1_NPC_bits = PLR1_SHOW_DIAG;
	gv_anti2_NPC_bits = PLR2_FREEZE | PLR2_NO_OPPONENT_FMSG |
		PLR2_NO_PERSONAL_FMSG | PLR2_NO_BYSTANDER_FMSG |
		PLR2_NO_EMOTE | PLR2_JAILED |
		PLR2_QUEST |
		PLR2_MUZZLE_ALL | PLR2_MUZZLE_SHOUT |
		PLR2_MUZZLE_BEEP |
		PLR2_PKILLABLE | PLR2_AFK |
		PLR2_BACKSTAB | PLR2_AFW;
	gv_anti3_NPC_bits = PLR3_NOSHOUT |
		PLR3_ENFORCER;

	/* NPC CHARS START OUT WITH THESE BITS TURNED ON */
	gv_good1_NPC_bits = PLR1_ISNPC;
	gv_good2_NPC_bits = PLR2_SHOW_NAME;
	gv_good3_NPC_bits = PLR3_BRIEF |
		PLR3_ANSI | PLR3_SHOW_ROOM |
		PLR3_SHOW_HP | PLR3_SHOW_MANA |
		PLR3_SHOW_MOVE | PLR3_SHOW_EXITS;

	fprintf(stderr, "Resetting game time to:");
	db2000_reset_time();

	fprintf(stderr, "Reading:");

	fprintf(stderr, " races,");
	db5200_load_races();

	fprintf(stderr, " classes,");
	db5300_load_classes();

	fprintf(stderr, " clans,");
	db10000_load_clans();

	fprintf(stderr, " news,");
	news = db7300_file_to_string(NEWS_FILE);
	
	fprintf(stderr, " changelog,");
	changelog = db7300_file_to_string(CHANGELOG_FILE);

	fprintf(stderr, " commands,");
	commands = db7300_file_to_string(COMMANDS_FILE);

	fprintf(stderr, " todo,");	/* To Do list */
	todo = db7300_file_to_string(TODO_FILE);

	fprintf(stderr, " credits,");
	credits = db7300_file_to_string(CREDITS_FILE);

	fprintf(stderr, " motd,");
	motd = db7300_file_to_string(MOTD_FILE);

	fprintf(stderr, " help,");
	help = db7300_file_to_string(HELP_PAGE_FILE);

	fprintf(stderr, " htbl,");
	if (!(help_fl = fopen(HELP_KWRD_FILE, "r"))) {
		main_log("   Could not open help file.");
		spec_log("   Could not open help file.", ERROR_LOG);
	}
	else
		help_index = build_help_index(help_fl, &top_of_helpt);

	fprintf(stderr, " info,");
	info = db7300_file_to_string(INFO_FILE);

	fprintf(stderr, " wiztitles,");
	loadwiztitles();
	setupwiztitles();

	fprintf(stderr, " syslog,\r\n");
	CREATE(gv_syslog, char, 7);
	strcpy(gv_syslog, "syslog");
	gv_syslog_loaded = FALSE;

	fprintf(stderr, "Loading: zone table,");
	db4700_boot_zones();
	fprintf(stderr, " rooms,");
	gv_donate_room = 0;	/* DEFAULT DONATION ROOM TO VOID */
	gv_mob_donate_room = 0;
	db4300_boot_world();
	fprintf(stderr, " player_index,");
	db3000_build_player_index();
	fprintf(stderr, " wizlist.\r\n");
	wizlist = db7300_file_to_string(WIZLIST_FILE);

	db3100_master_pwd();
	fprintf(stderr, " wipe,");
	wipe = db7300_file_to_string(WIPE_FILE);

	fprintf(stderr, "quest_index,");
	db4800_boot_quests();
	fprintf(stderr, " mob_index,");
	db4100_boot_mob();
	fprintf(stderr, " obj_index,");
	db4200_boot_obj();
	fprintf(stderr, " bountylist,");
	bt1100_load_bounty();
	fprintf(stderr, " banlist.\r\n");
	bn1000_load_banned();
	fprintf(stderr, "Renumbering: rooms,");
	db4500_renum_world();
	fprintf(stderr, " zone table.\r\n");
	db4600_renum_zone_table();

	fprintf(stderr, "Loading messages: fight,");
	ft1100_load_message();
	fprintf(stderr, " social,");
	boot_social_messages();
	fprintf(stderr, " pose.\r\n");
	boot_pose_messages();

	fprintf(stderr, "Assigning function pointers:");
	if (!IS_SET(gv_run_flag, RUN_FLAG_NOSPECIAL)) {
		fprintf(stderr, " Mobiles,");
		as1000_assign_mobiles();
		/* shops */
		boot_the_shops();
		/* shopkeepers */
		assign_the_shopkeepers();
		fprintf(stderr, " Objects,");
		as1100_assign_objects();
		//as1150_assign_objects2();
		fprintf(stderr, " Rooms,");
		as1200_assign_rooms();
	}
	fprintf(stderr, " Commands,");
	assign_command_pointers();
	fprintf(stderr, " Spells.\r\n");
	assign_spell_pointers();

	main_log("Checking item usage.");
	r5000_compute_maxed_items(0);

	fprintf(stderr, "Building: top xx lists.\r\n");
	in5500_build_top_lists();

	main_log("Resetting some important zones to begin with:");
	db5600_reset_zone(0);
	db5600_reset_zone(4);
	db5600_reset_zone(9);
	db5600_reset_zone(23);
	db5600_reset_zone(26);
	db5600_reset_zone(61);
	db5600_reset_zone(113);

	main_log("Queueing boot-time reset of all zones:");
	struct reset_q_element *update_u;
	for (i = 0; i <= top_of_zone_table; i++) {
		CREATE(update_u, struct reset_q_element, 1);
		if (i != 0 && i != 4 && i != 23 && i != 26 && i != 61 && i != 113 && i != 9) {
			update_u->zone_to_reset = i;
			update_u->next = 0;

			if (!reset_q.head)
				reset_q.head = reset_q.tail = update_u;
			else {
				reset_q.tail->next = update_u;
				reset_q.tail = update_u;
			}

			zone_table[i].age = 999;
		}
	}
	main_log("Finished Zone reset.");
	//reset_q.head = reset_q.tail = 0;
	fprintf(stderr, "Boot_DB done - Totals: Zones=%d Rooms=%d Mobs=%d Obj's=%d\r\n",
		top_of_zone_table + 1, top_of_world + 1, top_of_mobt + 1, top_of_objt + 1);

	main_log("Booting auction system\r\n");
	au1300_reboot_auction();
	boot_histories();
	gv_booting_mud = 0;
}				/* END OF db1000_boot_db() */


/* reset the time in the game from file */
void db2000_reset_time(void)
{
	char buf[MAX_STRING_LENGTH];
	long beginning_of_time = 650336715;  // <- Fri Aug 10 20:05:15 1990
	long seconds_since_beginning = (time(0) - beginning_of_time);
	long hours_since_beginning = (seconds_since_beginning/60);

	time_info = mud_time_passed(time(0), beginning_of_time);
	up_time = time(0);
	switch (time_info.hours) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:{
			weather_info.sunlight = SUN_DARK;
			break;
		}
	case 5:{
			weather_info.sunlight = SUN_RISE;
			break;
		}
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:{
			weather_info.sunlight = SUN_LIGHT;
			break;
		}
	case 21:{
			weather_info.sunlight = SUN_SET;
			break;
		}
	case 22:
	case 23:
	default:{
			weather_info.sunlight = SUN_DARK;
			break;
		}
	}

	sprintf(buf, " %dH %dD %dM %dY.\r\n",
		time_info.hours, time_info.day,
		time_info.month, time_info.year);
	fprintf(stderr, buf);

	weather_info.pressure = 960;
	if ((time_info.month >= 7) && (time_info.month <= 12))
		weather_info.pressure += dice(1, 50);
	else
		weather_info.pressure += dice(1, 80);

	weather_info.change = 0;

	if (weather_info.pressure <= 980)
		weather_info.sky = SKY_LIGHTNING;
	else if (weather_info.pressure <= 1000)
		weather_info.sky = SKY_RAINING;
	else if (weather_info.pressure <= 1020)
		weather_info.sky = SKY_CLOUDY;
	else
		weather_info.sky = SKY_CLOUDLESS;
		
	// Init weather using the same method as above.
	weather_info.lunar_phase = ((hours_since_beginning / HOURS_PER_PHASE) % MAX_LUNAR_PHASES);
	weather_info.current_hour_in_phase = hours_since_beginning % HOURS_PER_PHASE;
}				/* END OF db2000_reset_time() */


 /* for now I'm going to hard code the first MALLOC to the actual number of
  * rooms.. I'll have it save the info in the zones.tbl file sooner or later
  * tho.. */
void db2100_allocate_room(int new_top)
{
	int orig_size, i;
	char buf[MAX_STRING_LENGTH];



	if (new_top < carved_world)
		return;

	if (new_top > 0) {
		carved_world += 250;

		world = (struct room_data *) realloc(world,
			       (carved_world + 2) * sizeof(struct room_data));
		if (!(world)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc room\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}
	else {
		carved_world = INIT_CARVED_WORLD;
		CREATE(world, struct room_data, carved_world + 2);
	}

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	if (carved_world == INIT_CARVED_WORLD)
		orig_size = 0;
	else
		orig_size = (carved_world - 250);

	/*
	 sprintf (buf,"\nWorld orig size before alloc is %d, new size=%d",
						orig_size, carved_world);
	 main_log(buf);
   */
	for (i = orig_size; i < carved_world + 2; i++) {
		bzero((char *) &world[i], sizeof(struct room_data));
	}

}				/* END OF db2100_allocate_room() */


void db2200_allocate_zcmd(int zone, int new_top)
{
	/* this one is different from the rest */
	/* cmd pointer must be inited to NULL elsewhere */
	int i;
	char buf[MAX_STRING_LENGTH];



	if (new_top % 25 != 1)	/* do 25 at a time */
		return;

	if (new_top == 1) {	/* some machines dont like realloc null */
		zone_table[zone].cmd = (struct reset_com *) malloc(25 * sizeof(struct reset_com));
		if (!(zone_table[zone].cmd)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: malloc zcmd one.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}		/* if after malloc */
	}			/* if new_top = 1 */
	else {
		zone_table[zone].cmd = (struct reset_com *)
			realloc(zone_table[zone].cmd,
			((new_top / 25 + 1) * 25) * sizeof(struct reset_com));
		if (!(zone_table[zone].cmd)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc zcmd two\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}			/* END OF else */

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	/*
		     bzero(buf, sizeof(buf));
		     sprintf (buf,"Orig size of alloc is %d, old=%d",
						      new_top - 1, new_top + 24);
		     main_log(buf);
		     */

	for (i = (new_top - 1); i < new_top + 24; i++) {
		bzero((char *) &zone_table[zone].cmd[i], sizeof(struct reset_com));
	}

}				/* END OF db2200_allocate_zcmd() */


void db2300_allocate_mob(int new_top)
{				/* for allocating the index */
	int i, orig_size;
	char buf[MAX_STRING_LENGTH];



	if (new_top < carved_mob)
		return;

	if (new_top > 0) {
		carved_mob += 25;
		mob_index = (struct mob_index_data *)
			realloc(mob_index, (carved_mob + 2) *
				sizeof(struct mob_index_data));
		if (!(mob_index)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc mob.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
		sprintf(buf, "\r\nalloc_mob%d (better increase INIT_CARVED_MOB!",
			carved_mob);
	}
	else {
		carved_mob = INIT_CARVED_MOB;
		CREATE(mob_index, struct mob_index_data, carved_mob + 2);
	}

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	if (carved_mob == INIT_CARVED_MOB)
		orig_size = 0;
	else
		orig_size = (carved_mob - 25);

	/*
		     bzero(buf, sizeof(buf));
		     sprintf (buf,"Mob orig size before alloc is %d, new size=%d",
						      orig_size, carved_mob);
		     main_log(buf);
		     */

	for (i = orig_size; i < carved_mob + 2; i++) {
		bzero((char *) &mob_index[i], sizeof(struct mob_index_data));
	}			/* END OF for */

}				/* END OF db2300_allocate_mob() */


void db2400_allocate_obj(int new_top)
{
	int i, orig_size;
	char buf[MAX_STRING_LENGTH];



	if (new_top < carved_obj)
		return;

	if (new_top > 0) {
		carved_obj += 25;
		obj_index = (struct obj_index_data *)
			realloc(obj_index, (carved_obj + 2) *
				sizeof(struct obj_index_data));
		if (!(obj_index)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc obj.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
		sprintf(buf, "\nalloc_obj %d better increase INIT_CARVED_OBJ!",
			carved_obj);
	}
	else {
		carved_obj = INIT_CARVED_OBJ;
		CREATE(obj_index, struct obj_index_data, carved_obj + 2);
	}

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	if (carved_obj == INIT_CARVED_OBJ)
		orig_size = 0;
	else
		orig_size = (carved_obj - 25);

	/*
	 sprintf (buf,"Obj orig size before alloc is %d, new size=%d",
						orig_size, carved_obj);
	 main_log(buf);
   */

	for (i = orig_size; i < carved_obj + 2; i++) {
		bzero((char *) &obj_index[i], sizeof(struct obj_index_data));
	}			/* END OF for */

}				/* END OF db2400_allocate_obj() */

void db2500_allocate_quest(int new_top)
{
	int i, orig_size;
	char buf[MAX_STRING_LENGTH];



	if (new_top < carved_quest)
		return;

	if (new_top > 0) {
		carved_quest += 25;
		quest_index = (struct quest_data *)
			realloc(quest_index, (carved_quest + 2) *
				sizeof(struct quest_data));
		if (!(quest_index)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc quest.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}
	else {
		carved_quest = INIT_CARVED_QUEST;
		CREATE(quest_index, struct quest_data, carved_quest + 2);
	}

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	if (carved_quest == INIT_CARVED_QUEST)
		orig_size = 0;
	else
		orig_size = (carved_quest - 25);


	for (i = orig_size; i < carved_quest + 2; i++) {
		bzero((char *) &quest_index[i], sizeof(struct quest_data));
	}			/* END OF for */

}				/* END OF db2400_allocate_obj() */


void db2600_allocate_script(int new_top)
{
	int i, orig_size;
	char buf[MAX_STRING_LENGTH];



	if (new_top < carved_script)
		return;

	if (new_top > 0) {
		carved_script += 25;
		script_index = (struct questsys_script *)
			realloc(script_index, (carved_script + 2) *
				sizeof(struct questsys_script));
		if (!(script_index)) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: realloc script.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}
	else {
		carved_script = INIT_CARVED_SCRIPT;
		CREATE(script_index, struct questsys_script, carved_script + 2);
	}

	/* ZERO OUT NEWLY ALLOCATED MEMORY */
	if (carved_script == INIT_CARVED_SCRIPT)
		orig_size = 0;
	else
		orig_size = (carved_script - 25);


	for (i = orig_size; i < carved_script + 2; i++) {
		bzero((char *) &script_index[i], sizeof(struct questsys_script));
	}			/* END OF for */

}				/* END OF db2600_allocate_script() */



 /* generate index table for the player file */
void db3000_build_player_index(void)
{
	FILE *wipe_file, *fl_wizcopy, *fl_player;
	int lv_purge_counter, lv_purge_check, nr, idx, jdx, rc;
	unsigned long lv_seconds_gone;
	struct char_file_u dummy;
	char buf[MAX_STRING_LENGTH], wizlist_str[MAX_STRING_LENGTH], purge_name[MAX_PURGE_SIZE][20],
	  lv_temp_name[20];
	time_t t;



	bzero(purge_name, MAX_PURGE_SIZE * 20);
	lv_purge_counter = 0;


	fl_wizcopy = fopen(WIZCOPY_FILE, "r");
	if (!fl_wizcopy) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: unable to open input wizlist.cpy %d.\r\n",
			errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	/* fl_wizlist = fopen(WIZLIST_FILE, "w+"); if (!fl_wizlist) {
	 * bzero(buf, sizeof(buf)); sprintf(buf, "ERROR: unable to open output
	 * wizlist %d.\r\n", errno); perror(buf); ABORT_PROGRAM(); } */
	nr = -1;
	fl_player = fopen(PLAYER_FILE, "r+b");
	if (!fl_player) {
		if (errno != ENOENT) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: unable to open player index %d.\r\n", errno);
			perror(buf);
			ABORT_PROGRAM();
		}
		else {
			perror("No playerfile, attempting to create it now.");
			touch(PLAYER_FILE);
			if (!(fl_player = fopen(PLAYER_FILE, "r+b"))) {
				perror("Fatal error opening the players file");
				exit(1);
			}
		}
	}

	/* INSERT HEADER TO THE NEW WIZLIST FILE */
	fgets(buf, 80, fl_wizcopy);
	for (; !feof(fl_wizcopy);) {
		//fputs(buf, fl_wizlist);
		fgets(buf, 80, fl_wizcopy);
	}			/* END OF SEARCHING FOR METHUSELAH */
	fclose(fl_wizcopy);

	bzero(wizlist_str, sizeof(wizlist_str));
	strcpy(wizlist_str, "|| ");

	player_table = 0;
	for (; !feof(fl_player);) {
		fread(&dummy, sizeof(struct char_file_u), 1, fl_player);
		if (!feof(fl_player)) {
			/* IF ITS NOT ALREADY BEEN DELETED */
			if (*dummy.name != '1' && *dummy.name != '2' && lv_purge_counter < MAX_PURGE_SIZE) {
				lv_purge_check = db9900_purge_check(&dummy, 0);
				if (lv_purge_check == PURGE_NOW) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "PURGING: %s L[%d] pos[%d].", dummy.name, dummy.level, nr + 1);
					main_log(buf);
					spec_log(buf, PURGE_LOG);
					r1125_backup_delete_file(dummy.name);
					r1100_rent_delete_file(dummy.name);
					r1150_alias_delete_file(dummy.name);

					bzero(dummy.title, sizeof(dummy.title));
					sprintf(dummy.title, "%s the deleted", dummy.name);

					bzero(buf, sizeof(buf));
					sprintf(buf, "1%s", dummy.name);
					bzero(dummy.name, sizeof(dummy.name));
					strncpy(dummy.name, buf, 19);

					/* UPDATE FILE */
					fseek(fl_player, (nr + 1) * sizeof(struct char_file_u), 0);
					fwrite(&dummy, sizeof(struct char_file_u), 1, fl_player);
					/* Positon back to where we were */
					fseek(fl_player, (nr + 1) * sizeof(struct char_file_u), 0);
					fread(&dummy, sizeof(struct char_file_u), 1, fl_player);
				}
				if (lv_purge_check == PURGE_LATER && dummy.level > 1) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "%s               ", dummy.name);
					buf[10] = 0;
					strcpy(purge_name[lv_purge_counter], buf);
					//rlum
						// I used ifndef because it has to work on __APPLE__ and __LINUX__
					/*
#ifndef __WIN32__
					       if(strlen(dummy.name) > 0) {
						       char mail_cmd[1024];
						       strcat(mail_cmd,"mail -s \"Crimson2 Notice:  Character Deletion\" ");
						       strcat(mail_cmd,dummy.name);
						       strcat(mail_cmd," < message.txt"); //will have to be in the prod directory.
						       system(mail_cmd);
					       }
#endif
					       */
						lv_purge_counter++;
				}
			}

			/* Create new entry in the list */
			if (nr == -1) {
				CREATE(player_table, struct player_index_element, 25);
				nr = 0;
			}
			else {
				nr++;
				if (nr % 25 == 0) {
					if (!(player_table = (struct player_index_element *)
					    realloc(player_table, ((nr + 25) *
								   sizeof(struct player_index_element))))) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "ERROR: generate index.\r\n");
						perror(buf);
						ABORT_PROGRAM();
					}
					/* ZERO MEMORY */
					for (idx = nr; idx < nr + 25; idx++) {
						bzero((char *) &player_table[idx],
						      sizeof(struct player_index_element));
					}
				}
			}
			player_table[nr].pidx_nr = nr;
			GET_DSC1_NUM(nr) = dummy.descriptor_flag1;
			bzero(player_table[nr].pidx_email, EMAIL_SIZE);
			if (strlen(dummy.email_name) > 0) {
				strncpy(player_table[nr].pidx_email, dummy.email_name, EMAIL_SIZE - 1);
			}

			/* initialize to time player last left game */
			player_table[nr].pidx_last_on = dummy.logoff_time;
			player_table[nr].pidx_level = dummy.level;
			if ((dummy.name[0] != '2') && (dummy.name[0] != '1')) {
				player_table[nr].pidx_clan_number = dummy.clan.number;
				player_table[nr].pidx_clan_rank = dummy.clan.rank;
			}
			else {
				player_table[nr].pidx_clan_number = 0;
				player_table[nr].pidx_clan_rank = 0;
			}
			player_table[nr].pidx_score = dummy.points.score;
			lv_seconds_gone = time(0) - dummy.logoff_time;

			CREATE(player_table[nr].pidx_name, char, strlen(dummy.name) + 1);
			for (idx = 0; (*(player_table[nr].pidx_name + idx) = LOWER(*(dummy.name + idx))); idx++);
		}
	}
	fclose(fl_player);
	/* bzero(buf, sizeof(buf));
	 * 
	 * sprintf(buf, "%78s||\n", wizlist_str); fputs(buf, fl_wizlist);
	 * fputs("()============================================================
	 * ================()\n", fl_wizlist);
	 * 
	 * fclose(fl_wizlist); */
	top_of_p_table = nr;
	top_of_p_file = top_of_p_table;

	if (lv_purge_counter < 1)
		return;

	/* SORT PURGE TABLE */
	for (idx = 0; idx < lv_purge_counter; idx++) {
		for (jdx = 0; jdx < lv_purge_counter; jdx++) {
			if (*purge_name[idx] &&
			    strcmp(purge_name[idx], purge_name[jdx]) < 0) {
				strcpy(lv_temp_name, purge_name[idx]);
				strcpy(purge_name[idx], purge_name[jdx]);
				strcpy(purge_name[jdx], lv_temp_name);
			}
		}		/* END OF for jdx loop */
	}			/* END OF for idx loop */

	wipe_file = 0;
	wipe_file = fopen(WIPE_FILE, "wb");
	if (!wipe_file) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: opening wipe file.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}
	t = time(NULL);
	sprintf(buf, "Date: %s", ctime(&t));
	buf[strlen(buf) - 1] = 0;
	strcat(buf, "\r\n");
	fputs(buf, wipe_file);
	fputs("---------------------------------------------------------\r\n", wipe_file);
	fputs("The following players will be purged between now and the next\r\n", wipe_file);
	fputs("reboot.  If you see a char on this list, and it shouldn't\r\n", wipe_file);
	fputs("be here, log that char in and enter save.  This will\r\n", wipe_file);
	fputs("change the last login time.  List is updated each reboot.\r\n", wipe_file);
	fputs("---------------------------------------------------------\r\n", wipe_file);
	sprintf(buf, "There are %d names in this list.\r\n", lv_purge_counter);
	fputs(buf, wipe_file);

	rc = 0;
	for (idx = 0; idx < lv_purge_counter; idx += 7) {
		sprintf(buf, "%s %s %s %s %s %s %s\r\n",
			purge_name[idx],
			purge_name[idx + 1],
			purge_name[idx + 2],
			purge_name[idx + 3],
			purge_name[idx + 4],
			purge_name[idx + 5],
			purge_name[idx + 6]);
		fputs(buf, wipe_file);
	}
	fclose(wipe_file);

}				/* END OF db3000_build_player_index() */


void db3100_master_pwd()
{

	FILE *pwd_file;
	int rc;



	bzero(gv_master_pwd, sizeof(gv_master_pwd));
	pwd_file = fopen("master.pwd", "r+");
	if (pwd_file) {
		fgets(gv_master_pwd, sizeof(gv_master_pwd), pwd_file);
		rc = errno;
		fclose(pwd_file);
		if (strlen(gv_master_pwd) > 5) {
			return;
		}
	}

	/* DEFAULT TO GARBAGE - YOU CAN'T GET THIS VIA SCRAMBLE ROUTINE */
	strcpy(gv_master_pwd, "<undefined>");
	return;

}				/* END OF db3100_master_pwd() */


int db4000_zone_of(int virtual)
{
	int top, mid, bot;
	char buf[MAX_STRING_LENGTH];


	bot = 0;
	top = top_of_zone_table;

	/* if number is way out choke right off the bat */
	if ((virtual < zone_table[bot].bottom)
	    || (virtual > zone_table[top].top)) {
		sprintf(buf, "db4000_zone_of (1): zone containing virtual# %d does not exist\r\n", virtual);
		fprintf(stderr, buf);
		spec_log(buf, ERROR_LOG);
		return (-1);
	}

	while (top > bot) {
		mid = (bot + top) / 2;
		if (virtual < zone_table[mid].bottom)
			top = mid - 1;
		else if (virtual > zone_table[mid].top)
			bot = mid + 1;
		else {		/* this must be it */
			bot = mid;
			top = mid;
		}
	}

	/* if number fell in a crack between zones detect it here */
	if ((virtual < zone_table[bot].bottom)
	    || (virtual > zone_table[top].top)) {
		fprintf(stderr, "db4000_zone_of (2): zone containing virtual# %d does not exist\r\n", virtual);
		return (-1);
	}
	return (top);
}				/* END OF db4000_zone_of() */


/* generate index table for object or monster file */
void db4100_boot_mob()
{
	int zone, i = -1, last = -1, error = 0;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], *tmpchar;



	for (zone = 0; zone <= top_of_zone_table; zone++) {
		sprintf(buf, "areas/%s.mob", zone_table[zone].filename);
		if (gv_verbose_messages == TRUE) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Opening: %s\r\n", buf);
			main_log(buf2);
		}
		if (!(mob_f = fopen(buf, "rb"))) {

			fprintf(stderr, "db4100_boot_mob: could not open %s.mob\r\n",
				zone_table[zone].filename);

			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: fopen in db4100 boot mob.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
		rewind(mob_f);
		for (;;) {
			bzero(buf, sizeof(buf));
			if (fgets(buf, 81, mob_f)) {
				if (*buf == '#') {	/* allocate new cell */
					i++;
					db2300_allocate_mob(i);
					if (gv_mega_verbose_messages == TRUE) {
						bzero(buf2, sizeof(buf2));
						sprintf(buf2, "    Loading mob: %s", buf);
						main_log(buf2);
					}
					if (!sscanf(buf, "#%d", &mob_index[i].virtual)) {

						bzero(buf, sizeof(buf));
						sprintf(buf, "i= %d. Arg no virtual after #%d in %s.mob",
							i,
						     mob_index[i - 1].virtual,
						   zone_table[zone].filename);
						perror(buf);
						ABORT_PROGRAM();

					}
					if (mob_index[i].virtual > zone_table[zone].top
					    || mob_index[i].virtual < zone_table[zone].bottom) {
						fprintf(stderr, "%s.mob: mob#%d outside the range (%d - %d)\r\n",
						    zone_table[zone].filename,
							mob_index[i].virtual,
						      zone_table[zone].bottom,
							zone_table[zone].top);
						error = 1;
					}
					if (mob_index[i].virtual <= last) {
						fprintf(stderr, "%s.mob: mob#%d <= to previous mob#\r\n",
						    zone_table[zone].filename,
							mob_index[i].virtual);
						error = 1;
					}
					last = MAXV(last, mob_index[i].virtual);
					if (zone_table[zone].bot_of_mobt == -1)
						zone_table[zone].bot_of_mobt = i;
					if (i > zone_table[zone].top_of_mobt)
						zone_table[zone].top_of_mobt = i;
					mob_index[i].pos = ftell(mob_f);
					mob_index[i].name = db7000_fread_string(mob_f);
					if (strncmp(mob_index[i].name, "mob ", 4)) {	/* make sure obj is a
											 * keyword */
						tmpchar = (char *) malloc(strlen(mob_index[i].name) + 5);
						strcpy(tmpchar, "mob ");
						strcpy(tmpchar + 4, mob_index[i].name);
						free(mob_index[i].name);
						mob_index[i].name = tmpchar;
					}
					mob_index[i].zone = zone;
					mob_index[i].number = 0;
					mob_index[i].func = 0;
					mob_index[i].next = i + 1;
				}	/* END OF '#' */
				else if (*buf == '$')	/* EOF */
					break;
			}	/* END OF fgets() */
			else {
				perror("db4100_boot_mob: Unexpected EOF");
				if (i >= 0)
					sprintf(buf, "ERROR: %s.mob is corrupt after mob#%d!?!",
						zone_table[zone].filename, mob_index[i].virtual);
				else
					sprintf(buf, "ERROR: %s.mob is corrupt!?!",
						zone_table[zone].filename);
				perror(buf);
				ABORT_PROGRAM();
			}
		}
		top_of_mobt = i;
		fclose(mob_f);
	}			/* for */
	if (error && DIE_ON_ERROR) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: One or more fatal errors occurred; please fix and reboot the server.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}
}				/* END OF db4100_boot_mob() */
/* generate index table for object or monster file */
void db4200_boot_obj()
{
	long tmppos;
	int zone, i = -1, last = -1, lv_num, error = 0;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], *tmpchar;



	for (zone = 0; zone <= top_of_zone_table; zone++) {
		sprintf(buf, "areas/%s.obj", zone_table[zone].filename);
		if (gv_verbose_messages == TRUE) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Opening: %s\r\n", buf);
			main_log(buf2);
		}
		if (!(obj_f = fopen(buf, "rb"))) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "db4200_boot_obj: could not open %s.obj\r\n",
				zone_table[zone].filename);
			perror(buf);
			ABORT_PROGRAM();
		}
		rewind(obj_f);
		for (;;) {
			if (fgets(buf, 81, obj_f)) {
				if (*buf == '#') {	/* allocate new cell */
					i++;
					db2400_allocate_obj(i);
					if (gv_mega_verbose_messages == TRUE) {
						bzero(buf2, sizeof(buf2));
						sprintf(buf2, "    Loading obj: %s", buf);
						main_log(buf2);
					}
					if (!sscanf(buf, "#%d", &obj_index[i].virtual)) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "Arg no virtual after #%d in %s.obj",
							obj_index[i - 1].virtual, zone_table[zone].filename);
						perror(buf);
						ABORT_PROGRAM();
					}
					if (obj_index[i].virtual > zone_table[zone].top
					    || obj_index[i].virtual < zone_table[zone].bottom) {
						fprintf(stderr, "%s.obj: obj#%d outside the range (%d - %d)\r\n",
						    zone_table[zone].filename,
							obj_index[i].virtual,
						      zone_table[zone].bottom,
							zone_table[zone].top);
						error = 1;
					}
					if (obj_index[i].virtual <= last) {
						fprintf(stderr, "%s.obj: obj#%d <= to previous obj#\r\n",
						    zone_table[zone].filename,
							obj_index[i].virtual);
						error = 1;
					}
					last = MAXV(last, obj_index[i].virtual);
					if (zone_table[zone].bot_of_objt == -1)
						zone_table[zone].bot_of_objt = i;
					if (i > zone_table[zone].top_of_objt)
						zone_table[zone].top_of_objt = i;
					obj_index[i].pos = ftell(obj_f);
					obj_index[i].name = db7000_fread_string(obj_f);
					if (strncmp(obj_index[i].name, "obj ", 4)) {	/* make sure obj is a
											 * keyword */
						tmpchar = (char *) malloc(strlen(obj_index[i].name) + 5);
						strcpy(tmpchar, "obj ");
						strcpy(tmpchar + 4, obj_index[i].name);
						free(obj_index[i].name);
						obj_index[i].name = tmpchar;
					}
					obj_index[i].zone = zone;
					obj_index[i].number = 0;
					obj_index[i].func = 0;
					obj_index[i].next = i + 1;
					fgets(buf, 81, obj_f);
					lv_num = strlen(buf);
					if (lv_num > 0) {
						lv_num--;
						if (buf[lv_num] == '\n')
							buf[lv_num] = 0;
					}
					if (!is_number(buf)) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "ARG no maximum after #%d in %s.obj",
							obj_index[i].virtual, zone_table[zone].filename);
						perror(buf);
						ABORT_PROGRAM();
					}
					lv_num = atoi(buf);
					if (lv_num < 0 || lv_num > 32767) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "Maximum invalid after #%d in %s.obj",
							obj_index[i].virtual, zone_table[zone].filename);
						perror(buf);
						ABORT_PROGRAM();
					}
					obj_index[i].maximum = lv_num;

/* GET FLAGS */
					tmppos = ftell(obj_f);
					tmpchar = 0;
					tmpchar = db7000_fread_string(obj_f);	/* short desc */
					if (tmpchar)
						free(tmpchar);
					tmpchar = db7000_fread_string(obj_f);	/* descrip */
					if (tmpchar)
						free(tmpchar);
					tmpchar = db7000_fread_string(obj_f);	/* action */
					if (tmpchar)
						free(tmpchar);
					gv_num = 0;
					fscanf(obj_f, " %ld ", &gv_num);	/* type */
					gv_num = 0;
					fscanf(obj_f, " %ld ", &gv_num);	/* flag1 */
					/* SET SPECIAL OBJECT FLAGS IN MASTER
					 * OBJECT_INDEX */
					if (IS_SET(gv_num, OBJ1_OMAX)) {
						SET_BIT(obj_index[i].flags, OBJI_OMAX);
					}
					/* GET BACK TO PROPER POSITION */
					fseek(obj_f, tmppos, 0);
				}
				else if (*buf == '$')	/* EOF */
					break;
			}
			else {
				perror("db4200_boot_obj: what the hell was that? unexpected EOF");
				bzero(buf, sizeof(buf));
				if (i >= 0)
					sprintf(buf, "ERROR: %s.obj is corrupt after obj#%d!?!",
						zone_table[zone].filename, obj_index[i].virtual);
				else
					sprintf(buf, "ERROR: %s.obj is corrupt!?!",
						zone_table[zone].filename);
				perror(buf);
				ABORT_PROGRAM();
			}
		}
		top_of_objt = i;
		fclose(obj_f);
	}			/* for loop */
	if (error && DIE_ON_ERROR) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR2: One or more fatal errors occurred; please fix and reboot the server.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}
}				/* END OF db4200_boot_obj() */
/* load the rooms */
void db4300_boot_world(void)
{

	FILE *fl;
	int room_nr = -1, zone, virtual_nr, tmp;
	char *temp, chk[MAX_STRING_LENGTH], last_read[MAX_STRING_LENGTH],
	  buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct extra_descr_data *new_descr;



	for (zone = 0; zone <= top_of_zone_table; zone++) {
		sprintf(chk, "areas/%s.wld", zone_table[zone].filename);
		if (gv_verbose_messages == TRUE) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Opening: %s\r\n", chk);
			main_log(buf2);
		}
		if (!(fl = fopen(chk, "rb"))) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "db4300_boot_world: could not open %s.wld\r\n",
				zone_table[zone].filename);
			perror(buf);
			ABORT_PROGRAM();
		}
		bzero(last_read, sizeof(last_read));
		while (1) {
			tmp = fscanf(fl, " #%d\n", &virtual_nr);
			temp = db7000_fread_string(fl);
			if (!(temp)) {
				fprintf(stderr, "ERROR: db4300_boot_world: NULL STRING (%s.wld is corrupt)\r\n",
					zone_table[zone].filename);
				fprintf(stderr, "Last line read: *%s*.\r\n", last_read);
				sprintf(buf, "ERROR: NULL value in wld file\r\n");
				main_log(buf);
				spec_log(buf, ERROR_LOG);
				ABORT_PROGRAM();
			}
			else {
				strcpy(last_read, temp);
			}
			if (*temp == '$') {
				free(temp);
				break;
			}
			else if (!tmp)
				fprintf(stderr, "ERROR: db4300_boot_world: no virtual # found (%s.wld is corrupt)\r\n",
					zone_table[zone].filename);

			room_nr++;
			db2100_allocate_room(room_nr);
			if (gv_mega_verbose_messages == TRUE) {
				bzero(buf2, sizeof(buf2));
				sprintf(buf2, "    Loading wld room: r%d %s",
					virtual_nr, temp);
				main_log(buf2);
			}
			world[room_nr].number = virtual_nr;
			world[room_nr].name = temp;

			/* WE NEED TO LOCATE THE DONATE ROOM */
			if (strncmp(temp, "The Midgaard Donation Room", 25) == 0)
				gv_donate_room = room_nr;

			if (virtual_nr == 13)
				gv_mob_donate_room = room_nr;

			world[room_nr].description = db7000_fread_string(fl);
			if (world[room_nr].description == 0) {
				bzero(buf2, sizeof(buf2));
				sprintf(buf2, "UNDEFINED ROOM DESCRIPTION: %d\r\n", world[room_nr].number);
				world[room_nr].description = strdup(buf2);
				/*
				 main_log(buf2);
				 */
			}
			world[room_nr].next = room_nr + 1;	/* start list sorted */
			if (top_of_zone_table >= 0) {
				fscanf(fl, " %*d ");
				/* OBS: Assumes ordering of input rooms */
				if ((world[room_nr].number > zone_table[zone].top) ||
				    (world[room_nr].number < zone_table[zone].bottom)) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "Boot_world: Room %d is outside zone %s (skipping rest).\r\n",
					virtual_nr, zone_table[zone].filename);
					perror(buf);
					ABORT_PROGRAM();
					/* room_nr -= 1; */
					break;
				}
				world[room_nr].zone = zone;
				if (zone_table[zone].real_bottom == -1)
					zone_table[zone].real_bottom = room_nr;
				if (room_nr > zone_table[zone].real_top)
					zone_table[zone].real_top = room_nr;
			}


			if (!fscanf(fl, " %d ", &tmp)) {
				perror("db4300_boot_world: no room flag(1) found\r\n");
				printf("%s.wld is corrupt)\r\n", zone_table[zone].filename);
				exit(1);
			}
			world[room_nr].room_flags = tmp;

			if (!fscanf(fl, " %d ", &tmp)) {
				perror("db4300_boot_world: no room flags(2) found\r\n");
				printf("%s.wld is corrupt)\r\n", zone_table[zone].filename);
				exit(1);
			}
			world[room_nr].room_flags2 = tmp;

			if (!fscanf(fl, " %d ", &tmp)) {
				perror("db4300_boot_world: no min level found\r\n");
				printf("%s.wld is corrupt)\r\n", zone_table[zone].filename);
				exit(1);
			}
			world[room_nr].min_level = tmp;

			if (!fscanf(fl, " %d ", &tmp)) {
				perror("db4300_boot_world: no max level found\r\n");
				printf("%s.wld is corrupt)\r\n", zone_table[zone].filename);
				exit(1);
			}
			world[room_nr].max_level = tmp;
			/* SAFETY CHECK */
			if (world[room_nr].min_level < 1 || world[room_nr].min_level > 99)
				world[room_nr].min_level = 1;
			if (world[room_nr].max_level < 1 || world[room_nr].max_level > 99)
				world[room_nr].max_level = 99;


			if (!fscanf(fl, " %d ", &tmp))
				fprintf(stderr, "db4300_boot_world: no sector type found (%s.wld is corrupt)\r\n", zone_table[zone].filename);
			world[room_nr].sector_type = tmp;
			world[room_nr].funct = 0;
			world[room_nr].contents = 0;
			world[room_nr].people = 0;
			world[room_nr].light = 0;	/* Zero light sources */
			for (tmp = 0; tmp <= 5; tmp++)
				world[room_nr].dir_option[tmp] = 0;
			world[room_nr].ex_description = 0;
			for (;;) {
				fscanf(fl, " %s \n", chk);
				if (*chk == 'D')	/* direction field */
					db4400_setup_dir(fl, room_nr, atoi(chk + 1));
				else if (*chk == 'E') {	/* extra description
							 * field */
					CREATE(new_descr, struct extra_descr_data, 1);
					new_descr->keyword = db7000_fread_string(fl);
					new_descr->description = db7000_fread_string(fl);
					new_descr->next = world[room_nr].ex_description;
					world[room_nr].ex_description = new_descr;
				}
				else if (*chk == 'S')	/* end of current room */
					break;
			}
		}
		fclose(fl);
	}			/* for loop to get all zone files */
	top_of_world = room_nr;
}				/* END OF db4300_boot_world() */


/* read direction data */
void db4400_setup_dir(FILE * fl, int room, int dir)
{
	int tmp;
	char *ptr;
	char buf[MAX_STRING_LENGTH];



	CREATE(world[room].dir_option[dir],
	       struct room_direction_data, 1);

	ptr = db7000_fread_string(fl);
	if (ptr) {
		if (!(strcmp(ptr, "You see nothing special."))) {
			free(ptr);
			ptr = 0;
		}
		else {
			if (!(strncmp(ptr, "You see", 7))) {
				memcpy(ptr, "       ", 7);
			}
		}
	}
	world[room].dir_option[dir]->general_description = ptr;

	world[room].dir_option[dir]->keyword = db7000_fread_string(fl);

	fscanf(fl, " %d ", &tmp);
	world[room].dir_option[dir]->exit_info = tmp;
	if (IS_SET(world[room].dir_option[dir]->exit_info, BIT15)) {
		sprintf(buf, "Room %d has bit 15 set, changed it to EX_CLIMB_ONLY\r\n",
			world[room].number);
		printf(buf);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		/* Temporary code to change some bits as we boot up.  Needs to
		 * be removed eventually. */
		SET_BIT(world[room].dir_option[dir]->exit_info, EX_CLIMB_ONLY);
		REMOVE_BIT(world[room].dir_option[dir]->exit_info, BIT15);
		SET_BIT(zone_table[world[room].zone].dirty_wld, UNSAVED);
	}

	fscanf(fl, " %d ", &tmp);
	world[room].dir_option[dir]->key = tmp;

	fscanf(fl, " %d ", &tmp);
	world[room].dir_option[dir]->to_room = tmp;

}				/* END OF db4400_setup_dir() */

void db4500_renum_world(void)
{

	int room, to_room, door;
	char buf[MAX_STRING_LENGTH];



	for (room = 0; room <= top_of_world; room++)
		for (door = 0; door <= 5; door++)
			if (world[room].dir_option[door]) {
				if (world[room].dir_option[door]->to_room < 0) {
					sprintf(buf, "ERROR: Link to negative room in %d\r\n",
						world[room].number);
					main_log(buf);
					spec_log(buf, ERROR_LOG);
				}
				else {
					/* IS ROOM VALID? */
					to_room =
						db8000_real_room(world[room].dir_option[door]->to_room);
					if (to_room < 0) {
						sprintf(buf, "ERROR: getting link to room, returned negative room in %d\r\n",
							world[room].number);
						main_log(buf);
						spec_log(buf, ERROR_LOG);
					}
					else {
						world[room].dir_option[door]->to_room = to_room;
					}
				}
			}

	return;

}				/* END OF db4500_renum_world() */


void db4600_renum_zone_table(void)
{
	int zone, comm;



	for (zone = 0; zone <= top_of_zone_table; zone++)
		for (comm = 0; zone_table[zone].cmd[comm].command != 'S'; comm++)
			switch (zone_table[zone].cmd[comm].command) {
			case 'M':
				zone_table[zone].cmd[comm].arg1 =
					db8100_real_mobile(zone_table[zone].cmd[comm].arg1);
				zone_table[zone].cmd[comm].arg3 =
					db8000_real_room(zone_table[zone].cmd[comm].arg3);
				break;
			case 'O':
				zone_table[zone].cmd[comm].arg1 =
					db8200_real_object(zone_table[zone].cmd[comm].arg1);
				if (zone_table[zone].cmd[comm].arg3 != NOWHERE)
					zone_table[zone].cmd[comm].arg3 =
						db8000_real_room(zone_table[zone].cmd[comm].arg3);
				break;
			case 'G':
				zone_table[zone].cmd[comm].arg1 =
					db8200_real_object(zone_table[zone].cmd[comm].arg1);
				break;
			case 'E':
				zone_table[zone].cmd[comm].arg1 =
					db8200_real_object(zone_table[zone].cmd[comm].arg1);
				break;
			case 'P':
				zone_table[zone].cmd[comm].arg1 =
					db8200_real_object(zone_table[zone].cmd[comm].arg1);
				zone_table[zone].cmd[comm].arg3 =
					db8200_real_object(zone_table[zone].cmd[comm].arg3);
				break;
			case 'R':
				zone_table[zone].cmd[comm].arg1 =
					db8200_real_object(zone_table[zone].cmd[comm].arg1);
				if (zone_table[zone].cmd[comm].arg3 != NOWHERE)
					zone_table[zone].cmd[comm].arg3 =
						db8000_real_room(zone_table[zone].cmd[comm].arg3);
				break;
			case 'D':
				zone_table[zone].cmd[comm].arg1 =
					db8000_real_room(zone_table[zone].cmd[comm].arg1);
				break;
			}
}				/* END OF db4600_renum_zone_table() */
 /* load the zone table and command tables */
void db4700_boot_zones(void)
{

	FILE *fl, *tbl;
	int zon = 0, cmd_no = 0, expand, tmp;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], temp1[MAX_STRING_LENGTH];



	if (!(tbl = fopen("zones.tbl", "rb"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: db4700_boot_zones(zones.tbl)\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}
	while (1) {		/* Read in the Zone Table */
		bzero(temp1, sizeof(temp1));
		for (fscanf(tbl, " %s", temp1);
		     *temp1 == '*' || *temp1 == '#';
		     fscanf(tbl, " %s", temp1));
		if (*temp1 == '$')
			break;	/* end of file */

		if (gv_verbose_messages == TRUE) {
			printf("Read zonetbl: %s\r\n", temp1);
		}

		/* alloc a new zone */
		if (!zon) {
			CREATE(zone_table, struct zone_data, 1);
		}
		else {
			if (!(zone_table = (struct zone_data *)
			      realloc(zone_table, (zon + 1) * sizeof(struct zone_data)))) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: db4700_boot_zones realloc.\r\n");
				perror(buf);
				ABORT_PROGRAM();
			}
			bzero((char *) &zone_table[zon], sizeof(struct zone_data));
		}
		fscanf(tbl, " %d %d ",
		       &zone_table[zon].bottom,
		       &zone_table[zon].top);
		zone_table[zon].filename = str_alloc(temp1);
		zone_table[zon].lord = db7000_fread_string(tbl);

		zone_table[zon].dirty_wld = 0;	/* start all zones "clean"  */
		zone_table[zon].dirty_mob = 0;
		zone_table[zon].dirty_obj = 0;
		zone_table[zon].dirty_zon = 0;
		zone_table[zon].real_bottom = -1;	/* nothing read in yet */
		zone_table[zon].real_top = -1;
		zone_table[zon].top_of_mobt = -1;
		zone_table[zon].bot_of_mobt = -1;
		zone_table[zon].top_of_objt = -1;
		zone_table[zon].bot_of_objt = -1;
		zone_table[zon].flags = 0;
		zone_table[zon].cmd = 0;
		zon++;
	}
	fclose(tbl);
	/* DECREMENT zon COUNTER BECAUSE OF zon++ AS LAST STATEMENT */
	top_of_zone_table = --zon;

	for (zon = 0; zon <= top_of_zone_table; zon++) {
		sprintf(buf, "areas/%s.zon", zone_table[zon].filename);
		if (gv_verbose_messages == TRUE) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Opening: %s\r\n", buf);
			main_log(buf2);
		}
		if (!(fl = fopen(buf, "rb"))) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ARG! zone %s.zon not found\r\n",
				zone_table[zon].filename);
			perror(buf);
			ABORT_PROGRAM();
		}

		fscanf(fl, " #%d\n", &zone_table[zon].flags);
		/*
		 if (zone_table[zon].lord)
		 zone_table[zon].flags = ZONE_TESTING | ZONE_NO_ENTER;
		 */

		zone_table[zon].name = db7000_fread_string(fl);
		/* throw out old top of zone */
		fscanf(fl, " %d ", &zone_table[zon].lifespan);

		fscanf(fl, " %d ", &zone_table[zon].lifespan);
		fscanf(fl, " %d ", &zone_table[zon].reset_mode);

		fscanf(fl, " %d ", &zone_table[zon].x_coordinate);
		fscanf(fl, " %d ", &zone_table[zon].y_coordinate);
		fscanf(fl, " %d ", &zone_table[zon].z_coordinate);

		/* read the command table */
		cmd_no = 0;
		for (expand = 1;;) {
			if (expand)
				db2200_allocate_zcmd(zon, cmd_no + 1);
			expand = 1;
			fscanf(fl, " ");	/* skip blanks */
			fscanf(fl, "%c", &zone_table[zon].cmd[cmd_no].command);

			if (zone_table[zon].cmd[cmd_no].command == 'S')
				break;
			if (zone_table[zon].cmd[cmd_no].command == '*') {
				expand = 0;	/* note online edits will strip
						 * out comments */
				fgets(buf, 80, fl);	/* skip command */
				continue;
			}

			/* READ THE ZONE COMMAND LINE */
			fscanf(fl, " %d %d",
			       &tmp,
			       &zone_table[zon].cmd[cmd_no].arg1);
			zone_table[zon].cmd[cmd_no].if_flag = tmp;

			if (zone_table[zon].cmd[cmd_no].command == 'D') {
				bzero(buf, sizeof(buf));
				fscanf(fl, " %c", &buf[0]);
				switch (*buf) {
				case 'N':{
						zone_table[zon].cmd[cmd_no].arg2 = 0;
						break;
					}
				case 'E':{
						zone_table[zon].cmd[cmd_no].arg2 = 1;
						break;
					}
				case 'S':{
						zone_table[zon].cmd[cmd_no].arg2 = 2;
						break;
					}
				case 'W':{
						zone_table[zon].cmd[cmd_no].arg2 = 3;
						break;
					}
				case 'U':{
						zone_table[zon].cmd[cmd_no].arg2 = 4;
						break;
					}
				case 'D':{
						zone_table[zon].cmd[cmd_no].arg2 = 5;
						break;
					}
				default:
					if (is_number(buf))
						zone_table[zon].cmd[cmd_no].arg2 = atoi(buf);
					else {
						printf("Arg2 = %s", buf);
						db5625_print_error("LOC000 non zero arg2",
								   cmd_no,
								   zone_table[zon].cmd[cmd_no].command,
								   0,
								   zone_table[zon].cmd[cmd_no].arg1,
								   zone_table[zon].cmd[cmd_no].arg2,
								   zone_table[zon].cmd[cmd_no].arg3,
						     zone_table[zon].filename,
								   0);
					}	/* END OF else not numeric */
				}	/* END OF switch() */
			}	/* END OF 'D' */
			else {
				fscanf(fl, " %d", &zone_table[zon].cmd[cmd_no].arg2);
			}

			/* FORMAT FOR ZONE FILE */
			/* arg1     arg2                arg3               */
			/* O   IF   OBJECT   MAX (for room)      ROOM               */
			/* P   IF   OBJECT   MAX (in dest obj)   DESTINATION
			 * OBJECT */
			/* M   IF   MOB      MAX (in world)      ROOM               */
			/* G   IF   OBJECT   MAX (on mob)                           */
			/* E   IF   OBJECT   MAX (unused)        LOCATION           */
			/* D   IF   ROOM     DIRECTION           LOCK TYPE          */
			/* R   IF   OBJ/MOB  TYPE (1=obj 2=mob)  ROOM               */
			if (zone_table[zon].cmd[cmd_no].command == 'M' ||
			    zone_table[zon].cmd[cmd_no].command == 'O' ||
			    zone_table[zon].cmd[cmd_no].command == 'E' ||
			    zone_table[zon].cmd[cmd_no].command == 'P' ||
			    zone_table[zon].cmd[cmd_no].command == 'R' ||
			    zone_table[zon].cmd[cmd_no].command == 'D')
				fscanf(fl, " %d", &zone_table[zon].cmd[cmd_no].arg3);
			else
				zone_table[zon].cmd[cmd_no].arg3 = -1;

			fgets(buf, 80, fl);	/* read comment (if any) */
			if (expand)
				cmd_no++;
		}
		zone_table[zon].reset_num = cmd_no;
		fclose(fl);
	}			/* read all the zone files loop */
}				/* END OF  db4700_boot_zones() */

struct keyword_data *db4750_split_keywords(char *arg)
{

	struct keyword_data *org_keys = 0, *keys;
	char buf[MAX_STRING_LENGTH];

	do {
		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
		if (*buf) {
			if (!org_keys) {
				keys = (struct keyword_data *) malloc(sizeof(struct keyword_data));
				org_keys = keys;
			}
			else {
				keys->next = (struct keyword_data *) malloc(sizeof(struct keyword_data));
				keys = keys->next;
			};
		}
		bzero((char *) keys, sizeof(struct keyword_data));
		keys->keyword = strdup(buf);
	} while ((arg) && (*arg));
	return (org_keys);
}

void db4800_boot_quests(void)
{
	int zone, vmob;
	int i = -1;
	int tmp;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char *input;
	struct conversation_data *convo, *org_convo = 0;
	FILE *qst_f;

	for (zone = 0; zone <= top_of_zone_table; zone++) {
		sprintf(buf, "areas/%s.qst", zone_table[zone].filename);
		if (gv_verbose_messages == TRUE) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "Opening: %s\r\n", buf);
			main_log(buf2);
		}
		if (!(qst_f = fopen(buf, "rb")))
			continue;

		rewind(qst_f);
		if (fgets(buf, 81, qst_f)) {
			while (*buf == '#') {	/* allocate new cell */
				org_convo = 0;
				convo = 0;
				i++;
				db2500_allocate_quest(i);
				if (!sscanf(buf, "#%d", &vmob)) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "Arg no virtual in %s.qst", zone_table[zone].filename);
					perror(buf);
					ABORT_PROGRAM();
				}
				quest_index[i].vmob = vmob;

				do {
					if (!org_convo) {
						convo = (struct conversation_data *) malloc(sizeof(struct conversation_data));
						org_convo = convo;
					}
					else {
						convo->next = (struct conversation_data *) malloc(sizeof(struct conversation_data));
						convo = convo->next;
					};
					bzero((char *) convo, sizeof(struct conversation_data));
					fscanf(qst_f, " %d ", &tmp);	/* number */
					convo->number = tmp;
					fscanf(qst_f, " %d ", &tmp);	/* low level */
					convo->low = tmp;
					fscanf(qst_f, " %d ", &tmp);	/* high level */
					convo->high = tmp;

					input = db7000_fread_string(qst_f);	/* keywords */
					if (!input) {
						bzero(buf, sizeof(buf));
						sprintf(buf, "ERROR: %s.qst is corrupt!", zone_table[zone].filename);
						perror(buf);
						ABORT_PROGRAM();
					}
					convo->keywords = db4750_split_keywords(input);
					convo->reply = db7000_fread_string(qst_f);	/* reply string */

					fgets(buf, 81, qst_f);
				} while (*buf == '~');
				quest_index[i].convo = org_convo;
			}
		}
		else {
			perror("db4800_boot_quests: what the hell was that? unexpected EOF");
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: %s.qst is corrupt!", zone_table[zone].filename);
			perror(buf);
			ABORT_PROGRAM();
		}
		top_of_questt = i;
		sprintf(buf, "Read %d quest(s)", i + 1);
		main_log(buf);
		fclose(qst_f);
	}			/* for loop for zones */
}



/******************************************************************** *
*  procedures for resetting, both play-time and boot-time             *
********************************************************************* */

/* read a mobile from MOB_FILE */
struct char_data *db5000_read_mobile(int nr, int type)
{

	int lv_value, lv_attack_num, lv_attack_type, lv_attack_skill, lv_default_mob_skill,
	  i, idx, jdx, kdx;
	long tmp, tmp2, tmp3;
	struct char_data *mob, *proto_mob;
	char lv_filename[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH],
	  buf2[MAX_STRING_LENGTH];
	char letter;



	i = nr;
	if ((type == VIRTUAL) || (type == PROTOTYPE)) {
		nr = db8100_real_mobile(nr);
		if ((nr < 0) || (nr > top_of_mobt)) {
			sprintf(buf, "Mobile(R) %d does not exist in database.", i);
			main_log(buf);
			return (0);
		}
	}

	gv_total_mobiles++;

	/* NOW WE FIND IF THE MOB IS LOADED IN MEMORY ALREADY, OR IS */
	/* GOING TO COME FROM THE FILE */
	proto_mob = (struct char_data *) mob_index[nr].prototype;
	if (proto_mob) {
		if (type == PROTOTYPE) {
			return (proto_mob);
		}
		CREATE(mob, struct char_data, 1);
		db7500_clear_char(mob);

		/*
			     my_var = my_var + 1;
			     printf("var = %d\n", my_var);
			     if (my_var == my_stop) {
				     printf("var = %d\n", my_var);
				     ptr = proto_mob->player.name;
			     }
			     */

		/***** String data *** */
		mob->player.name = str_alloc(proto_mob->player.name);
		mob->player.short_descr = str_alloc(proto_mob->player.short_descr);
		mob->player.long_descr = str_alloc(proto_mob->player.long_descr);
		mob->player.description = str_alloc(proto_mob->player.description);
		mob->player.title = 0;

		/* *** Numeric data *** */

		GET_ACT1(mob) = GET_ACT1(proto_mob);
		GET_ACT2(mob) = GET_ACT2(proto_mob);
		GET_ACT3(mob) = GET_ACT3(proto_mob);

		for (idx = 0; idx < MAX_SKILLS; idx++) {
			if (proto_mob->skills[idx].learned > 99)
				mob->skills[idx].learned = 99;
			else
				mob->skills[idx].learned = proto_mob->skills[idx].learned;
		}

		mob->specials.affected_by = proto_mob->specials.affected_by;
		mob->specials.affected_02 = 0;
		mob->specials.jailtime = 0;
		mob->specials.deathshadowtime = 0;
		mob->specials.godmuzzletime = 0;
		mob->specials.clan.number = 0;
		mob->specials.clan.rank = 0;
		mob->specials.clan.flags1 = 0;
		mob->countdown = 0;
		mob->questmob = 0;
		mob->questobj = 0;
		mob->nextquest = 0;
		mob->reward = 0;
		mob->screen_lines = 25;
		mob->specials.alignment = proto_mob->specials.alignment;
		GET_RACE(mob) = GET_RACE(proto_mob);
		GET_CLASS(mob) = GET_CLASS(proto_mob);
		mob->abilities.str = GET_REAL_STR(proto_mob);
		mob->abilities.intel = GET_REAL_INT(proto_mob);
		mob->abilities.wis = GET_REAL_WIS(proto_mob);
		mob->abilities.dex = GET_REAL_DEX(proto_mob);
		mob->abilities.con = GET_REAL_CON(proto_mob);
		mob->abilities.cha = GET_REAL_CHA(proto_mob);
		GET_LEVEL(mob) = GET_LEVEL(proto_mob);
		mob->points.used_exp = 0;
		mob->points.fspells = 0;
		mob->points.class1 = 0;
		mob->points.class2 = 0;
		mob->points.class3 = 0;
		mob->points.prompt = 1;
		mob->points.extra_hits = 0;
		mob->points.hitroll = proto_mob->points.hitroll;
		mob->points.armor = proto_mob->points.armor;
		mob->points.max_hit = dice(proto_mob->points.max_hit,
					   proto_mob->player.weight)
			+ proto_mob->points.hit;
		mob->points.hit = mob->points.max_hit;
		mob->points.damroll = proto_mob->points.damroll;
		mob->specials.damnodice = proto_mob->specials.damnodice;
		mob->specials.damsizedice = proto_mob->specials.damsizedice;
		mob->points.mana = proto_mob->points.mana;
		mob->points.max_mana = proto_mob->points.max_mana;
		mob->points.move = proto_mob->points.move;
		mob->points.max_move = proto_mob->points.max_move;
		mob->points.gold = proto_mob->points.gold;
		GET_EXP(mob) = GET_EXP(proto_mob);
		mob->specials.position = proto_mob->specials.position;
		mob->specials.default_pos = proto_mob->specials.default_pos;
		mob->player.sex = proto_mob->player.sex;

		/* init play values */
		mob->player.class = GET_CLASS(proto_mob);
		mob->player.race = GET_RACE(proto_mob);
		mob->player.time.birth = time(0);
		mob->player.time.played = 0;
		mob->player.time.logon = time(0);
		mob->player.weight = 200;
		mob->player.height = 198;
		GET_START(mob) = 0;

		for (i = 0; i < 3; i++)
			GET_COND(mob, i) = -1;
		ha9700_set_saving_throws(mob, 0);
		mob->tmpabilities = mob->abilities;
		mob->bonus.str = 0;
		mob->bonus.dex = 0;
		mob->bonus.cha = 0;
		for (i = 0; i < MAX_WEAR; i++)	/* Initialisering Ok */
			mob->equipment[i] = 0;
		mob->nr = nr;
		mob->desc = 0;
		mob->questdata = 0;

		for (kdx = 0; kdx <= top_of_questt; kdx++)
			if (quest_index[kdx].vmob == mob_index[nr].virtual) {
				mob->questdata = &quest_index[kdx];
				break;
			};


		/* insert in list */
		mob->next = character_list;
		character_list = mob;
		mob_index[nr].number++;
		return (mob);	/* loaded mob data from prototype */
	}
	if (mob_f_zone != mob_index[nr].zone) {
		if (mob_f_zone != -1)
			fclose(mob_f);
		bzero(lv_filename, sizeof(lv_filename));
		sprintf(lv_filename, "areas/%s.mob",
			zone_table[mob_index[nr].zone].filename);
		if (!(mob_f = fopen(lv_filename, "rb"))) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ARG! file %s.mob not found\r\n",
				zone_table[mob_index[nr].zone].filename);
			perror(buf);
			ABORT_PROGRAM();
		}
	}			/* load mob data from file */
	mob_f_zone = mob_index[nr].zone;
	fseek(mob_f, mob_index[nr].pos, 0);
	CREATE(mob, struct char_data, 1);
	db7500_clear_char(mob);


	/***** String data *** */
	mob->player.name = db7000_fread_string(mob_f);
	mob->player.short_descr = db7000_fread_string(mob_f);
	mob->player.long_descr = db7000_fread_string(mob_f);
	mob->player.description = db7000_fread_string(mob_f);
	mob->player.title = 0;

	/* *** Numeric data *** */

	fscanf(mob_f, "%ld ", &tmp);
	GET_ACT1(mob) = tmp;
	REMOVE_BIT(GET_ACT1(mob), gv_anti1_NPC_bits);
	SET_BIT(GET_ACT1(mob), gv_good1_NPC_bits);

	fscanf(mob_f, "%ld ", &tmp);
	GET_ACT2(mob) = tmp;
	REMOVE_BIT(GET_ACT2(mob), gv_anti2_NPC_bits);
	SET_BIT(GET_ACT2(mob), gv_good2_NPC_bits);

	fscanf(mob_f, "%ld ", &tmp);
	GET_ACT3(mob) = tmp;
	REMOVE_BIT(GET_ACT3(mob), gv_anti3_NPC_bits);
	SET_BIT(GET_ACT3(mob), gv_good3_NPC_bits);

	SET_BIT(GET_ACT3(mob), PLR3_BRIEF);
	SET_BIT(GET_ACT3(mob), PLR3_AUTOAGGR);

	fscanf(mob_f, " %ld ", &tmp);
	mob->specials.affected_by = tmp;
	fscanf(mob_f, " %ld ", &tmp);
	mob->specials.alignment = tmp;
	fscanf(mob_f, " %c \n", &letter);

	if (letter == 'S') {

		fscanf(mob_f, " %ld ", &tmp);
		GET_LEVEL(mob) = tmp;
		fscanf(mob_f, " %ld ", &tmp);
		mob->points.hitroll = 40 - tmp;
		if (mob->points.hitroll < 0)
			mob->points.hitroll = 0;
		if (mob->points.hitroll > 99)
			mob->points.hitroll = 99;

		fscanf(mob_f, " %ld ", &tmp);
		mob->points.armor = 10 * tmp;

		fscanf(mob_f, " %ldd%ld+%ld ", &tmp, &tmp2, &tmp3);
		if (type == PROTOTYPE) {
			mob->points.max_hit = MAXV(1, tmp);
			mob->points.hit = MAXV(0, tmp3);
			mob->player.weight = MAXV(1, tmp2);
		}
		else {
			mob->points.max_hit = dice(MAXV(1, tmp), MAXV(1, tmp2)) + MAXV(0, tmp3);
			mob->points.hit = mob->points.max_hit;
			mob->player.weight = 200;
		}
		fscanf(mob_f, " %ldd%ld+%ld \n", &tmp, &tmp2, &tmp3);
		mob->points.damroll = MAXV(0, tmp3);
		mob->specials.damnodice = MAXV(1, tmp);
		mob->specials.damsizedice = MAXV(1, tmp2);

		mob->points.move = 50;
		mob->points.max_move = 50;

		fscanf(mob_f, " %ld ", &tmp);
		mob->points.gold = tmp;
		fscanf(mob_f, " %ld \n", &tmp);
		GET_EXP(mob) = tmp;
		fscanf(mob_f, " %ld ", &tmp);
		mob->specials.position = MINV(POSITION_STANDING, MAXV(POSITION_SLEEPING, tmp));
		fscanf(mob_f, " %ld ", &tmp);
		mob->specials.default_pos = MINV(POSITION_STANDING, MAXV(POSITION_SLEEPING, tmp));
		fscanf(mob_f, " %ld \n", &tmp);
		mob->player.sex = tmp;

		/* READ RACE/CLASS */
		fscanf(mob_f, " %ld ", &tmp);
		GET_RACE(mob) = tmp;
		if (GET_RACE(mob) < 0 || GET_CLASS(mob) > MAX_RACES) {
			sprintf(buf, "ERROR! file %s.mob has invalid race %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}

		fscanf(mob_f, " %ld ", &tmp);
		GET_CLASS(mob) = tmp;
		if (GET_CLASS(mob) < 0 || GET_CLASS(mob) > MAX_CLASSES) {
			sprintf(buf, "ERROR! file %s.mob has invalid class %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}

		/* READ MOB STATS: str, int, wis, dex, con, cha */

		fscanf(mob_f, " %ld ", &tmp);
		GET_REAL_STR(mob) = tmp;
		if (GET_REAL_STR(mob) < 0 || GET_REAL_STR(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid str %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}
		fscanf(mob_f, " %ld ", &tmp);
		GET_REAL_INT(mob) = tmp;
		if (GET_REAL_INT(mob) < 0 || GET_REAL_INT(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid int %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}
		fscanf(mob_f, " %ld ", &tmp);
		GET_REAL_WIS(mob) = tmp;
		if (GET_REAL_WIS(mob) < 0 || GET_REAL_WIS(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid wis %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}
		fscanf(mob_f, " %ld ", &tmp);
		GET_REAL_DEX(mob) = tmp;
		if (GET_REAL_DEX(mob) < 0 || GET_REAL_DEX(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid dex %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}
		fscanf(mob_f, " %ld ", &tmp);
		GET_REAL_CON(mob) = tmp;
		if (GET_REAL_CON(mob) < 0 || GET_REAL_CON(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid con %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}
		fscanf(mob_f, " %ld \n", &tmp);
		GET_REAL_CHA(mob) = tmp;
		if (GET_REAL_CHA(mob) < 0 || GET_REAL_CHA(mob) > 99) {
			sprintf(buf, "ERROR! file %s.mob has invalid cha %ld\r\n",
				zone_table[mob_index[nr].zone].filename, tmp);
			ABORT_PROGRAM();
		}

		for (jdx = 0; jdx < MAX_SKILLS; jdx++) {
			mob->skills[jdx].learned = 0;
		}

		/* READ SKILLS/SPELLS */
		lv_attack_num = 0;
		bzero(buf1, sizeof(buf1));
		strcpy(buf1, db7350_read_from_file(mob_f));

		lv_attack_type = 0;
		lv_attack_skill = 0;
		while (*buf1) {

			/* We have a record, Make sure its: */
			if (!strncmp("SKILL=", buf1, 6)) {
				bzero(buf2, sizeof(buf2));
				strcpy(buf2, db5250_parse_str("SKILL=", buf1,
							    lv_filename, 50));
				/* FIND NUMERIC VLAUE OF SKILL */
				lv_value = db5280_skill_number(buf2, buf);
				mob->skills[lv_value].learned =
					db5290_skill_percentage(buf2, buf);
			}
			else if (!strncmp("ATTACK_TYPE=", buf1, 12)) {
				/* PROVIDE FILE NAME */
				lv_value = db5275_part_num("ATTACK_TYPE=", buf1,
					   lv_filename, TYPE_HIT, TYPE_CRUSH);
				mob_index[nr].attack_type[lv_attack_type] = lv_value;
				lv_attack_type++;
				if (lv_attack_type > MAX_SPECIAL_ATTACK) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "ERROR: To many special attacks in %s.\r\n", lv_filename);
					perror(buf);
					ABORT_PROGRAM();
				}
				lv_attack_skill = 0;
			}
			else if (!strncmp("ATTACK_SKILL=", buf1, 13)) {
				lv_value = db5275_part_num("ATTACK_SKILL=", buf1,
							 lv_filename, 0, 100);
				mob_index[nr].attack_skill[lv_attack_type][lv_attack_skill] = lv_value;
				lv_attack_skill++;
			}
			else {
				/* FORCE EXIT */
				bzero(buf1, sizeof(buf1));
			}

			if (*buf1) {
				strcpy(buf1, db7350_read_from_file(mob_f));
			}
		}		/* END OF while */

		lv_default_mob_skill = (GET_LEVEL(mob) / 5 * 10) + 10;
		if (lv_default_mob_skill > 99)
			lv_default_mob_skill = 99;

		if (mob->skills[SKILL_SLASH].learned == 0)
			mob->skills[SKILL_SLASH].learned = lv_default_mob_skill;
		if (mob->skills[SKILL_BLUDGEON].learned == 0)
			mob->skills[SKILL_BLUDGEON].learned = lv_default_mob_skill;
		if (mob->skills[SKILL_PIERCE].learned == 0)
			mob->skills[SKILL_PIERCE].learned = lv_default_mob_skill;

		mob->player.time.birth = time(0);
		mob->player.time.played = 0;
		mob->player.time.logon = time(0);
		mob->player.height = 198;
		if (type != PROTOTYPE) {
			for (i = 0; i < 3; i++)
				GET_COND(mob, i) = -1;
			ha9700_set_saving_throws(mob, 0);
		}
	}
	else {			/* encountered an old style mob */
		fprintf(stderr, "(old style mob) Mobile(%c) %d does not exist in database.\r\n", letter, i);
		fprintf(stderr, "read_mob: old style mob not supported\r\n");
		/* not going to support old mobs */
		main_log("ERROR: old mob(1)");
		exit(1);
	}
	mob->desc = 0;
	mob->nr = nr;
	mob->questdata = 0;

	for (kdx = 0; kdx <= top_of_questt; kdx++)
		if (quest_index[kdx].vmob == mob_index[nr].virtual) {
			mob->questdata = &quest_index[kdx];
			break;
		};


	if (type == PROTOTYPE) {
		mob_index[nr].prototype = (char *) mob;
		mob->next = 0;
	}
	else {
		mob->tmpabilities = mob->abilities;
		for (i = 0; i < MAX_WEAR; i++)	/* Initialisering Ok */
			mob->equipment[i] = 0;
		/* insert in list */
		mob->next = character_list;
		character_list = mob;
		mob_index[nr].number++;
	}

	/* VERIFY NAME */
	if (!(mob->player.name)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR MOB NAME: mobile: V[%d] in zone: %s without name.\r\n",
			nr, zone_table[mob_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		bzero(buf, sizeof(buf));
		sprintf(buf, "UNDEFINED NAME:%d", nr);
		mob->player.name = strdup(buf);
	}

	/* VERIFY SHORT DESCRIPTION */
	if (!(mob->player.short_descr)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR MOB DESC1: mobile: V[%d] %s in zone: %s without short description\r\n",
		nr, mob->player.name, zone_table[mob_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		bzero(buf, sizeof(buf));
		sprintf(buf, "UNDEFINED MOB SDESCR: %s", mob->player.name);
		mob->player.short_descr = strdup(buf);
	}

	/* VERIFY LONG DESCRIPTION */
	if (!(mob->player.long_descr)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR MOB DESC1: mobile: V[%d] %s in zone: %s without long description\r\n",
		nr, mob->player.name, zone_table[mob_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		bzero(buf, sizeof(buf));
		sprintf(buf, "UNDEFINED SDESCRIPTION: %s", mob->player.name);
		mob->player.long_descr = strdup(buf);
	}

	/* VERIFY DESCRIPTION */
	if (!(mob->player.description)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR MOB DESC2: mobile: V[%d] %s in zone: %s without description\r\n",
		nr, mob->player.name, zone_table[mob_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		sprintf(buf, "UNDEFINED DESCRIPTION: %s", mob->player.name);
		mob->player.description = strdup(buf);
	}
	mob->points.max_mana = dice(GET_INT(mob), GET_LEVEL(mob)) + (GET_LEVEL(mob) * 10);
	mob->points.mana = mob->points.max_mana;

	return (mob);		/* loaded mob_data from file */
}				/* END OF db5000_read_mobil() */
/* read an object from OBJ_FILE */
struct obj_data *db5100_read_object(int nr, int type)
{
	struct obj_data *obj, *proto_obj;
	int tmp, i, jdx;
	char chk[50], buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct extra_descr_data *new_descr;
	struct extra_descr_data *old_descr;

	i = nr;
	if ((type == VIRTUAL) || (type == PROTOTYPE))
		if ((nr = db8200_real_object(nr)) < 0) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Object(V) %d does not exist in database.", i);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			return (0);
		}
	if (((nr < 0) || (nr > top_of_objt)) && (type == REAL)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Object(R) %d does not exist in database.", i);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return (0);
	}

	gv_total_objects++;

	if ((proto_obj = (struct obj_data *) obj_index[nr].prototype)) {
		if (type == PROTOTYPE) {
			return (proto_obj);
		}
		CREATE(obj, struct obj_data, 1);
		db7600_clear_object(obj);

		/* *** string data *** */
		obj->name = str_alloc(proto_obj->name);
		obj->short_description = str_alloc(proto_obj->short_description);
		obj->description = str_alloc(proto_obj->description);
		obj->action_description = str_alloc(proto_obj->action_description);

		/* *** numeric data *** */
		obj->obj_flags.type_flag = proto_obj->obj_flags.type_flag;
		obj->obj_flags.flags1 = proto_obj->obj_flags.flags1;
		obj->obj_flags.flags2 = proto_obj->obj_flags.flags2;
		obj->obj_flags.wear_flags = proto_obj->obj_flags.wear_flags;
		obj->obj_flags.value[0] = proto_obj->obj_flags.value[0];
		obj->obj_flags.value[1] = proto_obj->obj_flags.value[1];
		obj->obj_flags.value[2] = proto_obj->obj_flags.value[2];
		obj->obj_flags.value[3] = proto_obj->obj_flags.value[3];

		obj->obj_flags.weight = proto_obj->obj_flags.weight;
		obj->obj_flags.cost = proto_obj->obj_flags.cost;
		obj->obj_flags.cost_per_day = proto_obj->obj_flags.cost_per_day;

		/* *** extra descriptions *** */
		obj->ex_description = 0;
		old_descr = proto_obj->ex_description;
		while (old_descr) {
			CREATE(new_descr, struct extra_descr_data, 1);
			new_descr->keyword = str_alloc(old_descr->keyword);
			new_descr->description = str_alloc(old_descr->description);
			new_descr->next = obj->ex_description;
			obj->ex_description = new_descr;
			old_descr = old_descr->next;
		}

		for (i = 0; (i < MAX_OBJ_AFFECT); i++) {
			obj->affected[i].location = proto_obj->affected[i].location;
			obj->affected[i].modifier = proto_obj->affected[i].modifier;
		}

		obj->in_room = NOWHERE;
		obj->next_content = 0;
		obj->carried_by = 0;
		obj->in_obj = 0;
		obj->contains = 0;
		obj->item_number = nr;
		obj->next = object_list;
		object_list = obj;
		obj_index[nr].number++;

		if (type != PROTOTYPE) {
			db5150_add_obj_to_vehicle_list(obj);
		}
		return (obj);
	}
	if (obj_f_zone != obj_index[nr].zone) {
		if (obj_f_zone != -1)
			fclose(obj_f);
		bzero(buf, sizeof(buf));
		sprintf(buf, "areas/%s.obj", zone_table[obj_index[nr].zone].filename);
		if (!(obj_f = fopen(buf, "rb"))) {
			fprintf(stderr, "ARG! file %s.obj not found\r\n", zone_table[obj_index[nr].zone].filename);
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: read_obj.\r\n");
			perror(buf);
			ABORT_PROGRAM();
		}
	}
	obj_f_zone = obj_index[nr].zone;
	fseek(obj_f, obj_index[nr].pos, 0);
	CREATE(obj, struct obj_data, 1);
	db7600_clear_object(obj);

	/* *** string data *** */
	obj->name = db7000_fread_string(obj_f);
	fgets(buf2, 81, obj_f);
	obj->short_description = db7000_fread_string(obj_f);
	obj->description = db7000_fread_string(obj_f);
	obj->action_description = db7000_fread_string(obj_f);

	/* the following checks for an extra /n/r at the end of the obj
	 * description */
	/* fix for screwed up object descriptions */
	/* can be taken out in the future */
	{
		char *the_desc;
		int the_size;

		the_desc = obj->description;
		if (the_desc) {
			the_size = strlen(the_desc);
			if (the_size > 2)
				if ((the_desc[the_size - 2] == '\n') && (the_desc[the_size - 1] == '\r'))
					the_desc[the_size - 2] = 0;
		}
	}
	/* fix for screwed up object descriptions */
	/* can be taken out in the future */

	/* *** numeric data *** */
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.type_flag = tmp;

	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.flags1 = tmp;

	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.flags2 = tmp;
	REMOVE_BIT(GET_OBJ2(obj), OBJ2_VEHICLE_REV);


	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.wear_flags = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[0] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[1] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[2] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[3] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.weight = tmp;
	fscanf(obj_f, " %d \n", &tmp);
	obj->obj_flags.cost = tmp;
	fscanf(obj_f, " %d \n", &tmp);
	obj->obj_flags.cost_per_day = tmp;

	/* LOCATE OBJECTS THAT HEAL FOR SWITCH OF HEAL/DISPEL MAGIC */
	/*
	 tmp = 28;
	 if (obj->obj_flags.value[0] == tmp ||
			 obj->obj_flags.value[1] == tmp ||
			 obj->obj_flags.value[2] == tmp ||
			 obj->obj_flags.value[3] == tmp) {
		 bzero(buf, sizeof(buf));
		 sprintf(buf, "Object: %s in zone %d does %s\r\n",
						 obj->name,
						 obj_index[nr].zone,
						 spell_names[tmp - 1]);
		 printf(buf);
	 }
	 */

	/* *** extra descriptions *** */
	obj->ex_description = 0;
	while (fscanf(obj_f, " %s \n", chk), *chk == 'E') {
		CREATE(new_descr, struct extra_descr_data, 1);
		new_descr->keyword = db7000_fread_string(obj_f);
		new_descr->description = db7000_fread_string(obj_f);
		new_descr->next = obj->ex_description;
		obj->ex_description = new_descr;
	}

	for (i = 0; (i < MAX_OBJ_AFFECT) && (*chk == 'A'); i++) {
		fscanf(obj_f, " %d ", &tmp);
		obj->affected[i].location = tmp;
		fscanf(obj_f, " %d \n", &tmp);
		obj->affected[i].modifier = tmp;
		fscanf(obj_f, " %s \n", chk);

		/* MAKE SURE REGEN ALWAYS AFFECTS BY ONE if
		 * (obj->affected[i].location > APPLY_REGENERATION) {
		 * obj->affected[i].modifier = -1; } */

		if (obj->affected[i].location > APPLY_last_apply) {
			sprintf(buf, "ERROR %s has invalid APPLY.  Zeroing it. \r\n",
				GET_OBJ_NAME(obj));
			printf(buf);
			sprinttype(obj->affected[i].location, apply_types, buf2);
			sprintf(buf, " %d   Affects : %s By %d\r\n",
				obj->affected[i].location,
				buf2,
				obj->affected[i].modifier);
			printf(buf);
			/* ZERO IT */
			obj->affected[i].location = 0;
			obj->affected[i].modifier = 0;
		}		/* END OF INVALID APPLY */
	}			/* END OF FOR LOOP */

	/* NOTE:  THIS LOOP TAKES UP WHERE THE OTHER LEFT OFF */
	for (jdx = i; jdx < MAX_OBJ_AFFECT; jdx++) {
		obj->affected[jdx].location = APPLY_NONE;
		obj->affected[jdx].modifier = 0;
	}

	obj->next_content = 0;
	obj->carried_by = 0;
	obj->in_obj = 0;
	obj->contains = 0;
	obj->item_number = nr;
	obj->in_room = NOWHERE;
	if (type == PROTOTYPE) {
		obj_index[nr].prototype = (char *) obj;
	}
	else {
		obj->next = object_list;
		object_list = obj;
		obj_index[nr].number++;
	}

	/* VERIFY NAME */
	if (!(obj->name)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR OBJ NAME: object: V[%d] in zone: %s without name.\r\n",
			nr, zone_table[obj_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		bzero(buf, sizeof(buf));
		sprintf(buf, "UNDEFINED NAME:%d", nr);
		obj->name = strdup(buf);
	}

	/* VERIFY SHORT DESCRIPTION */
	if (!(obj->short_description)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR OBJ DESC1: object: V[%d] %s in zone: %s without short description\r\n",
		      nr, obj->name, zone_table[obj_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		bzero(buf, sizeof(buf));
		sprintf(buf, "UNDEFINED SDESCRIPTION: %s", obj->name);
		obj->short_description = strdup(buf);
	}

	/* VERIFY DESCRIPTION */
	if (!(obj->description)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR OBJ DESC2: object: V[%d] %s in zone: %s without description\r\n",
		      nr, obj->name, zone_table[obj_index[nr].zone].filename);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		sprintf(buf, "UNDEFINED DESCRIPTION: %s", obj->name);
		obj->description = strdup(buf);
	}

	if (type != PROTOTYPE) {
		db5150_add_obj_to_vehicle_list(obj);
	}
	return (obj);

}				/* END OF db5100_read_object() */

void db5150_add_obj_to_vehicle_list(struct obj_data * lv_obj)
{

	struct vehicle_data *lv_vehicle;
	char buf[MAX_STRING_LENGTH];



	/* IF THIS IS A VEHICLE, UPDATE VEHICLE LIST */
	if (GET_ITEM_TYPE(lv_obj) != ITEM_VEHICLE) {
		return;
	}

	if (CAN_WEAR(lv_obj, ITEM_TAKE)) {
		sprintf(buf, "Object %d is type VEHICLE, but is flagged TAKE",
			obj_index[lv_obj->item_number].virtual);
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		return;
	}

	CREATE(lv_vehicle, struct vehicle_data, 1);
	lv_vehicle->obj = lv_obj;
	lv_vehicle->room = 0;
	lv_vehicle->next = vehicle_list;
	vehicle_list = lv_vehicle;

}				/* END OF db5150_add_obj_to_vehicle_list() */


void db5175_remove_obj_from_vehicle_list(struct obj_data * lv_obj)
{

	struct vehicle_data *lv_veh, *last_veh;



	last_veh = 0;
	for (lv_veh = vehicle_list; lv_veh; lv_veh = lv_veh->next) {
		if (lv_veh->obj == lv_obj) {
			if (last_veh) {
				last_veh->next = lv_veh->next;
			}
			else {
				vehicle_list = 0;
			}
			free(lv_veh);
			return;
		}
		last_veh = lv_veh;
	}

	return;

}				/* END OF db5175_remove_obj_from_vehicle_list() */


char *db5250_parse_str(char *lv_search_str, char *str_in, char *file_name, int max_len)
{

	char buf[MAX_STRING_LENGTH];



	/* LOOK FOR STRING */
	if (strncmp(lv_search_str, str_in, strlen(lv_search_str))) {
		printf("ERROR: Unable to find %s in file %s\r\n",
		       lv_search_str, file_name);
		printf("String = *%s*/r/n", str_in);
		main_log("ERROR: db5250_parse_str(1)");
		exit(1);
	}

	bzero(buf, sizeof(buf));
	strcpy(buf, (char *) (str_in + strlen(lv_search_str)));

/* CHECK LENGTH */
	if (strlen(buf) == 0) {
		printf("ERROR: Can't find argument for %s in file %s\r\n",
		       lv_search_str, file_name);
		main_log("ERROR: db5250_parse_str(2)");
		exit(1);
	}

	if (strlen(buf) > max_len) {
		printf("ERROR: Str length for %s in file %s is > %d\r\n",
		       lv_search_str, file_name, max_len);
		printf("Adjusting Str size, cutting overhead.\r\n");
		buf[max_len + 1] = 0;
	}

	bzero(gv_str, sizeof(gv_str));
	strcpy(gv_str, buf);
	return (gv_str);

}				/* END OF db5250_parse_str() */

int db5275_part_num(char *lv_search_str, char *str_in, char *file_name, int min_val, int max_val)
{

	int lv_value, lv_sign, idx;
	char buf[MAX_STRING_LENGTH];



	/* LOOK FOR STRING */
	if (strncmp(lv_search_str, str_in, strlen(lv_search_str))) {
		printf("ERROR: Unable to find %s in file %s\r\n",
		       lv_search_str, file_name);
		main_log("ERROR: db5275_part_num(1)");
		exit(1);
	}

	bzero(buf, sizeof(buf));
	strcpy(buf, (char *) (str_in + strlen(lv_search_str)));

	/* NULL TERMINATE AT FIRST SPACE */
	for (idx = 0; idx < sizeof(buf); idx++) {
		if (buf[idx] == ' ')
			buf[idx] = 0;
	}

	/* CHECK LENGTH */
	if (strlen(buf) == 0) {
		printf("ERROR: Can't find argument for %s in file %s\r\n",
		       lv_search_str, file_name);
		main_log("ERROR: db5275_part_num(2)");
		exit(1);
	}

	/* LOOK FOR SIGN */
	lv_sign = 1;
	if (buf[0] == '-') {
		lv_sign = -1;
		strcpy(buf, buf + 1);
	}
	if (buf[0] == '+') {
		strcpy(buf, buf + 1);
	}

	/* IS IT A NUMBER? */
	if (!(is_number(buf))) {
		printf("ERROR: Argument for %s in file %s is not numeric *%s*.\r\n",
		       lv_search_str, file_name, str_in);
		main_log("ERROR: db5275_part_num(3)");
		exit(1);
	}

	lv_value = atoi(buf) * lv_sign;

	/* IS IT IN THE CORRECT RANGE? */
	if (lv_value < min_val || lv_value > max_val) {
		printf("ERROR: Argument %d for %s in file %s is out of bounds %d  %d\r\n",
		       lv_value, lv_search_str, file_name, min_val, max_val);
		main_log("ERROR: db5275_part_num(4)");
		exit(1);
	}

	return (lv_value);

}				/* END OF db5275_part_num() */


short int db5280_skill_number(char *str_in, char *file_name)
{

	int idx, lv_value;
	char buf1[MAX_STRING_LENGTH];



	if (*str_in == '0') {
		return (0);
	}

	/* STRIP OUT ACRONYM */
	bzero(buf1, sizeof(buf1));
	strcpy(buf1, str_in);
	for (idx = 0; idx < strlen(buf1); idx++)
		if (buf1[idx] == ' ')
			buf1[idx] = 0;

	lv_value = 0;
	lv_value = old_search_block(buf1, 0, strlen(buf1),
				    sk_sp_defines, 1);
	if (lv_value < 0 || lv_value >= MAX_SKILLS) {
		printf("ERROR: Skill %s in file %s is invalid.\r\n",
		       buf1, file_name);
		main_log("ERROR: db5280_skill_number(1)");
		exit(1);
	}

	return (lv_value);

}				/* END OF db5280_skill_number() */


int db5290_skill_percentage(char *str_in, char *file_name)
{

	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	sbyte lv_value;



	if (*str_in == '0') {
		return (0);
	}

	/* STRIP OUT ACRONYM */
	bzero(buf1, sizeof(buf1));
	str_in = (char *) one_argument(str_in, buf1);

	/* STRIP OUT PERCENTAGE */
	bzero(buf2, sizeof(buf2));
	str_in = (char *) one_argument(str_in, buf2);

	if (!(*buf2)) {
		printf("ERROR: Skill %s in file %s requires a value.\r\n",
		       str_in, file_name);
		main_log("ERROR: db5290(1)");
		exit(1);
	}

	/* IS IT A NUMBER? */
	if (!(is_number(buf2))) {
		printf("ERROR: percentage for skill %s in file %s is not numeric.\r\n",
		       buf1, file_name);
		main_log("ERROR: db5290(2)");
		exit(1);
	}

	lv_value = atoi(buf2);

	/* IS IT IN THE CORRECT RANGE? */
	if (lv_value < 0 || lv_value > 99) {
		printf("ERROR: percentage %d for skill %s in file %s is out of bounds 0 to 99\r\n",
		       lv_value, buf1, file_name);
		main_log("ERROR: db5290(3)");
		exit(1);
	}

	return (lv_value);

}				/* END OF db5290_skill_percentage() */


void db5200_load_races(void)
{

	FILE *race_file;
	int lv_value, idx, jdx;
	char file_name[80], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH],
	  buf3[MAX_STRING_LENGTH];



	/* ZERO STRUCTURE */
	for (idx = 0; idx < MAX_RACES; idx++) {
		bzero((char *) &races[idx], sizeof(struct race_data));
		strcpy(races[idx].name, "undefined");
		/* INIT MAX SKILL LEVELS */
		for (jdx = 0; jdx < MAX_SKILLS; jdx++) {
			races[idx].skill_max[jdx] = 99;
			races[idx].skill_min_level[jdx] = IMO_LEV;
		}
	}

	/* OPEN AND READ FILES */
	for (idx = 0; idx < MAX_RACES; idx++) {

		/* TRY TO OPEN THE FILE */
		bzero(file_name, sizeof(file_name));
		sprintf(file_name, "races/%d", idx);
		race_file = fopen(file_name, "rb");

		/* printf("Opening/reading %s\r\n", file_name); */
		//printf("Moo %d\r\n", idx);

		if (!race_file) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "ERROR: Unable to open file: %s errno: %d",
				file_name, errno);
			main_log(buf2);
			spec_log(buf2, ERROR_LOG);
			exit(1);
		}

		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].name, db5250_parse_str("NAME=", buf1, file_name, 19));
		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].desc, db5250_parse_str("DESC=", buf1, file_name, 5));

		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].text[0], db5250_parse_str("TEXT1=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].text[1], db5250_parse_str("TEXT2=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].text[2], db5250_parse_str("TEXT3=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(races[idx].text[3], db5250_parse_str("TEXT4=", buf1, file_name, 79));

		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(buf2, db5250_parse_str("ALIGNMENT=", buf1, file_name, 79));
		if (strcmp(buf2, "EVIL") == 0) {
			SET_BIT(races[idx].flag, RFLAG_EVIL_ALIGNMENT);
		}
		else {
			if (strcmp(buf2, "GOOD") == 0) {
				SET_BIT(races[idx].flag, RFLAG_GOOD_ALIGNMENT);
			}
			else {
				if (strcmp(buf2, "NORMAL") != 0) {
					printf("ERROR: ALIGNMENT=%s in file %s is not valid.\r\n",
					       buf2,
					       file_name);
					main_log("ERROR: geen idee (928736)");
					exit(1);
				}
			}
		}

		strcpy(buf1, db7350_read_from_file(race_file));
		strcpy(buf2, db5250_parse_str("PHYSICAL_SIZE=", buf1, file_name, 79));
		if (strcmp(buf2, "SMALL") == 0) {
			SET_BIT(races[idx].flag, RFLAG_SIZE_SMALL);
		}
		else {
			if (strcmp(buf2, "LARGE") == 0) {
				SET_BIT(races[idx].flag, RFLAG_SIZE_LARGE);
			}
			else {
				if (strcmp(buf2, "NORMAL") == 0) {
					SET_BIT(races[idx].flag, RFLAG_SIZE_NORMAL);
				}
				else {
					printf("ERROR: PHYSICAL=%s in file %s is not valid.\r\n",
					       buf2,
					       file_name);

					main_log("ERROR: geen idee (3284762)");
					exit(1);
				}
			}
		}

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_str = db5275_part_num("MAX_STR=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_int = db5275_part_num("MAX_INT=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_wis = db5275_part_num("MAX_WIS=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_dex = db5275_part_num("MAX_DEX=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_con = db5275_part_num("MAX_CON=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_cha = db5275_part_num("MAX_CHA=", buf1, file_name, 1, 30);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].base_hit = db5275_part_num("BASE_HIT=", buf1, file_name, 1, 250);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].base_mana = db5275_part_num("BASE_MANA=", buf1, file_name, 1, 250);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].base_move = db5275_part_num("BASE_MOVE=", buf1, file_name, 1, 250);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_hit = db5275_part_num("MAX_HIT=", buf1, file_name, 1, 1000000);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_mana = db5275_part_num("MAX_MANA=", buf1, file_name, 0, 1000000);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].max_move = db5275_part_num("MAX_MOVE=", buf1, file_name, 1, 1000000);

		strcpy(buf1, db7350_read_from_file(race_file));

		races[idx].max_food = db5275_part_num("MAX_FOOD=", buf1, file_name, 0, 250);
		strcpy(buf1, db7350_read_from_file(race_file));

		races[idx].max_thirst = db5275_part_num("MAX_THIRST=", buf1, file_name, 0, 250);
		strcpy(buf1, db7350_read_from_file(race_file));

		races[idx].max_drunk = db5275_part_num("MAX_DRUNK=", buf1, file_name, 0, 250);

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_str = db5275_part_num("ADJ_STR=", buf1, file_name, -25, 25);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_int = db5275_part_num("ADJ_INT=", buf1, file_name, -25, 25);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_wis = db5275_part_num("ADJ_WIS=", buf1, file_name, -25, 25);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_dex = db5275_part_num("ADJ_DEX=", buf1, file_name, -25, 25);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_con = db5275_part_num("ADJ_CON=", buf1, file_name, -25, 25);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_cha = db5275_part_num("ADJ_CHA=", buf1, file_name, -25, 25);

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_hit = db5275_part_num("ADJ_HIT=", buf1, file_name, -999, 999);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_mana = db5275_part_num("ADJ_MANA=", buf1, file_name, -999, 999);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_move = db5275_part_num("ADJ_MOVE=", buf1, file_name, -999, 999);

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_hitroll = db5275_part_num("ADJ_HITROLL=", buf1, file_name, -50, 50);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_dmgroll = db5275_part_num("ADJ_DMGROLL=", buf1, file_name, -50, 50);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_ac = db5275_part_num("ADJ_AC=", buf1, file_name, -400, 400);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_food = db5275_part_num("ADJ_FOOD=", buf1, file_name, 0, 500);

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_thirst = db5275_part_num("ADJ_THIRST=", buf1, file_name, 0, 500);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].adj_drunk = db5275_part_num("ADJ_DRUNK=", buf1, file_name, 0, 500);

		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].regen_hit = db5275_part_num("REGEN_HIT=", buf1, file_name, 0, 500);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].regen_mana = db5275_part_num("REGEN_MANA=", buf1, file_name, 0, 500);
		strcpy(buf1, db7350_read_from_file(race_file));
		races[idx].regen_move = db5275_part_num("REGEN_MOVE=", buf1, file_name, 0, 500);

		for (jdx = 1; jdx < MAX_RACE_ATTRIBUTES; jdx++) {
			strcpy(buf1, db7350_read_from_file(race_file));
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "RACE_PERM_SPELL%d=", jdx);
			bzero(buf3, sizeof(buf3));
			/* STRIP OUT SKILL */
			strcpy(buf3, db5250_parse_str(buf2, buf1, file_name, 50));

			/* FIND NUMERIC VLAUE OF SKILL */
			races[idx].perm_spell[jdx] =
				db5280_skill_number(buf3, file_name);


		}		/* END OF for jdx loop */

		/* LOAD SKILLS */
		bzero(buf1, sizeof(buf1));
		strcpy(buf1, db7350_read_from_file(race_file));
		while (*buf1) {
			/* We have a record, Make sure its: */
			/* SKILL_MIN_LEVEL <skill> <base>   */
			/* SKILL_MAX  <skill> <max>    */
			if (!strncmp("SKILL_MIN_LEVEL=", buf1, 11)) {
				/* STRIP OUT SKILL */
				bzero(buf3, sizeof(buf3));
				strcpy(buf3, db5250_parse_str("SKILL_MIN_LEVEL=", buf1, file_name, 50));
				/* FIND NUMERIC VLAUE OF SKILL */
				lv_value = db5280_skill_number(buf3, file_name);
				races[idx].skill_min_level[lv_value] =
					db5290_skill_percentage(buf3, file_name);
			}
			else if (!strncmp("SKILL_MAX=", buf1, 10)) {
				bzero(buf3, sizeof(buf3));
				strcpy(buf3, db5250_parse_str("SKILL_MAX=", buf1, file_name, 50));
				/* FIND NUMERIC VALUE OF SKILL */
				lv_value = db5280_skill_number(buf3, file_name);
				races[idx].skill_max[lv_value] =
					db5290_skill_percentage(buf3, file_name);
			}
			else {
				printf("ERROR: Invalid skill %s in file %s.\r\n",
				       buf1, file_name);
				main_log("ERROR: geen idee (2365326)");
				exit(1);
			}

			bzero(buf1, sizeof(buf1));
			strcpy(buf1, db7350_read_from_file(race_file));
		}

		fclose(race_file);

	}			/* END OF idx loop */

	return;

}				/* END OF db5200_load_races() */


void db5300_load_classes(void)
{

	FILE *class_file;
	int lv_value, idx, jdx;
	char file_name[80], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH],
	  buf3[MAX_STRING_LENGTH];



	/* ZERO STRUCTURE */
	for (idx = 0; idx < MAX_CLASSES; idx++) {
		bzero((char *) &classes[idx], sizeof(struct class_data));
		strcpy(classes[idx].name, "undefined");
		/* INIT MAX SKILL LEVELS */
		for (jdx = 0; jdx < MAX_SKILLS; jdx++) {
			classes[idx].skill_max[jdx] = 99;
			classes[idx].skill_min_level[jdx] = IMO_LEV;
		}
	}

	/* OPEN AND READ FILE */
	for (idx = 0; idx < MAX_CLASSES; idx++) {

		/* TRY TO OPEN THE FILE */
		bzero(file_name, sizeof(file_name));
		sprintf(file_name, "classes/%d", idx);
		class_file = fopen(file_name, "rb");

		if (!class_file) {
			bzero(buf2, sizeof(buf2));
			sprintf(buf2, "ERROR: Unable to open file: %s errno: %d",
				file_name, errno);
			main_log(buf2);
			spec_log(buf2, ERROR_LOG);
			exit(1);
		}

		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].name, db5250_parse_str("NAME=", buf1, file_name, 19));
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].desc, db5250_parse_str("DESC=", buf1, file_name, 5));
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].text[0], db5250_parse_str("TEXT1=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].text[1], db5250_parse_str("TEXT2=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].text[2], db5250_parse_str("TEXT3=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(classes[idx].text[3], db5250_parse_str("TEXT4=", buf1, file_name, 79));

		strcpy(buf1, db7350_read_from_file(class_file));
		classes[idx].adj_hit = atoi(db5250_parse_str("ADJHIT=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(class_file));
		classes[idx].adj_mana = atoi(db5250_parse_str("ADJMANA=", buf1, file_name, 79));
		strcpy(buf1, db7350_read_from_file(class_file));
		classes[idx].adj_move = atoi(db5250_parse_str("ADJMOVE=", buf1, file_name, 79));

		strcpy(buf1, db7350_read_from_file(class_file));
		classes[idx].thaco_numerator =
			db5275_part_num("THACO ", buf1, file_name, 0, 255);
		strcpy(buf2, db5250_parse_str("THACO ", buf1, file_name, 79));
		classes[idx].thaco_denominator =
			db5290_skill_percentage(buf2, file_name);
		if (classes[idx].thaco_denominator < 1)
			classes[idx].thaco_denominator = 1;

		/* ALIGNMENT */
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(buf2, db5250_parse_str("ALIGNMENT=", buf1, file_name, 79));
		if (strcmp(buf2, "EVIL") == 0) {
			SET_BIT(classes[idx].flag, CFLAG_EVIL_ALIGNMENT);
		}
		else {
			if (strcmp(buf2, "GOOD") == 0) {
				SET_BIT(classes[idx].flag, CFLAG_GOOD_ALIGNMENT);
			}
			else {
				if (strcmp(buf2, "NORMAL") != 0) {
					printf("ERROR: ALIGNMENT=%s in file %s is not valid.\r\n",
					       buf2,
					       file_name);
					main_log("ERROR: alignment (2348971)");
					exit(1);
				}
			}
		}

		/* CLASS TYPE */
		strcpy(buf1, db7350_read_from_file(class_file));
		strcpy(buf2, db5250_parse_str("CLASS=", buf1, file_name, 79));
		if (strcmp(buf2, "MAGE") == 0) {
			SET_BIT(classes[idx].flag, CFLAG_CLASS_MAGE);
		}
		else {
			if (strcmp(buf2, "CLERIC") == 0) {
				SET_BIT(classes[idx].flag, CFLAG_CLASS_CLERIC);
			}
			else {
				if (strcmp(buf2, "FIGHTER") == 0) {
					SET_BIT(classes[idx].flag, CFLAG_CLASS_FIGHTER);
				}
				else if (strcmp(buf2, "THIEF") == 0) {
					SET_BIT(classes[idx].flag, CFLAG_CLASS_THIEF);
				}
				else {
					printf("ERROR: CLASS=%s in file %s is not valid.\r\n",
					       buf2,
					       file_name);
					main_log("ERROR:CLASS(23851)");
					exit(1);
				}
			}
		}

		/* LOAD SKILLS */
		bzero(buf1, sizeof(buf1));
		strcpy(buf1, db7350_read_from_file(class_file));
		while (*buf1) {
			/* We have a record, Make sure its: */
			/* SKILL_MIN_LEVEL <level>     */
			/* SKILL_MAX  <skill> <max>    */
			if (!strncmp("SKILL_MIN_LEVEL=", buf1, 11)) {
				/* STRIP OUT SKILL */
				bzero(buf3, sizeof(buf3));
				strcpy(buf3, db5250_parse_str("SKILL_MIN_LEVEL=", buf1, file_name, 50));
				/* FIND NUMERIC VLAUE OF SKILL */
				lv_value = db5280_skill_number(buf3, file_name);
				classes[idx].skill_min_level[lv_value] =
					db5290_skill_percentage(buf3, file_name);
			}
			else if (!strncmp("SKILL_MAX=", buf1, 10)) {
				bzero(buf3, sizeof(buf3));
				strcpy(buf3, db5250_parse_str("SKILL_MAX=", buf1, file_name, 50));
				/* FIND NUMERIC VLAUE OF SKILL */
				lv_value = db5280_skill_number(buf3, file_name);
				classes[idx].skill_max[lv_value] =
					db5290_skill_percentage(buf3, file_name);
			}
			else {
				printf("ERROR: Invalid skill %s in file %s.\r\n",
				       buf1, file_name);
				main_log("ERROR: skill(2358971)");
				exit(1);
			}

			bzero(buf1, sizeof(buf1));
			strcpy(buf1, db7350_read_from_file(class_file));
		}

		fclose(class_file);

	}			/* END OF idx loop */

	return;

}				/* END OF db5300_load_classes() */


#define ZO_DEAD  999
 /* update zone ages, queue for reset if necessary, and dequeue when possible */
void db5500_zone_update(void)
{
	int i;
	struct reset_q_element *update_u, *temp;



	/* enqueue zones */
	for (i = 0; i <= top_of_zone_table; i++) {
		if (zone_table[i].age < zone_table[i].lifespan &&
		    zone_table[i].reset_mode)
			(zone_table[i].age)++;
		else if (zone_table[i].age < ZO_DEAD &&
			 zone_table[i].reset_mode) {
			/* enqueue zone */

			CREATE(update_u, struct reset_q_element, 1);

			update_u->zone_to_reset = i;
			update_u->next = 0;

			if (!reset_q.head)
				reset_q.head = reset_q.tail = update_u;
			else {
				reset_q.tail->next = update_u;
				reset_q.tail = update_u;
			}

			zone_table[i].age = ZO_DEAD;
		}
	}

	/* dequeue zones (if possible) and reset */
	for (i = 0; i < 10; i++) {	/* Do 5 zone resets/minute (assuming
					 * there are 5 to be done) */
		for (update_u = reset_q.head; update_u; update_u = update_u->next)
			if (zone_table[update_u->zone_to_reset].reset_mode == 2 ||
			    db5900_is_zone_empty(update_u->zone_to_reset)) {

				if (!IS_SET(gv_run_flag, RUN_FLAG_NOZONE_RESET)) {
					db5600_reset_zone(update_u->zone_to_reset);
				}

				/* dequeue */

				if (update_u == reset_q.head)
					reset_q.head = reset_q.head->next;
				else {
					for (temp = reset_q.head; temp->next != update_u; temp = temp->next);

					if (!update_u->next)
						reset_q.tail = temp;

					temp->next = update_u->next;
				}

				free(update_u);
				break;
			}
	}
}				/* END OF db5500_zone_update() */



 /* execute the reset command table of a given zone */
 /* db5601 */
void db5600_reset_zone(int zone)
{
	int cmd_no, rc, last_cmd_valid, last_mob_valid, lv_arg1, lv_arg2,
	  lv_arg3, lv_if_flag, lv_do_next_command;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], lv_command;
	struct char_data *mob;



	mob = 0;
	last_cmd_valid = 1;
	last_mob_valid = 1;

	/* LEAVE CODE IN CASE WE NEED TO QUICKLY SUPRESS A ZONE */
	/* THERE ARE UNDER A 100 ZONES SO WE SHOULDN'T HIT THIS */
	if (zone == 9999) {
		sprintf(buf2, "\nLoop: %d Not resetting %s", zone, zone_table[zone].filename);
		main_log(buf2);
		spec_log(buf2, ERROR_LOG);
		return;
	}
	else if (gv_verbose_messages == TRUE) {
		sprintf(buf2, "\nLoop: %d Resetting %s", zone, zone_table[zone].filename);
		main_log(buf2);
		spec_log(buf2, ERROR_LOG);
	}

	//fprintf(stderr, "Resetting zone #%d (%s)\r\n", zone, zone_table[zone].filename);

	if (IS_SET(zone_table[zone].dirty_zon, SHOWN))
		return;

	for (cmd_no = 0;; cmd_no++) {
		lv_if_flag = zone_table[zone].cmd[cmd_no].if_flag;
		lv_arg1 = zone_table[zone].cmd[cmd_no].arg1;
		lv_arg2 = zone_table[zone].cmd[cmd_no].arg2;
		lv_arg3 = zone_table[zone].cmd[cmd_no].arg3;
		lv_command = zone_table[zone].cmd[cmd_no].command;

		if (gv_mega_verbose_messages == TRUE) {
			printf("  %c %d %d %d.\r\n",
			       lv_command,
			       lv_arg1,
			       lv_arg2,
			       lv_arg3);
		}

		if (lv_command == 'S')
			break;

		lv_do_next_command = TRUE;
		/* IF its a 1, it means confirm last mob loaded */
		if (lv_if_flag == 1)
			if (last_mob_valid == 0)
				lv_do_next_command = FALSE;

		/* IF its a 2, it means confirm last cmd was valid */
		if (lv_if_flag == 2)
			if (last_cmd_valid == 0)
				lv_do_next_command = FALSE;

		if (lv_do_next_command == TRUE)
			switch (lv_command) {
			case 'm':	/* disabled command */
			case 'o':	/* disabled command */
			case 'p':	/* disabled command */
			case 'g':	/* disabled command */
			case 'e':	/* disabled command */
			case 'd':	/* disabled command */
				break;

			case 'M':	/* read a mobile */
				rc = db5700_zone_reset_mob(&mob, zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);
				last_mob_valid = 0;
				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'm';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
					last_mob_valid = 1;
				}
				break;

			case 'O':	/* read an object */
				rc = db5710_zone_reset_object(zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);

				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'o';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
				}
				break;

			case 'P':	/* object to object */
				rc = db5740_zone_reset_put(zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);
				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'p';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
				}
				break;


			case 'G':	/* ha1700_obj_to_char */
				rc = db5720_zone_reset_give(mob, zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);
				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'g';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
				}
				break;

			case 'E':	/* object to equipment list */
				rc = db5730_zone_reset_equip(&mob, zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);
				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'e';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
				}
				break;

			case 'R':	/* read an object */
				rc = db5760_zone_reset_remove(zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);

				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'r';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE LOADED ONE OR MORE
						 * OBJECTS */
					last_cmd_valid = 1;
				}
				break;

			case 'D':	/* set state of door */
				rc = db5750_zone_reset_doors(zone, cmd_no, lv_command, lv_if_flag, lv_arg1, lv_arg2, lv_arg3);
				last_cmd_valid = 0;
				if (rc < 0) {	/* WE GOT AN ERROR */
					lv_command = 'd';	/* lower case command to
								 * disable it */
				}
				if (rc > 0) {	/* WE RESET THE DOOR */
					last_cmd_valid = 1;
				}
				break;

			default:{
					bzero(buf, sizeof(buf));
					sprintf(buf, "Undefd cmd in reset table; zone %s cmd %d.\n\r",
					   zone_table[zone].filename, cmd_no);
					perror(buf);
					ABORT_PROGRAM();
					break;
				}
			}
		else {
			last_cmd_valid = 0;
		}
	}
	zone_table[zone].age = 0;

/* ************** */
/* SPECIAL RESETS */
/* ************** */

	/* IF THIS IS THE abyss zone, we need to remove links */
	if (!strcmp(zone_table[zone].filename, "TheAbyss")) {
		wi2925_remove_room_links(2472, CMD_UP - 1);
	}			/* END OF theabyss */

	/* IF THIS IS THE CloudPalace zone, we need to remove links */
	if (!strcmp(zone_table[zone].filename, "CloudPalace")) {
		wi2925_remove_room_links(2106, CMD_UP - 1);
		wi2925_remove_room_links(2107, CMD_DOWN - 1);
		wi2950_remove_virtual_obj_from_room(2141, 2106);
		wi2925_remove_room_links(2138, CMD_NORTH - 1);
		wi2925_remove_room_links(2188, CMD_SOUTH - 1);
		wi2950_remove_virtual_obj_from_room(2142, 2138);
	}			/* END OF CloudPalace zone */

}				/* END OF db5600_reset_zone() */

/* lv_flag:   BIT0 - DON'T GO FATAL */
/*                                  */
void db5625_print_error(char *lv_location, int lv_line, char lv_command, int lv_if_flag, int lv_arg1, int lv_arg2, int lv_arg3, char *lv_filename, int lv_flag)
{

	char buf[MAX_STRING_LENGTH];
	int lv_virtual;



	sprintf(buf, "ERROR: in db5600_reset_zone.  LOC=%s FILE=%s.\r\n",
		lv_location, lv_filename);
	main_log(buf);
	spec_log(buf, ERROR_LOG);
	switch (lv_command) {
	case 'D':
	case 'd':{
			if (lv_arg1 < 0 || lv_arg1 > top_of_world)
				lv_virtual = 0;
			else
				lv_virtual = world[lv_arg1].number;
			break;
		}
	case 'M':
	case 'm':{
			if (lv_arg1 < 0 || lv_arg1 > top_of_mobt)
				lv_virtual = 0;
			else
				lv_virtual = mob_index[lv_arg1].virtual;
			break;
		}
	case 'E':
	case 'e':
	case 'G':
	case 'g':
	case 'O':
	case 'o':
	case 'P':
	case 'p':{
			if (lv_arg1 < 0 || lv_arg1 > top_of_mobt)
				lv_virtual = 0;
			else
				lv_virtual = obj_index[lv_arg1].virtual;
			break;
		}
	default:{
			lv_virtual = 0;
		}
	}

	sprintf(buf, "line %d + 5: %c %d %d %d %d  arg1=%d.\r\n",
		lv_line,
		lv_command,
		lv_if_flag,
		lv_virtual,
		lv_arg2,
		lv_arg3,
		lv_arg1);
	main_log(buf);

	if (!IS_SET(lv_flag, BIT0)) {
		strcpy(buf, "Aborting program because of zone file problem.\r\n");
		ABORT_PROGRAM();
	}
	return;

}				/* END OF db5625_print_error() */

int db5700_zone_reset_mob(struct char_data ** mob_ptr, int zone, int cmd_no, int lv_command, int lv_if_flag, int mob_num, int mob_qty, int the_room)
{
	int lv_success, lv_temp_num;
	struct char_data *lv_mob;



	lv_mob = *mob_ptr;
	lv_success = 0;

	if (mob_num < 0 || mob_num > top_of_mobt) {
		db5625_print_error("LOC001 unknown mob#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   mob_num, mob_qty, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (the_room < 0 || the_room > top_of_world) {
		db5625_print_error("LOC002 unknown room#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   mob_num, mob_qty, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF room number invalid */


	/* MAKE SURE WE DON'T LOAD ENFYRN PRIEST WHEN DRAGON IS ON */
	lv_temp_num = db8100_real_mobile(MOB_ENFYRN_DRAGON);
	if (lv_temp_num < 0) {	/* SHOULD NEVER HAPPEN        */
		lv_temp_num = 0;/* PATCH SO WE DON'T CRASH */
	}

	/* DON'T LOAD ENFYRN PRIEST IF DRAGON EXIST */
	if (mob_index[mob_num].virtual == MOB_ENFYRN_PRIEST &&
	    mob_index[lv_temp_num].number > 0) {
		return (0);
	}

	lv_success = 0;

	/* IF NUMBER OF MOBS IN THE WORLD IS LESS, THEN LOAD MOB */
	if (mob_index[mob_num].number < mob_qty) {
		lv_mob = db5000_read_mobile(mob_num, REAL);
		GET_START(lv_mob) = world[the_room].number;
		ha1600_char_to_room(lv_mob, the_room);
		if (mob_index[mob_num].virtual == MOB_JINIPAN)
			killer_mob = lv_mob;
		if (mob_index[mob_num].virtual == MOB_QUESTMAN)
			questman = lv_mob;
		if (mob_index[mob_num].virtual == MOB_BOUNTYMASTER)
			bounty_master = lv_mob;
		if (mob_index[mob_num].virtual == MOB_BOUNTYHUNTER)
			bounty_hunter = lv_mob;
		lv_success = 1;
	}

	*mob_ptr = lv_mob;
	return (lv_success);

}				/* END OF db5700_zone_reset_mob() */



int db5710_zone_reset_object(int zone, int cmd_no, int lv_command,
			       int lv_if_flag, int obj_num, int obj_qty,
			       int the_room)
{

	int lv_success, lv_obj_count;
	struct obj_data *obj;



	lv_success = 0;

	if (obj_num < 0 || obj_num > top_of_objt) {
		db5625_print_error("LOC004 unknown obj#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (the_room < 0 || the_room > top_of_world) {
		db5625_print_error("LOC005 unknown room#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF room number invalid */

	/* HOW MANY ARE IN THE ROOM */
	lv_obj_count = ha2050_count_objs_in_list(obj_num,
						 world[the_room].contents);
	while (lv_obj_count < obj_qty &&
	       db8800_ok_to_load_obj(0, obj_num, 0)) {
		lv_obj_count++;
		/* WE FELL HERE SO COMMAND IS VALID */
		lv_success = 1;

		obj = db5100_read_object(obj_num, REAL);
		ha2100_obj_to_room(obj, the_room);

		/* safety check */
		if (!(obj->carried_by) &&
		    !(obj->in_obj) &&
		    obj->in_room == NOWHERE) {
			ha2700_extract_obj(obj);
		}
	}			/* END OF while */

	return (lv_success);

}				/* END OF db5710_zone_reset_object() */


int db5720_zone_reset_give(struct char_data * mob, int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int the_room)
{

	int lv_success, lv_obj_count;
	struct obj_data *obj;

	lv_success = 0;

	if (obj_num < 0 || obj_num > top_of_objt) {
		db5625_print_error("LOC011 unknown obj#", cmd_no, lv_command, lv_if_flag,
				   obj_num, obj_qty, the_room, zone_table[zone].filename, 0);
		return (-1);
	}/* END OF INVALID NUMBER */

	if (!mob) {
		db5625_print_error("LOC012 no mob def'd", cmd_no, lv_command, lv_if_flag, obj_num, 
		            obj_qty, the_room, zone_table[zone].filename, 0);
		return (-1);
	}/* END OF INVALID NUMBER */

	/* HOW MANY ARE ON THE MOB */
	lv_obj_count = ha2050_count_objs_in_list(obj_num, mob->carrying);
	while (lv_obj_count < obj_qty && db8800_ok_to_load_obj(0, obj_num, 0)) {
		lv_obj_count++;
		/* WE FELL HERE SO COMMAND IS VALID */
		lv_success = 1;

		obj = db5100_read_object(obj_num, REAL);
		ha1700_obj_to_char(obj, mob);

		/* safety check */
		if (!(obj->carried_by) && !(obj->in_obj) && (obj->in_room == NOWHERE)) {
			ha2700_extract_obj(obj);
		}
	}/* END OF while */

	return (lv_success);

}/* END OF db5720_zone_reset_give() */


int db5730_zone_reset_equip(struct char_data ** mob_ptr, int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int wear_loc)
{
	int lv_success;
	struct char_data *lv_mob;
	struct obj_data *obj;



	lv_mob = *mob_ptr;
	lv_success = 0;

	if (obj_num < 0 || obj_num > top_of_objt) {
		db5625_print_error("LOC014 unknown obj#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, wear_loc,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (!lv_mob) {
		db5625_print_error("LOC015 no mob def'd ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, wear_loc,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (wear_loc < 0 || wear_loc > MAX_WEAR) {
		db5625_print_error("LOC016 unknown wear loc ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, wear_loc,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF wear loc invalid */

	if (lv_mob->equipment[wear_loc]) {
		db5625_print_error("LOC017 Location already equipped. ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, wear_loc,
				   zone_table[zone].filename,
				   BIT0);
		return (-1);
	}			/* END OF already equipped */

	if (db8800_ok_to_load_obj(0, obj_num, 0)) {
		obj = db5100_read_object(obj_num, REAL);
		ha1925_equip_char(lv_mob, obj, wear_loc, 0, BIT1);
		/* safety check */
		if (!(obj->carried_by) &&
		    !(obj->in_obj) &&
		    obj->in_room == NOWHERE) {
			ha2700_extract_obj(obj);
		}
	}

	return (lv_success);

}				/* END OF db5730_zone_reset_equip() */


int db5740_zone_reset_put(int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int obj_container)
{

	int lv_success, lv_obj_count;
	struct obj_data *obj, *obj_to;



	lv_success = 0;

	if (obj_num < 0 || obj_num > top_of_objt) {
		db5625_print_error("LOC007 unknown obj# ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, obj_container,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (obj_container < 0 || obj_container > top_of_objt) {
		db5625_print_error("LOC008 unknown container obj# ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   obj_num, obj_qty, obj_container,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	/* FIND DESTINATION OBJECT */
	obj_to = ha2090_get_obj_using_num(obj_container);
	if (!obj_to) {
		return (0);
	}

	/* HOW MANY ARE ON THE MOB */
	lv_obj_count = ha2050_count_objs_in_list(obj_num,
						 obj_to->contains);
	while (lv_obj_count < obj_qty &&
	       db8800_ok_to_load_obj(0, obj_num, 0)) {
		lv_obj_count++;
		/* WE FELL HERE SO COMMAND IS VALID */
		lv_success = 1;

		obj = db5100_read_object(obj_num, REAL);
		ha2300_obj_to_obj(obj, obj_to);

		/* safety check */
		if (!(obj->carried_by) &&
		    !(obj->in_obj) &&
		    obj->in_room == NOWHERE) {
			ha2700_extract_obj(obj);
		}
	}			/* END OF while */

	return (lv_success);

}				/* END OF db5740_zone_reset_put() */


int db5750_zone_reset_doors(int zone, int cmd_no, int lv_command, int lv_if_flag, int the_room, int the_dir, int lock_type)
{

	int lv_success;



	lv_success = 0;

	if (the_room < 0 || the_room > top_of_world) {
		db5625_print_error("LOC020 unknown room# ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   the_room, the_dir, lock_type,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF room number invalid */

	if (the_dir < 0 || the_dir >= MAX_DIRS) {
		db5625_print_error("LOC021 unknown direction ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   the_room, the_dir, lock_type,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */


	/* IS DIRECTION DEFINED? */
	if (!world[the_room].dir_option[the_dir]) {
		db5625_print_error("LOC022 direction not allocated ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   the_room, the_dir, lock_type,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	switch (lock_type) {
	case 0:
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_ISDOOR);
		REMOVE_BIT(world[the_room].dir_option[the_dir]->exit_info,
			   EX_CLOSED);
		REMOVE_BIT(world[the_room].dir_option[the_dir]->exit_info,
			   EX_LOCKED);
		lv_success = 1;
		break;
	case 1:
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_ISDOOR);
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_CLOSED);
		REMOVE_BIT(world[the_room].dir_option[the_dir]->exit_info,
			   EX_LOCKED);
		lv_success = 1;
		break;
	case 2:
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_ISDOOR);
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_CLOSED);
		SET_BIT(world[the_room].dir_option[the_dir]->exit_info,
			EX_LOCKED);
		lv_success = 1;
		break;
	default:
		db5625_print_error("LOC023 Invalid lock type ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   the_room, the_dir, lock_type,
				   zone_table[zone].filename,
				   0);
		break;
	}			/* switch lock type */

	return (lv_success);

}				/* END OF db5750_zone_reset_doors() */


int db5760_zone_reset_remove(int zone, int cmd_no, int lv_command, int lv_if_flag, int item_num, int item_type, int the_room)
{

	int lv_success;
	struct char_data *lv_mob;
	struct obj_data *lv_obj;



	lv_success = 0;

	if (item_type != ZCREATE_OBJECT &&
	    item_type != ZCREATE_MOBILE) {
		db5625_print_error("LOC060 unknown item type",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   item_num, item_type, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if ((item_type == ZCREATE_OBJECT) &&
	    ((item_num < 0) || (item_num > top_of_objt))) {
		db5625_print_error("LOC061 unknown obj# ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   item_num, item_type, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if ((item_type == ZCREATE_MOBILE) &&
	    ((item_num < 0) || (item_num > top_of_mobt))) {
		db5625_print_error("LOC062 unknown mob# ",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   item_num, item_type, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF INVALID NUMBER */

	if (the_room < 0 || the_room > top_of_world) {
		db5625_print_error("LOC005 unknown room#",
				   cmd_no,
				   lv_command,
				   lv_if_flag,
				   item_num, item_type, the_room,
				   zone_table[zone].filename,
				   0);
		return (-1);
	}			/* END OF room number invalid */

	if (item_type == 1) {
		lv_obj = ha2025_get_obj_list_num(item_num,
						 world[the_room].contents);
		if (lv_obj) {
			ha2700_extract_obj(lv_obj);
			lv_success = 1;
		}
	}			/* END of type = object */

	if (item_type == 2) {
		lv_mob = ha2030_get_mob_list_num(item_num,
						 world[the_room].people);
		if (lv_mob) {
			ha3000_extract_char(lv_mob, END_EXTRACT_BY_ZONE_RESET);
			lv_success = 1;
		}
	}			/* END of type = mobile */

	return (lv_success);

}				/* END OF db5760_zone_reset_remove() */


/* Return TRUE if zone 'nr' is free of PC's  */
int db5900_is_zone_empty(int zone_nr)
{
	struct descriptor_data *i;



	for (i = descriptor_list; i; i = i->next)
		if (!i->connected)
			if (world[i->character->in_room].zone == zone_nr)
				return (0);

	return (1);
}				/* END OF db5900_is_zone_empty() */


 /****************************************************************** *
 *  stuff related to the save/load player system                     *
 ******************************************************************* */
 /* Load a char, TRUE if loaded, FALSE if not */
int db6000_load_char(char *name, struct char_file_u * char_element)
{

	FILE *fl;
	int player_i;
	char buf[MAX_STRING_LENGTH];



	player_i = pa2000_find_name(name);
	if (player_i >= 0) {
		if (!(fl = fopen(PLAYER_FILE, "rb"))) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: Opening player file for reading. (db.c, db6000_load_char)");
			perror(buf);
			ABORT_PROGRAM();
		}

		fseek(fl, (long) (player_table[player_i].pidx_nr *
				  sizeof(struct char_file_u)), 0);

		fread(char_element, sizeof(struct char_file_u), 1, fl);
		fclose(fl);
		return (player_i);
	}
	else
		return (-1);
}				/* END OF db6000_load_char() */


/* copy data from the file structure to a char struct */
void db6100_store_to_char(struct char_file_u * st, struct char_data * ch)
{
	int i, jdx, rc;

	/* SET nr so we can quickly find ourselves in the player_table */
	ch->nr = pa2000_find_name(st->name);
	if (ch->nr < 0) {
		ch->nr = 0;
		printf("db6100 ERROR - Unable to find %s's position\r\n",
		       st->name);
	}

	GET_SEX(ch) = st->sex;
	GET_CLASS(ch) = st->class;
	GET_RACE(ch) = st->race;
	GET_LEVEL(ch) = st->level;
	switch (GET_LEVEL(ch)) {
		case 49: GET_LEVEL(ch) = IMO_IMM; break;
		case 51: GET_LEVEL(ch) = IMO_IMP; break;
	}

	ch->player.short_descr = 0;
	ch->player.long_descr = 0;

	if (*st->title) {
		CREATE(ch->player.title, char, strlen(st->title) + 1);
		strcpy(ch->player.title, st->title);
	}
	else
		GET_TITLE(ch) = 0;

	if (*st->description) {
		CREATE(ch->player.description, char,
		       strlen(st->description) + 1);
		strcpy(ch->player.description, st->description);
	}
	else
		ch->player.description = 0;

	if (*st->immortal_enter) {
		CREATE(ch->player.immortal_enter, char, strlen(st->immortal_enter) + 1);
		strcpy(ch->player.immortal_enter, st->immortal_enter);
	}
	else
		ch->player.immortal_enter = 0;
	if (*st->immortal_exit) {
		CREATE(ch->player.immortal_exit, char, strlen(st->immortal_exit) + 1);
		strcpy(ch->player.immortal_exit, st->immortal_exit);
	}
	else
		ch->player.immortal_exit = 0;

	/* ZERO OUT ALIAS AREA */
	if (GET_ALIASES(ch))
		clear_aliases(ch);
	GET_ALIASES(ch) = 0;

	/* MAKE SURE ROOM EXIST */
	GET_START(ch) = 0;
	if (GET_LEVEL(ch) > PK_LEV) {
		for (jdx = 0; jdx <= top_of_world; jdx++) {
			if (world[jdx].number == st->start_room)
				GET_START(ch) = st->start_room;
		}
	}

	GET_PRIVATE(ch) = st->private_room;
	GET_VISIBLE(ch) = st->visible_level;
	GET_INCOGNITO(ch) = st->incognito_lvl;
	//Incognito & ghost Shalira 22.07 .01
		GET_GHOST(ch) = st->ghost_lvl;
	GET_BLESS(ch) = 0;
	//are we blessing ? Shalira 24.07 .01

		ch->player.time.birth = st->birth;
	ch->player.time.played = st->played;
	ch->player.time.lastpkill = st->lastpkill;
	ch->player.time.logon = time(0);


	if (ch->desc) {
		ch->desc->mailedit = 0;
		ch->desc->mailto = -1;
	}

	ch->questmob = 0;
	ch->questobj = 0;
	ch->reward = 0;
	ch->countdown = 0;
	ch->nextquest = st->nextquest;
	ch->nextcast = st->nextcast;
	ch->questpoints = st->questpoints;

	ch->screen_lines = st->screen_lines;

	ch->player.weight = st->weight;
	ch->player.height = st->height;

	ch->abilities = st->abilities;
	ch->tmpabilities = st->abilities;
	ch->bonus.str = 0;
	ch->bonus.dex = 0;
	ch->bonus.cha = 0;
	ch->points = st->points;

	for (i = 0; i < MAX_SKILLS; i++) {
		if (ch->skills[i].learned > 99)
			ch->skills[i].learned = 99;
		else
			ch->skills[i] = st->skills[i];
	}

	ch->specials.spells_to_learn = st->spells_to_learn;
	ch->specials.alignment = st->alignment;

	/* USER FLAGS */

	GET_ACT1(ch) = st->act;
	GET_ACT2(ch) = st->act2;
	GET_ACT3(ch) = st->act3;
	GET_ACT4(ch) = st->act4;
	GET_ACT5(ch) = st->act5;
	GET_ACT6(ch) = st->act6;
	GET_WIZ_PERM(ch) = st->wiz_perms;

	GET_JAILTIME(ch) = st->jailtime;
	GET_DSTIME(ch) = st->deathshadowtime;
	GET_GMTIME(ch) = st->godmuzzletime;
	CLAN_NUMBER(ch) = st->clan.number;
	CLAN_RANK(ch) = st->clan.rank;
	CLAN_FLAGS1(ch) = st->clan.flags1;
	GET_USEDEXP(ch) = st->points.used_exp;
	ch->points.fspells = st->points.fspells;
	ch->points.class1 = st->points.class1;
	ch->points.class2 = st->points.class2;
	ch->points.class3 = st->points.class3;
	ch->points.prompt = MAXV(1, st->points.prompt);
	
    ch->points.extra_hits = 0; //st->points.extra_hits;

	ch->jailed = st->jailed;

	/* REMOVE BITS MOBS SHOULDN'T HAVE */
	REMOVE_BIT(GET_ACT1(ch), PLR1_ISNPC);
	REMOVE_BIT(GET_ACT2(ch), PLR2_BACKSTAB);

	/* MORTALS CAN'T SEE ROOM NUMBERS */
	if (GET_LEVEL(ch) < IMO_LEV)
		REMOVE_BIT(GET_ACT3(ch), PLR3_SHOW_ROOM);

	if (GET_LEVEL(ch) < IMO_SPIRIT)
		REMOVE_BIT(GET_ACT3(ch), PLR3_NOHASSLE);
	else
		SET_BIT(GET_ACT3(ch), PLR3_NOHASSLE);

	/* DEFAULT TO ON FOR SHOUT */
	REMOVE_BIT(GET_ACT3(ch), PLR3_NOSHOUT);
	/* PLAYER ISN'T AFK OR AFW ANYMORE */
	REMOVE_BIT(GET_ACT2(ch), PLR2_AFK);
	REMOVE_BIT(GET_ACT2(ch), PLR2_AFW);

	/* DEFAULT TO CRASH, WE'LL CHANGE WHEN PLAYER LEAVES GAME */
	GET_HOW_LEFT(ch) = st->how_player_left_game;
	GET_LOGOFF_TIME(ch) = st->logoff_time;

	ch->specials.carry_weight = 0;
	ch->specials.carry_items = 0;

	ch->specials.time_of_muzzle = st->time_of_muzzle;
	ch->points.armor = MAX_ARMOUR;
	ch->points.hitroll = races[GET_RACE(ch)].adj_hitroll;
	ch->points.damroll = races[GET_RACE(ch)].adj_dmgroll;
	ch->questdata = 0;

	CREATE(GET_NAME(ch), char, strlen(st->name) + 1);
	strcpy(GET_NAME(ch), st->name);

	/* LOAD ALIASES */
	db9800_load_save_aliases(ch, LOAD_ALIAS);

	for (i = 0; i < MAX_SAVES; i++) {
		ch->specials.apply_saving_throw[i] = st->apply_saving_throw[i];
	}
	ha9700_set_saving_throws(ch, 0);

	for (i = 0; i < 3; i++) {
	}

	if (GET_LEVEL(ch) < 6 ||
	    GET_LEVEL(ch) >= IMO_LEV) {
		GET_COND(ch, FOOD) = -1;
		GET_COND(ch, THIRST) = -1;
		GET_COND(ch, DRUNK) = -1;
	}
	else {
		if (st->conditions[FOOD] < 1)
			GET_COND(ch, FOOD) = races[GET_RACE(ch)].max_food;
		else
			GET_COND(ch, FOOD) = st->conditions[FOOD];

		if (st->conditions[THIRST] < 1)
			GET_COND(ch, THIRST) = races[GET_RACE(ch)].max_thirst;
		else
			GET_COND(ch, THIRST) = st->conditions[THIRST];

		if (st->conditions[DRUNK] < 1)
			GET_COND(ch, DRUNK) = 0;
		else
			GET_COND(ch, DRUNK) = st->conditions[DRUNK];

	}

	/* IF THEY DON'T EAT/DRINK/DRUNK SET TO -1 */
	if (races[GET_RACE(ch)].adj_food == 0)
		GET_COND(ch, FOOD) = -1;
	if (races[GET_RACE(ch)].adj_thirst == 0)
		GET_COND(ch, THIRST) = -1;
	if (races[GET_RACE(ch)].adj_drunk == 0)
		GET_COND(ch, DRUNK) = -1;

	/* Add all spell effects */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			ha1300_affect_to_char(ch, &st->affected[i]);
	}

	/* SET STARTING ROOM */
	if (st->start_room == NOWHERE)
		ch->in_room = NOWHERE;
	else {
		/* DO THEY HAVE ACCESS TO ROOM */
		ch->in_room = db8000_real_room(st->start_room);
		if (ch->in_room < 0) {
			bzero(gv_str, sizeof(gv_str));
			sprintf(gv_str, " ERROR: %s has invalid start room %d.",
				GET_NAME(ch), st->start_room);
			main_log(gv_str);
			spec_log(gv_str, ERROR_LOG);
		}
		else {
			rc = li3000_is_blocked_from_room(ch, ch->in_room, 0);
			if (rc) {
				ch->in_room = NOWHERE;
			}
		}
	}

	ha1225_affect_total(ch);

	//Set foreign spell count(to remove bugged chars)
		ch->points.fspells = ot4700_count_foreign_spells(ch);

}				/* END OF db6100_store_to_char() */
 /* copy vital data from a players char-structure to the file structure */
void db6200_char_to_store(struct char_data * ch, struct char_file_u * st, bool UpdateTime)
{
	char *ptr;
	int i;
	struct obj_data *char_eq[MAX_WEAR];



	/* Unaffect everything a character can be affected by */
	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i])
			char_eq[i] = ha1930_unequip_char(ch, i);
		else
			char_eq[i] = 0;
	}

	for (i = 0; i < MAX_AFFECT; i++) {
		if (ch->affected) {
			st->affected[i].type = ch->affected->type;
			st->affected[i].duration = ch->affected->duration;
			st->affected[i].modifier = ch->affected->modifier;
			st->affected[i].location = ch->affected->location;
			st->affected[i].bitvector = ch->affected->bitvector;
			st->affected[i].bitvector2 = ch->affected->bitvector2;
			st->affected[i].bitvector3 = ch->affected->bitvector3;
			st->affected[i].bitvector4 = ch->affected->bitvector4;
			st->affected[i].next = 0;
			/* subtract effect of the spell or the effect will be
			 * doubled */
			ha1325_affect_remove(ch, ch->affected, 0);
		}
		else {
			st->affected[i].type = 0;	/* Zero signifies not
							 * used */
			st->affected[i].duration = 0;
			st->affected[i].modifier = 0;
			st->affected[i].location = 0;
			st->affected[i].bitvector = 0;
			st->affected[i].bitvector2 = 0;
			st->affected[i].bitvector3 = 0;
			st->affected[i].bitvector4 = 0;
			st->affected[i].next = 0;
		}
	}

	if ((i >= MAX_AFFECT) && ch->affected && ch->affected->next) {
		main_log("WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!");
		spec_log("WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!", ERROR_LOG);
	}

	ch->tmpabilities = ch->abilities;

	st->birth = ch->player.time.birth;
	st->played = ch->player.time.played;
	st->played += (long) (time(0) - ch->player.time.logon);
	st->lastpkill = ch->player.time.lastpkill;
	st->last_logon = time(0);

	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);

	st->weight = GET_WEIGHT(ch);
	st->height = GET_HEIGHT(ch);
	st->sex = GET_SEX(ch);
	st->class = GET_CLASS(ch);
	st->race = GET_RACE(ch);
	st->level = GET_LEVEL(ch);
	st->abilities = ch->abilities;

	st->nextquest = ch->nextquest;
	st->questpoints = ch->questpoints;
	st->nextcast = ch->nextcast;

	st->screen_lines = ch->screen_lines;

	st->points = ch->points;
	ptr = (char *) &st->points.filler;
	bzero(ptr, sizeof(st->points.filler));

	st->alignment = ch->specials.alignment;
	st->spells_to_learn = ch->specials.spells_to_learn;
	st->act = GET_ACT1(ch);
	st->act2 = GET_ACT2(ch);
	st->act3 = GET_ACT3(ch);
	st->act4 = GET_ACT4(ch);
	st->act5 = GET_ACT5(ch);
	st->act6 = GET_ACT6(ch);
	st->wiz_perms = GET_WIZ_PERM(ch);
	st->incognito_lvl = GET_INCOGNITO(ch);
	st->ghost_lvl = GET_GHOST(ch);
	st->jailtime = ch->specials.jailtime;
	st->deathshadowtime = ch->specials.deathshadowtime;
	st->godmuzzletime = ch->specials.godmuzzletime;
	st->clan.number = ch->specials.clan.number;
	st->clan.rank = ch->specials.clan.rank;
	st->clan.flags1 = ch->specials.clan.flags1;
	st->points.used_exp = ch->points.used_exp;
	st->points.fspells = ch->points.fspells;
	st->points.class1 = ch->points.class1;
	st->points.class2 = ch->points.class2;
	st->points.class3 = ch->points.class3;
	st->points.prompt = ch->points.prompt;
	st->points.extra_hits = ch->points.extra_hits;
	st->descriptor_flag1 = GET_DSC1(ch);
	if (strlen(GET_EMAIL(ch)) > 0) {
		strncpy(st->email_name, GET_EMAIL(ch), EMAIL_SIZE);
	}

	st->jailed = ch->jailed;

	st->points.armor = MAX_ARMOUR;
	st->points.hitroll = 0;
	st->points.damroll = 0;

	st->time_of_muzzle = ch->specials.time_of_muzzle;

	strcpy(st->name, GET_NAME(ch));
	if (GET_TITLE(ch))
		strcpy(st->title, GET_TITLE(ch));
	else
		*st->title = '\0';

	if (ch->player.description)
		strcpy(st->description, ch->player.description);
	else
		*st->description = '\0';

	if (ch->player.immortal_enter)
		strcpy(st->immortal_enter, ch->player.immortal_enter);
	else
		*st->immortal_enter = '\0';

	if (ch->player.immortal_exit)
		strcpy(st->immortal_exit, ch->player.immortal_exit);
	else
		*st->immortal_exit = '\0';

	/* WRITE ALIASES TO SAVE FILE */
	db9800_load_save_aliases(ch, SAVE_ALIAS);

	st->start_room = GET_START(ch);
	st->private_room = GET_PRIVATE(ch);
	st->visible_level = GET_VISIBLE(ch);

	/* ZERO OUT FILLER AREA BEFORE WRITING */
	ptr = (char *) &st->filler;
	bzero(ptr, sizeof(st->filler));

	for (i = 0; i < MAX_SKILLS; i++)
		st->skills[i] = ch->skills[i];

	strcpy(st->name, GET_NAME(ch));

	for (i = 0; i < MAX_SAVES; i++)
		st->apply_saving_throw[i] = ch->specials.apply_saving_throw[i];

	for (i = 0; i <= 2; i++)
		st->conditions[i] = GET_COND(ch, i);

	if (UpdateTime)
		GET_LOGOFF_TIME(ch) = time(0);
	/* player is leaving game so update last_on time */
	player_table[ch->nr].pidx_last_on = GET_LOGOFF_TIME(ch);
	st->logoff_time = GET_LOGOFF_TIME(ch);
	st->how_player_left_game = GET_HOW_LEFT(ch);

	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type) {
			/* Add effect of the spell or it will be lost */
			/* When saving without quitting               */
			ha1300_affect_to_char(ch, &st->affected[i]);
		}
	}

	for (i = 0; i < MAX_WEAR; i++) {
		if (char_eq[i])
			ha1925_equip_char(ch, char_eq[i], i, 0, BIT1);
	}

	/* ha1225_affect_total(ch); */

}				/* END OF db6200_char_to_store() */
 /* create a new entry in the in-memory index table for player file */
int db6300_create_entry(char *name)
{
	int i;

	if (top_of_p_table < 0) {
		CREATE(player_table, struct player_index_element, 1);
		top_of_p_table = 0;
	}
	else {
		top_of_p_table++;
		if ((!(player_table = (struct player_index_element *)
		   realloc(player_table, sizeof(struct player_index_element) *
			   (top_of_p_table + 1))))) {
			perror("create entry");
			exit(1);
		}
		bzero((char *) &player_table[top_of_p_table],
		      sizeof(struct player_index_element));
	}
	CREATE(player_table[top_of_p_table].pidx_name,
	       char, strlen(name) + 1);

	/* copy lowercase equivalent of name to table field */
	for (i = 0; (*(player_table[top_of_p_table].pidx_name + i) =
		     LOWER(*(name + i))); i++);

	player_table[top_of_p_table].pidx_nr = top_of_p_table;
	return (top_of_p_table);

}				/* END OF db6300_create_entry() */


/* write the vital data of a player to the player file */
void db6400_save_char(struct char_data * ch, sh_int start_room)
{

	/* Define save structure *st */
	struct char_file_u st;
	FILE *fl;
	char mode[4];
	int expand;
	char buf[MAX_STRING_LENGTH];



	/* ZERO OUT PLAYER AREA */
	bzero((char *) &st, sizeof(st));

	if (IS_NPC(ch) || !ch->desc)
		return;

	if ((expand = (ch->desc->pos > top_of_p_file))) {
		strcpy(mode, "ab");
		top_of_p_file++;
	}
	else
		strcpy(mode, "rb+");

	db6200_char_to_store(ch, &st, TRUE);
	/* IF THE CHAR IS IMMORTAL, LEAVE START ROOM AS THEY DEFINED IT */
	if (GET_LEVEL(ch) <= PK_LEV ||
	    GET_START(ch) < 1) {
		st.start_room = start_room;
	}

	strcpy(st.pwd, ch->desc->pwd);
	/* taking this out to reduce lag.. we think its fixed by now...
	 * bzero(buf2, sizeof(buf2)); sprintf(buf2, "Saving:  %d ",
	 * ch->desc->pos); strncat(buf2, st.name, 20);  for (idx=0; idx <
	 * strlen(st.pwd); idx++) { jdx = *(st.pwd+idx); bzero(buf,
	 * sizeof(buf)); sprintf(buf, "\\%o", jdx);   strcat(buf2, buf); }
	 * printf(buf2); trace_log(buf2); */
	fl = fopen(PLAYER_FILE, mode);
	if (!fl) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: db6400 saving player %d.\r\n", errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	if (!expand)
		fseek(fl, ch->desc->pos * sizeof(struct char_file_u), 0);

	fwrite(&st, sizeof(struct char_file_u), 1, fl);

	fclose(fl);

}				/* END OF db6400_save_char() */

void db6500_delete_char(struct char_data * ch, char *arg, int cmd)
{

	struct char_file_u st;
	FILE *fl;
	int lv_char_nr, idx;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* ZERO OUT PLAYER AREA */
	bzero((char *) &st, sizeof(st));
	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));

	for (; *arg == ' '; arg++);
	if (is_number(arg)) {
		lv_char_nr = atoi(arg);
	}
	else {
		lv_char_nr = pa2000_find_name(arg);
		if (lv_char_nr < 0) {
			sprintf(buf, "1%s", arg);
			lv_char_nr = pa2000_find_name(buf);
			if (lv_char_nr < 0) {
				send_to_char("Sorry, unknown player. Use nr address.\r\n",
					     ch);
				return;
			}
		}
	}

	if (lv_char_nr > top_of_p_file ||
	    lv_char_nr < 0) {
		sprintf(buf, "Position must be between 0 and %d.\r\n",
			top_of_p_file);
		send_to_char(buf, ch);
		return;
	}

	fl = fopen(PLAYER_FILE, "rb+");
	if (!fl) {
		perror("save char");
		sprintf(buf, "ERROR: %d db6500 player file for deleting.\r\n", errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fread(&st, sizeof(struct char_file_u), 1, fl);

	if (GET_LEVEL(ch) < player_table[lv_char_nr].pidx_level) {
		send_to_char("You keep trying crap like this and you'll lose more than DELETE!\r\n", ch);
		sprintf(buf2, "WARNING: %s just tried to delete %s!!",
			GET_REAL_NAME(ch),
			st.name);
		do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
		fclose(fl);
		return;
	}

	/* CHECK IF THEY ARE TRYING TO DELETE THE BOSS */

	if (!strncmp(st.name, "Xavior", 6) || !strncmp(st.name, "Oldschool", 9)) {
		send_to_char("Try that once more and you'll never be able to login here again.\r\n", ch);
		sprintf(buf2, "WARNING: %s just tried to delete %s!!",
			GET_REAL_NAME(ch), st.name);
		do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
		fclose(fl);
		return;
	}

	/* DELETE OR RESTORE CHAR? */
	if (*st.name == '1' || *st.name == '2') {
		strcpy(st.name, (st.name + 1));
		bzero(st.title, sizeof(st.title));
		sprintf(st.title, "%s returned from the dead", st.name);
		sprintf(buf, "You undeleted player: %d %s  %s.\r\n",
			lv_char_nr, st.name, st.title);
		sprintf(buf2, "%s undeleted player %d %s.",
			GET_REAL_NAME(ch), lv_char_nr, st.name);
	}
	else {
		bzero(st.title, sizeof(st.title));
		sprintf(st.title, "%s the deleted", st.name);

		r1150_alias_delete_file(st.name);
		r1125_backup_delete_file(st.name);
		r1100_rent_delete_file(st.name);
		bzero(buf, sizeof(buf));
		sprintf(buf, "1%s", st.name);
		bzero(st.name, sizeof(st.name));
		strncpy(st.name, buf, 19);

		sprintf(buf, "You deleted player: %d %s  %s.\r\n",
			lv_char_nr, (st.name + 1), st.title);
		sprintf(buf2, "%s deleted player %d %s.",
			GET_REAL_NAME(ch), lv_char_nr, st.name);
	}
	send_to_char(buf, ch);
	do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
	spec_log(buf2, GOD_COMMAND_LOG);

	/* UPDATE PLAYER TABLE */
	free(player_table[lv_char_nr].pidx_name);
	CREATE(player_table[lv_char_nr].pidx_name,
	       char, strlen(st.name) + 1);
	for (idx = 0; (*(player_table[lv_char_nr].pidx_name + idx) =
		       LOWER(*(st.name + idx))); idx++);

	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);

	fclose(fl);

}				/* END OF db6500_delete_char() */

void db6550_self_delete(struct char_data * ch, char *arg)
{

	struct char_file_u st;
	FILE *fl;
	int lv_char_nr, idx;

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* ZERO OUT PLAYER AREA */
	bzero((char *) &st, sizeof(st));
	bzero(buf, sizeof(buf));
	bzero(buf2, sizeof(buf2));

	for (; *arg == ' '; arg++);
	if (is_number(arg)) {
		lv_char_nr = atoi(arg);
	}
	else {
		lv_char_nr = pa2000_find_name(arg);
		if (lv_char_nr < 0) {
			sprintf(buf, "2%s", arg);
			lv_char_nr = pa2000_find_name(buf);
			if (lv_char_nr < 0) {
				do_wizinfo("Sorry, unknown player. Use nr address.\r\n",
					   IMO_IMP, ch);
				return;
			}
		}
	}

	if (lv_char_nr > top_of_p_file ||
	    lv_char_nr < 0) {
		sprintf(buf, "Position must be between 0 and %d.\r\n",
			top_of_p_file);
		send_to_char(buf, ch);
		return;
	}

	fl = fopen(PLAYER_FILE, "rb+");
	if (!fl) {
		perror("save char");
		sprintf(buf, "ERROR: %d db6550 player file for deleting.\r\n", errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	r1100_rent_delete_file(GET_REAL_NAME(ch));
	player_table[lv_char_nr].pidx_clan_number = 0;
	player_table[lv_char_nr].pidx_clan_rank = 0;
	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fread(&st, sizeof(struct char_file_u), 1, fl);

	bzero(st.title, sizeof(st.title));
	sprintf(st.title, "%s the deleted", st.name);

	bzero(buf, sizeof(buf));
	sprintf(buf, "2%s", st.name);
	bzero(st.name, sizeof(st.name));
	strncpy(st.name, buf, 19);

	sprintf(buf, "You deleted player: %d %s  %s.\r\n",
		lv_char_nr, (st.name + 1), st.title);
	sprintf(buf2, "%s self-deleted player %d %s.",
		GET_REAL_NAME(ch), lv_char_nr, st.name);

	do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
	spec_log(buf2, DELETE_LOG);

	/* UPDATE PLAYER TABLE */
	free(player_table[lv_char_nr].pidx_name);
	CREATE(player_table[lv_char_nr].pidx_name,
	       char, strlen(st.name) + 1);
	for (idx = 0; (*(player_table[lv_char_nr].pidx_name + idx) =
		       LOWER(*(st.name + idx))); idx++);

	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);
	fclose(fl);
}


void db6600_set_password(struct char_data * ch, char *arg, int cmd)
{

	struct char_file_u st;
	struct char_data *vict;
	FILE *fl;
	int lv_char_nr;
	char buf[MAX_STRING_LENGTH];



	/* ZERO OUT PLAYER AREA */
	bzero((char *) &st, sizeof(st));
	bzero(buf, sizeof(buf));

	/* STRIP OUT PLAYER TO MODIFY */
	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);

	if (is_number(buf)) {
		lv_char_nr = atoi(buf);
	}
	else {
		lv_char_nr = pa2000_find_name(buf);
		if (lv_char_nr < 0) {
			send_to_char("Sorry, unknown player. Use nr address.\r\n",
				     ch);
			return;
		}
	}

	if (lv_char_nr > top_of_p_file ||
	    lv_char_nr < 0) {
		sprintf(buf, "Position must be between 0 and %d.\r\n",
			top_of_p_file);
		send_to_char(buf, ch);
		return;
	}

	/* LOOK FOR THE PLAYER ONLINE */
	vict = 0;
	vict = ha3100_get_char_vis(ch, player_table[lv_char_nr].pidx_name);

	if (GET_LEVEL(ch) < player_table[lv_char_nr].pidx_level) {
		send_to_char("You can't change a password on a char higher than you!\r\n", ch);
		sprintf(buf, "WARNING: %s just tried to set %s's password",
			GET_REAL_NAME(ch),
			st.name);
		do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
		return;
	}

	/* IS THERE A PASSWORD? */
	for (; *arg == ' '; arg++);
	if (!*arg) {
		send_to_char("Sorry, but we only set em.\r\n", ch);
		return;
	}

	fl = fopen(PLAYER_FILE, "rb+");
	if (!fl) {
		perror("save char");
		sprintf(buf, "ERROR: %d db6600 player file for changing.\r\n", errno);
		perror(buf);
		ABORT_PROGRAM();
	}

	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fread(&st, sizeof(struct char_file_u), 1, fl);

	sprintf(buf, "You changed password on player: %d %s to %s.\r\n",
		lv_char_nr, st.name, arg);
	send_to_char(buf, ch);
	sprintf(buf, "%s set the password for %d %s.",
		GET_REAL_NAME(ch), lv_char_nr, st.name);
	do_wizinfo(buf, GET_LEVEL(ch) + 1, ch);
	spec_log(buf, GOD_COMMAND_LOG);

	/* CHANGE PASSWORD */
	bzero(buf, sizeof(buf));
	strcpy(buf, (char *) crypt(arg, arg));
	bzero(st.pwd, sizeof(st.pwd));
	strncpy(st.pwd, buf, sizeof(st.pwd) - 1);

	/* UPDATE PLAYER TABLE */
	fseek(fl, lv_char_nr * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);
	fclose(fl);
	/* gee this shows up all over the place  bzero(buf2, sizeof(buf2));
	 * sprintf(buf2, "Setting: %d ", lv_char_nr); strncat(buf2, st.name,
	 * 20);  for (idx=0; idx < strlen(st.pwd); idx++) { jdx =
	 * *(st.pwd+idx); bzero(buf3, sizeof(buf3)); sprintf(buf3, "\\%o",
	 * jdx);   strcat(buf2, buf3); } printf(buf2); trace_log(buf2); */
	if (!vict) {
		send_to_char("Player doesn't appear to be logged on.\r\n", ch);
		send_to_char("Updated player database.\r\n", ch);
		return;
	}

	/* SHOULDN'T HAPPEN, BUT... */
	if (IS_NPC(vict)) {
		return;
	}

	/* IS THE PERSON LINK DEAD? */
	if (!(vict->desc)) {
		send_to_char("Player is linkdead - updated database only.\r\n",
			     ch);
		return;
	}

	strncpy(vict->desc->pwd, buf, 10);

	return;

}				/* END OF db6600_set_password() */


 /************************************************************* *
 *  procs of a (more or less) general utility nature            *
 ************************************************************** */


 /* read and allocate space for a '~'-terminated string from a given file */
char *db7000_fread_string(FILE * fl)
{
#define READ_MAX_LENGTH 2048

	char buf[MAX_STRING_LENGTH], tmp[READ_MAX_LENGTH];
	char *rslt;
	register char *point;
	int flag;



	bzero(buf, MAX_STRING_LENGTH);

	do {

		if (!fgets(tmp, READ_MAX_LENGTH, fl)) {
			main_log("Oh oh string too long: hope this helps");
			main_log(buf);
			main_log(tmp);
			perror("fread_str: (db.c)");
			return (0);
		}

		if (strlen(tmp) + strlen(buf) > MAX_STRING_LENGTH) {
			main_log("db7000_fread_string: string too large (db.c)");
			main_log(tmp);
			return (0);
		}
		else
			strcat(buf, tmp);

		for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point); point--);

		if ((flag = (*point == '~')))
			if (*(buf + strlen(buf) - 3) == '\n') {
				*(buf + strlen(buf) - 2) = '\r';
				*(buf + strlen(buf) - 1) = '\0';
			}
			else
				*(buf + strlen(buf) - 2) = '\0';
		else {
			*(buf + strlen(buf) + 1) = '\0';
			*(buf + strlen(buf)) = '\r';
		}
	}

	while (!flag);

	/* do the allocate boogie  */

	if (strlen(buf) > 0) {
		CREATE(rslt, char, strlen(buf) + 1);
		strcpy(rslt, buf);
	}
	else
		rslt = 0;

	return (rslt);
}				/* END OF db7000_fread_string() */


 /* release memory allocated for a char struct */
void db7100_free_char(struct char_data * ch, int lv_flag)
{

	struct char_data *curr_char, *temp_char, *prev_char, *next_char;
	char buf[MAX_STRING_LENGTH];
	int idx;



	memcpy((char *) &gv_ch, ch, sizeof(struct char_data));;

	/* Check if mob is still a quest mob */
	/* This shouldn't happen but can happen if an IMM purges a quest mob */
	if (IS_NPC(ch) &&
	    (ch->questmob) &&
	    (ch->questmob->questmob == ch)) {
		main_log("Quest mob disappeard from game.");
		if ((ch->questmob->desc) &&
		    (ch->questmob->desc->connected == CON_PLAYING) &&
		    (ch->questmob->in_room >= 0))
			send_to_char("Quest mob lost.\r\n", ch->questmob);
		qu1400_clear_quest(ch->questmob);
	}
	if (IS_NPC(ch))
		reset_target(ch);

	if (ch->questsys) {
		free(ch->questsys);
		ch->questsys = 0;
	}

	curr_char = ch;
	prev_char = 0;
	next_char = 0;
	for (temp_char = character_list; temp_char; temp_char = next_char) {
		next_char = temp_char->next;
		if (temp_char->desc)
			if (temp_char->desc->last_target == ch)
				temp_char->desc->last_target = 0;
		if (temp_char == ch) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ERROR: Found the char in character list. flag: %d\r\n", lv_flag);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			if (temp_char->next) {
				main_log("There is a next char.");
			}
			else {
				main_log("There IS NOT a next char.");
			}
			sprintf(buf, "Pointers: temp %ld curr %ld prev %ld next %ld",
				(long) temp_char, (long) curr_char, (long) prev_char, (long) next_char);
			main_log(buf);
			/* IS THE PREVIOUS GUY OK? */
			if (prev_char) {
				if (FAILS_SANITY(prev_char))
					main_log("Previous char fails sanity.");
				else
					main_log("Previous char is sane.");
			}
			/* IS THE NEXT GUY OK? */
			if (next_char) {
				if (FAILS_SANITY(next_char))
					main_log("Next char fails sanity.");
				else
					main_log("Next char is sane.");
			}
			break;
		}
		if (next_char) {
			prev_char = temp_char;
		}
	}

	free(GET_NAME(ch));

	if (ch->player.title) {
		free(ch->player.title);
	}
	if (ch->player.short_descr) {
		free(ch->player.short_descr);
	}
	if (ch->player.long_descr) {
		free(ch->player.long_descr);
	}
	if (ch->player.description) {
		free(ch->player.description);
	}
	if (ch->player.immortal_enter) {
		free(ch->player.immortal_enter);
	}
	if (ch->player.immortal_exit) {
		free(ch->player.immortal_exit);
	}

	for (idx = 0; idx < MAX_AFFECT && ch->affected; idx++) {
		if (ch->affected) {
			ha1325_affect_remove(ch, ch->affected, REMOVE_FOLLOWER);
		}
	}
	/* for (af = ch->affected; af; af = af->next) ha1325_affect_remove(ch,
	 * af, 0); */

	free(ch);
	ch = 0;

}				/* END OF db7100_free_char() */


 /* release memory allocated for an obj struct */
void db7200_free_obj(struct obj_data * obj)
{
	struct extra_descr_data *this, *next_one;
	struct descriptor_data *d;



	if (IS_QUEST_ITEM(obj))
		for (d = descriptor_list; d; d = d->next) {
			if ((d->character) &&
			    (d->character->questobj) &&
			    (d->character->questobj == obj)) {
				qu1400_clear_quest(d->character);
				if ((d->connected == CON_PLAYING) &&
				    (d->character->in_room >= 0))
					send_to_char("Quest item is lost.\r\n", d->character);
			}
		}
	free(obj->name);
	if (obj->description)
		free(obj->description);
	if (obj->short_description)
		free(obj->short_description);
	if (obj->action_description)
		free(obj->action_description);

	for (this = obj->ex_description; (this != 0); this = next_one) {
		next_one = this->next;
		if (this->keyword)
			free(this->keyword);
		if (this->description)
			free(this->description);
		free(this);
	}

	free(obj);
}				/* END OF db7200_free_obj() */

char *db7300_file_to_string(char *name)
{
	struct stat theStat;
	char *theString, *finalString;
	FILE *theFile;
	long i, offset = 0, cr = 0, lf = 0;



	if ((stat(name, &theStat))
	    || (!(theString = (char *) malloc((size_t) theStat.st_size + 1)))
	    || (!(theFile = fopen(name, "rb")))) {
		if (!(theString = (char *) malloc(1))) {
			main_log("FATAL ERROR IN db7300_file_to_string(db.c) NOT EVEN 1!?! byte free");
			return (0);
		}
		fprintf(stderr, "\r\nERROR: db7300_file_to_string (db.c) unable to read FILE: %s\r\n", name);
		*theString = 0;	/* must return a valid string */
	}
	else {
		fread(theString, (size_t) theStat.st_size, 1, theFile);
		theString[(size_t) theStat.st_size] = 0;	/* terminate string */
		fclose(theFile);
	}
	for (i = 0; i < theStat.st_size; i++) {
		if (theString[i] == '\n')
			lf++;
		if (theString[i] == '\r')
			cr++;
	}
	if (cr >= lf)
		return theString;
	if (cr > 0) {
		main_log("db7300_file_to_string: some crlf's but not all!?!?");
		return theString;
	}

	/* not as many cr's as lf's */
	/* assuming here that either lines are crlf, or lf, lines ending in
	 * just cr will really screw this up */
	if (!(finalString = (char *) malloc((size_t) theStat.st_size + 1 + lf - cr))) {
		main_log("FATAL ERROR IN db7300_file_to_string(db.c) cant alloc 2nd buffer");
		exit(1);
	}
	for (i = 0; i <= theStat.st_size; i++) {	/* go right to
							 * terminating 0 */
		if (theString[i] == '\n') {
			finalString[i + offset] = '\r';
			offset++;
		}
		finalString[i + offset] = theString[i];
	}
	free(theString);
	return finalString;
}				/* END OF db7300_file_to_string() */

char *db7350_read_from_file(FILE * file_in)
{

	int my_len;
	char buf[MAX_STRING_LENGTH];



	bzero(buf, sizeof(buf));
	fgets(buf, 100, file_in);
	while (*buf == '#' || *buf == ' ') {
		bzero(buf, sizeof(buf));
		fgets(buf, 100, file_in);
	}

	/* REMOVE TRAILING <CR> and <NL> */
	while (strlen(buf) > 0) {
		my_len = strlen(buf) - 1;
		if (buf[my_len] == '\r' || buf[my_len] == '\n')
			buf[my_len] = 0;
		else
			break;
	}

	bzero(gv_str, sizeof(gv_str));
	strcpy(gv_str, buf);
	return (gv_str);

}				/* END OF db7350_read_from_file() */
 /* clear some of the the working variables of a char */
void db7400_reset_char(struct char_data * ch)
{
	int i;



	for (i = 0; i < MAX_WEAR; i++)
		ch->equipment[i] = 0;

	ch->followers = 0;
	ch->master = 0;
	ch->carrying = 0;
	ch->next = 0;
	ch->next_fighting = 0;
	ch->next_in_room = 0;
	ch->specials.fighting = 0;
	ch->specials.position = POSITION_STANDING;
	ch->specials.default_pos = POSITION_STANDING;
	ch->specials.carry_weight = 0;
	ch->specials.carry_items = 0;

	if (GET_HIT(ch) <= 0)
		GET_HIT(ch) = 1;
	if (GET_MOVE(ch) <= 0)
		GET_MOVE(ch) = 1;
	if (GET_MANA(ch) <= 0)
		GET_MANA(ch) = 1;
}				/* END OF db7400_reset_char() */


 /* clear ALL the working vars of a char, do NOT free any space */
void db7500_clear_char(struct char_data * ch)
{



	memset(ch, '\0', sizeof(struct char_data));
	ch->beg_rec_id = CH_SANITY_CHECK;
	ch->in_room = NOWHERE;
	ch->riding = 0;
	ch->specials.was_in_room = NOWHERE;
	ch->specials.position = POSITION_STANDING;
	ch->specials.default_pos = POSITION_STANDING;
	GET_AC(ch) = MAX_ARMOUR;/* Basic Armor */
	ch->end_rec_id = CH_SANITY_CHECK;
	GET_ALIASES(ch) = 0;

}				/* END OF db7500_clear_char() */


void db7600_clear_object(struct obj_data * obj)
{



	memset(obj, '\0', sizeof(struct obj_data));

	obj->item_number = -1;
	obj->in_room = NOWHERE;
}				/* END OF db7600_clear_object() */


 /* initialize a new character */
void db7700_init_char(struct char_data * ch)
{
	int i;



	/* *** if this is our first player --- default to IMO_IMP *** */

	if (top_of_p_table < 0) {
		GET_EXP(ch) = LEVEL_EXP(IMO_IMP) + 1;
		GET_LEVEL(ch) = IMO_IMP;
	}

	li2100_set_title(ch);

	ch->player.short_descr = 0;
	ch->player.long_descr = 0;
	ch->player.description = 0;
	ch->player.immortal_enter = 0;
	ch->player.immortal_exit = 0;
	GET_PRIVATE(ch) = 0;
	GET_VISIBLE(ch) = 0;
	GET_INCOGNITO(ch) = 0;
	//Incognito & ghost Shalira 22.07 .01
		GET_GHOST(ch) = 0;
	GET_BLESS(ch) = 0;
	//are we blessing ? Shalira 24.07 .01

		ch->player.time.birth = time(0);
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);

	db7750_roll_stat_dice(ch, 0);

	GET_DEATHS(ch) = 0;

	/* adjust weight/height based on sex */
	if (ch->player.sex == SEX_MALE) {
		ch->player.weight = number(120, 180);
		ch->player.height = number(160, 200);
	}
	else {
		GET_DEX(ch) = 11;
		ch->player.weight = number(100, 160);
		ch->player.height = number(150, 180);
	}

	ch->points.hit = 1;
	ch->points.mana = 1;
	ch->points.move = 1;

	ch->points.armor = MAX_ARMOUR;

	for (i = 0; i < MAX_SKILLS; i++) {
		if (GET_LEVEL(ch) > PK_LEV) {
			ch->skills[i].learned = 100;
		}
		else {
			ch->skills[i].learned = 0;
		}
	}

	ch->specials.affected_by = 0;
	ch->specials.affected_02 = 0;
	ch->specials.jailtime = 0;
	ch->specials.deathshadowtime = 0;
	ch->specials.godmuzzletime = 0;
	ch->specials.spells_to_learn = 0;

	ch->jailed = 0;

	for (i = 0; i < MAX_SAVES; i++)
		ch->specials.apply_saving_throw[i] = 0;
	ha9700_set_saving_throws(ch, 0);

	li9800_set_alignment(ch);
	li9850_set_conditions(ch);

	return;

}				/* END OF db7700_init_char() */


void db7750_roll_stat_dice(struct char_data * ch, int lv_flag)
{



	/* STR */
	ch->abilities.str = number(1, 7) + number(1, 7) + number(1, 7) +
	races[GET_RACE(ch)].adj_str;
	if (ch->abilities.str < 4)
		ch->abilities.str = 4;
	if (ch->abilities.str >
	    races[GET_RACE(ch)].max_str)
		ch->abilities.str =
			races[GET_RACE(ch)].max_str;

	/* INT */
	ch->abilities.intel = number(1, 7) + number(1, 7) + number(1, 7) +
		races[GET_RACE(ch)].adj_int;
	if (ch->abilities.intel < 4)
		ch->abilities.intel = 4;
	if (ch->abilities.intel >
	    races[GET_RACE(ch)].max_int)
		ch->abilities.intel =
			races[GET_RACE(ch)].max_int;

	/* WIS */
	ch->abilities.wis = number(1, 7) + number(1, 7) + number(1, 7) +
		races[GET_RACE(ch)].adj_wis;
	if (ch->abilities.wis < 4)
		ch->abilities.wis = 4;
	if (ch->abilities.wis >
	    races[GET_RACE(ch)].max_wis)
		ch->abilities.wis =
			races[GET_RACE(ch)].max_wis;

	/* DEX */
	ch->abilities.dex = number(1, 7) + number(1, 7) + number(1, 7) +
		races[GET_RACE(ch)].adj_dex;
	if (ch->abilities.dex < 4)
		ch->abilities.dex = 4;
	if (ch->abilities.dex >
	    races[GET_RACE(ch)].max_dex)
		ch->abilities.dex =
			races[GET_RACE(ch)].max_dex;

	/* CON */
	ch->abilities.con = number(1, 7) + number(1, 7) + number(1, 7) +
		races[GET_RACE(ch)].adj_con;
	if (ch->abilities.con < 4)
		ch->abilities.con = 4;
	if (ch->abilities.con >
	    races[GET_RACE(ch)].max_con)
		ch->abilities.con =
			races[GET_RACE(ch)].max_con;

	/* CHA */
	ch->abilities.cha = number(1, 7) + number(1, 7) + number(1, 7) +
		races[GET_RACE(ch)].adj_cha;
	if (ch->abilities.cha < 4)
		ch->abilities.cha = 4;
	if (ch->abilities.cha >
	    races[GET_RACE(ch)].max_cha)
		ch->abilities.cha =
			races[GET_RACE(ch)].max_cha;
	return;

}				/* END OF db7750_roll_stat_dice() */
 /* returns the real number of the room with given virtual number */
int db8000_real_room(int virtual)
{
	int bot, top, mid, zone;



	if ((zone = db4000_zone_of(virtual)) == -1) {
		fprintf(stderr, "Room %d does not exist (illegal zone)\r\n", virtual);
		return (-1);
	}
	bot = zone_table[zone].real_bottom;
	top = zone_table[zone].real_top;	/* should set to real_top of
						 * zone */

	if ((top < 0) || (bot < 0)) {
		fprintf(stderr, "Room %d does not exist (empty zone)\r\n", virtual);
		return (-1);
	}
	if (top > top_of_world) {
		fprintf(stderr, "db8000_real_room: force limit search to top_of_world\r\n");
		top = top_of_world;
	}
	if (IS_SET(zone_table[zone].dirty_wld, SCRAMBLED)) {
		/* oh oh order of zone is scrambled - no binary */
		for (mid = bot;
		     mid <= top_of_world && mid != world[zone_table[zone].real_top].next;
		     mid = world[mid].next) {
			if (world[mid].number == virtual)
				return (mid);
			else if (world[mid].number > virtual)
				break;
		}
		fprintf(stderr, "Real_Room(d): %d does not exist in database\r\n", virtual);
		return (-1);
	}
	else {
		/* perform binary search on world-table */
		for (;;) {
			mid = (bot + top) / 2;
			if ((world + mid)->number == virtual)
				return (mid);
			if (bot >= top) {
				fprintf(stderr, "Real_Room(c): %d does not exist in database\r\n", virtual);
				return (-1);
			}
			if ((world + mid)->number > virtual)
				top = mid - 1;
			else
				bot = mid + 1;
		}
	}
}				/* END OF db8000_real_room() */


 /* returns the real number of the object with given virtual number */
int db8100_real_mobile(int virtual)
{
	int bot, top, mid, zone;



	if ((zone = db4000_zone_of(virtual)) == -1) {
		fprintf(stderr, "Real_Mob: Mob %d does not exist (illegal zone)\r\n", virtual);
		return (-1);
	}
	bot = zone_table[zone].bot_of_mobt;
	top = zone_table[zone].top_of_mobt;	/* should set to real_top of
						 * zone */

	if ((top < 0) || (bot < 0)) {
		fprintf(stderr, "mob %d does not exist (empty zone)\r\n", virtual);
		return (-1);
	}
	if (top > top_of_mobt) {
		fprintf(stderr, "real_mob: force limit search to top_of_mobt\r\n");
		top = top_of_mobt;
	}
	if (IS_SET(zone_table[zone].dirty_mob, SCRAMBLED)) {
		/* oh oh order of zone is scrambled - no binary */
		for (mid = bot; mid <= top_of_mobt && mid <= top; mid = mob_index[mid].next) {
			if (mob_index[mid].virtual == virtual)
				return (mid);
			else if (mob_index[mid].virtual > virtual)
				break;
		}
		fprintf(stderr, "Real_Mob(d): %d does not exist in database\r\n", virtual);
		return (-1);
	}
	else {
		/* perform binary search on mob-table */
		for (;;) {
			mid = (bot + top) / 2;
			if ((mob_index + mid)->virtual == virtual) {
				return (mid);
			}
			if (bot >= top) {
				fprintf(stderr, "Real_Mob(c): %d does not exist in database\r\n", virtual);
				return (-1);
			}
			if ((mob_index + mid)->virtual > virtual)
				top = mid - 1;
			else
				bot = mid + 1;
		}
	}
}				/* END OF db8100_real_mobile() */


 /* returns the real number of the object with given virtual number */
int db8200_real_object(int virtual)
{
	int bot, top, mid, zone;



	if ((zone = db4000_zone_of(virtual)) == -1)
		return (-1);
	bot = zone_table[zone].bot_of_objt;
	top = zone_table[zone].top_of_objt;	/* should set to real_top of
						 * zone */

	if ((top < 0) || (bot < 0))
		return (-1);
	if (IS_SET(zone_table[zone].dirty_obj, SCRAMBLED)) {
		/* oh oh order of zone is scrambled - no binary */
		for (mid = bot; mid <= top_of_objt && mid <= top; mid = obj_index[mid].next) {
			if (obj_index[mid].virtual == virtual)
				return (mid);
			else if (obj_index[mid].virtual > virtual)
				break;
		}
		fprintf(stderr, "Real_obj(dirty): %d does not exist in database\r\n", virtual);
		return (-1);
	}
	else {
		/* perform binary search on obj-table */
		for (;;) {
			mid = (bot + top) / 2;
			if ((obj_index + mid)->virtual == virtual)
				return (mid);
			if (bot >= top) {
				fprintf(stderr, "Real_obj(clean): %d does not exist in database\r\n", virtual);
				return (-1);
			}
			if ((obj_index + mid)->virtual > virtual)
				top = mid - 1;
			else
				bot = mid + 1;
		}
	}
}				/* END OF db8200_real_object() */
/* Save routines go here! */
void db8300_write_string(FILE * theFile, char *theString)
{
	int i, j;



	if (theString) {
		j = strlen(theString);
		for (i = 0; i < j; i++) {
			if (theString[i] != 13)
				fputc(theString[i], theFile);
		}
	}
	fprintf(theFile, "~\n");
}

int db8400_virtual_room(int real)
{



	if ((real < 0) || (real > top_of_world))
		return -1;
	else
		return world[real].number;
}

int db8500_virtual_door(int real)
{
	/* this was a dumb idea, in order to make all the direction flags
	 * backwards compatible to original diku... however since the flags got
	 * screwed up somewhere during transition why worry about it, this
	 * routine isnt even called anymore btw and if you intend to resurrect
	 * it, it should read: if IS_SET(EX_ISDOOR) && !IS_SET(EX_PICK... etc
	 * etc */

	if (real == EX_ISDOOR)
		return 1;
	else if (real == (EX_ISDOOR | EX_PICKPROOF))
		return 2;
	else
		return 0;
}

int db8600_virtual_obj(int real)
{



	if ((real < 0) || (real > top_of_objt))
		return -1;
	else
		return obj_index[real].virtual;
}

int db8700_virutal_mob(int real)
{
	if ((real < 0) || (real > top_of_mobt))
		return -1;
	else
		return mob_index[real].virtual;

}				/* END OF db8700_virtual_mob() */


/* abc */
int db8800_ok_to_load_obj(struct obj_data * obj, int av_obj_num, int lv_flag)
{

	int chance_roll;
	char buf[100];



	if (obj) {
		av_obj_num = obj->item_number;
	}

	if (av_obj_num < 0 || av_obj_num > top_of_objt) {
		main_log("ERROR: db8800 invalid object number");
		return (FALSE);

	}

	/* IS THIS A MAXIMUM NUMBER OBJECT? */
	if (IS_SET(obj_index[av_obj_num].flags, OBJI_OMAX)) {
		if (obj_index[av_obj_num].number <
		    obj_index[av_obj_num].maximum) {
			/* LOAD IT */
			return (TRUE);
		}
		else {
			return (FALSE);
		}
	}

	/* Check against a 1d100 roll to see if load  *OBJECT_LOADING* */

	chance_roll = number(0, 100);
	if (obj_index[av_obj_num].maximum > 100) {
		if (!(IS_SET(obj_index[av_obj_num].flags, OBJI_OMAX))) {
			sprintf(buf, "WARNING: db8800 object %d has repop chance of %d%%",
				obj_index[av_obj_num].virtual, obj_index[av_obj_num].maximum);
			main_log(buf);
			spec_log(buf, ERROR_LOG);
		}
	}

	if ((gv_port != ZONING_PORT) && (chance_roll >= obj_index[av_obj_num].maximum)) {
		return (FALSE);
	}

	/* don't load items with a 1% or lower poprate if we just rebooted the
	 * mud */
	if ((gv_booting_mud == 1) && (obj_index[av_obj_num].maximum < 1)) {
		return (FALSE);
	}

	/* LOAD IT */
	return (TRUE);

}				/* END OF db8800_ok_to_load_obj() */


void db9000_save_zon(int zone)
{
	FILE *theFile;
	int cmd_no;
	char sys[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], lv_command;



	sprintf(sys, "areas/%s.zon", zone_table[zone].filename);
	if ((theFile = fopen(sys, "r+"))) {
		fclose(theFile);
		sprintf(sys, "mv areas/%s.zon areas.bak/%s.zon",
			zone_table[zone].filename, zone_table[zone].filename);
		system(sys);
	}
	else {
		fclose(theFile);
	}

	sprintf(sys, "areas/%s.zon", zone_table[zone].filename);
	if (!(theFile = fopen(sys, "w+"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: db9000_save_zon: could not open file for writing.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	/* and now the fun begins - write out the zones zon file */
	rewind(theFile);	/* just ensure we're at the start */
	/* next field isnt used anymore reserved for zone flags */
	fprintf(theFile, "#%d\n", zone_table[zone].flags);
	/* db8300_write_string(theFile, zone_table[zone].name); */
	db8300_write_string(theFile, zone_table[zone].filename);
	fprintf(theFile, "%d ", zone_table[zone].top);
	fprintf(theFile, "%d ", zone_table[zone].lifespan);
	fprintf(theFile, "%d ", zone_table[zone].reset_mode);
	fprintf(theFile, "%d ", zone_table[zone].x_coordinate);
	fprintf(theFile, "%d ", zone_table[zone].y_coordinate);
	fprintf(theFile, "%d\n*\n", zone_table[zone].z_coordinate);

	cmd_no = 0;
	while (1) {
		lv_command = zone_table[zone].cmd[cmd_no].command;

		if (lv_command == 'S')
			break;
		fprintf(theFile, "%c %d",
			lv_command,
			zone_table[zone].cmd[cmd_no].if_flag);
		switch (lv_command) {
		case 'M':
			fprintf(theFile, " %d %d %d",
			db8700_virutal_mob(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
				db8400_virtual_room(zone_table[zone].cmd[cmd_no].arg3));
			break;
		case 'O':
			fprintf(theFile, " %d %d %d",
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
				db8400_virtual_room(zone_table[zone].cmd[cmd_no].arg3));
			break;
		case 'G':
			fprintf(theFile, " %d %d",
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2);
			break;
		case 'E':
			fprintf(theFile, " %d %d %d",
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
				zone_table[zone].cmd[cmd_no].arg3);
			break;
		case 'P':
			fprintf(theFile, " %d %d %d",
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg3));
			break;
		case 'R':
			fprintf(theFile, " %d %d %d",
			db8600_virtual_obj(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
				db8400_virtual_room(zone_table[zone].cmd[cmd_no].arg3));
			break;
		case 'D':
			fprintf(theFile, " %d %d %d",
			db8400_virtual_room(zone_table[zone].cmd[cmd_no].arg1),
				zone_table[zone].cmd[cmd_no].arg2,
				zone_table[zone].cmd[cmd_no].arg3);
			break;
		default:
			fprintf(theFile, " %d %d",
				zone_table[zone].cmd[cmd_no].arg1,
				zone_table[zone].cmd[cmd_no].arg2);
			break;
		}

		fprintf(theFile, "\n");
		cmd_no++;
	}			/* END OF db9000_save_zon() */

	/* okey dokey all done close up shop */
	fprintf(theFile, "*\nS\n*\n$~\n#999999\n");	/* trailer */
	fclose(theFile);
	REMOVE_BIT(zone_table[zone].dirty_zon, UNSAVED);	/* no longer unsaved */
	sprintf(sys, "Zone %s.zon saved (%d zcmds)",
		zone_table[zone].filename, cmd_no);
	zone_table[zone].reset_num = cmd_no;
	main_log(sys);
	spec_log(sys, SYSTEM_LOG);
}

void db9100_save_wld(int zone)
{
	FILE *theFile;
	int i, dir, count = 0;
	struct extra_descr_data *extra;
	char sys[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];



	/* WE DON'T LET THEM UPDATE THE VEHICLE ZONE */
	if (gv_port != DEVELOPMENT_PORT) {
		if (is_abbrev("vehicle", zone_table[zone].filename)) {
			main_log("WARNING: You can't save the Vehicle zone.");
			return;
		}
	}

	sprintf(sys, "areas/%s.wld", zone_table[zone].filename);
	if ((theFile = fopen(sys, "r+"))) {
		fclose(theFile);
		sprintf(sys, "mv areas/%s.wld areas.bak/%s.wld",
			zone_table[zone].filename, zone_table[zone].filename);
		system(sys);
	}
	else {
		fclose(theFile);
	}

	sprintf(sys, "areas/%s.wld", zone_table[zone].filename);
	if (!(theFile = fopen(sys, "w+"))) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: db9100_save_wld: could not open file for writing.\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	/* and now the fun begins - write out the zones wld file */
	rewind(theFile);	/* just ensure we're at the start */
	if ((zone_table[zone].real_bottom < 0) ||
	    (zone_table[zone].real_top < 0))
		main_log("db9100_save_wld: no rooms in this zone");
	else
		for (i = zone_table[zone].real_bottom;
		     world[i].number <= zone_table[zone].top
		     && world[i].number >= zone_table[zone].bottom;
		     i = world[i].next) {
			if (i > top_of_world) {
				break;
			}
			if (i < 0) {
				main_log("db9100_save_wld: tried to advance below beginning of world");
				break;
			}
			fprintf(theFile, "#%d\n", world[i].number);
			db8300_write_string(theFile, world[i].name);
			db8300_write_string(theFile, world[i].description);
			fprintf(theFile, "%d ", world[i].zone);
			fprintf(theFile, "%ld ", world[i].room_flags);
			fprintf(theFile, "%ld ", world[i].room_flags2);
			fprintf(theFile, "%d ", world[i].min_level);
			fprintf(theFile, "%d ", world[i].max_level);
			fprintf(theFile, "%d\n", world[i].sector_type);

			for (dir = 0; dir <= 5; dir++) {
				if (world[i].dir_option[dir] != 0) {
					fprintf(theFile, "D%d\n", dir);
					db8300_write_string(theFile, world[i].dir_option[dir]->general_description);
					db8300_write_string(theFile, world[i].dir_option[dir]->keyword);
					fprintf(theFile, "%d %d %d\n",
					  world[i].dir_option[dir]->exit_info,	/* used to be
										 * db8500_virtual_door() */
						world[i].dir_option[dir]->key,
						db8400_virtual_room(world[i].dir_option[dir]->to_room));
				}
			}
			for (extra = world[i].ex_description;;
			     extra = extra->next) {
				if (!(extra))
					break;
				fprintf(theFile, "E\n");
				db8300_write_string(theFile, extra->keyword);
				db8300_write_string(theFile, extra->description);
			}
			fprintf(theFile, "S\n");
			count++;
		}
	/* okey dokey all done close up shop */
	fprintf(theFile, "$~\n#999999\n");	/* trailer */
	fclose(theFile);
	REMOVE_BIT(zone_table[zone].dirty_wld, UNSAVED);	/* zone saved but order
								 * is scrambled */
	sprintf(sys, "Zone %s.wld saved (%d rooms)",
		zone_table[zone].filename, count);
	main_log(sys);
	spec_log(sys, SYSTEM_LOG);
}				/* END OF db9100_save_wld() */


void db9200_write_mob(FILE * theNewFile, struct char_data * mob, int index)
{

	int idx;
	char buf[MAX_STRING_LENGTH];



	fprintf(theNewFile, "#%d\n", mob_index[index].virtual);
	mob_index[index].pos = ftell(theNewFile);

	/***** String data *** */
	db8300_write_string(theNewFile, mob_index[index].name);
	db8300_write_string(theNewFile, mob->player.short_descr);
	db8300_write_string(theNewFile, mob->player.long_descr);
	db8300_write_string(theNewFile, mob->player.description);

	/* *** Numeric data *** */
	fprintf(theNewFile, "%ld ", GET_ACT1(mob));
	fprintf(theNewFile, "%ld ", GET_ACT2(mob));
	fprintf(theNewFile, "%ld ", GET_ACT3(mob));
	fprintf(theNewFile, "%ld ", mob->specials.affected_by);
	fprintf(theNewFile, "%d S\n", mob->specials.alignment);

	/* all mobs are simple mobs */
	fprintf(theNewFile, "%d ", GET_LEVEL(mob));
	fprintf(theNewFile, "%d ", -1 * (mob->points.hitroll - 40));
	fprintf(theNewFile, "%d ", mob->points.armor / 10);
	fprintf(theNewFile, "%dd%d+%d ",
		mob->points.max_hit,
		mob->player.weight,
		mob->points.hit);
	fprintf(theNewFile, "%dd%d+%d\n",
		mob->specials.damnodice,
		mob->specials.damsizedice,
		mob->points.damroll);

	fprintf(theNewFile, "%d ", mob->points.gold);
	fprintf(theNewFile, "%d\n", GET_EXP(mob));
	fprintf(theNewFile, "%d ", mob->specials.position);
	fprintf(theNewFile, "%d ", mob->specials.default_pos);
	fprintf(theNewFile, "%d\n", mob->player.sex);

	fprintf(theNewFile, "%d ", GET_RACE(mob));
	fprintf(theNewFile, "%d ", GET_CLASS(mob));
	fprintf(theNewFile, "%d ", GET_REAL_STR(mob));
	fprintf(theNewFile, "%d ", GET_REAL_INT(mob));
	fprintf(theNewFile, "%d ", GET_REAL_WIS(mob));
	fprintf(theNewFile, "%d ", GET_REAL_DEX(mob));
	fprintf(theNewFile, "%d ", GET_REAL_CON(mob));
	fprintf(theNewFile, "%d\n", GET_REAL_CHA(mob));

	for (idx = 1; idx < MAX_SKILLS; idx++) {
		if (idx != SKILL_SLASH &&
		    idx != SKILL_BLUDGEON &&
		    idx != SKILL_PIERCE &&
		    mob->skills[idx].learned > 0) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "SKILL=%s %d",
			    sk_sp_defines[idx - 1], mob->skills[idx].learned);
			fprintf(theNewFile, "%s\n", buf);
		}
	}

	for (idx = 0; idx < 3; idx++) {
		if (mob_index[index].attack_type[idx] > 0) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "ATTACK_SKILL=%d",
				mob_index[index].attack_skill[idx][0]);
			fprintf(theNewFile, "%s\n", buf);

			bzero(buf, sizeof(buf));
			sprintf(buf, "ATTACK_SKILL=%d",
				mob_index[index].attack_skill[idx][1]);
			fprintf(theNewFile, "%s\n", buf);

			bzero(buf, sizeof(buf));
			sprintf(buf, "ATTACK_SKILL=%d",
				mob_index[index].attack_skill[idx][2]);
			fprintf(theNewFile, "%s\n", buf);

			bzero(buf, sizeof(buf));
			sprintf(buf, "ATTACK_TYPE=%d",
				mob_index[index].attack_type[idx]);
			fprintf(theNewFile, "%s\n", buf);

		}
	}

}				/* END OF db9200_write_mob() */
/* WRITE MOB FILE */
void db9300_save_mob(int zone)
{
	char sys[MAX_STRING_LENGTH];
	FILE *theNewFile;
	int index;
	struct char_data *mob;



	if (zone_table[zone].bot_of_mobt < 0) {
		sprintf(sys, "WARNING: Zone %s.mob no mobs to save", zone_table[zone].filename);
		main_log(sys);
		spec_log(sys, SYSTEM_LOG);
		return;
	}

	for (index = zone_table[zone].bot_of_mobt;
	     index <= zone_table[zone].top_of_mobt;
	     index = mob_index[index].next) {
		/* read 'em all */
		mob = db5000_read_mobile(mob_index[index].virtual, PROTOTYPE);
		if (!(mob)) {
			bzero(sys, sizeof(sys));
			sprintf(sys, "ERROR db9300 reading prototype index: %d  mob: %d",
				index, mob_index[index].virtual);
			main_log(sys);
			spec_log(sys, SYSTEM_LOG);
			return;
		}
		GET_START(mob) = 0;
	}

	fclose(mob_f);
	mob_f_zone = -1;
	sprintf(sys, "areas/%s.mob", zone_table[zone].filename);
	if ((theNewFile = fopen(sys, "r+"))) {
		fclose(theNewFile);
		sprintf(sys, "mv areas/%s.mob areas.bak/%s.mob",
			zone_table[zone].filename,
			zone_table[zone].filename);
		system(sys);
	}
	else {
		fclose(theNewFile);
	}

	sprintf(sys, "areas/%s.mob", zone_table[zone].filename);
	if (!(theNewFile = fopen(sys, "w+"))) {
		main_log("db9300_save_mob: could not open file for writing.");
		perror("fopen");
		exit(1);
	}

	/* and now the fun begins - write out the zones mob file */
	for (index = zone_table[zone].bot_of_mobt;
	     index <= zone_table[zone].top_of_mobt;
	     index = mob_index[index].next) {
		db9200_write_mob(theNewFile, mob = (struct char_data *) mob_index[index].prototype, index);
		/* now free up the space taken by the prototype */
		if (mob->player.name)
			free(mob->player.name);
		if (mob->player.short_descr)
			free(mob->player.short_descr);
		if (mob->player.long_descr)
			free(mob->player.long_descr);
		if (mob->player.description)
			free(mob->player.description);

		/* Confirm mobs have no extra descriptions.... next_descr = 0;
		 * for (new_descr = mob->ex_description; new_descr; new_descr =
		 * next_descr){ if (new_descr->keyword)
		 * free(new_descr->keyword); if (new_descr->description)
		 * free(new_descr->description); next_descr = new_descr->next;
		 * if (new_descr) free(new_descr); } */
		if (mob)
			free(mob);
		mob_index[index].prototype = 0;
	}

	/* okey dokey all done close up shop */
	fprintf(theNewFile, "$~\n#999999\n");	/* trailer */
	fclose(theNewFile);

	REMOVE_BIT(zone_table[zone].dirty_mob, UNSAVED);	/* mobs saved */
	sprintf(sys, "Zone %s.mob saved", zone_table[zone].filename);
	main_log(sys);
	spec_log(sys, SYSTEM_LOG);
	return;

}				/* END OF db9300_save_mob() */


void db9400_write_obj(FILE * theNewFile, struct obj_data * obj, int index)
{
	int i;

	struct extra_descr_data *new_descr;



	fprintf(theNewFile, "#%d\n", obj_index[index].virtual);
	obj_index[index].pos = ftell(theNewFile);
	db8300_write_string(theNewFile, obj_index[index].name);

	/* maximum */
	fprintf(theNewFile, "%d\n", obj_index[index].maximum);

	db8300_write_string(theNewFile, obj->short_description);
	db8300_write_string(theNewFile, obj->description);
	db8300_write_string(theNewFile, obj->action_description);

	/* *** numeric data *** */
	fprintf(theNewFile, "%d ", obj->obj_flags.type_flag);
	fprintf(theNewFile, "%d ", obj->obj_flags.flags1);
	fprintf(theNewFile, "%d ", obj->obj_flags.flags2);
	fprintf(theNewFile, "%d\n", obj->obj_flags.wear_flags);
	fprintf(theNewFile, "%d ", obj->obj_flags.value[0]);
	fprintf(theNewFile, "%d ", obj->obj_flags.value[1]);
	fprintf(theNewFile, "%d ", obj->obj_flags.value[2]);
	fprintf(theNewFile, "%d\n", obj->obj_flags.value[3]);
	fprintf(theNewFile, "%d ", obj->obj_flags.weight);
	fprintf(theNewFile, "%d ", obj->obj_flags.cost);
	fprintf(theNewFile, "%d\n", obj->obj_flags.cost_per_day);

	/* *** extra descriptions *** */
	for (new_descr = obj->ex_description; new_descr; new_descr = new_descr->next) {
		fprintf(theNewFile, "E\n");
		db8300_write_string(theNewFile, new_descr->keyword);
		db8300_write_string(theNewFile, new_descr->description);
	}

	for (i = 0; (i < MAX_OBJ_AFFECT); i++) {
		if (obj->affected[i].location != APPLY_NONE)
			fprintf(theNewFile, "A\n%d %d\n",
				obj->affected[i].location,
				obj->affected[i].modifier);
	}
}				/* END OF db9400_write_obj() */


void db9500_save_obj(int zone)
{
	char sys[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	FILE *theNewFile;
	int index;
	struct obj_data *obj;
	struct extra_descr_data *new_descr, *next_descr;



	if (zone_table[zone].bot_of_objt < 0) {
		sprintf(sys, "Zone %s.obj no objects to save", zone_table[zone].filename);
		main_log(sys);
		spec_log(sys, SYSTEM_LOG);
		return;
	}

	for (index = zone_table[zone].bot_of_objt;
	     index <= zone_table[zone].top_of_objt;
	     index = obj_index[index].next) {
		/* read 'em all */
		obj = db5100_read_object(obj_index[index].virtual, PROTOTYPE);
	}

	fclose(obj_f);
	obj_f_zone = -1;
	sprintf(sys, "areas/%s.obj", zone_table[zone].filename);
	if ((theNewFile = fopen(sys, "r+"))) {
		fclose(theNewFile);
		sprintf(sys, "mv areas/%s.obj areas.bak/%s.obj",
			zone_table[zone].filename, zone_table[zone].filename);
		system(sys);
	}
	else {
		fclose(theNewFile);
	}
	sprintf(sys, "areas/%s.obj", zone_table[zone].filename);
	if (!(theNewFile = fopen(sys, "w+"))) {
		main_log("db9500_save_obj: could not open file for writing.");
		perror("fopen");
		exit(1);
	}
	/* and now the fun begins - write out the zones obj file */
	for (index = zone_table[zone].bot_of_objt;
	     index <= zone_table[zone].top_of_objt;
	     index = obj_index[index].next) {
		if (gv_verbose_messages == TRUE) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "    Updating obj: %d\r\n",
				obj_index[index].virtual);
			main_log(buf);
			spec_log(buf, SYSTEM_LOG);
		}		/* END OF verbose message */
		db9400_write_obj(theNewFile, obj = (struct obj_data *) obj_index[index].prototype, index);
		/* now free up the space taken by the prototype */
		free(obj->name);
		free(obj->short_description);
		free(obj->description);
		free(obj->action_description);
		next_descr = 0;
		for (new_descr = obj->ex_description; new_descr; new_descr = next_descr) {
			free(new_descr->keyword);
			free(new_descr->description);
			next_descr = new_descr->next;
			free(new_descr);
		}
		free(obj);
		obj_index[index].prototype = 0;
	}
	/* okey dokey all done close up shop */
	fprintf(theNewFile, "$~\n#999999\n");	/* trailer */
	fclose(theNewFile);

	REMOVE_BIT(zone_table[zone].dirty_obj, UNSAVED);	/* objs saved */
	sprintf(sys, "Zone %s.obj saved", zone_table[zone].filename);
	main_log(sys);
	spec_log(sys, SYSTEM_LOG);
}				/* END OF db9500_save_obj() */

void db9600_save_area(int zone)
{



	if ((zone >= 0) && (zone <= top_of_zone_table)) {
		if (IS_SET(zone_table[zone].dirty_zon, UNSAVED))
			db9000_save_zon(zone);
		if (IS_SET(zone_table[zone].dirty_wld, UNSAVED))
			db9100_save_wld(zone);
		if (IS_SET(zone_table[zone].dirty_obj, UNSAVED))
			db9500_save_obj(zone);
		if (IS_SET(zone_table[zone].dirty_mob, UNSAVED))
			db9300_save_mob(zone);
	}
}				/* END OF db9600_save_area() */

void db9700_global_save(void)
{
	int idx;



	for (idx = 0; idx <= top_of_zone_table; idx++)
		db9600_save_area(idx);

}				/* END OF db9700_global_save() */

void db9800_load_save_aliases(struct char_data * ch, int lv_direction)
{



	if (lv_direction == LOAD_ALIAS) {
		//if (GET_ALIASES(ch))
			//clear_aliases(ch);
		read_aliases(ch);
	}


	if (lv_direction == SAVE_ALIAS) {
		delete_aliases(GET_NAME(ch));
		write_aliases(ch);
	}

}				/* END OF db9800_load_save_aliases() */

int db9900_purge_check(struct char_file_u * lv_char, int lv_flag)
{

	unsigned long lv_seconds_gone;
	int rc, lv_months, lv_days, lv_hours, lv_minutes;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* SAFETY CHECK */
	if (lv_char->logoff_time > time(0) ||
	    lv_char->logoff_time < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: %s has invalid logoff time: %d defaulting.\r\n",
			lv_char->name, (int) lv_char->logoff_time);
		perror(buf);
		lv_char->logoff_time = (int) time(0);
	}

	lv_seconds_gone = time(0) - lv_char->logoff_time;

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

	bzero(buf, sizeof(buf));
	sprintf(buf, "player %s L(%d) was gone %d months, %d days, %d hours, %d minutes, %ld seconds\r\n",
		lv_char->name, lv_char->level, lv_months, lv_days, lv_hours, lv_minutes, lv_seconds_gone);

	/* 12 MONTH PURGE ON EQUIPMENT */
	if ((lv_months > 16) && (lv_char->level < IMO_LEV)) {
		rc = r1100_rent_delete_file(lv_char->name);
		if (rc) {
			sprintf(buf2, "PURGING EQP: %s L(%d) %d months.",
				lv_char->name, lv_char->level, lv_months);
			main_log(buf2);
			spec_log(buf2, PURGE_LOG);
		}
	}

	/* NEWBIE */

	if (lv_char->level < 10) {
		if (lv_months > 12 ){
			return (PURGE_NOW);
		}
		if (lv_months > 9){
		return (PURGE_LATER);
		}
		return (0);
	}

	/* MORTALS 2 - 15 */
/*
	else if (lv_char->level < 16) {
		if (lv_months > 2) {
			main_log(buf);
			spec_log(buf, PURGE_LOG);
			return (PURGE_NOW);
		}
		if (lv_months > 0)
			return (PURGE_LATER);
		return (0);
	}
*/
/*
	else if (lv_char->level < 30) {
		if (lv_months > 11) {
			main_log(buf);
			spec_log(buf, PURGE_LOG);
			return (PURGE_NOW);
		}
		if (lv_months > 9)
			return (PURGE_LATER);
		return (0);
	}
*/
	/* PURGE turned off for level 30 plus
	else if (lv_char->level < 41) {
		if (lv_months > 23) {
			main_log(buf);
			spec_log(buf, PURGE_LOG);
			return (PURGE_NOW);
		}
		if (lv_months > 10)
			return (PURGE_LATER);
		return (0);
	}

	else if (lv_char->level == 41 || lv_char->level == 42) {
		if (lv_months >= 16) {
			main_log(buf);
			spec_log(buf, PURGE_LOG);
			return (PURGE_NOW);
		}
		if (lv_months == 10)
			return (PURGE_LATER);
		return (0);
	}

	else if (lv_char->level == 47) {
		if (lv_months > 15) {
			main_log(buf);
			spec_log(buf, PURGE_LOG);
			return (PURGE_NOW);
		}
		if (lv_months > 10)
			return (PURGE_LATER);
		return (0);
	}
        PURGE Turned off for Level 30 plus */
	/* GODS - NEVER PURGE */
	if (lv_char->level > IMO_SPIRIT) {
		return (0);
	}

	return (0);
	//Everyone 's OK, by default.

}				/* END OF db9900_purge_check() */

void db10000_load_clans(void)
{

	FILE *clans_file;
	int idx, jdx;
	char file_name[80], buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH],
	  buf2[MAX_STRING_LENGTH];



	/* load clans database */
	db10050_load_clans_new();

	/* load clans ASCII files if database was empty or not found */
	if (!gv_last_clan) {
		/* Create & ZERO STRUCTURE */
		for (idx = 0; idx < 20; idx++) {
			if (gv_last_clan > 0) {
				clans = (struct clans_data *) realloc(clans, (gv_last_clan + 1) * sizeof(struct clans_data));
				if (!(clans)) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "ERROR: realloc clans\r\n");
					perror(buf);
					ABORT_PROGRAM();
				}
			}
			else {
				CREATE(clans, struct clans_data, sizeof(struct clans_data));
			}
			bzero((char *) &clans[idx], sizeof(struct clans_data));
			strcpy(clans[idx].name, "undefined");
			gv_last_clan++;
		}

		/* OPEN AND READ FILES */
		for (idx = 0; idx < gv_last_clan; idx++) {

			/* TRY TO OPEN THE FILE */
			bzero(file_name, sizeof(file_name));
			sprintf(file_name, "clans/%d", idx);
			clans_file = fopen(file_name, "rb");

			if (!clans_file) {
				bzero(buf2, sizeof(buf2));
				sprintf(buf2, "ERROR: Unable to open file: %s errno: %d",
					file_name, errno);
				main_log(buf2);
				spec_log(buf2, ERROR_LOG);
				exit(1);
			}

			strcpy(buf1, db7350_read_from_file(clans_file));
			clans[idx].enabled = db5275_part_num("CLAN_ENABLED=", buf1, file_name, 0, 1);
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].color, db5250_parse_str("CLAN_COLOR=", buf1, file_name, 10));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].name, db5250_parse_str("CLAN_NAME=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].leader, db5250_parse_str("CLAN_LEADER=", buf1, file_name, 20));
			strcpy(buf1, db7350_read_from_file(clans_file));
			clans[idx].room = db5275_part_num("CLAN_ROOM=", buf1, file_name, 0, 100000);
			strcpy(buf1, db7350_read_from_file(clans_file));
			clans[idx].don_room = db5275_part_num("CLAN_DON_ROOM=", buf1, file_name, 0, 100000);
			strcpy(buf1, db7350_read_from_file(clans_file));
			clans[idx].board_num = db5275_part_num("CLAN_BOARD_NUM=", buf1, file_name, 0, 100000);
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].rank[MAX_CLAN_RANKS], db5250_parse_str("CLAN_RANK_LEADER=", buf1, file_name, 19));
			for (jdx = MAX_CLAN_RANKS - 1; jdx >= 0; jdx--) {
				bzero(buf2, sizeof(buf2));
				sprintf(buf2, "CLAN_RANK_%d=", jdx);
				strcpy(buf1, db7350_read_from_file(clans_file));
				strcpy(clans[idx].rank[jdx], db5250_parse_str(buf2, buf1, file_name, 20));
			}
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[0], db5250_parse_str("CLAN_DESC_1=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[1], db5250_parse_str("CLAN_DESC_2=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[2], db5250_parse_str("CLAN_DESC_3=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[3], db5250_parse_str("CLAN_DESC_4=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[4], db5250_parse_str("CLAN_DESC_5=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[5], db5250_parse_str("CLAN_DESC_6=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[6], db5250_parse_str("CLAN_DESC_7=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[7], db5250_parse_str("CLAN_DESC_8=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[8], db5250_parse_str("CLAN_DESC_9=", buf1, file_name, 79));
			strcpy(buf1, db7350_read_from_file(clans_file));
			strcpy(clans[idx].desc[9], db5250_parse_str("CLAN_DESC_10=", buf1, file_name, 79));

			fclose(clans_file);

		}		/* END OF idx loop */

		db10100_save_clans();
	}
	return;

}				/* END OF db10000_load_clans() */

void db10100_save_clans()
{

	FILE *fout;
	char buf[MAX_STRING_LENGTH];
	int idx;
	struct clan_war_data *save_war;

	fout = fopen("clans/clans", "wb");

	if (!fout) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Couldn't save clans\r\n");
		perror(buf);
		ABORT_PROGRAM();
	}

	for (idx = 0; idx < gv_last_clan; idx++) {
		save_war = clans[idx].war;
		clans[idx].war = 0;
		fwrite(&clans[idx], sizeof(struct clans_data), 1, fout);
		clans[idx].war = save_war;
	}

	fclose(fout);
}

void db10050_load_clans_new()
{

	FILE *fin;
	char buf[MAX_STRING_LENGTH];
	struct clans_data clan;

	if (clans)
		free(clans);
	gv_last_clan = 0;

	fin = fopen("clans/clans", "rb");

	if (!fin)
		return;

	while (!feof(fin)) {
		if (fread(&clan, sizeof(struct clans_data), 1, fin)) {
			if (gv_last_clan > 0) {
				clans = (struct clans_data *) realloc(clans, (gv_last_clan + 1) * sizeof(struct clans_data));
				if (!(clans)) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "ERROR: realloc clans\r\n");
					perror(buf);
					ABORT_PROGRAM();
				}
			}
			else {
				CREATE(clans, struct clans_data, sizeof(struct clans_data));
			}
			clan.war = 0;
			clans[gv_last_clan] = clan;
			gv_last_clan++;
		}
	}
}

void db10200_create_clan(struct char_data * ch)
{

	int lv_clan;
	int idx;
	struct clans_data clan;
	char buf[MAX_STRING_LENGTH];

	/* Find a disabled clan to overwrite */
		for (idx = 1; idx < gv_last_clan; idx++)
		if (!clans[idx].enabled)
			break;
	lv_clan = idx;

	/* Check if we need to create a new structure */
			if (lv_clan == gv_last_clan) {
			clans = (struct clans_data *) realloc(clans, (gv_last_clan + 1) * sizeof(struct clans_data));
			if (!(clans)) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: realloc clans\r\n");
				perror(buf);
				ABORT_PROGRAM();
			}
			gv_last_clan++;
		}

	/* Set default clan values */
		clan.board_num = 0;
	strcpy(clan.color, "&w");
	for (idx = 0; idx < MAX_CLAN_DESC; idx++)
		sprintf(clan.desc[idx], "desc #%d", idx);
	for (idx = 0; idx <= MAX_CLAN_RANKS; idx++)
		sprintf(clan.rank[idx], "rank #%d", idx);
	clan.don_room = gv_donate_room;
	clan.enabled = TRUE;
	strcpy(clan.leader, "Nobody");
	strcpy(clan.name, "Nameless");
	clan.number = lv_clan;
	clan.room = 0;
	clan.war = 0;

	clans[lv_clan] = clan;

	sprintf(buf, "Clan #%d created, please set the appropriate values before handing it over to a player\r\n", lv_clan);
	send_to_char(buf, ch);
	send_to_char("Making you leader of the clan to adjust settings.\r\n", ch);
	CLAN_RANK(ch) = 11;
	CLAN_NUMBER(ch) = lv_clan;
}

int db20000_copy_file(const char *from, const char *to)
{
	int inFile, outFile;
	char line[MAX_STRING_LENGTH];
	int bytes;

	/* I found this code on the internet, didn 't come up with it by myself
		   Relic */

		if (!*from || !*to) {
		perror("db20000_copy_file called incorrectly.");
		return 0;
	}

	if ((inFile = open(from, O_RDONLY)) == -1) {
		perror("db20000_copy_file error, couldn't open source file.");
		return 0;
	}

	if ((outFile = open(to, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP)) == -1) {
		perror("db20000_copy_file error, coudn't open destination file.");
		return 0;
	}

	while ((bytes = read(inFile, line, sizeof(line))) > 0)
		write(outFile, line, bytes);

	close(inFile);
	close(outFile);

	return 1;
}/* end of db20000_copy_file() */
