/* *******************************************************************
*  file: utility.h, Utility module.                  Part of DIKUMUD *
* ****************************************************************** */

#define TRUE  1
#define FALSE 0

#define CLEAR_SCREEN(ch) { send_to_char(ADV_CLEAR, ch); }

#define STATE(d) ((d)->connected)
#define ADMIN_STATE(d) ((d)->admin_state)
#define OLD_STATE(d) ((d)->old_state)

#define LEVEL_EXP(lv_level) (((lv_level)*(lv_level)*(lv_level)*(lv_level)*50) + ((lv_level)*2500))

#define PLAYED_TIME(ch) ((time(0) - ch->player.time.logon) + \
												 ch->player.time.played)
												
#if defined (DIKU_WINDOWS)
#define ABORT_PROGRAM() {                     \
	printf("ERROR:***** ABORTING\r\n");       \
	main_log("ERROR:***** ABORTING\r\n");     \
	printf(buf);                              \
	main_log(buf);                            \
	gv_pid_t = getpid();                      \
	abort();                                  \
}
#else
#define ABORT_PROGRAM() {                     \
	printf("ERROR:***** ABORTING\r\n");       \
	main_log("ERROR:***** ABORTING\r\n");     \
	printf(buf);                              \
	main_log(buf);                            \
	gv_pid_t = getpid();                      \
	if (kill(gv_pid_t, SIGTRAP)) {            \
		printf("kill FAILED: %d\r\n", errno); \
		trace_log("kill FAILED\r\n");         \
		main_log("kill FAILED\r\n");          \
		abort();                              \
		}                                         \
	}
#endif

#define CHECK_BOOTTIME_HIGH(lv_d, lv_temp) {\
	lv_temp = 0;\
    for (lv_d = descriptor_list; lv_d; lv_d = lv_d->next) {\
			if (lv_d->character) lv_temp++;\
		}\
    if (lv_temp > gv_boottime_high) gv_boottime_high = lv_temp;\
}

#define MUZZLECHECK() {\
  if (GET_LEVEL(ch)<IMO_IMP && \
			IS_SET(GET_ACT2(ch), PLR2_MUZZLE_ALL)) {\
    send_to_char("You are MUZZLED, unabled to make a sound!!!\n\r",ch);\
			return;\
  }\
  if (GET_LEVEL(ch)<IMO_IMP && \
			IS_AFFECTED(ch, AFF_SILENCE)) {\
    send_to_char("You are unable to make a sound!!!\n\r",ch);\
			return;\
  }\
}

#define SILENCE_CHECK() {\
	if (IS_AFFECTED(ch, AFF_SILENCE)) {\
		send_to_char("Magic stops you from speaking\r\n", ch); \
			act("$n tries to speak, but can't.", TRUE, ch, 0, 0, TO_ROOM);\
				return;\
	}\
}

#define CHECK_CAST_IN_ROOM() {\
	if (ch->in_room != victim->in_room) {\
		send_to_char("Your spell is unable to reach such a far distance...\n\r", ch);\
			return;\
	}\
}

#define CHECK_FOR_NO_FIGHTING() {     \
	if (ch &&                          \
			victim &&                      \
			GET_LEVEL(ch) < IMO_IMP) {     \
		if (IS_PC(victim)) {          \
			if (!victim->desc) {       \
				send_to_char("You can't attack a link dead char.\r\n",ch);\
					return;                   \
			}                           \
			else if (victim->desc->connected == CON_LINK_DEAD) { \
				send_to_char("You can't attack a link dead char!\r\n",ch);\
					return;                   \
			}                           \
		}                              \
		if (GET_LEVEL(ch) < IMO_IMM) {\
			if (IS_SET(GET_ACT1(victim), PLR1_NOKILL)) {\
				send_to_char("An all powerful force stops you from attacking this person.\r\n", ch);\
					return;\
			}\
			if (GET_LEVEL(victim) < 4 && !IS_NPC(victim)) { \
				send_to_char("A mysterious force stops you from attacking this person.\r\n", ch);\
					return;\
			}\
		}\
		if (gv_port == HELLNIGHT_PORT) {           \
			if (ch->in_room > 0 &&                    \
					(world[ch->in_room].number == 3000 ||  \
					 world[ch->in_room].number == 3001 ||  \
					 world[ch->in_room].number == 3005 ||  \
					 world[ch->in_room].number == 3006 ||  \
					 world[ch->in_room].number == 3008 ||  \
					 world[ch->in_room].number == 3014 ||  \
					 world[ch->in_room].number == 3054 ||  \
					 world[ch->in_room].number == 3434)) { \
				send_to_char("You can't attack here!\r\n", ch);\
					return;\
			}\
		}\
	}\
}

