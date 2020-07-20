/* cr */
/* gv_location: 4001-4500 */
/* ******************************************************************* *
*  file: create.c, Special module.                 Part of DIKUMUD     *
*  Usage: Procedures handling special procedures for the world         *
*         builders brought to you by the razor-keen intellect of       *
*         Cryogen.  Originally written for use with the Crimson server *
*                                                                      *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.    *
********************************************************************* */

#define DEFAULT_UPDATE_LEV IMO_IMM

#define CAN_PERSON_UPDATE(update_level) {                              \
  if (!zone_table[zone].lord) {                                        \
		zone_table[zone].lord = strdup(" ");                            \
  }                                                                    \
  if (GET_LEVEL(ch) < IMO_IMM) {                                  \
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
				return;                                                 \
    }                                                                  \
    /* KEEP THEIR FINGERS OUT OF XANTH! */                             \
    if (!strncmp(zone_table[zone].filename, "Xanth", 5)) {             \
			send_to_char("You do not have access to this zone.\r\n", ch); \
				return;                                                     \
    }                                                                  \
  } /* END OF < IMO_IMP */                                             \
}

#define LIMIT_TO_ZONE() {                                              \
	/* MAKE SURE MOB IS IN THE CURRENT ZONE */                         \
	if (GET_LEVEL(ch) < IMO_IMP &&                                    \
			zone != world[ch->in_room].zone) {                             \
		send_to_char("Zitems must come from current zone.\r\n", ch);   \
			return;                                                        \
	}                                                                  \
}

#define WRITE_RPT_FILE(str, report_file) { \
	fputs((str), (report_file));           \
    lv_line_count++;                       \
}

#define CHECK_VALUE(low, high) {                               \
	if ((lv_stat_value < (low) || lv_stat_value > (high))) {     \
		bzero(buf, sizeof(buf));                                \
			sprintf(buf,"Stat value must be between %d and %d.\r\n",\
							(low), (high));                                  \
								send_to_char(buf, ch);                                  \
									return;                                                 \
	} \
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "modify.h"
#include "globals.h"
#include "xanth.h"
#include "func.h"

#define RATTACK_FORMAT "Try mattack <mob> <slot> <type> <skill second> <skill third> <skill fourth>.\r\n"
#define MSKILL_FORMAT  "Try mskill <mob> <skill_type> <skill>.\r\n"
#define MSET_FORMAT    "Try mset <mob> <stat_type> <stat_value>.\r\n"
#define ZSET_FORMAT    "Try zset <field> <field_value>.\r\n"

const char *attack_types[] = {
	"none",
	"hit",
	"bludgeon",
	"pierce",
	"slash",
	"whip",
	"claw",
	"bite",
	"sting",
	"crush",
	"\n",
};
/* for zones */
#define ZCMD zone_table[zone].cmd

/* *******************************************************************
*  Special procedures for Creation!                                  *
******************************************************************** */


/*****************************************************************
*                                                               *
*                                                               *
*                     Z O N    S T U F F                        *
*                                                               *
*                                                               *
*****************************************************************/

void do_zhelp(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];

	bzero(buf, sizeof(buf));
	for (; *arg == ' '; arg++);
	if (*arg)
		sprintf(buf, "zhelp_%s", arg);
	else
		strcpy(buf, "zhelp");
	in3500_do_help(ch, buf, CMD_HELP);

	return;

}				/* END OF do_zhelp() */


void do_zlist(struct char_data * ch, char *arg, int cmd)
{
	int i, zone, start, end;
	extern int top_of_zone_table;
	char buf[MAX_STRING_LENGTH * 4], buf1[MAX_STRING_LENGTH * 4], buf2[MAX_STRING_LENGTH * 4];

	half_chop(arg, buf1, buf2);

	zone = world[ch->in_room].zone;
	if (is_number(buf1))
		start = (atoi(buf1));
	else
		start = 0;
	if (is_number(buf2))
		end = (atoi(buf2));
	else
		end = top_of_zone_table;

	sprintf(buf, "\r\n");	/* init buf to something for strcat */
	for (i = start; (i >= 0) && (i <= top_of_zone_table) && (i <= end); i++) {
		sprintf(buf1, "%3d. [%-20s] (%5d-%5d)",
			i,
			zone_table[i].filename,
			zone_table[i].bottom,
			zone_table[i].top);
		strcat(buf, buf1);

		sprintf(buf1, " rooms[%3d]",
			(!IS_SET(zone_table[i].dirty_wld, SCRAMBLED) ? (zone_table[i].real_top - zone_table[i].real_bottom + 1) : (zone_table[i].top - zone_table[i].bottom + 1)));
		strcat(buf, buf1);

		/* if (zone_table[i].lord == 0)  */
		if (!(zone_table[i].lord) || !*(zone_table[i].lord))
			sprintf(buf1, " No Lord");
		else
			sprintf(buf1, " Lord[%s]", zone_table[i].lord);
		strcat(buf, buf1);

		if ((GET_LEVEL(ch) >= IMO_IMP) || (gv_port == 4500)) {
			sprintf(buf1, " State[%s]", (IS_SET(zone_table[i].dirty_wld, UNSAVED)) ? "MODIFIED" : "saved");
			strcat(buf, buf1);
		}

		sprintf(buf1, "\r\n");
		strcat(buf, buf1);

	}
	strcat(buf, "Done.\r\n");
	if (ch->desc) {
		page_string(ch->desc, buf, 1);
	}
	return;
}


void cr0000_do_zset(struct char_data * ch, char *arg, int cmd)
{

	const char *lv_fields[] = {
		"zonelord",
		"reset_mode",
		"lifespan",
		"age",
		"x_coordinate",
		"y_coordinate",
		"z_coordinate",
		"\n",
	};

#define ZSET_ZONELORD     1
#define ZSET_RESET_MODE   2
#define ZSET_LIFESPAN     3
#define ZSET_RESET_AGE    4
#define ZSET_X_COORDINATE 5
#define ZSET_Y_COORDINATE 6
#define ZSET_Z_COORDINATE 7

	int lv_field_type, lv_stat_value, lv_old_value, lv_sign, zone;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	/* ****** */
	/* TYPE  */
	/* ***** */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("Sorry, but a field is required.\r\n", ch);
		send_to_char(buf2, ch);
		bzero(buf2, sizeof(buf2));
		send_to_char(ZSET_FORMAT, ch);
		return;
	}

	lv_field_type = old_search_block(buf, 0, strlen(buf), lv_fields, 0);
	if (lv_field_type < 1) {
		sprintf(buf2, "Sorry, but field: %s is invalid.\r\n", buf);
		send_to_char(buf2, ch);
		send_to_char("Use HELP ZSET for a list of fields.\r\n", ch);
		return;
	}

	/* ************ */
	/* FIELD VALUE  */
	/* ************ */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("A field value is required.\r\n", ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*buf == '-') {
		lv_sign = -1;
		strcpy(buf, (buf + 1));
	}

	/* IS VALUE NUMERIC */
	if (lv_field_type == ZSET_ZONELORD) {
		lv_stat_value = 0;
	}
	else {
		if (!is_number(buf)) {
			send_to_char("Field value must be numeric.\r\n", ch);
			send_to_char(MSET_FORMAT, ch);
			return;
		}
		lv_stat_value = atoi(buf) * lv_sign;
	}

	switch (lv_field_type) {

	case ZSET_ZONELORD:{
			CAN_PERSON_UPDATE(IMO_IMM);
			if (strlen(buf) > 15) {
				send_to_char("That name is a bit lengthy.\r\n", ch);
				return;
			}
			if (zone_table[zone].lord)
				free(zone_table[zone].lord);
			if (buf[0] == '0') {
				zone_table[zone].lord = strdup(" ");
				send_to_char("Removing zonelord.\r\n", ch);
			}
			else {
				zone_table[zone].lord = strdup(buf);
				send_to_char("Zonelord set.\r\n", ch);
			}
			break;
		}
	case ZSET_RESET_MODE:{
			CHECK_VALUE(0, 2)
				lv_old_value = zone_table[zone].reset_mode;
			zone_table[zone].reset_mode = lv_stat_value;
			break;
		}
	case ZSET_LIFESPAN:{
			CAN_PERSON_UPDATE(IMO_IMP);
			if (GET_LEVEL(ch) < IMO_IMP)
				CHECK_VALUE(25, 100)
					else
				CHECK_VALUE(1, 500)
					lv_old_value = zone_table[zone].lifespan;
			zone_table[zone].lifespan = lv_stat_value;
			break;
		}
	case ZSET_RESET_AGE:{
			CAN_PERSON_UPDATE(IMO_IMP);
			CHECK_VALUE(0, zone_table[zone].lifespan);
			lv_old_value = zone_table[zone].age;
			zone_table[zone].age = lv_stat_value;
			break;
		}
	case ZSET_X_COORDINATE:{
			CAN_PERSON_UPDATE(IMO_IMP);
			CHECK_VALUE(0, 255);
			lv_old_value = zone_table[zone].x_coordinate;
			zone_table[zone].x_coordinate = lv_stat_value;
			break;
		}
	case ZSET_Y_COORDINATE:{
			CAN_PERSON_UPDATE(IMO_IMP);
			CHECK_VALUE(0, 255);
			lv_old_value = zone_table[zone].y_coordinate;
			zone_table[zone].y_coordinate = lv_stat_value;
			break;
		}
	case ZSET_Z_COORDINATE:{
			CAN_PERSON_UPDATE(IMO_IMP);
			CHECK_VALUE(0, 255);
			lv_old_value = zone_table[zone].z_coordinate;
			zone_table[zone].z_coordinate = lv_stat_value;
			break;
		}
	default:{
			send_to_char("Hrmmm, Didn't find that field.\r\n",
				     ch);
			return;
		}
	}			/* END OF switch() */


	if (lv_field_type != ZSET_ZONELORD) {
		if (lv_stat_value == lv_old_value) {
			send_to_char("Value unchanged.\r\n", ch);
			return;
		}

		/* UPDATE DATABASE */
		bzero(buf, sizeof(buf));
		sprintf(buf, "Setting zone field %s from %d to %d.\r\n",
			lv_fields[lv_field_type - 1],
			lv_old_value,
			lv_stat_value);
		send_to_char(buf, ch);
	}

	SET_BIT(zone_table[zone].dirty_zon, UNSAVED);
	return;

}				/* END OF cr0000_do_zset() */


