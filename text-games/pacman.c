/*
 * PACMAN  - written by Dave Nixon, AGS Computers Inc., July, 1981.
 * Converted for curses Feb 1982 by Mark Horton.
 *
 * Terminal handling for video games taken from aliens
 *      the original version  of aliens is from 
 *      Fall 1979                      Cambridge               Jude Miller
 *
 * Score keeping modified and general terminal handling (termcap routines
 * from UCB's ex) added by Rob Coben, BTL, June, 1980.
 *
 * Converted to z88dk by Stefano Bodrato in Sept. 2024
 *
 * z88dk specific build options:
 *     -DUSE_UDGS uses UDGs for the characters
 *     -DSWITCH_MODE=x switches to screenmode x for display
 *     -DHAVE_FILES
 *     -DHAVE_CLOCK different way to init srand(), probably useless
 */



// TODO: shrink or disable extra text to fit in 40 (or 64) columns

// ZX Spectrum, (VT-ANSI terminal type pr generic console)
// zcc +zx -clib=ansi -lndos -DHAVE_CLOCK -lndos -create-app -pragma-define:ansicolumns=80 pacman.c
// zcc +zx -lndos -create-app -DNO_80COL -DHAVE_CLOCK -clib=ansi -pragma-define:ansicolumns=40 pacman.c
// zcc +zx -lndos -create-app -DNO_80COL -DHAVE_CLOCK pacman.c

// NCR Decision Mate V, Kaypro, Sanyo, Otrona Attachè, Televideo..
// zcc +cpm  -create-app -subtype=dmv -DUSE_CHR127 --generic-console pacman.c
// zcc +cpm  -create-app -subtype=kaypro83 -DUSE_CHR127 --generic-console pacman.c
// zcc +cpm  -create-app -subtype=kaypro84 -DUSE_CHR127 --generic-console pacman.c
// zcc +cpm  -create-app -subtype=mbc200 --generic-console pacman.c
// zcc +cpm  -create-app -subtype=attache --generic-console pacman.c
// zcc +cpm  -create-app -subtype=televideo --generic-console pacman.c

// Excalibur 64, NEC PC8801 (colour)
// zcc +cpm  -create-app -subtype=excali64 -DUSE_UDGS --generic-console pacman.c
// zcc +pc88 -create-app --generic-console  -subtype=disk pacman.c

// Visual 1050 (UDG)
// zcc +cpm  -create-app -subtype=v1050 -DUSE_UDGS --generic-console pacman.c

// C128 in 40 columns mode, UDG and redefined font (keyboard scanning stil hangs the game)
// zcc +c128 -create-app -DNO_80COL -clib=gencon -DUSE_UDGS pacman.c

// Xerox 820 (in 40 columns to deal with memory limits)
// zcc +cpm  -create-app -subtype=x820 --generic-console -DNO_80COL pacman.c

 
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
//#include <curses.h>
#include <sys/ioctl.h>


#define FALSE 0
#define TRUE 1

#define napms(a) msleep(a)

//#define refresh() {}   // curses stuff

/* dfp #define POS(row,col)    fputs(tgoto(vs_cm,(col),(row)),stdout)*/
/* #define POS(row,col)	tputs(tgoto(vs_cm,(col),(row)),1,putch) */
#define POS(row,col)	gotoxy(col, row)
/* dfp */
//#define	PLOT(A,B,C)	POS(A,B);addch(C)
#define	PLOT(A,B,C)   gotoxy(B,A);fputc_cons(C)
//#define	SPLOT(A,B,S)	POS(A,B);addstr(S)
#define	SPLOT(A,B,S)  gotoxy(B,A);puts_cons(S)
#define GAME1	'1'
#define GAME2	'2'
#define GAME3	'3'

#ifdef HAVE_FILES
//#define MAXSCORE	"/var/games/pacman.log"
#define MAXSCORE	"pacman.log"
#endif

#define MSSAVE	5	/* maximum scores saved per game type */
#define MGTYPE	3	/* Maximum game types */
#define	MAXPAC	3	/* maximum number of pacmen to start */
#define	MAXMONSTER	4	/* max number of monsters */
#define EMPTY	0
#define FULL	1
#define LEFT	'h'
#define NLEFT	's'
#define RIGHT	'l'
#define NRIGHT	'f'
#define	NORTH	'k'	/* means UP, but UP defined in vsinit() */
#define NNORTH	'e'
#define	DOWN	'j'
#define NDOWN	'c'
#define HALT	' '
#define REDRAW	'\14'
#define DELETE	'\177'
#define ABORT	'\34'
#define QUIT	'q'
#define CNTLS	'\23'
#define BUF_SIZE	32
#define	UPINT	(-1)
#define	DOWNINT	1
#define	LEFTINT	(-2)
#define	RIGHTINT	2
#define	MONSTER	'M'
#define	RUNNER	'S'
#define	TREASURE	'$'
#define	CHOICE		'*'
#define	GOLD		'.'
#define	POTION		'O'
#define	VACANT		' '	/* space */


#define	GATE		'-'
#define	START	0
#define	RUN	1
#define	FAST	1
#define	SLOW	0
#define	PSTARTX	18
#define	PSTARTY	17
#define	MSTARTX	16	/* monster starting position */
#define	MSTARTY	10	/* monster starting position */
#define	MBEGINX	18	/* monster beginning position */
#define	MBEGINY	7	/* monster beginning position */
#define	TRYPOS	13
#define	TRXPOS	20
#define	GOTONE	1
#define	TURKEY	(-1)
#define	DUP	1
#define	DDOWN	4
#define	DRIGHT	3
#define	DLEFT	2
#define	DNULL	0
#define	BRDX	40
#define	BRDY	23
#define XWRAP	38

/* Scores */
#define	TREASVAL	50
#define	KILLSCORE	200
#define GOLDVAL		10

#define	MSTARTINTVL	10
#define	POTINTVL	25
#define BINTVL		10

#define GOLDCNT	185

#ifdef USE_UDGS
static unsigned char udgs[] = {
    0b00000000,	// wall
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000,

    0b00110100, // cup
    0b01110110,
    0b11110111,
    0b11110111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,

    0b00111100, // cdown
    0b01111110,
    0b11111111,
    0b11111111,
    0b11101111,
    0b11101111,
    0b01101110,
    0b00101100,

    0b00111100, // cleft
    0b01111110,
    0b11111111,
    0b11111111,
    0b00001111,
    0b11111111,
    0b01111110,
    0b00111100,

    0b00111100, // cright
    0b01111110,
    0b11111111,
    0b11111111,
    0b11110000,
    0b11111111,
    0b01111110,
    0b00111100,

    0b00000000, // pup
    0b01000010,
    0b11000011,
    0b11100111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,

    0b00111100, // pdown
    0b01111110,
    0b11111111,
    0b11111111,
    0b11100111,
    0b11000011,
    0b01000010,
    0b00000000,

    0b00111100, // pleft
    0b01111110,
    0b00011111,
    0b00001111,
    0b00001111,
    0b00011111,
    0b01111110,
    0b00111100,

    0b00111100, // pright
    0b01111110,
    0b11111000,
    0b11110000,
    0b11110000,
    0b11111000,
    0b01111110,
    0b00111100,

    0b00111100, // Blinky
    0b01111110,
    0b10011001,
    0b10011001,
    0b11111111,
    0b11111111,
    0b10101011,
    0b10101001,

    0b00111100, // Inky
    0b01111110,
    0b10011001,
    0b11111111,
    0b11100111,
    0b11111111,
    0b11111111,
    0b10010101,

    0b00111100, // Pinky
    0b01111110,
    0b10011001,
    0b11011011,
    0b11111111,
    0b11000111,
    0b11111111,
    0b10101001,

    0b00111100, // Clyde
    0b01111110,
    0b10011001,
    0b10111101,
    0b11111111,
    0b11100111,
    0b11101011,
    0b10101001,


    0b00000000,
    0b00111100, // Scared Blinky
    0b01000010,
    0b10000001,
    0b10100101,
    0b10000001,
    0b10000001,
    0b10101001,

    0b00000000,
    0b00111100, // Scared Inky
    0b01000010,
    0b10100101,
    0b10100101,
    0b10000001,
    0b10000001,
    0b10101001,

    0b00000000,
    0b00111100, // Scared Pinky
    0b01000010,
    0b10100101,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10101001,

    0b00000000,
    0b00111100, // Scared Clyde
    0b01000010,
    0b10100101,
    0b10000001,
    0b10011001,
    0b10000001,
    0b10100100
};

