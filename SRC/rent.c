/* rn */
/* gv_location: 14001-14500 */
/* ***************************************************************** *
*  file: rent.c, Special module for Inn/Bank.        Part of DIKUMUD *
*  Usage: Procedures handling saving/loading of player objects       *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete inform. *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#if !defined(DIKU_WINDOWS)
#include <sys/time.h>
#include <unistd.h>
#endif

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "func.h"
#include "rent.h"
#include "globals.h"
#include "mobact.h"

#define OBJ_ARRAY 100
void actual_max_check();

int r500_alias_get_filename(char *orig_name, char *filename)
{

	char *ptr, name[30];



	if (!*orig_name)
		return 0;

	strcpy(name, orig_name);
	for (ptr = name; *ptr; ptr++)
		*ptr = tolower(*ptr);

	if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
		sprintf(filename, "rentfiles/%c/%s.alias",
			*name, name);
	}
	else {
		printf("ERROR: invalid aliasfile for user: %s.\r\n", name);
		sprintf(filename, "rentfiles/1/%s.alias", name);
	}

	return 1;

}				/* END OF r500_alias_get_filename() */


int r1000_rent_get_filename(char *orig_name, char *filename)
{

	char *ptr, name[30];
	char buf[MAX_STRING_LENGTH];



	if (!*orig_name)
		return 0;

	strcpy(name, orig_name);
	for (ptr = name; *ptr; ptr++)
		*ptr = tolower(*ptr);

	if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
		sprintf(filename, "rentfiles/%c/%s.objs",
			*name, name);
	}
	else {
		printf("ERROR: invalid rentfile for user: %s.\r\n", name);
		sprintf(buf, "ERROR: invalid rentfile for user: %s.\r\n", name);
		spec_log(buf, ERROR_LOG);
		sprintf(filename, "rentfiles/1/%s.objs", name);
	}

	return 1;
}				/* END OF r1000_rent_get_filename() */

int r1050_backup_get_filename(char *orig_name, char *filename)
{

	char *ptr, name[30];
	char buf[MAX_STRING_LENGTH];



	if (!*orig_name)
		return 0;

	strcpy(name, orig_name);
	for (ptr = name; *ptr; ptr++)
		*ptr = tolower(*ptr);

	if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
		sprintf(filename, "rentfiles/backups/%c/%s.objs",
			*name, name);
	}
	else {
		printf("ERROR: invalid rentfile for user: %s.\r\n", name);
		sprintf(buf, "ERROR: invalid rentfile for user: %s.\r\n", name);
		spec_log(buf, ERROR_LOG);
		sprintf(filename, "rentfiles/backups/1/%s.objs", name);
	}

	return 1;
}				/* END OF int r1050_backup_get_filename() */

int r1100_rent_delete_file(char *name)
{

	char filename[MAX_STRING_LENGTH], file_old[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
	int rc;
	FILE *fl;



	if (!r1000_rent_get_filename(name, filename))
		return 0;

	strcpy(file_old, filename);
	strcat(file_old, ".sav");

	/* CONFIRM CURRENT FILE EXIST */
	if (!(fl = fopen(filename, "rb"))) {
		if (errno != ENOENT) {	/* fails but NOT because of no file */
			sprintf(buf1, "ERROR: deleting rent file %s (1)", filename);
			perror(buf1);
		}
		return 0;
	}
	fclose(fl);

	/* GET RID OF OLD .SAV FILE NAME */
	fl = fopen(file_old, "rb");
	if (fl) {
		fclose(fl);
		if (unlink(file_old) < 0) {
			if (errno != ENOENT) {
				sprintf(buf1, "ERROR: deleting rent file %s (2)", file_old);
				perror(buf1);
			}
			return (0);
		}
	}
	/* RENAME CURRENT FILE NAME */
	rc = rename(filename, file_old);
	if (rc && errno != ENOENT) {
		sprintf(buf1, "ERROR: renaming rent file %d(2).\r\n", rc);
		perror(buf1);
		return (0);
	}

	return (1);

}				/* END OF r1100_rent_delete_file() */

int r1125_backup_delete_file(char *name)
{

	char filename[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	FILE *fl;



	if (!r1050_backup_get_filename(name, filename))
		return 0;


	if (!(fl = fopen(filename, "rb"))) {
		if (errno != ENOENT) {	/* fails but NOT because of no file */
			sprintf(buf1, "ERROR: deleting backup file %s (1)", filename);
			perror(buf1);
		}
		return 1;
	}
	fclose(fl);

	if (unlink(filename) < 0) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "ERROR: deleting backup file %s (2)", filename);
			perror(buf1);
		}
		return 0;
	}

	return 1;
}


int r1150_alias_delete_file(char *name)
{

	char filename[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	FILE *fl;



	if (!r500_alias_get_filename(name, filename))
		return 0;


	if (!(fl = fopen(filename, "rb"))) {
		if (errno != ENOENT) {	/* fails but NOT because of no file */
			sprintf(buf1, "ERROR: deleting alias file %s (1)", filename);
			perror(buf1);
		}
		return 0;
	}
	fclose(fl);

	if (unlink(filename) < 0) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "ERROR: deleting alias file %s (2)", filename);
			perror(buf1);
		}
		return 0;
	}

	return (1);

}				/* END OF r1100_rent_delete_file() */


/* ***************************************************************** *
* Routines used for the "Offer"                                     *
******************************************************************* */

void r2000_add_obj_cost(struct char_data *ch,
                        struct char_data *re,
                        struct obj_data *obj, 
                        struct obj_cost *cost, 
                        int lv_flag)
{
	/* VERBOSE CONTROLS WHETHER WE PRINT DETAILS OR NOT */
	char buf[MAX_STRING_LENGTH];

	/* Add cost for an item and it's contents, and next->contents */
	if (obj) {
		if ((obj->item_number > -1) &&
		    (cost->ok) &&
		    (obj->obj_flags.cost_per_day > 0) &&
		    !(IS_OBJ_STAT(obj, OBJ1_NORENT))) {
			/* ONLY MINLVL41 stuff cost rent */
			if (IS_OBJ_STAT(obj, OBJ1_MINLVL41) || IS_OBJ_STAT2(obj, OBJ2_MINLVL42)) {
				cost->total_cost += (MAXV(0, obj->obj_flags.cost_per_day) / 10);
			}
			cost->no_carried++;
			if (IS_SET(lv_flag, RECEPTION_VERBOSE)) {
				if (re == ch) {
					if ((IS_OBJ_STAT(obj, OBJ1_MINLVL41) || IS_OBJ_STAT2(obj, OBJ2_MINLVL42)) &&
					    (obj->obj_flags.cost_per_day) / 10 > 0) {
						sprintf(buf, "    %s will cost %d coins.\r\n",
							GET_OBJ_NAME(obj), (obj->obj_flags.cost_per_day) / 10);
					}
					else {
						sprintf(buf, "    %s is free.\r\n", GET_OBJ_NAME(obj));
					}
					send_to_char(buf, ch);
				}
				else {
					if ((IS_OBJ_STAT(obj, OBJ1_MINLVL41) || IS_OBJ_STAT2(obj, OBJ2_MINLVL42)) &&
					    (obj->obj_flags.cost_per_day) / 10 > 0) {
						sprintf(buf, "$n tells you '$p will cost %d coins.'", (obj->obj_flags.cost_per_day) / 10);
					}
					else {
						sprintf(buf, "$n tells you '$p is free.");
					}
					act(buf, FALSE, re, obj, ch, TO_VICT);
				}
			}
			r2000_add_obj_cost(ch, re, obj->contains, cost, lv_flag);
			r2000_add_obj_cost(ch, re, obj->next_content, cost, lv_flag);
		}
		else if (cost->ok) {
			if (re == ch) {
				sprintf(buf, "You can't store %s\r\n", GET_OBJ_NAME(obj));
				send_to_char(buf, ch);
			}
			else
				act("$n tells you 'I refuse to store $p'", FALSE, re, obj, ch, TO_VICT);
			/* IGNORE BAD STUFF IF WE ARE FORCING THEM TO LOG OFF */
			if (!(IS_SET(lv_flag, RECEPTION_IGNORE))) {
				cost->ok = FALSE;
			}
		}

	}

}				/* END OF r2000_add_obj_cost() */


