/********************************************************************
*  file: db.h , Database module.                     Part ofDIKUMUD *
*  Usage: Loading/Saving chars booting world.                       *
******************************************************************* */
/* various variables */
FILE *trace_file;

void Global_Save(void);

extern char *credits;		/* the Credits List                */
extern char *news;		/* the news                        */
extern char *todo;		/* the todo list                   */
extern char *wipe;		/* future player purges            */
extern char *motd;		/* the messages of today           */
extern char *help;		/* the main help page              */
extern char *info;		/* the info text                   */
extern char *gv_syslog;		/* the syslog                      */
extern char *wizlist;		/* the wizlist                     */
extern char *commands;		/* the commands list               */
extern char *changelog;		/* the changelog file				*/


extern struct zone_data *zone_table;
extern struct room_data *world;

extern struct mob_index_data *mob_index;
extern struct obj_index_data *obj_index;
extern struct quest_data *quest_index;

extern struct char_data *character_list;
extern struct char_data *combat_list;
extern struct obj_data *object_list;
extern struct vehicle_data *vehicle_list;
extern struct message_list fight_messages[MAX_MESSAGES];

extern int top_of_zone_table;
extern int top_of_world;
extern int top_of_mobt;
extern int top_of_objt;
extern int top_of_questt;
int carved_world;		/* number of blocks currently chunked */

extern struct help_index_element *help_index;
extern struct time_info_data time_info;
extern struct weather_data weather_info;

struct player_index_element *player_table;	/* index to player file */
extern int top_of_p_table;
extern int top_of_p_file;

/* data files used by the game system */

#define DEFAULT_DIR       "lib"	/* default data directory     */

#define WORLD_FILE        "tinyworld.wld"	/* room definitions          */
#define MOB_FILE          "tinyworld.mob"	/* monster prototypes        */
#define OBJ_FILE          "tinyworld.obj"	/* object prototypes         */
#define SHOP_FILE         "tinyworld.shp"	/* shopkeepers               */
#define ZONE_FILE         "tinyworld.zon"	/* zone defs & command tables */
#define CREDITS_FILE      "credits"	/* for the 'credits' command  */
#define NEWS_FILE         "news"/* for the 'news' command     */
#define CHANGELOG_FILE    "changelog"/* changelog */
#define TODO_FILE         "todo"/* for the 'todo' command     */
#define WIPE_FILE         "wipe"/* for the 'wipe' command     */
#define MOTD_FILE         "motd"/* messages of today        */
#define PLAYER_FILE       "players"	/* the player database      */
#define TIME_FILE         "time"/* game calendar information  */
#define IDEA_FILE         "ideas"	/* for the'idea'-command     */
#define TYPO_FILE         "typos"	/* 'typo'           */
#define BUG_FILE          "bugs"/* 'bug'            */
#define MESS_FILE         "messages"	/* damage message           */
#define SOCMESS_FILE      "actions"	/* messgs for socialacts     */
#define HELP_KWRD_FILE    "help"/* for HELP <keywrd>        */
#define HELP_PAGE_FILE    "help_index"	/* for HELP <CR>            */
#define INFO_FILE         "info"/* for INFO                 */
#define WIZLIST_FILE      "wizlist"	/* for WIZLIST              */
#define WIZCOPY_FILE      "wizlist.cpy"	/* for WIZLIST.cpy          */
#define SYSLOG_FILE				"../logs/syslog"	/* for SYSLOG              */
#define POSEMESS_FILE     "poses"	/* for 'pose'-command       */
#define COMMANDS_FILE         "commands"	/* for the 'commands' command     */
#define wiztitle_file_name "wiztitles"
#define BOUNTY_FILE				"bounties"	/* To save the bounty
								 * list	 */
/* New logfiles Shalira 23.07.01 */
/* add new entrys to the log_rotate script in /prod/logs/ */
/* If the logs need to be viewable from within the mud,   */
/* remember to update the loginf[] arrays in logview.c    */

#define DEATH_LOG	"../logs/deaths"	/* Log deaths here */
#define STEAL_LOG	"../logs/steals"	/* Log player steals, and mob
						 * steals/junks here */
#define DT_LOG		"../logs/dts"	/* Log DT's here   */
#define FIRSTHIT_LOG	"../logs/firsthit"	/* Log first hit here */
#define CONNECT_LOG	"../logs/connects"	/* Log connects/quits here */
#define WRONG_PW_LOG	"../logs/wrongpw"	/* Log exceding PW attempts
						 * here */
