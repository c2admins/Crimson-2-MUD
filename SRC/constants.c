/* gv_location: 3501-4000 */
/* *******************************************************************
*  file: constants.c                                 Part of DIKUMUD *
*  Usage: For constants used by the game.                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.   *
******************************************************************** */

#include "structs.h"
#include "limits.h"
#include "constants.h"
#include "utility.h"
#include "spells.h"
#include "ansi.h"

const char *rank_color[] = {
	GRAY,
	YELLOW,
	WHITE,
	LCYAN,
	LGREEN,
	LBLUE,
	LPURPLE,
	CYAN,
	GREEN,
	BLUE,
	RED,
	PURPLE,
};

const char *ban_types[] = {
	"no",
	"new",
	"select",
	"all",
	"ERROR"
};

const int skill_numbers[] = {
	45, 46, 47,
	48, 49, 50,
	51, 52, 56,
	57, 58, 59,
	60, 61, 62, 63,
	64, 65, 66, 67,
	69, 70, 71,
	89,
	136, 137, 138, 143,
	144, 145, 146, 147,
	148, 155, 156, 181,
	189, 191, 200, 201,
	202, 203, 214, 216,220,
	0
};

const char *skill_names[] = {
	"sneak",		/* 45 */
	"hide",			/* 46 */
	"steal",		/* 47 */

	"backstab",		/* 48 */
	"picklock",		/* 49 */
	"kick",			/* 50 */

	"bash",			/* 51 */
	"rescue",		/* 52 */
	"extra damage",		/* 56 */

	"slashing weapons",	/* 57 */
	"piercing weapons",	/* 58 */
	"bludgeoning weapons",	/* 59 */

	"parry death blow",	/* 60 */
	"dodge",		/* 61 */
	"tougher scales",	/* 62 */

	"fire breath",		/* 64 */
	"gas breath",		/* 64 */
	"frost breath",		/* 65 */
	"acid breath",		/* 66 */
	"lightning breath",	/* 67 */

	"second attack",	/* 69 */
	"third attack",		/* 70 */
	"fourth attack",	/* 71 */

	"hunt",			/* 89 */

	"assess",		/* 136 */
	"rage",			/* 137 */
	"switch",		/* 138 */

	"din mak",		/* 143 */
	"tsugiashi",		/* 144 */
	"meditation",		/* 145 */
	"sense danger",		/* 146 */
	"berserk",		/* 147 */
	"parry",		/* 148 */
	"escape",		/* 155 */
	"peek",			/* 156 */

	"forage",		/* 181 */
	"stab",			/* 189 */
	"switch opponent",	/* 191 */
	"circle attack",	/* 200 */
	"taunt",		/* 201 */
	"charge",		/* 202 */
	"airwalk",		/* 203 */
	"envenom weapon",	/* 214 */
	"track",		/* 216 */
	"fifth attack", /* 220 */
	"\n"
};


