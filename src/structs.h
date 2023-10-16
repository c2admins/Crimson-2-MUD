/* ********************************************************************
/* ********************************************************************
*  file: structs.h , Structures    .                  Part of DIKUMUD *
*  Usage: Declarations of central data structures                     *
********************************************************************* */

#define INT8  unsigned char
#define INT16 unsigned short int
#define INT32 unsigned long int

#include <sys/types.h>

typedef signed char sbyte;
typedef unsigned char ubyte;
typedef signed short int sh_int;
typedef unsigned short int ush_int;
typedef char bool;
typedef char byte;

/* The minimum time (in seconds) that must elapse before a player can clear
their quest flag (this is in active / played time, not real time). */
#define	MINIMUM_PKILLER_TIME (24 * 60 * 60)

//Quest system triggers
#define QT_ON_KILLED	0
#define QT_ON_FLEE	1
#define QT_ON_ATTACK	2
#define QT_ON_ENTER	3
#define QT_ON_EXIT	4
#define QT_ON_SAY	5
#define QT_ON_CMD	6
#define QT_ON_IDLE	7
#define QT_ON_DROP	8
#define QT_ON_TIMER	9
#define QT_ON_COUNT_TIMER 10
#define QT_ON_GIVE 11

// Bits
#define BIT0                1
#define BIT1                2
#define BIT2                4
#define BIT3                8
#define BIT4               16
#define BIT5               32
#define BIT6               64
#define BIT7              128
#define BIT8              256
#define BIT9              512
#define BIT10            1024
#define BIT11            2048
#define BIT12            4096
#define BIT13            8192
#define BIT14           16384
#define BIT15           32768
#define BIT16           65536
#define BIT17          131072
#define BIT18          262144
#define BIT19          524288
#define BIT20         1048576
#define BIT21         2097152
#define BIT22         4194304
#define BIT23         8388608
#define BIT24        16777216
#define BIT25        33554432
#define BIT26        67108864
#define BIT27       134217728
#define BIT28       268435456
#define BIT29       536870912
#define BIT30      1073741824
#define BIT31      2147483648

unsigned long thebits[32];

#define GREETING_SCREENS  10	/* can be any number between 1 and 999 */
#define PULSE_ZONE      960
#define PULSE_OBJECTS    48
#define PULSE_MOBILE    160
#define PULSE_HUNT       48
#define PULSE_VIOLENCE   48
#define PULSE_ESCAPE     32
#define PULSE_VEHICLE    48
#define WAIT_SEC         16
#define WAIT_ROUND       16
#define AUTOSAVE_DELAY 1500	/* in seconds */
#define DEF_FATE        150
#define MAX_DIRS         6
#define MAX_USERS       150
#define WAR_MIN_HOURS	24

/* Rush timers */

#define RUSH_INTERVAL	30	/* Rush interval in minutes */
#define RUSH_INTERVAL2  30	/* max. random value added to interval */
#define RUSH_TIME	5	/* Rush minimum time */
#define RUSH_TIME2	20	/* rush maximum time */

/* Max quest stuff */
#define MAX_QUEST_SYSREGS	12
#define MAX_QUEST_REGS		9
#define MAX_QUEST_COMMANDS	68
#define MAX_QUEST_TRIGGERS	12

/*  stuff */

#define MAX_CLAN_STR_LENGTH 200
#define MAX_CLAN_RANKS      11
#define MAX_CLAN_DESC       10
#define NO_CLAN             0
#define CLAN_APPLY_RANK     0
#define CLAN_FIRST_RANK		1

/* values for object values */
#define VALUE_SPELL_LEVEL      0
#define VALUE_MAX_CHARGES      1
#define VALUE_CURR_CHARGE      2
#define VALUE_SPELL            3

#define VALUE_PORTAL_ROOM      0

#define VALUE_LIGHT_CHARGES    2

/* Maxes for the output buffer system. */
#define SMALL_BUFSIZE					4096
#define MAX_SOCK_BUF					(12 * SMALL_BUFSIZE)
#define MAX_PROMPT_LENGTH			560
#define GARBAGE_SPACE					64
#define LARGE_BUFSIZE					(MAX_SOCK_BUF - MAX_PROMPT_LENGTH - GARBAGE_SPACE)

#define HISTORY_SIZE					5	/* keep last five
								 * commands */
#define MAX_STRING_LENGTH			16384
#define MAX_NAME_LENGTH       15/* moved from comm.c */
#define MAX_RAW_INPUT_LENGTH	2048	/* Max size of *raw* input */
#define MAX_INPUT_LENGTH			1024
#define MAX_MESSAGES          120
#define MAX_ITEMS							153
#define MAX_DESC							1024
#define MAX_SPECIAL_ATTACK		4

#define CH_SANITY_CHECK       42
#define QUEUE_SANITY_CHECK		7

#define MESS_ATTACKER					1
#define MESS_VICTIM						2
#define MESS_ROOM						3

#define SECS_PER_REAL_MIN			60
#define SECS_PER_REAL_HOUR		(60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY			(24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR		(365*SECS_PER_REAL_DAY)

#define SECS_PER_MUD_HOUR			75
#define SECS_PER_MUD_DAY			(24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH		(35*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR			(17*SECS_PER_MUD_MONTH)


/* For 'type_flag' */
#define ITEM_LIGHT      1
#define ITEM_SCROLL     2
#define ITEM_WAND       3
#define ITEM_STAFF      4
#define ITEM_WEAPON     5
#define ITEM_FURNITURE  6
#define ITEM_MISSILE    7
#define ITEM_TREASURE   8
#define ITEM_ARMOR      9
#define ITEM_POTION    10
#define ITEM_WORN      11
#define ITEM_OTHER     12
#define ITEM_TRASH     13
#define ITEM_TRAP      14
#define ITEM_CONTAINER 15
#define ITEM_NOTE      16
#define ITEM_DRINKCON  17
#define ITEM_KEY       18
#define ITEM_FOOD      19
#define ITEM_MONEY     20
#define ITEM_PEN       21
#define ITEM_BOAT      22
#define ITEM_ZCMD      23
#define ITEM_VEHICLE   24
#define ITEM_CLIMBABLE 25
#define ITEM_SPELL     26
#define ITEM_QUEST     27
#define ITEM_QSTWEAPON 28
#define ITEM_QSTLIGHT  29
#define ITEM_QSTCONT   30
#define ITEM_CORPSE    31

/* Bitvector For 'wear_flags' */

#define ITEM_TAKE          BIT0
#define ITEM_WEAR_FINGER   BIT1
#define ITEM_WEAR_NECK     BIT2
#define ITEM_WEAR_BODY     BIT3
#define ITEM_WEAR_HEAD     BIT4
#define ITEM_WEAR_LEGS     BIT5
#define ITEM_WEAR_FEET     BIT6
#define ITEM_WEAR_HANDS    BIT7
#define ITEM_WEAR_ARMS     BIT8
#define ITEM_WEAR_SHIELD   BIT9
#define ITEM_WEAR_ABOUT    BIT10
#define ITEM_WEAR_WAISTE   BIT11
#define ITEM_WEAR_WRIST    BIT12
#define ITEM_WIELD         BIT13
#define ITEM_HOLD          BIT14
#define ITEM_THROW         BIT15
/* ITEM_UNUSED_YET is used as the light, so let's go ahead
* and define it as such, shall we??  (Redundant defines
																			* are ok, and I don't want to hunt down all the
																			* ITEM_UNUSED_YET's */
#define ITEM_LIGHT_SOURCE  BIT16
#define ITEM_UNUSED_YET    BIT16
#define ITEM_USABLE        BIT17
#define ITEM_WEAR_TAIL     BIT18
#define ITEM_WEAR_4LEGS    BIT19

