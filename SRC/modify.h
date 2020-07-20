/* modify.h - soon to be history right along with modify.c */

void show_string(struct descriptor_data * d, char *input);

void do_set_char(struct char_data * ch, char *arg, int cmd);
void do_set_login(struct char_data * ch, char *arg, int cmd);
void do_set_trace(struct char_data * ch, char *arg, int cmd);
void do_set_channel(struct char_data * ch, char *arg, int cmd);
void do_set_flag(struct char_data * ch, char *arg, int cmd, struct char_data * victim);
void do_set_display(struct char_data * ch, char *arg, int cmd);
void do_set_prompt(struct char_data * ch, char *arg, int cmd);
void do_set_auto_level(struct char_data * ch, char *arg, int cmd);
void do_set_start(struct char_data * ch, char *arg, int cmd);
void do_set_uptime(struct char_data * ch, char *arg, int cmd);
void do_set_char_password(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_age(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_str(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_int(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_wis(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_dex(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_con(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_cha(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_hit(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_max_hit(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_mana(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_max_mana(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_move(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_max_move(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_start(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_private(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_gold(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_bank(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_inn(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_exp(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_level(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_practices(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_alignment(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_drunk(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_hunger(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_thirst(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_act1(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_act2(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_act3(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_carry_weight(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_carry_items(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_race(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_class(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_sex(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_ac(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_hitroll(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_dmgroll(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_position(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_visible(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_deaths(struct char_data * ch, char *arg, int cmd, struct char_data * vict);
void do_set_char_extrahits(struct char_data * ch, char *arg, int cmd, struct char_data * vict);

void page_string(struct descriptor_data * d, char *str, int keep_internal);