#define IS_ROOM_NO_KILL() {                                           \
	if (ch && victim) {                                                \
		if (GET_LEVEL(ch) < IMO_IMM) {                                 \
			if ((IS_SET(world[ch->in_room].room_flags, RM1_NOPKILL) &&  \
					 !IS_NPC(victim)) ||          \
					(IS_SET(world[ch->in_room].room_flags, RM1_NOMKILL) &&   \
					 IS_NPC(victim))) {          \
				send_to_char("Your are deathly afraid of attacking anything at this location.\r\n", ch);\
					return;                                      \
			}                                                       \
		}                                                           \
		if (IS_SET(gv_run_flag, RUN_FLAG_DISABLE_FIGHTING)) {       \
			send_to_char("Fighting has been disabled.\r\n", ch);    \
				return;                                          \
		}                                                           \
	}                                                             \
}                                                                \

#define CHECK_FOR_CHARM() { if (IS_AFFECTED(victim, AFF_CHARM) && !(victim->master == ch)) { \
	send_to_char("You cant do that, it is illegal to attack pets!\n\r", ch); \
		return; }\
	if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM) && IS_PC(victim)) { \
		if (ch->master) send_to_char ("Your pet refuses to attack another player!\r\n", \
																	ch->master); \
																		return; }\
}\

#define CHECK_TRANSFORMS() { \
	if (IS_NPC(victim)) { send_to_char ("You can not cast this on NPCs. Action logged.\n\r", ch); \
		return; } \
	if (IS_AFFECTED (victim, AFF_TRANSFORMED)) return; \
}

#define CHECK_FOR_PK() { \
	if (GET_LEVEL(ch) < IMO_IMM) { \
		if ((IS_PC(ch)) && (IS_PC(victim))) { \
			if ((GET_LEVEL(ch) < gv_pkill_level) || (GET_LEVEL(victim) < gv_pkill_level)) { \
				if (!cl1900_at_war(ch, victim) && \
						!(IS_SET(GET_ACT2(ch), PLR2_PKILLABLE) && IS_SET(GET_ACT2(victim), PLR2_PKILLABLE))) { \
					if (!bt1250_can_attack(ch, victim)) {\
						if (!IS_SET(GET_ROOM2(ch->in_room), RM2_ARENA)) {\
							if ((!IS_SET(GET_ACT2(ch), PLR2_PKILLABLE)) || \
									(!IS_SET(GET_ACT2(victim), PLR2_PKILLABLE)) || \
									(!(GET_LEVEL(ch) == GET_LEVEL(victim)) && \
									 (((GET_LEVEL(ch) > 40)) || (GET_LEVEL(victim) > 40))) || \
									(abs(GET_LEVEL(ch) - GET_LEVEL(victim)) > 5) || \
									(IS_AFFECTED(ch, AFF_TRANSFORMED)) || \
									(IS_AFFECTED(victim, AFF_TRANSFORMED))) { \
								send_to_char("You can't do that, the gods wont let you! (help PKILLABLE for more info).\n\r", ch); \
									return;\
							}\
						}\
					}\
				}\
			}\
		}\
	}\
	if (!IS_MOB(victim)) ch->player.time.lastpkill = PLAYED_TIME(ch);\
}


#define CHECK_LEVEL(min_lev)  if (GET_LEVEL(ch) < (min_lev)) {bzero(buf, sizeof(buf)); sprintf(buf, "You must be level %d to perform this command.\r\n", min_lev);send_to_char(buf, ch);return;}

/* eventually declare all std function types via macros */
#define SPELL_FN(spell) extern void spell(sbyte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj)
#define CAST_FN(spell)  extern void spell(sbyte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj)

#define LOWER(c) (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c) (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c))

/* Functions in utility.c */
#define MAXV(a,b) (((a) > (b)) ? (a) : (b))
#define MINV(a,b) (((a) < (b)) ? (a) : (b))

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r')
#define IF_STR(st) ((st) ? (st) : "\0")
#define CAP(st)  (*(st) = UPPER(*(st)))

