/*
 *		K A L E I D
 *
 *		Copyright (C) 1981, 1984
 *		Martin Minow
 *		Arlington, MA
 *
 * In the screen output routines, row 1 is the top row, row 0 means
 * "don't move the cursor", Col 1 is left-margin;  col 0 is meaningless.
 * vtout(row, col, NULL) flushes the buffer.
 *
 * NOTE: You must set your terminal to "NOWRAP"
 */

/*)BUILD

	(the z88dk library will react correctly in both VT52 [TRUE] or VT100 [FALSE] mode)
	 zcc +zx -clib=ansi -create-app -lndos -O3 kaleid.c -pragma-define:ansicolumns=64 -DVT52=TRUE
	 
	(default value for VT52 is set to FALSE, -vt52 option is available)
	 zcc +cpm -create-app -lndos -O3 kaleid.c -DVT52=FALSE
	 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FALSE 0
#define TRUE 1



#define	EOS		0		/* End of string		*/
#ifdef	rt11
int	$$narg		= 1;		/* Don't argv prompt		*/
#endif
//extern int	isvt52;			/* TRUE if vt52 output		*/

char	inline[513];			/* Argument line		*/

/*
 * Note: inline must be dimensioned longer than the longest possible
 * output string (20 * "longest move + 1") + 1
 */

/*
 * The font.. strings select the characters written into the various
 * quadrants: ul  = upper left, etc.
 */

char	fontul[] =	"!#@$%:-+|=**..~ >< )( ][ `' \\/ ";
char	fontur[] =	"!#@$%:-+|=**..~ <> () [] '` /\\ ";
char	fontll[] =	"!#@$%:-+|=**..~ >< )( ][ '` /\\ ";
char	fontlr[] =	"!#@$%:-+|=**..~ <> () [] `' \\/ ";
#define	FONTSIZE	((sizeof fontul) - 1)
//#define	FONTSIZE	32
int	verbose		= FALSE;

/*
 * Define the kaleidiscope as a circle centered on the display screen.
 * The screen is 24 lines high.  Thus the vertical radius is 12.
 * For an aspect ratio (width:height) of 0.6, the corresponding
 * horizontal radius, MAXX, will be 21.
 *
 * ytab[MAXX] entries give the maximum height for a given horizontal
 * distance from the center.  Values were tuned by inspection.
 */

#define	MAXX	21			/* Max. horizontal distance	*/

/*
 * Y-axis table.  Change this to change the shape of the display.
 */

int ytab[MAXX] = {
	12,	12,	12,	12,
	12,	12,	12,	11,
	11,	11,	10,	10,
	 9,	 9,	 8,	 8,
	 7,	 7,	 6,	 4,
	 1,
};


/*
 *		S i m p l e   S c r e e n   F u n c t i o n s
 *
 * All functions work in terms of [row, column] where the upper-left
 * corner is [1, 1].  Note that, in all routines, if row is zero,
 * the cursor will not be moved before the operation commenses.
 *
 * Set global isvt52 non-zero if running on that vernerable beast.
 *
 * You may need to call vtout(0, 0, NULL) occasionally.
 */

int		isvt52 = VT52;

#ifdef	decus
extern int	$$rsts;		/* On RSTS/E, we must set ESC's 8th bit	*/
#endif




vtcurse(int r, int c)
/*
 * Move the cursor to [r,c].  r == 0 is a no-op.
 */
{
	if (r == 0)
	    return;
#ifdef	decus
	if (isvt52) {
	    printf(($$rsts) ? "\r\233Y%c%c" : "\r\033Y%c%c",
		r + 040 - 1, c + 040 - 1);
	}
	else {
	    printf(($$rsts) ? "\233[%d;%dH" : "\033[%d;%dH", r, c);
	}
#else
	if (isvt52) {
	    printf("\r\033Y%c%c", r + 040 - 1, c + 040 - 1);
	}
	else {
	    printf("\033[%d;%dH", r, c);
	}
#endif
}


vtout(int r, int c, char *text)
/*
 * Write text starting at [r, c].  If r == 0, don't move cursor.
 */
{
	vtcurse(r, c);
	fputs(text, stdout);
}

erpage(int r, int c)
/*
 * Erase the page from [r,c].  If r == 0, erase from current position.
 */
{
	vtcurse(r, c);
#ifdef	decus
	if (isvt52)
	    fputs(($$rsts) ? "\233J" : "\033J", stdout);
	else
	    fputs(($$rsts) ? "\233[J" : "\033[J", stdout);
#else
	if (isvt52)
	    fputs("\033J", stdout);
	else
	    fputs("\033[J", stdout);
#endif
}

