#include <signal.h>
#include <errno.h>

char *gv_ptr;
struct char_data gv_ch;
struct char_data *gv_ch_ptr;
struct char_data *gv_ch_doing_command;
struct char_data *killer_mob;
struct char_data *questman;
struct char_data *bounty_master;
struct char_data *bounty_hunter;

struct obj_data *sp_obj;

sbyte gv_rush_hour;
int gv_rush_time;

char gv_str[MAX_STRING_LENGTH];
long gv_num;
long gv_total_objects;
long gv_total_mobiles;
int gv_port;
int gv_pkill_level;
int gv_last_clan;

int gv_questreact;
int gv_location1;
int gv_location2;
int gv_location3;
int gv_location4;
int gv_location5;
int gv_location6;
int gv_location7;
int gv_location8;
int gv_location9;
int gv_location10;
int gv_location11;
int gv_location12;
int gv_current_players;
int gv_boottime_high;
int gv_deathroom;
int gv_rebuild;
int gv_verbose_messages;
int gv_switched_orig_level;
int gv_mega_verbose_messages;
int gv_trace_commands;
int gv_auto_level;
int gv_highest_login_level;
int gv_syslog_loaded;
char gv_login_message[80];
char gv_master_pwd[80];
int gv_way_player_left_game;
int gv_mob_donate_room;
int gv_donate_room;
int gv_force_infinate_loop;
time_t gv_infinate_loop_count;
int gv_terminate;
time_t gv_terminate_count;

#if defined (DIKU_WINDOWS)
int gv_pid_t;
#else
pid_t gv_pid_t;
#endif

int gv_sanity_return;
int gv_run_flag;
ubyte gv_next_trace_file;
ubyte gv_use_nameserver;

int gv_anti1_PC_bits;
int gv_anti2_PC_bits;
int gv_anti3_PC_bits;
int gv_anti4_PC_bits;

int gv_anti1_NPC_bits;
int gv_anti2_NPC_bits;
int gv_anti3_NPC_bits;

int gv_good1_NPC_bits;
int gv_good2_NPC_bits;
int gv_good3_NPC_bits;

int buf_switches;
int buf_largecount;
int buf_overflows;

//Questmaster transformation sales
int isQuestTransformOn;
//0 / 1 off / on
int transformSalesChance;
//keep less than 10 %.i.e.1 d101 <= 10;
int isQuestAutoOn;
//decides on rush.
int isQuestOverrideOn;
#define QMSALES_MIN_CHANCE  1
#define QMSALES_MAX_CHANCE  100
//////////////////////////////////////

#define RUN_FLAG_NOSPECIAL		BIT0
#define RUN_FLAG_NOZONE_RESET           BIT1
#define RUN_FLAG_NOWEATHER		BIT2
#define RUN_FLAG_TERMINATE       	BIT3
#define RUN_FLAG_DISABLE_FIGHTING	BIT4
#define RUN_FLAG_CREATE_MAX_FILE 	BIT5
#define RUN_FLAG_CHAR_DIED       	BIT6
#define RUN_FLAG_REBUILD_EQUIPMENT      BIT7

/* because do_move doesn't return a value and speedwalk needs
to know if it worked, this global variable is needed */
int gv_move_succeeded;

/* GLOBAL VARIABLES USED ALL OVER THE PLACE */
#define PRODUCTION_PORT     4000
#define xRODUCTION_PORT     4002
#define QUEST_PORT          6913
#define HELLNIGHT_PORT      6666
#define ZONING_PORT         4500
#define DEFAULT_PORT        5000
#define DEVELOPMENT_PORT    17422

/* ROOM DEFINES */
#define VOID_ROOM                        0
#define LIMBO_ROOM                       1
#define JAIL_ROOM                        2
#define PADDED_ROOM                      3
#define IMM_CHAT_ROOM                    4
#define WEDDING_ROOM                     5
#define REHAB_ROOM                       6
#define SUNSHINES_ROOM                   7

