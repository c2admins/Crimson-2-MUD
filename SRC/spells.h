/* ********************************************************************
*  file: spells.h , Implementation of magic spells.   Part of DIKUMUD *
*  Usage : Spells                                                     *
********************************************************************* */

#define TYPE_UNDEFINED         	-1
#define SPELL_RESERVED_DBC       0 /* SKILL NUMBER ZERO */
#define SPELL_ARMOR              1 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ELSEWHERE          2 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLESS              3 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLINDNESS          4 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BURNING_HANDS      5 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HAIL_STORM         6
#define SPELL_CHARM_PERSON       7 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CHILL_TOUCH        8 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_IMPROVED_INVIS     9
#define SPELL_COLOUR_SPRAY     	 10 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CONTROL_WEATHER  	 11 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_FOOD      	 12 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_WATER     	 13 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_BLIND       	 14 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_CRITIC      	 15 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_LIGHT       	 16 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURSE            	 17 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_EVIL      	 18 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_INVISIBLE 	 19 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_MAGIC     	 20 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_POISON    	 21 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_EVIL      	 22 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_TREMOR           	 23 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENCHANT_WEAPON   	 24 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENERGY_DRAIN     	 25 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_FIREBALL         	 26 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HARM             	 27 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_MAGIC     	 28 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INVISIBLE        	 29 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LIGHTNING_BOLT   	 30 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LOCATE_OBJECT    	 31 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_MAGIC_MISSILE    	 32 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_POISON           	 33 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_PROTECT_FROM_EVIL	 34 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_CURSE     	 35 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SANCTUARY  		 36 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SHOCKING_GRASP   	 37 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SLEEP            	 38 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_STRENGTH         	 39 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SUMMON           	 40 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_VENTRILOQUATE    	 41 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WORD_OF_RECALL   	 42 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_POISON    	 43 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SENSE_LIFE       	 44 /* Reserved Skill[] DO NOT CHANGE */
/* types of attacks and skills must NOT use same numbers as spells! */
#define SKILL_SNEAK            	 45 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_HIDE             	 46 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_STEAL            	 47 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_BACKSTAB         	 48 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_PICK_LOCK        	 49 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_KICK             	 50 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_BASH             	 51 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_RESCUE           	 52 /*  */
#define SPELL_IDENTIFY           53
#define SPELL_CAUSE_CRITIC       54 
#define SPELL_CAUSE_LIGHT        55 
#define SKILL_EXTRA_DAMAGE       56
#define SKILL_SLASH              57
#define SKILL_PIERCE             58
#define SKILL_BLUDGEON           59
#define SKILL_PARRY              60
#define SKILL_DODGE              61
#define SKILL_SCALES             62
#define SPELL_FIRE_BREATH        63
#define SPELL_GAS_BREATH         64
#define SPELL_FROST_BREATH       65
#define SPELL_ACID_BREATH        66
#define SPELL_LIGHTNING_BREATH   67
#define SPELL_DARKSIGHT          68
#define SKILL_SECOND             69
#define SKILL_THIRD              70
#define SKILL_FOURTH             71
#define SPELL_UNDETECT_INVIS     72
#define SPELL_RESTORATION        73
#define SPELL_REGENERATION       74
#define SPELL_HEAL_MINOR         75
#define SPELL_HEAL_MEDIUM        76
#define SPELL_HEAL_MAJOR         77
#define SPELL_SPIRIT_HAMMER      78
#define SPELL_TURN_UNDEAD        79
#define SPELL_SUCCOR             80
#define SPELL_DONATE_MANA        81
#define SPELL_MANA_LINK          82
#define SPELL_MAGIC_RESIST       83
#define SPELL_MAGIC_IMMUNE       84
#define SPELL_DETECT_GOOD        85 
#define SPELL_CONFLAGRATION      86
#define SPELL_BREATHWATER        87
#define SPELL_SUSTENANCE         88
#define SKILL_HUNT               89
#define SPELL_HOLD_PERSON        90
#define SPELL_KNOCK              91
#define SPELL_PHASE_DOOR         92
#define SPELL_WATERWALK          93
#define SPELL_TELEPORT_SELF      94
#define SPELL_TELEPORT_GROUP     95
#define SPELL_TELEVIEW_MINOR     96
#define SPELL_TELEVIEW_MAJOR     97
#define SPELL_HASTE              98
#define SPELL_DREAMSIGHT         99
#define SPELL_GATE               100
#define SPELL_VITALIZE_MANA      101
#define SPELL_VITALIZE_HIT       102
#define SPELL_VIGORIZE_MINOR     103
#define SPELL_VIGORIZE_MEDIUM    104
#define SPELL_VIGORIZE_MAJOR     105
#define SPELL_AID                106
#define SPELL_SILENCE            107
#define SPELL_DISPEL_SILENCE     108
#define SPELL_DISPEL_HOLD        109
#define SPELL_BLOCK_SUMMON       110
#define SPELL_SMITE              111
#define SPELL_ANIMATE_DEAD       112
#define SPELL_BLACK_BURST        113
#define SPELL_CREMATION          114
#define SPELL_MORTICE_FLAME      115
#define SPELL_FIRELANCE          116
#define SPELL_FIRESTORM          117
#define SPELL_EARTHQUAKE         118
#define SPELL_DIVINE_RETRIBUTION 119
#define SPELL_MIRROR_SELF        120
#define SPELL_RECHARGE_LIGHT     121
#define SPELL_RECHARGE_WAND      122
#define SPELL_VENOM_BLADE        123
#define SPELL_IRRESISTABLE_DANCE 124
#define SPELL_WIZARD_LOCK        125
#define SPELL_DANCING_SWORD      126
#define SPELL_DEMONIC_AID        127
#define SPELL_GRANGORNS_CURSE    128
#define SPELL_CLAWED_HANDS       129
#define SPELL_DETECT_ANIMALS     130
#define SPELL_INTELLIGENCE       131
#define SPELL_WISDOM             132
#define SPELL_DEXTERITY          133
#define SPELL_CONSTITUTION       134
#define SPELL_CHARISMA           135
#define SKILL_ASSESS             136
#define SKILL_RAGE               137
#define SKILL_SWITCH             138
#define SPELL_PORTAL             139
#define SPELL_HEMMORAGE          140
#define SPELL_FEAR               141
#define SPELL_DEMON_FLESH        142
#define SKILL_DIN_MAK            143
#define SKILL_TSUGIASHI          144
#define SKILL_MEDITATE           145
#define SKILL_SENSE_DANGER       146
#define SKILL_BERSERK	         147
#define SKILL_FLURRY             148
#define SPELL_PESTILENCE	     149
#define SPELL_SPIRIT_TRANSFER    150
#define SPELL_BIND_SOULS         151
#define SPELL_MORDEN_SWORD       152
#define SPELL_CALL_LIGHTNING     153
#define SPELL_FLY                154
#define SKILL_ESCAPE		     155
#define SKILL_PEEK		     	 156
#define SPELL_DEATHSHADOW	     157
#define SPELL_TRANSFORM_WOLF     158
#define SPELL_TRANSFORM_BEAR	 159
#define SPELL_TRANSFORM_DRAGON	 160
#define SPELL_TRANSFORM_MONKEY	 161
#define SPELL_TRANSFORM_NINJA    162
#define SPELL_DETECT_GIANTS		 163
#define SPELL_FAERIE_FOG		 164
#define SPELL_DETECT_SHADOWS	 165
#define SPELL_BARKSKIN			 166
#define SPELL_STONESKIN			 167
#define SPELL_THORNWRACK		 168
#define SPELL_DETECT_DEMONS		 169
#define SPELL_DETECT_DRAGONS	 170
#define SPELL_DETECT_UNDEAD		 171
#define SPELL_ANIMAL_SUMMONING	 172
#define SPELL_DETECT_VAMPIRES    173
#define SPELL_EARTHMAW			 174
#define SPELL_NEEDLESTORM		 175
#define SPELL_TREEWALK			 176
#define SPELL_SUMMON_TREANT		 177
#define SPELL_SPORECLOUD		 178
#define SPELL_WRATH_OF_NATURE	 179
#define SPELL_FAERIE_FIRE		 180
#define SKILL_FORAGE			 181
#define SPELL_CREATE_SPRING		 182
#define SPELL_LEVIATHAN          183
#define SPELL_METEOR_SWARM		 184
#define SPELL_BEAST_TRANSFORM    185
#define SPELL_TSUNAMI			 186
#define SPELL_TORNADO			 187
#define SPELL_LANDSLIDE          188
#define SKILL_STAB				 189
#define SPELL_CHARM_ANIMAL		 190
#define SKILL_SWITCH_OPPONENT	 191
#define SPELL_ACID_BLAST		 192
#define SPELL_DISPLACEMENT		 193
#define SPELL_SUMMON_ELEMENTAL	 194
#define SPELL_SHOCKWAVE 		 195
#define SPELL_GUST      		 196
#define SPELL_GRANITE_FIST		 197
#define SPELL_PRAYER 			 198
#define SPELL_ELEMENTAL_SHIELD	 199
#define SKILL_TRIP				 200
#define SKILL_TAUNT				 201
#define SKILL_CHARGE			 202
#define SKILL_AIRWALK            203
#define SPELL_HOLY_HEALING       204
#define SPELL_LIFETAP            205
#define SPELL_ENCHANT_ARMOR      206
#define SPELL_VEX                207
#define SPELL_SANCTUARY_MINOR    208
#define SPELL_SANCTUARY_MEDIUM   209
#define SPELL_TRANSFORM_WOMBLE   210
#define SPELL_GUSHER             211
#define SPELL_TRANSFORM_MANTICORE 212
#define SPELL_SLEEP_IMMUNITY      213
#define SKILL_ENVENOM_WEAPON      214
#define SPELL_TRANSFORM_CHIMERA   215
#define SKILL_TRACK               216
#define SPELL_TRANSFORM_COW		  217
#define SPELL_CONVALESCE		  218
#define SPELL_PURGE_MAGIC		  219
#define SKILL_FIFTH				  220