void do_zstat(struct char_data * ch, char *arg, int cmd)
{
	int i, zone;
	char buf[MAX_STRING_LENGTH];

	zone = i = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	if (!(zone_table[i].lord) || !*(zone_table[i].lord)) {
		sprintf(buf, "[%-20s] (%5d-%5d) No Lord\r\n",
			zone_table[i].filename,
			zone_table[i].bottom,
			zone_table[i].top);
	}
	else {
		sprintf(buf, "[%-20s] (%5d-%5d) zonelord[%s]\r\n",
			zone_table[i].filename,
			zone_table[i].bottom,
			zone_table[i].top,
			zone_table[i].lord);
	}
	send_to_char(buf, ch);
	sprintf(buf, "resetmode:[%3d]\r\n", zone_table[i].reset_mode);
	send_to_char(buf, ch);
	sprintf(buf, "lifespan: [%3d]\r\n", zone_table[i].lifespan);
	send_to_char(buf, ch);
	sprintf(buf, "age:      [%3d]\r\n", zone_table[i].age);
	send_to_char(buf, ch);

	sprintf(buf, "Coord:  x:[%3d] y:[%3d] z[%3d]\r\n",
		zone_table[i].x_coordinate,
		zone_table[i].y_coordinate,
		zone_table[i].z_coordinate);
	send_to_char(buf, ch);

	send_to_char("Zone flags: ", ch);
	sprintbit(zone_table[i].flags, zflag_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	sprintf(buf, "zcmds:    [%3d]\r\n", zone_table[i].reset_num);
	send_to_char(buf, ch);

	sprintf(buf, "\r\nrooms:    [%3d]\r\n",
		(!IS_SET(zone_table[i].dirty_wld, SCRAMBLED)
		 ? (zone_table[i].real_top - zone_table[i].real_bottom + 1)
		 : (zone_table[i].top - zone_table[i].bottom + 1)));
	send_to_char(buf, ch);

	sprintf(buf, "mobiles:  [%3d]\r\n",
		(!IS_SET(zone_table[i].dirty_mob, SCRAMBLED)
		 ? (zone_table[i].real_top - zone_table[i].real_bottom + 1)
		 : -1));
	send_to_char(buf, ch);

	sprintf(buf, "objects:  [%3d]\r\n",
		(!IS_SET(zone_table[i].dirty_obj, SCRAMBLED)
		 ? (zone_table[i].top_of_objt - zone_table[i].bot_of_objt + 1)
		 : -1));
	send_to_char(buf, ch);

	send_to_char("\r\n.zon flags: ", ch);
	sprintbit(zone_table[i].dirty_zon, dirty_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	send_to_char(".mob flags: ", ch);
	sprintbit(zone_table[i].dirty_mob, dirty_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	send_to_char(".obj flags: ", ch);
	sprintbit(zone_table[i].dirty_obj, dirty_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	return;

}				/* END OF do_zstat() */


void do_zflag(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	int loc, i, zone;
	unsigned long flag;

	zone = world[ch->in_room].zone;
	one_argument(arg, buf1);
	if (!*buf1) {
		send_to_char("Try zflag <flag>\r\n", ch);
		sprintf(buf, "Zone flags: \r\n");
		for (i = 0; *zflag_bits[i] != '\n'; i++) {
			strcat(buf, zflag_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf1, 0, strlen(buf1), zflag_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(zone_table[zone].flags, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", zflag_bits[loc]);
		REMOVE_BIT(zone_table[zone].flags, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", zflag_bits[loc]);
		SET_BIT(zone_table[zone].flags, flag);
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[zone].dirty_zon, UNSAVED);
}




void do_zreboot(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone, lv_was_in_room, idx;

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[zone].dirty_zon, SHOWN)) {
		send_to_char("ZCMDS are shown! Cancel or update changes first.\r\n", ch);
		return;
	}

	lv_was_in_room = ch->in_room;
	for (idx = MAXV(0, zone_table[zone].real_bottom);
	     idx <= top_of_world && idx <= zone_table[zone].real_top;
	     idx = world[idx].next) {
		ha1500_char_from_room(ch);
		ha1600_char_to_room(ch, idx);

		/* -1 bypassed error/auth check */
		wi2800_do_purge(ch, "\0", -1);
	}
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, lv_was_in_room);

	db5600_reset_zone(zone);
	sprintf(buf, "Done, %s *REBOOTED* (All rooms purged then reset)!",
		zone_table[zone].filename);
	send_to_char(buf, ch);
	return;

}				/* END OF do_zreboot() */


void do_zreset(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone;

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[zone].dirty_zon, SHOWN)) {
		send_to_char("ZCMDS are shown! Cancel or update changes first.\r\n", ch);
		return;
	}
	db5600_reset_zone(zone);
	sprintf(buf, "Done, %s reset!", zone_table[zone].filename);
	send_to_char(buf, ch);
}				/* proc */


void remove_zcmds(struct char_data * ch, int zone)
{
	/* turf all the zcmds */
	struct obj_data *tmp_obj, *next_obj;
	int i;

	if (!IS_SET(zone_table[zone].dirty_zon, SHOWN)) {
		send_to_char("Nothing to do.\r\n", ch);
		return;
	}
	/* scan the zone for zcmds */
	for (i = zone_table[zone].real_bottom; (i >= 0) && (i <= top_of_world) && (i <= zone_table[zone].real_top); i = world[i].next) {
		for (tmp_obj = world[i].contents; tmp_obj; tmp_obj = next_obj) {
			next_obj = tmp_obj->next_content;
			if (tmp_obj->obj_flags.type_flag == ITEM_ZCMD)
				ha2700_extract_obj(tmp_obj);
		}
	}
	send_to_char("Zone commands hidden.\r\n", ch);
	REMOVE_BIT(zone_table[zone].dirty_zon, SHOWN);
}

void update_zcmds(struct char_data * ch, int zone)
{
	/* turf old zcmd table and reload with visible one */
	struct obj_data *tmp_obj, *next_obj;
	int i, j = 0;

	if (!IS_SET(zone_table[zone].dirty_zon, SHOWN)) {
		send_to_char("Nothing to do.\r\n", ch);
		return;
	}
	/* lose existing table */
	free(zone_table[zone].cmd);
	ha9900_sanity_check(0, "FREE02", "SYSTEM");
	zone_table[zone].cmd = 0;

	/* scan the zone for zcmds */
	for (i = MAXV(0, zone_table[zone].real_bottom); (i <= top_of_world) && (i <= zone_table[zone].real_top); i = world[i].next) {
		for (tmp_obj = world[i].contents; tmp_obj; tmp_obj = next_obj) {
			next_obj = tmp_obj->next_content;
			if (tmp_obj->obj_flags.type_flag == ITEM_ZCMD) {
				/* make space for a new command */
				db2200_allocate_zcmd(zone, j + 1);
				ZCMD[j].command = (char) tmp_obj->obj_flags.value[0];
				ZCMD[j].arg1 = tmp_obj->obj_flags.value[1];
				ZCMD[j].arg2 = tmp_obj->obj_flags.value[2];
				/* if M, O, or R use room instead of value[3] */
				if (ZCMD[j].command == 'M' ||
				    ZCMD[j].command == 'O' ||
				    ZCMD[j].command == 'R')
					ZCMD[j].arg3 = i;
				else
					ZCMD[j].arg3 = tmp_obj->obj_flags.value[3];
				ZCMD[j].if_flag = tmp_obj->obj_flags.weight;
				j++;
			}
			ha2700_extract_obj(tmp_obj);
		}
	}

	/* add terminating entry */
	db2200_allocate_zcmd(zone, j + 1);
	ZCMD[j].command = 'S';
	ZCMD[j].arg1 = 0;
	ZCMD[j].arg2 = 0;
	ZCMD[j].arg3 = 0;
	ZCMD[j].if_flag = 0;

	REMOVE_BIT(zone_table[zone].dirty_zon, SHOWN);
	SET_BIT(zone_table[zone].dirty_zon, UNSAVED);
	send_to_char("Zone updated.\r\n", ch);
}


void show_zcmds(struct char_data * ch, int zone)
{
	/* show all the zcmds so that zone lords (only) can see them */
	int i, room;
	struct obj_data *tmp_obj;
	char buf[MAX_STRING_LENGTH], name[MAX_STRING_LENGTH];

	if (IS_SET(zone_table[zone].dirty_zon, SHOWN)) {
		send_to_char("Allready shown (to abort changes ZEDIT CANCEL).\r\n", ch);
		return;
	}
	for (i = 0; ZCMD[i].command != 'S'; i++) {	/* check against illegal
							 * zone stuff first off */
		if ((ZCMD[i].command != 'M')
		    && (ZCMD[i].command != 'O')
		    && (ZCMD[i].command != 'P')
		    && (ZCMD[i].command != 'G')
		    && (ZCMD[i].command != 'E')
		    && (ZCMD[i].command != 'R')
		    && (ZCMD[i].command != 'D')
			) {	/* if its not any of those then do nothing */
		}
		else {
			CREATE(tmp_obj, struct obj_data, 1);
			db7600_clear_object(tmp_obj);
			sprintf(name, "obj zcmd %c ", ZCMD[i].command);
			tmp_obj->obj_flags.type_flag = ITEM_ZCMD;
			tmp_obj->obj_flags.wear_flags = 0;	/* Not takeable */
			if (gv_port != ZONING_PORT) {
				tmp_obj->obj_flags.flags1 = OBJ1_NO_SEE;
			}
			tmp_obj->obj_flags.value[0] = (char) ZCMD[i].command;
			tmp_obj->obj_flags.value[1] = ZCMD[i].arg1;
			tmp_obj->obj_flags.value[2] = ZCMD[i].arg2;
			tmp_obj->obj_flags.value[3] = ZCMD[i].arg3;
			tmp_obj->obj_flags.weight = ZCMD[i].if_flag;
			tmp_obj->obj_flags.cost = -1;
			tmp_obj->obj_flags.cost_per_day = -1;
			tmp_obj->next = object_list;
			object_list = tmp_obj;
			tmp_obj->item_number = -1;

			if (ZCMD[i].if_flag) {
				sprintf(buf, "(IF) ");
				sprintf(name + strlen(name), "if ");
			}
			else
				sprintf(buf, "ZCMD ");
			switch (ZCMD[i].command) {
			case 'M':
				sprintf(buf + strlen(buf), "[Mob] Mob[%5d]", mob_index[ZCMD[i].arg1].virtual);
				sprintf(buf + strlen(buf), " (Max[%4d]) ", ZCMD[i].arg2);
				strcat(buf, ha1100_fname(mob_index[ZCMD[i].arg1].name));
				strcat(name, ha1100_fname(mob_index[ZCMD[i].arg1].name));
				room = ZCMD[i].arg3;
				break;

			case 'O':
				sprintf(buf + strlen(buf), "[Obj] Obj[%5d]", obj_index[ZCMD[i].arg1].virtual);
				sprintf(buf + strlen(buf), " (Max[%4d]) ", ZCMD[i].arg2);
				strcat(buf, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				strcat(name, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				room = ZCMD[i].arg3;
				break;

			case 'P':
				sprintf(buf + strlen(buf), "[Put] Obj[%5d]", obj_index[ZCMD[i].arg1].virtual);
				sprintf(buf + strlen(buf), " (Max[%4d])", ZCMD[i].arg2);
				sprintf(buf + strlen(buf), " in Obj:[%5d] ", obj_index[ZCMD[i].arg3].virtual);
				strcat(buf, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				strcat(name, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				break;

			case 'G':
				sprintf(buf + strlen(buf), "[Giv] Obj[%5d]", obj_index[ZCMD[i].arg1].virtual);
				sprintf(buf + strlen(buf), " (Max[%4d]) ", ZCMD[i].arg2);
				strcat(buf, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				strcat(name, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				break;

			case 'E':
				sprintf(buf + strlen(buf), "[Equ] Obj[%5d]", obj_index[ZCMD[i].arg1].virtual);
				sprintf(buf + strlen(buf), " (Max[%4d]) ", ZCMD[i].arg2);
				strcat(buf, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				strcat(name, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				if ((ZCMD[i].arg3 >= 0) && (ZCMD[i].arg3 < MAX_WEAR))
					sprintf(buf + strlen(buf), " %s", loc[ZCMD[i].arg3]);
				else
					sprintf(buf + strlen(buf), " <ERROR!>");
				break;

			case 'R':
				if (ZCMD[i].arg2 == 1) {
					sprintf(buf + strlen(buf), "[Rem] Obj[%5d]", obj_index[ZCMD[i].arg1].virtual);
					sprintf(buf + strlen(buf), " ([Object]) ");
					strcat(buf, ha1100_fname(obj_index[ZCMD[i].arg1].name));
					strcat(name, ha1100_fname(obj_index[ZCMD[i].arg1].name));
				}
				else {
					sprintf(buf + strlen(buf), "[Rem] Mob[%5d]", mob_index[ZCMD[i].arg1].virtual);
					sprintf(buf + strlen(buf), " ([Mobile]) ");
					strcat(buf, ha1100_fname(mob_index[ZCMD[i].arg1].name));
					strcat(name, ha1100_fname(mob_index[ZCMD[i].arg1].name));
				}
				room = ZCMD[i].arg3;
				break;

			case 'D':
				sprintf(buf + strlen(buf), "[Door] dir[%s] = %d", dirs[ZCMD[i].arg2], ZCMD[i].arg3);
				strcat(name, "door");
				room = ZCMD[i].arg1;
				break;

			default:	/* what the hell could this be? */
				sprintf(buf + strlen(buf), "%c %d %d %d (%d)", ZCMD[i].command, ZCMD[i].arg1, ZCMD[i].arg2, ZCMD[i].arg3, ZCMD[i].if_flag);
				break;
			}

			tmp_obj->description = strdup(buf);
			if (room == ch->in_room) {
				send_to_char(buf, ch);
				send_to_char("\r\n", ch);
			}
			buf[21] = 0;
			tmp_obj->short_description = strdup(buf);
			tmp_obj->name = strdup(name);

			ha2100_obj_to_room(tmp_obj, room);
		}
	}

/* mark it as shown and exit */
	send_to_char("Done. (zone commands shown)\r\n", ch);
	SET_BIT(zone_table[zone].dirty_zon, SHOWN);
}

void do_zedit(struct char_data * ch, char *arg, int cmd)
{
	int zone, room;
	char buf[MAX_STRING_LENGTH];


	room = ch->in_room;
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Try zedit <show || cancel || update>.\r\n", ch);
		return;
	}
	if (is_abbrev(buf, "show"))
		show_zcmds(ch, zone);
	else if (is_abbrev(buf, "cancel"))
		remove_zcmds(ch, zone);
	else if (is_abbrev(buf, "update")) {
		update_zcmds(ch, zone);
	}
	else
		send_to_char("Try zedit <show || cancel || update>.\r\n", ch);
}				/* proc */


void do_zfirst(struct char_data * ch, char *arg, int cmd)
{
	int zone, room;
	struct obj_data *obj;
	char buf[MAX_STRING_LENGTH];


	room = ch->in_room;
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Try zfirst <ZCMD>.\r\n", ch);
		return;
	}
	if ((obj = ha2000_get_obj_list(buf, world[room].contents))) {
		if (obj->obj_flags.type_flag == ITEM_ZCMD) {
			ha2200_obj_from_room(obj);
			/* now put at front of room, ha2100_obj_to_room would
			 * put at end of ZCMDS */
			obj->in_room = room;
			obj->carried_by = NULL;
			obj->next_content = world[room].contents;
			world[room].contents = obj;
			sprintf(buf, "%s moved to front.\r\n", obj->description);
			send_to_char(buf, ch);
		}
		else {
			send_to_char("Thats no zone command\r\n", ch);
			return;
		}
	}
	else
		send_to_char("What ZCMD was that again?\r\n", ch);
}				/* proc */


void do_zmax(struct char_data * ch, char *arg, int cmd)
{
	int zone, room, arg1;
	struct obj_data *obj;
	char buf[MAX_STRING_LENGTH];


	room = ch->in_room;
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Try zmax <ZCMD> <NEW_MAX>.\r\n", ch);
		return;
	}
	if ((obj = ha2000_get_obj_list(buf, world[room].contents))) {
		if (obj->obj_flags.type_flag == ITEM_ZCMD) {
			/* update max field and desc */
			if ((char) obj->obj_flags.value[0] != 'D') {
				arg = one_argument(arg, buf);
				arg1 = atoi(buf);
				if (arg1 <= 0)
					arg1 = 1;	/* min of 1 */
				obj->obj_flags.value[2] = arg1;
				sprintf(buf, "%4d", arg1);
				if (strlen(obj->description) > 31)
					strncpy(obj->description + 27, buf, 4);
				sprintf(buf, "Now %s.\r\n", obj->description);
				send_to_char(buf, ch);
			}
			else {
				send_to_char("Doors dont have max's?!?\r\n", ch);
			}
		}
		else {
			send_to_char("Thats no zone command\r\n", ch);
			return;
		}
	}
	else {
		send_to_char("What ZCMD was that again?\r\n", ch);
	}

	return;

}				/* END OF do_zmax() */


void do_zcreate(struct char_data * ch, char *arg, int cmd)
{
	int zone, i, room;
	struct obj_data *tmp_obj;
	char buf[MAX_STRING_LENGTH];	/* must be big enuff to store entire
					 * loc array && >= maxinputlen */
	char name[MAX_STRING_LENGTH];
	char desc[MAX_STRING_LENGTH];
	char command;
	int arg1, arg2, arg3, if_flag;


	room = ch->in_room;
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf);
	if (!*arg) {
		sprintf(buf, "zhelp_zcreate");
		in3500_do_help(ch, buf, CMD_HELP);
		return;
	}

	/* ******* */
	/* MOBILE */
	/* ******* */
	if (is_abbrev(buf, "mobile")) {
		/* default IF flag setting */
		sprintf(desc, "ZCMD ");
		if_flag = 0;
		command = 'M';

		/* what mob are we */
		arg = one_argument(arg, buf);
		arg1 = db8100_real_mobile(atoi(buf));
		if (arg1 < 0) {
			send_to_char("What mob was that again?.\r\n", ch);
			return;
		}

		zone = mob_index[arg1].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "[Mob] Mob[%5d]", mob_index[arg1].virtual);
		sprintf(name, ha1100_fname(mob_index[arg1].name));

		/* how many are there */
		arg = one_argument(arg, buf);
		arg2 = atoi(buf);
		if (arg2 < 1)
			arg2 = 1;	/* must be at least one */
		sprintf(desc + strlen(desc), " (Max[%4d]) ", arg2);
		strcat(desc, ha1100_fname(mob_index[arg1].name));
		strcat(name, " mobile");

		/* where are we */
		arg3 = room;

	}			/* END OF mobile */
	/* ******** */
	/* OBJECT  */
	/* ******** */
	else if (is_abbrev(buf, "object")) {
		/* default IF flag setting */
		sprintf(desc, "ZCMD ");
		if_flag = 0;
		command = 'O';

		/* what obj are we */
		arg = one_argument(arg, buf);
		arg1 = db8200_real_object(atoi(buf));
		if (arg1 < 0) {
			send_to_char("What obj was that again?.\r\n", ch);
			return;
		}

		zone = obj_index[arg1].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "[Obj] Obj[%5d]", obj_index[arg1].virtual);
		sprintf(name, ha1100_fname(obj_index[arg1].name));
		strcat(name, " object");

		/* how many are there */
		arg = one_argument(arg, buf);
		arg2 = atoi(buf);
		if (arg2 < 1)
			arg2 = 1;	/* must be at least one */
		/* SAFTY CHECK */
		if (arg2 > 10 &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("Sorry, but MAX must be from 1 to 10.\r\n", ch);
			return;
		}

		sprintf(desc + strlen(desc), " (Max[%4d]) ", arg2);
		strcat(desc, ha1100_fname(obj_index[arg1].name));

		/* where are we */
		arg3 = room;
	}			/* END OF object */


	/* ******** */
	/* GIVE   */
	/* ******** */
	else if (is_abbrev(buf, "give")) {
		/* default IF flag setting */
		sprintf(desc, "(IF) ");
		if_flag = 1;
		command = 'G';

		/* what obj are we */
		arg = one_argument(arg, buf);
		arg1 = db8200_real_object(atoi(buf));
		if (arg1 < 0) {
			send_to_char("What obj was that again?.\r\n", ch);
			return;
		}
		zone = obj_index[arg1].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "[Giv] Obj[%5d]", obj_index[arg1].virtual);
		sprintf(name, ha1100_fname(obj_index[arg1].name));
		strcat(name, " give");

		/* how many are there */
		arg = one_argument(arg, buf);
		arg2 = atoi(buf);
		if (arg2 < 1)
			arg2 = 1;	/* must be at least one */
		/* SAFTY CHECK */
		if (arg2 > 10 &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("Sorry, but MAX must be from 1 to 10.\r\n", ch);
			return;
		}

		sprintf(desc + strlen(desc), " (Max[%4d]) ", arg2);
		strcat(desc, ha1100_fname(obj_index[arg1].name));

		/* where are we */
		arg3 = room;	/* not actually used for this command */
	}			/* END OF give */
	/* ******** */
	/* PUT   */
	/* ******** */
	else if (is_abbrev(buf, "put")) {
		/* default IF flag setting */
		sprintf(desc, "(IF) ");
		if_flag = 1;
		command = 'P';

		/* what obj are we */
		arg = one_argument(arg, buf);
		arg1 = db8200_real_object(atoi(buf));
		if (arg1 < 0) {
			send_to_char("What obj was that again?.\r\n", ch);
			return;
		}

		zone = obj_index[arg1].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "[Put] Obj[%5d]", obj_index[arg1].virtual);
		sprintf(name, ha1100_fname(obj_index[arg1].name));
		strcat(name, " put");

		/* how many are there */
		arg = one_argument(arg, buf);
		arg2 = atoi(buf);
		if (arg2 < 1)
			arg2 = 1;	/* must be at least one */
		/* SAFTY CHECK */
		if (arg2 > 10 &&
		    GET_LEVEL(ch) < IMO_IMP) {
			send_to_char("Sorry, but MAX must be from 1 to 10.\r\n", ch);
			return;
		}
		sprintf(desc + strlen(desc), " (Max[%4d]) ", arg2);

		/* in which obj? */
		arg = one_argument(arg, buf);
		arg3 = db8200_real_object(atoi(buf));
		if (arg3 < 0) {
			send_to_char("Put it in what obj?.\r\n", ch);
			return;
		}
		zone = obj_index[arg3].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "in Obj[%5d] ", obj_index[arg3].virtual);
		strcat(desc, ha1100_fname(obj_index[arg1].name));

	}			/* END OF put */
	/* ******** */
	/* REMOVE  */
	/* ******** */
	else if (is_abbrev(buf, "remove")) {
		/* default IF flag setting */
		sprintf(desc, "ZCMD ");
		if_flag = 0;
		command = 'R';

		/* what obj/mob are we */
		arg = one_argument(arg, buf);
		arg1 = atoi(buf);
		if (arg1 < 0) {
			send_to_char("What was that object/mobile again?\r\n", ch);
			return;
		}

		/* Are we working with OBJECTS or MOBILES? */
		arg = one_argument(arg, buf);
		arg2 = 0;
		if (is_abbrev(buf, "object"))
			arg2 = ZCREATE_OBJECT;
		else {
			if (is_abbrev(buf, "mobile") ||
			    is_abbrev(buf, "character"))
				arg2 = ZCREATE_MOBILE;
			else
				arg2 = atoi(buf);
		}

		if (arg2 != ZCREATE_OBJECT &&
		    arg2 != ZCREATE_MOBILE) {
			send_to_char("Sorry, but you must specify a valid type.\r\n",
				     ch);
			return;
		}

		if (arg2 == ZCREATE_OBJECT) {
			arg1 = db8200_real_object(arg1);
			if (arg1 < 0) {
				send_to_char("What obj was that again?.\r\n", ch);
				return;
			}

			zone = obj_index[arg1].zone;
			CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
			LIMIT_TO_ZONE();
			sprintf(desc + strlen(desc), "[Rem] Obj[%5d]",
				obj_index[arg1].virtual);
			sprintf(name, ha1100_fname(obj_index[arg1].name));
			strcat(name, " object");
			sprintf(desc + strlen(desc), " ([Object]) ");
			strcat(desc, ha1100_fname(obj_index[arg1].name));
		}

		if (arg2 == ZCREATE_MOBILE) {
			arg1 = db8100_real_mobile(arg1);
			if (arg1 < 0) {
				send_to_char("What mob was that again?.\r\n", ch);
				return;
			}

			zone = mob_index[arg1].zone;
			CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
			LIMIT_TO_ZONE();
			sprintf(desc + strlen(desc), "[Rem] Mob[%5d]",
				mob_index[arg1].virtual);
			sprintf(name, ha1100_fname(mob_index[arg1].name));
			strcat(name, " mofile");
			sprintf(desc + strlen(desc), " ([Mobile]) ");
			strcat(desc, ha1100_fname(mob_index[arg1].name));
		}

		/* where are we */
		arg3 = room;
	}			/* END OF remove */
	/* ******** */
	/* DOOR   */
	/* ******** */
	else if (is_abbrev(buf, "door")) {
		/* default IF flag setting */
		sprintf(desc, "ZCMD ");
		if_flag = 0;
		command = 'D';

		arg = one_argument(arg, buf);
		arg2 = (old_search_block(buf, 0, strlen(buf), dirs, 0));
		if (arg2 == -1) {	/* the 0 above allows for substr match */
			send_to_char("That direction doesn't exist.\r\n", ch);
			return;
		}

		/* MAKE SURE WE HAVE AN EXIT */
		if (!EXIT(ch, arg2 - 1)) {
			send_to_char("Sorry, but there is no exit in that direction.\r\n", ch);
			return;
		}

		arg2--;
		arg = one_argument(arg, buf);
		arg3 = atoi(buf);
		if (arg3 < 0 || arg3 > 2) {
			send_to_char("Invalid door state.\r\n", ch);
			return;
		}
		sprintf(desc + strlen(desc), "[Door] dir[%s] = %d", dirs[arg2], arg3);
		strcpy(name, "door");
		/* where are we */
		arg1 = room;
	}			/* END OF door */
	/* ******** */
	/* EQUIP   */
	/* ******** */
	else if (is_abbrev(buf, "equip")) {
		/* default IF flag setting */
		sprintf(desc, "(IF) ");
		if_flag = 1;
		command = 'E';

		/* what obj are we */
		arg = one_argument(arg, buf);
		arg1 = db8200_real_object(atoi(buf));
		if (arg1 < 0) {
			send_to_char("What obj was that again?.\r\n", ch);
			return;
		}
		zone = obj_index[arg1].zone;
		CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);
		LIMIT_TO_ZONE();

		sprintf(desc + strlen(desc), "[Equ] Obj[%5d]", obj_index[arg1].virtual);
		sprintf(name, ha1100_fname(obj_index[arg1].name));
		strcat(name, " equip");

		/* how many are there */
		arg = one_argument(arg, buf);
		arg2 = atoi(buf);

		/* THIS VALUE IS WORTHLESS - ZERO IT */
		arg2 = 0;

		sprintf(desc + strlen(desc), " (Max[%4d]) ", arg2);
		strcat(desc, ha1100_fname(obj_index[arg1].name));

		/* where do we equip it */
		arg = one_argument(arg, buf);
		if (*buf)
			arg3 = (old_search_block(buf, 0, strlen(buf), loc, 0));
		else
			arg3 = -1;
		arg3--;
		if ((arg3 >= 0) && (arg3 < MAX_WEAR))
			sprintf(desc + strlen(desc), " %s", loc[arg3]);
		else {
			send_to_char("invalid location specified! choose from:\r\n", ch);
			buf[0] = '\0';
			for (i = 0; *loc[i] != '\n'; i++) {
				strcat(buf, loc[i]);
				strcat(buf, " ");
			}
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
			return;
		}

	}			/* END OF equipment */
	else {
		send_to_char("Nice one, never heard of that ZCMD before.\r\n", ch);
		return;
	}

	/* check for if flag override */
	arg = one_argument(arg, buf);
	if (*arg) {
		if (is_abbrev(buf, "if")) {
			strncpy(desc, "(IF)", 4);
			if_flag = 1;
		}
		else if (is_abbrev(buf, "!if")) {
			strncpy(desc, "ZCMD", 4);
			if_flag = 0;
		}
	}
	if (if_flag)
		strcat(name, " if");


	/* create the object */
	CREATE(tmp_obj, struct obj_data, 1);
	db7600_clear_object(tmp_obj);
	tmp_obj->obj_flags.type_flag = ITEM_ZCMD;
	tmp_obj->obj_flags.wear_flags = 0;	/* Not takeable */
	tmp_obj->obj_flags.flags1 = OBJ1_NO_SEE;
	tmp_obj->obj_flags.value[0] = (char) command;
	tmp_obj->obj_flags.value[1] = arg1;
	tmp_obj->obj_flags.value[2] = arg2;
	tmp_obj->obj_flags.value[3] = arg3;
	tmp_obj->obj_flags.weight = if_flag;
	tmp_obj->obj_flags.cost = -1;
	tmp_obj->obj_flags.cost_per_day = -1;
	tmp_obj->next = object_list;
	object_list = tmp_obj;
	tmp_obj->item_number = -1;
	tmp_obj->description = strdup(desc);
	buf[21] = 0;
	tmp_obj->short_description = strdup(desc);
	sprintf(buf, "obj zcmd %s", name);
	tmp_obj->name = strdup(buf);

	/* append object to list of zone objects in room */
	ha2100_obj_to_room(tmp_obj, room);

	sprintf(buf, "%s created.\r\n(ZEDIT UPDATE then ZREBOOT for changes to take affect)\r\n", desc);
	send_to_char(buf, ch);
	return;

}				/* END OF do_zcreate() */


void do_zsave(struct char_data * ch, char *arg, int cmd)
{
	int zone;
	char buf[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[world[ch->in_room].zone].dirty_zon, UNSAVED)) {
		db9000_save_zon(world[ch->in_room].zone);
		sprintf(buf, "areas/%s.zon saved.\r\n", zone_table[world[ch->in_room].zone].filename);
	}
	else {
		sprintf(buf, "%s.zon hasn't changed.\r\n", zone_table[world[ch->in_room].zone].filename);
	}
	send_to_char(buf, ch);
	return;
}				/* END OF do_zsave() */



