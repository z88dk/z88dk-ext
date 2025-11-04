
/* This is the port to z88dk of what looks a nice piece of history (around 1982).
   Keyboard is adapted to work with keys QAOP.
   The ANSI VT emulation is close to the original program requirements.

/* zcc +zx -create-app -O3 --opt-code-size -lndos -clib=ansi -DHAVEGOTOXY -DQAOP maze.c */
/* zcc +zx81 -create-app -O3 --opt-code-size -lndos -clib=ansi -subtype=wrx -DHAVEGOTOXY -DQAOP maze.c */

/*
 *	Amazing demonstration program
 *
 *	Rewritten from a Pascal program marked:
 *		"Copyright 1976,
 *		Oregon Museum of Science and Industry
 *		4015 SW Canyon Road
 *		Portland Oregon 97221
 *		(503) 248-5923
 *	"Permission is hereby granted to republish (but not
 *	for profit) any or all of this program, provided that
 *	this copyright notice is included."
 *
 *	This version by:
 *		Martin Minow
 *		Digital Equipment Corp.
 *		146 Main St.
 *		Maynard, MA  01754
 *
 * Maze may be compiled for RSTS/E RT11 or VMS-native.
 * On VMS, maze must be linked with kbgetc and kbputb.
 *
 */

/*)BUILD
*/

#include <stdio.h>
#include <stdlib.h>

#pragma output hrgpage = 36096

#ifdef JOYSTICK
#include <games.h>
int stick;
#endif

#ifdef	rsx
FILE	*fd;
#endif

//#define	WSIZE		((80 / 2) - 1)	/* Maximum maze width		*/
//#define LSIZE		((24 / 2) - 1)	/* Maximum maze length		*/
/*
 * Conroy's compiler does not permit constant expressions in array
 * definitions, so the sizes must be worked out by hand:
 */
 
 /* Constants for the ZX Spectrum on z88dk*/
#define WSIZE		31		/* Columns in the maze		*/
#define LSIZE		11		/* Rows in the maze		*/

#define	LINESIZE	31		/* Input text line size		*/

/*
 * Character definitions (and constants for video terminal output)
 */

#define	BELL	0007
#define BS	0010
#define ESC	0233

#define	UP		ESC, 'A'
#define	DOWN		ESC, 'B'
#define	RIGHT		ESC, 'C'
#define	LEFT		BS
//#define	LEFT		ESC, 'D'
#define ONGRAPH		ESC, 'F'	/* VT52 graphics mode, not used	*/
#define OFFGRAPH	ESC, 'G'	/* VT52 graphics mode, not used	*/
#define	HOME		ESC, 'H'
#define	ERASEEOS	ESC, 'J'
#define	ERASEEOL	ESC, 'K'
#define	DOT		'o'
#define SOUTHWARD	LEFT, DOWN
#define WESTWARD	LEFT, LEFT
#define NORTHWARD	LEFT, UP
char	eraseline[] = { ERASEEOL, 0 };

/*
 * Fake some datatype definitions
 */

#define	BOOLEAN		int
#define	DIRECTION	int	/* VAX compiler dies if these are 	*/
#define	FLAG		int	/* defined as char			*/
#define TRUE		1
#define FALSE		0

/*
 * Define maze directions
 */

#define EAST	 0		/* Do					*/
#define SOUTH	 1		/*   NOT				*/
#define WEST	 2		/*     change				*/
#define NORTH	 3		/*       this				*/
#define	UNKNOWN	 4		/*	   order			*/
#define NORTHP1	 4		/* NORTH+1 for wall dimension		*/

/*
 * Define mask values for square[][].flag
 */

#define	EBIT	001
#define	SBIT	002
#define	WBIT	004
#define	NBIT	010
#define	HERE	020

/*
 * Define maze items
 */

struct square {
	FLAG		flag;	/* Has walls and "beenhere" bit		*/
	DIRECTION	path;	/* Link to next square on this path	*/
};

struct sqrptr {
	unsigned int	row;
	unsigned int	col;
};


#ifdef __Z88DK

#define LARGEPRIME 32749
#define fgetss fgets

ttyput(buffer, buflen)
char		*buffer;	/* What to output			*/
int		buflen;		/* Number of bytes to output		*/
/*
 * Output to the console
 */
{
	int	i;
	char	*bp;

	bp = buffer;
	for (i = buflen; i-- > 0; fputc_cons(*bp++));
}
#endif

#ifdef __Z88DK
char	clrscr[]	=	{ 12 };
#else
char	clrscr[]	=	{ HOME, ERASEEOS };
#endif

