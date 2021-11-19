/* as */
/* *******************************************************************
*  file: assign.c , Special module.                  Part of DIKUMUD *
*  Usage: Procedures assigning function pointers.                    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>

#include "structs.h"
#include "db.h"
#include "utility.h"
#include "globals.h"
#include "func.h"

// int special_shield(struct char_data * ch, int cmd, char *arg);

#define ASSIGN_MOB(the_mob, the_function) { \
	if ((lv_num = db8100_real_mobile(the_mob)) && (lv_num > -1)) \
		mob_index[lv_num].func = (the_function); \
}

#define ASSIGN_OBJ(the_obj, the_function) { \
	if ((lv_num = db8200_real_object(the_obj)) && (lv_num > -1)) \
		obj_index[lv_num].func = (the_function); \
}

#define ASSIGN_ROOM(the_room, the_function) { \
	if ((lv_num = db8000_real_room(the_room)) && (lv_num > -1)) \
		world[lv_num].funct = (the_function); \
}

/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* assign special procedures to mobiles */
void as1000_assign_mobiles(void)
{

	int lv_num;

	/* NOTE:  IF WE HAVE AN UNASSIGNED ROOM/MOBILE/OBJECT, THE */
	/* real_x() CODE WILL REPORT IT AS AN ERROR.        */

	/* LIMBO */
	mob_index[0].func = pr2900_puff;	/* Assign Puff */
	ASSIGN_MOB(3, pr2800_gothelred);
	ASSIGN_MOB(4, r7000_func_receptionist);
	ASSIGN_MOB(8, pr2950_santa);

	/* Bounty master/hunter */
	ASSIGN_MOB(26, bt2000_bounty_master);
	ASSIGN_MOB(27, bt2200_bounty_hunter);

	/* CAMELOT */
	ASSIGN_MOB(2045, pr2799_camelot);	/* Added, comment out again if
						 * doesnt work */

	/* NEWBIE AREA */
	/* ASSIGN_MOB(5600, pr2899_newbie_helper); ASSIGN_MOB(5601,
	 * pr2898_newbie_blesser); *//* not in yet */


	/* MIDGAARD */
	ASSIGN_MOB(2609, pr2600_high_priest);
	ASSIGN_MOB(3061, pr3100_janitor);
	//ASSIGN_MOB(3062, pr3000_fido);
	//ASSIGN_MOB(3066, pr3000_fido);
	ASSIGN_MOB(3005, r7000_func_receptionist);
	ASSIGN_MOB(3148, r7200_func_banker);
	ASSIGN_MOB(3143, pr1700_mayor);
	ASSIGN_MOB(3145, pr3500_bountyhunter);
	//special forces guard
		ASSIGN_MOB(3146, pr3500_bountyhunter);
	//jinipan
		ASSIGN_MOB(3061, pr3500_bountyhunter);
	//wombles
		ASSIGN_MOB(14100, pr3500_bountyhunter);
	//chia
		ASSIGN_MOB(2034, pr3500_bountyhunter);
	//pim
		ASSIGN_MOB(3169, pr3500_bountyhunter);
	//billy
		ASSIGN_MOB(8300, pr3500_bountyhunter);
	//DM
		// ASSIGN_MOB(3137, pr3500_bountyhunter);
	//Questmaster
	ASSIGN_MOB(3060, pr3500_bountyhunter);
	ASSIGN_MOB(3165, pr3500_bountyhunter);
	ASSIGN_MOB(3170, pc1600_healer);

	/* these mobs remove objs left layign around from play */
	ASSIGN_MOB(3147, pr3200_slime);
	ASSIGN_MOB(8310, pr3200_slime);
	ASSIGN_MOB(8303, pr3400_gremlin);


	/* Dungeonmaster and Tiamat  */
	/* ASSIGN_MOB(8300, pr3300_dungeon_master); ASSIGN_MOB(8311,
	 * pr3300_dungeon_master); */

	/* SilverDale mobs */
	ASSIGN_MOB(4510, si1000_nosferatu);

	/* Elmpatris */
	ASSIGN_MOB(4836, r7000_func_receptionist);
	/* shaolintemple */
	ASSIGN_MOB(14120, pr3500_bountyhunter);

	/* dragon realms */
	/* ASSIGN_MOB(13802, pr3500_bountyhunter); */

	/* snotlings */
	ASSIGN_MOB(14302, pr3500_bountyhunter);
	ASSIGN_MOB(14303, pr3500_bountyhunter);

	/* egypt */

	/* GUILDS and such */
	ASSIGN_MOB(3020, pr1400_do_guild);
	ASSIGN_MOB(3021, pr1400_do_guild);
	ASSIGN_MOB(3022, pr1400_do_guild);
	ASSIGN_MOB(3023, pr1400_do_guild);
	ASSIGN_MOB(3024, pr2700_guild_guard);
	ASSIGN_MOB(3025, pr2700_guild_guard);
	ASSIGN_MOB(3026, pr2700_guild_guard);
	ASSIGN_MOB(3027, pr2700_guild_guard);
	ASSIGN_MOB(5142, pr1400_do_guild);
	ASSIGN_MOB(5143, pr1400_do_guild);
	ASSIGN_MOB(5144, pr1400_do_guild);
	ASSIGN_MOB(5145, pr1400_do_guild);

	/* MORIA */
	ASSIGN_MOB(4000, pr2200_snake);
	ASSIGN_MOB(4001, pr2200_snake);
	ASSIGN_MOB(4053, pr2200_snake);
	ASSIGN_MOB(4103, pr2300_thief);
	ASSIGN_MOB(4102, pr2200_snake);

	/* The Guys */
	ASSIGN_MOB(800, pc2200_sretaw);

	/* SEWERS */
	ASSIGN_MOB(7006, pr2200_snake);

	/* FOREST */
	ASSIGN_MOB(6113, pr2200_snake);
	ASSIGN_MOB(6114, pr2200_snake);

	/* Island3 */
	ASSIGN_MOB(MOB_ISLAND3_MOB, pc1400_island3_parts);

	/* GerinsHill */
	ASSIGN_MOB(MOB_GERINSHILL_THOTH, pc1100_gerinshill_dentures_key);

	/* Gnomes */
	ASSIGN_MOB(8357, r7000_func_receptionist);

	/* Dusk */
	ASSIGN_MOB(1910, r7000_func_receptionist);

	/* Camelot */
	/* ASSIGN_MOB(2030, r7000_func_receptionist); */


	/* Arnath */
	ASSIGN_MOB(14431, r7000_func_receptionist);

	/* Quest */
	ASSIGN_MOB(MOB_QUESTMAN, qu1000_questmaster);

	/* Killer Womble */
	ASSIGN_MOB(902, pc1800_killer_womble);
	ASSIGN_MOB(1715, pc1800_killer_womble);

	/* Midgaard */
	/* ASSIGN_MOB(3060, sm1000_midgaard_cityguard); ASSIGN_MOB(3146,
	 * sm1100_midgaard_jinipan); */
	/* Lucifers domain */
	ASSIGN_MOB(15500, pc2000_darken);
	/* Village of Korum */
	ASSIGN_MOB(12630, pc2400_cliff);

}				/* END OF as1000_assign_mobiles() */