const char *spell_names[] = {
	"armor",		/* 1 */
	"elsewhere",
	"bless",
	"blindness",
	"burning hands",
	"hail storm",
	"charm person",
	"chill touch",
	"improved invisibility",
	"colour spray",
	"control weather",	/* 11 */
	"create food",
	"create water",
	"cure blind",
	"cure critic",
	"cure light",
	"curse",
	"detect evil",
	"detect invisibility",
	"detect magic",
	"detect poison",	/* 21 */
	"dispel evil",
	"tremor",
	"enchant weapon",
	"energy drain",
	"fireball",
	"harm",
	"dispel magic",
	"invisibility",
	"lightning bolt",
	"locate object",	/* 31 */
	"magic missile",
	"poison",
	"protection from evil",
	"remove curse",
	"sanctuary major",
	"shocking grasp",
	"sleep",
	"strength",
	"summon",
	"ventriloquate",	/* 41 */
	"word of recall",
	"remove poison",
	"sense life",		/* 44 */
	"sneak",		/* 45 */
	"hide",
	"steal",
	"backstab",
	"pick_lock",
	"kick",			/* 50 */
	"bash",
	"rescue",
	"identify",		/* 53 */
	"cause critic",
	"cause light",		/* 55 */
	"extra_damage",
	"slash",
	"pierce",
	"bludgeon",
	"parry",		/* 60 */
	"dodge",
	"scales",
	"fire breath",
	"gas breath",
	"frost breath",
	"acid breath",		/* 66 */
	"lightning breath",
	"darksight",
	"second",		/* 69 */
	"third",		/* 70 */
	"fourth",		/* 71 */
	"undetect invis",
	"restoration",
	"regeneration",
	"heal minor",		/* 75 */
	"heal medium",		/* 76 */
	"heal major",		/* 77 */
	"spiritual hammer",	/* 78 */
	"turn undead",		/* 79 */
	"succor",		/* 80 */
	"donate mana",		/* 81 */
	"mana link",		/* 82 */
	"magic resist",		/* 83 */
	"magic immune",		/* 84 */
	"detect good",		/* 85 */
	"conflagration",	/* 86 */
	"breathwater",		/* 87 */
	"sustenance",		/* 88 */
	"hunt",			/* 89 */
	"hold person",		/* 90 */
	"knock",		/* 91 */
	"phase door",		/* 92 */
	"waterwalk",		/* 93 */
	"teleport self",	/* 94 */
	"teleport group",	/* 95 */
	"teleview minor",	/* 96 */
	"teleview major",	/* 97 */
	"haste",		/* 98 */
	"dreamsight",		/* 99 */
	"gate",			/* 100 */
	"vitalize mana",	/* 101 */
	"vitalize hit",		/* 102 */
	"vigorize minor",	/* 103 */
	"vigorize medium",	/* 104 */
	"vigorize major",	/* 105 */
	"aid",			/* 106 */
	"silence",		/* 107 */
	"dispel silence",	/* 108 */
	"dispel hold",		/* 109 */
	"block summon",		/* 110 */
	"smite",		/* 111 */
	"animate dead",		/* 112 */
	"black burst",		/* 113 */
	"cremation",		/* 114 */
	"mortice flame",	/* 115 */
	"firelance",		/* 116 */
	"firestorm",		/* 117 */
	"earthquake",		/* 118 */
	"divine retribution",	/* 119 */
	"mirror self",		/* 120 */
	"recharge light",	/* 121 */
	"recharge wand",	/* 122 */
	"venom blade",		/* 123 */
	"irresitable dance",	/* 124 */
	"wizard lock",		/* 125 */
	"dancing sword",	/* 126 */
	"demonic aid",		/* 127 */
	"grangorns curse",	/* 128 */
	"clawed hands",		/* 129 */
	"detect animals",	/* 130 */
	"intelligence",		/* 131 */
	"wisdom",		/* 132 */
	"dexterity",		/* 133 */
	"constitution",		/* 134 */
	"charisma",		/* 135 */
	"assess",		/* 136 */
	"rage",			/* 137 */
	"switch",		/* 138 */
	"portal",		/* 139 */
	"hemmorage",		/* 140 */
	"fear",			/* 141 */
	"demon flesh",		/* 142 */
	"din mak",		/* 143 */
	"tsugiashi",		/* 144 */
	"meditation",		/* 145 */
	"sense danger",		/* 146 */
	"berserk",		/* 147 */
	"parry",		/* 148 */
	"pestilence",		/* 149 */
	"spirit transfer",	/* 150 */
	"bind souls",		/* 151 */
	"mordenk sword",	/* 152 */
	"call lightning",	/* 153 */
	"fly",			/* 154 */
	"escape",		/* 155 */
	"peek",			/* 156 */
	"deathshadow",		/* 157 */
	"wolf form",		/* 158 */
	"bear form",		/* 159 */
	"dragon form",		/* 160 */
	"monkey form",		/* 161 */
	"ninja form",		/* 162 */
	"chloroplast",		/* 163 */
	"faerie fog",		/* 164 */
	"entangle",		/* 165 */
	"barkskin",		/* 166 */
	"stoneskin",		/* 167 */
	"thornwrack",		/* 168 */
	"calm",			/* 169 */
	"detect dragons",	/* 170 */
	"detect undead",	/* 171 */
	"animal summoning",	/* 172 */
	"pass without trace",	/* 173 */
	"earthmaw",		/* 174 */
	"needlestorm",		/* 175 */
	"treewalk",		/* 176 */
	"summon treant",	/* 177 */
	"spore cloud",		/* 178 */
	"wrath of nature",	/* 179 */
	"faerie fire",		/* 180 */
	"forage",		/* 181 */
	"create spring",	/* 182 */
	"leviathan",		/* 183 */
	"meteor swarm",		/* 184 */
	"beastial transform",	/* 185 */
	"tsunami",		/* 186 */
	"tornado",		/* 187 */
	"landslide",		/* 188 */
	"stab",			/* 189 */
	"charm_animal",		/* 190 */
	"switch opponent",	/* 191 */
	"acid blast",       /* 192 */
	"displacement",		/* 193 */
	"summon elemental",	/* 194 */
	"shockwave",		/* 195 */
	"gust",			/* 196 */
	"granite fist",		/* 197 */
	"prayer",		/* 198 */
	"elemental shield",	/* 199 */
	"circle attack",	/* 200 */
	"taunt",		/* 201 */
	"charge",		/* 202 */
	"airwalk",		/* 203 */
	"holy healing",		/* 204 */
	"lifetap",		/* 205 */
	"enchant armor",	/* 206 */
	"vex",			/* 207 */
	"sanctuary minor",	/* 208 */
	"sanctuary medium",	/* 210 */
	"womble form",		/* 211 */
	"gusher",		/* 212 */
	"manticore form",	/* 213 */
	"sleep immunity",	/* 214 */
	"envenom_weapon",	/* 215 */
	"chimera form",		/* 215 */
	"track",		/* 216 */
	"cow form",		/* 217 */
	"convalesce",		/* 218 */
	"purge magic",      /* 219 */
	"fifth",            /* 220 */
	"\n"			/* we need this to end searching */
};

