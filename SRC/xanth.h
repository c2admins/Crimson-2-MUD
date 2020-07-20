/* ********************************************************************
*  file: xanth.h.                                Part of DIKUMUD     *
**********************************************************************/

#define NUMBER_PUZZLE_ROOMS 10
#define XANTH_ENTRY_ROW      1
#define XANTH_ENTRY_COL      1

#define XANTH_SOLVE_BLOCK    5
#define XANTH_SOLVE_ROW      7
#define XANTH_SOLVE_COL      3
#define XANTH_EXIT_ROW       6
#define XANTH_EXIT_COL       3

#define PZ_MARBLE            1
#define PZ_MOVE_STONE        2
#define PZ_LADDER_WEST       3
#define PZ_LADDER_EAST       4
#define PZ_LADDER_NORTH      5
#define PZ_LADDER_SOUTH      6

/* ROOM DEFINES */
#define XANTH_NORTH_VILLAGE    510	/* North Village               */
#define XANTH_PINE_GROVE1      520	/* */
#define XANTH_PINE_GROVE2      521	/* */
#define XANTH_PINE_GROVE3      522	/* */
#define XANTH_PINE_GROVE4      523	/* */
#define XANTH_PINE_GROVE5      524	/* */

#define XANTH_OUTER_GUARD_ROOM 527	/* Guard room near portcullis  */
#define XANTH_HEAL_LEVER_ROOM  532	/* */
#define XANTH_ISLE_BEACH       537	/* room links east to isle     */
#define XANTH_PUZZLE_ENTRY     538	/* ROOM THAT RESETS PUZZLE     */
#define XANTH_PUZZLE_ROOM      539	/* ROOM USED FOR PUZZLE        */
#define XANTH_PUZZLE_EXIT      540	/* EXIT ROOM FROM PUZZLE       */
#define XANTH_RACE_CLASS_ROOM  540	/* race/class button room      */
#define XANTH_FROM_NORTH       544	/* Secret path from N. Village */
#define XANTH_FRIEND_JUNCTION  548	/* Say friend and enter        */
#define XANTH_ABOVE_CREVICE    557	/* dex check made here         */
#define XANTH_OUTER_JAIL_CELL  560	/* Jail cell near portcullis   */
#define XANTH_CREVICE_ROOM     561	/* where you end if dex fails  */
#define XANTH_ELSEWHERE_ROOM   591	/* */
#define XANTH_OCEAN_AUTO_LOAD1 612	/* */
#define XANTH_OCEAN_AUTO_DUMP1 613	/* */
#define XANTH_OCEAN_AUTO_DUMP2 614	/* */
#define XANTH_OCEAN_AUTO_DUMP3 615	/* */
#define XANTH_OCEAN_AUTO_LOAD2 619	/* */
#define XANTH_OCEAN_AUTO_LOAD3 620	/* */
#define XANTH_MOUSEVATOR_EXIT  624	/* exit   elevator             */
#define XANTH_FRIEND_NRTH_ENT  627	/* Say friend and enter        */
#define XANTH_PORTCULIS_ROOM   628	/* */
#define XANTH_MOUSEVATOR_ENTR  629	/* enter  elevaator            */
#define XANTH_MOUSEVATOR_ROOM  630	/* inside elevator             */
#define XANTH_ISLE_WEEDGROWN   633	/* */
#define XANTH_ISLE_PANTRY      634	/* */
#define XANTH_ISLE_SHACK       635	/* */
#define XANTH_ISLE_OUTHOUSE    636	/* */
#define XANTH_ISLE_MAINPATH_1  640	/* */
#define XANTH_ISLE_MAINPATH_2  641	/* */
#define XANTH_ISLE_GARDEN      642	/* */
#define XANTH_ISLE_PALACE      644	/* */
#define XANTH_ISLE_DINING      643	/* */
#define XANTH_ISLE_BATHROOM    645	/* */