bool r2100_recep_offer(struct char_data *ch,
                       struct char_data *receptionist, 
                       struct obj_cost *cost,
                       int lv_flag)
{
	int i;
	char buf[MAX_STRING_LENGTH];

	cost->total_cost = 0;	/* Minimum cost - obj rent is added to this */
	cost->no_carried = 0;
	cost->ok = TRUE;	/* Use if any "-1"/NORENT objects */
	
	if (receptionist == ch) {
		if (IS_SET(lv_flag, RECEPTION_VERBOSE)) {
			sprintf(buf, "A room has a base price of %d coins.\r\n",
				cost->total_cost);
			send_to_char(buf, ch);
		}
	}
	else {
		sprintf(buf, "\r\n$n tells you 'A room has a base price of %d coins.'",
			cost->total_cost);
		act(buf, FALSE, receptionist, 0, ch, TO_VICT);
	}

	r2000_add_obj_cost(ch, receptionist, ch->carrying, cost, lv_flag);

	for (i = 0; i < MAX_WEAR; i++) {
		r2000_add_obj_cost(ch, receptionist, ch->equipment[i], cost, lv_flag);
	}

	if (!cost->ok)
		return (FALSE);

	if (cost->total_cost < 0) {
		if (receptionist == ch) {
			send_to_char("We can't afford to pay you to stay here\r\n", ch);
		}
		else {
			act("$n tells you 'I can't afford to pay you to stay here.'",
			    FALSE, receptionist, 0, ch, TO_VICT);
			act("$n tells you 'Come back when you have a fee greater than zero.",
			    FALSE, receptionist, 0, ch, TO_VICT);
		}
		return (FALSE);
	}

	if (cost->no_carried == 0) {
		if (receptionist == ch) {
			send_to_char("You don't have anything\r\n", ch);
		}
	}

	if (ch != receptionist && GET_LEVEL(ch) <= 5) {
		cost->total_cost = MAXV(2, cost->total_cost - 1000);
		sprintf(buf, "\r\n$n tells you 'Tell you what kid, I'll only charge you %d coins.'", cost->total_cost);
		act(buf, FALSE, receptionist, 0, ch, TO_VICT);
	}
	else {
		if (cost->total_cost > 0) {
			sprintf(buf, "          Under 5 hours is free, 5 to 10 hours is %d coins.", (int) cost->total_cost / 2);
			if (receptionist == ch) {
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
			}
			else {
				act(buf, FALSE, receptionist, 0, ch, TO_VICT);
			}
		}
		sprintf(buf, "          Maximum storage fee is %d coins.",
			cost->total_cost);
		if (receptionist == ch &&
		(IS_SET(lv_flag, RECEPTION_VERBOSE) || cost->total_cost > 0)) {
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}
		else
			act(buf, FALSE, receptionist, 0, ch, TO_VICT);
		if (cost->total_cost > 0 &&
		    IS_SET(lv_flag, RECEPTION_VERBOSE) &&
		    cost->total_cost > GET_GOLD(ch) + GET_INN_GOLD(ch)) {
			strcpy(buf, "          I don't know that you'll be back in an hour so");
			if (receptionist == ch) {
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
			}
			else
				act(buf, FALSE, receptionist, 0, ch, TO_VICT);
			sprintf(buf, "          you've gotta have the maximum and don't whine!");
			if (receptionist == ch) {
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
			}
			else
				act(buf, FALSE, receptionist, 0, ch, TO_VICT);
		}
	}

	//needed 2 ^ 32 instead of 2 ^ (32 - 1)
		// 6442450944
		// 4294967296
		unsigned total_gold;
	if (GET_GOLD(ch) >= 0) {
		total_gold = (unsigned) GET_GOLD(ch);
	}
	else {
		total_gold = 0;
	}
	if (GET_INN_GOLD(ch) >= 0) {
		total_gold += (unsigned) GET_INN_GOLD(ch);
	}

	if (cost->total_cost > total_gold) {
		if (GET_LEVEL(ch) > IMO_SPIRIT) {
			if (receptionist == ch)
				send_to_char("You don't have enough money, but since you're a God, I guess its Okay\r\n", ch);
			else
				act("$n tells you 'You don't have enough money, but since you're a God, I guess it's okay'",
				    FALSE, receptionist, 0, ch, TO_VICT);
			cost->total_cost = 0;
		}
		else {
			if (GET_LEVEL(ch) <= gv_highest_login_level) {
				if (GET_GOLD(ch) < 2) {
					GET_GOLD(ch) = 2;
				}
				cost->total_cost = GET_GOLD(ch) - 1;	/* LEAVE EM 1 COIN */
			}
			else {
				/* DO WE HAVE ENOUGH IN THE BANK */
				unsigned int bankgold;

				if (GET_BANK_GOLD(ch) >= 0) {
					bankgold = (unsigned) GET_BANK_GOLD(ch);
				}
				else {
					bankgold = 0;
				}


				if (cost->total_cost > total_gold + bankgold) {
					if (receptionist == ch)
						send_to_char("Which you can't afford\r\n", ch);
					else
						act("$n tells you 'Which I see you can't afford'",
						    FALSE, receptionist, 0, ch, TO_VICT);
				}
				else {
					if (receptionist == ch)
						send_to_char("If you withdraw money from the bank, you can afford it\r\n", ch);
					else {
						act("$n tells you 'If you withdraw some of your money from the Bank,'",
						    FALSE, receptionist, 0, ch, TO_VICT);
						act("                       'you can afford it'",
						    FALSE, receptionist, 0, ch, TO_VICT);
					}
				}
			}
		}
	}			/* END OF NOT ENOUGH GOLD */

	if (cost->total_cost > total_gold)
		return (FALSE);

	return (TRUE);

}				/* END OF r2100_recep_offer() */


/* *******************************************************************
 * Routines used to load a characters equipment from disk            *
 * ***************************************************************** */