const char *sk_sp_defines[] = {
	"SPELL_ARMOR",
	"SPELL_ELSEWHERE",
	"SPELL_BLESS",
	"SPELL_BLINDNESS",
	"SPELL_BURNING_HANDS",
	"SPELL_HAIL_STORM",
	"SPELL_CHARM_PERSON",
	"SPELL_CHILL_TOUCH",
	"SPELL_IMPROVED_INVIS",
	"SPELL_COLOUR_SPRAY",	/* 10 */
	"SPELL_CONTROL_WEATHER",
	"SPELL_CREATE_FOOD",
	"SPELL_CREATE_WATER",
	"SPELL_CURE_BLIND",
	"SPELL_CURE_CRITIC",
	"SPELL_CURE_LIGHT",
	"SPELL_CURSE",
	"SPELL_DETECT_EVIL",
	"SPELL_DETECT_INVISIBLE",
	"SPELL_DETECT_MAGIC",	/* 20 */
	"SPELL_DETECT_POISON",
	"SPELL_DISPEL_EVIL",
	"SPELL_TREMOR",
	"SPELL_ENCHANT_WEAPON",
	"SPELL_ENERGY_DRAIN",
	"SPELL_FIREBALL",
	"SPELL_HARM",
	"SPELL_DISPEL_MAGIC",
	"SPELL_INVISIBLE",
	"SPELL_LIGHTNING_BOLT",	/* 30 */
	"SPELL_LOCATE_OBJECT",
	"SPELL_MAGIC_MISSILE",
	"SPELL_POISON",
	"SPELL_PROTECT_FROM_EVIL",
	"SPELL_REMOVE_CURSE",
	"SPELL_SANCTUARY",
	"SPELL_SHOCKING_GRASP",
	"SPELL_SLEEP",
	"SPELL_STRENGTH",
	"SPELL_SUMMON",		/* 40 */
	"SPELL_VENTRILOQUATE",
	"SPELL_WORD_OF_RECALL",
	"SPELL_REMOVE_POISON",
	"SPELL_SENSE_LIFE",
	"SKILL_SNEAK",
	"SKILL_HIDE",
	"SKILL_STEAL",
	"SKILL_BACKSTAB",
	"SKILL_PICK_LOCK",
	"SKILL_KICK",		/* kick */
	"SKILL_BASH",
	"SKILL_RESCUE",
	"SPELL_IDENTIFY",
	"SPELL_CAUSE_CRITIC",
	"SPELL_CAUSE_LIGHT",
	"SKILL_EXTRA_DAMAGE",
	"SKILL_SLASH",
	"SKILL_PIERCE",
	"SKILL_BLUDGEON",
	"SKILL_PARRY",		/* 60 */
	"SKILL_DODGE",
	"SKILL_SCALES",
	"SPELL_FIRE_BREATH",
	"SPELL_GAS_BREATH",
	"SPELL_FROST_BREATH",
	"SPELL_ACID_BREATH",
	"SPELL_LIGHTNING_BREATH",
	"SPELL_DARKSIGHT",
	"SKILL_SECOND",
	"SKILL_THIRD",		/* 70 */
	"SKILL_FOURTH",
	"SPELL_UNDETECT_INVIS",	/* 72 */
	"SPELL_RESTORATION",
	"SPELL_REGENERATION",
	"SPELL_HEAL_MINOR",
	"SPELL_HEAL_MEDIUM",
	"SPELL_HEAL_MAJOR",
	"SPELL_SPIRIT_HAMMER",
	"SPELL_TURN_UNDEAD",
	"SPELL_SUCCOR",		/* 80 */
	"SPELL_DONATE_MANA",
	"SPELL_MANA_LINK",
	"SPELL_MAGIC_RESIST",
	"SPELL_MAGIC_IMMUNE",
	"SPELL_DETECT_GOOD",
	"SPELL_CONFLAGRATION",
	"SPELL_BREATHWATER",
	"SPELL_SUSTENANCE",
	"SKILL_HUNT",
	"SPELL_HOLD_PERSON",	/* 90 */
	"SPELL_KNOCK",
	"SPELL_PHASE_DOOR",
	"SPELL_WATERWALK",
	"SPELL_TELEPORT_SELF",
	"SPELL_TELEPORT_GROUP",
	"SPELL_TELEVIEW_MINOR",
	"SPELL_TELEVIEW_MAJOR",
	"SPELL_HASTE",
	"SPELL_DREAMSIGHT",
	"SPELL_GATE",		/* 100 */
	"SPELL_VITALIZE_MANA",
	"SPELL_VITALIZE_HIT",
	"SPELL_VIGORIZE_MINOR",
	"SPELL_VIGORIZE_MEDIUM",
	"SPELL_VIGORIZE_MAJOR",
	"SPELL_AID",
	"SPELL_SILENCE",
	"SPELL_DISPEL_SILENCE",
	"SPELL_DISPEL_HOLD",
	"SPELL_BLOCK_SUMMON",	/* 110 */
	"SPELL_SMITE",
	"SPELL_ANIMATE_DEAD",
	"SPELL_BLACK_BURST",
	"SPELL_CREMATION",
	"SPELL_MORTICE_FLAME",
	"SPELL_FIRELANCE",
	"SPELL_FIRESTORM",
	"SPELL_EARTHQUAKE",
	"SPELL_DIVINE_RETRIBUTION",
	"SPELL_MIRROR_SELF",	/* 120 */
	"SPELL_RECHARGE_LIGHT",
	"SPELL_RECHARGE_WAND",
	"SPELL_VENOM_BLADE",
	"SPELL_IRRESISTABLE_DANCE",
	"SPELL_WIZARD_LOCK",
	"SPELL_DANCING_SWORD",
	"SPELL_DEMONIC_AID",
	"SPELL_GRANGORNS_CURSE",
	"SPELL_CLAWED_HANDS",
	"SPELL_DETECT_ANIMALS",	/* 130 */
	"SPELL_INTELLIGENCE",
	"SPELL_WISDOM",
	"SPELL_DEXTERITY",
	"SPELL_CONSTITUTION",
	"SPELL_CHARISMA",
	"SKILL_ASSESS",
	"SKILL_RAGE",
	"SKILL_SWITCH",
	"SPELL_PORTAL",
	"SPELL_HEMMORAGE",	/* 140 */
	"SPELL_FEAR",
	"SPELL_DEMON_FLESH",
	"SPELL_DIN_MAK",
	"SKILL_TSUGIASHI",
	"SKILL_MEDITATE",
	"SKILL_SENSE_DANGER",
	"SKILL_BERSERK",
	"SKILL_PARR",
	"SPELL_PESTILENCE",
	"SPELL_SPIRIT_TRANSFER",/* 150 */
	"SPELL_BIND_SOULS",
	"SPELL_MORDEN_SWORD",
	"SPELL_CALL_LIGHTNING",
	"SPELL_FLY",
	"SKILL_ESCAPE",
	"SKILL_PEEK",
	"SPELL_DEATHSHADOW",
	"SPELL_TRANSFORM_WOLF",
	"SPELL_TRANSFORM_BEAR",
	"SPELL_TRANSFORM_DRAGON",
	"SPELL_TRANSFORM_MONKEY",
	"SPELL_TRANSFORM_NINJA",
	"SPELL_CHLOROPLAST",
	"SPELL_FAERIE_FOG",
	"SPELL_ENTANGLE",
	"SPELL_BARKSKIN",
	"SPELL_STONESKIN",
	"SPELL_THORNWRACK",
	"SPELL_CALM",
	"SPELL_DETECT_DRAGONS",
	"SPELL_DETECT_UNDEAD",
	"SPELL_ANIMAL_SUMMONING",
	"SPELL_PASS_WITHOUT_TRACE",
	"SPELL_EARTHMAW",
	"SPELL_NEEDLESTORM",
	"SPELL_TREEWALK",
	"SPELL_SUMMON_TREANT",
	"SPELL_SPORECLOUD",
	"SPELL_WRATH_OF_NATURE",
	"SPELL_FAERIE_FIRE",
	"SKILL_FORAGE",
	"SPELL_CREATE_SPRING",
	"SPELL_LEVIATHAN",
	"SPELL_METEOR_SWARM",
	"SPELL_BEAST_TRANSFORM",
	"SPELL_TSUNAMI",
	"SPELL_TORNADO",
	"SPELL_LANDSLIDE",
	"SKILL_STAB",
	"SPELL_CHARM_ANIMAL",
	"SKILL_SWITCH_OPPONENT",
	"SPELL_ACID_BLAST",
	"SPELL_DISPLACEMENT",
	"SPELL_SUMMON_ELEMENTAL",
	"SPELL_SHOCKWAVE",
	"SPELL_GUST",
	"SPELL_GRANITE_FIST",
	"SPELL_PRAYER",
	"SPELL_ELEMENTAL_SHIELD",
	"SKILL_CIRCLE_ATTACK",
	"SKILL_TAUNT",
	"SKILL_CHARGE",
	"SKILL_AIRWALK",
	"SPELL_HOLY_HEALING",
	"SPELL_LIFETAP",
	"SPELL_ENCHANT_ARMOR",
	"SPELL_VEX",
	"SPELL_SANCTUARY_MINOR",
	"SPELL_SANCTUARY_MEDIUM",
	"SPELL_WOMBLE_FORM",
	"SPELL_GUSHER",
	"SPELL_TRANSFORM_MANTICORE",
	"SPELL_SLEEP_IMMUNITY",
	"SKILL_ENVENOM_WEAPON",
	"SPELL_TRANSFORM_CHIMERA",
	"SKILL_TRACK",
	"SPELL_TRANSFORM_COW",
	"SPELL_CONVALESCE",
	"SPELL_PURGE_MAGIC",
	"SKILL_FIFTH",
	"\n"
};

