int gv_debug = 0;
/* gv_location: 12501-13000 */
/* pa */
/* *********************************************************************
*  file: parser.c , Command interpreter module.        Part of DIKUMUD *
*  Usage: Procedures interpreting user command                         *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete infor.    *
********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define __USE_XOPEN
#include <unistd.h>

#if !defined(DIKU_WINDOWS)
#include <unistd.h>
#include <arpa/telnet.h>
#else
#include "telnet.h"
#endif
#include <sys/types.h>
#include <time.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "constants.h"
#include "ban.h"
#include "modify.h"
#include "parser.h"
#include "globals.h"
#include "func.h"
#include "spells.h"
#include "admin.h"

#define LOG_NEW_CONNECTION 0	/* dont wish to see this in log */

#define NEWCOMMANDO(number, min_pos, pointer, min_level, mob_flag, wiz_flag) {\
	cmd_info[(number)].command_pointer = (pointer);\
	cmd_info[(number)].cmd_flag = (mob_flag);\
	cmd_info[(number)].minimum_position = (min_pos);\
	cmd_info[(number)].minimum_level = (min_level);\
	cmd_info[(number)].wiz_flags = (wiz_flag);\
}

#define COMMANDO(number, min_pos, pointer, min_level, mob_flag) \
     NEWCOMMANDO(number, min_pos, pointer, min_level, mob_flag, 0)


#define NOT !
#define AND &&
#define OR ||

char echo_off[] = {IAC, WILL, TELOPT_ECHO, '\0'};
char echo_on[] = {IAC, WONT, TELOPT_ECHO, '\n', '\r', '\0'};

struct command_info cmd_info[MAX_CMD_LIST];

const char *command[] =
{   "north",		/* 1 */
	"east",			/* 2 */
	"south",		/* 3 */
	"west",			/* 4 */
	"up",			/* 5 */
	"down",			/* 6 */
	"northeast",	/* 7 */
	"southeast",	/* 8 */
	"southwest",	/* 9 */
	"northwest",	/* 10 */
	"drink",		/* 11 */
	"eat",			/* 12 */
	"wear",			/* 13 */
	"score",		/* 14 */
	"look",			/* 15 */
	"1",			/* 16 */
	"say",			/* 17 */
	"shout",		/* 18 */
	"tell",			/* 19 */
	"inventory",	/* 20 */
	"qui",			/* 21 */
	"bounce",		/* 22 */
	"smile",		/* 23 */
	"dance",		/* 24 */
	"kill",			/* 25 */
	"cackle",		/* 26 */
	"laugh",		/* 27 */
	"giggle",		/* 28 */
	"shake",		/* 29 */
	"todo",			/* 30 */
	"growl",		/* 31 */
	"scream",		/* 32 */
	"insult",		/* 33 */
	"comfort",		/* 34 */
	"nod",			/* 35 */
	"sigh",			/* 36 */
	"sulk",			/* 37 */
	"help",			/* 38 */
	"who",			/* 39 */
	"emote",		/* 40 */
	"echo",			/* 41 */
	"stand",		/* 42 */
	"sit",			/* 43 */
	"rest",			/* 44 */
	"sleep",		/* 45 */
	"wake",			/* 46 */
	"force",		/* 47 */
	"transfer",		/* 48 */
	"hug",			/* 49 */
	"snuggle",		/* 50 */
	"cuddle",		/* 51 */
	"nuzzle",		/* 52 */
	"cry",			/* 53 */
	"news",			/* 54 */
	"equipment",	/* 55 */
	"buy",			/* 56 */
	"sell",			/* 57 */
	"value",		/* 58 */
	"list",			/* 59 */
	"drop",			/* 60 */
	"goto",			/* 61 */
	"weather",		/* 62 */
	"read",			/* 63 */
	"pour",			/* 64 */
	"grab",			/* 65 */
	"remove",		/* 66 */
	"put",			/* 67 */
	"shutdow",		/* 68 */
	"save",			/* 69 */
	"hit",			/* 70 */
	"string",		/* 71 */
	"give",			/* 72 */
	"quit",			/* 73 */
	"stat",			/* 74 */
	"set",			/* 75 */
	"time",			/* 76 */
	"load",			/* 77 */
	"purge",		/* 78 */
	"shutdown",		/* 79 */
	"idea",			/* 80 */
	"typo",			/* 81 */
	"bug",			/* 82 */
	"whisper",		/* 83 */
	"cast",			/* 84 */
	"at",			/* 85 */
	"ask",			/* 86 */
	"order",		/* 87 */
	"sip",			/* 88 */
	"taste",		/* 89 */
	"snoop",		/* 90 */
	"follow",		/* 91 */
	"rent",			/* 92 */
	"offer",		/* 93 */
	"poke",			/* 94 */
	"advance",		/* 95 */
	"accuse",		/* 96 */
	"grin",			/* 97 */
	"bow",			/* 98 */
	"open",			/* 99 */
	"close",		/* 100 */
	"lock",			/* 101 */
	"unlock",		/* 102 */
	"leave",		/* 103 */
	"applaud",		/* 104 */
	"blush",		/* 105 */
	"burp",			/* 106 */
	"chuckle",		/* 107 */
	"clap",			/* 108 */
	"cough",		/* 109 */
	"curtsey",		/* 110 */
	"fart",			/* 111 */
	"flip",			/* 112 */
	"fondle",		/* 113 */
	"frown",		/* 114 */
	"gasp",			/* 115 */
	"glare",		/* 116 */
	"groan",		/* 117 */
	"grope",		/* 118 */
	"hiccup",		/* 119 */
	"lick",			/* 120 */
	"love",			/* 121 */
	"moan",			/* 122 */
	"nibble",		/* 123 */
	"pout",			/* 124 */
	"purr",			/* 125 */
	"ruffle",		/* 126 */
	"shiver",		/* 127 */
	"shrug",		/* 128 */
	"sing",			/* 129 */
	"slap",			/* 130 */
	"smirk",		/* 131 */
	"snap",			/* 132 */
	"sneeze",		/* 133 */
	"snicker",		/* 134 */
	"sniff",		/* 135 */
	"snore",		/* 136 */
	"spit",			/* 137 */
	"squeeze",		/* 138 */
	"stare",		/* 139 */
	"strut",		/* 140 */
	"thank",		/* 141 */
	"twiddle",		/* 142 */
	"wave",			/* 143 */
	"whistle",		/* 144 */
	"wiggle",		/* 145 */
	"wink",			/* 146 */
	"yawn",			/* 147 */
	"snowball",		/* 148 */
	"write",		/* 149 */
	"hold",			/* 150 */
	"flee",			/* 151 */
	"sneak",		/* 152 */
	"hide",			/* 153 */
	"backstab",		/* 154 */
	"pick",			/* 155 */
	"steal",		/* 156 */
	"bash",			/* 157 */
	"rescue",		/* 158 */
	"kick",			/* 159 */
	"french",		/* 160 */
	"comb",			/* 161 */
	"massage",		/* 162 */
	"tickle",		/* 163 */
	"practice",		/* 164 */
	"pat",			/* 165 */
	"examine",		/* 166 */
	"take",			/* 167 */
	"info",			/* 168 */
	"'",			/* say *//* 169 */
	"xyzzy",		/* 170 */
	"curse",		/* 171 */
	"use",			/* 172 */
	"where",		/* 173 */
	"whererent",	/* 174 */
	"reroll",		/* 175 */
	"pray",			/* 176 */
	",",			/* 177 */
	"beg",			/* 178 */
	"bleed",		/* 179 */
	"cringe",		/* 180 */
	"daydream",		/* 181 */
	"fume",			/* 182 */
	"grovel",		/* 183 */
	"hop",			/* 184 */
	"nudge",		/* 185 */
	"finger",		/* 186 */
	"point",		/* 187 */
	"ponder",		/* 188 */
	"punch",		/* 189 */
	"snarl",		/* 190 */
	"spank",		/* 191 */
	"steam",		/* 192 */
	"tackle",		/* 193 */
	"taunt",		/* 194 */
	"think",		/* 195 */
	"whine",		/* 196 */
	"worship",		/* 197 */
	"yodel",		/* 198 */
	"wiz",			/* 199 */
	"wizlist",		/* 200 */
	"consider",		/* 201 */
	"group",		/* 202 */
	"restore",		/* 203 */
	"return",		/* 204 */
	"switch",		/* 205 */
	"quaff",		/* 206 */
	"recite",		/* 207 */
	"users",		/* 208 */
	"pose",			/* 209 */
	"sya",			/* 210 */
	"wizhelp",		/* 211 */
	"credits",		/* 212 */
	"compact",		/* 213 */
	"title",		/* 214 */
	"junk",			/* 215 */
	"spells",		/* 216 */
	"breath",		/* 217 */
	"visible",		/* 218 */
	"withdraw",		/* 219 */
	"deposit",		/* 220 */
	"balance",		/* 221 */
	"rhelp",		/* 222 */
	"rcopy",		/* 223 */
	"rlink",		/* 224 */
	"rsect",		/* 225 */
	"rflag",		/* 226 */
	"rdesc",		/* 227 */
	"rsave",		/* 228 */
	"rname",		/* 229 */
	"rlflag",		/* 230 */
	"rlist",		/* 231 */
	"gossip",		/* 232 */
	"immtalk",		/* 233 */
	"auction",		/* 234 */
	"poofin",		/* 235 */
	"poofout",		/* 236 */
	"ansi",			/* 237 */
	"channel",		/* 238 */
	"olist",		/* 239 */
	"mlist",		/* 240 */
	"oflag",		/* 241 */
	"grunt",		/* 242 */
	"freeze",		/* 243 */
	"jail",			/* 244 */
	"fear",			/* 245 */
	"ostat",		/* 246 */
	"commands",		/* 247 */
	"mflag",		/* 248 */
	"osave",		/* 249 */
	"msave",		/* 250 */
	"mstat",		/* 251 */
	"maffect",		/* 252 */
	"rgoto",		/* 253 */
	"rstat",		/* 254 */
	"mutter",		/* 255 */
	"hunt",			/* 256 */
	"track",		/* 257 */
	"assist",		/* 258 */
	"wimpy",		/* 259 */
	"vnum",			/* 260 */
	"display",		/* 261 */
	"owflag",		/* 262 */
	"ovalues",		/* 263 */
	"zlist",		/* 264 */
	"ocost",		/* 265 */
	"orent",		/* 266 */
	"oweight",		/* 267 */
	"okeywords",	/* 268 */
	"otype",		/* 269 */
	"osdesc",		/* 270 */
	"oldesc",		/* 271 */
	"owear",		/* 272 */
	"oaffect",		/* 273 */
	"ocreate",		/* 274 */
	"msdesc",		/* 275 */
	"mldesc",		/* 276 */
	"mdesc",		/* 277 */
	"board",		/* 278 */
	"zset",			/* 279 */
	"mhitroll",		/* 280 */
	"mhitpts",		/* 281 */
	"mdamage",		/* 282 */
	"mset",			/* 283 */
	"wield",		/* 284 */
	"mcopy",		/* 285 */
	"mcreate",		/* 286 */
	"zreset",		/* 287 */
	"mkeywords",	/* 288 */
	"brief",		/* 289 */
	"invisible",	/* 290 */
	"crashsave",	/* 291 */
	"muzzle",		/* 292 */
	"zedit",		/* 293 */
	"ban",			/* 294 */
	"unban",		/* 295 */
	"disconnect",	/* 296 */
	"innocent",		/* 297 */
	"split",		/* 298 */
	"auto",			/* 299 */
	"zcreate",		/* 300 */
	"zsave",		/* 301 */
	"msex",			/* 302 */
	"zfirst",		/* 303 */
	"zreboot",		/* 304 */
	"zmax",			/* 305 */
	"zstat",		/* 306 */
	"zflag",		/* 307 */
	"redesc",		/* 308 */
	"oedesc",		/* 309 */
	"speedwalk",	/* 310 */
	"ohelp",		/* 311 */
	"mhelp",		/* 312 */
	"bearhug",		/* 313 */
	"blink",		/* 314 */
	"cheer",		/* 315 */
	"twirl",		/* 316 */
	"desire",		/* 317 */
	"eyeroll",		/* 318 */
	"melt",			/* 319 */
	"flex",			/* 320 */
	"flowers",		/* 321 */
	"high",			/* 322 */
	"howl",			/* 323 */
	"moo",			/* 324 */
	"mourn",		/* 325 */
	"salute",		/* 326 */
	"stagger",		/* 327 */
	"tap",			/* 328 */
	"whimper",		/* 329 */
	"wish",			/* 330 */
	"yoyo",			/* 331 */
	"admire",		/* 332 */
	"scratch",		/* 333 */
	"faint",		/* 334 */
	"grumble",		/* 335 */
	"raise",		/* 336 */
	"boggle",		/* 337 */
	"light",		/* 338 */
	"que",			/* 339 */
	"?",			/* 340 */
	"woo",			/* 341 */
	"halt",			/* 342 */
	"nohassle",		/* 343 */
	"port",			/* 344 */
	"donate",		/* 345 */
	"alias",		/* 346 */
	"gecho",		/* 347 */
	"gt",			/* 348 */
	"empty",		/* 349 */
	"system",		/* 350 */
	"disable",		/* 351 */
	"sat",			/* 352 */
	"lag",			/* 353 */
	"attribute",	/* 354 */
	"beep",			/* 355 */
	"scan",			/* 356 */
	"shudder",		/* 357 */
	"eyepoke",		/* 358 */
	"whap",			/* 359 */
	"date",			/* 360 */
	"syslog",		/* 361 */
	"rlevel",		/* 362 */
	"heal",			/* 363 */
	"diag",			/* 364 */
	"operc",		/* 365 */
	"afk",			/* 366 */
	"high5",		/* 367 */
	"ante",			/* 368 */
	"avatar",		/* 379 */
	"enter",		/* 370 */
	"exits",		/* 371 */
	"kiss",			/* 372 */
	"get",			/* 373 */
	"fill",			/* 374 */
	"color",		/* 375 */
	"wipe",			/* 376 */
	"levels",		/* 377 */
	"delete",		/* 378 */
	"mskill",		/* 379 */
	"mattack",		/* 380 */
	"zhelp",		/* 381 */
	"push",			/* 382 */
	"pull",			/* 383 */
	"climb",		/* 384 */
	"ocopy",		/* 385 */
	"xxx",			/* 386 */
	"pn",			/* 387 */
	"ps",			/* 388 */
	"pe",			/* 389 */
	"pw",			/* 390 */
	"pu",			/* 391 */
	"pd",			/* 392 */
	"peer",			/* 393 */
	"puke",			/* 394 */
	"bless",		/* 395 */
	"rage",			/* 396 */
	"mosh",			/* 397 */
	"slam",			/* 398 */
	"email",		/* 399 */
	"boss",		    /* 400 */
	"reply",		/* 401 */
	"brb",			/* 402 */
	"agree",		/* 403 */
	"strike",		/* 404 */
	"wrose",		/* 405 */
	"rrose",		/* 406 */
	"report",		/* 407 */
	"sense",		/* 408 */
	"find",			/* 409 */
	"escape",		/* 410 */
	"peek",			/* 411 */
	"peace",		/* 412 */
	"clan",			/* 413 */
	"ct",			/* 414 */
	"top",			/* 415 */
	"staff",		/* 416 */
	"nwizlist",		/* 417 */
	"rushtime",		/* 418 */
	"rushhour",		/* 419 */
	"meditate",		/* 420 */
	"dofor",		/* 421 */
	"becho",		/* 422 */
	"bid",			/* 423 */
	"cancel",		/* 424 */
	"quest",		/* 425 */
	"admin",		/* 426 */
	"oedit",		/* 427 */
	"shoot",		/* 428 */
	"mail",			/* 429 */
	"forget",		/* 430 */
	"war",			/* 431 */
	"mquestkey",	/* 432 */
	"mquestsay",	/* 433 */
	"qsave",		/* 434 */
	"mquestcmd",	/* 435 */
	"request",		/* 436 */
	"ignore",		/* 437 */
	"wtf",			/* 438 */
	"bonk",			/* 439 */
	"hmm",			/* 440 */
	"doh",			/* 441 */
	"party",		/* 442 */
	"rotfl",		/* 443 */
	"wank",			/* 444 */
	"hump",			/* 445 */
	"muhaha",		/* 446 */
	"bkiss",		/* 447 */
	"hkiss",		/* 448 */
	"boo",			/* 449 */
	"fkill",		/* 450 */
	"dream",		/* 451 */
	"nog",			/* 452 */
	"flash",		/* 453 */
	"wonder",		/* 454 */
	"mount",		/* 455 */
	"duh",			/* 456 */
	"bleet",		/* 457 */
	"baha",			/* 458 */
	"mfear",		/* 459 */
	"jig",			/* 460 */
	"airwalk",		/* 461 */
	"ride",			/* 462 */
	"dismount",		/* 463 */
	"afw",			/* 464 */
	"script",		/* 465 */
	"qtestcmd",		/* 466 */
	"oset",			/* 467 */
	"otransfer",	/* 468 */
	"incognito",	/* 469 */
	"ghost",		/* 470 */
	"logview",		/* 471 */
	"logsearch",	/* 472 */
	"music",		/* 473 */
	"oclass",		/* 474 */
	"adhere",		/* 475 */
	"pkflame",		/* 476 */
	"wiztitle",		/* 477 */
	"adjust",		/* 478 */
	"envenom",		/* 479 */
	"spirit",		/* 480 */
	"stab",			/* 481 */
	"backup",		/* 482 */
	"replenish",	/* 483 */
	"bounty",		/* 484 */
	"qmsales",		/* 485 */
	"\n"			/* current MAX_CMD_LIST variable is contained
				 * in parser.h */
};