void r2200_obj_store_to_char(struct char_data * ch, struct obj_data last_obj[OBJ_ARRAY], struct obj_file_elem * st)
{

	struct obj_data *obj;
	int idx, IsNoRent, obj_real_number, keyword;




	if (st->item_number < 0) {
		return;
	}

	obj_real_number = db8200_real_object(st->item_number);
	if (obj_real_number < 0) {
		return;
	}

	obj = db5100_read_object(st->item_number, VIRTUAL);
	/* SUBTRACT THIS AS WE ADDED IT WHEN COMPUTING MAXED ITEMS */
	if (gv_port == PRODUCTION_PORT) {
		if (obj_index[obj_real_number].number > 0)
			obj_index[obj_real_number].number--;
	}

	/* Fix to make it easier to remove objects */
	IsNoRent = IS_OBJ_STAT(obj, OBJ1_NORENT) ? OBJ1_NORENT : 0;

	if (strcmp(st->name, "Null")) {
		free(obj->name);
		obj->name = strdup(st->name);
	}
	if (strcmp(st->description, "Null")) {
		free(obj->description);
		obj->description = strdup(st->description);
	}
	if (strcmp(st->short_description, "Null")) {
		free(obj->short_description);
		obj->short_description = strdup(st->short_description);
	}
	if (strcmp(st->action_description, "Null")) {
		free(obj->action_description);
		obj->action_description = strdup(st->action_description);
	}

	obj->obj_flags.value[0] = st->value[0];
	obj->obj_flags.value[1] = st->value[1];
	obj->obj_flags.value[2] = st->value[2];
	obj->obj_flags.value[3] = st->value[3];

	obj->obj_flags.flags1 = st->flags1 | IsNoRent;
	obj->obj_flags.flags2 = st->flags2;

	obj->obj_flags.timer = st->timer;
	obj->obj_flags.bitvector = st->bitvector;

	for (idx = 0; idx < MAX_OBJ_AFFECT; idx++)
		obj->affected[idx] = st->affected[idx];


	ha1700_obj_to_char(obj, ch);

	if (st->item_state < MAX_WEAR) {
		keyword = st->item_state + 1000;
		if (st->item_state > 1)
			keyword--;	/* ADJUST FINGER */
		if (st->item_state > 3)
			keyword--;	/* ADJUST NECK */
		if (st->item_state > 10)
			keyword--;	/* ADJUST SHIELD GAP */
		if (st->item_state > 14)
			keyword--;	/* ADJUST WRIST */
		if (st->item_state == 11)
			keyword = 1014;	/* SHIELD IS AT THE END */

		keyword = st->item_state;
		oj5000_wear_obj(ch, obj, keyword, FALSE);
	}

	/* IF WE HAVE AN ITEM NOT WORN, SHOULD IT BE IN SOMETHING? */
	if (st->item_state > MAX_WEAR &&
	    last_obj[st->item_state].in_obj) {
		/*
		 printf("    Item %s should be in %s.\r\n",
						marker1                obj->name, last_obj[st->item_state].in_obj->name);
		 */
		/* PUT ITEM IN BAG */
		ha1800_obj_from_char(obj);
		IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(obj);
		ha2300_obj_to_obj(obj, last_obj[st->item_state].in_obj);
	}

	if (GET_ITEM_TYPE(obj) == ITEM_CONTAINER ||
	    GET_ITEM_TYPE(obj) == ITEM_QSTCONT)
		last_obj[st->item_state + 1].in_obj = obj;
	else
		last_obj[st->item_state + 1].in_obj = 0;

	return;

}				/* END OF r2200_obj_store_to_char() */

void r2300_del_char_objs(struct char_data * ch)
{



	if (IS_PC(ch) && !gv_terminate) {
		r1100_rent_delete_file(GET_NAME(ch));
	}

}				/* END OF r2300_del_char_objs() */


void r2400_load_char_objs(struct char_data * ch, int lv_backup)
{
	FILE *fl;
	char buf[MAX_STRING_LENGTH], ha1100_fname[MAX_STRING_LENGTH];
	int idx, lv_rent_cost, lv_months, lv_days, lv_hours, lv_minutes;
	unsigned long lv_seconds_gone;
	struct affected_type *af, *next_af;
	struct obj_data last_obj[OBJ_ARRAY];
	struct obj_file_u theHeader;
	struct obj_file_elem st;/* stored item var */



	if (!r1000_rent_get_filename(GET_NAME(ch), ha1100_fname))
		return;		/* char has no name ?!? */
	if (lv_backup)
		strcat(ha1100_fname, ".sav");
	if (!(fl = fopen(ha1100_fname, "r+b"))) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf, "ERROR: READING OBJECT FILE %s (5)", ha1100_fname);
			perror(buf);
			send_to_char("\n\r********************* NOTICE *********************\n\r"
				     "There was a problem loading your objects from disk.\n\r"
				     "Contact a God for assistance.\n\r", ch);
		}
		sprintf(buf, "   %s entering game with no equipment.", GET_NAME(ch));
		main_log(buf);
		return;
	}
	sprintf(buf, "   %s entering game. (recovered equip ok)", GET_NAME(ch));
	main_log(buf);

	fread(&theHeader, sizeof(struct obj_file_u), 1, fl);
	if (str_cmp(theHeader.owner, GET_NAME(ch))) {
		sprintf(buf, "ERROR: %s's rent file says was saved for char %s.",
			GET_NAME(ch), theHeader.owner);
	}
	for (idx = 0; idx < 100; idx++) {
		last_obj[idx].in_obj = 0;
	}

	while (!feof(fl)) {
		fread(&st, sizeof(struct obj_file_elem), 1, fl);
		if (!feof(fl))
			r2200_obj_store_to_char(ch, last_obj, &st);
	}

	/* close up the file now that we're done */
	fclose(fl);

	/* calculate time away */
	if (GET_LOGOFF_TIME(ch) > 0) {
		lv_seconds_gone = time(0) - GET_LOGOFF_TIME(ch);
	}
	else {
		lv_seconds_gone = 0;
	}

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

	/* SECONDS (casting) */
	int lv_int_seconds_gone = (int) lv_seconds_gone;

	bzero(buf, sizeof(buf));
	sprintf(buf, "You were gone %d months, %d days, %d hours, %d minutes, %d seconds\r\n",
		lv_months, lv_days, lv_hours, lv_minutes, lv_int_seconds_gone);
	send_to_char(buf, ch);

	/* Calculate the chars rent, max one days worth of rent */
	/* 5 to 10 hours is half price,  under 5 is free.       */
	if (lv_months > 0 ||
	    lv_days > 0 ||
	    lv_hours > 10)
		lv_rent_cost = (int) theHeader.total_cost;
	else if (lv_hours > 5)
		lv_rent_cost = (int) (theHeader.total_cost / 2);
	else
		lv_rent_cost = 0;

	if (lv_rent_cost < 1)
		send_to_char("No storage fee is being charged.\r\n", ch);
	else {
		bzero(buf, sizeof(buf));
		sprintf(buf, "Storage cost you %d coins.\r\n", lv_rent_cost);
		send_to_char(buf, ch);
		if (GET_GOLD(ch) > lv_rent_cost)
			GET_GOLD(ch) = (GET_GOLD(ch) - lv_rent_cost);
		else {
			send_to_char("You inn account has been debited\r\n", ch);
			lv_rent_cost = lv_rent_cost - GET_GOLD(ch);
			GET_GOLD(ch) = 0;
			if (GET_INN_GOLD(ch) < lv_rent_cost) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: Player %s entering game without sufficient storage money\r\n", GET_NAME(ch));
				do_sys(buf, IMO_IMP, ch);
				spec_log(buf, ERROR_LOG);
				GET_INN_GOLD(ch) = 0;
			}
			else {
				GET_INN_GOLD(ch) = GET_INN_GOLD(ch) - lv_rent_cost;
			}
		}
	}

	if (GET_GOLD(ch) < 0)
		GET_GOLD(ch) = 0;
	if (GET_BANK_GOLD(ch) < 0)
		GET_BANK_GOLD(ch) = 0;
	if (GET_INN_GOLD(ch) < 0)
		GET_INN_GOLD(ch) = 0;

	sprintf(buf, "You now have %d coins (INN GOLD: %d).\r\n",
		GET_GOLD(ch), GET_INN_GOLD(ch));
	send_to_char(buf, ch);

	/* Save char, to avoid strange data if crashing */
	db6400_save_char(ch, NOWHERE);

	/* Restore original AC by reapplying all AC affects */
	for (af = ch->affected; af; af = next_af) {
		next_af = af->next;
		if ((af->location == APPLY_AC) &&
		    (af->duration != -1))
			GET_AC(ch) += af->modifier;
	}

}				/* END OF r2400_load_char_objs() */


