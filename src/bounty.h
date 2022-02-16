/* type definitions and function prototypes for bounty.c */

struct bounty_info {
	char *name;		/* Name of the character */
	char *placed_by;	/* Who placed the bounty? */
	int amount;		/* How big is the bounty? */
	int killed;		/* Once they've been killed once */
	int killable;		/* If 1, kill able by all characters */
	struct bounty_info *next;	/* Pointer to the next in the list */
};
/* Global variable, head of the bounty linked list */
struct bounty_info *bounty_list;

/* functions implemented */

void bt1000_write_bounty_list(void);
void bt1100_load_bounty(void);
int bt1200_bounty_check(struct char_data * ch);
int bt1250_can_attack(struct char_data * killer, struct char_data * vict);
void bt1300_remove_bounty(struct char_data * ch);
struct obj_data *bt1400_make_head(struct char_data * ch, int loot);
int bt2000_bounty_master(struct char_data * ch, int cmd, char *arg);
void bt2100_do_bounty(struct char_data * ch, char *arg, int cmd);
int bt2200_bounty_hunter(struct char_data * ch, int cmd, char *arg);
int bt2300_perform_mob_bounty(struct char_data * mob, struct char_data * victim);