/* VERY IMPORTANT!!!!  This next define must be the same as the last skill/spell!!!
otherwise, it will be very easy to crash the mud using forget!!!
*/
#define NUM_SKILLS                      220

/* Be sure to update spell_names & spell_wear_off_msg in constants.c */
/* oh oh... will conflict with spells */
/* weapon types and spell types use common damage messages */

#define TYPE_HIT                     100
#define TYPE_BLUDGEON                101
#define TYPE_PIERCE                  102
#define TYPE_SLASH                   103
#define TYPE_WHIP                    104
#define TYPE_CLAW                    105
#define TYPE_BITE                    106
#define TYPE_STING                   107
#define TYPE_CRUSH                   108
#define TYPE_ARROW	                 109
#define TYPE_DUALWIELD		         110
#define TYPE_SHIELDSHOCK             201
#define TYPE_SUFFERING               200
#define TYPE_OTHER					 202
/* More anything but spells and weapontypes can be insterted here! */

#define MAX_TYPES 70

#define ENABLED_SPELL    BIT1
#define MOB_SPELL        BIT2
#define MOB_SKILL        BIT3
#define VIOLENT_SPELL    BIT4
#define VIOLENT_SKILL    BIT5
#define DEFENSIVE_SPELL  BIT6
#define MOVEMENT_SPELL   BIT7
#define GENERAL_SPELL    BIT8
#define HEAL_SPELL       BIT9
#define HELPFUL_SPELL    BIT10