const char *spell_wear_off_msg[] = {
	"RESERVED DB.C",
	"You feel less protected.",
	"!Elsewhere!",
	"You feel less righteous.",
	"You feel a cloak of blindness disolve.",
	"!Burning Hands!",
	"!Call Lightning",
	"You feel more self-confident.",
	"!Chill Touch!",
	"You feel yourself exposed.",
	"!Color Spray!",
	"!Control Weather!",
	"!Create Food!",
	"!Create Water!",
	"!Cure Blind!",
	"!Cure Critic!",
	"!Cure Light!",
	"You feel better.",
	"You sense the red in your vision disappear.",
	"The detect invisible wears off.",
	"The detect magic wears off.",
	"The detect poison wears off.",
	"!Dispel Evil!",
	"!Earthquake!",
	"!Enchant Weapon!",
	"!Energy Drain!",
	"!Fireball!",
	"!Harm!",
	"!Dispel Magic!",
	"You feel yourself exposed!",
	"!Lightning Bolt!",
	"!Locate object!",
	"!Magic Missile!",
	"You feel less sick.",
	"You feel less protected.",
	"!Remove Curse!",
	"The white aura around your body fades.",
	"!Shocking Grasp!",
	"You feel less drousy.",
	"You don't feel as strong.",
	"!Summon!",
	"!Ventriloquate!",
	"!Word of Recall!",
	"!Remove Poison!",
	"You feel less aware of your suroundings.",
	"!sneak!",
	"!Hide!",
	"!Steal!",
	"!Backstab!",
	"!Pick Lock!",
	"!Kick!",
	"!Bash!",
	"!Rescue!",
	"!identify!",
	"!Cause Critic!",
	"!Cause light!",
	"!Extra Damage!",
	"!slash!",
	"!pierce!",
	"!bludgeon!",
	"!parry!",
	"Whew you're too tired to dodge any more...",
	"!scales!",
	"!breath fire!",
	"!breath gas!",
	"!breath frost!",
	"!breath acid!",
	"!breath lightning!",
	"Things suddenly seem a lot darker now.",
	"!Second!",
	"!Third!",
	"!Fourth!",
	"blind souls!",		/* 72 */
	"!restoration!",
	"You feel your recuperative powers reduced to normal.",
	"!Heal minor!",
	"!Heal medium!",
	"!Heal major!",
	"Your phantasmal hammer winks out of existence.",
	"!turn undead!",
	"!succor!",
	"!donate mana!",
	"You feel your magical link suddenly break...",
	"You feel more suseptible to magic...",
	"You feel more susceptible to magic...",
	"Its harder to tell the nicer things in life",
	"!conflagration!",
	"You begin to breath normally...",
	"!sustenance!",
	"!hunt!",
	"You can move again.",
	"!Knock!",
	"!Phase door!",
	"Water doesn't look as solid anymore.",
	"!teleport self!",
	"!teleport group!",
	"!teleview minor!",
	"!teleview major!",
	"You feel slower.",
	"You could sleep soundly now.",
	"!gate!",
	"!vitalize mana!",
	"!vitalize hit!",
	"!vigorize minor!",
	"!vigorize medium!",
	"!vigorize major!",
	"You don't feel as healthy.",
	"You can speak again!",
	"!dispel silence!",
	"!dispel holdperson!",
	"!block summon!",
	"!smite!",
	"!animate dead!",
	"!black burst!",	/* 113 */
	"!cremation!",		/* 114 */
	"!mortice flame!",	/* 115 */
	"!fireland!",		/* 116 */
	"!firestorm!",		/* 117 */
	"!earthquake!",		/* 118 */
	"!divine retribution!",	/* 119 */
	"!mirror self",		/* 120 */
	"!recharge light",	/* 121 */
	"!recharge wand",	/* 122 */
	"!venom blade",		/* 123 */
	"!irresistable dance",	/* 124 */
	"!wizard lock",		/* 125 */
	"!dancing sword",	/* 126 */
	"!demonic aid",		/* 127 */
	"!grangorns curse",	/* 128 */
	"!clawed hands",	/* 129 */
	"You can no longer sense animals.",		/* 130 */
	"You don't feel as smart.",	/* 131 */
	"You don't feel as wise.",	/* 132 */
	"You don't feel as nimble.",	/* 133 */
	"You don't feel as healthy.",	/* 134 */
	"You don't feel as attractive.",	/* 135 */
	"!assess",		/* 136 */
	"!rage",		/* 137 */
	"!switch",		/* 138 */
	"!portal",		/* 139 */
	"You feel reliefed as the hemmorage leaves your body",	/* 140 */
	"fear",			/* 141 */
	"Your body returns to normal, leaving you feeling drained",	/* 142 */
	"din mak",		/* 143 */
	"tsugiashi",		/* 144 */
	"meditation",		/* 145 */
	"sense danger",		/* 146 */
	"berserk",		/* 147 */
	"parry",		/* 148 */
	"pestilence",		/* 149 */
	"spiritual transfer",	/* 150 */
	"The souls you are bound too have been released.",	/* 151 */
	"!mordenkainens sword!",/* 152 */
	"!call lightning!",	/* 153 */
	"You float to the ground and go THUD!",	/* 154 */
	"!escape!",		/* 155 */
	"!peek!",		/* 156 */
	"!death shadow!",	/* 157 */
	"You are no longer a wolf.",	/* 158 */
	"You are no longer a bear.",	/* 159 */
	"You are no longer a dragon.",	/* 160 */
	"You are no longer a monkey.",	/* 161 */
	"You are no longer a ninja.",	/* 162 */
	"The no longer regenerate.",
	"The purple haze clinging to your body suddenly dissipates.",
	"!SPELL_ENTANGLE!",
	"Your skin feels less like bark",
	"Your skins loses its stone-like qualities.",
	"!SPELL_THORNWRACK!",
	"You feel less calm.",
	"You can no longer sense dragons.",
	"You can no longer sense undead.",
	"!SPELL_ANIMAL_SUMMONING!",
	"!SPELL_PASS_WITHOUT_TRACE!",
	"!SPELL_EARTHMAW!",
	"!SPELL_NEEDLESTORM!",
	"!SPELL_TREEWALK!",
	"!SPELL_SUMMON_TREANT!",
	"!SPELL_SPORECLOUD!",
	"!SPELL_WRATH_OF_NATURE!",
	"The pink aura around your body fades.",
	"!FORAGE!",
	"!CREATE SPRING!",
	"Your leviathan departs.",
	"!METEOR SWARM!",
	"Your body returns to normal as the transformation wears off.",
	"!TSUNAMI!",
	"!TORNADO!",
	"!LANDSLIDE!",
	"!STAB!",
	"!SPELL_CHARM_ANIMAL!",
	"!SWITCH OPPONENTS!",
	"!SPELL_ACID BLAST!",
	"You feel as if you are no longer displaced.",
	"!SPELL_SUMMON_ELEMENTAL!",
	"!SPELL_SHOCKWAVE!",
	"!SPELL_GUST!",
	"!SPELL_GRANITE_FIST!",
	"The affects of your prayer wear off.",
	"You are no longer protected from the elements.",
	"!SKILL_CIRCLE_ATTACK!",/* 200 */
	"!SKILL_TAUNT!",	/* 201 */
	"!SKILL_CHARGE!",   /* 202 */
	"Your tiny wings are stressed out and you float to the ground.",	/* 203 */
	"!SPELL_HOLY_HEALING!",	/* 204 */
	"!SPELL_LIFETAP!",	/* 205 */
	"!SPELL_ENCHANT_ARMOR!",/* 206 */
	"Your inner vexation tempers and dissipates.",	/* 207 */
	"Your sanctuary wears off",	/* 208 */
	"Your sanctuary wears off",	/* 209 */
	"You are no longer a womble.",	/* 210 */
	"!gusher!",		/* 211 */
	"You are no longer a manticore.",	/* 212 */
	"You lose your immunity to sleep",	/* 213 */
	"!SKILL_ENVENOM_WEAPON!",	/* 214 */
	"You are no longer a chimera.",	/* 215 */
	"!track!",		/* 217 */
	"You moo no more.",	/* 217 */
	"!Convolesence",	/* 218 */
	"!Purge Magic!",	/* 219 */
	"!Fifth",           /* 220*/
	"\n",
};