/* ********************************************************************
 * Routines used to save a characters equipment to disk               *
  ******************************************************************* */

/* Puts object in store */
void r3000_put_obj_in_store(struct obj_data * obj, struct obj_file_elem * st, int lv_state)
{
	int j;


	if ((obj)->name)
		strncpy(st->name, (obj)->name, sizeof(st->name));
	else
		strncpy(st->name, "Null", sizeof(st->name));
	if ((obj)->description)
		strncpy(st->description, (obj)->description, sizeof(st->description));
	else
		strncpy(st->description, "Null", sizeof(st->description));
	if ((obj)->short_description)
		strncpy(st->short_description, (obj)->short_description, sizeof(st->short_description));
	else
		strncpy(st->short_description, "Null", sizeof(st->short_description));
	if ((obj)->action_description)
		strncpy(st->action_description, (obj)->action_description, sizeof(st->action_description));
	else
		strncpy(st->action_description, "Null", sizeof(st->action_description));
	st->item_number = obj_index[obj->item_number].virtual;
	st->item_state = lv_state;
	st->value[0] = obj->obj_flags.value[0];
	st->value[1] = obj->obj_flags.value[1];
	st->value[2] = obj->obj_flags.value[2];
	st->value[3] = obj->obj_flags.value[3];

	st->flags1 = obj->obj_flags.flags1;
	st->flags2 = obj->obj_flags.flags2;
	st->timer = obj->obj_flags.timer;
	st->bitvector = obj->obj_flags.bitvector;

	for (j = 0; j < MAX_OBJ_AFFECT; j++)
		st->affected[j] = obj->affected[j];

}				/* END OF r3000_put_obj_in_store() */
/* Destroy inventory after transferring it to store */
void r3100_obj_to_store(FILE * fl, struct obj_data * obj, struct obj_file_elem * st, struct char_data * ch, int *lv_state)
{

	static char buf[MAX_STRING_LENGTH];
	struct obj_data *tmp_obj;
	int lv_junk_item;

	/* IF NULL OBJECT, WE DON'T NEED TO DO ANYTHING */
	if (!obj) {
		return;
	}

	/* ADJUST WEIGHT IF THERE IS A CONTAINER, AND THE CONTAINER */
	/* THAT CONTAINER IS IN IF ITS IN A CONTAINER, etc          */
	for (tmp_obj = obj->in_obj; tmp_obj; tmp_obj = tmp_obj->in_obj)
		GET_OBJ_WEIGHT(tmp_obj) -= GET_OBJ_WEIGHT(obj);

	lv_junk_item = FALSE;

	if (obj->item_number < 0) {
		lv_junk_item = TRUE;
	}

	if (obj->obj_flags.cost_per_day < 1) {
		lv_junk_item = TRUE;
	}

	/* NORENT AND USER IS FORCED OFF? */
	if ((IS_OBJ_STAT(obj, OBJ1_NORENT))) {
		lv_junk_item = TRUE;
	}

	/* IF ITS JUNK, WE DON'T NEED IT */
	if ((GET_TIMER(obj) < 1) && (TIMER(obj) != OBJ_NOTIMER)) {
		lv_junk_item = TRUE;
	}

	if (lv_junk_item == TRUE) {
		/* TELL THEM ABOUT IT? */
		if (GET_HOW_LEFT(ch) == LEFT_BY_RENT_WITH_ITEMS) {
			sprintf(buf, "You're told: 'The %s is just old junk, I'll throw it away for you.'\r\n",
				ha1100_fname(obj->name));
			send_to_char(buf, ch);
			sprintf(buf, "%s is told that the %s is old junk and will be thrown away.",
				GET_REAL_NAME(ch), ha1100_fname(obj->name));

			act(buf, TRUE, ch, 0, ch, TO_ROOM);
		}
		else {
			bzero(buf, sizeof(buf));
			sprintf(buf, "%s is told that the %s is old junk and will be thrown away.",
				GET_REAL_NAME(ch), ha1100_fname(obj->name));
			/* do_sys(buf, IMO_IMP, ch); */
		}
	}
	else {			/* marker3 */
		r3000_put_obj_in_store(obj, st, *lv_state);

		/* WRITE ST TO FILE HERE */
		if (fwrite(st, sizeof(struct obj_file_elem), 1, fl) < 1) {
			sprintf(buf, "ERROR1: Creating rent file for %s.",
				GET_NAME(ch));
			main_log(buf);
			spec_log(buf, ERROR_LOG);
			spec_log(buf, EQUIPMENT_LOG);

			return;
		}
	}			/* END OF else */

	if (obj->contains) {
		if (*lv_state < MAX_WEAR)
			*lv_state = MAX_WEAR + 1;
		else
			*lv_state = *lv_state + 1;
		r3100_obj_to_store(fl, obj->contains, st, ch, lv_state);
		*lv_state = *lv_state - 1;
	}

	r3100_obj_to_store(fl, obj->next_content, st, ch, lv_state);

	return;

}				/* END OF r3100_obj_to_store() */


void r3200_obj_restore_weight(struct obj_data * obj)
{



	if (obj) {
		r3200_obj_restore_weight(obj->contains);
		r3200_obj_restore_weight(obj->next_content);
		if (obj->in_obj)
			GET_OBJ_WEIGHT(obj->in_obj) += GET_OBJ_WEIGHT(obj);
	}
}				/* END OF r3200_obj_restore_weight() */


void r3300_extract_all_objs(struct obj_data * obj)
{



	if (obj) {
		r3300_extract_all_objs(obj->contains);
		r3300_extract_all_objs(obj->next_content);

		/* IF LEAVING VIA RENT AND THIS IS THE PRODUCTION PORT    */
		/* WE DON'T WANT TO ADJUST OBJECT QUANTITY BECAUSE WE     */
		/* COMPUTED IT WHEN WE COUNTED MAXED ITEMS.  THEREFORE,   */
		/* ADD BACK THE VALUE SUBTRACTED FROM THE EXTRACT ROUTINE */
		if (gv_port == PRODUCTION_PORT) {
			if ((obj->obj_flags.cost_per_day > 0) &&
			    !(IS_OBJ_STAT(obj, OBJ1_NORENT)) &&
				obj->item_number > -1) {
				obj_index[obj->item_number].number++;
			}
		}		/* END OF not production port */

		ha2700_extract_obj(obj);
	}

	return;

}				/* END OF r3300_extract_all_objs() */