/* Bitvector for object flags1 */
#define OBJ1_GLOW         BIT0
#define OBJ1_HUM          BIT1
#define OBJ1_DARK         BIT2
#define OBJ1_OMAX         BIT3
#define OBJ1_EVIL         BIT4
#define OBJ1_INVISIBLE    BIT5
#define OBJ1_MAGIC        BIT6
#define OBJ1_CURSED       BIT7
#define OBJ1_BLESS        BIT8
#define OBJ1_ANTI_GOOD    BIT9
#define OBJ1_ANTI_EVIL    BIT10
#define OBJ1_ANTI_NEUTRAL BIT11
#define OBJ1_ANTI_MAGE    BIT12
#define OBJ1_ANTI_THIEF   BIT13
#define OBJ1_ANTI_CLERIC  BIT14
#define OBJ1_ANTI_WARRIOR BIT15
#define OBJ1_ANTI_DRAGON  BIT16
#define OBJ1_GOOD         BIT17
#define OBJ1_NORENT       BIT18
#define OBJ1_MINLVL10     BIT19
#define OBJ1_MINLVL20     BIT20
#define OBJ1_MINLVL30     BIT21
#define OBJ1_MINLVL41     BIT22
#define OBJ1_NO_SEE       BIT23
#define OBJ1_SMALL_ONLY   BIT24
#define OBJ1_LARGE_ONLY   BIT25
#define OBJ1_NORMAL_ONLY  BIT26
#define OBJ1_NO_LOCATE    BIT27
#define OBJ1_NO_SUMMON    BIT28
#define OBJ1_PERSONAL     BIT29
#define OBJ1_QUEST_ITEM   BIT30

/* Bitvector for object flags2 */
#define OBJ2_LOCKED					BIT0
#define OBJ2_VEHICLE_REV			BIT1
#define OBJ2_NO_IDENTIFY			BIT2
#define OBJ2_MINLVL15				BIT3
#define OBJ2_MINLVL25				BIT4
#define OBJ2_MINLVL35				BIT5
#define OBJ2_IMMONLY				BIT6
#define OBJ2_ANTI_DRUID				BIT7
#define OBJ2_ANTI_RANGER			BIT8
#define OBJ2_NODROP					BIT9
#define OBJ2_LORE					BIT10
#define OBJ2_DUALWIELD				BIT11
#define OBJ2_ANTI_PALADIN			BIT12
#define OBJ2_ANTI_PRIEST			BIT13
#define OBJ2_ANTI_BARD				BIT14
#define OBJ2_NO_EXTEND				BIT15
#define OBJ2_MINLVL42    			BIT16
#define OBJ2_ANTI_ELDRITCH	BIT17
#define OBJ2_ANTI_MONK				BIT18


/* Some different kinds of weapons */
#define WEAPON_POISONOUS           BIT0
#define WEAPON_BLIND               BIT1
#define WEAPON_PARALYZE            BIT2
#define WEAPON_SILVER              BIT3
#define WEAPON_WOOD_STAKE          BIT4
#define WEAPON_VAMPIRIC            BIT5
#define WEAPON_DRAIN_MANA          BIT6
#define WEAPON_GOOD                BIT7
#define WEAPON_NEUTRAL             BIT8
#define WEAPON_EVIL                BIT9
#define WEAPON_GOOD_SLAYER         BIT10
#define WEAPON_EVIL_SLAYER         BIT11
#define WEAPON_UNDEAD_SLAYER       BIT12
#define WEAPON_DRAGON_SLAYER       BIT13
#define WEAPON_ANIMAL_SLAYER       BIT14
#define WEAPON_GIANT_SLAYER        BIT15
#define WEAPON_FLAME_ATTACK        BIT16
#define WEAPON_ICE_ATTACK          BIT17
#define WEAPON_ELEC_ATTACK         BIT18
#define WEAPON_SILENCE             BIT19
#define WEAPON_MAGIC_ATTACK        BIT20
#define WEAPON_ACID_ATTACK         BIT21
#define WEAPON_DEMON_SLAYER        BIT22
#define WEAPON_DARKMAGIC_ATTACK    BIT23
#define WEAPON_PULSE_ATTACK        BIT24

/* Anti_class flags for objects */

#define OVAL_ANTI_MAGE           	BIT0
#define OVAL_ANTI_CLERIC         	BIT1
#define OVAL_ANTI_THIEF          	BIT2
#define OVAL_ANTI_WARRIOR        	BIT3
#define OVAL_ANTI_BARD           	BIT4
#define OVAL_ANTI_PRIEST         	BIT5
#define OVAL_ANTI_PALADIN        	BIT6
#define OVAL_ANTI_DRUID          	BIT7
#define OVAL_ANTI_RANGER         	BIT8
#define OVAL_ANTI_ELDRITCH    BIT9
#define OVAL_ANTI_MONK	         	BIT10

/* Some different kind of liquids */

#define LIQ_WATER      0
#define LIQ_BEER       1
#define LIQ_WINE       2
#define LIQ_ALE        3
#define LIQ_DARKALE    4
#define LIQ_WHISKY     5
#define LIQ_LEMONADE   6
#define LIQ_FIREBRT    7
#define LIQ_LOCALSPC   8
#define LIQ_SLIME      9
#define LIQ_MILK       10
#define LIQ_TEA        11
#define LIQ_COFFE      12
#define LIQ_BLOOD      13
#define LIQ_SALTWATER  14
#define LIQ_PEPPER     15
/* for containers  - value[1] */

#define CONT_CLOSEABLE      BIT0
#define CONT_PICKPROOF      BIT1
#define CONT_CLOSED         BIT2
#define CONT_LOCKED         BIT3

/* top xx lists */
#define top_avats       1
#define top_mortals     2
#define top_kills		3
#define top_deaths		4
#define top_scores      5

/* size of top xx lists */
#define size_top_avats    20
#define size_top_mortals  20
#define size_top_kills	  20
#define size_top_deaths	  20
#define size_top_scores   20

struct questsys_reg {
	int value;
	int rettype;

	char *name;
};

struct questsys_data {
	struct char_data *ch;	/* Quest owning mob */
	struct obj_data *obj;	/* Quest owning object */
	struct room_data *room;	/* Quest owning room */
	int sysregs[MAX_QUEST_SYSREGS];	/* System registers */
	int sysregs_types[MAX_QUEST_SYSREGS];
	int regs[MAX_QUEST_REGS];	/* User registers */
	int regs_types[MAX_QUEST_REGS];

	int userregs;

	struct questsys_reg *userreg;
	struct questsys_script *script;	/* scripts attached */
};

struct questsys_script {
	int mob_vnum;
	char *description;
	char *triggers[MAX_QUEST_TRIGGERS];	/* Trigger scripts */
};

struct top_stats {
	char name[20];
	INT32 score;
} topx_avatars[size_top_avats + 5],
  topx_mortals[size_top_mortals + 5],
  topx_kills[size_top_kills + 5],
  topx_deaths[size_top_deaths + 5],
  topx_scores[size_top_scores + 5];


struct extra_descr_data {
	char *keyword;		/* Keyword in look/examine       */
	char *description;	/* What to see                   */
	struct extra_descr_data *next;	/* Next in list                  */
};
#define MAX_OBJ_AFFECT 4	/* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
#define OBJ_NOTIMER    0

struct clan_war_data {
	long enemy_clan;
	long start_time;
	long attack_time;
	long kill_time;
	long attacks;
	long kills;
	long deaths;
	long escapes;
	struct clan_war_data *next;
};

