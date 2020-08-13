/* sp */
/* gv_location: 17001-17500 */
/* ***************************************************************** *
*  file: spell_parser.c , Basic routines and parsing Part of DIKUMUD *
*  Usage : Interpreter of spells                                     *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "parser.h"
#include "handler.h"
#include "constants.h"
#include "spells.h"
#include "globals.h"
#include "func.h"

struct spell_info_type spell_info[MAX_SKILLS];

#define SPELLO(nr, beat, pos, mana, unused, flags, \
							 def_mod, def_loc, def_bit,tar, func) { \
	spell_info[nr].spell_pointer = (func);      \
		spell_info[nr].minimum_position = (pos);    \
			spell_info[nr].min_usesmana = (mana);       \
				spell_info[nr].beats = (beat*4);            \
					spell_info[nr].spell_flag = (flags);        \
						spell_info[nr].modifier = (def_mod);        \
							spell_info[nr].location = (def_loc);        \
								spell_info[nr].bitvector = (def_bit);       \
									spell_info[nr].targets = (tar);             \
}


void do_adjust(struct char_data * ch, char *arg, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int spell, duration;
	struct char_data *vict;
	struct affected_type *aff;

	if (!ch->desc)
		return;

	if (IS_NPC(ch)) {
		send_to_char("However you did this, you aren't alowed.\r\n", ch);
		return;
	}

	/* Get spell # */
		arg = one_argument(arg, buf);

	if (!*buf) {
		send_to_char("Usage: adjust <spell> <player> <duration>\r\n", ch);
		return;
	}


	if (is_number(buf)) {
		spell = atoi(buf);
	}
	else {
		spell = old_search_block(buf, 0, strlen(buf), spell_names, 0);

		if (spell < 0) {
			sprintf(buf2, "Spell: %s is invalid.\r\n", buf);
			send_to_char(buf2, ch);
			return;
		}
	}



	/* Get player name */
		arg = one_argument(arg, buf);
	if (!*buf) {
		send_to_char("Missing player name and duration.\r\n", ch);
		return;
	}

	/* check if we can find the player */
			if (!(vict = ha3100_get_char_vis_ex(ch, buf, TRUE))) {
			send_to_char("Nobody around with that name.\r\n", ch);
			return;
		}

	/* get duration */
		arg = one_argument(arg, buf);

	if (!*buf) {
		send_to_char("Missing duration\r\n", ch);
		return;
	}

	if (!is_number(buf)) {
		send_to_char("Duration has to be numeric.\r\n", ch);
		return;
	}

	duration = atoi(buf);

	if (duration > 500) {
		send_to_char("Duration must be between 0 and 500.\r\n", ch);
		return;
	}

	/* Check if the player is affected by the spell */
			if (!IS_CASTED_ON(vict, spell)) {
			sprintf(buf, "%s is not affected by that spell.\r\n", GET_REAL_NAME(vict));
			send_to_char(buf, ch);
			return;
		}

	/* Adjust the duration of all the affects associated to the spell */
		for (aff = vict->affected; aff; aff = aff->next)
		if (aff->type == spell)
			aff->duration = duration;

	sprintf(buf, "Duration of spell %d changed to %d for %s\r\n", spell, duration, GET_REAL_NAME(vict));
	send_to_char(buf, ch);

	/* Log it */
		sprintf(buf, " %s changed duration of spell %d to %d on %s.", GET_REAL_NAME(ch), spell, duration, GET_REAL_NAME(vict));
	spec_log(buf, GOD_COMMAND_LOG);
}

int s1000_cost_of_mana(struct char_data * ch, int lv_spl)
{

	int lv_divisor, lv_spell_level, lv_cost;



	lv_spell_level = MINV(races[GET_RACE(ch)].skill_min_level[lv_spl],
			      classes[GET_CLASS(ch)].skill_min_level[lv_spl]);

	lv_divisor =
		2 + GET_LEVEL(ch) - lv_spell_level;
	if (lv_divisor < 1)
		lv_divisor = 2;

	lv_cost = 100 / lv_divisor;
	lv_cost = MAXV(spell_info[lv_spl].min_usesmana, lv_cost);

	/* IF THIS IS HEAL MEDIUM/MAJOR ADJUST COST */
	if (lv_spl == SPELL_HEAL_MEDIUM) {
		lv_cost = (3 * lv_cost) + 20;
	}
	if (lv_spl == SPELL_HEAL_MAJOR) {
		lv_cost = (5 * lv_cost) + 30;
	}
	/* New costs for old spells */
	if (lv_spl == SPELL_PORTAL) {
		lv_cost = (4 * lv_cost);
	}
	if (lv_spl == SPELL_TELEPORT_GROUP) {
		lv_cost = (4 * lv_cost);
	}
	if (lv_spl == SPELL_BEAST_TRANSFORM) {
		lv_cost = (5 * lv_cost);
	}
	return (lv_cost);

}				/* END OF s1000_cost_of_mana() */