#if defined (DIKU_WINDOWS)
#define CREATE(result, type, number) do{\
	result = (type *) malloc( (number) * sizeof(type) ); \
		if(!(result)) {\
			perror("malloc failure");\
				exit(1);\
		}\
		bzero(result,(number) * sizeof(type));\
}while(0)
#else
#define CREATE(result, type, number)  do {\
	if ((number) * sizeof(type) <= 0)	\
		main_log("SYSERR: Zero bytes or less requested");	\
			if (!((result) = (type *) calloc ((number), sizeof(type))))	\
			{ perror("SYSERR: malloc failure"); exit(1); } } while(0)

#define CREATE_OLD(result, type, number) do{\
				result = (type *) malloc( (number) * sizeof(type) ); \
					if(!(result)) {\
						perror("malloc failure");\
							exit(1);\
					}\
					bzero(result,(number) * sizeof(type));\
			}while(0)
#endif

#define RECREATE(result,type,number) do {if (!((result) = (type *) realloc ((result), sizeof(type) * (number)))) { perror("realloc failure"); abort(); } } while(0)

#define IS_SET(flag,bit)  ((flag) & (bit))

#define IS_PERSONAL(obj) (IS_SET((obj)->obj_flags.flags1, OBJ1_PERSONAL))
#define IS_QUEST_ITEM(obj) (IS_SET((obj)->obj_flags.flags1, OBJ1_QUEST_ITEM))

#define IS_MAGE(ch) (classes[ch->player->class].flag, CFLAG_CLASS_MAGE);
#define IS_CLERIC(ch) (classes[ch->player->class].flag, CFLAG_CLASS_CLERIC);
#define IS_WARRIOR(ch) (classes[ch->player->class].flag, CFLAG_CLASS_WARRIOR);
#define IS_THIEF(ch) (classes[ch->player->class].flag, CFLAG_CLASS_THIEF);
#define IS_PRIEST(ch) (classes[ch->player->class].flag, CFLAG_CLASS_PRIEST);
#define IS_PALADIN(ch) (classes[ch->player->class].flag, CFLAG_CLASS_PALADIN);
#define IS_BARD(ch) (classes[ch->player->class].flag, CFLAG_CLASS_BARD);
#define IS_DRUID(ch) (classes[ch->player->class].flag, CFLAG_CLASS_DRUID);
#define IS_RANGER(ch) (classes[ch->player->class].flag, CFLAG_CLASS_RANGER);
#define IS_ELDRITCH(ch) (classes[ch->player->class].flag, CFLAG_CLASS_ELDRITCH);
#define IS_MONK(ch) (classes[ch->player->class].flag, CFLAG_CLASS_MONK);

#define SWITCH(a,b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }

#define USES_ANSI(ch) ( IS_SET(GET_ACT3(ch), PLR3_ANSI) )

#define IS_AFFECTED(ch,skill) ( IS_SET((ch)->specials.affected_by, (skill)) )
#define IS_AFFECTED2(ch,skill) ( IS_SET((ch)->specials.affected_02, (skill)) )
#define	IS_AFFECTED3(ch, skill) ( IS_SET((ch)->specials.affected_03, (skill)) )
#define	IS_AFFECTED4(ch, skill) ( IS_SET((ch)->specials.affected_04, (skill)) )
#define IS_DARK(ch, room)  (li1200_is_it_dark(ch, room, 0))
#define IS_LIGHT(ch, room)  (!IS_DARK(ch, room))

#define SET_BIT(var,bit)  ((var) = (var) | (bit))
#define REMOVE_BIT(var,bit)  ((var) = (var) & ~(bit) )
#define CHANCE100(percent) ((number (0,100) < percent) ? TRUE : FALSE)
#define CHANGE1000(percent) ((number (0,1000) < percent) ? TRUE : FALSE)

/* Can subject see character "obj"? */
#define CAN_SEE(sub, obj) (li1000_can_see_char(sub, obj, 0))
#define CAN_SEE_OBJ(sub, obj) (li1100_can_see_obj(sub, obj, 0))
#define CAN_SEE_OBJ_NOLOCATE(sub, obj) (li1100_can_see_obj(sub, obj, BIT0))

#define GET_REQ(i) (i<2  ? "Awful" :(i<4  ? "Bad"     :(i<7  ? "Poor"      :(i<10 ? "Average" :(i<14 ? "Fair"    :(i<20 ? "Good"    :(i<24 ? "Very good" :\
																																																									"Superb" )))))))