struct clans_data {
	int number;
	sbyte enabled;		/* If 0 clan is disabled 		 */
	char name[MAX_CLAN_STR_LENGTH];	/* Clan name			 */
	char leader[20];	/* Name of the clan leader		 */
	int room;		/* Clan hall start room #		 */
	int don_room;		/* Clan donation room #		 */
	int board_num;		/* Clan Board vnum			 */
	char color[20];		/* Clan color			 */
	char rank[MAX_CLAN_RANKS + 1][MAX_CLAN_STR_LENGTH];	/* Clan ranks			 */
	char desc[MAX_CLAN_DESC][MAX_CLAN_STR_LENGTH];	/* Clan description */
	struct clan_war_data *war;
};


struct obj_flag_data {
	int value[4];		/* Values of the item (see list)    */
	sbyte type_flag;	/* Type of item                     */
	int wear_flags;		/* Where you can wear it            */
	int flags1;		/* If it hums,glows etc             */
	int flags2;		/* */
	int weight;		/* Weight what else                 */
	int cost;		/* Value when sold (gp.)            */
	int cost_per_day;	/* Cost to keep pr. real day        */
	int timer;		/* Timer for object                 */
	long bitvector;		/* To set chars bits                */
};
/* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
struct obj_affected_type {
	sbyte location;		/* Which ability to change (APPLY_XXX) */
	sbyte modifier;		/* How much it changes by              */
};
/* ======================== Structure for object ========================= */
struct obj_data {
	sh_int item_number;	/* Where in data-base               */
	sh_int in_room;		/* In what room -1 when conta/carr  */
	struct obj_flag_data obj_flags;	/* Object information               */
	struct obj_affected_type
	  affected[MAX_OBJ_AFFECT];	/* Which abilities in PC to change  */

	char *name;		/* Title of object :get etc.        */
	char *description;	/* When in room                     */
	char *short_description;/* when worn/carry/in cont.         */
	char *action_description;	/* What to write when used          */
	struct extra_descr_data *ex_description;	/* extra descriptions     */
	struct char_data *carried_by;	/* Carried by :NULL in room/conta   */

	struct obj_data *in_obj;/* In what object NULL when none    */
	struct obj_data *contains;	/* Contains objects                 */

	struct obj_data *next_content;	/* For 'contains' lists             */
	struct obj_data *next;	/* For the object list              */

	struct questsys_data *questsys;	/* quest system */

};
/* ======================================================================= */

struct vehicle_data {
	struct obj_data *obj;	/* Which object */
	sh_int room;		/* room used for vehicle */
	struct vehicle_data *next;	/* For the object list */
};
/* The following defs are for room_data  */

#define NOWHERE    -1		/* nil reference for room-database    */

/* Bitvector For 'room_flags1' */
#define RM1_DARK            BIT0
#define RM1_DEATH           BIT1
#define RM1_NO_MOB          BIT2
#define RM1_INDOORS         BIT3
#define RM1_ANTI_GOOD       BIT4
#define RM1_ANTI_NEUTRAL    BIT5
#define RM1_ANTI_EVIL       BIT6
#define RM1_NO_MAGIC        BIT7
#define RM1_TUNNEL          BIT8
#define RM1_PRIVATE         BIT9
#define RM1_DRAIN_MAGIC     BIT10
#define RM1_HARMFULL1       BIT11
#define RM1_HARMFULL2       BIT12
#define RM1_unused13        BIT13
#define RM1_NOSTEAL         BIT14
#define RM1_NOPKILL         BIT15
#define RM1_NOMKILL         BIT16
#define RM1_WATER           BIT17
#define RM1_SOUNDPROOF      BIT18
#define RM1_DUMP            BIT19
#define RM1_BFS_MARK        BIT20
#define RM1_CANT_HUNT       BIT21
#define RM1_MAZE            BIT22
#define RM1_ANTI_MAGIC      BIT23
#define RM1_INN             BIT24
#define RM1_MYST            BIT25
#define RM1_FALL_DMG        BIT26
#define RM1_unused127       BIT27
#define RM1_NO_TELEPORT_IN  BIT28
#define RM1_NO_TELEPORT_OUT BIT29
#define RM1_NO_SUMMON_OUT   BIT30

/* Bitvector For 'room_flags2' */
#define RM2_DAMAGE            BIT0
#define RM2_TROPICAL          BIT1
#define RM2_ARCTIC            BIT2
#define RM2_DESERT            BIT3
#define RM2_FLOW_NORTH        BIT4
#define RM2_FLOW_EAST         BIT5
#define RM2_FLOW_SOUTH        BIT6
#define RM2_FLOW_WEST         BIT7
#define RM2_FLOW_UP           BIT8
#define RM2_FLOW_DOWN         BIT9
#define RM2_ROUTE_NORTH       BIT10
#define RM2_ROUTE_EAST        BIT11
#define RM2_ROUTE_SOUTH       BIT12
#define RM2_ROUTE_WEST        BIT13
#define RM2_ROUTE_UP          BIT14
#define RM2_ROUTE_DOWN        BIT15
#define RM2_ROUTE_STATION     BIT16
#define RM2_ROUTE_REVERSE     BIT17
#define RM2_ROUTE_DEATH       BIT18
#define RM2_ROUTE_SLOW        BIT19
#define RM2_ARENA             BIT20
#define RM2_unused221         BIT21
#define RM2_unused222         BIT22
#define RM2_unused223         BIT23
#define RM2_unused224         BIT24
#define RM2_unused225         BIT25
#define RM2_unused226         BIT26
#define RM2_unused227         BIT27
#define RM2_NO_QUIT           BIT28
#define RM2_NO_SUMMON_IN      BIT29
#define RM2_NO_DONATE         BIT30

/* zone flags */
#define ZONE_TESTING      BIT0
#define ZONE_NO_ENTER     BIT1
#define ZONE_NO_GOTO      BIT2
#define ZONE_NEW_ZONE     BIT3
#define ZONE_undefined04  BIT4
#define ZONE_undefined05  BIT5
#define ZONE_undefined06  BIT6
#define ZONE_undefined07  BIT7
#define ZONE_undefined08  BIT8
#define ZONE_undefined09  BIT9
#define ZONE_undefined10  BIT10
#define ZONE_undefined11  BIT11
#define ZONE_undefined12  BIT12
#define ZONE_undefined13  BIT13
#define ZONE_undefined14  BIT14
#define ZONE_undefined15  BIT15
#define ZONE_undefined16  BIT16
#define ZONE_undefined17  BIT17
#define ZONE_undefined18  BIT18
#define ZONE_undefined19  BIT19
#define ZONE_undefined20  BIT20
#define ZONE_undefined21  BIT21
#define ZONE_undefined22  BIT22
#define ZONE_undefined23  BIT23
#define ZONE_undefined24  BIT24
#define ZONE_undefined25  BIT25
#define ZONE_undefined26  BIT26
#define ZONE_undefined27  BIT27
#define ZONE_undefined28  BIT28
#define ZONE_undefined29  BIT29
#define ZONE_undefined30  BIT30

/* For 'dir_option' */

#define NORTH          0
#define EAST           1
#define SOUTH          2
#define WEST           3
#define UP             4
#define DOWN           5

/* These only work up to 15, because it's a sh_int. */
#define EX_ISDOOR				BIT0
#define EX_PICKPROOF			BIT1
#define EX_LOCKED				BIT2
#define EX_CLOSED				BIT3
#define EX_HIDDEN				BIT4
#define EX_NOMOB				BIT5
#define EX_NOCHAR				BIT6
#define EX_CLIMB_ONLY			BIT7
#define EX_NO_MOBS_IN_ROOM		BIT8
#define EX_PHASEPROOF			BIT9
#define EX_BASHPROOF			BIT10
#define EX_TRIPPROOF			BIT11

/* For 'Sector types' */