/* ********************************************************* */
void affect_update(void)
{

	int prev_type;
	static struct affected_type *af, *next_af_dude, ds_aff;
	static struct char_data *i;
	char buf[MAX_STRING_LENGTH];

	int lv_perm_spell, idx, lv_type;



	for (i = character_list; i; i = i->next) {
		prev_type = 0;
		for (af = i->affected; af; af = next_af_dude) {
			next_af_dude = af->next;
			if (af->duration > 0) {
				af->duration--;
			}
			if (IS_AFFECTED(i, AFF_HASTE)) {
				if (af->duration > 0) {
					af->duration--;
				}
			}
			if (af->duration == 0) {
				lv_type = af->type;
				if (af->type > 0 &&
				    af->type < MAX_SKILLS) {
					/* It must be a spell */
					if (!af->next
					    || (af->next->type != af->type)
					    || (af->next->duration > 0))
						if (*spell_wear_off_msg[af->type] &&
						    *spell_wear_off_msg[af->type] != '!' &&
						    *spell_wear_off_msg[af->type] != '#') {
							if (prev_type != af->type) {
								send_to_char((char *) spell_wear_off_msg[af->type], i);
								send_to_char("\r\n", i);
							}
							prev_type = af->type;
						}
				}
				ha1325_affect_remove(i, af, REMOVE_FOLLOWER);
			}
		}

		/* MAKE SURE AVATAR HAVE DARKSIGHT */
		if (GET_LEVEL(i) == IMO_LEV || GET_LEVEL(i) == PK_LEV) {
			if (!(IS_AFFECTED((i), AFF_DARKSIGHT))) {
				ds_aff.type = SPELL_DARKSIGHT;
				ds_aff.modifier = 0;
				ds_aff.location = APPLY_NONE;
				ds_aff.bitvector = AFF_DARKSIGHT;
				ds_aff.duration = -1;
				ha1300_affect_to_char(i, &ds_aff);
			}	/* END OF darksight SPELL NOT IN AFFECT */
		}		/* END OF AVATAR */

		/* CONFIRM DEFAULT SPELLS */
		for (idx = 0; idx < MAX_RACE_ATTRIBUTES; idx++) {
			lv_perm_spell = races[GET_RACE(i)].perm_spell[idx];

			if (lv_perm_spell > 0) {
				/* IS THIS A VALID SPELL? */
				if (spell_info[lv_perm_spell].modifier == 0 &&
				    spell_info[lv_perm_spell].location == 0 &&
				    spell_info[lv_perm_spell].bitvector == 0) {
					bzero(buf, sizeof(buf));
					sprintf(buf, "ERROR: Tried to set default spell: (%d)%s on %s - zero args\r\n",
						lv_perm_spell,
						spell_names[lv_perm_spell - 1],
						GET_REAL_NAME(i));
					do_sys(buf, IMO_IMP, i);
					spec_log(buf, ERROR_LOG);
				}
				else {
					if (!(IS_AFFECTED(i, spell_info[lv_perm_spell].bitvector))) {
						ds_aff.type = lv_perm_spell;
						ds_aff.modifier =
							spell_info[lv_perm_spell].modifier;
						ds_aff.location =
							spell_info[lv_perm_spell].location;
						ds_aff.bitvector =
							spell_info[lv_perm_spell].bitvector;
						ds_aff.duration = -1;
						ha1300_affect_to_char(i, &ds_aff);
					}	/* END OF SPELL NOT IN AFFECT */
				}	/* END OF else */
			}	/* END OF we have a perm spell */

		}		/* END OF for idx loop */

	}			/* END OF for i loop */
}