#define HSHR(ch) ((ch)->player.sex ?    /* $s HISHER */      \
									(((ch)->player.sex == 1) ? "his" : "her") : "its")
#define HSSH(ch) ((ch)->player.sex ?    /* $e HESHE*/      \
									(((ch)->player.sex == 1) ? "he" : "she") : "it")
#define HMHR(ch) ((ch)->player.sex ?    /* $m HIMHER */      \
									(((ch)->player.sex == 1) ? "him" : "her") : "it")

#define ANA(obj) (index("aeiouyAEIOUY", *(obj)->name) ? "An" : "A")
#define SANA(obj) (index("aeiouyAEIOUY", *(obj)->name) ? "an" : "a")

#define IS_NPC(ch)  (IS_SET((ch)->specials.act, PLR1_ISNPC))
#define IS_UNCHARMED_NPC(ch) ((IS_SET(GET_ACT1(ch), PLR1_ISNPC) && !IS_AFFECTED(ch, AFF_CHARM)))
#define IS_PC(ch)  (!(IS_SET((ch)->specials.act, PLR1_ISNPC)))
#define IS_MOB(ch)  (IS_SET((ch)->specials.act, PLR1_ISNPC) && ((ch)->nr >-1))
#define IS_UNDEAD(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_UNDEAD))
#define IS_ANIMAL(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_ANIMAL))
#define IS_DRAGON(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_DRAGON))
#define IS_DEMON(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_DEMON))
#define IS_GIANT(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_GIANT))
#define IS_SHADOW(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_SHADOW))
#define IS_VAMPIRE(ch) (IS_SET((ch)->specials.act, PLR1_CLASS_VAMPIRE))
#define FAILS_SANITY(ch) (((ch)->beg_rec_id != CH_SANITY_CHECK || \
                           (ch)->end_rec_id != CH_SANITY_CHECK))

#define QFAILS_SANITY(lv_que) (((lv_que)->beg_rec_id != QUEUE_SANITY_CHECK || \
																(lv_que)->end_rec_id != QUEUE_SANITY_CHECK))

#define AVAT_MORTAL(ch, victim) ((((GET_LEVEL(ch) == 41) && (GET_LEVEL(victim) < 41)) || \
                                  ((GET_LEVEL(ch) < 41) && (GET_LEVEL(victim) == 41))) && \
																 (IS_PC(ch) && IS_PC(victim)))


#define IS_CASTED_ON(ch, spell) (ha1375_affected_by_spell(ch, spell))
#define GET_POS(ch)       ((ch)->specials.position)
#define GET_COND(ch, i)   ((ch)->specials.conditions[(i)])
#define GET_FIGHTING(ch)  ((ch)->specials.fighting)

#define RIDING(ch)        ((ch)->specials.mount)
#define RIDDEN_BY(ch)     ((ch)->specials.ridden_by)