erline(int r, int c)
/*
 * Erase to end of line from [r,c].  If r == 0, don't move cursor.
 */
{
	vtcurse(r, c);
#ifdef	decus
	if (isvt52)
	    fputs(($$rsts) ? "\233K" : "\033K", stdout);
	else
	    fputs(($$rsts) ? "\233[K" : "\033[K", stdout);
#else
	if (isvt52)
	    fputs("\033K", stdout);
	else
	    fputs("\033[K", stdout);
#endif
}




/*
 * There are two move strings per octant -- one for straight-line
 * movement, one for diagonals.  The octant numbering is defined
 * by a series of tests.  The numbering forms a gray code:
 *
 *		 -X  +X
 *
 *		\ 1 | 5 /	abs(Y-difference) > abs(X-difference)
 *	-Y     0 \  |  / 4	abs(Y-difference) < abs(X-difference)
 *		  \ | /
 *	      ------+------
 *		  / | \
 *	+Y     2 /  |  \ 6	abs(Y-difference) < abs(X-difference)
 *		/ 3 | 7 \	abs(Y-difference) > abs(X-difference)
 *
 * In the following, \233 is a "parity escape."  By setting the
 * parity bit, (some) operating systems will not notice the
 * control character.  The vt100 sequences follow the ANSI standard
 * for display control.
 */

typedef struct move {
    char	*straight;
    char	*diagonal;
} MOVE;

MOVE vt52move[] = {
    { "\b\b",		"\b\b\233A" },	/* -X	-X -Y	0		*/
    { "\b\233A",	"\b\b\233A" },	/* -Y	-X -Y	1		*/
    { "\b\b",		"\b\b\233B" },	/* -X	-X +Y	2		*/
    { "\b\233B",	"\b\b\233B" },	/* +Y	-X +Y	3		*/
    { "",		"\233A" },	/* +X	+X -Y	4		*/
    { "\b\233A", 	"\233A" },	/* -Y	+X -Y	5		*/
    { "",		"\233B" },	/* +X	+X +Y	6		*/
    { "\b\233B",	"\233B" },	/* +Y	+X +Y	7		*/
};

MOVE vt100move[] = {
	{ "\b\b",	"\b\b\233[A" },	/* -X	-X -Y	0		*/
	{ "\b\233[A",	"\b\b\233[A" },	/* -Y	-X -Y	1		*/
	{ "\b\b",	"\b\b\233[B" },	/* -X	-X +Y	2		*/
	{ "\b\233[B",	"\b\b\233[B" },	/* +Y	-X +Y	3		*/
	{ "",		"\233[A" },	/* +X	+X -Y	4		*/
	{ "\b\233[A", 	"\233[A" },	/* -Y	+X -Y	5		*/
	{ "",		"\233[B" },	/* +X	+X +Y	6		*/
	{ "\b\233[B",	"\233[B" },	/* +Y	+X +Y	7		*/
};


xymove(int x1, int y1, int x2, int y2, char c)
/* int		x1, y1;		 From here				*/
/* int		x2, y2;		 To here				*/
/* char		c;			 Spray me				*/