#define SECT_INSIDE          0
#define SECT_CITY            1
#define SECT_FIELD           2
#define SECT_FOREST          3
#define SECT_HILLS           4
#define SECT_MOUNTAIN        5
#define SECT_WATER_SWIM      6
#define SECT_WATER_NOSWIM    7
#define SECT_UNDERWATER      8

struct room_direction_data {
	char *general_description;	/* When look DIR.                  */
	char *keyword;		/* for open/close                  */
	sh_int exit_info;	/* Exit info                       */
	sh_int key;		/* Key's number (-1 for no key)    */
	sh_int to_room;		/* Where direction leeds (NOWHERE) */
};
/* ========================= Structure for room ========================== */
struct room_data {
	sh_int number;		/* Rooms number                       */
	sh_int zone;		/* Room zone (for resetting)          */
	int sector_type;	/* sector type (move/hide)            */
	char *name;		/* Rooms name 'You are ...'           */
	char *description;	/* Shown when entered                 */
	struct extra_descr_data *ex_description;	/* for examine/look    */
	struct room_direction_data *dir_option[6];	/* Directions        */
	unsigned long room_flags;	/* DEATH,DARK ... etc             */
	unsigned long room_flags2;	/* DEATH,DARK ... etc             */
	sbyte light;		/* Number of lightsources in room */
	sbyte min_level;	/* min level to enter room        */
	sbyte max_level;	/* max level to enter room        */
	int (*funct) ();	/* special procedure              */
	ush_int next;		/* used when zone is dirty        */
	struct obj_data *contents;	/* List of items in room           */
	struct char_data *people;	/* List of NPC / PC in room        */
	struct questsys_data *questsys;	/* quest data */

};
/* ================================================================== */

/* The following defs and structures are related to char_data   */
/* For 'equipment' */
#define WEAR_LIGHT      0
#define WEAR_FINGER_R   1
#define WEAR_FINGER_L   2
#define WEAR_NECK_1     3
#define WEAR_NECK_2     4
#define WEAR_BODY       5
#define WEAR_HEAD       6
#define WEAR_LEGS       7
#define WEAR_FEET       8
#define WEAR_HANDS      9
#define WEAR_ARMS      10
#define WEAR_SHIELD    11
#define WEAR_ABOUT     12
#define WEAR_WAISTE    13
#define WEAR_WRIST_R   14
#define WEAR_WRIST_L   15
#define WIELD          16
#define HOLD           17
#define WEAR_TAIL      18
#define WEAR_4LEGS_1   19
#define WEAR_4LEGS_2   20

//Make sure to update this ! !!!!Should be the last wear slot + 1
#define MAX_WEAR_SLOTS       21

/* For 'char_payer_data' */

#define MAX_TOUNGE      3	/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
/* abc We gotta find af-type and changed to an unsignged byte */
#define MAX_SKILLS    250	/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_SAVES       5	/* Number of saving throws */
#define MAX_CONDITIONS  3	/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_ALIASES    21	/* maximum alias commands */
#define MAX_ALIAS_CMD   7
#define MAX_ALIAS_ACT  50
#define MAX_WEAR       21
#define MAX_AFFECT     128	/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */

/* Predifined  conditions */
#define DRUNK        0
#define FOOD         1
#define THIRST       2

/* Bitvector for 'affected_by' */
#define AFF_BLIND             BIT0
#define AFF_INVISIBLE         BIT1
#define AFF_DETECT_EVIL       BIT2
#define AFF_DETECT_INVISIBLE  BIT3
#define AFF_DETECT_MAGIC      BIT4
#define AFF_SENSE_LIFE        BIT5
#define AFF_HASTE             BIT6
#define AFF_SANCTUARY         BIT7
#define AFF_GROUP             BIT8
#define AFF_WATERWALK         BIT9
#define AFF_CURSE             BIT10
#define AFF_SILENCE           BIT11
#define AFF_POISON            BIT12
#define AFF_PROTECT_EVIL      BIT13
#define AFF_FLY               BIT14
#define AFF_RAGE              BIT15
#define AFF_HOLD_PERSON       BIT16
#define AFF_SLEEP             BIT17
#define AFF_BLOCK_SUMMON      BIT18
#define AFF_SNEAK             BIT19
#define AFF_HIDE              BIT20
#define AFF_DEMON_FLESH       BIT21
#define AFF_CHARM             BIT22
#define AFF_FOLLOW            BIT23
#define AFF_BREATHWATER       BIT24
#define AFF_DARKSIGHT         BIT25
#define AFF_REGENERATION      BIT26
#define AFF_TRANSFORMED       BIT27
#define AFF_HEMMORAGE         BIT28
#define AFF_MAGIC_RESIST      BIT29
#define AFF_MAGIC_IMMUNE      BIT30

/* bitvectors for 'affected_02' These don't work */

#define APPLY_SANCTUARY_MEDIUM  BIT0
#define APPLY_SANCTUARY_MINOR   BIT1
#define APPLY_SLEEP_IMMUNITY    BIT2
#define APPLY_BARKSKIN          BIT3
#define APPLY_STONESKIN         BIT4
#define APPLY_FAERIE_FIRE       BIT5
#define APPLY_FAERIE_FOG        BIT6

/*#define AFF2_DETECT_UNDEAD     BIT5
#define AFF2_DETECT_ANIMALS    BIT8
#define AFF2_DETECT_DRAGONS    BIT9
#define AFF2_DETECT_DEMONS     BIT10
#define AFF2_DETECT_GIANTS     BIT11
#define AFF2_DETECT_SHADOWS    BIT12
#define AFF2_DETECT_VAMPIRES   BIT13
*/


/* modifiers to char's abilities */

#define APPLY_NONE              0
#define APPLY_STR               1
#define APPLY_DEX               2
#define APPLY_INT               3
#define APPLY_WIS               4
#define APPLY_CON               5
#define APPLY_CHA               6
#define APPLY_CLASS             7
#define APPLY_LEVEL             8
#define APPLY_AGE               9
#define APPLY_CHAR_WEIGHT      10
#define APPLY_CHAR_HEIGHT      11
#define APPLY_MANA             12
#define APPLY_HIT              13
#define APPLY_MOVE             14
#define APPLY_GOLD             15
#define APPLY_EXP              16
#define APPLY_AC               17
#define APPLY_ARMOR            17
#define APPLY_HITROLL          18
#define APPLY_DAMROLL          19
#define APPLY_SAVING_PARA      20
#define APPLY_SAVING_ROD       21
#define APPLY_SAVING_PETRI     22
#define APPLY_SAVING_BREATH    23
#define APPLY_SAVING_SPELL     24
#define APPLY_EXTRA_ATTACKS    25
#define APPLY_DETECT_INVIS     26
#define APPLY_DARKSIGHT        27
#define APPLY_MAGIC_RESIST     28
#define APPLY_MAGIC_IMMUNE     29
#define APPLY_BREATHWATER      30
#define APPLY_DETECT_MAGIC     31
#define APPLY_SENSE_LIFE       32
#define APPLY_DETECT_EVIL      33
#define APPLY_SNEAK            34
#define APPLY_INVIS            35
#define APPLY_IMPROVED_INVIS   36
#define APPLY_REGENERATION     37
#define APPLY_HOLD_PERSON      38
#define APPLY_HASTE            39
#define APPLY_BONUS_STR        40
#define APPLY_BONUS_DEX        41
#define APPLY_BONUS_CHA        42
#define APPLY_DETECT_GOOD      43
#define APPLY_SLEEP_IMMUNITY   44
#define APPLY_DETECT_UNDEAD    45
#define APPLY_DETECT_ANIMALS   46
#define APPLY_DETECT_DRAGONS   47
#define APPLY_DETECT_DEMONS    48
#define APPLY_DETECT_GIANTS    49
#define APPLY_DETECT_SHADOWS   50
#define APPLY_DETECT_VAMPIRES  51
#define APPLY_last_apply       52	/* KEEP ADJUSTING THE last_apply */