void r3400_del_objs(struct char_data * ch)
{
	int i;



	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i]) {
			ha1700_obj_to_char(ha1930_unequip_char(ch, i), ch);
		}
	}
	if (ch->carrying) {
		r3300_extract_all_objs(ch->carrying);
		ch->carrying = 0;
	}

	return;

}				/* END OF r3400_del_objs() */


/*************************************************************
 WRITE THE OBJ DATA FILE OUT, SAVE EVERYTHING NO QUESTIONS ASKED
 *************************************************************/

/* write the vital data of a player to the player file */
void r4000_save_obj(struct char_data * ch, struct obj_cost * cost)
{
	struct obj_file_u theHeader;
	struct obj_file_elem st;/* stored item var */
	FILE *fl;
	int i, lv_state;
	static char buf[512];



	bzero((char *) &st, sizeof(struct obj_file_elem));

	for (i = 0; i < 20; i++)
		theHeader.owner[i] = 0;
	strcpy(theHeader.owner, GET_NAME(ch));
	theHeader.total_cost = cost->total_cost;
	theHeader.last_update = time(0);

	if (IS_NPC(ch))		/* cheap insurance cant happen but cant hurt */
		return;

	r2300_del_char_objs(ch);/* turf old file if any */

	if (!r1000_rent_get_filename(GET_NAME(ch), buf))
		return;

	/* open the file */
	if (!(fl = fopen(buf, "wb")))
		return;

	/* Write the header out */
	if (fwrite(&theHeader, sizeof(struct obj_file_u), 1, fl) < 1) {
		sprintf(buf, "ERROR2: Creating rent file for %s.", GET_NAME(ch));
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		spec_log(buf, EQUIPMENT_LOG);
		return;
	}

	/* write out all the equipment */

	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i]) {
			if (GET_ITEM_TYPE(ch->equipment[i]) == ITEM_CONTAINER ||
			    GET_ITEM_TYPE(ch->equipment[i]) == ITEM_QSTCONT)
				lv_state = MAX_WEAR;
			else
				lv_state = i;
			r3100_obj_to_store(fl, ch->equipment[i], &st, ch, &lv_state);
			r3200_obj_restore_weight(ch->equipment[i]);
		}
	}

	lv_state = MAX_WEAR;
	r3100_obj_to_store(fl, ch->carrying, &st, ch, &lv_state);
	/* THE ABOVE ROUTINE SUBTRACTS WEIGHTS, PUT THEM BACK */
	r3200_obj_restore_weight(ch->carrying);

	fclose(fl);		/* close up shop */

	return;

}				/* END OF r4000_save_obj() */

int r4100_backup_obj(struct char_data * ch)
{
	//, struct obj_cost *cost
	struct obj_file_u theHeader;
	struct obj_file_elem st;/* stored item var */
	FILE *fl;
	int i, lv_state;
	static char buf[512];
	char error_buf[MAX_STRING_LENGTH];



	bzero((char *) &st, sizeof(struct obj_file_elem));

	for (i = 0; i < 20; i++)
		theHeader.owner[i] = 0;
	strcpy(theHeader.owner, GET_NAME(ch));
	theHeader.total_cost = 0;
	theHeader.last_update = time(0);

	if (IS_NPC(ch))		/* cheap insurance cant happen but cant hurt */
		return 0;

	if (!r1050_backup_get_filename(GET_NAME(ch), buf)) {
		sprintf(error_buf, "Coudn't get backup file name for %s.", GET_NAME(ch));
		main_log(error_buf);
		spec_log(error_buf, ERROR_LOG);
		spec_log(error_buf, EQUIPMENT_LOG);
		return 0;
	}

	if (!r1125_backup_delete_file(GET_NAME(ch))) {
		sprintf(error_buf, "Couldn't delete backup file for %s.", GET_NAME(ch));
		main_log(error_buf);
		spec_log(error_buf, ERROR_LOG);
		spec_log(error_buf, EQUIPMENT_LOG);
		return 0;	/* turf old file if any */
	}

	/* open the file */
	if (!(fl = fopen(buf, "wb")))
		return 0;

	/* Write the header out */
	if (fwrite(&theHeader, sizeof(struct obj_file_u), 1, fl) < 1) {
		sprintf(buf, "ERROR2: Creating backup file for %s.", GET_NAME(ch));
		main_log(buf);
		spec_log(buf, ERROR_LOG);
		spec_log(buf, EQUIPMENT_LOG);
		return 0;
	}

	/* write out all the equipment */

	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i]) {
			if (GET_ITEM_TYPE(ch->equipment[i]) == ITEM_CONTAINER ||
			    GET_ITEM_TYPE(ch->equipment[i]) == ITEM_QSTCONT)
				lv_state = MAX_WEAR;
			else
				lv_state = i;
			r3100_obj_to_store(fl, ch->equipment[i], &st, ch, &lv_state);
			r3200_obj_restore_weight(ch->equipment[i]);
		}
	}

	lv_state = MAX_WEAR;
	r3100_obj_to_store(fl, ch->carrying, &st, ch, &lv_state);
	/* THE ABOVE ROUTINE SUBTRACTS WEIGHTS, PUT THEM BACK */
	r3200_obj_restore_weight(ch->carrying);

	fclose(fl);		/* close up shop */

	return 1;

}				/* END OF r4100_back_obj() */

int r4150_restore_backup(char *character_name)
{
	char objs_file[MAX_STRING_LENGTH], backup_file[MAX_STRING_LENGTH];

	/* First, remove the old .objs file */
	r1100_rent_delete_file(character_name);

	/* Now get the file paths right */
	if (r1000_rent_get_filename(character_name, objs_file) == 0)
		return 0;

	if (r1050_backup_get_filename(character_name, backup_file) == 0)
		return 0;

	/* Now copy the backup file to the .objs file spot */
	if (db20000_copy_file(backup_file, objs_file) == 0)
		return 0;

	return 1;
}


/* ************************************************************************
* Routines used to update object file, upon boot time                     *
************************************************************************* */