const int reverse_dir[] = {2, 3, 0, 1, 5, 4, 8, 9, 6, 7};

const int movement_loss[] = {
	1,			/* Inside     */
	2,			/* City       */
	2,			/* Field      */
	3,			/* Forest     */
	4,			/* Hills      */
	6,			/* Mountains  */
	4,			/* Swimming   */
	1,			/* Unswimable */
	6			/* underwater */
};
const char *dirs[] = {
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"northeast",
	"southeast",
	"southwest",
	"northwest",
	"\n"
};

const char *weekdays[7] = {
	"the Day of the Moon",
	"the Day of the Bull",
	"the Day of the Deception",
	"the Day of Thunder",
	"the Day of Freedom",
	"the day of the Great Gods",
"the Day of the Sun"};

const char *month_name[17] = {
	"Month of Winter",	/* 0 */
	"Month of the Winter Wolf",
	"Month of the Frost Giant",
	"Month of the Old Forces",
	"Month of the Grand Struggle",
	"Month of the Spring",
	"Month of Nature",
	"Month of Futility",
	"Month of the Dragon",
	"Month of the Sun",
	"Month of the Heat",
	"Month of the Battle",
	"Month of the Dark Shades",
	"Month of the Shadows",
	"Month of the Long Shadows",
	"Month of the Ancient Darkness",
	"Month of the Great Evil"
};

const int sharp[] = {
	0,
	0,
	0,
	1,			/* Slashing */
	0,
	0,
	0,
	0,			/* Bludgeon */
	0,
	0,
	0,
0};				/* Pierce   */

const char *where[] = {
	"<used as light>      ",
	"<worn on rfinger>    ",
	"<worn on lfinger>    ",
	"<worn around neck>   ",
	"<worn around neck>   ",
	"<worn on body>       ",
	"<worn on head>       ",
	"<worn on legs>       ",
	"<worn on feet>       ",
	"<worn on hands>      ",
	"<worn on arms>       ",
	"<worn as shield>     ",
	"<worn about body>    ",
	"<worn about waist>   ",
	"<worn on rwrist>     ",
	"<worn on lwrist>     ",
	"<wielded>            ",
	"<held>               ",
	"<tail>               ",
	"<worn on front legs> ",
	"<worn on hind legs>  ",
};


const char *loc[] = {		/* mirrors wear */
	"light",
	"lfinger",
	"rfinger",
	"neck1",
	"neck2",
	"body",
	"head",
	"legs",
	"feet",
	"hands",
	"arms",
	"shield",
	"mbody",
	"waist",
	"lwrist",
	"rwrist",
	"wield",
	"held",
	"tail",
	"4legs",
	"\n"
};


const char *drinks[] = {
	"water",
	"beer",
	"wine",
	"ale",
	"dark ale",
	"whiskey",
	"lemonade",
	"firebreather",
	"local speciality",
	"slime mold juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt water",
	"Dr Pepper",
	"\n"
};

const char *drinknames[] = {
	"water",
	"beer",
	"wine",
	"ale",
	"ale",
	"whiskey",
	"lemonade",
	"firebreather",
	"local",
	"juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt",
	"Dr Pepper"
	"\n"
};

const int drink_aff[][3] = {
	{0, 1, 10},		/* Water    */
	{3, 2, 5},		/* beer     */
	{5, 2, 5},		/* wine     */
	{2, 2, 5},		/* ale      */
	{1, 2, 5},		/* ale      */
	{6, 1, 4},		/* Whiskey  */
	{0, 1, 8},		/* lemonade */
	{10, 0, 0},		/* firebr   */
	{3, 3, 3},		/* local    */
	{0, 4, -8},		/* juice    */
	{0, 3, 6},
	{0, 1, 6},
	{0, 1, 6},
	{0, 2, -1},
	{0, 1, -2},
	{0, 1, 5}
};