/* race flags */
#define RFLAG_GOOD_ALIGNMENT     BIT0
#define RFLAG_EVIL_ALIGNMENT     BIT1
#define RFLAG_SIZE_SMALL         BIT2
#define RFLAG_SIZE_LARGE         BIT3
#define RFLAG_SIZE_NORMAL        BIT4
#define RFLAG_five               BIT5
#define RFLAG_six                BIT6
#define RFLAG_seven              BIT7

/* class flags */
#define CFLAG_GOOD_ALIGNMENT     		BIT0
#define CFLAG_EVIL_ALIGNMENT     		BIT1
#define CFLAG_2                  		BIT2
#define CFLAG_CLASS_MAGE         		BIT3
#define CFLAG_CLASS_CLERIC       		BIT4
#define CFLAG_CLASS_FIGHTER      		BIT5
#define CFLAG_CLASS_THIEF        		BIT6
#define CFLAG_CLASS_PRIEST       		BIT7
#define CFLAG_CLASS_PALADIN      		BIT8
#define CFLAG_CLASS_BARD        		 BIT9
#define CFLAG_CLASS_DRUID        		BIT10
#define CFLAG_CLASS_RANGER       		BIT11
#define CFLAG_CLASS_ELDRITCH      BIT12
#define CFLAG_CLASS_MONK	     		BIT13



/* races for PC's */
#define RACE_HUMAN        1
#define RACE_DWARF        2
#define RACE_GNOME        3
#define RACE_HALFLING     4
#define RACE_HALF_GIANT   5
#define RACE_PIXIE        6
#define RACE_DEMON        7	/* HAS TO STAY EVIL */
#define RACE_SNOTLING     8
#define RACE_FELINE       9
#define RACE_TROLL       10
#define RACE_DRAGONKIN   11
#define RACE_CENTAUR     12
#define RACE_MERMAN      13
#define RACE_ELF_HIGH    14	/* HAS TO STAY GOOD */
#define RACE_ELF_HALF    15
#define RACE_ELF_DROW    16	/* HAS TO STAY EVIL */
#define RACE_ORC         17
#define RACE_ORC_HALF    18
#define RACE_DARKLING    19
#define RACE_OGRE        20
#define RACE_GOBLIN      21
#define RACE_MINOTAUR    22
#define RACE_BUGBEAR     23
#define RACE_GIANT		 24
#define RACE_RAVSHI      25 /* HAS TO STAY EVIL*/
#define RACE_IRDA        26 /* HAS TO STAY GOOD */
#define RACE_THRI_KREEN  27
#define RACE_BULLYWUG    28
#define RACE_GARGOYLE    29
#define RACE_IMP         30
#define RACE_SAURIAN     31
#define RACE_LIZARDMAN   32
#define RACE_SAHAUGIN    33
#define RACE_SATYR       34
#define RACE_YUANTI      35
#define RACE_WEMIC       36

#define MAX_RACES        37	/* WARNING:  You have to have a file for each
				 * one of these.  The 0 FILE is used as an init
				 * file.  Races start at 1. */
#define MAX_RACE_ATTRIBUTES 6

struct race_data {
	char name[20];
	char desc[6];
	char text[4][80];
	int flag;

	sbyte max_str;
	sbyte max_int;
	sbyte max_wis;
	sbyte max_dex;
	sbyte max_con;
	sbyte max_cha;
	int max_hit;
	int max_mana;
	int max_move;

	ubyte max_food;
	ubyte max_thirst;
	ubyte max_drunk;

	int base_hit;
	int base_mana;
	int base_move;

	sbyte adj_str;
	sbyte adj_int;
	sbyte adj_wis;
	sbyte adj_dex;
	sbyte adj_con;
	sbyte adj_cha;
	int adj_hit;
	int adj_mana;
	int adj_move;
	sbyte adj_hitroll;
	sbyte adj_dmgroll;
	int adj_ac;
	int adj_food;
	int adj_thirst;
	int adj_drunk;

	int regen_hit;
	int regen_mana;
	int regen_move;

	//sbyte perm_spell[MAX_RACE_ATTRIBUTES];
	ubyte perm_spell[MAX_RACE_ATTRIBUTES];
	sbyte skill_min_level[MAX_SKILLS];
	sbyte skill_max[MAX_SKILLS];

} races[MAX_RACES];
/* 'class' for PC's */
#define CLASS_MAGIC_USER  		1
#define CLASS_CLERIC      		2
#define CLASS_THIEF       		3
#define CLASS_WARRIOR     		4
#define CLASS_BARD        		5
#define CLASS_PRIEST      		6
#define CLASS_PALADIN     		7
#define CLASS_DRUID       		8
#define CLASS_ELDRITCH	9
#define CLASS_MONK       		10
#define CLASS_RANGER      		13
#define CLASS_NECROMANCER 		15
#define CLASS_CHAOSKNIGHT 		20

#define MAX_CLASSES      20	/* WARNING:  You have to have a file for each
				 * one of these.  The 0 FILE is used as an init
				 * file.  Classes start at 1. */

struct class_data {
	char name[20];
	char desc[5];
	char text[4][80];
	ubyte thaco_numerator;
	ubyte thaco_denominator;
	int flag;
	sbyte skill_min_level[MAX_SKILLS];
	sbyte skill_max[MAX_SKILLS];
	ubyte adj_hit;
	ubyte adj_mana;
	ubyte adj_move;
} classes[MAX_CLASSES];
/* sex */
#define SEX_NEUTRAL   0
#define SEX_MALE      1
#define SEX_FEMALE    2

/* positions */
#define POSITION_DEAD       0
#define POSITION_MORTALLYW  1
#define POSITION_INCAP      2
#define POSITION_STUNNED    3
#define POSITION_SLEEPING   4
#define POSITION_RESTING    5
#define POSITION_SITTING    6
#define POSITION_FIGHTING   7
#define POSITION_STANDING   8

/* THESE ARE USED IN PLAYER FILE - REBUILD TO CHANGE */
/* HOW WE LEFT THE GAME */
#define LEFT_BY_UNKNOWN          0
#define LEFT_BY_CRASH            1
#define LEFT_BY_RENT_WITH_ITEMS  2
#define LEFT_BY_COMMAND_QUIT     3
#define LEFT_BY_PURGE            4
#define LEFT_BY_LINK_NORENT      5
#define LEFT_BY_LINK_RENT        6
#define LEFT_BY_LINK_NO_$4_RENT  7
#define LEFT_BY_DEATH_ZTELEPORT  8
#define LEFT_BY_DEATH_ROOM       9
#define LEFT_BY_DEATH_AGE       10
#define LEFT_BY_DEATH_FIGHT     11
#define LEFT_BY_LINK_DEATH      12
#define LEFT_BY_SHUTDOWN        13
#define LEFT_BY_RENT_NO_ITEMS   14
#define LEFT_BY_LIGHTNING_DEATH 15
#define LEFT_BY_DEATH_TELEPORT  16
#define LEFT_BY_DEATH_GERINS    17
#define LEFT_BY_DEATH_VEHICLE   18

