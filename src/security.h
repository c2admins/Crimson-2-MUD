/*
Header file for security.c
Written by Relic
*/

/* Hardcode list struct definition */
struct hardcode_list_type {
	char name[30];
	int level;
	int port;
};
/* Global variables */
struct hardcode_list_type hardcode_list[];

/* Functions implemented in security.c */
void sr1000_restore_char(struct char_data * ch);
void sr2000_do_replenish(struct char_data * ch, char *arg, int cmd);
int sr3000_is_name_secure(char *arg);
void sr3100_show_hardcoded_list(struct char_data * ch);