#define NEW_PLAYER_LOG	"../logs/newplayers"	/* Log new players here */
#define DELETE_LOG	"../logs/delete"	/* Log deletes here */
#define GOD_COMMAND_LOG	"../logs/godcommands"	/* Log god commands here */
#define ERROR_LOG	"../logs/errors"	/* Log system errors here */
#define SYSTEM_LOG	"../logs/system"	/* Log system messages here */
#define RENT_GONE_LOG	"../logs/rentgone"	/* Log rent gone */
#define PURGE_LOG	"../logs/purges"	/* Log purges */
#define EQUIPMENT_LOG	"../logs/equipment"	/* Log equipment transfers */
#define NO_HELP_LOG	"../logs/nohelp"	/* Log missing help entries */
#define PKILL_LOG	"../logs/pkill"	/* Log pkill actions here */
#define MAIN_LOG	"../logs/mainlog"	/* Log every thing here (old
						 * syslog) */
#define MAIN_LOG1	"../logs/mainlog.1"	/* Yesterdays mainlog */
#define MAIN_LOG2	"../logs/mainlog.2"	/* You guessed it. the logs
						 * from two days ago :) */
#define SPELL_LOG "../logs/spells"	/* To log oft misused spells */
#define FUN_LOG   "../logs/fun"	/* Fun little tidbits for IMP+'s */
#define BOUNTY_LOG "../logs/bounty"	/* Log file for bounties. */

/* Files used for the control of the autorun script from within the MUD
   Relic Oct.2003 */
#define KILL_SCRIPT_FILE "../.killscript"	/* To kill the autorun script */
#define FASTBOOT_SCRIPT_FILE "../.fastboot"	/* To make the script boot up
						 * faster */
#define PAUSE_SCRIPT_FILE "../pause"	/* To pause the script (keep it from
					 * booting the mud) indefinately */

/* Dirty flags */
#define UNSAVED   BIT0
#define SCRAMBLED BIT1		/* for mob/obj */
#define SHOWN     BIT2		/* for zone file */

#define REAL 0
#define VIRTUAL 1
#define PROTOTYPE 2

struct char_data *db5000_read_mobile(int nr, int type);
struct obj_data *db5100_read_object(int nr, int type);

#define MENU         \
"\n\rCrimsonMUD II Main Menu:\n\r\n\r\
&W1&w) &cEnter the game.&n\n\r\
&W2&w) &cConfigure character.&n\n\r\
&W3&w) &cEnter description.&n\n\r\
&W4&w) &cChange password.&n\n\r\
&W5&w) &cSet player killable flag to ON.&n\n\r\
&W6&w) &cDelete this character.&n\n\r\n\r\
&W0&w) &rExit from CrimsonMUD II.&n\n\r\n\r\
Make your choice: "

#define MENU_PK         \
"\n\rCrimsonMUD II Main Menu:\n\r\n\r\
&W1&w) &cEnter the game.&n\n\r\
&W2&w) &cConfigure character.&n\n\r\
&W3&w) &cEnter description.&n\n\r\
&W4&w) &cChange password.&n\n\r\
&W5&w) &cSet player killable flag to OFF.&n\n\r\
&W6&w) &cDelete this character.&n\n\r\n\r\
&W0&w) &rExit from CrimsonMUD II.&n\n\r\n\r\
Make your choice: "

#define CHOOSE_MENU(ch) (IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)?MENU_PK:MENU)

#define WELC_MESSG \
"\n\rWelcome to the land of CrimsonMUD II. May your visit here be...Interesting.\n\r"\
"&RPlease give us a vote on MUDConnect:&n\n\r"\
"https:\/\/www.mudconnect.com/cgi-bin/search.cgi?mode=mud_listing&&mud=Crimson+2+MUD\n\r\n\r"


/* structure for the reset commands */
struct reset_com {
	char command;		/* current command                      */
	bool if_flag;		/* if TRUE: exe only if preceding exe'd */
	int arg1;		/* */
	int arg2;		/* Arguments to the command             */
	int arg3;		/* */