#define CHALICE_ROOM                  2500
#define DRESSING_ROOM                 3000
#define ROYAL_ACADEMY                  100
#define FOUNTAIN_ROOM                 3014
#define MAGES_GUILD_ROOM              3017
#define PET_ROOM                      3032
#define VEHICLE_BEG_ROOM              5000
#define VEHICLE_END_ROOM              5099
#define DEATH_ROOM                    3000

/* OBJECT DEFINES */
#define OBJ_GERINSHILL_FLAME_SWORD    1404
#define OBJ_GERINSHILL_TEETH          1438

#define OBJ_ICE_PICK                  2411
#define OBJ_PART_ONE                  2419
#define OBJ_PART_TWO                  2420
#define OBJ_PART_THREE                2421
#define OBJ_PART_FOUR                 2422
#define OBJ_PART_FIVE                 2423

#define OBJ_PORTAL                    2691
#define OBJ_DEMON_FLESH               2692
#define OBJ_BOARD_MORTAL              3099
#define OBJ_BOARD_APPEALS             8335
#define OBJ_BOARD_IMMORTAL            3097
#define OBJ_BOARD_ZONEBUILD           3098
#define OBJ_BOARD_SOCIAL              3436
#define OBJ_BOARD_AUCTION             3447
#define OBJ_BOARD_COUNSIL             3448
#define OBJ_BOARD_CREGRP              3449
#define OBJ_BOARD_COUGRP              3450
#define OBJ_BOARD_DEVGRP              3451
#define OBJ_BOARD_RELGRP              3452
#define OBJ_BOARD_CODER               3453
#define OBJ_NEWBIE_BOOK               3435
#define OBJ_BOARD_ANTE                8337
#define OBJ_BOARD_HIGHER              3096
#define OBJ_BOARD_IMPLEMENTOR         8336
#define OBJ_BOARD_OVERLORD            3462
#define OBJ_BOARD_HELLNIGHT           3074
#define OBJ_BOARD_ZONECOUNCIL         8371
#define OBJ_BOARD_ROGERWATERS         8302


#define OBJ_CLAN_BOARD2                415
#define OBJ_CLAN_BOARD5		       404
#define OBJ_CLAN_BOARD6                401
#define OBJ_CLAN_BOARD7		       410
#define OBJ_CLAN_BOARD10               411
#define OBJ_CLAN_BOARD11	       406
#define OBJ_CLAN_BOARD12               414
#define OBJ_CLAN_BOARD13	       405
#define OBJ_CLAN_BOARD20             21017
#define OBJ_CLAN_BOARD21             21014
#define OBJ_CLAN_BOARD23             21000


#define OBJ_ARROW			552
#define OBJ_P_ARROW			553
#define OBJ_BOLT			554
#define OBJ_P_BOLT			555
#define OBJ_D_BOLT			556
#define OBJ_BOW				557
#define OBJ_CROSSBOW			558
#define OBJ_HEAVYCROSSBOW		559
#define OBJ_FYREBRAND                  7763

#define OBJ_BLUE_POTION			3428
#define OBJ_WHITE_POTION		3465
#define OBJ_HEALING_POTION              12637
#define OBJ_NEWBIE_HEALING_POTION	8052
/* MOB DEFINES */
#define MOB_RAHN			2609
#define MOB_JINIPAN			3146
#define MOB_ISLAND3_MOB			2417
#define MOB_CAMELOT			2045
#define MOB_GERINSHILL_FLAME_ELEMENTAL	1408
#define MOB_GERINSHILL_THOTH		1432
#define MOB_QUESTMAN			3171
#define MOB_BOUNTYMASTER		26
#define MOB_BOUNTYHUNTER		27

#define ZCREATE_OBJECT      1
#define ZCREATE_MOBILE      2

#define DEBUG_CHAR          "debugger"
#define DEBUG_HOST          "209.94.64.80"

/*2^(32-1)-1*/
#define MAX_GOLD 2147483647
#define MAX_DEPOSIT 2000000000
#define MAX_BALANCE 2000000000