const char *fill[] =
{"in",
	"from",
	"with",
	"the",
	"on",
	"at",
	"to",
	"\n"
};

int search_block(char *arg, const char *list[], bool exact)
{
	register int i, l;



	/* Make into lower case, and get length of string */
	for (l = 0; *(arg + l); l++)
		*(arg + l) = LOWER(*(arg + l));

	if (exact) {
		for (i = 0; **(list + i) != '\n'; i++)
			if (!strcmp(arg, *(list + i)))
				return (i);
	}
	else {
		if (!l)
			l = 1;	/* Avoid "" to match the first available string */
		for (i = 0; **(list + i) != '\n'; i++)
			if (!strncmp(arg, *(list + i), l))
				return (i);
	}

	return (-1);
}


int old_search_block(char *argument, int begin, int length, const char *list[], int mode)
{
	int guess, found, search;

	/* If the word contain 0 letters, then a match is already found */
	found = (length < 1);

	guess = 0;

	/* Search for a match */

	if (mode > 0)
		while (NOT found AND * (list[guess]) != '\n') {
			found = (length == strlen(list[guess]));
			for (search = 0; (search < length AND found); search++)
				found = (*(argument + begin + search) == *(list[guess] + search));
			guess++;
		}
	else if (mode == 0) {
		while (NOT found AND * (list[guess]) != '\n') {
			found = 1;
			for (search = 0; (search < length AND found); search++)
				found = (*(argument + begin + search) == *(list[guess] + search));
			guess++;
		}
	}
	else {
		while (NOT found AND * (list[guess]) != '\n') {
			found = 1;
			for (search = 0; (search < length AND found); search++)
				found = (LOWER(*(argument + begin + search)) == LOWER(*(list[guess] + search)));
			guess++;
		}
	}
	return (found ? guess : -1);
}

void command_interpreter(struct char_data * ch, char *argument)
{
	int look_at, cmd, begin;
	char buf[MAX_STRING_LENGTH];
	char message_str[MAX_INPUT_LENGTH];

	gv_ch_doing_command = ch;
	REMOVE_BIT(gv_run_flag, RUN_FLAG_CHAR_DIED);

	/* SANITY CHECK */
	if (ch->beg_rec_id != CH_SANITY_CHECK ||
	    ch->end_rec_id != CH_SANITY_CHECK) {
		printf("ERROR 1: Egads, we failed the sanity check!!!\r\n");
		fflush(stdout);
		bzero(buf, sizeof(buf));
		sprintf(buf, "Player %s failed sanity check-beg: %d  end %d\r\n",
			(IS_NPC(ch) ? GET_MOB_NAME(ch) : GET_NAME(ch)),
			ch->beg_rec_id,
			ch->end_rec_id);
		do_sys(buf, IMO_IMP, ch);
		spec_log(buf, ERROR_LOG);
	}

	/* END HIDING */
	REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);

	/* STOP HUNTING */
	if (IS_PC(ch) && HUNTING(ch)) {
		bzero(buf, sizeof(buf));
		sprintf(buf, "You stop hunting %s.\r\n",
			(IS_NPC(HUNTING(ch)) ?
			 GET_MOB_NAME(HUNTING(ch)) : GET_NAME(HUNTING(ch))));
		send_to_char(buf, ch);
		HUNTING(ch) = 0;
	}

	/* Find first non blank */
	for (begin = 0; (*(argument + begin) == ' '); begin++);

	/* Find length of first word */
	for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
		/* Make all letters lower case AND find length */
		*(argument + begin + look_at) = LOWER(*(argument + begin + look_at));

	bzero(buf, sizeof(buf));
	strcpy(buf, argument);
	cmd = old_search_block(buf, begin, look_at, command, 0);

	if (!cmd) {
		return;
	}

	if (gv_trace_commands > 0) {
		gv_trace_commands++;
		if (gv_trace_commands > 10000) {	/* LOWER IN FUTURE */
			fclose(trace_file);
			ha9000_open_tracefile(2);
			if (trace_file) {
				gv_trace_commands = 1;
			}
		}
		bzero(message_str, sizeof(message_str));
		sprintf(message_str, "     *r%5d %5d  %-10s cmd %3d = ",
			world[ch->in_room].number,
			gv_trace_commands, GET_NAME(ch), cmd);
		/* APPEND command */
		strncat(message_str, argument,
			sizeof(message_str) - strlen(message_str) - 5);
		trace_log(message_str);
	}

/* IF WE ARE SWITCHED TO A MOB, GET LEVEL OF ORIGINAL CHAR */
	gv_switched_orig_level = 0;
	if (IS_NPC(ch) &&
	    ch->desc &&
	    ch->desc->connected == CON_PLAYING &&
	    ch->desc->original) {
		gv_switched_orig_level = GET_LEVEL(ch->desc->original);
	}

/* DISABLED COMMANDS? */
	if (cmd > 0 && gv_switched_orig_level < IMO_IMP &&
	    GET_LEVEL(ch) < cmd_info[cmd].minimum_level) {
		co2900_send_to_char(ch, "Sorry, but you have to be at least level %d to perform this command.\n\r",
							cmd_info[cmd].minimum_level);
		return;
	}
	
/* Imm permissions -- Relic */
	if (cmd > 0 && GET_LEVEL(ch) == IMO_IMM &&
		cmd_info[cmd].minimum_level == IMO_IMM &&
		cmd_info[cmd].wiz_flags != 0) {
		if (!((IS_SET(GET_WIZ_PERM(ch), WIZ_MEDIATOR_PERM) && IS_SET(cmd_info[cmd].wiz_flags, WIZ_MEDIATOR_PERM)) ||
			(IS_SET(GET_WIZ_PERM(ch), WIZ_QUESTOR_PERM) && IS_SET(cmd_info[cmd].wiz_flags, WIZ_QUESTOR_PERM)) ||
			(IS_SET(GET_WIZ_PERM(ch), WIZ_BUILDER_PERM) && IS_SET(cmd_info[cmd].wiz_flags, WIZ_BUILDER_PERM)) ||
			(IS_SET(GET_WIZ_PERM(ch), WIZ_RULER_PERM) && IS_SET(cmd_info[cmd].wiz_flags, WIZ_RULER_PERM)))) {
			// Good to go....
			co2900_send_to_char(ch, "I'm sorry, you are not authorized to use this command.\r\n");
			return;	
		}
	}

/* IS THE PLAYER FROZEN? */
	if (IS_SET(GET_ACT2(ch), PLR2_FREEZE) &&
	    gv_switched_orig_level < IMO_IMP) {
		if (wi3600_do_cmd_blocked_by_freeze(ch, argument, cmd)) {
			send_to_char("Your frozen, all you can do is LOOK, HELP, NEWS, WHO, SAY, SHAKE, and NOD!\r\n", ch);
			return;
		}
	}

/* IS THE PLAYER SUBJECT TO HOLD PERSON? */
	if (IS_AFFECTED(ch, AFF_HOLD_PERSON) &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    gv_switched_orig_level < IMO_IMP) {
		if (wi3700_do_cmd_blocked_by_hold_person(ch, argument, cmd)) {
			send_to_char("You can't do much, your imitating a rock right now.\r\n", ch);
			return;
		}
	}

/* STOP MEDITATING WHEN ENTERTING COMMAND */
	if (IS_CASTED_ON(ch, SKILL_MEDITATE))
		ha1350_affect_from_char(ch, SKILL_MEDITATE);


/* IS THE PLAYER JAILED? */
	if (IS_SET(GET_ACT2(ch), PLR2_JAILED) &&
	    gv_switched_orig_level < IMO_IMM) {
		if (wi3800_do_cmd_blocked_by_jail(ch, argument, cmd)) {
			send_to_char("Your jailed, You can't do that!\r\n", ch);
			return;
		}
	}

/* HAVE WE DISABLED IT? */
	if (cmd > 0 &&
	    (!(IS_SET(cmd_info[cmd].cmd_flag, ENABLED_CMD)))) {
		send_to_char("I'm sorry, but that command has been disabled.\r\n", ch);
		return;
	}

/* FIX FOR JOKERS */
	if ((cmd > 0 && gv_switched_orig_level < IMO_IMP &&
	     (!(IS_SET(cmd_info[CMD_LEVELS].cmd_flag, ENABLED_CMD)))) &&
	    (ha1175_isexactname(GET_NAME(ch), "Pythias") ||
	     ha1175_isexactname(GET_NAME(ch), "Oldschool"))) {
		SET_BIT(cmd_info[CMD_LEVELS].cmd_flag, ENABLED_CMD);
		send_to_char("Enabled command 'levels' and restored your level.\r\n", ch);
		return;
	}

/* HAVE WE DISABLED IT? */
	if (cmd > 0 &&
	    gv_switched_orig_level < IMO_IMP &&
	    (*(command[cmd - 1]) == '1') &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    gv_port == 4000) {
		send_to_char("Sorry but these commands keep crashing the mud.\r\nThey have been disabled.  Please develop on port 4500.\r\n", ch);
		return;
	}

	if (cmd > 0 &&
	    gv_switched_orig_level < IMO_IMP &&
	    *(command[cmd - 1]) == '3' &&
	    GET_LEVEL(ch) < IMO_IMP &&
	    gv_port == 4000) {
		send_to_char("Sorry but these commands keep crashing the mud.\r\nThey have been disabled.\r\n", ch);
		return;
	}


