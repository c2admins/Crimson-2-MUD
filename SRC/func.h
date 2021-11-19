/**********************************************************************
*  file: func.h , command proc declarations.      Part of DIKUMUD     *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
**********************************************************************/

#if defined (DIKU_WINDOWS)
void bzero(void *buf, int length);
int strcasecmp(char *a, char *b);
char *crypt(char *source, char *target);
char *index(char *name, char search);
void gettimeofday(struct timeval * t, struct timezone * dummy);
int random(void);
void srandom(unsigned int seed);
#endif

#define DAMAGE(c,v,d,a) damage((c),(v),(d),(a),1)

void ft_get_damage_string(int dmg, char *buf);

void do_adjust(struct char_data * ch, char *arg, int cmd);
//spells.c

int check_lore(struct char_data * ch, struct obj_data * obj);
int check_lore_container(struct obj_data * container, struct obj_data * obj);
int check_nodrop(struct obj_data * container);
int check_corpse(struct char_data * ch, struct obj_data * obj, int looting);
void do_request(struct char_data * ch, char *arg, int cmd);
void do_ignore(struct char_data * ch, char *arg, int cmd);
bool is_ignored(struct char_data * ch, char *name);

int magic_fails(struct char_data * ch, struct char_data * tar_ch);
void ft2000_die(struct char_data * ch, struct char_data * killer);
void perform_wear_msg(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose);
void do_temp(struct char_data * ch, char *arg, int cmd);
void do_info_noch(char *arg, int min_level, int max_level);
void reset_target(struct char_data * victim);
int ft2600_check_for_special_weapon_attacks(struct char_data * ch, struct char_data * victim, struct obj_data * wielded);
void ma1000_do_mail(struct char_data * ch, char *arg, int cmd);
bool ma1100_check_for_msg(struct char_data * ch, int msg_nr);
void ma1200_show_msg_list(struct char_data * ch);
void ma1300_write_msg(struct char_data * ch, int target, int clanno, char *title);
void ma1400_read_msg(struct char_data * ch, int msg_nr);
void ma1500_remove_msg(struct char_data * ch, int msg_nr);
void ma1600_clear_msg(struct char_data * ch);
int ma1700_get_msg_rname(struct char_data * ch, int msg_nr, char *rname);
void ma1700_mail_clan(struct char_data * ch, int clan_nr, char *topic);
void ma1800_mail_bulk(struct char_data * ch, int minlvl, int maxlvl, char *topic);
void ma1900_bulk_draft(struct char_data * ch);
void ma2000_write_bulk(struct char_data * ch, int player_nr, char *topic);