const char *color_liquid[] = {
	"clear",
	"brown",
	"clear",
	"brown",
	"dark",
	"golden",
	"red",
	"green",
	"clear",
	"light green",
	"white",
	"brown",
	"black",
	"red",
	"clear",
	"black",
	"\n"
};

const char *fullness[] = {
	" less than half ",
	" about half ",
	" more than half ",
	" "
};

const char *item_types[] = {
	"UNDEFINED",
	"LIGHT",
	"SCROLL",
	"WAND",
	"STAFF",
	"WEAPON",
	"FURNITURE",
	"!MISSILE",
	"TREASURE",
	"ARMOR",
	"POTION",
	"WORN",
	"OTHER",
	"TRASH",
	"TRAP",
	"CONTAINER",
	"NOTE",
	"DRINKCON",
	"KEY",
	"FOOD",
	"MONEY",
	"PEN",
	"BOAT",
	"!ZCMD",
	"VEHICLE",
	"CLIMBABLE",
	"SPELL",
	"QUEST",
	"QUEST_WEAPON",
	"QUEST_LIGHT",
	"QUEST_CONTAINER",
	"\n"
};

const char *wear_bits[] = {
	"TAKE",
	"FINGER",
	"NECK",
	"BODY",
	"HEAD",
	"LEGS",
	"FEET",
	"HANDS",
	"ARMS",
	"SHIELD",
	"ABOUT",
	"WAIST",
	"WRIST",
	"WIELD",
	"HOLD",
	"THROW",
	"LIGHT_SOURCE",
	"USABLE",
	"TAIL",
	"4LEGS",
	"\n"
};

const char *oflag1_bits[] = {
	"GLOW",
	"HUM",
	"DARK",
	"OMAX",
	"EVIL",
	"INVISIBLE",
	"MAGIC",
	"CURSED",
	"BLESS",
	"ANTI_GOOD",
	"ANTI_EVIL",
	"ANTI_NEUTRAL",
	"ANTI_MAGE",
	"ANTI_THIEF",
	"ANTI_CLERIC",
	"ANTI_WARRIOR",
	"ANTI_DRAGON",
	"GOOD",
	"NO_RENT",
	"MINLVL10",
	"MINLVL20",
	"MINLVL30",
	"MINLVL41",
	"NO_SEE",
	"SMALLONLY",
	"LARGEONLY",
	"NORMALONLY",
	"NO_LOCATE",
	"NO_SUMMON",
	"PERSONAL",
	"QUEST_ITEM",
	"\n"
};

const char *oflag2_bits[] = {
	"unused200",
	"VEHICLE_REVERSED",
	"NO_IDENTIFY",
	"MINLVL15",
	"MINLVL25",
	"MINLVL35",
	"IMMONLY",
	"ANTI_DRUID",
	"ANTI_RANGER",
	"NODROP",
	"LORE",
	"DUALWIELD",
	"ANTI_PALADIN",
	"ANTI_PRIEST",
	"ANTI_BARD",
	"NO_EXTEND",
	"MINLVL42",
	"ANTI_ELDRITCHKNIGHT",
	"ANTI_MONK",
	"\n"
};

const char *weapon_bits[] = {
	"POISONOUS",
	"BLINDING",
	"!PARALYZING",
	"SILVER",
	"WOOD_STAKE",
	"VAMPIRIC",
	"MANA_DRAINING",
	"GOOD",
	"NEUTRAL",
	"EVIL",
	"GOOD_SLAYER",
	"EVIL_SLAYER",
	"UNDEAD_SLAYER",
	"DRAGON_SLAYER",
	"ANIMAL_SLAYER",
	"GIANT_SLAYER",
	"FLAME_ATTACK",
	"ICE_ATTACK",
	"ELEC_ATTACK",
	"SILENCE",
	"MAGIC_ATTACK",
	"ACID_ATTACK",
	"DEMON_SLAYER",
	"DARKMAGIC_ATTACK",
	"PULSE_ATTACK",
	"\n"
};


const char *room_bits[] = {
	"DARK",
	"DEATH",
	"NO_MOB",
	"INDOORS",
	"ANTI_GOOD",
	"ANTI_NEUTRAL",
	"ANTI_EVIL",
	"NO_MAGIC",
	"TUNNEL",
	"PRIVATE",
	"DRAINS_MAGIC",
	"HARMFULL1",
	"HARMFULL2",
	"unused113",
	"NO_STEAL",
	"NO_PKILL",
	"NO_MKILL",
	"WATER",
	"SOUNDPROOF",
	"DUMP",
	"reserved",
	"CANT_HUNT",
	"MAZE",
	"unused123",
	"INN",
	"MYST",
	"FALL_DAMAGE",
	"ANTI_MAGIC",
	"NO_TELEPORT_IN",
	"NO_TELEPORT_OUT",
	"NO_SUMMON_OUT",
	"\n"
};

const char *room_bits2[] = {
	"DAMAGE",
	"TROPICAL",
	"ARCTIC",
	"DESERT",
	"FLOW_NORTH",
	"FLOW_EAST",
	"FLOW_SOUTH",
	"FLOW_WEST",
	"FLOW_UP",
	"FLOW_DOWN",
	"ROUTE_NORTH",
	"ROUTE_EAST",
	"ROUTE_SOUTH",
	"ROUTE_WEST",
	"ROUTE_UP",
	"ROUTE_DOWN",
	"ROUTE_STATION",
	"ROUTE_REVERSE",
	"ROUTE_DEATH",
	"ROUTE_SLOW",
	"ARENA",
	"unused221",
	"unused222",
	"unused223",
	"unused224",
	"unused225",
	"unused226",
	"unused227",
	"NO_QUIT",
	"NO_SUMMON_IN",
	"NO_DONATE",
	"\n"
};

const char *zflag_bits[] = {
	"TESTING",
	"NO_ENTER",
	"NO_GOTO",
	"NEW_ZONE",
	"undefined04",
	"undefined05",
	"undefined06",
	"undefined07",
	"undefined08",
	"undefined09",
	"undefined10",
	"undefined11",
	"undefined12",
	"undefined13",
	"undefined14",
	"undefined15",
	"undefined16",
	"undefined17",
	"undefined18",
	"undefined19",
	"undefined20",
	"undefined21",
	"undefined22",
	"undefined23",
	"undefined24",
	"undefined25",
	"undefined26",
	"undefined27",
	"undefined28",
	"undefined29",
	"undefined30",
	"\n"
};