void erase()
/*
 * Clear the screen
 */
{
	ttyput(clrscr, sizeof clrscr);
}



/*
 * Define the maze
 */

struct square maze[LSIZE][WSIZE];

int	length;			/* Actual length (rows) of the maze	*/
int	width;			/* Actual width  (cols) of the maze	*/
int	minimum;		/* How many squares in the solution	*/
int	moves;			/* Actual number of squares visited	*/

struct sqrptr	start;		/* The starting square (lower right)	*/
struct sqrptr	finish;		/* The ending square (upper left)	*/
struct sqrptr	current;	/* Used by buildmaze and findpath	*/
BOOLEAN		showoff;	/* True to show maze building		*/
BOOLEAN		showxpath;	/* True to show backtracking		*/

/*
 * The reverse vector qives the reverse sense of any direction
 * Wall and rwall are used to set and clear the maze "walls"
 */

/*	    	argument =	EAST	SOUTH	WEST	NORTH	UNKNOWN	*/
static DIRECTION reverse[] = {	WEST,	NORTH,	EAST,	SOUTH,	UNKNOWN };
static FLAG rwall[]	 =  {	WBIT,	 NBIT,	EBIT,	 SBIT,	0 };
static FLAG wall[]	 =  {	EBIT,	 SBIT,	WBIT,	 NBIT,	0 };

/*
 * Note the following use of wall and rwall:
 *
 *	square[x][y].flag &   wall[direction]	TRUE if a wall is present
 *	square[x][y].flag |=  wall[direction]	SET a wall in this direction
 *	square[x][y].flag &= ~wall[direction]	REMOVE a wall if present
 *
 * The HERE bit may be set and cleared in the same manner.  Also, note that
 *
 *	rwall[direction] is identical to wall[reverse[direction]]
 */



/*
 *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*               M a n g l e   t h e   M a z e                   *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */


resetmaze()
/*
 * Initialize the maze (slightly ugly since C lacks structure assignment)
 */
{
	struct square	*mp;
	
	for (mp = &maze[0][0]; mp <= &maze[LSIZE-1][WSIZE-1]; mp++) {
		mp->flag = EBIT | SBIT | WBIT | NBIT;
		mp->path = UNKNOWN;
	}
}




clearmarks()
/*
 * Make sure nobody thinks we've been anywhere in the maze
 */
{
	struct square	*mp;
	
	for (mp = &maze[0][0]; mp <= &maze[LSIZE-1][WSIZE-1]; mp++)
		mp->flag &= ~HERE;
}


BOOLEAN equal(x, y)
struct sqrptr	*x;
struct sqrptr	*y;
/*
 * True if they're equal
 */
{
	return((x->row == y->row) && (x->col == y->col));
}


mark(x)
struct sqrptr	*x;
/*
 * Mark we've visited this place
 */
{
	maze[x->row][x->col].flag |= HERE;
}




/*
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*               C o n s t r u c t   a   M a z e                 *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */


/*				EAST, SOUTH, WEST, NORTH		*/
static DIRECTION rowchange[] = {     0,    -1,    0,     1 };
static DIRECTION colchange[] = {     1,     0,   -1,     0 };


struct square *movesquare(x, y, d)
struct sqrptr	*x;
struct sqrptr	*y;
DIRECTION	d;
/*
 * On return y will be x moved in the indicated direction.
 * movesquare returns a pointer to the new square.
 * Note that there is not check for "out of bounds."
 */
{
	y->row = x->row + rowchange[d];
	y->col = x->col + colchange[d];
	return(&maze[y->row][y->col]);
}


removewall(x, d)
struct sqrptr	*x;		/* The current element			*/
DIRECTION	d;		/* Which wall to remove			*/
/*
 * We've found (made) a connection.  Mark the path in the current element
 * and in the reverse direction from the connection.
 */
{
	struct sqrptr		nearby;

	maze[x->row][x->col].flag &= ~wall[d];
	(movesquare(&current, &nearby, d))->flag &= ~rwall[d];
}


char eastpath[]	   = { DOT,            DOT,            DOT };
char southpath[]   = { DOT, SOUTHWARD, DOT, SOUTHWARD, DOT };
char westpath[]	   = { DOT, WESTWARD,  DOT, WESTWARD,  DOT };
char northpath[]   = { DOT, NORTHWARD, DOT, NORTHWARD, DOT };
char unknownpath[] = { '?' };

static char *pathstring[] = {
	&eastpath,
	&southpath,
	&westpath,
	&northpath,
	&unknownpath
};

