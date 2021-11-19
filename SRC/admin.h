/* *******************************************************************
*  file: admin.h , Special module.           Part of Crimson MUD     *
*  Usage: Admin system for Crimson MUD II                            *
*                  Written by Hercules                               *
******************************************************************** */


#define BIT1_YES_NO(flag) ( IS_SET(GET_ACT1(ch), flag) ? "Yes" : "No " )
#define BIT2_YES_NO(flag) ( IS_SET(GET_ACT2(ch), flag) ? "Yes" : "No " )
#define BIT3_YES_NO(flag) ( IS_SET(GET_ACT3(ch), flag) ? "Yes" : "No " )

#define BIT1_NO_YES(flag) ( IS_SET(GET_ACT1(ch), flag) ? "No " : "Yes" )
#define BIT2_NO_YES(flag) ( IS_SET(GET_ACT2(ch), flag) ? "No " : "Yes" )
#define BIT3_NO_YES(flag) ( IS_SET(GET_ACT3(ch), flag) ? "No " : "Yes" )


#define BIT1_ON_OFF(flag) ( IS_SET(GET_ACT1(ch), flag) ? "Off" : "On " )
#define BIT2_ON_OFF(flag) ( IS_SET(GET_ACT2(ch), flag) ? "Off" : "On " )
#define BIT3_ON_OFF(flag) ( IS_SET(GET_ACT3(ch), flag) ? "Off" : "On " )

#define BIT1_OFF_ON(flag) ( IS_SET(GET_ACT1(ch), flag) ? "On " : "Off" )
#define BIT2_OFF_ON(flag) ( IS_SET(GET_ACT2(ch), flag) ? "On " : "Off" )
#define BIT3_OFF_ON(flag) ( IS_SET(GET_ACT3(ch), flag) ? "On " : "Off" )

#define TOGGLE_BIT1(flag) ( IS_SET(GET_ACT1(ch), flag) ? REMOVE_BIT(GET_ACT1(ch), flag) : SET_BIT (GET_ACT1(ch), flag) )
#define TOGGLE_BIT2(flag) ( IS_SET(GET_ACT2(ch), flag) ? REMOVE_BIT(GET_ACT2(ch), flag) : SET_BIT (GET_ACT2(ch), flag) )
#define TOGGLE_BIT3(flag) ( IS_SET(GET_ACT3(ch), flag) ? REMOVE_BIT(GET_ACT3(ch), flag) : SET_BIT (GET_ACT3(ch), flag) )


#define CAN_PERSON_UPDATE(update_level) {                              \
  if (!zone_table[zone].lord) {                                        \
		zone_table[zone].lord = strdup(" ");                            \
  }                                                                    \
  if (GET_LEVEL(ch) < IMO_IMP) {                                       \
    if (GET_LEVEL(ch) < (update_level)) {                              \
      if (!ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord)) {  \
				send_to_char("You aren't authorized to create in this zone.\r\n", ch); \
          return;                                                      \
      }                                                                \
      /* STOP ZONELORDS ON PRODUCTON */                                \
      if (gv_port == PRODUCTION_PORT) {                                \
				send_to_char("You are not authorized to create on this port.\r\n", ch); \
          return;                                                      \
      }                                                                \
    } /* END OF < update_level */                                      \
    if (GET_LEVEL(ch) < IMO_IMM &&                                    \
        IS_SET(zone_table[zone].flags, ZONE_NEW_ZONE) &&               \
        !ha1175_isexactname(GET_NAME(ch), zone_table[zone].lord)) {    \
			send_to_char("Sorry, that is a new zone.\r\n", ch);          \
				return;                                                      \
    }                                                                  \
  } /* END OF < IMO_IMP */                                             \
}


#define PRESS_ENTER(ch) { \
	send_to_char ("\r\nPress enter to continue.\r\n", ch); \
		OLD_STATE(ch->desc) = ADMIN_STATE(ch->desc); \
			ADMIN_STATE(ch->desc) = ADMIN_PRESS_ENTER; \
}

/* display menus */
#define MENU_EDIT_PLAYER	1
#define MENU_ADMIN		2
#define MENU_EDIT		3
#define MENU_OBJECTS		4
#define MENU_MOBILES		5
#define MENU_ZONES		6
#define MENU_CONFIG		7


/* Menus */
#define ADMIN_EDIT_START	1
#define ADMIN_EDIT_CHOOSE	2
#define ADMIN_EDIT_GENERAL	3
#define ADMIN_EDIT_ATT		4
#define ADMIN_MAIN		5
#define ADMIN_EDIT		6
#define ADMIN_OBJECTS		7
#define ADMIN_MOBILES		8
#define ADMIN_ZONES		9
#define ADMIN_CONFIG		10
#define ADMIN_CONFIG_PROMPT	11
#define ADMIN_CONFIG_DISPLAY	12
#define ADMIN_CONFIG_CHANNEL	13
#define ADMIN_CONFIG_FLAG	14
#define ADMIN_CONFIG_GENERAL	15
#define ADMIN_EDIT_END		50