char monst_names[] =  "\211\212\213\214";
char runner_names[] = "\215\216\217\220";

#define	PACMAN	132
#define	WALL	128
#define CUP	129
#define CDOWN	130
#define CLEFT	131
#define CRIGHT	132
#define PUP	133	
#define PDOWN	134
#define PLEFT	135
#define PRIGHT	136

#else

char monst_names[] =  "BIPC";
char runner_names[] = "bipc";

#define	PACMAN	'@'
#define CUP	'|'
#define CDOWN	'|'
#define CLEFT	'-'
#define CRIGHT	'-'
#define PUP	'v'
#define PDOWN	'^'
#define PLEFT	'>'
#define PRIGHT	'<'

#define	WALL		'#'

#endif


#ifdef __PC88__
#define	WALL2		134
#undef	PACMAN
#define	PACMAN		236
#undef	CUP
#define CUP	150
#undef	CDOWN
#define CDOWN	150
#endif

#ifdef USE_CHR127
#define	WALL2		127 // in octal = 177
#endif

#ifdef __SANYO__
#define	WALL2		146  // in octal = 222
#undef	PACMAN
#define	PACMAN		249
#endif

#ifdef __X820__
#define	WALL2		141   // 129 for a full block
#undef	PACMAN
#define	PACMAN		152   // try also 156 or 159
#endif

#ifdef USE_UDGS
#define	WALL2		128  // in octal = 222
#endif


struct	pac
{
	int	xpos;	/* real horizontal position */
	int	ypos;	/* real vertical   position */
	int	dirn;	/* direction of travel */
	int	speed;	/* FAST/SLOW	       */
	int	danger;	/* TRUE if can eat     */
	int	stat;	/* status		*/
	int	xdpos;	/* horizontal position currently displayed at */
	int	ydpos;	/* vertical   position currently displayed at */
};

int  which(struct pac *, int, int);


/*
 * global variables
 */

int	pacsymb = PACMAN,
	rounds,		/* time keeping mechanism */
	killflg,
	delay,
	potion,
	goldcnt,		/* no. of gold pieces remaining */
	game,
	monst_often,
	monsthere,
	boardcount = 1,
	wmonst,
	potintvl = POTINTVL,
	potioncnt,
	bcount = 0,
	/* attrs: normal monster, edible monster, pacman */
	mflash, rflash, pflash,
	showcount,
	treascnt = 0;

unsigned pscore;

extern struct pac
	*pacptr;

int	rscore[MAXMONSTER];

struct pac
	monst[MAXMONSTER];

char *full_names[] = {
	"Blinky", "Inky", "Pinky", "Clyde", 0
};



/*
 * initbrd is used to re-initialize the display
 * array once a new game is started.
 */
char	initbrd[BRDY][BRDX] =
{
"#######################################",
"# . . . * . . . . ### . . . . * . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# * . . * . * . * . . * . * . * . . * #",
"# . ### . # . ########### . # . ### . #",
"# . . . * # . . . ### . . . # * . . . #",
"####### . ##### . ### . ##### . #######",
"      # . # . . * . . * . . # . #      ",
"      # . # . ### - - ### . # . #      ",
"####### . # . #         # . # . #######",
"        * . * #         # * . *        ",
"####### . # . #         # . # . #######",
"      # . # . ########### . # . #      ",
"      # . # * . . . . . . * # . #      ",
"####### . # . ########### . # . #######",
"# . . . * . * . . ### . . * . * . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . # * . * . * . . * . * . * # . . #",
"### . # . # . ########### . # . # . ###",
"# . * . . # . . . ### . . . # . . * . #",
"# . ########### . ### . ########### . #",
"# . . . . . . . * . . * . . . . . . . #",
"#######################################",
};

/*
 * brd is kept for historical reasons.
 * It should only be used in the routine "which"
 * to determine the next move for a monster or
 * in the routine "monster" to determine if it
 * was a valid move. Admittedly this is redundant
 * and could be replaced by initbrd, but it is kept
 * so that someday additional intelligence or
 * optimization could be added to the choice of
 * the monster's next move. Hence, note the symbol
 * CHOICE at most points that a move decision
 * logically HAS to be made.
 */
char	brd[BRDY][BRDX] =
{
"#######################################",
"# . . . * . . . . ### . . . . * . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# * . . * . * . * . . * . * . * . . * #",
"# . ### . # . ########### . # . ### . #",
"# . . . * # . . . ### . . . # * . . . #",
"####### . ##### . ### . ##### . #######",
"      # . # . . * . . * . . # . #      ",
"      # . # . ### - - ### . # . #      ",
"####### . # . #         # . # . #######",
"        * . * #         # * . *        ",
"####### . # . #         # . # . #######",
"      # . # . ########### . # . #      ",
"      # . # * . . . . . . * # . #      ",
"####### . # . ########### . # . #######",
"# . . . * . * . . ### . . * . * . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . # * . * . * . . * . * . * # . . #",
"### . # . # . ########### . # . # . ###",
"# . * . . # . . . ### . . . # . . * . #",
"# . ########### . ### . ########### . #",
"# . . . . . . . * . . * . . . . . . . #",
"#######################################",
};

/*
 * display reflects the screen on the player's
 * terminal at any point in time.
 */
char	display[BRDY][BRDX] =
{
"#######################################",
"# . . . . . . . . ### . . . . . . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . . . . . . . . . . . . . . . . . #",
"# . ### . # . ########### . # . ### . #",
"# . . . . # . . . ### . . . # . . . . #",
"####### . ##### . ### . ##### . #######",
"      # . # . . . . . . . . # . #      ",
"      # . # . ### - - ### . # . #      ",
"####### . # . #         # . # . #######",
"        . . . #         # . . .        ",
"####### . # . #         # . # . #######",
"      # . # . ########### . # . #      ",
"      # . # . . . . . . . . # . #      ",
"####### . # . ########### . # . #######",
"# . . . . . . . . ### . . . . . . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . # . . . . . . . . . . . . # . . #",
"### . # . # . ########### . # . # . ###",
"# . . . . # . . . ### . . . # . . . . #",
"# . ########### . ### . ########### . #",
"# . . . . . . . . . . . . . . . . . . #",
"#######################################",
};



