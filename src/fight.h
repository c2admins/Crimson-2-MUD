/* fight.h */

static struct dam_weapon_type {
	char *to_room;
	char *to_char;
	char *to_victim;
} dam_weapons[] = {
	
	{"$n misses $N with $s #W.",                           /*  0 */
		"You miss $N with your #W.",
		"$n misses you with $s #W." },
	
	{"$n tickles $N with $s #W.",                          /*  1  */
		"You tickle $N as you #W $M.",
		"$n tickles you as $e #P you." },
	
	{"$n barely #P $N.",                                   /*  2  */
		"You barely #W $N.",
		"$n barely #P you."},
	
	{"$n #P $N.",                                          /*  3  */
		"You #W $N.",
		"$n #P you."}, 
	
	{"$n #P $N hard.",                                     /*  4  */
		"You #W $N hard.",
		"$n #P you hard."},
	
	{"$n #P $N very hard.",                                /*  5 */
		"You #W $N very hard.",
		"$n #P you very hard."},
	
	{"$n #P $N extremely hard.",                           /*  6 */
		"You #W $N extremely hard.",
		"$n #P you extremely hard."},
	
	{"$n massacres $N with $s #W.",                        /*  7 */
		"You massacre $N with your #W.", 
		"$n massacres you with $s #W."},
	
	{"$n annihilates $N with $s deadly #W.",               /*  8 */
		"You annihilate $N with your deadly #W.",
		"$n annihilates you with $s deadly #W."},
	
	{"$n obliterates $N with $s deadly #W.",               /*  9 */
		"You obliterate $N with your deadly #W.",
		"$n obliterates you with $s deadly #W."},
	
	{"$n demolishes $N with $s deadly #W.",                /* 10 */
		"You demolish $N with your deadly #W.",
		"$n demolishes you with $s deadly #W."},
	
	{"$n mangles $N with $s deadly #W.",
		"You mangle $N with your deadly #W.",                 /* 11 */
		"$n mangles you with $s deadly #W."},
	
	{"$n brutalizes $N with $s deadly #W.",
		"You brutalize $N with your deadly #W.",              /* 12 */
		"$n brutalizes you with $s deadly #W."},
	
	{"$n rends $N asunder with $s deadly #W.",
		"You rend $N asunder with your deadly #W.",           /* 13 */
		"$n rends you asunder with $s deadly #W."},
	
	{"$n liquifies $N with $s deadly #W.",
		"You liquify $N with your deadly #W.",                /* 14 */
		"$n liquifies you with $s deadly #W."},
	
	{"$n decimates $N with $s deadly #W.",
		"You decimate $N with your deadly #W.",              /* 15 */
		"$n decimates you with $s deadly #W."},
	
	{"$n mortalizes $N with $s deadly #W.",
		"You mortalize $N with your deadly #W.",             /* 16 */
		"$n mortalizes you with $s deadly #W."},
	
	{"$n decapitates $N with $s deadly #W.",
		"You decapitate $N with your deadly #W.",           /* 17 */
		"$n decapitates you with $s deadly #W."},
	
	{"$n eviscerates $N with $s deadly #W.",
		"You eviscerate $N with your deadly #W.",            /* 18 */
		"$n eviscerates you with $s deadly #W."},
	
	{"$n shatters $N with $s deadly #W.",
		"You shatter $N with your deadly #W.",               /* 19 */
		"$n shatters you with $s deadly #W."},
	
	{"$n negates $N with $s deadly #W.",
		"You negate $N with your deadly #W.",                /* 20 */
		"$n negates you with $s deadly #W."},
	
	{"$n shish kabobs $N with $s deadly #W.",
		"You shish kabob $N with your deadly #W.",           /* 21 */
		"$n shish kabobs you with $s deadly #W."},
	
	{"$n pulverizes $N with $s deadly #W.",
		"You pulverize $N with your deadly #W.",             /* 22 */
		"$n pulverizes you with $s deadly #W."},
	
	{"$n ravages $N with $s deadly #W.",
		"You ravage $N with your deadly #W.",                /* 23 */
		"$n ravages you with $s deadly #W."},
	
	{"$n mutilates $N with $s deadly #W.",
		"You mutilate $N with your deadly #W.",              /* 24 */
		"$n mutilates you with $s deadly #W."},
	
	{"$n disembowels $N with $s deadly #W.",
		"You disembowel $N with your deadly #W.",            /* 25 */
		"$n disembowels you with $s deadly #W."},
	
	{"$n slaughters $N with $s deadly #W.",
		"You slaughter $N with your deadly #W.",             /* 26 */
		"$n slaughters you with $s deadly #W."},
	
	{"$n dismembers $N with $s deadly #W.",
		"You dismember $N with your deadly #W.",             /* 27 */
		"$n dismembers you with $s deadly #W."},
	
	{"$n splatters $N with $s deadly #W.",
		"You splatter $N with your deadly #W.",              /* 28 */
		"$n splatters you with $s deadly #W."},
	
	{"$n eradicates $N with $s deadly #W.",
		"You eradicate $N with your deadly #W.",            /* 29 */
		"$n eradicates you with $s deadly #W."},
	
	{"$n butchers $N with $s deadly #W.",
		"You butcher $N with your deadly #W.",              /* 30 */
		"$n butchers you with $s deadly #W."},
	
	{"$n ruptures $N with $s deadly #W.",
		"You rupture $N with your deadly #W.",              /* 31 */
		"$n rupturs you with $s deadly #W."},
	
	{"$n exterminates $N with $s deadly #W.",
		"You exterminate $N with your deadly #W.",          /* 32 */
		"$n exterminates you with $s deadly #W."},
	
	{"$n destroys $N with $s deadly #W.",
		"You destroy $N with your deadly #W.",              /* 33 */
		"$n destroys you with $s deadly #W."},
	
	{"$n atomizes $N with $s deadly #W.",
		"You atomize $N with your deadly #W.",              /* 34 */
		"$n atomizes you with $s deadly #W."},
	/* JUST GUESS HOW MUCH */
};