#define ADMIN_PRESS_ENTER   	 51
#define ADMIN_MAIN_NAME  	 52
#define ADMIN_EDIT_CHOOSE_PLAYER 53

/* General settings */
#define ADMIN_GEN_START		100
#define ADMIN_GEN_NAME		101
#define ADMIN_GEN_TITLE		102
#define ADMIN_GEN_LEVEL		103
#define ADMIN_GEN_EXP		104
#define ADMIN_GEN_GOLD		105
#define ADMIN_GEN_SCORE		106
#define ADMIN_GEN_DEATHS	107
#define ADMIN_GEN_KILLS		108
#define ADMIN_GEN_CLAN		109
#define ADMIN_GEN_RANK		110
#define ADMIN_GEN_QPOINTS	111
#define ADMIN_GEN_POOFIN	112
#define ADMIN_GEN_POOFOUT	113
#define ADMIN_GEN_JAIL		114
#define ADMIN_GEN_MUZZLE	115
#define ADMIN_GEN_END		199

/* Attribute settings */
#define ADMIN_ATT_START		200
#define ADMIN_ATT_CLASS		201
#define ADMIN_ATT_RACE		202
#define ADMIN_ATT_SEX		203
#define ADMIN_ATT_AGE		204
#define ADMIN_ATT_STR		205
#define ADMIN_ATT_INT		206
#define ADMIN_ATT_WIS		207
#define ADMIN_ATT_DEX		208
#define ADMIN_ATT_CON		209
#define ADMIN_ATT_CHA		210
#define ADMIN_ATT_PRAC		211
#define ADMIN_ATT_HIT		212
#define ADMIN_ATT_MANA		213
#define ADMIN_ATT_MOVE		214
#define ADMIN_ATT_ALIGN		215
#define ADMIN_ATT_END		299

#define ADMIN_CONFIG_START	 300
#define ADMIN_CONFIG_SET_LINES	 301
#define ADMIN_CONFIG_SET_TITLE	 302
#define ADMIN_CONFIG_SET_POOFIN	 303
#define ADMIN_CONFIG_SET_POOFOUT 304
#define ADMIN_CONFIG_SET_DESC	 305
#define ADMIN_CONFIG_END	 400

static struct ovalues_type {
	char *value1;
	char *value2;
	char *value3;
	char *value4;
} 

ovalues [] = {
	{"Color", "Type", "Hours of light", "Undefined"},	/* 1 TYPE_LIGHT		*/
	{"Level", "Spell 1", "Spell 2", "Spell 3"}, 		/* 2 TYPE_SCROLL	*/
	{"Level", "Max charges", "Charges left", "Spell"},	/* 3 TYPE_WAND		*/
	{"Level", "Max charges", "Charges left", "Spell"},	/* 4 TYPE_STAFF		*/
	{"Weapon flags", "Damage dice", "Damage roll", "Type of weapon"}, /* 5 TYPE_WEAPON	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 6 TYPE_FIREWEAPON	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 7 TYPE_MISSILE	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 8 TYPE_TREASURE	*/
	{"AC apply", "Max charges", "Charges left", "Spell"},	/* 9 TYPE_ARMOR		*/
	{"Level", "Spell 1", "Spell 2", "Spell 3"}, 		/* 10 TYPE_POTION	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 11 TYPE_WORN		*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 12 TYPE_OTHER	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 13 TYPE_TRASH	*/
	{"Spell", "Hitpoints", "Undefined", "Undefined"},	/* 14 TYPE_TRAP		*/
	{"Max contains", "Lock type", "Undefined", "Corpse"},	/* 15 TYPE_CONTAINER	*/
	{"Tongue", "Undefined", "Undefined", "Undefined"},	/* 16 TYPE_NOTE		*/
	{"Max contain", "Contains", "Liquid type", "Poisoned"}, /* 17 TYPE_DRINKCON     */
	{"Opens object", "# of uses", "Undefined", "Undefined"},/* 18 TYPE_KEY		*/
	{"Makes full", "Undefined", "Undefined", "Poisoned"},   /* 19 TYPE_FOOD		*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 20 TYPE_MONEY	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 21 TYPE_PEN		*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 22 TYPE_BOAT		*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 23 TYPE_ZCMD		*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 24 TYPE_VEHICLE	*/
	{"Value 0", "Value 1", "Value 2", "Value 3"},		/* 25 TYPE_CLIMABLE	*/
	{"Level", "Max charges", "Charges left", "Spell"},	/* 26 TYPE_SPELL	*/
};