/*
 * Stockton's algorithm from CACM (Algorithm 162)  -- as revised
 * (reinvented) by Bresenham (IBM Systems Journal vol. 4, no. 1, 1965).
 * This implementation is based on one by Don North and closely follows
 * Bresenham's version.
 */
{
	int		dx, dy;	/* X and Y distance			*/
	int		smagic;	/* Magic straight move increment	*/
	int		dmagic;	/* Magic diagonal move increment	*/
	int		longer;	/* Long side of the plot triangle	*/
	int		dda;	/* Distance to hypotenuse (diagonal)	*/
	char		*smove;	/* -> Straight line move string		*/
	char		*dmove;	/* -> Diagonal line move string		*/

	union {
	    char	*tp;	/* Text pointer				*/
	    MOVE	*index;	/* Octant index				*/
	} r;

	extern char	*cpystr(); /* Copy string, return ptr. to end	*/
	

	/*
	 * Determine which octant.
	 */
	r.index = (isvt52) ? &vt52move[0] : &vt100move[0];
	if ((dx = x2 - x1) >= 0)
	    r.index += 4;
	if ((dy = y2 - y1) >= 0)
	    r.index += 2;
	/*
	 * Determine the move parameters:
	 * 	r.index	The octant in which the move takes place.
	 *	longer	The longer of the two sides of the right triangle
	 *		[x1,y1], [x1, y2], [x2, y2].  The computation
	 * 		calculation uses absolute distances, normalizing
	 *		the triangle to the upper right quadrant.
	 *	shorter	The other side (not explicitly calculated).
	 *	smagic	A magic number for straight moves: (shorter * 2)
	 *	dmagic	A magic number for diagonal moves:
	 *			(2 * (smagic - longer))
	 *	dda	The distance to the diagonal.  The intial value
	 *		is:  ((2 * smagic) - shorter).
	 *	smove	-> the string for straight moves.
	 *	dmove	-> the string for diagonal moves.
	 * The magic numbers are explained (somewhat) in Bresenham.
	 * Stockton's algorithm does the same thing in an even less
	 * transparent manner.
	 */
	if ((dx = abs(dx)) >= (dy = abs(dy))) {
	    longer = dx;		/* dx is the long side		*/
	    smagic = dy * 2;
	}
	else {
	    longer = dy;		/* dy is the long side		*/
	    smagic = dx * 2;
	    r.index++;			/* Finialize the octant		*/
	}
	dda = smagic - longer;		/* Initial error value		*/
	dmagic = dda - longer;		/* Diagonal magic value		*/
	smove = r.index->straight;	/* Output smove on a straight	*/
	dmove = r.index->diagonal;	/* and dmove on a diagonal move	*/
	/*
	 * Do the pen moves.
	 */
	r.tp = inline;			/* Store pen moves into inline	*/
	while (*r.tp++ = c, --longer >= 0) {
	    if (dda < 0) {
		r.tp = cpystr(r.tp, smove);		/* Straight	*/
		dda += smagic;
	    }
	    else {
		r.tp = cpystr(r.tp, dmove);		/* Diagonal	*/
		dda += dmagic;
	    }
	}
	*r.tp = EOS;			/* Terminate pen move string	*/
	vtout(y1, x1, inline);		/* and write it to the screen	*/
}


/*
 * Random stuff
 */

int irand(int	max)
{
	return (rand()%max);
}

/*
 * Copy in to out, return a pointer to the trailing EOS.
 */
char *cpystr(char *out, char *in)
{
 strcpy(out,in)+strlen(out)-1;
}


#ifdef	MINOV_CODE
int irand(int	max)

/*
 * Return a value from 0 to max-1.
 */
{
	long	temp;

	temp = (rand() & 32767L) * max;
	return (temp / 32768L);
}

char *
cpystr(out, in)
char	*out, *in;
/*
 * Copy in to out, return a pointer to the trailing EOS.
 */
{
	while ((*out++ = *in++) != EOS)
	    ;
	return (out - 1);
}
#endif




kaleid()
/*
 * Do one kaleidiscope vector
 */
{
	int	x1;
	int	y1;
	int	x2;
	int	y2;
	int		whichc;
	extern int	irand();	/* Returns rand() mod argument	*/

	do {
	    /*
	     * Get the starting and ending points of the vector
	     */
	    x1 = irand(MAXX);
	    if ((y1 = ytab[x1]) != 0)
		y1 = irand(y1);
	    x2 = irand(MAXX);
	    if ((y2 = ytab[x2]) != 0)
		y2 = irand(y2);
	} while (x1 == x2 && y1 == y2);
	/*
	 * Plot the kaleidiscope pattern. 
	 * Note that [x1,y1] and [x2,y2] are
	 * in the upper-left quadrant. 
	 * Plot calls xymove with values for all
	 * four quadrants.
	 */

	whichc = irand(FONTSIZE);
	
	if (verbose) {
	    sprintf(inline, "[%2d, %2d]\r\n[%2d, %2d] %2d",
		x1, y1, x2, y2, whichc);
	    vtout(1, 1, inline);
	    vtout(0, 0, NULL);
	}
	xymove(40 + x1, 12 - y1, 40 + x2, 12 - y2, fontur[whichc]);
	xymove(40 + x1, 13 + y1, 40 + x2, 13 + y2, fontlr[whichc]);
	xymove(39 - x1, 12 - y1, 39 - x2, 12 - y2, fontul[whichc]);
	xymove(39 - x1, 13 + y1, 39 - x2, 13 + y2, fontll[whichc]);
}


int main(int argc, char **argv)
{
	int	count;
#ifndef	Z80
	extern char	*getenv();
#endif
	/*
 	 * Determine the terminal type and initialize the screen handler
	 */
#ifndef	Z80
	isvt52 = (strcmp(getenv("TERM"), "vt52") == 0
	       || strcmp(getenv("TERM"), "VT52") == 0);
#endif

	if (argc > 1
	  && (strcmp(argv[1], "-vt52") == 0
	   || strcmp(argv[1], "-VT52") == 0)) {
	    isvt52 = TRUE;
	}
	erpage(1, 1);			/* Erase entire page		*/
	for (;;)
	    kaleid();
}