/* look for items that are over the maximum */
void r5000_compute_maxed_items(int lv_direction)
{

	int lv_found_it, size_of_header, size_of_element, rc, idx, jdx;

#define MAXARY 32767
	int ary[MAXARY];

	FILE *file_in;
	struct obj_file_u eqp_head;
	struct obj_file_elem eqp_elem;
	char filename[256], buf[2048];
	char *ptr;



	/* ONLY CHECK MAXED ITEMS ON PRODUCTION PORT */
	if (gv_port != PRODUCTION_PORT) {
		main_log("  Not checking maxed items on this port.");
		return;
	}

	file_in = fopen("eqp_usage_file", "r+");
	if (file_in) {
		main_log("Using temporary usage file.");
		while (!feof(file_in)) {
			bzero(buf, sizeof(buf));
			fgets(buf, 80, file_in);
			rc = atoi(buf);

			bzero(buf, sizeof(buf));
			fgets(buf, 80, file_in);
			idx = atoi(buf);

			for (jdx = 0; jdx < top_of_objt; jdx++) {
				if (rc == obj_index[jdx].virtual) {
					obj_index[jdx].number = idx;
				}
			}
		}
		/* PURGE MAX FILE SO WE WON'T USE IT AGAIN */
		unlink("eqp_usage_file");
		return;
	}

	for (idx = 0; idx < MAXARY; idx++) {
		ary[idx] = 0;
	}

	main_log("Reading individual rent files.");

	size_of_header = sizeof(struct obj_file_u);
	size_of_element = sizeof(struct obj_file_elem);

	for (idx = 0; idx < top_of_p_table; idx++) {
		bzero(buf, sizeof(buf));
		strcpy(buf, (char *) (player_table + idx)->pidx_name);
		if (*buf != '1' && *buf != '2') {
			rc = r1000_rent_get_filename(buf, filename);
			file_in = fopen(filename, "rb+");
			if (!file_in)
				continue;
			fread(&eqp_head, size_of_header, 1, file_in);
			if (feof(file_in)) {
				fclose(file_in);
				continue;
			}
			ptr = (char *) &eqp_elem;
			bzero(ptr, sizeof(eqp_elem));
			fread(&eqp_elem, size_of_element, 1, file_in);
			while (!feof(file_in)) {
				lv_found_it = FALSE;
				for (jdx = 0; jdx < top_of_objt; jdx++) {
					if (eqp_elem.item_number == obj_index[jdx].virtual) {
						ary[eqp_elem.item_number]++;
						obj_index[jdx].number++;
						lv_found_it = TRUE;
					}
				}

				if (lv_found_it == FALSE) {
					sprintf(gv_str, "%s has invalid eqp: %d.",
						buf, eqp_elem.item_number);
					main_log(gv_str);
					spec_log(gv_str, ERROR_LOG);
				}
				fread(&eqp_elem, size_of_element, 1, file_in);
			}
			fclose(file_in);
		}
	}

	/* IF WE ARE CREATING THE RENT FILE, DO SO AND REBOOT */
	if (IS_SET(gv_run_flag, RUN_FLAG_CREATE_MAX_FILE)) {
		main_log("Creating file of item usage.");
		file_in = fopen("eqp_usage_file", "w+");
		for (idx = 0; idx < MAXARY; idx++) {
			if (ary[idx] > 0) {
				fprintf(file_in, "%d\n%d\n", idx, ary[idx]);
			}
		}
		fclose(file_in);
		main_log("ABORTING so we'll use rent file.\r\n");
		exit(1);
	}
	return;

}				/* END OF actual_max_check() */


void r6000_do_offer(struct char_data * ch, char *arg, int cmd)
{

	struct obj_cost cost;
	r2100_recep_offer(ch, ch, &cost, RECEPTION_VERBOSE);
	return;

}				/* END OF r6000_do_offer() */


/* *********************************************************** *
* Routine Receptionist                                         *
************************************************************** */
//2 ^ (32 - 1) - 147483648 = 2000000000

int r7000_func_receptionist(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	//int max_deposit;
	struct obj_cost cost;
	struct char_data *recep = 0;
	struct char_data *temp_char;
	sh_int action_tabel[9] = {
		CMD_SMILE,
		CMD_RROSE,
		CMD_SIGH,
		CMD_PUKE,
		CMD_WIGGLE,
		CMD_SHAKE,
		CMD_WOO,
		CMD_WHISTLE,
		CMD_YAWN
	};
	int amount /* , tax */ ;
	int number(int from, int to);



	if (!ch->desc)
		return (FALSE);	/* You've forgot FALSE - NPC couldn't leave */

	for (temp_char = world[ch->in_room].people; (temp_char) && (!recep);
	     temp_char = temp_char->next_in_room)
		if (IS_MOB(temp_char))
			if (mob_index[temp_char->nr].func == r7000_func_receptionist)
				recep = temp_char;

	if (!recep) {
		send_to_char("No receptionist in room!\r\n", ch);
		main_log("ERROR: NO RECEPTIONIST!!");
		spec_log("ERROR: NO RECEPTIONIST!!", ERROR_LOG);
		return (FALSE);
	}


	if (IS_NPC(ch))
		return (FALSE);

	if ((GET_LEVEL(ch) < IMO_IMM) &&
	    ((cmd == CMD_KILL) ||
	     (cmd == CMD_HIT) ||
	     (cmd == CMD_BACKSTAB) ||
	     (cmd == CMD_BASH) ||
	     (cmd == CMD_KICK) ||
		 (cmd == CMD_FLURRY) ||
	     (cmd == CMD_BREATH))) {
		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
		if (cmd == CMD_BREATH &&
		    is_abbrev(buf, "gas")) {
			send_to_char("\r\nYou keep that up and you won't rent here!\r\n", ch);
			return (TRUE);
		}
		if (cmd == CMD_BREATH) {	/* STRIP OUT TYPE */
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
		}
		if (!(*buf)) {
			return (FALSE);
		}
		if (recep == ha2100_get_char_in_room(buf, ch->in_room)) {
			send_to_char("\r\nYou keep that up and you won't rent here!\r\n", ch);
			return (TRUE);
		}
		else {
			/* LET THE FIGHT CODE HANDLE THIS */
			return (FALSE);
		}
	}

	if ((cmd != CMD_LIST) &&
	    //***(cmd != CMD_RENT) &&
	    (cmd != CMD_OFFER) &&	/* replaced command with global one */
	    (cmd != CMD_DEPOSIT) &&
	    (cmd != CMD_BALANCE) &&
	    (cmd != CMD_WITHDRAW)) {
		if (!number(0, 30))	/* DO ONCE IN 30 */
			do_action(recep, "", action_tabel[number(0, 8)]);
		return (FALSE);
	}

	if (!AWAKE(recep)) {
		act("$e isn't able to talk to you...", FALSE, recep, 0, ch, TO_VICT);
		return (TRUE);
	}

	if (!CAN_SEE(recep, ch) && GET_LEVEL(ch) < IMO_LEV) {
		act("$n says, 'I don't deal with people I can't see!'", FALSE, recep, 0, 0, TO_ROOM);
		return (TRUE);
	}

	if (cmd == CMD_LIST) {
		send_to_char("You can enter: RENT, OFFER, BALANCE, WITHDRAW, DEPOSIT\r\n", ch);
		return (TRUE);
	}
	if (cmd == CMD_RENT) {	/* Rent  */
		send_to_char("Rent has been removed, please use quit.\r\n", ch);
		return (TRUE);
	}


	if (cmd == CMD_OFFER) {	/* Offer */
		r2100_recep_offer(ch, recep, &cost, RECEPTION_VERBOSE);
		act("$N gives $n an offer.", FALSE, ch, 0, recep, TO_ROOM);
		return (TRUE);
	}


	if (cmd == CMD_WITHDRAW) {
		arg = one_argument(arg, buf);
		if (!(is_number(buf))) {
			send_to_char("Tell me how much money you want to withdraw.\r\n", ch);
			return (TRUE);
		}
		amount = atoi(buf);
		if (amount < 0) {
			send_to_char("Ha ha ha... nice try\n\r", ch);
			return (TRUE);
		}

		if (amount > GET_INN_GOLD(ch)) {
			send_to_char("I'm afraid you don't have that much in your account.\n\r", ch);
			return (TRUE);
		}

		if ((MAX_GOLD - GET_GOLD(ch) - amount) < 0) {
			act("$n tells you you can't carry that much gold, but she'll give you as much as you can carry.",
			    FALSE, recep, 0, ch, TO_VICT);
			act("$n hands you your money.", FALSE, recep, 0, ch, TO_VICT);
			act("$n gives $N some money, but $N looks a tad disappointed.", FALSE, recep, 0, ch, TO_NOTVICT);
			amount = MAX_GOLD - GET_GOLD(ch);
			GET_GOLD(ch) += amount;
			GET_INN_GOLD(ch) -= amount;

			return (TRUE);
		}

		act("$n hands you your money.", FALSE, recep, 0, ch, TO_VICT);
		act("$n gives $N some money.", FALSE, recep, 0, ch, TO_NOTVICT);
		ch->points.gold += amount;
		ch->points.inn_gold -= amount;
		return (TRUE);

	}			/* END OF WITHDRAW */

	if (cmd == CMD_DEPOSIT) {
		arg = one_argument(arg, buf);
		if (!(is_number(buf))) {
			send_to_char("Tell me how much money you want to deposit.\r\n", ch);
			return (TRUE);
		}

		amount = atoi(buf);
		/* tax = amount / 20; */
		if (amount < 0) {
			send_to_char("Ha ha ha... nice try\n\r", ch);
			return (TRUE);
		}

		if (amount > ch->points.gold) {
			send_to_char("I'm afraid you don't have that much.\n\r", ch);
			return (TRUE);
		}

		/* CHECK DEPOSIT LIMITS */
		/* remember to check GET_INN_GOLD not GET_BANK_GOLD */

		if ((amount /*-tax*/ ) > MAX_BALANCE ||
		    (amount) > MAX_DEPOSIT ||
		    GET_INN_GOLD(ch) + (amount /*-tax*/ ) > MAX_BALANCE
			) {
			send_to_char("I'm afraid considering the transaction tax that would exceed our maximum allowable balance\r\n", ch);
			bzero(buf, sizeof(buf));
			sprintf(buf, "The maximum amout is currently set to %d million\r\n", MAX_DEPOSIT);
			send_to_char(buf, ch);
			return (TRUE);
		}

		/* sprintf(buf, "I'm afraid the tax on this transaction comes
		 * to %d.\n\r", tax); send_to_char(buf, ch); */
		act("$n takes your money.", FALSE, recep, 0, ch, TO_VICT);
		act("$n takes some money from $N.", FALSE, recep, 0, ch, TO_NOTVICT);
		ch->points.gold -= amount;
		/* amount -= tax; */
		ch->points.inn_gold += amount;
	}			/* END OF DEPOSIT */


	if (cmd == CMD_BALANCE) {

		sprintf(buf, "You have %d coins on account.\n\r", GET_INN_GOLD(ch));
		send_to_char(buf, ch);
		return (TRUE);

	}			/* END OF BALANCE */

	return (TRUE);

}				/* END OF r7000_func_receptionist() */
/* *********************************************************** *
* Routine Banker                                               *
************************************************************** */