#define GET_ACT1(ch)      ((ch)->specials.act)
#define GET_ACT2(ch)      ((ch)->specials.act2)
#define GET_ACT3(ch)      ((ch)->specials.act3)
#define GET_ACT4(ch)      ((ch)->specials.act4)
#define	GET_ACT5(ch)	  ((ch)->specials.act5)
#define	GET_ACT6(ch)	  ((ch)->specials.act6)
#define GET_WIZ_PERM(ch)  ((ch)->specials.wiz_perm)
#define GET_JAILTIME(ch)  ((ch)->specials.jailtime)
#define GET_DSTIME(ch)    ((ch)->specials.deathshadowtime)
#define GET_GMTIME(ch)    ((ch)->specials.godmuzzletime)
#define CLAN_NUMBER(ch)   ((ch)->specials.clan.number)
#define CLAN_RANK(ch)     ((ch)->specials.clan.rank)
#define CLAN_FLAGS1(ch)   ((ch)->specials.clan.flags1)
#define CLAN_EXISTS(clan_nr)                 (clan_nr < gv_last_clan ? clans[clan_nr].enabled : 0 )
#define AT_WAR (ch, vict)		     (cl1900_at_war(ch, vict))
#define END_COLOR(ch)			     (IS_SET(GET_ACT3(ch), PLR3_ANSI) ? END : "" )
#define GET_NEWGRAPHICS(ch)		     (IS_SET(GET_ACT2(ch), PLR2_NEWGRAPHICS))
#define GET_RANK_COLOR(ch, rank)	     (IS_SET(GET_ACT3(ch), PLR3_ANSI) ? rank_color[rank] : "")
#define GET_CLAN_ENABLED(clan_nr)            (clans[clan_nr].enabled)
#define GET_CLAN_COLOR(ch, clan_nr)          (IS_SET(GET_ACT3(ch), PLR3_ANSI) ? clans[clan_nr].color : "" )
#define GET_CLAN_NAME(clan_nr)               (clans[clan_nr].name)
#define GET_CLAN_LEADER(clan_nr)             (clans[clan_nr].leader)
#define GET_CLAN_ROOM(clan_nr)               (clans[clan_nr].room)
#define GET_CLAN_DON_ROOM(clan_nr)           (clans[clan_nr].don_room)
#define GET_CLAN_BOARD_NUM(clan_nr)          (clans[clan_nr].board_num)
#define GET_CLAN_RANK_NAME(clan_nr, rank_nr) (clans[clan_nr].rank[rank_nr])
#define GET_CLAN_DESC(clan_nr, desc_line)    (clans[clan_nr].desc[desc_line])
#define GET_DSC1(ch)      (player_table[(ch)->nr].pidx_flag1)
#define GET_DSC1_NUM(nr)  (player_table[(nr)].pidx_flag1)
#define GET_EMAIL(ch)     (player_table[(ch)->nr].pidx_email)
#define GET_DSC_LEVEL(ch) (player_table[(ch)->nr].pidx_level)
#define GET_DSC_LEVEL_NUM(nr) (player_table[(nr)].pidx_level)
#define GET_NAME(ch)      ((ch)->player.name)
#define GET_MOB_NAME(ch)  ((ch)->player.short_descr)
#define GET_REAL_NAME(ch) ((IS_NPC(ch) ? (ch)->player.short_descr : (ch)->player.name))
#define GET_TITLE(ch)     ((ch)->player.title)
#define GET_LEVEL(ch)     ((ch)->player.level)
#define GET_RACE(ch)      ((ch)->player.race)
#define GET_CLASS(ch)     ((ch)->player.class)
#define GET_START(ch)     ((ch)->player.start_room)
#define GET_PRIVATE(ch)   ((ch)->player.private_room)
#define GET_VISIBLE(ch)   ((ch)->player.visible_level)
#define GET_INCOGNITO(ch) ((ch)->incognito_lvl) //Incognito & ghost Shalira 22.07.01
#define GET_GHOST(ch)     ((ch)->ghost_lvl) //Incognito & ghost Shalira 22.07.01
#define GET_BLESS(ch)	  ((ch)->blessing)  //are we blessing Shalira 24.07.01
#define GET_LOGOFF_TIME(ch) ((ch)->player.logoff_time)
#define GET_HOW_LEFT(ch)  ((ch)->player.how_player_left_game)
#define GET_HOW_SAVE(ch)  ((ch)->player.how_player_left_save)
#define GET_AGE(ch)       (z_age(ch).year)
#define GET_STR(ch)       ((ch)->tmpabilities.str)
#define GET_REAL_STR(ch)  ((ch)->abilities.str)
#define GET_BONUS_STR(ch) ((ch)->bonus.str)
#define GET_DEX(ch)       ((ch)->tmpabilities.dex)
#define GET_REAL_DEX(ch)  ((ch)->abilities.dex)
#define GET_BONUS_DEX(ch) ((ch)->bonus.dex)
#define GET_INT(ch)       ((ch)->tmpabilities.intel)
#define GET_REAL_INT(ch)  ((ch)->abilities.intel)
#define GET_WIS(ch)       ((ch)->tmpabilities.wis)
#define GET_REAL_WIS(ch)  ((ch)->abilities.wis)
#define GET_CON(ch)       ((ch)->tmpabilities.con)
#define GET_REAL_CON(ch)  ((ch)->abilities.con)
#define GET_CHA(ch)       ((ch)->tmpabilities.cha)
#define GET_REAL_CHA(ch)  ((ch)->abilities.cha)
#define GET_BONUS_CHA(ch) ((ch)->bonus.cha)
#define HUNTING(ch)       ((ch)->specials.hunting)
#define ROOM_FLAGS(loc)   (world[(loc)].room_flags)
#define GET_SKILL(ch, i)  ((ch)->skills[i].learned)
#define GET_AC(ch)        ((ch)->points.armor)
#define GET_HIT(ch)       ((ch)->points.hit)
#define GET_HIT_LIMIT(ch) (li1500_hit_limit(ch))	/* use for con
							 * adjustment */