static int pathstrlen[5];
/*
static int pathstrlen[] = {
	sizeof eastpath,
	sizeof southpath,
	sizeof westpath,
	sizeof northpath,
	sizeof unknownpath
};
*/



#ifdef HAVEGOTOXY

#include <conio.h>

dca(vtrow, vtcol)
{
	gotoxy(vtcol,23-vtrow);
}


#else

char dcastring[] = { ESC, 'Y', '?', '?' };

#define DCAROW	2
#define DCACOL	3

dca(vtrow, vtcol)
int	vtrow;		/* Row on the screen				*/
int	vtcol;		/* Column on the screen				*/
/*
 * Move the cursor to the indicated position.  Note that, unlike the
 * vt52 hardware, [0,0] is at the bottom left hand corner of the screen.
 */
{
	dcastring[DCAROW] = 55 - vtrow;
	dcastring[DCACOL] = 32 + vtcol;
	ttyput(dcastring, sizeof dcastring);
}

#endif


move(x)
struct sqrptr	*x;
/*
 * Move the cursor to the center of the square  (zero, zero is bottom left).
 */
{
	dca(x->row*2 + 1, x->col*2 + 1);
}



drawpath(x, d)
struct strptr	*x;
DIRECTION	d;
/*
 * Extend the path in the indicated direction
 */
{
	move(x);				/* Move to box's center	*/
	ttyput(pathstring[d], pathstrlen[d]);	/* and draw the path	*/
}



void buildmaze(display)
BOOLEAN		display;	/* Show handiwork if set		*/
/*
 * Maze builder driving program.
 *
 * The algorithm builds a simply-connected reasonably random maze.
 * (Simply-connected means that there is exactly one path between any
 * two cells of the maze.)
 *
 *	1. Starting in the middle, execute the loop (size-1) times (since
 *		the algorithm must connect the current cell to (size-1)
 *		other cells.  The "pathlen" variable defines the randomness
 *		of the maze and was apparently set by trial and error.
 *	2. We are at a new cell.  Mark "this one visited" and spin the
 *		compass looking for an unvisited neighbour.
 *	3. If all the neighbours have been visited (compass == UNKNOWN)
 *		or if this path has gone on long enough, go to 5.
 *	4. Move to the neighbour (drawing a path on the screen for debugging),
 *		increment the path length and continue at step 2.
 *	5. We can't extend the path any further.  Jump randomly around
 *		in the maze until we find a cell that is the neighbour
 *		of a cell on the path.  Break down the wall between
 *		these two cells and start a new path.  Continue at step 2.
 *
 */
{
	unsigned int	count;		/* How many squares visited	*/
	unsigned int	size;		/* How many squares to visit	*/
	DIRECTION compass;
	int		pathmax;	/* Possible path maximum	*/
	int		pathlen;	/* Current path length		*/
	int		somewhere;	/* Random place in the maze	*/
	BOOLEAN		virgin();	/* Check func. for randompath()	*/
	BOOLEAN		visited();	/* Check func. for randompath()	*/
	DIRECTION	randompath();
	//struct square	*movesquare();

	pathlen = 0;
	somewhere = 0;
	current.row = length / 2;
	current.col = width  / 2;
	pathmax = width;		/* "For no reason"		*/
	size = length * width;
	clearmarks();
	for (count = 1; count < size; count++) {
		mark(&current);		/* We're here, we're here	*/
		compass = randompath(&current, virgin);
		if (compass == UNKNOWN || pathlen > pathmax) {
			do {		/* Jump around in the maze	*/
				somewhere = (somewhere + LARGEPRIME) % size;
				current.row = somewhere / width;
				current.col = somewhere % width;
				compass = randompath(&current, &visited);
			} while (compass == UNKNOWN);
			if (display) drawpath(&current, compass);
			removewall(&current, compass);
			pathlen = 1;
		}
		else {			/* No place to go from here	*/
			if (display) drawpath(&current, compass);
			removewall(&current, compass);
			movesquare(&current, &current, compass);
			pathlen++;
		}
	}
	maze[start.row][start.col].flag &= ~SBIT;
				/* Remove the South wall in order to	*/
				/* Leave space for the maze printer	*/
}



/*
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*                    M a z e   O u t p u t                      *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */



intput(value)
int		value;		/* What to output			*/
/*
 * Convert the (positive) integer to ascii, and output it via ttyput
 */
{
	char		line[6];
	char		*lp;
	int		i;

	lp = &line[6];		/* Do it backwards to fake recursion	*/
	do {
		*--lp = (value % 10) + '0';
		value /= 10;
	} while (value > 0);
	ttyput(lp, &line[6] - lp);
}