/* assign special procedures to objects */
void as1100_assign_objects(void)
{
	int lv_num;

	/* CLOUD PALACE */
	ASSIGN_OBJ(2126, sp2000_cloud_stone);
	ASSIGN_OBJ(2136, sp2100_cloud_bookshelf);

	/* MIDGAARD */
	ASSIGN_OBJ(OBJ_BOARD_HIGHER, board);
	ASSIGN_OBJ(OBJ_BOARD_IMMORTAL, board);
	ASSIGN_OBJ(OBJ_BOARD_ZONEBUILD, board);
	ASSIGN_OBJ(OBJ_BOARD_AUCTION, board);
	ASSIGN_OBJ(OBJ_BOARD_SOCIAL, board);
	ASSIGN_OBJ(OBJ_BOARD_COUNSIL, board);
	ASSIGN_OBJ(OBJ_BOARD_MORTAL, board);
	ASSIGN_OBJ(OBJ_NEWBIE_BOOK, pc1300_newbie_book);
	ASSIGN_OBJ(OBJ_BOARD_APPEALS, board);
	ASSIGN_OBJ(OBJ_BOARD_OVERLORD, board);
	ASSIGN_OBJ(OBJ_BOARD_IMPLEMENTOR, board);
	ASSIGN_OBJ(OBJ_BOARD_ANTE, board);
	ASSIGN_OBJ(OBJ_BOARD_CREGRP, board);
	ASSIGN_OBJ(OBJ_BOARD_COUGRP, board);
	ASSIGN_OBJ(OBJ_BOARD_DEVGRP, board);
	ASSIGN_OBJ(OBJ_BOARD_RELGRP, board);
	ASSIGN_OBJ(OBJ_BOARD_HELLNIGHT, board);
	ASSIGN_OBJ(OBJ_BOARD_CODER, board);
	ASSIGN_OBJ(OBJ_BOARD_ZONECOUNCIL, board);
	ASSIGN_OBJ(OBJ_BOARD_ROGERWATERS, board);

	ASSIGN_OBJ(3459, sp2200_clan_teleporter_button);

	ASSIGN_OBJ(3073, pc1900_slot_machine);

	ASSIGN_OBJ(4510, forge);
	/* CLAN BOARDS */
	ASSIGN_OBJ(OBJ_CLAN_BOARD5, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD6, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD7, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD10, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD11, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD12, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD13, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD2, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD21, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD23, board);
	ASSIGN_OBJ(OBJ_CLAN_BOARD20, board);

	/* Arcain */
//	ASSIGN_OBJ(2704, sp1500_arcain_chest);

	/* Arena */
	ASSIGN_OBJ(OBJ_PORTAL, mo2500_enter_portal);

	/* CityofMirielle */
	ASSIGN_OBJ(7646, sp1000_citym_rainbow_box);
	
	/* Myth Drannor */
	ASSIGN_OBJ(8661, sp1050_mithril_fountain);

	/* Quel'sith Elves */
	ASSIGN_OBJ(20766, sp1250_qelves_dias);

	/* ammo */
	ASSIGN_OBJ(552, sp2300_load_ammo);
	ASSIGN_OBJ(553, sp2300_load_ammo);
	ASSIGN_OBJ(554, sp2300_load_ammo);
	ASSIGN_OBJ(555, sp2300_load_ammo);
	ASSIGN_OBJ(556, sp2300_load_ammo);

	/* Special sword */
	ASSIGN_OBJ(960, pc1700_special_sword);

	/* Special shield */
	//ASSIGN_OBJ(20085, special_shield);


} /* END OF as1100_assign_objects() */