#define GET_MAX_HIT(ch)   ((ch)->points.max_hit)
#define GET_MOVE(ch)      ((ch)->points.move)
#define GET_MOVE_LIMIT(ch) (li1600_move_limit(ch))	/* use for move adjust. */
#define GET_MAX_MOVE(ch)  ((ch)->points.max_move)
#define GET_MANA(ch)      ((ch)->points.mana)
#define GET_MANA_LIMIT(ch)(li1300_mana_limit(ch))	/* use for mana adjust. */
#define GET_SPECIAL_MANA(ch) (IS_CASTED_ON(ch,SPELL_BIND_SOULS) ? li1300_mana_limit(ch) + (ch)->extra_mana : li1300_mana_limit(ch))
#define GET_MAX_MANA(ch)  ((ch)->points.max_mana)
#define GET_GOLD(ch)      ((ch)->points.gold)
#define GET_QPS(ch)	  ((ch)->questpoints)
#define GET_BANK_GOLD(ch) ((ch)->points.bank_gold)
#define GET_INN_GOLD(ch)  ((ch)->points.inn_gold)
#define GET_STAT_COUNT(ch)((ch)->points.stat_count)
#define GET_STAT_GOLD(ch) ((ch)->points.stat_gold)
#define GET_EXP(ch)       ((ch)->points.exp)
#define GET_USEDEXP(ch)   ((ch)->points.used_exp)
#define GET_AVAILABLE_EXP(ch) ( (GET_LEVEL(ch) < IMO_LEV) ? MAXV(0, (ch)->points.exp - (ch)->points.used_exp) : (ch)->points.exp )
#define GET_HEIGHT(ch)    ((ch)->player.height)
#define GET_WEIGHT(ch)    ((ch)->player.weight)
#define GET_SEX(ch)       ((ch)->player.sex)
#define GET_ARMOR(ch)     ((ch)->points.armor)
#define GET_HITROLL(ch)   ((ch)->points.hitroll)
#define GET_DAMROLL(ch)   ((ch)->points.damroll)
#define GET_PRACTICES(ch) ((ch)->specials.spells_to_learn)
#define GET_DEATHS(ch)    ((ch)->points.stat_gold)	/* I cheated here,
							 * recycling */
#define GET_KILLS(ch)     ((ch)->points.fate)
#define GET_SCORE(ch)     ((ch)->points.score)
#define GET_ALIASES(ch)		((ch)->player.aliases)

/* #define GET_REAL_NAME(ch)  if(IS_NPC(ch) ? GET_MOB_NAME(ch) : GET_NAME(ch)) */

#define AWAKE(ch) (GET_POS(ch) > POSITION_SLEEPING)

#define WAIT_STATE(ch, cycle) (((ch)->desc) ? (ch)->desc->wait = (cycle) : 0)
/* Object And Carry related macros */
#define GET_ITEM_TYPE(obj) ((obj)->obj_flags.type_flag)
#define CAN_WEAR(obj, part) (IS_SET((obj)->obj_flags.wear_flags,part))
#define GET_OBJ_NAME(obj) ((obj)->name ? ha1100_fname((obj)->name) : \
													 (obj)->short_description ? (obj)->short_description  : "noshortdesc")
#define GET_OSDESC(obj) ((obj)->short_description ? (obj)->short_description  : "noshortdesc")
#define GET_OBJ1(obj) ((obj)->obj_flags.flags1)
#define GET_OBJ2(obj) ((obj)->obj_flags.flags2)
#define GET_OBJ_WEIGHT(obj) ((obj)->obj_flags.weight)

#define OVAL0(obj) ((obj)->obj_flags.value[0])
#define OVAL1(obj) ((obj)->obj_flags.value[1])
#define OVAL2(obj) ((obj)->obj_flags.value[2])
#define OVAL3(obj) ((obj)->obj_flags.value[3])

#define GET_ROOM1(room) (world[(room)].room_flags)
#define GET_ROOM2(room) (world[(room)].room_flags2)

#define CAN_CARRY_W(ch) (((GET_STR(ch)+GET_BONUS_STR(ch)) * 10)  + (MAXV(0, (GET_STR(ch)+GET_BONUS_STR(ch)) - 14) * 60))