void say_spell(struct char_data * ch, int si)
{

	char splwd[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	int j, offs;
	struct char_data *temp_char;

	struct syllable {
		char org[10];
		char new[10];
	};

	struct syllable syls[] = {
		{" ", " "},
		{"ar", "abra"},
		{"au", "kada"},
		{"bless", "fido"},
		{"blind", "nose"},
		{"bur", "mosa"},
		{"cu", "judi"},
		{"de", "oculo"},
		{"en", "unso"},
		{"light", "dies"},
		{"lo", "hi"},
		{"mor", "zak"},
		{"move", "sido"},
		{"ness", "lacri"},
		{"ning", "illa"},
		{"per", "duda"},
		{"ra", "gru"},
		{"re", "candus"},
		{"son", "sabru"},
		{"tect", "infra"},
		{"tri", "cula"},
		{"ven", "nofo"},
		{"_", " "},
		{"1", "zudo"},
		{"2", "diro"},
		{"3", "dara"},
		{"4", "loz"},
		{"5", "puma"},
		{"a", "a"}, {"b", "b"}, {"c", "q"}, {"d", "e"}, {"e", "z"}, {"f", "y"}, {"g", "o"},
		{"h", "p"}, {"i", "u"}, {"j", "y"}, {"k", "t"}, {"l", "r"}, {"m", "w"}, {"n", "i"},
		{"o", "a"}, {"p", "s"}, {"q", "d"}, {"r", "f"}, {"s", "g"}, {"t", "h"}, {"u", "j"},
		{"v", "z"}, {"w", "x"}, {"x", "n"}, {"y", "l"}, {"z", "k"}, {"", ""}
	};



	strcpy(buf, "");
	strcpy(splwd, spell_names[si - 1]);

	offs = 0;

	while (*(splwd + offs)) {
		for (j = 0; *(syls[j].org); j++)
			if (strncmp(syls[j].org, splwd + offs, strlen(syls[j].org)) == 0) {
				strcat(buf, syls[j].new);
				if (strlen(syls[j].org))
					offs += strlen(syls[j].org);
				else
					++offs;
			}
	}


	sprintf(buf2, "$n utters the words, '%s'", buf);
	sprintf(buf, "$n utters the words, '%s'", spell_names[si - 1]);

	for (temp_char = world[ch->in_room].people;
	     temp_char; temp_char = temp_char->next_in_room) {
		if (temp_char != ch) {

			if (GET_CLASS(ch) == GET_CLASS(temp_char) ||
			    GET_SKILL(temp_char, si) > 0) {
				act(buf, FALSE, ch, 0, temp_char, TO_VICT);
			}
			else {
				act(buf2, FALSE, ch, 0, temp_char, TO_VICT);
			}
		}
	}			/* END OF for loop() */

}				/* END OF say_spell() */


bool saves_spell(struct char_data * ch, struct char_data * victim, sh_int save_type)
{

	int save;



	/* IF ITS AN IMP OR HIGHER, THERE IS NO SAVING THROW */
	if (GET_LEVEL(ch) >= IMO_IMP) {
		return (FALSE);
	}

	/* YOU CAN'T CAST ON PC IMMORTALS */
	if (IS_PC(victim) && GET_LEVEL(victim) > PK_LEV) {
		return (TRUE);
	}

	/* Negative apply_saving_throw makes saving throw better! */
	save = MAXV(1, victim->specials.apply_saving_throw[save_type]);

	if (save < number(1, 20))
		return (TRUE);	/* char made the save */
	else
		return (FALSE);	/* char failed the save */

}				/* END OF saves_spell() */


char *skip_spaces(char *string)
{



	for (; *string && (*string) == ' '; string++);
	return (string);

}



/* Assumes that *argument does start with first letter of chopped string */

void do_cast(struct char_data * ch, char *argument, int cmd)
{



	do_cast_proc(ch, 0, argument, cmd, 0);
	return;

}				/* END OF do_cast() */

void do_cast_proc(struct char_data * ch, struct char_data * cast_victim, char *argument, int cmd, int lv_flag)
{

	struct obj_data *tar_obj;
	struct char_data *tar_char, *victim;
	char name[MAX_STRING_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char buf2[MAX_INPUT_LENGTH];
	//char buf3[MAX_INPUT_LENGTH];
	int lv_min_lev, qend, spl, i, idx, lv_dice_roll;
	bool target_ok;



	SILENCE_CHECK();

	bzero(name, sizeof(name));

	if (IS_NPC(ch) && !gv_questreact) {
		/* DON'T LET MOBS CAST IF 'CHARMED */
		if (IS_AFFECTED(ch, AFF_CHARM)) {
			send_to_char("Sorry, you can only think about how neat your master is.\r\n", ch);
			return;
		}
	}

	if (IS_SET(GET_ACT2(ch), PLR2_JAILED)) {
		send_to_char("You can't cast spells you are jailed!\n\r", ch);
		return;
	}

	argument = skip_spaces(argument);

	/* If there are no chars in argument */
	if (!(*argument)) {
		send_to_char("Cast which what where?\n\r", ch);
		return;
	}

	/* ANY QUOTES? */
	bzero(buf2, sizeof(buf2));
	if (*argument != '\'' && *argument != '\"') {
		strcpy(buf2, argument);
		for (idx = 0; *(buf2 + idx) && *(buf2 + idx) != ' '; idx++);
		buf2[idx] = 0;
		qend = idx;
		if (is_number(buf2)) {
			spl = atoi(buf2);
		}
		else {
			for (qend = 1; *(argument + qend) && isalpha(*(argument + qend)); qend++)
				*(argument + qend) = LOWER(*(argument + qend));

			spl = old_search_block(argument, 0, qend, spell_names, 0);
			--qend;
		}
	}			/* END OF NO QUOTES */
	else {
		/* Locate the last quote && lowercase the magic words (if any) */
		for (qend = 1; *(argument + qend) &&
		     (*(argument + qend) != '\'') &&
		     (*(argument + qend) != '\"'); qend++)
			*(argument + qend) = LOWER(*(argument + qend));

		if (*(argument + qend) != '\'') {
			send_to_char("Come again? try something like cast \'a spell\'\n\r", ch);
			return;
		}

		strcpy(buf2, argument + 1);
		for (idx = 0; *(buf2 + idx) &&
		     *(buf2 + idx) != '\'' && *(buf2 + idx) != ' '; idx++);
		buf2[idx] = 0;

		if (is_number(buf2)) {
			spl = atoi(buf2);
		}
		else {
			spl = old_search_block(argument, 1, qend - 1, spell_names, 0);
		}
	}			/* END OF QUOTE CHECK */

	/* DID WE FIND THE SPELL */
	if (!spl) {
		send_to_char("Your lips move, but no magic appears.\r\n", ch);
		return;
	}

	if ((spl < 1) || (spl > MAX_SKILLS) ||
	    !spell_info[spl].spell_pointer) {
		switch (number(1, 10)) {
		case 1:
			send_to_char("Now you know why your master sighed a lot!!!\n\r", ch);
			/* send_to_char("Bylle Grylle Grop Gryf???\n\r", ch); */
			break;
		case 2:
			/* send_to_char("Olle Bolle Snop Snyf?\n\r",ch); */
			send_to_char("That doesn't work in this dimension.\n\r", ch);
			break;
		case 3:
			send_to_char("You really expect that to work?!?\n\r", ch);
			break;
		case 4:
			/* send_to_char("Gryffe Olle Gnyffe Snop???\n\r",ch); */
			send_to_char("Are you trying to cast or make up a new song?\n\r", ch);
			break;
		case 5:
			send_to_char("You must have been asleep in CANTRIPS 101!!!\n\r", ch);
			break;
		case 6:
			send_to_char("How did you ever pass typing class???\n\r", ch);
			break;
		case 7:
			send_to_char("I've never heard of that spell, wonder if its useful???\n\r", ch);
			break;
		case 8:
			send_to_char("You are not worthy of that spell!!!\n\r", ch);
			break;
		default:
			send_to_char("Bolle Snylle Gryf Bylle?!!?\n\r", ch);
			break;
		}
		return;
	}			/* END OF INVALID SPELL */

	/* IF THE SPELL ISN'T ENABLED, DON'T LET EM CAST IT */
	if (GET_LEVEL(ch) < IMO_IMP &&
	    !(IS_SET(spell_info[spl].spell_flag, ENABLED_SPELL))) {
		sprintf(buf, "Sorry, %s has been temporarily disabled.\r\n",
			spell_names[spl - 1]);
		send_to_char(buf, ch);
		return;
	}

	/* MAKE SURE WE ARE IN THE CORRECT POSITION */
	if ((IS_NPC(ch) || GET_LEVEL(ch) < IMO_IMP) &&
	    GET_POS(ch) < spell_info[spl].minimum_position) {
		switch (GET_POS(ch)) {
		case POSITION_SLEEPING:
			send_to_char("You dream about great magical powers.\n\r", ch);
			break;
		case POSITION_RESTING:
			send_to_char("You can't concentrate enough while resting.\n\r", ch);
			break;
		case POSITION_SITTING:
			send_to_char("You can't do this sitting!\n\r", ch);
			break;
		case POSITION_FIGHTING:
			send_to_char("Impossible! You can't concentrate enough!.\n\r", ch);
			break;
		default:
			send_to_char("It seems like you're in a pretty bad shape!\n\r", ch);
			break;
		}		/* Switch */
		return;

	}			/* END OF NOT IN DEFAULT POSITION */


	/* CAN THE CHAR CAST THIS SPELL? */
	if (GET_SKILL(ch, spl) == 0 &&
	    IS_PC(ch) &&
	    GET_LEVEL(ch) < IMO_SPIRIT) {
		lv_min_lev = MINV(classes[GET_CLASS(ch)].skill_min_level[spl],
				  races[GET_RACE(ch)].skill_min_level[spl]);
		if (lv_min_lev > GET_LEVEL(ch) ||
		    lv_min_lev == 0) {
			bzero(buf, sizeof(buf));
			sprintf(buf, "Sorry, but you are unable to cast that.\r\n");
			send_to_char(buf, ch);
			return;
		}
		send_to_char("You don't know that spell.\r\n", ch);
		return;
	}			/* END OF unlearned spell and LEVEL < IMO_LEV */

	/* DELAY FIGHTING SPELLS */
	if (IS_PC(ch) &&
	    (ch->specials.fighting) && (pulse % PULSE_VIOLENCE)) {
		/* if we are fighting delay spell till start of next round */
		sprintf(buf, "cast %s", argument);	/* queue command until
							 * next wait */
		WAIT_STATE(ch, PULSE_VIOLENCE - pulse % PULSE_VIOLENCE);
		co1500_write_to_head_q(buf, &ch->desc->input);
		return;		/* we'll be back! */
	}

	argument += qend + 1;	/* Point to the last ' */
	for (; *argument == ' '; argument++);

/* **************** Locate targets **************** */

	target_ok = FALSE;
	tar_char = 0;
	tar_obj = 0;

	if (IS_SET(spell_info[spl].targets, TAR_IGNORE)) {
		target_ok = TRUE;	/* No target, is a good target */
	}
	else {

		argument = one_argument(argument, name);

		/* DID USER SPECIFY A TARGET? */
		if (*name) {
			/* FIND OUT IF THE TARGET IS VALID AND MATCHES SPELL */
			if (IS_SET(spell_info[spl].targets, TAR_CHAR_ROOM)) {
				if (cast_victim)
					tar_char = cast_victim;
				else
					tar_char = ha2125_get_char_in_room_vis(ch, name);
				if (tar_char &&
				    tar_char->in_room == ch->in_room) {
					target_ok = TRUE;
				}
				else {
					if (ha1175_isexactname(GET_REAL_NAME(ch), name) &&
					    cast_victim == 0) {
						tar_char = ch;
						target_ok = TRUE;
					}
				}
			}	/* END OF target room */

			if (!target_ok &&
			    IS_SET(spell_info[spl].targets, TAR_CHAR_WORLD)) {
				if (cast_victim)
					tar_char = cast_victim;
				else
					tar_char = ha3100_get_char_vis(ch, name);
				if (tar_char)
					target_ok = TRUE;
			}

			if (!target_ok &&
			    IS_SET(spell_info[spl].targets, TAR_OBJ_INV))
				if ((tar_obj = ha2075_get_obj_list_vis(ch, name, ch->carrying)))
					target_ok = TRUE;

			if (!target_ok && IS_SET(spell_info[spl].targets, TAR_OBJ_ROOM))
				if ((tar_obj = ha2075_get_obj_list_vis(ch, name, world[ch->in_room].contents)))
					target_ok = TRUE;

			if (!target_ok && IS_SET(spell_info[spl].targets, TAR_OBJ_WORLD))
				if ((tar_obj = ha3200_get_obj_vis(ch, name)))
					target_ok = TRUE;

			if (!target_ok && IS_SET(spell_info[spl].targets, TAR_OBJ_EQUIP)) {
				for (i = 0; i < MAX_WEAR && !target_ok; i++)
					if (ch->equipment[i]) {
						/*
						       if (str_cmp(name, ch->equipment[i]->name) == 0) {
							       */
						if (ha1150_isname(name, ch->equipment[i]->name)) {
							tar_obj = ch->equipment[i];
							target_ok = TRUE;
						}
					}
			}
			if (!target_ok &&
			    IS_SET(spell_info[spl].targets, TAR_SELF_ONLY)) {
				if ((str_cmp(GET_REAL_NAME(ch), name) == 0) ||
				    (cast_victim == ch)) {
					tar_char = ch;
					target_ok = TRUE;
				}
			}


		}		/* END OF USER DIDN'T GIVE US A TARGET */

		/* NO TARGET SPECIFIED - BUT ONE IS REQUIRED */
		/* LOOK FOR DEFAULTS */
		else {

			if (IS_SET(spell_info[spl].targets, TAR_FIGHT_SELF))
				if (ch->specials.fighting) {
					tar_char = ch;
					target_ok = TRUE;
				}

			if (!target_ok && IS_SET(spell_info[spl].targets, TAR_FIGHT_VICT))
				if (ch->specials.fighting) {
					/* WARNING, MAKE INTO POINTER */
					tar_char = ch->specials.fighting;
					target_ok = TRUE;
				}

			if (!target_ok && IS_SET(spell_info[spl].targets, TAR_SELF_ONLY)) {
				tar_char = ch;
				target_ok = TRUE;
			}

		}		/* END OF NO TARGET SPECIFIED - LOOKING FOR
				 * DEFAULT */

	}			/* END OF TARGET REQUIRED */


	/* WE DIDN'T GET A TARGET */
	if (!target_ok) {
		if (*name) {	/* WAS TARGET SPECIFIED? */
			if (IS_SET(spell_info[spl].targets, TAR_CHAR_ROOM))
				send_to_char("Nobody here by that name.\n\r", ch);
			else if (IS_SET(spell_info[spl].targets, TAR_CHAR_WORLD))
				send_to_char("Nobody playing by that name.\n\r", ch);
			else if (IS_SET(spell_info[spl].targets, TAR_OBJ_INV))
				send_to_char("You are not carrying anything like that.\n\r", ch);
			else if (IS_SET(spell_info[spl].targets, TAR_OBJ_ROOM))
				send_to_char("Nothing here by that name.\n\r", ch);
			else if (IS_SET(spell_info[spl].targets, TAR_OBJ_WORLD))
				send_to_char("Ok.\n\r", ch);	/* so No-locate works */
			else if (IS_SET(spell_info[spl].targets, TAR_OBJ_EQUIP))
				send_to_char("You are not wearing anything like that.\n\r", ch);
			else if (IS_SET(spell_info[spl].targets, TAR_OBJ_WORLD))
				send_to_char("Nothing at all by that name.\n\r", ch);
			else
				send_to_char("A valid target type has not been defined for this spell.\r\n", ch);

		}		/* END OF TARGET SPECIFIED */
		else {		/* Nothing was given as argument */
			if (spell_info[spl].targets < TAR_OBJ_INV)
				send_to_char("Who should the spell be cast upon?\n\r", ch);
			else
				send_to_char("What should the spell be cast upon?\n\r", ch);
		}
		return;
	}

/* WE HAVE A TARGET */


	if (GET_LEVEL(ch) < IMO_IMP &&
	    tar_char == ch &&
	    IS_SET(spell_info[spl].targets, TAR_SELF_NONO)) {
		send_to_char("You can not cast this spell upon yourself.\n\r", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMP &&
	    tar_char != ch &&
	    IS_SET(spell_info[spl].targets, TAR_SELF_ONLY) &&
	    !((spl == SPELL_HEAL_MINOR) ||
	      (spl == SPELL_HEAL_MEDIUM) ||
	      (spl == SPELL_HEAL_MAJOR) ||
	      (spl == SPELL_CONVALESCE))) {
		send_to_char("You can only cast this spell upon yourself.\n\r", ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == tar_char)) {
		send_to_char("You are afraid that it could harm your master.\n\r", ch);
		return;
	}

	if (IS_PC(ch) &&
	    GET_LEVEL(ch) < IMO_IMM) {
		if (GET_MANA(ch) < s1000_cost_of_mana(ch, spl)) {
			send_to_char("You can't summon enough energy to cast the spell.\n\r", ch);
			return;
		}
	}

	if (IS_NPC(ch)) {
		/* IF THIS ISN'T A MOB SPELL, DON'T LET EM CAST */
		if (!(IS_SET(spell_info[spl].spell_flag, MOB_SPELL))) {
			send_to_char("Sorry, mobs can't cast this spell\r\n", ch);
			return;
		}
	}

/* IS THIS A VIOLENT SPELL? */
	if (IS_SET(spell_info[spl].spell_flag, VIOLENT_SPELL)) {
		if (tar_char) {
			victim = tar_char;
			CHECK_FOR_NO_FIGHTING();
			IS_ROOM_NO_KILL();
		}
		/* SPELL DOESN'T REQUIRE A VICTIM - FOR EXAMPLE, EARTHQUAKE */
		else {
			for (victim = world[ch->in_room].people; victim;
			     victim = victim->next_in_room) {
				if (ch != victim) {
					CHECK_FOR_NO_FIGHTING();
					IS_ROOM_NO_KILL();
				}
			}
		}

	}			/* END OF THIS IS A VIOLENT SPELL */


	/* CASTING GROUP SPELL AND NOT GROUPED? */
	if (!(IS_AFFECTED(ch, AFF_GROUP))) {
		if (spl == SPELL_TELEPORT_GROUP) {
			send_to_char("You are not grouped.\r\n", ch);
			return;
		}
	}

	if (IS_PC(ch) &&
	    GET_LEVEL(ch) > PK_LEV &&
	    *buf2) {
		bzero(buf2, sizeof(buf2));
		if (IS_SET(spell_info[spl].spell_flag, VIOLENT_SPELL)) {
			sprintf(buf2, "%s cast (V%d) %s ",
				GET_REAL_NAME(ch),
				spl,
				spell_names[spl - 1]);
		}
		else {
			sprintf(buf2, "%s cast (%d) %s ",
				GET_REAL_NAME(ch),
				spl,
				spell_names[spl - 1]);
		}
		if (*name) {
			strncat(buf2, name, sizeof(buf2) - strlen(buf2) - 2);
		}
		if (!(GET_BLESS(ch))) {
			do_wizinfo(buf2, GET_LEVEL(ch) + 1, ch);
			spec_log(buf2, SPELL_LOG);
		}
	}			/* END OF DO WIZINFO */

	if (spl != SPELL_VENTRILOQUATE)	/* :-) */
		say_spell(ch, spl);

	/* synchronise spells with fighting */
	if (IS_PC(ch) &&
	    GET_LEVEL(ch) < IMO_IMP) {
		WAIT_STATE(ch, spell_info[spl].beats);
	}

	if ((spell_info[spl].spell_pointer == 0) && spl > 0) {
		send_to_char("Sorry, this magic has not yet been implemented :(\n\r", ch);
		return;
	}

	if (GET_LEVEL(ch) < IMO_IMM) {
		lv_dice_roll = number(0, 100);
		if (lv_dice_roll == 0 || lv_dice_roll > GET_SKILL(ch, spl) + GET_INT(ch)) {
			//11.22 .2004 - RLUM
				// Removed because was in the game.
			/* if (ha1175_isexactname(GET_NAME(ch), "cleric") ||
			 * ha1175_isexactname(GET_NAME(ch), "crystal") ||
			 * ha1175_isexactname(GET_NAME(ch), "catalyst")) {
			 * sprintf(buf3, "DEBUG: roll: %d skill: %d INT:
			 * %d\r\n", lv_dice_roll, GET_SKILL(ch, spl),
			 * GET_INT(ch) ); send_to_char(buf3, ch); } */
				send_to_char("You lost your concentration!\n\r", ch);
			if (IS_PC(ch)) {
				GET_MANA(ch) -= (s1000_cost_of_mana(ch, spl) >> 1);
			}
			return;
		}

		if (IS_PC(ch)) {
			GET_MANA(ch) -= (s1000_cost_of_mana(ch, spl));
		}

		/* IS ROOM NO MAGIC? */
		if (IS_SET(world[ch->in_room].room_flags, RM1_NO_MAGIC)) {
			send_to_char("Hrmmm, you chanted and hopped around like the book said, but nothing happens.\r\n", ch);
			return;
		}

		li9900_gain_proficiency(ch, spl);
	}			/* END OF level < IMO_IMM */

	send_to_char("Ok.\r\n", ch);
	if (spl == SPELL_LOCATE_OBJECT) {
		spell_locate_object(GET_LEVEL(ch), ch, 0, tar_obj, name);
	}
	else {
		((*spell_info[spl].spell_pointer) (GET_LEVEL(ch), ch, argument, SPELL_TYPE_SPELL, tar_char, tar_obj));
	}
	return;

}				/* END OF do_cast_proc() */


void assign_spell_pointers(void)
{

	int idx;



	for (idx = 0; idx < MAX_SKILLS; idx++) {
		spell_info[idx].spell_flag = 0;
		spell_info[idx].spell_pointer = 0;
	}

	/* ARGUMENTS 1) spell number 2) beats 3) minimum position 4) minimum
	 * uses mana 5) unused           6) flags           7) modifier 8)
	 * location 9) bitvector 10) targets 11) func pointer SPELLO(nr, beat,
	 * pos, mana, tar, flag, func) { \ */

	SPELLO(1, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       10, APPLY_AC, 0,
	       TAR_CHAR_ROOM, cast_armor);

	SPELLO(2, 12, POSITION_FIGHTING, 35, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_SELF_ONLY, cast_elsewhere);

	SPELLO(3, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       10, APPLY_HITROLL, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_CHAR_ROOM, cast_bless);

	SPELLO(4, 12, POSITION_FIGHTING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_blindness);

	SPELLO(5, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_burning_hands);

	SPELLO(6, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_hail_storm);

	SPELLO(7, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_NONO, cast_charm_person);

	SPELLO(8, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_chill_touch);

	SPELLO(9, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       -40, APPLY_AC, AFF_INVISIBLE,
	       TAR_CHAR_ROOM, cast_improved_invis);

	SPELLO(10, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_colour_spray);

	SPELLO(11, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_control_weather);

	SPELLO(12, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_create_food);

	SPELLO(13, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_create_water);

	SPELLO(14, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_cure_blind);

	SPELLO(15, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_cure_critic);

	SPELLO(16, 12, POSITION_FIGHTING, 10, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_cure_light);

	SPELLO(17, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_OBJ_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP,
	       cast_curse);

	SPELLO(18, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, AFF_DETECT_EVIL,
	       TAR_CHAR_ROOM, cast_detect_evil);

	SPELLO(19, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       0, APPLY_NONE, AFF_DETECT_INVISIBLE,
	       TAR_CHAR_ROOM, cast_detect_invisibility);

	SPELLO(20, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, AFF_DETECT_MAGIC,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_detect_magic);

	SPELLO(21, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	     TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_detect_poison);

	SPELLO(22, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_dispel_evil);

	SPELLO(23, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_tremor);

	SPELLO(24, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_enchant_weapon);

	SPELLO(25, 12, POSITION_FIGHTING, 35, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_energy_drain);

	SPELLO(26, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_fireball);

	SPELLO(27, 12, POSITION_FIGHTING, 35, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_harm);

	SPELLO(28, 12, POSITION_FIGHTING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_dispel_magic);

	SPELLO(29, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       -40, APPLY_AC, AFF_INVISIBLE,
	       TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP,
	       cast_invisibility);

	SPELLO(30, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_lightning_bolt);

	SPELLO(31, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_WORLD, cast_locate);

	SPELLO(32, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_magic_missile);

	SPELLO(33, 12, POSITION_STANDING, 10, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_NONO | TAR_OBJ_INV | TAR_OBJ_EQUIP,
	       cast_poison);

	SPELLO(34, 12, POSITION_STANDING, 10, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, AFF_PROTECT_EVIL,
	       TAR_CHAR_ROOM, cast_protection_from_evil);

	SPELLO(35, 12, POSITION_STANDING, 10, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_OBJ_ROOM,
	       cast_remove_curse);

	SPELLO(36, 12, POSITION_STANDING, 75, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,	/* general so mob won't
								 * cast it   */
	/* as a defensive spell.  We'll   */
	/* force it when we get in combat */
	       0, APPLY_NONE, AFF_SANCTUARY,
	       TAR_CHAR_ROOM, cast_sanctuary);

	SPELLO(37, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_shocking_grasp);

	SPELLO(38, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_sleep);

	SPELLO(39, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_strength);

	SPELLO(40, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_WORLD, cast_summon);

	SPELLO(41, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	    TAR_CHAR_ROOM | TAR_OBJ_ROOM | TAR_SELF_NONO, cast_ventriloquate);

	SPELLO(42, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_word_of_recall);

	SPELLO(43, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, cast_remove_poison);

	SPELLO(44, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_SENSE_LIFE,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_sense_life);

	SPELLO(45, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(46, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(47, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(48, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(49, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(50, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(51, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(52, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	SPELLO(53, 1, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP | TAR_OBJ_ROOM, cast_identify);
	SPELLO(54, 1, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_cause_critic);
	SPELLO(55, 1, POSITION_FIGHTING, 10, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_cause_light);

	SPELLO(56, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(57, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(58, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(59, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(60, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(61, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(62, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	/* BREATH ATTACKS */
	SPELLO(63, 0, POSITION_STANDING, 200, 0,
	       ENABLED_SPELL | MOB_SKILL | VIOLENT_SKILL,
	       0, 0, 0, TAR_IGNORE, cast_fire_breath);
	SPELLO(64, 0, POSITION_STANDING, 200, 0,
	       ENABLED_SPELL | MOB_SKILL | VIOLENT_SKILL,
	       0, 0, 0, TAR_IGNORE, cast_gas_breath);
	SPELLO(65, 0, POSITION_STANDING, 200, 0,
	       ENABLED_SPELL | MOB_SKILL | VIOLENT_SKILL,
	       0, 0, 0, TAR_IGNORE, cast_frost_breath);
	SPELLO(66, 0, POSITION_STANDING, 200, 0,
	       ENABLED_SPELL | MOB_SKILL | VIOLENT_SKILL,
	       0, 0, 0, TAR_IGNORE, cast_acid_breath);
	SPELLO(67, 0, POSITION_STANDING, 200, 0,
	       ENABLED_SPELL | MOB_SKILL | VIOLENT_SKILL,
	       0, 0, 0, TAR_IGNORE, cast_lightning_breath);
	/* END OF BREATH ATTACKS */

	SPELLO(68, 3, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_DARKSIGHT,
	       TAR_CHAR_ROOM, cast_darksight);

	SPELLO(69, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(70, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(71, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(72, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	SPELLO(73, 3, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_restoration);

	SPELLO(74, 3, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_REGENERATION,
	       TAR_CHAR_ROOM, cast_regeneration);

	SPELLO(75, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_heal_minor);

	SPELLO(76, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_heal_medium);

	SPELLO(77, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_heal_major);

	SPELLO(78, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	SPELLO(79, 3, POSITION_FIGHTING, 10, 0,
	       ENABLED_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_turn_undead);

	SPELLO(80, 3, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_succor);

	SPELLO(81, 3, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_donate_mana);

	SPELLO(83, 3, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_magic_resist);

	SPELLO(84, 3, POSITION_FIGHTING, 75, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, AFF_MAGIC_IMMUNE,
	       TAR_CHAR_ROOM, cast_magic_immune);

	SPELLO(85, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_DETECT_EVIL,
	       TAR_CHAR_ROOM, cast_detect_good);

	SPELLO(86, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_conflagration);

	SPELLO(87, 3, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_BREATHWATER,
	       TAR_CHAR_ROOM, cast_breathwater);

	SPELLO(88, 3, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_sustenance);

	SPELLO(89, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	SPELLO(90, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_NONO, cast_hold_person);

	SPELLO(91, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_knock);

	SPELLO(92, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_phase_door);

	SPELLO(93, 3, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF_WATERWALK, TAR_CHAR_ROOM, cast_waterwalk);

	SPELLO(94, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_WORLD, cast_teleport_self);

	SPELLO(95, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_WORLD, cast_teleport_group);

	SPELLO(96, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ZONE, cast_teleview_minor);

	SPELLO(97, 3, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_WORLD, cast_teleview_major);

	SPELLO(98, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_haste);

	SPELLO(99, 3, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_dreamsight);

	SPELLO(100, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_gate);

	SPELLO(101, 3, POSITION_FIGHTING, 30, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_vitalize_mana);

	SPELLO(102, 3, POSITION_FIGHTING, 30, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_vitalize_hit);

	SPELLO(103, 3, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_vigorize_minor);

	SPELLO(104, 3, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_vigorize_medium);

	SPELLO(105, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_vigorize_major);

	SPELLO(106, 12, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       10, APPLY_HITROLL, 0,
	       TAR_CHAR_ROOM, cast_aid);

	SPELLO(107, 3, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_silence);

	SPELLO(108, 3, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_dispel_silence);

	SPELLO(109, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_dispel_hold);

	SPELLO(110, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_block_summon);

	SPELLO(111, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_smite);

	SPELLO(112, 3, POSITION_STANDING, 25, 0,
	       MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_OBJ_ROOM, cast_animate_dead);

	SPELLO(113, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_black_burst);

	SPELLO(114, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_cremation);

	SPELLO(115, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_mortice_flame);

	SPELLO(116, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_firelance);

	SPELLO(117, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_firestorm);

	SPELLO(118, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_earthquake);

	SPELLO(119, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_divine_retribution);

	SPELLO(120, 12, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_mirror_self);

	SPELLO(121, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       10, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_recharge_light);

	SPELLO(122, 12, POSITION_STANDING, 100, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_recharge_wand);

	SPELLO(123, 12, POSITION_STANDING, 100, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_venom_blade);

	SPELLO(124, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_IGNORE, cast_irresistable_dance);

	SPELLO(125, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_wizard_lock);

	SPELLO(126, 12, POSITION_STANDING, 100, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_dancing_sword);

	SPELLO(127, 12, POSITION_STANDING, 80, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_demonic_aid);


	SPELLO(128, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM, cast_grangorns_curse);

	SPELLO(129, 12, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       10, APPLY_HITROLL, 0,
	       TAR_SELF_ONLY, cast_clawed_hands);

	SPELLO(130, 12, POSITION_STANDING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       10, APPLY_HITROLL, 0,
	       TAR_SELF_ONLY, cast_flying_disk);

	SPELLO(131, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_intelligence);

	SPELLO(132, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_wisdom);

	SPELLO(133, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_dexterity);

	SPELLO(134, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_constitution);

	SPELLO(135, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_charisma);

	SPELLO(136, 0, POSITION_STANDING,
	       200, 0, MOB_SKILL, 0, 0, 0,
	       TAR_IGNORE, 0);

	SPELLO(137, 0, POSITION_STANDING,
	       200, 0, MOB_SKILL, 0, 0, 0,
	       TAR_IGNORE, 0);

	SPELLO(138, 0, POSITION_STANDING,
	       200, 0, MOB_SKILL, 0, 0, 0,
	       TAR_IGNORE, 0);

	SPELLO(139, 3, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | MOVEMENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_WORLD, cast_portal);

	SPELLO(140, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	    0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_hemmorage);

	SPELLO(141, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_fear);

	SPELLO(142, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | DEFENSIVE_SPELL,
	       -40, APPLY_AC, AFF_DEMON_FLESH, TAR_SELF_ONLY, cast_demon_flesh);

	SPELLO(143, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_din_mak);

	SPELLO(144, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(145, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(146, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(147, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(148, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);


	SPELLO(149, 12, POSITION_FIGHTING, 50, 0,
	       MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_pestilence);

	SPELLO(150, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_spiritual_transfer);

	SPELLO(151, 12, POSITION_STANDING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL,
	       0, APPLY_MANA, 0, TAR_CHAR_ROOM, cast_bind_souls);

	SPELLO(152, 12, POSITION_STANDING, 250, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_mordenkainens_sword);

	SPELLO(153, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_call_lightning);

	SPELLO(154, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_fly);

	SPELLO(155, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(156, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);
	SPELLO(157, 0, POSITION_STANDING, 200, 0, MOB_SKILL, 0, 0, 0, TAR_IGNORE, 0);

	SPELLO(158, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_wolf);

	SPELLO(159, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_bear);

	SPELLO(160, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_dragon);

	SPELLO(161, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_monkey);

	SPELLO(162, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_ninja);

	SPELLO(163, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_chloroplast);

	SPELLO(164, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	   0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_faerie_fog);

	SPELLO(165, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	     0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_entangle);

	SPELLO(166, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF2_BARKSKIN, TAR_SELF_ONLY, cast_barkskin);

	SPELLO(167, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF2_STONESKIN, TAR_SELF_ONLY, cast_stoneskin);

	SPELLO(168, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	   0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_thornwrack);

	SPELLO(169, 12, POSITION_STANDING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_calm);

	SPELLO(170, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_web);

	SPELLO(171, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	0, APPLY_NONE, AFF2_DETECT_UNDEAD, TAR_CHAR_ROOM, cast_detect_undead);

	SPELLO(172, 12, POSITION_FIGHTING, 150, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_animal_summoning);

	SPELLO(173, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_pass_without_trace);

	SPELLO(174, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	     0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_earthmaw);

	SPELLO(175, 12, POSITION_FIGHTING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	  0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_needlestorm);

	SPELLO(176, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_treewalk);

	SPELLO(177, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_summon_treant);

	SPELLO(178, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	   0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_sporecloud);

	SPELLO(179, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_wrath_of_nature);

	SPELLO(180, 12, POSITION_STANDING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	  0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_faerie_fire);

	SPELLO(182, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_create_spring);

	SPELLO(183, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_IGNORE, cast_leviathan);

	SPELLO(184, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	 0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_meteor_swarm);

	SPELLO(185, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_beast_transform);

	SPELLO(186, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_tsunami);

	SPELLO(187, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_tornado);

	SPELLO(188, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	    0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_landslide);

	SPELLO(190, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_NONO, cast_charm_animal);

	SPELLO(192, 12, POSITION_FIGHTING, 10, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	   0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_acid_blast);

	SPELLO(193, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_displacement);

	SPELLO(194, 12, POSITION_STANDING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_summon_elemental);

	SPELLO(195, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	    0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_shockwave);

	SPELLO(196, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_gust);

	SPELLO(197, 12, POSITION_FIGHTING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	 0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_granite_fist);

	SPELLO(198, 12, POSITION_FIGHTING, 20, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_prayer);

	SPELLO(199, 12, POSITION_FIGHTING, 50, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_SELF_ONLY, cast_elemental_shield);

	SPELLO(204, 12, POSITION_STANDING, 60, 0,
	       ENABLED_SPELL | HEAL_SPELL,
	  0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_SELF_NONO, cast_holy_healing);

	SPELLO(205, 12, POSITION_FIGHTING, 60, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_lifetap);

	SPELLO(206, 12, POSITION_STANDING, 75, 0,
	       ENABLED_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_enchant_armor);

	SPELLO(207, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_vex);

	SPELLO(208, 12, POSITION_STANDING, 55, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, AFF_SANCTUARY,
	       TAR_CHAR_ROOM, cast_sanctuary_minor);

	SPELLO(209, 12, POSITION_STANDING, 65, 0,
	       ENABLED_SPELL | MOB_SPELL | GENERAL_SPELL,
	       0, APPLY_NONE, AFF_SANCTUARY,
	       TAR_CHAR_ROOM, cast_sanctuary_medium);

	SPELLO(210, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_womble);

	SPELLO(211, 12, POSITION_FIGHTING, 15, 0,
	       ENABLED_SPELL | MOB_SPELL | VIOLENT_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_FIGHT_VICT, cast_gusher);

	SPELLO(212, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_manticore);

	SPELLO(213, 12, POSITION_STANDING, 5, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, AFF2_SLEEP_IMMUNITY,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_sleep_immunity);

	SPELLO(215, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_chimera);

	SPELLO(217, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | MOB_SPELL | HELPFUL_SPELL,
	       0, APPLY_NONE, 0, TAR_CHAR_ROOM, cast_transform_cow);

	SPELLO(218, 12, POSITION_FIGHTING, 100, 0,
	       ENABLED_SPELL | MOB_SPELL | HEAL_SPELL,
	       0, APPLY_NONE, 0,
	       TAR_CHAR_ROOM | TAR_SELF_ONLY, cast_convalesce);

	SPELLO(219, 12, POSITION_STANDING, 25, 0,
	       ENABLED_SPELL | GENERAL_SPELL, 0, APPLY_NONE, 0,
	       TAR_OBJ_INV | TAR_OBJ_EQUIP, cast_purge_magic);

}				/* END OF assign_spell_pointers() */
