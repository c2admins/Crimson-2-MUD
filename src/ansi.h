/* the codes below are used for highlighting text, and is set for the */
/* codes for VT-100 terminal emulation. If you are using a different  */
/* teminal type, replace the codes below with the correct codes       */
/******************************************************************** */
#define BLACK	"[0;30m" /* darker shades */
#define RED	"[0;31m" /* test */
#define GREEN	"[0;32m" /* test */
#define BROWN	"[0;33m" /* test */
#define BLUE	"[0;34m" /* test */
#define PURPLE	"[0;35m" /* test */
#define CYAN	"[0;36m" /* test */
#define DGRAY	"[1;30m" /* test */
#define LRED	"[1;31m" /* lighter shades */
#define LGREEN	"[1;32m" /* test */
#define YELLOW	"[1;33m" /* test */
#define LBLUE	"[1;34m" /* test */
#define LPURPLE	"[1;35m" /* test */
#define LCYAN	"[1;36m" /* test */
#define WHITE	"[1;37m" /* test */
#define GRAY	"[0;37m" /* test */
#define BLINK	"[5m" /* test */
#define UNDERL	"[4m" /* test */
#define INVERSE	"[7m" /* test */
#define NEWLINE "\r\n"  /* newline */
#define END	"[0m" 
#define BACK_BLACK	"[40m" 
#define BACK_RED	"[41m" 
#define BACK_GREEN	"[42m" 
#define BACK_BROWN	"[43m" 
#define BACK_BLUE	"[44m" 
#define BACK_PURPLE	"[45m" 
#define BACK_CYAN	"[46m" 
#define BACK_GRAY	"[47m" 
#define ADV_CLEAR	"H[2J"  /* Clear screen */ 
#define ADV_UP  	"[A"   /* Move up one line */
#define ADV_DOWN	"[B"   /* Move down one line */
#define ADV_ENTER	"\r\n"   /* Next line (+ ct) */
#define ADV_ETEOL	"[0K"  /* Erase from cursor to end of line */
#define ADV_ETCP	"[1K"  /* Erase from begin of line to cursor */
#define ADV_START	"["    /* Start code for ANSI codes */

#define OLD_CHAR  "&"
#define OLD_CHAR2 "&&"

#define CLR_COMM    LPURPLE 	/* say, tell, shout etc */
#define CLR_DAM     LRED   	/* ouch I'm hit messages */
#define CLR_FIGHT   RED    	/* other peoples fight messages */
#define CLR_HIT     RED    	/* you hit the target messages */
#define CLR_ACTION  LCYAN	/* color of character actions */
#define CLR_TITLE   CYAN  	/* color of room titles */
#define CLR_DESC    LBLUE   	/* color of extra/room descriptions */
#define CLR_OBJ     GREEN 	/* color of objects */
#define CLR_MOB     GREEN 	/* color of mobs */

#define CLR_MAX      RED
#define CLR_GOD      BLUE
#define CLR_BOSS     YELLOW
#define CLR_IMP      PURPLE
#define CLR_LEV5     GREEN
#define CLR_LEV4     LGREEN
#define CLR_LEGEND   CYAN
#define CLR_LEV3     LCYAN
#define CLR_LEV2     LCYAN
#define CLR_LEV1     BROWN
#define CLR_SPIRIT   GRAY
#define CLR_MEDIATOR LCYAN
#define CLR_OVERSEER LRED
#define CLR_MORTAL   WHITE
#define CLR_NEWBIE   YELLOW

/* the communications channels */
#define CLR_GOSSIP  WHITE      	/* communications channel */
#define CLR_AUCTION YELLOW    	/* communications channel */
#define CLR_INFO    CYAN      	/* communications channel */
#define CLR_SYS     RED      	/* communications channel */
#define CLR_IMMTALK LCYAN     	/* communications channel */
#define CLR_CONNECT YELLOW    	/* communications channel */
#define CLR_WIZINFO PURPLE    	/* communications channel */
#define CLR_AVAT    BROWN     	/* communications channel */
#define CLR_MUSIC   BLUE        /* communications channel */
#define CLR_ANTE    LGREEN    	/* communications channel */
#define CLR_HIGH    GREEN     	/* communications channel */
#define CLR_SAY     LPURPLE   	/* communications channel */
#define CLR_SHOUT   RED       	/* communications channel */
#define CLR_TELL    LCYAN     	/* communications channel */
#define CLR_WHISPER CYAN      	/* communications channel */
#define CLR_ASK     PURPLE    	/* communications channel */
#define CLR_        YELLOW    	/* communications channel */
#define CLR_CLAN_TELL LRED

#define REVERSE	"[7m"  /* test  - so you can more ansi.h to see it */
#define BOLD 	"[1m"  /* test */
#define BLINK 	"[5m"  /* test */
#define ITALIC 	"[3m"  /* test */
#define FAINT 	"[2m"  /* test */
#define NEWLINE "\r\n"  /* newline */
