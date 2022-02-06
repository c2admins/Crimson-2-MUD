#define BAN_FILE "badsites"
#define BAN_NOT 0
#define BAN_NEW 1
#define BAN_SELECT 2
#define BAN_ALL 3
#define BANNED_SITE_LENGTH 50
#define XNAME_FILE "badnames"

struct ban_list_element {
	char site[BANNED_SITE_LENGTH + 1];
	int type;
	long date;
	char name[MAX_NAME_LENGTH + 1];
	char reason[31];
	struct ban_list_element *next;
};