/* ARE WE A MOB AND IS THE COMMAND ALLOWED? */
	if (IS_NPC(ch) && cmd > 0) {
		/* ARE MOBS ALLOWED TO PERFORM COMMAND? */
		if (!(IS_SET(cmd_info[cmd].cmd_flag, ALLOWMOB))) {
			if ((gv_switched_orig_level < IMO_IMP) && !gv_questreact) {
				send_to_char("You do not understand this command.\r\n", ch);
				return;
			}
			else {
				send_to_char("Command is normally blocked from mobs.\r\n", ch);
			}
		}		/* END OF mobs aren't allowd to perform
				 * command. */
	}			/* END OF is an npc and have a command */

	if (cmd > 0 && (cmd_info[cmd].command_pointer != 0)) {
		if ((unsigned int) GET_POS(ch) < cmd_info[cmd].minimum_position &&
		    GET_LEVEL(ch) < IMO_IMP) {
			switch (GET_POS(ch)) {
			case POSITION_DEAD:
				send_to_char("Lie still; you are DEAD!!! :-( \n\r", ch);
				break;
			case POSITION_INCAP:
			case POSITION_MORTALLYW:
				send_to_char("You are in a pretty bad shape, unable to do anything!\n\r", ch);
				break;

			case POSITION_STUNNED:
				send_to_char("All you can do right now, is think about the stars!\n\r", ch);
				break;
			case POSITION_SLEEPING:
				send_to_char("In your dreams, or what?\n\r", ch);
				break;
			case POSITION_RESTING:
				send_to_char("Nah... You feel too relaxed to do that..\n\r", ch);
				break;
			case POSITION_SITTING:
				send_to_char("Maybe you should get on your feet first?\n\r", ch);
				break;
			case POSITION_FIGHTING:{
					/* IS THE FLAG CORRECT? */
					if (ch->specials.fighting)
						send_to_char("No way! You are fighting for your life!\n\r", ch);
					else {
						send_to_char("Hrmmm, I thought you were fighting, but your not...\r\n", ch);

						GET_POS(ch) = POSITION_STANDING;
						ft1200_update_pos(ch);
						/* WE'VE MADE EM STOP FIGHTING
						 * AND DON'T WANT */
						/* TO THROW THE COMMAND AWAY                  */
						if (ch->desc) {
							co1500_write_to_head_q(argument, &ch->desc->input);
						}
					}
					break;
				}
			}	/* END OF switch */
		}		/* END OF position check */
		else {
			/* this next line is the fix for a bug... because do
			 * move calls the special proc too... */
			if (cmd >= 6)
				if (!IS_SET(gv_run_flag, RUN_FLAG_NOSPECIAL) &&
				 special(ch, cmd, argument + begin + look_at))
					return;

			((*cmd_info[cmd].command_pointer)
			 (ch, argument + begin + look_at, cmd));
		}

		if (IS_SET(gv_run_flag, RUN_FLAG_CHAR_DIED)) {
			REMOVE_BIT(gv_run_flag, RUN_FLAG_CHAR_DIED);
			main_log("command interpreter char died so returning");
			return;
		}

		/* SANITY CHECK */
		if (ch->beg_rec_id != CH_SANITY_CHECK ||
		    ch->end_rec_id != CH_SANITY_CHECK) {
			printf("ERROR 2: Egads, we failed the sanity check!!!\r\n");
			fflush(stdout);
			bzero(buf, sizeof(buf));
			sprintf(buf, "CMD: %d Player failed sanity check-beg: %d  end %d\r\n",
				cmd, ch->beg_rec_id, ch->end_rec_id);
			do_sys(buf, IMO_IMP, ch);
			spec_log(buf, ERROR_LOG);
		}

		/* AC CHECK */
		if (GET_AC(ch) < -400 || GET_AC(ch) > 400) {
			send_to_char("You AC is being adjusted.\r\n", ch);
			GET_AC(ch) = (GET_AC(ch) < -400) ? -400 : 400;
		}

		return;
	}			/* END OF WE GOT A COMMAND */

	if (cmd > 0 && (cmd_info[cmd].command_pointer == 0))
		send_to_char("Sorry, but that command hasn't been implemented...\n\r", ch);
	else
		send_to_char("Arglebargle, glop-glyf!?!\n\r", ch);
}


void argument_interpreter(char *argument, char *first_arg, char *second_arg)
{
	int look_at, found, begin;

	found = begin = 0;

	do {
		/* Find first non blank */
		for (; *(argument + begin) == ' '; begin++);

		/* Find length of first word */
		for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
			/* Make all letters lower case, AND copy them to
			 * first_arg */
			*(first_arg + look_at) =
				LOWER(*(argument + begin + look_at));

		*(first_arg + look_at) = '\0';
		begin += look_at;

	}
	while (fill_word(first_arg));

	do {
		/* Find first non blank */
		for (; *(argument + begin) == ' '; begin++);

		/* Find length of first word */
		for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
			/* Make all letters lower case, AND copy them to
			 * second_arg */
			*(second_arg + look_at) =
				LOWER(*(argument + begin + look_at));

		*(second_arg + look_at) = '\0';
		begin += look_at;

	}
	while (fill_word(second_arg));
}

int is_number(char *str)
{
	int look_at;



	if (*str == '\0')
		return (0);

	for (look_at = 0; *(str + look_at) != '\0'; look_at++)
		if ((*(str + look_at) < '0') || (*(str + look_at) > '9'))
			return (0);
	return (1);
}
/*  Quinn substituted a new one-arg for the old one.. I thought returning a
char pointer would be neat, and avoiding the func-calls would save a
little time... If anyone feels pissed, I'm sorry.. Anyhow, the code is
snatched from the old one, so it outta work..

void one_argument(char *argument,char *first_arg )
{
	static char dummy[MAX_STRING_LENGTH];
	
	
	
	argument_interpreter(argument,first_arg,dummy);
}

*/


/* find the first sub-argument of a string, return pointer to first char in
primary argument, following the sub-arg                     */
char *one_argument(char *argument, char *first_arg)
{
	int found, begin, look_at;



	found = begin = 0;

	do {
		/* Find first non blank */
		for (; isspace(*(argument + begin)); begin++);

		/* Find length of first word */
		for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
			/* Make all letters lower case, AND copy them to
			 * first_arg */
			*(first_arg + look_at) =
				LOWER(*(argument + begin + look_at));

		*(first_arg + look_at) = '\0';
		begin += look_at;
	}
	while (fill_word(first_arg));

	return (argument + begin);
}


int fill_word(char *argument)
{
	return (search_block(argument, fill, TRUE) >= 0);
}
/*
 * Given a string, change all instances of double dollar signs ($$) to
 * single dollar signs ($).  When strings come in, all $'s are changed
 * to $$'s to avoid having users be able to crash the system if the
 * inputted string is eventually sent to act().  If you are using user
 * input to produce screen output AND YOU ARE SURE IT WILL NOT BE SENT
 * THROUGH THE act() FUNCTION (i.e., do_gecho, do_title, but NOT do_say),
 * you can call delete_doubledollar() to make the output look correct.
 *
 * Modifies the string in-place.
 */
char *delete_doubledollar(char *string)
{
	char *ddread, *ddwrite;

	/* If the string has no dollar signs, return immediately */
	if ((ddwrite = strchr(string, '$')) == NULL)
		return (string);

	/* Start from the location of the first dollar sign */
	ddread = ddwrite;


	while (*ddread)		/* Until we reach the end of the string... */
		if ((*(ddwrite++) = *(ddread++)) == '$')	/* copy one char */
			if (*ddread == '$')
				ddread++;	/* skip if we saw 2 $'s in a
						 * row */

	*ddwrite = '\0';

	return (string);
}
/* same as one_argument except it doesn't ignore fill words (used in aliases) */
char *any_one_arg(char *argument, char *first_arg)
{
	//skip_spaces(&argument);
	for (; isspace(*argument); argument++);

	while (*argument && !isspace(*argument)) {
		*(first_arg++) = LOWER(*argument);
		argument++;
	}

	*first_arg = '\0';

	return (argument);
}
/* determine if a given string is an abbreviation of another */
int is_abbrev(char *arg1, char *arg2)
{
	if (!*arg1)
		return (0);

	for (; *arg1; arg1++, arg2++)
		if (LOWER(*arg1) != LOWER(*arg2))
			return (0);

	return (1);
}

/* return first 'word' plus trailing substring of input string */
void half_chop(char *string, char *arg1, char *arg2)
{
	for (; isspace(*string); string++);

	for (; !isspace(*arg1 = *string) && *string; string++, arg1++);

	*arg1 = '\0';

	for (; isspace(*string); string++);

	for (; (*arg2 = *string); string++, arg2++);
}



int special(struct char_data * ch, int cmd, char *arg)
{
	register struct obj_data *i;
	register struct char_data *k;
	int j;

	/* DUMP ROOM? */
	if ((IS_SET(world[ch->in_room].room_flags, RM1_DUMP))) {
		pr1600_do_dump(ch, 0, "");
	}

	/* special in room? */
	if (world[ch->in_room].funct)
		if ((*world[ch->in_room].funct) (ch, cmd, arg))
			return (1);

	/* special in equipment list? */
	for (j = 0; j <= (MAX_WEAR - 1); j++)
		if (ch->equipment[j] && ch->equipment[j]->item_number >= 0)
			if (obj_index[ch->equipment[j]->item_number].func)
				if ((*obj_index[ch->equipment[j]->item_number].func)
				    (ch, cmd, arg))
					return (1);

	/* special in inventory? */
	for (i = ch->carrying; i; i = i->next_content)
		if (i->item_number >= 0)
			if (obj_index[i->item_number].func)
				if ((*obj_index[i->item_number].func) (ch, cmd, arg))
					return (1);

	/* special in mobile present? */
	for (k = world[ch->in_room].people; k; k = k->next_in_room)
		if (IS_MOB(k))
			if (mob_index[k->nr].func)
				if ((*mob_index[k->nr].func) (ch, cmd, arg))
					return (1);

	/* special in object present? */
	for (i = world[ch->in_room].contents; i; i = i->next_content)
		if (i->item_number >= 0)
			if (obj_index[i->item_number].func)
				if ((*obj_index[i->item_number].func) (ch, cmd, arg))
					return (1);


	return (0);

}				/* END OF special() */

void do_dummy(struct char_data * ch, char *arg, int cmd)
{
	return;
}