/* for mobile/player actions: specials.act */
//
#define unused10                BIT0
#define PLR1_UNFILTER           BIT0
#define PLR1_SENTINEL           BIT1
#define PLR1_SCAVENGER          BIT2
#define PLR1_ISNPC              BIT3
#define PLR1_CLASS_DEMON        BIT4
#define PLR1_CHAR_WAS_HELD      BIT5	/* HOLD PERSON WAS CAST */
#define PLR1_STAY_ZONE          BIT6
#define PLR1_WIMPY              BIT7
#define PLR1_SIGNAL_HUNT_HELP   BIT8
#define PLR1_GUARD1             BIT9
#define PLR1_GUARD2             BIT10
#define PLR1_NOSTEAL            BIT11
#define PLR1_INJURED_ATTACK     BIT12
#define PLR1_AGGRESSIVE_EVIL    BIT13
#define PLR1_AGGRESSIVE_NEUT    BIT14
#define PLR1_AGGRESSIVE_GOOD    BIT15
#define PLR1_POISONOUS          BIT16
#define PLR1_SHOW_DIAG          BIT17
#define PLR1_NOCHARM            BIT18
#define PLR1_DRAIN_XP           BIT19
#define PLR1_CLASS_SHADOW       BIT20
#define PLR1_CLASS_VAMPIRE      BIT21
#define PLR1_CLASS_DRAGON       BIT22
#define PLR1_CLASS_GIANT        BIT23
#define PLR1_CLASS_ANIMAL       BIT24
#define PLR1_CLASS_UNDEAD       BIT25
#define PLR1_CANT_HUNT          BIT26
#define PLR1_HUNTASSIST         BIT27
#define PLR1_NOFOLLOW           BIT28
#define PLR1_NOKILL             BIT29
#define PLR1_NOHOLD             BIT30

/* for mobile/player actions: specials.act2 */
#define PLR2_SHOW_NAME         BIT0
#define PLR2_FREEZE            BIT1
#define PLR2_NO_OPPONENT_FMSG  BIT2
#define PLR2_NO_PERSONAL_FMSG  BIT3
#define PLR2_NO_BYSTANDER_FMSG BIT4
#define PLR2_NO_EMOTE          BIT5
#define PLR2_JAILED            BIT6
#define PLR2_QUEST             BIT7
#define PLR2_MOUNT             BIT8
#define PLR2_MUZZLE_ALL        BIT9
#define PLR2_MUZZLE_SHOUT      BIT10
#define PLR2_MUZZLE_BEEP       BIT11
#define PLR2_KICKME            BIT12
#define PLR2_PKILLABLE         BIT13
#define PLR2_AFK               BIT14
#define PLR2_MOB_AI            BIT15
#define PLR2_CLOSE_DOORS       BIT16
#define PLR2_BACKSTAB          BIT17
#define PLR2_EQTHIEF           BIT18
#define PLR2_OPEN_DOORS        BIT19
#define PLR2_USES_EQ           BIT20
#define PLR2_JUNKS_EQ          BIT21
#define PLR2_NO_LOCATE         BIT22
#define PLR2_ATTACKER          BIT23
#define PLR2_DEATH_PROCEDURE   BIT24
#define PLR2_NOSTAFF           BIT25
#define PLR2_NOBOSS            BIT26
#define PLR2_NEWGRAPHICS       BIT27
#define PLR2_SHOWNOEXP	       BIT28
#define PLR2_AFW               BIT29
#define PLR2_SHOW_TICK         BIT30

/* for mobile/player actions: specials.act3 */
#define PLR3_BRIEF        BIT0
#define PLR3_NOSHOUT      BIT1
#define PLR3_COMPACT      BIT2
#define PLR3_SHOW_VEHICLE BIT3
#define PLR3_ANSI         BIT4
#define PLR3_NOSUMMON     BIT5
#define PLR3_NOGOSSIP     BIT6
#define PLR3_NOAUCTION    BIT7
#define PLR3_NOINFO       BIT8
#define PLR3_NOCLAN       BIT9
#define PLR3_ENFORCER     BIT10
#define PLR3_NOTELL       BIT11
#define PLR3_ASSIST       BIT12
#define PLR3_AUTOSPLIT    BIT13
#define PLR3_NOPKFLAME    BIT14
#define PLR3_WIMPY        BIT15
#define PLR3_SHOW_ROOM    BIT16
#define PLR3_SHOW_HP      BIT17
#define PLR3_SHOW_MANA    BIT18
#define PLR3_SHOW_MOVE    BIT19
#define PLR3_SHOW_EXITS   BIT20
#define PLR3_NOHASSLE     BIT21
#define PLR3_NOWIZINFO    BIT22
#define PLR3_NOSYSTEM     BIT23
#define PLR3_NOIMM        BIT24
#define PLR3_AUTOGOLD     BIT25
#define PLR3_AUTOLOOT     BIT26
#define PLR3_AUTOAGGR     BIT27
#define PLR3_NOMUSIC      BIT28
#define PLR3_NOAVATAR     BIT29
#define PLR3_NOCONNECT    BIT30

/* for mobile/player actions: specials.act4 */
#define PLR4_MLAND        BIT0	/* Can travel over land */
#define PLR4_MAIR         BIT1	/* Can travel through air */
#define PLR4_MWATER       BIT2	/* Can travel over water */
#define PLR4_TAMED        BIT3	/* Mount is tamed */
#define PLR4_FQUEST1      BIT4	/* Fyrebrand quest flags FQUEST1-7 */
#define PLR4_FQUEST2      BIT5
#define PLR4_FQUEST3      BIT6
#define PLR4_FQUEST4      BIT7
#define PLR4_FQUEST5      BIT8
#define PLR4_FQUEST6      BIT9
#define PLR4_FQUEST7      BIT10
#define PLR4_NOSPIRIT     BIT11


/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data {
	sbyte hours, day, month;
	sh_int year;
};
/* These data contain information about a players time data */
struct time_data {
	signed long birth;	/* This represents the characters age         */
	time_t logon;		/* Time of the last logon (used to calculate
				 * played) */
	int lastpkill;		/* The amount of mud time since this player
				 * last killed */
	int played;		/* This is the total accumulated time played in
				 * secs */
};
/* New alias type-struct --Relic */
struct alias_data {
	char *alias;
	char *replacement;
	int type;
	struct alias_data *next;
};