#define SAVING_PARA   0
#define SAVING_ROD    1
#define SAVING_PETRI  2
#define SAVING_BREATH 3
#define SAVING_SPELL  4


#define TELEPORT_WORKED        BIT0
#define TELEPORT_FAILED_NORMAL BIT1
#define TELEPORT_FAILED_ZCORD  BIT2
#define TELEPORT_FAILED_MAJOR  BIT3

#define TAR_IGNORE        BIT0
#define TAR_CHAR_ROOM     BIT1
#define TAR_CHAR_WORLD    BIT2
#define TAR_FIGHT_SELF    BIT3
#define TAR_FIGHT_VICT    BIT4
#define TAR_SELF_ONLY     BIT5 /*Check, use with either TAR_CHAR_ROOM */
#define TAR_SELF_NONO     BIT6 /*Check, use with either TAR_CHAR_ROOM */
#define TAR_OBJ_INV       BIT7
#define TAR_OBJ_ROOM      BIT8
#define TAR_OBJ_WORLD     BIT9
#define TAR_OBJ_EQUIP     BIT10
#define TAR_CHAR_ZONE     BIT11

struct spell_info_type {
	void (*spell_pointer) (signed char level, struct char_data *ch, char *arg, int type,
												 struct char_data *tar_ch, struct obj_data *tar_obj);
	signed char minimum_position;   /* Position for caster             */
	sh_int min_usesmana;            /* Amount of mana used by a spell  */
	signed char beats;              /* Heartbeats until ready for next */
	unsigned long spell_flag;       /* VIOLENT/FRIENDLY/GENERAL        */
	sh_int targets;                 /* See below for use with TAR_XXX  */
	/* Modifier, location, and bitvector are     */ 
	/* used for automatic spells based on race   */
	/* if all three are zero spell isn't allowed */
	int    modifier;     /* what the spell modifies.                  */ 
	int    location;     /* how much the spell affects the location.  */
	int    bitvector;    /* the bit set when the spell is in affect.  */
};