report()
/*
 * Note how many moves were needed.  To avoid being hassled by the I/O
 * system, we must do the conversion ourselves!
 */
{

	dca(finish.row*2 + 3, finish.col*2 + 3);
	ttyput("The solution took ", 18);
	intput(moves);
	ttyput(" moves, best is ", 16);
	intput(minimum);
}



//char blobstring[]	=	{ DOT };
char blobstring[]	=	{ '*' };

blob(x, offset)
struct sqrptr	*x;
int		offset;		/* Which row offset from the center	*/
/*
 * Put a blob in the entrance/exit of the maze.  Note that this code
 * must change if move() changes.
 */
{
	dca(x->row*2 + 1 + offset, x->col*2 + 1);
	ttyput(blobstring, sizeof blobstring);
}


showpath()
/*
 * Write the path through the maze (The maze is linked from start to finish)
 */
{
	DIRECTION	d;
	struct sqrptr		next;
/*	struct square		*movesquare();	-- A compiler objected	*/

	current.row = start.row;
	current.col = start.col;
	minimum = 0;
	blob(&start, -1);			/* At the entrance	*/
	while(!equal(&current, &finish)) {
		d = maze[current.row][current.col].path;
		if (d == UNKNOWN) {
			dca(23,0);
			printf("Dead at [%d, %d]\n",
					current.row, current.col);
			exit(0);
		}
		minimum++;
		drawpath(&current, d);
		movesquare(&current, &current, d);
	}
	blob(&finish, 1);			/* At the exit		*/
	report();
}



BOOLEAN outofbounds(x)
struct sqrptr	*x;
/*
 * True if we're off the edge of the universe.
 */
{
	return(		(x->row <  0)
		|| 	(x->row >= length)
		|| 	(x->col <  0)
		||	(x->col >= width));
}



showreverse()
/*
 * Write the path through the maze (The maze is linked from finish to start)
 */
{
	DIRECTION	d;
	struct square	*mp;
	struct sqrptr		next;
/*	struct square		*movesquare();	-- A compiler complaint	*/

	current.row = start.row;
	current.col = start.col;
	minimum = 0;
	blob(&start, -1);			/* At the entrance	*/
	while(!equal(&current, &finish)) {
		for (d = EAST; d <= NORTH; d++) {
			mp = movesquare(&current, &next, d);
			if (!outofbounds(&next) && mp->path == reverse[d]) {
				minimum++;
				drawpath(&current, d);
				movesquare(&current, &current, d);
				break;
			}
		}
		if (d > NORTH) {		/* Note: never true	*/
			dca(23,0);
			printf("Dead at [%d, %d]\n",
					current.row, current.col);
			exit(0);
		}
	}
	blob(&finish, 1);			/* At the exit		*/
	report();
}


char floorstring[]	=	{ '_', '_' };
char hack1string[]	=	{ '_', ' ' };
char hack2string[]	=	{ '_' };

drawfloor(oldrow, oldcol, newcol, hack)
int	oldrow;		/* Row, may range from -1 to width-1		*/
int	oldcol;		/* Column, may range from -1 to width-1		*/
int	newcol;
BOOLEAN	hack;		/* Special case for the top line.		*/
/*
 * Draw a floor (or ceiling) from [oldrow,oldcol] to [oldrow,newcol]
 * The hack flag leaves an opening at the top of the maze.
 */
{
	unsigned int	i;

	dca(oldrow*2 + 2, oldcol*2);
	if (hack) {
		ttyput(hack1string, sizeof hack1string);
		for (i = oldcol+1; i<= newcol; i++)
			ttyput(floorstring, sizeof floorstring);
		ttyput(hack2string, sizeof hack2string);
	}
	else	for (i = oldcol; i <= newcol; i++)
			ttyput(floorstring, sizeof floorstring);
}


char wallstring[] = { '|', LEFT, UP, '|', LEFT, UP };

drawwall(oldrow, oldcol, newrow)
int	oldrow;		/* Row, may range from -1 to width-1		*/
int	oldcol;		/* Column, may range from -1 to width-1		*/
int	newrow;
/*
 * Draw a sidewall from [oldrow,oldcol] to [newrow,oldcol]
 */
{

	unsigned int	i;

	dca(oldrow*2, oldcol*2 + 2);
	for (i = oldrow; i <= newrow; i++)
		ttyput(wallstring, sizeof wallstring);
}


