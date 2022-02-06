/* ******************************
* File: alias.c
*
* Alias system of Circle, for Crimson II
*
* Relic
*********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "globals.h"
#include "func.h"
#include "alias.h"

#define ALIAS_SIMPLE			1
#define ALIAS_COMPLEX			0

#define NUM_TOKENS				9

#define ALIAS_SEP_CHAR		';'
#define ALIAS_VAR_CHAR		'$'
#define ALIAS_GLOB_CHAR		'*'

/* in rent.c */
int r500_alias_get_filename(char *orig_name, char *filename);

/* in parser.c */
char *any_one_arg(char *argument, char *first_arg);
char *delete_doubledollar(char *string);


struct alias_data *find_alias(struct alias_data * alias_list, char *str)
{
	while (alias_list != NULL) {
		if (*str == *alias_list->alias)	/* hey, every little bit counts
						 * :-) */
			if (!strcmp(str, alias_list->alias))
				return (alias_list);

		alias_list = alias_list->next;
	}

	return (NULL);
}

void clear_aliases(struct char_data * ch)
{
	struct alias_data *a, *tmp;

	for (a = GET_ALIASES(ch); a; a = tmp) {
		tmp = a->next;
		free_alias(a);
	}
}

void free_alias(struct alias_data * a)
{
	if (a->alias)
		free(a->alias);
	if (a->replacement)
		free(a->replacement);
	free(a);
}

void write_aliases(struct char_data * ch)
{
	FILE *file;
	char fn[MAX_STRING_LENGTH];
	struct alias_data *temp;

	r500_alias_get_filename(GET_NAME(ch), fn);
	remove(fn);

	if (GET_ALIASES(ch) == NULL)
		return;

	if ((file = fopen(fn, "w")) == NULL) {
		//log("SYSERR: Couldn't save aliases for %s in '%s'.", GET_NAME(ch), fn);
		//perror("SYSERR: write_aliases");
		return;
	}

	for (temp = GET_ALIASES(ch); temp; temp = temp->next) {
		int aliaslen = strlen(temp->alias);
		int repllen = strlen(temp->replacement) - 1;

		fprintf(file, "%d\n%s\n"	/* Alias */
			"%d\n%s\n"	/* Replacement */
			"%d\n",	/* Type */
			aliaslen, temp->alias,
			repllen, temp->replacement + 1,
			temp->type);
	}

	fclose(file);
}

void read_aliases(struct char_data * ch)
{
	FILE *file;
	char xbuf[MAX_STRING_LENGTH];
	struct alias_data *t2, *prev = NULL;
	int length;

	r500_alias_get_filename(GET_NAME(ch), xbuf);

	if ((file = fopen(xbuf, "r")) == NULL) {
		if (errno != ENOENT) {
			//log("SYSERR: Couldn't open alias file '%s' for %s.", xbuf, GET_NAME(ch));
			//perror("SYSERR: read_aliases");
		}
		return;
	}

	CREATE(GET_ALIASES(ch), struct alias_data, 1);
	t2 = GET_ALIASES(ch);

	for (;;) {
		/* Read the aliased command. */
		if (fscanf(file, "%d\n", &length) != 1)
			goto read_alias_error;

		fgets(xbuf, length + 1, file);
		t2->alias = strdup(xbuf);

		/* Build the replacement. */
		if (fscanf(file, "%d\n", &length) != 1)
			goto read_alias_error;

		*xbuf = ' ';	/* Doesn't need terminated, fgets() will. */
		fgets(xbuf + 1, length + 1, file);
		t2->replacement = strdup(xbuf);

		/* Figure out the alias type. */
		if (fscanf(file, "%d\n", &length) != 1)
			goto read_alias_error;

		t2->type = length;

		if (feof(file))
			break;

		CREATE(t2->next, struct alias_data, 1);
		prev = t2;
		t2 = t2->next;
	};

	fclose(file);
	return;


read_alias_error:
	if (t2->alias)
		free(t2->alias);
	free(t2);
	if (prev)
		prev->next = NULL;
	fclose(file);
}

void delete_aliases(char *charname)
{
	char filename[MAX_STRING_LENGTH];

	if (!r500_alias_get_filename(charname, filename))
		return;

	if (remove(filename) < 0 && errno != ENOENT);
	//log("SYSERR: deleting alias file %s: %s", filename, strerror(errno));
}

