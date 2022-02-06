/* *********************************************************************
*  file: comm.h , Communication module.                Part of DIKUMUD *
*  Usage: Prototypes for the common functions in comm.c                *
********************************************************************** */

/* comm.c */
size_t co2900_send_to_char(struct char_data * ch, const char *messg,...) __attribute__((format(printf, 2, 3)));
size_t co2901_ansi_send_to_char(struct char_data * ch, const char *ansi, const char *messg,...) __attribute__((format(printf, 3, 4)));
size_t write_to_output(struct descriptor_data * d, const char *color, const char *txt,...) __attribute__((format(printf, 3, 4)));
size_t parse_colors(char *txt, size_t maxlen, struct char_data * ch, const char *color);
/* variables */
extern struct descriptor_data *descriptor_list;
extern int pulse;

/* ANSI_ACT show_flags */
#define AA_FIGHT        1
#define AA_IGNORE_SLEEP 2

#define TO_ROOM    0
#define TO_NOTCHAR 0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3

#define SEND_TO_Q(messg, desc)  write_to_output(desc, 0, messg)
