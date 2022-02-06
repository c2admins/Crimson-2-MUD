/* ***************************************************************** *
*  file: handler.h , Handler module.                 Part of DIKUMUD *
*  Usage: Various routines for moving about objects/players          *
******************************************************************** */

#define  REMOVE_FOLLOWER                 1

#define  END_EXTRACT_BY_THALOS           1
#define  END_EXTRACT_BY_2                2
#define  END_EXTRACT_BY_COMMAND_QUIT     3
#define  END_EXTRACT_BY_PURGE_NPC        4
#define  END_EXTRACT_BY_PURGE_PC         5
#define  END_EXTRACT_BY_PURGE_ALL        6
#define  END_EXTRACT_BY_RAW_KILL         7
#define  END_EXTRACT_BY_AGE              8
#define  END_EXTRACT_BY_MENU_QUIT        9
#define  END_EXTRACT_BY_EQ_TIMEOUT      10
#define  END_EXTRACT_BY_NOEQ_TIMEOUT    11
#define  END_EXTRACT_BY_12              12
#define  END_EXTRACT_BY_13              13
#define  END_EXTRACT_BY_14              14
#define  END_EXTRACT_BY_15              15
#define  END_EXTRACT_BY_RENT            16
#define  END_EXTRACT_BY_GATE_MOB        17
#define  END_EXTRACT_BY_GATE_NO_LEADER  18
#define  END_EXTRACT_BY_REMOVE_PRIEST   19
#define  END_EXTRACT_BY_REMOVE_DRAGON   20
#define  END_EXTRACT_BY_DEAD_MOB        21
#define  END_EXTRACT_BY_22              22
#define  END_EXTRACT_BY_DEATH_ROOM      23
#define  END_EXTRACT_BY_ZONE_RESET      24
#define  END_EXTRACT_BY_XANTH           25
#define  END_EXTRACT_BY_ANIMAL_SUMMON   26
/* these are also passed to stop_follower so keep */
/* them separate from END_EXTRACT so you'll know  */
/* how stop_follower got called.                  */
#define  END_FOLLOW_BY_SELF               9001
#define  END_FOLLOW_BY_ANOTHER            9002
#define  END_FOLLOW_BY_TREASON            9003
#define  END_FOLLOW_BY_BEG_CHARM_PERSON   9004
#define  END_FOLLOW_BY_END_CHARM_PERSON   9005
#define  END_FOLLOW_BY_GATE_SPELL         9006
#define  END_FOLLOW_BY_ANIMATE_DEAD_SPELL 9007
#define  END_FOLLOW_BAD_LANGUAGE          9008
#define  END_FOLLOW_1                     9009
#define  END_FOLLOW_2                     9010
#define  END_FOLLOW_3                     9011
#define  END_FOLLOW_4                     9012
#define  END_FOLLOW_5                     9013
#define  END_FOLLOW_6                     9014
#define  END_FOLLOW_7                     9015
#define  END_FOLLOW_BY_ANIMAL_SUMM_SPELL  9016
/* Generic Find */

#define FIND_CHAR_ROOM     1
#define FIND_CHAR_WORLD    2
#define FIND_OBJ_INV       4
#define FIND_OBJ_ROOM      8
#define FIND_OBJ_WORLD    16
#define FIND_OBJ_EQUIP    32

#define TIMER_USE_DROP 0
#define TIMER_USE_INV  1
#define TIMER_USE_EQ   2