horizontal(i, dir)
int		i;		/* The current row			*/
DIRECTION	dir;		/* Which wall (SOUTH or NORTH)		*/
/*
 * Draw the horizontal information
 */
{
	unsigned int		j;
	unsigned int		k;
	DIRECTION	d;

	d = dir;
	for (j = 0; j < width;) {
		while ((j < width) && (maze[i][j].flag & wall[d]) == 0)
			j++;
		if (j < width) {
			k = j + 1;
			while ((k < width)
					&& (maze[i][k].flag & wall[d]) != 0)
				k++;
			if (d == NORTH)
				drawfloor(i,   j, k-1, TRUE);
			else	drawfloor(i-1, j, k-1, FALSE);
			j = k;
		}			
	}
}

vertical(i, dir)
int		i;		/* The current col			*/
DIRECTION	dir;		/* Which wall (SOUTH or NORTH)		*/
/*
 * Draw the vertical information
 */
{
	unsigned int		j;
	unsigned int		k;
	DIRECTION	d;

	d = dir;
	for (j = 0; j < length;) {
		while ((j < length) && (maze[j][i].flag & wall[d]) == 0)
			j++;
		if (j < length) {
			k = j + 1;
			while ((k < length)
					&& (maze[k][i].flag & wall[d]) != 0)
				k++;
			if (d == EAST)
				drawwall(j, i,   k-1);
			else	drawwall(j, i-1, k-1);
			j = k;
		}			
	}
}


showmaze()
/*
 * Write the maze on the screen
 */
{
	unsigned int	i;

	erase();

	for (i = 0; i < length; i++)
		horizontal(i, SOUTH);
	horizontal(length-1, NORTH);

	for (i = 0; i < width; i++)
		vertical(i, WEST);
	vertical(width-1, EAST);
}



char unxeast[]	= { 'X',            'X' };
char unxsouth[]	= { 'X', SOUTHWARD, 'X' };
char unxwest[]	= { 'X', WESTWARD,  'X' };
char unxnorth[]	= { 'X', NORTHWARD, 'X' };

char uneast[]	= { ' ',            ' ' };
char unsouth[]	= { ' ', SOUTHWARD, ' ' };
char unwest[]	= { ' ', WESTWARD,  ' ' };
char unnorth[]	= { ' ', NORTHWARD, ' ' };

struct undraw {
	char *str;
	int  len;
};

static struct undraw xout[] = {
	{ &unxeast,  2 },
	{ &unxsouth, 6 },
	{ &unxwest,  6 },
	{ &unxnorth, 6 }
};

static struct undraw whiteout[] = {
	{ &uneast,  2 },
	{ &unsouth, 6 },
	{ &unwest,  6 },
	{ &unnorth, 6 }
};

/*
static struct undraw xout[] = {
	{ &unxeast,	sizeof unxeast	},
	{ &unxsouth,	sizeof unxsouth },
	{ &unxwest,	sizeof unxwest	},
	{ &unxnorth,	sizeof unxnorth	},
};

static struct undraw whiteout[] {
	{ &uneast,	sizeof uneast	},
	{ &unsouth,	sizeof unsouth },
	{ &unwest,	sizeof unwest	},
	{ &unnorth,	sizeof unnorth	},
};
*/

crossout(x)
struct sqrptr	*x;
/*
 * Mark the path "visited with a dead end"
 */
{
	DIRECTION	d;

	d = maze[x->row][x->col].path;
	move(x);
	if (showxpath)
		ttyput(xout[d].str,	xout[d].len);
	else	ttyput(whiteout[d].str,	whiteout[d].len);

}



/*
 *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*    F i n d   a   P a t h   T h r o u g h   t h e   M a z e    *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */

