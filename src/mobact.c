/* mb */
/* gv_location: 9501-1000 */
/* ******************************************************************* *
*  file: mobact.c , Mobile action module.            Part of DIKUMUD   *
*  Usage: Procedures generating 'intelligent' behavior in the mobiles. *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.     *
********************************************************************* */

#define ROOM_CHECK(lv_text) {    \
	if (ch->in_room < 0) {        \
		bzero(buf, sizeof(buf)); \
		sprintf(buf, "ERROR: Found mob with negative room in %s.\r\n", \
				(lv_text)); \
		main_log(buf);           \
	}                             \
}
/*
 sprintf(gv_str, "01 mob %s", GET_REAL_NAME(ch));
 ROOM_CHECK(gv_str);
 */
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "parser.h"
#include "spells.h"
#include "handler.h"
#include "constants.h"
#include "func.h"
#include "mobact.h"
#include "globals.h"

void mb1000_mobile_activity(void)
{

	struct char_data *ch, *next_ch, *tmp_ch;
	struct obj_data *obj, *best_obj;
	int found, max, lv_attack, lv_mob_num;

	char buf[MAX_STRING_LENGTH];

	extern int no_specials;



	for (ch = character_list; ch; ch = next_ch) {
		next_ch = ch->next;
		if (IS_MOB(ch) && !(IS_AFFECTED(ch, AFF_HOLD_PERSON)) && AWAKE(ch)) {

			lv_mob_num = mob_index[ch->nr].virtual;

			if (ch->in_room < 0) {
				bzero(buf, sizeof(buf));
				sprintf(buf, "ERROR: Mob with negative room. %s %d.\r\n",
					GET_REAL_NAME(ch), ch->in_room);
				main_log(buf);
			}

			/* IF THIS IS A GATE Demon AND IT DOESN'T HAVE */
			/* A LEADER/FOLLOWER THEN SEND IT HOME.        */
			if (lv_mob_num > 2689 && lv_mob_num < 2700) {
				if (!ch->master &&
				    !ch->followers) {
					act("$n returns to the pits from which it came!", FALSE, ch, 0, 0, TO_ROOM);
					wi2900_purge_items(ch, "\0", 0);
					ha3000_extract_char(ch, END_EXTRACT_BY_GATE_NO_LEADER);
					continue;
				}
				/* IF DEMON ISN'T IN THE SAME ROOM, WITH ITS
				 * MASTER, */
				/* TELEPORT TO HIM/HER                               */
				if (ch->master) {
					if (ch->in_room != ch->master->in_room) {
						GET_SKILL(ch, SPELL_TELEPORT_SELF) = 70;
						spell_teleport_self(GET_LEVEL(ch), ch, ch->master, 0);
						continue;
					}	/* END OF NOT IN THE SAME ROOM */
				}	/* END OF WE HAVE A MASTER */
			}	/* END OF its a demon */

			/* IF THIS IS A TREANT or ANIMAL SUMMON AND IT DOESN'T HAVE */
			/* A LEADER/FOLLOWER THEN SEND IT HOME.        */
			if (lv_mob_num > 2609 && lv_mob_num < 2619) {
				if (!ch->master &&
				    !ch->followers) {
					act("$n returns to the wilderness!", FALSE, ch, 0, 0, TO_ROOM);
					wi2900_purge_items(ch, "\0", 0);
					ha3000_extract_char(ch, END_EXTRACT_BY_GATE_NO_LEADER);
					continue;
				}
				/* IF TREANT/ANIMAL ISN'T IN THE SAME ROOM, WITH ITS MASTER, */
				/* TELEPORT TO HIM/HER                               */
				if (ch->master) {
					if (ch->in_room != ch->master->in_room) {
						GET_SKILL(ch, SPELL_TELEPORT_SELF) = 70;
						spell_teleport_self(GET_LEVEL(ch), ch, ch->master, 0);
						continue;
					}	/* END OF NOT IN THE SAME ROOM */
				}	/* END OF WE HAVE A MASTER */
			}	/* END OF its a TREANT or ANIMAL */

			/* IF THIS IS A PET FROM THE PET SHOP AND IT     */
			/* DOESN'T HAVE A LEADER/FOLLOWER, SEND IT HOME. */
			if (lv_mob_num > 3089 && lv_mob_num < 3095) {
				if (!ch->master &&
				    !ch->followers) {
					act("$n returns to the Pet Shop it was adopted from!", FALSE, ch, 0, 0, TO_ROOM);
					wi2900_purge_items(ch, "\0", 0);
					ha3000_extract_char(ch, END_EXTRACT_BY_GATE_NO_LEADER);
					continue;
				}
				/* IF PET ISN'T IN THE SAME ROOM, WITH ITS MASTER, */
				/* TELEPORT TO HIM/HER                               */
				if (ch->master) {
					if (ch->in_room != ch->master->in_room) {
						GET_SKILL(ch, SPELL_TELEPORT_SELF) = 70;
						spell_teleport_self(GET_LEVEL(ch), ch, ch->master, 0);
						continue;
					}	/* END OF NOT IN THE SAME ROOM */
				}/* END OF WE HAVE A MASTER */
			}	/* END OF its a PET */

			/* IF THIS IS A Dancing Sword AND IT DOESN'T HAVE */
			/* A LEADER/FOLLOWER THEN SEND IT HOME.        */
			if (lv_mob_num == 909) {
				if (!ch->master &&
				    !ch->followers) {
					act("$n disappears as the magic that animated it fades!", FALSE, ch, 0, 0, TO_ROOM);
					wi2900_purge_items(ch, "\0", 0);
					ha3000_extract_char(ch, END_EXTRACT_BY_GATE_NO_LEADER);
					continue;
				}
				/* IF Dancing Sword ISN'T IN THE SAME ROOM, WITH ITS
				 * MASTER, */
				/* TELEPORT TO HIM/HER                               */
				if (ch->master) {
					if (ch->in_room != ch->master->in_room) {
						GET_SKILL(ch, SPELL_TELEPORT_SELF) = 70;
						spell_teleport_self(GET_LEVEL(ch), ch, ch->master, 0);
						continue;
					}	/* END OF NOT IN THE SAME ROOM */
				}	/* END OF WE HAVE A MASTER */
			} /* End of its a Dancing Sword*/
			
			/* IF THIS IS THE ENFYRN DRAGON, AND ITS NOT FIGHTING */
			/* AND FULLY HEALED, CONVERT IT BACK TO THE PRIEST    */
			if (mob_index[ch->nr].virtual == MOB_ENFYRN_DRAGON) {
				if (GET_HIT(ch) >= li1500_hit_limit(ch) &&
				    !(ch->specials.fighting)) {
					mb1125_enfyrn_dragon_changes_to_priest(ch);
					continue;
				}
			}

			/* Examine call for special procedure */
			if (no_specials == 0 && mob_index[ch->nr].func) {
				if ((*mob_index[ch->nr].func) (ch, 0, "")) {
					continue;
				}
			}

			if (IS_SET(GET_ACT2(ch), PLR2_MOB_AI)) {
				if (mb1200_mob_artificial_int(ch, 0)) {
					continue;
				}
			}

			if (!(ch->specials.fighting)) {
				if (IS_SET(GET_ACT2(ch), PLR2_USES_EQ)) {
					if (mb2000_uses_equipment(ch, 0)) {
						continue;
					}
				}

				if (IS_SET(GET_ACT2(ch), PLR2_JUNKS_EQ)) {
					if (mb2100_junks_equipment(ch, 0)) {
						continue;
					}
				}

				if (IS_SET(GET_ACT2(ch), PLR2_EQTHIEF)) {
					if (mb2200_equipment_thief(ch, 0)) {
						continue;
					}
				}

				if (IS_SET(GET_ACT2(ch), PLR2_CLOSE_DOORS)) {
					if (mb2400_do_mob_close(ch, 0)) {
						continue;
					}
				}

				if (IS_SET(GET_ACT1(ch), PLR1_GUARD1)) {
					if (pr3450_cityguard(ch, 0, "")) {
						continue;
					}
				}

				if (GET_SKILL(ch, SKILL_STEAL) > 0 &&
				    !IS_AFFECTED(ch, AFF_CHARM)) {
					if (pr2300_thief(ch, 0, "")) {
						continue;
					}
				}

				if (IS_SET(ch->specials.act, PLR1_SCAVENGER)) {
					if (world[ch->in_room].contents && !number(0, 10)) {
						for (max = 1, best_obj = 0,
						     obj = world[ch->in_room].contents;
						obj; obj = obj->next_content) {
							if (CAN_GET_OBJ(ch, obj)) {
								if (obj->obj_flags.cost > max) {
									best_obj = obj;
									max = obj->obj_flags.cost;
								}
							}
						}	/* END OF for max = 1 */

						/* ZCMDS worth -1, DONT pickup */
						if ((best_obj) && (max > 0)) {
							ha2200_obj_from_room(best_obj);
							ha1700_obj_to_char(best_obj, ch);
							act("$n gets $p.",
							    FALSE, ch, best_obj, 0, TO_ROOM);
						}
					}
				}	/* Scavenger */

				if (!IS_SET(GET_ACT1(ch), PLR1_SENTINEL)) {
					if (mb2300_do_mob_move(ch, 0)) {
						continue;
					}
				}

				if ((IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_EVIL) ||
				 IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_NEUT) ||
				   IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_GOOD))
				    ||
				 (IS_SET(GET_ACT1(ch), PLR1_INJURED_ATTACK) &&
				  GET_HIT(ch) < li1500_hit_limit(ch))) {
					found = FALSE;
					for (tmp_ch = world[ch->in_room].people;
					     tmp_ch && !found;
					     tmp_ch = tmp_ch->next_in_room) {

						/* BY DEFAULT, WE CAN'T ATTACK */
						lv_attack = FALSE;

						/* LETS SEE IF WE CAN ATTACK */
						if (IS_EVIL(tmp_ch) &&
						    IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_EVIL)) {
							lv_attack = TRUE;
						}
						if (IS_NEUTRAL(tmp_ch) &&
						    IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_NEUT)) {
							lv_attack = TRUE;
						}
						if (IS_GOOD(tmp_ch) &&
						    IS_SET(GET_ACT1(ch), PLR1_AGGRESSIVE_GOOD)) {
							lv_attack = TRUE;
						}

						if (IS_SET(GET_ACT1(ch), PLR1_INJURED_ATTACK) &&
						    GET_HIT(ch) < li1500_hit_limit(ch)) {
							lv_attack = TRUE;
						}
						/*
						 if (number(0,2))
						 lv_attack = FALSE;
						 */
						if (IS_NPC(tmp_ch))
							lv_attack = FALSE;

						if (!(CAN_SEE(ch, tmp_ch)))
							lv_attack = FALSE;

						if (ha1375_affected_by_spell(tmp_ch,
							  AFF_PROTECT_EVIL)) {
							if (GET_LEVEL(ch) < GET_LEVEL(tmp_ch)) {
								lv_attack = FALSE;
							}
							else {
								if ((number(1, 20) > 5) &&
								    GET_ALIGNMENT(ch) < -100 &&
								    GET_ALIGNMENT(ch) + 999 <
								    GET_ALIGNMENT(tmp_ch)) {
									lv_attack = FALSE;
								}
							}
						}

						if (IS_SET(GET_ACT3(tmp_ch), PLR3_NOHASSLE))
							lv_attack = FALSE;

						if (IS_SET(gv_run_flag,
						   RUN_FLAG_DISABLE_FIGHTING))
							lv_attack = FALSE;

						if (IS_SET(world[ch->in_room].room_flags,
							   RM1_NOPKILL))
							lv_attack = FALSE;

						if (ch->in_room > 0 &&
						    world[ch->in_room].number >= VEHICLE_BEG_ROOM &&
						    world[ch->in_room].number <= VEHICLE_END_ROOM) {
							lv_attack = FALSE;
						}
						if (lv_attack == TRUE) {
							if (!IS_SET(ch->specials.act, PLR1_WIMPY) ||
							    !AWAKE(tmp_ch)) {
								hit(ch, tmp_ch, 0);
								found = TRUE;
								continue;
							}	/* END OF PLR1_WIMPY */
						}	/* END OF random number */

					}	/* END OF for tmp_ch loop */
				}	/* IF MOB IS AGGRESSIVE */

			}	/* If AWAKE(ch)   */
		}		/* If IS_MOB(ch)  */
	}			/* END OF for LOOP */
}				/* END OF mb1000_mobile_activity */