/* Possible Targets:

bit 0 : IGNORE TARGET
bit 1 : PC/NPC in room
bit 2 : PC/NPC in world
bit 3 : Object held
bit 4 : Object in inventory
bit 5 : Object in room
bit 6 : Object in world
bit 7 : If fighting, and no argument, select tar_char as self
bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
bit 9 : If no argument, select self, if argument check that it IS self.

*/

#define SPELL_TYPE_SPELL   0
#define SPELL_TYPE_POTION  1
#define SPELL_TYPE_WAND    2
#define SPELL_TYPE_STAFF   3
#define SPELL_TYPE_SCROLL  4


/* Attacktypes with grammar */

struct attack_hit_type {
  char *singular;
  char *plural;
};


/* and the two big arrays of course */
extern const char *spell_names[];
extern const char *skill_names[];
extern const int   skill_numbers[];
extern struct spell_info_type spell_info[MAX_SKILLS];
int                    spell_flag[MAX_SKILLS];

/* spell related functions */
extern bool saves_spell(struct char_data *ch, struct char_data *victim, sh_int save_type);

/* the various spells */
SPELL_FN(spell_magic_missile);
CAST_FN (cast_magic_missile);
SPELL_FN(spell_chill_touch);
CAST_FN (cast_chill_touch);
SPELL_FN(spell_burning_hands);
CAST_FN (cast_burning_hands);
SPELL_FN(spell_shocking_grasp);
CAST_FN (cast_shocking_grasp);
SPELL_FN(spell_lightning_bolt);
CAST_FN (cast_lightning_bolt);
SPELL_FN(spell_colour_spray);
CAST_FN (cast_colour_spray);
SPELL_FN(spell_energy_drain);
CAST_FN (cast_energy_drain);
SPELL_FN(spell_fireball);
CAST_FN (cast_fireball);
SPELL_FN(spell_turn_undead);
CAST_FN (cast_turn_undead);
SPELL_FN(spell_tremor);
CAST_FN (cast_tremor);
SPELL_FN(spell_enchant_weapon);
CAST_FN (cast_enchant_weapon);
SPELL_FN(spell_dispel_evil);
CAST_FN (cast_dispel_evil);
SPELL_FN(spell_hail_storm);
CAST_FN (cast_hail_storm);
SPELL_FN(spell_harm);
CAST_FN (cast_harm);
CAST_FN (cast_dispel_magic);
SPELL_FN(spell_dispel_magic);
SPELL_FN(spell_invisibility);
CAST_FN (cast_invisibility);
SPELL_FN(spell_detect_good);
CAST_FN (cast_detect_good);
SPELL_FN(spell_conflagration);
CAST_FN (cast_conflagration);
SPELL_FN(spell_armor);
CAST_FN (cast_armor);
SPELL_FN(spell_elsewhere);
CAST_FN (cast_elsewhere);
SPELL_FN(spell_bless);
CAST_FN (cast_bless);
SPELL_FN(spell_blindness);
CAST_FN (cast_blindness);
SPELL_FN(spell_control_weather);
CAST_FN (cast_control_weather);
SPELL_FN(spell_create_food);
CAST_FN (cast_create_food);
SPELL_FN(spell_sustenance);
CAST_FN (cast_sustenance);
SPELL_FN(spell_create_water);
CAST_FN (cast_create_water);
SPELL_FN(spell_cure_blind);
CAST_FN (cast_cure_blind);
SPELL_FN(spell_cure_critic);
CAST_FN (cast_cure_critic);
SPELL_FN(spell_donate_mana);
CAST_FN (cast_donate_mana);
SPELL_FN(spell_mana_link);
CAST_FN (cast_mana_link);
SPELL_FN(spell_cause_critic);
CAST_FN (cast_cause_critic);
SPELL_FN(spell_cure_light);
CAST_FN (cast_cure_light);
SPELL_FN(spell_cause_light);
CAST_FN (cast_cause_light);
SPELL_FN(spell_curse);
CAST_FN (cast_curse);
SPELL_FN(spell_detect_evil);
CAST_FN (cast_detect_evil);
SPELL_FN(spell_detect_invisibility);
CAST_FN (cast_detect_invisibility);
SPELL_FN(spell_detect_magic);
CAST_FN (cast_detect_magic);
SPELL_FN(spell_detect_poison);
CAST_FN (cast_detect_poison);
SPELL_FN(spell_improved_invis);
CAST_FN (cast_improved_invis);