findpath(display)
BOOLEAN		display;	/* Show the maze if set			*/
/*
 * Solve the maze (driver program)
 * A note on the algorithm.
 *	1. Begin at the beginning.
 *	2. We are at a new square, exit if we're at the end.
 *		Note that we've been to this square
 *		Choose a random path to a square connected to us.
 *		The new square must be inside the maze, have no
 *		wall between us, and be unvisited.
 *	3. If we are connected, move to this square (make it current)
 *		and, in the new current square, remember where we
 *		came from.  Then, go to step 2.
 *	4. If no connection could be made (we are blocked), then,
 *		Exit (no solution) if we're back at the start.
 *		Use the "where we came from" information to backup
 *		one step (but "beenhere" will prevent further
 *		attempts to use this square) and go back to step 2.
 *		
 */
{
	DIRECTION	compass;
	BOOLEAN	soluble;
	struct square	*mp;
	
	BOOLEAN			clearahead();
	DIRECTION		randompath();

	clearmarks();
	current.row = start.row;
	current.col = start.col;
	moves = 0;
	minimum = 0;
	if (display) blob(&start, -1);	/* Note where we start from	*/
	soluble = TRUE;			/* Well, hopefully...		*/

	while (!equal(&current, &finish) && soluble) {
		mark(&current);		/* Well, here we are		*/
		moves++;
		compass = randompath(&current, clearahead);
		if (compass != UNKNOWN) {
			if (display) drawpath(&current, compass);
			mp = movesquare(&current, &current, compass);
			mp->path = reverse[compass];
			minimum++;
		}
		else {			/* Not here, backup one step	*/
			soluble = !equal(&current, &start);
			if (soluble) {	/* Still hope for the maze	*/
				if (display) crossout(&current);
				compass = mp->path;
				mp->path = UNKNOWN;
				mp = movesquare(&current, &current, compass);
				minimum--;
			}
		}
	}
	if (display) {
		blob(&finish, 1);	/* Note where we end up		*/
		report();
	}
}




isolve()
/*
 * The computer builds and solves the maze (without cheating!).
 */
{

	for (;;) {			/* Forever, ...			*/
		erase();

		resetmaze();		/* Clean it out			*/
		buildmaze(showoff);	/* Make a maze			*/
	
		showmaze();  /* Display the maze		*/
		findpath(TRUE);		/* Find a path (display it)	*/
	
		if (showxpath) {	/* If the display is unclean	*/
			showmaze();     /* Display the maze		*/
			showreverse();	/* And the path			*/
		}
		sleep(10);		/* Show the amazing result	*/
	}
}



static char *dirname[] = {
	"East  ",
	"South ",
	"West  ",
	"North "
};




DIRECTION getmove()
/*
 * Read a move from the terminal (using the VT52 arrow keys).
 * Note: this requires single-character input (enbspc()).
 * Return UNKNOWN if CTRL/C or CTRL/Z struck.
 */
{

	unsigned int	c;
	
#ifdef JOYSTICK
    
	while ( c=joystick(stick) == 0 ) {}
	while ( joystick(stick) != 0 ) {}

	switch (c) {
	case MOVE_RIGHT:  return(EAST);
	case MOVE_LEFT:   return(WEST);
	case MOVE_UP:     return(NORTH);
	case MOVE_DOWN:   return(SOUTH);
	default:	return(UNKNOWN);
	}
	
	return(UNKNOWN);

#else
	
#ifdef QAOP
	for (;;) {
		
		c = getch();

		switch (c) {
			
		case 'q':	return(NORTH);

		case 'a':	return(SOUTH);

		case 'p':	return(EAST);

		case 'o':	return(WEST);
		
		}
		
		return(UNKNOWN);

	}
#else

	BOOLEAN		flag;
	
	flag = TRUE;			/* Looking for an ESCAPE	*/
	for (;;) {
#ifdef	rt11
		c = ttyin() & 0177;
#endif
#ifdef	vms
		c = kbgetc();
#endif
		if (c == ('C' - 0100) || c == ('Z' - 0100)) {
#ifdef	rt11
			disspc();
#endif
			return(UNKNOWN);
		}
		if (c == 0 || c == 0177) continue;
		if (flag && c == (ESC & 0177)) {
			flag = FALSE;
			continue;
		}
		switch (c) {
		case 'A':	return(NORTH);

		case 'B':	return(SOUTH);

		case 'C':	return(EAST);

		case 'D':	return(WEST);
		default:
				flag = TRUE;
		}
	}
#endif
#endif
}


yousolve()
/*
 * The human solves the maze.
 */
{

	DIRECTION	compass;
	struct square	*mp;
	BOOLEAN	errflag;
	//DIRECTION		getmove();
	
	for (;;) {			/* Forever, ...			*/
		erase();

		resetmaze();		/* Clean it out			*/
		buildmaze(showoff);	/* Make a maze			*/
		showmaze();		/* Display the maze		*/
		mp = &maze[start.row][start.col];
		mp->flag |= SBIT;	/* Set the south wall (no exit)	*/
		current.row = start.row;
		current.col = start.col;
		moves = 0;
		blob(&start, 0);
#ifdef	rt11
		enbspc();		/* Set for single char. i/o	*/
#endif
		while (!equal(&current, &finish)) {
			move(&current);	/* Move cursor to current col.	*/
			compass = getmove();
			if (compass == UNKNOWN) return;
#ifdef	rt11
			ttyout(' ');	/* Erase the blob		*/
#else
			ttyput(" ", 1);	/* Erase the blob		*/
#endif
			if ((mp->flag & wall[compass]) == 0) {
				if (errflag) {
					dca(23,0);
					ttyput(eraseline, sizeof eraseline);
					errflag = FALSE;
				}
				moves++;	/* It's a valid move	*/
				maze[current.row][current.col].path = compass;
				mp = movesquare(&current, &current, compass);
			}
			else {
				dca(23,0);
				errflag = TRUE;
				ttyput("Possible moves are ", 19);
				for (compass = EAST;
						compass <= NORTH; compass++) 
					if ((mp->flag & wall[compass]) == 0)
						ttyput(dirname[compass], 6);
			}
			blob(&current, 0);
		}

		sleep(2);		/* Let it stand as shown	*/
	
		move(&finish);		/* Now, build the true path	*/
#ifdef	rt11
		ttyout(' ');		/* First erase exit blob	*/
#else
		ttyput(" ", 1);		/* First erase exit blob	*/
#endif
		showpath();		/* And show the amazing result	*/
		sleep(10);
	}
}



