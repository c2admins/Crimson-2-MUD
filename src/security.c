/* sr####
***************************************************************************
File: security.c

Part of Crimson MUD 2

Commands/functions related to ownership/security of the MUD.

Written by Relic
***************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "globals.h"
#include "constants.h"
#include "security.h"

/* Hard coded people */
/* Usage:
If you want people to be hardcoded for a particular port, but to also have
a default hardcoded level, then put the default hardcode first.  For example, Joe
is to have a standard hardcode of 48, but on port 5000 50 and on 4500 49:
{"joe"							,		48,		0			},
{"joe"							,		50,		5000	},
{"joe"							,		49,		4500	},
The order of the second two doesn't matter, but the default *must* be first, otherwise
it will override any previous hardcode settings.
Relic
*/

struct hardcode_list_type hardcode_list[] = {

/*{"Name"					,Level,   Port	}, */
	{"Pythias", 50, 0},
	{"Oldschool", 50, 0},
/* How we find the end of the list, add others ABOVE. */
	{"666", -1, 0}
};
/* Functions that need prototyping for use in security.c */
int ha1175_isexactname(char *str, char *namelist);
void send_to_char(char *message, struct char_data * ch);
int is_abbrev(char *arg1, char *arg2);
char *one_argument(char *argument, char *first_arg);

/* Implementation */
void sr1000_restore_char(struct char_data * ch)
{
	int idx;

	for (idx = 0; strcmp(hardcode_list[idx].name, "666"); idx++) {
		if (ha1175_isexactname(GET_NAME(ch), hardcode_list[idx].name) &&
		    (!(hardcode_list[idx].port) || hardcode_list[idx].port == gv_port)) {
			GET_LEVEL(ch) = hardcode_list[idx].level;
		}
	}
}
/* To make this it's own command or not.... */
void sr2000_do_replenish(struct char_data * ch, char *arg, int cmd)
{

	char buf[MAX_STRING_LENGTH];

	for (; isspace(*arg); arg++);
	//Yes, typing is OK !
		arg = one_argument(arg, buf);

	/* Only argument is "list" */
	if (is_abbrev(buf, "list") && GET_LEVEL(ch) >= IMO_IMP)
		sr3100_show_hardcoded_list(ch);	/* If it's "list" show them the
						 * hardcode list */
	else
		sr1000_restore_char(ch);	/* Else restore their level. */

}
/* If name's in the hardcode list, make sure we don't create new characters using them.*/
int sr3000_is_name_secure(char *arg)
{
	int idx;

	for (idx = 0; strcmp(hardcode_list[idx].name, "666"); idx++) {
		if (!strcmp(hardcode_list[idx].name, arg))
			return FALSE;
	}

	return TRUE;
}

void sr3100_show_hardcoded_list(struct char_data * ch)
{
	int idx;

	co2900_send_to_char(ch, " Name                Level Port \r\n");
	co2900_send_to_char(ch, "--------------------------------\r\n");
	for (idx = 0; strcmp(hardcode_list[idx].name, "666"); idx++) {
		co2900_send_to_char(ch, " %-20s%-5d %-4d\r\n", hardcode_list[idx].name,
			   hardcode_list[idx].level, hardcode_list[idx].port);
	}
}