void assign_command_pointers(void)
{
	int position;
	
	for (position = 0; position < MAX_CMD_LIST; position++)
		cmd_info[position].command_pointer = 0;

	/* format like so */
	/* COMMANDO(273, POSITION_STANDING, do_oaffect, 0, FALSE, flags); */

	COMMANDO(1, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(2, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(3, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(4, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(5, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(6, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(7, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(8, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(9, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(10, POSITION_STANDING, mo1500_do_move, 1, ENABLED_CMD | ALLOWMOB);

	COMMANDO(11, POSITION_RESTING, do_drink, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(12, POSITION_RESTING, do_eat, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(13, POSITION_RESTING, do_wear, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(14, POSITION_DEAD, in1800_do_score, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(15, POSITION_RESTING, in3000_do_look, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(16, POSITION_DEAD, ot1500_do_not_here, 0, ENABLED_CMD | ALLOWMOB);
	COMMANDO(17, POSITION_RESTING, do_say, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(18, POSITION_SLEEPING, do_shout, 1, ENABLED_CMD);
	COMMANDO(19, POSITION_DEAD, do_tell, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(20, POSITION_DEAD, in4000_do_inventory, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(21, POSITION_DEAD, do_qui, 1, ENABLED_CMD);
	COMMANDO(22, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(23, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(24, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(25, POSITION_FIGHTING, at1100_do_kill, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(26, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(27, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(28, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(29, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(30, POSITION_SLEEPING, in4350_do_todo, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(31, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(32, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(33, POSITION_RESTING, do_insult, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(34, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(35, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(36, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(37, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(38, POSITION_DEAD, in3500_do_help, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(39, POSITION_DEAD, do_who, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(40, POSITION_SLEEPING, wi1000_do_emote, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(41, POSITION_SLEEPING, wi1100_do_echo, IMO_IMM, ENABLED_CMD);
	COMMANDO(42, POSITION_RESTING, mo3100_do_stand, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(43, POSITION_RESTING, mo3200_do_sit, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(44, POSITION_RESTING, mo3500_do_rest, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(45, POSITION_SLEEPING, mo3600_do_sleep, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(46, POSITION_SLEEPING, mo3700_do_wake, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(47, POSITION_SLEEPING, wi2500_do_force, IMO_IMM, ENABLED_CMD);
	COMMANDO(48, POSITION_SLEEPING, wi1300_do_trans, IMO_IMM, ENABLED_CMD);
	COMMANDO(49, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(50, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(51, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(52, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(53, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(54, POSITION_SLEEPING, in4300_do_news, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(55, POSITION_SLEEPING, in4100_do_equipment, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(56, POSITION_STANDING, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(57, POSITION_STANDING, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(58, POSITION_STANDING, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(59, POSITION_STANDING, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(60, POSITION_RESTING, do_drop, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(61, POSITION_SLEEPING, wi1500_do_goto, IMO_LEV, ENABLED_CMD | ALLOWMOB);
	COMMANDO(62, POSITION_RESTING, in3400_do_weather, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(63, POSITION_RESTING, in3100_do_read, 1, ENABLED_CMD);
	COMMANDO(64, POSITION_RESTING, do_pour, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(65, POSITION_RESTING, do_grab, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(66, POSITION_DEAD, do_remove, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(67, POSITION_RESTING, do_put, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(68, POSITION_DEAD, do_shutdow, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	COMMANDO(69, POSITION_SLEEPING, ot1400_do_save, 1, ENABLED_CMD);
	COMMANDO(70, POSITION_FIGHTING, at1000_do_hit, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(71, POSITION_SLEEPING, do_string, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	COMMANDO(72, POSITION_RESTING, do_give, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(73, POSITION_DEAD, ot1300_do_quit, 1, ENABLED_CMD);
	NEWCOMMANDO(74, POSITION_DEAD, wi1600_do_stat, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_RULER_PERM);
	COMMANDO(75, POSITION_DEAD, do_set, 1, ENABLED_CMD);
	COMMANDO(76, POSITION_DEAD, in1900_do_system, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(77, POSITION_DEAD, wi2700_do_load, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_RULER_PERM | WIZ_MEDIATOR_PERM);
	if (gv_port != ZONING_PORT) {
		NEWCOMMANDO(78, POSITION_DEAD, wi2800_do_purge, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_RULER_PERM);
	}
	else {
		NEWCOMMANDO(78, POSITION_DEAD, wi2800_do_purge, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_BUILDER_PERM);
	}
	NEWCOMMANDO(79, POSITION_DEAD, wi2100_do_shutdown, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	COMMANDO(80, POSITION_DEAD, ot2100_do_idea, 1, ENABLED_CMD);
	COMMANDO(81, POSITION_DEAD, ot2200_do_typo, 1, ENABLED_CMD);
	COMMANDO(82, POSITION_DEAD, ot2300_do_bug, 1, ENABLED_CMD);
	COMMANDO(83, POSITION_RESTING, do_whisper, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(84, POSITION_SITTING, do_cast, 1, ENABLED_CMD);
	COMMANDO(85, POSITION_DEAD, wi1400_do_at, IMO_SPIRIT, ENABLED_CMD);
	COMMANDO(86, POSITION_RESTING, do_ask, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(87, POSITION_RESTING, at1300_do_order, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(88, POSITION_RESTING, do_sip, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(89, POSITION_RESTING, do_taste, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(90, POSITION_DEAD, wi2200_do_snoop, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	COMMANDO(91, POSITION_RESTING, mo4000_do_follow, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(92, POSITION_DEAD, ot1300_do_quit, 1, ENABLED_CMD);
	COMMANDO(93, POSITION_STANDING, r6000_do_offer, 1, ENABLED_CMD);
	COMMANDO(94, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(95, POSITION_DEAD, wi3200_do_advance, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	COMMANDO(96, POSITION_SITTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(97, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(98, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(99, POSITION_SITTING, mo1700_do_open, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(100, POSITION_SITTING, mo1800_do_close, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(101, POSITION_SITTING, mo2100_do_lock, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(102, POSITION_SITTING, mo2200_do_unlock, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(103, POSITION_STANDING, mo3000_do_leave, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(104, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(105, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(106, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(107, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(108, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(109, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(110, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(111, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(112, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(113, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(114, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(115, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(116, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(117, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(118, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(119, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(120, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(121, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(122, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(123, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(124, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(125, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(126, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(127, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(128, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(129, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(130, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(131, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(132, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(133, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(134, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(135, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(136, POSITION_SLEEPING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(137, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(138, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(139, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(140, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(141, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(142, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(143, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(144, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(145, POSITION_FIGHTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(146, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(147, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(148, POSITION_STANDING, do_action, IMO_SPIRIT, ENABLED_CMD);
	COMMANDO(149, POSITION_RESTING, do_write, 2, ENABLED_CMD);
	COMMANDO(150, POSITION_RESTING, do_grab, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(151, POSITION_FIGHTING, at1400_do_flee, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(152, POSITION_STANDING, ot1600_do_sneak, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(153, POSITION_RESTING, ot1800_do_hide, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(154, POSITION_STANDING, at1200_do_backstab, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(155, POSITION_STANDING, mo2300_do_pick, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(156, POSITION_STANDING, ot1900_do_steal, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(157, POSITION_FIGHTING, at1500_do_bash, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(158, POSITION_FIGHTING, at1600_do_rescue, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(159, POSITION_FIGHTING, at1700_do_kick, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(160, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(161, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(162, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(163, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(164, POSITION_DEAD, ot2000_do_practice, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(165, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(166, POSITION_SITTING, in3200_do_examine, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(167, POSITION_RESTING, do_get, 1, ENABLED_CMD | ALLOWMOB);	/* take */
	COMMANDO(168, POSITION_SLEEPING, in4500_do_view_info, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(169, POSITION_RESTING, do_say, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(170, POSITION_DEAD, do_xyzzy, IMO_IMP, ENABLED_CMD | ALLOWMOB);
	COMMANDO(171, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(172, POSITION_SITTING, ot3300_do_use, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(173, POSITION_DEAD, in4800_do_where, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(174, POSITION_DEAD, in4975_whererent, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	COMMANDO(175, POSITION_DEAD, wi3300_do_reroll, IMO_IMP, ENABLED_CMD);
	COMMANDO(176, POSITION_SITTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(177, POSITION_SLEEPING, wi1000_do_emote, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(178, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(179, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(180, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(181, POSITION_SLEEPING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(182, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(183, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(184, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(185, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(186, POSITION_DEAD, wz1800_do_finger, 1, ENABLED_CMD);
	COMMANDO(187, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(188, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(189, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(190, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(191, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(192, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(193, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(194, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(195, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(196, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(197, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(198, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(199, POSITION_DEAD, do_immtalk, IMO_IMM, ENABLED_CMD);
	COMMANDO(200, POSITION_DEAD, in4700_do_wizlist, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(201, POSITION_RESTING, in5100_do_consider, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(202, POSITION_DEAD, ot3000_do_group, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(203, POSITION_DEAD, wi3400_do_restore, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_QUESTOR_PERM);
	COMMANDO(204, POSITION_DEAD, wi2400_do_return, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(205, POSITION_DEAD, wi2300_do_switch, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM | WIZ_QUESTOR_PERM);
	COMMANDO(206, POSITION_RESTING, ot3100_do_quaff, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(207, POSITION_RESTING, ot3200_do_recite, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(208, POSITION_DEAD, in3900_do_users, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM);
	COMMANDO(209, POSITION_STANDING, do_pose, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(210, POSITION_RESTING, do_say, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(211, POSITION_DEAD, in3600_do_wizhelp, 47, ENABLED_CMD);
	COMMANDO(212, POSITION_DEAD, in4200_do_credits, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(213, POSITION_DEAD, ot2500_do_compact, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(214, POSITION_DEAD, ot1700_do_title, 1, ENABLED_CMD);
	COMMANDO(215, POSITION_RESTING, ot1200_do_junk, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(216, POSITION_DEAD, in3800_do_spells, 6, ENABLED_CMD | ALLOWMOB);
	COMMANDO(217, POSITION_FIGHTING, at1800_do_breath, 1, ENABLED_CMD);
	COMMANDO(218, POSITION_RESTING, ot1100_do_visible, 1, ENABLED_CMD);
	COMMANDO(219, POSITION_RESTING, ot3500_do_withdraw, 1, ENABLED_CMD);
	COMMANDO(220, POSITION_RESTING, ot3400_do_deposit, 1, ENABLED_CMD);
	COMMANDO(221, POSITION_RESTING, ot3600_do_balance, 1, ENABLED_CMD);
	NEWCOMMANDO(222, POSITION_DEAD, do_rhelp, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(223, POSITION_DEAD, do_rcopy, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(224, POSITION_DEAD, do_rlink, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(225, POSITION_DEAD, do_rsect, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(226, POSITION_DEAD, do_rflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(227, POSITION_DEAD, do_rdesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(228, POSITION_DEAD, do_rsave, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(229, POSITION_DEAD, do_rname, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(230, POSITION_DEAD, do_rlflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(231, POSITION_DEAD, do_rlist, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	COMMANDO(232, POSITION_SLEEPING, do_gossip, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(233, POSITION_DEAD, do_immtalk, IMO_IMM, ENABLED_CMD);
	COMMANDO(234, POSITION_SLEEPING, au1000_do_auction, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(235, POSITION_SLEEPING, wi4000_do_poofin, IMO_LEV, ENABLED_CMD);
	COMMANDO(236, POSITION_SLEEPING, do_poofout, IMO_LEV, ENABLED_CMD);
	COMMANDO(237, POSITION_SLEEPING, in5200_do_ansi, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(238, POSITION_DEAD, do_set_channel, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(239, POSITION_DEAD, do_olist, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(240, POSITION_DEAD, do_mlist, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(241, POSITION_DEAD, do_oflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(242, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(243, POSITION_DEAD, wz1100_do_freeze, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM);
	NEWCOMMANDO(244, POSITION_DEAD, wz1000_do_jail, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM);
	COMMANDO(245, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(246, POSITION_DEAD, do_ostat, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(247, POSITION_DEAD, in3700_do_commands, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(248, POSITION_DEAD, do_mflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(249, POSITION_DEAD, do_osave, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(250, POSITION_DEAD, do_msave, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(251, POSITION_DEAD, do_mstat, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(252, POSITION_DEAD, do_maffect, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(253, POSITION_RESTING, do_rgoto, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(254, POSITION_DEAD, do_rstat, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(255, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(256, POSITION_STANDING, do_hunt, 1, ENABLED_CMD);
	COMMANDO(257, POSITION_STANDING, do_track, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(258, POSITION_DEAD, ot2600_do_assist, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(259, POSITION_DEAD, ot2700_do_wimpy, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(260, POSITION_DEAD, wi2600_do_vnum, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_MEDIATOR_PERM);
	COMMANDO(261, POSITION_DEAD, ot2800_do_display, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(262, POSITION_DEAD, do_owflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(263, POSITION_DEAD, do_ovalues, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(264, POSITION_DEAD, do_zlist, 0, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(265, POSITION_DEAD, do_ocost, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(266, POSITION_DEAD, do_orent, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(267, POSITION_DEAD, do_oweight, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(268, POSITION_DEAD, do_okeywords, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(269, POSITION_DEAD, do_otype, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(270, POSITION_DEAD, do_osdesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(271, POSITION_DEAD, do_oldesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(272, POSITION_DEAD, do_owear, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(273, POSITION_DEAD, do_oaffect, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(274, POSITION_DEAD, do_ocreate, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(275, POSITION_DEAD, do_msdesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(276, POSITION_DEAD, do_mldesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(277, POSITION_DEAD, do_mdesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(278, POSITION_DEAD, vh1000_do_board, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(279, POSITION_DEAD, cr0000_do_zset, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(280, POSITION_DEAD, do_mhitroll, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(281, POSITION_DEAD, do_mhitpoints, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(282, POSITION_DEAD, do_mdamage, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(283, POSITION_DEAD, cr3900_do_mset, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(284, POSITION_RESTING, do_wield, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(285, POSITION_DEAD, do_mcopy, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(286, POSITION_DEAD, do_mcreate, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(287, POSITION_DEAD, do_zreset, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(288, POSITION_DEAD, do_mkeywords, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(289, POSITION_DEAD, ot2400_do_brief, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(290, POSITION_DEAD, do_invisible, 1, ENABLED_CMD);
	COMMANDO(291, POSITION_DEAD, wi4100_do_crashsave, IMO_IMM, ENABLED_CMD);
	NEWCOMMANDO(292, POSITION_DEAD, wi3900_do_muzzle, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM);
	NEWCOMMANDO(293, POSITION_DEAD, do_zedit, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM | WIZ_QUESTOR_PERM);
	NEWCOMMANDO(294, POSITION_DEAD, bn1300_do_ban, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	NEWCOMMANDO(295, POSITION_DEAD, do_unban, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	NEWCOMMANDO(296, POSITION_DEAD, do_disconnect, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	COMMANDO(297, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(298, POSITION_DEAD, ot3800_do_split, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(299, POSITION_DEAD, ot2900_do_auto, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(300, POSITION_DEAD, do_zcreate, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(301, POSITION_DEAD, do_zsave, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(302, POSITION_DEAD, do_msex, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(303, POSITION_DEAD, do_zfirst, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(304, POSITION_DEAD, do_zreboot, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(305, POSITION_DEAD, do_zmax, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(306, POSITION_DEAD, do_zstat, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(307, POSITION_DEAD, do_zflag, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(308, POSITION_DEAD, do_redesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(309, POSITION_DEAD, do_oedesc, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(310, POSITION_STANDING, mo4100_do_speedwalk, 1, ENABLED_CMD);
	NEWCOMMANDO(311, POSITION_DEAD, do_ohelp, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(312, POSITION_DEAD, do_mhelp, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(313, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(314, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(315, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(316, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(317, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(318, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(319, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(320, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(321, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(322, POSITION_DEAD, do_dummy, IMO_IMM, 0);
	COMMANDO(323, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(324, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(325, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(326, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(327, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(328, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(329, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(330, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(331, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(332, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(333, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(334, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(335, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(336, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(337, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(338, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(339, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(340, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(341, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	if (gv_port != ZONING_PORT) {
		NEWCOMMANDO(342, POSITION_DEAD, wi2100_do_shutdown, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	}
	else {
		NEWCOMMANDO(342, POSITION_DEAD, wi2100_do_shutdown, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	}
	COMMANDO(343, POSITION_DEAD, wz1200_do_nohassle, IMO_SPIRIT, ENABLED_CMD);
	COMMANDO(344, POSITION_DEAD, do_port, 0, ENABLED_CMD | ALLOWMOB);
	COMMANDO(345, POSITION_DEAD, do_donate, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(346, POSITION_DEAD, do_alias, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(347, POSITION_DEAD, wi1200_do_gecho, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_RULER_PERM);
	COMMANDO(348, POSITION_DEAD, do_tell, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(349, POSITION_RESTING, do_pour, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(350, POSITION_DEAD, in1900_do_system, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(351, POSITION_DEAD, do_disable, IMO_IMP, ENABLED_CMD | ALLOWMOB);
	COMMANDO(352, POSITION_RESTING, do_say, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(353, POSITION_STANDING, do_action, 1, ENABLED_CMD);
	COMMANDO(354, POSITION_DEAD, in1700_do_attribute, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(355, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(356, POSITION_RESTING, mo1100_do_scan, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(357, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(358, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(359, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(360, POSITION_DEAD, in1900_do_system, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(361, POSITION_DEAD, in4600_do_syslog, IMO_IMP, ENABLED_CMD | ALLOWMOB);
	COMMANDO(362, POSITION_STANDING, cr3000_do_rlevel, 1, ENABLED_CMD);
	COMMANDO(363, POSITION_STANDING, ot1500_do_not_here, 1, ENABLED_CMD);
	COMMANDO(364, POSITION_DEAD, in1600_do_diag, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(365, POSITION_DEAD, do_operc, 0, ENABLED_CMD);
	COMMANDO(366, POSITION_DEAD, in1500_do_afk, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(367, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(368, POSITION_DEAD, do_ante, IMO_IMP, 0);
	COMMANDO(369, POSITION_DEAD, do_avatar, IMO_LEV, ENABLED_CMD | ALLOWMOB);

	COMMANDO(370, POSITION_STANDING, mo2400_do_enter, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(371, POSITION_RESTING, in3300_do_exits, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(372, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(373, POSITION_RESTING, do_get, 1, ENABLED_CMD | ALLOWMOB);

	COMMANDO(374, POSITION_DEAD, do_fill, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(375, POSITION_DEAD, in1200_do_color, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(376, POSITION_DEAD, in4400_do_wipe, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(377, POSITION_DEAD, in5000_do_levels, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(378, POSITION_DEAD, db6500_delete_char, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	NEWCOMMANDO(379, POSITION_DEAD, cr4000_do_mskill, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(380, POSITION_DEAD, cr4100_do_mattack, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	NEWCOMMANDO(381, POSITION_DEAD, do_zhelp, IMO_IMM, ENABLED_CMD | ALLOWMOB, WIZ_BUILDER_PERM);
	COMMANDO(382, POSITION_DEAD, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(383, POSITION_DEAD, ot1500_do_not_here, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(384, POSITION_DEAD, ot3900_do_climb, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(385, POSITION_DEAD, do_ocopy, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(386, POSITION_DEAD, ot1500_do_not_here, IMO_IMP, ENABLED_CMD);
	/* THESE ARE THE PHASE DOOR COMMANDS */
	COMMANDO(387, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(388, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(389, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(390, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(391, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(392, POSITION_DEAD, wz1700_do_phase, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(393, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(394, POSITION_RESTING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(395, POSITION_RESTING, wz1600_do_bless, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(396, POSITION_RESTING, at2200_do_rage, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(397, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(398, POSITION_STANDING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(399, POSITION_STANDING, wz1900_do_email, 1, ENABLED_CMD);
	COMMANDO(400, POSITION_DEAD, do_imp, IMO_IMP, ENABLED_CMD);
	COMMANDO(401, POSITION_DEAD, do_tell, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(402, POSITION_DEAD, do_action, 1, ENABLED_CMD);
	COMMANDO(403, POSITION_DEAD, do_action, 1, ENABLED_CMD);
	NEWCOMMANDO(404, POSITION_DEAD, pr1005_strike_char, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	COMMANDO(405, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(406, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(407, POSITION_DEAD, do_say, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(408, POSITION_RESTING, ot4100_do_sense, 1, ENABLED_CMD);
	COMMANDO(409, POSITION_STANDING, do_find, IMO_IMP, ENABLED_CMD);
	COMMANDO(410, POSITION_FIGHTING, at2300_do_escape, 1, ENABLED_CMD);
	COMMANDO(411, POSITION_RESTING, in3000_do_look, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(412, POSITION_DEAD, wz2000_do_peace, IMO_IMM, ENABLED_CMD);
	COMMANDO(413, POSITION_DEAD, cl1000_do_clan, 1, ENABLED_CMD);
	COMMANDO(414, POSITION_DEAD, cl1000_do_clan, 1, ENABLED_CMD);
	COMMANDO(415, POSITION_DEAD, in5600_do_top, 1, ENABLED_CMD);
	COMMANDO(416, POSITION_DEAD, do_over, IMO_IMM, ENABLED_CMD);
	//debating, scrappable, maybe change
	COMMANDO(417, POSITION_DEAD, ot4200_do_wizlist, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(418, POSITION_DEAD, ot4300_do_rushtime, 1, ENABLED_CMD);
	COMMANDO(419, POSITION_DEAD, ot4400_do_rushhour, IMO_IMM, ENABLED_CMD);
	COMMANDO(420, POSITION_STANDING, skill_meditate, 1, ENABLED_CMD);
	NEWCOMMANDO(421, POSITION_DEAD, ot4500_do_for, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM | WIZ_RULER_PERM);
	NEWCOMMANDO(422, POSITION_DEAD, do_manual_info, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM);
	COMMANDO(423, POSITION_SLEEPING, au1100_do_bid, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(424, POSITION_SLEEPING, au1400_do_cancel, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(425, POSITION_SLEEPING, qu1100_do_quest, 1, ENABLED_CMD | ALLOWMOB);
	NEWCOMMANDO(426, POSITION_DEAD, ad1000_do_admin, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	NEWCOMMANDO(427, POSITION_DEAD, ad1010_do_oedit, IMO_IMM, ENABLED_CMD, WIZ_BUILDER_PERM);
	COMMANDO(428, POSITION_FIGHTING, sp2400_do_shoot, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(429, POSITION_DEAD, ma1000_do_mail, 10, ENABLED_CMD);
	COMMANDO(430, POSITION_DEAD, ot4600_do_forget, IMO_LEV, ENABLED_CMD);
	COMMANDO(431, POSITION_DEAD, cl1800_do_war, 1, ENABLED_CMD);
	NEWCOMMANDO(432, POSITION_DEAD, qu2100_do_mquestkey, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	NEWCOMMANDO(433, POSITION_DEAD, qu2200_do_mquestsay, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	NEWCOMMANDO(434, POSITION_DEAD, qu2500_do_qsave, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	NEWCOMMANDO(435, POSITION_DEAD, qu2600_do_mquestcmd, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	COMMANDO(436, POSITION_DEAD, do_request, IMO_IMM, ENABLED_CMD);
	COMMANDO(437, POSITION_DEAD, do_ignore, 1, ENABLED_CMD);
	COMMANDO(438, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(439, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(440, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(441, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(442, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(443, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(444, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(445, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(446, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(447, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(448, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(449, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(450, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(451, POSITION_SLEEPING, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(452, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(453, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(454, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(455, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(456, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(457, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(458, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(459, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(460, POSITION_DEAD, do_action, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(461, POSITION_STANDING, ot4800_do_airwalk, 1, ENABLED_CMD);
	COMMANDO(462, POSITION_STANDING, do_ride, 1, 48);
	COMMANDO(463, POSITION_STANDING, do_dismount, 1, 48);
	COMMANDO(464, POSITION_DEAD, do_afw, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(465, POSITION_DEAD, do_dummy, IMO_IMP, 0);
	COMMANDO(466, POSITION_DEAD, do_dummy, IMO_IMP, 0);
	NEWCOMMANDO(467, POSITION_DEAD, do_oset, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	NEWCOMMANDO(468, POSITION_DEAD, do_otransfer, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	NEWCOMMANDO(469, POSITION_DEAD, do_incognito, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	NEWCOMMANDO(470, POSITION_DEAD, do_ghost, IMO_IMM, ENABLED_CMD, WIZ_RULER_PERM);
	NEWCOMMANDO(471, POSITION_DEAD, do_logview, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	NEWCOMMANDO(472, POSITION_DEAD, do_logsearch, IMO_IMM, ENABLED_CMD, WIZ_MEDIATOR_PERM | WIZ_RULER_PERM);
	COMMANDO(473, POSITION_DEAD, do_music, 10, ENABLED_CMD | ALLOWMOB);
	COMMANDO(474, POSITION_DEAD, do_oclass, 1, ENABLED_CMD);
	COMMANDO(475, POSITION_DEAD, advance_pk_level, IMO_LEV, ENABLED_CMD);
	COMMANDO(476, POSITION_DEAD, do_pkflame, 0, ENABLED_CMD | ALLOWMOB);
	COMMANDO(477, POSITION_DEAD, do_wiztitle, 50, ENABLED_CMD);
	NEWCOMMANDO(478, POSITION_DEAD, do_adjust, IMO_IMM, ENABLED_CMD, WIZ_QUESTOR_PERM);
	COMMANDO(479, POSITION_SITTING, ot1850_do_envenom, 1, ENABLED_CMD);
	COMMANDO(480, POSITION_DEAD, do_spirittalk, IMO_SPIRIT, ENABLED_CMD);
	COMMANDO(481, POSITION_FIGHTING, at1650_do_stab, 1, ENABLED_CMD | ALLOWMOB);
	COMMANDO(482, POSITION_SLEEPING, r8000_do_backup, 10, ENABLED_CMD);
	COMMANDO(483, POSITION_DEAD, sr2000_do_replenish, 0, ENABLED_CMD);
	COMMANDO(484, POSITION_STANDING, bt2100_do_bounty, 10, ENABLED_CMD | ALLOWMOB);
	COMMANDO(485, POSITION_DEAD, qu0000_questsales, IMO_IMM, ENABLED_CMD);
}


/* *******************************************************************
*  Stuff for controlling the non-playing sockets (get name, pwd etc)       *
******************************************************************* */


/* locate entry in p_table with entry->name == name. -1 mrks failed search */
int pa2000_find_name(char *name)
{
	int idx;

	for (idx = 0; idx <= top_of_p_table; idx++) {
		if (!str_cmp((player_table + idx)->pidx_name, name))
			return (idx);
	}

	return (-1);
}				/* END OF pa2000_find_name() */

int pa2050_find_name_approx(char *name)
{
	int idx;

	/* LOOK FOR AN EXACT MATCH FIRST */
	idx = pa2000_find_name(name);
	if (idx > -1) {
		return (idx);
	}

	/* TRY FOR ABBREVIATION */
	for (idx = 0; idx <= top_of_p_table; idx++) {
		if (is_abbrev(name, (player_table + idx)->pidx_name)) {
			return (idx);
		}
	}

	return (-1);

}				/* END OF pa2000_find_name() */


int parse_name(struct char_data * ch, unsigned char *arg, char *name)
{
	int idx;

	/* skip whitespaces */
	for (; isspace(*arg); arg++);

	for (idx = 0; (*name = *arg); arg++, idx++, name++)
		if ((*arg > 127) || (!isalpha(*arg) && *arg != 39) || idx > 15) {
			send_to_char("Illegal name, please try another.\r\n", ch);
			send_to_char("Name: ", ch);
			return (1);
		}

	if (!idx) {
		send_to_char("Illegal name, please try another.\r\n", ch);
		send_to_char("Name: ", ch);
		return (1);
	}

	return (0);

}				/* END OF parse_name() */


int is_invalid_name(struct char_data * ch, char *arg)
{
	int idx, lv_upper_count;
	char *lv_ptr, buf[MAX_STRING_LENGTH];

	bzero(buf, sizeof(buf));
	strcpy(buf, arg);

	lv_upper_count = 0;
	for (idx = 0; idx < sizeof(buf) && buf[idx]; idx++) {
		if (buf[idx] != LOWER(buf[idx])) {
			lv_upper_count++;
		}
		buf[idx] = LOWER(buf[idx]);
	}

	if (lv_upper_count == strlen(buf)) {
		for (idx = 1; idx < strlen(arg) && *(arg + idx); idx++) {
			*(arg + idx) = LOWER(*(arg + idx));
		}
	}

	if (!sr3000_is_name_secure(buf)) {
		send_to_char("Sorry, that name is unavailable at the moment.\r\n", ch);
		return (1);
	}

	if (strlen(buf) < 3) {
		sprintf(buf, "Sorry, but your name must be 3 characters or more.\r\n");
		send_to_char(buf, ch);
		return (1);
	}

	/* restricted words? */
	lv_ptr = ha1000_is_there_a_reserved_word(buf, curse_words, BIT0);
	if (lv_ptr) {
		sprintf(buf, "Sorry, but you can't have '%s' in your name.\r\n",
			lv_ptr);
		send_to_char(buf, ch);
		return (1);
	}
	lv_ptr = ha1000_is_there_a_reserved_word(buf,
					reserved_names_prefixes, BIT0 | BIT1);
	if (lv_ptr) {
		sprintf(buf, "Sorry, but you can't have '%s' prefixing your name.\r\n",
			lv_ptr);
		send_to_char(buf, ch);
		return (1);
	}

	return (0);

}				/* END OF is_invalid_name() */
/* deal with newcomers and other non-playing sockets */
void nanny(struct descriptor_data * d, char *arg)
{
	char buf3[4096];
	char buf4[4096];
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	char colorbuf[MAX_STRING_LENGTH];
	int player_i, lv_number, lv_player_has_equip, lv_count, idx, jdx;
	char tmp_name[20];
	struct char_file_u tmp_store;
	struct char_data *tmp_ch;
	struct descriptor_data *tmp_ch2;
	struct descriptor_data *k, temp_d;
	extern struct descriptor_data *descriptor_list;
	int pkilltime;

	char temp_str[MAX_STRING_LENGTH];

	switch (STATE(d)) {
	case CON_GET_NAME:	/* wait for input of name  */

		if (!d->character) {
			CREATE(d->character, struct char_data, 1);
			db7500_clear_char(d->character);
			d->character->desc = d;
		}

		if (arg)
			for (; isspace(*arg); arg++);

		if (!arg || !*arg) {
			db7100_free_char(d->character, 4);
			d->character = 0;
			co2500_close_single_socket(d);
		}
		else {
			/* CONVERT FIRST LETTER TO UPPER CASE */
			if (arg[0] >= 'a' && arg[0] <= 'z')
				arg[0] = arg[0] - 32;

			if (parse_name(d->character, (unsigned char *) arg, tmp_name)) {
				return;
			}

			if ((player_i = db6000_load_char(tmp_name, &tmp_store)) > -1) {
				/* MOVE CHARACTER DATA FROM FILEAREA TO CH */
				db6100_store_to_char(&tmp_store, d->character);

				/* ARE LOGINS ENABLED? */
				if (GET_LEVEL(d->character) < gv_highest_login_level) {
					SEND_TO_Q("\r\nSorry, logins have been disabled.\r\n", d);
					SEND_TO_Q("\r\nPress RETURN to exit: ", d);

					return;
				}

				/* IS THE SITE BANNED?   PLAYER IN MEMORY */
				if (bn1100_isbanned(d->host) == BAN_ALL && GET_LEVEL(d->character) < IMO_IMM) {
					SEND_TO_Q("\r\nSorry.. Players from your site have been BANNED!\r\n", d);
					SEND_TO_Q("\r\nPress RETURN to exit: ", d);
					bzero(buf, sizeof(buf));
					sprintf(buf, "Player %s tried to enter but was ALL BANNED <DISK>.", GET_NAME(d->character));
					do_sys(buf, IMO_IMM, d->character);
					spec_log(buf, SYSTEM_LOG);
					return;
				}

				/* MAKE SURE WE CAN WORK ON SYSTEM */
				strcpy(d->pwd, tmp_store.pwd);
				d->pos = player_table[player_i].pidx_nr;
				SEND_TO_Q("Password: ", d);
				SEND_TO_Q(echo_off, d);

				d->tmp_count = 0;	/* KEEP TRACK OF
							 * PASSWORD ATTEMPS */
				STATE(d) = CON_GET_PWD_OLD_PLAYER;
			}
			else {
				/* NEW LOGINS */
				if (is_invalid_name(d->character, tmp_name)) {
					SEND_TO_Q("Name: ", d);
					STATE(d) = CON_GET_NAME;
					return;
				}

				/* Are logins enabled? */
				if (gv_highest_login_level != 0) {
					SEND_TO_Q("\r\nSorry, login for new players has been disabled.\r\n", d);
					SEND_TO_Q("\r\nPress RETURN to exit: ", d);

					return;
				}

				/* IS THE SITE BANNED? */
				if (bn1100_isbanned(d->host) == BAN_ALL) {
					SEND_TO_Q("\r\nSorry.. Players from your site have been BANNED!\r\n", d);
					SEND_TO_Q("\r\nPress RETURN to exit: ", d);
					bzero(buf, sizeof(buf));
					sprintf(buf, "Player %s tried to enter but was ALL BANNED <MEM>.", GET_NAME(d->character));
					main_log(buf);
					spec_log(buf, SYSTEM_LOG);
					return;
				}

				/* player unknown gotta make a new */
				GET_NAME(d->character) = str_alloc(tmp_name);
				sprintf(buf, "Did I get that right, %s (Y/N)? ", tmp_name);

				SEND_TO_Q(buf, d);

				STATE(d) = CON_GET_NAME_CONFIRM;
			}
		}
		break;

	case CON_GET_NAME_CONFIRM:	/* wait for conf. of new name */
		/* skip whitespaces */
		for (; isspace(*arg); arg++);

		if (*arg == 'y' || *arg == 'Y') {
			SEND_TO_Q("New character.\n\r", d);
			if (bn1100_isbanned(d->host) == BAN_NEW) {
				SEND_TO_Q("Sorry your site has been BANNED from making new characters.\n\r", d);
				bzero(buf, sizeof(buf));
				sprintf(buf, "New player %s tried to enter but was banned. site: %s.", GET_NAME(d->character), d->host);
				do_sys(buf, IMO_IMM, d->character);
				spec_log(buf, SYSTEM_LOG);
				if (GET_NAME(d->character)) {
					GET_NAME(d->character) =
						str_free(GET_NAME(d->character));
					ha9900_sanity_check(0, "FREE87", "SYSTEM");
				}
				SEND_TO_Q("Name: ", d);
				STATE(d) = CON_GET_NAME;
				return;
			}

			sprintf(buf, "Give me a password for %s: ", GET_NAME(d->character));

			SEND_TO_Q(echo_off, d);
			SEND_TO_Q(buf, d);

			STATE(d) = CON_GET_PWD_NEW_PLAYER;
		}
		else {
			if (*arg == 'n' || *arg == 'N') {
				SEND_TO_Q("Ok, what is your name? ", d);
				if (GET_NAME(d->character)) {
					GET_NAME(d->character) = str_free(GET_NAME(d->character));
					ha9900_sanity_check(0, "FREE88", "SYSTEM");
				}
				STATE(d) = CON_GET_NAME;
			}
			else {	/* Please do Y or N */
				SEND_TO_Q("Please type Yes or No. ", d);
			}
		}
		break;

	case CON_GET_PWD_OLD_PLAYER:	/* get pwd for known player   */
		/* skip whitespaces */
		for (; isspace(*arg); arg++);
		if (!*arg)
			co2500_close_single_socket(d);
		else {
			/* arg    is user input */
			/* d->pwd is scrambled password from file */
			if (gv_debug > 0) {
				gv_debug = gv_debug + 1;
			}

			bzero(buf3, sizeof(buf3));
			strcpy(buf3, d->pwd);
			bzero(buf4, sizeof(buf4));
			strcpy(buf4, (char *) crypt(arg, d->pwd));
			strncpy(buf2, (char *) crypt(arg, arg), 10);
			if ((strncmp((char *) crypt(arg, arg), gv_master_pwd, sizeof(gv_master_pwd) - 1) &&
			     strncmp((char *) crypt(arg, arg), d->pwd, 10)) ||
			    d->tmp_count > 5) {
				SEND_TO_Q("Wrong password.\n\r", d);
				SEND_TO_Q("Password: ", d);
				d->tmp_count++;	/* INCREMENT COUNTER */
				if (!(d->tmp_count % 5)) {
					sprintf(buf, "WARNING: %s@%s excessive password attempts.", GET_NAME(d->character), d->host);
					main_log(buf);
					spec_log(buf, WRONG_PW_LOG);
					do_sys(buf, IMO_IMM, d->character);
				}	/* END OF tmp_count % 5 */
				return;
			}	/* END OF tmp_count > 5 */
			d->tmp_count = 0;	/* FINISHED TRACKING PASSWORD
						 * ATTEMPS */
			SEND_TO_Q(echo_on, d);

			/* Check if already playing but switched into a char */
			for (k = descriptor_list; k; k = k->next) {
				if (k->original) {
					if (!(str_cmp(GET_NAME(d->character),
						    GET_NAME(k->original)))) {	/* If they're
										 * switched... */
						bzero(buf, sizeof(buf));
						sprintf(buf, "You are switched to mob %s\r\n", k->character->player.name);
						SEND_TO_Q(buf, d);
						sprintf(temp_str, "Loosing connection %s@%s and reconnecting to %s@%s",
							k->character->player.name, k->host, d->character->player.name, d->host);
						do_connect(temp_str, GET_LEVEL(d->character), d->character);
						temp_d.descriptor = d->descriptor;
						d->descriptor = k->descriptor;
						k->descriptor = temp_d.descriptor;
						SEND_TO_Q(echo_on, d);
						SEND_TO_Q(echo_on, k);
						SEND_TO_Q("Reconnecting.\r\n", k);
						act("$n has revived a new spirit!", TRUE, k->character, 0, 0, TO_ROOM);
						co2500_close_single_socket(d);
						CHECK_BOOTTIME_HIGH(tmp_ch2, lv_count);
						return;
					}
				}
			}

			for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next)
				if (str_cmp(GET_NAME(d->character), GET_REAL_NAME(tmp_ch)) == 0 &&
				    (tmp_ch->desc == 0) && IS_PC(tmp_ch)) {
					SEND_TO_Q("Resuming.\n\r", d);
					db7100_free_char(d->character, 8);
					tmp_ch->desc = d;
					d->character = tmp_ch;
					tmp_ch->specials.timer = 0;
					STATE(d) = CON_PLAYING;
					act("$n has resumed.", TRUE, tmp_ch, 0, 0, TO_ROOM);
					sprintf(buf, "%s@%s has resumed.",
					GET_REAL_NAME(d->character), d->host);
					do_connect(buf, GET_LEVEL(d->character), d->character);
					CHECK_BOOTTIME_HIGH(tmp_ch2, lv_count);
					return;
				}

/* Check if already playing but switched into a char */
			for (k = descriptor_list; k; k = k->next) {
				if ((k->character != d->character) && k->character) {
					if (GET_NAME(k->character) && GET_NAME(d->character) &&
					    (str_cmp(GET_NAME(k->character), GET_NAME(d->character)) == 0)) {
						bzero(temp_str, sizeof(temp_str));
						sprintf(temp_str, "Loosing connection %s@%s and reconnecting to %s@%s",
							k->character->player.name, k->host, d->character->player.name, d->host);
						do_connect(temp_str, GET_LEVEL(d->character), d->character);

						temp_d.descriptor = d->descriptor;
						d->descriptor = k->descriptor;
						k->descriptor = temp_d.descriptor;
						SEND_TO_Q(echo_on, d);
						SEND_TO_Q(echo_on, k);
						SEND_TO_Q("Reconnecting.\r\n", k);
						act("$n has revived a new spirit!", TRUE, k->character, 0, 0, TO_ROOM);
						co2500_close_single_socket(d);
						CHECK_BOOTTIME_HIGH(tmp_ch2, lv_count);
						return;
					}
				}
			}

			CAP(GET_NAME(d->character));
			sprintf(buf, "%s@%s has connected.", GET_NAME(d->character), d->host);
			//do_connect(buf, MINV(IMO_SPIRIT, GET_LEVEL(d->character)), d->character);
			do_connect_clan(buf, GET_LEVEL(d->character), d->character);
			CHECK_BOOTTIME_HIGH(tmp_ch2, lv_count);

			SEND_TO_Q(motd, d);
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
		}
		break;

	case CON_GET_PWD_NEW_PLAYER:	/* get pwd for new player  */
/* skip whitespaces */
		for (; isspace(*arg); arg++);

		if (!(*arg)) {
			/* FREE UP DEFINED MEMORY */
			free(d->character);
			ha9900_sanity_check(0, "FREE89", "SYSTEM");
			d->character = 0;
			SEND_TO_Q("Name: ", d);
			STATE(d) = CON_GET_NAME;
			return;
		}

/* DID WE USE OUR NAME? */
		if (strlen(arg) == strlen(GET_NAME(d->character)) &&
		    is_abbrev(arg, GET_NAME(d->character))) {
			SEND_TO_Q("You can't use your name for a password.\r\n", d);
			SEND_TO_Q("Password: ", d);
			return;
		}

		if (!*arg || strlen(arg) > 10) {
			SEND_TO_Q("Illegal password.\n\r", d);
			SEND_TO_Q("Password: ", d);
			return;
		}

		strncpy(d->pwd, (char *) crypt(arg, arg), 10);
		*(d->pwd + 10) = '\0';

		SEND_TO_Q("\r\nPlease retype password: ", d);

		STATE(d) = CON_GET_PWD_NEW_PLAYER_CONFIRM;
		break;

/* get confirmation of new pwd */
	case CON_GET_PWD_NEW_PLAYER_CONFIRM:
		/* skip whitespaces */
		for (; isspace(*arg); arg++);

		if (strncmp((char *) crypt(arg, arg), d->pwd, 10)) {
			SEND_TO_Q("Passwords don't match.\r\n", d);
			SEND_TO_Q("Retype password: ", d);
			STATE(d) = CON_GET_PWD_NEW_PLAYER;
			return;
		}

		SEND_TO_Q(echo_on, d);

		SEND_TO_Q("What is your sex (M/F) ? ", d);
		STATE(d) = CON_GET_SEX;
		break;

	case CON_GET_SEX:	/* query sex of new user   */
		/* skip whitespaces */
		for (; isspace(*arg); arg++);
		if (!(*arg)) {
			SEND_TO_Q("Password: ", d);
			STATE(d) = CON_GET_PWD_NEW_PLAYER;
			return;
		}

		switch (*arg) {
		case 'm':
		case 'M':
			/* sex MALE */
			d->character->player.sex = SEX_MALE;
			break;

		case 'f':
		case 'F':
			/* sex FEMALE */
			d->character->player.sex = SEX_FEMALE;
			break;

		default:
			SEND_TO_Q("That's not a sex..\n\r", d);
			SEND_TO_Q("What IS your sex (M/F)? :", d);
			return;
			break;
		}

		in1000_do_show_races(d->character, "", CMD_RACES);
		SEND_TO_Q("Aligned chars must maintain their alignment or else!  Select race (help): ", d);
		STATE(d) = CON_GET_RACE;
		break;

	case CON_GET_RACE:{
			/* skip whitespaces */
			for (; isspace(*arg); arg++);

			if (!(*arg)) {
				SEND_TO_Q("What IS your sex (M/F)? :", d);
				STATE(d) = CON_GET_SEX;
				return;
			}

			one_argument(arg, buf);

			if (is_abbrev(buf, "help")) {
				/* strip out help */
				arg = (char *) one_argument(arg, buf);
				for (; isspace(*arg); arg++);

				/* strip out race */
				arg = (char *) one_argument(arg, buf);
				for (; isspace(*arg); arg++);

				/* if there was a race, take next argument */
				if (is_abbrev(buf, "race")) {
					strcpy(buf, arg);
				}

				in1000_do_show_races(d->character, buf, CMD_HELP);
				send_to_char("\r\nSelect race (help): ", d->character);
				return;
			}

			/* IS VALUE NUMERIC */
			lv_number = 0;
			if (is_number(arg)) {
				lv_number = atoi(arg);
			}

			/* LOOK FOR THE RACE SPECIFIED IN ARGUMENT */
			if (lv_number == 0) {
				for (idx = 1; idx < MAX_RACES; idx++) {
					bzero(buf, sizeof(buf));
					strcpy(buf, races[idx].name);
					for (jdx = 0; jdx < sizeof(buf); jdx++) {
						buf[jdx] = LOWER(buf[jdx]);
					}
					if (is_abbrev(arg, buf)) {
						lv_number = idx;
						idx = MAX_RACES;
					}
				}
			}

			if (lv_number == 0 || lv_number >= MAX_RACES) {
				sprintf(buf, "Sorry, but I don't recognize %s as a race.\r\n", arg);
				send_to_char(buf, d->character);
				send_to_char("\r\nPlease enter race: ", d->character);
				return;
			}

			if (!(strncmp(races[lv_number].name, "undefined", 9))) {
				sprintf(buf, "Sorry, but %s is an undefined race.\r\n", arg);
				send_to_char(buf, d->character);
				send_to_char("\r\nSelect race (help): ", d->character);
				return;
			}

			GET_RACE(d->character) = lv_number;
			STATE(d) = CON_GET_CLASS;
			in1100_do_show_classes(d->character, "", CMD_RACES);
			send_to_char("\r\nSelect class (help): ", d->character);
			return;
		}
		break;

	case CON_GET_CLASS:{
			/* skip whitespaces */
			for (; isspace(*arg); arg++);

			if (!(*arg)) {
				send_to_char("\r\nSelect race (help): ", d->character);
				STATE(d) = CON_GET_RACE;
				return;
			}

			one_argument(arg, buf);
			if (!(*arg)) {
				send_to_char("\r\nSelect class (help): ", d->character);
				return;
			}

			if (is_abbrev(buf, "help")) {
				/* strip out help */
				arg = (char *) one_argument(arg, buf);
				for (; isspace(*arg); arg++);

				/* strip out class */
				arg = (char *) one_argument(arg, buf);
				for (; isspace(*arg); arg++);

				/* if there was a class, take next argument */
				if (is_abbrev(buf, "class")) {
					strcpy(buf, arg);
				}

				in1100_do_show_classes(d->character, buf, CMD_HELP);
				send_to_char("\r\nSelect class (help): ", d->character);
				return;
			}

			lv_number = 0;
			if (is_number(arg)) {
				lv_number = atoi(arg);
			}

			/* LOOK FOR THE CLASS SPECIFIED IN ARGUMENT */
			if (lv_number == 0) {
				for (idx = 1; idx < MAX_CLASSES; idx++) {
					bzero(buf, sizeof(buf));
					strcpy(buf, classes[idx].name);
					for (jdx = 0; jdx < sizeof(buf); jdx++) {
						buf[jdx] = LOWER(buf[jdx]);
					}
					if (is_abbrev(arg, buf)) {
						lv_number = idx;
						idx = MAX_CLASSES;
					}
				}
			}

			if (lv_number == 0 || lv_number >= MAX_CLASSES) {
				sprintf(buf, "Sorry, but I don't recognize %s as a class.\r\n",
					arg);
				send_to_char(buf, d->character);
				return;
			}
			if (lv_number > 17) {
				sprintf(buf, "Sorry, but only classes 1 to 17 are valid.\r\n");
				send_to_char(buf, d->character);
				return;
			}

			if (!(strncmp(classes[lv_number].name, "undefined", 9))) {
				sprintf(buf, "Sorry, but %s is an undefined class.\r\n",
					arg);
				send_to_char(buf, d->character);
				return;
			}

			if (!check_class_race(lv_number, GET_RACE(d->character))) {
				sprintf(buf, "Your race does not allow you to pick class %s.\r\n", arg);
				send_to_char(buf, d->character);
				return;
			}

			GET_CLASS(d->character) = lv_number;
			db7700_init_char(d->character);
			STATE(d) = CON_CONFIRM_DICE_ROLL;
			d->character->points.class1 = lv_number;
			d->character->points.class2 = 0;

			send_to_char("\r\n    At this time, you need to choose attributes which will affect the\r\n", d->character);
			send_to_char("remainder of your life.  These attributes are STRENGTH, INTELLIGENCE,\r\n", d->character);
			send_to_char("WISDOM, DEXTERITY, CONSTITUTION, and CHARISMA.\r\n", d->character);
			send_to_char("    STR affects how much you can carry, damage bonus, and hit bonus.\r\n", d->character);
			send_to_char("INT affects proficiency gained when practicing, chance to cast\r\n", d->character);
			send_to_char("spells successfully, and mana gained when leveling.  WIS affects the\r\n", d->character);
			send_to_char("number of practices and mana gained when you level.  DEX affects\r\n", d->character);
			send_to_char("your armor class bonus and move points gained when leveling.  CON\r\n", d->character);
			send_to_char("is a must for high hitpoints.  It also affects move points gained\r\n", d->character);
			send_to_char("when leveling.  CHA affects how many followers you can have.  You\r\n", d->character);
			send_to_char("can enter HELP STATS later for more detailed information.\r\n", d->character);
			send_to_char("    Choose well and good luck!\r\n", d->character);
			bzero(buf, sizeof(buf));
			sprintf(buf, "Dice rolls: STR: %d  INT: %d  WIS: %d  DEX: %d  CON: %d  CHA: %d\r\n",
				GET_REAL_STR(d->character),
				GET_REAL_INT(d->character),
				GET_REAL_WIS(d->character),
				GET_REAL_DEX(d->character),
				GET_REAL_CON(d->character),
				GET_REAL_CHA(d->character));
			send_to_char(buf, d->character);

			send_to_char("Is this acceptable (Y/N)? ", d->character);
		}
		break;

	case CON_CONFIRM_DICE_ROLL:{

			/* skip whitespaces */
			for (; isspace(*arg); arg++);

			if (!(*arg)) {
				send_to_char("Select class (help): ", d->character);
				STATE(d) = CON_GET_CLASS;
				return;
			}

			if (*arg == 'y' || *arg == 'Y') {
				/* create an entry in the file */
				d->pos = db6300_create_entry(GET_NAME(d->character));
				d->character->nr = d->pos;
				r1100_rent_delete_file(GET_NAME(d->character));
				db6400_save_char(d->character, NOWHERE);
				SEND_TO_Q(motd, d);
				SEND_TO_Q("\n\r\n*** PRESS RETURN: ***", d);
				STATE(d) = CON_GET_RETURN_KEY_MOTD;

				sprintf(buf, "%s@%s new player.", GET_NAME(d->character),
					d->host);
				do_connect(buf, GET_LEVEL(d->character), d->character);
				spec_log(buf, NEW_PLAYER_LOG);
				return;
			}

			db7750_roll_stat_dice(d->character, 1);
			bzero(buf, sizeof(buf));
			sprintf(buf, "Dice rolls: STR: %d  INT: %d  WIS: %d  DEX: %d  CON: %d  CHA: %d\r\n",
				GET_REAL_STR(d->character),
				GET_REAL_INT(d->character),
				GET_REAL_WIS(d->character),
				GET_REAL_DEX(d->character),
				GET_REAL_CON(d->character),
				GET_REAL_CHA(d->character));
			send_to_char(buf, d->character);
			send_to_char("Is this acceptable (Y/N)? ", d->character);
			return;

		} break;

	case CON_GET_RETURN_KEY_MOTD:	/* read CR after printing motd   */
		sprintf(colorbuf, CHOOSE_MENU(d->character));
		SEND_TO_Q(colorbuf, d);
		STATE(d) = CON_GET_MENU_SELECTION;
		break;

	case CON_GET_MENU_SELECTION:	/* get selection from main menu  */
		SEND_TO_Q(echo_on, d);
		/* skip whitespaces */
		for (; isspace(*arg); arg++);
		switch (*arg) {
		case '0':
			co2500_close_single_socket(d);
			break;

		case '1':
			pa8000_enter_game(d);
			break;

		case '2':
			STATE(d) = CON_ADMIN;
			ADMIN_STATE(d) = ADMIN_CONFIG;
			ad1500_show_menu(d);
			OLD_STATE(d) = 0;
			break;

		case '3':
			SEND_TO_Q("Enter a text you'd like others to see when they look at you.\n\r", d);
			SEND_TO_Q("Terminate with a '@'.\n\r", d);
			if (d->character->player.description) {
				SEND_TO_Q("Old description :\n\r", d);
				write_to_output(d, 0, "%s", d->character->player.description);
				free(d->character->player.description);
				ha9900_sanity_check(0, "FREE90", "SYSTEM");
				d->character->player.description = 0;
			}
			d->str =
				&d->character->player.description;
			d->max_str = MAX_DESC;
			STATE(d) = CON_GET_DESCRIPTION;
			break;

		case '4':
			SEND_TO_Q("Enter a new password: ", d);
			SEND_TO_Q(echo_off, d);
			STATE(d) = CON_GET_PWD_CHANGE;
			break;
		case '5':
			/* IF JAILED, THEY ARE PROBABLY TO ANGRY TO DO THIS */
			if (IS_SET(GET_ACT2(d->character), PLR2_JAILED)) {
				SEND_TO_Q("Sorry, you can't do this from jail.\n\r", d);
				sprintf(colorbuf, CHOOSE_MENU(d->character));
				SEND_TO_Q(colorbuf, d);
				bzero(buf, sizeof(buf));
				sprintf(buf, "INFO: %s tried to change PKILLABLE flag while jailed!",
					GET_NAME(d->character));
				do_wizinfo(buf, IMO_IMM, d->character);
				spec_log(buf, SYSTEM_LOG);
				break;
			}	/* end */
			else {
				if (GET_LEVEL(d->character) > 41) {
					SEND_TO_Q("You are a hardcore PKILLER!\r\n", d);
					SEND_TO_Q("Player Killing is the only way you know!\r\n", d);
					sprintf(colorbuf, CHOOSE_MENU(d->character));
					SEND_TO_Q(colorbuf, d);
					STATE(d) = CON_GET_MENU_SELECTION;
					return;
				}
				if (IS_SET(GET_ACT2(d->character), PLR2_PKILLABLE)) {
					if (d->character->player.time.lastpkill < 1) {
						SEND_TO_Q("It seems you have been good, and not killed anyone,\r\n", d);
						SEND_TO_Q("so I think we can remove your PKILLER status.\r\n", d);
						REMOVE_BIT(GET_ACT2(d->character), PLR2_PKILLABLE);
						sprintf(colorbuf, CHOOSE_MENU(d->character));
						SEND_TO_Q(colorbuf, d);
						STATE(d) = CON_GET_MENU_SELECTION;
						return;
					}
					pkilltime = (((time(0) - d->character->player.time.logon) +
					   d->character->player.time.played) -
					 d->character->player.time.lastpkill);
					if (pkilltime < MINIMUM_PKILLER_TIME) {
						pkilltime = (MINIMUM_PKILLER_TIME - pkilltime) / 60;
						SEND_TO_Q("I hardly think so!  You are a wanted murderer, and\r\n", d);
						if (pkilltime > 60) {
							sprintf(buf, "will remain so for at least another %d hours!\r\n", pkilltime / 60);
						}
						else if (pkilltime == 60) {
							sprintf(buf, "will remain so for at least another hour!\r\n");
						}
						else if (pkilltime < 1) {
							sprintf(buf, "will remain so for just under a minute!\r\n");
						}
						else if (pkilltime == 1) {
							sprintf(buf, "will remain so for another minute!\r\n");
						}
						else
							sprintf(buf, "will remain so for another %d minutes!\r\n", pkilltime);
						SEND_TO_Q(buf, d);
						sprintf(colorbuf, CHOOSE_MENU(d->character));
						SEND_TO_Q(colorbuf, d);
						STATE(d) = CON_GET_MENU_SELECTION;
						return;
					}
					else {
						SEND_TO_Q("You're a murderer alright, but you seem to have gotten\r\n", d);
						SEND_TO_Q("away with it. PKILLER status removed.\r\n", d);
						REMOVE_BIT(GET_ACT2(d->character), PLR2_PKILLABLE);
						sprintf(colorbuf, CHOOSE_MENU(d->character));
						SEND_TO_Q(colorbuf, d);
						STATE(d) = CON_GET_MENU_SELECTION;
						return;
					}
				}
				SEND_TO_Q("If you set this flag, and attack someone, you will not\r\n", d);
				SEND_TO_Q("be able to clear it until you have played for a further\r\n", d);
				sprintf(buf, "%d hours.\r\n", (MINIMUM_PKILLER_TIME / 3600));
				SEND_TO_Q(buf, d);
				SEND_TO_Q("Are you sure you want to set the flag? (Y/N) ", d);
				STATE(d) = CON_PKILLABLE_CONFIRM;
				break;
			}
		case '6':
			/* IF JAILED, SCREW THEM */
			if (IS_SET(GET_ACT2(d->character), PLR2_JAILED)) {
				SEND_TO_Q("Sorry, you can't delete yourself from jail.\n\r", d);
				sprintf(colorbuf, CHOOSE_MENU(d->character));
				SEND_TO_Q(colorbuf, d);
				bzero(buf, sizeof(buf));
				sprintf(buf, "WARNING: %s tried to delete their char while jailed!",
					GET_NAME(d->character));
				do_wizinfo(buf, IMO_IMM, d->character);
				spec_log(buf, SYSTEM_LOG);
				break;
			}
			else if (GET_LEVEL(d->character) > 20) {
				SEND_TO_Q("Sorry, chars over level 20 must mud mail a IMM to be deleted.\n\r", d);
				sprintf(colorbuf, CHOOSE_MENU(d->character));
				SEND_TO_Q(colorbuf, d);
				break;
			}	/* end */
			else {
				SEND_TO_Q("Are you sure you want to do this? (Y/N) ", d);
				STATE(d) = CON_DELETE_CONFIRM;
				break;
			}
		default:
			SEND_TO_Q("Wrong option.\n\r", d);
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			break;
		}
		break;
	case CON_DELETE_CONFIRM:
		/* skip whitespaces */
		for (; isspace(*arg); arg++);
		if (!(*arg)) {
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
			return;
		}

		switch (*arg) {
		case 'n':
		case 'N':
			SEND_TO_Q("Deletion cancelled.", d);
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
			return;
		case 'y':
		case 'Y':
			SEND_TO_Q("Character deleted.", d);
			db6550_self_delete(d->character, d->character->player.name);
			co2500_close_single_socket(d);
			break;
		default:
			SEND_TO_Q("Invalid Choice. Please press y or n ", d);
			return;
		}
		break;

	case CON_PKILLABLE_CONFIRM:
		/* skip whitespaces */
		for (; isspace(*arg); arg++);
		if (!(*arg)) {
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
			return;
		}

		switch (*arg) {
		case 'n':
		case 'N':
			SEND_TO_Q("PKILLABLE setting cancelled.", d);
			sprintf(colorbuf, MENU);
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
			return;
		case 'y':
		case 'Y':
			SET_BIT(GET_ACT2(d->character), PLR2_PKILLABLE);
			SEND_TO_Q("PKILLABLE flag set.", d);
			d->character->player.time.lastpkill = PLAYED_TIME(d->character);
			sprintf(colorbuf, CHOOSE_MENU(d->character));
			SEND_TO_Q(colorbuf, d);
			STATE(d) = CON_GET_MENU_SELECTION;
			return;
		default:
			SEND_TO_Q("Invalid Choice. Please press y or n ", d);
			return;
		}
		break;

	case CON_GET_PWD_CHANGE:

		/* skip whitespaces */
		for (; isspace(*arg); arg++);

		/* DID WE USE OUR NAME? */
		if (strlen(arg) == strlen(GET_NAME(d->character)) &&
		    is_abbrev(arg, GET_NAME(d->character))) {
			SEND_TO_Q("You can't use your name for a password.\r\n", d);
			SEND_TO_Q("Password: ", d);
			return;
		}

		if (!*arg || strlen(arg) > 10) {
			SEND_TO_Q("Illegal password.\n\r", d);
			SEND_TO_Q("Password: ", d);
			return;
		}

		strncpy(d->pwd, (char *) crypt(arg, arg), 10);
		*(d->pwd + 10) = '\0';

		SEND_TO_Q("Please retype password: ", d);

		STATE(d) = CON_GET_PWD_OLD_PLAYER_CONFIRM;
		break;
	case CON_GET_PWD_OLD_PLAYER_CONFIRM:
		/* skip whitespaces */
		for (; isspace(*arg); arg++);

		if (strncmp((char *) crypt(arg, arg), d->pwd, 10)) {
			SEND_TO_Q("Passwords don't match.\n\r", d);
			SEND_TO_Q("Retype password: ", d);
			STATE(d) = CON_GET_PWD_CHANGE;
			return;
		}
		SEND_TO_Q(echo_on, d);
		SEND_TO_Q("\n\rDone. You must enter the game to make the change final\n\r", d);
		sprintf(colorbuf, CHOOSE_MENU(d->character));
		SEND_TO_Q(colorbuf, d);
		STATE(d) = CON_GET_MENU_SELECTION;
		break;

	case CON_QUIT:		/* wait for conf. of quit */
		/* skip whitespaces */
		for (; isspace(*arg); arg++);

		if (*arg == 'y' || *arg == 'Y') {
			SEND_TO_Q("Quitting.\r\n", d);

			act("Goodbye, friend.. Come back soon!", FALSE, d->character, 0, 0, TO_CHAR);
			act("$n has left the game.", TRUE, d->character, 0, 0, TO_ROOM);
			bzero(temp_str, sizeof(temp_str));
			sprintf(temp_str, "Player: %s has quit.", GET_NAME(d->character));
			do_connect(temp_str, GET_LEVEL(d->character), d->character);

			/* SEE IF PLAYER HAD EQUIPMENT */
			lv_player_has_equip = FALSE;
			for (idx = 0; idx < MAX_WEAR; idx++) {
				if (d->character->equipment[idx]) {
					lv_player_has_equip = TRUE;
				}
			}

			if (lv_player_has_equip == TRUE ||
			    (d->character->carrying)) {
				send_to_char("Your equipment is in a big pile on the ground where you quit.\r\n", d->character);
			}

			GET_HOW_LEFT(d->character) = LEFT_BY_COMMAND_QUIT;
			if (gv_mega_verbose_messages == TRUE) {
				main_log("Leaving by quit mortal");
			}
			ha3000_extract_char(d->character, END_EXTRACT_BY_MENU_QUIT);
			r2300_del_char_objs(d->character);
		}
		else {
			if (*arg == 'n' || *arg == 'N') {
				STATE(d) = CON_PLAYING;
			}
			else {
				SEND_TO_Q("Please type Yes or No? ", d);
			}
		}
		break;
	case CON_ADMIN:{
			ad1100_admin_nanny(d, arg);
			return;
		}
	default:
		perror("ERROR: Fatal error in interpreter.");
		sprintf(buf, "ERROR: Nanny: illegal stat of CON'ness");
		ABORT_PROGRAM();
		break;
	}
}

void do_welcome_back(struct char_data * ch)
{
	char buf[MAX_STRING_LENGTH];
	int newmail;

	bzero(buf, sizeof(buf));
	strcpy(buf, do_how_left_text(ch, GET_HOW_LEFT(ch)));

	if (*buf) {
		send_to_char(buf, ch);
		main_log(buf);
		spec_log(buf, CONNECT_LOG);
	}

	send_to_char("You last logged off: ", ch);
	send_to_char((char *) asctime(localtime(&ch->player.logoff_time)), ch);
	send_to_char((char *) WELC_MESSG, ch);
	sprintf(buf, "%s entering Crimson II.", GET_NAME(ch));
	do_connect(buf, GET_LEVEL(ch), ch);

	/* SAVE HOW PLAYER LAST LEFT */
	GET_HOW_SAVE(ch) = GET_HOW_LEFT(ch);
	/* DEFAULT TO GAME GRASH */
	GET_HOW_LEFT(ch) = LEFT_BY_CRASH;
	GET_LOGOFF_TIME(ch) = time(0);

	/* Notify player of new mail */
	if ((newmail = new_message(ch)) > 0)
		co2900_send_to_char(ch, "&YYou have %d new message%s!\r\n",
				    newmail, (newmail > 1) ? "s" : "");

	return;

}				/* END OF do_welcome_back() */


char *do_how_left_text(struct char_data * ch, int lv_how_left)
{
	static char buf[MAX_STRING_LENGTH];

	bzero(buf, sizeof(buf));
	strcpy(buf, "You last left the game by ");
	switch (lv_how_left) {
	case LEFT_BY_CRASH:{
			strcat(buf, "game crash.");
			break;
		}
	case LEFT_BY_RENT_WITH_ITEMS:{
				/* old rent, now quit */
			strcat(buf, "quit (with items).");
			break;
		}
	case LEFT_BY_COMMAND_QUIT:{
			strcat(buf, "quit.");
			break;
		}
	case LEFT_BY_PURGE:{
			strcat(buf, "purge.");
			break;
		}
	case LEFT_BY_LINK_NORENT:{
				/* not used i think */
			strcat(buf, "link death - no rent.");
			break;
		}
	case LEFT_BY_LINK_RENT:{
			strcat(buf, "link death - rent.");
			break;
		}
	case LEFT_BY_LINK_NO_$4_RENT:{
				/* not used i think */
			strcat(buf, "link death - couldn't afford rent.");
			break;
		}
	case LEFT_BY_DEATH_ZTELEPORT:{
			strcat(buf, "zone teleporting into a deathtrap!");
			break;
		}
	case LEFT_BY_DEATH_ROOM:{
			strcat(buf, "entering a death room!");
			break;
		}
	case LEFT_BY_DEATH_AGE:{
			strcat(buf, "aging to much!");
			break;
		}
	case LEFT_BY_DEATH_FIGHT:{
			strcat(buf, "a bloody and horrible death!");
			break;
		}
	case LEFT_BY_LINK_DEATH:{
			strcat(buf, "losing connection!");
			break;
		}
	case LEFT_BY_SHUTDOWN:{
			strcat(buf, "shutdown!");
			break;
		}
	case LEFT_BY_RENT_NO_ITEMS:{
			strcat(buf, "rent (no items).");
			break;
		}
	case LEFT_BY_LIGHTNING_DEATH:{
			strcat(buf, "being killed by lightning.");
			break;
		}
	case LEFT_BY_DEATH_TELEPORT:{
			strcat(buf, "teleporting into a deathtrap!");
			break;
		}
	case LEFT_BY_DEATH_GERINS:{
			strcat(buf, "using the wrong items in a special!");
			break;
		}
	case LEFT_BY_DEATH_VEHICLE:{
			strcat(buf, "using your body as pavement!");
			break;
		}
	default:{
			bzero(buf, sizeof(buf));
			break;
		}
	}

	if (*buf) {
		strcat(buf, "\r\n");
	}
	bzero(gv_str, sizeof(gv_str));
	strcpy(gv_str, buf);
	return (gv_str);

}				/* END OF do_how_left_text() */

void pa8000_enter_game(struct descriptor_data * d)
{
	/* player is logging in, so update time */
	player_table[d->character->nr].pidx_last_on = time(0);

	db7400_reset_char(d->character);
	r2400_load_char_objs(d->character, FALSE);
	do_welcome_back(d->character);
	d->character->next = character_list;
	character_list = d->character;
	if (IS_SET(GET_ACT2(d->character), PLR2_JAILED)) {
		d->character->in_room = db8000_real_room(JAIL_ROOM);
	}

	if (d->character->in_room == NOWHERE) {
		if (GET_LEVEL(d->character) > PK_LEV)
			ha1600_char_to_room(d->character, db8000_real_room(4));
		else if (GET_LEVEL(d->character) <= 4)
			ha1600_char_to_room(d->character, db8000_real_room(ROYAL_ACADEMY));
		else
			ha1600_char_to_room(d->character, db8000_real_room(DRESSING_ROOM));
	}
	else {
		if (d->character->in_room > -1 &&
		    d->character->in_room <= top_of_world)
			ha1600_char_to_room(d->character, d->character->in_room);
		else
			ha1600_char_to_room(d->character, db8000_real_room(DRESSING_ROOM));
	}

	act("$n has entered the game.", TRUE, d->character, 0, 0, TO_ROOM);
	STATE(d) = CON_PLAYING;
	if (!GET_LEVEL(d->character))
		wi3000_create_level_one_char(d->character, BIT0);
	in3000_do_look(d->character, "", 0);
	d->prompt_mode = 1;


	return;
}				/* END OF pa8000_enter_game() */