int	incharbuf;
int	bufstat;
char	message[81],	/* temporary message buffer */
	inbuf[2];

int	ppid,
	cpid,
	killcnt = 0;

unsigned int vs_rows;
unsigned int vs_cols;

unsigned
	pscore;

long	timein;

struct uscore
{
	unsigned score;	/* same type as pscore */
	int uid;	/* uid of player */
};

struct scorebrd
{
	struct uscore entry[MSSAVE];
} scoresave[MGTYPE];


void leave();


//------------------------------------------------
#ifndef NO_MOVIE

/*
 * "Movie" routine for Mark Horton's version of PACMAN which uses the
 *  curses library stuff.  You could probably diddle with it a little
 *  and make it shorter/smaller, however, I haven't seen any document-
 *  ation on the "new" curses stuff (although we have it), so I don't
 *  know what I can get away with.
 *
 *  NOTE: This routine was written for a 24 x 80 screen (adm3a, adm5).
 *        If your screen is different (columns matter, lines don't),
 *	  you will have to change some things.  It shouldn't be very
 *	  difficult, but be warned.
 *
 *  Dave Curry (pur-ee!davy)
 *
 */

/* This is the monster we use.  Don't take out the extra spaces here, 
 * they are there so that the thing "cleans up" after itself, since 
 * I'm too lazy to move all over the place writing out spaces.
 */
char *bigmonster[] = {		
	"   _____    ",	
	"  /     \\   ",
	"  | O O |   ",
	"  v^v^v^v   ",
};

