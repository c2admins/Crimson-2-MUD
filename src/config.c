/* cf */
/* *******************************************************************
*  file: config.c , Special module.           Part of Crimson MUD    *
*  Usage: Config system for Crimson MUD II                           *
*                  Written by Hercules (derived from admin.c)        *
******************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "parser.h"
#include "spells.h"
#include "constants.h"
#include "modify.h"
#include "ansi.h"
#include "globals.h"
#include "func.h"
#include "config.h"


/* ***********************************************************
*  CONFIG - Basic menu nanny                                 *
*********************************************************** */