struct char_player_data {
	char *name;		/* PC / NPC s name (kill ...  )      */
	char *short_descr;	/* for 'actions'                     */
	char *long_descr;	/* for 'look'.. Only here for testing */
	char *description;	/* Extra descriptions                */
	char *title;		/* PC / NPC s title                  */
	sbyte sex;		/* PC / NPC s sex                    */
	sbyte class;		/* PC s class or NPC alignment       */
	sbyte race;		/* PC s race                         */
	sbyte level;		/* PC / NPC s level                  */
	struct time_data time;	/* PC s AGE in days                   */
	ubyte weight;		/* PC / NPC s weight                 */
	ubyte height;		/* PC / NPC s height                 */
	char *immortal_enter;	/* Poofin message                     */
	char *immortal_exit;	/* Poofout message                    */
	struct alias_data *aliases;
	sh_int start_room;	/* Which room to place char in        */
	sh_int private_room;	/* private room                       */
	sbyte visible_level;	/* Level at which chars can see you   */
	time_t logoff_time;
	ubyte how_player_left_game;
	ubyte how_player_left_save;
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_ability_data {
	sbyte str;
	sbyte intel;
	sbyte wis;
	sbyte dex;
	sbyte con;
	sbyte cha;
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_point_data {
	int mana;
	int max_mana;		/* Not useable may be erased upon player file renewal */
	int hit;
	int max_hit;		/* Max hit for NPC                         */
	int move;
	int max_move;		/* Max move for NPC                        */
	sh_int armor;		/* Internal -100..100, external -10..10 AC */
	int gold;		/* Money carried                           */
	int inn_gold;		/* gold in Inn */
	int bank_gold;		/* gold in bank */
	int exp;		/* The experience of the player            */
	sbyte hitroll;		/* Any bonus or penalty to the hit roll    */
	sbyte damroll;		/* Any bonus or penalty to the damage roll */
	int fate;
	int stat_count;		/* number stats bought */
	int stat_gold;		/* temp field showing gold from stats */
	int score;		/* Holds the players score. */
	int used_exp;		/* Amount of exp used for buying stats */
	int fspells;		/* Amount of foreign spells (not in current class) learned */
	int class1;		/* Class the player started with */
	int class2;		/* Class the player remorted to */
	int class3;		/* Second remort class */
	int prompt;
	sbyte extra_hits;
	char filler[31];
};


struct clan_data {
	ubyte number;		/* Thier clan number */
	ubyte rank;		/* thier rank within the clan */
	ubyte flags1;		/* Clan flags */
};

struct char_special_data {
	struct char_data *fighting;	/* Opponent                          */
	struct char_data *hunting;	/* Hunting person..                  */
	struct char_data *ridden_by;	/* Who is  riding me? */
	struct char_data *mount;/* Who am I riding?  */
	struct clan_data clan;	/* Clan information		      */
	long affected_by;	/* Bitvector: spells/skills affected */
	long affected_02;	/* Bitvector: spells/skills affected */
	long affected_03;	/* more affected bitvectors! woo hoo (Deus ;-) */
	long affected_04;	/* and more still!?!? */
	sbyte position;		/* Standing or ...                     */
	sbyte default_pos;	/* Default position for NPC            */
	long act;		/* Player flags                        */
	long act2;		/* Player flags 2                      */
	long act3;		/* Player flags 3                      */
	long act4;		/* Player flags 4                      */
	long act5;		/* Player flags 5		      */
	long act6;		/* Player flags 6		      */
	long wiz_perm;  /* Wiz command permissions -- Relic */
	long jailtime;		/* Time in jail left		      */
	long deathshadowtime;	/* Time left for DeathShadow           */
	long godmuzzletime;	/* Time left for GodMuzzle	      */
	ubyte spells_to_learn;	/* How many can you learn this level   */
	int carry_weight;	/* Carried weight                      */
	sbyte carry_items;	/* Number of items carried             */
	int timer;		/* Timer for update                    */
	sh_int was_in_room;	/* storage of location for linkdead people */
	sh_int apply_saving_throw[5];	/* Saving throw (Bonuses)          */
	signed short int conditions[3];
	/* Drunk full etc.                     */
	sbyte damnodice;	/* The number of damage dice's         */
	sbyte damsizedice;	/* The size of the damage dice's       */
	sbyte last_direction;	/* The last direction the monster went */
	int attack_type;	/* The Attack Type Bitvector for NPC's */
	int alignment;		/* +-1000 for alignments               */
	time_t time_of_muzzle;	/* Time automuzzle applied */
};
/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_skill_data {
	ubyte learned;		/* % chance for success 0 = not learned */
};
/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct affected_type {
	ubyte type;		/* The type of spell that caused this      */
	sh_int duration;	/* For how long its effects will last      */
	sbyte modifier;		/* This is added to apropriate ability     */
	sbyte location;		/* Tells which ability to change(APPLY_XXX) */
	long bitvector;		/* Tells which bits to set (AFF_XXX)       */
	long bitvector2;	/* Tells which bits to set (AFF2_XXX)      */
	long bitvector3;	/* AFF3_XXX. */
	long bitvector4;	/* AFF4_XXX. */
	struct affected_type *next;
};

struct follow_type {
	struct char_data *follower;
	struct follow_type *next;
};

struct keyword_data {
	char *keyword;
	struct keyword_data *next;
};

struct conversation_data {
	int number;
	sbyte low;
	sbyte high;
	struct keyword_data *keywords;
	char *reply;
	char *command;
	struct conversation_data *next;
};

struct quest_data {
	int vmob;
	struct conversation_data *convo;
};
/* ================== Structure for player/non-player ===================== */
struct char_data {
	sh_int beg_rec_id;	/* front sanity check       */
	sh_int nr;		/* monster nr (pos in file) */
	sh_int in_room;		/* Location                 */
	sh_int riding;		/* if in room VEHICLE_ROOM then its the   */
	/* object number char is riding.          */
	sh_int extra_mana;	/* Extra max_mana used for certain spells */
	sbyte language;		/* Number of language fauls               */
	struct char_player_data player;	/* Normal data               */
	struct char_ability_data abilities;	/* Abilities               */
	struct char_ability_data tmpabilities;	/* The abilities we'll use */
	struct char_ability_data bonus;
	struct char_point_data points;	/* Points                  */
	struct char_special_data specials;	/* Special play constants  */
	struct char_skill_data skills[MAX_SKILLS];	/* Skills             */
	struct affected_type *affected;	/* affected by what spells */
	struct obj_data *equipment[MAX_WEAR];	/* Equipment array         */
	struct obj_data *carrying;	/* Head of list            */
	struct descriptor_data *desc;	/* NULL for mobiles        */
	struct char_data *next_in_room;	/* For room->people - list   */
	struct char_data *next;	/* For char list             */
	struct char_data *next_fighting;	/* For fighting list         */
	struct follow_type *followers;	/* List of chars followers   */
	struct char_data *master;	/* Who is char following?    */

	int incognito_lvl;	/* Incognito & ghost shalira 22.07.01 */
	int ghost_lvl;		/* incognito & ghost shalira 22.07.01 */
	int blessing;		/* are we giving a godbless? */

	int nextcast;		/* Time before spell can be cast again */
	int castcount;		/* Time remaining before able to cast */
	int questpoints;	/* Number of Quest points    */
	int nextquest;		/* Time before next quest    */
	int countdown;		/* Time left to finish quest */
	int reward;		/* reward for the quest      */
	struct obj_data *questobj;	/* Object needed for quest   */
	struct char_data *questmob;	/* Mob needed for Quest      */
	struct quest_data *questdata;
	struct questsys_data *questsys;

	int jailed;

	sh_int screen_lines;

	sh_int end_rec_id;	/* ending sanity check       */
};


/* ================================================================= */

/* How much light is in the land ? */
#define SUN_DARK    			0
#define SUN_RISE    			1
#define SUN_LIGHT   			2
#define SUN_SET     			3

/* And how is the sky ? */
#define SKY_CLOUDLESS    		0
#define SKY_CLOUDY       		1
#define SKY_RAINING      		2
#define SKY_LIGHTNING    		3

/* lunar phase defines */

#define PHASE_NEW				0
#define PHASE_CRESCENT_1		1
#define PHASE_FIRST_QUARTER		2
#define PHASE_WAXING_GIBBOUS	3
#define PHASE_FULL_MOON			4
#define PHASE_WANING_GIBBOUS	5
#define PHASE_LAST_QUARTER		6
#define PHASE_CRESCENT_2		7

// Maximum lunar phase:
#define MAX_LUNAR_PHASES		7

struct weather_data {
	int pressure;		/* How is the pressure ( Mb ) */
	int change;		/* How fast and what way does it change. */
	int sky;		/* How is the sky. */
	int sunlight;		/* And how much sun. */
	int lunar_phase;	/* Current phase of the moon. */
	int current_hour_in_phase;	/* int counter for the current day in the phase.*/
};


/* ***************************************************************** *
*  file element for player file.
*  BEWARE: Changing it will ruin the file
******************************************************************* */

struct char_file_u {
	sbyte sex;
	sbyte class;
	sbyte race;
	sbyte level;
	time_t birth;		/* Time of birth of character     */
	int played;		/* Number of secs played in total */
	int lastpkill;		/* The time the last pkill was commited. */
	time_t last_logon;	/* Time (in secs) of last logon */
	time_t logoff_time;
	ubyte how_player_left_game;

	ubyte weight;
	ubyte height;
	char title[80];
	char description[MAX_DESC];
	char immortal_enter[80];/* Poofin message                     */
	char immortal_exit[80];	/* Poofout message                    */

	sh_int start_room;	/* Which room to place char in        */
	sh_int private_room;	/* Immort's private room              */
	sbyte visible_level;	/* Level chars must be to see you     */

	struct char_ability_data abilities;
	struct char_point_data points;
	struct char_skill_data skills[MAX_SKILLS];
	struct affected_type affected[MAX_AFFECT];
	struct clan_data clan;	/* The data for clan stuff */
	sh_int apply_saving_throw[5];
	signed short int conditions[3];
	ubyte spells_to_learn;
	int alignment;

	long act;		/* ACT Flags                    */
	long act2;		/* ACT Flags 2                  */
	long act3;		/* ACT Flags 3                  */
	long act4;		/* ACT Flags 4                  */
	long act5;		/* ACT Flags 5 (You know the drill, Deus ;-) */
	long act6;
	long jailtime;

	long deathshadowtime;
	long godmuzzletime;

	int ghost_lvl;
	int incognito_lvl;

	int questpoints;	/* number of questpoints user has */
	int nextquest;		/* time before next quest */

	sh_int screen_lines;	/* Number of lines fitting on a screen */

	/* char data */
	char name[20];
	char pwd[11];

	ubyte filler_one;
	ubyte filler_two;
	ubyte filler_three;

	time_t time_of_muzzle;	/* Time automuzzle applied */
	long descriptor_flag1;
	char email_name[36];
	char filler[200];
	int nextcast;		/* Time before next casting of bestial transformation */

	int jailed;
	/* Some spares, so we don't have to update the players file for a while */
	/* Using long spare6 for the new wiz_permisions.  God I love being forsightful :) */

	int spare1;
	int spare2;
	int spare3;
	int spare4;
	int spare5;
	long wiz_perms;
	long spare7;
	long spare8;
	long spare9;
	long spare10;
	ubyte spare11;
	ubyte spare12;
	ubyte spare13;
	ubyte spare14;
	ubyte spare15;

	char sparestring1[200];
	char sparestring2[200];
	char sparestring3[200];
	char sparestring4[200];
	char sparestring5[200];
};
/* ***************************************************************** *
*  file element for object file.
* BEWARE: Changing it will ruin the file
******************************************************************* */

/* this isnt used anymore I dont think */
#define MAX_OBJ_SAVE 25		/* Used in OBJ_FILE_U *DO*NOT*CHANGE* */

struct obj_cost {
	int total_cost;
	int no_carried;
	bool ok;
};

struct obj_file_elem {
	char name[75];
	char description[150];
	char short_description[80];
	char action_description[50];
	/* Don't believe this is even used. */
	sh_int item_number;
	sh_int item_state;
	int value[4];
	int flags1;
	int flags2;
	int timer;
	long bitvector;
	struct obj_affected_type affected[MAX_OBJ_AFFECT];
};

struct obj_file_u {
	char owner[20];		/* Name of player                     */
	int total_cost;		/* The cost for all items, per day    */
	long last_update;	/* Time in seconds, when last updated */
	int flags;		/* filler                             */
};
/* ***********************************************************
*  The following structures are related to descriptor_data   *
*********************************************************** */

struct txt_block {
	unsigned char beg_rec_id;
	char *text;
	struct txt_block *next;
	int aliased;
	unsigned char end_rec_id;
};

struct txt_q {
	struct txt_block *head;
	struct txt_block *tail;
};
/* modes of connectedness */
#define CON_PLAYING                    0
#define CON_GET_NAME                   1
#define CON_GET_NAME_CONFIRM           2
#define CON_GET_PWD_OLD_PLAYER         3
#define CON_GET_PWD_CHANGE             4
#define CON_GET_PWD_NEW_PLAYER         5
#define CON_GET_PWD_OLD_PLAYER_CONFIRM 6
#define CON_GET_PWD_NEW_PLAYER_CONFIRM 7
#define CON_GET_SEX                    8
#define CON_GET_RACE                   9
#define CON_GET_CLASS                 10
#define CON_GET_RETURN_KEY_MOTD       11
#define CON_GET_MENU_SELECTION        12
#define CON_GET_DESCRIPTION           13
#define CON_LINK_DEAD                 14
#define CON_QUIT                      15
#define CON_CONFIRM_DICE_ROLL         16
#define CON_DELETE_CONFIRM	   		  17
#define CON_ADMIN	 				  18
#define CON_PKILLABLE_CONFIRM	      19


struct snoop_data {
	struct char_data *snooping;
	/* Who is this char snooping */
	struct char_data *snoop_by;
	/* And who is snooping on this char */
};

struct descriptor_data {
	int descriptor;		/* file descriptor for socket */
	char host[50];		/* hostname                   */
	char pwd[12];		/* password                   */
	int pos;		/* position in player-file    */
	int connected;		/* mode of 'connectedness'    */
	int connected_at;	/* time descriptor connected */
	int admin_state;	/* state of edit modes 	 */
	int old_state;		/* Previous edit mode         */
	int wait;		/* wait for how many loops    */
	char *showstr_head;	/* for paging through texts   */
	char **showstr_vector;	/* -                    */
	int showstr_count;	/* -										 */
	int showstr_page;	/* -										 */
	char **str;		/* for the modify-str system  */
	int max_str;		/* -                          */
	int has_prompt;
	int prompt_mode;	/* control of prompt-printing */
	int prompt_cr;		/* control of prompt-printing */
	char curr_input[MAX_INPUT_LENGTH];	/* the current inputs     */
	char last_input[MAX_INPUT_LENGTH];	/* the last input         */
	char **history;
	int history_pos;
	struct txt_q input;	/* q of unprocessed input     */
	struct char_data *character;	/* linked to char             */
	struct char_data *original;	/* original char              */
	char *reply_to;		/* Who last told to this char */
	char *ignore[20];
	struct snoop_data snoop;/* to snoop people.           */

	struct char_data *admin;
	struct char_data *last_target;
	long last_hit_time;
	struct obj_data *admin_obj;
	int admin_zone;

	int mailtoclan;
	int mailedit;
	int mailto;

	/* Stuff for the new output buffer system. */
	char small_outbuf[SMALL_BUFSIZE];
	int bufptr;
	int bufspace;
	struct txt_block *large_outbuf;
	char *output;

	struct descriptor_data *next;	/* link to next descriptor    */
	int tmp_count;		/* temporary count variable   */
	int autologin;

};

struct msg_type {
	char *attacker_msg;	/* message to attacker */
	char *victim_msg;	/* message to victim   */
	char *room_msg;		/* message to room     */
};

struct message_type {
	struct msg_type die_msg;/* messages when death            */
	struct msg_type miss_msg;	/* messages when miss             */
	struct msg_type hit_msg;/* messages when hit              */
	struct msg_type sanctuary_msg;	/* messages when hit on sanctuary */
	struct msg_type god_msg;/* messages when hit on god       */
	struct message_type *next;	/* to next messages of this kind. */
};

struct message_list {
	int a_type;		/* Attack type                        */
	int number_of_attacks;	/* How many attack messages           */
	struct message_type *msg;	/* List of messages.                  */
};

struct auction_bidders {
	struct char_data *current;
	int bid;
	struct auction_bidders *next;
};

struct auction_type {
	struct auction_bidders *bidder;
	struct char_data *auctioner;
	struct obj_data *obj;
	int price;
	int raise;
	sh_int counter;
	int min_lvl;
	int max_lvl;
} auction_data;

struct clans_data *clans;

/** The wiz title data structure. */
struct wiztitle_data {
	/** Next pointer, points to the next structure in the linked list. */
	struct wiztitle_data *next;
	/** The name of this character. */
	char name[20];
	/** The wiz titles of this character. */
	char wiztitle[32][45];
	/** The mask of allocated wiztitles for this character. */
	int wtmask;
};

extern struct wiztitle_data *wiztitles;