/*****************************************************************
 *                                                               *
 *                                                               *
 *                     W L D    S T U F F                        *
 *                                                               *
 *                                                               *
 *****************************************************************/

void do_rhelp(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];


	bzero(buf, sizeof(buf));
	for (; *arg == ' '; arg++);
	if (*arg)
		sprintf(buf, "rhelp_%s", arg);
	else
		strcpy(buf, "rhelp");
	in3500_do_help(ch, buf, CMD_HELP);

	return;

}				/* END OF do_rhelp() */


void do_rstat(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int zone, rc;
	struct extra_descr_data *desc;
	int loc_nr, location, i;
	extern int top_of_world;


	if (IS_NPC(ch))
		return;

	one_argument(arg, buf);
	if (!*buf) {
		location = ch->in_room;
	}
	else {
		if (isdigit(*buf)) {
			loc_nr = atoi(buf);
			for (location = 0; location <= top_of_world; location++)
				if (world[location].number == loc_nr)
					break;
				else if (location == top_of_world) {
					send_to_char("No room exists with that number.\r\n", ch);
					return;
				}
		}
		else {
			send_to_char("Not a valid room number.\r\n", ch);
			return;
		}
	}

	/* a location has been found. */
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	rc = li3000_is_blocked_from_room(ch, location, 0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	/* output the results of the stat */
	sprinttype(world[location].sector_type, sector_types, buf2);
	sprintf(buf, "#:[%d] Name:[%s] Type:[%s] Spec:[%s]\r\n",
		world[location].number, world[location].name, buf2,
		(world[location].funct) ? "Y" : "N");
	send_to_char(buf, ch);

	send_to_char("Flags: ", ch);
	sprintbit((long) world[location].room_flags, room_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("       ", ch);
	sprintbit((long) world[location].room_flags2, room_bits2, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("Description:\r\n", ch);
	send_to_char(world[location].description, ch);

	for (desc = world[location].ex_description; desc; desc = desc->next) {
		sprintf(buf, "Extra Desc. /w/ Keywords:[%s]\r\n", desc->keyword);
		send_to_char(buf, ch);
	}

	send_to_char("\r\nExits:\r\n", ch);
	for (i = 0; i <= 5; i++) {
		if (world[location].dir_option[i]) {
			sprintbit(world[location].dir_option[i]->exit_info, exit_bits, buf2);
			sprintf(buf, "[%s] to room:[%d], flags:[%s]\r\n",
				dirs[i], world[world[location].dir_option[i]->to_room].number, buf2);
			send_to_char(buf, ch);
			if (world[location].dir_option[i]->keyword) {
				sprintf(buf, "Key:[%d] Keywords:[%s]\r\n",
					world[location].dir_option[i]->key, world[location].dir_option[i]->keyword);
				send_to_char(buf, ch);
			}

			if (world[location].dir_option[i]->general_description) {
				send_to_char(world[location].dir_option[i]->general_description, ch);
			}
		}		/* END OF dir_option[0] */
	}			/* END OF MAX_DIRS */

	sprintf(buf, "Minimum level %d maximum level %d.\r\n",
		world[ch->in_room].min_level,
		world[ch->in_room].max_level);
	send_to_char(buf, ch);
	sprintf(buf, "RSTAT: %s did rstat %d.", GET_REAL_NAME(ch), world[location].number);
	spec_log(buf, GOD_COMMAND_LOG);

	return;
}				/* END OF do_rstat() */


void cr3000_do_rlevel(struct char_data * ch, char *arg, int cmd)
{

	int lv_min_level, lv_max_level, lv_sign, zone;
	char buf[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* ************* */
	/* MINIMUM LEVEL */
	/* ************* */
	for (; isspace(*arg); arg++);

	/* WE NEED A MIN LEVEL */
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*arg)) {
		bzero(buf, sizeof(buf));
		send_to_char("Format rlevel <min level> <max level>.\r\n",
			     ch);
		sprintf(buf, "Rooms min: %d max: %d.\r\n",
			world[ch->in_room].min_level,
			world[ch->in_room].max_level);
		send_to_char(buf, ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Format rlevel <min level> <max level>.\r\n",
			     ch);
		return;
	}

	lv_min_level = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_min_level < 0 || lv_min_level > 99)) {
		send_to_char("Minimum level must be between 0 and 99.\r\n", ch);
		return;
	}

	/* ************* */
	/* MAXIMUM LEVEL */
	/* ************* */
	for (; isspace(*arg); arg++);

	/* WE NEED A MAX LEVEL */
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Format rlevel <min level> <max level>.\r\n",
			     ch);
		return;
	}

	lv_max_level = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_max_level < 0 || lv_max_level > 99)) {
		send_to_char("Maxmimum level must be between 0 and 99.\r\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "Rooms min: %d max: %d.  old values min: %d max %d.\r\n",
		lv_min_level,
		lv_max_level,
		world[ch->in_room].min_level,
		world[ch->in_room].max_level);
	send_to_char(buf, ch);

	world[ch->in_room].min_level = lv_min_level;
	world[ch->in_room].max_level = lv_max_level;
	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);

	return;

}				/* END OF  cr3000_do_rlevel() */


void do_rgoto(struct char_data * ch, char *argument, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int loc_nr, location;
	extern int top_of_world;
	int rc, zone;


	if (IS_NPC(ch))
		return;

	one_argument(argument, buf);
	if (!*buf) {
		send_to_char("You must supply a room number.\r\n", ch);
		return;
	}

	if (isdigit(*buf)) {
		loc_nr = atoi(buf);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\r\n", ch);
				return;
			}
	}
	else {
		send_to_char("Not a valid room number.\r\n", ch);
		return;
	}

	/* a location has been found. */
	zone = world[location].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	rc = li3000_is_blocked_from_room(ch, location, 0);
	if (rc) {
		li3100_send_blocked_text(ch, rc, 0);
		return;
	}

	if (ch->player.immortal_exit) {
		strcpy(buf, ch->player.immortal_exit);
	}
	else {
		strcpy(buf, "$n disappears in a puff of smoke.");
	}
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, location);
	if (ch->player.immortal_enter)
		strcpy(buf, ch->player.immortal_enter);
	else
		strcpy(buf, "$n magically appears in the middle of the room.");
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	in3000_do_look(ch, "", 0);

}				/* END OF do_rgoto() */


void do_rcopy(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone, tmp;
	struct extra_descr_data *theExtra, *new_descr;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	tmp = zone_table[zone].bottom;
	main_log("Looking for unallocated room for rcopy");
	while (db8000_real_room(tmp) > -1)
		tmp++;

	if (tmp > zone_table[zone].top) {
		send_to_char("Sorry but this zone is full... nothing more can be added\r\n", ch);
		return;
	}
	top_of_world++;
	db2100_allocate_room(top_of_world);
	SET_BIT(zone_table[zone].dirty_wld, UNSAVED);
	SET_BIT(zone_table[zone].dirty_wld, SCRAMBLED);
	world[top_of_world].number = tmp;
	if (tmp == zone_table[zone].bottom) {
		/* insert at beginning of zone */
		world[top_of_world].next = zone_table[zone].real_bottom;
		if (zone > 0)
			world[zone_table[zone - 1].real_top].next = top_of_world;
		zone_table[zone].real_bottom = top_of_world;
	}
	else {
		if (db8000_real_room(tmp - 1) == zone_table[zone].real_top) {
			/* insert end */
			world[top_of_world].next = world[db8000_real_room(tmp - 1)].next;
			world[db8000_real_room(tmp - 1)].next = top_of_world;
			zone_table[zone].real_top = top_of_world;
		}
		else {
			/* insert somewhere in the middle */
			world[top_of_world].next = world[db8000_real_room(tmp - 1)].next;
			world[db8000_real_room(tmp - 1)].next = top_of_world;
		}
	}
	world[zone_table[top_of_zone_table].real_top].next = top_of_world + 1;

	world[top_of_world].zone = zone;

	if (world[ch->in_room].name)
		world[top_of_world].name = strdup(world[ch->in_room].name);
	else
		world[top_of_world].name = (char *) 0;
	if (world[ch->in_room].description)
		world[top_of_world].description = strdup(world[ch->in_room].description);
	else
		world[top_of_world].description = (char *) 0;
	world[top_of_world].room_flags = world[ch->in_room].room_flags;
	world[top_of_world].sector_type = world[ch->in_room].sector_type;
	world[top_of_world].funct = 0;
	world[top_of_world].contents = 0;
	world[top_of_world].people = 0;
	world[top_of_world].light = 0;	/* Zero light sources */
	world[top_of_world].min_level = 0;
	world[top_of_world].max_level = 99;

	/* dont copy directions */
	for (tmp = 0; tmp <= 5; tmp++)
		world[top_of_world].dir_option[tmp] = 0;

	/* but copy the extra descriptions (never know I guess) */
	world[top_of_world].ex_description = 0;
	for (theExtra = world[ch->in_room].ex_description;
	     theExtra;
	     theExtra = theExtra->next) {
		CREATE(new_descr, struct extra_descr_data, 1);
		if (theExtra->keyword)
			new_descr->keyword = strdup(theExtra->keyword);
		else
			new_descr->keyword = (char *) 0;
		if (theExtra->description)
			new_descr->description = strdup(theExtra->description);
		else
			new_descr->description = (char *) 0;
		new_descr->next = world[top_of_world].ex_description;
		world[top_of_world].ex_description = new_descr;
	}			/* for loop */
	ha1500_char_from_room(ch);
	ha1600_char_to_room(ch, top_of_world);
	sprintf(buf, "Done, room #%d created, and you're now in it!", world[top_of_world].number);
	send_to_char(buf, ch);
}				/* END OF do_rcopy() */
/* THIS ROUTINE RETURNS THE CREATED ROOM.  IF NO ROOM IS ALLOCATED */
/* USER IS NOTIFIED */
int do_rcopy_for_vehicle(struct char_data * ch, char *arg, int cmd)
{
	int zone, tmp, lv_room;
	struct extra_descr_data *theExtra, *new_descr;


	lv_room = db8000_real_room(VEHICLE_BEG_ROOM);
	zone = world[lv_room].zone;

	tmp = zone_table[zone].bottom;
	main_log("Looking for unallocated room for rcopy for vehicle");
	while (db8000_real_room(tmp) > -1)
		tmp++;

	if (tmp > zone_table[zone].top) {
		send_to_char("Sorry but there is no ROOM in there!\r\n", ch);
		return (0);
	}

	top_of_world++;
	db2100_allocate_room(top_of_world);
	SET_BIT(zone_table[zone].dirty_wld, UNSAVED);
	SET_BIT(zone_table[zone].dirty_wld, SCRAMBLED);
	world[top_of_world].number = tmp;
	if (tmp == zone_table[zone].bottom) {
		/* insert at beginning of zone */
		world[top_of_world].next = zone_table[zone].real_bottom;
		if (zone > 0)
			world[zone_table[zone - 1].real_top].next = top_of_world;
		zone_table[zone].real_bottom = top_of_world;
	}
	else {
		if (db8000_real_room(tmp - 1) == zone_table[zone].real_top) {
			/* insert end */
			world[top_of_world].next = world[db8000_real_room(tmp - 1)].next;
			world[db8000_real_room(tmp - 1)].next = top_of_world;
			zone_table[zone].real_top = top_of_world;
		}
		else {
			/* insert somewhere in the middle */
			world[top_of_world].next = world[db8000_real_room(tmp - 1)].next;
			world[db8000_real_room(tmp - 1)].next = top_of_world;
		}
	}
	world[zone_table[top_of_zone_table].real_top].next = top_of_world + 1;

	world[top_of_world].zone = zone;

	if (world[lv_room].name)
		world[top_of_world].name = strdup(world[lv_room].name);
	else
		world[top_of_world].name = (char *) 0;
	if (world[lv_room].description)
		world[top_of_world].description = strdup(world[lv_room].description);
	else
		world[top_of_world].description = strdup("Undefined");
	world[top_of_world].room_flags = world[lv_room].room_flags;
	world[top_of_world].sector_type = world[lv_room].sector_type;
	world[top_of_world].funct = vh3000_vehicle_proc;
	world[top_of_world].contents = 0;
	world[top_of_world].people = 0;
	world[top_of_world].light = 0;	/* Zero light sources */
	world[top_of_world].min_level = world[lv_room].min_level;
	world[top_of_world].max_level = world[lv_room].max_level;

	/* dont copy directions */
	for (tmp = 0; tmp <= 5; tmp++)
		world[top_of_world].dir_option[tmp] = 0;

	/* but copy the extra descriptions (never know I guess) */
	world[top_of_world].ex_description = 0;
	for (theExtra = world[lv_room].ex_description;
	     theExtra;
	     theExtra = theExtra->next) {
		CREATE(new_descr, struct extra_descr_data, 1);
		if (theExtra->keyword)
			new_descr->keyword = strdup(theExtra->keyword);
		else
			new_descr->keyword = (char *) 0;
		if (theExtra->description)
			new_descr->description = strdup(theExtra->description);
		else
			new_descr->description = (char *) 0;
		new_descr->next = world[top_of_world].ex_description;
		world[top_of_world].ex_description = new_descr;
	}			/* for loop */

	return (top_of_world);

}				/* END OF do_rcopy_for_vehicle() */


/* link one room to another, destroy old link if necessary */
void do_rlink(struct char_data * ch, char *arg, int cmd)
{
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	int dir, zone, lv_rc;
	int cha_rm, tar_rm;


	cha_rm = ch->in_room;
	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf1);
	if (!*buf1) {
		send_to_char("Try rlink <dir> (<room#> || delete)\r\n", ch);
		return;
	}
	half_chop(arg, buf2, buf3);

	dir = (old_search_block(buf1, 0, strlen(buf1), dirs, 0));
	if (dir == -1) {
		send_to_char("That type doesn't exist.\r\n", ch);
		return;
	}
	dir--;			/* search block returns 1 as first position */

	if (dir == -1) {
		send_to_char("What direction is that?\r\n", ch);
		return;
	}
	if (*buf2 && is_abbrev(buf2, "delete")) {
		if (world[ch->in_room].dir_option[dir]) {
			if (world[ch->in_room].dir_option[dir]->general_description) {
				free((char *) world[ch->in_room].dir_option[dir]->general_description);
			}
			if (world[ch->in_room].dir_option[dir]->keyword) {
				free((char *) world[ch->in_room].dir_option[dir]->keyword);
			}
			free((char *) world[ch->in_room].dir_option[dir]);
			world[ch->in_room].dir_option[dir] = 0;
			SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
		}
		return;
	}
	if (!(is_number(buf2))) {
		send_to_char("Please enter a valid room number!\r\n", ch);
		return;
	}
	tar_rm = db8000_real_room(atoi(buf2));

	if ((tar_rm < 0) || (tar_rm > top_of_world)) {
		send_to_char("There's no room of that number.\r\n", ch);
		return;
	}
	if ((world[tar_rm].zone != world[cha_rm].zone)
	    && (GET_LEVEL(ch) < IMO_IMM)) {
		send_to_char("You cannot create an inter-zone link unless you're an Overlord.\r\n", ch);
		return;
	}
	if (world[ch->in_room].dir_option[dir]) {
		if (world[ch->in_room].dir_option[dir]->general_description) {
			free((char *) world[ch->in_room].dir_option[dir]->general_description);
			ha9900_sanity_check(0, "FREE06", "SYSTEM");
		}
		if (world[ch->in_room].dir_option[dir]->keyword) {
			free((char *) world[ch->in_room].dir_option[dir]->keyword);
			ha9900_sanity_check(0, "FREE07", "SYSTEM");
		}
		free((char *) world[ch->in_room].dir_option[dir]);
		ha9900_sanity_check(0, "FREE08", "SYSTEM");
		world[ch->in_room].dir_option[dir] = 0;
	}

	lv_rc = wi2920_create_room_links(ch->in_room, tar_rm, dir, buf3);
	if (lv_rc != TRUE) {
		send_to_char("Error creating room link.  Post a message.\r\n", ch);
		return;
	}

	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
	send_to_char("Enter description terminate with @.\r\n", ch);
	ch->desc->str = &(world[ch->in_room].dir_option[dir]->general_description);
	ch->desc->max_str = 2048;
}				/* END OF do_rlink() */