/* assign special procedures to rooms */
void as1200_assign_rooms(void)
{

	int lv_num;

	//ASSIGN_ROOM(8835, fi1000_door_knocker);

	/* GERINSHILL */
	ASSIGN_ROOM(1414, pc1000_gerinshill_room_1414);
	ASSIGN_ROOM(1496, pc1200_gerinshill_room_1496);

	/* ARENA */
	ASSIGN_ROOM(2690, pr1500_do_arena);
	ASSIGN_ROOM(2691, pr1500_do_arena);
	ASSIGN_ROOM(2692, pr1500_do_arena);
	ASSIGN_ROOM(2693, pr1500_do_arena);
	ASSIGN_ROOM(2694, pr1500_do_arena);
	ASSIGN_ROOM(2695, pr1500_do_arena);
	ASSIGN_ROOM(2696, pr1500_do_arena);
	ASSIGN_ROOM(2697, pr1500_do_arena);
	ASSIGN_ROOM(2698, pr1500_do_arena);
	ASSIGN_ROOM(2699, pr1500_do_arena);

	ASSIGN_ROOM(3030, pr1600_do_dump);
	ASSIGN_ROOM(3054, pr4000_pray_for_items);
	ASSIGN_ROOM(4513, cliff);

	ASSIGN_ROOM(3031, pr3600_pet_shops);

	
	ASSIGN_ROOM(17933, pc1500_glass_door);

	ASSIGN_ROOM(19282, pc1550_darkcastle_portal);
	ASSIGN_ROOM(8229, cows_room);
	ASSIGN_ROOM(3078, council_room);
     /* Elfin Citadel */
	ASSIGN_ROOM(14670, citadel_altar);
     /* Halfling Shire */
	ASSIGN_ROOM(17607, halfling_smith);
	/* Teldescian Swamps */
	ASSIGN_ROOM(16956, swamp_priest);
	/* Enchated Hollow */
	if (gv_port == ZONING_PORT) {
		//zone not finished.won 't work on mainport until moved over from zone
			ASSIGN_ROOM(23917, pc2300_teleport_room);
			ASSIGN_ROOM(21833, pr3750_brim_pet_shop); //Brimstone City Pet Shop
	}
}/* END OF as1200_assign_rooms() */