/* OBJECT DEFINES */
#define OBJ_XANTH_SPONGE               500
#define OBJ_XANTH_BUTTON_BLACK         510
#define OBJ_XANTH_BUTTON_RED           511
#define OBJ_XANTH_BUTTON_GREEN         512
#define OBJ_XANTH_BUTTON_BROWN         513
#define OBJ_XANTH_BUTTON_BLUE          514
#define OBJ_XANTH_BUTTON_PURPLE        515
#define OBJ_XANTH_BUTTON_YELLOW        516
#define OBJ_XANTH_BUTTON_GRAY          517
#define OBJ_XANTH_BUTTON_GENERIC_ONE   518
#define OBJ_XANTH_BUTTON_GENERIC_TWO   519
#define OBJ_XANTH_BUTTON_GENERIC_THREE 520
#define OBJ_XANTH_LEVER                522
#define OBJ_XANTH_PANDORAS_BOX         524
#define OBJ_XANTH_SHADES_GLASSES       527
#define OBJ_XANTH_PLANK                528

#define OBJ_XANTH_LEVER_BLACK          520
#define OBJ_XANTH_LEVER_RED            521
#define OBJ_XANTH_LEVER_GREEN          522
#define OBJ_XANTH_LEVER_BROWN          523
#define OBJ_XANTH_LEVER_BLUE           524
#define OBJ_XANTH_LEVER_PURPLE         525
#define OBJ_XANTH_LEVER_YELLOW         526
#define OBJ_XANTH_LEVER_GRAY           527

#define OBJ_XANTH_IRIS_BEG_ITEM        539
#define OBJ_XANTH_IRIS_FLOWER          539
#define OBJ_XANTH_IRIS_STEAK           540
#define OBJ_XANTH_IRIS_COOKIE          541
#define OBJ_XANTH_IRIS_WINE            542
#define OBJ_XANTH_IRIS_KNICKERS        543
#define OBJ_XANTH_IRIS_UNIFORM         544
#define OBJ_XANTH_IRIS_SHAWL           545
#define OBJ_XANTH_IRIS_GRASS           546
#define OBJ_XANTH_IRIS_CUP_WATER       547
#define OBJ_XANTH_IRIS_RICE            548
#define OBJ_XANTH_IRIS_PANTIES         549
#define OBJ_XANTH_IRIS_END_ITEM        549

#define OBJ_XANTH_BEG_MAGIC_ITEMS      800
#define OBJ_XANTH_RING_OPPOSIT_ALIGN   800
#define OBJ_XANTH_RING_SPELL_FIREBALL  801	/* spell absorption */

#define OBJ_XANTH_HELM_OPPOSIT_ALIGN   805
#define OBJ_XANTH_HELM_SPELL_HEAL      806	/* spell absorption */

#define OBJ_XANTH_ROBE_OPPOSIT_ALIGN   810
#define OBJ_XANTH_ROBE_SPELL_M_MISSLE  811	/* spell absorption */

#define OBJ_XANTH_JEWEL_SPELL_HEAL     815	/* spell absorption */

#define OBJ_XANTH_END_MAGIC_ITEMS      850

/* MOB DEFINES */
#define MOB_XANTH_JUSTIN               500
#define MOB_XANTH_JAMA                 501
#define MOB_XANTH_ZINK                 502
#define MOB_XANTH_POTIPHER             503
#define MOB_XANTH_SABRINA              504
#define MOB_XANTH_CHAMELEON            505
#define MOB_XANTH_ELEVATOR_REPAIRMAN   511
#define MOB_XANTH_WRAITH               512
#define MOB_XANTH_GATE_GUARD           514
#define MOB_XANTH_QUEEN_IRIS           515
#define MOB_XANTH_IRIS_DRAGON          601
#define MOB_XANTH_IRIS_GARDNER         602

char pz_ar[10][10];
unsigned char pz_puzzle_row, pz_puzzle_col;

void xn1200_xanth_create_link(int lv_room, int lv_dir);
