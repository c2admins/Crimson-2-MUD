/* Qelves */
/* gv_location: 13501-14000 */
/* ****************************************************************** *
*  file: qelves.c , Special module.                    Part of DIKUMUD *
*  Usage: Procedures handling special procs for object/room/mobile    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.   *
********************************************************************* */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "constants.h"
#include "globals.h"
#include "func.h"

int qelves_dias(struct char_data * ch, int cmd, char *arg)
{

	struct obj_data *obj, *tmp_obj;
	struct char_data *victim;
	int lv_found_runes;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];



	if (cmd != CMD_CLOSE) {
		return (FALSE);
	}

	/* IF THEY CLOSE THE DIAS AND IT CONTAINS THE CORRECT RUNESTONES CONVERT THEM TO THE DRAGON SHIELD     */
	argument_interpreter(arg, type, dir);

	if (!*type) {
		return (FALSE);
	}

	victim = 0;
	obj = 0;
	if (!(ha3500_generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM, ch,
				  &victim, &obj))) {
		return (FALSE);
	}

	/* this is an object */
	if (obj->obj_flags.type_flag != ITEM_CONTAINER)
		return (FALSE);

	if (IS_SET(obj->obj_flags.value[1], CONT_CLOSED))
		return (FALSE);

	if (!IS_SET(obj->obj_flags.value[1], CONT_CLOSEABLE))
		return (FALSE);

	if (obj_index[obj->item_number].virtual != 20766)
		return (FALSE);

	/* WE HAVE OUR DIAS, DOES IT CONTAIN THE RUNESTONES? */
	lv_found_runes = 0;
	for (tmp_obj = obj->contains; tmp_obj; tmp_obj = tmp_obj->next_content) {
		if (obj_index[tmp_obj->item_number].virtual == 20759)
			SET_BIT(lv_found_runes, BIT0);
		if (obj_index[tmp_obj->item_number].virtual == 20760)
			SET_BIT(lv_found_runes, BIT1);
		if (obj_index[tmp_obj->item_number].virtual == 20761)
			SET_BIT(lv_found_runes, BIT2);
		if (obj_index[tmp_obj->item_number].virtual == 20762)
			SET_BIT(lv_found_runes, BIT3);
	}			/* END OF for loop */


	/* IF WE DON'T HAVE THE RIGHT RUNESTONES, EXIT */
	if (lv_found_runes != BIT4 - 1)
		return (FALSE);

	/* SWAP THE KEYS FOR THE DRAGON SHIELD */
	lv_found_runes = db8200_real_object(20758);
	if (lv_found_runes < 0) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "ERROR: Tried to load object: 20758 for %s and it doesn't exist.",
			GET_REAL_NAME(ch));
		do_sys(buf, GET_LEVEL(ch) + 1, ch);
		spec_log(buf, ERROR_LOG);
		return (FALSE);
	}

	/* IF MAXED, DON'T DO ANYTHING */
	if (obj_index[lv_found_runes].number >=
	    obj_index[lv_found_runes].maximum) {
		return (FALSE);
	}

	/* REMOVE RUNESTONES */
	while (obj->contains) {
		GET_OBJ_WEIGHT(obj) -= GET_OBJ_WEIGHT(obj->contains);
		ha2700_extract_obj(obj->contains);
	}			/* END OF while loop */

	/* LOAD THE OBJECT */
	tmp_obj = db5100_read_object(lv_found_runes, REAL);

	/* PUT THE SHIELD IN THE FOUNTAIN */
	ha2300_obj_to_obj(tmp_obj, obj);

	return (FALSE);

}				/* END OF qelves_dias() */