void mb1100_enfyrn_priest_changes_to_dragon(struct char_data * ch)
{

	struct char_data *lv_mob, *tmp_ch, *lv_next_combat;

	struct obj_data *lv_obj;
	int real_num;



	if (IS_PC(ch)) {
		return;
	}

	if (mob_index[ch->nr].virtual != MOB_ENFYRN_PRIEST) {
		return;
	}

	real_num = db8100_real_mobile(MOB_ENFYRN_DRAGON);
	if (real_num < 0) {
		send_to_char("Ack, you can't focus on your dragon form.\r\n",
			     ch);
		return;
	}

	lv_mob = db5000_read_mobile(real_num, REAL);
	GET_START(lv_mob) = world[ch->in_room].number;
	ha1600_char_to_room(lv_mob, ch->in_room);

	/* LOAD A DRAGON CLAW */
	real_num = db8200_real_object(2211);
	if (real_num < 0) {
		send_to_char("Hrmmm, we can't find the claw for your dragon.\r\n", ch);
	}
	else {
		/* IF NOT MAXED, LOAD AND GIVE TO DRAGON */
		if (obj_index[real_num].number < obj_index[real_num].maximum) {
			lv_obj = db5100_read_object(real_num, REAL);
			/* WEAR CLAW */
			ha1925_equip_char(lv_mob, lv_obj, 9, 0, BIT1);
		}
	}

	/* OK, EVERYBODY FIGHTING THE PRIEST FIGHTS THE DRAGON */
	for (tmp_ch = combat_list; tmp_ch; tmp_ch = lv_next_combat) {
		lv_next_combat = tmp_ch->next_fighting;
		if (tmp_ch->specials.fighting == ch) {
			ft1400_stop_fighting(tmp_ch, 0);
			ft1300_set_fighting(tmp_ch, lv_mob, 1);
			send_to_char("I don't think this dragon is going to be as easy as that priest!", tmp_ch);
		}		/* END OF found a char fighting priest */
	}			/* END OF for combat list */

	/* MAKE THE DRAGON FIGHT THE SAME PERSON THE PRIEST IS WORKING ON */
	ft1300_set_fighting(lv_mob, ch->specials.fighting, 1);

	/* STOP THE PRIEST FIGHTING */
	ft1400_stop_fighting(ch, 0);

	/* REMOVE ANY EQUIPMENT THE PRIEST HAS */
	wi2900_purge_items(ch, "\0", 0);

	/* REMOVE THE PRIEST */
	ha3000_extract_char(ch, END_EXTRACT_BY_REMOVE_PRIEST);

	act("Muttering words under his breath, the priest Enfyrn jumps back from you.",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	act("As you watch dumbstruck, the small priest drops down onto all fours and he",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	act("begins to increase in size dramatically.  Almost filling the cave in seconds.",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	act("His skin begins to turn much redder and you realize the shape he is becoming",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	act("is that of a great red dragon!",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	return;

}				/* END OF
				 * mb1100_enfyrn_priest_changes_to_dragon() */


void mb1125_enfyrn_dragon_changes_to_priest(struct char_data * ch)
{

	struct char_data *lv_mob;
	int real_num;



	if (IS_PC(ch)) {
		return;
	}

	if (mob_index[ch->nr].virtual != MOB_ENFYRN_DRAGON) {
		return;
	}

	if (ch->specials.fighting) {
		return;
	}

	real_num = db8100_real_mobile(MOB_ENFYRN_PRIEST);
	if (real_num < 0) {
		send_to_char("Ack, you can't focus on your priest form.\r\n",
			     ch);
		return;
	}

	lv_mob = db5000_read_mobile(real_num, REAL);
	GET_START(lv_mob) = world[ch->in_room].number;
	ha1600_char_to_room(lv_mob, ch->in_room);

	/* REMOVE ANY EQUIPMENT THE DRAGON HAS */
	wi2900_purge_items(ch, "\0", 0);

	/* REMOVE THE DRAGON */
	ha3000_extract_char(ch, END_EXTRACT_BY_REMOVE_DRAGON);

	act("Having healed itself, the dragon recites several phrases in an arcane",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	act("language and slowly meta-morphs into its smaller human form.",
	    TRUE, lv_mob, 0, 0, TO_ROOM);
	return;

}				/* END OF
				 * mb1125_enfyrn_dragon_changes_to_priest() */

void mb1150_force_from_parser(struct char_data * ch, char *arg, int cmd)
{



	if (gv_port != DEVELOPMENT_PORT) {
		send_to_char("Nothing happens.\r\n", ch);
		return;
	}

	mb1200_mob_artificial_int(ch, 0);
	return;
}

int mb1200_mob_artificial_int(struct char_data * ch, int lv_flag)
{
	int rc, lv_skill, lv_spell, lv_random, lv_type_of_spell;



	/* IF CHARMED, WE DON'T DO ANYTHING */
	if (IS_AFFECTED(ch, AFF_CHARM)) {
		return (0);
	}

	/* IF WE ARE FIGHTING, DECIDE ON HEAL/OFFENSIVE SPELL */
	if (ch->specials.fighting) {
		/* FIRST THING TO LOOK FOR IS SANC SPELL */
		if (ch->skills[SPELL_SANCTUARY].learned > 0 &&
		    !IS_AFFECTED(ch, AFF_SANCTUARY)) {
			rc = mb1300_mob_cast(ch, SPELL_SANCTUARY);
			return (rc);
		}
		rc = 0;
		if (!number(0, 3))
			rc = mb1400_pick_spell_skill(ch, HEAL_SPELL);
		if (!(rc)) {
			/* WE DON'T HAVE A HEAL SPELL */
			lv_spell = mb1400_pick_spell_skill(ch, VIOLENT_SPELL);
			lv_skill = mb1400_pick_spell_skill(ch, VIOLENT_SKILL);
			/* IF WE HAVE A SKILL/SPELL */
			if (lv_spell && lv_skill) {
				if (number(0, 1))
					rc = lv_spell;
				else
					rc = lv_skill;
			}
			else {
				/* ONE OF THESE IS ZERO */
				rc = lv_spell + lv_skill;
			}
		}
		if (rc) {
			rc = mb1300_mob_cast(ch, rc);
		}
		return (rc);

	}			/* END OF FIGHTING */

	/* WE AREN'T FIGHTING, SO CHECK OTHER SPELLS */
	/* WHAT KIND OF SPELL WILL IT BE? */
	lv_random = number(0, 20);

	if (lv_random < 5) {
		lv_type_of_spell = HEAL_SPELL;
	}
	else if (lv_random < 8) {
		lv_type_of_spell = DEFENSIVE_SPELL;
	}
	else if (lv_random < 10) {
		lv_type_of_spell = HELPFUL_SPELL;
	}
	else if (lv_random < 11) {
		lv_type_of_spell = MOVEMENT_SPELL;
	}
	else {
		return (0);
	}

	rc = mb1400_pick_spell_skill(ch, lv_type_of_spell);
	if (rc) {
		rc = mb1300_mob_cast(ch, rc);
	}

	return (FALSE);

}				/* END OF mb1200_mob_artificial_int() */

int mb1300_mob_cast(struct char_data * ch, int lv_spell)
{

	int rc;
	char buf[MAX_STRING_LENGTH];
	struct char_data *victim;



	/* IF AFFECTED BY THIS SPELL, FORGET IT */
	if (ha1375_affected_by_spell(ch, lv_spell)) {
		if (gv_port == DEVELOPMENT_PORT) {
			send_to_char("Mob is already affected by this spell\r\n", ch);
		}
		return (0);
	}

	/* ASSUME WE DID SOMETHING */
	rc = TRUE;

	/* DEFAULT TO MOST COMMON FORMAT FOR MOST SPELLS */
	bzero(buf, sizeof(buf));
	victim = mb1500_pick_victim(ch, lv_spell, 0);
	if (!victim) {
		return (0);
	}

	if (gv_port == DEVELOPMENT_PORT) {
		sprintf(buf, "casting spell %s[%d]\r\n",
			spell_names[lv_spell - 1], lv_spell);
		send_to_char(buf, ch);
	}

	sprintf(buf, "'%s' %s",
		spell_names[lv_spell - 1],
		GET_REAL_NAME(victim));

	switch (lv_spell) {
	case SPELL_ARMOR:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_ELSEWHERE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BLESS:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BLINDNESS:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BURNING_HANDS:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_HAIL_STORM:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CHARM_PERSON:{
			rc = FALSE;
			break;
		}
	case SPELL_CHILL_TOUCH:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_IMPROVED_INVIS:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_COLOUR_SPRAY:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CONTROL_WEATHER:{
			rc = FALSE;
			break;
		}
	case SPELL_CREATE_FOOD:{
			rc = FALSE;
			break;
		}
	case SPELL_CREATE_WATER:{
			rc = FALSE;
			break;
		}
	case SPELL_CURE_BLIND:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CURE_CRITIC:{
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_CURE_LIGHT:{
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_CURSE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DETECT_EVIL:{
			rc = FALSE;
			break;
		}
	case SPELL_DETECT_INVISIBLE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DETECT_MAGIC:{
			rc = FALSE;
			break;
		}
	case SPELL_DETECT_POISON:{
			rc = FALSE;
			break;
		}
	case SPELL_DISPEL_EVIL:{
			rc = FALSE;
			break;
		}
	case SPELL_TREMOR:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_ENCHANT_WEAPON:{
			rc = FALSE;
			break;
		}
	case SPELL_ENERGY_DRAIN:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_FIREBALL:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_HARM:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DISPEL_MAGIC:{
			rc = FALSE;
			break;
		}
	case SPELL_INVISIBLE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_LIGHTNING_BOLT:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_LOCATE_OBJECT:{
			rc = FALSE;
			break;
		}
	case SPELL_MAGIC_MISSILE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_POISON:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_PROTECT_FROM_EVIL:{
			rc = FALSE;
			break;
		}
	case SPELL_REMOVE_CURSE:{
			if (ha1375_affected_by_spell(victim, SPELL_CURSE) ||
			    ha1375_affected_by_spell(victim, SPELL_GRANGORNS_CURSE)) {
				do_cast_proc(ch, victim, buf, 0, 2);
				rc = FALSE;
				break;
			}
			else
				rc = FALSE;
			break;
		}

	case SPELL_SANCTUARY:{
			say_spell(ch, SPELL_SANCTUARY);
			spell_sanctuary(GET_LEVEL(ch), ch, victim, 0);
			break;
		}

	case SPELL_SANCTUARY_MEDIUM:{
			say_spell(ch, SPELL_SANCTUARY_MEDIUM);
			spell_sanctuary_medium(GET_LEVEL(ch), ch, victim, 0);
			break;
		}


	case SPELL_SANCTUARY_MINOR:{
			say_spell(ch, SPELL_SANCTUARY_MINOR);
			spell_sanctuary_minor(GET_LEVEL(ch), ch, victim, 0);
			break;
		}
	case SPELL_SHOCKING_GRASP:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SLEEP:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_STRENGTH:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SUMMON:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_VENTRILOQUATE:{
			rc = FALSE;
			break;
		}
	case SPELL_WORD_OF_RECALL:{
			if (!ch->specials.fighting &&
			    ch->in_room > 0 &&
			    GET_START(ch) > 0 &&
			    world[ch->in_room].number != GET_START(ch)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_REMOVE_POISON:{
			if (ha1375_affected_by_spell(victim, SPELL_POISON)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_SENSE_LIFE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SKILL_SNEAK:{
			rc = FALSE;
			break;
		}
	case SKILL_HIDE:{
			rc = FALSE;
			break;
		}
	case SKILL_STEAL:{
			rc = FALSE;
			break;
		}
	case SKILL_BACKSTAB:{
			rc = FALSE;
			break;
		}
	case SKILL_PICK_LOCK:{
			rc = FALSE;
			break;
		}
	case SKILL_KICK:{
			rc = FALSE;
			break;
		}
	case SKILL_BASH:{
			rc = FALSE;
			break;
		}
	case SKILL_RESCUE:{
			rc = FALSE;
			break;
		}
	case SPELL_IDENTIFY:{
			rc = FALSE;
			break;
		}
	case SPELL_CAUSE_CRITIC:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CAUSE_LIGHT:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SKILL_EXTRA_DAMAGE:{
			rc = FALSE;
			break;
		}
	case SKILL_SLASH:{
			rc = FALSE;
			break;
		}
	case SKILL_PIERCE:{
			rc = FALSE;
			break;
		}
	case SKILL_BLUDGEON:{
			rc = FALSE;
			break;
		}
	case SKILL_PARRY:{
			rc = FALSE;
			break;
		}
	case SKILL_DODGE:{
			rc = FALSE;
			break;
		}
	case SKILL_SCALES:{
			rc = FALSE;
			break;
		}
	case SPELL_FIRE_BREATH:{
			cast_fire_breath(GET_LEVEL(ch), ch,
					 "", SPELL_TYPE_SPELL, victim, 0);
			break;
		}
	case SPELL_GAS_BREATH:{
			cast_gas_breath(GET_LEVEL(ch), ch,
					"", SPELL_TYPE_SPELL, 0, 0);
			break;
		}
	case SPELL_FROST_BREATH:{
			cast_frost_breath(GET_LEVEL(ch), ch,
					  "", SPELL_TYPE_SPELL, victim, 0);
			break;
		}
	case SPELL_ACID_BREATH:{
			cast_acid_breath(GET_LEVEL(ch), ch,
					 "", SPELL_TYPE_SPELL, victim, 0);
			break;
		}
	case SPELL_LIGHTNING_BREATH:{
			cast_lightning_breath(GET_LEVEL(ch), ch,
					      "", SPELL_TYPE_SPELL, victim, 0);
			break;
		}
	case SPELL_DARKSIGHT:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SKILL_SECOND:{
			rc = FALSE;
			break;
		}
	case SKILL_THIRD:{
			rc = FALSE;
			break;
		}
	case SKILL_FOURTH:{
			rc = FALSE;
			break;
		}
	case SPELL_UNDETECT_INVIS:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_RESTORATION:{
			rc = FALSE;
			break;
		}
	case SPELL_REGENERATION:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_HEAL_MINOR:{
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_HEAL_MEDIUM:{
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_HEAL_MAJOR:{
			if (GET_HIT(victim) < li1500_hit_limit(victim)) {
				do_cast_proc(ch, victim, buf, 0, 2);
			}
			else {
				rc = FALSE;
			}
			break;
		}
	case SPELL_SPIRIT_HAMMER:{
			rc = FALSE;
			break;
		}
	case SPELL_SUCCOR:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DONATE_MANA:{
			rc = FALSE;
			break;
		}
	case SPELL_MANA_LINK:{
			rc = FALSE;
			break;
		}
	case SPELL_MAGIC_RESIST:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_MAGIC_IMMUNE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DETECT_GOOD:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CONFLAGRATION:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BREATHWATER:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SUSTENANCE:{
			rc = FALSE;
			break;
		}
	case SKILL_HUNT:{
			rc = FALSE;
			break;
		}
	case SPELL_HOLD_PERSON:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_KNOCK:{
			rc = FALSE;
			break;
		}
	case SPELL_PHASE_DOOR:{
			rc = FALSE;
			break;
		}
	case SPELL_WATERWALK:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_TELEPORT_SELF:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_TELEPORT_GROUP:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_TELEVIEW_MINOR:{
			rc = FALSE;
			break;
		}
	case SPELL_TELEVIEW_MAJOR:{
			rc = FALSE;
			break;
		}
	case SPELL_HASTE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DREAMSIGHT:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_GATE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_VITALIZE_MANA:{
			rc = FALSE;
			break;
		}
	case SPELL_VITALIZE_HIT:{
			rc = FALSE;
			break;
		}
	case SPELL_VIGORIZE_MINOR:{
			rc = FALSE;
			break;
		}
	case SPELL_VIGORIZE_MEDIUM:{
			rc = FALSE;
			break;
		}
	case SPELL_VIGORIZE_MAJOR:{
			rc = FALSE;
			break;
		}
	case SPELL_AID:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SILENCE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DISPEL_SILENCE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DISPEL_HOLD:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BLOCK_SUMMON:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SMITE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_ANIMATE_DEAD:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BLACK_BURST:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_CREMATION:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_MORTICE_FLAME:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_FIRELANCE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_FIRESTORM:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_EARTHQUAKE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DIVINE_RETRIBUTION:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_FAERIE_FIRE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_FAERIE_FOG:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_BARKSKIN:{
			say_spell(ch, SPELL_BARKSKIN);
			spell_barkskin(GET_LEVEL(ch), ch, victim, 0);
			break;
		}
	case SPELL_STONESKIN:{
			say_spell(ch, SPELL_STONESKIN);
			spell_stoneskin(GET_LEVEL(ch), ch, victim, 0);
			break;
		}
	case SPELL_THORNWRACK:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DETECT_UNDEAD:{
			rc = FALSE;
			break;
		}
	case SPELL_BEAST_TRANSFORM:{
			rc = FALSE;
			break;
		}
	case SPELL_NEEDLESTORM:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_METEOR_SWARM:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_TSUNAMI:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_TORNADO:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_LANDSLIDE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SHOCKWAVE:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_GUST:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_DISPLACEMENT:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_SUMMON_ELEMENTAL:{
			rc = FALSE;
			break;
		}
	case SPELL_GRANITE_FIST:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_PRAYER:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_ELEMENTAL_SHIELD:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_GUSHER:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SPELL_LIFETAP:{
			do_cast_proc(ch, victim, buf, 0, 2);
			break;
		}
	case SKILL_FIFTH:{
			rc = FALSE;
			break;
		}
	case SKILL_TRIP:{
			rc = FALSE;
			break;
		}
	default:{
			rc = FALSE;
		}
	}			/* END OF switch() */

	return (rc);

}				/* END OF mb1300_mob_cast() */


int mb1400_pick_spell_skill(struct char_data * ch, int lv_type)
{
	/* abc */

	int spells[MAX_SKILLS], lv_num_found, lv_spell_to_return, idx;



	/* LOAD ARRAY WITH SPELLS/SKILLS OF THIS TYPE */
	lv_num_found = 0;
	for (idx = 1; idx < MAX_SKILLS; idx++) {
		if (IS_SET(spell_info[idx].spell_flag, lv_type)) {
			if (ch->skills[idx].learned > 0) {
				spells[lv_num_found] = idx;
				lv_num_found++;
			}
		}
	}

	/* IF WE DIDN'T FIND ONE, BACK OUT */
	if (lv_num_found < 1) {
		return (0);
	}

	switch (lv_type) {
	case VIOLENT_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case VIOLENT_SKILL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case DEFENSIVE_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case MOVEMENT_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case GENERAL_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case HEAL_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	case HELPFUL_SPELL:{
			lv_spell_to_return = number(1, lv_num_found);
			break;
		}
	default:{
			return (0);
		}
	}			/* END OF switch() */

	return (spells[lv_spell_to_return - 1]);

}				/* END OF mb1400_pick_spell_skill() */


struct char_data *mb1500_pick_victim(
			     struct char_data * ch, int lv_spell, int lv_flag)
{

	struct char_data *victim;



	if (lv_spell == SPELL_SUMMON ||
	    lv_spell == SPELL_SUCCOR ||
	    lv_spell == SPELL_TELEPORT_SELF ||
	    lv_spell == SPELL_TELEPORT_GROUP) {
		if (HUNTING(ch))
			victim = HUNTING(ch);
		else
			victim = 0;
	}
	else if (IS_SET(spell_info[lv_spell].spell_flag, VIOLENT_SPELL) ||
		 IS_SET(spell_info[lv_spell].spell_flag, VIOLENT_SKILL) ||
		 IS_SET(spell_info[lv_spell].spell_flag, MOVEMENT_SPELL)) {
		if (ch->specials.fighting) {
			/* WE CAN'T MOVE WHILE FIGHTING! */
			if (IS_SET(spell_info[lv_spell].spell_flag, MOVEMENT_SPELL)) {
				victim = 0;
			}
			else {
				victim = ch->specials.fighting;
			}
		}
		else {
			if (HUNTING(ch)) {
				victim = HUNTING(ch);
			}
			else {
				victim = ch;
			}
		}		/* END OF else not fighting */
	}			/* END OF VIOLENT/MOVEMENT */
	else {
		victim = ch;
	}

	/* IF victim is AFFECTED BY THIS SPELL, FORGET IT */
	if (victim) {
		if (ha1375_affected_by_spell(victim, lv_spell)) {
			if (gv_port == DEVELOPMENT_PORT) {
				send_to_char("Victim is already affected by this spell\r\n", ch);
			}
			return (0);
		}
		/* IF THIS ISN'T A MOVEMENT SPELL AND THE VICTIM IS IN */
		/* ANOTHER ROOM, FORGET IT */
		if (IS_SET(spell_info[lv_spell].spell_flag, MOVEMENT_SPELL)) {
			if (ch->in_room != victim->in_room) {
				return (0);
			}
		}
	}			/* END OF victim */

	return (victim);

}				/* END OF mb1400_pick_victim() */

int mb2000_uses_equipment(struct char_data * ch, int lv_type)
{

	struct obj_data *obj, *next_obj;



	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (number(1, 10) == 1 && GET_TIMER(obj) == -1) {
			do_find_wear_location(ch, obj, 0);
			return (TRUE);
		}		/* END OF number() */
	}			/* END OF for LOOP */

	return (FALSE);

}				/* END OF mb2000_uses_equipment */

int mb2100_junks_equipment(struct char_data * ch, int lv_type)
{

	struct obj_data *obj, *next_obj;
	char buf[MAX_STRING_LENGTH];



	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (number(1, 2) == 1) {
			act("$n junks $p.\r\n", FALSE, ch, obj, 0, TO_ROOM);
			act("You junk $p.\r\n", FALSE, ch, obj, 0, TO_CHAR);
			sprintf(buf, "%s entered: junk obj %d %s",
				GET_REAL_NAME(ch),
				GET_OBJ_VNUM(obj),
				GET_OSDESC(obj));
			do_wizinfo(buf, IMO_IMM, ch);
			spec_log(buf, STEAL_LOG);
			ha2700_extract_obj(obj);
			return (TRUE);
		}		/* END OF number() */
	}			/* END OF for LOOP */

	return (FALSE);

}				/* END OF mb2100_junkds_equipment */


/* MOB steals EQUIPMENT */
int mb2200_equipment_thief(struct char_data * ch, int lv_type)
{


	struct char_data *victim;
	struct obj_data *obj, *valuable_obj;
	char buf[MAX_STRING_LENGTH];
	int percent, lv_save_invis;



	if (number(0, 1)) {
		return (FALSE);
	}

	for (victim = world[ch->in_room].people;
	     victim && number(0, 2);
	     victim = victim->next_in_room) {
	}
	if (!victim) {
		return (FALSE);
	}

	/* DON'T STEAL FROM OURSELF OR NON CHARMED PCs */
	if (victim == ch) {
		return (FALSE);
	}

	if (IS_AFFECTED(ch, AFF_CHARM)) {
		return (FALSE);
	}

	if (IS_NPC(victim) &&
	    !IS_AFFECTED(victim, AFF_CHARM)) {
		return (FALSE);
	}
	/* DON'T PICK ON LINKDEAD PCs */
	if (IS_PC(victim)) {
		if (!victim->desc) {
			return (FALSE);
		}
		if (victim->desc->connected == CON_LINK_DEAD) {
			return (FALSE);
		}
	}
	/* DON'T STEAL FROM IMMORTALS */
	if (GET_LEVEL(victim) > PK_LEV) {
		return (FALSE);
	}

	/* IS ROOM NO_STEAL? */
	if (IS_SET(world[ch->in_room].room_flags, RM1_NOSTEAL)) {
		return (FALSE);
	}
	/* OK TO PERFORM AGGRESSIVE ACT? */
	//CHECK_FOR_NO_FIGHTING();
	//IS_ROOM_NO_KILL();

	/* FIND THE MOST VALUABLE OBJECT IN THEIR INVENTORY */
	valuable_obj = 0;
	for (obj = victim->carrying; obj; obj = obj->next_content) {
		if (!IS_PERSONAL(obj)) {
			if (!valuable_obj) {
				valuable_obj = obj;
			}
			else {
				if (obj->obj_flags.cost > valuable_obj->obj_flags.cost) {
					valuable_obj = obj;
				}
			}
		}
	}			/* END OF for LOOP */

	/* DID WE FIND ANYTHING? */
	if (!valuable_obj) {
		return (FALSE);
	}

	/* WHATS OUR CHANCE OF STEALING? */
	percent = number(1, 101) -
		(li9650_dex_adjustment(ch, 3) - GET_LEVEL(victim));
	if (!AWAKE(victim)) {
		percent = 1;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%s entered: steal %s(%d) %s\r\n",
		GET_REAL_NAME(ch),
		GET_OSDESC(valuable_obj),
		GET_OBJ_VNUM(valuable_obj),
		GET_REAL_NAME(victim));
	do_wizinfo(buf, IMO_IMM, victim);

	lv_save_invis = GET_VISIBLE(ch);
	if (GET_LEVEL(ch) < IMO_IMP) {
		GET_VISIBLE(ch) = IMO_IMM;
		act(buf, TRUE, ch, 0, victim, TO_ROOM);
		GET_VISIBLE(ch) = lv_save_invis;
	}
	else if (GET_LEVEL(victim) == IMO_IMP) {
		send_to_char(buf, victim);
	}


	/* DO WE MAKE THE STEAL ATTEMPT? */
	if (percent > ch->skills[SKILL_STEAL].learned) {
		sprintf(buf, "You failed to steal %s.\r\n",
			GET_OBJ_NAME(valuable_obj));
		send_to_char(buf, ch);
		sprintf(buf, "%s tried to steal from you.\r\n",
			GET_REAL_NAME(ch));
		send_to_char(buf, victim);
		return (TRUE);
	}

	ha1800_obj_from_char(valuable_obj);
	ha1700_obj_to_char(valuable_obj, ch);
	sprintf(buf, "You steal %s from %s.\r\n",
		GET_OBJ_NAME(valuable_obj),
		GET_REAL_NAME(victim));
	send_to_char(buf, ch);
	sprintf(buf, "STEAL: %s stole %s(%d) from: %s.",
		GET_REAL_NAME(ch),
		GET_OSDESC(valuable_obj),
		GET_OBJ_VNUM(valuable_obj),
		GET_REAL_NAME(victim));
	spec_log(buf, STEAL_LOG);
	return (TRUE);

}				/* END OF mb2200_equipment_thief */


int mb2300_do_mob_move(struct char_data * ch, int lv_flag)
{

	int lv_move_dir;
	char buf[MAX_STRING_LENGTH];



	if (GET_POS(ch) != POSITION_STANDING) {
		return (FALSE);
	}

	if (mob_index[ch->nr].virtual == 516) {
		lv_move_dir = 0;
	}
	lv_move_dir = number(0, 30);

	if (lv_move_dir >= MAX_DIRS) {
		return (FALSE);
	}

	if (!world[ch->in_room].dir_option[lv_move_dir]) {
		return (FALSE);
	}

	/* IF THERE IS A CLOSED DOOR, TRY TO OPEN IT */
	if (IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_CLOSED)) {
		if (!IS_AFFECTED(ch, AFF_CHARM)) {
			if (IS_SET(GET_ACT2(ch), PLR2_OPEN_DOORS)) {
				sprintf(buf, "%s", dirs[lv_move_dir]);
				mo1700_do_open(ch, buf, CMD_OPEN);
			}	/* END OF MOB OPENS DOORS */
		}		/* END OF NPC and NOT CHARMED */
	}			/* END OF CLOSED EXIT */

	if (!CAN_GO(ch, lv_move_dir)) {
		return (FALSE);
	}

	if (IS_SET(world[EXIT(ch, lv_move_dir)->to_room].room_flags,
		   RM1_NO_MOB)) {
		return (FALSE);
	}

	if (IS_SET(world[EXIT(ch, lv_move_dir)->to_room].room_flags,
		   RM1_DEATH)) {
		return (FALSE);
	}

	/* DON'T LET EM GO THE SAME DIRECTION IMMEDIATELY   */
	/* THIS MIGHT KEEP A MOB FROM GOING NORTH THE FIRST */
	/* TIME BECAUSE ITS SET TO ZERO WHEN CHAR IS LOADED */
	if (ch->specials.last_direction == lv_move_dir) {
		ch->specials.last_direction = -1;
		return (FALSE);
	}

	if (IS_SET(GET_ACT1(ch), PLR1_STAY_ZONE)) {
		if (world[EXIT(ch, lv_move_dir)->to_room].zone !=
		    world[ch->in_room].zone) {
			return (FALSE);
		}
	}			/* END OF STAY ZONE */

	ch->specials.last_direction = lv_move_dir;
	mo1500_do_move(ch, "", lv_move_dir + 1);

	/* DOES MOB CLOSE DOORS? */
	if (IS_SET(GET_ACT2(ch), PLR2_CLOSE_DOORS)) {
		sprintf(buf, dirs[reverse_dir[lv_move_dir]]);
		mo1800_do_close(ch, buf, CMD_CLOSE);
	}

	return (TRUE);

}				/* END OF mb2300_do_mob_move() */


int mb2400_do_mob_close(struct char_data * ch, int lv_flag)
{

	int lv_move_dir;
	char buf[MAX_STRING_LENGTH];



	if (GET_POS(ch) != POSITION_STANDING) {
		return (FALSE);
	}

	if (mob_index[ch->nr].virtual == 516) {
		lv_move_dir = 0;
	}
	lv_move_dir = number(0, 30);

	if (lv_move_dir >= MAX_DIRS) {
		return (FALSE);
	}

	if (!world[ch->in_room].dir_option[lv_move_dir]) {
		return (FALSE);
	}

	/* IF THERE IS A CLOSED DOOR, TRY TO OPEN IT */
	if (!IS_SET(EXIT(ch, lv_move_dir)->exit_info, EX_CLOSED)) {
		if (!IS_AFFECTED(ch, AFF_CHARM)) {
			sprintf(buf, "%s", dirs[lv_move_dir]);
			mo1800_do_close(ch, buf, CMD_CLOSE);
			return (TRUE);
		}		/* END OF NPC and NOT CHARMED */
	}			/* END OF OPEN EXIT */

	return (FALSE);

}				/* END OF mb2400_do_mob_close() */