const char *dirty_bits[] = {
	"UNSAVED",
	"SCRAMBLED",
	"SHOWN",
	"\n"
};

const char *exit_bits[] = {
	"IS_DOOR",
	"PICKPROOF",
	"LOCKED",
	"CLOSED",
	"HIDDEN",
	"NO_MOB",
	"NO_CHAR",
	"CLIMB_ONLY",
	"NO_MOBS_IN_ROOM",
	"PHASEPROOF",
	"BASHPROOF",
	"\n"
};

const char *sector_types[] = {
	"Inside",
	"City",
	"Field",
	"Forest",
	"Hills",
	"Mountains",
	"WaterSwim",
	"WaterNoSwim",
	"Underwater",
	"\n"
};

const char *equipment_types[] = {
	"Special",
	"Worn on right finger",
	"Worn on left finger",
	"First worn around Neck",
	"Second worn around Neck",
	"Worn on body",
	"Worn on head",
	"Worn on legs",
	"Worn on feet",
	"Worn on hands",
	"Worn on arms",
	"Worn as shield",
	"Worn about body",
	"Worn around waist",
	"Worn around right wrist",
	"Worn around left wrist",
	"Wielded",
	"Held",
	"Tail",
	"Worn on front legs",
	"Worn on hind legs",
	"\n"
};

const char *affected_bits[] =
{"BLIND",
	"INVISIBLE",
	"DETECT_EVIL",
	"DETECT_INVISIBLE",
	"DETECT_MAGIC",
	"SENSE_LIFE",
	"HASTE",
	"SANCTUARY",
	"GROUP",
	"WATERWALK",
	"CURSE",
	"SILENCE",
	"POISON",
	"PROTECT_EVIL",
	"FLY",
	"unused115",
	"HOLD",
	"SLEEP",
	"BLOCK_SUMMON",
	"SNEAK",
	"HIDE",
	"DEMON_FLESH",
	"CHARM",
	"FOLLOW",
	"BREATHWATER",		/* not used , I think... */
	"DARKSIGHT",
	"REGENERATION",
	"DREAMSIGHT",
	"AID",
	"MAGIC_RESIST",
	"MAGIC_IMMUNE",
	"AIRWALK",
	"SANCTUARY_MINOR",
	"SANCTUAR_MEDIUM",
	"SLEEP_IMMUNITY",
	"\n"
};

const char *affected_02_bits[] =
{"SANCTUARY_MEDIUM",
	"SANCTUARY_MINOR",
	"SLEEP_IMMUNITY",
	"BARKSKIN",
	"STONESKIN",
	"DETECT_UNDEAD",
	"FAERIE_FIRE",
	"FAERIE_FOG",
	"\n"
};

const char *apply_types[] = {
	"NONE",
	"STR",
	"DEX",
	"INT",
	"WIS",
	"CON",
	"CHA",
	"!CLASS",
	"!LEVEL",
	"AGE",
	"!CHAR_WEIGHT",
	"!CHAR_HEIGHT",
	"MANA",
	"HIT",
	"MOVE",
	"!GOLD",
	"!EXP",
	"AC",
	"HITROLL",
	"DAMROLL",
	"SAVING_PARA",
	"SAVING_ROD",
	"SAVING_PETRI",
	"SAVING_BREATH",
	"SAVING_SPELL",
	"EXTRA_ATTACKS",
	"DETECT_INVIS",
	"DARKSIGHT",
	"MAGIC_RESIST",
	"MAGIC_IMMUNE",
	"BREATHWATER",
	"DETECT_MAGIC",
	"SENSE_LIFE",
	"DETECT_EVIL",
	"!SNEAK",
	"INVISIBILITY",
	"IMPROVED_INVIS",
	"REGENERATION",
	"HOLD_PERSON",
	"HASTE",
	"BONUS_STR",
	"BONUS_DEX",
	"BONUS_CHA",
	"\n"
};

const char *pc_class_types[] = {
	"unused",
	"Magic_user",
	"Cleric",
	"Thief",
	"Warrior",
	"Bard",
	"Priest",
	"Paladin",
	"Druid",
	"Eldritch_Knight",
	"Monk",
	"unused",
	"\n"
};

const char *npc_class_types[] = {	/* Not used anywhere yet */
	"Normal",
	"Undead",
	"Humanoid",
	"Animal",
	"Dragon",
	"Giant",
	"\n"
};

const char *player_bits1[] = {
	"unused10",		/* BIT0 */
	"SENTINEL",
	"SCAVENGER",
	"ISNPC",
	"CLASS_DEMON",
	"unused5",
	"STAY_ZONE",
	"NPC_WIMPY",
	"SIGNAL_HELP",
	"GUARD1",
	"GUARD2",		/* BIT10 */
	"NOSTEAL",
	"INJURED_ATTACK",
	"AGGRESSIVE_EVIL",
	"AGGRESSIVE_NEUT",
	"AGGRESSIVE_GOOD",
	"POISONOUS",
	"SHOW_DIAG",
	"NO_CHARM",
	"DRAINS_XP",
	"CLASS_SHADOW",		/* BIT20 */
	"CLASS_VAMPIRE",
	"CLASS_DRAGON",
	"CLASS_GIANT",
	"CLASS_ANIMAL",
	"CLASS_UNDEAD",
	"CANT_HUNT",
	"HUNTASSIST",
	"NO_FOLLOW",
	"NO_KILL",
	"NO_HOLD_PERSON",	/* BIT30 */
	"\n"
};

const char *player_bits2[] = {
	"SHOW_NAME",		/* BIT0 */
	"FREEZE",
	"NO_OPPONENT_FMSG",
	"NO_PERSONAL_FMSG",
	"NO_BYSTANDER_FMSG",
	"NO_EMOTE",		/* BIT5 */
	"JAILED",
	"QUEST",
	"MOUNT",
	"MUZZLE_ALL",
	"MUZZLE_SHOUT",		/* BIT10 */
	"MUZZLE_BEEP",
	"PKILLER_BULLY",
	"PKILLABLE",
	"AFK",
	"MOB_AI",		/* BIT15 */
	"CLOSE_DOORS",
	"BACKSTAB",
	"EQTHIEF",
	"OPEN_DOORS",
	"USES_EQ",		/* BIT20 */
	"JUNKS_EQ",
	"NO_LOCATE",
	"ATTACKER",
	"DEATH_PROCEDURE",
	"NO_OVER",		/* BIT25 */
	"NO_IMP",
	"NEW_GRAPHICS",
	"SHOWNOEXP",
	"AFW",
	"SHOW_TICK",		/* BIT30 */
	"\n"
};