#define CAN_CARRY_N(ch) (5+GET_DEX(ch)/2+GET_LEVEL(ch)/2)
#define IS_CARRYING_W(ch) ((ch)->specials.carry_weight)
#define IS_CARRYING_N(ch) ((ch)->specials.carry_items)

#define CAN_CARRY_OBJ(ch,obj)	(((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) <= CAN_CARRY_W(ch)) && ((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)))

#define CAN_GET_OBJ(ch, obj)   (CAN_WEAR((obj), ITEM_TAKE) && CAN_CARRY_OBJ((ch),(obj)) && CAN_SEE_OBJ((ch),(obj)))

#define IS_OBJ_STAT(obj,stat) (IS_SET((obj)->obj_flags.flags1,stat))
#define IS_OBJ_STAT2(obj,stat) (IS_SET((obj)->obj_flags.flags2,stat))

#define REMOVE_COLOR(str) parse_colors(str, sizeof(str), 0, 0)

#define SCRAM1 crypt
#define SCRAM2 scramble
/* char name/short_desc(for mobs) or someone?  */
#define PERS(ch, vict)   ( \CAN_SEE(vict, ch) ? (!IS_NPC(ch) ? (ch)->player.name : (ch)->player.short_descr) : "someone")

#define OBJS(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? (obj)->short_description  : "something")

#define GET_TIMER(obj) ((obj)->obj_flags.timer - 1)
/* Only use this one for incrementing/decrementing, also, add 1 to it if your setting it:
TIMER(obj) = my_value + 1 */
#define TIMER(obj) ((obj)->obj_flags.timer)

#define OBJN(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? ha1100_fname((obj)->name) : "something")

//Utilities added by Odin 2 / 01 / 01
#define GET_OBJ_VNUM(obj) (((obj)->item_number >= 0) ? obj_index[(obj)->item_number].virtual : 0)
#define GET_EQ(ch, wear) ((ch)->equipment[wear])

#define OUTSIDE(ch) (!IS_SET(world[(ch)->in_room].room_flags,RM1_INDOORS))
#define EXIT(ch, door)  (door <6 ? world[(ch)->in_room].dir_option[door] : NULL)
#define CAN_GO(ch, door) ((ch->in_room != NOWHERE) && EXIT(ch,door)  &&  (EXIT(ch,door)->to_room != NOWHERE) && !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))

#define GET_ALIGNMENT(ch) ((ch)->specials.alignment)
#define GET_CARRY_WEIGHT(ch) ((ch)->specials.carry_weight)
#define GET_CARRY_ITEMS(ch) ((ch)->specials.carry_items)
#define IS_GOOD(ch)    (GET_ALIGNMENT(ch) >= 350)
#define IS_EVIL(ch)    (GET_ALIGNMENT(ch) <= -350)
#define IS_NEUTRAL(ch) (!IS_GOOD(ch) && !IS_EVIL(ch))

/* HUNT Utility macros */
#define MARK(room) (SET_BIT(ROOM_FLAGS(room), RM1_BFS_MARK))
#define UNMARK(room) (REMOVE_BIT(ROOM_FLAGS(room), RM1_BFS_MARK))
#define IS_MARKED(room) (IS_SET(ROOM_FLAGS(room), RM1_BFS_MARK))
#define TOROOM(x, y) (world[(x)].dir_option[(y)]->to_room)
#define IS_CLOSED(x, y) (IS_SET(world[(x)].dir_option[(y)]->exit_info, EX_CLOSED))

/* The source previously used the same code in many places to remove
* an item from a list: if it's the list head, change the head, else
* traverse the list looking for the item before the one to be removed.
* Now, we have a macro to do this.  To use, just make sure that there
* is a variable 'lv_temp' declared as the same type as the list to be
* manipulated.
*/
#define REMOVE_FROM_LIST(item, head, loc)	\
if ((item) == (head))			\
head = (item)->next;			\
else {					\
	lv_temp = head;				\
		while (lv_temp && (lv_temp->next != (item))) \
			lv_temp = lv_temp->next;		\
				if (lv_temp)				\
					lv_temp->next = (item)->next;		\
						else {\
							bzero(gv_str, sizeof(gv_str));          \
								sprintf(gv_str, "ERROR:  Removing from list %d\r\n", (loc));  \
									main_log(buf);				\
						}\
}