int r7200_func_banker(struct char_data * ch, int cmd, char *arg)
{

	char buf[MAX_STRING_LENGTH];
	//int max_deposit;
	struct char_data *char_banker = 0;
	struct char_data *temp_char;
	sh_int which_action;
	sh_int action_tabel[9] = {
		CMD_SMILE,
		CMD_DANCE,
		CMD_NOD,
		CMD_SIGH,
		CMD_STAGGER,
		CMD_COUGH,
		CMD_WOO,
		CMD_YAWN,
		CMD_YOYO
	};
	int amount;
	int number(int from, int to);



	if (!ch->desc)
		return (FALSE);	/* You've forgot FALSE - NPC couldn't leave */

	for (temp_char = world[ch->in_room].people;
	     (temp_char) && (!char_banker);
	     temp_char = temp_char->next_in_room) {
		if (IS_MOB(temp_char))
			if (mob_index[temp_char->nr].func == r7200_func_banker)
				char_banker = temp_char;
	}

	if (!char_banker) {
		send_to_char("no banker in room.\r\n", ch);
		main_log("ERROR: NO BANKER!!");
		spec_log("ERROR: NO BANKER!!", ERROR_LOG);
		return (FALSE);
	}

	if (IS_NPC(ch))
		return (FALSE);

	if ((GET_LEVEL(ch) < IMO_IMM) &&
	    ((cmd == CMD_KILL) ||
	     (cmd == CMD_HIT) ||
	     (cmd == CMD_BACKSTAB) ||
	     (cmd == CMD_BASH) ||
	     (cmd == CMD_KICK) ||
		 (cmd == CMD_FLURRY) ||
	     (cmd == CMD_BREATH))) {
		bzero(buf, sizeof(buf));
		arg = one_argument(arg, buf);
		if (cmd == CMD_BREATH &&
		    is_abbrev(buf, "gas")) {
			send_to_char("\r\nYou keep that up and you won't bank here!\r\n", ch);
			return (TRUE);
		}
		if (cmd == CMD_BREATH) {	/* STRIP OUT TYPE */
			bzero(buf, sizeof(buf));
			arg = one_argument(arg, buf);
		}
		if (!(*buf)) {
			return (FALSE);
		}
		if (char_banker == ha2100_get_char_in_room(buf, ch->in_room)) {
			send_to_char("\r\nYou keep that up and you won't bank here!\r\n", ch);
			return (TRUE);
		}
		else {
			/* LET THE FIGHT CODE HANDLE THIS */
			return (FALSE);
		}
	}
	if ((cmd != CMD_LIST) &&
	    (cmd != CMD_DEPOSIT) &&
	    (cmd != CMD_BALANCE) &&
	    (cmd != CMD_WITHDRAW)) {
		if (!number(0, 30)) {
			which_action = number(0, 8);
			do_action(char_banker, "", action_tabel[which_action]);
			if (which_action == CMD_STAGGER) {
				do_action(char_banker, "", CMD_BLUSH);
			}
		}
		return (FALSE);
	}

	if (!AWAKE(char_banker)) {
		act("$e isn't able to talk to you...", FALSE, char_banker, 0, ch, TO_VICT);
		return (TRUE);
	}

	if (!CAN_SEE(char_banker, ch) && GET_LEVEL(ch) < IMO_LEV) {
		act("$n says, 'I don't deal with people I can't see!'",
		    FALSE, char_banker, 0, 0, TO_ROOM);
		act("$n says, 'GUARDS! Watch this person. They are probably up to no good!'", FALSE, char_banker, 0, 0, TO_ROOM);
		return (TRUE);
	}

	if (cmd == CMD_LIST) {
		send_to_char("You can enter: BALANCE, WITHDRAW, DEPOSIT\r\n", ch);
		return (TRUE);
	}

	if (cmd == CMD_WITHDRAW) {
		arg = one_argument(arg, buf);
		if (!(is_number(buf))) {
			send_to_char("Tell me how much money you want to withdraw.\r\n", ch);
			return (TRUE);
		}
		amount = atoi(buf);
		if (amount < 0) {
			send_to_char("Ha ha ha... nice try\n\r", ch);
			return (TRUE);
		}

		if (amount > GET_BANK_GOLD(ch)) {
			send_to_char("I'm afraid you don't have that much in your account.\n\r", ch);
			return (TRUE);
		}

		if ((MAX_GOLD - GET_GOLD(ch) - amount) < 0) {
			act("$n tells you you can't hold that much gold, but he'll give you as much as you can carry.",
			    FALSE, char_banker, 0, ch, TO_VICT);
			amount = MAX_GOLD - GET_GOLD(ch);
			act("$n hands you your money.", FALSE, char_banker, 0, ch, TO_VICT);
			act("$n gives $N some money, but $N looks a little disappointed", FALSE, char_banker, 0, ch, TO_NOTVICT);
			ch->points.gold += amount;
			ch->points.bank_gold -= amount;
			return (TRUE);
		}

		act("$n hands you your money.", FALSE, char_banker, 0, ch, TO_VICT);
		act("$n gives $N some money.", FALSE, char_banker, 0, ch, TO_NOTVICT);
		ch->points.gold += amount;
		ch->points.bank_gold -= amount;
		return (TRUE);

	}			/* END OF WITHDRAW */

	if (cmd == CMD_DEPOSIT) {
		arg = one_argument(arg, buf);
		if (!(is_number(buf))) {
			send_to_char("Tell me how much money you want to deposit.\r\n", ch);
			return (TRUE);
		}

		amount = atoi(buf);
		if (amount < 0) {
			send_to_char("Ha ha ha... nice try\n\r", ch);
			return (TRUE);
		}

		if (amount > ch->points.gold) {
			send_to_char("I'm afraid you don't have that much.\n\r", ch);
			return (TRUE);
		}

		/* CHECK DEPOSIT LIMITS */

		if (amount > MAX_BALANCE ||
		    amount > MAX_DEPOSIT ||
		    GET_BANK_GOLD(ch) + amount > MAX_BALANCE
			) {
			send_to_char("I'm afraid that would exceed our maximum allowable balance\r\n", ch);
			bzero(buf, sizeof(buf));
			sprintf(buf, "The maximum amout is currently set to %d million\r\n", MAX_DEPOSIT);
			send_to_char(buf, ch);
			return (TRUE);
		}

		act("$n takes your money.", FALSE, char_banker, 0, ch, TO_VICT);
		act("$n takes some money from $N.", FALSE, char_banker, 0, ch, TO_NOTVICT);
		ch->points.gold -= amount;
		ch->points.bank_gold += amount;
	}			/* END OF DEPOSIT */

	if (cmd == CMD_BALANCE) {
		sprintf(buf, "You have %d coins on account.\n\r", GET_BANK_GOLD(ch));
		send_to_char(buf, ch);
		return (TRUE);

	}			/* END OF BALANCE */

	return (TRUE);

}				/* END OF r7200_func_banker() */