const char *player_bits3[] = {
	"BRIEF",		/* BIT0 */
	"NO_SHOUT",
	"COMPACT",
	"SHOW_VEHICLE",
	"ANSI",
	"NO_SUMMON",
	"NO_GOSSIP",
	"NO_AUCTION",
	"NO_INFO",
	"PKILLER",
	"ENFORCER",		/* BIT10 */
	"NO_TELL",
	"ASSIST",
	"AUTOSPLIT",
	"NO_PKFLAME",
	"PC_WIMPY",
	"SHOW_ROOM",
	"SHOW_HP",
	"SHOW_MANA",
	"SHOW_MOVE",
	"SHOW_EXITS",		/* BIT20 */
	"NO_HASSLE",
	"NO_WIZINFO",
	"NO_SYSTEM",
	"NO_IMM",
	"AUTOGOLD",
	"AUTOLOOT",
	"AUTOAGGR",
	"NO_MUSIC",
	"NO_AVATAR",
	"NO_CONNECT",		/* BIT30 */
	"\n"
};

const char *wizperm_bits[] = {
	"MEDIATOR",
	"QUESTOR",
	"RULER",
	"BUILDER",
	"\n"
};

const char *descriptor_bits1[] = {
	"CAN_FINGER",
	"unused11",
	"unused12",
	"unused13",
	"unused14",
	"unused15",
	"unused16",
	"unused17",
	"unused18",
	"unused19",
	"unused20",
	"unused21",
	"unused22",
	"unused23",
	"unused24",
	"unused25",
	"unused26",
	"unused27",
	"unused28",
	"unused29",
	"unused30",
	"unused31",
	"\n"
};

const char *position_types[] = {
	"Dead",
	"Mortally wounded",
	"Incapacitated",
	"Stunned",
	"Sleeping",
	"Resting",
	"Sitting",
	"Fighting",
	"Standing",
	"\n"
};

const char *connected_types[] = {
	"Playing",
	"Get name",
	"Confirm name",
	"Get old password",
	"Get password change",
	"Get new password",
	"Confirm old password",
	"Confirm new password",
	"Get sex",
	"Get race",
	"Get class",
	"Read messages of today",
	"Read Menu",
	"Get extra description",
	"Link Dead",
	"Quit",
	"Confirm dice roll",
	"Confirm Deletion",
	"Admin menu",
	"Pkillabe Confirm",
	"\n"
};
/* [level] backstab multiplyer (thieves only) */
const byte backstab_mult[MAX_LEV] = {
	1,			/* 0 */
	2,			/* 1 */
	2,
	2,
	2,
	3,			/* 5 */
	3,
	3,
	3,
	3,
	4,			/* 10 */
	4,
	4,
	4,
	4,
	5,			/* 15 */
	5,
	5,
	5,
	5,
	6,			/* 20 */
	6,
	6,
	6,
	6,
	7,			/* 25 */
	7,
	7,
	7,
	7,
	8,			/* 30 */
	8,
	8,
	8,
	8,
	9,			/* 35 */
	9,
	9,
	9,
	9,
	10,			/* 40 */
	11,
	12,
	13,
	14,
	15,			/* 45 */
	15,
	15,
	15,
	15,
	15,
	15,			/* 51 */

};

const char *reserved_names_prefixes[] = {
	"someone",
	/* "sunshine", "omega", "kinetic", "bollie", "debugger", */
	"\n",
};
/* THESE ARE USED TO CHECK LOGIN NAMES AND USER TITLES */
const char *curse_words[] = {
	"acehole",
	"asshole",
	"bitch",
	"dick",
	"fuck",
	"lingus",
	"penis",
	"raper",
	"rapist",
	"urine",
	"vagina",
	"cunt",
	"shit",
	"faggot",
	"orgasm",
	"nigger",
	"screw",
	"*EOL*",
	"\n",
};
/* THESE ARE USED TO CHECK FOR GOSSIP */
const char *curse_gossip[] = {
	"muzzle_me_please",
	"f!ck",
	"asshole",
	"bitch",
	"fuck",
	"f@@ck",
	"cunt",
	"shit",
	"f*ck",
	"f#ck",
	"f$ck",
	"f%ck",
	"f^ck",
	"f(ck",
	"f)ck",
	"f**k",
	" cock",
	"blowjob",
	"pussy",
	" dick",
	"faggot",
	"clit",
	"twat",
	"b|tch",
	"b!tch",
	"sh|t",
	"sh!t",
	"a$$hole",
	"as$hole",
	"a$shole",
	"b*tch",
	"sh*t",
	"fag",
	"f@@g",
	"vagina",
	"penis",
	"whore",
	"slut",
	"\n",
};

const char *race_list[] = {
	"none",
	"HUMAN",
	"DWARF",
	"GNOME",
	"HALFLING",
	"HALF_GIANT",
	"PIXIE",
	"DEMON",
	"SNOTLING",
	"FELINE",
	"TROLL",
	"DRAGON",
	"CENTAUR",
	"MERMAN",
	"HIGH_ELF",
	"HALF_ELF",
	"DROW_ELF",
	"ORC",
	"HALF_ORC",
	"DARKLING",
	"OGRE",
	"GOBLIN",
	"MINOTAUR",
	"BUGBEAR",
	"GIANT",
	"RAVSHI",
	"IRDA",
	"THRI_KREEN",
	"BULLYWUG",
	"GARGOYLE",
	"IMP",
	"empty",
	"LIZARDMAN",
	"SAHUAGIN",
	"SATYR",
	"YUANTI",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty"
};

const char *class_anti[] = {
	"ANTI_MAGE",
	"ANTI_CLERIC",
	"ANTI_THIEF",
	"ANTI_WARRIOR",
	"ANTI_BARD",
	"ANTI_PRIEST",
	"ANTI_PALADIN",
	"ANTI_DRUID",
	"ANTI_ELDRITCHKNIGHT",
	"ANTI_MONK",
	"\n"
};

const char *gv_overflow_buf = "&n***TOO LONG TO FIT BUFFER***\r\n";
