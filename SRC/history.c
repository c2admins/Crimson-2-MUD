/*
 ***************************************************************************
 File: history.c

 History module for Crimson MUD 2.

 Commands/functions pertaining to the channel histories.

 Copyright Frank Fleschner
 ***************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "ansi.h"
#include "history.h"
#include "func.h"

/* Local prototypes */
void _add_to_history(struct history_data * history, const char *message, va_list args);

/* External prototypes */
void search_replace(char *string, const char *find, const char *replace);

/* Simple initialization */
void boot_histories(void)
{
	int i;

	for (i = 0; i < CHANNELS_WITH_HISTORIES; i++)
		CREATE(histories[i], struct history_data, 1);

}
/* External, printf-like call.  */
void add_to_history(int type, const char *format,...)
{
	va_list args;

	va_start(args, format);
	_add_to_history(histories[type], format, args);
	va_end(args);
}
/* private, does all the real work */
void _add_to_history(struct history_data * history, const char *message, va_list args)
{
	static char buffer[MAX_STRING_LENGTH];
	int i, size;
	size_t size2;

	bzero(buffer, sizeof(buffer));

	/* Do the whole virtual argument thing. */
	size = size2 = vsnprintf(buffer, sizeof(buffer), message, args);

	if (size < 0 || size2 > sizeof(buffer)) {
		size = sizeof(buffer) - strlen("..." END NEWLINE) - 1;
		strncpy(buffer + size, "..." END NEWLINE, sizeof(buffer) - size);
	}

	/* Add the message to the history.  There are 3 cases. Case 1:  bottom
	 * does not point to the last entry in the array, so just strdup it in
	 * the right place, the move bottom to the next */
	if (history->bottom < MAX_HISTORY_ENTRIES - 1) {
		history->messages[history->bottom] = strdup(buffer);
		history->bottom++;
	}
	else {
		/* Case 2:  bottom points to the last member of the array, but
		 * it's still empty, so just strdup it. */
		if (!(history->messages[history->bottom])) {
			history->messages[history->bottom] = strdup(buffer);
		}
		else {
			/* Ok, free one, the rotate the rest up, then save the
			 * new message.  First, free the first message, and
			 * rotate the next one up. */
			free(history->messages[0]);
			history->messages[0] = history->messages[1];

			/* Now we rotate the rest up, quick and easy with no
			 * expensive system calls. */
			for (i = 1; i < MAX_HISTORY_ENTRIES - 1; i++) {
				history->messages[i] = history->messages[i + 1];
			}
			/* And finally just strdup it into place. */
			history->messages[MAX_HISTORY_ENTRIES - 1] = strdup(buffer);
		}
	}
}


void show_history_to_char(int type, struct char_data * ch)
{
	struct history_data *history = histories[type];
	int i;
	char *ansi;

	if (type >= CHANNELS_WITH_HISTORIES)
		return;

	if (history->bottom == 0) {
		co2900_send_to_char(ch, "No history for this channel, yet!\r\n");
		return;
	}

	switch (type) {
	case 0:
		ansi = CLR_GOSSIP;
		co2901_ansi_send_to_char(ch, CLR_GOSSIP, "&nPrevious %d gossips:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 1:
		ansi = CLR_AVAT;
		co2901_ansi_send_to_char(ch, CLR_AVAT, "Previous %d avats:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 2:
		ansi = LGREEN;
		co2901_ansi_send_to_char(ch, LGREEN, "Previous %d spirits:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 3:
		ansi = CLR_IMMTALK;
		co2901_ansi_send_to_char(ch, CLR_IMMTALK, "Previous %d imms:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 4:
		ansi = RED;
		co2901_ansi_send_to_char(ch, RED, "Previous %d STAFF:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 5:
		ansi = GRAY;
		co2901_ansi_send_to_char(ch, GRAY, "Previous %d ADMIN:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 6:
		ansi = CLR_MUSIC;
		co2901_ansi_send_to_char(ch, CLR_MUSIC, "Previous %d musics:\r\n", MAX_HISTORY_ENTRIES);
		break;
	case 7:
		ansi = LRED;
		co2901_ansi_send_to_char(ch, LRED, "Previous %d pkflames:\r\n", MAX_HISTORY_ENTRIES);
		break;
	default:
		ansi = END;
		co2901_ansi_send_to_char(ch, END, "Previous %d messages of this channel:\r\n", MAX_HISTORY_ENTRIES);
		break;
	}

	for (i = 0; i < history->bottom; i++)
		co2901_ansi_send_to_char(ch, ansi, "&R%-2d&W) &n%s&n\r\n", i + 1, history->messages[i]);
	if (history->messages[history->bottom])
		co2901_ansi_send_to_char(ch, ansi, "&R%-2d&W) &n%s&n\r\n", i + 1, history->messages[history->bottom]);

}

void destroy_histories(void)
{
	int i, j;

	for (i = 0; i < CHANNELS_WITH_HISTORIES; i++) {
		for (j = 0; j < MAX_HISTORY_ENTRIES; j++) {
			if (histories[i] && histories[i]->messages[j])
				free(histories[i]->messages[j]);
		}
		free(histories[i]);
	}
}