void r7300_do_quit(struct char_data * ch, char *arg, int cmd)
{

	struct obj_cost cost;
	char buf[MAX_STRING_LENGTH];
	int save_room;
	int rc;

	cost.no_carried = 0;
	cost.total_cost = 0;
	cost.ok = TRUE;

	if (GET_LEVEL(ch) > gv_highest_login_level) {
		rc = r2100_recep_offer(ch, ch, &cost, 0);
	}
	else {
		rc = r2100_recep_offer(ch, ch, &cost, RECEPTION_IGNORE);
	}
	if (!rc) {
		send_to_char("Sorry, but you can't quit\n", ch);
		return;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "Losing player %s via quit.",
		GET_NAME(ch));
	do_connect(buf, GET_LEVEL(ch), ch);


	if (cost.no_carried == 0) {
		send_to_char("Saving character data - No rent items.\r\n", ch);
		GET_HOW_LEFT(ch) = LEFT_BY_RENT_NO_ITEMS;
		if (gv_mega_verbose_messages == TRUE) {
			main_log("Quitting withOUT items");
		}
	}
	else {
		send_to_char("Saving equipment and character data.\r\n", ch);
		GET_HOW_LEFT(ch) = LEFT_BY_RENT_WITH_ITEMS;
		if (gv_mega_verbose_messages == TRUE) {
			main_log("Quitting with items");
		}
	}

	act("$n has left the game.", FALSE, ch, 0, 0, TO_ROOM);
	r4000_save_obj(ch, &cost);
	r3400_del_objs(ch);
	save_room = ch->in_room;
	/* THIS WILL SAVE CHAR TO PLAYER FILE */
	ha3000_extract_char(ch, END_EXTRACT_BY_RENT);
	/* ABOVE EXTRACT DOESN'T FREE THE MEMORY */
	ch->in_room = save_room;
	/* THIS WILL SAVE THE CHAR AGAIN WITH A DIFFERENT ROOM */
	db6400_save_char(ch, world[ch->in_room].number);
	return;

} //END OF r7300_do_quit()
	void r8000_do_backup(struct char_data * ch, char *arg, int cmd)
{

	/* struct obj_cost cost;
	 * 
	 * cost.no_carried = 0; cost.total_cost = 0; cost.ok = TRUE; */

	char buf[MAX_STRING_LENGTH], restore[MAX_STRING_LENGTH], name[MAX_STRING_LENGTH], backup_file[MAX_STRING_LENGTH];
	struct char_data *victim = 0;
	int victim_player_no;
	int fl;

	if (IS_NPC(ch)) {
		send_to_char("Nope, sorry.\r\n", ch);
		return;
	}


	if ((*arg) && !(GET_LEVEL(ch) < IMO_IMP)) {
		half_chop(arg, restore, name);
		if (strcmp(restore, "restore")) {
			send_to_char("Usage: backup restore <character>.  NOTE: character must be offline.\r\n", ch);
			return;
		}

		if (!name) {
			send_to_char("Restore who's backup?\r\n", ch);
			return;
		}

		victim_player_no = pa2000_find_name(name);

		if (victim_player_no < 0 || victim_player_no > top_of_p_table) {
			send_to_char("Couldn't find that player.\r\n", ch);
			return;
		}

		victim = ha3100_get_char_vis(ch, name);
		if (victim) {
			if (!strcmp(GET_NAME(victim), name)) {
				sprintf(buf, "That person (%s) is online, it won't work.  Make them log off.\r\n", GET_NAME(victim));
				send_to_char(buf, ch);
				return;
			}
			else {
				send_to_char("Who?\r\n", ch);
				return;
			}
		}

		if (r1050_backup_get_filename(name, backup_file)) {
			if ((fl = open(backup_file, O_RDONLY)) == -1) {
				send_to_char("Can't open that players backup file, maybe it doesn't exist???\r\n", ch);
				return;

			}
		}

		if (r4150_restore_backup(name))
			send_to_char("Okay.\r\n", ch);
		else
			send_to_char("Something unexpected happened.\r\n", ch);
	}
	else {
		if (r4100_backup_obj(ch)) {
			sprintf(buf, "New backup file for %s.", GET_NAME(ch));
			main_log(buf);
			spec_log(buf, EQUIPMENT_LOG);
			send_to_char("Ok.\r\n", ch);
		}
		else
			send_to_char("Uh-oh, something weird happened...ask someone to help you.\r\n", ch);
	}

}