static void left1(void)
{
	textcolor(RED);
	SPLOT(10, 0, "_____    \n"); 
	SPLOT(11, 0, "     \\   \n");
	SPLOT(12, 0, " O O |   \n"); 
	SPLOT(13, 0, "^v^v^v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left2(void)
{
	textcolor(RED);
	SPLOT(10, 0, "____    \n"); 
	SPLOT(11, 0, "    \\   \n");
	SPLOT(12, 0, "O O |   \n"); 
 	SPLOT(13, 0, "v^v^v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left3(void)
{
	textcolor(RED);
	SPLOT(10, 0, "___    \n"); 
	SPLOT(11, 0, "   \\   \n");
	SPLOT(12, 0, " O |   \n"); 
	SPLOT(13, 0, "^v^v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left4(void)
{
	textcolor(RED);
	SPLOT(10, 0, "__    \n"); 
	SPLOT(11, 0, "  \\   \n");
	SPLOT(12, 0, "O |   \n"); 
	SPLOT(13, 0, "v^v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left5(void)
{
	textcolor(RED);
	SPLOT(10, 0, "_    \n"); 
	SPLOT(11, 0, " \\   \n");
	SPLOT(12, 0, " |   \n"); 
	SPLOT(13, 0, "^v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left6(void)
{
	textcolor(RED);
	SPLOT(10, 0, "    \n"); 
	SPLOT(11, 0, "\\   \n");
	SPLOT(12, 0, "|   \n"); 
	SPLOT(13, 0, "v   \n");
	textcolor(WHITE);
	//refresh();
}

static void left7(void)
{
	textcolor(RED);
	SPLOT(10, 0, "   \n"); 
	SPLOT(11, 0, "   \n");
	SPLOT(12, 0, "   \n"); 
	SPLOT(13, 0, "   \n");
	textcolor(WHITE);
	//refresh();
}

static void right1(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-11, "   _____ "); 
	SPLOT(11, vs_cols-11, "  /     \\");
	SPLOT(12, vs_cols-11, "  | O O |"); 
	SPLOT(13, vs_cols-11, "  v^v^v^v");
	textcolor(WHITE);
}

static void right2(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-10, "   _____"); 
	SPLOT(11, vs_cols-10, "  /     ");
	SPLOT(12, vs_cols-10, "  | O O "); 
	SPLOT(13, vs_cols-10, "  v^v^v^");
	textcolor(WHITE);
}

static void right3(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-9, "   ____"); 
	SPLOT(11, vs_cols-9, "  /    ");
	SPLOT(12, vs_cols-9, "  | O O"); 
	SPLOT(13, vs_cols-9, "  v^v^v");
	textcolor(WHITE);
}

static void right4(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-8, "   ___"); 
	SPLOT(11, vs_cols-8, "  /   ");
	SPLOT(12, vs_cols-8, "  | O "); 
	SPLOT(13, vs_cols-8, "  v^v^");
	textcolor(WHITE);
}

static void right5(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-7, "   __"); 
	SPLOT(11, vs_cols-7, "  /  ");
	SPLOT(12, vs_cols-7, "  | O"); 
	SPLOT(13, vs_cols-7, "  v^v");
	textcolor(WHITE);
}

static void right6(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-6, "   _"); 
	SPLOT(11, vs_cols-6, "  / ");
	SPLOT(12, vs_cols-6, "  | "); 
	SPLOT(13, vs_cols-6, "  v^");
	textcolor(WHITE);
}

static void right7(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-5, "   "); 
	SPLOT(11, vs_cols-5, "  /");
	SPLOT(12, vs_cols-5, "  |"); 
	SPLOT(13, vs_cols-5, "  v");
	textcolor(WHITE);
}

static void right8(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-4, " ");
	SPLOT(11, vs_cols-4, " ");
	SPLOT(12, vs_cols-4, " ");
	SPLOT(13, vs_cols-4, " ");
	textcolor(WHITE);
}

static void right9(void)
{
	textcolor(RED);
	SPLOT(10, vs_cols-3, " ");
	SPLOT(11, vs_cols-3, " ");
	SPLOT(12, vs_cols-3, " ");
	SPLOT(13, vs_cols-3, " ");
	textcolor(WHITE);
}

static void monst7(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT(10, xxx, "     <");
	textcolor(WHITE);
}

static void monst6(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT( 9, xxx, "     /");
	SPLOT(10, xxx, "    < ");
	SPLOT(11, xxx, "     \\");
	textcolor(WHITE);
}

static void monst5(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT( 8, xxx, "     /");
	SPLOT( 9, xxx, "    / ");
	SPLOT(10, xxx, "   <  ");
	SPLOT(11, xxx, "    \\ ");
	SPLOT(12, xxx, "     \\");
	textcolor(WHITE);
}

static void monst4(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT( 7, xxx, "     /");
	SPLOT( 8, xxx, "    / ");
	SPLOT( 9, xxx, "   /  ");
	SPLOT(10, xxx, "  <   ");
	SPLOT(11, xxx, "   \\   ");
	SPLOT(12, xxx, "    \\  ");
	SPLOT(13, xxx, "     \\ ");
	textcolor(WHITE);
}

static void monst3(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT(7 , xxx, "    / ");
	SPLOT(8 , xxx, "   /  ");
	SPLOT(9 , xxx, "  /   ");
	SPLOT(10, xxx, "      ");
	SPLOT(11, xxx, "  \\   ");
	SPLOT(12, xxx, "   \\  ");
	SPLOT(13, xxx, "    \\ ");
	textcolor(WHITE);
}

static void monst2(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT( 7, xxx, "   / ");
	SPLOT( 8, xxx, "  /  ");
	SPLOT( 9, xxx, "     ");
	SPLOT(10, xxx, "     ");
	SPLOT(11, xxx, "     ");
	SPLOT(12, xxx, "  \\  ");
	SPLOT(13, xxx, "   \\ ");
	textcolor(WHITE);
}

static void monst1(xxx)
int xxx;
{
	textcolor(YELLOW);
	SPLOT( 7, xxx, "  / ");
	SPLOT( 8, xxx, "    ");
	SPLOT( 9, xxx, "    ");
	SPLOT(10, xxx, "    ");
	SPLOT(11, xxx, "    ");
	SPLOT(12, xxx, "    ");
	SPLOT(13, xxx, "  \\ ");
	textcolor(WHITE);
}

void movie(void)
{
	int i, j;

	clrscr();
	//refresh();

	/*
	 * this loop moves the monster and the small pacman from right to
	 * left, until it's time to start printing "fractional" monsters
	 */
	for (i=vs_cols-10; i > (-1); i--) {
		textcolor(RED);
		for (j=0; j < 4; j++) {
			SPLOT((j+10), i, bigmonster[j]);
		}
		textcolor(YELLOW);
		if (i > 20) {
			SPLOT(13, (i-20), "> ");
		}
		else {
			SPLOT(13, 1, " ");
		}
		textcolor(WHITE);
		//refresh();
	}

	/*
	 * left1-left7 are "partial" monster routines, they are also called when
	 * the monster comes back on from the left (called in opposite order)
	 */
	left1(); left2();
	left3(); left4();
	left5(); left6();
	left7();

	sleep(1);

	/* Now we come back on...  */
	left7(); left6();
	left5(); left4();
	left3(); left2();
	left1();

	/*
	 * Now we start moving the big pacman across.  monst1-monst3 are
	 * routines for drawing pieces of the pacman, until the whole thing
	 * is on the screen.
	 */
	for (i=0; i < vs_cols-10; i++) {
		textcolor(RED);
		for(j=0; j < 4; j++) {
			SPLOT((j+10), i, bigmonster[j]);
		}
		textcolor(WHITE);
		if (i > 20) {
			switch(i) {
			case 21:
				monst1((i-20));
				break;
			case 22:
				monst2((i-20));
				break;
			case 23:
				monst3((i-20));
				break;
			default:
				monst4((i-20));
				break;
			}
		}
		//refresh();
	}
	/*
	 * right1-right9 are partial monster routines, for moving him off to
	 * the right of the screen.  monst4 prints the whole pacman.
	 */
	right1(); monst4(vs_cols-30); //refresh();
	right2(); monst4(vs_cols-29); //refresh();
	right3(); monst4(vs_cols-28); //refresh();
	right4(); monst4(vs_cols-27); //refresh();
	right5(); monst4(vs_cols-26); //refresh();
	right6(); monst4(vs_cols-25); //refresh();
	right7(); monst4(vs_cols-24); //refresh();
	right8(); monst4(vs_cols-23); //refresh();
	right9(); monst4(vs_cols-22); //refresh();

	/* Now finish moving the pacman to the end of the screen.  */
	for (i=vs_cols-21; i < vs_cols-6; i++) {
		monst4(i);
		//refresh();
	}

	/* monst5-monst7 print pieces of pacman as he moves off the screen */
	monst5(vs_cols-6); //refresh();
	monst6(vs_cols-5); //refresh();
	monst7(vs_cols-4); //refresh();

	/* clean up a little bit */
	clrscr();
	//refresh();
}

#endif
//------------------------------------------------


void beep() {
	fputc_cons(7);
}

char	str[10];

#ifndef NO_80COL
void update()
{
	sprintf(str, "%6d", pscore);
	SPLOT(0, 52, str);
	sprintf(str, "%6d", goldcnt);
	SPLOT(21, 57, str);
}
#endif

void reinit()
{
	int locx, locy;
	char tmp;

#ifndef NO_MOVIE
	if (boardcount % 2 == 0)
		movie();
#endif
	for (locy = 0; locy < BRDY; locy++)
	{
		for (locx = 0; locx < BRDX; locx++)
		{
			tmp = initbrd[locy][locx];
			brd[locy][locx] = tmp;
			if ((display[locy][locx] = tmp) == CHOICE)
			{
				display[locy][locx] = GOLD;
			};
		};
	};
	goldcnt = GOLDCNT;
	delay = delay * 3 / 4;	/* hot it up */
	boardcount++;
}

//#ifndef NO_80COL
//void errgen(string)
//char	*string;
//{
//	SPLOT(23,45,string);
//}
//#endif

int dokill(mnum)
	int mnum;
{
	struct pac *mptr;
	char msgbuf[50];
	int bscore;

	beep();
	if (monst[mnum].danger == FALSE)
	{
		if (++killcnt == MAXMONSTER)
		{
			if (display[TRYPOS][TRXPOS] == GOLD)
			{
				goldcnt--;
			};
			display[TRYPOS][TRXPOS] = TREASURE;
			PLOT(TRYPOS, TRXPOS, TREASURE);
			killcnt = 0;
			treascnt = potintvl;
		}
#ifndef NO_80COL
		SPLOT(5, 45, "MONSTERS KILLED: ");
		sprintf(message, "%1d", killcnt);
		SPLOT(5, 62, message);
#endif
		mptr = (&monst[mnum]);
		mptr->ypos = MSTARTY;
		mptr->xpos = MSTARTX + (2 * mnum);
		mptr->danger = TRUE;
		mptr->stat = START;
		textcolor(mnum+2);
		PLOT(mptr->ypos, mptr->xpos, monst_names[mnum]);
		textcolor(WHITE);
		monsthere++;
		rounds = 1;	/* force it to be a while before he comes out */
		switch (monsthere) {
		case 1: bscore =     KILLSCORE; break;
		case 2: bscore = 2 * KILLSCORE; break;
		case 3: bscore = 4 * KILLSCORE; break;
		case 4: bscore = 8 * KILLSCORE; break;
		}
		pscore += bscore;
		bcount = BINTVL;
#ifndef NO_80COL
		sprintf(msgbuf, "BONUS: %4d", bscore);
		SPLOT(7, 45, msgbuf);
		sprintf(msgbuf, "You got %s!\n", full_names[mnum]);
		SPLOT(4, 45, msgbuf);
#endif
		return(GOTONE);
	};
	wmonst = mnum;
	return(TURKEY);
}

/*
 * clr -- issues an escape sequence to clear the display
 */

void clr()
{
	clrscr();
}

/*
 *	display initial instructions
 */

void instruct()
{
	clr();
	POS(0, 0);
	printf("Attention: you are in a maze,\nbeing chased by monsters!\n\n");
#ifndef NO_80COL
	printf("There is food scattered uniformly in the maze, marked by \".\".\n");
	printf("One magic potion is available at each spot marked \"O\". Each potion will\n");
	printf("enable you to eat monsters for a limited duration. It will also\n");
	printf("scare them away. When you eat a monster it is regenerated, but this takes\n");
	printf("time. You can also regenerate yourself %d times. Eating all the monsters\n", MAXPAC);
	printf("results in further treasure appearing magically somewhere in the dungeon,\n");
	printf("marked by \"$\". There is a magic tunnel connecting the center left and\n");
	printf("center right parts of the dungeon. The monsters know about it!\n\n");
#endif
	printf("  Type:   h or s  to move left\n");
	printf("          l or f  to move right\n");
	printf("          k or e  to move up\n");
	printf("          j or c  to move down\n");
	printf("          <space> to halt \n");
	printf("          q       to quit\n\n");
	printf("  Type:   1  easy game\n");
	printf("          2  intelligent monsters\n");
	printf("          3  very intelligent monsters\n");
	//refresh();
}

/*
 * over -- game over processing
 */

void over(int signo)
{
	struct passwd *p;
#ifdef HAVE_FILES
	int scorefile = 0;
#endif
	int line, col;
	int i;

	//signo;
	//refresh();
	//signal(SIGINT, SIG_IGN);
	/* high score to date processing */
	if (game != 0)
	{
		col = 45;
		line = 10;
		POS(line++, col);
		printf(" ___________________________ ");
		POS(line++, col);
		printf("|                           |");
		POS(line++, col);
		printf("| G A M E   O V E R         |");
		POS(line++, col);
		printf("|                           |");
		POS(line++, col);
		printf("| Game type: %6.6s         |",game==1?"easy":game==2?"medium":"smart");

//		scorefile = open(MAXSCORE, 2);
//		if (scorefile != -1)
//		{
//			int rc;
//
//			rc = read(scorefile, (char *)scoresave, sizeof(scoresave));
//			if (rc == -1)
//				warn("Failed reading score file %s", MAXSCORE);
//
//			for (i = MSSAVE - 1; i >= 0; i--) {
//				if (scoresave[game - 1].entry[i].score < pscore)
//				{
//					if (i < MSSAVE - 1)
//					{
//						scoresave[game - 1].entry[i + 1].score =
//							scoresave[game - 1].entry[i].score;
//						scoresave[game - 1].entry[i + 1].uid =
//							scoresave[game - 1].entry[i].uid;
//					};
//					scoresave[game - 1].entry[i].score = pscore;
//					scoresave[game - 1].entry[i].uid = getuid();
//				};
//			};
//			lseek(scorefile, 0l, 0);
//			rc = write(scorefile, (char *)scoresave, sizeof(scoresave));
//			if (rc == -1)
//				warn("Failed writing score file %s", MAXSCORE);
//			close(scorefile);
//			POS(line++, col);
//			printf("| High Scores to date:      |");
//			for (i = 0; i < MSSAVE; i++)
//			{
//				setpwent();
//				p = getpwuid(scoresave[game - 1].entry[i].uid);
//				POS(line++, col);
//				printf("| Player : %-8s  %5u  |", p->pw_name,
//					scoresave[game - 1].entry[i].score);
//			};
//		}
//		else
//		{
//			POS(line++, col);
//			printf("|                           |");
//			POS(line++, col);
//			printf("| Please create a 'paclog'  |");
//			POS(line++, col);
//			printf("| file. See 'MAXSCORE' in   |");
//			POS(line++, col);
//			printf("| 'pacdefs.h'.              |");
//		};
//		POS(line, col);
//		printf("|___________________________|");
	};


	//refresh();
	leave();
}


/*
 * leave -- flush buffers,kill the Child, reset tty, and delete tempfile
 */

void leave()
{
	//leaveok(stdscr, FALSE);
	POS(23, 0);
	//refresh();
//	endwin();
	exit(0);
}


/*
 * poll -- read characters sent by input subprocess and set global flags
 */

void pollch(int sltime)
{
	int stop;
	int c;
	int clk;

//	sltime;
	stop = 0;
readin:

	//refresh();
	if (bufstat == EMPTY) {
		if (sltime == 1) {
#ifdef HAVE_CLOCK
		  c = fgetc_cons();
#else
		  do {
			  c = getk();
			  clk++;
		  }
		  while (c == 0);
		  srand(clk);
#endif
		} else
		  c = getk();
		//if (c < 0) {
		if (c == 0) {
			bufstat = EMPTY;
		} else {
			bufstat = FULL;
			incharbuf = c;
		}
	}

	if (bufstat == EMPTY) 
	{
		if (stop)
		{
			goto readin;
		};
		return;
	};
	bufstat = EMPTY;

	switch(incharbuf)
	{
	case LEFT:
	case NLEFT:
	//case KEY_LEFT:
		pacptr->dirn = DLEFT;
		break;

	case RIGHT:
	case NRIGHT:
	//case KEY_RIGHT:
		pacptr->dirn = DRIGHT;
		break;

	case NORTH:
	case NNORTH:
	//case KEY_UP:
		pacptr->dirn = DUP;
		break;

	case DOWN:
	case NDOWN:
	//case KEY_DOWN:
		pacptr->dirn = DDOWN;
		break;

	case HALT:
	//case KEY_HOME:
		pacptr->dirn = DNULL;
		break;

//	case REDRAW:
//		clearok(curscr, TRUE);
//		draino(0);
//		tcdrain(STDOUT_FILENO);
//		break;

	case ABORT:
	case DELETE:
	case QUIT:
		over(0);
		break;

	case 'S':
		stop = 1;
		goto readin;

	case 'G':
		stop = 0;
		goto readin;

	case GAME1:
		if (game == 0)
			game = 1;
		break;

	case GAME2:
		if (game == 0)
			game = 2;
		break;

	case GAME3:
		if (game == 0)
			game = 3;
		break;

	default:
		goto readin;
	}
}

unsigned int getrand(range)
	int range;
{
	unsigned int q;

	q = rand();
	return(q % range);
}



/*
 * init -- does global initialization and spawns a child process to read
 *      the input terminal.
 */

void init()
{
	int tries = 0;
	int c;

#ifdef SWITCH_MODE
   c = SWITCH_MODE;
   console_ioctl(IOCTL_GENCON_SET_MODE, &c);
#endif

//	textcolor(WHITE);
//	textbackground(BLACK);
//	fputc_cons(12);

#ifdef USE_UDGS
//#ifdef __C128__
//	outp(0xD018,0x8C);		// Set character generator ptr to 14336
//	/* Load user defined graphics (reversed) onto character set */	
//	for (i=0; i<128; i++)
//		udg[i+8]=sprites[i]^0xff;
//#else
  void *param = &udgs;
  console_ioctl(IOCTL_GENCON_SET_UDGS, &param);
//#endif
#endif

	//errno = 0;
	//() time(&timein);	/* get start time */
	//srand((unsigned)timein);	/* start rand randomly */
#ifdef HAVE_CLOCK
	srand(clock());	/* start rand randomly */
#endif
	//signal(SIGINT, over);
	//signal(SIGQUIT, over);

	/* Curses init - could probably eliminate much of stuff below */
	//initscr();
	//if ((start_color()) == OK)
	//{
	//     init_pair (1, COLOR_YELLOW, COLOR_BLUE);
	//     init_pair (2, COLOR_BLUE, COLOR_YELLOW);
	//     init_pair (3, COLOR_YELLOW, COLOR_GREEN);
	//     init_pair (4, COLOR_MAGENTA, COLOR_CYAN);
	//     init_pair (5, COLOR_YELLOW, COLOR_RED);
	//}
	//noecho();
	//crmode();
	//nonl();
	//leaveok(stdscr, TRUE);
	//keypad(stdscr, TRUE);
	//nodelay(stdscr, TRUE);

	//vs_rows = LINES;
	//vs_cols = COLS;

	screensize(&vs_cols,&vs_rows);

	if (delay == 0)
		delay = 500;	/* number of ticks per turn */

	/*
	 * New game starts here
	 */
	if (game == 0)
		instruct();
	while ((game == 0) && (tries++ < 300))
	{
		napms(100);
		pollch(1);
	};
	if (tries >= 300)
	{
		/* I give up. Let's call it quits. */
		leave();
	};
	goldcnt = GOLDCNT;
	pscore = 0;
	clr();
}



// #define FIRSTMSGLINE	13
// #define LASTMSGLINE	13
// /*
//  * This function is convenient for debugging pacman.  It isn't used elsewhere.
//  * It's like printf and prints in a window on the right hand side of the screen.
//  */
// void msgf(fmt, arg1, arg2, arg3, arg4)
// char *fmt;
// int arg1, arg2, arg3, arg4;
// {
// 	char msgbuf[100];
// 	static char msgline = FIRSTMSGLINE;
// 
// 	sprintf(msgbuf, fmt, arg1, arg2, arg3, arg4);
// 	SPLOT(msgline, 45, msgbuf);
// 	if (++msgline > LASTMSGLINE)
// 		msgline = FIRSTMSGLINE;
// }
// 


void startmonst()
{
	struct pac *mptr;
	int monstnum;

	for (mptr = &monst[0], monstnum = 0; monstnum < MAXMONSTER; mptr++, monstnum++)
	{
		if (mptr->stat == START)
		{
			rscore[monstnum] = 1;

			/* clear home */
			PLOT(mptr->ypos, mptr->xpos, VACANT);

			/* initialize moving monster */
			mptr->ypos = MBEGINY;
			mptr->xpos = MBEGINX;
			mptr->ydpos = MBEGINY;
			mptr->xdpos = MBEGINX;
			mptr->stat = RUN;
			if (mptr->danger == TRUE)
			{
				textcolor(monstnum+10);
				PLOT(MBEGINY, MBEGINX, monst_names[monstnum] | mflash);
			} else {
				textcolor(monstnum+2);
				PLOT(MBEGINY, MBEGINX, runner_names[monstnum] | mflash);
			}
			textcolor(WHITE);

//			PLOT(MBEGINY, MBEGINX, mptr->danger ?
//				monst_names[monstnum] | mflash | COLOR_PAIR (monstnum+1):
//				runner_names[monstnum] | rflash | COLOR_PAIR (monstnum+1));

			/* DRIGHT or DLEFT? */
			mptr->dirn = getrand(2) + DLEFT;
			break;
		}
	}
}

void monster(mnum)
	int mnum;
{
	struct pac *mptr;
	int newx = 0, newy = 0;
	int tmpx, tmpy;

	mptr = &monst[mnum];

	/* remember monster's current position */
	tmpx = mptr->xpos;
	tmpy = mptr->ypos;

	/* if we can, let's move a monster */
	if (mptr->stat == RUN)
	{
		/* get a new direction */
		mptr->dirn = which(mptr, tmpx, tmpy);
		switch (mptr->dirn)
		{
		case DUP:
			newy = tmpy + UPINT;
			newx = tmpx;
			break;

		case DDOWN:
			newy = tmpy + DOWNINT;
			newx = tmpx;
			break;

		case DLEFT:
			newx = tmpx + LEFTINT;
			newy = tmpy;
			if (newx <= 0)
				newx = XWRAP;	/* wrap around */
			break;

		case DRIGHT:
			newx = tmpx + RIGHTINT;
			newy = tmpy;
			if (newx >= XWRAP)
				newx = 0;	/* wrap around */
			break;
		}

		/* use brd to determine if this was a valid direction */
		switch (brd[newy][newx])
		{
		case GOLD:
		case VACANT:
		case POTION:
		case TREASURE:
		case CHOICE:
			/* set new position */
			mptr->xpos = newx;
			mptr->ypos = newy;

			/* run into a pacman? */
			if ((newy == pacptr->ypos) &&
				(newx == pacptr->xpos))
			{
				killflg = dokill(mnum);
			};
			if (rounds % monst_often == 0 || killflg == TURKEY) {
				textcolor(BLUE);
				PLOT(mptr->ydpos,mptr->xdpos,
					display[mptr->ydpos][mptr->xdpos]);
				if (mptr->danger == TRUE)
				{
					textcolor(mnum+10);
					PLOT(newy, newx, monst_names[mnum] | mflash);
				}
				else if (killflg != GOTONE)
				{
					textcolor(mnum+2);
					PLOT(newy, newx, runner_names[mnum] | rflash);
				};
				textcolor(WHITE);
				mptr->ydpos = newy;
				mptr->xdpos = newx;
			}
			break;

		default:
//#ifndef NO_80COL
//			errgen("bad direction");
//#endif
			break;
		};
	}
}

int which(mptr, x, y)	/* which directions are available ? */
	struct pac *mptr;
	int x, y;
{
	char *brdptr;
	int movecnt;
	int submovecnt;
	int next;
	int moves[4];
	int submoves[4];
	int nydirn, nxdirn;
	int goodmoves;
	int offx, offy;
	int tmpdirn;

	/*
	 * As a general rule: determine the set of all
	 * possible moves, but select only those moves
	 * that don't require a monster to backtrack.
	 */
	movecnt = 0;
	brdptr = &(brd[y][x]);
	if (((tmpdirn = mptr->dirn) != DDOWN) &&
		((next = *(brdptr + (BRDX * UPINT))) != WALL) &&
		(next != GATE))
	{
		moves[movecnt++] = DUP;
	};
	if ((tmpdirn != DUP) &&
		((next = *(brdptr + (BRDX * DOWNINT))) != WALL) &&
		(next != GATE))
	{
		moves[movecnt++] = DDOWN;
	};
	if ((tmpdirn != DRIGHT) &&
		((next = *(brdptr + LEFTINT)) != WALL) &&
		(next != GATE))
	{
		moves[movecnt++] = DLEFT;
	};
	if ((tmpdirn != DLEFT) &&
		((next = *(brdptr + RIGHTINT)) != WALL) &&
		(next != GATE))
	{
		moves[movecnt++] = DRIGHT;
	};

	/*
	 * If the player requested intelligent monsters and
	 * the player is scoring high ...
	 */
	if (game >= 2 && getrand(game == 2 ? 10000 : 1000) < pscore)
	{
		/* make monsters intelligent */
		if (mptr->danger == FALSE)
		{
			/*
			 * Holy Cow!! The pacman is dangerous,
			 * permit monsters to reverse direction
			 */
			switch (tmpdirn)
			{
			case DUP:
				if ((*(brdptr + (BRDX * DOWNINT)) != WALL) &&
					(*(brdptr + (BRDX * DOWNINT)) != GATE))
				{
					moves[movecnt++] = DDOWN;
				};
				break;

			case DDOWN:
				if ((*(brdptr + (BRDX * UPINT)) != WALL) &&
					(*(brdptr + (BRDX * UPINT)) != GATE))
				{
					moves[movecnt++] = DUP;
				};
				break;

			case DRIGHT:
				if ((*(brdptr + LEFTINT) != WALL) &&
					(*(brdptr + LEFTINT) != GATE))
				{
					moves[movecnt++] = DLEFT;
				};
				break;

			case DLEFT:
				if ((*(brdptr + RIGHTINT) != WALL) &&
					(*(brdptr + RIGHTINT) != GATE))
				{
					moves[movecnt++] = DRIGHT;
				};
				break;
			};
		};

		/* determine the offset from the pacman */
		offx = x - pacptr->xpos;
		offy = y - pacptr->ypos;
		if (offx > 0)
		{
			/*need to go left */
			nxdirn = DLEFT;
		}
		else
		{
			if (offx < 0)
			{
				nxdirn = DRIGHT;
			}
			else
			{
				/*need to stay here */
				nxdirn = DNULL;
			};
		};
		if (offy > 0)
		{
			/*need to go up */
			nydirn = DUP;
		}
		else
		{
			if (offy < 0)
			{
				/* need to go down */
				nydirn = DDOWN;
			}
			else
			{
				/* need to stay here */
				nydirn = DNULL;
			};
		};
		goodmoves = 0;
		for (submovecnt = 0; submovecnt < movecnt; submovecnt++)
		{
			if (mptr->danger == TRUE)
			{
				if ((moves[submovecnt] == nydirn) ||
					(moves[submovecnt] == nxdirn))
				{
					submoves[goodmoves++] = moves[submovecnt];
				};
			}
			else
			{
				if ((moves[submovecnt] != nydirn) &&
					(moves[submovecnt] != nxdirn))
				{
					submoves[goodmoves++] = moves[submovecnt];
				};
			};
		};
		if (goodmoves > 0)
		{
			return(submoves[getrand(goodmoves)]);
		};
	};
	return(moves[getrand(movecnt)]);
}


#ifdef WALL2
char* replacechar(char *str, char orig, char rep) {
    char *ix = str;
    while((ix = strchr(ix, orig)) != NULL)
        *ix++ = rep;
    return str;
}
#endif
 
/*
 * global variables
 */

struct pac
	pac;

struct pac
	pacstart =
{
	PSTARTX,
	PSTARTY,
	DNULL,
	SLOW,
	FALSE,
	0,
	0,
	0
};

struct pac
	*pacptr = &pac;

#define DEFCHPERTURN 60

static void pacman();

int main(argc, argv)
int argc;
char **argv;
{
	struct pac *mptr;
	char msgbuf[50];
	char gcnt[10];
	long denom;
	int chperturn = DEFCHPERTURN;
	int monstcnt;	/* monster number */
	int tmp;	/* temp variables */
	int pac_cnt;
	int tries;
	int c;

// Debugging, print the special characters available to look for the nice ones
// fputc_cons(12);
// for (c=128;c<=255;c++) printf("%c-%d   ",c,c);
//	while(1){};

	game = 0;
	
	pflash=mflash=rflash=0;

	//pflash = A_BLINK | COLOR_PAIR (5);
	//mflash = A_BOLD;
	//rflash = A_REVERSE;

//	while ((c = getopt(argc, argv, "cemhpn:")) != EOF)
//		switch(c) {
//		case 'c':
//			showcount = 1;
//			break;
//		case 'e':
//			game = 1;
//			break;
//		case 'm':
//			game = 2;
//			break;
//		case 'h':
//			game = 3;
//			break;
//		case 'p':
//			mflash = pflash = rflash = 0;
//			break;
//		case 'n':
//			chperturn = atoi(optarg);
//			break;
//		default:
//			fprintf(stderr, "Usage: pacman -emh -p -n#\n");
//			exit(1);
//		}
	
	init();		/* global init */

	for (pac_cnt = MAXPAC; pac_cnt > 0; pac_cnt--)
	{
redraw:
//		erase();
		potioncnt = 0;
		treascnt = 0;
		bcount = 0;
		potion = FALSE;

#ifndef NO_80COL
		SPLOT(0, 45, "SCORE: ");
		sprintf(msgbuf, "GAME: %s",	game==1 ? "EASY" :
							game==2 ? "MEDIUM" :
								  "HARD");
		SPLOT(0, 65, msgbuf);
		SPLOT(21, 45, "food left = ");
		sprintf(gcnt, "%6d", goldcnt);
		SPLOT(21, 57, gcnt);
#endif

		/*
		 * We update the monsters every monst_often turns, to keep
		 * the CRT caught up with the computer.  The fudge factor
		 * was calculated from the assumption that each full syncscreen
		 * outputs chperturn characters.  The default is pessimistic
		 * based on ANSI and HP terminals w/verbose cursor addressing.
		 */
		//denom = ((long) delay) * baudrate();
		denom = ((long) delay) * 3000;
		monst_often = (chperturn * 10000L + denom - 1) / denom;
		if (monst_often < 1)
			monst_often = 1;

#ifndef NO_80COL
		if (potion == TRUE)
		{
			SPLOT(3, 45, "COUNTDOWN: ");
			sprintf(message, "%2d", potioncnt);
			SPLOT(3, 60, message);
		};
#endif

		pacsymb = PACMAN;
		killflg = FALSE;
#ifndef NO_80COL
		sprintf(message,
			"delay = %3d, syncscreen = %3d", delay, monst_often);
		SPLOT(22, 45, message);
#endif
		/*
		 * PLOT maze
		 */
		textcolor(BLUE);
		for (tmp = 0; tmp < BRDY; tmp++)
		{
#ifdef WALL2
			replacechar(&(display[tmp][0]),'#',WALL2);
#endif
			SPLOT(tmp, 0, &(display[tmp][0]));
		};
		/* initialize a pacman */
		pac = pacstart;
		textcolor(YELLOW);
		PLOT(pacptr->ypos, pacptr->xpos, pacsymb | pflash);
		/* display remaining pacmen */
		for (tmp = 0; tmp < pac_cnt - 1; tmp++)
		{
			PLOT(vs_rows >=24 ? 23 : 22, (MAXPAC * tmp), PACMAN);
		};
		textcolor(WHITE);
		/*
		 * Init. monsters
	 	 */
		for (mptr = &monst[0], monstcnt = 0; monstcnt < MAXMONSTER; mptr++, monstcnt++)
		{
			mptr->xpos = MSTARTX + (2 * monstcnt);
			mptr->ypos = MSTARTY;
			mptr->speed = SLOW;
			mptr->dirn = DNULL;
			mptr->danger = TRUE;
			mptr->stat = START;
			textcolor(monstcnt+2);
			PLOT(mptr->ypos, mptr->xpos, monst_names[monstcnt]);
			textcolor(WHITE);
			mptr->xdpos = mptr->xpos;
			mptr->ydpos = mptr->ypos;
		};
		rounds = 0;	/* timing mechanism */
#ifndef NO_80COL
		update();
#endif
		//refresh();
		tries = 0;
		while ((pacptr->dirn == DNULL) && (tries++ < 300))
		{
			napms(100);
			pollch(1);
		}

		/* main game loop */
		do
		{
			if (rounds++ % MSTARTINTVL == 0)
			{
				startmonst();
			};
			pacman();
			if (killflg == TURKEY)
				break;
			for (monstcnt = 0; monstcnt < (MAXMONSTER / 2); monstcnt++)
			{
				monster(monstcnt);	/* next monster */
			};
			if (killflg == TURKEY)
				break;
			if (pacptr->speed == FAST)
			{
				pacman();
				if (killflg == TURKEY)
					break;
			};
			for (monstcnt = (MAXMONSTER / 2); monstcnt < MAXMONSTER; monstcnt++)
			{
				monster(monstcnt);	/* next monster */
			};
			if (killflg == TURKEY)
				break;
			if (potion == TRUE)
			{
#ifndef NO_80COL
				sprintf(message, "%2d", potioncnt);
				SPLOT(3, 60, message);
#endif
				if (potioncnt == 10 || potioncnt < 5)
					beep();
				if (--potioncnt <= 0)
				{
#ifndef NO_80COL
					SPLOT(3,45,"                        ");
					SPLOT(4,45,"                        ");
					SPLOT(5,45,"                        ");
#endif
					potion = FALSE;
					pacptr->speed = SLOW;
					pacptr->danger = FALSE;
					for (monstcnt = 0; monstcnt < MAXMONSTER; monstcnt++)
					{
						monst[monstcnt].danger = TRUE;
					}
				}
			}
#ifndef NO_80COL
			if (bcount && --bcount == 0) {
				SPLOT(7,45,"                   ");
			}
#endif
			if (treascnt && --treascnt == 0) {
				display[TRYPOS][TRXPOS] = VACANT;
				PLOT(TRYPOS, TRXPOS, VACANT);
			}
#ifndef NO_80COL
			if (rounds % monst_often == 0)
				update();	/* score display etc */
#endif
			//refresh();
			if (goldcnt <= 0)
			{
				potintvl -= 5;
				if (potintvl <= 0)
					potintvl = 5;
				reinit();
				goto redraw;
			};
		} while (killflg != TURKEY);
#ifndef NO_80COL
		sprintf(msgbuf, "Oops!  %s got you!\n", full_names[wmonst]);
		SPLOT(5, 45, msgbuf);
#endif
		//flushinp();
		getk();  //  throws away any typeahead that has been typed by the user and has not yet been read by the program.
		//refresh();
		sleep(2);
	}
#ifndef NO_80COL
	SPLOT(8, 45, "THE MONSTERS ALWAYS TRIUMPH");
	SPLOT(9, 45, "IN THE END!");
	update();
#else

	clrscr();

	sprintf(msgbuf, "Oops!  %s got you!\n", full_names[wmonst]);
	SPLOT(5, 5, msgbuf);

	SPLOT(8, 5, "THE MONSTERS ALWAYS TRIUMPH");
	SPLOT(9, 5, "IN THE END!");

	SPLOT(0, 5, "SCORE: ");
	sprintf(str, "%6d", pscore);
	SPLOT(0, 12, str);
	sprintf(str, "%6d", goldcnt);
	SPLOT(21, 17, str);
#endif
	//refresh();
	over(0);
}

static void pacman()
{
	struct pac *mptr;
	char msgbuf[50];
	int sqtype = VACANT;
	int mcnt;
	int tmpx, tmpy;
	int bscore;

	//refresh();

	/* pause; this is the main delay on each turn */
	napms(delay);

	/*
	 * Wait until .1 seconds or less of output in queue.
	 * This is to make it work better on verbose terminals
	 * at 1200 baud.
	 */
////	draino(100);
//	tcdrain(STDOUT_FILENO);

	/* get command from player, but don't wait */
	pollch(0);

	/* remember current pacman position */
	tmpx = pacptr->xpos;
	tmpy = pacptr->ypos;

	/* "eat" any gold */
	/* update display array to reflect what is on terminal */
	display[tmpy][tmpx] = VACANT;

	/* what next? */
	switch (pacptr->dirn)
	{
	case DUP:
		pacsymb = (rounds%2) ? CUP : PUP;
		switch (sqtype = display[tmpy + UPINT][tmpx])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->ypos += UPINT;
			break;

		default:
			pacptr->dirn = DNULL;
			pacsymb = PACMAN;
			break;
		};
		break;
	case DDOWN:
		pacsymb = (rounds%2) ? CDOWN : PDOWN;
		switch (sqtype = display[tmpy + DOWNINT][tmpx])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->ypos += DOWNINT;
			break;

		default:
			pacptr->dirn = DNULL;
			pacsymb = PACMAN;
			break;
		};
		break;
	case DLEFT:
		if(tmpx == 0)
		{
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos = XWRAP;
			sqtype = VACANT;
			break;
		};
		pacsymb = (rounds%2) ? CLEFT : PLEFT;
		switch (sqtype = display[tmpy][tmpx + LEFTINT])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos += LEFTINT;
			break;
		
		default:
			pacptr->dirn = DNULL;
			pacsymb = PACMAN;
			break;
		};
		break;
	case DRIGHT:
		if(tmpx == XWRAP)
		{
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos = 0;
			sqtype = VACANT;
			break;
		};
		pacsymb = (rounds%2) ? CRIGHT : PRIGHT;
		switch (sqtype = display[tmpy][tmpx + RIGHTINT])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos += RIGHTINT;
			break;

		default:
			pacptr->dirn = DNULL;
			pacsymb = PACMAN;
			break;
		};
		break;
	case DNULL:
		pacsymb = PACMAN;
		break;
	}

	/* did the pacman get any points or eat a potion? */
	switch (sqtype)
	{
	case CHOICE:
	case GOLD:
		pscore += GOLDVAL;
		goldcnt--;
		break;

	case TREASURE:
		switch (boardcount) {
			case 0:
			case 1:          bscore =  100; break;
			case 2:          bscore =  200; break;
			case 3: case  4: bscore =  500; break;
			case 5: case  6: bscore =  700; break;
			case 7: case  8: bscore = 1000; break;
			default:
			case 9: case 10: bscore = 2000; break;
		}
		pscore += bscore;
#ifndef NO_80COL
		sprintf(msgbuf, "BONUS: %4d", bscore);
		SPLOT(7, 45, msgbuf);
#endif
		bcount = BINTVL;
		break;

	case POTION:
#ifndef NO_80COL
		SPLOT(3, 45, "COUNTDOWN: ");
#endif
		potion = TRUE;
		potioncnt = potintvl;
		monsthere = 0;
		pacptr->speed = FAST;
		pacptr->danger = TRUE;

		/* slow down monsters and make them harmless */
		mptr = &monst[0];
		for (mcnt = 0; mcnt < MAXMONSTER; mcnt++)
		{
			mptr->speed = SLOW;
			mptr->danger = FALSE;
			mptr++;
		}
		break;
	}

	/* did the pacman run into a monster? */
	killflg = FALSE;
	for (mptr = &monst[0], mcnt = 0; mcnt < MAXMONSTER; mptr++, mcnt++)
	{
		if ((mptr->xpos==pacptr->xpos) && (mptr->ypos==pacptr->ypos))
			killflg = dokill(mcnt);
	};
	if (killflg != TURKEY)
	{
		textcolor(YELLOW);
		PLOT(pacptr->ypos, pacptr->xpos, pacsymb | pflash);
		textcolor(WHITE);
	};
	//refresh();
}