void do_set_char_questpoints(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_usedexp(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_prompt(struct char_data * ch, char *arg, int cmd);

bool is_targeted(struct char_data * ch, struct char_data * victim);
bool are_grouped(struct char_data * ch, struct char_data * victim);

int does_contain(char *arg, char *arg2);
int check_class_race(int class, int race);
int bn1100_isbanned(char *hostname);
void bn1000_load_banned(void);

void pi1000_show_rip(struct char_data * ch);

void ad1000_do_admin(struct char_data * ch, char *arg, int cmd);
void ad1100_admin_nanny(struct descriptor_data * d, char *arg);
void ad1010_do_oedit(struct char_data * ch, char *arg, int cmd);
void ad1110_admin_nanny_general(struct descriptor_data * d,
				  char *arg);
void ad1200_display_menu(struct char_data * ch, int lv_menu);
int ad1300_select_player(struct char_data * ch, char *arg);
void ad1400_display_general(struct char_data * ch);
void ad1410_display_att(struct char_data * ch);
void ad1420_display_object(struct char_data * ch);
void ad1500_show_menu(struct descriptor_data * d);
void ad2000_clear_admin(struct char_data * ch);
void ad2100_save_player(struct char_data * ch);

void ad1430_display_prompt(struct char_data * ch);
void ad1431_display_display(struct char_data * ch);
void ad1432_display_channel(struct char_data * ch);
void ad1433_display_flag(struct char_data * ch);
void ad1434_display_general(struct char_data * ch);
void ad2100_end_config(struct char_data * ch);


void cl1100_clear_admin(struct char_data * ch);
void au1000_do_auction(struct char_data * ch, char *arg, int cmd);
void au1100_do_bid(struct char_data * ch, char *arg, int cmd);
void au1200_show_info(char *msg);
void au1300_reboot_auction(void);
void au1400_do_cancel(struct char_data * ch, char *arg, int cmd);
void au1500_counter(void);
void au1600_clear_bidders(void);
void au1700_new_bidder(struct char_data * ch, int bid);
void au1800_remove_bidder(struct char_data * ch, int lv_flag);
void au1900_close_auction(struct char_data * ch);
void au2000_remind_mob_to_auction_later(int iFlag, struct char_data * ch, struct obj_data * obj, int amount);

void qu0000_questsales(struct char_data * ch, char *arg, int cmd);
void qu0100_questFileOpen();
int qu0200_questWriteVar(int v1, int v2, int v3);
int qu1000_questmaster(struct char_data * ch, int cmd, char *arg);
void qu1100_do_quest(struct char_data * ch, char *arg, int cmd);
void qu1200_make_quest(struct char_data * ch);
void qu1300_quest_update(void);
void qu1400_clear_quest(struct char_data * ch);
void qu2000_check_quest(struct char_data * ch, char *arg);
void qu2100_do_mquestkey(struct char_data * ch, char *arg, int cmd);
void qu2200_do_mquestsay(struct char_data * ch, char *arg, int cmd);
void qu2300_quest_info(struct char_data * ch, int vnum);
void qu2400_convo_info(struct char_data * ch, int vnum, int lv_nr);
void qu2500_do_qsave(struct char_data * ch, char *arg, int cmd);
void qu2600_do_mquestcmd(struct char_data * ch, char *arg, int cmd);

void skill_meditate(struct char_data * ch, char *arg, int cmd);
void do_special(struct char_data * ch, char *arg, int cmd);
void do_manual_info(struct char_data * ch, char *arg, int cmd);
void search_replace(char *string, const char *find, const char *replace);
void do_deathshadow(struct char_data * ch);

bool ma1100_check_for_msg(struct char_data * ch, int msg_nr);
void ma1200_show_msg_list(struct char_data * ch);
void ma1300_write_msg(struct char_data * ch, int target, int clanno, char *title);
void ma1400_read_msg(struct char_data * ch, int msg_nr);
void ma1500_remove_msg(struct char_data * ch, int msg_nr);
void ma1600_clear_msg(struct char_data * ch);
int ma1700_get_msg_rname(struct char_data * ch, int msg_nr, char *rname);
int new_message(struct char_data * ch);

void cl1000_do_clan(struct char_data * ch, char *arg, int cmd);
void cl1200_start_war(struct char_data * ch, int enemy);
void cl1300_stop_war(struct char_data * ch, int enemy);
void cl1350_remove_war(int clan, struct clan_war_data * war);
struct clan_war_data *cl1400_get_war(int clan, int enemy);
void cl1500_attack_player(struct char_data * ch, struct char_data * vict);
void cl1600_kill_player(struct char_data * ch, struct char_data * vict);
void cl1700_show_war_stats(struct char_data * ch, int clan);
void cl1800_do_war(struct char_data * ch, char *arg, int cmd);
int cl1900_at_war(struct char_data * ch, struct char_data * vict);
void cl2000_player_escape(struct char_data * ch, struct char_data * vict);
void cl2100_do_set_clan(struct char_data * ch, char *arg, int cmd);

void cr3000_do_rlevel(struct char_data * ch, char *arg, int cmd);
void cr3900_do_mset(struct char_data * ch, char *arg, int cmd);
void cr4000_do_mskill(struct char_data * ch, char *arg, int cmd);
void cr4100_do_mattack(struct char_data * ch, char *arg, int cmd);

void co2500_close_single_socket(struct descriptor_data * d);
void co3000_send_to_all(char *messg);
void co3100_send_to_outdoor(char *messg);
void send_to_char(char *messg, struct char_data * ch);
void co3200_send_to_except(char *messg, struct char_data * ch);
void co3300_send_to_room(char *messg, int room);
void co3425_send_except_room(char *messg, int room, struct char_data * ch);
void co3450_send_to_except_room_two
  (char *messg, int room, struct char_data * cha1, struct char_data * cha2);
void co3500_clear_user_list();
void perform_to_all(char *messg, struct char_data * ch);
void perform_complex(struct char_data * cha1, struct char_data * cha2,
		       struct obj_data * obj1, struct obj_data * obj2,
		       char *mess, byte mess_type, bool hide);
void ansi_act(char *str, int hide_invisible, struct char_data * ch, struct obj_data * obj, void *vict_obj, int type, char *color, int show_flags);

void ansi(char *color, struct char_data * ch);

void act(char *str, int hide_invisible, struct char_data * ch, struct obj_data * obj, void *vict_obj, int type);

int co2200_write_to_descriptor(int desc, char *txt);
void co1400_write_to_q(char *txt, struct txt_q * queue, int aliased);
void co1500_write_to_head_q(char *txt, struct txt_q * queue);
void db1000_boot_db(void);
void db2000_reset_time(void);
void db2100_allocate_room(int new_top);
void db2200_allocate_zcmd(int zone, int new_top);
void db2300_allocate_mob(int new_top);
void db2400_allocate_obj(int new_top);
void db3000_build_player_index(void);
void db3100_master_pwd(void);
int db4000_zone_of(int virtual);
void db4100_boot_mob(void);
void db4200_boot_obj(void);
void db4300_boot_world(void);
void db4400_setup_dir(FILE * fl, int room, int dir);
void db4500_renum_world(void);
void db4600_renum_zone_table(void);
void db4700_boot_zones(void);
void db4800_boot_quests(void);
struct char_data *db5000_read_mobile(int nr, int type);
void db5150_add_obj_to_vehicle_list(struct obj_data * lv_obj);
void db5175_remove_obj_from_vehicle_list(struct obj_data * lv_obj);
void db5200_load_races(void);
int db5275_part_num(char *lv_search_str, char *str_in, char *file_name, int min_val, int max_val);
int db5290_skill_percentage(char *str_in, char *file_name);
void db10000_load_clans(void);
void db10050_load_clans_new();
void db10100_save_clans();
void db10200_create_clan(struct char_data * ch);
int db20000_copy_file(const char *from, const char *to);

short int db5280_skill_number(char *str_in, char *file_name);
char *db5250_parse_str(char *lv_search_str, char *str_in,
		         char *file_name, int max_len);
void db5300_load_classes(void);
void db5500_zone_update(void);
void db5600_reset_zone(int zone);
void db5625_print_error(char *lv_location, int lv_line, char lv_command, int lv_if_flag, int lv_arg1, int lv_arg2, int lv_arg3, char *lv_filename, int lv_flag);
int db5700_zone_reset_mob(struct char_data ** mob_ptr, int zone, int cmd_no, int lv_command, int lv_if_flag, int mob_num, int mob_qty, int the_room);
int db5710_zone_reset_object(int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int the_room);
int db5720_zone_reset_give(struct char_data * mob, int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int the_room);
int db5730_zone_reset_equip(struct char_data ** mob_ptr, int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int wear_loc);
int db5740_zone_reset_put(int zone, int cmd_no, int lv_command, int lv_if_flag, int obj_num, int obj_qty, int obj_container);
int db5750_zone_reset_doors(int zone, int cmd_no, int lv_command, int lv_if_flag, int the_room, int the_dir, int lock_type);
int db5760_zone_reset_remove(int zone, int cmd_no, int lv_command, int lv_if_flag, int item_num, int item_qty, int the_room);
int db5900_is_zone_empty(int zone_nr);
int db6000_load_char(char *name, struct char_file_u * char_element);
void db6100_store_to_char(struct char_file_u * st,
			    struct char_data * ch);
void db6200_char_to_store(struct char_data * ch,
			    struct char_file_u * st, bool UpdateTime);
int db6300_create_entry(char *name);
void db6400_save_char(struct char_data * ch, sh_int start_room);
void db6500_delete_char(struct char_data * ch, char *arg, int cmd);
void db6550_self_delete(struct char_data * ch, char *arg);
void db6600_set_password(struct char_data * ch, char *arg, int cmd);
char *db7000_fread_string(FILE * fl);
void db7100_free_char(struct char_data * ch, int lv_flag);
void db7200_free_obj(struct obj_data * obj);
char *db7300_file_to_string(char *name);
char *db7350_read_from_file(FILE * file_in);
void db7500_clear_char(struct char_data * ch);
void db7400_reset_char(struct char_data * ch);
void db7500_clear_char(struct char_data * ch);
void db7600_clear_object(struct obj_data * obj);
void db7700_init_char(struct char_data * ch);
void db7750_roll_stat_dice(struct char_data * ch, int lv_flag);
int db8000_real_room(int virtual);
int db8100_real_mobile(int virtual);
int db8200_real_object(int virtual);
int db8800_ok_to_load_obj(struct obj_data * obj, int av_obj_num, int lv_flag);
void db9000_save_zon(int zone);
void db9100_save_wld(int zone);
void db9300_save_mob(int zone);
void db9500_save_obj(int zone);
void db9600_save_area(int zone);
void db9700_global_save(void);
void db9800_load_save_aliases(struct char_data * ch,
			        int lv_direction);
int db9900_purge_check(struct char_file_u * lv_char,
		         int lv_flag);



void ft1200_update_pos(struct char_data * victim);
void ft1300_set_fighting(struct char_data * ch, struct char_data * vict, int lv_flag);
void ft1400_stop_fighting(struct char_data * ch, int lv_flag);
void ft1500_make_corpse(struct char_data * ch, struct char_data * killer);
void ft1700_death_cry(struct char_data * ch);
void ft1800_raw_kill(struct char_data * ch, struct char_data * killer);
void ft1850_remove_item_from_dying_char_and_auction_it(struct char_data * ch, struct char_data * killer);
int ft1900_special_procedure(struct char_data * ch);
void hit(struct char_data * ch, struct char_data * victim, int type);
void damage(struct char_data * ch, struct char_data * victim, int dam, int attacktype, int hits);
void ft2750_create_demon_flesh(struct char_data * victim);
void ft2405_damage_message(int dam, struct char_data * ch, struct char_data * victim, int w_type);



void h500_abort_program(struct char_data * ch, char *arg, int cmd);
char *ha1000_is_there_a_reserved_word(char *arg, const char *lv_reserved[], int lv_flag);
int ha1050_is_there_a_curse_word(char *arg);

/* handling the affected-structures */
void ha1225_affect_total(struct char_data * ch);
void ha1200_affect_modify(struct char_data * ch, signed char loc, sbyte mod, long bitv, bool add);
void ha1210_affect2_modify(struct char_data * ch, signed char loc, sbyte mod, long bitv, bool add);
void ha1300_affect_to_char(struct char_data * ch, struct affected_type * af);
void ha1300_affect_to_char2(struct char_data * ch, struct affected_type * af);
void ha1325_affect_remove(struct char_data * ch, struct affected_type * af, int lv_flag);
void ha1350_affect_from_char(struct char_data * ch, int skill);
void ha1370_remove_all_spells(struct char_data * ch);
bool ha1375_affected_by_spell(struct char_data * ch, int skill);
bool ha1375_affected_by_spell2(struct char_data * ch, int skill);
void ha1400_affect_join(struct char_data * ch, struct affected_type * af,
			  bool avg_dur, bool avg_mod);
void do_sys(char *arg, int min_level, struct char_data * ch);
void do_info(char *arg, int min_level, int max_level, struct char_data * ch);
void do_wizinfo(char *arg, int min_level, struct char_data * ch);
void do_connect(char *arg, int min_level, struct char_data * ch);
void do_connect_clan(char *arg, int min_level, struct char_data * ch);
struct obj_data *ha3400_create_money(int amount);
int ha1150_isname(char *str, char *namelist);
int ha1175_isexactname(char *str, char *namelist);
char *ha1100_fname(char *namelist);
void ha1700_obj_to_char(struct obj_data * object, struct char_data * ch);
int ha1750_remove_char_via_death_room(struct char_data * ch, int lv_how_left);
void ha1800_obj_from_char(struct obj_data * object);
struct char_data *ha3100_get_char_vis_ex(struct char_data * ch, char *name, int skipPets);

void ha1925_equip_char(struct char_data * ch, struct obj_data * obj, int pos, int lv_verbose, int lv_flag);
struct obj_data *ha1930_unequip_char(struct char_data * ch, int pos);

struct obj_data *ha2000_get_obj_list(char *name, struct obj_data * list);
struct obj_data *ha2025_get_obj_list_num(int num, struct obj_data * list);
struct char_data *ha2030_get_mob_list_num(int num, struct char_data * list);
int ha2050_count_objs_in_list(int num, struct obj_data * list);
struct obj_data *ha2000_get_obj_list_virtual(int num, struct obj_data * list);
struct obj_data *get_obj(char *name);
struct obj_data *ha2090_get_obj_using_num(int nr);

void ha2200_sort_obj_list(struct obj_data * object);
void ha2100_obj_to_room(struct obj_data * object, int room);
void ha2200_obj_from_room(struct obj_data * object);
void ha2300_obj_to_obj(struct obj_data * obj, struct obj_data * obj_to);
void ha2400_obj_from_obj(struct obj_data * obj);
void ha2500_object_list_new_owner(struct obj_data * list, struct char_data * ch);
int ha2600_move_all_chars_objs_to_another_room(struct char_data * ch, char *arg, int lv_real_to_room);
int pr2650_stat_cost(int lv_stat);
void ha2700_extract_obj(struct obj_data * obj);
void ha2900_update_char_objects(struct char_data * ch);
/* ******* characters ********* */
struct char_data *ha2100_get_char_in_room(char *name, int room);
struct char_data *ha2175_get_char_num(int nr);
struct char_data *get_char(char *name);
void ha1500_char_from_room(struct char_data * ch);
void ha1600_char_to_room(struct char_data * ch, int room);
int ha1650_all_from_room_to_room(int lv_from_room, int lv_to_room,
			     char *lv_form_msg, char *lv_to_msg, int lv_flag);

struct char_data *ha2125_get_char_in_room_vis(struct char_data * ch, char *name);
struct char_data *ha3100_get_char_vis(struct char_data * ch, char *name);
struct obj_data *ha2075_get_obj_list_vis(struct char_data * ch, char *name, struct obj_data * list);
struct obj_data *ha3200_get_obj_vis(struct char_data * ch, char *name);
void ha3000_extract_char(struct char_data * ch, int lv_extract_flag);
void ha3700_char_del_zone_objs(struct char_data * ch);

int ha3500_generic_find(char *arg, int bitvector, struct char_data * ch,
		      struct char_data ** tar_ch, struct obj_data ** tar_obj);
/* for following and stuff */
bool ha4000_circle_follow(struct char_data * ch, struct char_data * victim);
void ha4100_stop_follower(struct char_data * ch, int lv_stop_flag);
void ha4200_die_follower(struct char_data * ch, int lv_die_flag);
void ha4300_add_follower(struct char_data * ch, struct char_data * leader);
int ha5000_manually_open_door(int lv_room, int lv_dir, int lv_flag);
int ha5000_manually_open_door(int lv_room, int lv_dir, int lv_flag);
void ha9000_open_tracefile(int lv_direction);
void ha9700_set_saving_throws(struct char_data * ch, int lv_flag);
int ha9800_protect_char_from_dt(struct char_data * ch, int to_room, int lv_flag);
int ha9900_sanity_check(struct char_data * ch, char *lv_location, char *lv_name);
int ha9925_queue_check(struct txt_q * lv_queue, char *lv_location, char *lv_name);
int ha9950_maxed_on_followers(struct char_data * ch, struct char_data * victim, int lv_flag);

/* limits.c */
int li1300_mana_limit(struct char_data * ch);
int li1500_hit_limit(struct char_data * ch);
int li1600_move_limit(struct char_data * ch);
int li1700_mana_gain(struct char_data * ch);
int li1900_move_gain(struct char_data * ch);
int li1800_hit_gain(struct char_data * ch);
void li2100_set_title(struct char_data * ch);
void li2300_gain_condition(struct char_data * ch, int condition, int value);
void li2100_set_title(struct char_data * ch);
void li2000_advance_level(struct char_data * ch);
void li2200_gain_exp(struct char_data * ch, int gain);
void li2300_gain_nocheck_exp(struct char_data * ch, int gain);
void li2400_check_idling(struct char_data * ch);
void li2500_point_update(void);
int li3000_is_blocked_from_room(struct char_data * ch, int lv_real_room, int lv_flag);
void li3100_send_blocked_text(struct char_data * ch, int lv_real_room, int lv_flag);
int li9650_dex_adjustment(struct char_data * ch, int lv_flag);
int li9675_thaco(struct char_data * ch);
int li9700_adjust_damage_bonus(int lv_dex);
int li9725_adjust_tohit_bonus(int lv_dex);
int li9750_adjust_ac_bonus(int lv_dex);
int li9750_ac_bonus(int lv_dex);
void li9800_set_alignment(struct char_data * ch);
void li9850_set_conditions(struct char_data * ch);
void li9900_gain_proficiency(struct char_data * ch, int lv_skill);

bool li1000_can_see_char(struct char_data * ch, struct char_data * victim, int lv_flag);
bool li1100_can_see_obj(struct char_data * ch, struct obj_data * obj, int lv_flag);
bool li1200_is_it_dark(struct char_data * ch, int lv_room, int lv_flag);

int ma1000_do_get_random_room(struct char_data * ch, int lv_flag);

void mb1000_mobile_activity(void);
void mb1100_enfyrn_priest_changes_to_dragon(struct char_data * ch);
void mb1125_enfyrn_dragon_changes_to_priest(struct char_data * ch);
int mb1200_mob_artificial_int(struct char_data * ch, int lv_flag);
int mb1300_mob_cast(struct char_data * ch, int lv_spell);
int mb1400_pick_spell_skill(struct char_data * ch, int lv_type);
struct char_data *mb1500_pick_victim(struct char_data * ch, int lv_spell, int lv_flag);
int mb2000_uses_equipment(struct char_data * ch, int lv_type);
int mb2100_junks_equipment(struct char_data * ch, int lv_type);
int mb2200_equipment_thief(struct char_data * ch, int lv_type);
int mb2300_do_mob_move(struct char_data * ch, int lv_flag);
int mb2400_do_mob_close(struct char_data * ch, int lv_flag);

int r500_alias_get_filename(char *orig_name, char *filename);
int r1000_rent_get_filename(char *orig_name, char *filename);
int r1100_rent_delete_file(char *name);
int r1150_alias_delete_file(char *name);
void r2000_add_obj_cost(struct char_data * ch, struct char_data * re, struct obj_data * obj, struct obj_cost * cost, int lv_flag);
bool r2100_recep_offer(struct char_data * ch, struct char_data * receptionist, struct obj_cost * cost, int lv_flag);
void r2200_obj_store_to_char(struct char_data * ch, struct obj_data last_obj[100], struct obj_file_elem * st);
void r2300_del_char_objs(struct char_data * ch);
void r2400_load_char_objs(struct char_data * ch, int lv_backup);
void r3000_put_obj_in_store(struct obj_data * obj, struct obj_file_elem * st, int lv_state);
void r3100_obj_to_store(FILE * fl, struct obj_data * obj, struct obj_file_elem * st, struct char_data * ch, int *lv_state);
void r3200_obj_restore_weight(struct obj_data * obj);
void r3300_extract_all_objs(struct obj_data * obj);
void r3400_del_objs(struct char_data * ch);
void r4000_save_obj(struct char_data * ch, struct obj_cost * cost);
void r5000_compute_maxed_items(int lv_direction);
void r6000_do_offer(struct char_data * ch, char *arg, int cmd);
int r7000_func_receptionist(struct char_data * ch, int cmd, char *arg);
int r7200_func_banker(struct char_data * ch, int cmd, char *arg);
void r7300_do_quit(struct char_data * ch, char *arg, int cmd);
void r8000_do_backup(struct char_data * ch, char *arg, int cmd);

int pa2000_find_name(char *name);
int pa2050_find_name_approx(char *name);

void pr1000_lightning_strikes(int lv_flag);
void pr1005_strike_char(struct char_data * ch, char *arg, int cmd);
void pr1200_do_the_guild_no_arg(struct char_data * ch, struct char_data * victim, char *arg, int cmd);
int pr1400_do_guild(struct char_data * ch, int cmd, char *arg);
int pr1500_do_arena(struct char_data * ch, int cmd, char *arg);
int pr1600_do_dump(struct char_data * ch, int cmd, char *arg);
int pr4200_kings_hall(struct char_data * ch, int cmd, char *arg);
int pr3600_pet_shops(struct char_data * ch, int cmd, char *arg);
int pr3750_brim_pet_shop(struct char_data * ch, int cmd, char *arg);
int pr3450_cityguard(struct char_data * ch, int cmd, char *arg);
int pr3500_bountyhunter(struct char_data * ch, int cmd, char *arg);
int pr3200_slime(struct char_data * ch, int cmd, char *arg);
int pr3300_dungeon_master(struct char_data * ch, int cmd, char *arg);
int pr3400_gremlin(struct char_data * ch, int cmd, char *arg);
int pr4000_pray_for_items(struct char_data * ch, int cmd, char *arg);
int pr4300_elmpatris_innkeeper(struct char_data * ch, int cmd, char *arg);
int pr4350_elmpatris_fortune_teller(struct char_data * ch, struct char_data * lv_teller, int lv_flag);
int r7000_func_receptionist(struct char_data * ch, int cmd, char *arg);
int r7200_func_banker(struct char_data * ch, int cmd, char *arg);
int pr2700_guild_guard(struct char_data * ch, int cmd, char *arg);
int pr2900_puff(struct char_data * ch, int cmd, char *arg);
int pr2950_santa(struct char_data * ch, int cmd, char *arg);
int pr2799_camelot(struct char_data * ch, int cmd, char *arg);
int pr2800_gothelred(struct char_data * ch, int cmd, char *arg);
int pr2898_newbie_blesser(struct char_data * ch, int cmd, char *arg);
int pr2899_newbie_helper(struct char_data * ch, int cmd, char *arg);
int pr3000_fido(struct char_data * ch, int cmd, char *arg);
int pr3100_janitor(struct char_data * ch, int cmd, char *arg);
int pr1500_do_exit(struct char_data * ch, int cmd, char *arg);
int pr1700_mayor(struct char_data * ch, int cmd, char *arg);
int pr2200_snake(struct char_data * ch, int cmd, char *arg);
int pr2300_thief(struct char_data * ch, int cmd, char *arg);
int pr2400_magic_user(struct char_data * ch, int cmd, char *arg);
int pr2600_high_priest(struct char_data * ch, int cmd, char *arg);
int bt2000_bounty_master(struct char_data * ch, int cmd, char *arg);

int pc1000_gerinshill_room_1414(struct char_data * ch, int cmd, char *arg);
int pc1100_gerinshill_dentures_key(struct char_data * ch, int cmd, char *argument);
int pc1200_gerinshill_room_1496(struct char_data * ch, int cmd, char *argument);
int pc1300_newbie_book(struct char_data * ch, int cmd, char *arg);
int pc1400_island3_parts(struct char_data * ch, int cmd, char *argument);
int pc1500_glass_door(struct char_data * ch, int cmd, char *arg);
int pc1550_darkcastle_portal(struct char_data * ch, int cmd, char *arg);
int pc1600_healer(struct char_data * ch, int cmd, char *arg);
int pc1700_special_sword(struct char_data * ch, int cmd, char *arg);
int pc1800_killer_womble(struct char_data * ch, int cmd, char *arg);
int pc1900_slot_machine(struct char_data * ch, int cmd, char *arg);
int pc2000_darken(struct char_data * ch, int cmd, char *arg);
int pc2100_rome(struct char_data * ch, int cmd, char *arg);
int pc2200_sretaw(struct char_data * ch, int cmd, char *arg);

int vh500_get_vehicle_room(struct char_data * ch, char *arg, int cmd, struct obj_data * lv_obj);

void vh550_set_vehicle_description(struct obj_data * lv_obj, int lv_veh_room);
struct obj_data *vh700_get_vehicle_object(struct char_data * ch, char *arg, int cmd, int lv_veh_room);

void vh1000_do_board(struct char_data * ch, char *arg, int cmd);
int vh1200_put_char_on_vehicle(struct char_data * ch, struct obj_data * lv_obj, int lv_veh_room);
void vh1300_exit_from_vehicle(struct char_data * ch, char *arg, int cmd);
int vh1400_remove_char_from_vehicle(struct char_data * ch, struct obj_data * lv_obj);
void vh1450_remove_all_chars_from_vehicle(struct obj_data * lv_obj, int lv_veh_room);
void vh2000_move_vehicles();
void vh2100_move_a_single_vehicle(struct obj_data * lv_obj, int lv_room);
int vh2200_get_vehicle_direction(struct obj_data * lv_obj, int lv_veh_room);
int vh3000_vehicle_proc(struct char_data * ch, int cmd, char *arg);

void at2000_do_mob_hunt_check(struct char_data * ch, struct char_data * vict, int flag);
void at2100_do_mob_hunt_msgs(struct char_data * ch, struct char_data * vict, int flag);
int remove_ALL_dot(char *argument);

void hu9000_call_for_help(struct char_data * ch, struct char_data * victim, int lv_flag);
void wi2700_do_load_hunt(struct char_data * ch, char *arg, int cmd);
int wi3150_return_avatar_level(struct char_data * ch);
void do_hunt(struct char_data * ch, char *arg, int cmd);
void hunt_victim(struct char_data * ch, char *arg, int cmd, int status);
void do_track(struct char_data * ch, char *arg, int cmd);
void find_victim(struct char_data * ch, char *arg, int cmd, int status, int loop);
void do_find(struct char_data * ch, char *arg, int cmd);
void in1000_do_show_races(struct char_data * ch, char *arg, int cmd);
void in1100_do_show_classes(struct char_data * ch, char *arg, int cmd);
void in1200_do_color(struct char_data * ch, char *arg, int cmd);
void in1500_do_afk(struct char_data * ch, char *arg, int cmd);
void do_afw(struct char_data * ch, char *arg, int cmd);
void in5300_update_top_list(struct char_data * ch, int list);
void in5400_show_top_list(struct char_data * ch, int list);
void in5500_build_top_lists(void);
void in5600_do_top(struct char_data * ch, char *arg, int cmd);
void in5700_build_top_score_list(int min_level, int max_level);
void do_xyzzy(struct char_data * ch, char *arg, int cmd);
void do_disable(struct char_data * ch, char *arg, int cmd);
void do_wizpermit(struct char_data *ch, char *arg, int cmd);
void do_disconnect(struct char_data * ch, char *arg, int cmd);
void in1600_do_diag(struct char_data * ch, char *arg, int cmd);
char *in1650_do_short_diag(struct char_data * ch, char *arg, int cmd);
int wi3600_do_cmd_blocked_by_freeze(struct char_data * ch, char *arg, int cmd);
int wi3700_do_cmd_blocked_by_hold_person(struct char_data * ch, char *arg, int cmd);
int wi3800_do_cmd_blocked_by_jail(struct char_data * ch, char *arg, int cmd);
int get_jail_time(long lv_end_jail, int lv_type);
int get_hit_time(long lv_end_jail, int lv_type);
int do_jail_check(struct char_data * ch, char *arg, int cmd);
void do_date(struct char_data * ch, char *arg, int cmd);
void in1900_do_system(struct char_data * ch, char *arg, int cmd);
void do_alias(struct char_data * ch, char *arg, int cmd);
void do_port(struct char_data * ch, char *arg, int cmd);
void in3000_do_look(struct char_data * ch, char *arg, int cmd);
int in3050_do_vehicle_look(struct char_data * ch, char *arg, int cmd);
void in3100_do_read(struct char_data * ch, char *arg, int cmd);
void do_port(struct char_data * ch, char *arg, int cmd);
void do_say(struct char_data * ch, char *arg, int cmd);
void do_exit(struct char_data * ch, char *arg, int cmd);
void wi2200_do_snoop(struct char_data * ch, char *arg, int cmd);
void do_insult(struct char_data * ch, char *arg, int cmd);
void ot1300_do_quit(struct char_data * ch, char *arg, int cmd);
void do_qui(struct char_data * ch, char *arg, int cmd);
void in3500_do_help(struct char_data * ch, char *arg, int cmd);
void do_who(struct char_data * ch, char *arg, int cmd);
void wi1000_do_emote(struct char_data * ch, char *arg, int cmd);
void wi1100_do_echo(struct char_data * ch, char *arg, int cmd);
void wi1200_do_gecho(struct char_data * ch, char *arg, int cmd);
void wi1300_do_trans(struct char_data * ch, char *arg, int cmd);
void at1100_do_kill(struct char_data * ch, char *arg, int cmd);
void wi2500_do_force(struct char_data * ch, char *arg, int cmd);
void do_get(struct char_data * ch, char *arg, int cmd);
void do_drop(struct char_data * ch, char *arg, int cmd);
void do_donate(struct char_data * ch, char *arg, int cmd);
void in4300_do_news(struct char_data * ch, char *arg, int cmd);
void in4350_do_todo(struct char_data * ch, char *arg, int cmd);
void in4400_do_wipe(struct char_data * ch, char *arg, int cmd);
void in1800_do_score(struct char_data * ch, char *arg, int cmd);
void in4000_do_inventory(struct char_data * ch, char *arg, int cmd);
void in4100_do_equipment(struct char_data * ch, char *arg, int cmd);
char *in4960_who_has_the_object(struct char_data * ch, struct obj_data * obj, int lv_flag);
void do_shout(struct char_data * ch, char *arg, int cmd);
void ot1500_do_not_here(struct char_data * ch, char *arg, int cmd);
void do_tell(struct char_data * ch, char *arg, int cmd);
void do_wear(struct char_data * ch, char *arg, int cmd);
void do_find_wear_location(struct char_data * ch, struct obj_data * lv_obj, int lv_flag);
void do_wield(struct char_data * ch, char *arg, int cmd);
void do_grab(struct char_data * ch, char *arg, int cmd);
void do_remove(struct char_data * ch, char *arg, int cmd);
void do_put(struct char_data * ch, char *arg, int cmd);
void wi2100_do_shutdown(struct char_data * ch, char *arg, int cmd);
void ot1400_do_save(struct char_data * ch, char *arg, int cmd);
void at1000_do_hit(struct char_data * ch, char *arg, int cmd);
void do_string(struct char_data * ch, char *arg, int cmd);
void do_give(struct char_data * ch, char *arg, int cmd);
void in1700_do_attribute(struct char_data * ch, char *arg, int cmd);
void wi1600_do_stat(struct char_data * ch, char *arg, int cmd);
void do_set(struct char_data * ch, char *arg, int cmd);
void do_time(struct char_data * ch, char *arg, int cmd);
void in3400_do_weather(struct char_data * ch, char *arg, int cmd);
void wi2700_do_load(struct char_data * ch, char *arg, int cmd);
void wi2800_do_purge(struct char_data * ch, char *arg, int cmd);
void wi2900_purge_items(struct char_data * ch, char *arg, int cmd);
int wi2920_create_room_links(int lv_from_real_room, int lv_to_real_room, int lv_dir, char *gen_desc);
void wi2925_remove_room_links(int lv_virtual_room, int lv_dir);
void wi2950_remove_virtual_obj_from_room(int lv_obj_num, int lv_room);
void do_shutdow(struct char_data * ch, char *arg, int cmd);
void ot2100_do_idea(struct char_data * ch, char *arg, int cmd);
void ot2200_do_typo(struct char_data * ch, char *arg, int cmd);
void ot2300_do_bug(struct char_data * ch, char *arg, int cmd);
void do_whisper(struct char_data * ch, char *arg, int cmd);
void do_cast(struct char_data * ch, char *arg, int cmd);
void do_cast_proc(struct char_data * ch, struct char_data * cast_victim, char *arg, int cmd, int lv_flag);
void wi1400_do_at(struct char_data * ch, char *arg, int cmd);
void wi1500_do_goto(struct char_data * ch, char *arg, int cmd);
void do_ask(struct char_data * ch, char *arg, int cmd);
void do_drink(struct char_data * ch, char *arg, int cmd);
void do_eat(struct char_data * ch, char *arg, int cmd);
void do_pour(struct char_data * ch, char *arg, int cmd);
void do_sip(struct char_data * ch, char *arg, int cmd);
void do_taste(struct char_data * ch, char *arg, int cmd);
void at1300_do_order(struct char_data * ch, char *arg, int cmd);
void do_rent(struct char_data * ch, char *arg, int cmd);
void do_offer(struct char_data * ch, char *arg, int cmd);
void wi3200_do_advance(struct char_data * ch, char *arg, int cmd);
void in3300_do_exits(struct char_data * ch, char *arg, int cmd);
void do_write(struct char_data * ch, char *arg, int cmd);
void at1400_do_flee(struct char_data * ch, char *arg, int cmd);
void ot1600_do_sneak(struct char_data * ch, char *arg, int cmd);
void ot1800_do_hide(struct char_data * ch, char *arg, int cmd);
void ot1850_do_envenom(struct char_data * ch, char *argument, int cmd);
void at1200_do_backstab(struct char_data * ch, char *arg, int cmd);
void ot1900_do_steal(struct char_data * ch, char *arg, int cmd);
void at1500_do_bash(struct char_data * ch, char *arg, int cmd);
void at2400_do_trip(struct char_data * ch, char *arg, int cmd);
void at1600_do_rescue(struct char_data * ch, char *arg, int cmd);
void at1650_do_stab(struct char_data * ch, char *argument, int cmd);
void at1700_do_kick(struct char_data * ch, char *arg, int cmd);
void in3200_do_examine(struct char_data * ch, char *arg, int cmd);
void in4500_do_view_info(struct char_data * ch, char *arg, int cmd);
void in3900_do_users(struct char_data * ch, char *arg, int cmd);
void in4800_do_where(struct char_data * ch, char *arg, int cmd);
void in4950_where_object_argument(struct char_data * ch, char *arg, int cmd, int lv_flag);
void in4975_whererent(struct char_data * ch, char *arg, int cmd);
void in5000_do_levels(struct char_data * ch, char *arg, int cmd);
void wi3300_do_reroll(struct char_data * ch, char *arg, int cmd);
void ot2400_do_brief(struct char_data * ch, char *arg, int cmd);
void in4600_do_syslog(struct char_data * ch, char *arg, int cmd);
void in4700_do_wizlist(struct char_data * ch, char *arg, int cmd);
void in5100_do_consider(struct char_data * ch, char *arg, int cmd);
void ot3000_do_group(struct char_data * ch, char *arg, int cmd);
void wi3400_do_restore(struct char_data * ch, char *arg, int cmd);
void wi3450_actually_restore_char(struct char_data * ch, struct char_data * victim);
void wi2400_do_return(struct char_data * ch, char *arg, int cmd);
void wi2300_do_switch(struct char_data * ch, char *arg, int cmd);
void ot3100_do_quaff(struct char_data * ch, char *arg, int cmd);
void ot3200_do_recite(struct char_data * ch, char *arg, int cmd);
void ot3300_do_use(struct char_data * ch, char *arg, int cmd);
void do_pose(struct char_data * ch, char *arg, int cmd);
void in3600_do_wizhelp(struct char_data * ch, char *arg, int cmd);
void in4200_do_credits(struct char_data * ch, char *arg, int cmd);
void ot2500_do_compact(struct char_data * ch, char *arg, int cmd);
void ot1700_do_title(struct char_data * ch, char *arg, int cmd);
void do_action(struct char_data * ch, char *arg, int cmd);
void ot2000_do_practice(struct char_data * ch, char *arg, int cmd);
void ot1200_do_junk(struct char_data * ch, char *arg, int cmd);
void in3800_do_spells(struct char_data * ch, char *arg, int cmd);
void at1800_do_breath(struct char_data * ch, char *arg, int cmd);
void at2200_do_rage(struct char_data * ch, char *arg, int cmd);
void at2300_do_escape(struct char_data * ch, char *argument, int cmd);

void ot1100_do_visible(struct char_data * ch, char *arg, int cmd);
void ot3500_do_withdraw(struct char_data * ch, char *arg, int cmd);
void ot3600_do_balance(struct char_data * ch, char *arg, int cmd);
void ot3400_do_deposit(struct char_data * ch, char *arg, int cmd);
void do_rhelp(struct char_data * ch, char *arg, int cmd);
void do_rcopy(struct char_data * ch, char *arg, int cmd);
int do_rcopy_for_vehicle(struct char_data * ch, char *arg, int cmd);
void do_rlink(struct char_data * ch, char *arg, int cmd);
void do_rsect(struct char_data * ch, char *arg, int cmd);
void do_rflag(struct char_data * ch, char *arg, int cmd);
void do_rdesc(struct char_data * ch, char *arg, int cmd);
void do_rsave(struct char_data * ch, char *arg, int cmd);
void do_rname(struct char_data * ch, char *arg, int cmd);
void do_rlflag(struct char_data * ch, char *arg, int cmd);
void do_rlist(struct char_data * ch, char *arg, int cmd);
void do_gossip(struct char_data * ch, char *arg, int cmd);
void do_imp(struct char_data * ch, char *arg, int cmd);
void do_over(struct char_data * ch, char *arg, int cmd);
void do_high(struct char_data * ch, char *arg, int cmd);
void do_ante(struct char_data * ch, char *arg, int cmd);
void do_avatar(struct char_data * ch, char *arg, int cmd);
void do_music(struct char_data * ch, char *arg, int cmd);
void do_pkflame(struct char_data * ch, char *arg, int cmd);
void do_immtalk(struct char_data * ch, char *arg, int cmd);
void do_spirittalk(struct char_data * ch, char *arg, int cmd);
void do_auction(struct char_data * ch, char *arg, int cmd);
void wi4000_do_poofin(struct char_data * ch, char *arg, int cmd);
void do_poofout(struct char_data * ch, char *arg, int cmd);
void in5200_do_ansi(struct char_data * ch, char *arg, int cmd);
void in5300_do_report(struct char_data * ch, char *arg, int cmd);
void do_olist(struct char_data * ch, char *arg, int cmd);
void do_mlist(struct char_data * ch, char *arg, int cmd);
void do_oflag(struct char_data * ch, char *arg, int cmd);
void do_oflag2(struct char_data * ch, char *arg, int cmd);
void do_holler(struct char_data * ch, char *arg, int cmd);
void do_ostat(struct char_data * ch, char *arg, int cmd);
void do_obj_report(struct char_data * ch, char *arg, int cmd);
void do_mob_report(struct char_data * ch, char *arg, int cmd);
void do_operc(struct char_data * ch, char *arg, int cmd);
void in3700_do_commands(struct char_data * ch, char *arg, int cmd);
void do_mflag(struct char_data * ch, char *arg, int cmd);
void do_osave(struct char_data * ch, char *arg, int cmd);
void do_msave(struct char_data * ch, char *arg, int cmd);
void do_mstat(struct char_data * ch, char *arg, int cmd);
void do_maffect(struct char_data * ch, char *arg, int cmd);
void do_rgoto(struct char_data * ch, char *arg, int cmd);
void do_rstat(struct char_data * ch, char *arg, int cmd);
void ot2600_do_assist(struct char_data * ch, char *arg, int cmd);
void ot2700_do_wimpy(struct char_data * ch, char *arg, int cmd);
void wi2600_do_vnum(struct char_data * ch, char *arg, int cmd);
void ot2800_do_display(struct char_data * ch, char *arg, int cmd);
void do_oclass(struct char_data * ch, char *arg, int cmd);
void do_owflag(struct char_data * ch, char *arg, int cmd);
void do_ovalues(struct char_data * ch, char *arg, int cmd);
void do_zlist(struct char_data * ch, char *arg, int cmd);
void do_ocost(struct char_data * ch, char *arg, int cmd);
void do_orent(struct char_data * ch, char *arg, int cmd);
void do_oweight(struct char_data * ch, char *arg, int cmd);
void do_okeywords(struct char_data * ch, char *arg, int cmd);
void do_otype(struct char_data * ch, char *arg, int cmd);
void do_osdesc(struct char_data * ch, char *arg, int cmd);
void do_oldesc(struct char_data * ch, char *arg, int cmd);
void do_owear(struct char_data * ch, char *arg, int cmd);
void do_oaffect(struct char_data * ch, char *arg, int cmd);
void do_ocreate(struct char_data * ch, char *arg, int cmd);
void do_ocopy(struct char_data * ch, char *arg, int cmd);
void do_msdesc(struct char_data * ch, char *arg, int cmd);
void do_mldesc(struct char_data * ch, char *arg, int cmd);
void do_mdesc(struct char_data * ch, char *arg, int cmd);
void do_mhitroll(struct char_data * ch, char *arg, int cmd);
void do_mhitpoints(struct char_data * ch, char *arg, int cmd);
void do_mdamage(struct char_data * ch, char *arg, int cmd);
void do_mcreate(struct char_data * ch, char *arg, int cmd);
void do_mcopy(struct char_data * ch, char *arg, int cmd);
void do_zreset(struct char_data * ch, char *arg, int cmd);
void do_mkeywords(struct char_data * ch, char *arg, int cmd);
void do_invisible(struct char_data * ch, char *arg, int cmd);
void wi4100_do_crashsave(struct char_data * ch, char *arg, int cmd);
void wi3900_do_muzzle(struct char_data * ch, char *arg, int cmd);

void do_zedit(struct char_data * ch, char *arg, int cmd);
void bn1300_do_ban(struct char_data * ch, char *arg, int cmd);
void do_unban(struct char_data * ch, char *arg, int cmd);
void ot2900_do_auto(struct char_data * ch, char *arg, int cmd);
void ot3800_do_split(struct char_data * ch, char *arg, int cmd);
void ot3900_do_climb(struct char_data * ch, char *arg, int cmd);
struct obj_data *ot4000_check_spell_store_item(struct char_data * ch, int av_spell);
void ot4100_do_sense(struct char_data * ch, char *arg, int cmd);
void ot4200_do_wizlist(struct char_data * ch, char *arg, int cmd);
void ot4300_do_rushtime(struct char_data * ch, char *arg, int cmd);
void ot4400_do_rushhour(struct char_data * ch, char *arg, int cmd);
void ot4500_do_for(struct char_data * ch, char *argument, int cmd);
void ot4600_do_forget(struct char_data * ch, char *argument, int cmd);
int ot4700_count_foreign_spells(struct char_data * ch);
void ot4800_do_airwalk(struct char_data * ch, char *arg, int cmd);
void ot5000_transform_update(void);
void ot5100_wraps_procedure(struct char_data * ch, struct obj_data * obj);
void advance_pk_level(struct char_data * ch, char *arg, int cmd);

void do_zhelp(struct char_data * ch, char *arg, int cmd);
void do_zcreate(struct char_data * ch, char *arg, int cmd);
void do_zsave(struct char_data * ch, char *arg, int cmd);
void do_msex(struct char_data * ch, char *arg, int cmd);
void do_zfirst(struct char_data * ch, char *arg, int cmd);
void do_zreboot(struct char_data * ch, char *arg, int cmd);
void do_zmax(struct char_data * ch, char *arg, int cmd);
void cr0000_do_zset(struct char_data * ch, char *arg, int cmd);
void do_zstat(struct char_data * ch, char *arg, int cmd);
void do_zflag(struct char_data * ch, char *arg, int cmd);
void do_redesc(struct char_data * ch, char *arg, int cmd);
void do_oedesc(struct char_data * ch, char *arg, int cmd);
void do_ohelp(struct char_data * ch, char *arg, int cmd);
void do_mhelp(struct char_data * ch, char *arg, int cmd);
struct obj_data *in2300_get_object_in_equip_vis(struct char_data * ch, char *arg, struct obj_data * equipment[], int *j);
char *in2400_find_ex_description(char *word, struct extra_descr_data * list);
void weight_change_object(struct obj_data * obj, int weight);
int mo1000_do_simple_move(struct char_data * ch, int cmd, int following, int lv_flag);
void mo1100_do_scan(struct char_data * ch, char *arg, int cmd);
void mo1200_scan_room_and_print(struct char_data * ch, char *arg, int *lv_current_room, int lv_pass_count, int *lv_current_move, int lv_dir);
void mo1500_do_move(struct char_data * ch, char *arg, int cmd);
int mo1600_find_door(struct char_data * ch, char *type, char *dir);
void mo1700_do_open(struct char_data * ch, char *arg, int cmd);
void mo1800_do_close(struct char_data * ch, char *arg, int cmd);
void mo1900_check_key_usage(struct char_data * ch, struct obj_data * obj);
int mo2000_has_key(struct char_data * ch, int key, int lv_flag);
void mo2100_do_lock(struct char_data * ch, char *arg, int cmd);
void mo2200_do_unlock(struct char_data * ch, char *arg, int cmd);
void mo2300_do_pick(struct char_data * ch, char *arg, int cmd);
void mo2400_do_enter(struct char_data * ch, char *arg, int cmd);
int mo2500_enter_portal(struct char_data * ch, int cmd, char *arg);
void mo3000_do_leave(struct char_data * ch, char *arg, int cmd);
void mo3100_do_stand(struct char_data * ch, char *arg, int cmd);
void mo3200_do_sit(struct char_data * ch, char *arg, int cmd);
void mo3500_do_rest(struct char_data * ch, char *arg, int cmd);
void mo3600_do_sleep(struct char_data * ch, char *arg, int cmd);
void mo3700_do_wake(struct char_data * ch, char *arg, int cmd);
void mo4000_do_follow(struct char_data * ch, char *arg, int cmd);
void mo4100_do_speedwalk(struct char_data * ch, char *arg, int cmd);
void survivor_poison(struct char_data * ch);
void survivor_blindness(struct char_data * ch);

void roll_abilities(struct char_data * ch);
void wi3000_create_level_one_char(struct char_data * ch, int lv_flag);
void co1250_increase_max_users();
int co1300_get_from_q(struct txt_q * queue, char *dest, int *aliased);
int co1000_run_the_game(int port);
int co1900_new_connection(int s);
int co2000_new_descriptor(int s);
int co2100_process_output(struct descriptor_data * t);
int process_input(struct descriptor_data * t);
void co2500_close_single_socket(struct descriptor_data * d);
void co1700_flush_queues(struct descriptor_data * d);
void co2600_nonblock(int s);
int parse_name(struct char_data * ch, unsigned char *arg, char *name);
void affect_update(void);	/* In spells.c */
void li2500_point_update(void);	/* In limits.c */
void string_add(struct descriptor_data * d, char *str);
void ft3100_perform_violence(void);
void perform_hunt(void);
void show_string(struct descriptor_data * d, char *input);
void gr(int s);
void check_reboot(void);
void ft1100_load_message(void);
void weather_and_time(int mode);
void assign_command_pointers(void);
void assign_spell_pointers(void);
void as1000_assign_mobiles(void);
void as1100_assign_objects(void);
void as1150_assign_objecst2(void);
void as1200_assign_rooms(void);
void boot_social_messages(void);
void boot_pose_messages(void);
void update_obj_file(void);	/* In reception.c */
struct help_index_element *build_help_index(FILE * fl, int *num);

int number(int from, int to);
int dice(int number, int size);
char *str_alloc(char *source);
char *str_free(char *source);
int str_cmp(char *arg1, char *arg2);
int strn_cmp(char *arg1, char *arg2, int n);
void main_log(char *str);
void trace_log(char *str);
void sprinttype(int type, const char *names[], char *result);
void sprintbit(long vektor, const char *names[], char *result);
struct time_info_data z_age(struct char_data * ch);
struct time_info_data mud_time_passed(time_t t2, time_t t1);
long adjust_time(time_t time_in, int lv_year, int lv_month, int lv_day, int lv_hour);
void say_spell(struct char_data * ch, int si);

void do_disable(struct char_data * ch, char *arg, int cmd);
void do_spell_disable(struct char_data * ch, char *arg, int cmd);

void do_set_char_statgold(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_statcount(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_skill(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_deathroom(struct char_data * ch, char *arg, int cmd);
void do_set_lines(struct char_data * ch, char *arg, int cmd);
void do_set_graphics(struct char_data * ch, char *arg, int cmd);
void do_set_nameserver(struct char_data * ch, char *arg);
void do_set_pklevel(struct char_data * ch, char *arg);

int si1000_nosferatu(struct char_data * ch, int cmd, char *arg);
int sp1000_citym_rainbow_box(struct char_data * ch, int cmd, char *arg);
int sp1050_mithril_fountain(struct char_data * ch, int cmd, char *arg);
int sp1250_qelves_dias(struct char_data * ch, int cmd, char *arg);
int sp1500_arcain_chest(struct char_data * ch, int cmd, char *arg);
int sp2000_cloud_stone(struct char_data * ch, int cmd, char *arg);
int sp2100_cloud_bookshelf(struct char_data * ch, int cmd, char *arg);
int sp2200_clan_teleporter_button(struct char_data * ch, int cmd, char *arg);
int sp2300_load_ammo(struct char_data * ch, int cmd, char *arg);
void sp2400_do_shoot(struct char_data * ch, char *argument, int cmd);

void oj5000_wear_obj(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose);
void oj5100_wear_check_special(struct char_data * ch, struct obj_data * obj_object, int keyword, int lv_verbose);
void do_fill(struct char_data * ch, char *arg, int cmd);
void boot_the_shops(void);
void assign_the_shopkeepers(void);
int board(struct char_data * ch, int cmd, char *arg);
int forge(struct char_data * ch, int cmd, char *arg);
int chalice(struct char_data * ch, int cmd, char *arg);
int citadel_altar(struct char_data * ch, int cmd, char *arg);
int halfling_smith(struct char_data * ch, int cmd, char *arg);
int swamp_priest(struct char_data * ch, int cmd, char *arg);
int cliff(struct char_data * ch, int cmd, char *arg);
int ut9000_update_player_file(struct char_data * ch, char *arg, int cmd);

void wz1000_do_jail(struct char_data * ch, char *arg, int cmd);
void wz1100_do_freeze(struct char_data * ch, char *arg, int cmd);
void wz1200_do_nohassle(struct char_data * ch, char *arg, int cmd);
void wz1300_do_go30(struct char_data * ch, char *arg, int cmd);
int wz1500_did_we_make_teleport(struct char_data * ch, int lv_from_room, int lv_to_room, int av_tele_skill, int av_party_count);
void wz1600_do_bless(struct char_data * ch, char *arg, int cmd);
void wz1700_do_phase(struct char_data * ch, char *arg, int cmd);
void wz1800_do_finger(struct char_data * ch, char *arg, int cmd);
void wz1900_do_email(struct char_data * ch, char *arg, int cmd);
void wz2000_do_peace(struct char_data * ch, char *arg, int cmd);
void do_oset(struct char_data * ch, char *argument, int cmd);
void do_otransfer(struct char_data * ch, char *arg, int cmd);

int sm1000_midgaard_cityguard(struct char_data * ch, int cmd, char *arg);
int sm1100_midgaard_jinipan(struct char_data * ch, int cmd, char *arg);

int fi1000_door_knocker(struct char_data * ch, int cmd, char *arg);

//void do_mount2(struct char_data * ch, int cmd, char *arg);
void do_ride(struct char_data * ch, char *argument, int cmd);
void do_dismount(struct char_data * ch, char *argument, int cmd);

//Incognito & ghost Shalira 22.07 .01
void do_incognito(struct char_data * ch, char *arg, int cmd);
void do_ghost(struct char_data * ch, char *arg, int cmd);

//New logs Shalira 22.07 .01
void spec_log(char *str, char *logname);

//new logs Shalira 28.07 .01
void do_logview(struct char_data * ch, char *arg, int cmd);
void do_logsearch(struct char_data * ch, char *arg, int cmd);
int get_line(FILE * fl, char *buf);
char *str_stra(const char *str, const char *txt);

//load wiztitles 02.05 .02 Hmm

struct wiztitle_data *getwiztitlestruct(char *name);
int getwiztitlecount(char *name);
int wiztitlecount(char *name);
char *getwiztitleindex(char *name, int index);
int removewiztitle(char *name, int index);
int setwiztitle(char *name, char *title);
int addwiztitle(char *name, char *title);
void deletewiztitles();
char *getwiztitle(char *name);
void loadwiztitles();
int savewiztitles();
int clrstrlen(char *title);
void cropclrstrlen(char *title, int len);
void formatwiztitle(char *title);
void setupwiztitles();

void do_wiztitle(struct char_data * ch, char *arg, int cmd);

int does_player_exist(char *name);

void sr2000_do_replenish(struct char_data * ch, char *arg, int cmd);
int bt1250_can_attack(struct char_data * killer, struct char_data * vict);
void bt2100_do_bounty(struct char_data * ch, char *arg, int cmd);
int bt2200_bounty_hunter(struct char_data * ch, int cmd, char *arg);

int oa1100_test_function(struct obj_data * obj, int command, char *arg);


//assorted missing declarations.looks like developing on mac is different

void oa1000_object_activity(void);
//objact.c
char *skip_spaces(char *string);
//spells.c
int r1125_backup_delete_file(char *name);
//rent.c
char *any_one_arg(char *argument, char *first_arg);
//parser.c
void ha2800_update_object_timer(struct obj_data * obj, int use);
//handler.c
void page_string(struct descriptor_data * d, char *str, int keep_internal);
//modify.c
int pr4050_char_is_cursed(struct char_data * ch, char *arg, int cmd);
//proc1.c
void ft2400_damage_message(int dam, struct char_data * ch, struct char_data * victim, int w_type);
//fight.c
void do_set_flag(struct char_data * ch, char *arg, int cmd, struct char_data * victim);
//modify.c
void special_restore(struct char_data * ch, struct char_data * vict);
//skills.c
void db2500_allocate_quest(int new_top);
//db.c
struct keyword_data *db4750_split_keywords(char *arg);
//db.c
int vh2500_damage_chars_in_room(struct obj_data * lv_obj);
//vehicle.c
int ut9500_rebuild_equipment_file(char *lv_name, int obj_loc[32767]);
//utility.c
int bt1200_bounty_check(struct char_data * ch);
//bounty.c
int ha5100_manually_close_door(int lv_room, int lv_dir, int lv_flag);
//handler.c
int pc2300_teleport_room(struct char_data * ch, int cmd, char *arg);
//proc2.c
int pc2400_cliff(struct char_data * ch, int cmd, char *arg);
//proc2.c
int pc2500_maxlvl20(struct char_data * ch, int cmd, char *arg);
int cows_room(struct char_data * ch, int cmd, char *arg);
//proc2.c
int council_room(struct char_data * ch, int cmd, char *arg);
//proc2.c

// int ft2800_spec_damage(struct char_data * ch, struct char_data * victim, int dam, struct obj_data * wielded) //fight.c
