/*
 ***************************************************************************
 File: history.c

 History module for Crimson MUD 2.

 Commands/functions pertaining to the channel histories.

 Copyright Frank Fleschner
 ***************************************************************************
 */

/* type definitions, function prototypes, and global variables for history.c */

/* Make this whatever you want, keeping in mind that they do take up some memory. */
#define MAX_HISTORY_ENTRIES 25

/* Too add more channels to the channel history, just add more defines above, and update
CHANNELS_WITH_HISTORIES below to be the total count.  Also, you'll need to modify the
show_history_to_char in history.c */
#define CHANNEL_GOSSIP			0
#define CHANNEL_AVATAR			1
#define CHANNEL_SPIRIT			2
#define CHANNEL_IMMTALK			3
#define CHANNEL_STAFF			4
#define CHANNEL_BOSS			5
#define CHANNEL_MUSIC			6
#define CHANNEL_PKFLAME			7

/* Should be the highest CHANNEL_BLAH X, X + 1 */
#define CHANNELS_WITH_HISTORIES  8

/* The history structure, bottom is a pointer to the next free insertion point (if any),
and messages is an array of string pointers, which should be null until messages are added
to the history. */
struct history_data {
	int bottom;
	char *messages[MAX_HISTORY_ENTRIES];
};
/* Global history data. */
struct history_data *histories[CHANNELS_WITH_HISTORIES];

/* Functions implenented in history.c */
void boot_histories(void);
void add_to_history(int type, const char *format,...) __attribute__((format(printf, 2, 3)));
void show_history_to_char(int type, struct char_data * ch);
void destroy_histories(void);