void perform_complex_alias(struct txt_q * input_q, char *orig, struct alias_data * a)
{
	struct txt_q temp_queue;
	char *tokens[NUM_TOKENS], *temp, *write_point;
	char buf2[MAX_RAW_INPUT_LENGTH], buf[MAX_RAW_INPUT_LENGTH];	/* raw? */
	int num_of_tokens = 0, num;

	/* First, parse the original string */
	strcpy(buf2, orig);	/* strcpy: OK (orig:MAX_INPUT_LENGTH <
				 * buf2:MAX_RAW_INPUT_LENGTH) */
	temp = strtok(buf2, " ");
	while (temp != NULL && num_of_tokens < NUM_TOKENS) {
		tokens[num_of_tokens++] = temp;
		temp = strtok(NULL, " ");
	}

	/* initialize */
	write_point = buf;
	temp_queue.head = temp_queue.tail = NULL;

	/* now parse the alias */
	for (temp = a->replacement; *temp; temp++) {
		if (*temp == ALIAS_SEP_CHAR) {
			*write_point = '\0';
			buf[MAX_INPUT_LENGTH - 1] = '\0';
			co1400_write_to_q(buf, &temp_queue, 1);
			write_point = buf;
		}
		else if (*temp == ALIAS_VAR_CHAR) {
			temp++;
			if ((num = *temp - '1') < num_of_tokens && num >= 0) {
				strcpy(write_point, tokens[num]);	/* strcpy: OK */
				write_point += strlen(tokens[num]);
			}
			else if (*temp == ALIAS_GLOB_CHAR) {
				strcpy(write_point, orig);	/* strcpy: OK */
				write_point += strlen(orig);
			}
			else if ((*(write_point++) = *temp) == '$')	/* redouble $ for act
									 * safety */
				*(write_point++) = '$';
		}
		else
			*(write_point++) = *temp;
	}

	*write_point = '\0';
	buf[MAX_INPUT_LENGTH - 1] = '\0';
	co1400_write_to_q(buf, &temp_queue, 1);

/* push our temp_queue on to the _front_ of the input queue */
	if (input_q->head == NULL)
		*input_q = temp_queue;
	else {
		temp_queue.tail->next = input_q->head;
		input_q->head = temp_queue.head;
	}
}
/*
 * Given a character and a string, perform alias replacement on it.
 *
 * Return values:
 *   0: String was modified in place; call command_interpreter immediately.
 *   1: String was _not_ modified in place; rather, the expanded aliases
 *      have been placed at the front of the character's input queue.
 */
int perform_alias(struct descriptor_data * d, char *orig, size_t maxlen)
{
	char first_arg[MAX_INPUT_LENGTH], *ptr;
	struct alias_data *a, *tmp;

	/* Mobs don't have alaises. */
	if (IS_NPC(d->character))
		return (0);

	/* bail out immediately if the guy doesn't have any aliases */
	if ((tmp = GET_ALIASES(d->character)) == NULL)
		return (0);

	/* find the alias we're supposed to match */
	ptr = any_one_arg(orig, first_arg);

	/* bail out if it's null */
	if (!*first_arg)
		return (0);

	/* if the first arg is not an alias, return without doing anything */
	if ((a = find_alias(tmp, first_arg)) == NULL)
		return (0);

	if (a->type == ALIAS_SIMPLE) {
		strncpy(orig, a->replacement, maxlen);
		return (0);
	}
	else {
		perform_complex_alias(&d->input, ptr, a);
		return (1);
	}
}

void do_alias(struct char_data * ch, char *argument, int cmd)
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	char *repl;
	struct alias_data *a, *lv_temp;

	if (IS_NPC(ch))
		return;

	repl = any_one_arg(argument, arg);

	if (!*arg) {		/* no argument specified -- list currently
				 * defined aliases */
		co2900_send_to_char(ch, "Currently defined aliases:\r\n");
		if ((a = GET_ALIASES(ch)) == NULL)
			co2900_send_to_char(ch, " None.\r\n");
		else {
			while (a != NULL) {
				co2900_send_to_char(ch, "%-15s %s\r\n", a->alias, a->replacement);
				a = a->next;
			}
		}
	}
	else {			/* otherwise, add or remove aliases */
/* is this an alias we've already defined? */
		if ((a = find_alias(GET_ALIASES(ch), arg)) != NULL) {
			REMOVE_FROM_LIST(a, GET_ALIASES(ch), 1);
			free_alias(a);
		}
/* if no replacement string is specified, assume we want to delete */
		if (!*repl) {
			if (a == NULL)
				co2900_send_to_char(ch, "No such alias.\r\n");
			else
				co2900_send_to_char(ch, "Alias deleted.\r\n");
		}
		else {		/* otherwise, either add or redefine an alias */
			if (!strcmp(arg, "alias")) {
				co2900_send_to_char(ch, "You can't alias 'alias'.\r\n");
				return;
			}
			CREATE(a, struct alias_data, 1);
			a->alias = strdup(arg);
			delete_doubledollar(repl);
			a->replacement = strdup(repl);
			if (strchr(repl, ALIAS_SEP_CHAR) || strchr(repl, ALIAS_VAR_CHAR))
				a->type = ALIAS_COMPLEX;
			else
				a->type = ALIAS_SIMPLE;
			a->next = GET_ALIASES(ch);
			GET_ALIASES(ch) = a;
			co2900_send_to_char(ch, "Alias added.\r\n");
		}
	}
}