static struct mult_dam_weapon_type {
	char *to_room;
	char *to_char;
	char *to_victim;
} mult_dam_weapons[] = {
	
	{"$n misses $N with all #H of $s #W.",                 /*  0 */
		"You miss $N with all #H of your #W.",
		"$n misses you with all #H $s #W." },
	
	{"$n tickles $N with #H quick #W.",                    /*  1  */
		"You tickle $N with #H quick #W.",
		"$n tickles you with #H quick #W." },
	
	{"$n barely #W $N.",                                   /*  2  */
		"You barely #W $N.",
		"$n barely #W you."},
	
	{"$n hits $N with #H quick #W.",                     /*  3  */
		"You hit $N with #H quick #W.",
		"$n hits you with #H quick #W."},
	
	{"$n hits $N with #H hard #W.",                      /*  4  */
		"You hit $N with #H hard #W.",
		"$n hits you with #H hard #W."},
	
	{"$n hits $N with #H very hard #W.",                 /*  5 */
		"You hit $N with #H very hard #W.",
		"$n hits you with #H very hard #W."},
	
	{"$n hits $N with #H extremely hard #W.",            /*  6 */
		"You hit $N with #H extremely hard #W.",
		"$n hits you with #H extremely hard #W."},
	
	{"$n massacres $N with a flurry of #H #W.",          /*  7 */
		"You massacre $N with a flurry of #H #W.",
		"$n massacres you with a flurry of #H #W."},
	
	{"$n annihilates $N with #H deadly #W.",             /*  8 */
		"You annihilate $N with #H deadly #W.",
		"$n annihilates you with #H deadly #W."},
	
	{"$n obliterates $N with #H deadly #W.",             /*  9 */
		"You obliterate $N with your #H deadly #W.",
		"$n obliterates you with #H deadly #W."},
	
	{"$n demolishes $N with $s #H deadly #W.",           /* 10 */
		"You demolish $N with your #H deadly #W.",
		"$n demolishes you with $s #H deadly #W."},
	
	{"$n mangles $N with $s #H deadly #W.",
		"You mangle $N with your #H deadly #W.",           /* 11 */
		"$n mangles you with $s #H deadly #W."},
	
	{"$n brutalizes $N with $s #H deadly #W.",
		"You brutalize $N with your #H deadly #W.",           /* 12 */
		"$n brutalizes you with $s #H deadly #W's."},
	
	{"$n rends $N asunder with $s #H deadly #W.",
		"You rend $N asunder with your #H deadly #W.",      /* 13 */
		"$n rends you asunder with $s #H deadly #W."},
	
	{"$n liquifies $N with $s #H deadly #W.",
		"You liquify $N with your #H deadly #W.",             /* 14 */
		"$n liquifies you with $s #H deadly #W."},
	
	{"$n decimates $N with $s #H deadly #W.",
		"You decimate $N with your #H deadly #W.",            /* 15 */
		"$n decimates you with $s #H deadly #W."},
	
	{"$n mortalizes $N with $s #H deadly #W.",
		"You mortalize $N with your #H deadly #W.",           /* 16 */
		"$n mortalizes you with $s #H deadly #W."},
	
	{"$n decapitates $N with $s #H deadly #W.",
		"You decapitate $N with your #H deadly #W.",         /* 17 */
		"$n decapitates you with $s #H deadly #W."},
	
	{"$n eviscerates $N with $s #H deadly #W.",
		"You eviscerate $N with your #H deadly #W.",          /* 18 */
		"$n eviscerates you with $s #H deadly #W."},
	
	{"$n shatters $N with $s #H deadly #W.",
		"You shatter $N with your #H deadly #W.",             /* 19 */
		"$n shatters you with $s #H deadly #W."},
	
	{"$n negates $N with $s #H deadly #W.",
		"You negate $N with your #H deadly #W.",              /* 20 */
		"$n negates you with $s #H deadly #W."},
	
	{"$n shish kabobs $N with $s #H deadly #W.",
		"You shish kabob $N with your #H deadly #W.",         /* 21 */
		"$n shish kabobs you with $s #H deadly #W."},
	
	{"$n pulverizes $N with $s #H deadly #W.",
		"You pulverize $N with your #H deadly #W.",           /* 22 */
		"$n pulverizes you with $s #H deadly #W."},
	
	{"$n ravages $N with $s #H deadly #W.",
		"You ravage $N with your #H deadly #W.",           /* 23 */
		"$n ravages you with $s #H deadly #W."},
	
	{"$n mutilates $N with $s #H deadly #W.",
		"You mutilate $N with your #H deadly #W.",           /* 24 */
		"$n mutilates you with $s #H deadly #W."},
	
	{"$n disembowels $N with $s #H deadly #W.",
		"You disembowel $N with your #H deadly #W.",           /* 25 */
		"$n disembowels you with $s #H deadly #W."},
	
	{"$n slaughters $N with $s #H deadly #W.",
		"You slaughter $N with your #H deadly #W.",           /* 26 */
		"$n slaughters you with $s #H deadly #W."},
	
	{"$n dismembers $N with $s #H deadly #W.",
		"You dismember $N with your #H deadly #W.",           /* 27 */
		"$n dismembers you with $s #H deadly #W."},
	
	{"$n splatters $N with $s #H deadly #W.",
		"You splatter $N with your #H deadly #W.",           /* 28 */
		"$n splatters you with $s #H deadly #W."},
	
	{"$n eradicates $N with $s #H deadly #W.",
		"You eradicate $N with your #H deadly #W.",           /* 29 */
		"$n eradicates you with $s #H deadly #W."},
	
	{"$n butchers $N with $s #H deadly #W.",
		"You butcher $N with your #H deadly #W.",           /* 30 */
		"$n butchers you with $s #H deadly #W."},
	
	{"$n ruptures $N with $s #H deadly #W.",
		"You rupture $N with your #H deadly #W.",           /* 31 */
		"$n ruptures you with $s #H deadly #W."},
	
	{"$n exterminates $N with $s #H deadly #W.",
		"You exterminate $N with your #H deadly #W.",           /* 32 */
		"$n exterminates you with $s #H deadly #W."},
	
	{"$n destroys $N with $s #H deadly #W.",
		"You destroy $N with your #H deadly #W.",           /* 33 */
		"$n destroys you with $s #H deadly #W."},
	
	{"$n atomizes $N with $s #H deadly #W.",
		"You atomize $N with your #H deadly #W.",           /* 34 */
		"$n atomizes you with $s #H deadly #W."},
	/* JUST GUESS HOW MUCH */
};