BOOLEAN marked(x)
struct sqrptr	*x;
/*
 * True if we've been here
 */
{
	return((maze[x->row][x->col].flag & HERE) != 0);
}



BOOLEAN visited(x, d)
struct sqrptr	*x;		/* The current element			*/
DIRECTION	d;		/* The possible direction		*/
/*
 * Visited() is called from within randompath() to determine if the
 * neighbour (in the indicated direction) has already been processed.
 */
{
	struct sqrptr	nearby;
	//struct square	*movesquare();

	if (marked(x)) return(FALSE);
	movesquare(x, &nearby, d);
	return(!outofbounds(&nearby) && marked(&nearby));
}

BOOLEAN virgin(x, d)
struct sqrptr	*x;		/* The element to test			*/
DIRECTION	d;		/* Where to go				*/
/*
 * Virgin() is called from within randompath() to determine if we can
 * move in the indicated direction.
 */
{
	struct sqrptr	nearby;
	//struct square	*movesquare();

	movesquare(x, &nearby, d);
	return(!(outofbounds(&nearby) || marked(&nearby)));
}



BOOLEAN clearahead(x, d)
struct sqrptr		*x;	/* Where we are				*/
DIRECTION		d;	/* Where we are trying to get to	*/
/*
 * Test whether we can go in the desired direction
 */
{
	BOOLEAN	blocked;
	struct square	*mp;

	struct sqrptr		nearby;

	blocked = (maze[x->row][x->col].flag & wall[d]) != 0;
	if (!blocked) {
		mp = movesquare(x, &nearby, d);
		blocked = outofbounds(&nearby);
		if (!blocked)
			blocked = (mp->flag & (rwall[d] | HERE)) != 0;
	}
	return(!blocked);
}



static DIRECTION paths[4] = { NORTH, EAST, SOUTH, WEST };

DIRECTION randompath(x, goodpath)
struct sqrptr	*x;
BOOLEAN		(*goodpath)();		/* Function called from here	*/
/*
 * Move in a random direction, calling the caller-supplied function
 * to test for a reasonable direction.
 */
{

	unsigned int i;
	unsigned int j;
	DIRECTION temp;

	for (i = 1; i < 4; i++) {	/* Shuffle the path vector	*/
		//j = irand(i);		/* Locate a random one to do	*/
		j = rand()%i;		/* Locate a random one to do	*/
		temp = paths[j];
		paths[j] = paths[i];
		paths[i] = temp;
	}

	for (i = 0; i < 4 && !((*goodpath)(x, paths[i])); i++);
	return((i >= 4) ? UNKNOWN : paths[i]);
}









/*
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*                   T e r m i n a l   I / O                     *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */



getvalue(row, text, min, max)
int		row;		/* Which row on the screen		*/
char		*text;		/* What to ask for			*/
int		min;		/* Minimum value to accept, see below	*/
int		max;		/* Maximum value to accept, see below	*/
/*
 * Input a number from the terminal, first prompting the text on the
 * indicated row.  min and max force "good" values to be returned.
 * Note:  getvalue(row, text, 1, 0) accepts any input value.
 */
{
	BOOLEAN	allok;
	unsigned int		value;
	char			line[LINESIZE];

	allok = (min == 1) && (max == 0);	/* Accept anything	*/
	for(;;) {
		dca(row, 0);
		if (allok)
			printf("%s%s: ", eraseline, text);
		else
			printf("%s%s, (minimum %d, maximum %d): ", 
				eraseline, text, min, max);
#ifdef	vms
		fflush(stdout);
#endif
		if (fgetss(line, sizeof line, stdin) == NULL)
			exit(0);
		value = atoi(line);
		if (allok || (value >= min && value <= max))
			return(value);
	}
}

