/* ***************************************************************** *
* Constants.h                                                       *
* ***************************************************************** */

#define MAX_ARMOUR 400

/*
#define IMO_LEV  41     // AVATAR
#define IMO_LEV2 43     // ARCH
#define IMO_LEGEND 42   // LEGEND
#define IMO_LEV3 44     // METH
#define IMO_LEV4 45     // ANTE
#define IMO_LEV5 46     // HIGHLORD
#define IMO_LEV6 47     // OVERLORD
#define IMO_IMP  48     // IMPLEMENTOR
#define GOD_LEV  49     // Kinetic
#define MAX_LEV  75     // DUNGEONMASTER
 */

/* New Immstructure defines */

#define IMO_LEV          41 // Avatar
#define PK_LEV           42 // New player killer level
#define IMO_SPIRIT	 47 // Spirit/Legend
#define IMO_IMM		 48 // Immortal (admin 1)
#define IMO_IMP          50 // God (admin 2, imp)
#define MAX_LEV          75

#define NPC_LEV 75

extern const int reverse_dir[];
extern const char *sk_sp_defines[];
extern const char *rank_color[];
extern const char *ban_types[];
extern const char *spell_wear_off_msg[];
extern const int movement_loss[];
extern const char *dirs[];
extern const char *weekdays[7];
extern const char *month_name[17];
extern const int sharp[];
extern const char *where[];
extern const char *loc[];
extern const char *drinks[];
extern const char *drinknames[];
extern const int drink_aff[][3];
extern const char *color_liquid[];
extern const char *fullness[];
extern const char *item_types[];
extern const char *wear_bits[];
extern const char *oflag1_bits[];
extern const char *oflag2_bits[];
extern const char *oflag3_bits[];
extern const char *weapon_bits[];
extern const char *room_bits[];
extern const char *room_bits2[];
extern const char *zflag_bits[];
extern const char *dirty_bits[];
extern const char *exit_bits[];
extern const char *sector_types[];
extern const char *equipment_types[];
extern const char *affected_bits[];
extern const char *affected_02_bits[];
extern const char *apply_types[];
extern const char *pc_class_types[];
extern const char *npc_class_types[];
extern const char *action_bits[];
extern const char *player_bits1[];
extern const char *player_bits2[];
extern const char *player_bits3[];
extern const char *player_bits4[];
extern const char *wizperm_bits[];
extern const char *descriptor_bits1[];
extern const char *position_types[];
extern const char *connected_types[];
extern const int thaco[MAX_CLASSES][MAX_LEV];
extern const byte backstab_mult[MAX_LEV];
extern const char *curse_words[];
extern const char *curse_gossip[];
extern const char *reserved_names_prefixes[];
extern const char *race_list[];
extern const char *class_anti[];
extern const char *gv_overflow_buf;
