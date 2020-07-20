/* Header file for new alias system (basically the Circle alias system) */

#define ALIAS_SIMPLE	1
#define ALIAS_COMPLEX 0

#define ALIAS_SEP_CHAR	';'
#define ALIAS_VAR_CHAR	'$'
#define ALIAS_GLOB_CHAR '*'

void write_aliases(struct char_data *ch);
void read_aliases(struct char_data *ch);
void clear_aliases(struct char_data *ch);
void delete_aliases(char *charname);
void free_alias(struct alias_data *a);
void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias_data *a);
int perform_alias(struct descriptor_data *d, char *orig, size_t maxlen);