BOOLEAN getyesno(row, prompt, assume)
int		row;		/* Where on the screen			*/
char		*prompt;	/* A message of sorts			*/
char		*assume;	/* Assumed answer			*/
/*
 * Prompt and get a yes/no answer
 */
{
	char	*lp;
	char	line[LINESIZE];

	for (;;) {
		dca(row, 0);
		printf("%s? <%s> %s", prompt, assume, eraseline);
#ifdef	vms
		fflush(stdout);
#endif
		if (fgetss(line, LINESIZE, stdin) == NULL)
			exit(0);
		for (lp = line; lp == ' '; lp++);
		if (*lp == 0) *lp = *assume;
		if (*lp == 'y' || *lp == 'Y') return(TRUE);
		if (*lp == 'n' || *lp == 'N') return(FALSE);
	}
}





/*
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	*                                                               *
 *	*                   M a i n   P r o g r a m                     *
 *	*                                                               *
 *	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */

main(argc, argv)
int 	argc;		/* Number of arguments			*/
char	*argv[];	/* Argument pointer array		*/
{

pathstrlen[0] = sizeof eastpath;
pathstrlen[1] = sizeof southpath;
pathstrlen[2] = sizeof westpath;
pathstrlen[3] = sizeof northpath;
pathstrlen[4] = sizeof unknownpath;

#ifdef	rsx
	if ((fd = fopen("ti:", "wun")) == NULL)
		error("Can't open ti:");
#endif

#ifdef JOYSTICK
	erase();
	int i;

	for (i=0 ; i!=GAME_DEVICES; i++)
		printf("%u - %s\n",i+1,joystick_type[i]);
		
	stick=0;
	while ((stick<1) || (stick>GAME_DEVICES))
		stick=getk()-48;
#endif

	erase();
#ifdef	vms
	printf("\n");            /* Prime the I/O package */
#endif
	length = getvalue(23, "Maze length", 2, LSIZE);
	width  = getvalue(22, "Maze width",  2, WSIZE);	
	start.row  = 0;          /* The maze           */
	start.col  = width - 1;  /*   starts here      */
	finish.row = length-1;   /*     and it         */
	finish.col = 0;          /*       ends here.   */

	showoff = getyesno(21, "Show internal maze building process", "No");

	if (getyesno(20,"Do you want to solve the mazes", "No"))
		yousolve();		
	else {
		showxpath = getyesno(19, "Show backtrack paths", "No");
		isolve();
	}
}




#ifdef	rsx
ttyput(buffer, buflen)
char	*buffer;	/* What to output			*/
int 	buflen;		/* Number of bytes to output	*/
/*
 * Output to the terminal.  RSX mode.
 */
{
	fput(buffer, buflen, fd);
}
#endif

#ifdef	vms
ttyput(buffer, buflen)
char		*buffer;	/* What to output			*/
int		buflen;		/* Number of bytes to output		*/
/*
 * Output to the terminal, VMS mode
 */
{
	register int	i;
	register char	*bp;

	bp = buffer;
	for (i = buflen; i-- > 0; kbputb(*bp++));
	kbflush();
}
#endif

#ifdef	foort11

/*
 * RT11 I/O interface routines.  These should be rewritten in Macro
 * and put in the library.  They assume that you have the system subroutine
 * library installed.
 */

struct	MEM_WORD {
	int	mem_word;
};

#define	JSW	044
extern	int	ittinr();
extern	int	ittour();

enbspc()
/*
 * Enable special mode
 */
{
	JSW->mem_word |= 010100;
}

disspc()
/*
 * Disable special mode
 */
{
	JSW->mem_word &= ~010100;
}

ttyin()
/*
 * Read one byte from the terminal
 */
{
	register int	c;

	while ((c = call(&ittinr, 0)) < 0);
	return (c);
}

ttyinr()
/*
 * Read a byte from the terminal, return -1 if nothing ready
 */
{
	register int	c;

	c = call(&ittinr, 0);
	return ((c < 0) ? -1 : c);
}

ttyout(c)
char		c;
/*
 * Output one byte to the terminal
 */
{
	while (call(&ittour, 1, c) != 0);
}

ttyput(buffer, count)
register char	*buffer;
register int	count;
/*
 * Write a buffer to the tty
 */
{
	while (--count >= 0) {
		while (call(&ittour, 1, *buffer) != 0);
		buffer++;
	}
}	
#endif