SPELL_FN(spell_locate);
CAST_FN (cast_locate);
extern void spell_locate_object(sbyte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj, char *lv_name);

SPELL_FN(spell_poison);
CAST_FN (cast_poison);
SPELL_FN(spell_protection_from_evil);
CAST_FN (cast_protection_from_evil);
SPELL_FN(spell_remove_curse);
CAST_FN (cast_remove_curse);
SPELL_FN(spell_remove_poison);
CAST_FN (cast_remove_poison);
SPELL_FN(spell_sanctuary);
CAST_FN (cast_sanctuary);
SPELL_FN(spell_sleep);
CAST_FN (cast_sleep);
SPELL_FN(spell_strength);
CAST_FN (cast_strength);
SPELL_FN(spell_ventriloquate);
CAST_FN (cast_ventriloquate);
SPELL_FN(spell_word_of_recall);
CAST_FN (cast_word_of_recall);
SPELL_FN(spell_summon);
CAST_FN (cast_summon);
SPELL_FN(spell_succor);
CAST_FN (cast_succor);
SPELL_FN(spell_charm_person);
CAST_FN (cast_charm_person);
SPELL_FN(spell_sense_life);
CAST_FN (cast_sense_life);
SPELL_FN(spell_identify);
CAST_FN (cast_identify);
SPELL_FN(spell_fire_breath);
CAST_FN (cast_fire_breath);
SPELL_FN(spell_frost_breath);
CAST_FN (cast_frost_breath);
SPELL_FN(spell_acid_breath);
CAST_FN (cast_acid_breath);
SPELL_FN(spell_gas_breath);
CAST_FN (cast_gas_breath);
SPELL_FN(spell_lightning_breath);
CAST_FN (cast_lightning_breath);
SPELL_FN(spell_darksight);
CAST_FN (cast_darksight);
SPELL_FN(spell_restoration);
CAST_FN (cast_restoration);
SPELL_FN(spell_regeneration);
SPELL_FN(spell_heal_minor);
CAST_FN (cast_heal_minor);
SPELL_FN(spell_heal_medium);
CAST_FN (cast_heal_medium);
SPELL_FN(spell_heal_major);
CAST_FN (cast_heal_major);
CAST_FN (cast_regeneration);
SPELL_FN(spell_magic_resist);
CAST_FN (cast_magic_resist);
SPELL_FN(spell_magic_immune);
CAST_FN (cast_magic_immune);
SPELL_FN(spell_breathwater);
CAST_FN (cast_breathwater);
SPELL_FN(spell_hold_person);
CAST_FN (cast_hold_person);
SPELL_FN(spell_knock);
CAST_FN (cast_knock);
SPELL_FN(spell_phase_door);
CAST_FN (cast_phase_door);
SPELL_FN(spell_waterwalk);
CAST_FN (cast_waterwalk);
SPELL_FN(spell_teleport_self);
CAST_FN (cast_teleport_self);
SPELL_FN(spell_teleport_group);
CAST_FN (cast_teleport_group);
SPELL_FN(spell_teleview_minor);
CAST_FN (cast_teleview_minor);
SPELL_FN(spell_teleview_major);
CAST_FN (cast_teleview_major);
SPELL_FN(spell_haste);
CAST_FN (cast_haste);
SPELL_FN(spell_dreamsight);
CAST_FN (cast_dreamsight);
SPELL_FN(spell_gate);
CAST_FN (cast_gate);
SPELL_FN(spell_vitalize_mana);
CAST_FN (cast_vitalize_mana);
SPELL_FN(spell_vitalize_hit);
CAST_FN (cast_vitalize_hit);
SPELL_FN(spell_vigorize_minor);
CAST_FN (cast_vigorize_minor);
SPELL_FN(spell_vigorize_medium);
CAST_FN (cast_vigorize_medium);
SPELL_FN(spell_vigorize_major);
CAST_FN (cast_vigorize_major);
SPELL_FN(spell_aid);
CAST_FN (cast_aid);
SPELL_FN(spell_silence);
CAST_FN (cast_silence);
SPELL_FN(spell_dispel_silence);
CAST_FN (cast_dispel_silence);
SPELL_FN(spell_dispel_hold);
CAST_FN (cast_dispel_hold);
SPELL_FN(spell_block_summon);
CAST_FN (cast_block_summon);
SPELL_FN(spell_smite);
CAST_FN (cast_smite);
SPELL_FN(spell_animate_dead);
CAST_FN (cast_animate_dead);
SPELL_FN(spell_black_burst);
CAST_FN (cast_black_burst);
SPELL_FN(spell_cremation);
CAST_FN (cast_cremation);
SPELL_FN(spell_mortice_flame);
CAST_FN (cast_mortice_flame);
SPELL_FN(spell_firelance);
CAST_FN (cast_firelance);
SPELL_FN(spell_firestorm);
CAST_FN (cast_firestorm);
SPELL_FN(spell_earthquake);
CAST_FN (cast_earthquake);
SPELL_FN(spell_divine_retribution);
CAST_FN (cast_divine_retribution);
SPELL_FN(spell_mirror_self);
CAST_FN (cast_mirror_self);
SPELL_FN(spell_recharge_light);
CAST_FN (cast_recharge_light);
SPELL_FN(spell_recharge_wand);
CAST_FN (cast_recharge_wand);
SPELL_FN(spell_venom_blade);
CAST_FN (cast_venom_blade);
SPELL_FN(spell_irresistable_dance);
CAST_FN (cast_irresistable_dance);
SPELL_FN(spell_wizard_lock);
CAST_FN (cast_wizard_lock);
SPELL_FN(spell_dancing_sword);
CAST_FN (cast_dancing_sword);
SPELL_FN(spell_demonic_aid);
CAST_FN (cast_demonic_aid);
SPELL_FN(spell_grangorns_curse);
CAST_FN (cast_grangorns_curse);
SPELL_FN(spell_clawed_hands);
CAST_FN (cast_clawed_hands);
SPELL_FN(spell_detect_animals);
CAST_FN (cast_detect_animals);
SPELL_FN(spell_intelligence);
CAST_FN (cast_intelligence);
SPELL_FN(spell_wisdom);
CAST_FN (cast_wisdom);
SPELL_FN(spell_dexterity);
CAST_FN (cast_dexterity);
SPELL_FN(spell_constitution);
CAST_FN (cast_constitution);
SPELL_FN(spell_charisma);
CAST_FN (cast_charisma);
SPELL_FN(spell_portal);
CAST_FN (cast_portal);
SPELL_FN(spell_hemmorage);
CAST_FN (cast_hemmorage);
SPELL_FN(spell_fear);
CAST_FN (cast_fear);
SPELL_FN(spell_demon_flesh);
CAST_FN (cast_demon_flesh);
SPELL_FN(spell_bind_souls);
CAST_FN(cast_bind_souls);
SPELL_FN(spell_pestilence);
CAST_FN(cast_pestilence);
SPELL_FN(spell_call_lightning);
CAST_FN(cast_call_lightning);
SPELL_FN(spell_fly);
CAST_FN(cast_fly);
SPELL_FN(spell_spiritual_transfer);
CAST_FN(cast_spiritual_transfer);
SPELL_FN(spell_mordenkainens_sword);
CAST_FN(cast_mordenkainens_sword);
SPELL_FN(spell_transform_wolf);
CAST_FN(cast_transform_wolf);
SPELL_FN(spell_transform_bear);
CAST_FN(cast_transform_bear);
SPELL_FN(spell_transform_dragon);
CAST_FN(cast_transform_dragon);
SPELL_FN(spell_transform_monkey);
CAST_FN(cast_transform_monkey);
SPELL_FN(spell_transform_ninja);
CAST_FN(cast_transform_ninja);
SPELL_FN(spell_detect_giants);
CAST_FN(cast_detect_giants);
SPELL_FN(spell_faerie_fog);
CAST_FN(cast_faerie_fog);
SPELL_FN(spell_detect_shadows);
CAST_FN(cast_detect_shadows);
SPELL_FN(spell_barkskin);
CAST_FN(cast_barkskin);
SPELL_FN(spell_stoneskin);
CAST_FN(cast_stoneskin);
SPELL_FN(spell_thornwrack);
CAST_FN(cast_thornwrack);
SPELL_FN(spell_detect_demons);
CAST_FN(cast_detect_demons);
SPELL_FN(spell_detect_dragons);
CAST_FN(cast_detect_dragons);
SPELL_FN(spell_detect_undead);
CAST_FN(cast_detect_undead);
SPELL_FN(spell_animal_summoning);
CAST_FN(cast_animal_summoning);
SPELL_FN(spell_detect_vampires);
CAST_FN(cast_detect_vampires);
SPELL_FN(spell_earthmaw);
CAST_FN(cast_earthmaw);
SPELL_FN(spell_needlestorm);
CAST_FN(cast_needlestorm);
SPELL_FN(spell_treewalk);
CAST_FN(cast_treewalk);
SPELL_FN(spell_summon_treant);
CAST_FN(cast_summon_treant);
SPELL_FN(spell_sporecloud);
CAST_FN(cast_sporecloud);
SPELL_FN(spell_wrath_of_nature);
CAST_FN(cast_wrath_of_nature);
SPELL_FN(spell_faerie_fire);
CAST_FN(cast_faerie_fire);
SPELL_FN(spell_create_spring);
CAST_FN(cast_create_spring);
SPELL_FN(spell_leviathan);
CAST_FN(cast_leviathan);
SPELL_FN(spell_meteor_swarm);
CAST_FN(cast_meteor_swarm);
SPELL_FN(spell_beast_transform);
CAST_FN(cast_beast_transform);
SPELL_FN(spell_create_spring);
CAST_FN(cast_create_spring);
SPELL_FN(spell_tsunami);
CAST_FN(cast_tsunami);
SPELL_FN(spell_tornado);
CAST_FN(cast_tornado);
SPELL_FN(spell_landslide);
CAST_FN(cast_landslide);
SPELL_FN(spell_charm_animal);
CAST_FN(cast_charm_animal);
SPELL_FN(spell_acid_blast);
CAST_FN(cast_acid_blast);
SPELL_FN(spell_displacement);
CAST_FN(cast_displacement);
SPELL_FN(spell_summon_elemental);
CAST_FN(cast_summon_elemental);
SPELL_FN(spell_shockwave);
CAST_FN(cast_shockwave);
SPELL_FN(spell_gust);
CAST_FN(cast_gust);
SPELL_FN(spell_granite_fist);
CAST_FN(cast_granite_fist);
SPELL_FN(spell_prayer);
CAST_FN(cast_prayer);
SPELL_FN(spell_elemental_shield);
CAST_FN(cast_elemental_shield);
SPELL_FN(spell_holy_healing);
CAST_FN(cast_holy_healing);
SPELL_FN(spell_lifetap);
CAST_FN(cast_lifetap);
SPELL_FN(spell_enchant_armor);
CAST_FN(cast_enchant_armor);
SPELL_FN(spell_vex);
CAST_FN (cast_vex);
SPELL_FN(spell_sanctuary_minor);
CAST_FN(cast_sanctuary_minor);
SPELL_FN(spell_sanctuary_medium);
CAST_FN(cast_sanctuary_medium);
SPELL_FN(spell_transform_womble);
CAST_FN(cast_transform_womble);
SPELL_FN(spell_gusher);
CAST_FN(cast_gusher);
SPELL_FN(spell_transform_manticore);
CAST_FN(cast_transform_manticore);
SPELL_FN(spell_sleep_immunity);
CAST_FN(cast_sleep_immunity);
SPELL_FN(spell_transform_chimera);
CAST_FN(cast_transform_chimera);
SPELL_FN(spell_transform_cow);
CAST_FN(cast_transform_cow);
SPELL_FN(spell_convalesce);
CAST_FN (cast_convalesce);
SPELL_FN(spell_purge_magic);
CAST_FN(cast_purge_magic);