/* link one room to another, destroy old link if necessary */
void do_rlflag(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH], buf4[MAX_STRING_LENGTH];
	int dir, loc, zone, i;
	unsigned long flag;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf1);
	if (!*buf1) {
		send_to_char("Try rlflag <dir> <flag> [<key>] [<keywords>]\r\n", ch);
		sprintf(buf, "exit flags: \r\n");
		for (i = 0; *exit_bits[i] != '\n'; i++) {
			strcat(buf, exit_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	arg = one_argument(arg, buf2);
	half_chop(arg, buf3, buf4);
	if (!*buf1 || !*buf2) {
		send_to_char("Try rlflag <dir> <flag> [<key>] [<keywords>]\r\n", ch);
		return;
	}

	dir = (old_search_block(buf1, 0, strlen(buf1), dirs, 0));
	if (dir == -1) {	/* the 0 above allows for substr match */
		send_to_char("That direction doesn't exist.\r\n", ch);
		return;
	}
	dir--;			/* search block returns 1 as first position */

	loc = (old_search_block(buf2, 0, strlen(buf2), exit_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (!world[ch->in_room].dir_option[dir]) {
		send_to_char("No exit exists for that direction.\r\n", ch);
		return;
	}

	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);

	if (IS_SET(world[ch->in_room].dir_option[dir]->exit_info, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", exit_bits[loc]);
		REMOVE_BIT(world[ch->in_room].dir_option[dir]->exit_info, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", exit_bits[loc]);
		SET_BIT(world[ch->in_room].dir_option[dir]->exit_info, flag);
		send_to_char(buf, ch);
	}

	/* IF WE AREN'T SETTING THE IS_DOOR FLAG, WE ARE FINISHED */
	if (!is_abbrev(buf2, "is_door")) {
		return;
	}

	/* SET KEY - IF ARGUMENT ISN'T NUMERIC, WE'LL STILL ADD KEYWORDS */
	if (*buf3 && is_number(buf3)) {
		world[ch->in_room].dir_option[dir]->key = atoi(buf3);
		send_to_char("A key IS required", ch);
	}
	else {
		world[ch->in_room].dir_option[dir]->key = 0;
		send_to_char("A key IS NOT required", ch);
	}

	/* SET KEYWORDS */
	if (world[ch->in_room].dir_option[dir]->keyword) {
		free((char *) world[ch->in_room].dir_option[dir]->keyword);
	}
	/* set keywords if entered */
	if (*buf4) {
		world[ch->in_room].dir_option[dir]->keyword = str_alloc(buf4);
		send_to_char(" and there are keywords.\r\n", ch);
	}
	else {
		send_to_char(" and there AREN'T any keywords - YUK!!!\r\n", ch);
	}

	return;

}				/* END OF do_rlflag() */

void do_rsect(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	int loc, zone, i;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, buf1);
	if (!*buf1) {
		send_to_char("Try rsect <sector_type>\r\n", ch);
		sprintf(buf, "Sector types: \r\n");
		for (i = 0; *sector_types[i] != '\n'; i++) {
			strcat(buf, sector_types[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf1, 0, strlen(buf1), sector_types, -1));
	if (loc == -1) {
		send_to_char("That type doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	world[ch->in_room].sector_type = loc;
	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
	sprintf(buf, "sector type is now '%s'.\r\n", sector_types[loc]);
	send_to_char(buf, ch);
}


void do_rlist(struct char_data * ch, char *arg, int cmd)
{
	int i, number_found, zone, start, end;
	char buf[16384], buf1[128], buf2[128];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	half_chop(arg, buf1, buf2);

	if (is_number(buf1))
		start = atoi(buf1);
	else
		start = world[zone_table[zone].real_bottom].number;

	if (is_number(buf2))
		end = atoi(buf2);
	else
		end = world[zone_table[zone].real_top].number;

	/* DID THEY GOOF ON THE ORDER? */
	if (start > end) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Sorry but start %d can't be greater than end %d\r\n",
			start, end);
		send_to_char(buf, ch);
		return;
	}

	/* CRASH PROTECTION */
	if (start < 1)
		start = 1;

	if (end < 1)
		end = 1;

	bzero(buf, sizeof(buf));
	sprintf(buf, "\r\n");

	number_found = 0;
	for (i = 0; i < carved_world && number_found < 200; i++) {
		if (world[i].number >= start && world[i].number <= end) {
			number_found++;
			sprintf(buf1, "%d - %s\r\n", world[i].number, world[i].name);
			strcat(buf, buf1);
		}
	}
	sprintf(buf1, "There were %d rooms\r\n", number_found);
	strcat(buf, buf1);
	if (number_found > 199)
		strcat(buf, "Limiting search to 199 rooms\r\n");
	strcat(buf, "Done.\r\n");

	if (ch->desc) {
		page_string(ch->desc, buf, 1);
	}
	return;
}


void do_rflag(struct char_data * ch, char *arg, int cmd)
{
	char lv_name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	int loc, zone, lv_which_flag, idx;
	unsigned long lv_flag_to_set, lv_current_flag;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	for (; *arg == ' '; arg++);
	one_argument(arg, buf1);
	if (!*buf1) {
		send_to_char("Try rflag <flag>\r\n", ch);
		sprintf(buf, "Room flags: \r\n");
		for (idx = 0; *room_bits[idx] != '\n'; idx++) {
			strcat(buf, room_bits[idx]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		*buf = 0;
		for (idx = 0; *room_bits2[idx] != '\n'; idx++) {
			strcat(buf, room_bits2[idx]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}

	loc = (old_search_block(buf1, 0, strlen(buf1), room_bits, -1));
	if (loc > 0) {
		lv_which_flag = 1;
		lv_current_flag = world[ch->in_room].room_flags;
		strcpy(lv_name, room_bits[loc - 1]);
	}
	else {
		loc = (old_search_block(buf1, 0, strlen(buf1), room_bits2, -1));
		if (loc > 0) {
			lv_which_flag = 2;
			lv_current_flag = world[ch->in_room].room_flags2;
			strcpy(lv_name, room_bits2[loc - 1]);
		}
	}

	/* DID WE FIND ANYTHING? */
	if (loc < 1) {
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}

	/* ADJUST FOR 0 ARRAY BOUNDARY */
	loc--;

	/* COMPUTE FLAG */
	lv_flag_to_set = 1;
	for (idx = 0; idx < loc; idx++)
		lv_flag_to_set *= 2;

	if (IS_SET(lv_current_flag, lv_flag_to_set)) {
		REMOVE_BIT(lv_current_flag, lv_flag_to_set);
		sprintf(buf, "You have removed the '%s' flag.\r\n", lv_name);
		send_to_char(buf, ch);
	}
	else {
		SET_BIT(lv_current_flag, lv_flag_to_set);
		sprintf(buf, "You have set the '%s' flag.\r\n", lv_name);
		send_to_char(buf, ch);
	}

	if (lv_which_flag == 1)
		world[ch->in_room].room_flags = lv_current_flag;
	else
		world[ch->in_room].room_flags2 = lv_current_flag;

	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
	return;

}				/* END OF do_rflag() */


/* Give a room a new description */
void do_rdesc(struct char_data * ch, char *arg, int cmd)
{
	char arg1[MAX_STRING_LENGTH];
	int zone;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, arg1);
	if (!*arg1) {
		send_to_char("Try rdesc <new || append>\r\n", ch);
		return;
	}
	if (world[ch->in_room].description) {
		send_to_char("The old description was: \r\n", ch);
		send_to_char(world[ch->in_room].description, ch);
		if (!is_abbrev(arg1, "append")) {
			free((char *) world[ch->in_room].description);
			ha9900_sanity_check(0, "FREE10", "SYSTEM");
			world[ch->in_room].description = (char *) 0;
		}
	}
	send_to_char("\r\nEnter a new description.  Terminate with a '@'\r\n", ch);
	ch->desc->str = &(world[ch->in_room].description);
	ch->desc->max_str = 2048;
	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
}
/* Give a room an extra description */
void do_redesc(struct char_data * ch, char *arg, int cmd)
{
	char arg1[MAX_STRING_LENGTH];
	int zone;
	struct extra_descr_data *theExtra, *prev;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	arg = one_argument(arg, arg1);
	for (; *arg == ' '; arg++);	/* strip out preceeding spaces if any */
	if (!*arg1 || !*arg) {
		send_to_char("Try redesc <new || append || delete> <keyword list>\r\n", ch);
		return;
	}

	/* find keyword - if it exists */
	for (theExtra = world[ch->in_room].ex_description;
	     theExtra; theExtra = theExtra->next) {
		if (theExtra->keyword)
			if (ha1150_isname(arg, theExtra->keyword))
				break;
	}			/* for loop */

	if (is_abbrev(arg1, "delete")) {	/* icky delete */
		if (!theExtra) {
			send_to_char("that Extra description does not exist.\r\n", ch);
			return;
		}
		if (world[ch->in_room].ex_description == theExtra) {
			world[ch->in_room].ex_description = theExtra->next;
		}
		else {
			for (prev = world[ch->in_room].ex_description;
			     prev->next != theExtra;
			     prev = prev->next);
			prev->next = theExtra->next;	/* take this one out of
							 * chain */
		}
		send_to_char("Extra description deleted.\r\n", ch);
		free(theExtra->description);
		ha9900_sanity_check(0, "FREE11", "SYSTEM");
		free(theExtra->keyword);
		ha9900_sanity_check(0, "FREE12", "SYSTEM");
		free(theExtra);
		ha9900_sanity_check(0, "FREE13", "SYSTEM");
		SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
		return;
	}

	if (!theExtra) {
		CREATE(theExtra, struct extra_descr_data, 1);
		theExtra->next = world[ch->in_room].ex_description;
		world[ch->in_room].ex_description = theExtra;
		theExtra->keyword = strdup(arg);
		theExtra->description = (char *) 0;
	}

	if (theExtra->description) {
		send_to_char("The old description was: \r\n", ch);
		send_to_char(theExtra->description, ch);
		if (!is_abbrev(arg1, "append")) {
			free(theExtra->description);
			ha9900_sanity_check(0, "FREE14", "SYSTEM");
			theExtra->description = (char *) 0;
		}
	}
	send_to_char("\r\nEnter a new description.  Terminate with a '@'\r\n", ch);
	ch->desc->str = &theExtra->description;
	ch->desc->max_str = 2048;
	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
}

void do_rname(struct char_data * ch, char *arg, int cmd)
{
	int zone;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (!arg || !*arg) {
		send_to_char("Try rname [<desc>]\r\n", ch);
		return;
	}
	for (; *arg == ' '; arg++);	/* strip out preceeding spaces if any */

	send_to_char("The old name was: \r\n", ch);
	send_to_char(world[ch->in_room].name, ch);

	free(world[ch->in_room].name);
	ha9900_sanity_check(0, "FREE15", "SYSTEM");
	world[ch->in_room].name = str_alloc(arg);
	send_to_char("\r\nThe new name is: \r\n", ch);
	send_to_char(world[ch->in_room].name, ch);
	SET_BIT(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED);
}

void do_rsave(struct char_data * ch, char *arg, int cmd)
{
	int zone;
	char buf[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[world[ch->in_room].zone].dirty_wld, UNSAVED)) {

		/* WE DON'T LET THEM UPDATE THE VEHICLE ZONE */
		if (gv_port != DEVELOPMENT_PORT) {
			if (is_abbrev("vehicle", zone_table[world[ch->in_room].zone].filename)) {
				main_log("WARNING: You can't save the Vehicle zone.");
				return;
			}
		}

		db9100_save_wld(world[ch->in_room].zone);
		sprintf(buf, "areas/%s.wld saved.\r\n", zone_table[world[ch->in_room].zone].filename);
	}
	else
		sprintf(buf, "areas/%s.wld unchanged since last save.\r\n", zone_table[world[ch->in_room].zone].filename);
	send_to_char(buf, ch);
	return;
}
/*****************************************************************
*                                                               *
*                                                               *
*                     O B J    S T U F F                        *
*                                                               *
*                                                               *
*****************************************************************/

void do_ohelp(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];


	bzero(buf, sizeof(buf));
	for (; *arg == ' '; arg++);
	if (*arg)
		sprintf(buf, "ohelp_%s", arg);
	else
		strcpy(buf, "ohelp");
	in3500_do_help(ch, buf, CMD_HELP);

	return;

}				/* END OF do_ohelp() */


void do_ocreate(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int zone, i;
	struct obj_data *obj;



	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* check for empty */
	if (zone_table[zone].bot_of_objt == -1) {
		zone_table[zone].bot_of_objt = top_of_objt + 1;
	}
	else {
		/* and what if its full */
		if (obj_index[zone_table[zone].top_of_objt].virtual + 1 >
		    zone_table[zone].top) {
			send_to_char("Sorry but this zone is full... nothing more can be added\r\n", ch);
			return;
		}
	}

	/* fix the top/bottom of the index for the new entry */
	top_of_objt++;
	db2400_allocate_obj(top_of_objt);
	SET_BIT(zone_table[zone].dirty_obj, UNSAVED);
	SET_BIT(zone_table[zone].dirty_obj, SCRAMBLED);
	if (zone_table[zone].top_of_objt >= 0) {
		obj_index[top_of_objt].virtual =
			obj_index[zone_table[zone].top_of_objt].virtual + 1;
		obj_index[zone_table[zone].top_of_objt].next = top_of_objt;
	}
	else {
		obj_index[top_of_objt].virtual = zone_table[zone].bottom;
	}
	obj_index[top_of_objt].next = top_of_objt + 1;
	obj_index[top_of_objt].zone = zone;
	obj_index[top_of_objt].number = 0;
	obj_index[top_of_objt].maximum = 32767;
	obj_index[top_of_objt].func = 0;
	obj_index[top_of_objt].next = top_of_objt + 1;
	zone_table[zone].top_of_objt = top_of_objt;

	/* give it some initial values */
	CREATE(obj, struct obj_data, 1);
	db7600_clear_object(obj);

	obj_index[top_of_objt].name = str_alloc("obj torch");
	obj->name = str_alloc("obj torch");
	obj->short_description = str_alloc("A torch");
	obj->description = str_alloc("A torch is lying here.");
	obj->action_description = 0;

	obj->obj_flags.type_flag = 1;
	obj->obj_flags.flags1 = 0;
	obj->obj_flags.wear_flags = 1;
	obj->obj_flags.value[0] = 0;
	obj->obj_flags.value[1] = 0;
	obj->obj_flags.value[2] = 0;
	obj->obj_flags.value[3] = 0;
	obj->obj_flags.weight = 1;
	obj->obj_flags.cost = 5;
	obj->obj_flags.cost_per_day = 5;
	obj->ex_description = 0;

	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		obj->affected[i].location = APPLY_NONE;
		obj->affected[i].modifier = 0;
	}

	obj->next_content = 0;
	obj->carried_by = 0;
	obj->in_obj = 0;
	obj->contains = 0;
	obj->item_number = top_of_objt;
	obj->in_room = NOWHERE;

	obj_index[top_of_objt].prototype = (char *) obj;

	/* let 'em know what happened */
	sprintf(buf, "Done, obj #%d created!", obj_index[top_of_objt].virtual);
	send_to_char(buf, ch);

}				/* END OF do_ocreate() */


void do_ocopy(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];
	int zone, lv_value, i;
	struct obj_data *obj, *lv_source_obj;



	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* check for empty */
	if (zone_table[zone].bot_of_objt == -1) {
		zone_table[zone].bot_of_objt = top_of_objt + 1;
	}
	else {
		/* and what if its full */
		if (obj_index[zone_table[zone].top_of_objt].virtual + 1 >
		    zone_table[zone].top) {
			send_to_char("Sorry but this zone is full... nothing more can be added\r\n", ch);
			return;
		}
	}

	/* ************ */
	/* SOURCE OBJ  */
	/* ************ */
	for (; isspace(*arg); arg++);

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("An obj number is required.\r\n", ch);
		send_to_char("Format: OCOPY <source obj>\r\n", ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Obj number must be numeric.\r\n", ch);
		return;
	}

	lv_value = atoi(buf);
	lv_source_obj = db5100_read_object(lv_value, PROTOTYPE);
	if (!(lv_source_obj)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "I'm sorry, %d is an invalid obj number.\r\n",
			lv_value);
		send_to_char(buf, ch);
		return;
	}

	/* KEEP EM OUT OF XANTH */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    !strcmp(zone_table[zone].filename, "Xanth")) {
		send_to_char("You can't use objects from that zone.\r\n", ch);
		return;
	}

	/* fix the top/bottom of the index for the new entry */
	top_of_objt++;
	db2400_allocate_obj(top_of_objt);
	SET_BIT(zone_table[zone].dirty_obj, UNSAVED);
	SET_BIT(zone_table[zone].dirty_obj, SCRAMBLED);
	if (zone_table[zone].top_of_objt >= 0) {
		obj_index[top_of_objt].virtual =
			obj_index[zone_table[zone].top_of_objt].virtual + 1;
		obj_index[zone_table[zone].top_of_objt].next = top_of_objt;
	}
	else {
		obj_index[top_of_objt].virtual = zone_table[zone].bottom;
	}

	obj_index[top_of_objt].next = top_of_objt + 1;
	obj_index[top_of_objt].zone = zone;
	obj_index[top_of_objt].number = 0;
	obj_index[top_of_objt].maximum =
		obj_index[lv_source_obj->item_number].maximum;
	obj_index[top_of_objt].func = 0;
	obj_index[top_of_objt].next = top_of_objt + 1;
	zone_table[zone].top_of_objt = top_of_objt;

	/* give it some initial values */
	CREATE(obj, struct obj_data, 1);
	db7600_clear_object(obj);

	sprintf(buf, "ocopyof%d", lv_value);
	obj_index[top_of_objt].name = strdup(buf);
	obj->name = strdup(buf);
	obj->short_description = strdup(lv_source_obj->short_description);
	obj->description = strdup(lv_source_obj->description);
	if (lv_source_obj->action_description)
		obj->action_description =
			strdup(lv_source_obj->action_description);
	else
		obj->action_description = 0;

	obj->obj_flags = lv_source_obj->obj_flags;
	/*
	 obj->obj_flags.type_flag = 0;
	 obj->obj_flags.flags1 = 0;
	 obj->obj_flags.wear_flags = 1;
	 obj->obj_flags.value[0] = 0;
	 obj->obj_flags.value[1] = 0;
	 obj->obj_flags.value[2] = 10;
	 obj->obj_flags.value[3] = 0;
	 obj->obj_flags.weight = 1;
	 obj->obj_flags.cost = 5;
	 obj->obj_flags.cost_per_day = 5;
	 */
	/* WE DON'T COPY EXTENDED DESCRIPTIONS */
	obj->ex_description = 0;

	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		obj->affected[i].location = lv_source_obj->affected[i].location;
		obj->affected[i].modifier = lv_source_obj->affected[i].modifier;
	}

	obj->next_content = 0;
	obj->carried_by = 0;
	obj->in_obj = 0;
	obj->contains = 0;
	obj->item_number = top_of_objt;
	obj->in_room = NOWHERE;

	obj_index[top_of_objt].prototype = (char *) obj;

	/* let 'em know what happened */
	sprintf(buf, "Done, obj #%d created!", obj_index[top_of_objt].virtual);
	send_to_char(buf, ch);

}				/* END OF do_ocopy() */


void do_olist(struct char_data * ch, char *arg, int cmd)
{
	int i, zone, start, end, no = 1;
	char buf[2 * MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	half_chop(arg, buf1, buf2);

	if (is_number(buf1)) {
		start = atoi(buf1);
		zone = db4000_zone_of(start);
	}
	else
		start = zone_table[zone].bottom;

	if (is_number(buf2))
		end = atoi(buf2);
	else
		end = zone_table[zone].top;
	if (end < start)
		end = start + 15;
	if (end > zone_table[zone].top)
		end = zone_table[zone].top;
	if (end - start >= 100) {
		end = start + 99;
		send_to_char("Search will be limited to 200 objects (try olist <start num> <end num> for rest)\r\n", ch);
	}

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	sprintf(buf, "\r\n");	/* init buf to something for strcat */
	for (i = zone_table[zone].bot_of_objt; (i >= 0) && (i <= top_of_objt) && (obj_index[i].virtual <= end); i = obj_index[i].next) {
		if (obj_index[i].virtual >= start) {
			sprintf(buf1, "[%5d] [%-28s] ", obj_index[i].virtual, obj_index[i].name);
			if (!(no % 2))
				strcat(buf1, "\r\n");
			no++;
			strcat(buf, buf1);
		}
	}
	sprintf(buf1, "Done. objs[%d to %d]\r\n", start, end);
	strcat(buf, buf1);
	if (ch->desc) {
		page_string(ch->desc, buf, 1);
	}
	return;
}

void do_oflag(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct obj_data *obj;


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try oflag <obj> <flag>\r\n", ch);
		sprintf(buf, "Obj flags: \r\n");
		for (i = 0; *oflag1_bits[i] != '\n'; i++) {
			strcat(buf, oflag1_bits[i]);
			strcat(buf, " ");
		}
		for (i = 0; *oflag2_bits[i] != '\n'; i++) {
			strcat(buf, oflag2_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), oflag1_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		do_oflag2(ch, arg, cmd);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(obj->obj_flags.flags1, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", oflag1_bits[loc]);
		/* IF REMOVING OMAX FLAG, MAKE SURE MAX IS 100 */
		if (flag == OBJ1_OMAX) {
			REMOVE_BIT(obj_index[obj->item_number].flags, OBJI_OMAX);
			if (obj_index[obj->item_number].maximum > 100) {
				obj_index[obj->item_number].maximum = 100;
				send_to_char("Defaulting maximum to 100\r\n", ch);
			}
		}
		REMOVE_BIT(obj->obj_flags.flags1, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", oflag1_bits[loc]);
		SET_BIT(obj->obj_flags.flags1, flag);

		/* IF ADDING OFLAG, UPDATE OBJECT_INDEX ALSO */
		if (flag == OBJ1_OMAX) {
			SET_BIT(obj_index[obj->item_number].flags, OBJI_OMAX);
		}
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_oflag2(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct obj_data *obj;


	half_chop(arg, buf1, buf2);

	loc = (old_search_block(buf2, 0, strlen(buf2), oflag2_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(obj->obj_flags.flags2, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", oflag2_bits[loc]);

		REMOVE_BIT(obj->obj_flags.flags2, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", oflag2_bits[loc]);
		SET_BIT(obj->obj_flags.flags2, flag);

		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_oclass(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct obj_data *obj;

	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try oclass <flag>\r\n", ch);
		sprintf(buf, "Class flags: \r\n");
		for (i = 0; *class_anti[i] != '\n'; i++) {
			strcat(buf, class_anti[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), class_anti, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(obj->obj_flags.value[2], flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", class_anti[loc]);
		REMOVE_BIT(obj->obj_flags.value[2], flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", class_anti[loc]);
		SET_BIT(obj->obj_flags.value[2], flag);
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_owflag(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct obj_data *obj;


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try owflag <obj> <flag>\r\n", ch);
		sprintf(buf, "Weapon flags: \r\n");
		for (i = 0; *weapon_bits[i] != '\n'; i++) {
			strcat(buf, weapon_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), weapon_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(obj->obj_flags.value[0], flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", weapon_bits[loc]);
		REMOVE_BIT(obj->obj_flags.value[0], flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", weapon_bits[loc]);
		SET_BIT(obj->obj_flags.value[0], flag);
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}


void do_owear(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct obj_data *obj;


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try owflag <obj> <flag>\r\n", ch);
		sprintf(buf, "Wear flags: \r\n");
		for (i = 0; *wear_bits[i] != '\n'; i++) {
			strcat(buf, wear_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), wear_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(obj->obj_flags.wear_flags, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", wear_bits[loc]);
		REMOVE_BIT(obj->obj_flags.wear_flags, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", wear_bits[loc]);
		SET_BIT(obj->obj_flags.wear_flags, flag);
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}




void do_otype(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	struct obj_data *obj;


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try otype <obj> <type>\r\n", ch);
		sprintf(buf, "Item types: \r\n");
		for (i = 1; *item_types[i] != '\n'; i++) {
			strcat(buf, item_types[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), item_types, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */
	if (loc == 0) {
		send_to_char("You cant set an item type to UNDEFINED...sheesh\r\n", ch);
		return;
	}
	if (loc == ITEM_ZCMD) {
		send_to_char("Those are reserved for online zone editing...sheesh\r\n", ch);
		return;
	}
	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}
	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	GET_ITEM_TYPE(obj) = loc;

	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_oaffect(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH], buf4[MAX_STRING_LENGTH];
	int loc, zone, i, one;
	struct obj_data *obj;


	arg = one_argument(arg, buf1);
	if (*buf1)
		arg = one_argument(arg, buf2);
	if (*buf2)
		half_chop(arg, buf3, buf4);
	if (!isdigit(*buf1) || !isdigit(*buf2) || !*buf3 || !*buf4) {
		send_to_char("Try oaffect <obj> <aff#> <type> <modifier>\r\n", ch);
		sprintf(buf, "Apply types: \r\n");
		for (i = 0; *apply_types[i] != '\n'; i++) {
			strcat(buf, apply_types[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf3, 0, strlen(buf3), apply_types, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(obj = db5100_read_object(atoi(buf1), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}
	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	i = atoi(buf2);
	if (i < 0 || i >= MAX_OBJ_AFFECT) {
		send_to_char("Affect numbers range from 0 to 3\r\n", ch);
		return;
	}

	obj->affected[i].location = loc;
	sscanf(buf4, " %d", &one);
	obj->affected[i].modifier = one;

	sprinttype(obj->affected[i].location, apply_types, buf2);
	sprintf(buf, "Ok, it now affects: %s By %d\r\n", buf2, obj->affected[i].modifier);
	send_to_char(buf, ch);

	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}


void do_ostat(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int i, zone;
	struct obj_data *obj;
	struct extra_descr_data *desc;


	arg = one_argument(arg, buf);
	if (!is_number(buf)) {
		send_to_char("Try ostat <obj>\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}
	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

/* output the results of the stat */
	sprintf(buf, "Obj#:[%d]  Keywords:[%s] Type:[",
		obj_index[obj->item_number].virtual, obj->name);
	sprinttype(GET_ITEM_TYPE(obj), item_types, buf2);
	strcat(buf, buf2);
	strcat(buf, "]\r\n");
	send_to_char(buf, ch);

	sprintf(buf, "Short description:[%s] name:[%s]\r\nLong description:\r\n%s\r\n",
		((obj->short_description) ? obj->short_description : "None"),
		ha1100_fname(obj_index[obj->item_number].name),
		((obj->description) ? obj->description : "None"));
	send_to_char(buf, ch);

	sprintf(buf, "Currently in game: %d  Percent reset: %d%%\r\n",
		obj_index[obj->item_number].number,
		obj_index[obj->item_number].maximum);
	send_to_char(buf, ch);

	send_to_char("Worn : ", ch);
	sprintbit(obj->obj_flags.wear_flags, wear_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("Extra : ", ch);
	sprintbit(obj->obj_flags.flags1, oflag1_bits, buf);
	send_to_char(buf, ch);
	sprintbit(obj->obj_flags.flags2, oflag2_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	if (GET_ITEM_TYPE(obj) == ITEM_WEAPON) {
		send_to_char("Weapon :", ch);
		sprintbit(obj->obj_flags.value[0], weapon_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
	}

	sprintf(buf, "Weight:[%d] Cost:[%d] Rent:[%d]\r\n",
		obj->obj_flags.weight,
		obj->obj_flags.cost,
		obj->obj_flags.cost_per_day);
	send_to_char(buf, ch);

	switch (obj->obj_flags.type_flag) {
	case ITEM_LIGHT:
		sprintf(buf, "Colour:[%d] Type:[%d] Hours:[%d] Undef:[]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2]);
		break;
	case ITEM_SCROLL:
	case ITEM_POTION:
		sprintf(buf, "Level: [%d] Spells: [%d], [%d], [%d]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		break;
	case ITEM_WAND:
	case ITEM_STAFF:
	case ITEM_SPELL:
		sprintf(buf, "Level[%d], Max-Charge:[%d], Left:[%d], Spell:[%d]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		break;
	case ITEM_WEAPON:
		sprintf(buf, "Wflag:[] Dam:[%d]D[%d] Type:[%d]\r\n",
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		break;
	case ITEM_ARMOR:
		if (obj_index[obj->item_number].virtual >= OBJ_XANTH_BEG_MAGIC_ITEMS &&
		    obj_index[obj->item_number].virtual <= OBJ_XANTH_END_MAGIC_ITEMS) {
			sprintf(buf, "AC-apply:[%d] Max Charge:[%d] Charges:[%d] Undef:[%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
		}
		else {
			sprintf(buf, "AC-apply:[%d] Undef:[%d] Undef:[%d] Undef:[%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
		}
		break;
	case ITEM_TRAP:
		sprintf(buf, "Spell:[%d] Hitpoints:[%d] Undef:[] Undef:[]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1]);
		break;
	case ITEM_CONTAINER:
		sprintf(buf, "Max-contains:[%d] Locktype:[%d] Undef:[] Corpse:[%s]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[3] ? "Yes" : "No");
		break;
	case ITEM_DRINKCON:
		sprinttype(obj->obj_flags.value[2], drinks, buf2);
		sprintf(buf, "Max:[%d] Contains:[%d] Liquid:[%s] Poisoned:[%d]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			buf2,
			obj->obj_flags.value[3]);
		break;
	case ITEM_NOTE:
		sprintf(buf, "Tongue:[%d] Undef:[] Undef:[] Undef:[]\r\n",
			obj->obj_flags.value[0]);
		break;
	case ITEM_KEY:
		sprintf(buf, "opens obj:[%d]  # of uses:[%d] Undef:[] Undef:[]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1]);
		break;
	case ITEM_FOOD:
		sprintf(buf, "Makes-full:[%d] Poisoned:[%d] Undef:[] Undef:[]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[3]);
		break;
	case ITEM_VEHICLE:
		sprintf(buf, "Chance to hit %% :[%d] Damage %%:[%d] Min dmg:[%d] Max dmg:[%d]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		break;
	case ITEM_FURNITURE:
		sprintf(buf, "Hit regen:[%d] Mana regen:[%d] Move regen:[%d] Undef:[]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2]);
		break;
	default:
		sprintf(buf, "Values 0:[%d] 1:[%d] 2:[%d] 3:[%d]\r\n",
			obj->obj_flags.value[0],
			obj->obj_flags.value[1],
			obj->obj_flags.value[2],
			obj->obj_flags.value[3]);
		break;
	}
	send_to_char(buf, ch);

	send_to_char("Can affect char :\r\n", ch);

	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		sprinttype(obj->affected[i].location, apply_types, buf2);
		sprintf(buf, "    Affects[%d] : %s By %d\r\n",
			i,
			buf2,
			obj->affected[i].modifier);
		send_to_char(buf, ch);
	}
	if (obj->ex_description) {
		strcpy(buf, "Extra description keyword(s):\r\n----------\r\n");
		for (desc = obj->ex_description; desc; desc = desc->next) {
			strcat(buf, desc->keyword);
			strcat(buf, "\r\n");
		}
		send_to_char(buf, ch);
	}
	else {
		strcpy(buf, "Extra description keyword(s): None\r\n");
		send_to_char(buf, ch);
	}
	sprintf(buf, "OSTAT: %s did ostat %d.", GET_REAL_NAME(ch),
		obj_index[obj->item_number].virtual);
	spec_log(buf, GOD_COMMAND_LOG);
	return;
}				/* END OF do_ostat() */

void do_ovalues(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;


	arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Try ovalues <obj> <value> <value> <value> <value>\r\n", ch);
		send_to_char("ie ovalue 6405 - 2 - 2 would change value[1] and value[3] to 2.\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	//skip first value for weapons
		if (GET_ITEM_TYPE(obj) != ITEM_WEAPON)
			obj->obj_flags.value[0] = 0;

	obj->obj_flags.value[1] = 0;
	obj->obj_flags.value[2] = 0;
	obj->obj_flags.value[3] = 0;

	//skip first value for weapons
		if (GET_ITEM_TYPE(obj) != ITEM_WEAPON) {
			arg = one_argument(arg, buf);
			if (isdigit(*buf))
				obj->obj_flags.value[0] = atoi(buf);
		}

	arg = one_argument(arg, buf);
	if (isdigit(*buf))
		obj->obj_flags.value[1] = atoi(buf);
	arg = one_argument(arg, buf);
	if (isdigit(*buf))
		obj->obj_flags.value[2] = atoi(buf);
	arg = one_argument(arg, buf);
	if (isdigit(*buf))
		obj->obj_flags.value[3] = atoi(buf);
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}


void do_ocost(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;


	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !is_number(buf)) {
		send_to_char("Try ocost <value>\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (is_number(buf))
		obj->obj_flags.cost = atoi(buf);
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_operc(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct obj_data *obj;
	int lv_num, zone;


	arg = one_argument(arg, buf);	/* object number to change */
	arg = one_argument(arg, buf2);	/* value */

	if (!*buf || !is_number(buf)) {
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf), PROTOTYPE))) {
		send_to_char("Sorry I can't locate that obj.\r\n", ch);
		return;
	}

	/* IF OMAX FLAG IS ON, VALUE CAN BE 0 TO 32767 */
	if (!*buf2 || !(is_number(buf2))) {
		if (IS_SET(GET_OBJ1(obj), OBJ1_OMAX)) {
			send_to_char("Try operc <obj#> <value> where value is 0 to 32767\r\n", ch);
		}
		else {
			send_to_char("Try operc <obj#> <value> where value is 0 to 100\r\n", ch);
		}
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	lv_num = atoi(buf2);
	if (IS_SET(GET_OBJ1(obj), OBJ1_OMAX)) {
		if (lv_num < 0) {
			lv_num = 1;
		}
		if (lv_num > 32767) {
			send_to_char("Sorry, 32767 is the maximum.\r\n", ch);
			return;
		}
	}
	else {
		if (lv_num < 0) {
			lv_num = 100;
		}
		if (lv_num > 100) {
			send_to_char("Sorry, 100 is the maximum.\r\n", ch);
			return;
		}
	}

	obj_index[obj->item_number].maximum = lv_num;
	bzero(buf, sizeof(buf));
	if (IS_SET(GET_OBJ1(obj), OBJ1_OMAX)) {
		sprintf(buf, "You have set the maximum on object: %d to %d.\r\n",
			obj_index[obj->item_number].virtual,
			obj_index[obj->item_number].maximum);
	}
	else {
		sprintf(buf, "You have set the percent on object: %d to %d.\r\n",
			obj_index[obj->item_number].virtual,
			obj_index[obj->item_number].maximum);
	}
	send_to_char(buf, ch);
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
	return;

}				/* END OF do_operc() */


void do_orent(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct obj_data *obj;
	int one, zone;

	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try orent <obj> <value>\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}


	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (sscanf(buf, " %d", &one))
		obj->obj_flags.cost_per_day = one;
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_okeywords(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;


	half_chop(arg, buf2, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try okeywords <obj> keywords\r\n", ch);
		return;
	}

	if (!is_number(buf2)) {
		send_to_char("The object must be a number.\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (obj_index[obj->item_number].name) {
		str_free(obj_index[obj->item_number].name);
		ha9900_sanity_check(0, "FREE16", "SYSTEM");
		str_free(obj->name);
		ha9900_sanity_check(0, "FREE17", "SYSTEM");
	} if (strncmp(buf, "obj ", 4)) {
		sprintf(buf2, "obj %s", buf);
		obj_index[obj->item_number].name = str_alloc(buf2);
		obj->name = str_alloc(buf2);
	}
	else {
		obj_index[obj->item_number].name = str_alloc(buf);
		obj->name = str_alloc(buf);
	}

	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}



void do_osdesc(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;


	half_chop(arg, buf2, buf);
	if (!is_number(buf2) || !*buf) {
		send_to_char("Try osdesc <obj> osdesc\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	obj->short_description = str_free(obj->short_description);
	ha9900_sanity_check(0, "FREE18", "SYSTEM");
	obj->short_description = str_alloc(buf);

	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}

void do_oldesc(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;


	half_chop(arg, buf2, buf);
	if (!is_number(buf2) || !*buf) {
		send_to_char("Try oldesc <obj> oldesc\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	obj->description = str_free(obj->description);
	ha9900_sanity_check(0, "FREE19", "SYSTEM");
	obj->description = str_alloc(buf);

	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}




void do_oweight(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;



	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !is_number(buf)) {
		send_to_char("Try oweight <obj> <value>\r\n", ch);
		return;
	}

	if (!(obj = db5100_read_object(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (is_number(buf))
		obj->obj_flags.weight = atoi(buf);
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}


void do_osave(struct char_data * ch, char *arg, int cmd)
{
	extern void db9500_save_obj(int zone);
	int zone;
	char buf[MAX_STRING_LENGTH];

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[world[ch->in_room].zone].dirty_obj, UNSAVED)) {
		db9500_save_obj(world[ch->in_room].zone);
		sprintf(buf, "areas/%s.obj saved.\r\n",
			zone_table[world[ch->in_room].zone].filename);
	}
	else {
		sprintf(buf, "areas/%s.obj unchanged since last save.\r\n",
			zone_table[world[ch->in_room].zone].filename);
	}
	send_to_char(buf, ch);
	return;

}				/* END OF do_osave */


/* Give an object an extra description */
void do_oedesc(struct char_data * ch, char *arg, int cmd)
{

	char arg1[MAX_STRING_LENGTH], arg_obj[MAX_STRING_LENGTH];
	int zone;
	struct obj_data *obj;
	struct extra_descr_data *theExtra, *prev;


	arg = one_argument(arg, arg_obj);
	arg = one_argument(arg, arg1);
	for (; *arg == ' '; arg++);	/* strip out preceeding spaces if any */
	if (!*arg1 || !*arg) {
		send_to_char("Try oedesc <obj> <new || append || delete> <keyword list>\r\n", ch);
		return;
	}
	if (!(obj = db5100_read_object(atoi(arg_obj), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that obj.\r\n", ch);
		return;
	}

	zone = obj_index[obj->item_number].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

/* find the damn keyword if it exists */
	for (theExtra = obj->ex_description;
	     theExtra;
	     theExtra = theExtra->next) {
		if (theExtra->keyword)
			if (ha1150_isname(arg, theExtra->keyword))
				break;
	}			/* for loop */

	if (is_abbrev(arg1, "delete")) {	/* icky delete */
		if (!theExtra) {
			send_to_char("that Extra description does not exist.\r\n", ch);
			return;
		}
		if (obj->ex_description == theExtra) {
			obj->ex_description = theExtra->next;
		}
		else {
			for (prev = obj->ex_description;
			     prev->next != theExtra;
			     prev = prev->next);
			prev->next = theExtra->next;	/* take this one out of
							 * chain */
		}
		send_to_char("Extra description deleted.\r\n", ch);
		theExtra->description = str_free(theExtra->description);
		theExtra->keyword = str_free(theExtra->keyword);
		ha9900_sanity_check(0, "FREE20", "SYSTEM");
		free(theExtra);
		ha9900_sanity_check(0, "FREE21", "SYSTEM");
		SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
		return;
	}

	if (!theExtra) {
		CREATE(theExtra, struct extra_descr_data, 1);
		theExtra->next = obj->ex_description;
		obj->ex_description = theExtra;
		theExtra->keyword = str_alloc(arg);
		theExtra->description = (char *) 0;
	}

	if (theExtra->description) {
		send_to_char("The old description was: \r\n", ch);
		send_to_char(theExtra->description, ch);
		if (!is_abbrev(arg1, "append")) {
			str_free(theExtra->description);
			ha9900_sanity_check(0, "FREE22", "SYSTEM");
			theExtra->description = (char *) 0;
		}
	}
	send_to_char("\r\nEnter a new description.  Terminate with a '@'\r\n", ch);
	ch->desc->str = &theExtra->description;
	ch->desc->max_str = 2048;
	SET_BIT(zone_table[obj_index[obj->item_number].zone].dirty_obj, UNSAVED);
}


/*****************************************************************
*                                                               *
*                                                               *
*                     M O B    S T U F F                        *
*                                                               *
*                                                               *
*****************************************************************/

void do_mhelp(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];


	bzero(buf, sizeof(buf));
	for (; *arg == ' '; arg++);
	if (*arg)
		sprintf(buf, "mhelp_%s", arg);
	else
		strcpy(buf, "mhelp");
	in3500_do_help(ch, buf, CMD_HELP);

	return;

}

void cr3900_do_mset(struct char_data * ch, char *arg, int cmd)
{

	const char *lv_stats[] = {
		"ac",
		"alignment",
		"unused",
		"charisma",
		"class",
		"constitution",
		"unused-damage",
		"dexterity",
		"experience",
		"gold",
		"intelligenc",
		"level",
		"race",
		"sex",
		"strength",
		"wisdom",
		"\n",
	};

#define STAT_AC          1
#define STAT_ALIGNMENT   2
#define STAT_ATTACK      3
#define STAT_CHA         4
#define STAT_CLASS       5
#define STAT_CON         6
#define STAT_DAMAGE      7
#define STAT_DEX         8
#define STAT_EXPERIENCE  9
#define STAT_GOLD       10
#define STAT_INT        11
#define STAT_LEVEL      12
#define STAT_RACE       13
#define STAT_SEX        14
#define STAT_STR        15
#define STAT_WIS        16

	struct char_data *lv_mob;
	int lv_stat_type, lv_stat_value, lv_old_value, lv_value, lv_sign, zone;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* ************* */
	/* MOB      */
	/* ************* */
	for (; isspace(*arg); arg++);

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		send_to_char("A mob number is required.\r\n", ch);
		send_to_char(MSET_FORMAT, ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Mob number must be numeric.\r\n", ch);
		send_to_char(MSET_FORMAT, ch);
		return;
	}

	lv_value = atoi(buf);
	lv_mob = db5000_read_mobile(lv_value, PROTOTYPE);
	if (!(lv_mob)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "I'm sorry, %d is an invalid mob number.\r\n", lv_value);
		send_to_char(buf, ch);
		return;
	}

	/* *********** */
	/* STAT TYPE  */
	/* *********** */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("Sorry, but a stat is required.\r\n", ch);
		send_to_char(buf2, ch);
		bzero(buf2, sizeof(buf2));
		strcpy(buf2, "mhelp_mset");
		in3500_do_help(ch, buf2, CMD_HELP);
		return;
	}

	lv_stat_type = old_search_block(buf, 0, strlen(buf), lv_stats, 0);
	if (lv_stat_type < 1) {
		sprintf(buf2, "Sorry, but stat: %s is invalid.\r\n", buf);
		send_to_char(buf2, ch);
		send_to_char("Use HELP MSET for a list of types.\r\n", ch);
		return;
	}

	/* ************ */
	/* STAT VALUE  */
	/* ************ */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("A value is required.\r\n", ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*buf == '-') {
		lv_sign = -1;
		strcpy(buf, (buf + 1));
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Stat value must be numeric.\r\n", ch);
		send_to_char(MSET_FORMAT, ch);
		return;
	}

	lv_stat_value = atoi(buf) * lv_sign;

	/* SAFTY CHECK */
	if (GET_RACE(lv_mob) < 0 || GET_RACE(lv_mob) > MAX_RACES) {
		GET_RACE(lv_mob) = 0;
	}
	if (GET_CLASS(lv_mob) < 0 || GET_CLASS(lv_mob) > MAX_CLASSES) {
		GET_CLASS(lv_mob) = 0;
	}

	switch (lv_stat_type) {
	case STAT_AC:{
			CHECK_VALUE(-400, 400)
				lv_old_value = GET_ARMOR(lv_mob);
			GET_ARMOR(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_ALIGNMENT:{
			CHECK_VALUE(-1000, 1000)
				lv_old_value = GET_ALIGNMENT(lv_mob);
			GET_ALIGNMENT(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_CHA:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_cha)
				lv_old_value = GET_REAL_CHA(lv_mob);
			GET_REAL_CHA(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_CLASS:{
			CHECK_VALUE(0, MAX_CLASSES)
			/* Is it defined? */
				if (!(strncmp(classes[lv_stat_value].name, "undefined", 9)) &&
				    lv_stat_value != 0) {
				send_to_char("Sorry, that class is undefined.\r\n", ch);
				return;
			}
			lv_old_value = GET_CLASS(lv_mob);
			GET_CLASS(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_CON:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_con)
				lv_old_value = GET_REAL_CON(lv_mob);
			GET_REAL_CON(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_DEX:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_dex)
				lv_old_value = GET_REAL_DEX(lv_mob);
			GET_REAL_DEX(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_EXPERIENCE:{
			CHECK_VALUE(1, 2500000)
				lv_old_value = GET_EXP(lv_mob);
			GET_EXP(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_GOLD:{
			if (GET_LEVEL(ch) < IMO_IMP)
				CHECK_VALUE(0, 5500000)
					else
				CHECK_VALUE(0, 100000000)
					lv_old_value = GET_GOLD(lv_mob);
			GET_GOLD(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_INT:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_int)
				lv_old_value = GET_REAL_INT(lv_mob);
			GET_REAL_INT(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_LEVEL:{
			CHECK_VALUE(1, 50)
				lv_old_value = GET_LEVEL(lv_mob);
			GET_LEVEL(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_RACE:{
			CHECK_VALUE(0, MAX_RACES)
			/* Is it defined? */
				if (!(strncmp(races[lv_stat_value].name, "undefined", 9)) &&
				    lv_stat_value != 0) {
				send_to_char("Sorry, that race is undefined.\r\n", ch);
				return;
			}
			lv_old_value = GET_RACE(lv_mob);
			GET_RACE(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_SEX:{
			CHECK_VALUE(0, 2)
				lv_old_value = GET_SEX(lv_mob);
			GET_SEX(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_STR:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_str)
				lv_old_value = GET_REAL_STR(lv_mob);
			GET_REAL_STR(lv_mob) = lv_stat_value;
			break;
		}
	case STAT_WIS:{
			CHECK_VALUE(3, races[GET_RACE(lv_mob)].max_wis)
				lv_old_value = GET_REAL_WIS(lv_mob);
			GET_REAL_WIS(lv_mob) = lv_stat_value;
			break;
		}
	default:{
			send_to_char("Hrmmm, Didn't find that type after all.\r\n",
				     ch);
			return;
		}
	}			/* END OF switch() */


	if (lv_stat_value == lv_old_value) {
		send_to_char("Value unchanged.\r\n", ch);
		return;
	}

	/* UPDATE DATABASE */
	bzero(buf, sizeof(buf));
	sprintf(buf, "Setting mob %s %s from %d to %d.\r\n",
		GET_REAL_NAME(lv_mob),
		lv_stats[lv_stat_type - 1],
		lv_old_value,
		lv_stat_value);
	send_to_char(buf, ch);

	SET_BIT(zone_table[mob_index[lv_mob->nr].zone].dirty_mob, UNSAVED);

	return;

}				/* END OF void cr3900_do_mset() */

void cr4000_do_mskill(struct char_data * ch, char *arg, int cmd)
{

	struct char_data *lv_mob;
	int lv_skill_type, lv_skill_value, lv_value, lv_sign, zone;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* ************* */
	/* MOB      */
	/* ************* */
	for (; isspace(*arg); arg++);

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		send_to_char("A mob number is required.\r\n", ch);
		send_to_char(MSKILL_FORMAT, ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Mob number must be numeric.\r\n", ch);
		send_to_char(RATTACK_FORMAT, ch);
		return;
	}

	lv_value = atoi(buf);
	lv_mob = db5000_read_mobile(lv_value, PROTOTYPE);
	if (!(lv_mob)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "I'm sorry, %d is an invalid mob number.\r\n", lv_value);
		send_to_char(buf, ch);
		return;
	}

	/* ************ */
	/* SKILL TYPE  */
	/* ************ */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("Sorry, but a skill type is required.\r\n", ch);
		send_to_char("Type is <skill>, <spell> or <#>.\r\n", ch);
		send_to_char(buf2, ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (is_number(buf)) {
		lv_skill_type = atoi(buf);
	}
	else {
		lv_skill_type = old_search_block(buf, 0, strlen(buf),
						 spell_names, 0);
		if (lv_skill_type < 1) {
			sprintf(buf2, "Sorry, but skill type: %s is invalid.\r\n",
				buf);
			send_to_char(buf2, ch);
			send_to_char("Type is <skill>, <spell> or <#>.\r\n", ch);
			return;
		}
	}

	/* IS SKILL IN CORRECT RANGE? - DON'T LET IT GO OVER ARRAY BOUNDRY */
	if (lv_skill_type < 1 || lv_skill_type >= MAX_SKILLS) {
		sprintf(buf, "Skill type must be between 0 and %d.\r\n",
			MAX_SKILLS);
		send_to_char(buf, ch);
		send_to_char(MSKILL_FORMAT, ch);
		return;
	}

	/* SOME SKILLS CAN'T BE CHANGED */
	if (lv_skill_type == SKILL_SLASH ||
	    lv_skill_type == SKILL_BLUDGEON ||
	    lv_skill_type == SKILL_PIERCE) {
		send_to_char("Sorry, but you can't set this skill.  It defaults to (level / 5 * 10).", ch);
		return;
	}

	/* CAN MOBS CAST IT? */
	/* Add this after skills have a MOB_SKILL added */
	if (!IS_SET(spell_info[lv_skill_type].spell_flag, MOB_SPELL) &&
	    !IS_SET(spell_info[lv_skill_type].spell_flag, MOB_SKILL)) {
		send_to_char("Sorry, but you can't set this spell/skill for mobs.", ch);
		return;
	}

	/* ************* */
	/* SKILL VALUE  */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("A value is required.\r\n", ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Skill value must be numeric.\r\n", ch);
		send_to_char(MSKILL_FORMAT, ch);
		return;
	}

	lv_skill_value = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_skill_value < 0 || lv_skill_value > 99)) {
		send_to_char("Skill value must be between 0 and 99.\r\n", ch);
		return;
	}

	/* UPDATE DATABASE */
	bzero(buf, sizeof(buf));
	sprintf(buf, "Setting mob %s %s[%d] from %d to %d.\r\n",
		GET_REAL_NAME(lv_mob),
		sk_sp_defines[lv_skill_type - 1],
		lv_skill_type,
		lv_mob->skills[lv_skill_type].learned,
		lv_skill_value);
	send_to_char(buf, ch);
	lv_mob->skills[lv_skill_type].learned = lv_skill_value;
	SET_BIT(zone_table[mob_index[lv_mob->nr].zone].dirty_mob, UNSAVED);

	return;

}				/* END OF void cr4000_do_mskill() */


void cr4100_do_mattack(struct char_data * ch, char *arg, int cmd)
{

	int lv_mob, lv_slot, lv_type, lv_second_skill, lv_third_skill, lv_fourth_skill, lv_value, lv_sign, idx, zone;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];



	/* ************* */
	/* MOB      */
	/* ************* */
	for (; isspace(*arg); arg++);

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		send_to_char("A mob number is required.\r\n", ch);
		send_to_char(RATTACK_FORMAT, ch);
		return;
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Mob number must be numeric.\r\n", ch);
		send_to_char(RATTACK_FORMAT, ch);
		return;
	}

	lv_value = atoi(buf);
	lv_mob = db8100_real_mobile(lv_value);
	if (lv_mob < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "I'm sorry, %d is an invalid mob number.\r\n", lv_value);
		send_to_char(buf, ch);
		return;
	}

	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* MAKE SURE WE DON'T HAVE VALUES THAT WILL CRASH US */
	for (idx = 0; idx < MAX_SPECIAL_ATTACK; idx++) {
		if (mob_index[lv_mob].attack_type[idx] < TYPE_HIT ||
		    mob_index[lv_mob].attack_type[idx] > TYPE_CRUSH) {
			mob_index[lv_mob].attack_type[idx] = 0;
		}
	}

	/* ************* */
	/* SLOT      */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	if (!(*buf)) {
		bzero(buf, sizeof(buf));
		send_to_char(RATTACK_FORMAT, ch);
		lv_value = FALSE;
		for (idx = 0; idx < MAX_SPECIAL_ATTACK; idx++) {
			if (mob_index[lv_mob].attack_type[idx] != 0) {
				lv_value = TRUE;
				sprintf(buf, "[%s] current attack[%d]: %s[%d] %d %d %d.\r\n",
					mob_index[lv_mob].name,
					idx + 1,
					attack_types[mob_index[lv_mob].attack_type[idx] - 99],
					mob_index[lv_mob].attack_type[idx],
					mob_index[lv_mob].attack_skill[idx][0],
					mob_index[lv_mob].attack_skill[idx][1],
				      mob_index[lv_mob].attack_skill[idx][2]);
				send_to_char(buf, ch);
			}
		}
		if (lv_value == FALSE) {
			send_to_char("There are no special attacks for this mob.\r\n", ch);
		}
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Slot must be numeric.\r\n", ch);
		send_to_char(RATTACK_FORMAT, ch);
		return;
	}

	lv_slot = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if (lv_slot < 1 || lv_slot > MAX_SPECIAL_ATTACK) {
		sprintf(buf, "Slot must be between 1 and %d.\r\n",
			MAX_SPECIAL_ATTACK);
		send_to_char(buf, ch);
		send_to_char(RATTACK_FORMAT, ch);
		return;
	}

	/* ADJUST lv_SLOT for 0 array boundaries */
	lv_slot--;

	/* ************* */
	/* TYPE     */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!*(buf)) {
		send_to_char("Sorry, but a type is required.\r\n", ch);
		send_to_char("Type is NONE, HIT, BLUDGEON, PIERCE, SLASH, WHIP, CLAW, BITE, STING, CRUSH\r\n", ch);
		send_to_char(buf2, ch);
		return;
	}
	lv_type = old_search_block(buf, 0, strlen(buf), attack_types, 0);
	lv_type--;
	if (lv_type < 0) {
		sprintf(buf2, "Sorry, but type: %s is invalid.\r\n", buf);
		send_to_char("Type is NONE, HIT, BLUDGEON, PIERCE, SLASH, WHIP, CLAW, BITE, STING, CRUSH\r\n", ch);
		send_to_char(buf2, ch);
		return;
	}

	/* ************* */
	/* SECOND SKILL */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* IF TYPE IS none, zero out the rest */
	if (lv_type == 0) {
		strcpy(buf, "0");
	}

	if (!*(buf)) {
		strcpy(buf, "0");
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char(RATTACK_FORMAT, ch);
		send_to_char("Second skill must be numeric.\r\n", ch);
		return;
	}

	lv_second_skill = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_second_skill < 0 || lv_second_skill > 99)) {
		send_to_char("Second skill must be between 0 and 99.\r\n", ch);
		return;
	}

	/* ************* */
	/* THIRD SKILL  */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* IF TYPE IS none, zero out the rest */
	if (lv_type == 0) {
		strcpy(buf, "0");
	}

	if (!*(buf)) {
		strcpy(buf, "0");
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char(RATTACK_FORMAT, ch);
		send_to_char("Third skill must be numeric.\r\n", ch);
		return;
	}

	lv_third_skill = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_third_skill < 0 || lv_third_skill > 99)) {
		send_to_char("Third skill must be between 0 and 99.\r\n", ch);
		return;
	}

	/* ************* */
	/* FOURTH SKILL */
	/* ************* */
	for (; isspace(*arg); arg++);
	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	/* IF TYPE IS none, zero out the rest */
	if (lv_type == 0) {
		strcpy(buf, "0");
	}

	if (!*(buf)) {
		strcpy(buf, "0");
	}

	/* NEGATIVE SIGN? */
	lv_sign = 1;
	if (*arg == '-') {
		lv_sign = -1;
		arg++;
		for (; isspace(*arg); arg++);	/* just in case */
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char(RATTACK_FORMAT, ch);
		send_to_char("Fourth skill must be numeric.\r\n", ch);
		return;
	}

	lv_fourth_skill = atoi(buf) * lv_sign;

	/* IS VALUE IN CORRECT LIMITS? */
	if ((lv_fourth_skill < 0 || lv_fourth_skill > 99)) {
		send_to_char("Fourth skill must be between 0 and 99.\r\n", ch);
		return;
	}

	/* UPDATE PROCEDURES */
	if (lv_type == 0)
		mob_index[lv_mob].attack_type[lv_slot] = 0;
	else
		mob_index[lv_mob].attack_type[lv_slot] = lv_type + 99;
	mob_index[lv_mob].attack_skill[lv_slot][0] = lv_second_skill;
	mob_index[lv_mob].attack_skill[lv_slot][1] = lv_third_skill;
	mob_index[lv_mob].attack_skill[lv_slot][2] = lv_fourth_skill;

	bzero(buf, sizeof(buf));
	for (idx = 0; idx < MAX_SPECIAL_ATTACK; idx++) {
		if (mob_index[lv_mob].attack_type[idx] != 0) {
			sprintf(buf, "[%s] current attack[%d]: %s[%d] %d %d %d.\r\n",
				mob_index[lv_mob].name,
				idx + 1,
			attack_types[mob_index[lv_mob].attack_type[idx] - 99],
				mob_index[lv_mob].attack_type[idx],
				mob_index[lv_mob].attack_skill[idx][0],
				mob_index[lv_mob].attack_skill[idx][1],
				mob_index[lv_mob].attack_skill[idx][2]);
			send_to_char(buf, ch);
		}
	}

	send_to_char("Mattack updated.\r\n", ch);
	SET_BIT(zone_table[mob_index[lv_mob].zone].dirty_mob, UNSAVED);

	return;

}				/* END OF void cr4100_do_mattack() */


void do_mcopy(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone, lv_value, idx;
	struct char_data *mob, *lv_source_mob;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* check for empty */
	if (zone_table[zone].bot_of_mobt == -1)
		zone_table[zone].bot_of_mobt = top_of_mobt + 1;
	else {
		/* and what if its full */
		if (mob_index[zone_table[zone].top_of_mobt].virtual + 1 > zone_table[zone].top) {
			send_to_char("Sorry but this zone is full... nothing more can be added\r\n", ch);
			return;
		}
	}

	/* ************ */
	/* SOURCE MOB  */
	/* ************ */
	for (; isspace(*arg); arg++);

	bzero(buf, sizeof(buf));
	arg = one_argument(arg, buf);

	if (!(*buf)) {
		send_to_char("A mob number is required.\r\n", ch);
		send_to_char("Format: MCOPY <source mob>\r\n", ch);
		return;
	}

	/* IS VALUE NUMERIC */
	if (!is_number(buf)) {
		send_to_char("Mob number must be numeric.\r\n", ch);
		return;
	}

	lv_value = atoi(buf);
	lv_source_mob = db5000_read_mobile(lv_value, PROTOTYPE);
	if (!(lv_source_mob)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "I'm sorry, %d is an invalid mob number.\r\n", lv_value);
		send_to_char(buf, ch);
		return;
	}

	/* KEEP EM OUT OF XANTH */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    !strcmp(zone_table[zone].filename, "Xanth")) {
		send_to_char("You can't use chars from that zone.\r\n", ch);
		return;
	}

	/* fix the top/bottom of the index for the new entry */
	top_of_mobt++;
	db2300_allocate_mob(top_of_mobt);

	SET_BIT(zone_table[zone].dirty_mob, UNSAVED);
	SET_BIT(zone_table[zone].dirty_mob, SCRAMBLED);
	if (zone_table[zone].top_of_mobt >= 0) {
		mob_index[top_of_mobt].virtual = mob_index[zone_table[zone].top_of_mobt].virtual + 1;
		mob_index[zone_table[zone].top_of_mobt].next = top_of_mobt;
	}
	else {
		mob_index[top_of_mobt].virtual = zone_table[zone].bottom;
	}

	mob_index[top_of_mobt].next = top_of_mobt + 1;
	mob_index[top_of_mobt].zone = zone;
	mob_index[top_of_mobt].number = 0;
	mob_index[top_of_mobt].func = 0;
	mob_index[top_of_mobt].next = top_of_mobt + 1;
	zone_table[zone].top_of_mobt = top_of_mobt;

	/* give it some initial values */
	CREATE(mob, struct char_data, 1);
	db7500_clear_char(mob);

	GET_LEVEL(mob) = GET_LEVEL(lv_source_mob);

	sprintf(buf, "mcopyof%d", lv_value);
	mob_index[top_of_mobt].name = strdup(buf);
	mob->player.name = strdup(buf);

	mob->player.short_descr = strdup(lv_source_mob->player.short_descr);
	mob->player.long_descr = strdup(lv_source_mob->player.long_descr);
	mob->player.description = strdup(lv_source_mob->player.description);
	mob->player.title = 0;
	mob->player.weight = 1;
	mob->player.sex = 0;
	mob->player.class = 0;
	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon = mob->player.time.birth;
	mob->player.height = 198;

	mob->specials = lv_source_mob->specials;
	mob->points = lv_source_mob->points;
	if (GET_GOLD(mob) > 1000000)
		GET_GOLD(mob) = 1000000;
	for (idx = 0; idx < MAX_SKILLS; idx++) {
		mob->skills[idx] = lv_source_mob->skills[idx];
	}

	mob->next = 0;
	mob->nr = top_of_mobt;
	mob->desc = 0;
	GET_RACE(mob) = GET_RACE(lv_source_mob);
	GET_CLASS(mob) = GET_CLASS(lv_source_mob);
	GET_REAL_STR(mob) = GET_REAL_STR(lv_source_mob);
	GET_REAL_INT(mob) = GET_REAL_INT(lv_source_mob);
	GET_REAL_DEX(mob) = GET_REAL_DEX(lv_source_mob);
	GET_REAL_WIS(mob) = GET_REAL_WIS(lv_source_mob);
	GET_REAL_CON(mob) = GET_REAL_CON(lv_source_mob);
	GET_REAL_CHA(mob) = GET_REAL_CHA(lv_source_mob);

	mob->in_room = NOWHERE;

	mob_index[top_of_mobt].prototype = (char *) mob;

	/* let 'em know what happened */
	sprintf(buf, "Done, mob #%d created!", mob_index[top_of_mobt].virtual);
	send_to_char(buf, ch);

	return;

}				/* END OF do_mcopy() */


void do_mcreate(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone;
	struct char_data *mob;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	/* check for empty */
	if (zone_table[zone].bot_of_mobt == -1)
		zone_table[zone].bot_of_mobt = top_of_mobt + 1;
	else {
		/* and what if its full */
		if (mob_index[zone_table[zone].top_of_mobt].virtual + 1 > zone_table[zone].top) {
			send_to_char("Sorry but this zone is full... nothing more can be added\r\n", ch);
			return;
		}
	}

	/* fix the top/bottom of the index for the new entry */
	top_of_mobt++;
	db2300_allocate_mob(top_of_mobt);
	SET_BIT(zone_table[zone].dirty_mob, UNSAVED);
	SET_BIT(zone_table[zone].dirty_mob, SCRAMBLED);
	if (zone_table[zone].top_of_mobt >= 0) {
		mob_index[top_of_mobt].virtual = mob_index[zone_table[zone].top_of_mobt].virtual + 1;
		mob_index[zone_table[zone].top_of_mobt].next = top_of_mobt;
	}
	else {
		mob_index[top_of_mobt].virtual = zone_table[zone].bottom;
	}
	mob_index[top_of_mobt].next = top_of_mobt + 1;
	mob_index[top_of_mobt].zone = zone;
	mob_index[top_of_mobt].number = 0;
	mob_index[top_of_mobt].func = 0;
	mob_index[top_of_mobt].next = top_of_mobt + 1;
	zone_table[zone].top_of_mobt = top_of_mobt;

	/* give it some initial values */
	CREATE(mob, struct char_data, 1);
	db7500_clear_char(mob);

	GET_LEVEL(mob) = 20;
	mob_index[top_of_mobt].name = str_alloc("mob blank");
	mob->player.name = str_alloc("mob blank");
	mob->player.short_descr = str_alloc("the Blank Mob");
	mob->player.long_descr = str_alloc("The Blank Mob standing here, looking formless.\r\n");
	mob->player.description = str_alloc("The Blank Mob is pretty formless, awaiting a creators molding.\r\n");
	mob->player.title = 0;

	mob->specials.act = PLR1_ISNPC;
	mob->specials.affected_by = 0;
	mob->specials.affected_02 = 0;
	mob->specials.affected_03 = 0;
	mob->specials.affected_04 = 0;
	mob->specials.alignment = 0;
	GET_LEVEL(mob) = 20;
	mob->points.hitroll = 0;
	mob->points.armor = MAX_ARMOUR;
	mob->points.max_hit = 1;
	mob->points.hit = 1;
	mob->player.weight = 1;
	mob->points.damroll = 1;
	mob->specials.damnodice = 1;
	mob->specials.damsizedice = 1;
	mob->points.mana = 10;
	mob->points.max_mana = 100;
	mob->points.move = 50;
	mob->points.max_move = 50;
	mob->points.gold = 1;
	mob->specials.position = POSITION_STANDING;
	mob->specials.default_pos = POSITION_STANDING;
	mob->player.sex = 0;
	mob->player.class = 0;
	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon = mob->player.time.birth;
	mob->player.height = 198;
	mob->next = 0;
	mob->nr = top_of_mobt;
	mob->desc = 0;
	GET_RACE(mob) = 0;
	GET_CLASS(mob) = 0;
	GET_REAL_STR(mob) = (GET_LEVEL(mob) / 4) + 10 + number(1, 2);
	GET_REAL_INT(mob) = (GET_LEVEL(mob) / 4) + 9 + number(1, 4);
	GET_REAL_DEX(mob) = (GET_LEVEL(mob) / 4) + 8 + number(1, 3);
	GET_REAL_WIS(mob) = (GET_LEVEL(mob) / 4) + 8 + number(1, 3);
	GET_REAL_CON(mob) = (GET_LEVEL(mob) / 4) + 6 + number(1, 4);
	GET_REAL_CHA(mob) = (GET_LEVEL(mob) / 4) + 5 + number(1, 5);

	mob->in_room = NOWHERE;

	mob_index[top_of_mobt].prototype = (char *) mob;

	/* let 'em know what happened */
	sprintf(buf, "Done, mob #%d created!", mob_index[top_of_mobt].virtual);
	send_to_char(buf, ch);

	return;

}				/* END OF do_mcreate() */


void do_mkeywords(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int zone;
	struct char_data *mob;


	half_chop(arg, buf2, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try mkeywords <mob> keywords\r\n", ch);
		return;
	}

	if (!is_number(buf2)) {
		send_to_char("The mob must be a number.\r\n", ch);
		return;
	}

	mob = db5000_read_mobile(atoi(buf2), PROTOTYPE);
	if (!(mob)) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (mob_index[mob->nr].name) {
		free(mob_index[mob->nr].name);
	}
	if (mob->player.name) {
		free(mob->player.name);
	}

	bzero(buf2, sizeof(buf2));
	if (strncmp(buf, "mob ", 4)) {
		sprintf(buf2, "mob %s", buf);
	}
	else {
		strcpy(buf2, buf);
	}

	mob_index[mob->nr].name = strdup(buf2);
	mob->player.name = strdup(buf2);

	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	return;

}				/* END OF do_mkeywords()  */


void do_mlist(struct char_data * ch, char *arg, int cmd)
{
	int i, zone, start, end, no = 1;
	char buf[2 * MAX_STRING_LENGTH], buf1[128], buf2[128];


	zone = world[ch->in_room].zone;
	half_chop(arg, buf1, buf2);

	if (is_number(buf1)) {
		start = atoi(buf1);
		zone = db4000_zone_of(start);
	}
	else
		start = zone_table[zone].bottom;

	if (is_number(buf2))
		end = atoi(buf2);
	else
		end = zone_table[zone].top;
	if (end < start)
		end = start + 15;
	if (end > zone_table[zone].top)
		end = zone_table[zone].top;
	if (end - start >= 100) {
		end = start + 99;
		send_to_char("limiting search to 200 mobs (try mlist <start num> <end num> for rest)\r\n", ch);
	}

	CAN_PERSON_UPDATE(IMO_IMM);

	sprintf(buf, "\r\n");	/* init buf to something for strcat */
	for (i = zone_table[zone].bot_of_mobt; (i >= 0) && (i <= top_of_mobt) && (mob_index[i].virtual <= end); i = mob_index[i].next) {
		if (mob_index[i].virtual >= start) {
			sprintf(buf1, "[%d] [%-28s] ", mob_index[i].virtual, mob_index[i].name);
			if (!(no % 2))
				strcat(buf, "\r\n");
			no++;
			strcat(buf, buf1);
		}
	}
	sprintf(buf1, "\r\nDone. mobs[%d to %d]\r\n", start, end);
	strcat(buf, buf1);
	if (ch->desc) {
		page_string(ch->desc, buf, 1);
	}
	return;
}


void do_mflag(struct char_data * ch, char *arg, int cmd)
{
	char lv_name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, lv_which_flag, zone, idx;
	unsigned long lv_flag_to_set, lv_current_flag;
	struct char_data *mob;


	half_chop(arg, buf1, buf2);

	if (!is_number(buf1) ||
	    !*buf2) {
		send_to_char("Try mflag <mob> <flag>\r\n", ch);
		sprintf(buf, "Mob flags: \r\n");
		for (idx = 0; *player_bits1[idx] != '\n'; idx++) {
			strcat(buf, player_bits1[idx]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");

		for (idx = 0; *player_bits2[idx] != '\n'; idx++) {
			strcat(buf, player_bits2[idx]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");

		for (idx = 0; *player_bits3[idx] != '\n'; idx++) {
			strcat(buf, player_bits3[idx]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");

		send_to_char(buf, ch);
		return;
	}

	if (!(mob = db5000_read_mobile(atoi(buf1), PROTOTYPE))) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	loc = (old_search_block(buf2, 0, strlen(buf2), player_bits1, -1));
	if (loc > 0) {
		lv_which_flag = 1;
		lv_current_flag = GET_ACT1(mob);
		strcpy(lv_name, player_bits1[loc - 1]);
	}
	else {
		loc = (old_search_block(buf2, 0, strlen(buf2), player_bits2, -1));
		if (loc >= 0) {
			lv_which_flag = 2;
			lv_current_flag = GET_ACT2(mob);
			strcpy(lv_name, player_bits2[loc - 1]);
		}
		else {
			loc = old_search_block(buf2, 0, strlen(buf2), player_bits3, -1);
			if (loc >= 0) {
				lv_which_flag = 3;
				lv_current_flag = GET_ACT3(mob);
				strcpy(lv_name, player_bits3[loc - 1]);
			}
		}
	}

	if (loc < 1) {
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}

	/* ADJUST FOR 0 ARRAY BOUNDARY */
	loc--;

	/* COMPUTE FLAG */
	lv_flag_to_set = 1;
	for (idx = 0; idx < loc; idx++)
		lv_flag_to_set *= 2;

	if (IS_SET(lv_current_flag, lv_flag_to_set)) {
		REMOVE_BIT(lv_current_flag, lv_flag_to_set);
		sprintf(buf, "You have removed the '%s' flag.\r\n", lv_name);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", lv_name);
		SET_BIT(lv_current_flag, lv_flag_to_set);
		send_to_char(buf, ch);
	}

	if (lv_which_flag == 1)
		GET_ACT1(mob) = lv_current_flag;
	else {
		if (lv_which_flag == 2)
			GET_ACT2(mob) = lv_current_flag;
		else {
			if (lv_which_flag == 3)
				GET_ACT3(mob) = lv_current_flag;
		}
	}

	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
}

void do_msex(struct char_data * ch, char *arg, int cmd)
{
	char buf[512], buf1[512], buf2[512];
	int loc, zone, i;
	struct char_data *mob;
	const char *sex[] = {
		"it",
		"male",
		"female",
		"\n"
	};


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try mflag <mob> <flag>\r\n", ch);
		sprintf(buf, "Mob flags: \r\n");
		for (i = 0; *sex[i] != '\n'; i++) {
			strcat(buf, sex[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), sex, 0));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That sex doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(mob = db5000_read_mobile(atoi(buf1), PROTOTYPE))) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	mob->player.sex = loc;
	sprintf(buf, "You have made the %s '%s'.\r\n", ha1100_fname(mob_index[mob->nr].name), sex[loc]);
	send_to_char(buf, ch);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
}

void do_maffect(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int loc, zone, i;
	unsigned long flag;
	struct char_data *mob;


	half_chop(arg, buf1, buf2);
	if (!is_number(buf1) || !*buf2) {
		send_to_char("Try maffect <mob> <flag>\r\n", ch);
		sprintf(buf, "Mob flags: \r\n");
		for (i = 0; *affected_bits[i] != '\n'; i++) {
			strcat(buf, affected_bits[i]);
			strcat(buf, " ");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	loc = (old_search_block(buf2, 0, strlen(buf2), affected_bits, -1));
	if (loc == -1) {	/* the -1 in old_search_block lowercases the
				 * list */
		send_to_char("That flag doesn't exist.\r\n", ch);
		return;
	}
	loc--;			/* search block returns 1 as first position */

	if (!(mob = db5000_read_mobile(atoi(buf1), PROTOTYPE))) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	flag = 1;
	for (i = 0; i < loc; i++)
		flag *= 2;

	if (IS_SET(mob->specials.affected_by, flag)) {
		sprintf(buf, "You have removed the '%s' flag.\r\n", affected_bits[loc]);
		REMOVE_BIT(mob->specials.affected_by, flag);
		send_to_char(buf, ch);
	}
	else {
		sprintf(buf, "You have set the '%s' flag.\r\n", affected_bits[loc]);
		SET_BIT(mob->specials.affected_by, flag);
		send_to_char(buf, ch);
	}
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
}


void do_msdesc(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct char_data *mob;


	half_chop(arg, buf2, buf);
	if (!is_number(buf2) || !*buf) {
		send_to_char("Try msdesc <mob> msdesc\r\n", ch);
		return;
	}

	mob = db5000_read_mobile(atoi(buf2), PROTOTYPE);
	if (!(mob)) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (mob->player.short_descr) {
		free(mob->player.short_descr);
		mob->player.short_descr = 0;
		ha9900_sanity_check(0, "FREE25", "SYSTEM");
	}

	mob->player.short_descr = strdup(buf);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	return;

}				/* END OF do_msdesc() */


void do_mldesc(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int zone;
	struct char_data *mob;


	half_chop(arg, buf2, buf);
	if (!is_number(buf2) || !*buf) {
		send_to_char("Try mldesc <mob> mldesc\r\n", ch);
		return;
	}

	mob = db5000_read_mobile(atoi(buf2), PROTOTYPE);
	if (!(mob)) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (mob->player.long_descr) {
		free(mob->player.long_descr);
	}
	strcat(buf, "\r\n");
	mob->player.long_descr = strdup(buf);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	return;

}				/* END OF do_mldesc() */


void do_mdesc(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int zone;
	struct char_data *mob;

	half_chop(arg, buf2, buf);
	if (!is_number(buf2) || !*buf) {
		send_to_char("Try mdesc <mob> <new || append>\r\n", ch);
		return;
	}

	mob = db5000_read_mobile(atoi(buf2), PROTOTYPE);
	if (!(mob)) {
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}


	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (mob->player.description) {
		send_to_char("The old description was: \r\n", ch);
		send_to_char(mob->player.description, ch);
		if (!is_abbrev(buf, "append")) {
			free((char *) mob->player.description);
			mob->player.description = 0;
		}
	}
	else {
		mob->player.description = strdup("\000");
	}
	send_to_char("\r\nEnter a new description.  Terminate with a '@'\r\n", ch);
	ch->desc->str = &(mob->player.description);
	ch->desc->max_str = 2048;
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	return;

}				/* END OF do_mdesc() */


void do_mhitroll(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct char_data *mob;
	int zone, lv_new_value, lv_old_value;

	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try mhitroll <mob> <value 0 to 99>\r\n", ch);
		return;
	}

	if (!(mob = db5000_read_mobile(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}


	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (!is_number(buf)) {
		send_to_char("Sorry, but the hitroll must be 0 to 99.\r\n", ch);
		return;
	}

	lv_new_value = atoi(buf);
	if (lv_new_value < 0 || lv_new_value > 99) {
		send_to_char("Sorry, hitroll must be 0 to 99.\r\n", ch);
		return;
	}

	lv_old_value = mob->points.hitroll;
	mob->points.hitroll = lv_new_value;
	sprintf(buf, "Hitroll for %s changed from %d to %d.\r\n",
		GET_REAL_NAME(mob), lv_old_value, mob->points.hitroll);
	send_to_char(buf, ch);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	return;

}				/* END OF do_mhitroll */


void do_mhitpoints(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct char_data *mob;
	char throwaway;
	int zone, one, two, three, old_one, old_two, old_three;


	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try mhitpoints <mob> <value>d<value>+<value>\r\n", ch);
		return;
	}

	if (!(mob = db5000_read_mobile(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	sscanf(buf, "%d %c %d + %d", &one, &throwaway, &two, &three);
	if (one < 0 || one > 100) {
		send_to_char("Sorry, but the number of dice must be from 1 to 100.\r\n", ch);
		return;
	}
	if (two < 0 || two > 125) {
		send_to_char("Sorry, but the dice size must be from 1 to 125.\r\n", ch);
		return;
	}
	if (three < 0 || three > 500000000) {
		send_to_char("Sorry, but the + adjustment must be from 1 to 500000000 thousand.\r\n", ch);
		return;
	}

	old_one = mob->points.max_hit;
	old_two = mob->player.weight;
	old_three = mob->points.hit;
	mob->points.max_hit = MAXV(1, one);
	mob->player.weight = MAXV(1, two);
	mob->points.hit = MAXV(0, three);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	sprintf(buf, "Setting [mob %s] hits from %dd%d+%d to %dd%d+%d.\r\n",
		mob->player.short_descr,
		old_one,
		old_two,
		old_three,
		one,
		two,
		three);
	send_to_char(buf, ch);
	return;

}				/* END OF do_mhitpoints() */


void do_mdamage(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct char_data *mob;
	char throwaway;
	int zone, one, two, three, old_one, old_two, old_three;


	arg = one_argument(arg, buf2);
	arg = one_argument(arg, buf);
	if (!*buf2 || !*buf) {
		send_to_char("Try mdamage <mob> <value>d<value>+<value>\r\n", ch);
		return;
	}

	if (!(mob = db5000_read_mobile(atoi(buf2), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	sscanf(buf, "%d %c %d + %d", &one, &throwaway, &two, &three);
	if (one < 0 || one > 100) {
		send_to_char("Sorry, but the number of dice must be from 1 to 100.\r\n", ch);
		return;
	}
	if (two < 0 || two > 125) {
		send_to_char("Sorry, but the dice size must be from 1 to 125.\r\n", ch);
		return;
	}
	if (three < 0 || three > 1000) {
		send_to_char("Sorry, but the + adjustment must be from 1 to 1000.\r\n", ch);
		return;
	}
	old_one = mob->specials.damnodice;
	old_two = mob->specials.damsizedice;
	old_three = mob->points.damroll;
	mob->specials.damnodice = MAXV(1, one);
	mob->specials.damsizedice = MAXV(1, two);
	mob->points.damroll = MAXV(0, three);
	SET_BIT(zone_table[mob_index[mob->nr].zone].dirty_mob, UNSAVED);
	sprintf(buf, "Setting [mob %s] hits from %dd%d+%d to %dd%d+%d.\r\n",
		mob->player.short_descr,
		old_one,
		old_two,
		old_three,
		one,
		two,
		three);
	send_to_char(buf, ch);


}				/* END OF do_mdamage() */


void do_mstat(struct char_data * ch, char *arg, int cmd)
{
	int zone, idx;
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];

	struct char_data *mob;


	arg = one_argument(arg, buf);
	if (!is_number(buf)) {
		send_to_char("Try mstat <mob>\r\n", ch);
		return;
	}

	if (!(mob = db5000_read_mobile(atoi(buf), PROTOTYPE))) {	/* if already loaded
									 * will do nothing */
		send_to_char("Sorry don't know that mob.\r\n", ch);
		return;
	}

	zone = mob_index[mob->nr].zone;
	CAN_PERSON_UPDATE(IMO_IMM);

	/* output the results of the stat */
	sprintf(buf, "Mob#:[%d]  Name:[%s] Sex:[", mob_index[mob->nr].virtual, mob->player.name);
	switch (mob->player.sex) {
	case SEX_NEUTRAL:
		strcat(buf, "IT]\r\n");
		break;
	case SEX_MALE:
		strcat(buf, "Male]\r\n");
		break;
	case SEX_FEMALE:
		strcat(buf, "Female]\r\n");
		break;
	default:
		strcpy(buf, "???]\r\n");
		break;
	}
	send_to_char(buf, ch);


	sprintf(buf, "sdesc:[%s] name:[%s]\r\nldesc:[%s] Description:\r\n%s\r\n",
		((mob->player.short_descr) ? mob->player.short_descr : "None"),
		ha1100_fname(mob_index[mob->nr].name),
		((mob->player.long_descr) ? mob->player.long_descr : "None"),
	      ((mob->player.description) ? mob->player.description : "None"));
	send_to_char(buf, ch);

	send_to_char("Act1 : ", ch);
	sprintbit(GET_ACT1(mob), player_bits1, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("Act2 : ", ch);
	sprintbit(GET_ACT2(mob), player_bits2, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("Act3 : ", ch);
	sprintbit(GET_ACT3(mob), player_bits3, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	send_to_char("Aff : ", ch);
	sprintbit(mob->specials.affected_by, affected_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	sprintf(buf, "Race: %s  Class: %s\r\n",
		races[GET_RACE(mob)].name, classes[GET_CLASS(mob)].name);
	send_to_char(buf, ch);

	/* STATS */
	sprintf(buf, "Base/adj STR:[%d] INT:[%d] WIS:[%d] DEX:[%d] CON:[%d] CHA:[%d]\r\n",
		GET_REAL_STR(mob),
		GET_REAL_INT(mob),
		GET_REAL_WIS(mob),
		GET_REAL_DEX(mob),
		GET_REAL_CON(mob),
		GET_REAL_CHA(mob));
	send_to_char(buf, ch);
	sprintf(buf, "Align:[%d] Level:[%d] Gold:[%d] Experience:[%d]\r\n",
		mob->specials.alignment, GET_LEVEL(mob), mob->points.gold, GET_EXP(mob));
	send_to_char(buf, ch);

	sprintf(buf, "AC:[%d] HP:[%dD%d+%d] HITROLL:[%d] Damage:[%dD%d+%d]\r\n",
		mob->points.armor,
		mob->points.max_hit,
		mob->player.weight,
		mob->points.hit,
		mob->points.hitroll,
		mob->specials.damnodice,
		mob->specials.damsizedice,
		mob->points.damroll);
	send_to_char(buf, ch);

	for (idx = 0; idx < MAX_SPECIAL_ATTACK; idx++) {
		if (mob_index[mob->nr].attack_type[idx] != 0) {
			sprintf(buf, "Special attack[%d]: %s[%d] %d %d %d.\r\n",
				idx + 1,
			attack_types[mob_index[mob->nr].attack_type[idx] - 99],
				mob_index[mob->nr].attack_type[idx],
				mob_index[mob->nr].attack_skill[idx][0],
				mob_index[mob->nr].attack_skill[idx][1],
				mob_index[mob->nr].attack_skill[idx][2]);
			send_to_char(buf, ch);
		}
	}
	sprintf(buf1, "MSTAT: %s did mstat %d.", GET_REAL_NAME(ch), mob_index[mob->nr].virtual);
	spec_log(buf1, GOD_COMMAND_LOG);
	return;
}


void do_msave(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int zone;


	zone = world[ch->in_room].zone;
	CAN_PERSON_UPDATE(DEFAULT_UPDATE_LEV);

	if (IS_SET(zone_table[world[ch->in_room].zone].dirty_mob, UNSAVED)) {
		db9300_save_mob(world[ch->in_room].zone);
		sprintf(buf, "areas/%s.mob saved.\r\n",
			zone_table[world[ch->in_room].zone].filename);
	}
	else {
		sprintf(buf, "areas/%s.mob unchanged since last save.\r\n",
			zone_table[world[ch->in_room].zone].filename);
	}
	send_to_char(buf, ch);
	return;

}				/* END OF do_msave() */


void do_mob_report(struct char_data * ch, char *arg, int cmd)
{


	send_to_char("Nothing happens\r\n", ch);
	return;
}				/* END OF do_mob_report() */


void do_obj_report(struct char_data * ch, char *arg, int cmd)
{

	FILE *rpt_file, *index_file;
	int idx, jdx, lv_line_count, lv_item_count, lv_type, lv_special;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
	struct obj_data *obj;
	const char *types[] = {
		"about",
		"arms",
		"body",
		"feet",
		"finger",
		"hands",
		"head",
		"hold,",
		"legs",
		"neck",
		"shield",
		"waist",
		"wield",
		"wrist",
		"tail",
		"4legs",
		"\n",
	};
	unsigned int type_bit[] = {
		ITEM_WEAR_ABOUT,
		ITEM_WEAR_ARMS,
		ITEM_WEAR_BODY,
		ITEM_WEAR_FEET,
		ITEM_WEAR_FINGER,
		ITEM_WEAR_HANDS,
		ITEM_WEAR_HEAD,
		ITEM_HOLD,
		ITEM_WEAR_LEGS,
		ITEM_WEAR_NECK,
		ITEM_WEAR_SHIELD,
		ITEM_WEAR_WAISTE,
		ITEM_WIELD,
		ITEM_WEAR_WRIST,
		ITEM_WEAR_TAIL,
		ITEM_WEAR_4LEGS,
		0,
	};


	for (; *arg == ' '; arg++);
	arg = one_argument(arg, buf);

	if (*buf) {
		lv_type = old_search_block(buf, 0, strlen(buf), types, 0);
		if (lv_type < 1) {
			sprintf(buf2, "Sorry, but type: %s is invalid.\r\n", buf);
			send_to_char(buf2, ch);
			strcpy(buf, "Try: ");
			for (idx = 0; *types[idx] && *types[idx] != '\n'; idx++) {
				strcat(buf, types[idx]);
				strcat(buf, " ");
			}
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
			return;
		}
		lv_special = type_bit[lv_type - 1];
	}
	else {
		lv_special = 0;
	}

	unlink("reporto.r1");
	rpt_file = fopen("reporto.r1", "wb");
	if (!rpt_file) {
		sprintf(buf, "Error %d opening reporto.r1.\r\n", errno);
		send_to_char(buf, ch);
		return;
	}

	unlink("reporto.r2");
	index_file = fopen("reporto.r2", "wb");
	if (!index_file) {
		sprintf(buf, "Error %d opening reporto.r2.\r\n", errno);
		send_to_char(buf, ch);
		return;
	}

	lv_item_count = 0;
	lv_line_count = 0;
	for (idx = 0; idx <= top_of_objt; idx++) {

		if (lv_line_count > 55) {
			lv_line_count = 0;
			WRITE_RPT_FILE("\014\r\n", rpt_file)
		}

		if (!(obj = db5100_read_object(obj_index[idx].virtual, PROTOTYPE))) {
			continue;
		}

		if (lv_special) {
			if (!CAN_WEAR(obj, lv_special)) {
				continue;
			}
		}

		lv_item_count++;

		sprintbit(obj->obj_flags.wear_flags, wear_bits, buf2);
		sprintf(buf, "%-20s  %5d  %s\r\n",
			ha1100_fname(obj_index[obj->item_number].name),
			obj_index[obj->item_number].virtual,
			buf2);
		fputs(buf, index_file);

		sprintf(buf, "%d  %s  Keywords:[%s] Type:[",
			obj_index[obj->item_number].virtual,
			ha1100_fname(obj_index[obj->item_number].name),
			obj->name);
		sprinttype(GET_ITEM_TYPE(obj), item_types, buf2);
		strcat(buf, buf2);
		strcat(buf, "]\r\n");
		WRITE_RPT_FILE(buf, rpt_file)
			sprintf(buf, "    %s\r\n",
		((obj->short_description) ? obj->short_description : "None"));
		WRITE_RPT_FILE(buf, rpt_file)
			strcpy(buf, "    Worn : ");
		sprintbit(obj->obj_flags.wear_flags, wear_bits, buf2);
		strcat(buf, buf2);
		strcat(buf, " - ");
		sprintbit(obj->obj_flags.flags1, oflag1_bits, buf2);
		strcat(buf, buf2);
		strcat(buf, "\r\n");
		WRITE_RPT_FILE(buf, rpt_file)
			if (GET_ITEM_TYPE(obj) == ITEM_WEAPON || GET_ITEM_TYPE(obj) == ITEM_QSTWEAPON) {
			strcpy(buf, "    Weapon: ");
			sprintbit(obj->obj_flags.value[0], weapon_bits, buf2);
			strcat(buf, buf2);
			strcat(buf, "\r\n");
			WRITE_RPT_FILE(buf, rpt_file)
		}

		bzero(buf, sizeof(buf));
		switch (obj->obj_flags.type_flag) {
		case ITEM_LIGHT:
			sprintf(buf, "    Colour:[%d] Type:[%d] Hours:[%d] Undef:[]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			break;
		case ITEM_SCROLL:
		case ITEM_POTION:
			sprintf(buf, "    Level: [%d] Spells: [%d], [%d], [%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
			break;
		case ITEM_WAND:
		case ITEM_STAFF:
		case ITEM_SPELL:
			sprintf(buf, "    Level[%d], Max-mana[%d], Left:[%d], Spell:[%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
			break;
		case ITEM_WEAPON:
			sprintf(buf, "    Values: Dam:[%d]D[%d] Type:[%d]\r\n",
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
			break;
		case ITEM_ARMOR:
			if (obj_index[obj->item_number].virtual >= OBJ_XANTH_BEG_MAGIC_ITEMS &&
			    obj_index[obj->item_number].virtual <= OBJ_XANTH_END_MAGIC_ITEMS) {
				sprintf(buf, "AC-apply:[%d] Max Charge:[%d] Charges:[%d] Undef:[%d]\r\n",
					obj->obj_flags.value[0],
					obj->obj_flags.value[1],
					obj->obj_flags.value[2],
					obj->obj_flags.value[3]);
			}
			else {
				sprintf(buf, "AC-apply:[%d] Race(Anti/Only):[%d] Anti_Class:[%d] Undef:[%d]\r\n",
					obj->obj_flags.value[0],
					obj->obj_flags.value[1],
					obj->obj_flags.value[2],
					obj->obj_flags.value[3]);
			}
			break;
		case ITEM_TRAP:
			sprintf(buf, "    Spell:[%d] Hitpoints:[%d] Undef:[] Undef:[]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1]);
			break;
		case ITEM_CONTAINER:
			sprintf(buf, "    Max-contains:[%d] Locktype:[%d] Undef:[] Corpse:[%s]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[3] ? "Yes" : "No");
			break;
		case ITEM_DRINKCON:
			sprinttype(obj->obj_flags.value[2], drinks, buf2);
			sprintf(buf, "    Max:[%d] Contains:[%d] Liquid:[%s] Poisoned:[%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				buf2,
				obj->obj_flags.value[3]);
			break;
		case ITEM_NOTE:
			sprintf(buf, "    Tongue:[%d] Undef:[] Undef:[] Undef:[]\r\n",
				obj->obj_flags.value[0]);
			break;
		case ITEM_KEY:
			sprintf(buf, "    opens obj:[%d]  # of uses:[%d] Undef:[] Undef:[]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1]);
			break;
		case ITEM_FOOD:
			sprintf(buf, "    Makes-full:[%d] Poisoned:[%d] Undef:[] Undef:[]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[3]);
			break;
		case ITEM_QUEST:
			sprintf(buf, "AC-apply :[%d] Undef:[] Hitpoints :[%d] Undef:[]\r\n",
				obj->obj_flags.value[0],
				//obj->obj_flags.value[1],
				obj->obj_flags.value[2]);
			//obj->obj_flags.value[3]);
			break;
		case ITEM_QSTWEAPON:
			sprintf(buf, "    Wflag:[%d] Dam:[%d]D[%d] Type:[%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
			break;
		case ITEM_QSTLIGHT:
			sprintf(buf, "    Undef:[] Undef:[] Hours:[%d] Undef:[]\r\n",
				obj->obj_flags.value[2]);
			break;
		case ITEM_QSTCONT:
			sprintf(buf, "    Max-contains:[%d] Undef:[] Undef:[] Undef:[]\r\n",
				obj->obj_flags.value[0]);
			break;
		default:
			sprintf(buf, "    Values 0-3 : [%d] [%d] [%d] [%d]\r\n",
				obj->obj_flags.value[0],
				obj->obj_flags.value[1],
				obj->obj_flags.value[2],
				obj->obj_flags.value[3]);
			break;
		}		/* END OF switch() */
		WRITE_RPT_FILE(buf, rpt_file)
			strcpy(buf, "    Affect: ");
		for (jdx = 0; jdx < MAX_OBJ_AFFECT; jdx++) {
			sprinttype(obj->affected[jdx].location, apply_types, buf2);
			if (strncmp(buf2, "NONE", 4)) {
				sprintf(buf3, " %s By %d ", buf2, obj->affected[jdx].modifier);
				strcat(buf, buf3);
			}
		}
		strcat(buf, "\r\n\r\n");
		WRITE_RPT_FILE(buf, rpt_file)
	}			/* END OF for loop */

	sprintf(buf, "Reported on %d items.\r\n", lv_item_count);
	WRITE_RPT_FILE(buf, rpt_file)
		fclose(rpt_file);
	fclose(index_file);
	send_to_char("Files created\r\n", ch);
	return;

}				/* END OF do_obj_report() */