	/* Commands:              * 'M': Read a mobile     * 'O': Read an
	 * object    * 'G': Give obj to mob   * 'P': Put obj in obj    * 'G':
	 * Obj to char       * 'E': Obj to char equip * 'D': Set state of door * */
};
/* element in monster index-tables   */
struct mob_index_data {
	int virtual;		/* virtual number of this mob              */
	long pos;		/* file position of this field             */
	char *name;		/* file position of this field             */
	int number;		/* number of existing units of this mob    */
	sh_int maximum;		/* maximum number allowed in the game      */
	char *prototype;	/* prototype for online creation */
	int (*func) ();		/* special procedure for this mob      */
	int next;		/* number of next entry in table if scrambled */
	int zone;		/* zone obj belongs too */
	ubyte attack_type[MAX_SPECIAL_ATTACK];
	/* for this we need skill for the second, third, and forth attack */
	ubyte attack_skill[MAX_SPECIAL_ATTACK][3];
};
/* element in object index-tables   */
struct obj_index_data {
	int virtual;		/* virtual number of this mob/obj          */
	long pos;		/* file position of this field             */
	char *name;		/* file position of this field             */
	int number;		/* number of existing units of this mob/obj */
	sh_int maximum;		/* maximum number allowed in the game */
	char *prototype;	/* prototype for online creation */
	int (*func) ();		/* special procedure for this mob/obj      */
	int (*func2) ();	/* Other special procedure (new type, more
				 * independent objs) ff */
	int next;		/* number of next entry in table if scrambled */
	int zone;		/* zone mob belongs too */
	int flags;		/* special object flags */
};
#define OBJI_OMAX      BIT0
#define OBJI_unused1_2 BIT1
#define OBJI_unused1_3 BIT2
#define OBJI_unused1_4 BIT3
#define OBJI_unused1_5 BIT4
#define OBJI_unused1_6 BIT5
#define OBJI_unused1_7 BIT6

/* zone definition structure. for the 'zone-table'   */
struct zone_data {
	char *filename;		/* whats the file name for this zone */
	char *name;		/* old name of this zone - maybe help soon */
	char *lord;		/* lord/author of the zone */
	int lifespan;		/* how long between resets (minutes) */
	int age;		/* current age of this zone(minutes) */
	int flags;		/* zone flags, notably ZONE_CLOSED */

	int dirty_wld;		/* technically I guess I should use one byte &
				 * flags */
	int dirty_zon;		/* but there's never going to be that many
				 * zones */
	int dirty_obj;
	int dirty_mob;

	int top;		/* upper virtual limit for rooms in thiszone */
	int bottom;		/* the bottom of the rooms(low #/virtual) */
	int real_top;		/* for intelligent search */
	int real_bottom;
	int bot_of_mobt;	/* real range for mob's */
	int top_of_mobt;
	int bot_of_objt;	/* real range for obj's */
	int top_of_objt;

	int x_coordinate;
	int y_coordinate;
	int z_coordinate;
	int reset_mode;		/* conditions for reset (see below) */
	struct reset_com *cmd;	/* command table for reset          */
	int reset_num;		/* conditions for reset (see below) */
	/*
	 *  Reset mode:                                   *
	 *  0: Don't reset, and don't update age.         *
	 *  1: Reset if no PC's are located in zone.      *
	 *  2: Just reset.                                *
	 *  -1: Zone offline for online creation purposes *
	 *  -2: reserved (maybe for zone cmds online)     *
	 */
};




/* for queueing zones for update   */
struct reset_q_element {
	int zone_to_reset;	/* ref to zone_data */
	struct reset_q_element *next;
};



/* structure for the update queue     */
struct reset_q_type {
	struct reset_q_element *head;
	struct reset_q_element *tail;
} reset_q;



struct player_index_element {
	char *pidx_name;
	int pidx_nr;
	int pidx_flag1;
	char pidx_email[36];
	time_t pidx_last_on;
	sbyte pidx_level;
	ubyte pidx_clan_number;
	ubyte pidx_clan_rank;
	int pidx_score;
};
#define EMAIL_SIZE           36

#define DFLG1_CAN_FINGER     BIT0
#define DFLG1_unused11       BIT1
#define DFLG1_unused12       BIT2
#define DFLG1_unused13       BIT3
#define DFLG1_unused14       BIT4
#define DFLG1_unused15       BIT5
#define DFLG1_unused16       BIT6
#define DFLG1_unused17       BIT7
#define DFLG1_unused18       BIT8
#define DFLG1_unused19       BIT9
#define DFLG1_unused20       BIT10

struct help_index_element {
	char *keyword;
	long pos;
};
